/*
 * clignotant_asm_full.c
 *
 *  Created on: 15 oct. 2019
 *      Author: varoqui
 */
#include <cr_section_macros.h>
#include "LPC8xx.h"
#include "syscon.h"

// GPIO
#define blue  (1<<11) // led bleue sur P0.11

void main2();

int main(void) {
	//LPC_SYSCON->SYSAHBCLKCTRL0 |= (IOCON | GPIO0);
	//LPC_GPIO_PORT->DIR0 |= blue;
	main2();
}

