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
	LPC_GPIO_PORT->DIR0 |= (1 << 17)|(1<<21);
	//chrono initialement actif
	LPC_CTIMER0->TCR=(1<<CEN);
	//precision à la milliseconde
	LPC_CTIMER0->PR=11;

	int bp1_state = 0,bp2_state = 0, old_timer = 0;//memorisation
	char snum[100];//affichage
	//Initialisation de l'afficheur lcd et affichage d'un texte
	init_lcd();
	lcd_puts("TP3 chrono :");

	while (1) {
		//si BP1 & BP2 on reset
		if ((BP1 == 0) && (BP2 == 0)){
			LPC_CTIMER0->TCR=(1<<CRST);
		}

		//si fm de BP1 on lance le chrono
		else if ((bp1_state==1) && (BP1==0)){
			LPC_CTIMER0->TCR=(1<<CEN);
		}

		//si fm de BP2 on arret le chrono
		else if ((bp2_state==1) && (BP2==0)){
			LPC_CTIMER0->TCR=0;
		}
		//memorisation de l'etat des boutons
		bp1_state = BP1;
		bp2_state = BP2;

		//actualisation de l'affichage ssinouvelle valeur à afficher
		if (LPC_CTIMER0->TC != old_timer){
			sprintf(snum,"%16d", LPC_CTIMER0->TC);
			lcd_position(1,0);
			lcd_puts(snum);
		}
		//memorisation ancienne valeur du chrono
		old_timer = LPC_CTIMER0->TC;
	} // end of while(1)
} // end of main
