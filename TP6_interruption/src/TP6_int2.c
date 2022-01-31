/*
 ===============================================================================
 Name        : test2_LPC804.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#include <cr_section_macros.h>
#include "LPC8xx.h"
#include "syscon.h"
#include "swm.h"
#include "syscon.h"
#include "utilities.h"
#include "uart.h"
#include "chip_setup.h"
#include "lib_ENS_II1_lcd.h"
#include "rom_api.h"
#include "stdio.h"
#include "math.h"
#include "ctimer.h"
#include "core_cm0plus.h"
#include "mrt.h"
#include "core_cm0plus.h"

#include "lib_ENS_II1_lcd.h"


char new_RX,old_RX;

void UART0_IRQHandler() {
	new_RX=LPC_USART0->RXDAT;
}

int main(void) {
	//Activation de l'horloge des périphériques et configuration des sorties GPIO
	LPC_SYSCON->SYSAHBCLKCTRL0 |= SWM | UART0; //module interruption activé aussi
	// Connect UART0 TXD, RXD signals to port pins
	ConfigSWM(U0_TXD, DBGTXPIN);
	ConfigSWM(U0_RXD, DBGRXPIN);

	// Select frg0clk as the source for fclk0 (to UART0)
	LPC_SYSCON->UART0CLKSEL = FCLKSEL_FRO_CLK;

	// Give USART0 a reset
	LPC_SYSCON->PRESETCTRL0 &= (UART0_RST_N);
	LPC_SYSCON->PRESETCTRL0 |= ~(UART0_RST_N);

	// Configure the USART0 baud rate generator
	LPC_USART0->BRG = 7;

	// Configure the USART0 CFG register:
	// 8 data bits, no parity, one stop bit, no flow control, asynchronous mode
	LPC_USART0->CFG = DATA_LENG_8|PARITY_NONE|STOP_BIT_1;

	// Clear any pending flags, just in case
	LPC_USART0->STAT = 0xFFFF;

	// Enable USART0
	LPC_USART0->CFG |= UART_EN;

	//interruption à la reception
	LPC_USART0->INTENSET = RXRDY;
	NVIC_EnableIRQ(UART0_IRQn);

	init_lcd();
	lcd_gohome();
	lcd_puts("UE441 - TP6");

	while (1) {
		if (new_RX != old_RX){
			lcd_position(1,0);
			lcd_putc(new_RX);
			old_RX = new_RX;
		}

	}
	return 0;
}

