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
 * PROJECT: AS911 firmware
 * $Revision: $
 * LANGUAGE: ANSI C
 */

/*! \file emv_main.c
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV terminal application.
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include <stddef.h>

//#include "usb_hid_stream_appl_handler.h"
//
//#include "emv_hal.h"
//#include "emv_standard.h"
//#include "emv_poll.h"
//#include "emv_layer4.h"
//#include "emv_display.h"
//#include "sleep.h"
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
//#define as3911_emv_debug
#ifdef as3911_emv_debug
#define EMV_DIS(...)    Dprintk(__VA_ARGS__)
#else
#define EMV_DIS(...)    {}
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

/*! Indicates whether a stop request has been received from the GUI or not. */
static volatile bool_t emvStopRequestReceivedFlag = FALSE;

RFID_EMV_DEBUGFLG gemvdebugflg; 

RFID_EMV_CARDINFO gemvcardinfo;
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

/*!
 *****************************************************************************
 * \brief Convert 13.56 MHz carrier cycle numbers to milliseconds.
 *
 * \note \a numCarrierCycles must be <= 888720133 (65535 ms).
 *
 * \param numCarrierCycles Number of carrier cycles.

 * \return \a numCarrierCycles converted to milliseconds.
 *****************************************************************************
 */
static u16 emvConvertCarrierCyclesToMilliseconds(u32 numCarrierCycles);

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
void displayCardinfo(void);

