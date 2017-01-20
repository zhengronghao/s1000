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

/*! \file emv_display.c
 *
 * \author Oliver Regenfelder
 *
 * \brief Methods to display EMV data on the GUI.
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "emv_display.h"
//
//#include <stddef.h>
//
//#include "logger.h"
//
//#include "emv_layer4.h"
//#include "emv_response_buffer.h"

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

//#define as3911_debug
#ifdef as3911_debug
#define EMV_LOG(...)    Dprintk(__VA_ARGS__)
#else
#define EMV_LOG(...)    {}
#endif

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

void emvDisplayString(const char *string)
{
    EMV_LOG("%s", string);
}

void emvDisplayByteArray(const u8 *array, size_t length)
{
    size_t index = 0;
    EMV_LOG("\r\n");
    for (index = 0; index < length; index++)
        EMV_LOG("\t %02hhx", array[index]);
    EMV_LOG("\r\n");
}

void emvDisplayUid(const u8 *uid, size_t length)
{
    size_t index = length - 1;

    if (length == 0)
    {
        EMV_LOG("UID length is zero");
        return;
    }
    
    do
    {
        EMV_LOG("%02hhx ", uid[index]);
        index--;
    } while (index > 0);
}

void emvDisplayError(s16 errorCode)
{
    switch (errorCode)
    {
    case EMV_ERR_OK:
//        EMV_LOG("EMV: no error\r\n");
        EMV_LOG("no error\r\n");
        break;
    case EMV_ERR_COLLISION:
//        EMV_LOG("EMV: collision error\r\n");
        EMV_LOG("collision error\r\n");
        break;
    case EMV_ERR_PROTOCOL:
//        EMV_LOG("EMV: protocol error\r\n");
        EMV_LOG("protocol error\r\n");
        break;
    case EMV_ERR_TRANSMISSION:
//        EMV_LOG("EMV: transmission error\r\n");
        EMV_LOG("transmission error\r\n");
        break;
    case EMV_ERR_TIMEOUT:
//        EMV_LOG("EMV: timeout error\r\n");
        EMV_LOG("timeout error\r\n");
        break;
    case EMV_ERR_INTERNAL:
//        EMV_LOG("EMV: internal error\r\n");
        EMV_LOG("internal error\r\n");
        break;
    case EMV_ERR_STOPPED:
//        EMV_LOG("EMV: stopped error\r\n");
        EMV_LOG("stopped error\r\n");
        break;
    case EMV_ERR_POWEROFF_REQ:
//        EMV_LOG("EMV: power off requested\n");
        EMV_LOG("power off requested\n");
        break;
    default:
        EMV_LOG("EMV: unkown error code\r\n");
    }
}

void emvDisplayMessage(s16 messageCode)
{
    switch (messageCode)
    {
    case EMV_M_POLLING:
//        EMV_LOG("EMV: Polling ...\r\n");
        EMV_LOG("Polling\r\n");
        break;
    case EMV_M_REMOVE_CARD:
//        EMV_LOG("EMV: Remove card ...\r\n");
        EMV_LOG("Remove card ...\r\n");
        break;
    default:
        EMV_LOG("EMV: unkown message code\r\n");
    }
}

void emvDisplayCAPDU(const u8 *apdu, size_t length)
{
    emvDisplayString("EMV: C-APDU ");
    emvDisplayByteArray(apdu, length);
    emvDisplayString("\r\n");
}

void emvDisplayRAPDU(const u8 *apdu, size_t length)
{
    emvDisplayString("EMV: R-APDU ");
    emvDisplayByteArray(apdu, length);
    emvDisplayString("\r\n");
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

#endif
#endif

