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
static AS3911GainMode_t as3911GainMode = AS3911_GAIN_FIXED;

/*! Lookup table for the active modulation level adjustment mode. */
static const AS3911GainTable_t *as3911GainTable = NULL;

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

void as3911SetGainMode(AS3911GainMode_t modulationLevelMode
    , const AS3911GainTable_t *modulationLevelTable)
{
    as3911GainMode = modulationLevelMode;
    as3911GainTable = modulationLevelTable;
}

void as3911GetGainMode(AS3911GainMode_t *modulationLevelMode
    , const AS3911GainTable_t *modulationLevelTable)
{
//    *modulationLevelMode = as3911GainMode;
//    modulationLevelTable = as3911GainTable;
}

void as3911AdjustGain()
{
    u32 irqs = 0;
    u8 measurementCommand = 0;
    u8 amplitudePhase = 0;
    u8 index = 0;

    D2(LABLE(0xEE);DATAIN(as3911GainMode));
    if (AS3911_GAIN_FIXED == as3911GainMode)
        return;
    else if (AS3911_GAIN_FROM_AMPLITUDE == as3911GainMode)
        measurementCommand = AS3911_CMD_MEASURE_AMPLITUDE;
    else if (AS3911_GAIN_FROM_PHASE == as3911GainMode)
        measurementCommand = AS3911_CMD_MEASURE_PHASE;
    else
    {
        /* ToDo: enter debug code here. */
        return;
    }

    /* Measurement based modulation strength adjustment requires a modulation
     * level table with at least 2 entries to perform interpolation.
     */
    if ((as3911GainTable == NULL) || (as3911GainTable->length < 2))
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

    for (index = 0; index < as3911GainTable->length; index++)
    {
        if (amplitudePhase <= as3911GainTable->x[index])
            break;
    }

	/* If amplitudePhase is greater than the last table entry, then use the
	 * gain reduction of the last table entry.
	 */
	if (index == as3911GainTable->length)
		index--;

    as3911WriteRegister(AS3911_REG_RX_CONF3, as3911GainTable->y[index]);
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

#endif

