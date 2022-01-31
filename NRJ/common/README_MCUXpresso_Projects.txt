Tool chain versions supported
=============================
- This package was built using MCUXpresso IDE v10.0.1_alpha [Build 371]



Steps for building this workspace when it's clean
=================================================
1. Click 'Build all projects [flash]' in the Quickstart Panel.



Note about using the debug COM port
=================================================================
Most projects in this package are interactive and communicate with the user
via a terminal emulator running on the host PC, connected either to the VCOM
serial port, or to a USB-to-UART cable connected to pins on the Arduino header.

It is recommended to run the project UART0_Terminal first, in order to
establish that communication between the terminal emulator (via the VCOM
serial port and/or the USB COM port with USB-to-UART cable) and the LPC8xx are
working.

* To use the VCOM Serial Port:
    1. No external connections are necessary.
    2. In the project's inc/chip_setup.h, near the bottom, make sure there is 
       #define USE_VCOM_PORT 1

* To use a USB-to-RS232 breakout cable:
    1. There are three external connections necessary: UART RXD and TXD based
       on the #defines near the bottom of the project's inc/chip_setup.h, plus
       a ground connection:
       DBGTXPIN = breakout cable RXD
       DBGRXPIN = breakout cable TXD
       Board GND = breakout cable GND

