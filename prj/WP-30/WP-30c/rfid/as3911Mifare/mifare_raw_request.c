/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
//#include "as3911.h"
//#include "as3911_com.h"
///#include "delay.h"
//#include "mifare_parity_data_t.h"
//#include "mifare.h"
//#include <string.h>
//#include "logger.h"
#include "wp30_ctrl.h"
#include "mifare_parity_data_t.h"
#include "mifare.h"

#if (defined(EM_AS3911_Module))

#ifdef __DEBUG_Dx__
// #define __debugMifare__
 #ifdef __debugMifare__//__DEBUG_Dx__
     #define dbgLog(...)  Dprintk(__VA_ARGS__)
     #define dbgHexDump(x,y)  DISPBUF(x,y,0)
 #else
     #define dbgLog(...)  
     #define dbgHexDump(x,y)
#endif
#else
#define dbgLog(...)  
#define dbgHexDump(x,y)
#endif
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

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/
static u8 mifareRawBuffer[((MIFARE_TRANSMIT_BUFFER_SIZE*9)+7)/8];

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/
static u16 mifareCopyToRawBuffer(const parity_data_t *message, u16 length);
static u16 mifareExtractMessage(u8* response, u16 responseLength);

void as3911TransmitMif(u8 *message, size_t messageLength, size_t bits, AS3911RequestFlags_t requestFlags)
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
        as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, 0x01, 0x01);
        as3911ExecuteCommand(AS3911_CMD_TRANSMIT_WUPA);
    }
    else if (transmissionModeFlags == AS3911_TRANSMIT_REQA)
    {
        /* Enable anticollision frame handling. */
        as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, 0x01, 0x01);
        as3911ExecuteCommand(AS3911_CMD_TRANSMIT_REQA);
    }
    else
    {
        /* Setup number of bytes to send. */
		messageLength = messageLength*8 + bits;
		as3911WriteRegister(AS3911_REG_NUM_TX_BYTES1, (messageLength >> 8) & 0xFF);
        as3911WriteRegister(AS3911_REG_NUM_TX_BYTES2, messageLength & 0xFF);
//		as3911WriteRegister(AS3911_REG_NUM_TX_BYTES1, (((messageLength + 7)/8) >> 5) & 0xFF);
//        as3911WriteRegister(AS3911_REG_NUM_TX_BYTES2, (((messageLength + 7)/8) << 3) | (bits&0x07));
		messageLength = (messageLength + 7)/8;
//        D2(
//           DATAIN((uchar)messageLength);
//           as3911ReadRegister(AS3911_REG_NUM_TX_BYTES1, &tmp1);
//           as3911ReadRegister(AS3911_REG_NUM_TX_BYTES2, &tmp2);
//           DATAIN((uchar)tmp1);
//           DATAIN((uchar)tmp2);
//          );

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
            100, &irqStatus);

        D2(LABLE(0xDD);
           DATAIN((irqStatus));
           DATAIN((irqStatus>>8));
           DATAIN((irqStatus>>16)););
        if (irqStatus & AS3911_IRQ_MASK_WL)
        {
            /* FIFO water level interrupt. */
            if(numBytesTransmitted < messageLength)
            {
                int numBytesForFifotmp = messageLength - numBytesTransmitted;

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
/*
******************************************************************************
* GLOBAL FUNCTIONS
* mifare卡操作返回值的确定
* 认证过程: step1 ---- 收到若干字节数据 最后一字节4bit
*           step2 ---- 收到若干字节数据 最后一字节4bit
* 读卡过程: 收到若干数据 最后一字节不一定是整字节 做实验大概是2bit
* 写卡过程: step1 ---- 收到一个字节 4bit 必须参与加解密
*           step2 ---- 收到一个字节 4bit 必须参与加解密
*                      如果不参与(比如人为超时退出) 会导致后续读卡操作失败(实验得出)
* 增减过程: step1 ---- 收到一个字节 4bit 必须参与加解密
*           step2 ---- 卡不会回应数据 应该超时处理 一定不能参与加解密
*                      如果参与加解密 会导致后续读卡操作失败 与写卡过程相对
* 发送过程: 收到一个字节 4bit
* 存储过程: step1 ---- 收到一个字节 4bit 必须参与加解密
*           step2 ---- 卡不会回应数据 应该超时处理 一定不能参与加解密
*                      如果参与加解密 会导致后续读卡操作失败 与增减过程一致
******************************************************************************
*/
s8 mifareSendRawRequest(const parity_data_t *request,
        u16 requestLength,
        u8 *response,
        u16 maxResponseLength,
        u16 *responseLength,
        u32 timeout,
        uchar fourBitResponse)
{
    s8 err = ERR_NONE;
    u16 len_bits;
    u8 aux = 0;
    size_t responseLengthtmp;
    s16 returnValue = AS3911_NO_ERROR;

    D2(LABLE(0xEE);
       DATAIN((uchar)requestLength);
       for (guiDebugj = 0; guiDebugj < requestLength; guiDebugj++){
            DATAIN((request[guiDebugj]>>8)&0xFF);
            DATAIN((request[guiDebugj]&0xFF));
            }
       );

    len_bits = mifareCopyToRawBuffer(request, requestLength);

    /* Adjust modulation level. */
    //as3911AdjustModulationLevel();
	
	/* Adjust gain. */
	//as3911AdjustGain();

    /* Wait for the frame delay time to pass. */
    while (1)
	{
		as3911ReadRegister(AS3911_REG_REGULATOR_RESULT, &aux);
		if (!(aux & AS3911_REG_REGULATOR_RESULT_gpt_on))
			break;
	}

//    as3911WriteRegister(AS3911_REG_ISO14443A_NFC, 0x80);
//    as3911ModifyRegister(AS3911_REG_AUX, 0xC0, 0x40);
//    as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, 0x01, 0x00);

    /* Setup timeout. */
//    as3911SetReceiveTimeout(200000);
//    as3911SetReceiveTimeout(678000); //预设50ms
    as3911SetReceiveTimeout(timeout); //预设50ms

    /* Reset irq status flags. */
    as3911ClearInterrupts(AS3911_IRQ_MASK_ALL);

    /* Enable interrupts. */
    as3911EnableInterrupts(AS3911_IRQ_MASK_ALL);
    AS3911_IRQ_ON();

    D2(LABLE(0xEE);
       DATAIN((uchar)len_bits);
       DATAIN((uchar)(len_bits/8));
       DATAIN((uchar)(len_bits%8));
       for (guiDebugj = 0; guiDebugj < (len_bits+7)/8; guiDebugj++){
            DATAIN(mifareRawBuffer[guiDebugj]);
            }
       );

//    SETSIGNAL_H();
    as3911TransmitMif(mifareRawBuffer, len_bits/8, len_bits%8, AS3911_TRANSMIT_WITHOUT_CRC);
//    SETSIGNAL_L();
    //    as3911Receive(mifareRawBuffer, sizeof(mifareRawBuffer), responseLength);
//    SETSIGNAL1_H();
    returnValue = as3911Receive(mifareRawBuffer, sizeof(mifareRawBuffer), &responseLengthtmp);
//    SETSIGNAL1_L();
    *responseLength = responseLengthtmp;

    D2(LABLE(0xBB);DATAIN((uchar)returnValue);DATAIN((uchar)(sizeof(mifareRawBuffer)));
       if ( responseLength ) {
           DATAIN((uchar)*responseLength);
           for (guiDebugj = 0; guiDebugj < *responseLength; guiDebugj++)
           DATAIN(mifareRawBuffer[guiDebugj]);
       }
       );

    /* Restore old irq settings. */
    /* Disable AS3911 interrupts. */
    as3911DisableInterrupts(AS3911_IRQ_MASK_ALL);
    AS3911_IRQ_OFF();

//	if (err != ERR_NONE)
//		goto out;

    //返回值参见函数title
    if ( returnValue ==  AS3911_TIMEOUT_ERROR) {
        err = ERR_NOTFOUND;
		goto out;
    }

    if (((*responseLength * 8) / 9) > maxResponseLength)
    {
        dbgLog("limiting l=%hx ,ml=%hx\n",*responseLength,maxResponseLength);
        *responseLength = maxResponseLength / 8 * 9;
        err = ERR_NONE; /* This will but an existing CRC */
    }
    *responseLength = mifareExtractMessage(response, *responseLength);

out:
    return err;
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/
static u16 mifareCopyToRawBuffer(const parity_data_t *message, u16 length)
{
    int i, bytepos = 0;
    int bitpos = 0;
    memset(mifareRawBuffer,0,sizeof(mifareRawBuffer));
    dbgLog("transmitting: ");
    for(i = 0; i<length; i++)
    {
        dbgLog("%hx,",message[i]);
    }
    dbgLog("\n");


    for (i = 0; i < length; i++)
    {
        u16 m = message[i];
        mifareRawBuffer[bytepos] |= (m & ((1<<(8 - bitpos))-1)) << bitpos;
        bytepos++;
        mifareRawBuffer[bytepos] |= (m >> (8-bitpos));

        bitpos += 1;
        if (bitpos >=8 )
        {
            bitpos -= 8;
            bytepos++;
        }
    }
    dbgLog("  raw: ");
    for ( i= 0; i< ((length*9)+7)/8;i++)
    {
        dbgLog("%hhx,",mifareRawBuffer[i]);
    }
    dbgLog("\n");
    return length*9;
}
static u16 mifareExtractMessage(u8* response, u16 responseLength)
{
    int bytes = responseLength * 8 / 9;
    int i, bytepos = 0;
    int bitpos = 0;
    dbgLog("extracting ");
    dbgHexDump(mifareRawBuffer,responseLength);
    if (responseLength==1)
    {
        response[0] = mifareRawBuffer[0];
        return 1;
    }
    for (i = 0; i < bytes; i++)
    {
        u8 m;
        m = (mifareRawBuffer[bytepos] >> bitpos);
        bytepos++;
        m |= (mifareRawBuffer[bytepos] << (8-bitpos));

        bitpos += 1;
        if (bitpos >=8 )
        {
            bitpos -= 8;
            bytepos++;
        }

        response[i] = m;
    }
    dbgLog(" extracted: ");
    dbgHexDump(response,bytes);
    return bytes;
}

#endif

