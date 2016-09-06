#ifndef MIFARE_H
#define MIFARE_H

/*!
  \file
  Mifare Classic implmementation taken from Karsten Nohl, Henryk Ploetz and Sean O'Neil.
  This no complete implementation but shows that AS3911 is able to correctly talk to 
  Mifare Classic cards.
*/

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
//#include "platform.h"
#include "wp30_ctrl.h"

#if (defined(EM_AS3911_Module))
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*! Size of the intermal MiFare transmit buffer. FIXME: Should be reduced to 18 */
#define MIFARE_TRANSMIT_BUFFER_SIZE         64
#define MIFARE_TRANSCEIVE_DEFTIME           678000 //д╛хо50ms

#define MIFARE_AUTH_KEY_A                   0x60
#define MIFARE_AUTH_KEY_B                   0x61
#define MIFARE_READ_BLOCK                   0x30
#define MIFARE_WRITE_BLOCK                  0xA0
#define MIFARE_ACK                          0x0A
#define MIFARE_NACK_NOT_ALLOWED             0x04
#define MIFARE_NACK_TRANSMISSION_ERROR      0x04


//anticollision
#define ISO14443A_MAX_UID_LENGTH 10
#define ISO14443A_MAX_CASCADE_LEVELS 3
#define ISO14443A_CASCADE_LENGTH 7
#define ISO14443A_RESPONSE_CT  0x88

typedef struct
{
    u8 uid[ISO14443A_MAX_UID_LENGTH]; /*<! UID of the PICC */
    u8 actlength; /*!< actual UID length */
    u8 atqa[2]; /*!< content of answer to request byte */
    u8 sak[ISO14443A_MAX_CASCADE_LEVELS]; /*!< SAK bytes */
    u8 cascadeLevels; /*!< number of cascading levels */
    bool_t collision; /*!< TRUE, if there was a collision which has been resolved,
                        otherwise no collision occured */
}iso14443AProximityCard_t;
/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/
s8 mifareInitialize();
s8 mifareDeinitialize(u8 keep_on);

/*!
 *****************************************************************************
 * Set the MiFare key.
 *
 * \note: The array pointed to by \a key must be at least 6 bytes long.
 *
 * \param key Pointer to an array storing the key.
 *****************************************************************************
 */
void mifareSetKey(const u8 *key);

/*!
 *****************************************************************************
 * \brief Reset the MiFare cipher.
 *
 * Reset the MiFare crypto1 engine and deactive encryption/decryption for
 * communications. This must be called when prior to communicating with a
 * new card.
 *****************************************************************************
 */
void mifareResetCipher();

/*!
 *****************************************************************************
 * \brief Perform a complete MiFare authentication.
 *
 * This function provides a convenient interface to MIFARE_authenticateStep1 and
 * MIFARE_authenticateStep2.
 * The reader nonce used in the second authentication step is defined by
 * MIFARE_DEFAULT_READER_NONCE.
 *
 * \param keySelect Select key for authentication. 0x60 selects key A and 0x61 selects key B.
 * \param block Block to authenticate for.
 * \param uid UID of the card we authenticate to.
 * \param uidLength Length of the UID pointed to by \a uid in bytes.
 * \param key Pointer to the key.
 *
 * \return 0 if the authentication step was successfull. Otherwise a negative error code is returend.
 *****************************************************************************
 */
s8 mifareAuthenticate(u8 key_select,
        u8 block,
        const u8 *uid,
        u8 uidLength,
        const u8 *key);

/*!
 *****************************************************************************
 * \brief First step of mifare authentication (reader side).
 *
 * \Note The array pointed to by \a uid must be at least 4 bytes long.
 *
 * \param keySelect Select key for authentication. 0x60 selects key A and
 *        0x61 selects key B.
 * \param block Block to authenticate for.
 * \param uid UID of the card we authenticate to.
 * \param uidLength Length of the UID pointed to by \a uid in bytes.
 * \param key Pointer to the key.
 *
 * \return ERR_NONE: The authentication step was successfull.
 * \return ERR_PARAM: UID too short.
 *****************************************************************************
 */
s8 mifareAuthenticateStep1(u8 key_select,
        u8 block,
        const u8 *uid,
        u8 uidLength,
        const u8 *key);

s8 mifareAuthenticateStep2(u32 readerNonce);

s8 mifareSendRequest(const u8 *request,
        u16 requestLength,
        u8 *response,
        u16 maxResponseLength,
        u16 *responseLength,
        u32 timeout,
        uchar four_bit_response);

u8 as3911AntiCollision(uchar bitcol, uchar ucSEL, uchar * pucUID, uint timeout);

#endif /* MIFARE_H */
#endif

