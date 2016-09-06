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

/*! \file emv_typeA.c
 *
 * \author Oliver Regenfelder
 *
 * \brief Type A specific PICC functions.
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include <stddef.h>

//#include "emv_hal.h"
//#include "emv_prelayer4.h"
//#include "emv_main.h"
//#include "emv_typeA.h"
//#include "emv_standard.h"
//#include "emv_error_codes.h"
#include "wp30_ctrl.h"

#if (defined(EM_AS3911_Module))
#if defined(EMV_AS3911)
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*!
 *****************************************************************************
 * \brief Frame wait time for n = 9 frames of the ISO14443A protocol.
 * 
 * The frame wait time depends on the last
 * transmitted bit, therefore the higher value (last transmitted
 * bit is asumed to be one) has beent taken (1236 carrier cycles)
 * with an additional 14 cycles safety margin.
 *****************************************************************************
 */
#define EMV_TYPEA_FDT_9     1250

/*! Bit mask for the UID size field of the first ATQA byte. */
#define EMV_ATQA_UID_SIZE_MASK      0xC0
/*! ATQA UID size field value for a single size UID. */
#define EMV_ATQA_UID_SIZE_SINGLE    0x00
/*! ATQA UID size field value for a double size UID. */
#define EMV_ATQA_UID_SIZE_DOUBLE    0x40
/*! ATQA UID size field value for a triple size UID. */
#define EMV_ATQA_UID_SIZE_TRIPLE    0x80
/*!
 *****************************************************************************
 * Invalid ATQA UID size field value. A standard conforming card
 * should never return this value.
 *****************************************************************************
 */
#define EMV_ATQA_UID_SIZE_INVALID   0xC0

/*! Value of the SEL byte of a cascade level 1 anticollision or select request.*/
#define EMV_SEL_CL1     0x93
/*! Value of the SEL byte of a cascade level 2 anticollision or select request.*/
#define EMV_SEL_CL2     0x95
/*! Value of the SEL byte of a cascade level 3 anticollision or select request.*/
#define EMV_SEL_CL3     0x97

/*! NVB (number of valid bits) byte value for an ISO14443-A anticollision request. */
#define EMV_ANTICOLLISION_NVB   0x20
/*! NVB (number of valid bits) byte value for an ISO14443-A select request. */
#define EMV_SELECT_NVB          0x70

/*! ISO14434-A anticollision response cascade tag value. */
#define EMV_CASCADE_TAG         0x88
/*! Mask for the cascade bit of an ISO14443-A SAK. */
#define EMV_SAK_CASCADE_BIT_MASK                0x04
/*! Mask for the ISO14443-4 compliance bit fo an ISO14443-A SAK. */
#define EMV_SAK_ISO144434_COMPLIANT_BIT_MASK    0x20

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
 * \brief Perform a cascade level 1,2, or 3 anticollision and select sequence.
 *
 * \param[in] sel Value of the SEL byte of the anticollision and the select
 * sequence. Use one of the following constants: \n
 * EMV_SEL_CL1: anticollision/select cascade level 1 \n
 * EMV_SEL_CL2: anticollision/select cascade level 2 \n
 * EMV_SEL_CL3: anticollision/select cascade level 3 \n
 * 
 * \param[in] cascaded
 *   TRUE: This is a cascaded anticollision/select sequence. \n
 *   FALSE: This is a non cascaded anticollision/select sequence.
 * \param[in] uid Buffer to store the received uid part. For a cascaded request
 *  this buffer must be able to store 3 bytes. For a non cascaded request this
 *  buffer must be able to store 4 bytes.
 *****************************************************************************
 */
static s16 emvAnticollisionLevelx(u8 sel, bool_t cascaded, u8 *uid);

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

