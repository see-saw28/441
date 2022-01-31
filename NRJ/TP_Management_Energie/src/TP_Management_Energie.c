/*
===============================================================================
TP Management de l'Energie
===============================================================================
*/
#include <stdio.h>
#include "LPC8xx.h"
#include "i2c.h"
#include "swm.h"
#include "syscon.h"
#include "uart.h"
#include "utilities.h"
#include "chip_setup.h"
#include "rom_api.h"
#include "adc.h"
#include "ctimer.h"
#include "LPC8xx.h"

#include "swm.h"
#include "syscon.h"
#include "utilities.h"
#include "uart.h"
#include "pmu.h"
#include "chip_setup.h"

#define BP1 		LPC_GPIO_PORT->B0[13]
#define BP2 		LPC_GPIO_PORT->B0[12]
#define LED4 		LPC_GPIO_PORT->B0[11]

#define I2CBAUD 		100000
#define LM75_ADDR			0x90
#define LM75_CONFIG		0x01
#define LM75_TEMP			0x00

#define BUFSIZE       64

uint8_t I2CMasterBuffer[BUFSIZE];
uint8_t I2CSlaveBuffer[BUFSIZE];
uint32_t I2CReadLength, I2CWriteLength;
uint32_t tempReading;

void setup_debug_uart(void);
uint32_t ReadTempSensor(void);

int main(void) {

	uint32_t i,pot;
	//LPC_PWRD_API->set_fro_frequency(30000);

//	LPC_SYSCON->PDRUNCFG &= ~LPOSC_PD;
//
//	// Select the clock source to FRG0 by writing to the FRG0CLKSEL register
//	LPC_SYSCON->MAINCLKSEL = 2;         // '1' selects main_clk as input to FRG0
//	LPC_SYSCON->MAINCLKUEN = 0;
//	LPC_SYSCON->MAINCLKUEN = 1;
//	LPC_SYSCON->PDRUNCFG |= FRO_PD;
	// Select the function clock source for the USART by writing to the appropriate FCLKSEL register.
	LPC_SYSCON->FCLKSEL[INDEX] = FCLKSEL_MAIN_CLK;     // Select main_clk as fclk to this USART

	// Alimentation des périphériques utilisés
	LPC_SYSCON->SYSAHBCLKCTRL0 |=  IOCON  | GPIO0|GPIO_INT;

	// Make all ports outputs driving '1', except wakeup pins PIO0_12 and PIO0_13 are inputs
	LPC_GPIO_PORT->DIR[0] = 0xFFFFCFFF;
	LPC_GPIO_PORT->PIN[0] = 0xFFFFFFFF;
	LPC_GPIO_PORT->DIR0 |= 0x00000800; // broche 11 connectée à led 4 en sortie

	// Configuration liaison série
	ConfigSWM(DBGUTXD, DBGTXPIN);
	ConfigSWM(DBGURXD, DBGRXPIN);
	LPC_SYSCON->PRESETCTRL[0] &= (DBGURST);
	LPC_SYSCON->PRESETCTRL[0] |= ~(DBGURST);
	// Get the Main Clock frequency for the BRG calculation.
	SystemCoreClockUpdate();
	// Write calculation result to BRG register
	pDBGU->BRG = (main_clk / (16 * DBGBAUDRATE)) - 1;
	pDBGU->CFG = DATA_LENG_8|PARITY_NONE|STOP_BIT_1; // 8 data bits, no parity, one stop bit, no flow control, asynchronous mode
	pDBGU->CTL = 0; // No continuous break, no address detect, no Tx disable, no CC, no CLRCC
	pDBGU->STAT = 0xFFFF; // Clear any pending flags (for illustration, isn't necessary after the peripheral reset)
	pDBGU->CFG |= UART_EN; // Enable USART


	// Configuration i2C
	LPC_SYSCON->I2C0CLKSEL = FCLKSEL_MAIN_CLK;
	ConfigSWM(I2C0_SCL, TARTET_I2CSCL);               // Use for LPC804
	ConfigSWM(I2C0_SDA, TARTET_I2CSDA);               // Use for LPC804
	LPC_SYSCON->PRESETCTRL0 &= (I2C0_RST_N);
	LPC_SYSCON->PRESETCTRL0 |= ~(I2C0_RST_N);
	LPC_I2C0->DIV = (main_clk/(4*I2CBAUD)) - 1;
	LPC_I2C0->CFG = CFG_MSTENA;

	// Configuration ADC
	 // on active l'ADC
	LPC_IOCON->PIO0_10 = 0x00000080; //on enlève les pullup/pulldown
    LPC_SWM->PINENABLE0 &= 	~ADC_7;
	LPC_ADC->SEQA_CTRL &= ~(1 << 31); //arret du convertisseur
	LPC_ADC->SEQA_CTRL |= (1 << 7); //selection de la voie 8
	LPC_ADC->SEQA_CTRL |= 1 << 31; //demarrage du convertisseur
	LPC_ADC->SEQA_CTRL |= 1 << 27; //demarrage de la conversion en mode BURST

	//Configuration du timer 0
	LPC_CTIMER0->PR = 15000;
	LPC_CTIMER0->MR[3] = 1000;
	// Configure the Match Control register to interrupt and reset on match in MR3
	LPC_CTIMER0->MCR = (1 << MR3R); //chaque fois que M3R est atteint, interruption et reset
	LPC_CTIMER0->EMR = (0x03 << 10); //T0_MAT3 change d'etat a chaque fois que MR3 est atteint
	LPC_CTIMER0->TCR = 1;
	LPC_SWM->PINASSIGN4 = 11 << 24; //On affecte T0_MAT3 a LED4

	int bp1_old=0,bp1_new=0;

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
		LPC_PMU->WUENAREG |= (1<<4);

	while ( 1 ) {

		bp1_new=BP1;

		if(bp1_new!=bp1_old & bp1_new){
			// Prepare for low-power mode
			LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDRUNCFG;

			  // Turn clocks off to unused peripherals (Leave GPIO0 and GPIO_INT on so we can wake up)
			  LPC_SYSCON->SYSAHBCLKCTRL[0] &= ~(UART0|IOCON);
			// Clear the deep PD pin wakeup source flags in WUSRCREG
			LPC_PMU->WUSRCREG = 0xFF;
			// Set the SleepDeep bit
			SCB->SCR |= (1<<2);
			// PCON = 3 selects Deep power-down mode
			LPC_PMU->PCON = 0x3;
			// Wait here for wakeup pin.
			__WFI();
		}
		if ( !(i++ % 100000) ) {
		//	tempReading = ReadTempSensor();
		//	while (!(LPC_ADC->DAT[7] & 0x80000000));
		//	pot = (LPC_ADC->DAT[7] & 0x0000FFF0)>>4;
		//	printf("Temperature : %3d degree C.  Potentiometre : %4d \r\n", tempReading,pot);
			LPC_GPIO_PORT->B0[11] = ! LPC_GPIO_PORT->B0[11];
		}
	}

} // end of main

