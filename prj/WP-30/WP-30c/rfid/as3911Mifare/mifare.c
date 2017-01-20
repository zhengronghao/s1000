/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
//#include "platform.h"
//#include "as3911_com.h"
//#include "as3911_def.h"
//#include "as3911_io.h"
//#include "crc.h"
//#include "mifare_crypto1.h"
//#include "mifare.h"
//#include "mifare_raw_request.h"
//#include "EMV/emv_hal.h"
//#include "logger.h"
#include "wp30_ctrl.h"
#include "mifare_crypto1.h"
#include "mifare.h"
#include "mifare_raw_request.h"

#if (defined(EM_AS3911_Module))
/*
******************************************************************************
* MACROS
******************************************************************************
*/
#ifdef __DEBUG_Dx__
// #define __debugMifare__
 #ifdef __debugMifare__//__DEBUG_Dx__
     #define dbgLog(...)  Dprintk(__VA_ARGS__)
     #define MIFARE_DEBUG(...) Dprintk(__VA_ARGS__)
     #define dbgHexDump(x,y)  DISPBUF(x,y,0)
 #else
     #define dbgLog(...)  
     #define MIFARE_DEBUG(...)
     #define dbgHexDump(x,y)
 #endif
#else
#define dbgLog(...)  
#define MIFARE_DEBUG(...)
#define dbgHexDump(x,y)
#endif

/*
******************************************************************************
* DEFINES
******************************************************************************
*/
/*! Reader nonce used by the MiFare single step authentication code. */
#define MIFARE_DEFAULT_READER_NONCE             0xAA55AA55

/*! Timeout for mifare authentication step 1 in milliseconds. */
#define MIFARE_AUTHENTICATION_STEP1_TIMEOUT     10

/*! Timeout for mifare authentication step 2 in milliseconds. */
#define MIFARE_AUTHENTICATION_STEP2_TIMEOUT     10

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/
/*!
 *****************************************************************************
 * Internal buffer for transmitted and received messages in the
 * partiy_data_t format.
 * This buffer is also used as intermediate storage buffer during encryption
 * and decryption.
 *****************************************************************************
 */
static parity_data_t transceive_buffer[MIFARE_TRANSMIT_BUFFER_SIZE];

/*! Global crypto1 cipher state object. */
static crypto1_state mifareCipherState;

/*!
 *****************************************************************************
 * If \a mifareCipherActive is \a TRUE, then a block authentication has been
 * successfully performed and all data transmission will be encrypted.
 *****************************************************************************
 */
static u8 mifareCipherActive;

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/
u16 mifareCalculateIso14443aCrc(const u8 *buffer, u16 length);

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
s8 mifareInitialize()
{
    s8 err = ERR_NONE;

	err = emvHalActivateField(TRUE);
	emvHalSetStandard(EMV_HAL_TYPE_A);
    emvHalSetErrorHandling(EMV_HAL_PREACTIVATION_ERROR_HANDLING);
    //err = iso14443AInitialize();

    mifareCipherActive = FALSE;

    err |= as3911WriteRegister(AS3911_REG_ISO14443A_NFC, 0xC0);
    return err;
}

s8 mifareDeinitialize(u8 keep_on)
{
    s8 err;

    //err = iso14443ADeinitialize(keep_on);
	err = emvHalActivateField(keep_on);

    as3911ModifyRegister(AS3911_REG_NUM_TX_BYTES2, 0x07, 0);
    err |= as3911WriteRegister(AS3911_REG_ISO14443A_NFC, 0);

    return err;
}

void mifareSetKey(const u8 *key)
{
    s16 i;

    if (!crypto1_new(&mifareCipherState, CRYPTO1_CIPHER_READER, CRYPTO1_IMPLEMENTATION_CLEAN))
    {
        MIFARE_DEBUG("Initialization failed \n");
    }

    uint64emu_storage_t corrected_key;

    for (i = 0; i < 6; i++)
    {
        uint64emu_setbyte(&corrected_key, 5 - i, key[i]);
    }

    crypto1_init(&mifareCipherState, &corrected_key);

    MIFARE_DEBUG("Initialization ok\n");
}