s16 emvTypeACardPresent()
{
    u8 atqa[2];
    size_t responseLength = 0;

    emvHalSetStandard(EMV_HAL_TYPE_A);
    emvHalSetErrorHandling(EMV_HAL_PREACTIVATION_ERROR_HANDLING);

    /* Send WUPA command. */
    s8 error = emvHalTransceive(NULL, 0, &atqa[0], sizeof(atqa), &responseLength, EMV_TYPEA_FDT_9, EMV_HAL_TRANSCEIVE_WUPA);

    /* Any response shall be taken as a card presence indication. */
    if (EMV_HAL_ERR_TIMEOUT == error)
        return 0;
    else{
        if ( EMV_HAL_ERR_OK == error ) {
            gemvcardinfo.ATQA[0] = atqa[0];
            gemvcardinfo.ATQA[1] = atqa[1];
        }
        return 1;
    }
}

s16 emvTypeAAnticollision(EmvPicc_t *picc)
{
    s8 error = EMV_ERR_OK;
    u8 atqa[2];
    size_t responseLength = 0;

    /* Set ISO14443-A mode. */
    emvHalSetStandard(EMV_HAL_TYPE_A);
    emvHalSetErrorHandling(EMV_HAL_PREACTIVATION_ERROR_HANDLING);

    /* Send WUPA command. */
    error = emvPrelayer4Transceive(NULL, 0, &atqa[0], sizeof(atqa), &responseLength,
                EMV_TYPEA_FDT_9, EMV_HAL_TRANSCEIVE_WUPA);

    if (error == EMV_ERR_TIMEOUT)
        return EMV_ERR_TIMEOUT;
    else if (error == EMV_ERR_STOPPED)
        return EMV_ERR_STOPPED;
    else if (error != EMV_ERR_OK)
        return EMV_ERR_COLLISION;
    else if (responseLength != 2)
        return EMV_ERR_COLLISION;

    /* Check correctness of ATQA. */
    /* Check correctness of UID size encoding. */
    if ((atqa[0] & EMV_ATQA_UID_SIZE_MASK) == EMV_ATQA_UID_SIZE_INVALID)
        return EMV_ERR_PROTOCOL;

    /* Check correctness of bit frame anticollision bits. */
    {
        u8 numBitsSetInAnticollisionBits = 0;
        u8 bitMask;

        for (bitMask = 0x01; bitMask != 0x20; bitMask <<= 1)
        {
            if (atqa[0] & bitMask)
                numBitsSetInAnticollisionBits++;
        }
        if (numBitsSetInAnticollisionBits != 1)
            return EMV_ERR_PROTOCOL;
    }

    /* Check correctness of ATQA[1] high nibble.
     * All bits of ATQA[1] are either RFU bits or defined as any value.
     * Therefore no special checking of ATQA[1] is required.
     */

    /* Install PICC activation and remove handlers.
     */
    picc->activate = emvTypeAActivation;
    picc->remove = emvTypeARemove;

    if ((atqa[0] & EMV_ATQA_UID_SIZE_MASK) == EMV_ATQA_UID_SIZE_SINGLE)
    {
        picc->uidLength = 4;
        return emvAnticollisionLevelx(EMV_SEL_CL1, FALSE, &picc->uid[0]);
    }
    else
    {
        error = emvAnticollisionLevelx(EMV_SEL_CL1, TRUE, &picc->uid[0]);
        if (error != EMV_ERR_OK)
            return error;
    }

    if ((atqa[0] & EMV_ATQA_UID_SIZE_MASK) == EMV_ATQA_UID_SIZE_DOUBLE)
    {
        picc->uidLength = 7;
        return emvAnticollisionLevelx(EMV_SEL_CL2, FALSE, &picc->uid[3]);
    }
    else
    {
        error = emvAnticollisionLevelx(EMV_SEL_CL2, TRUE, &picc->uid[3]);
        if (error != EMV_ERR_OK)
            return error;
    }

    if ((atqa[0] & EMV_ATQA_UID_SIZE_MASK) == EMV_ATQA_UID_SIZE_TRIPLE)
    {
        picc->uidLength = 10;
        return emvAnticollisionLevelx(EMV_SEL_CL3, FALSE, &picc->uid[6]);
    }
    else
    {
        /* This branch should never be reached because the validity of
         * the ATQA uid size field is already checked before the
         * anticollision sequence starts.
         */
        return EMV_ERR_INTERNAL;
    }
}

