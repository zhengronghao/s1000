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

/*! \file emv_prelayer4.c
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV compliant data transmission prior to activation of the PICC.
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "emv_prelayer4.h"
//
//#include "emv_error_codes.h"
//#include "emv_main.h"
#include "wp30_ctrl.h"

#if (defined(EM_AS3911_Module))
#if defined(EMV_AS3911)
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

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

s8 emvPrelayer4Transceive(const u8 *request, size_t requestLength, u8* response, size_t maxResponseLength,
    size_t *responseLength, u32 timeout, EmvHalTransceiveMode_t transceiveMode)
{
    int numTries = 0;

    while (numTries < 3)
    {
        s8 error = EMV_ERR_OK;

//        if (emvStopRequestReceived())
//            return EMV_ERR_STOPPED;

        error = emvHalTransceive((u8 *)request, requestLength, response, maxResponseLength
            , responseLength, timeout, transceiveMode);

        if (EMV_HAL_ERR_OK == error)
            return EMV_ERR_OK;
        else if (EMV_HAL_ERR_TIMEOUT == error)
            numTries++;
        else
            return EMV_ERR_TRANSMISSION;
    }

    /* Three retries without a proper response are a timeout error. */
    return EMV_ERR_TIMEOUT;
}

#endif
#endif