void mifareResetCipher(void)
{
    //AMS_MEMSET(&mifareCipherState, 0, sizeof(mifareCipherState));
	u8 i;
	u8 *p;
	
	p = (u8 *)&mifareCipherState;
	for (i = 0; i < sizeof(mifareCipherState); i ++)
		*p++ = 0;
		
    mifareCipherActive = FALSE;
}

s8 mifareAuthenticate(u8 keySelect,
        u8 block,
        const u8 *uid,
        u8 uidLength,
        const u8 *key)
{
    s8 err;

    err = mifareAuthenticateStep1(keySelect, block, uid, uidLength, key);
    //EVAL_ERR_NE_GOTO(ERR_NONE, err, out);
	if (err != ERR_NONE) 
        goto out;

    err = mifareAuthenticateStep2(MIFARE_DEFAULT_READER_NONCE);

out:
    return err;
}

s8 mifareAuthenticateStep1(u8 keySelect,
        u8 block,
        const u8 *uid,
        u8 uidLength,
        const u8 *key)
{
    s8 err = ERR_NONE;
    u8 authenticationCommand[4];
    u8 authenticationResponse[5];
    u16 crc;
    size_t index;
    u16 numReceivedBytes;
    u32 uid_as_u32;
    u32 tag_nonce;

    authenticationCommand[0] = keySelect;
    authenticationCommand[1] = block;
    /* Append CRC. */
    crc = mifareCalculateIso14443aCrc(authenticationCommand, 2);
    authenticationCommand[2] = crc & 0xFF;
    authenticationCommand[3] = (crc >> 8) & 0xFF;

    /* Convert authenticate_command to parity_data_t. */
    for(index = 0; index < 4; index++)
        transceive_buffer[index] = authenticationCommand[index];

    calculateParity(transceive_buffer, 4);
  
    D2(LABLE(0xEE);
       DATAIN((uchar)mifareCipherActive);
       for (guiDebugj = 0; guiDebugj < 4; guiDebugj++){
            DATAIN(authenticationCommand[guiDebugj]);
            }
       );

    if(uidLength < 4)
    {
        MIFARE_DEBUG("!! UID too short. Abort.\n");

        return ERR_PARAMS;
    }

    if(mifareCipherActive)
        crypto1_transcrypt(&mifareCipherState, transceive_buffer, 4);

//    err = mifareSendRawRequest(transceive_buffer, 4, authenticationResponse
//        , sizeof(authenticationResponse), &numReceivedBytes, MIFARE_AUTHENTICATION_STEP1_TIMEOUT, FALSE);
    err = mifareSendRawRequest(transceive_buffer, 4, authenticationResponse
        , sizeof(authenticationResponse), &numReceivedBytes, MIFARE_TRANSCEIVE_DEFTIME, FALSE);
    //EVAL_ERR_NE_GOTO(ERR_NONE, err, out);
	if (err != ERR_NONE) 
        goto out;

    if(numReceivedBytes != 4)
    {
        MIFARE_DEBUG("!! ");
        MIFARE_DEBUG("Reveiced 0x%x byte and 0x%x bits", numReceivedBytes, 0);
        MIFARE_DEBUG(", expected 4 bytes and 0 bits. Abort.\n");
        dbgHexDump(authenticationResponse,numReceivedBytes);

        mifareResetCipher();
        return ERR_NOTFOUND;
    }
    dbgHexDump(authenticationResponse,numReceivedBytes);

    uid_as_u32 = ARRAY_TO_UINT32(uid);
    tag_nonce = ARRAY_TO_UINT32(authenticationResponse);

    if(mifareCipherActive)
    {
        mifareResetCipher();
        mifareSetKey(key);
        crypto1_clean_mutual_1_2(&mifareCipherState, uid_as_u32, tag_nonce);
    }
    else
    {
        mifareResetCipher();
        mifareSetKey(key);
        crypto1_mutual_1(&mifareCipherState, uid_as_u32, tag_nonce);
    }

    MIFARE_DEBUG("uid: %04X%04X\r\n", (u16) (uid_as_u32 >> 16), (u16) uid_as_u32);
    MIFARE_DEBUG("Card nonce: 0x%04X%04X\r\n", (u16) (tag_nonce >> 16), (u16) tag_nonce);
    MIFARE_DEBUG("Auth step 1 Successfull\r\n");

out:
    return err;
}

