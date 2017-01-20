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

/*! \file as3911_com.c
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 RFID communication
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "as3911_com.h"
//
//#include "aS3911.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

#include "wp30_ctrl.h"

#if (defined(EM_AS3911_Module))
/*! Sanity timeout during an AS3911 transmit operation (milliseconds).
 * If no interrupt is signaled by the AS3911 within this timeframe during
 * a tranmsit operation then a severe software or hardware failure has to be
 * asumed.
 */
#define AS3911_TX_IRQ_SANITY_TIMEOUT        100

/*! Sanity timeout during an AS3911 receive operation (milliseconds).
 * If no interrupt is signaled by the AS3911 within this timeframe during
 * a receive operation then a severe software or hardware failure has to be
 * asumed.
 */
#define AS3911_RX_IRQ_SANITY_TIMEOUT        6000

/*! Sanity timeout during AS3911 receive operation if rxs was already detected.
 * After 10ms@106kbps we must have more than filled the FIFO and water level
 * must have been signaled
 */ 
#define AS3911_RX_IRQ_SANITY_TIMEOUT_AFTER_RXS        50

/*! Time offset between the start of a PICC response and the start of
 * receive interrupt (carrier cycles).
 */
#define AS3911_SOR_IRQ_OFFSET               640

/*! Maximum size of a continuous FIFO read operation inside the irq handler.
 * Limiting the fifo read size is necessary to enable proper handling of
 * EMV transmission error recovery. A complete FIFO read time would take
 * to long and exceed the maximum response time to a transmission error
 * as defined by the EMV standard.
 */
#define AS3911_IRQ_FIFO_READ_CHUNK_SIZE     10

/*! Bitmask for the rxon bit of the auxiliary display register of the AS3911. */
#define AS3911_AUX_DISPLAY_RXON_BIT         0x08

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

/*! Minimum delay between a received PICC message and the next PCD message (carrier cycles). */
static u32 as3911FrameDelayTime = 0;

/*! Indicates whether a reception is currently in progress (TRUE) or not (FALSE). */
static volatile uchar as3911ReceptionInProgress = OFF;

/*! \ToDo: Remove this variable. It is no longer used.
 */
static volatile uchar as3911TransmissionInProgress = OFF;

/*!
 *****************************************************************************
 * \brief Enable exception processing according to the EMV standard.
 *
 * If \a as3911EmvExceptionProcessing is set to TRUE, then the receiver will be
 * reenabled  and the received data ignored if certain combinations of receive
 * errors occure.
 *****************************************************************************
 */
static uchar as3911EmvExceptionProcessing = OFF;

/*!
 *****************************************************************************
 * This variable is only considered if emv_exception_processing is enabled.
 *
 * If a frame is received with a data encoding (parity), or crc error
 * AND the length of the frame is <= \a as3911TransmissionErrorThreshold then
 * the receieved frame is asumed to be noise and the receiver is reseted to
 * receive another frame.
 *****************************************************************************
 */
static size_t as3911TransmissionErrorThreshold = 0;

extern EmvHalTransceiveMode_t gtransceiveMode; 
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
 * \brief Check whether the frame delay timer is running.
 *
 * \return TRUE: The FDT is running.
 * \return FALSE: The FDT is not running.
 *****************************************************************************
 */
static uchar as3911FdtIsRunning();

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

void as3911EnableEmvExceptionProcessing(uchar enableEmvExceptionProcessing)
{
    as3911EmvExceptionProcessing = enableEmvExceptionProcessing;
}

uchar as3911EmvExceptionProcessingIsEnabled()
{
    return as3911EmvExceptionProcessing;
}

void as3911SetTransmissionErrorThreshold(size_t transmissionErrorThreshold)
{
    as3911TransmissionErrorThreshold = transmissionErrorThreshold;
}

size_t as3911GetTransmissionErrorThreshold()
{
    return as3911TransmissionErrorThreshold;
}

void as3911SetReceiverDeadtime(u32 receiverDeadtime)
{
    u8 reg_mode;
    u8 mrt_value;

    as3911ReadRegister(AS3911_REG_MODE, &reg_mode);
    if ((reg_mode & AS3911_REG_MODE_mask_om) == AS3911_REG_MODE_om_nfc)
    {
        /* NFCIP1 mode. */
        mrt_value = (u8) ((receiverDeadtime >> 9) & 0xFF);
    }
    else
    {
        /* Non NFCIP1 mode. */
        mrt_value = (u8) ((receiverDeadtime >> 6) & 0xFF);
    }

    as3911WriteRegister(AS3911_REG_MASK_RX_TIMER, mrt_value);
}

