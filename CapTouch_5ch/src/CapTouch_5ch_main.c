/*
===============================================================================
 Name        : CapTouch_ch5.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "app_inc.h"


/*
* GPIO - LED.
*/
const uint32_t cLEDxIdxArr[LEDx_COUNT] =
{
    LED0_IDX,
    LED1_IDX,
    LED2_IDX,
    LED3_IDX,
    LED4_IDX
};

const uint32_t cLEDxMaskArr[LEDx_COUNT] =
{
    LED0_MASK,
    LED1_MASK,
    LED2_MASK,
    LED3_MASK,
    LED4_MASK
};

/*
* MRTTicker.
*/
void MRTTicker_Init(void);
void MRTTicker_BlockingDelay10ms(uint32_t ticks);

/*
* CapTouch.
*/
void App_CapTouch_Init(void);
void App_CapTouch_StartPollContinuous(uint32_t chnMasks);
void App_CapTouch_WaitPollDoneBlocking(uint32_t waitPollCount);
uint32_t App_CapTouch_GetTouchedIndex(uint32_t *buf, uint32_t len, uint32_t *touchThresholdVals);

/* User application entry. */
int main(void)
{
    uint32_t key_value_pre;
    
    BOARD_InitBootClock();
    BOARD_InitBootPins();

    LEDx_Init();
    MRTTicker_Init();

    App_ProbeLEDx();
    
    bAppCapTouchCalibrationDone = false;
    LEDx_On(LEDx_ALL_MASK); /* turn on all the leds here, they would be trun off when the calibraiton is done. */

    /* CapTouch. */
    key_value_pre = gAppCapTouchKeyValue;
    App_CapTouch_Init();
    App_CapTouch_StartPollContinuous(0x1F); /* Active all the five X channels. */

    while (1)
    {

        /* gAppCapTouchKeyValue; */
        if (key_value_pre != gAppCapTouchKeyValue)
        {
            /* disable the leds for old key_value_pre */
            if (key_value_pre >= LEDx_COUNT)
            {
                LEDx_Off(LEDx_ALL_MASK);
            }
            else
            {
                LEDx_Off(cLEDxMaskArr[key_value_pre]);
            }
            
            key_value_pre = gAppCapTouchKeyValue;
            
            /* enable led for new gAppCapTouchKeyValue. */
            
            if (key_value_pre >= LEDx_COUNT)
            {
                LEDx_Off(LEDx_ALL_MASK);
            }
            else
            {
                LEDx_On(cLEDxMaskArr[key_value_pre]);
            }

        }
    }
}


void BOARD_InitBootClock(void)
{
    /* Enable clocks to relevant peripherals. */
    SystemCoreClockUpdate();
    /* Enable the clock to access the modules. */
    LPC_SYSCON->SYSAHBCLKCTRL0 |= (SWM | IOCON | GPIO0 | MRT);
}

void BOARD_InitBootPins(void)
{
    /* GPIO - LEDx. */
    /* - PIO0_20 */ LPC_IOCON->PIO0_20  = IOCON_RESERVED789; /* Disable internal pull up and pull down. */
    /* - PIO0_18 */ LPC_IOCON->PIO0_18  = IOCON_RESERVED789; /* Disable internal pull up and pull down. */
    /* - PIO0_15 */ LPC_IOCON->PIO0_15  = IOCON_RESERVED789; /* Disable internal pull up and pull down. */
    /* - PIO0_8  */ LPC_IOCON->PIO0_8   = IOCON_RESERVED789; /* Disable internal pull up and pull down. */
    /* - PIO0_9  */ LPC_IOCON->PIO0_9   = IOCON_RESERVED789; /* Disable internal pull up and pull down. */

    /* ACMP. */
    /* CAPT. */
}

/******************************************************************************
* LEDx Functions.
******************************************************************************/
void App_ProbeLEDx(void)
{
    uint32_t i;
    uint32_t ledxMaskAll = 0U;

    for (i = 0U; i < LEDx_COUNT; i++)
    {
        LEDx_On(cLEDxMaskArr[i]);
        MRTTicker_BlockingDelay10ms(20U);
        LEDx_Off(cLEDxMaskArr[i]);
        MRTTicker_BlockingDelay10ms(20U);

        ledxMaskAll |= cLEDxMaskArr[i];
    }

    for (i = 0U; i < 3U; i++)
    {
        LEDx_On(ledxMaskAll);
        MRTTicker_BlockingDelay10ms(20U);
        LEDx_Off(ledxMaskAll);
        MRTTicker_BlockingDelay10ms(20U);
    }
}

void LEDx_Init(void)
{
    uint32_t i;

    for (i = 0U; i < LEDx_COUNT; i++)
    {
        GPIOSetBitValue(0U, cLEDxIdxArr[i], 1U);
        GPIOSetDir(0U, cLEDxIdxArr[i], OUTPUT);
    }
}

