${ProjName}:
SPI_Flash


Intended purpose:
To demonstrate the configuration and use of the SPI master and communicate with the on-board SPI flash
Winbond W25X20CLUXIG. Some simple SPI flash commands are implemented and tested in this example including
Read Device ID 		(0x9F)
Chip Erase 			(0xC7)
Page Program 		(0x02)
Read Data 			(0x03)
Write Enable 		(0x06)
Write Disable 		(0x04)
Read Status 		(0x05)

Functional description:
* Configuration of SPI master including configing SWM for SPI interface, clock derived from system clock 
used for the SPI master, due to the nature of the SPI flash, a GPIO pin is configured as the SPI CS pin.
SPI CS		p0.16
SPI CLK		P0.1
SPI MISO	p0.9
SPI MOSI	p0.8 

See More details on SPI flash, please check Winbond website.

External connections:
* MEM_CS pin need to connected to the SPI flash. See schematics for more details.


Detailed Program Flow:
* This example runs at 30 MHz.
  See funtion SystemInit() and chip_setup.h.


* main() routine
   1. Turn on clocks to peripherals  
   2. Configure SPI clock
   3. Configure GPIO and SWM for SPI pins.
   4. Configure SPI to master mode and use polling to simpify the process.
   5. Read Manufacture ID from the SPI slave flash. Scope oe SPI bus analyzer can be used to monitor the 
activities on the bus.
 
  
This example runs from Flash. 


To run this code: 1. Build
                  2. Load to flash
                  3. Reset
              or:
                  1. Build
                  2. Download and Debug
                  3. Run or Go


Note: It may be necessary to power-cycle the board in order to regain control of the reset button after programming.





