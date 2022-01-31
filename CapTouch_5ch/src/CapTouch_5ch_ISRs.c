/*
 * Example_CapTouch_5ch_ISRs.c
 *
 *  Created on: Feb 7, 2018
 *      Author: 
 */

#include "app_inc.h"


volatile uint32_t gAppCapTouchTimeoutErrCounter;
uint32_t gAppCapTouchValue[CAPT_X_PIN_COUNT];
volatile uint32_t gAppCapTouchKeyValue;


/* for auto-calibration. */
#define APP_CAPTOUCH_CALIB_FILTER_DIFF  20u
#define APP_CAPTOUCH_CALIB_FIFO_COUNT   5u
uint32_t gAppCapTouchCalibrationFIFO[APP_CAPTOUCH_CALIB_FIFO_COUNT] = {0u};
volatile uint32_t gAppCapTouchCalibrationIdx = 0u;
volatile uint32_t gAppCapTouchThresholdValue;
volatile bool     bAppCapTouchCalibrationDone = false;

/* anti-rebounce filter for key value. */
volatile uint32_t gAppCapTouchKeyValueFilterCountMax = 6u;
volatile uint32_t gAppCapTouchKeyValueFilterCount;
volatile uint32_t gAppCapTouchKeyValueFilterValue = CAPT_X_PIN_COUNT; /* the stable output. */
volatile uint32_t gAppCapTouchKeyValueFilterValueNew; /* the protential one. */


uint32_t get_min_idx(uint32_t *buf, uint32_t len);
uint32_t get_max_idx(uint32_t *buf, uint32_t len);

extern void LEDx_Off(uint32_t ledMask);



/* CapTouch ISR entry. */
#define CAPT_IRQHandler CMP_IRQHandler
void CAPT_IRQHandler(void)
{
    uint32_t flags = LPC_CAPT->STATUS;
    uint32_t touch = LPC_CAPT->TOUCH;
    uint32_t chn = (touch >> 12U) & 0xF;
    touch = touch & 0xFFF;


    /* record the sensing value for each channel. */
    if (0u != ((YESTOUCH | NOTOUCH) & flags ) )
    {
        /* Record the sensing value into buffer. */
        //gAppCapTouchValue[ (touch >> 12U) & 0xF] = touch & 0xFFF;
        
        if (!bAppCapTouchCalibrationDone) /* for auto-calibration. */
        {
            gAppCapTouchValue[chn] = touch;
        }
        else /* for normal process, with delta filter. */
        {
            /* update sensing value. */
            if (gAppCapTouchValue[chn] < touch)
            {
                gAppCapTouchValue[chn] += ((touch - gAppCapTouchValue[chn]) >> 4u);
            }
            else if (gAppCapTouchValue[chn] > touch)
            {
                gAppCapTouchValue[chn] -= ((gAppCapTouchValue[chn] - touch) >> 4u);
            }
        }
        
    }

    /* record the timeout error. */
    if (0u != (TIMEOUT & flags) )
    {
        gAppCapTouchTimeoutErrCounter++; /* developer can watch this value when debugging to check if there is any timeout event. */
    }
    
    /* process the detection when the scan sequence is done. */
    if (0U != (POLLDONE & flags) ) /* PollDone is for the whole sequence. */
    {
        if (!bAppCapTouchCalibrationDone) /* for auto calibration. */
        {
            uint32_t calib_min_idx, calib_max_idx;

            gAppCapTouchCalibrationFIFO[gAppCapTouchCalibrationIdx] = gAppCapTouchValue[0]; /* use 0 channel as reference. */
            gAppCapTouchCalibrationIdx = (gAppCapTouchCalibrationIdx+1u) % APP_CAPTOUCH_CALIB_FIFO_COUNT;
            
            calib_min_idx = get_min_idx(gAppCapTouchCalibrationFIFO, APP_CAPTOUCH_CALIB_FIFO_COUNT);
            calib_max_idx = get_max_idx(gAppCapTouchCalibrationFIFO, APP_CAPTOUCH_CALIB_FIFO_COUNT);
            if (    (gAppCapTouchCalibrationFIFO[calib_min_idx] > 0u)
                && ((gAppCapTouchCalibrationFIFO[calib_max_idx] - gAppCapTouchCalibrationFIFO[calib_min_idx]) <= APP_CAPTOUCH_CALIB_FILTER_DIFF)  )
            {
                /* record the final threshold value. */
                gAppCapTouchThresholdValue = (gAppCapTouchCalibrationFIFO[calib_min_idx] / 5u ) * 4u; /* calc the thershold value based on the idle value. */
                
                /* initialize the filter. */
                gAppCapTouchKeyValueFilterCountMax = 6u;
                gAppCapTouchKeyValueFilterCount = 0u;
                gAppCapTouchKeyValueFilterValue = CAPT_X_PIN_COUNT;
                gAppCapTouchKeyValueFilterValueNew = gAppCapTouchKeyValueFilterValue;
                
                /* mark the calibration is done. */
                bAppCapTouchCalibrationDone = true;
                LEDx_Off(LEDx_ALL_MASK); /* turn off all the led. */
            }
        }
        else /* normal detection. */
        {
            uint8_t touch_count = 0u;
            uint8_t i;
            
            /* get the key value. */
            for (i = 0u; i < CAPT_X_PIN_COUNT; i++)
            {
                if (gAppCapTouchValue[i] <= gAppCapTouchThresholdValue)
                {
                    touch_count++;
                }
            }
            if (touch_count >= (CAPT_X_PIN_COUNT-1u))
            {
                gAppCapTouchKeyValue = get_max_idx(gAppCapTouchValue, CAPT_X_PIN_COUNT);
            }
            else
            {
                gAppCapTouchKeyValue = CAPT_X_PIN_COUNT;
            }
            
            /* execute the anti-rebounce filter */
            if (gAppCapTouchKeyValue != gAppCapTouchKeyValueFilterValue)
            {
                if (gAppCapTouchKeyValue == gAppCapTouchKeyValueFilterValueNew)
                {
                    if (gAppCapTouchKeyValueFilterCount >= gAppCapTouchKeyValueFilterCountMax)
                    {
                        gAppCapTouchKeyValueFilterValue = gAppCapTouchKeyValueFilterValueNew; /* update the output of filter. */
                    }
                    else
                    {
                        gAppCapTouchKeyValueFilterCount++;
                    }
                }
                else
                {
                    gAppCapTouchKeyValueFilterValueNew = gAppCapTouchKeyValue; /* use the most recent RAW value as the protential new outoput. */
                }
            }
            /* use the filter output as the final key value. */
            gAppCapTouchKeyValue = gAppCapTouchKeyValueFilterValue; 
        }
    }

    LPC_CAPT->STATUS = flags;
}

/* EOF. */

