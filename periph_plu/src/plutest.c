/*
 * @brief PLU Peripheral test
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */


#include "LPC8xx.h"
#include "core_cm0plus.h"
#include "syscon.h"
#include "utilities.h"
#include "plu.h"
#include <stdio.h>
#include "iocon.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for PLU Peripheral test example
 * @return	Function should not exit.
 */
int main(void)
{
	SystemCoreClockUpdate();

  // Enable clocks to relevant peripherals
  LPC_SYSCON->SYSAHBCLKCTRL0 |= (IOCON|SWM);
	Enable_Periph_Clock(CLK_PLU);
	/* Reset PLU */
	Do_Periph_Reset(RESET_PLU);

	/* Connect the PLU_CLK signal to port pin P0.1 */
	LPC_IOCON->PIO0_1 &=(IOCON_MODE_MASK | MODE_INACTIVE);			/* Disable PU/PD and make this pin inactive */
	ConfigSWM(PLU_CLKIN, P0_1);
	
	/* Assign PLU input/output to pins, only configured those inputs/outputs which will be used. */
	LPC_SWM->PINASSIGN_4PIN &= ~(PLU_INPUT0_NONE);
	LPC_SWM->PINASSIGN_4PIN |= (PLU_INPUT0_PIO0_0);
	LPC_SWM->PINASSIGN_4PIN &= ~(PLU_INPUT1_NONE);
	LPC_SWM->PINASSIGN_4PIN |= (PLU_INPUT1_PIO0_1);
	LPC_SWM->PINASSIGN_4PIN &= ~(PLU_INPUT2_NONE);
	LPC_SWM->PINASSIGN_4PIN |= (PLU_INPUT2_PIO0_10);
	LPC_SWM->PINASSIGN_4PIN &= ~(PLU_INPUT3_NONE);
	LPC_SWM->PINASSIGN_4PIN |= (PLU_INPUT3_PIO0_11);
	LPC_SWM->PINASSIGN_4PIN &= ~(PLU_INPUT4_NONE);
	LPC_SWM->PINASSIGN_4PIN |= (PLU_INPUT4_PIO0_4);
	LPC_SWM->PINASSIGN_4PIN &= ~(PLU_INPUT5_NONE);
	LPC_SWM->PINASSIGN_4PIN |= (PLU_INPUT5_PIO0_13);

	LPC_SWM->PINASSIGN_4PIN &= ~PLU_OUTPUT0_NONE; 	
	LPC_SWM->PINASSIGN_4PIN |= PLU_OUTPUT0_PIO0_7;
	LPC_IOCON->PIO0_7 &= ~(0x3<<3);
	LPC_SWM->PINASSIGN_4PIN &= ~PLU_OUTPUT1_NONE; 	
	LPC_SWM->PINASSIGN_4PIN |= PLU_OUTPUT1_PIO0_15;
	LPC_IOCON->PIO0_15 &= ~(0x3<<3);
	LPC_SWM->PINASSIGN_4PIN &= ~PLU_OUTPUT2_NONE; 	
	LPC_SWM->PINASSIGN_4PIN |= PLU_OUTPUT2_PIO0_16;
	LPC_IOCON->PIO0_16 &= ~(0x3<<3);
	LPC_SWM->PINASSIGN_4PIN &= ~PLU_OUTPUT3_NONE; 	
	LPC_SWM->PINASSIGN_4PIN |= PLU_OUTPUT3_PIO0_10;
	LPC_IOCON->PIO0_10 &= ~(0x3<<3);
	LPC_SWM->PINASSIGN_4PIN &= ~PLU_OUTPUT4_NONE; 	
	LPC_SWM->PINASSIGN_4PIN |= PLU_OUTPUT4_PIO0_18;
	LPC_IOCON->PIO0_18 &= ~(0x3<<3);
	LPC_SWM->PINASSIGN_4PIN &= ~PLU_OUTPUT5_NONE; 	
	LPC_SWM->PINASSIGN_4PIN |= PLU_OUTPUT5_PIO0_12;
	LPC_IOCON->PIO0_12 &= ~(0x3<<3);
	LPC_SWM->PINASSIGN_4PIN &= ~PLU_OUTPUT6_NONE; 	
	LPC_SWM->PINASSIGN_4PIN |= PLU_OUTPUT6_PIO0_20;
	LPC_IOCON->PIO0_20 &= ~(0x3<<3);
	LPC_SWM->PINASSIGN_4PIN &= ~PLU_OUTPUT7_NONE; 	
	LPC_SWM->PINASSIGN_4PIN |= PLU_OUTPUT7_PIO0_14;
	LPC_IOCON->PIO0_14 &= ~(0x3<<3);

/* 
* Copyright 2017 NXP
* 6 Inputs 26 LUTs 4 flip flops and 8 outputs */

/* LUT0 (LUT0) */
LPC_PLU0->LUT_MUX[0].INP[0] = 0x00000000; /* IN0 (IN1) */
LPC_PLU0->LUT_MUX[0].INP[1] = 0x00000001; /* IN1 (IN2) */
LPC_PLU0->LUT_MUX[0].INP[2] = 0x00000006; /* default */
LPC_PLU0->LUT_MUX[0].INP[3] = 0x00000006; /* default */
LPC_PLU0->LUT_MUX[0].INP[4] = 0x00000006; /* default */
LPC_PLU0->LUT_TRUTH[0] = 0xeeeeeeee; /* LUT0 (LUT0) STD 2 INPUT OR */

/* LUT1 (LUT1) */
LPC_PLU0->LUT_MUX[1].INP[0] = 0x00000002; /* IN2 (IN0) */
LPC_PLU0->LUT_MUX[1].INP[1] = 0x00000007; /* default */
LPC_PLU0->LUT_MUX[1].INP[2] = 0x00000007; /* default */
LPC_PLU0->LUT_MUX[1].INP[3] = 0x00000007; /* default */
LPC_PLU0->LUT_MUX[1].INP[4] = 0x00000007; /* default */
LPC_PLU0->LUT_TRUTH[1] = 0x55555555; /* LUT1 (LUT1) STD 1 INPUT NOT */

/* LUT2 (LUT2) */
LPC_PLU0->LUT_MUX[2].INP[0] = 0x00000006; /* LUT0 (LUT0) */
LPC_PLU0->LUT_MUX[2].INP[1] = 0x00000008; /* default */
LPC_PLU0->LUT_MUX[2].INP[2] = 0x00000008; /* default */
LPC_PLU0->LUT_MUX[2].INP[3] = 0x00000008; /* default */
LPC_PLU0->LUT_MUX[2].INP[4] = 0x00000008; /* default */
LPC_PLU0->LUT_TRUTH[2] = 0x55555555; /* LUT2 (LUT2) STD 1 INPUT NOT */

/* LUT3 (LUT3) */
LPC_PLU0->LUT_MUX[3].INP[0] = 0x00000002; /* IN2 (IN0) */
LPC_PLU0->LUT_MUX[3].INP[1] = 0x00000009; /* default */
LPC_PLU0->LUT_MUX[3].INP[2] = 0x00000009; /* default */
LPC_PLU0->LUT_MUX[3].INP[3] = 0x00000009; /* default */
LPC_PLU0->LUT_MUX[3].INP[4] = 0x00000009; /* default */
LPC_PLU0->LUT_TRUTH[3] = 0x55555555; /* LUT3 (LUT3) STD 1 INPUT NOT */

/* LUT4 (LUT4) */
LPC_PLU0->LUT_MUX[4].INP[0] = 0x00000007; /* LUT1 (LUT1) */
LPC_PLU0->LUT_MUX[4].INP[1] = 0x00000006; /* LUT0 (LUT0) */
LPC_PLU0->LUT_MUX[4].INP[2] = 0x00000003; /* IN3 (IN3) */
LPC_PLU0->LUT_MUX[4].INP[3] = 0x0000000a; /* default */
LPC_PLU0->LUT_MUX[4].INP[4] = 0x0000000a; /* default */
LPC_PLU0->LUT_TRUTH[4] = 0x80808080; /* LUT4 (LUT4) STD 3 INPUT AND */

/* LUT5 (LUT5) */
LPC_PLU0->LUT_MUX[5].INP[0] = 0x00000002; /* IN2 (IN0) */
LPC_PLU0->LUT_MUX[5].INP[1] = 0x00000008; /* LUT2 (LUT2) */
LPC_PLU0->LUT_MUX[5].INP[2] = 0x00000003; /* IN3 (IN3) */
LPC_PLU0->LUT_MUX[5].INP[3] = 0x0000000b; /* default */
LPC_PLU0->LUT_MUX[5].INP[4] = 0x0000000b; /* default */
LPC_PLU0->LUT_TRUTH[5] = 0x80808080; /* LUT5 (LUT5) STD 3 INPUT AND */

/* LUT6 (LUT6) */
LPC_PLU0->LUT_MUX[6].INP[0] = 0x00000009; /* LUT3 (LUT3) */
LPC_PLU0->LUT_MUX[6].INP[1] = 0x00000001; /* IN1 (IN2) */
LPC_PLU0->LUT_MUX[6].INP[2] = 0x00000003; /* IN3 (IN3) */
LPC_PLU0->LUT_MUX[6].INP[3] = 0x0000000c; /* default */
LPC_PLU0->LUT_MUX[6].INP[4] = 0x0000000c; /* default */
LPC_PLU0->LUT_TRUTH[6] = 0x80808080; /* LUT6 (LUT6) STD 3 INPUT AND */

/* LUT7 (LUT7) */
LPC_PLU0->LUT_MUX[7].INP[0] = 0x00000002; /* IN2 (IN0) */
LPC_PLU0->LUT_MUX[7].INP[1] = 0x00000001; /* IN1 (IN2) */
LPC_PLU0->LUT_MUX[7].INP[2] = 0x00000003; /* IN3 (IN3) */
LPC_PLU0->LUT_MUX[7].INP[3] = 0x0000000d; /* default */
LPC_PLU0->LUT_MUX[7].INP[4] = 0x0000000d; /* default */
LPC_PLU0->LUT_TRUTH[7] = 0x80808080; /* LUT7 (LUT7) STD 3 INPUT AND */

/* LUT8 (LUT8) */
LPC_PLU0->LUT_MUX[8].INP[0] = 0x0000000a; /* LUT4 (LUT4) */
LPC_PLU0->LUT_MUX[8].INP[1] = 0x0000000b; /* LUT5 (LUT5) */
LPC_PLU0->LUT_MUX[8].INP[2] = 0x0000000e; /* default */
LPC_PLU0->LUT_MUX[8].INP[3] = 0x0000000e; /* default */
LPC_PLU0->LUT_MUX[8].INP[4] = 0x0000000e; /* default */
LPC_PLU0->LUT_TRUTH[8] = 0xeeeeeeee; /* LUT8 (LUT8) STD 2 INPUT OR */

/* LUT9 (LUT9) */
LPC_PLU0->LUT_MUX[9].INP[0] = 0x0000000c; /* LUT6 (LUT6) */
LPC_PLU0->LUT_MUX[9].INP[1] = 0x0000000d; /* LUT7 (LUT7) */
LPC_PLU0->LUT_MUX[9].INP[2] = 0x0000000f; /* default */
LPC_PLU0->LUT_MUX[9].INP[3] = 0x0000000f; /* default */
LPC_PLU0->LUT_MUX[9].INP[4] = 0x0000000f; /* default */
LPC_PLU0->LUT_TRUTH[9] = 0xeeeeeeee; /* LUT9 (LUT9) STD 2 INPUT OR */

/* LUT10 (LUT10) */
LPC_PLU0->LUT_MUX[10].INP[0] = 0x0000000e; /* LUT8 (LUT8) */
LPC_PLU0->LUT_MUX[10].INP[1] = 0x00000012; /* LUT12 (LUT12) */
LPC_PLU0->LUT_MUX[10].INP[2] = 0x00000010; /* default */
LPC_PLU0->LUT_MUX[10].INP[3] = 0x00000010; /* default */
LPC_PLU0->LUT_MUX[10].INP[4] = 0x00000010; /* default */
LPC_PLU0->LUT_TRUTH[10] = 0x88888888; /* LUT10 (LUT10) STD 2 INPUT AND */

/* LUT11 (LUT11) */
LPC_PLU0->LUT_MUX[11].INP[0] = 0x0000000f; /* LUT9 (LUT9) */
LPC_PLU0->LUT_MUX[11].INP[1] = 0x00000012; /* LUT12 (LUT12) */
LPC_PLU0->LUT_MUX[11].INP[2] = 0x00000011; /* default */
LPC_PLU0->LUT_MUX[11].INP[3] = 0x00000011; /* default */
LPC_PLU0->LUT_MUX[11].INP[4] = 0x00000011; /* default */
LPC_PLU0->LUT_TRUTH[11] = 0x88888888; /* LUT11 (LUT11) STD 2 INPUT AND */

/* LUT12 (LUT12) */
LPC_PLU0->LUT_MUX[12].INP[0] = 0x00000004; /* IN4 (IN4) */
LPC_PLU0->LUT_MUX[12].INP[1] = 0x00000005; /* IN5 (IN5) */
LPC_PLU0->LUT_MUX[12].INP[2] = 0x00000012; /* default */
LPC_PLU0->LUT_MUX[12].INP[3] = 0x00000012; /* default */
LPC_PLU0->LUT_MUX[12].INP[4] = 0x00000012; /* default */
LPC_PLU0->LUT_TRUTH[12] = 0x77777777; /* LUT12 (LUT12) STD 2 INPUT NAND */

LPC_PLU0->OUTPUT_MUX[0] = 0x0000000a; /* LUT10 (LUT10) -> OUT0 */
LPC_PLU0->OUTPUT_MUX[1] = 0x0000000b; /* LUT11 (LUT11) -> OUT1 */

	while ( 1 );
}
