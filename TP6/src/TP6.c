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

uint32_t carre(uint32_t operande) {
	return operande * operande;
}

uint32_t allumer_led(uint32_t numero_led, uint32_t valeur) {
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
	return carre(valeur);
}

uint32_t eteindre_led(uint32_t numero_led, uint32_t valeur) {
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
	return carre(valeur);
}

int main(void) {

	LPC_PWRD_API->set_fro_frequency(30000);
	/*
	 * code assembleur pour mettre la totalité de la ram dans un état connu (OxA5 partout)
	 * attention : à exécuter au début de main
	 * PV 12/11/2019
	 */
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

	//Création et initialisation des variables locales
	uint32_t bp1, bp1_old = 1, bp2, bp2_old = 1, ma_variable_locale = 4;
	//RAZ de la variable globale (écrasée par le code ASM)
	num_led = 0;

	//Activation de l'horloge des périphériques et configuration des sorties GPIO
	LPC_SYSCON->SYSAHBCLKCTRL0 |= IOCON | GPIO0 | SWM | CTIMER0 | GPIO_INT; //module interruption activé aussi
	LPC_GPIO_PORT->DIR0 |= 0x002A0800; // broches connectées aux leds en sortie

	while (1) {
		//lecture de l'état des boutons puis détection des fronts descendants
		bp2 = BP2;
		bp1 = BP1;

		if (!bp1 && bp1_old) {
			if (num_led < 4) {
				num_led++;
				ma_variable_locale = allumer_led(num_led,ma_variable_locale);
			}
		}
		if (!bp2 && bp2_old) {
			if (num_led > 0) {
				ma_variable_locale = eteindre_led(num_led,ma_variable_locale);
				num_led--;
			}
		}
		bp1_old = bp1;
		bp2_old = bp2;
	}
	return 0;
}

