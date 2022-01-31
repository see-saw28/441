/*
===============================================================================
 Name        : PMU_Modes.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include "LPC8xx.h"
#include "PMU_Modes.h"
#include "swm.h"
#include "syscon.h"
#include "utilities.h"
#include "uart.h"
#include "pmu.h"
#include "chip_setup.h"

const unsigned char promptstring[] = "\rChoose a low power mode:\n\r0 for Sleep\n\r1 for Deep-sleep\n\r2 for Power-down\n\r3 for Deep power-down\n\r";
const unsigned char sleepstring[] = "Entering Sleep ... Use pin interrupt to wake up\n\n\r";
const unsigned char dsleepstring[] = "Entering Deep-sleep ... Use pin interrupt to wake up\n\n\r";
const unsigned char pdstring[] = "Entering Power-down ... Use pin interrupt to wake up\n\n\r";
const unsigned char dpdstring[] = "Entering Deep power-down ... Use wakeup pin to wake up\n\n\r";
const unsigned char ascii[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

extern unsigned char rx_buffer[RX_BUFFER_SIZE];
volatile enum {false, true} handshake;


int main(void) {

  unsigned int k, temp;
  char buff[30] = "WUSRCREG content is 0xYZ\n\n\r";
  uint32_t * addr = (uint32_t *)LPC_IOCON_BASE;

  // Configure the debug uart (see Serial.c)
  setup_debug_uart();

  // Read the WUSRCREG and print the content if this is a pin wakeup from deep PD mode
  temp = LPC_PMU->WUSRCREG;
  if (temp != 0) {
    buff[23] = ascii[temp&0xF];
    buff[22] = ascii[(temp>>4)&0xF];
    PutTerminalString(pDBGU, (uint8_t *)buff);
  }
  
  // Enable clocks to relevant peripherals
  LPC_SYSCON->SYSAHBCLKCTRL[0] |= (GPIO0|GPIO_INT|IOCON);

  // Power down the various analog blocks that aren't needed
  LPC_SYSCON->PDSLEEPCFG |= (BOD_PD|LPOSC_PD);
  LPC_SYSCON->BODCTRL &= ~(1<<BODRSTENA);                          // Disable BOD reset before de-powering BOD
  LPC_SYSCON->PDRUNCFG |= (BOD_PD|ADC_PD|LPOSC_PD|DAC0_PD|ACMP_PD);

  // Don't clock clock select muxes and dividers that aren't needed
  LPC_SYSCON->CLKOUTSEL = CLKOUTSEL_OFF;
  LPC_SYSCON->FRG0CLKSEL = FRGCLKSEL_OFF;
  LPC_SYSCON->ADCCLKSEL = ADCCLKSEL_OFF;
  for (k = 1; k <= 10; k++) {
    LPC_SYSCON->FCLKSEL[k] = FCLKSEL_OFF;
  }

  // Disable pull-ups and pull-downs.
  // If internal pull-ups are disabled,
  // wakeup pins MUST be pulled high externally.
  // For Deep PD wakeup, wakeup pins must always
  // be pulled high externally.
  for (k = 0; k < NUM_IOCON_SLOTS; k++) {
    *(addr + k) &= ~(0x3<<3);
  }

  // Make all ports outputs driving '1', except wakeup pins PIO0_12 and PIO0_13 are inputs
	  LPC_GPIO_PORT->DIR[0] = 0xFFFFCFFF;
	  LPC_GPIO_PORT->PIN[0] = 0xFFFFFFFF;
	
	// Configure P0.12 as PININT7, falling edge sensitive
  LPC_PIN_INT->IST = 1<<7;     // Clear any pending edge-detect interrupt flag
  LPC_SYSCON->PINTSEL[7] = 12;  // P0.12 assigned to PININT7
  LPC_PIN_INT->ISEL |= 0<<7;   // Edge-sensitive
  LPC_PIN_INT->SIENF = 1<<7;   // Falling-edge interrupt enabled on PININT7

  // Configure PININT7 as wake up interrupt in SYSCON block
  LPC_SYSCON->STARTERP0 |= 1<<7;
  
  // Enable pin interrupt 7 in the NVIC
  NVIC_EnableIRQ(PININT7_IRQn);

  // Enable P0.13 as Deep power-down wakeup source
	LPC_PMU->WUENAREG |= PIO_13_WU;

  // Enable UART Rx ready interrupt
  pDBGU->INTENSET = RXRDY;
  NVIC_EnableIRQ(DBGUIRQ);

	Config_LED(LED_RED);
	
  while(1) {

		LED_On(LED_RED);
		
    PutTerminalString(pDBGU, (uint8_t *)promptstring);

    handshake = false;                                      // Clear handshake flag, will be set by ISR at end of user input
    while (handshake == false);                             // Wait here for handshake from ISR

		LED_Off(LED_RED);
		
    switch(rx_buffer[0]) {
      default:
      case('0'):
        PutTerminalString(pDBGU, (uint8_t *)sleepstring);
        // Wait for TX buffer empty
        while (!((pDBGU->STAT) & TXRDY));
        // Prepare for low-power mode
        Setup_Low_Power();
        // Clear the SleepDeep bit
        SCB->SCR &= ~(1<<2);
        // PCON = 0 selects sleep mode
        LPC_PMU->PCON = 0x0;
        // Wait here for pin interrupt.
        __WFI();

        break;

      case('1'):
        PutTerminalString(pDBGU, (uint8_t *)dsleepstring);
        // Wait for TX buffer empty
        while (!((pDBGU->STAT) & TXRDY));
        // Prepare for low-power mode
        Setup_Low_Power();
        // Set the SleepDeep bit
        SCB->SCR |= (1<<2);
        // PCON = 1 selects Deep-sleep mode
        LPC_PMU->PCON = 0x1;
        // Wait here for pin interrupt.
        __WFI();

        break;

      case('2'):
        PutTerminalString(pDBGU, (uint8_t *)pdstring);
        // Wait for TX buffer empty
        while (!((pDBGU->STAT) & TXRDY));
        // Prepare for low-power mode
        Setup_Low_Power();
        // Set the SleepDeep bit
        SCB->SCR |= (1<<2);
        // PCON = 2 selects Power-down mode mode
        LPC_PMU->PCON = 0x2;
        // Wait here for pin interrupt.
        __WFI();

        break;

      case('3'):
        PutTerminalString(pDBGU, (uint8_t *)dpdstring);
        // Wait for TX buffer empty
        while (!((pDBGU->STAT) & TXRDY));
        // Prepare for low-power mode
        Setup_Low_Power();
        // Clear the deep PD pin wakeup source flags in WUSRCREG
        LPC_PMU->WUSRCREG = 0xFF;        
        // Set the SleepDeep bit
        SCB->SCR |= (1<<2);
        // PCON = 3 selects Deep power-down mode
        LPC_PMU->PCON = 0x3;
        // Wait here for wakeup pin.
        __WFI();

        break;

    } // end of switch

  } // end of while(1)

} // end of main