void as3911SetReceiveTimeout(u32 receiveTimeout)
{
    u16 nrtValue;

    if (receiveTimeout > (uint)(0x003FFFC0))
    {
        /* Use 1:4096 prescaler. */
        nrtValue = (u16) (receiveTimeout >> 12);

        /* Round up if necessary. */
        if ((((u16) receiveTimeout) & 0x0FFF) != 0)
            nrtValue += 1;

//        as3911ModifyRegister(AS3911_REG_GPT_CONF, 0x03, 0x03);
        as3911ModifyRegister(AS3911_REG_GPT_CONTROL,
            AS3911_REG_GPT_CONTROL_nrt_emv | AS3911_REG_GPT_CONTROL_nrt_step,
            AS3911_REG_GPT_CONTROL_nrt_emv | AS3911_REG_GPT_CONTROL_nrt_step);
    }
    else
    {
        /* Use 1:64 prescaler. */
        nrtValue = (u16) (receiveTimeout >> 6);

        /* Round up if necessary. */
        if ((((u16) receiveTimeout) & 0x03F) != 0)
            nrtValue += 1;

//        as3911ModifyRegister(AS3911_REG_GPT_CONF, 0x03, 0x02);
        as3911ModifyRegister(AS3911_REG_GPT_CONTROL,
            AS3911_REG_GPT_CONTROL_nrt_emv | AS3911_REG_GPT_CONTROL_nrt_step,
            AS3911_REG_GPT_CONTROL_nrt_emv);
    }

    as3911WriteRegister(AS3911_REG_NO_RESPONSE_TIMER1, (u8) (nrtValue >> 8));
    as3911WriteRegister(AS3911_REG_NO_RESPONSE_TIMER2, (u8) (nrtValue & 0xFF));
}

void as3911SetFrameDelayTime(u32 frameDelayTime)
{
    u16 gptValue;

    as3911FrameDelayTime = frameDelayTime;

//    as3911ModifyRegister(AS3911_REG_GPT_CONF, 0xE0, 0x20);
    as3911ModifyRegister(AS3911_REG_GPT_CONTROL, AS3911_REG_GPT_CONTROL_gptc_mask, AS3911_REG_GPT_CONTROL_gptc_no_trigger);
    
    gptValue = (u16) (as3911FrameDelayTime >> 3);
    as3911WriteRegister(AS3911_REG_GPT1, (u8) ((gptValue >> 8) & 0xFF));
    as3911WriteRegister(AS3911_REG_GPT2, (u8) (gptValue & 0xFF));
}

u32 as3911GetFrameDelayTime()
{
    return as3911FrameDelayTime;
}

