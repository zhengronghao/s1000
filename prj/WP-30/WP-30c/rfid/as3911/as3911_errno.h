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
 
/*! \file as3911_errno.h
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 module error codes.
 */

#ifndef AS3911_ERRNO_H
#define AS3911_ERRNO_H

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

/*! \name AS3911 error codes
 * This are the error codes used by the AS3911 module and its submodules.
 */

/**@{*/
/*! \ingroup as3911 */
#define AS3911_NO_ERROR                  0 /*!< Operation completed successfully. */
#define AS3911_RECEIVE_ERROR            -1 /*!< Unspecified reception error. Wherever possible a more detailed error code is returned. */
#define AS3911_COLLISION_ERROR          -2 /*!< Bit collision error. */
#define AS3911_CRC_ERROR                -3 /*!< CRC incorrect. */
#define AS3911_PARITY_ERROR             -4 /*!< Parity bit incorrect. */
 /*!
  * \brief Soft framing error.
  *
  * Some frame timing parameters have been violated, but the data has been
  * correctly received.
  */
#define AS3911_SOFT_FRAMING_ERROR       -5
/*!
 * \brief Hard framing error.
 *
 * Some frame timing parameters or signal shape requirements have been violated,
 * and the received data has been corrupted.
 */
#define AS3911_HARD_FRAMING_ERROR       -6
#define AS3911_TIMEOUT_ERROR            -7 /*!< Operation timeout out. */
#define AS3911_OVERFLOW_ERROR           -8 /*!< AS3911 FIFO or internal buffer overflow error. */
#define AS3911_UNKOWN_ERROR             -9 /*!< An unspecified error has occured. */
#define AS3911_INTERNAL_ERROR           -10 /*!< Internal error of the AS3911 software stack. */
#define AS3911_NFC_EVENT                -11 /*!< An NFC event (RF field on/off, collision) occured. */

/**}*/

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

#endif /* AS3911_ERRNO_H */
