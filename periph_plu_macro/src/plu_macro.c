/*
 * @brief Using pre-defined Macro instead of PLU tool to generate application
 * based on schematic design.
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

/* If 1, test customized LUT with FFs: test Neil's PLU auto ththreshold detection example, 
please note: since two FFs are not allowed to connect directly, a buffer has to be added 
in between; otherwise, test Neil's PLU example 1 */
#define TEST_CUSTOMIZED_LUT_AND_FF				0				


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
#if TEST_CUSTOMIZED_LUT_AND_FF
enum plu_inputs   {Inp0 = 0, Inp1, Inp2, Inp3, range, sample};
enum plu_luts 		{ic0 = 0, ic1, ic2, ic3, ic4, ic5, ic6, ic7, ic8};
enum plu_ffs			{ff0 = 0, ff1, ff2, ff3};
enum plu_outputs  {out_of_range = 0, high_th_cross, low_th_cross};
#else
enum plu_inputs   {Input_A=0, Input_B, Input_C, Input_D, Input_E, Input_F};
enum plu_luts 		{ic0 = 0, ic1, ic2, ic3, ic4, ic5, ic6, ic7, ic8, ic9, ic10, ic11, ic12};
enum plu_ffs			{ff0 = 0, ff1, ff2, ff3};
enum plu_outputs  {Out0 = 0, Out1, Out2, Out3, Out4, Out5, Out6, Out7};
#endif

uint32_t temp_tt;

/*****************************************************************************
 * Private functions
 ****************************************************************************/
#if TEST_CUSTOMIZED_LUT_AND_FF
/*
 * logical functions used to calculate LUT's truth table entries must be defined as:
 *
 * uint32_t function_name(uint32_t lut_in4, uint32_t lut_in3, uint32_t lut_in2, uint32_t lut_in1, uint32_t lut_in0);
 *
 * this function returns only 0 or 1; lut_in4/3/2/1/0 are 0s and 1s
 *
 * when called, this function returns a result that matches the input selected by provided lut_in4/3/2/1/0 only!
 * no matter how many inputs given LUT uses, the function describing it always has 5 inputs
 *
 */
uint32_t lf_ic6(uint32_t in_above, uint32_t in_above_th_high, uint32_t in2, uint32_t in3, uint32_t in4)
{
	uint32_t result;
	
	result = (~in_above & in_above_th_high) | ( in_above & ~in_above_th_high);
	
	return result;
}

uint32_t lf_above_th_high(uint32_t Inp0, uint32_t Inp1, uint32_t Inp2, uint32_t Inp3, uint32_t range)
{
	uint32_t result;
	uint32_t dec_inp;
	
	dec_inp = Inp3<<3 | Inp2<<2 | Inp1<<1 | Inp0;
	
	result = 0;
	if (range == 0)
	{
		if (dec_inp >= 0xD)
		{
			result = 1;
		}
	}
	else
	{
		if (dec_inp >= 0xB)
		{
			result = 1;
		}
	}
	
	return result;
}

uint32_t lf_below_th_low(uint32_t Inp0, uint32_t Inp1, uint32_t Inp2, uint32_t Inp3, uint32_t range)
{
	uint32_t result;
	uint32_t dec_inp;
	
	dec_inp = Inp3<<3 | Inp2<<2 | Inp1<<1 | Inp0;

	result = 0;
	if (range == 0)
	{
		if (dec_inp <= 0x2)
		{
			result = 1;
		}
	}
	else
	{
		if (dec_inp <= 0x4)
		{
			result = 1;
		}
	}
	
	return result;
}
#endif

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for blinky example
 * @return	Function should not exit.
 */