s16 emvStartTerminalApplication(s16 (*application)(void))
{
    s16 error = EMV_ERR_OK;
    s16 pollret = EMV_ERR_OK;
    s16 acollret = EMV_ERR_OK;
    s16 activeret = EMV_ERR_OK;
    s16 apduret = EMV_ERR_OK;
    s16 removeret = EMV_ERR_OK;

    /* Reset stop request received flag. */
    emvStopRequestReceivedFlag = FALSE;

    guiDebugi = 0;
    gemvdebugflg.polldeflg = 0;
    gemvdebugflg.acolldeflg = 0;
    gemvdebugflg.activedeflg = 0;
    gemvdebugflg.apdudeflg = 0;
    /* Implementation:
     * Error checking is done at the beginning of the while loop because
     * error handling is the same for all stages of the terminal main loop.
     * If any stage encounters an error it uses a continue statement to fall
     * through the rest of the while loop and reach the error handling code.
     */
    while(1)
    {
        EmvPicc_t picc;
        if(kb_hit())//cf20140423
        {
            return EMV_ERR_STOPPED;
        }    
        if ( IfInkey(0)) {

//            EMV_DIS("\r\n----press 99-exit 1-debug\r\n");
//            removeret = InkeyCount(0);
//            if ( removeret == 99 ) {
//                /* Received stop request, stop terminal main loop. */
//                return EMV_ERR_STOPPED;
//            }else if (removeret == 1){
//                displayCardinfo();
//                TRACE("\r\n\r\n -----111-debug information-------");
//                //            DISPBUF(gcDebugBuf, guiDebugi, 0);
//                vDispBufKey(guiDebugi,gcDebugBuf);
//            }
//            guiDebugi = 0;
            return EMV_ERR_STOPPED;
        }
        if (EMV_ERR_STOPPED == error)
        {
            /* Received stop request, stop terminal main loop. */
//            EMV_DIS("\r\n----press 99-exit 1-debug\r\n");
//            removeret = InkeyCount(0);
//            if ( removeret == 99 ) {
//                return EMV_ERR_STOPPED;
//            }else if ( removeret == 1 ){
//                displayCardinfo();
//                TRACE("\r\n\r\n ----222--debug information-------");
//                //            DISPBUF(gcDebugBuf, guiDebugi, 0);
//                vDispBufKey(guiDebugi,gcDebugBuf);
//            }
//            guiDebugi = 0;
            return EMV_ERR_STOPPED;
        }
        if (EMV_ERR_OK != error)
        {
            //debug信息影响TB404_4 TB411_4复位时间(max 33000us)
            EMV_DIS("\r\n pollret:%d\t acollret:%d\t activeret:%d\t apduret:%d\t removeret:%d\r\n"
                    ,pollret, acollret, activeret, apduret, removeret);

            //必须加调试信息否则影响TA305_3 出现奇怪错误待查 09022013 chenf
            emvDisplayError(error);

            if (EMV_ERR_POWEROFF_REQ == error)
            {
                emvHalActivateField(FALSE);

                emvHalSleepMilliseconds(EMV_T_POWEROFF);
                /* Falling through to emvHalResetField below */
            }

            /* Reset field and continue with polling. */
            emvHalResetField();

            pollret = EMV_ERR_OK;
            acollret = EMV_ERR_OK;
            activeret = EMV_ERR_OK;
            apduret = EMV_ERR_OK;
            removeret = EMV_ERR_OK;

//            displayCardinfo();
//
//            TRACE("\r\n\r\n ------debug information-------");
////            DISPBUF(gcDebugBuf, guiDebugi, 0);
//            vDispBufKey(guiDebugi,gcDebugBuf);
//            guiDebugi = 0;
//            InkeyCount(0);
        }

        /* Polling. */
        emvDisplayMessage(EMV_M_POLLING);
guiDebugFlg = gemvdebugflg.polldeflg;
        error = emvPoll();
//        TRACE("\r\n poll:%d",error);
        pollret = error;
guiDebugFlg = 0;
        if (EMV_ERR_OK != error)
            continue;

//        Dprintk("\r\n--acollision\r\n");
        /* Anticollision. */
        sleepMilliseconds(EMV_T_P);
guiDebugFlg = gemvdebugflg.acolldeflg;
        error = emvCollisionDetection(&picc);
//        TRACE("\r\n coll:%d",error);
//        s_DelayUs(300);//TB305_03 原因可能太早下电20141027 chenf
        acollret = error;
guiDebugFlg = 0;
        if (EMV_ERR_OK != error)
            continue;

        /* Activation. */
guiDebugFlg = gemvdebugflg.activedeflg;
        error = emvActivate(&picc);
//        TRACE("\r\n active:%d",error);
        activeret = error;
guiDebugFlg = 0;
        if (EMV_ERR_OK != error)
            continue;

        /* Wait for SFGT. */
        /* test sfgt real time
         * sfgtMilliseconds         timer(ms)           standard(ms)    1/fc
         *      0                   0.33(not use)       0.5             4480                    
         *      1                   1.01                0.66            8960
         *      2                   2.01                1.321           17920
         *      3                   3.03                2.643           35840
         *      4                   6.00                5.286           71680
         *      5                   11.00               10.572          143360
         *      6                   22.10               21.144          286720
         *      7                   43.10               42.289          573440
         *      8                   84.90               84.578          1146880
         *      9                   170.00              169.156         2293760
         *      10                  339.00              338.321         4587520
         *      11                  677.00              676.625         9175040
         *      12                  1350.00             1353.250        18350080
         *      13                  2710.00             2706.501        36700160
         *      14                  5411.00(warning)    5413.002        73400320
         */
        if(picc.sfgi > 0)
        {
#if 11
            /* EMV_DELTA_FWT_PCD needs to be added and not shifted by FWI
             * See: Table A.5 Annex A.4, change request 10.3.5.5 and 10.3.5.8 */
            u32 sfgtCycles = EMV_CONVERT_SFGT_TO_CARRIER_CYCLES(picc.sfgi);
            u16 sfgtMilliseconds = emvConvertCarrierCyclesToMilliseconds(sfgtCycles);
            sleepMilliseconds(sfgtMilliseconds);
#else
            u32 sfgtCycles = (4096UL + 384) << picc.sfgi;
            u16 sfgtMilliseconds = emvConvertCarrierCyclesToMilliseconds(sfgtCycles);
            sleepMilliseconds(sfgtMilliseconds);
            s_DelayUs(500);//+500us TB105_8 09062013 chenf

            if(gemvcardinfo.cardtype == 1)//TA105_11 - TA105_15
            {    
                if(picc.sfgi > 9)
                {
                    sys_delay_ms(70);
                }    
                if(picc.sfgi == 9)
                {
                    sys_delay_ms(1);
                }    
            }
#endif
        }

        /* Initialize layer 4. */
        error = emvInitLayer4(&picc);
        if (EMV_ERR_OK != error)
            continue;

guiDebugFlg = gemvdebugflg.apdudeflg;
        /* Start terminal application. */
        if (application != NULL)
            error = application();
        else
            error = EMV_ERR_OK;
        apduret = error;
//        TRACE("\r\n apdu:%d",error);
guiDebugFlg = 0;

        if (EMV_ERR_OK != error)
            continue;

        /* Card removal. */
        emvDisplayMessage(EMV_M_REMOVE_CARD);
        error = emvRemove(&picc);
//        TRACE("\r\n remove:%d",error);
        removeret = error;
        if (EMV_ERR_OK != error)
            continue;
    }
}

bool_t emvStopRequestReceived()
{
//    ProcessIO();

    if (emvStopRequestReceivedFlag)
    {
        emvStopRequestReceivedFlag = FALSE;
        return TRUE;
    }

    return FALSE;
}

void emvStopTerminalApplication()
{
    emvStopRequestReceivedFlag = TRUE;
}


/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

static u16 emvConvertCarrierCyclesToMilliseconds(u32 num_cycles)
{
    return (num_cycles / 13560) + 1;
}


