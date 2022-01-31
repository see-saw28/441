Project_Name
============
- PMU_Modes



Intended purpose
================
- To demonstrate the entry into, and wakeup from, the four low-power modes, 
  while allowing the user to measure current consumption and wakeup time.
- The wakeup pin is currently hard-coded to PIO0_12 for sleep, deep sleep, power-down. For deep power down, wake up pin is PIO0_13.
  Since on initial rev LPCXpresso board, PIO0_17 is connected to pot, it can't be used to wake-up from deep power-down mode.
  Once updated board is available, software will change to pin interrupt from PIO0_12 to PIO_17 to allow one pin to wake-up from all 4 modes.



Hardware requirements
=====================
- Micro USB cable
- LPC8xx mbed/Xpresso Max board
- Personal Computer



Board settings / external connections
=====================================
- One or more wakeup pins, connected to external pullups, which can be grounded.
- Potentiometer connected on PIO0_10 should be tweaked to 3.30 v to get correct power measurements.
- Remove resistor (if populated) R37 (10k)



Functional description
======================
- Input/output is accomplished by UART0 communicating with a
  terminal emulator running on the PC, via a USB-to-RS232 cable using 9600/8/N/1.
- One or more port pins are configured as wakeup pins, and used to wakeup 
  the target from the low power modes.
- A falling edge on a wakeup pin is used to trigger the wakeup interrupt from 
  Sleep, Deep-sleep, and Power-down modes.
- Grounding an EXTERNALLY pulled up wakeup pin will wakeup the target from Deep
  power-down mode. Wakeup pins MUST be pulled high externally to enter Deep 
  power-down mode. On the board this pullup may already be in place.
- The user is prompted to enter '0', '1', '2', or '3' to enter Sleep, Deep-sleep, 
  Power-down, or Deep power-down modes, respectively.
- The target enters the low power mode selected, and is awakened by grounding
  an enabled wakeup pin.
- After wakeup from Deep power-down, the contents of the wakeup
  source register (WUSRCREG) in the PMU are printed, to indicate which of the 
  wakeup pins caused the wakeup.

- This example runs from the default 12MHz FRO.
- This example runs from Flash. 



Power mode        Typical VDD_main current running from 12 MHz FRO
==================================================================
while(1)          1.4   mA
Sleep             0.58  mA
Deep sleep        124.0 uA
Power down        6.00  uA
Deep power down   0.16  uA



To run this code
================
- Build, Debug, Run.
- or
- Download to flash, then reset.

- Note: It may be necessary to power-cycle the board in order to regain control of the reset button after programming.
      

                                