int main(void)
{
	SystemCoreClockUpdate();

  // Enable clocks to relevant peripherals
  LPC_SYSCON->SYSAHBCLKCTRL0 |= (IOCON|SWM|GPIO);
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

#if TEST_CUSTOMIZED_LUT_AND_FF
	/* customized LUT and FF. In the comparator logic, ic4 and ic5 are two LUTs,
	inside ic4, if range(input E) is 0, threshold high >= 1101B, if range is 1,
	threshold high >= 1011B.
	inside ic5, if range(input E) is 0, threshold low <= 0010B, if range is 1,
	threshold low <= 0100B. */
  PLU_LUT_2IN1SEL_MUX(ic0,INMUX_FF(ff0),INMUX_LUT(ic4),INMUX_IN(sample));
	PLU_LUT_2IN1SEL_MUX(ic1,INMUX_FF(ff1),INMUX_LUT(ic5),INMUX_IN(sample));
	PLU_LUT_2IN1SEL_MUX(ic2,INMUX_FF(ff2),INMUX_LUT(ic6),INMUX_IN(sample));
	PLU_LUT_2IN1SEL_MUX(ic3,INMUX_FF(ff3),INMUX_LUT(ic7),INMUX_IN(sample));
	
	generate_plu_tt(lf_above_th_high, &temp_tt);
  PLU_LUT_5IN_CUSTOM(ic4, Inp0, Inp1, Inp2, Inp3, range, temp_tt);
	
  generate_plu_tt(lf_below_th_low, &temp_tt);
	PLU_LUT_5IN_CUSTOM(ic5, Inp0, Inp1, Inp2, Inp3, range, temp_tt);
	
	/* "lf_ic6" is a XOR gate as ic7, we try to use different ways to show customer 
	how to create customized LUT truth table before setting INPUTs to the LUT. */
  generate_plu_tt(lf_ic6, &temp_tt);
	PLU_LUT_2IN_CUSTOM(ic6, INMUX_FF(ff0), INMUX_LUT(ic4), temp_tt);
	
	PLU_LUT_2IN_XOR(ic7, INMUX_FF(ff1), INMUX_LUT(ic5));
	PLU_LUT_2IN_OR(ic8, INMUX_FF(ff0), INMUX_FF(ff1));
	
	PLU_OUT(out_of_range,  OUTMUX_LUT(ic8));
	PLU_OUT(high_th_cross, OUTMUX_FF(ff2));
	PLU_OUT(low_th_cross,  OUTMUX_FF(ff3));
#else
	/* PLU example 1 */
	PLU_LUT_2IN_OR(ic0, INMUX_IN(Input_A), INMUX_IN(Input_B));
	PLU_LUT_2IN_NAND(ic12, INMUX_IN(Input_E), INMUX_IN(Input_F));
	
	PLU_LUT_INVERTER(ic1, INMUX_IN(Input_C));
	PLU_LUT_INVERTER(ic2, INMUX_LUT(ic0));
	PLU_LUT_INVERTER(ic3, INMUX_IN(Input_C));
	
	PLU_LUT_3IN_AND(ic4, INMUX_LUT(ic1), INMUX_LUT(ic0), INMUX_IN(Input_D));
	PLU_LUT_3IN_AND(ic5, Input_C, INMUX_LUT(ic2), INMUX_IN(Input_D));
	PLU_LUT_3IN_AND(ic6, INMUX_LUT(ic3), INMUX_IN(Input_B), INMUX_IN(Input_D));
	PLU_LUT_3IN_AND(ic7, INMUX_IN(Input_C), INMUX_IN(Input_B), INMUX_IN(Input_D));
	
	PLU_LUT_2IN_OR(ic8, INMUX_LUT(ic4), INMUX_LUT(ic5));
	PLU_LUT_2IN_OR(ic9, INMUX_LUT(ic6), INMUX_LUT(ic7));
	
	PLU_LUT_2IN_AND(ic10, INMUX_LUT(ic8), INMUX_LUT(ic12));
	PLU_LUT_2IN_AND(ic11, INMUX_LUT(ic9), INMUX_LUT(ic12));
	
	//connect PLU outputs to LUTs and FFs
	PLU_OUT(Out0, OUTMUX_LUT(ic10));
	PLU_OUT(Out1, OUTMUX_LUT(ic11));
#endif

	while ( 1 );
}