/*!
 *****************************************************************************
 * Perform second step of authentication (reader side) using a given random
 * number for the reader nonce.
 *
 * \param readerNonce Reader nonce to use during the authentication.
 *
 * \return ERR_NONE: The authentication step was successfull.
 * \return ERR_
 *****************************************************************************
 */
s8 mifareAuthenticateStep2(u32 readerNonce)
{
    s8 err = ERR_NONE;
    parity_data_t auth2Command[8];
    u8 auth2Response[5];
    u16 numReceivedBytes;
    u16 index;

    UINT32_TO_ARRAY_WITH_PARITY( readerNonce, auth2Command);
    crypto1_mutual_2(&mifareCipherState, auth2Command);

//    err = mifareSendRawRequest(auth2Command, 8, auth2Response
//        , sizeof(auth2Response), &numReceivedBytes, MIFARE_AUTHENTICATION_STEP2_TIMEOUT, FALSE);
    err = mifareSendRawRequest(auth2Command, 8, auth2Response
        , sizeof(auth2Response), &numReceivedBytes, MIFARE_TRANSCEIVE_DEFTIME, FALSE);
    //EVAL_ERR_NE_GOTO(ERR_NONE, err, out);
	if (err != ERR_NONE) 
        goto out;

    if (numReceivedBytes != 4)
    {
        MIFARE_DEBUG("!! received 0x%x bytes, expected 4 bytes and 0 bits. Abort.\n", numReceivedBytes);

        mifareResetCipher();
        return ERR_NOTFOUND;
    }

    // Convert auth2_response to parity_data_t
    for(index = 0; index < 5; index++)
        transceive_buffer[index] = auth2Response[index];

    if(!crypto1_mutual_3(&mifareCipherState, transceive_buffer))
    {
        MIFARE_DEBUG("!! Invalid card response. Abort\n");

        mifareResetCipher();
        return ERR_NOMSG;
    }
  
    mifareCipherActive = TRUE;
    MIFARE_DEBUG("Authentication ok. Success\n");

out:
    return err;
}

/*!
 *****************************************************************************
 * \brief Send a request to a MiFare card.
 * 
 * Send a request to a MiFare card. If a sector has been authenticated for
 * then all data transmissions will be encrypted. If no authentication has
 * been performed yet then all data transmission is done unencrypted.
 * 
 * \note \a request and \a response always contain unencrypted data.
 * \note The CRC of the request is automatically calculated and appended
 * to the transmitted data by this routine. The CRC of the response is
 * included in the response data field and NOT checked by this function
 * as some MiFare responses do not contain a CRC.
 * \note \a four_bit_response is ignored right now.
 * \note If a four bit frame is received then the number of received bytes
 * returned will be zero and no data will be stored in the receive buffer.
 *
 * \param request Pointer to a buffer holding the request.
 * \param requestLength Length of the request in bytes.
 * \param response Pointer to the buffer used for storing the response.
 * \param maxResponseLength Maximum Length of the response buffer.
 * \param responseLength Length of the received response.
 * \param timeout Timeout in microseconds.
 * \param fourBitResponse If this is \a TRUE, then a 4 bit response is
 * expected.
 *
 * \return ERR_NONE: 
 *****************************************************************************
 */