void LEDx_On(uint32_t ledMask)
{
    uint32_t i;

    for (i = 0U; i < LEDx_COUNT; i++)
    {
        if (0U != (cLEDxMaskArr[i] & ledMask))
        {
            GPIOSetBitValue(0U, cLEDxIdxArr[i], 0U);
        }
    }
}

void LEDx_Off(uint32_t ledMask)
{
    uint32_t i;

    for (i = 0U; i < LEDx_COUNT; i++)
    {
        if (0U != (cLEDxMaskArr[i] & ledMask))
        {
            GPIOSetBitValue(0U, cLEDxIdxArr[i], 1U);
        }
    }
}

void LEDx_OnOnly(uint32_t ledMask)
{
    uint32_t i;

    for (i = 0U; i < LEDx_COUNT; i++)
    {
        if (0U != (cLEDxMaskArr[i] & ledMask))
        {
            GPIOSetBitValue(0U, cLEDxIdxArr[i], 0U);
        }
        else
        {
            GPIOSetBitValue(0U, cLEDxIdxArr[i], 1U);
        }
    }
}
/*****************************************************************************/

/******************************************************************************
* MRT Timer Functions.
******************************************************************************/
volatile uint32_t gAppTickerCounter;
void MRTTicker_Init(void)
{
    LPC_MRT->Channel[0].CTRL = (MRT_Repeat<<MRT_MODE)
                             | (1U <<MRT_INTEN)
                             ;
    NVIC_EnableIRQ(MRT_IRQn);

    /* Start the channel 0. */
    LPC_MRT->Channel[0].INTVAL = 15000000UL / 100UL; /* 100Hz */
}

/* 10ms per tick. */
void MRTTicker_BlockingDelay10ms(uint32_t ticks)
{
    gAppTickerCounter = ticks;
    while (gAppTickerCounter > 0U)
    {}
}

void MRT_IRQHandler(void)
{
    uint32_t flags = LPC_MRT->IRQ_FLAG;

    if (gAppTickerCounter > 0U)
    {
        gAppTickerCounter--;
    }

    LPC_MRT->IRQ_FLAG = flags;
}
/*****************************************************************************/

/******************************************************************************
* CapTouch.
******************************************************************************/
void App_CapTouch_InitClock(void)
{
    /* Enable clocks to relevant peripherals. */
    LPC_SYSCON->SYSAHBCLKCTRL0 |= (1U << CLK_GPIO0)
                                | (1U << CLK_IOCON)
                                | (1U << CLK_SWM)
                                ;
    LPC_SYSCON->SYSAHBCLKCTRL1 |= (1U << 0U); /* CLK_CAPT */

    /* Reset the CAPT module. */
    Do_Periph_Reset(RESET_CAPT);

    SystemCoreClockUpdate();

    /* Setup the FCLK for the CAP Touch block. */
    LPC_SYSCON->CAPTCLKSEL = 0U; /* CAPTCLKSEL_FRO_CLK. Use FRO 15MHz clock. */
}

#define IOCON_PIO_RESERVED (0x1 << 7U)
#define IOCON_PIO_MODE(x)  (((x) & 0x3) << 3U)
#define IOCON_PIO_OD_MASK  (0x1 << 10U)

void App_CapTouch_InitPins(void)
{
    // Enable the CAP Touch functions on their pins in the SWM
    ConfigSWM(CAPT_X0, P0_12);
    ConfigSWM(CAPT_X1, P0_21);
    ConfigSWM(CAPT_X2, P0_13);
    ConfigSWM(CAPT_X3, P0_11);
    ConfigSWM(CAPT_X4, P0_19);
    ConfigSWM(CAPT_YH, P0_17);
    ConfigSWM(CAPT_YL, P0_7 );

    LPC_IOCON->PIO0_12 = IOCON_PIO_MODE(0) /* | IOCON_PIO_OD_MASK */ | IOCON_PIO_RESERVED; /* CAPT_X0. */
    LPC_IOCON->PIO0_21 = IOCON_PIO_MODE(0) /* | IOCON_PIO_OD_MASK */ | IOCON_PIO_RESERVED; /* CAPT_X1. */
    LPC_IOCON->PIO0_13 = IOCON_PIO_MODE(0) /* | IOCON_PIO_OD_MASK */ | IOCON_PIO_RESERVED; /* CAPT_X2. */
    LPC_IOCON->PIO0_11 = IOCON_PIO_MODE(0) /* | IOCON_PIO_OD_MASK */ | IOCON_PIO_RESERVED; /* CAPT_X3. */
    LPC_IOCON->PIO0_19 = IOCON_PIO_MODE(0) /* | IOCON_PIO_OD_MASK */ | IOCON_PIO_RESERVED; /* CAPT_X4. */
    LPC_IOCON->PIO0_17 = IOCON_PIO_MODE(0) /* | IOCON_PIO_OD_MASK */ | IOCON_PIO_RESERVED; /* CAPT_YH. */
    LPC_IOCON->PIO0_7  = IOCON_PIO_MODE(0) /* | IOCON_PIO_OD_MASK */ | IOCON_PIO_RESERVED; /* CAPT_YL */
}

