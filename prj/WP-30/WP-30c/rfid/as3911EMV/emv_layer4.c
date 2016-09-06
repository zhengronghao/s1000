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

/*! \file emv_layer4.c
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV compliant ISO14443-4 data transmission.
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "emv_layer4.h"
//#include "emv_hal.h"
//#include "emv_standard.h"
//
//#include "sleep.h"
#include "wp30_ctrl.h"

#if (defined(EM_AS3911_Module))
#if defined(EMV_AS3911)
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*! Maximum number of retransmission for the layer 4 protocol. */
#define EMV_MAX_RETRANSMISSIONS     2
/*! Maximum number of S(WTX) requests allowed for the PICC during a single layer-4 block transfer. */
#define EMV_MAX_NUM_CONSECUTIVE_SWTX_REQUESTS_AFTER_RETRANSMISSION_REQUESTS 2
/*! ISO14443-4 I-block PCB with the chaining bit set. */
#define EMV_PCB_IBLOCK_CHAINED      0x12
/*! ISO14443-4 I-block PCB with the chaining bit cleared. */
#define EMV_PCB_IBLOCK_UNCHAINED    0x02
/*! ISO14443-4 acknowledge R-block PCB. This constant needs to be xored with the current block number. */
#define EMV_PCB_RACK                0xA2
/*! ISO14443-4 not acknowledge R-block PCB. This constant needs to be xored with the current block number. */
#define EMV_PCB_RNAK                0xB2
/*! ISO14443-4 WTX request or response S-block PCB. This must not be combined with the current block number. */
#define EMV_PCB_SWTX                0xF2
/*! Bitmask for the chaining bit of an ISO14443-4 I-block PCB. */
#define EMV_PCB_CHAINING_BIT_MASK   0x10
/*! Bitmask for the block number of an ISO14443-4 R-block or I-block PCB. */
#define EMV_PCB_BLOCK_NUMBER_MASK   0x01
/*! Bitmask for the WTXM bits of the inf byte of an WTX request or response. */
#define EMV_WTXM_MASK               0x3F
/*!
 *****************************************************************************
 * Maximum allowed value for the WTXM of an WTX request. If a value above
 * EMV_MAX_WTXM is requested, then EMV_MAX_WTXM will be used instead of the
 * requested value.
 *****************************************************************************
 */
#define EMV_MAX_WTXM                59

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

/*!
 *****************************************************************************
 * Retransmission request type to use when data from the PICC is not received
 * correctly (e.g. timeut or transmission error).
 *****************************************************************************
 */
typedef enum EMVRetransmissionRequestType
{
    /*! Retransmissions are requested with an R(ACK) block. */
    EMV_RETRANSMISSION_REQUEST_RACK,
    /*! Retransmissions are requested with an R(NAK) block. */
    EMV_RETRANSMISSION_REQUEST_RNAK
} EMVRetransmissionRequestType_t;

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/

/*! EMV PICC data of the card used for ISO14443-4 communication */
static EmvPicc_t *emvPicc;

/*! ISO14443-4 block number */
static u8 emvBlockNumber;

/*!
 *****************************************************************************
 * Buffer for low level data blocks (R,S and I blocks). This buffer is used
 * for transmission AND reception.
 *****************************************************************************
 */