s8 mifareSendRequest(const u8 *request, u16 requestLength
    , u8 *response, u16 maxResponseLength, u16 *responseLength, u32 timeout, uchar fourBitResponse)
{
    s8 err = ERR_NONE;
    size_t index;
    u16 crc;

    /* Copy request into parity_data_t transmit buffer. */
    for(index = 0; index < requestLength; index++)
    {
        transceive_buffer[index] = request[index];
    }

    /* Append CRC. */
    crc = mifareCalculateIso14443aCrc(request, requestLength);
    transceive_buffer[requestLength] = ((u8) (crc & 0xFF));
    transceive_buffer[requestLength+1] = ((u8) ((crc>>8) & 0xFF));

    // Calculate Parity
    calculateParity(&transceive_buffer[0], requestLength+2);

    // Encrypt if cipher is in use
    if(mifareCipherActive)
        crypto1_transcrypt(&mifareCipherState, transceive_buffer, requestLength+2);
  
    err = mifareSendRawRequest(transceive_buffer, requestLength+2
        , response, maxResponseLength, responseLength, timeout, fourBitResponse);
    dbgLog("mifareSendRequest[%hhx]\n",err);
    //EVAL_ERR_NE_GOTO(ERR_NONE, err, out);
	if (err != ERR_NONE) 
        goto out;
//    if ( mifareflg ) {
//        TRACE("\r\n\r\n---miflag set 1---\r\n");
//        goto out;
//    }

    // Copy response into transceive buffer for decryption.
    for(index = 0; index < *responseLength; index++)
        transceive_buffer[index] = response[index];
  
    D2(LABLE(0xBB);
       DATAIN((uchar)mifareCipherActive);
       DATAIN((uchar)(*responseLength));
       for (guiDebugj = 0; guiDebugj < *responseLength; guiDebugj++){
            DATAIN(response[guiDebugj]);
            }
       );

    // Decrypt message in transceive buffer if cipher is in use.
    if(mifareCipherActive)
    {
        // If a response with a length of 0 or 1 byte is received it is asumed that this
        // actually was an ACK,NACK and 4 bits are fed into the cipher
        if((*responseLength == 0) || (*responseLength == 1))
        {
            // The AS3911 stores the 4 bit response in the high nibble but the
            // crypto1 implementation expects it in the low nibble. We also need
            // to reverse the bit order.

            transceive_buffer[0] >>= 4;
            // Reverse bit order of the low nibble.
            /*transceive_buffer[1] = 0;
            for(unsigned int index = 0; index < 4; index++)
            {
                transceive_buffer[1] <<= 1;

                if(transceive_buffer[0] & 0x01)
                    transceive_buffer[1] & 0x01;

                transceive_buffer[0] >>= 1;
            }
            transceive_buffer[0] = transceive_buffer[1];*/

            crypto1_transcrypt_bits(&mifareCipherState, &transceive_buffer[0], 0, 4);
        }
        else
            crypto1_transcrypt_bits(&mifareCipherState, transceive_buffer, *responseLength, 0);
    }

    D2(LABLE(0xBB);
       DATAIN((uchar)mifareCipherActive);
       DATAIN((uchar)*responseLength);
       for (guiDebugj = 0; guiDebugj < *responseLength; guiDebugj++){
            DATAIN((transceive_buffer[guiDebugj]>>8)&0xFF);
            DATAIN((transceive_buffer[guiDebugj]&0xFF));
            }
       );
    /* Copy decrypted message back into response buffer. */
    /* ToDo: Check why not only numReceivedBytes bytes are copied. */
    for(index = 0; index < *responseLength; index++)
        response[index] = (unsigned char) transceive_buffer[index];

out:
    return err;
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/
static u16 crcUpdateCcitt(u16 crc, u8 dat)
{
    dat ^= ((u8)crc) & 0xFF;
    dat ^= dat << 4;

    crc = (crc >> 8)^(((u16) dat) << 8)^(((u16) dat) << 3)^(((u16) dat) >> 4);

    return crc;
}
/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
u16 crcCalculateCcitt(u16 preloadValue, const u8* buf, u16 length)
{
    u16 crc = preloadValue;
    u16 index;

    for (index = 0; index < length; index++)
    {
        crc = crcUpdateCcitt(crc, buf[index]);
    }

    return crc;
}

/*
 *****************************************************************************
 * \brief Calculate the ISO14443A CRC of a message.
 *
 * \param[in] buffer : Buffer holding the message.
 * \param[in] length : Length of the message in bytes.
 *
 * \return ISO14443A CRC of the message stored in the buffer.
 *****************************************************************************
 */
u16 mifareCalculateIso14443aCrc(const u8 *buffer, u16 length)
{
    return crcCalculateCcitt(0x6363, buffer, length);
}

u16 mifareCopyToRawBuffertest(const parity_data_t *message, u16 length)
{
    int i, bytepos = 0;
    int bitpos = 0;
    uchar testbuff[128];

    memset(testbuff,0,sizeof(testbuff));
    dbgLog("transmitting: ");
    for(i = 0; i<length; i++)
    {
        dbgLog("%hx,",message[i]);
    }
    dbgLog("\n");


    for (i = 0; i < length; i++)
    {
        u16 m = message[i];
        testbuff[bytepos] |= (m & ((1<<(8 - bitpos))-1)) << bitpos;
        bytepos++;
        testbuff[bytepos] |= (m >> (8-bitpos));

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
        dbgLog("%hhx,",testbuff[i]);
    }
    dbgLog("\n");
    return length*9;
}

//bitcol:0 or 1
//冲突位后 发送冲突位1或则0 可以选中知道uid的卡 默认1
u8 as3911AntiCollision(uchar bitcol, uchar ucSEL, uchar * pucUID, uint timeout)
{
    u8 colreg, aux, bcc, colbyte = 0;
    u8 bytesBeforeCol = 2;
    u8 bitsBeforeCol = 0;
    u8 buf[8];
    s16 err;
    size_t responseLength;
    int i;

    as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, 0x01, 0x01);
    as3911ModifyRegister(AS3911_REG_AUX, 0xC0, 0x80);

    memset(buf, 0, sizeof(buf));
    buf[0] = ucSEL;
    buf[1] = 0x20;

    do {

        while (1)
        {
            as3911ReadRegister(AS3911_REG_REGULATOR_RESULT, &aux);
            if (!(aux & AS3911_REG_REGULATOR_RESULT_gpt_on))
                break;
        }

        as3911SetReceiveTimeout(timeout); //预设50ms

        /* Reset irq status flags. */
        as3911ClearInterrupts(AS3911_IRQ_MASK_ALL);

        /* Enable interrupts. */
        as3911EnableInterrupts(AS3911_IRQ_MASK_ALL);
        AS3911_IRQ_ON();

        dbgLog("\r\nStart Anticollision start:%d  %d\r\n",bytesBeforeCol,bitsBeforeCol);
        dbgHexDump(buf, 7);

        D2(LABLE(0xEE);DATAIN(bytesBeforeCol);DATAIN(bitsBeforeCol);
           for (guiDebugj = 0; guiDebugj < bytesBeforeCol; guiDebugj++){
                DATAIN(buf[guiDebugj]);
           }
           if ( bitsBeforeCol ) {
                DATAIN(buf[guiDebugj]);
           }
          );
        as3911TransmitMif(buf, bytesBeforeCol, bitsBeforeCol, AS3911_TRANSMIT_WITHOUT_CRC);

        err = as3911Receive(buf + bytesBeforeCol, ISO14443A_CASCADE_LENGTH - bytesBeforeCol, &responseLength);

        D2(LABLE(0xBB);DATAIN((uchar)err);
           DATAIN((uchar)responseLength);
           for (guiDebugj = 0; guiDebugj < responseLength; guiDebugj++)
           DATAIN(buf[bytesBeforeCol+guiDebugj]);
          );
        as3911DisableInterrupts(AS3911_IRQ_MASK_ALL);
        AS3911_IRQ_OFF();

        dbgLog("after rx  %d\r\n",err);
//        dbgLog("Received 0x%x bytes\r\n", actlength);

        if (bitsBeforeCol > 0)
        {
            buf[bytesBeforeCol] >>= bitsBeforeCol;
            buf[bytesBeforeCol] <<= bitsBeforeCol;
            buf[bytesBeforeCol] |= colbyte;
        }

        if ( err == AS3911_NO_ERROR ) {
            dbgLog("\r\nGot a frame\r\n");
            bcc = 0;
            for (i = 0; i< 4; i++)
                bcc ^= buf[2+i];
            if ( bcc == buf[2+i] ) {
                memcpy(pucUID, &buf[2], 5);
                return EM_mifs_SUCCESS;
            }else
                return EM_mifs_SERNRERR;
        }
        else if (err == AS3911_COLLISION_ERROR)
        {
            /* now check for collision */
            dbgLog("Collision!\n");
            /* read out collision register */
            as3911ReadRegister(AS3911_REG_COLLISION_STATUS, &colreg);

            bytesBeforeCol = (colreg >> 4) & 0xf;
            bitsBeforeCol = (colreg >> 1) & 0x7;
            bitsBeforeCol++;
            if (bitsBeforeCol == 8)
            {
                bitsBeforeCol = 0;
                bytesBeforeCol++;
            }
            dbgLog("\r\nBytes before Col 0x%x ", bytesBeforeCol);
            dbgLog("\r\nBits before Col 0x%x\r\n", bitsBeforeCol);
            /* FIXME handle c_pb collision in parity bit */
            /* update NVB. Add 2 bytes for SELECT and NVB itself */
            buf[1] = bytesBeforeCol << 4;
            buf[1] |= bitsBeforeCol;
            /* save the colision byte */
            buf[bytesBeforeCol] <<= (8 - bitsBeforeCol);
            buf[bytesBeforeCol] >>= (8 - bitsBeforeCol);

            //选择后续是发送冲突位1 还是0
            buf[bytesBeforeCol] |= (bitcol<<(bitsBeforeCol-1));

            colbyte = buf[bytesBeforeCol];

            dbgLog("\r\nbuf[1]: 0x%x ", buf[1]);
            dbgLog("\r\nbuf[%02d]: 0x%x ",bytesBeforeCol, buf[bytesBeforeCol]);
        }
        else
        {
            return EM_mifs_TRANSERR;
        } /* no collision detected */

    } while (1);
}

