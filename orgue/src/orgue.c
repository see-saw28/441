// Programme de base TP II ENS
#include "LPC8xx.h"
#include "syscon.h"
#include "swm.h"
#include "syscon.h"
#include "utilities.h"
#include "uart.h"
#include "chip_setup.h"
#include "lib_ENS_II1_lcd.h"
#include "rom_api.h"
#include "stdio.h"
#include "math.h"
#include "ctimer.h"
#include "core_cm0plus.h"
#include "mrt.h"

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

void MRT_IRQHandler(void){

	LED2=!LED2;


	LPC_MRT->Channel[0].STAT |=0;
}

int main(void) {
	LPC_PWRD_API->set_fro_frequency(30000);


	// Activation du périphérique d'entrées/sorties TOR, du timer et switch matrix
	LPC_SYSCON->SYSAHBCLKCTRL0 |= UART0 | SWM |GPIO | CTIMER0 | MRT | IOCON;

	LPC_GPIO_PORT->DIR0 |= (1 << 17)|(1<<21) | (1<<19)|(1<<11);

	LED1=1;
	LED2=0;
	 ///////////
	// TIMER //
   ///////////

  //precision 0.1 microseconde
	LPC_CTIMER0->PR=149;

	//440Hz ie comp0 = 100 000 / 440
	LPC_CTIMER0->MSR[3]=227;

	//mise a zero par rapport à MR3
	LPC_CTIMER0->MCR |= (1<<MR3R)|(1<<25)|(1<<27);

	//pwm pour générer un signal rectangulaire ie reglage de comp1
	LPC_CTIMER0->MSR[1]=2;

	LPC_CTIMER0->TCR=0;
	//mat1
	LPC_CTIMER0->PWMC = (1<<PWMEN1);

	//mat1 relié a LED1 qui est relié au HP
	LPC_SWM->PINASSIGN4 &= ~(0xFF<<8);
	LPC_SWM->PINASSIGN4 |= 19<<8;


	 /////////
	//SERIE//
   /////////

	// Connect UART0 TXD, RXD signals to port pins
	ConfigSWM(U0_TXD, DBGTXPIN);
	ConfigSWM(U0_RXD, DBGRXPIN);


	// Select frg0clk as the source for fclk0 (to UART0)
	LPC_SYSCON->UART0CLKSEL = FCLKSEL_FRO_CLK;

	// Give USART0 a reset
	LPC_SYSCON->PRESETCTRL0 &= (UART0_RST_N);
	LPC_SYSCON->PRESETCTRL0 |= ~(UART0_RST_N);

	// Configure the USART0 baud rate generator
	LPC_USART0->BRG = 7;

	// Configure the USART0 CFG register:
	// 8 data bits, no parity, one stop bit, no flow control, asynchronous mode
	LPC_USART0->CFG = DATA_LENG_8|PARITY_NONE|STOP_BIT_1;


	// Clear any pending flags, just in case
	LPC_USART0->STAT = 0xFFFF;

	// Enable USART0
	LPC_USART0->CFG |= UART_EN;

	 ///////////
	//SYSTICK//
   ///////////

	//enable + clk/2 comme horloge
	SysTick->CTRL = (1<<SysTick_CTRL_ENABLE_Pos);
	// Reload value
	//longueur des note en ms
	int longueur = 500;
	SysTick->LOAD = 7500*longueur - 1;


	 /////////
	// MRT //
   /////////

	// Give the module a reset
    LPC_SYSCON->PRESETCTRL0 &= (MRT_RST_N);
    LPC_SYSCON->PRESETCTRL0 |= ~(MRT_RST_N);
	// Mode = repeat, interrupt = enable
	LPC_MRT->Channel[0].CTRL = (MRT_Repeat<<MRT_MODE) | (1<<MRT_INTEN);
	// Enable the MRT interrupt in the NVIC
	NVIC->ISER[0] |= 1<<10;
	NVIC_EnableIRQ(MRT_IRQn);

	//tempo metronome
	int tempo=80;
	LPC_MRT->Channel[0].INTVAL = (int)(60/tempo * 15000000);
	LPC_MRT->Channel[0].INTVAL |= 1<<31;



	//Initialisation de l'afficheur lcd et affichage d'un texte
	init_lcd();


	int button_bp1 = 0, new_bp1 = 0, fm_bp1 = 0;
	int button_bp2 = 0, new_bp2 = 0, fm_bp2 = 0;

	int ecran = 0;
	const int nombre_pages = 5;
	int etat_metronome=0;

	char last_char=0;
	int note = 0;
	float note_jouee = 440;

	int refresh = 0;

	int volume = 2;
	const int volume_min = 1, volume_max = 5;
	int volumes[5] = {1,2,3,5,20};

	const int tempo_min = 40, tempo_max = 220;


	const int longueur_min = 100, longueur_max = 1000;

	int octave = 4;
	const int octave_min = -1, octave_max = 9;

	char display[100],note_en_cours[100]="";

	while (1) {

		LED3=LED2;
		LED4=LED2;

		refresh = 0;
		fm_bp1 = 0;
		fm_bp2 = 0;

		new_bp1=BP1;
		new_bp2=BP2;


		//lecture bouton
		if ((new_bp1==0)&&(button_bp1==1)){
			fm_bp1=1;
			refresh = 1;


		}

		button_bp1 = new_bp1;

		if ((new_bp2==0)&&(button_bp2==1)){
			fm_bp2 = 1;
			refresh = 1;
		}

		button_bp2 = new_bp2;

		//longueur notes
		if (SysTick->CTRL & 1<<16){
			LPC_CTIMER0->TCR=0;
			SysTick->CTRL = (0<<SysTick_CTRL_ENABLE_Pos);
		}
		//LED4=etat_metronome;

		//lecture clavier
		if (LPC_USART0->STAT & RXRDY){

			last_char=LPC_USART0->RXDAT;

			switch(last_char){
				case 38 :
					sprintf(note_en_cours,"%s","Do");
					note = 0;
					break;
				case 233 :
					sprintf(note_en_cours,"%s","Do#");
					note = 1;
					break;

				case 34 :
					sprintf(note_en_cours,"%s","Re");
					note = 2;
					break;

				case 39 :
					sprintf(note_en_cours,"%s","Re#");
					note = 3;
					break;

				case 40 :
					sprintf(note_en_cours,"%s","Mi");
					note = 4;
					break;

				case 45 :
					sprintf(note_en_cours,"%s","Fa");
					note = 5;
					break;

				case 232 :
					sprintf(note_en_cours,"%s","Fa#");
					note = 6;
					break;

				case 95 :
					sprintf(note_en_cours,"%s","Sol");
					note = 7;
					break;

				case 231 :
					sprintf(note_en_cours,"%s","Sol#");
					note = 8;
					break;

				case 224 :
					sprintf(note_en_cours,"%s","La");
					note = 9;
					break;

				case 41 :
					sprintf(note_en_cours,"%s","La#");
					note = 10;
					break;

				case 61 :
					sprintf(note_en_cours,"%s","Si");
					note = 11;
					break;

			}

			refresh=1;
			note_jouee = 440 * pow(2,(float)(note-9)/12) * pow(2,octave-4);
			LPC_CTIMER0->MSR[3]=(int)100000/note_jouee;
			LPC_CTIMER0->TCR=(1<<CEN);

			//activation systick + reset
			SysTick->CTRL = (1<<SysTick_CTRL_ENABLE_Pos);
			SysTick->VAL = 0;

		}

		//navigation
		if (fm_bp1){
			if (ecran<nombre_pages){
				ecran++;
			}

			else {
				ecran = 1;
			}
		}


		switch(ecran){
			case 1 :
				if (fm_bp2){
					if (longueur < longueur_max){
						longueur += 100;
					}

					else {
						longueur = longueur_min;
					}
					//longueur en ms
					SysTick->LOAD = 7500*longueur - 1;
					//SysTick->VAL = 0;
				}
				break;
			case 2 :
				if (fm_bp2){
					if (tempo < tempo_max){
						tempo += 5;
					}

					else {
						tempo = tempo_min;
					}
//MRT
					LPC_MRT->Channel[0].INTVAL |= (60/tempo * 15000000);
					LPC_MRT->Channel[0].INTVAL |= 1<<31;
				}


				break;
			case 3 :
				if (fm_bp2){
					if (octave < octave_max){
						octave += 1;
					}

					else {
						octave = octave_min;
					}

					note_jouee = 440 * pow(2,(float)(note-9)/12) * pow(2,octave-4);
					LPC_CTIMER0->MSR[3]=(int)100000/note_jouee;
				}

				break;
			case 4 :
				if (fm_bp2){
					if (volume < volume_max){
						volume += 1;
					}

					else {
						volume = volume_min;
					}
					//reglage pwm pour jouer sur le volume
					LPC_CTIMER0->MSR[1]=volumes[volume];
				}

				break;
			case 5 :

				break;

			}


		//gestion affichage
		if (refresh){

			lcd_clear();
			switch(ecran){
			case 1 :
				sprintf(display,"TEMPS NOTE %d", longueur);
				lcd_puts(display);
				break;
			case 2 :
				sprintf(display,"TEMPO %d", tempo);
				lcd_puts(display);

				break;
			case 3 :
				sprintf(display,"OCTAVE %d", octave);
				lcd_puts(display);

				break;
			case 4 :
				sprintf(display,"VOLUME %d", volume);
				lcd_puts(display);
				break;
			case 5 :
				lcd_puts("SELECTION MUSIQUE");
				break;

			}

			lcd_position(1,0);
			sprintf(display,"%4s %6dHz",note_en_cours,(int)note_jouee);
			lcd_puts(display);

		}

	} // end of while(1)

} // end of main