static u8 emvBlockBuffer[EMV_FSD_MIN_PCD];

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
 * \brief Transceive a single low level block of the ISO1443-4 protocol and
 * employ error handling.
 *
 * Transceive a single low level block of the ISO14443-4 protocol. Error
 * handling is performed according to requirement 10.13. The parameter
 * \a retransmissionRequestType is used to distinguish between error
 * handling after a non chaining block has been received and error handling
 * after a chaining block has beeen received.
 *
 * \param[in] pcb The PCB of the block.
 * \param[in] inf Pointer to the INF field of the block.
 * \param[in] infLength Length of the INF field in bytes.
 * \param[out] response Buffer for the piccs response.
 * \param[in] maxResponseLength Size of the response buffer in bytes.
 * \param[out] responseLength Length of the received response in bytes.
 * \param[in] retransmissionRequestType Defines the PCB Block to use for
 * retransmission requests.
 *
 * \return EMV_ERR_OK: No error, response APDU received.
 * \return EMV_ERR_PROTOCOL: Protocl error during reception of the response
 * APDU.
 * \return EMV_ERR_TRANSMISSION: Transmission error during reception of the
 * response APDU.
 * \return EMV_ERR_TIMEOUT: No response APDU receied, or a timeout occured during
 * reception of the response APDU.
 * \return EMV_ERR_INTERNAL: Internal buffer overflow during reception of the
 * response APDU.
 *****************************************************************************
 */
 static s16 emvTransceiveBlock(u8 pcb, const u8 *inf, size_t infLength,
    u8 *response, size_t maxResponseLength, size_t *responseLength,
    enum EMVRetransmissionRequestType retransmissionRequestType);

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

s16 emvInitLayer4(EmvPicc_t *picc)
{
    emvPicc = picc;
    emvBlockNumber = 0;
    return EMV_ERR_OK;
}

