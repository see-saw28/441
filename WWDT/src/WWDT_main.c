/*
===============================================================================
 Name        : WWDT_main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC8xx.h"
#include "wwdt.h"
#include "syscon.h"
#include "utilities.h"

// Detailed program flow
// 
// main() routine
// 1. Turn on peripheral clocks to relevant peripherals.
// 2. Examine the WDTOF flag. If set, this current reset is due to a watchdog event.
//    Illuminate red LED and stop here.
// 3. Apply power to the LP oscillator.
// 4. Enable the LPOSC clock to the WWDT
// 5. Configure the WWDT
//    A. TC = number
//    B. WINDOW = number/2
//    C. WARNINT = WINDOW - 1
//    D. MOD = (WDEN=1) | (WDRESET=1)
// 6. Enable the WWDT interrupt in the NVIC.
// 7. Turn on the green LED.
// 8. Do a feed sequence to enable the WWDT.
// 9. Enter the main while(1) loop.
//    A. WWDT interrupts will feed the WWDT as long as they occur inside the window.
//    B. Users can try lowering the WINDOW value so that the WARNINT interrupt, and the feed sequence
//       in the ISR, happen while the WDTV register is greater-than-or-equal-to the WINDOW value,
//       to verify that the WWDT reset will occur, and that the reentrant code will become stuck in the
//       while(1) loop of step 2, with the red LED illuminated.


// WWDT ISR
void WDT_IRQHandler(void) {
  uint32_t temp;

  // Clear the WDINT interrupt flag by writing '1' to it (go figure)
  LPC_WWDT->MOD |= (1<<WDINT);

  // Save the currently enabled interrupts
  temp = NVIC->ISER[0];

  // Disable all interrupts around the feed sequence, re-enable before returning
  NVIC->ICER[0] = 0xFFFFFFFF;
  LPC_WWDT->FEED = 0xAA;
  LPC_WWDT->FEED = 0x55;
  NVIC->ISER[0] = temp;

  return;
}


// main routine
int main(void) {
  // Turn on peripheral clocks to relevant peripherals.
  LPC_SYSCON->SYSAHBCLKCTRL0 |= (WWDT)|(GPIO0);
  Config_LED(LED_GREEN);
  Config_LED(LED_RED);

  // Examine the WDTOF flag. If set, this reset is due to a watchdog event. Stop here.
  if (LPC_WWDT->MOD & (1<<WDTOF)) {
    // Clear the WDTOF flag by writing '0' to it (go figure)
    LPC_WWDT->MOD &= ~(1<<WDTOF);
    LED_Off(LED_GREEN);
    LED_On(LED_RED);
    while(1);
  }

  // Apply power to the low power oscillator.
  LPC_SYSCON->PDRUNCFG &= ~(LPOSC_PD);
  //Enable the watchdog interrupt for wake-up
  LPC_SYSCON->STARTERP1 |= (1<<12);
  //Enable the LPOSC clock to the WWDT
  LPC_SYSCON->LPOSCCLKEN |= 1<<WWDT_CLK_EN;
  
  // Configure the WWDT
  LPC_WWDT->TC = 0x800;
  LPC_WWDT->WINDOW = 0x400;
	
  LPC_WWDT->WARNINT = 0x3FF;
  LPC_WWDT->MOD = (0<<WDTOF)|(1<<WDEN)|(1<<WDRESET);

  // Enable the WWDT interrupt in the NVIC.
  NVIC_EnableIRQ(WDT_IRQn);

  // Turn on the green LED.
  LED_On(LED_GREEN);

  // Do a feed sequence to enable the WWDT.
  LPC_WWDT->FEED = 0xAA;
  LPC_WWDT->FEED = 0x55;

  // Enter the main while(1) loop. WWDT interrupts will feed the WWDT as long as they occur inside the window.
  while(1) {
  } // end of while(1)
} // end of main
