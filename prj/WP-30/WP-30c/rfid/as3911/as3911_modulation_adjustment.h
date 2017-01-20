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
 
/*! \file as3911_modulation_adjustment.h
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 modulation level adjustment
 */

#ifndef AS3911_MODULATION_ADJUSTMENT_H
#define AS3911_MODULATION_ADJUSTMENT_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "ams_types.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL DATA TYPES
******************************************************************************
*/

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * Available modulation level adjustment modes.
 *****************************************************************************
 */
typedef enum _AS3911ModulationLevelMode_
{
    AS3911_MODULATION_LEVEL_FIXED /*!< No dynamic modulation level adjustment. */,
	AS3911_MODULATION_LEVEL_AUTOMATIC /*!< Modulation level set with AS3911 automatic adjustment. */,
    AS3911_MODULATION_LEVEL_FROM_AMPLITUDE /*!< Modulation level is adjusted based on RF amplitude. */,
    AS3911_MODULATION_LEVEL_FROM_PHASE /*!< Modulation level is adjusted based on RFI, RFO signal phase difference. */
} AS3911ModulationLevelMode_t;

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Modulation level adjustment lookup table.
 *
 * Lookup table for modulation level adjustment. This table consists of x and
 * y values as separate arrays. So x[i], y[i] define the (x,y) coordinates of
 * point i of the lookup table.
 *
 * \note The x values must be monotonically increasing to produce correct
 * results during linear interpolation.
 *****************************************************************************
 */
typedef struct _AS3911ModulationLevelTable_
{
    u8 length; /*!< Length of the \a x and \a y point arrays. */
    u8 *x; /*!< X data points of the modulation level table. */
    u8 *y; /*!< Y data points of the modulation level table. */
} AS3911ModulationLevelTable_t;

typedef struct _AS3911ModulationLevelAutomaticAdjustmentData_
{
	u8 targetValue; /*!< Target value for the AS3911 automatic modulation depth adjustment. */
	u8 hysteresis; /*!< Hysteresis for doing adjustment again */
	u16 delay; /*!< Time to wait after automatic modulation depth adjustment (µs). */
} AS3911ModulationLevelAutomaticAdjustmentData_t;

struct AS3911OutputLevels
{
    u8 lowRed; /*!< low level reduction value for reg 0x27 */
    u8 highRed;/*!< high level reduction value for reg 0x27 */
    u8 decThresh;/*!< amplitude threshold for going high->low */
    u8 incThresh;/*!< amplitude threshold for going low->high  */
};

struct AS3911GainTableVal
{
       u8 amp;
       u8 phase;
       u8 reg02;
       u8 regA;
       u8 regB;
       u8 regC;
       u8 regD;
};

struct AS3911GainTable
{
       int     num_vals;
       struct  AS3911GainTableVal table[16];
       int used_id;
       const char *text;
};


/*
******************************************************************************
* GLOBAL VARIABLE DECLARATIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Set the modulation level adjustment mode and modulation level
 * adjustment table.
 *
 * Set the modualtion level adjustment mode and configure additional data
 * needed by some of the adjustment modes. There are three modes available:
 *
 * AS3911_MODULATION_LEVEL_FIXED: no dynamic adjustment of the modulation level
 * is performed. The \a modulationLevelTable parameter will be ignored.
 *
 * AS3911_MODULATION_LEVEL_AUTOMATIC: Use the AS3911 automatic modulation depth
 * adjustment to set the modulation depth.
 *
 * AS3911_MODULATION_LEVEL_FROM_AMPLITUDE: The modulation level is adjusted
 * based on the RF amplitude and the lookup table values in
 * \a modulationLevelModeData.
 *
 * AS3911_MODULATION_LEVEL_FROM_PHASE: The modulation level is adjusted based on
 * the phase difference between RFI and RFO signals and the lookup table values
 * in \a modulationLevelModeData.
 *
 * \note Internally only a pointer to the modulationLevelModeData will be stored.
 * The data structure pointed to by \a modulationLevelModeData should therefore
 * not be modified while in use by this module.
 *
 * \param[in] modulationLevelMode Set the modulation level adjustment mode.
 * \param[in] modulationLevelModeData Additional data for the selected modulation
 *   level adjustment mode. This must be of the following datatype:
 *   AS3911_MODULATION_LEVEL_FIXED: no additional data required, can be NULL.
 *   AS3911_MODULATION_LEVEL_AUTOMATIC: Pointer to AS3911ModulationLevelAutomaticAdjustmentData_t.
 *   AS3911_MODULATION_LEVEL_FROM_AMPLITUDE: Pointer to AS3911ModulationLevelTable_t.
 *   AS3911_MODULATION_LEVEL_FROM_PHASE: Pointer to AS3911ModulationLevelTable_t.
 *   
 * AS3911_MODULATION_LEVEL_FROM_PHASE modes.
 *****************************************************************************
 */
void as3911SetModulationLevelMode(AS3911ModulationLevelMode_t modulationLevelMode
    , const void *modulationLevelModeData);

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Get the active modulation level adjustment mode and modulation level
 * adjustment table.
 *
 * \param[out] modulationLevelMode Set to the active modulation level adjustment
 * mode
 * \param[out] modulationLevelTable Set to the active modulation level
 * adjustment table.
 *****************************************************************************
 */
void as3911GetModulationLevelMode(AS3911ModulationLevelMode_t *modulationLevelMode
    , const void *modulationLevelModeData);

void as3911SetOutputLevels(const struct AS3911OutputLevels *outputLevels);

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Adjust the modulation level.
 *
 * Adjust the modualtion level based on the active modulation level
 * adjustment mode and the modulation level adjustment table.
 *
 * \note This method may block for up to 50 microseconds depending on the active
 * modulation level adjustment mode.
 *****************************************************************************
 */
void as3911AdjustModulationLevel();

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Provide gain table which will be applied when adjusting (modulation) level
 *
 *****************************************************************************
 */
void as3911SetGainTables(const struct AS3911GainTable *gainTableLow, const struct AS3911GainTable *gainTableNorm );
#endif /* AS3911_MODULATION_ADJUSTMENT_H */
