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
  
/*! \file emv_poll.h
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV compliant polling and collision detection.
 */

#ifndef EMV_POLL_H
#define EMV_POLL_H

#if defined(EMV_AS3911)
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

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

/*!
 *****************************************************************************
 * \brief Perform a single iteration of the polling loop without field reset.
 *
 * \note This function is needed for some of the EMV L1 certification analog
 * tests and should not be used to implement the polling loop of an EMV
 * compliant terminal.
 *****************************************************************************
 */
void emvPollSingleIteration();

/*!
 *****************************************************************************
 * \brief Perform an EMV polling loop as defined in [CCP, section 9.2].
 *
 * This function polls until the presence of one or more cards
 * is detected or a stop request is received. The polling flags are set
 * according to the detected cards prior to returning.
 *
 * \note This function blocks until at least one card is detected or a
 * stop is requested (emvStopTerminalApplication()).
 *
 * \note emvCollisionDetection() must be called after emvPoll() to
 * perform the collision detection for the cards found.
 *
 * \return EMV_ERR_OK: At least one card is present in the field.
 * \return EMV_ERR_STOPPED: A stop request has been received.
 *****************************************************************************
 */
s16 emvPoll();

/*!
 *****************************************************************************
 * \brief Perform collision detection.
 *
 * \note This function does NOT include the required wait of T_P milliseconds
 * before the first PCD request is send (see [CCP v2.01, PCD 9.3.2.1]). So the
 * caller must assure this wait is performed between the return of emvPoll()
 * and the call to emvCollisionDetection().
 *
 * \param[out] picc The picc struct will be initialized with the uid
 * of the detected card if only one card is present in the field.
 *
 * \return EMV_ERR_OK: No error, a single card has been found and selected.
 * \return EMV_ERR_COLLISION: A collision occured during card singulation.
 * \return EMV_ERR_TIMEOUT: No card found, or a timeout occured at some point
 * during the collision detection.
 *****************************************************************************
 */
s16 emvCollisionDetection(EmvPicc_t *picc);

#endif
#endif /* EMV_POLL_H */

