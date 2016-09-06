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
 
/*! \file emv_picc.h
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV PICC abstraction.
 */

#ifndef EMV_PICC_H
#define EMV_PICC_H

#if defined(EMV_AS3911)
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

/*! \ingroup emvMain
 *****************************************************************************
 * \brief Representaiton of a PICC for the EMV software stack.
 * 
 * Representation of a PICC for the EMV software stack. The variables \a uid
 * and \a uidLength contain valid data after successful anticollision
 * (emvCollisionDetection()). Other data members are only guaranteed to hold
 * valid data after successful activation (emvActivate()).
 *****************************************************************************
 */
typedef struct EmvPicc_s
{
    u8 uid[10]; /*!< UID or PUPI of the PICC. */
    u8 uidLength; /*!< Length of the UID/PUPI in bytes. */
    u8 fwi; /*!< Frame wait integer of the PICC. */
    u8 fsci; /*!< Frame size integer of the PICC. */
    u8 sfgi; /*!< Special frame guard time of the PICC. */
    u8 dPiccPcd; /*!< Datarate bits PICC->PCD. */
    u8 dPcdPicc; /*!< Datarate bits PCD->PICC. */

    s16 (*activate)(struct EmvPicc_s *picc); /*!< Activation function callback. */
    s16 (*remove)(struct EmvPicc_s *picc); /*!< Card removal function callback. */
} EmvPicc_t;

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
 * \brief Activate the PICC.
 *
 * \param[in] picc The PICC to activate.
 *
 * \return EMV_ERR_OK: No error, operation successfull.
 * \return EMV_ERR_STOPPED: A stop request was received during activation.
 * \return EMV_ERR_TIMEOUT: Timeout during PICC activation.
 * \return EMV_ERR_TRANSMISSION: Transmission error during PICC activation.
 * \return EMV_ERR_PROTOCOL: Protocol error during PICC activation.
 *****************************************************************************
 */
s16 emvActivate(EmvPicc_t *picc);


/*! \ingroup emvMain
 *****************************************************************************
 * \brief Remove the PICC.
 *
 * \note This function blocks until the PICC is physically removed from the
 * reader field or a stop request is received via emvStopTerminalApplication().
 *
 * \param[in] picc The PICC to remove.
 *
 * \return EMV_ERR_OK: No error, operation successfull.
 * \return EMV_ERR_STOPPED: A stop request was received during activation.
 *****************************************************************************
 */
s16 emvRemove(EmvPicc_t *picc);

#endif
#endif /* EMV_PICC_H */

