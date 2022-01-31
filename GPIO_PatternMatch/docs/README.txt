${ProjName}:
GPIO_PatternMatch

Intended purpose:
* To demonstrate the configuration and use of the Pattern Match engine.
* User input is via the debug UART.
* Connect a terminal emulator (9600/8/N/1) to the debug UART COM port.
* See Serial.c to setup the debug COM port.


 In this example we setup the pattern match engine to interrupt on the following Boolean expression:

 (IN0re AND IN1re) + (IN2re)
 where subscript re = rising edge

 The ISR is written to handle three conditions:
 1. Term0 and Term1 are asserted, in this case only the green LED is lit.
 2. Term0 only is asserted, in this case only the red LED is lit.
 3. Term1 only is asserted, in this case only the blue LED is lit.

 The interesting case is when both Term0 and Term1 become true and assert their IRQs together.
 Since we only want to enter the ISR once when this happens, we must take care to clear the
 interrupt-generating edge-detect logic, and the Interrupt Pending bits in the NVIC, and the
 edge-detect history in the PMCTRL register before returning from the interrupt. Please see
 PatternMatch_ISR function in GPIO_PatternMatch_ISR.c.

Here is the configuration for the pin interrupts, the GPIO port bits, and the pattern match engine. Each bit slice is programmed to represent one component of one minterm (product term) within the boolean expression. The interrupt request associated with the last bit slice for a particular minterm will be asserted whenever that minterm is matched.

  //   --------------------------------------------------------------------------------
  //   PINTSEL:          IN0     IN1     IN2     IN3     IN4     IN5     IN6     IN7
  //   GPIO port bit:    P0.18   P0.19   P0.20   unused   unused  unused  unused  unused

  //   Boolean minterm:  |-Term0-|       |---- Term1 ----|     |---------------Term2 ----------------|
  //   Bit-slice:        0       1       2       					3       4          5       6       7
  //   Input:            IN0     IN1     		IN2     		unused     unused     unused  unused  unused
  //   Condition:        re      re      		re      			-      	-          -       -       -

  //   Pin interrupt:    0       1       2       3       4       5       6       7
  //   Associated term:  unused  Term0   Term1  unused  unused   unused  unused  unused
  //   --------------------------------------------------------------------------------

 The pattern match inputs can be activated in two ways:

 1. By configuring the port bits as inputs, and driving the package pins high and low externally.

 2. By configuring the port bits as outputs, and writing to their port bit registers via software.
    The enabled pattern match logic will always monitor the state of the pins,
    even when configured as outputs.

 We will use approach #2. 



Program Flow:
* This example runs with:
    system_ahb_clk = 15 MHz.
    See chip_setup.h and SystemInit()

* main() routine
  1. Clocks are enabled.
  
  2. P0.18 - P0.20 are assigned to pattern match inputs IN0 - IN2.
  
  3. The bit slices are configured from IN0, IN1 and IN2.
  
  4. The conditions (rising, falling, etc.) and the Boolean minterms are configured.
  
  5. PORT0.18 - PORT0.20 are configured as outputs driving '0'.
  
  6. PININT0 and PININT1 IRQs are enabled, controlled by the pattern match interrupts,
     and code enters a while(1) loop.
  
  7. Execution pauses while awaiting user input of one hex digit (with no prefix) via the terminal.
  
  7. The value is written to P0[20:18] where the bits are detected by the pattern match engine.
  
  8. When rising edges on bits 0 or/and 1 have been detected, the LEDs light up in the interrupt routine based on the condition.
     
  9. On exit from the interrupt routine, edge detect history is cleared from the pattern match control register.
 
* For example:
  The inputs in order '0', '3', '4', '0', '7' will turn on RED, BLUE and GREEN LEDs.


To run this code:
* Build, Debug, Run
or
* Download to flash, then reset.


Note: It may be necessary to power-cycle the board in order to regain control of the reset button after programming.



