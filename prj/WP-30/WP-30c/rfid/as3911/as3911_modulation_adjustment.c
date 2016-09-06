/*
 *****************************************************************************
 * Copyright @ 2009                                 *
 * All rights are reserved.                                                  *
 *                                                                           *
 * Reproduction in whole or in part is prohibited without the written consent*
 * of the copyright owner. Austriamicrosystems reserves the right to make    *
 * changes without notice at any time. The software is provided as is and    *
 * Austriamicrosystems makes no warranty, expressed, implied or statutory,   *
 * including but not limited to any implied warranty of merchantability or   *
 * fitness for any particular purpose, or that the use will not infringe any *
 * third party patent, copyright or trademark. Austriamicrosystems should    *
 * not be liable for any loss or damage arising from its use.                *
 *****************************************************************************
 */

/*
 * PROJECT: AS3911 firmware
 * $Revision: $
 * LANGUAGE: ANSI C
 */

/*! \file as3911_modulation_adjustment.c
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 modulation level adjustment
 *
 * Adjust the modulation level of the AS3911 based on the amplitude or phase of
 * the reader field.
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "wp30_ctrl.h"

#if (defined(EM_AS3911_Module))
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*! Sanity timeout for the AS3911 direct command completed interrupt. */
#define AS3911_DCT_IRQ_SANITY_TIMEOUT   20//5

/*
******************************************************************************
* MACROS
******************************************************************************
*/

/*
******************************************************************************
* LOCAL DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/

/*! Active modulation level adjustment mode. */
static AS3911ModulationLevelMode_t as3911ModulationLevelMode = AS3911_MODULATION_LEVEL_FIXED;

/*! Lookup table for the active modulation level adjustment mode. */
static const AS3911ModulationLevelTable_t *as3911ModulationLevelTable = NULL;
static const AS3911ModulationLevelAutomaticAdjustmentData_t *as3911ModulationLevelAutomaticAdjustmentData = NULL;

/*
******************************************************************************
* LOCAL TABLES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*!
 *****************************************************************************
 * \brief Calculate a linear interpolated value.
 *
 * \note \a x1 must be <= x2.
 *
 * Calculate the linear interpolated value at \a xi. The curve for the linear
 * interpolation is defined by (\a x1, \a y1) and (\a x2, \a y2).
 * For values \a xi below \a x1 the function will return \a y1, and for values
 * \a xi above \a x2 the function will return \a y2. For any intermediate value
 * the function will return y1 + (y2 - y1) * (xi - x1) / (x2 - x1).
 *
 * \param[in] x1 X-coordinate of the first point.
 * \param[in] y1 Y-coordinate of the first point.
 * \param[in] x2 X-coordinate of the second point.
 * \param[in] y2 Y-coordinate of the second point.
 * \param[in] xi X-coordinate of the interpolation point.
 *
 * \return The interpolated Y-value at \a xi.
 *****************************************************************************
 */
int as3911GetInterpolatedValue(int x1, int y1, int x2, int y2, int xi);