/* Initialize the CapTouch application with interrupts enabled */
void App_CapTouch_Init(void)
{
    App_CapTouch_InitClock();
    App_CapTouch_InitPins();

    /* CAPT_CTRL - Setup the working condition of CapTouch.
     *  0: 1 - POLLMODE : Scan method including: Inactive (stop the scan right now, control and status)
     *                                            PollNow (one time scan and trigger by software)
     *                                            Continuous (continuously autumatically by hardware)
     *  2: 3 - TYPE     : No other selection, the only available value is 0, can be reserved.
     *  4    - TRIGGER  : Assert the scan event done by checking the YH GPIO logic level or from the external ACMP module.
     *  5    - WAIT     : Enable blocking the scan before the available touch value is read.
     *  6: 8
     *  8:11 - FDIV     : Divider from CAPT clock source to FCLK.
     * 12:13 - XPINUSE  : Logic state for unactivated X pins: High-Z (0x0) or Grounded (0x1).
     * 15    - INCHANGE : Check if this CAPT_CTRL resiger is accept by hardware.
     * 31:16 - XPINSEL  : Enable the activated X pins in the scan (or the sequence for continuous mode)
     */
    while (0U != (INCHANGE & LPC_CAPT->CTRL)) /* Wait while the LPC_CAPT is in change. */
    {}
    LPC_CAPT->CTRL = POLLMODE_INACTIVE /* Would launch the scan later. */
                   | TYPE_TRIGGER_YH /* Use YH trigger as the simplest use case. */
                   /* | WAIT)*/  /* NO wait, since no continuous mode is used. */
                   | ((15000000UL/3000000UL)-1) << FDIV /* Divider for FCLK from clock source. */
                   /* | XPINUSE_HIGHZ */ /* X pins state when they are not active. */
                   | (XPINUSE_LOW)
                   | (0UL << XPINSEL) /* Would indicated the channel when launch the scan later. */
                   ;

    /* CAPT_POLL_TCNT - Setup the timing and its judging parameters during the scan.
     * 0 :11 - TCNT    : Hardware threshold value between YES_TOUCH and NO_TOUCH.
     * 12:15 - TOUT    : Timeout count before a scan for one X pad is done.
     * 16:23 - POLL    : x4096 count PCLK between the scan for each X pad when in continuous scan mode.
     * 24:25 - MDELAY  : Delay PCLK when in Measure step.
     * 27:26 - RDELAY  : Delay PCLK when in Reset step.
     * 31    - TCHLOWER: Assert touch event when sensing value is higher (0) / lower (1) than the threshold (TCNT) value.
     */
    LPC_CAPT->POLL_TCNT = (0U  << TCNT) /* No hardware judgment for YES/NO touch. Software would check the sensing value after PollDone event. */
                        | (12U << TOUT) /* Timeout value. */
                        | (5U << POLL)  /* x4096 FCLK between poll round in continuous scan mode. */
                        | (3U  << RDELAY) /* Reset delay. */
                        | (3U  << MDELAY) /* Measurement delay. */
                        | (1U  << TCHLOWER)
                        ;

    /* Enable the interrupts. */
    LPC_CAPT->INTENSET = YESTOUCH | NOTOUCH | TIMEOUT | OVERRUN;
    NVIC_EnableIRQ(CAPT_IRQn);
}

void App_CapTouch_StartPollNow(uint32_t chnIdx)
{
    while (0U != (INCHANGE & LPC_CAPT->CTRL)) /* Wait while the LPC_CAPT is in change. */
    {}
    LPC_CAPT->CTRL |= ( POLLMODE_NOW | ((1U << chnIdx) << XPINSEL) );
}

void App_CapTouch_StartPollContinuous(uint32_t chnMasks)
{
    while (0U != (INCHANGE & LPC_CAPT->CTRL)) /* Wait while the LPC_CAPT is in change. */
    {}
    LPC_CAPT->CTRL |= ( POLLMODE_CONTINUOUS | (chnMasks << XPINSEL) );
}

uint32_t get_min_idx(uint32_t *buf, uint32_t len)
{
    uint32_t i, min_idx = 0u;
    
    if (len > 1)
    {
        for (i = 1u; i < len; i++)
        {
            if ( buf[i] < buf[min_idx])
            {
                min_idx = i;
            }
        }
    }

    return min_idx;
}

uint32_t get_max_idx(uint32_t *buf, uint32_t len)
{
    uint32_t i, max_idx = 0u;
    
    if (len > 1)
    {
        for (i = 1u; i < len; i++)
        {
            if ( buf[i] > buf[max_idx])
            {
                max_idx = i;
            }
        }
    }

    return max_idx;
}


/* EOF. */

