
/*
===============================================================================
 Name        : SPI_Flash.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC8xx.h"
#include "core_cm0plus.h"
#include "syscon.h"
#include "utilities.h"
#include "spi.h"
#include "driver_spiflash.h"

#define MANUFACTURE_ID		0xef3012	/* This may change depending on the SPI flash device on the board. */

uint8_t  g_SPI0ReadPage[PAGE_SIZE];
uint8_t  g_SPI0WritePage[PAGE_SIZE];
uint32_t gFlashID = 0;

int main(void) {
	
	uint32_t i;
	
	LPC_SYSCON->SYSAHBCLKCTRL0 |= ( SWM | GPIO | SPI0 | IOCON);
  
	// Provide main_clk as function clock to SPI0
  LPC_SYSCON->SPI0CLKSEL = FCLKSEL_MAIN_CLK;
  // Configure the SWM (see peripherals_lib and swm.h)
  ConfigSWM(SPI0_SCK,   P0_1);   /* Use for LPC804 */ 
  ConfigSWM(SPI0_MISO,  P0_9);   /* Use for LPC804 */
  ConfigSWM(SPI0_MOSI,  P0_8);   /* Use for LPC804 */ 
	
	// SPI Flash CS Pin Initialize
  SPI_FLASH_CS1(); /* Make the port bit an output driving '1' */
  LPC_GPIO_PORT->DIRSET[0] = 1<<P0_16;
	
  // Setup the SPIs ...
  LPC_SYSCON->PRESETCTRL0 &=  (SPI0_RST_N);
  LPC_SYSCON->PRESETCTRL0 |= ~(SPI0_RST_N);
  // Configure the SPI master's clock divider, slave's value meaningless. (value written to DIV divides by value+1)
  SystemCoreClockUpdate();                // Get main_clk frequency
  LPC_SPI0->DIV = (main_clk/LPC_SPI0BAUDRate) - 1; //(2-1);
  // Configure the CFG registers:
  // Enable=true, master/slave, no LSB first, CPHA=0, CPOL=0, no loop-back, SSEL active low
  LPC_SPI0->CFG = SPI_CFG_ENABLE | SPI_CFG_MASTER;
  // Configure the master SPI delay register (DLY), slave's value meaningless.
  // Pre-delay = 0 clocks, post-delay = 0 clocks, frame-delay = 0 clocks, transfer-delay = 0 clocks
  LPC_SPI0->DLY = 0x0000;

	/* Polling is used in this SPI flash access example. */
//  LPC_SPI0->INTENSET = SPI_RXRDYEN;                    // Master interrupt only on received data
//  NVIC_EnableIRQ(SPI0_IRQn);  

	gFlashID = spiflash_read_id();
	if(gFlashID != MANUFACTURE_ID)
	{
		while(1);
	}

	for ( i = 0; i < PAGE_SIZE; i++ ) {
		g_SPI0WritePage[i] = i;
		g_SPI0ReadPage[i] = 0;		
	}
	
	/* Set WEL bit to enable flash write */
	spiflash_write_enable();
	spiflash_chip_erase();
	while ( (spiflash_check_status()&SPIFLASH_Status_Busy) != 0 );
	
	/* WEL will be gone after erase, check status register and make sure it's set before programming. */
	while ( (spiflash_check_status()&SPIFLASH_Status_WEL) == 0 ) {
		spiflash_write_enable();
	}
	
	spiflash_write_data(0x0, PAGE_SIZE, g_SPI0WritePage);
	while ( (spiflash_check_status()&SPIFLASH_Status_Busy) != 0 );
	spiflash_read_data(0x0, PAGE_SIZE, g_SPI0ReadPage);
	while ( (spiflash_check_status()&SPIFLASH_Status_Busy) != 0 );
	spiflash_write_disable();
	
	/* Please note: the data comparison should exclude the command and 24-bit address field (total 4 bytes). */
	for ( i = 0; i < PAGE_SIZE - 4; i++ ) {
		if ( g_SPI0WritePage[i] != g_SPI0ReadPage[i] ) {
			while ( 1 );		/* Programming is not successful, something very bad happened. */
		}	
	}	
  while(1);
	
} // end of main
