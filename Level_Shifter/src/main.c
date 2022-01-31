/*
===============================================================================
 Name        : /Level_Shifter.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include <stdio.h>
#include "LPC8xx.h"
#include "core_cm0plus.h"
#include "syscon.h"
#include "utilities.h"


int main(void) {


  // Enable clocks to relevant peripherals
  LPC_SYSCON->SYSAHBCLKCTRL0 |= (SWM | GPIO | GPIO0);

  //Disable any special functions on pin PIO0_0 and PI00_7 in the PINENABLE0
  LPC_SWM->PINENABLE0 |= (ACMP_I1|ADC_1);

  //Reset peripherals
  LPC_SYSCON->PRESETCTRL0 &=  (GPIO0_RST_N & GPIOINT_RST_N);
  LPC_SYSCON->PRESETCTRL0 |= ~(GPIO0_RST_N & GPIOINT_RST_N);

  // Initialize the LED pin
  Config_LED(LED_RED);
  LED_Off(LED_RED);

  //Initialize the GPIOs used for LVLSHFT example
  LPC_GPIO_PORT->DIRCLR[0] |= (1<<P0_0);      //Make the LVLSHFT_IN0 port bit an input


  LPC_GPIO_PORT->DIRSET[0] = 1<<P0_7; // Make the LVLSHFT_OUT0 port bit an output
  LPC_GPIO_PORT->SET[0]    = 1<<P0_7; // Configure LVLSHFT_OUT0 port bit to drive '1' making BLUE LED off


/* Configure the PINASSIGN6 register to choose your LVLSHFT input and output pins.
   LVLSHFT_IN0 = 0x0 (PIO0_0) ,LVLSHFT_OUT0 = 0x7 (PIO0_7)
   Register default value is 0xFFFFFFFF*/

  LPC_SWM->PINASSIGN6 &= 0x07FF00FF;


  while(1)
  {
  };

} // end of main