void as3911Transmit(u8 *message, size_t messageLength, AS3911RequestFlags_t requestFlags)
{
    u8 numBytesForFifo = 0;
    size_t numBytesTransmitted = 0;
    u32 irqStatus = 0;
    AS3911RequestFlags_t transmissionModeFlags = (AS3911RequestFlags_t)(requestFlags & AS3911_TX_MODE_MASK);

    /* Clear FIFO. */
    as3911ExecuteCommand(AS3911_CMD_CLEAR_FIFO);

    /* Reset squelch gain reduction. */
    as3911ExecuteCommand(AS3911_CMD_CLEAR_SQUELCH);

    if (transmissionModeFlags == AS3911_TRANSMIT_WUPA)
    {
        /* Enable anticollision frame handling. */
        as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, AS3911_REG_ISO14443A_NFC_antcl, AS3911_REG_ISO14443A_NFC_antcl);
        as3911ExecuteCommand(AS3911_CMD_TRANSMIT_WUPA);
    }
    else if (transmissionModeFlags == AS3911_TRANSMIT_REQA)
    {
        /* Enable anticollision frame handling. */
        as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, AS3911_REG_ISO14443A_NFC_antcl, AS3911_REG_ISO14443A_NFC_antcl);
        as3911ExecuteCommand(AS3911_CMD_TRANSMIT_REQA);
    }
    else
    {
        /* Setup number of bytes to send. */
        as3911WriteRegister(AS3911_REG_NUM_TX_BYTES1, (messageLength >> 5) & 0xFF);
        as3911WriteRegister(AS3911_REG_NUM_TX_BYTES2, (messageLength << 3) & 0xFF);

        if (messageLength > AS3911_FIFO_SIZE)
            numBytesForFifo = AS3911_FIFO_SIZE;
        else
            numBytesForFifo = messageLength;

        /* Load first part of the message into the FIFO. */
        as3911WriteFifo(message, numBytesForFifo);
        numBytesTransmitted = numBytesForFifo;

        /* Start transmission. */
        if (transmissionModeFlags == AS3911_TRANSMIT_WITHOUT_CRC)
            as3911ExecuteCommand(AS3911_CMD_TRANSMIT_WITHOUT_CRC);
        else
            as3911ExecuteCommand(AS3911_CMD_TRANSMIT_WITH_CRC);
    }

    do
    {
        as3911WaitForInterruptTimed(AS3911_IRQ_MASK_TXE | AS3911_IRQ_MASK_WL,
            AS3911_TX_IRQ_SANITY_TIMEOUT, &irqStatus);

        D2(LABLE(0xDD);
           DATAIN((irqStatus));
           DATAIN((irqStatus>>8));
           DATAIN((irqStatus>>16)););
        if (irqStatus & AS3911_IRQ_MASK_WL)
        {
            D2(LABLE(0x99);
//               DATAIN((messageLength>>8));
//               DATAIN((messageLength));
               DATAIN((numBytesTransmitted>>8));
               DATAIN((numBytesTransmitted));
               );
            /* FIFO water level interrupt. */
            if(numBytesTransmitted < messageLength)
            {
//                size_t numBytesForFifo = messageLength - numBytesTransmitted;

//                if(numBytesForFifo > (AS3911_FIFO_SIZE - AS3911_FIFO_TRANSMIT_WL0))
//                {
//                    numBytesForFifo = AS3911_FIFO_SIZE - AS3911_FIFO_TRANSMIT_WL0;
//                }
//
//                as3911WriteFifo(&message[numBytesTransmitted], (u8) numBytesForFifo);
//                numBytesTransmitted += numBytesForFifo;
                
                int numBytesForFifotmp = messageLength - numBytesTransmitted;
                D2(LABLE(0x99);
                   DATAIN((numBytesForFifotmp));
                  );
                if(numBytesForFifotmp > (AS3911_FIFO_SIZE - AS3911_FIFO_TRANSMIT_WL0))
                {
                    numBytesForFifotmp = AS3911_FIFO_SIZE - AS3911_FIFO_TRANSMIT_WL0;
                }

                as3911WriteFifo(&message[numBytesTransmitted], (u8) numBytesForFifotmp);
                numBytesTransmitted += numBytesForFifotmp;
            }
        }
        if (irqStatus & AS3911_IRQ_MASK_TXE)
        {
            return;
        }
    } while (0 != irqStatus);

    /* This code is only reached when the AS3911 interrupt sanity timeout expires. */
    return;
}

