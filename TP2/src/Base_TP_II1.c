// Programme de base TP II ENS

#include "LPC8xx.h"
#include "syscon.h"
#include "lib_ENS_II1_lcd.h"

#define BP1  LPC_GPIO_PORT->B0[13]
#define BP2  LPC_GPIO_PORT->B0[12]
#define LED1 LPC_GPIO_PORT->B0[19]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]

uint8_t compteur =0;

int main(void) {

	uint32_t i = 0;

	// Activation du périphérique d'entrées/sorties TOR
	LPC_SYSCON->SYSAHBCLKCTRL0 |= GPIO;

	//Configuration en sortie des broches P0_11, 17, 19 et 21
	LPC_GPIO_PORT->DIR0 |= (1 << 11) | (1 << 17) | (1 << 19) | (1 << 21);

	//Mise à 1 de la sortie P0_11 et mise à 0 des autres
	LPC_GPIO_PORT->PIN0 = (1 << 11);

	//Initialisation de l'afficheur lcd et affichage d'un texte
	init_lcd();
	lcd_puts("TP ENS II1 2021");

	while (1) {

		//Recopie de l'état du bouton 1 (P0_13) sur la LED2 (P0_17)
		LED2 = BP1;

		//Recopie du complément de l'état du bouton 2 (P0_12) sur la LED1 (P0_19)
		LED1 = !BP2;

		//Complémentation de l'état des LED3 (P0_21) et LED4 (P0_11)
		LPC_GPIO_PORT->NOT0 = (1 << 11) | (1 << 21);

		//attente brève
		for (i = 0; i < 100000; i++);

		compteur = compteur + (BP2 ? 1: -1);

	} // end of while(1)

} // end of main