/*
******************************************************************************
* GLOBAL VARIABLE DEFINITIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/

void as3911SetModulationLevelMode(AS3911ModulationLevelMode_t modulationLevelMode
    , const void *modulationLevelData)
{
    as3911ModulationLevelMode = modulationLevelMode;
	
    if (AS3911_MODULATION_LEVEL_FIXED == modulationLevelMode)
	{
		return;
	}
	else if (AS3911_MODULATION_LEVEL_AUTOMATIC == modulationLevelMode)
	{
		as3911ModulationLevelAutomaticAdjustmentData = modulationLevelData;
	}
	else if (  (AS3911_MODULATION_LEVEL_FROM_PHASE == modulationLevelMode)
	        || (AS3911_MODULATION_LEVEL_FROM_AMPLITUDE == modulationLevelMode))
	{
		as3911ModulationLevelTable = modulationLevelData;
	}
}

void as3911GetModulationLevelMode(AS3911ModulationLevelMode_t *modulationLevelMode
    , const void *modulationLevelData)
{
//    *modulationLevelMode = as3911ModulationLevelMode;
//    if (AS3911_MODULATION_LEVEL_FIXED == as3911ModulationLevelMode)
//	{
//		modulationLevelData = NULL;
//	}
//	else if (AS3911_MODULATION_LEVEL_AUTOMATIC == as3911ModulationLevelMode)
//	{
//		modulationLevelData = as3911ModulationLevelAutomaticAdjustmentData;
//	}
//	else if (  (AS3911_MODULATION_LEVEL_FROM_PHASE == as3911ModulationLevelMode)
//	        && (AS3911_MODULATION_LEVEL_FROM_AMPLITUDE == as3911ModulationLevelMode))
//	{
//		modulationLevelData = as3911ModulationLevelTable;
//	}
}

void as3911AdjustModulationLevel()
{
    u32 irqs = 0;
    u8 measurementCommand = 0;
    u8 amplitudePhase = 0;
    u8 antennaDriverStrength = 0;
    u8 index = 0;

    D2(LABLE(0xEE);DATAIN(as3911ModulationLevelMode));
    if (AS3911_MODULATION_LEVEL_FIXED == as3911ModulationLevelMode)
        return;
	else if (AS3911_MODULATION_LEVEL_AUTOMATIC == as3911ModulationLevelMode)
	{
		int count = 0;
		
		as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
		as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
		as3911ExecuteCommand(AS3911_CMD_CALIBRATE_MODULATION);
		as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, AS3911_DCT_IRQ_SANITY_TIMEOUT, &irqs);
		if (irqs != AS3911_IRQ_MASK_DCT)
		{
			/* ToDo: enter debug code here. */
			return;
		}
		as3911DisableInterrupts(AS3911_IRQ_MASK_DCT);

        if ( as3911ModulationLevelAutomaticAdjustmentData ) {
            for (count = 0; count < as3911ModulationLevelAutomaticAdjustmentData->delay; count++)
            {
                /* FIXME: replace with microseconds sleep from AMS base firmware. */
                //			__asm("REPEAT #0x99");
                volatile int i = 50; //暂时代替
                while(i--);
                __asm("NOP");
            }
        }else{
            //自动调整无table
            s_DelayUs(5000);
        }
	}
    else if (AS3911_MODULATION_LEVEL_FROM_AMPLITUDE == as3911ModulationLevelMode)
        measurementCommand = AS3911_CMD_MEASURE_AMPLITUDE;
    else if (AS3911_MODULATION_LEVEL_FROM_PHASE == as3911ModulationLevelMode)
        measurementCommand = AS3911_CMD_MEASURE_PHASE;
    else
    {
        /* ToDo: enter debug code here. */
        return;
    }

    /* Measurement based modulation strength adjustment requires a modulation
     * level table with at least 2 entries to perform interpolation.
     */
    if ((as3911ModulationLevelTable == NULL) || (as3911ModulationLevelTable->length < 2))
    {
        /* ToDo: enter debug code here. */
        return;
    }

    as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
    as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
    as3911ExecuteCommand(measurementCommand);
    as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, AS3911_DCT_IRQ_SANITY_TIMEOUT, &irqs);
    if (irqs != AS3911_IRQ_MASK_DCT)
    {
        /* ToDo: enter debug code here. */
        return;
    }
    as3911DisableInterrupts(AS3911_IRQ_MASK_DCT);
    as3911ReadRegister(AS3911_REG_AD_RESULT, &amplitudePhase);

    for (index = 0; index < as3911ModulationLevelTable->length; index++)
    {
        if(amplitudePhase <= as3911ModulationLevelTable->x[index])
            break;
    }
    /* Use the last interpolation level dataset for any values outside the highest.
     * x-value from the datasets.
     */
    if (index == as3911ModulationLevelTable->length)
        index--;

    if (index == 0)
        antennaDriverStrength = as3911GetInterpolatedValue(
            as3911ModulationLevelTable->x[index],
            as3911ModulationLevelTable->y[index],
            as3911ModulationLevelTable->x[index+1],
            as3911ModulationLevelTable->y[index+1],
            amplitudePhase);
    else
        antennaDriverStrength = as3911GetInterpolatedValue(
            as3911ModulationLevelTable->x[index-1],
            as3911ModulationLevelTable->y[index-1],
            as3911ModulationLevelTable->x[index],
            as3911ModulationLevelTable->y[index],
            amplitudePhase);

    D2(DATAIN(antennaDriverStrength));
    as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, antennaDriverStrength);
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

int as3911GetInterpolatedValue(int x1, int y1, int x2, int y2, int xi)
{
    if (xi <= x1) {
        return y1;
    } else if (xi >= x2) {
        return y2;
    } else {
        return y1 + (((long) y2 - y1) * (xi - x1)) / (x2 - x1);
    }
}
#endif

