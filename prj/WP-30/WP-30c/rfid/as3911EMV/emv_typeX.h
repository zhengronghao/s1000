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
 
/*! \file emv_doc.h
 *
 * \author Oliver Regenfelder
 *
 * \brief Contains the doxygen documentation for the EMV card technology
 * specific submodule.
 */

#ifndef EMV_TYPEX_H
#define EMV_TYPEX_H

/*! \defgroup emvCardTechnologySpecific EMV Card Technology Specific Functions
 *****************************************************************************
 * \ingroup emv
 * 
 * \brief This part contains RFID card operations which are in general
 * available on every card technology but the implementation of which depends
 * on the card technology (ISO14443-A or ISO14443-B) in use.
 *
 * This module contains RFID operations which are typically available on
 * ISO14443-A and ISO14443-B cards. But, due to differences between these
 * two card technologies it is not possible to provide a single implementation
 * for both technologies.
 * 
 * This operations include:
 * - Presence detection: Used by the polling loop to detect the presence
 * of cards from a specific technology (emvTypeACardPresent(),
 * emvTypeBCardPresent()).
 * - Singulation: Used after cars of one technology only have been found
 * to ensure that only one card of that technology is present in the reader
 * field (emvTypeAAnticollision(), emvTypeBAnticollision()).
 * - Card activation: Used to activate the ISO14443-4 layer on an already
 * singulated card (emvTypeAActivation(), emvTypeBActivation).
 * - Card removal: Used to detect phydical removal of the card from the
 * reader field after a payment transaction is completed
 * (emvTypeARemoval(), emvTypeBRemoval()).
 *
 * The activation and removal operations can also be used via the callbacks
 * provided by ::EmvPicc_t after anticollision has been completed successfully.
 *
 * Additionally, this module includes a function to communicate with a
 * type A or type B card with error handling as required by the EMV specifciaton
 * prior to card activation (emvPrelayer4Transceive()).
 *****************************************************************************
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

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

#endif /* EMV_TYPEX_H */