s16 emvTypeAActivation(EmvPicc_t *picc)
{
    u8 rats[2];
    u8 ats[22];
    u8 t0 = 0;
    s8 error = EMV_ERR_OK;
    size_t responseLength = 0;

    /* tx_byte first points to the potential location of TA(1)
     * then to the potential location of TB(1) and at last to
     * the potential location of TC(1).
     */
    u8 *tx_byte = &ats[2];

    emvHalSetErrorHandling(EMV_HAL_LAYER4_ERROR_HANDLING);

    rats[0] = 0xE0;
    rats[1] = EMV_FSDI_MIN_PCD << 4;

    error = emvPrelayer4Transceive(rats, 2, ats, sizeof(ats), &responseLength 
        , EMV_FWT_ACTIVATION_PCD, EMV_HAL_TRANSCEIVE_WITH_CRC);

    /* Report errors back to higher layer. */
    /* The response must at least contain the TL byte (and the CRC). */
    if (error != EMV_ERR_OK)
        return error;
    else if (responseLength < (1 + 2))
        return EMV_ERR_PROTOCOL;

    /* Check TL (length byte) for correctness. */
    if ((responseLength - 2) != ats[0])
        return EMV_ERR_PROTOCOL;

    if ( error == EMV_ERR_OK ) {
        memcpy(gemvcardinfo.ATS, ats, responseLength - 2);
        gemvcardinfo.cardtype = 1;
    }

    /* Check for the presence of the T0 byte. If no T0 byte is present
     * use a default T0 byte which specifies no TA, TB, and TC bytes and
     * the default FSCI value to be used. */
    if (ats[0] == 0x01)
        t0 = EMV_FSCI_DEFAULT;
    else
        t0 = ats[1];

    /* Set PICC struct default values. */
    picc->dPiccPcd = EMV_D_PICC_PCD_DEFAULT;
    picc->dPcdPicc = EMV_D_PCD_PICC_DEFAULT;
    picc->sfgi = EMV_SFGI_DEFAULT;
    picc->fwi = EMV_FWI_DEFAULT;

    /* Parse and check FSCI. */
    picc->fsci = t0 & 0x0F;

    if (picc->fsci < EMV_FSCI_MIN_PCD)
        return EMV_ERR_PROTOCOL;
    else if (picc->fsci > 8)
        picc->fsci = 8;

    /* Parse TA(1) (datarate flags). */
    if (t0 & 0x10)
    {
        picc->dPcdPicc = *tx_byte & 0x07;
        picc->dPiccPcd = (*tx_byte >> 4) & 0x07;
        tx_byte++;
    }

    /* Parse TB(1) (FWI, SFGI). */
    if (t0 & 0x20)
    {
        picc->sfgi = *tx_byte & 0x0F;
        picc->fwi = *tx_byte >> 4;

        if(picc->sfgi > EMV_SFGI_MAX_PCD)
            return EMV_ERR_PROTOCOL;

        if(picc->fwi > EMV_FWI_MAX_PCD)
            return EMV_ERR_PROTOCOL;

        tx_byte++;
    }

    return EMV_ERR_OK;
}

