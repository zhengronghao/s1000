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
 
/*! \file emv_layer4.h
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV compliant ISO14443-4 data transmission.
 */

#ifndef EMV_LAYER4_H
#define EMV_LAYER4_H

#if defined(EMV_AS3911)
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include <stddef.h>
//
//#include "ams_types.h"
//
//#include "emv_picc.h"
//#include "emv_error_codes.h"

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

/*! \ingroup emvMain
 *****************************************************************************
 * \brief Initializes the ISO14443-4 protocol layer (EMV layer 4).
 *
 * Resets the ISO14443-4 protocoll stack and initializes it for communicaiton
 * with a specific card.
 *
 * \note The PICC must be in the active state for layer-4 data transmissions
 * to work.
 *
 * \param[in] picc The target PICC for ISO14443-4 data transmissions.
 *
 * \return EMV_ERR_OK: No error, operation successful.
 *****************************************************************************
 */
s16 emvInitLayer4(EmvPicc_t *picc);

/*! \ingroup emvMain
 *****************************************************************************
 * \brief Transeive an APDU.
 *
 * Send a command APDI and receives the response APDU from the PICC.
 *
 * \param[in] apdu Command APDU.
 * \param[in] apduLength Length of the command APDI in bytes.
 * \param[out] response Buffer for the response APDU.
 * \param[in] maxResponseLength Size of the response APDI buffer in bytes.
 * \param[out] responseLength The length of the received response APDU in
 * bytes.
 *
 * \return EMV_ERR_OK: No error, response APDU received.
 * \return EMV_ERR_PROTOCOL: Protocl error during reception of the response
 * APDU.
 * \return EMV_ERR_TRANSMISSION: Transmission error during reception of the
 * response APDU.
 * \return EMV_ERR_TIMEOUT: No response APDU receied, or a timeout occured during
 * reception of the response APDU.
 * \return EMV_ERR_INTERNAL: Internal buffer overflow during reception of the
 * response APDU.
 *****************************************************************************
 */
s16 emvTransceiveApdu(const u8 *apdu, size_t apduLength, u8 *response,
        size_t maxResponseLength, size_t *responseLength);

#endif
#endif /* EMV_LAYER4_H */

