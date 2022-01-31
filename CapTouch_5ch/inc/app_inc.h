/* app_inc.h */
#ifndef __APP_INC_H__
#define __APP_INC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "LPC8xx.h"
#include "chip_setup.h"
#include "utilities.h"

#include "syscon.h"
#include "iocon.h"
#include "swm.h"
#include "gpio.h"
#include "mrt.h"
#include "uart.h"
#include "capt.h"

/*
* GPIO - LED.
*/
#define LEDx_COUNT 5U

#define LED0_IDX  20U /* PIO0_20. */
#define LED1_IDX  18U /* PIO0_18. */
#define LED2_IDX  15U /* PIO0_15. */
#define LED3_IDX  8U  /* PIO0_8.  */
#define LED4_IDX  9U  /* PIO0_9.  */

#define LED0_MASK  (1U << LED0_IDX) /* PIO0_20. */
#define LED1_MASK  (1U << LED1_IDX) /* PIO0_18. */
#define LED2_MASK  (1U << LED2_IDX) /* PIO0_15. */
#define LED3_MASK  (1U << LED3_IDX) /* PIO0_8.  */
#define LED4_MASK  (1U << LED4_IDX) /* PIO0_19. */

#define LEDx_ALL_MASK (LED0_MASK | LED1_MASK | LED2_MASK | LED3_MASK | LED4_MASK)

void LEDx_Init(void);
void LEDx_On(uint32_t ledMask);
void LEDx_Off(uint32_t ledMask);
void LEDx_OnOnly(uint32_t ledMask);
void App_ProbeLEDx(void);

extern volatile uint32_t gAppCapTouchKeyValue;
extern volatile bool     bAppCapTouchCalibrationDone;

void BOARD_InitBootClock(void);
void BOARD_InitBootPins(void);

#endif /* __APP_INC_H__ */