#ifdef __DEBUG_Dx__

int mifareTestTransceive(u8 *request, u32 requestLength
    , u8 *response, u32 maxResponseLength, u32 *responseLength
    , u32 timeout, AS3911RequestFlags_t requestFlags)
{

    int len_bits = 0;
    int i;
    u16 crc;
    u16 partiybuf[64];
    u8 authenticationResponse[5];
    u16 numReceivedBytes;

    crc = mifareCalculateIso14443aCrc(request, requestLength);
//    TRACE("\r\n\r\n---crc---%x\r\n",crc);

    memset(partiybuf, 0, sizeof(partiybuf));
    for ( i = 0 ; i < requestLength ; i++ ) {
        partiybuf[i] = request[i];
    }
    partiybuf[i++] = crc & 0xFF;
    partiybuf[i++] = (crc >> 8) & 0xFF;
    
    calculateParity(partiybuf, i);

//    len_bits = mifareCopyToRawBuffertest(partiybuf, i);
//    TRACE("\r\n\r\n---bit---%d\r\n",len_bits);
    len_bits = mifareSendRawRequest(partiybuf, i, authenticationResponse
        , sizeof(authenticationResponse), &numReceivedBytes, MIFARE_TRANSCEIVE_DEFTIME, FALSE);

    return len_bits;
}

