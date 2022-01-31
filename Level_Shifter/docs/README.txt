${ProjName}:
Level_Shifter


Intended purpose:
* To demonstrate the level shifter functionality that allows up to two selected signals to be routed from
user-selected pins in one voltage domain to selected pins in the alternate domain.
* The functionality is provided in the Switch Matrix.

In this example, the P0_0 and P0_7 bits are configured to be normally high turning the LED off. When P0_0 is connected to GND,
level shifting functionality routes the 0 voltage level from P0_0 to P0_7 turning the LED on.

External connections:
* Connect LVLSHFT_IN0 port bit to GND.
* Connect LVLSHFT_OUT0 port bit to P0_13 (Red LED).
* Connect LVLSHFT_IN0 port bit to a VDD to see the Red LED turn off.

Program Flow:
* Enable clock to Switch matrix and GPIO
* Initialize the LED pin and GPIO port bits. LVLSHFT_IN0 (PO_0 here) is configured as an input 
  and LVLSHFT_OUT0 (P0_7 here) is configured as an output driving 1, so that the Red LED is initially OFF.
* Byte 15:8 in PINASSIGN6 register (LVLSHFT_IN0) is configured with the pin number to be assigned to it (0x0 in this example).
* Byte 31:24 in PINASSIGN6 register (LVLSHFT_OUT0) is configured with the pin number to be assigned to it (0x7 in this example).
* Every other byte retain the reset value (0xFF).


To run this code:
* Build, Debug, Run.
or
* Download to flash, then reset.


Note: It may be necessary to power-cycle the board in order to regain control of the reset button after programming.
      