uint32_t ReadTempSensor( void )
{
  uint32_t i;
	uint32_t tempValue;

  WaitI2CMasterState(LPC_I2C0, I2C_STAT_MSTST_IDLE); // Wait for the master state to be idle	
  
  /* clear buffer */
  for ( i = 0; i < BUFSIZE; i++ )	{ 
    I2CMasterBuffer[i] = 0;    
		I2CSlaveBuffer[i] = 0;
  }

  I2CWriteLength = 2;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = LM75_ADDR;
  I2CMasterBuffer[1] = LM75_CONFIG;    	
  I2CMasterBuffer[2] = 0x00;		/* configuration value, no change from default */ 
  I2CmasterWrite( I2CMasterBuffer, I2CWriteLength ); 

  /* clear buffer, then, get temp reading from LM75B */
  for ( i = 0; i < BUFSIZE; i++ )	{
    I2CMasterBuffer[i] = 0;
    I2CSlaveBuffer[i] = 0;
  }
  I2CWriteLength = 1;
  I2CReadLength = 2;
  I2CMasterBuffer[0] = LM75_ADDR;
  I2CMasterBuffer[1] = LM75_TEMP;
  I2CmasterWriteRead( I2CMasterBuffer, I2CSlaveBuffer, I2CWriteLength, I2CReadLength ); 
  /* The temp reading value should reside in I2CSlaveBuffer... */
	tempValue = (uint32_t)(((I2CSlaveBuffer[0]<<8) | I2CSlaveBuffer[1]) >> 5);
	/* D10 is the sign bit */
	if ( tempValue & 0x400 ) {
		/* after conversion, bit 16 is the sign bit */
		tempValue = (int32_t)(((-tempValue+1)&0x3FF) * 0.125) | 0x10000;
	}
	else {
		/* Bit 7 (D10) is the polarity, 0 is Plus temperature and 1 is Minus temperature */
		tempValue = (int32_t)(tempValue * 0.125);
	}
  return ( tempValue );
}

// Implementation of sendchar (used by printf)
// This is for Keil and MCUXpresso projects.
int sendchar (int ch) {
  while (!((pDBGU->STAT) & TXRDY));   // Wait for TX Ready
  return (pDBGU->TXDAT  = ch);        // Write one character to TX data register
}




