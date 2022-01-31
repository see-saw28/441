${ProjName}:
WWDT


Intended purpose:
* To demonstrate the configuration and use of the Windowed Watchdog Timer using both its windowing and
  warning interrupt features.


Functional description / configuration:
 * The watchdog oscillator and wdt_osc_clk are configured, the WWDT is configured to count down from
   0x800 with the WINDOW set to 0x400 and the WARNINT set to 0x3FF.
 * The feed sequence is done inside the WARNINT ISR.
 * The green LED is lit, and the main while(1) loop (which does nothing) is entered.
 * If a WWDT reset occurs, the re-entrant code will illuminate the red LED and stop.


External connections:
* None


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


  
  
* This example runs at 15 MHz. See function SystemInit() and chip_setup.h.

* This example runs from flash, but does not lend itself to running under a debug session,
  if the watchdog reset is going to be triggered (by modifying the code as explained above).
  
* It is recommended that if the watchdog reset is going to be triggered, the code be compiled (built),
  then loaded to flash. Once loaded, reset the target on the board.


To run this code:
  1. Build
  2. Debug
OR ...
  1. Build
  2. Program the image 
  3. Reset

Note: It may be necessary to power-cycle the board in order to regain control of the reset button after programming.




