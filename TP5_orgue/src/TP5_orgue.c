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
#include "math.h"
#include "ctimer.h"

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
	LPC_SYSCON->SYSAHBCLKCTRL0 |= UART0 | SWM |GPIO | CTIMER0;

	LPC_GPIO_PORT->DIR0 |= (1 << 17)|(1<<21) | (1<<19);

	 ///////////
	// TIMER //
   ///////////

  //precision 0.1 microseconde
  	LPC_CTIMER0->PR=149;

  	//440Hz ie comp0 = 100 000 / 440
  	LPC_CTIMER0->MR[3]=227;

  	//mise a zero par rapport à MR3
  	LPC_CTIMER0->MCR |= (1<<MR3R);

  	//pwm 50% pour générer un signal rectangulaire ie reglage de comp1
  	LPC_CTIMER0->MR[1]=2;

  	//mat1
  	LPC_CTIMER0->PWMC = (1<<PWMEN1);

  	//mat1 relié a LED1 qui est relié au HP
  	LPC_SWM->PINASSIGN4 &= ~(0xFF<<8);
  	LPC_SWM->PINASSIGN4 |= 19<<8;


  	 /////////
  	//SERIE//
   /////////

	// Connect UART0 TXD, RXD signals to port pins
	ConfigSWM(U0_TXD, DBGTXPIN);
	ConfigSWM(U0_RXD, DBGRXPIN);


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


	int old_bp1=0,new_bp1=0;
	int old_bp2=0,new_bp2=0;

	char last_char=0;

	int note = 0;
	float note_jouee = 440;

	char display[100];
	while(1) {

	new_bp1=BP1;
	new_bp2=BP2;
	if (BP1){
		LPC_CTIMER0->TCR=0;
	}

	else {
		LPC_CTIMER0->TCR=(1<<CEN);//generation du signal
	}
	//PutTerminalString(LPC_USART0, (uint8_t *)thestring);

	/*handshake = false;                                   // Clear handshake flag, will be set by ISR at end of user input
	while (handshake == false);                          // Wait here for handshake from ISR
	*/
	//PutTerminalString(LPC_USART0, rx_buffer);            // Echo received string from the rx_buffer to the terminal if desired. Otherwise comment out.

	if (LPC_USART0->STAT & RXRDY){

		lcd_position(0,0);
		last_char=LPC_USART0->RXDAT;

		switch(last_char){
			case 38 :
				sprintf(display,"%16s","Do");
				note = 0;
				break;
			case 233 :
				sprintf(display,"%16s","Do#");
				note = 1;
				break;

			case 34 :
				sprintf(display,"%16s","Re");
				note = 2;
				break;

			case 39 :
				sprintf(display,"%16s","Re#");
				note = 3;
				break;

			case 40 :
				sprintf(display,"%16s","Mi");
				note = 4;
				break;

			case 45 :
				sprintf(display,"%16s","Fa");
				note = 5;
				break;

			case 232 :
				sprintf(display,"%16s","Fa#");
				note = 6;
				break;

			case 95 :
				sprintf(display,"%16s","Sol");
				note = 7;
				break;

			case 231 :
				sprintf(display,"%16s","Sol#");
				note = 8;
				break;

			case 224 :
				sprintf(display,"%16s","La");
				note = 9;
				break;

			case 41 :
				sprintf(display,"%16s","La#");
				note = 10;
				break;

			case 61 :
				sprintf(display,"%16s","Si");
				note = 11;
				break;

		}
		lcd_puts(display);
		lcd_position(1,0);
		note_jouee = 440 * pow(2,(float)(note-9)/12);
		sprintf(display,"%d",(int)note_jouee);
		lcd_puts(display);

		//440Hz ie comp0 = 100 000 / 440
		 LPC_CTIMER0->MR[3]=(int)100000/note_jouee;

	}
	old_bp1=new_bp1;
	old_bp2=new_bp2;

	};

} // end of main

