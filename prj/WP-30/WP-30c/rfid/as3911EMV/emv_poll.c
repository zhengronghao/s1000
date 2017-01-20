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

/*! \file emv_poll.c
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV compliant polling and collision detection.
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include <stddef.h>
//
//#include "emv_poll.h"
//#include "emv_hal.h"
//#include "emv_standard.h"
//#include "emv_main.h"
//#include "emv_typeA.h"
//#include "emv_typeB.h"
#include "wp30_ctrl.h"

#if (defined(EM_AS3911_Module))
#if defined(EMV_AS3911)
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*! Frame delay time (timeout) used for ISO14443-A HLTA commands. */
#define EMV_HLTA_FDT   1250

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

/*! ISO14443-A cards polling flag as recommended by the EMV standard. */
static int emvTypeA;

/*! ISO14443-B cards polling flag as recommended by the EMV standard. */
static int emvTypeB;

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

void emvPollSingleIteration()
{
    emvTypeA = 0;
    emvTypeB = 0;

    emvHalSleepMilliseconds(EMV_T_P);
    if (emvTypeACardPresent())
    {
        /* Send HLTA command. */
        u8 hltaCommand[2] = { 0x50, 0x00 };
        /* ISO14443-A card(s) found. */
        emvTypeA = 1;
        emvHalTransceive(hltaCommand, sizeof(hltaCommand), NULL, 0, NULL, EMV_HLTA_FDT, EMV_HAL_TRANSCEIVE_WITH_CRC);
    }

    emvHalSleepMilliseconds(EMV_T_P);
    if (emvTypeBCardPresent())
    {
        /* ISO14443-B card(s) found. */
        emvTypeB = 1;
    }
}

s16 emvPoll()
{
    emvTypeA = 0;
    emvTypeB = 0;
    uchar tem[16];
    size_t temlen = 0;

    /* Poll as long as no cards are found. */
    while (1)
    {
        if(kb_hit())//cf20140423
        {
            return EMV_ERR_STOPPED;
        }
        if ( IfInkey(0)) {
//            Dprintk("\r\n\r\n ----poll 99 to exit-------");
            return EMV_ERR_STOPPED;
//            if ( InkeyCount(0) == 99 ) {
//                /* Received stop request, stop terminal main loop. */
//                return EMV_ERR_STOPPED;
//            }
        }

        if (emvStopRequestReceived())
            return EMV_ERR_STOPPED;

        if (emvTypeA != 0){
//            TRACE("\r\n--type a");
            break;
        }

        /* Wait for t_p. */
        emvHalSleepMilliseconds(EMV_T_P);

        //guiDebugFlg = 1;
        //guiDebugS3 = 0;
        if (emvTypeACardPresent())
        {
            /* Send HLTA command. */
            u8 hltaCommand[2] = { 0x50, 0x00 };
            /* ISO14443-A card(s) found. */
            emvTypeA = 1;
            //gemvdebugflg.emvproflg0 = 1;
//            emvHalTransceive(hltaCommand, sizeof(hltaCommand), NULL, 0, NULL, EMV_HLTA_FDT, EMV_HAL_TRANSCEIVE_WITH_CRC);
//            if ( emvTypeB == 0 ) {
                //TB303
                emvHalTransceive(hltaCommand, sizeof(hltaCommand), tem, 0, &temlen, EMV_HLTA_FDT, EMV_HAL_TRANSCEIVE_WITH_CRC);
//            }
            //gemvdebugflg.emvproflg0 = 0;
        }

//guiDebugFlg = 0;

        if (emvTypeB != 0){
//            TRACE("\r\n--type b");
            break;
        }

//        SETSIGNAL2_H();
        /* Wait for t_p. */
        emvHalSleepMilliseconds (EMV_T_P);

        if (emvTypeBCardPresent())
        {
            /* ISO14443-B card(s) found. */
            emvTypeB = 1;
        }
//        SETSIGNAL2_L();
    }

    return EMV_ERR_OK;
}

s16 emvCollisionDetection(EmvPicc_t *picc)
{
    s16 ret;

    if ((emvTypeA) != 0 && (emvTypeB != 0))
        return EMV_ERR_COLLISION;

    if (emvTypeA != 0)
    {
        ret = emvTypeAAnticollision(picc);
        if ( ret == EMV_ERR_OK) {
            gemvcardinfo.uidLength = picc->uidLength;
            memcpy(gemvcardinfo.uid, picc->uid, gemvcardinfo.uidLength);
        }
        return ret;
    }
    else if (emvTypeB != 0)
    {
        ret = emvTypeBAnticollision(picc);
//        if ( ret == EMV_ERR_OK) {
//
//        }
        return ret;
    }
    else
        return EMV_ERR_COLLISION;
}

//s16 emvPollstart()
//{
//    emvTypeA = 0;
//    emvTypeB = 0;
//    uchar tem[16];
//    int temlen = 0;
//
//    /* Poll as long as no cards are found. */
//    while (1)
//    {
//        if ( IfInkey(0)) {
//            if ( InkeyCount(0) == 99 ) {
//                /* Received stop request, stop terminal main loop. */
//                return EMV_ERR_STOPPED;
//            }
//        }
//
////        if (emvStopRequestReceived())
////            return EMV_ERR_STOPPED;
//
//        if (emvTypeA != 0)
//        {
////            TRACE("\r\n--type a");
//            break;
//        }
//
//        /* Wait for t_p. */
//        emvHalSleepMilliseconds(EMV_T_P);
//
////guiDebugFlg = 1;
////guiDebugS3 = 0;
//        if (emvTypeACardPresent())
//        {
//            /* ISO14443-A card(s) found. */
//            emvTypeA = 1;
//
//            /* Send HLTA command. */
//            u8 hltaCommand[2] = { 0x50, 0x00 };
////gemvdebugflg.emvproflg0 = 1;
//////            emvHalTransceive(hltaCommand, sizeof(hltaCommand), NULL, 0, NULL, EMV_HLTA_FDT, EMV_HAL_TRANSCEIVE_WITH_CRC);
//            emvHalTransceive(hltaCommand, sizeof(hltaCommand), tem, 0, &temlen, EMV_HLTA_FDT, EMV_HAL_TRANSCEIVE_WITH_CRC);
////gemvdebugflg.emvproflg0 = 0;
//        }
//
////guiDebugFlg = 0;
//
//        if (emvTypeB != 0)
//        {
////            TRACE("\r\n--type b");
////
////            TRACE("\r\n11111--type b");
////            sys_beep();
////            InkeyCount(0);
//            break;
//        }
//
//        /* Wait for t_p. */
//        emvHalSleepMilliseconds (EMV_T_P);
//
//        if (emvTypeBCardPresent())
//        {
//            /* ISO14443-B card(s) found. */
//            emvTypeB = 1;
//        }
//    }
//
//    return EMV_ERR_OK;
//}
/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

#endif
#endif