//s16 emvStartTerminalApplicationstart(s16 (*application)(void))
//{
//    s16 error = EMV_ERR_OK;
//    s16 pollret = EMV_ERR_OK;
//    s16 acollret = EMV_ERR_OK;
//    s16 activeret = EMV_ERR_OK;
//    s16 apduret = EMV_ERR_OK;
//    s16 removeret = EMV_ERR_OK;
//
//    /* Reset stop request received flag. */
//    emvStopRequestReceivedFlag = FALSE;
//
//    /* Implementation:
//     * Error checking is done at the beginning of the while loop because
//     * error handling is the same for all stages of the terminal main loop.
//     * If any stage encounters an error it uses a continue statement to fall
//     * through the rest of the while loop and reach the error handling code.
//     */
//    while(1)
//    {
//        EmvPicc_t picc;
//
//        if ( IfInkey(0)) {
//            EMV_DIS("\r\n----press 99 to exit\r\n");
//            if ( InkeyCount(0) == 99 ) {
//                /* Received stop request, stop terminal main loop. */
//                return EMV_ERR_STOPPED;
//            }
//        }
//        if (EMV_ERR_STOPPED == error)
//        {
//            /* Received stop request, stop terminal main loop. */
//            return EMV_ERR_STOPPED;
//        }
//        if (EMV_ERR_OK != error)
//        {
//            EMV_DIS("\r\n pollret:%d\t acollret:%d\t activeret:%d\t apduret:%d\t removeret:%d\r\n"
//                    ,pollret, acollret, activeret, apduret, removeret);
//            emvDisplayError(error);
//
//            /* Reset field and continue with polling. */
//            emvHalResetField();
//
////                DISPBUF(gucDebugBuf, guiDebugS3, 0);
////                vDispBufKey(guiDebugS3,gucDebugBuf);
////                guiDebugS3 = 0;
////                
////                InkeyCount(0);
//        }
//
//        /* Polling. */
////        emvDisplayMessage(EMV_M_POLLING);
//guiDebugFlg = gemvdebugflg.polldeflg;
//        error = emvPollstart();
//        pollret = error;
//guiDebugFlg = 0;
//        if (EMV_ERR_OK != error)
//            continue;
//
////        TRACE("\r\n--acollision");
//        /* Anticollision. */
//        sleepMilliseconds(EMV_T_P);
//guiDebugFlg = gemvdebugflg.acolldeflg;
//        error = emvCollisionDetection(&picc);
//        acollret = error;
//guiDebugFlg = 0;
//        if (EMV_ERR_OK != error)
//            continue;
//
//        /* Activation. */
//guiDebugFlg = gemvdebugflg.activedeflg;
////        TRACE("\r\n--active");
////        DISPBUF(picc.uid, picc.uidLength, 0);
//        error = emvActivate(&picc);
//        activeret = error;
//guiDebugFlg = 0;
//        if (EMV_ERR_OK != error)
//            continue;
//
//        /* Wait for SFGT. */
//        /* test sfgt real time
//         * sfgtMilliseconds         timer(ms)           standard(ms)    1/fc
//         *      0                   0.33(not use)       0.5             4480                    
//         *      1                   1.01                0.66            8960
//         *      2                   2.01                1.321           17920
//         *      3                   3.03                2.643           35840
//         *      4                   6.00                5.286           71680
//         *      5                   11.00               10.572          143360
//         *      6                   22.10               21.144          286720
//         *      7                   43.10               42.289          573440
//         *      8                   84.90               84.578          1146880
//         *      9                   170.00              169.156         2293760
//         *      10                  339.00              338.321         4587520
//         *      11                  677.00              676.625         9175040
//         *      12                  1350.00             1353.250        18350080
//         *      13                  2710.00             2706.501        36700160
//         *      14                  5411.00(warning)    5413.002        73400320
//         */
//        if(picc.sfgi > 0)
//        {
//            u32 sfgtCycles = (4096UL + 384) << picc.sfgi;
//            u16 sfgtMilliseconds = emvConvertCarrierCyclesToMilliseconds(sfgtCycles);
//            sleepMilliseconds(sfgtMilliseconds);
//        }
//
//        /* Initialize layer 4. */
//        error = emvInitLayer4(&picc);
//        if (EMV_ERR_OK != error)
//            continue;
//
//guiDebugFlg = gemvdebugflg.apdudeflg;
//        /* Start terminal application. */
////        TRACE("\r\n--aptu");
//        if (application != NULL)
//            error = application();
//        else
//            error = EMV_ERR_OK;
//        apduret = error;
//guiDebugFlg = 0;
//
//        if (EMV_ERR_OK != error)
//            continue;
//
//        /* Card removal. */
//        emvDisplayMessage(EMV_M_REMOVE_CARD);
//        error = emvRemove(&picc);
//        removeret = error;
//        if (EMV_ERR_OK != error)
//            continue;
//    }
//}

#endif
#endif

