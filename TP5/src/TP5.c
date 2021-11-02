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
  LPC_SYSCON->SYSAHBCLKCTRL0 |= UART0 | SWM |GPIO;

  //config des LED
  LPC_GPIO_PORT->DIR0 |= (1 << 17)|(1<<21) | (1<<19);

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
  // 8 data bits, no parity, one stop bit
  LPC_USART0->CFG = DATA_LENG_8|PARITY_NONE|STOP_BIT_1;


  // Clear any pending flags, just in case
  LPC_USART0->STAT = 0xFFFF;

  // Enable USART0
  LPC_USART0->CFG |= UART_EN;

  init_lcd();
  lcd_puts("TP5 serie");

  int old_bp1=0,new_bp1=0;
  int old_bp2=0,new_bp2=0;

  char last_char=0;

  while(1) {
	new_bp1=BP1;
	new_bp2=BP2;
	if (new_bp1 && old_bp1==0){
		if (LPC_USART0->STAT & TXRDY){
			LPC_USART0->TXDAT='b';
		}
	}

	else if (new_bp1==0 && old_bp1){
		if (LPC_USART0->STAT & TXRDY){
			LPC_USART0->TXDAT='B';
		}
	}

	//envoi d'une chaine de caractere avec le bp2
	if (new_bp2==0 && old_bp2){
		PutTerminalString(LPC_USART0, "Bouton 2 enfoncé\n\r");
	}
    //lecture des données
	if (LPC_USART0->STAT & RXRDY){

		lcd_position(1,0);
		last_char=LPC_USART0->RXDAT;//lecture du caractere reçu
		char display[100];
		sprintf(display,"%16d", last_char);
		lcd_puts(display);//affichage du caractere reçu

		//controle de la LED
		if (last_char=='a'){
			LED2=1;
		}
		else if (last_char=='e'){
			LED2=0;
		}
	}

    old_bp1=new_bp1;
    old_bp2=new_bp2;

  };

} // end of main

