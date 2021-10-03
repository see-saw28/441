// Programme de base TP II ENS

#include "LPC8xx.h"
#include "syscon.h"
#include "lib_ENS_II1_lcd.h"

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
	lcd_puts("CHENILLARD");

	int led[4]={1,0,0,0};
	int active = 0;
	int button = 0;

	while (1) {

		LPC_GPIO_PORT->B0[19] = led[0];
		LPC_GPIO_PORT->B0[17] = led[1];
		LPC_GPIO_PORT->B0[21] = led[2];
		LPC_GPIO_PORT->B0[11] = led[3];

		//changement de sens
		if ((LPC_GPIO_PORT->B0[13]==0)&&(button==1)){
					active = !active;
		}
		button = LPC_GPIO_PORT->B0[13];

		//decalage en fonction du sens
		if (active){
			int tampon = led[3];
			led[3]=led[2];
			led[2]=led[1];
			led[1]=led[0];
			led[0]=tampon;
		}
		else {
			int tampon = led[0];
			led[0]=led[1];
			led[1]=led[2];
			led[2]=led[3];
			led[3]=tampon;
		}
		//attente brève
		for (i = 0; i < 200000; i++);

	} // end of while(1)

} // end of main
