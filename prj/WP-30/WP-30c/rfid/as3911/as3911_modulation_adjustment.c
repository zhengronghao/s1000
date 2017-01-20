/*
 *****************************************************************************
 * Copyright @ 2009 by austriamicrosystems AG                                *
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
#include <limits.h>

#if (defined(EM_AS3911_Module))
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*! Sanity timeout for the AS3911 direct command completed interrupt. */
#define AS3911_DCT_IRQ_SANITY_TIMEOUT   20//5

#define DELTA(A,B) (((A)>(B))?((A)-(B)):((B)-(A)))

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

//#define EMV_AS3911TABLE_TEST

#if defined(EMV_AS3911TABLE_TEST)
static u16 modLastAmp = SHRT_MAX;

static const u8 lowXs[2] = {0x5e,0x6c};
static const u8 lowYs[2] = {0xce,0xae};

static const AS3911ModulationLevelTable_t as3911LowModulationLevelTable = 
{
    2,
    (u8*)lowXs,
    (u8*)lowYs
};

static struct AS3911OutputLevels outputLevels = { 0, 0, 0, 0 };

static const struct AS3911GainTable * lowGainTable = NULL;
static const struct AS3911GainTable * normGainTable = NULL;
#endif

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

void as3911SetOutputLevels(const struct AS3911OutputLevels *ol)
{
#if defined(EMV_AS3911TABLE_TEST)
    outputLevels = *ol;
#endif
}

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
//	} /* FIXME: this always evaluates to false, no variable is 2 and 3 at the same time */	
//	  /* use "||" instead? */
//	else if (  (AS3911_MODULATION_LEVEL_FROM_PHASE == as3911ModulationLevelMode)
//	        && (AS3911_MODULATION_LEVEL_FROM_AMPLITUDE == as3911ModulationLevelMode))
//	{
//		modulationLevelData = as3911ModulationLevelTable;
//	}
}

