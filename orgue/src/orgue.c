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

#define BP1  LPC_GPIO_PORT->B0[13]
#define BP2  LPC_GPIO_PORT->B0[12]
#define LED1 LPC_GPIO_PORT->B0[19]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]
#define SYSTICK_TIME (1 * 15000000) - 1 //1 secondes




void lcd_clear(){
	lcd_position(0,0);
	lcd_puts("                ");
	lcd_position(1,0);
	lcd_puts("                ");
	lcd_position(0,0);

}

int main(void) {
	LPC_PWRD_API->set_fro_frequency(30000);


	// Activation du périphérique d'entrées/sorties TOR, du timer et switch matrix
	LPC_SYSCON->SYSAHBCLKCTRL0 |= UART0 | SWM |GPIO | CTIMER0;

	LPC_GPIO_PORT->DIR0 |= (1 << 17)|(1<<21) | (1<<19)|(1<<11);

	 ///////////
	// TIMER //
   ///////////

  //precision 0.1 microseconde
	LPC_CTIMER0->PR=149;

	//440Hz ie comp0 = 100 000 / 440
	LPC_CTIMER0->MSR[3]=227;

	//mise a zero par rapport à MR3
	LPC_CTIMER0->MCR |= (1<<MR3R)|(1<<25)|(1<<27);

	//pwm 50% pour générer un signal rectangulaire ie reglage de comp1
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

	//enable + clk/2 comme horloge
	SysTick->CTRL = (1<<SysTick_CTRL_ENABLE_Pos);
	// Reload value
	int tempo=80;
	SysTick->LOAD = (60/tempo * 15000000) - 1;


	//Initialisation de l'afficheur lcd et affichage d'un texte
	init_lcd();


	int button_bp1 = 0, fm_bp1 = 0;
	int button_bp2 = 0, fm_bp2 = 0;

	int ecran = 0;
	const int nombre_pages = 5;
	int etat_metronome=0;

	char last_char=0;
	int note = 0;
	float note_jouee = 440;

	int refresh = 0;

	const int tempo_min = 40, tempo_max = 220;


	int octave = 4;
	const int octave_min = -1, octave_max = 9;

	char display[100],note_en_cours[100]="";

	while (1) {

		refresh = 0;
		fm_bp1 = 0;
		fm_bp2 = 0;


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

		//metronome
		if (SysTick->CTRL & 1<<16){
			etat_metronome=!etat_metronome;
			LPC_CTIMER0->TCR=0;
		}
		LED4=etat_metronome;

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

				break;
			case 2 :
				if (fm_bp2){
					if (tempo < tempo_max){
						tempo += 5;
					}

					else {
						tempo = tempo_min;
					}

					SysTick->LOAD = (int)(7500000*60/tempo - 1);
					//SysTick->VAL = 0;
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

				break;
			case 5 :

				break;

			}


		//gestion affichage
		if (refresh){

			lcd_clear();
			switch(ecran){
			case 1 :
				lcd_puts("MANUEL/AUTO");
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
				lcd_puts("VOLUME");
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
