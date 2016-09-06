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
 
/*! \file emv_error_codes.h
 *
 * \author Oliver Regenfelder
 *
 * \brief Error codes specific to the EMV code.
 */

#ifndef EMV_ERROR_CODES_H
#define EMV_ERROR_CODES_H

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

/*! \name EMV module error codes
 *****************************************************************************
 * Error codes used by the EMV module.
 *****************************************************************************
 */
/**@{*/
/*! \ingroup emv
 */
#define EMV_ERR_OK               0 /*!< No error occured. */
#define EMV_ERR_COLLISION       -1 /*!< Received a card response with a bit collision. */
#define EMV_ERR_PROTOCOL        -2 /*!< Received a card response with a protocol error. */
#define EMV_ERR_TRANSMISSION    -3 /*!< Received a card response with a transmission error. */
#define EMV_ERR_TIMEOUT         -4 /*!< Timeout occured while waiting for a card response. */
#define EMV_ERR_INTERNAL        -5 /*!< EMV software stack internal error. */
#define EMV_ERR_STOPPED         -6 /*!< Stop current operation request received. */
/**@}*/

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

#endif /* EMV_ERROR_CODES_H */
