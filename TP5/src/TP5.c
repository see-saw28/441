/*
===============================================================================
 Name        : UART0_Terminal.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC8xx.h"
#include "syscon.h"
#include "swm.h"
#include "syscon.h"
#include "utilities.h"
#include "uart.h"
#include "chip_setup.h"
#include "lib_ENS_II1_lcd.h"
#include "rom_api.h"
#include "stdio.h"

#define BP1  LPC_GPIO_PORT->B0[13]
#define BP2  LPC_GPIO_PORT->B0[12]
#define LED1 LPC_GPIO_PORT->B0[19]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]


#define RX_BUFFER_SIZE 35
#define WaitForUART0txRdy  while(((LPC_USART0->STAT) & (1<<2)) == 0)

const unsigned char thestring[] = "What do you have to say for yourself now?\n\r";
unsigned char rx_buffer[RX_BUFFER_SIZE];
volatile enum {false, true} handshake;

//
// Function name: UART0_IRQHandler
// Description:	  UART0 interrupt service routine.
//                This ISR reads one received char from the UART0 RXDAT register,
//                appends it to the rx_buffer array, and echos it back via the
//                UART0 transmitter. If the char. is 0xD (carriage return),
//                new line char (0xA) is appended to the array and echoed,
//                then a NUL char (0x0) is appended to the array to terminate the string
//                for future use.
// Parameters:    None
// Returns:       void
//
void UART0_IRQHandler() {
  static uint32_t rx_char_counter = 0;
  unsigned char temp;

  temp = LPC_USART0->RXDAT ;
  rx_buffer[rx_char_counter] = temp;        // Append the current character to the rx_buffer
  WaitForUART0txRdy;                        // Wait for TXREADY
  LPC_USART0->TXDAT  = temp;                // Echo it back to the terminal

  if (temp == 0x0D) {                       // CR (carriage return) is current character. End of string.
    rx_buffer[rx_char_counter+1] = 0x0A;    // Append a new line character to rx_buffer.
    rx_buffer[rx_char_counter+2] = 0x00;    // Append a NUL terminator character to rx_buffer to complete the string.
    WaitForUART0txRdy;                      // Wait for TXREADY
    LPC_USART0->TXDAT  = 0x0A;              // Echo a NL (new line) character to the terminal.
    handshake = true;                       // Set handshake for main()
    rx_char_counter = 0;                    // Clear array index counter
  }
  else {                                    // Current character is not CR, keep collecting them.
    rx_char_counter++;                      // Increment array index counter.

    if (rx_char_counter == RX_BUFFER_SIZE)  // If the string overruns the buffer, stop here before all hell breaks lose.
      while(1);
  }
  return;
}



//
// Main routine
//
int main(void) {
  LPC_PWRD_API->set_fro_frequency(30000);
  // Enable clocks to relevant peripherals
  LPC_SYSCON->SYSAHBCLKCTRL0 |= UART0 | SWM |GPIO;

  LPC_GPIO_PORT->DIR0 |= (1 << 17)|(1<<21) | (1<<19);

  // Connect UART0 TXD, RXD signals to port pins
  ConfigSWM(U0_TXD, DBGTXPIN);
  ConfigSWM(U0_RXD, DBGRXPIN);

  // Configure UART0 for 9600 baud, 8 data bits, no parity, 1 stop bit.
  // For asynchronous mode (UART mode) the formula is:
  // (BRG + 1) * (1 + (m/256)) * (16 * baudrate Hz.) = FRG_in Hz.
  //
  // We proceed in 2 steps.
  // Step 1: Let m = 0, and round (down) to the nearest integer value of BRG for the desired baudrate.
  // Step 2: Plug in the BRG from step 1, and find the nearest integer value of m, (for the FRG fractional part).
  //
  // Step 1 (with m = 0)
  // BRG = ((FRG_in Hz.) / (16 * baudrate Hz.)) - 1
  //     = (15,000,000/(16 * 115200)) - 1
  //     = 7
  //
  // Step 2.
  // m = 256 * [-1 + {(FRG_in Hz.) / (16 * baudrate Hz.)(BRG + 1)}]
  //   = 256 * [-1 + {(15,000,000) / (16*9600)(96+1)}]
  //   = 1.73
  //   = 2 (rounded)

  // Configure FRG0
  LPC_SYSCON->FRG0MULT = 0;
  LPC_SYSCON->FRG0DIV = 255;

  // Select main_clk as the source for FRG0
  LPC_SYSCON->FRG0CLKSEL = FRGCLKSEL_MAIN_CLK;

  // Select frg0clk as the source for fclk0 (to UART0)
  LPC_SYSCON->UART0CLKSEL = FCLKSEL_FRO_CLK;

  // Give USART0 a reset
  LPC_SYSCON->PRESETCTRL0 &= (UART0_RST_N);
  LPC_SYSCON->PRESETCTRL0 |= ~(UART0_RST_N);

  // Configure the USART0 baud rate generator
  LPC_USART0->BRG = 7;

  // Configure the USART0 CFG register:
  // 8 data bits, no parity, one stop bit, no flow control, asynchronous mode
  LPC_USART0->CFG = DATA_LENG_8|PARITY_NONE|STOP_BIT_1;

  // Configure the USART0 CTL register (nothing to be done here)
  // No continuous break, no address detect, no Tx disable, no CC, no CLRCC
  LPC_USART0->CTL = 0;

  // Clear any pending flags, just in case
  LPC_USART0->STAT = 0xFFFF;

  // Enable USART0
  LPC_USART0->CFG |= UART_EN;



  init_lcd();
  lcd_puts("TP5 serie");

  int old_bp1=0,new_bp1=0;
  int old_bp2=0,new_bp2=0;

  char last_char=0;

  while(1) {

	new_bp1=BP1;
	new_bp2=BP2;
	if (new_bp1 && old_bp1==0){
		if (LPC_USART0->STAT & TXRDY){
			LPC_USART0->TXDAT=130;
		}
		//PutTerminalString(LPC_USART0, (uint8_t *)"b");
	}

	else if (new_bp1==0 && old_bp1){
		if (LPC_USART0->STAT & TXRDY){
			LPC_USART0->TXDAT='B';
		}
		//PutTerminalString(LPC_USART0, (uint8_t *)"B");
	}


	if (new_bp2==0 && old_bp2){
		PutTerminalString(LPC_USART0, "Bouton 2 enfoncé\n\r");
	}
    //PutTerminalString(LPC_USART0, (uint8_t *)thestring);

    /*handshake = false;                                   // Clear handshake flag, will be set by ISR at end of user input
    while (handshake == false);                          // Wait here for handshake from ISR
*/
    //PutTerminalString(LPC_USART0, rx_buffer);            // Echo received string from the rx_buffer to the terminal if desired. Otherwise comment out.

	if (LPC_USART0->STAT & RXRDY){

		lcd_position(1,0);
		last_char=LPC_USART0->RXDAT;
		char display[100];
		sprintf(display,"%16d", last_char);
		lcd_puts(display);
		//lcd_putc(last_char);

		if (last_char=='a'){
			LED2=1;
		}
		else if (last_char=='e'){
			LED2=0;
		}
	}

    old_bp1=new_bp1;
    old_bp2=new_bp2;

  };

} // end of main

