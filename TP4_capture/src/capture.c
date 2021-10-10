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

	//timer enable
	LPC_CTIMER0->TCR=(1<<CEN);

	//precision microseconde
	LPC_CTIMER0->PR=1499;

	/*
	//recopie de la valeur du compteur sur CR0 sur front descendant
	LPC_CTIMER0->CCR=(1<<CAP0FE);

	//BP1 sur CAP0
	LPC_SWM->PINASSIGN3 &= ~(0xFF<<8);
	LPC_SWM->PINASSIGN3 |= 13<<8;
	*/

	//recopie de la valeur du compteur sur CR1 sur front montant
	LPC_CTIMER0->CCR=(1<<CAP1RE);

	//BP1 sur CAP1
	LPC_SWM->PINASSIGN3 &= ~(0xFF<<16);
	LPC_SWM->PINASSIGN3 |= 13<<16;


	/*
	//440Hz ie comp0 = 100 000 / 440
	LPC_CTIMER0->MR[3]=227;

	//mise a zero / MR3
	LPC_CTIMER0->MCR |= (1<<MR3R);


	//LPC_CTIMER0->EMR|=(3<<4);

	//pwm 50% ie comp1
	LPC_CTIMER0->MR[1]=50;

	//mat1
	LPC_CTIMER0->PWMC = (1<<PWMEN1);


	LPC_SWM->PINASSIGN4 &= ~(0xFF<<8);
	LPC_SWM->PINASSIGN4 |= 19<<8;

	*/



	int old_bp1=0;


	char snum[100];


	//Initialisation de l'afficheur lcd et affichage d'un texte
	init_lcd();

	lcd_puts("capture");

	while (1) {

		if (old_bp1 && (BP1==0)){
			sprintf(snum,"%16d", LPC_CTIMER0->CR[1]);
			lcd_position(1,0);
			lcd_puts(snum);
		}

		if (BP1){
			LPC_CTIMER0->TCR=(1<<CRST);
		}

		else {
			LPC_CTIMER0->TCR=(1<<CEN);
		}


		old_bp1=BP1;



		//LED1 = LPC_GPIO_PORT->B0[21];
	} // end of while(1)

} // end of main
