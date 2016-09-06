/*
 * =====================================================================================
 *
 *       Filename:  as3911_emvGui.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2/27/2013 2:18:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  chenf
 *        Company:  start
 *
 * =====================================================================================
 */

#include "wp30_ctrl.h"

#if (defined(EM_AS3911_Module))
#if defined(EMV_AS3911)

#define EMV_HLTA_FDT   1250

uchar s_emvGuiWupa(uchar * pucResp)
{
    /* EMV TTA L1 WUPA command. */
    size_t numBytesReceived;
    s8 error;
    u8 atqa[2];

    if (!emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: carrier not activated!\r\n");
        return 2;
    }

    emvHalSetStandard(EMV_HAL_TYPE_A);
    emvHalSetErrorHandling(EMV_HAL_PREACTIVATION_ERROR_HANDLING);

    emvDisplayString("EMV: WUPA\r\n");
    error = emvHalTransceive(NULL, 0, &atqa[0], sizeof(atqa), &numBytesReceived, 1800, EMV_HAL_TRANSCEIVE_WUPA);

    if (EMV_HAL_ERR_OK == error)
    {
        emvDisplayString("EMV: ");
        emvDisplayByteArray(&atqa[0], numBytesReceived);
        emvDisplayString("\r\n");
        memcpy(pucResp, atqa, 2);
        return 0;
    }
    else if (EMV_HAL_ERR_TIMEOUT == error)
    {
        emvDisplayString("EMV: timeout error\r\n");
    }
    else if (EMV_HAL_ERR_OVERFLOW == error)
    {
        emvDisplayString("EMV: buffer overflow error\r\n");
    }
    else if (EMV_HAL_ERR_ENCODING == error)
    {
        emvDisplayString("EMV: transmission error\r\n");
    }
    else if (EMV_HAL_ERR_PARAM == error)
    {
        emvDisplayString("EMV: function call parameter error\r\n");
    }
    return 1;
}

void s_emvGuiHaltA(void)
{
    uchar hltaCommand[2] = { EM_mifs_PICC_HALT, 0x00 };
    emvHalTransceive(hltaCommand, sizeof(hltaCommand), 
                     NULL, 0, NULL, EMV_HLTA_FDT, EMV_HAL_TRANSCEIVE_WITH_CRC);
}

uchar s_emvGuiWupb(uchar * pucOutLen, uchar * pucOut)
{
    /* EMV TTA L1 WUPB command. */
    u8 wupb[3];
    u8 atqb[32];
    size_t numBytesReceived;
    s8 error;

    if (!emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: carrier not activated!\r\n");
        return 2;
    }

    emvHalSetStandard(EMV_HAL_TYPE_B);
    emvHalSetErrorHandling(EMV_HAL_PREACTIVATION_ERROR_HANDLING);

    /* Setup WUPB command according to section 6.3.1 of the EMV standard. */
    wupb[0] = EM_mifs_PICC_REQB;
    wupb[1] = 0x00;
    wupb[2] = 0x08;

    emvDisplayString("EMV: WUPB\r\n");

    error = emvHalTransceive(wupb, 3, atqb, sizeof(atqb), &numBytesReceived, EMV_FWT_ATQB_PCD, EMV_HAL_TRANSCEIVE_WITH_CRC);

    if (EMV_HAL_ERR_OK == error)
    {
        emvDisplayString("EMV: ");
        emvDisplayByteArray(&atqb[0], numBytesReceived);
        emvDisplayString("\r\n");
        *pucOutLen = numBytesReceived;
        memcpy(pucOut, atqb, numBytesReceived);
        return 0;
    }
    else if (EMV_HAL_ERR_TIMEOUT == error)
    {
        emvDisplayString("EMV: timeout error\r\n");
    }
    else if (EMV_HAL_ERR_OVERFLOW == error)
    {
        emvDisplayString("EMV: buffer overflow error\r\n");
    }
    else if (EMV_HAL_ERR_ENCODING == error)
    {
        emvDisplayString("EMV: transmission error\r\n");
    }
    else if (EMV_HAL_ERR_PARAM == error)
    {
        emvDisplayString("EMV: function call parameter error\r\n");
    }
    return 1;
}

void emvGuiToggleCarrier()
{
    /* EMV Testpad toggle CARRIER command. */
    if (emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: deactivating carrier\r\n");
        emvHalActivateField(FALSE);
    }
    else
    {
        emvDisplayString("EMV: activating carrier\r\n");
        emvHalActivateField(TRUE);
    }
}

void emvGuiPoll()
{
    if (!emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: carrier not activated!\r\n");
        return;
    }

    emvDisplayString("EMV: single poll\r\n");
    emvPollSingleIteration();
}

void emvGuiReset()
{
    if (!emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: carrier not activated!\r\n");
        return;
    }

    emvDisplayString("EMV: reseting operating field\r\n");
    emvHalResetField();
}

void emvGuiWupa()
{
    /* EMV TTA L1 WUPA command. */
    size_t numBytesReceived;
    s8 error;
    u8 atqa[2];

    if (!emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: carrier not activated!\r\n");
        return;
    }

    emvHalSetStandard(EMV_HAL_TYPE_A);
    emvHalSetErrorHandling(EMV_HAL_PREACTIVATION_ERROR_HANDLING);

    emvDisplayString("EMV: WUPA\r\n");
    error = emvHalTransceive(NULL, 0, &atqa[0], sizeof(atqa), &numBytesReceived, 1800, EMV_HAL_TRANSCEIVE_WUPA);

    if (EMV_HAL_ERR_OK == error)
    {
        emvDisplayString("EMV: ");
        emvDisplayByteArray(&atqa[0], numBytesReceived);
        emvDisplayString("\r\n");
    }
    else if (EMV_HAL_ERR_TIMEOUT == error)
    {
        emvDisplayString("EMV: timeout error\r\n");
    }
    else if (EMV_HAL_ERR_OVERFLOW == error)
    {
        emvDisplayString("EMV: buffer overflow error\r\n");
    }
    else if (EMV_HAL_ERR_ENCODING == error)
    {
        emvDisplayString("EMV: transmission error\r\n");
    }
    else if (EMV_HAL_ERR_PARAM == error)
    {
        emvDisplayString("EMV: function call parameter error\r\n");
    }
}

