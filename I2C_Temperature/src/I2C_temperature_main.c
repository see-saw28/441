/*
===============================================================================
 Name        : Example_I2C_Temperature.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include <stdio.h>
#include "LPC8xx.h"
#include "i2c.h"
#include "swm.h"
#include "syscon.h"
#include "uart.h"
#include "utilities.h"

#define I2CBAUD 		100000

/* For more info, read NXP's LM75B datasheet */
#define LM75_ADDR			0x90
#define LM75_CONFIG		0x01
#define LM75_TEMP			0x00

#define BUFSIZE       64

uint8_t I2CMasterBuffer[BUFSIZE];
uint8_t I2CSlaveBuffer[BUFSIZE];
uint32_t I2CReadLength, I2CWriteLength;
uint32_t tempReading;

void setup_debug_uart(void);


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

  /* the example used to test the I2C interface is 
  a NXP's LM75 temp sensor. LPC MCU is used a I2C
  master, the temp sensor is a I2C slave. */
    
  /* the sequence to get the temp reading is: 
  set configuration register,
  get temp reading
  */

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

/*****************************************************************************
*****************************************************************************/
int main(void) {

	uint32_t i;
	
	// Configure the debug uart (see Serial.c)
	setup_debug_uart();
	
  // Provide main_clk as function clock to I2C0
  LPC_SYSCON->I2C0CLKSEL = FCLKSEL_MAIN_CLK;
  
  // Enable bus clocks to I2C0, SWM
  LPC_SYSCON->SYSAHBCLKCTRL0 |= (I2C0 | SWM);
  
	ConfigSWM(I2C0_SCL, TARTET_I2CSCL);               // Use for LPC804
  ConfigSWM(I2C0_SDA, TARTET_I2CSDA);               // Use for LPC804

  // Give I2C0 a reset
  LPC_SYSCON->PRESETCTRL0 &= (I2C0_RST_N);
  LPC_SYSCON->PRESETCTRL0 |= ~(I2C0_RST_N);

  // Configure the I2C0 clock divider
  // Desired bit rate = Fscl = 100,000 Hz (1/Fscl = 10 us, 5 us low and 5 us high)
  // Use default clock high and clock low times (= 2 clocks each)
  // So 4 I2C_PCLKs = 100,000/second, or 1 I2C_PCLK = 400,000/second
  // I2C_PCLK = SystemClock = 30,000,000/second, so we divide by 30000000/400000 = 75
  // Remember, value written to DIV divides by value+1.
	// main_clk value is updated inside setup_debug_uart() already.
  LPC_I2C0->DIV = (main_clk/(4*I2CBAUD)) - 1;

  LPC_I2C0->CFG = CFG_MSTENA;

  while ( 1 ) {
		tempReading = ReadTempSensor();
		if ( !(i++ % 0x200) ) {
			/* Bit 16 is the sign (1 is negative, 0 is positive) after temp conversion */
			if ( tempReading & 0x10000 ) {
				printf("Current temperature is -%d degree C.\r\n", (tempReading & 0xFFFF));
			}
			else {
				printf("Current temperature is %d degree C.\r\n", tempReading);
			}
		}
	}

} // end of main