s16 emvTypeARemove(EmvPicc_t *picc)
{
    u8 hlta[2] = { 0x50, 0x00 };
    /* A WUPA is considered successfull during removal if NO repsonse
     * of any kind is received.
     */
    u8 numWupaWithoutResponse;
    /* Reset operating field. */

    /* Do not ignore any transmission errors. */
    emvHalSetErrorHandling(EMV_HAL_PREACTIVATION_ERROR_HANDLING);

    /* Reset operating field. */
    emvHalResetField();

    /* Wait t_p milliseconds. */
    emvHalSleepMilliseconds(EMV_T_P);
    
    numWupaWithoutResponse = 0;
    while (numWupaWithoutResponse < 3)
    {
        if ( IfInkey(0)) {
            Dprintk("--type a remove 99 to exit");
            if ( InkeyCount(0) == 99 ) {
                /* Received stop request, stop terminal main loop. */
                return EMV_ERR_STOPPED;
            }
        }

        if (emvStopRequestReceived())
            return EMV_ERR_STOPPED;

        if (emvTypeACardPresent())
        {
            /* Send hlta command. */
            emvHalTransceive(hlta, 2, NULL, 0, NULL, EMV_TYPEA_FDT_9, EMV_HAL_TRANSCEIVE_WITH_CRC);
            /* Wait t_p milliseconds. */
            emvHalSleepMilliseconds(EMV_T_P);

            numWupaWithoutResponse = 0;
        }
        else
            numWupaWithoutResponse++;
    }

    return EMV_ERR_OK;
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

static s16 emvAnticollisionLevelx(u8 sel, bool_t cascaded, u8 *uid)
{
    s16 error = EMV_ERR_OK;
    u8 index = 0;
    u8 command[7];
    u8 response[5];
    u8 bcc = 0;
    size_t responseLength = 0;

    /* Send CLx anticollision command. */
    command[0] = sel;
    command[1] = EMV_ANTICOLLISION_NVB;

    error = emvPrelayer4Transceive(command, 2, response, sizeof(response),
                &responseLength, EMV_TYPEA_FDT_9, EMV_HAL_TRANSCEIVE_WITHOUT_CRC);

    /* Any transmission error shall be considered a collision. */
    if (EMV_ERR_STOPPED == error)
        return EMV_ERR_STOPPED;
    else if ((EMV_ERR_OK != error) || (responseLength != 5))
        return EMV_ERR_COLLISION;

    /* Verify the BCC.
     */
    bcc = 0;
    for (index = 0; index < 4; index++)
        bcc ^= response[index];
    if (response[4] != bcc)
        return EMV_ERR_TRANSMISSION;

    if (cascaded)
    {
        /* The first uid byte of a cascaded request must be the cascade tag. */
        if (EMV_CASCADE_TAG != response[0])
            return EMV_ERR_PROTOCOL;

        for (index = 0; index < 3; index++)
            uid[index] = response[1+index];
    }
    else
    {
        /* The first uid byte of a non cascaded request must not be the cascade tag. */
        if (EMV_CASCADE_TAG == response[0])
            return EMV_ERR_PROTOCOL;

        for (index = 0; index < 4; index++)
            uid[index] = response[index];
    }

    /* Send CLx select command. */
    command[0] = sel;
    command[1] = EMV_SELECT_NVB;

    for (index = 0; index < 5; index++)
        command[2 + index] = response[index];

    error = emvPrelayer4Transceive(command, 7, response, sizeof(response), &responseLength,
                EMV_TYPEA_FDT_9, EMV_HAL_TRANSCEIVE_WITH_CRC);

    /* SAK responses of invalid length should be treated as protocol error.
     * (see EMV PCD Digital Test Bench & Test Cases v2.0.1a test cases
     * TA305.11 and TA305.12.
     */
    if (EMV_ERR_STOPPED == error)
        return EMV_ERR_STOPPED;
    else if (EMV_ERR_OK != error)
        return error;
    else if (responseLength != (1 + 2))
        return EMV_ERR_PROTOCOL;

    if (cascaded)
    {
        /* The SAK of a cascaded request must have the cascade bit set. */
        if (!(response[0] & EMV_SAK_CASCADE_BIT_MASK))
            return EMV_ERR_PROTOCOL;
    }
    else
    {
        /* The SAK of a non cascaded request must not have the cascade bit set. */
        if (response[0] & EMV_SAK_CASCADE_BIT_MASK)
            return EMV_ERR_PROTOCOL;

        /* The SAK of a non cascaded request must have the ISO14443-4 compliant bit set. */
        //mifare 卡 此bit有可能为0
//        if (!(response[0] & EMV_SAK_ISO144434_COMPLIANT_BIT_MASK))
//            return EMV_ERR_PROTOCOL;
    }

    return EMV_ERR_OK;
}

#endif
#endif