void as3911AdjustModulationLevel()
{
#if 0
    u32 irqs = 0;
    u8 measurementCommand = 0;
    u8 amplitudePhase = 0;
    u8 phase = 0;
    u8 antennaDriverStrength = 0;
    u8 index = 0;
    u8 am_off_level;
    AS3911ModulationLevelTable_t const *p;
    struct AS3911GainTable const *gt = NULL;
    int i;
    u8 j;

    p = as3911ModulationLevelTable;
    as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &am_off_level);
    for ( i = 0; i<2 ; i++)
    {
        as3911ExecuteCommandAndGetResult(AS3911_CMD_MEASURE_AMPLITUDE, AS3911_REG_AD_RESULT, AS3911_DCT_IRQ_SANITY_TIMEOUT, &amplitudePhase);
        //emvPrintf("i=%x, am_off_lvl=%x, amp=%x\n",i,am_off_level, amplitudePhase);
        if (i == 0)
        {
            if (outputLevels.lowRed == outputLevels.highRed)
            {
                gt = normGainTable;
                break;
            }
            if(outputLevels.highRed == am_off_level)
            {
                if(amplitudePhase > outputLevels.decThresh)
                { /* amplitude is fine, output power and current table are o.k. */
                    gt = normGainTable;
                    break;
                }
                /* Reduce output power, redo measuremnt, change table */
                j = outputLevels.highRed;
                do{
                    j++;
                    as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, j);
                }while (j < outputLevels.lowRed);

                emvPrintf("%x <= %x : reduced AM_OFF_LEVEL\ -> 0x%x\n", amplitudePhase, outputLevels.decThresh,j);
                modLastAmp = SHRT_MAX; /* Force triggering of automatic mod depth calibration if used */
                p = &as3911LowModulationLevelTable;
            }
            else
            {
                if(amplitudePhase < outputLevels.incThresh)
                { /* Output power was reduced, amplitude is still fine, use low table */
                    p = &as3911LowModulationLevelTable;
                    gt = lowGainTable;
                    break;
                }
                /* Output power was reduced, increase again, redo measuremnt, use normal table */
                j = am_off_level;
                do{
                	j--;
                    as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, j);
                }while(j>outputLevels.highRed);
                gt = normGainTable;
                    
                emvPrintf("%x >= %x: increased AM_OFF_LEVEL\ -> 0x%x\n",amplitudePhase, outputLevels.incThresh, j);
                modLastAmp = SHRT_MAX; /* Force triggering of automatic mod depth calibration if used */
            }
        }
    }

    if (gt != NULL)
    {
        const struct AS3911GainTableVal *gtv = NULL;
        i = 0;
        while (i+1 < gt->num_vals)
        {
            if (DELTA(gt->table[i+1].amp,amplitudePhase) > DELTA(gt->table[i].amp,amplitudePhase))
            { /* If amplitude delta gets bigger, we can break since table is first order sorted by amp */
                break;
            }
            i++;
        }
        //emvPrintf("highest index of closest amp: %d\n",i);
        /* We found now highest index with the closest amplitude.*/
        /* Now go down to find the one with the closest phase */
        if ( (i-1 >= 0) && (gt->table[i-1].amp == gt->table[i].amp) )
        { /* Need to measure phase and find the one with closest phase */

            as3911ExecuteCommandAndGetResult(AS3911_CMD_MEASURE_PHASE, AS3911_REG_AD_RESULT, AS3911_DCT_IRQ_SANITY_TIMEOUT, &phase);
            while (i-1 >= 0)
            {
                if (gt->table[i-1].amp != gt->table[i].amp)
                { /* Only continue if amp is still the same */
                    //emvPrintf("break amp not equal %d\n",i);
                    break;
                }
                if (DELTA(gt->table[i-1].phase,phase) > DELTA (gt->table[i].phase, phase))
                { /* If phase delta gets bigger, we can break since table is second order sorted by phase */
                    //emvPrintf("break phase delta too low %d\n",i);
                    break;
                }
                i--;
            }
        }
        if (i < gt->num_vals)
        {
            gtv = gt->table + i;
        }
        if (gtv)
        {
            if (gt->used_id != i)
            {
                const char *text = "";
                /* FIXME Remove const, as used_id is solely used for printing in here. Will not work with gain tables in ROM! */
                ((struct AS3911GainTable*)gt)->used_id = i;
                if (gt->text != NULL) 
                {
                    text = gt->text;
                }
                emvPrintf("DGT %s adjusted gains: %02hx %02hx -> %02hx %02hx %02hx %02hx %02hx\n",text , amplitudePhase, phase, gtv->reg02, gtv->regA, gtv->regB, gtv->regC, gtv->regD);
            }
            as3911ModifyRegister(AS3911_REG_OP_CONTROL,
                                 AS3911_REG_OP_CONTROL_rx_man | AS3911_REG_OP_CONTROL_rx_chn,
                                 gtv->reg02 & 0x30);
            as3911ContinuousWrite(AS3911_REG_RX_CONF1, (uchar *)&gtv->regA, 4);
        }
    }

    /* Done with OFF level, now handle ON level if needed */
    if (AS3911_MODULATION_LEVEL_FIXED == as3911ModulationLevelMode)
        return;
    else if (AS3911_MODULATION_LEVEL_AUTOMATIC == as3911ModulationLevelMode)
    {
        as3911ExecuteCommandAndGetResult(AS3911_CMD_MEASURE_AMPLITUDE, AS3911_REG_AD_RESULT, AS3911_DCT_IRQ_SANITY_TIMEOUT, &amplitudePhase);
        
        if (abs(((s32)amplitudePhase - (s32)modLastAmp)) > as3911ModulationLevelAutomaticAdjustmentData->hysteresis)
        {

            modLastAmp = amplitudePhase;
            as3911ExecuteCommandAndGetResult(AS3911_CMD_CALIBRATE_MODULATION, AS3911_REG_AM_MOD_DEPTH_RESULT, AS3911_DCT_IRQ_SANITY_TIMEOUT, NULL);

//            emvDisplayString("Recalibrated mod depth\n");
            { /* delay some time after this pulse */
                int i;
                for (i = 0; i<1000; i++); // FIXME wild approximation
            }
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
    if ((p == NULL) || (p->length < 2))
    {
        /* ToDo: enter debug code here. */
        return;
    }

    as3911ExecuteCommandAndGetResult(measurementCommand, AS3911_REG_AD_RESULT, AS3911_DCT_IRQ_SANITY_TIMEOUT, &amplitudePhase);

    for (index = 0; index < p->length; index++)
    {
        if(amplitudePhase <= p->x[index])
            break;
    }
    /* Use the last interpolation level dataset for any values outside the highest.
     * x-value from the datasets.
     */
    if (index == p->length)
        index--;

    if (index == 0)
        antennaDriverStrength = as3911GetInterpolatedValue(
            p->x[index],
            p->y[index],
            p->x[index+1],
            p->y[index+1],
            amplitudePhase);
    else
        antennaDriverStrength = as3911GetInterpolatedValue(
            p->x[index-1],
            p->y[index-1],
            p->x[index],
            p->y[index],
            amplitudePhase);

    as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, antennaDriverStrength);
#else
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
#endif
}

void as3911SetGainTables(const struct AS3911GainTable *gainTableLow, const struct AS3911GainTable *gainTableNorm)
{
#if defined(EMV_AS3911TABLE_TEST)
    lowGainTable = gainTableLow;
    normGainTable = gainTableNorm;
#endif
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

