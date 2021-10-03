// Programme de base TP II ENS

#include "LPC8xx.h"
#include "syscon.h"
#include "lib_ENS_II1_lcd.h"
#include "ctimer.h"
#include "stdio.h"

#define BP1  LPC_GPIO_PORT->B0[13]
#define BP2  LPC_GPIO_PORT->B0[12]
#define LED1 LPC_GPIO_PORT->B0[19]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]

void lcd_clear(){
	lcd_position(0,0);
	lcd_puts("                ");
	lcd_position(1,0);
	lcd_puts("                ");
	lcd_position(0,0);

}

int main(void) {

	uint32_t i = 0;

	// Activation du périphérique d'entrées/sorties TOR, du timer et switch matrix
	LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<25) | (SWM) | GPIO;

	//Configuration en sortie des broches P0_11, 17, 19 et 21
	LPC_GPIO_PORT->DIR0 |= (1 << 11) | (1 << 17) | (1 << 19);

	LPC_CTIMER0->TCR=(1<<CEN);

	LPC_CTIMER0->PR=14999;


	LPC_CTIMER0->MCR |= (1<<1);


	LPC_CTIMER0->MR[0]=1000;
	LPC_CTIMER0->EMR|=(3<<4);

	LPC_SWM->PINASSIGN4 &= ~(0xFF);
	LPC_SWM->PINASSIGN4 |= 11; //metronome sur la led 3



	//Initialisation de l'afficheur lcd et affichage d'un texte
	init_lcd();
	lcd_puts("ORGUE");



	int button_bp1 = 0, fm_bp1 = 0;
	int button_bp2 = 0, fm_bp2 = 0;

	int ecran = 0;
	const int nombre_pages = 5;
	int option = 0, home = 1, option_en = 0;


	int refresh = 0;
	int tempo = 100;
	const int tempo_min = 40, tempo_max = 220;

	char display[100];



	while (1) {


		refresh = 0;
		fm_bp1 = 0;
		fm_bp2 = 0;

		LED1 = home;
		LED2 = option;
		LED3 = option_en;

		//lecture bouton
		if ((BP1==0)&&(button_bp1==1)){
			fm_bp1=1;
			refresh = 1;


		}

		button_bp1 = BP1;

		if ((BP2==0)&&(button_bp2==1)){
			fm_bp2 = 1;
			refresh = 1;
		}

		button_bp2 = BP2;

		//gestion bouton
		if (home){
			if (fm_bp2){
				home = 0;
				option = 1;
			}

		}

		else if (option){
			if (fm_bp1){
				if (ecran<nombre_pages){
					ecran++;
				}

				else {
					ecran = 1;
				}
			}
			else if (fm_bp2){
				if (ecran!=nombre_pages){
					option_en = 1;
					option = 0;
				}
				else{
					home = 1;
					option = 0;
				}
			}
		}

		else if (option_en){
			switch(ecran){
				case 1 :

					break;
				case 2 :
					if (fm_bp1){
						if (tempo < tempo_max){
							tempo += 5;
						}

						else {
							tempo = tempo_min;
						}

						LPC_CTIMER0->MR[0]=1000*60/tempo;
					}
					else if (fm_bp2){
						home = 0;
						option = 1;
					}

					break;
				case 3 :

					break;
				case 4 :

					break;
				case 5 :
					if (fm_bp2 || fm_bp1){
						home = 1;
						option = 0;
					}

					break;

				}
		}




		//gestion affichage
		if (refresh){

			lcd_clear();
			if (home){
				lcd_puts("ORGUE");

			}

			else if (option){
				lcd_gohome();
				switch(ecran){
				case 1 :
					sprintf(display,"%16s", "MANUEL/AUTO");
					break;
				case 2 :
					sprintf(display,"%16s", "TEMPO");
					break;
				case 3 :
					sprintf(display,"%16s", "CHOIX MUSIQUE");
					break;
				case 4 :
					sprintf(display,"%16s", "VOLUME");
					break;
				case 5 :
					sprintf(display,"%16s", "EXIT");
					break;

				}

				lcd_puts(display);
			}

			else if (option_en){
				lcd_gohome();
				switch(ecran){
				case 1 :
					lcd_puts("ORGUE1");
					break;
				case 2 :

					lcd_position(0,0);
					sprintf(display,"%16s", "TEMPO");
					lcd_puts(display);
					lcd_position(1,0);
					sprintf(display,"%16d", tempo);
					lcd_puts(display);


					break;
				case 3 :
					lcd_puts("ORGUE3");
					break;
				case 4 :
					lcd_puts("ORGUE4");
					break;
				case 5 :
					lcd_puts("EXIT");
					break;

				}
			}



		}









	} // end of while(1)

} // end of main
