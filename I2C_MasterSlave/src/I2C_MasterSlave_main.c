/*
===============================================================================
 Name        : Example_I2C_MasterSlave.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC8xx.h"
#include "i2c.h"
#include "swm.h"
#include "syscon.h"
#include "uart.h"
#include "utilities.h"


#define BUFFER_SIZE 35
#define WaitForUART0txRdy  while(((LPC_USART0->STAT) & (1<<2)) == 0)
#define Self_Slave_address 0x55
#define I2CBAUD 100000


void setup_debug_uart(void);


const unsigned char the_prompt[] = "Enter some characters to be transmitted from the I2C master to the slave\n\r";
const unsigned char the_message[] = "Characters received by the I2C slave were \n\r";
unsigned char u0_rx_buffer[BUFFER_SIZE];
unsigned char slave_rx_data[BUFFER_SIZE];
volatile enum {false, true} uart_handshake;

static uint32_t rx_char_counter = 0;
static uint32_t slave_data_counter = 0;


/*****************************************************************************
** Function:    UART0_IRQHandler
** Description: UART0 interrupt service routine.
**              This ISR reads one received char from the UART0 RXDAT register,
**              appends it to the u0_rx_buffer array, and echos it back via the
**              UART0 transmitter. If the char. is 0xD (carriage return),
**              a new line char (0xA) is appended to the array and echoed,
**              then a NUL char (0x0) is appended to the array to terminate the string
**              for future use.
** Parameters:  None
** Returns:     void
*****************************************************************************/
void UART0_IRQHandler() {
  unsigned char temp;
//Config_LED(LED_BLUE);
//LED_On(LED_BLUE);
  temp = LPC_USART0->RXDAT ;
  u0_rx_buffer[rx_char_counter] = temp;     // Append the current character to the rx_buffer
  WaitForUART0txRdy;                        // Wait for TXREADY
  LPC_USART0->TXDAT  = temp;                // Echo it back to the terminal
  //LPC_USART0->TXDAT  = 'Q';

  if (temp == 0x0D) {                       // CR (carriage return) is current character. End of string.
    u0_rx_buffer[rx_char_counter+1] = 0x0A; // Append a new line character to u0_rx_buffer.
    u0_rx_buffer[rx_char_counter+2] = 0x00; // Append a NUL terminator character to u0_rx_buffer to complete the string.
    WaitForUART0txRdy;                      // Wait for TXREADY
    LPC_USART0->TXDAT  = 0x0A;              // Echo a NL (new line) character to the terminal.
    uart_handshake = true;                  // Set handshake for main()
    rx_char_counter = 0;                    // Clear array index counter
  }
  else {                                    // Current character is not CR, keep collecting them.
    rx_char_counter++;                      // Increment array index counter.

    if (rx_char_counter == BUFFER_SIZE)     // If the string overruns the buffer, stop here before all hell breaks lose.
      while(1);
  }
  return;
}


/*****************************************************************************
** Function: I2C0_IRQHandler
** Description: I2C0 interrupt service routine
** parameters: None
** Returns: void
*****************************************************************************/
void I2C0_IRQHandler(void) {
  uint32_t temp;

  if ((LPC_I2C0->STAT & SLAVE_STATE_MASK) == STAT_SLVADDR) {
    LPC_I2C0->SLVCTL = CTL_SLVCONTINUE;                     // ACK the address
    return;
  }

  if ((LPC_I2C0->STAT & SLAVE_STATE_MASK) == STAT_SLVRX) {
    temp = LPC_I2C0->SLVDAT;                                // Read the data
    slave_rx_data[slave_data_counter++] = temp;             // Store it in the array, increment counter
    LPC_I2C0->SLVCTL = CTL_SLVCONTINUE;                     // ACK the data
    if (temp == 0)                                          // If current char is NUL terminator
      slave_data_counter = 0;                               // clear the counter for next string
    return;
  }

  while(1);                                                // Any other slave state, stop here and debug
}



