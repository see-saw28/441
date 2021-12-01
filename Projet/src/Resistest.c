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

#define BP1  LPC_GPIO_PORT->B0[13]
#define BP2  LPC_GPIO_PORT->B0[12]
#define LED1 LPC_GPIO_PORT->B0[19]
#define LED2 LPC_GPIO_PORT->B0[17]
#define LED3 LPC_GPIO_PORT->B0[21]
#define LED4 LPC_GPIO_PORT->B0[11]


int main(void) {

  //config de la frequence de l'horloge fro
  LPC_PWRD_API->set_fro_frequency(30000);
  // Enable clocks to relevant peripherals
  LPC_SYSCON->SYSAHBCLKCTRL0 |= UART0 | SWM |GPIO |ADC;

  //config des LED
  LPC_GPIO_PORT->DIR0 |= (1 << 17)|(1<<21) | (1<<19);



  LPC_SYSCON->PDRUNCFG &=~(ADC_PD);
  LPC_SYSCON->ADCCLKDIV = 1;
  LPC_SYSCON->ADCCLKSEL = 0;
  LPC_SWM->PINENABLE0 &=~(ADC_8);
  LPC_IOCON->PIO0_15 = 0;

  LPC_ADC->SEQA_CTRL|=1<<8|1<<18;

  init_lcd();
  lcd_puts("Resitest");

  int old_bp1=1,new_bp1=0;
  int old_bp2=1,new_bp2=0;

  char display[17];

  while(1) {
	new_bp1=BP1;
	new_bp2=BP2;

	if(new_bp1!=old_bp1){
		LPC_ADC->SEQA_CTRL|=1<<31|1<<26;
		LED1=1;
	}

	if (LPC_ADC->SEQA_GDAT & (1<<31)){
		sprintf(display,"ADC :%4d", (LPC_ADC->SEQA_GDAT & 0xFFF0)>>4);
		lcd_position(1,0);
		lcd_puts(display);

		LPC_ADC->SEQA_CTRL&=~(1<<31);
	}
    old_bp1=new_bp1;
    old_bp2=new_bp2;

  };

} // end of main

