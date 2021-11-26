/*
===============================================================================
 Name        : TP_i2c
 Description : source de base pour MCP23008 (I2C GPIO)
===============================================================================
*/

#include <cr_section_macros.h>
#include <stdio.h>
#include "LPC8xx.h"
#include "fro.h"
#include "rom_api.h"
#include "syscon.h"
#include "swm.h"
#include "i2c.h"
#include "ctimer.h"
#include "core_cm0plus.h"

#include "lib_ENS_II1_lcd.h"

#define MCP23_I2C_AD	0x40
#define MCP23_IODIR_REG		0
#define MCP23_IPOL_REG		1
#define MCP23_GPINTEN_REG	2
#define MCP23_DEFVAL_REG	3
#define MCP23_INTCON_REG	4
#define MCP23_IOCON_REG		5
#define MCP23_GPPU_REG		6
#define MCP23_INTF_REG		7
#define MCP23_INTCAP_REG	8
#define MCP23_GPIO_REG		9
#define MCP23_OLAT_REG		10

////boutons et leds de la carte
//#define BP1 LPC_GPIO_PORT->B0[13]
//#define BP2 LPC_GPIO_PORT->B0[12]
//#define LED1 LPC_GPIO_PORT->B0[19]
//#define LED2 LPC_GPIO_PORT->B0[17]
//#define LED3 LPC_GPIO_PORT->B0[21]
//#define LED4 LPC_GPIO_PORT->B0[11]

#define SYSTICK_TIME 15000 // 1 ms pour clk 15 MHz

void MCP23_write_reg(uint8_t reg_num,uint8_t valeur){

	uint8_t I2CMasterBuffer[3]; // ad, #reg, valeur
	uint8_t I2CWriteLength=2;
	I2CMasterBuffer[0]=MCP23_I2C_AD;
	I2CMasterBuffer[1]=reg_num;
	I2CMasterBuffer[2]=valeur;
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength );
}

uint8_t MCP23_read_reg(uint8_t reg_num){
	uint8_t I2CMasterBuffer[2]; // ad, #reg
	uint8_t I2CSlaveBuffer[1];
	uint8_t I2CWriteLength=1;
	uint8_t I2CReadLength=1;
	I2CMasterBuffer[0]=MCP23_I2C_AD;
	I2CMasterBuffer[1]=reg_num;
	//I2CmasterWriteRead( I2CMasterBuffer, I2CSlaveBuffer, I2CWriteLength, I2CReadLength );
	// autre possibilité :
	I2CmasterWrite(I2CMasterBuffer, I2CWriteLength );
	I2CmasterRead( MCP23_I2C_AD, I2CSlaveBuffer, I2CReadLength );
	return I2CSlaveBuffer[0];
}

void MCP23_init_OD() {
	MCP23_write_reg(MCP23_OLAT_REG, 0x00); // Toutes les sorties à la masse
	MCP23_write_reg(MCP23_IODIR_REG, 0xFF); // Tous les GPIO en entrée
	MCP23_write_reg(MCP23_GPPU_REG, 0xFF); // Pull-up partout
}
void MCP23_write_OD(uint8_t valeur) {
	// Tout en entrée sauf un bit des 4 premiers à la masse
	MCP23_write_reg(MCP23_IODIR_REG, (~valeur) | 0x0F);
 }


volatile uint32_t millis;

void init_cpt_millis() {
	millis=0;
	// mise en place du compteur de millisecondes cadencé par SysTick
	SysTick->LOAD = SYSTICK_TIME;  // période = valeur de rechargement
	// clock = system_clock, tick interrupt enabled, count enabled
	SysTick->CTRL = (1<<SysTick_CTRL_CLKSOURCE_Pos) |
			         (1<<SysTick_CTRL_TICKINT_Pos) |
					 (1<<SysTick_CTRL_ENABLE_Pos);
	// Clear the counter and the countflag bit by writing any value to SysTick_VAL
	SysTick->VAL = 0;
	// Enable the SYSTICK interrupt in the NVIC
	NVIC_EnableIRQ(SysTick_IRQn);
}