/*****************************************************************************
*****************************************************************************/
int main(void) {
  uint32_t temp;
  unsigned char * tx_ptr;

  // Configure the debug uart (see Serial.c)
  setup_debug_uart();
	
  // Enable the USART0 RX Ready Interrupt, this project assumes an interrupt-driven use case
  LPC_USART0->INTENSET = RXRDY;
  NVIC_EnableIRQ(UART0_IRQn);

  // Provide main_clk as function clock to I2C0
  LPC_SYSCON->I2C0CLKSEL = FCLKSEL_MAIN_CLK;
  
  // Enable bus clocks to I2C0, SWM
  LPC_SYSCON->SYSAHBCLKCTRL0 |= (I2C0 | SWM);

  // Configure the SWM
  // On the LPC824, LPC84x I2C0_SDA and I2C0_SCL are fixed pin functions which are enabled / disabled in the pinenable0 register
  // On the LPC812, I2C0_SDA and I2C0_SCL are movable functions which are assigned using the pin assign registers
  //LPC_SWM->PINENABLE0 &= ~(I2C0_SCL|I2C0_SDA); // Use for LPC824 and LPC84x
  
	ConfigSWM(I2C0_SCL, TARTET_I2CSCL);                // Use for LPC804
  ConfigSWM(I2C0_SDA, TARTET_I2CSDA);                // Use for LPC804

  // Give I2C0 a reset
  LPC_SYSCON->PRESETCTRL0 &= (I2C0_RST_N);
  LPC_SYSCON->PRESETCTRL0 |= ~(I2C0_RST_N);

  // Configure the I2C0 clock divider
  // Desired bit rate = Fscl = 100,000 Hz (1/Fscl = 10 us, 5 us low and 5 us high)
  // Use default clock high and clock low times (= 2 clocks each)
  // So 4 I2C_PCLKs = 100,000/second, or 1 I2C_PCLK = 400,000/second
  // I2C_PCLK = SystemClock = 30,000,000/second, so we divide by 30000000/400000 = 75
  // Remember, value written to DIV divides by value+1
  SystemCoreClockUpdate(); // Get main_clk frequency
  LPC_I2C0->DIV = (main_clk/(4*I2CBAUD)) - 1;

  // Configure the I2C0 CFG register:
  // Master enable = true
  // Slave enable = true
  // Monitor enable = false
  // Time-out enable = false
  // Monitor function clock stretching = false
  //
  LPC_I2C0->CFG = CFG_MSTENA | CFG_SLVENA;


  // Configure the I2C0 SLVADR3 address register with 0 in l.s.b. to enable slave address 3
  LPC_I2C0->SLVADR3 = (Self_Slave_address<<1) | 0;

  // Enable the I2C0 slave pending interrupt
  LPC_I2C0->INTENSET = STAT_SLVPEND;
  NVIC_EnableIRQ(I2C0_IRQn);


  while(1) {

    PutTerminalString(LPC_USART0, (uint8_t *)the_prompt);// 

    uart_handshake = false;                              // Clear handshake flag, will be set by ISR at end of user input
    while (!uart_handshake);                             // Wait here for handshake from debug UART ISR

    //PutTerminalString(LPC_USART0, u0_rx_buffer);       // Echo string to the terminal if desired. Otherwise comment out.

    // Transmit the string to the slave, where it will get stored in another array by the I2C slave ISR
    tx_ptr = &u0_rx_buffer[0];                           // Initialize pointer to start of string

    do {
      temp = *tx_ptr++;                                  // Get a character from the string
      WaitI2CMasterState(LPC_I2C0, I2C_STAT_MSTST_IDLE); // Wait for the master state to be idle
      LPC_I2C0->MSTDAT = (Self_Slave_address<<1) | 0;    // Address with 0 for RWn bit (WRITE)
      LPC_I2C0->MSTCTL = CTL_MSTSTART;                   // Start the transaction by setting the MSTSTART bit to 1 in the Master control register.
      WaitI2CMasterState(LPC_I2C0, I2C_STAT_MSTST_TX);   // Wait for the address to be ACK'd

      LPC_I2C0->MSTDAT = temp;                           // Send the data to the slave
      LPC_I2C0->MSTCTL = CTL_MSTCONTINUE;                // Continue the transaction
      WaitI2CMasterState(LPC_I2C0, I2C_STAT_MSTST_TX);   // Wait for the data to be ACK'd

      LPC_I2C0->MSTCTL = CTL_MSTSTOP;                    // Send a stop to end the transaction

    } while (temp != 0);

    PutTerminalString(LPC_USART0, (uint8_t *)the_message);// Print a message
    PutTerminalString(LPC_USART0, slave_rx_data);         // Echo data received by the slave to the terminal

  } // end of while 1

} // end of main

