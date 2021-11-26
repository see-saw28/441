#include <cr_section_macros.h>
#include <stdio.h>
#include "LPC8xx.h"
#include "fro.h"
#include "rom_api.h"
#include "syscon.h"
#include "swm.h"
#include "lib_ENS_II1_lcd.h"
#include "mrt.h"
#include "chip_setup.h"
#include "uart.h"
#include "ctimer.h"

#define BP1 		LPC_GPIO_PORT->B0[13]
#define BP2 		LPC_GPIO_PORT->B0[12]
#define LED2 		LPC_GPIO_PORT->B0[17]
#define LED3 		LPC_GPIO_PORT->B0[21]
#define LED4 		LPC_GPIO_PORT->B0[11]

int main(void) {
	int soleil = 0;

	int bp1 = 0, bp2 = 0, old_bp1 = 0, old_bp2 = 0;
	int i = 0, old_i = 0;
	char text[32] = { };
	char car_recu = 0;
	//initialisation de l'horloge à 9 MHz
	LPC_PWRD_API->set_fro_frequency(24000);

	//Initialisation de l'afficheur LCD
	init_lcd();
	//lcd_position(0, 0);
	//lcd_puts("Morse 1TX 2RX");

	LPC_SYSCON->SYSAHBCLKCTRL0 |= IOCON | GPIO0 | SWM | CTIMER0;

	LPC_GPIO_PORT->DIR0 |= (1 << 11) | (1 << 17) | (1 << 19) | (1 << 21);

	//configuration de la liaison série
	LPC_SYSCON->SYSAHBCLKCTRL0 |= UART0;
	LPC_SYSCON->UART0CLKSEL = 0;
	LPC_SYSCON->PRESETCTRL0 &= (UART0_RST_N);
	LPC_SYSCON->PRESETCTRL0 |= ~(UART0_RST_N);
	LPC_SWM->PINASSIGN0 = 0xFFFF0004;
	//LPC_USART0->OSR = 5;
	LPC_USART0->BRG = 624;
	LPC_USART0->CFG = DATA_LENG_8 | STOP_BIT_1 | PARITY_EVEN;
	LPC_USART0->CFG |= UART_EN;

	//precision à la microseconde
	LPC_CTIMER0->PR = 11;

	LPC_CTIMER0->CCR = 1 << CAP0FE;
	LPC_CTIMER0->CCR |= 1 << CAP1RE;
	//LPC_CTIMER0->CTCR=0<<CINSEL;

	//pio0_15 to cap0
	LPC_SWM->PINASSIGN3 = 0xFF0F0FFF;

	LPC_CTIMER0->TCR = (1 << CEN);

	/*j = 0;
	 while (text[j] != 0) {
	 LPC_USART0->TXDAT = text[j];
	 while ((LPC_USART0->STAT & TXRDY) == 0)
	 ;
	 //while((LPC_USART0->STAT & TXRDY)!=0);
	 j++;
	 }
	 */

	PutTerminalString(LPC_USART0, "Partiel_2021\n\r");
	while (1) {
		/*if ((LPC_USART0->STAT & RXRDY) != 0) {
		 car_recu = LPC_USART0->RXDAT;
		 while (1) {
		 if ((LPC_USART0->STAT & RXRDY) != 0) {
		 car_recu = LPC_USART0->RXDAT;
		 if ((car_recu >= 0x41) && (car_recu <= (0x41 + 25))) {
		 lcd_position(1, 0);
		 lcd_putc(car_recu);
		 emission(car_recu);
		 }
		 }
		 }
		 }*/
		bp1 = BP1;
		bp2=BP2;

		if (bp1==0 && bp2) {
			soleil=1;
		}
		else if (bp2==0 && bp1) {
			soleil=3;
		}

		else if (bp2 && bp1) {
			soleil=0;
		}

		else if (bp2==0 && bp1==0) {
			soleil=2;
		}

		if (bp1!=old_bp1 || bp2!=old_bp2) {
			lcd_position(0,0);
			sprintf(text, "E%d\n",soleil);
			lcd_puts(text);
			PutTerminalString(LPC_USART0, text);
		}

		old_bp1=bp1;
		old_bp2=bp2;

		i=LPC_CTIMER0->CR[1]-LPC_CTIMER0->CR[0];
		if (i!=old_i) {
			lcd_position(1,0);
			sprintf(text, "Vitesse :%d",i);
			lcd_puts(text);
		}
		old_i=i;

	}
}

