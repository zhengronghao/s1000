#ifndef MIFARE_RAW_REQUEST_H
#define MIFARE_RAW_REQUEST_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
//#include "platform.h"
//#include "mifare_parity_data_t.h"
#include "wp30_ctrl.h"
#include "mifare_parity_data_t.h"

#if (defined(EM_AS3911_Module))
/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/
/*!
 *****************************************************************************
 * \brief Send a raw Mifare request.
 *
 * Send a raw MiFare request.
 *
 * \note: No more than \a maxResponseLength bytes are written into
 * the response buffer. If more than \a maxResponseLength bytes are
 * received a buffer overflow error will be returned.
 *
 * \note: Receiving a 4 bit response while 4 bit response receiving
 * is not enabled will lead to the returned number of received bytes being
 * zero and no data being placed into the response buffer.
 *
 * \note: Receiving 4 bit response is disabled right now and the parameter
 * \a fourBitResponse will not have any effect!
 *
 * \param request Pointer to the array storing the request.
 * \param requestLength Length of the request.
 * \param response Pointer to the array where the received response is stored.
 * \param maxResponseLength Size of the response buffer in bytes.
 * \param *responseLength Set to the length of the received response in bytes.
 * \param timeout Timeout in milliseconds.
 * \param fourBitRespnose If this parameter is \a TRUE, then
 *        receiving of four bit responses is enabled.
 *
 * \return Returns the number of received bytes or a negative error code.
 *****************************************************************************
 */
s8 mifareSendRawRequest(const parity_data_t *request,
        u16 requestLength,
        u8 *response,
        u16 maxResponseLength,
        u16 *responseLength,
        u32 timeout,
        uchar fourBitResponse);

void as3911TransmitMif(u8 *message, size_t messageLength, size_t bits, AS3911RequestFlags_t requestFlags);

#endif /* MIFARE_RAW_REQUEST_H */
#endif