s16 emvTransceiveApdu(const u8 *apdu, size_t apduLength, u8 *response, size_t maxResponseLength, size_t *responseLength)
{
    size_t numApduBytesTransmitted = 0;
    size_t numResponseBytesReceived = 0;
    size_t index = 0;
    size_t piccResponseLength = 0;
    u16 fsc = 0;
    u8 pcb = 0;
    u8 numIBlockRetransmissions = 0;
    s16 error = EMV_ERR_OK;
    bool_t awaitingAnotherIBlock = FALSE;

    switch (emvPicc->fsci)
    {
    case 0: fsc = 16; break;
    case 1: fsc = 24; break;
    case 2: fsc = 32; break;
    case 3: fsc = 40; break;
    case 4: fsc = 48; break;
    case 5: fsc = 64; break;
    case 6: fsc = 96; break;
    case 7: fsc = 128; break;
    case 8: fsc = 256; break;
    default: fsc = 256; break;
    }

    numIBlockRetransmissions = 0;
    while ((apduLength - numApduBytesTransmitted) > (fsc - 3))
    {
        /* I-block with chaining. */
        pcb = EMV_PCB_IBLOCK_CHAINED | emvBlockNumber;

        error = emvTransceiveBlock(pcb, &apdu[numApduBytesTransmitted], fsc - 3,
                emvBlockBuffer, sizeof(emvBlockBuffer), &piccResponseLength, EMV_RETRANSMISSION_REQUEST_RNAK);

        if (EMV_ERR_OK != error)
        {
            /* Pass on errors to higher layer. */
            return error;
        }

        /* PICC response must have a length of 1 (R(ACK)). */
        if (piccResponseLength != 1)
        {
            return EMV_ERR_PROTOCOL;
        }

        /* Response must be a R(ACK) */
        if ((emvBlockBuffer[0] & ~EMV_PCB_BLOCK_NUMBER_MASK) != 0xA2)
        {
            /* The received block is not a proper R(ACK). */
            return EMV_ERR_PROTOCOL;
        }

        /* Check block number of the received R(ACK) */
        if ((emvBlockBuffer[0] & EMV_PCB_BLOCK_NUMBER_MASK) != emvBlockNumber)
        {
            /* Non matching block number. Retransmit last I-block. */
            if (numIBlockRetransmissions >= EMV_MAX_RETRANSMISSIONS)
            {
                /* Too many retransmissions already, report protocol error. */
                return EMV_ERR_PROTOCOL;
            }
            else
            {
                numIBlockRetransmissions++;
                /* Do not toggle the block number and also do not increase the
                 * number of transmitted apdu bytes. This will cause the next I-block
                 * to be equal to the last one.
                 */
            }
        }
        else
        {
            /* Received proper R(ACK) =>  Toggle block number. */
            emvBlockNumber ^= 0x01;
            numApduBytesTransmitted += fsc - 3;
            numIBlockRetransmissions = 0;
        }
    }

    /* Send an I-block without chaining. Either as the last I-block of a chained sequence.
     * or as a single I-block if the APDU is small enough.
     */
    numIBlockRetransmissions = 0;
    while (1)
    {
        pcb = EMV_PCB_IBLOCK_UNCHAINED | emvBlockNumber;

        error = emvTransceiveBlock(pcb, &apdu[numApduBytesTransmitted]
                                , apduLength - numApduBytesTransmitted, emvBlockBuffer, sizeof(emvBlockBuffer)
                                , &piccResponseLength, EMV_RETRANSMISSION_REQUEST_RNAK);

        if (EMV_ERR_OK != error)
        {
            /* Pass on errors to higher layer. */
            return error;
        }

        /* If the response is an R(ACK) block, then we must check whether a retransmission
         * has to be performed.
         */
        if ((emvBlockBuffer[0] & ~EMV_PCB_BLOCK_NUMBER_MASK) != EMV_PCB_RACK)
        {
            /* The received block is not an R(ACK).
             * Break the while loop and continue with the handling of the response I-block.
             */
            break;
        }

        /* Check block number of the received R(ACK) */
        if ((emvBlockBuffer[0] & EMV_PCB_BLOCK_NUMBER_MASK) != emvBlockNumber)
        {
            /* Non matching block number. Retransmit last I-block. */
            if (numIBlockRetransmissions >= EMV_MAX_RETRANSMISSIONS)
            {
                /* Too many retransmissions already, report protocol error. */
                return EMV_ERR_PROTOCOL;
            }
            else
            {
                numIBlockRetransmissions++;
                /* Do not toggle the block number and also do not increase the
                 * number of transmitted apdu bytes. This will cause the next I-block
                 * to be equal to the last one.
                 */
            }
        }
        else
        {
            /* Received an R(ACK) with correct block number as response to an unchained
             * I-block. This is a protocol error.
             */
            return EMV_ERR_PROTOCOL;
        }
    }

    /* Handle the answer of the card to the APDU. */
    do
    {
        /* Pass on errors to the higher layer. */
        if (EMV_ERR_OK != error)
            return error;

        /* An I-block must have a size > 0. */
        if (0 == piccResponseLength)
            return EMV_ERR_PROTOCOL;

        /* The received block must be a proper I-block. */
        if (  ((emvBlockBuffer[0] & ~EMV_PCB_BLOCK_NUMBER_MASK) != EMV_PCB_IBLOCK_UNCHAINED)
           && ((emvBlockBuffer[0] & ~EMV_PCB_BLOCK_NUMBER_MASK) != EMV_PCB_IBLOCK_CHAINED))
            return EMV_ERR_PROTOCOL;

        /* The received block must have a correct block number. */
        if ((emvBlockBuffer[0] & EMV_PCB_BLOCK_NUMBER_MASK) != emvBlockNumber)
            return EMV_ERR_PROTOCOL;

        /* Received proper I-block => toggle block number. */
        emvBlockNumber ^= 0x01;

        /* Check for response buffer overflow */
        if (numResponseBytesReceived + piccResponseLength > maxResponseLength) {
            /* Copy as much bytes as possible into the response buffer. */
            for (index = 0; index < maxResponseLength - numResponseBytesReceived; index++)
                response[numResponseBytesReceived + index] = emvBlockBuffer[1 + index];

            /* Signal buffer overflow to caller. */
            return EMV_ERR_INTERNAL;
        }

        for (index = 0; index < piccResponseLength - 1; index++)
            response[numResponseBytesReceived + index] = emvBlockBuffer[1 + index];
        numResponseBytesReceived += index;

        if (emvBlockBuffer[0] & EMV_PCB_CHAINING_BIT_MASK)
        {
            /* Chaining bit set. Send R(ACK) and receive next I-block. */
            emvBlockBuffer[0] = EMV_PCB_RACK | emvBlockNumber;

            error = emvTransceiveBlock(emvBlockBuffer[0], NULL, 0, emvBlockBuffer, sizeof(emvBlockBuffer),
                        &piccResponseLength, EMV_RETRANSMISSION_REQUEST_RACK);
            awaitingAnotherIBlock = TRUE;
        }
        else
            awaitingAnotherIBlock = FALSE;

    } while (awaitingAnotherIBlock); /* Continue while the chained receive is in progress. */

    *responseLength = numResponseBytesReceived;
    return EMV_ERR_OK;
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

static s16 emvTransceiveBlock(u8 pcb, const u8 *inf, size_t infLength,
    u8 *response, size_t maxResponseLength, size_t *responseLength,
    enum EMVRetransmissionRequestType retransmissionRequestType)
{
    size_t index = 0;
    s16 error = EMV_HAL_ERR_OK;
    u8 numRetransmissions = 0;
    u8 numConsecutiveSwtxRequestsAfterRnakReceived = 0;
    bool_t lastBlockWasRetransmissionRequest = FALSE;
    u32 fwtInCarrierCycles = 0;

    if (infLength + 3 > sizeof(emvBlockBuffer))
    {
        /* The given block is bigger than the internal buffer.
         * This should actually never happen.
         */
        return EMV_ERR_INTERNAL;
    }
    
    /* Construct layer 4 low level block */
    emvBlockBuffer[0] = pcb;
    for (index = 0; index < infLength; index++)
        emvBlockBuffer[1 + index] = inf[index];

    /* Calculate frame wait time */
    fwtInCarrierCycles = (4096L + 384) << emvPicc->fwi;

    error = emvHalTransceive(emvBlockBuffer, infLength + 1, response,
        maxResponseLength , responseLength, fwtInCarrierCycles,
        EMV_HAL_TRANSCEIVE_WITH_CRC);

    numRetransmissions = 0;
    numConsecutiveSwtxRequestsAfterRnakReceived = 0;
    lastBlockWasRetransmissionRequest = FALSE;
    while (numRetransmissions < EMV_MAX_RETRANSMISSIONS)
    {
        /* Mesages larger than the reported reader buffer size (FSD) must be
         * treated as a protocol error.
         * Note: There have been diverting statements from FIME regarding
         * treatment of recieved block exceeding FSD. One statement was to
         * tread it as a protocol error (which implies that no further
         * retransmissions are requested). Another statement was to treat this
         * as a transmission error which might require a retransmission request
         * to be send.
         *
         * This implementation treats too long messages as protocol error.
         */
        if (*responseLength > EMV_FSD_MIN_PCD)
            return EMV_ERR_PROTOCOL;
        else if (EMV_HAL_ERR_OVERFLOW == error)
            return EMV_ERR_PROTOCOL;
        else if (EMV_HAL_ERR_OK == error)
        {
            if (response[0] == 0xF2)
            {
                /* Handle frame wait extension requests. */
                u8 requestedWtxm = 0;
                u8 usedWtxm = 0;
                u8 swtxResponse[2];
                u32 fwtInCarrierCycles = 0;

                /* The EMV standard limits the number of consecutive sequences of
                 * 'retransmission request followed by S(WTX) request' but there is
                 * no limit to the number of S(WTX) requests received after any
                 * other block.
                 */
                if (lastBlockWasRetransmissionRequest)
                    numConsecutiveSwtxRequestsAfterRnakReceived++;
                else
                    numConsecutiveSwtxRequestsAfterRnakReceived = 0;

                /* According to FIME reception of an S(WTX) request counts as reception of a correct frame.
                 * Thus the counter for consecutive transmission errors must be reseted.
                 */
                numRetransmissions = 0;

                /* Check for proper S(WTX) request size (2 byte + 2 bytes CRC). */
                if (*responseLength != 2 + 2)
                    return EMV_ERR_PROTOCOL;

                /* An excess of S(WTX) requests after retransmission requests must
                 * be treated as timeout error.
                 * See 10.3.5.5. The explanation from this clause also applies to
                 * 10.3.5.8 although consecutive S(WTX) requests are not mentioned there.
                 */
                if (numConsecutiveSwtxRequestsAfterRnakReceived > EMV_MAX_NUM_CONSECUTIVE_SWTX_REQUESTS_AFTER_RETRANSMISSION_REQUESTS)
                    return EMV_ERR_TIMEOUT;

                requestedWtxm = response[1] & EMV_WTXM_MASK;

                /* Check validity of wtxm value and adjust range. */
                if (requestedWtxm == 0)
                    return EMV_ERR_PROTOCOL;
                else if (requestedWtxm > EMV_MAX_WTXM)
                    usedWtxm = EMV_MAX_WTXM;
                else
                    usedWtxm = requestedWtxm;

                /* Calculate requested frame wait time in carrier cycles. */
                fwtInCarrierCycles = ((4096L + 384) << emvPicc->fwi) * usedWtxm;

                /* In the EMV standard there is no clear definition on what to do
                 * if the requested FWT_TEMP exceeds FWT_MAX.
                 * But, according to FIME the PCD shall limit FWT_TEMP to FWT_MAX if that
                 * happens.
                 */
                if (fwtInCarrierCycles > ((4096L + 384) << EMV_FWI_MAX_PCD))
                    fwtInCarrierCycles = ((4096L + 384) << EMV_FWI_MAX_PCD);

                swtxResponse[0] = 0xF2;
                swtxResponse[1] = requestedWtxm;

                lastBlockWasRetransmissionRequest = FALSE;
                error = emvHalTransceive(&swtxResponse[0], 2 , response, maxResponseLength
                    , responseLength, fwtInCarrierCycles, EMV_HAL_TRANSCEIVE_WITH_CRC);
                /* ToDo: Check whether S(WTX) pairs are counted as retransmissions. */
            }
            else
            {
                /* The CRC bytes are not reported in the response length. */
                *responseLength -= 2;
                return EMV_ERR_OK;
            }
        }
        else
        {
            /* Check for a timeout after consecutive R(NAK), S(WTX), S(WTX) response chains.
             * See PCD 10.3.5.5 for details.
             *
             * Note: The >= is there for a reason. Please think _twice_ before changing it into >.
             */
            if ((EMV_HAL_ERR_TIMEOUT == error)
               && (numConsecutiveSwtxRequestsAfterRnakReceived >= EMV_MAX_NUM_CONSECUTIVE_SWTX_REQUESTS_AFTER_RETRANSMISSION_REQUESTS))
                return EMV_ERR_TIMEOUT;

            /* All (other) error conditions lead to a retransmission request. */
            if (EMV_RETRANSMISSION_REQUEST_RACK == retransmissionRequestType)
                emvBlockBuffer[0] = EMV_PCB_RACK | emvBlockNumber;
            else if(EMV_RETRANSMISSION_REQUEST_RNAK == retransmissionRequestType)
                emvBlockBuffer[0] = EMV_PCB_RNAK | emvBlockNumber;
            else
                return EMV_ERR_INTERNAL;

            lastBlockWasRetransmissionRequest = TRUE;
            error = emvHalTransceive(emvBlockBuffer, 1, response, maxResponseLength
                , responseLength, fwtInCarrierCycles, EMV_HAL_TRANSCEIVE_WITH_CRC);
            numRetransmissions++;
        }
    }

    /* Blocks greater than the FSD must be considered as a protocol error. */
    if (*responseLength > EMV_FSD_MIN_PCD)
        return EMV_ERR_PROTOCOL;
    else if (EMV_HAL_ERR_OVERFLOW == error)
        return EMV_ERR_PROTOCOL;
    else if (EMV_HAL_ERR_OK == error)
    {
        /* The last retransmitted block is not allowed to be an S(WTX) request. */
        if (response[0] == EMV_PCB_SWTX)
            return EMV_ERR_TIMEOUT;
        /* The CRC bytes are not reported as part of the repsonse length. */
        *responseLength -= 2;
        return EMV_ERR_OK;
    }
    else if (EMV_HAL_ERR_TIMEOUT == error)
        return EMV_ERR_TIMEOUT;
    else
        return EMV_ERR_TRANSMISSION;
}

#endif
#endif

