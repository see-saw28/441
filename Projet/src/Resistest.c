/*
===============================================================================
 Name        : UART0_Terminal.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

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
#include "acomp.h"
#include "mrt.h"

#define BP1  LPC_GPIO_PORT->B0[13]
#define BP2  LPC_GPIO_PORT->B0[12]
#define LED1 LPC_GPIO_PORT->B0[19]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]

int outputs[4] = {11, 21, 17, 19};

void reset_output(void){
	LPC_GPIO_PORT->DIR0 &= (~(1<<11)) & (~(1<<21)) & (~(1<<17)) & (~(1<<19)) ;
	LPC_GPIO_PORT->PIN0 = 0;
}

void set_output(int output){
	LPC_GPIO_PORT->DIR0|= (1<<outputs[output]);
	LPC_GPIO_PORT->PIN0|= (1<<outputs[output]);
}

void CMP_IRQHandler(void) {

  uint32_t temp = LPC_CMP->CTRL;
  static a=0;
  // If comparator out is a '1' light the green LED, otherwise light the red LED
  if (temp & (1<<COMPSTAT)) {
	  a+=1;
  }
  else {

  }

  // Clear the edge-detects interrupt flag
  LPC_CMP->CTRL |= 1<<EDGECLR;
  LPC_CMP->CTRL &= ~(1<<EDGECLR);

  return;
}

void MRT_IRQHandler(void){

	if (LPC_MRT->Channel[1].STAT & 1<<0){
			LPC_ADC->SEQB_CTRL|=1<<31|1<<26;
			LPC_MRT->Channel[1].STAT |=1;
		}

}

int main(void) {

	//config de la frequence de l'horloge fro
	LPC_PWRD_API->set_fro_frequency(30000);
	// Enable clocks to relevant peripherals
	LPC_SYSCON->SYSAHBCLKCTRL0 |= UART0 | SWM |GPIO |ADC |ACMP|IOCON|MRT;

	//config des sorties
	LPC_GPIO_PORT->DIR0 |= (1<<21)|(1<<11)|(1<<17)|(1<<19) ;

	//config de l'adc et comparateur analogique
	LPC_SYSCON->PDRUNCFG &=~(ADC_PD);
	LPC_SYSCON->PDRUNCFG &=~(ACMP_PD);
	LPC_SYSCON->ADCCLKDIV = 1;
	LPC_SYSCON->ADCCLKSEL = 0;
	LPC_SWM->PINENABLE0 &=~(ADC_8);
	LPC_SWM->PINENABLE0 &=~(ADC_7);
	LPC_SWM->PINENABLE0 &=~(ACMP_I4);
	LPC_IOCON->PIO0_15 = 0;
	LPC_IOCON->PIO0_16 = 0;

	LPC_ADC->SEQA_CTRL|=1<<8|1<<18;
	LPC_ADC->SEQB_CTRL|=1<<7|1<<18;

	LPC_CMP->CTRL = (NONE<<HYS) | (1<<INTENA) | (V_LADDER_OUT<<COMP_VM_SEL) | (ACOMP_IN4<<COMP_VP_SEL) | (RISING<<EDGESEL);

	LPC_CMP->LAD = (SUPPLY_VDD<<LADREF) | ((20)<<LADSEL) | (1<<LADEN);

	// Clear any edge-detects
	LPC_CMP->CTRL |= 1<<EDGECLR;
	LPC_CMP->CTRL &= ~(1<<EDGECLR);

	// Enable the Acomp interrupt
	NVIC_EnableIRQ(CMP_IRQn);

  //chrono lecture potentiometre toutes les 10ms
	LPC_MRT->Channel[1].INTVAL = 150000;

	// Mode = repeat, interrupt = enable
	LPC_MRT->Channel[1].CTRL = (MRT_Repeat<<MRT_MODE) | (1<<MRT_INTEN);

	NVIC_EnableIRQ(MRT_IRQn);
	NVIC->IP[2] &= ~(0x00C00000);


	init_lcd();
	lcd_puts("Resitest");

	int old_bp1=1,new_bp1=0;
	int old_bp2=1,new_bp2=0;

	int resistor,capture,adc;

	char display[17];

	int resitance[4] = {100, 1000, 10000, 100000};

	int mode = 0,old_mode=0;


	int choix_resistance=0;



	while(1) {
		new_bp1=BP1;
		new_bp2=BP2;

		//lecture potentiometre selection resitance/capacité
		if (LPC_ADC->SEQB_GDAT & (1<<31)){
			adc = (LPC_ADC->SEQB_GDAT & 0xFFF0)>>4;
			if (adc<2048){
				mode=1;
				if(old_mode!=mode){
					lcd_position(0,0);
					sprintf(display,"Resitest %7d", resitance[choix_resistance]);
					lcd_puts(display);
					old_mode=mode;
				}

			}
			else {
				mode=0;
				if(old_mode!=mode){
					lcd_position(0,0);
					lcd_puts("Capatest        ");
					old_mode=mode;
				}
			}
			LPC_ADC->SEQB_CTRL &= ~(1<<31);
		}

		//mode resistance
		if (mode){
			if(new_bp1!=old_bp1 & old_bp1){

				LPC_ADC->SEQA_CTRL|=1<<31|1<<26;


			}



			if (LPC_ADC->SEQA_GDAT & (1<<31)){
				capture = (LPC_ADC->SEQA_GDAT & 0xFFF0)>>4;
				resistor = (int)capture*(float)(resitance[choix_resistance]+560)/(4096-capture);
				sprintf(display,"R = %6d", resistor);
				//sprintf(display,"ADC %6d", capture);
				lcd_position(1,0);
				lcd_puts(display);

				LPC_ADC->SEQA_CTRL&=~(1<<31);
			}

			if(new_bp2!=old_bp2 & old_bp2){
				if (choix_resistance < 3){
					choix_resistance++;
				}

				else {
					choix_resistance = 0;
				}
				reset_output();
				set_output(choix_resistance);
				sprintf(display,"Resitest %7d", resitance[choix_resistance]);

				lcd_position(0,0);
				lcd_puts(display);



			}
		}
		//mode capacité
		else {
			if(new_bp2!=old_bp2 & old_bp2){
				if (choix_resistance < 3){
					choix_resistance++;
				}

				else {
					choix_resistance = 0;
				}
				reset_output();
				//set_output(choix_resistance);
				sprintf(display,"Capatest %7d", resitance[choix_resistance]);

				lcd_position(0,0);
				lcd_puts(display);


			}
			if(new_bp1!=old_bp1 & old_bp1){
				set_output(choix_resistance);
			}
		}
		old_bp1=new_bp1;
		old_bp2=new_bp2;

	};

} // end of main