void emvGuiWupb()
{
    /* EMV TTA L1 WUPB command. */
    u8 wupb[3];
    u8 atqb[32];
    size_t numBytesReceived;
    s8 error;

    if (!emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: carrier not activated!\r\n");
        return;
    }

    emvHalSetStandard(EMV_HAL_TYPE_B);
    emvHalSetErrorHandling(EMV_HAL_PREACTIVATION_ERROR_HANDLING);

    /* Setup WUPB command according to section 6.3.1 of the EMV standard. */
    wupb[0] = 0x05;
    wupb[1] = 0x00;
    wupb[2] = 0x08;

    emvDisplayString("EMV: WUPB\r\n");

    error = emvHalTransceive(wupb, 3, atqb, sizeof(atqb), &numBytesReceived, EMV_FWT_ATQB_PCD, EMV_HAL_TRANSCEIVE_WITH_CRC);

    if (EMV_HAL_ERR_OK == error)
    {
        emvDisplayString("EMV: ");
        emvDisplayByteArray(&atqb[0], numBytesReceived);
        emvDisplayString("\r\n");
    }
    else if (EMV_HAL_ERR_TIMEOUT == error)
    {
        emvDisplayString("EMV: timeout error\r\n");
    }
    else if (EMV_HAL_ERR_OVERFLOW == error)
    {
        emvDisplayString("EMV: buffer overflow error\r\n");
    }
    else if (EMV_HAL_ERR_ENCODING == error)
    {
        emvDisplayString("EMV: transmission error\r\n");
    }
    else if (EMV_HAL_ERR_PARAM == error)
    {
        emvDisplayString("EMV: function call parameter error\r\n");
    }
}

void emvGuiRats()
{
    /* EMV Testpad RATS command. */
    int error = 0;
    EmvPicc_t emvPicc;

    if (!emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: carrier not activated!\r\n");
        return;
    }

    error = emvTypeAAnticollision(&emvPicc);
    if (error != EMV_ERR_OK)
    {
        emvDisplayError(error);
        return;
    }

    emvDisplayString("EMV: card selected ");
    emvDisplayUid(emvPicc.uid, emvPicc.uidLength);
    emvDisplayString("\r\n");

    error = emvTypeAActivation(&emvPicc);
    if (error == EMV_ERR_OK)
        emvDisplayString("EMV: card activated\r\n");
    else
        emvDisplayError(error);
}

void emvGuiAttrib()
{
    /* EMV Testpad ATTRIB command. */
    int error = 0;
    EmvPicc_t emvPicc;

    if (!emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: carrier not activated!\r\n");
        return;
    }

    error = emvTypeBAnticollision(&emvPicc);
    if (error != EMV_ERR_OK)
    {
        emvDisplayError(error);
        return;
    }

    emvDisplayString("EMV: card selected ");
    emvDisplayUid(emvPicc.uid, emvPicc.uidLength);
    emvDisplayString("\r\n");

    error = emvTypeBActivation(&emvPicc);
    if (error == EMV_ERR_OK)
        emvDisplayString("EMV: card activated\r\n");
    else
        emvDisplayError(error);
}

void emvGuiPrevalidation()
{
    /* EMV Testpad pre validation application. */
    s16 error;

    if (!emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: activating carrier\r\n");
        emvHalActivateField(TRUE);
    }

    emvDisplayString("EMV: starting prevalidation application.\r\n");
    error = emvStartTerminalApplication(emvPrevalidationApplication);
    if (error == EMV_ERR_OK)
        emvDisplayString("\r\nEMV: prevalidation application finished.\r\n");
    else if (error == EMV_ERR_STOPPED)
        emvDisplayString("\r\nEMV: prevalidation application stopped.\r\n");
    else
        emvDisplayError(error);
}

void emvGuiDigital()
{
    /* EMV Testpad digital application. */
    s16 error;

    if (!emvHalFieldIsActivated())
    {
        emvDisplayString("EMV: activating carrier\r\n");
        emvHalActivateField(TRUE);
    }

    emvDisplayString("EMV: starting digital application.\r\n");
    error = emvStartTerminalApplication(emvDigitalApplication);
    if (error == EMV_ERR_OK)
        emvDisplayString("\r\nEMV: digital application finished.\r\n");
    else if (error == EMV_ERR_STOPPED)
        emvDisplayString("\r\nEMV: digital application stopped.\r\n");
    else
        emvDisplayError(error);
}

void emvGuiStop()
{
    emvStopTerminalApplication();
}


//void emvGuiDigitalstart()
//{
//    /* EMV Testpad digital application. */
//    s16 error;
//
//    if (!emvHalFieldIsActivated())
//    {
//        emvDisplayString("EMV: activating carrier\r\n");
//        emvHalActivateField(TRUE);
//    }
//
//    emvDisplayString("EMV: starting digital application.\r\n");
//    error = emvStartTerminalApplicationstart(emvDigitalApplication);
//    if (error == EMV_ERR_OK)
//        emvDisplayString("\r\nEMV: digital application finished.\r\n");
//    else if (error == EMV_ERR_STOPPED)
//        emvDisplayString("\r\nEMV: digital application stopped.\r\n");
//    else
//        emvDisplayError(error);
//}

#endif
#endif

