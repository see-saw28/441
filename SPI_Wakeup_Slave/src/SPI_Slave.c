/*
===============================================================================
 Name        : SPI_Wakeup_Slave.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include <stdio.h>
#include "LPC8xx.h"
#include "spi.h"
#include "syscon.h"
#include "swm.h"
#include "utilities.h"

// Program flow
// 1. Configure SPI1 as slave (requires external connections with SPI0 on the master board)
// 2. Wait for a character from the master, and go into the low-power mode requested.
// 3. Wake-up from low-power mode on the next character.

// Configure SPI1 as slave
// SPI mode 0 (CPOL = 0 CPHA = 0)
// 8 data bits
//

#define SPIBAUD 115200
#define SCK_PIN P0_19
#define MOSI_PIN P0_18
#define SSEL_PIN P0_21

unsigned char rx_buffer;
volatile enum {false, true} handshake;


//
// SPI1 interrupt service routine. Reads one received char from the RXDAT register, stores it in the rx_buffer.
//
void SPI0_IRQHandler() {
  LPC_SYSCON->SYSAHBCLKCTRL0 |= GPIO0;       // Turn GPIO clock back on for the LEDs
  rx_buffer = LPC_SPI0->RXDAT;              // Get the current character, store it for main
  handshake = true;                         // Set handshake for main()
  return;
}


//
// main routine
//
int main(void) {

  int k;

  uint32_t * addr = (uint32_t *)LPC_IOCON_BASE;

  // Enable clocks to relevant peripherals
  LPC_SYSCON->SYSAHBCLKCTRL[0] |= (SPI0|SWM|GPIO0|GPIO1|IOCON);

  // Power down the various analog blocks that aren't needed
  LPC_SYSCON->PDSLEEPCFG |= (BOD_PD|LPOSC_PD);
  LPC_SYSCON->PDRUNCFG |= (BOD_PD|ADC_PD|LPOSC_PD|DAC0_PD|ACMP_PD);

  // Disable pull-ups and pull-downs in IOCON
  for (k = 0; k < NUM_IOCON_SLOTS; k++) {
    *(addr + k) &= ~(0x3<<3);
  }

  // Make all GPIO ports outputs driving '1'
  LPC_GPIO_PORT->DIR[0] = 0xFFFFFFFF;
  LPC_GPIO_PORT->PIN[0] = 0xFFFFFFFF;
  LPC_GPIO_PORT->DIR[1] = 0xFFFFFFFF;
  LPC_GPIO_PORT->PIN[1] = 0xFFFFFFFF;

  // Configure the SWM
  ConfigSWM(SPI0_SCK, SCK_PIN);
  ConfigSWM(SPI0_MOSI, MOSI_PIN);
  ConfigSWM(SPI0_SSEL0, SSEL_PIN);

  // Kill the clocks to unused clock select muxes
  //LPC_SYSCON->CAPTCLKSEL = CAPTCLKSEL_OFF;
  LPC_SYSCON->CLKOUTSEL  = CLKOUTSEL_OFF;
  LPC_SYSCON->ADCCLKSEL  = ADCCLKSEL_OFF;
  LPC_SYSCON->FRG0CLKSEL = FRGCLKSEL_OFF;
  for (k = 0; k <= 10; k++) {
    LPC_SYSCON->FCLKSEL[k] = FCLKSEL_OFF;
  }

  // Select main_clk as the function clock source for SPI1
  LPC_SYSCON->SPI0CLKSEL = FCLKSEL_MAIN_CLK;

  // Get the main_clk frequency
  SystemCoreClockUpdate();

   // Give SPI0 a reset
  LPC_SYSCON->PRESETCTRL0 &= (SPI0_RST_N);
  LPC_SYSCON->PRESETCTRL0 |= ~(SPI0_RST_N);

  // Configure the CFG register:
  // Enable=true, slave, no LSB first, CPHA=0, CPOL=0, no loop-back, SSEL active low
  LPC_SPI0->CFG = SPI_CFG_ENABLE | SPI_CFG_SLAVE;

  // Configure the SPI control register
  // Slave: LEN 8 bits.
  LPC_SPI0->TXCTL = SPI_CTL_LEN(8);

  // Write dummy data to slave TXDAT
  LPC_SPI0->TXDAT = 0xAA;

  // Enable SPI1 interrupt as wake-up interrupt in SYSCON
  LPC_SYSCON->STARTERP1 = SPI0_INT_WAKEUP;

  // Enable SPI1 interrupt on received data
  LPC_SPI0->INTENSET = SPI_RXRDYEN;
  NVIC_EnableIRQ(SPI0_IRQn);

  Config_LED(LED_GREEN);
  Config_LED(LED_BLUE);
  Config_LED(LED_RED);

  while(1) {

    LED_On(LED_GREEN);
    LED_On(LED_RED);
    LED_On(LED_BLUE);

    // Clear handshake, will be set by ISR
    handshake = false;

    // Wait for handshake
    while(handshake == false);

    switch (rx_buffer) {
      case(0x30):
        LED_Off(LED_GREEN);
        // Prepare for low-power mode
        Setup_LPC8xx_Low_Power();
        // Clear the SleepDeep bit
        SCB->SCR &= ~(1<<2);
        // PCON = 0 selects Sleep mode
        LPC_PMU->PCON = 0x0;
        // Wait here for wake-up interrupt.
        __WFI();
        break;

      case(0x31):
        LED_Off(LED_RED);
        // Prepare for low-power mode
        Setup_LPC8xx_Low_Power();
        // Set the SleepDeep bit
        SCB->SCR |= (1<<2);
        // PCON = 1 selects Deep-sleep mode
        LPC_PMU->PCON = 0x1;
        // Wait here for wake-up interrupt.
        __WFI();
        break;

      case(0x32):
        LED_Off(LED_BLUE);
        // Prepare for low-power mode
        Setup_LPC8xx_Low_Power();
        // Set the SleepDeep bit
        SCB->SCR |= (1<<2);
        // PCON = 2 selects Power-down mode
        LPC_PMU->PCON = 0x2;
        // Wait here for wake-up interrupt.
        __WFI();
        break;

      default:    // This code should never execute
        while(1);

    } // end of switch

  } // end of while(1)

} // end of main
