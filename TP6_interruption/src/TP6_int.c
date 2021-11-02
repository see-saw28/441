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
#include <stdio.h>
#include "LPC8xx.h"
#include <stdint.h>
#include "fro.h"
#include "rom_api.h"
#include "syscon.h"
#include "swm.h"
#include "i2c.h"
#include "ctimer.h"
#include "core_cm0plus.h"

#include "lib_ENS_II1_lcd.h"

#define BP1 LPC_GPIO_PORT->B0[13]
#define BP2 LPC_GPIO_PORT->B0[12]
#define LED1 LPC_GPIO_PORT->B0[19]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]

int32_t num_led = 0;

void allumer_led(uint32_t numero_led) {
	switch (numero_led) {
	case 1:
		LED1= 1;
		break;
		case 2: LED2 = 1;
		break;
		case 3: LED3 = 1;
		break;
		case 4: LED4 = 1;
		break;
	}
	return;
}

void eteindre_led(uint32_t numero_led) {
	switch (numero_led) {
	case 1:
		LED1= 0;
		break;
		case 2: LED2 = 0;
		break;
		case 3: LED3 = 0;
		break;
		case 4: LED4 = 0;
		break;
	}
	return;
}

void PININT0_IRQHandler(void) {
	if (LPC_PIN_INT->FALL & (1<<0)) {
		if (num_led < 4) {
			num_led++;
			allumer_led(num_led);
		}
	    LPC_PIN_INT->FALL = 1<<0;
	}
	return;
}

void PININT1_IRQHandler(void) {
	if (LPC_PIN_INT->FALL & (1<<1)) {
		if (num_led > 0) {
			eteindre_led(num_led);
			num_led--;
		}
	    LPC_PIN_INT->FALL = 1<<1;
	}
	return;
}



int main(void) {

	LPC_PWRD_API->set_fro_frequency(30000);

	LPC_SYSCON->SYSAHBCLKCTRL0 |= UART0 | SWM |GPIO | CTIMER0 | GPIO_INT;

	NVIC->ISER[0] |= 1<<23;//ISE_PININT0;
	NVIC->IP[6] |= 0<<6; //prioritaire
	LPC_SYSCON->PINTSEL0 = 13; //BP1

	NVIC->ISER[0] |= 1<<24; //ISE_PININT1;
	NVIC->IP[6] |= 0<<14; //moins prioritaire
	LPC_SYSCON->PINTSEL1 = 12; //BP1

	LPC_PIN_INT->IENF = 3; //PININT0 et PININT1


	/*
	 * code assembleur pour mettre la totalité de la ram dans un état connu (OxA5 partout)
	 * attention : à exécuter au début de main
	 * PV 12/11/2019

	asm volatile(
			".syntax unified\n\t"
			"ldr r4, =0x10000000\n\t"
			"ldr r5, =0xA5A5A5A5\n\t"
			"ldr r6, =0x10001000\n\t"
			"bcl: str r5, [r4, #0]\n\t"
			"adds r4,#4\n\t"
			"cmp r4,r6\n\t"
			"bne bcl\n\t"
	);
*/

	//RAZ de la variable globale (écrasée par le code ASM)
	num_led = 0;

	//Activation de l'horloge des périphériques et configuration des sorties GPIO
	LPC_SYSCON->SYSAHBCLKCTRL0 |= IOCON | GPIO0 | SWM | CTIMER0 | GPIO_INT; //module interruption activé aussi
	LPC_GPIO_PORT->DIR0 |= 0x002A0800; // broches connectées aux leds en sortie

	while (1) {




	}
	return 0;
}

