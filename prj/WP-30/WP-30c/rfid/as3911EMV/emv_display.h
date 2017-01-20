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
  
/*! \file emv_display.h
 *
 * \author Oliver Regenfelder
 *
 * \brief Methods to display EMV data on the GUI.
 */

#ifndef EMV_DISPLAY_H
#define EMV_DISPLAY_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "ams_types.h"
#include "wp30_ctrl.h"

//#define as3911_debug
#ifdef as3911_debug
#define emvPrintf(...)    Dprintk(__VA_ARGS__)
#else
#define emvPrintf(...)    {}
#endif

#if defined(EMV_AS3911)
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/** EMV message code for the polling status message */
#define EMV_M_POLLING       1
/** EMV message code for the remove card status message */
#define EMV_M_REMOVE_CARD   2

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
 * \brief Display a C string on the GUI.
 *
 * \param string Pointer to a C string.
 *****************************************************************************
 */
void emvDisplayString(const char *string);

/*!
 *****************************************************************************
 * \brief Display an array of byte values.
 *
 * \param array Pointer to the array.
 * \param length Length of the array in bytes.
 *****************************************************************************
 */
void emvDisplayByteArray(const u8 *array, size_t length);

/*!
 *****************************************************************************
 * \brief Display a card UID (or PUPI) on the GUI.
 *
 * Card UIDs are displayed as byte arrays with the most significant byte
 * first.
 *
 * \param uid Pointer to the UID.
 * \param length Length of the UID in bytes.
 *****************************************************************************
 */
void emvDisplayUid(const u8 *uid, size_t length);

/*!
 *****************************************************************************
 * \brief Display an EMV error code converted to readable text on the GUI.
 *
 * \param errorCode EMV error code defined in emv_error_codes.h.
 *****************************************************************************
 */
void emvDisplayError(s16 errorCode);

/*!
 *****************************************************************************
 * \brief Display a standardized EMV Message on the GUI.
 *
 * \param messageCode Message code of the message to display on the GUI.
 *****************************************************************************
 */
void emvDisplayMessage(s16 messageCode);

/*!
 *****************************************************************************
 * \brief Display a command APDU on the GUI.
 *
 * \param apdu Command APDU to display.
 * \param length Length of the command APDU in bytes.
 *****************************************************************************
 */
void emvDisplayCAPDU(const u8 *apdu, size_t length);

/*!
 *****************************************************************************
 * \brief Display a response APDU on the GUI.
 *
 * \param apdu Response APDU to display.
 * \param length Length of the response APDU in bytes.
 *****************************************************************************
 */
void emvDisplayRAPDU(const u8 *apdu, size_t length);

#endif
#endif /* EMV_DISPLAY_H */

