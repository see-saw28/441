// Programme de base TP II ENS

#include "LPC8xx.h"
#include "syscon.h"
#include "lib_ENS_II1_lcd.h"
#include "rom_api.h"
#include "ctimer.h"
#include "stdio.h"

#define BP1  LPC_GPIO_PORT->B0[13]
#define BP2  LPC_GPIO_PORT->B0[12]
#define LED1 LPC_GPIO_PORT->B0[19]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]

//LPC_PWRD_API→set_fro_frequency(30000);

int main(void) {
	LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<25) | (SWM) | GPIO;

	//Configuration en sortie des broches P0_11, 17, 19 et 21
	LPC_GPIO_PORT->DIR0 |= (1 << 17)|(1<<21) | (1<<19);

	//precision microseconde
	LPC_CTIMER0->PR=1499;

	//recopie de la valeur du compteur sur CR1 sur front montant
	LPC_CTIMER0->CCR=(1<<CAP1RE);

	//BP1 sur CAP1
	LPC_SWM->PINASSIGN3 &= ~(0xFF<<16);
	LPC_SWM->PINASSIGN3 |= 13<<16;

	int old_timer=0;//ancienne valeur temps de capture
	char snum[100];//string à afficher

	//Initialisation de l'afficheur lcd et affichage d'un texte
	init_lcd();
	lcd_puts("capture");

	while (1) {
		if (BP1){//bouton relaché
			if (old_timer !=LPC_CTIMER0->CR[1]){//affichage si nouvelle valeur ie un appui réalisé
				sprintf(snum,"%16d", LPC_CTIMER0->CR[1]);
				lcd_position(1,0);
				lcd_puts(snum);
				old_timer = LPC_CTIMER0->CR[1];
			}
			LPC_CTIMER0->TCR=(1<<CRST);//reinit du timer
		}

		else {//bouton enfoncé
			LPC_CTIMER0->TCR=(1<<CEN);//activation du timer
		}

	} // end of while(1)

} // end of main
