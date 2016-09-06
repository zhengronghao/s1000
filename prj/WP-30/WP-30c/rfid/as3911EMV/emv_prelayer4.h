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
 
/*! \file emv_prelayer4.h
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV compliant data transmission prior to activation of the PICC.
 */

#ifndef EMV_PRELAYER4_H
#define EMV_PRELAYER4_H
#if defined(EMV_AS3911)

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "ams_types.h"
//
//#include "emv_hal.h"

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

/*! \ingroup emvCardTechnologySpecific
 *****************************************************************************
 * \brief Send a request to a type A or B card.
 *
 * Send a request to a type A or B card with exception handling according to
 * [CCP v2.01, 9.6 Exception Processing].
 *
 * \param request Pointer to the request.
 * \param requestLength Length of the request in bytes.
 * \param response Pointer to the array storing the response.
 * \param maxResponseLength Maximum length of the response in bytes.
 * \param responseLength Set to the length of the received response in bytes.
 * \param timeout Timeout in carrier cycles.
 * \param transceiveMode EMV IO layer transceive mode to use for the request.
 *
 * \return EMV_ERR_OK: An error free response has been successfully received.
 * \return EMV_ERR_STOPPED: An asynchronous stop request has been received.
 * \return EMV_ERR_TIMEOUT: No card response has been received.
 * \return EMV_ERR_TRANSMISSION: An errornous card response has been received.
 *****************************************************************************
 */
s8 emvPrelayer4Transceive(const u8 *request, size_t requestLength,
        u8 *response, size_t maxResponseLength, size_t *responseLength,
        u32 timeout, EmvHalTransceiveMode_t transceiveMode);

#endif
#endif /* EMV_PRELAYER4_H */

