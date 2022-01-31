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
        int cpt;
        LPC_SYSCON->SYSAHBCLKCTRL0 |= (IOCON | GPIO0);
        LPC_GPIO_PORT->DIR0 |= blue;

    while(1) {
        LPC_GPIO_PORT->NOT0 = blue;
        for(cpt=0;cpt<200000;cpt++);
    }
}
