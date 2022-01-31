${ProjName}:
I2C_Temperature


Intended purpose:
* To demonstrate the configuration and use of the I2C-bus interface in both master modes and communicate
with the I2C slave temperature sensor.
* Temperature reading is sent out to the debug UART.
* Connect a terminal emulator (9600/8/N/1) to the debug UART COM port.
* See chip_setup.h and Serial.c to setup the debug com port.

* The data sheet of on-board temperature sensor LM75BD can be found under:
https://www.nxp.com/docs/en/data-sheet/LM75B.pdf

External connections:
* None (see note).
* note: This example uses I2C0 which is available on the pads P0.7 and P0.14.
        The MAX board has pullups on these pins. If you use another I2C instance, the addition of 
        external pullups on the pins you choose may be required.

Program Flow:
* This example runs at system_ahb_clk = 15 MHz. See SystemInit() and chip_setup.h.
* The debug UART is initialized at 9600/8/N/1, UART TX and RX use p0.4 and p0.0 respectively. 
* Use finger or freeze spray on the on-board temperature sensor LM75BD, the temperature reading should change. 
* This example runs from Flash. 


To run this code:
* Build, Debug, Run.
or
* Download to flash, then reset.


Note: It may be necessary to power-cycle the board in order to regain control of the reset button after programming.
      

