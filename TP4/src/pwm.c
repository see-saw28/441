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

	//100Hz ie comp0
	LPC_CTIMER0->MR[3]=100;

	//mise a zero par rapport MR3
	LPC_CTIMER0->MCR |= (1<<MR3R);

	//pwm 25% ie comp1
	LPC_CTIMER0->MR[1]=75;

	//mat1
	LPC_CTIMER0->PWMC = (1<<PWMEN1);

	//mat1 relié a LED1
	LPC_SWM->PINASSIGN4 &= ~(0xFF<<8);
	LPC_SWM->PINASSIGN4 |= 19<<8;

	//Initialisation de l'afficheur lcd et affichage d'un texte
	init_lcd();
	lcd_puts("PWM 25%");

	while (1) {
		LED2=1;
	} // end of while(1)

} // end of main