//uint gas3911emvtb145flg = 0;
const uchar tb145atqb[] = {0x50,0x46,0xB5,0xC7,0xA0,0x00,0x00,0x00,0x00,0x00,0x21,0x81,0x90,0x43};
s16 as3911Receive(u8 *response, size_t maxResponseLength, size_t *responseLength)
{
    u32 irqStatus = 0;
    size_t numBytesReceived = 0;
    uchar receiving = OFF;
    uchar nrtExpired = OFF;
    uchar overflowOccured = OFF;

    u8 auxDisplay = 0;
    u8 fifoStatus[2];
    u8 numBytesInFifoWL = 0;

    u8 fifoReadSize = 0;

    u32 errorIrqStatus = 0;
    u8 numBytesInFifo = 0;
    uchar reenableReceiver = OFF;

//    uchar temp[16];

//    if (!response || !maxResponseLength || !responseLength)
    if (!response || !responseLength)
    {
        return AS3911_NO_ERROR;
    }


    *responseLength = 0;

    do
    {

//        as3911WaitForInterruptTimed(
//            AS3911_IRQ_MASK_RXS | AS3911_IRQ_MASK_RXE | AS3911_IRQ_MASK_WL | AS3911_IRQ_MASK_NRE,
//            AS3911_RX_IRQ_SANITY_TIMEOUT,
//            &irqStatus
//            );
        as3911WaitForInterruptTimed(
            AS3911_IRQ_MASK_RXS | AS3911_IRQ_MASK_RXE | AS3911_IRQ_MASK_WL | AS3911_IRQ_MASK_NRE,
            (receiving ? AS3911_RX_IRQ_SANITY_TIMEOUT_AFTER_RXS : AS3911_RX_IRQ_SANITY_TIMEOUT),
            &irqStatus
            );

        D2(LABLE(0xCC);
           DATAIN((irqStatus));
           DATAIN((irqStatus>>8));
           DATAIN((irqStatus>>16)););
        if (irqStatus & AS3911_IRQ_MASK_WL)
        {
            /* FIFO water level interrupt. */
//            u8 auxDisplay = 0;
//            u8 fifoStatus[2];
//            u8 numBytesInFifoWL = 0;
            auxDisplay = 0;
            numBytesInFifoWL = 0;

            /* Read data from the FIFO in chunks as long as FIFO data is available
             * and the chip is receiving. Reading the FIFO data in chunks asures that
             * a too long FIFO read doesn't interfere with the error handling required
             * at the end of receive interrupt.
             */
            as3911ContinuousRead(AS3911_REG_FIFO_RX_STATUS1, &fifoStatus[0], 2);
            numBytesInFifoWL = fifoStatus[0] & AS3911_REG_FIFO_RX_STATUS1_mask_fifo_b;
            
            as3911ReadRegister(AS3911_REG_AUX_DISPLAY, &auxDisplay);
            D2(LABLE(0xF0);
               DATAIN((auxDisplay));
               DATAIN((numBytesInFifoWL));
               );
            while ((auxDisplay & AS3911_REG_AUX_DISPLAY_rx_on) && (numBytesInFifoWL > 0))
            {
                fifoReadSize = 0;

                fifoReadSize = (numBytesInFifoWL > AS3911_IRQ_FIFO_READ_CHUNK_SIZE) ? AS3911_IRQ_FIFO_READ_CHUNK_SIZE : numBytesInFifoWL;

                if (numBytesReceived + fifoReadSize > maxResponseLength)
                {
                    fifoReadSize = maxResponseLength - numBytesReceived;
                    overflowOccured = ON;
                }
                
                as3911ReadFifo(&response[numBytesReceived], fifoReadSize);
                numBytesReceived += fifoReadSize;
                numBytesInFifoWL -= fifoReadSize;

                as3911ReadRegister(AS3911_REG_AUX_DISPLAY, &auxDisplay);
//                D2(DATAIN((numBytesInFifoWL)););
            }

            if (!(auxDisplay & AS3911_REG_AUX_DISPLAY_rx_on))
                irqStatus |= AS3911_IRQ_MASK_RXE;
            D2(DATAIN((auxDisplay));
               DATAIN((numBytesReceived>>8));
               DATAIN((numBytesReceived));
               );
        }
        if (irqStatus & AS3911_IRQ_MASK_RXS)
        {
            D2(LABLE(0xF1););
            /* start of receive interrupt. */
            receiving = ON;
        }
        if (irqStatus & AS3911_IRQ_MASK_NRE)
        {
            D2(LABLE(0xF2););
            /* no response timer expired. */
            if (!receiving)
                return AS3911_TIMEOUT_ERROR;
            else
                nrtExpired = ON;
        }
        if (irqStatus & AS3911_IRQ_MASK_RXE)
        {
            /* End of reception. */
//            u8 fifoStatus[2];
//            u32 errorIrqStatus = 0;
//            u8 numBytesInFifo = 0;
//            uchar reenableReceiver = OFF;
            errorIrqStatus = 0;
            numBytesInFifo = 0;
            reenableReceiver = OFF;

            as3911ContinuousRead(AS3911_REG_FIFO_RX_STATUS1, &fifoStatus[0], 2);
            numBytesInFifo = fifoStatus[0] & AS3911_REG_FIFO_RX_STATUS1_mask_fifo_b;

            as3911GetInterrupts(
                AS3911_IRQ_MASK_COL | AS3911_IRQ_MASK_CRC | AS3911_IRQ_MASK_PAR | AS3911_IRQ_MASK_ERR1 | AS3911_IRQ_MASK_ERR2,
                &errorIrqStatus
                );

            if (fifoStatus[1] & AS3911_REG_FIFO_RX_STATUS2_fifo_ovr)
                overflowOccured = ON;

            D2(LABLE(0xF3);
               DATAIN(overflowOccured);
               DATAIN(numBytesInFifo);
               DATAIN(fifoStatus[1]);
               DATAIN((errorIrqStatus));
               DATAIN((errorIrqStatus>>8));
               DATAIN((errorIrqStatus>>16)););
            /* Residual bits are handled as hard framing error. */
            if ((fifoStatus[1] & (AS3911_REG_FIFO_RX_STATUS2_fifo_ncp | AS3911_REG_FIFO_RX_STATUS2_np_lb)) != 0x00)
                errorIrqStatus |= AS3911_IRQ_MASK_ERR2;

            D2(LABLE(0x88);
               DATAIN((numBytesReceived>>8));
               DATAIN((numBytesReceived));
               );

            if ( maxResponseLength == 0 ) {
                //halt命令 08262013 chenf
//                as3911_SetTimerCount(500);//general purpose timer not work  09012013 chenf
//                as3911_StartTimer(AS3911_TIMER);
                return AS3911_NO_ERROR;
            }

            if(as3911EmvExceptionProcessing)
            { 
#if 11
                /* EMV 2.4 Requirement 4.29: PCD EMD Handling:
                 * If a transmission error is detected and the number of received
                 * bytes is less than 4, then the PCD shall ignore the transmission and
                 * be ready to receive a PICC frame within a time tRECOVERY after the
                 * end of the modulation of the received bytes with the transmission
                 * error. */
                if((numBytesReceived + numBytesInFifo) <= as3911TransmissionErrorThreshold)
                {
                    if (((errorIrqStatus & AS3911_IRQ_MASK_COL)
                      || (errorIrqStatus & AS3911_IRQ_MASK_ERR1)
                      || (errorIrqStatus & AS3911_IRQ_MASK_ERR2)
                      || (errorIrqStatus & AS3911_IRQ_MASK_PAR)
                      || (errorIrqStatus & AS3911_IRQ_MASK_CRC)))
                        reenableReceiver = TRUE;
                }
                if (!receiving) /* no RXS but RXE case (for 3-bit messages) */
                    reenableReceiver = ON;
#else
                if (  (errorIrqStatus & AS3911_IRQ_MASK_COL)
                   || (errorIrqStatus & AS3911_IRQ_MASK_ERR1)
                   || (errorIrqStatus & AS3911_IRQ_MASK_ERR2))
                    reenableReceiver = ON;

                if (  (errorIrqStatus & AS3911_IRQ_MASK_PAR)
                   && ((numBytesReceived + numBytesInFifo) < as3911TransmissionErrorThreshold))
                    reenableReceiver = ON;

                /* Exclude messages shorter than 2 bytes from CRC checking as they can not
                 * contain a CRC. This is done specifically to fullfil test case TB306.12
                 * of the EMV_CL_PCD_TEST_BENCH_TEST_CASES_v2.0.1a, which requires a too
                 * short message (1 byte) without a CRC to be handled as protocol error.
                 */
                if (  (errorIrqStatus & AS3911_IRQ_MASK_CRC)
                   && ((numBytesReceived + numBytesInFifo) < as3911TransmissionErrorThreshold)
                   && ((numBytesReceived + numBytesInFifo) >= 2))
                    reenableReceiver = ON;
#endif
            }

            D2(DATAIN(reenableReceiver));
            if (reenableReceiver)
            {
                D2(LABLE(0x77);
                   DATAIN(nrtExpired);
                  );
                /* Reset the receive buffer. */
                numBytesReceived = 0;

                /* If no receive timeout has occured yet then the
                 * receiver needs to be reenabled. If a receive timeout
                 * has already occured then we have a reception timeout.
                 */
                if (!nrtExpired)
                {
//                    receiving = OFF;
//                    as3911ExecuteCommand(AS3911_CMD_CLEAR_FIFO);
//                    as3911ExecuteCommand(AS3911_CMD_UNMASK_RECEIVE_DATA);
                    u8 receiverEnableDirectCommands[] = { AS3911_CMD_CLEAR_FIFO, AS3911_CMD_UNMASK_RECEIVE_DATA };
                    receiving = OFF;
                    as3911ExecuteCommands(receiverEnableDirectCommands, sizeof(receiverEnableDirectCommands));
                }
                else
                {
//                    as3911_SetTimerCount(500);//general purpose timer not work  09012013 chenf
//                    as3911_StartTimer(AS3911_TIMER);
                    return AS3911_TIMEOUT_ERROR;
                }
            }
            else
            {
                /* Read data from the FIFO. */
                if (numBytesReceived + numBytesInFifo > maxResponseLength)
                {

//                //08262013 chenf
//if ( gemvdebugflg.emvproflg0 == 1 ) {
//    //置halt 不能收到数据
//                    as3911ReadFifo(&temp[0], numBytesInFifo);
//                    numBytesReceived += numBytesInFifo;
//                    DISPBUF(temp, numBytesInFifo, 0);
//                    sys_beep();
//                    TRACE("\r\nhalt\r\n");
//                DISPBUF(gucDebugBuf, guiDebugS3, 0);
//                vDispBufKey(guiDebugS3,gucDebugBuf);
//                    InkeyCount(0);
//}else{

                    as3911ReadFifo(&response[numBytesReceived], maxResponseLength - numBytesReceived);
                    numBytesReceived = maxResponseLength;
                    overflowOccured = ON;
//}
                }
                else
                {
                    as3911ReadFifo(&response[numBytesReceived], numBytesInFifo);
                    numBytesReceived += numBytesInFifo;
                }

                D2(LABLE(0x66);
                   DATAIN(numBytesInFifo);
                   DATAIN(overflowOccured);
                  );
                /* Stop NRT timer. */
                // as3911ModifyRegister(AS3911_REG_GPT_CONF, 0x02, 0x00);
                // as3911ExecuteCommand(AS3911_CMD_CLEAR_FIFO);

                /* Start GPT timer used for FDT_PCD. */
                // as3911ExecuteCommand(AS3911_CMD_START_TIMER);

                /* Stop NRT timer. */
                as3911ModifyRegister(AS3911_REG_GPT_CONTROL, AS3911_REG_GPT_CONTROL_nrt_emv, 0x00);
                as3911ExecuteCommand(AS3911_CMD_CLEAR_FIFO);

                /* Start GPT timer used for FDT_PCD. */
                as3911ExecuteCommand(AS3911_CMD_START_GP_TIMER);

//                as3911_SetTimerCount(500);//general purpose timer not work  09012013 chenf
//                as3911_StartTimer(AS3911_TIMER);

                as3911ReceptionInProgress = OFF;
                *responseLength = numBytesReceived;

                if (errorIrqStatus & AS3911_IRQ_MASK_COL)
                    return AS3911_COLLISION_ERROR;
                else if (errorIrqStatus & AS3911_IRQ_MASK_ERR1)
                    return AS3911_HARD_FRAMING_ERROR;
                else if (errorIrqStatus & AS3911_IRQ_MASK_CRC)
                    return AS3911_CRC_ERROR;
                else if (errorIrqStatus & AS3911_IRQ_MASK_PAR)
                    return AS3911_PARITY_ERROR;
                else if (errorIrqStatus & AS3911_IRQ_MASK_ERR2)
                {
//                    if ( gas3911emvtb145flg == 1 ) {
//                        //TB145 特殊处理
//                        if ( memcmp(response, tb145atqb, 14) == 0 ) {
//                            return AS3911_NO_ERROR;
//                        }else{
//                            return AS3911_SOFT_FRAMING_ERROR;
//                        }
//                    }else{
                        EG_mifs_tWorkInfo.lBitsReceived = numBytesReceived*8;
                        EG_mifs_tWorkInfo.lBitsReceived += ((fifoStatus[1]>>1)&0x07);
                        return AS3911_SOFT_FRAMING_ERROR;
//                    }
                }
                else if (overflowOccured)
                    return AS3911_OVERFLOW_ERROR;
                else
                {
                    EG_mifs_tWorkInfo.ulBytesReceived = numBytesReceived;
                    if ( EMV_HAL_TRANSCEIVE_WITH_CRC == gtransceiveMode ) {
                        EG_mifs_tWorkInfo.ulBytesReceived -= 2; //CRC
                    }
                    EG_mifs_tWorkInfo.lBitsReceived = EG_mifs_tWorkInfo.ulBytesReceived*8;
                    return AS3911_NO_ERROR;
                }
            }
        }

        D2(LABLE(0x55);
           DATAIN((errorIrqStatus));
           DATAIN((errorIrqStatus>>8));
           DATAIN((errorIrqStatus>>16));
           );

    } while (0 != irqStatus);

    /* We only reach this code if the IRQ sanity timeout expired. */
    return AS3911_INTERNAL_ERROR;
}

