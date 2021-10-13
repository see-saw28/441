// Programme de base TP II ENS

#include "LPC8xx.h"
#include "syscon.h"
#include "lib_ENS_II1_lcd.h"
#include "rom_api.h"

#define BP1  LPC_GPIO_PORT->B0[13]
#define BP2  LPC_GPIO_PORT->B0[12]
#define LED1 LPC_GPIO_PORT->B0[19]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]





void delay(int value){
	for(;value>0;value--);
}

int main(void) {
	uint32_t secondes = 0;

	// Activation du périphérique d'entrées/sorties TOR
	LPC_SYSCON->SYSAHBCLKCTRL0 |= GPIO;

	//Configuration en sortie des broches P0_11, 17, 19 et 21
	LPC_GPIO_PORT->DIR0 |= (1 << 11) | (1 << 17) | (1 << 19) | (1 << 21);

	//Mise à 1 de la sortie P0_11 et mise à 0 des autres
	LPC_GPIO_PORT->PIN0 = (1 << 11);

	//Initialisation de l'afficheur lcd et affichage d'un texte
	init_lcd();

	char snum[5];


	while (1) {

		//Recopie de l'état du bouton 1 (P0_13) sur la LED2 (P0_17)
		LED2 = BP1;

		//affichage sur l'écran
		lcd_gohome();
		sprintf(snum,"%4d", secondes);
		lcd_puts(snum);

		//attente brève
		delay(1000000);
		secondes++;
	} // end of while(1)

} // end of main