s8 iso14443ADoAntiCollisionLoop(iso14443AProximityCard_t* card)
{
    u8 cscs[ISO14443A_MAX_CASCADE_LEVELS][ISO14443A_CASCADE_LENGTH];
    u8 cl = 0x93;
    u8 colreg;
    u8 bytesBeforeCol = 2;
    u8 bitsBeforeCol = 0;
    s16 err;
    u8 colbyte = 0;
    u8 i;
    u16 actlength;
    u16 actsaklength;
    u8* buf;
    size_t responseLengthtmp;
    u8 aux;

    memset(cscs, 0, sizeof(cscs));
    card->cascadeLevels = 0;
    card->collision = FALSE;
    card->actlength = 0;
    buf = cscs[card->cascadeLevels];
    /* start anticollosion loop by sending SELECT command and NVB 0x20 */
    buf[1] = 0x20;

    as3911ModifyRegister(AS3911_REG_ISO14443A_NFC, 0x01, 0x01);
    as3911ModifyRegister(AS3911_REG_AUX, 0xC0, 0x80);

    as3911ReadRegister(AS3911_REG_ISO14443A_NFC,&i);
//    TRACE("Start 05h:%x\r\n",i);
    as3911ReadRegister(AS3911_REG_AUX,&i);
//    TRACE("Start 09h:%x\r\n",i);
//    TRACE("Start Anticollision loop\r\n");

    do {
        buf[0] = cl;

        while (1)
        {
            as3911ReadRegister(AS3911_REG_VSS_REGULATOR_RESULT, &aux);
            if (!(aux & 0x04))
                break;
        }

        as3911SetReceiveTimeout(678000); //预设50ms
        //    as3911SetReceiveTimeout(timeout); //预设50ms

        /* Reset irq status flags. */
        as3911ClearInterrupts(AS3911_IRQ_MASK_ALL);

        /* Enable interrupts. */
        as3911EnableInterrupts(AS3911_IRQ_MASK_ALL);
        AS3911_IRQ_ON();

//        TRACE("\r\nStart Anticollision start:%d  %d\r\n",bytesBeforeCol,bitsBeforeCol);
        DISPBUF(buf, 7, 0);

        D2(LABLE(0xEE);
           for (guiDebugj = 0; guiDebugj < bytesBeforeCol; guiDebugj++){
                DATAIN(buf[guiDebugj]);
           }
           if ( bitsBeforeCol ) {
                DATAIN(buf[guiDebugj]);
           }
          );
        as3911TransmitMif(buf, bytesBeforeCol, bitsBeforeCol, AS3911_TRANSMIT_WITHOUT_CRC);

        err = as3911Receive(buf + bytesBeforeCol, ISO14443A_CASCADE_LENGTH - bytesBeforeCol, &responseLengthtmp);

        D2(LABLE(0xBB);DATAIN((uchar)err);
           DATAIN((uchar)responseLengthtmp);
           for (guiDebugj = 0; guiDebugj < responseLengthtmp; guiDebugj++)
           DATAIN(buf[bytesBeforeCol+guiDebugj]);
          );
        as3911DisableInterrupts(AS3911_IRQ_MASK_ALL);
        AS3911_IRQ_OFF();

//        TRACE("after rx  %d\r\n",err);
//        TRACE("Received 0x%x bytes\r\n", actlength);


        if (bitsBeforeCol > 0)
        {
            buf[bytesBeforeCol] >>= bitsBeforeCol;
            buf[bytesBeforeCol] <<= bitsBeforeCol;
            buf[bytesBeforeCol] |= colbyte;
        }

        /* now check for collision */
        if (err == AS3911_COLLISION_ERROR)
        {
//            TRACE("Collision!\n");
            card->collision = TRUE;
            /* read out collision register */
            err = as3911ReadRegister(AS3911_REG_COLLISION_STATUS, &colreg);

            bytesBeforeCol = (colreg >> 4) & 0xf;
            bitsBeforeCol = (colreg >> 1) & 0x7;
            bitsBeforeCol++;
            if (bitsBeforeCol == 8)
            {
                bitsBeforeCol = 0;
                bytesBeforeCol++;
            }
//            TRACE("\r\nBytes before Col 0x%x ", bytesBeforeCol);
//            TRACE("\r\nBits before Col 0x%x\r\n", bitsBeforeCol);
            /* FIXME handle c_pb collision in parity bit */
            /* update NVB. Add 2 bytes for SELECT and NVB itself */
            buf[1] = bytesBeforeCol << 4;
            buf[1] |= bitsBeforeCol;
            /* save the colision byte */
            buf[bytesBeforeCol] <<= (8 - bitsBeforeCol);
            buf[bytesBeforeCol] >>= (8 - bitsBeforeCol);

//            buf[bytesBeforeCol] |= (1<<(bitsBeforeCol-1));

            colbyte = buf[bytesBeforeCol];
//            TRACE("\r\nbuf[1]: 0x%x ", buf[1]);
//            TRACE("\r\nbuf[%02d]: 0x%x ",bytesBeforeCol, buf[bytesBeforeCol]);
        }
        else
        {
//            TRACE("\r\nGot a frame\r\n");
            /* got a frame w/o collision - store the uid and check for CT */

            /* enable CRC while receiving SAK */
//            err = as3911ModifyRegister(AS3911_REG_AUX,
//                                       AS3911_REG_AUX_no_crc_rx, 0x0);
//
//            err = as3911EnableInterrupts(AS3911_IRQ_MASK_CRC);

            /* answer with complete uid and check for SAK. */
//            buf[1] = (actlength + bytesBeforeCol) << 4;
//            buf[0] = cl;
//            TRACE("Request SAK\n");
//            err = iso14443TransmitAndReceive(buf,
//                                            actlength + bytesBeforeCol,
//                                            &card->sak[card->cascadeLevels],
//                                            1,
//                                            &actsaklength);
//            TRACE("Got SAK\n");
//
//            if (as3911GetInterrupt(AS3911_IRQ_MASK_CRC))
//            {
//                /* CRC error... */
//                err = ERR_CRC;
//                goto out;
//            }

            /* disable CRC again */
//            err = as3911ModifyRegister(AS3911_REG_AUX,
//                                       AS3911_REG_AUX_no_crc_rx,
//                                       AS3911_REG_AUX_no_crc_rx);
//
//            err = as3911DisableInterrupts(AS3911_IRQ_MASK_CRC);

//            if (card->sak[card->cascadeLevels] & 0x4)
//            {
//                TRACE("\r\n\r\n Next cascading level \r\n");
//                /* reset variables for next cascading level */
//                bytesBeforeCol = 2;
//                bitsBeforeCol = 0;
//
//                if (0x93 == cl)
//                {
//                    cl = 0x95;
//                }
//                else if (0x95 == cl)
//                {
//                    cl = 0x97;
//                }
//                else
//                {
//                    /* more than 3 cascading levels are not possible ! */
//                    err = ERR_NOMSG;
//                    goto out;
//                }
//            }
//            else
//            {
//                TRACE("\r\n\r\nUID done\r\n");
//                card->cascadeLevels++;
//                break;
//            }
//            card->cascadeLevels++;
//            buf = cscs[card->cascadeLevels];
//            buf[0] = cl;
//            buf[1] = 0x20;
            break;
        } /* no collision detected */

    } while (card->cascadeLevels <= ISO14443A_MAX_CASCADE_LEVELS);

    /* do final checks... */
    DISPBUF(buf, 7, 0);
//    uchar bcc;
//    for (i = 0; i< card->cascadeLevels; i++)
//    {
//
//    }

    /* extract pure uid */
//    switch (card->cascadeLevels)
//    {
//        case 3:
//            AMS_MEMMOVE(card->uid+6, cscs[2]+2, 4);
//            AMS_MEMMOVE(card->uid+3, cscs[1]+3, 3);
//            AMS_MEMMOVE(card->uid+0, cscs[0]+3, 3);
//            card->actlength = 10;
//            break;
//        case 2:
//            AMS_MEMMOVE(card->uid+3, cscs[1]+2, 4);
//            AMS_MEMMOVE(card->uid+0, cscs[0]+3, 3);
//            card->actlength = 7;
//            break;
//        case 1:
//            AMS_MEMMOVE(card->uid+0, cscs[0]+2, 4);
//            card->actlength = 4;
//            break;
//        default:
//            err = ERR_NOTSUPP;
//            goto out;
//    }

out:
    /* clean up a bit */
//    as3911DisableInterrupts(AS3911_IRQ_MASK_CRC);
//    /* enable CRC checking for upcoming commands */
//    as3911ModifyRegister(AS3911_REG_AUX, AS3911_REG_AUX_no_crc_rx, 0x0);
    return err;
}
#endif

#endif

