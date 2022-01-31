/*
 * PMU_Modes_Funcs.c
 *
 *  Created on: Today
 *      Author: F.F.
 */

#include "LPC8xx.h"
#include "syscon.h"
#include "uart.h"
#include "utilities.h"
#include "swm.h"
#include "PMU_Modes.h"


unsigned char rx_buffer[RX_BUFFER_SIZE];
extern volatile enum {false, true} handshake;
const static uint32_t rx_char_counter = 0;

#define WaitForUART0txRdy  while(((LPC_USART0->STAT) & TXRDY) == 0)


/*****************************************************************************
** Function name:		PININT7_IRQHandler
**
** Description:		    Pin interrupt 7 ISR.
**                      This is the wake up ISR for Sleep, Deep-sleep, and Power-down modes.
**                      This ISR turns on the green LED,
**                      clears the pin interrupt flag, clears the SleepDeep
**                      bit, restores clocks to GPIO and UART0 for main, then
**                      returns.
**
** Parameters:			None
** Returned value:		void
**
*****************************************************************************/
void PININT7_IRQHandler(void)
{

  // Startup time ends on this falling edge (green LED on)
		LED_On(LED_RED);
	
  // Clear the pin interrupt flag
  LPC_PIN_INT->IST = 1<<7;

  // Clear the SleepDeep bit
  SCB->SCR &= ~(1<<2);

  // Activate clocks to the needed peripherals for normal mode if they are off
  LPC_SYSCON->SYSAHBCLKCTRL[0] |= (UART0 | GPIO0);

  return;
}




/*****************************************************************************
** Function name:		PININT6_IRQHandler
**
** Description:		    Pin interrupt 6 ISR.
**                      This is the wake up ISR for Sleep, Deep-sleep, and Power-down modes.
**                      This ISR turns on the green LED,
**                      clears the pin interrupt flag, clears the SleepDeep
**                      bit, restores clocks to GPIO and UART0 for main, then
**                      returns.
**
** Parameters:			None
** Returned value:		void
**
*****************************************************************************/
void PININT6_IRQHandler(void)
{

  // Startup time ends on this falling edge (green LED on)
  LED_On(LED_GREEN);

  // Clear the pin interrupt flag
  LPC_PIN_INT->IST = 1<<6;

  // Clear the SleepDeep bit
  SCB->SCR &= ~(1<<2);

  // Activate clocks to the needed peripherals for normal mode if they are off
  LPC_SYSCON->SYSAHBCLKCTRL[0] |= (UART0 | GPIO0);

  return;
}




/*****************************************************************************
** Function name:		UART0_IRQHandler
**
** Description:		    UART0 interrupt service routine.
**                      This ISR reads one received char from the UART0 RXDAT register,
**                      stores it in the rx_buffer array, and echos it back via the
**                      UART0 transmitter. If the char. is 0xD (carriage return),
**                      both a new line char (0xA) and a CR (0xD) are echoed.
**                      Only '0', '1', '2', '3', and 'CR' are handled, any other
**                      value is ignored.
**
** Parameters:			None
** Returned value:		void
**
*****************************************************************************/
void UART0_IRQHandler() {
  unsigned char temp;

  temp = LPC_USART0->RXDAT ;                // Get the current character

  if ((temp >= 0x30 && temp <= 0x33)) {     // Only integers 0-3 and CR are accepted
    rx_buffer[rx_char_counter] = temp;      // Append the current character to the rx_buffer
    WaitForUART0txRdy;                      // Wait for TXREADY
    LPC_USART0->TXDAT  = temp;              // Echo it back to the terminal
  }

  if (temp == 0x0D) {                       // CR (carriage return) is current character. End of entry.
    WaitForUART0txRdy;                      // Wait for TXREADY
    LPC_USART0->TXDAT  = 0x0A;              // Echo a NL (new line) character to the terminal.
    WaitForUART0txRdy;                      // Wait for TXREADY
    LPC_USART0->TXDAT  = 0x0D;              // Echo a CR character to the terminal.
    handshake = true;                       // Set handshake for main()
  }

  return;
}





/*****************************************************************************
** Function name:		Setup_Low_Power
**
** Description:		    Copies PDRUNCFG contents into PDAWAKECFG register and
**                      turns off clocks to UART0, IOCON, and SWM.
**
** Parameters:			None
** Returned value:		void
**
*****************************************************************************/
void Setup_Low_Power() {

  LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDRUNCFG;

  // Turn clocks off to unused peripherals (Leave GPIO0 and GPIO_INT on so we can wake up)
  LPC_SYSCON->SYSAHBCLKCTRL[0] &= ~(UART0|IOCON);
}