s16 as3911Transceive(u8 *request, size_t requestLength
    , u8 *response, size_t maxResponseLength, size_t *responseLength
    , u32 timeout, AS3911RequestFlags_t requestFlags)
{
    s16 returnValue = AS3911_NO_ERROR;

    /* When the CRC is ignored, then it is automatically but into the FIFO by
     * the AS3911.
     */
    D2(LABLE(0xEE);DATAIN((uchar)requestFlags);
       DATAIN(as3911EmvExceptionProcessing);
       DATAIN(as3911TransmissionErrorThreshold);
       DATAIN((uchar)requestLength);
       for (guiDebugj = 0; guiDebugj < requestLength; guiDebugj++)
            DATAIN(request[guiDebugj]);
       );
    if (requestFlags & AS3911_IGNORE_CRC)
    {
        as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, AS3911_REG_ISO14443A_NFC_antcl, AS3911_REG_ISO14443A_NFC_antcl);
        as3911ModifyRegister(AS3911_REG_AUX, AS3911_REG_AUX_no_crc_rx | AS3911_REG_AUX_crc_2_fifo, AS3911_REG_AUX_no_crc_rx);
    }
    else if (requestFlags & AS3911_CRC_TO_FIFO)
    {
        as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, AS3911_REG_ISO14443A_NFC_antcl, 0x00);
        as3911ModifyRegister(AS3911_REG_AUX, AS3911_REG_AUX_no_crc_rx | AS3911_REG_AUX_crc_2_fifo, AS3911_REG_AUX_crc_2_fifo);
    }
    else
    {
        as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, AS3911_REG_ISO14443A_NFC_antcl, 0x00);
        as3911ModifyRegister(AS3911_REG_AUX, AS3911_REG_AUX_no_crc_rx | AS3911_REG_AUX_crc_2_fifo, 0x00);
    }

    /* Adjust modulation level. */
    as3911AdjustModulationLevel();
	
	/* Adjust gain. */
