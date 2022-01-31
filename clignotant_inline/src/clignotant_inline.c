/*
===============================================================================
 Name        : clignotant.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include <cr_section_macros.h>
#include "LPC8xx.h"
#include "syscon.h"

// GPIO
#define blue  (1<<11) // led bleue sur P0.11

int main(void) {
        LPC_SYSCON->SYSAHBCLKCTRL0 |= (IOCON | GPIO0);
        LPC_GPIO_PORT->DIR0 |= blue;
    while(1) {
        asm volatile(
        		".syntax unified\n\t"
        		".equ GPIO_PORT_NOT0,0xa0002300\n\t"
        		".equ blue,(1<<11)\n\t"
        		".equ init_cpt,200000\n\t"
                "push {r4, r5, r6}\n\t"
                "ldr r4, =GPIO_PORT_NOT0\n\t"
                "ldr r5, =blue\n\t"
                "str r5, [r4, #0]\n\t"
        		"ldr r6, =init_cpt\n\t"
        		"boucle:\n\t"
        		"subs r6, #1\n\t"
        		"bne boucle\n\t"
        		"pop {r4, r5, r6}"
        );
    }
}