uint8_t Ftouche() {
	uint8_t lecture = MCP23_read_reg(MCP23_GPIO_REG); // Lecture GPIO

	uint8_t ligne = 0;
	while (((lecture >> (4 + ligne)) & 0b1) && ligne < 4) {
		ligne++;
	}
	// Itération sur 4 machines à état correspondant à la ligne
	// etat contient les 16 variables d'états
	static uint16_t etats = 0x0000;
	for (int i = 0; i < 4; i++) {
		uint8_t touche = ~(lecture >> i) & 0x1;
		uint8_t etat = etats >> (4 * ligne + i) & 0x1;

		if (!etat && touche) { // Front montant
			etats |= (1 << (4 * ligne + i));

			// Renvoie le code de la touche seule
			return ~(1 << (ligne + 4)) & ~(1 << i);
		} else if (etat && !touche) { // Front descendant
			etats &= ~(1 << (4 * ligne + i));
		}
	}
	return 0xFF;
}

uint8_t decode_touche(uint8_t code) {
	const uint8_t Tcodes[]={ 0x77, 0x7b, 0x7d, 0x7e, 0xb7, 0xbb, 0xbd, 0xbe,
			0xd7, 0xdb, 0xdd, 0xde, 0xe7, 0xeb, 0xed, 0xee };
	const uint8_t Tascii[]={'1','2','3','A',
							'4','5','6','B',
							'7','8','9','C',
							'*','0','#','D'};
	uint8_t i=0;
	while ((i<16)&&(Tcodes[i]!=code)) {
		i++;
	}
	return (i<16)?Tascii[i]:'?';
}

void SysTick_Handler(void) {
  // Clear the interrupt flag by reading the SysTick_CTRL register
  uint32_t temp = SysTick->CTRL;
  millis++;
  return;
}

int main(void) {

	char text[17]; // une ligne + \0
	int position_ecran=0;



	uint8_t touche,c_touche;
	//Configuration de l'horloge à 15 MHz
	LPC_PWRD_API->set_fro_frequency(30000);

	// Peripheral reset to the GPIO0 and pin interrupt modules. '0' asserts, '1' deasserts reset.
	LPC_SYSCON->PRESETCTRL0 &=  (GPIO0_RST_N & GPIOINT_RST_N);
	LPC_SYSCON->PRESETCTRL0 |= ~(GPIO0_RST_N & GPIOINT_RST_N);

	//Mise en fonctionnement des périphériques utilisés
	LPC_SYSCON->SYSAHBCLKCTRL0 |= (IOCON | GPIO0 | SWM | CTIMER0 | GPIO_INT);


	//initialisation de l'afficheur lcd avec un affichage
	init_lcd();
    lcd_gohome();
    lcd_puts("UE441 - TP8");
    init_cpt_millis();
    MCP23_init_OD(); // après init_lcd !!

    while(1) {
    	/*if (millis>ech_secondes){
    		sprintf(text,"%02d",(secondes++)%100);
			lcd_position(1,1);
			lcd_puts(text);
			ech_secondes+=1000;
    	}
		if (millis>ech_led){
			led*=2;
			if (led==0){
				led=1;
			}
			MCP23_write_OD(~led);
			ech_led+=250;

		}

    	}*/
    	// Détection d'une touche
    	touche = Ftouche();
		if (touche != 0xFF) {
			// On affiche le caractère lu
			c_touche = decode_touche(touche);
			lcd_position(1, position_ecran);
			lcd_putc(c_touche);

			// Incrémente le curseur
			position_ecran++;
			if (position_ecran > 15) {
				position_ecran = 0;
			}
		}

    }
}


//fonction interruption 0 GPIO sur front descendant
//void PININT0_IRQHandler(void){
//	//votre code
//	LPC_PIN_INT->FALL = 1<<0;       // Clear the interrupt flag
//	return;
//}