//	as3911AdjustGain();

    /* Wait for the frame delay time to pass. */
    while (as3911FdtIsRunning())
        ;

    //FDTmin
//    as3911_DelayForTimer(AS3911_TIMER);//general purpose timer not work  09012013 chenf

    /* Setup timeout. */
    as3911SetReceiveTimeout(timeout);

    /* Reset irq status flags. */
    as3911ClearInterrupts(AS3911_IRQ_MASK_ALL);

    /* Enable interrupts. */
    as3911EnableInterrupts(AS3911_IRQ_MASK_ALL);
    AS3911_IRQ_ON();

    as3911Transmit(request, requestLength, requestFlags);
    returnValue = as3911Receive(response, maxResponseLength, responseLength);
//    SETSIGNAL_H(); 
//    s_DelayUs(500);//general purpose timer not work  09012013 chenf
//    SETSIGNAL_L(); 

    D2(LABLE(0xBB);DATAIN((uchar)returnValue);DATAIN((uchar)maxResponseLength);
       if ( responseLength ) {
           DATAIN((uchar)*responseLength);
           for (guiDebugj = 0; guiDebugj < *responseLength; guiDebugj++)
           DATAIN(response[guiDebugj]);
       }
       );

    //试验:通讯结束最好停止no-response timer 11h的bit1 清空 
//    as3911ModifyRegister(AS3911_REG_GPT_CONF, 0x03, 0x00);
//    as3911WriteRegister(AS3911_REG_NO_RESPONSE_TIMER1, (u8) 0);
//    as3911WriteRegister(AS3911_REG_NO_RESPONSE_TIMER2, (u8) 0);
//    s_DelayUs(30);
//    as3911ClearInterrupts(AS3911_IRQ_MASK_ALL);
    /* Restore old irq settings. */
    /* Disable AS3911 interrupts. */
    as3911DisableInterrupts(AS3911_IRQ_MASK_ALL);
//    AS3911_IRQ_OFF();//wp30 portB口还有其他中断 不能关闭03192015chenf

    return returnValue;
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

static uchar as3911FdtIsRunning()
{
    u8 regulatorResult = 0;
    as3911ReadRegister(AS3911_REG_REGULATOR_RESULT, &regulatorResult);

    if (regulatorResult & AS3911_REG_REGULATOR_RESULT_gpt_on)
        return ON;
    else
        return OFF;
}

#endif

