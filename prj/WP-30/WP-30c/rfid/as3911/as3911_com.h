/*
 *****************************************************************************
 * Copyright @ 2009                                *
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
 
/*! \file as3911_com.h
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 RFID communication
 */

/*! \defgroup as3911RfidCom AS3911 RFID Communication
 * \ingroup as3911
 *
 * \brief This part of the AS9311 module abstracts Communication with an
 * ISO14443-A or ISO14443-B card including features required for EMV.
 *
 * This part of the AS3911 module consists of functions to configure
 * paramters of the datatarnsmission to and from an RFID card and functions to
 * actually perfrom the transmissions.
 *
 * \section sec_as3911RfidCom_1 Receiver Deadtime
 *
 * After the data transmission from the reader is completed the receivers will
 * be kept disbabled until the receiver dead time has expired. This dead time
 * ensures that the receiver does not pick up noise still present from the
 * transmit operation.
 *
 * For EMV the standard specifies that the PCD must ignore any load modulation
 * produced by the PICC for a certain time immediately after the completion of
 * the PCD message. The receveiver dead time can be used to implement this EMV
 * deaf time.
 *
 * \section sec_as3911RfidCom_2 Frame Delay Time
 *
 * The frame delay time is a forced delay between the end of the last message
 * received from a card and the beginning of the next message send by the PCD.
 *
 * If a frame delay time greater than zero is configured then as3911Transmit()
 * and as3911Transceive() make sure that this frame delay time is kept. If
 * necessary they will wait with starting the data transmission until the set
 * frame delay time has passed.
 *
 * If no card response was received after the last message transmitted by the
 * PCD (as3911Receive() or as3911Transceive() returned with a timeout error),
 * then no frame delay time will be applied to the next data transmission.
 *
 * \section sec_as3911RfidCom_3 Receive Timeout
 *
 * The receive timeout specifies the time the AS3911 will wait for a card
 * response after it has completed the transmission of the reader message. After
 * the
 * receive timeout expires the receiver will be disabled internally and no
 * further card responses can be received. If a receive timeout occures while
 * a reception is in progress, then this reception continues until the message
 * from the card has been completely received. So the receive timeout specifies
 * a timeout for the start of the response from the RFID card.
 *
 * If as3911Transceive is used then the receive timeout can be specified as a
 * paramter to the functino call. If as3911Transmit() and as3911Receive() are
 * used seperately then the receive timeout must be set via
 * as3911SetReceiveTimeout() prior to the call to as3911Transmit().
 *
 * The currently active receive timeout is used until a new receive timeout is
 * set.
 *
 * \section sec_as3911RfidCom_4 Error Handling
 *
 * To support EMV special error handling capabilities are build into the AS3911
 * RFID communication code.
 * 
 * If EMV error handling is disabled, then no special
 * error handling takes place and any reception errors are reported via the
 * return values of as3911Transmit() and as3911Transceive() respectively.
 *
 * If EMV error handling is enabled then a second parameter defines the details
 * of the error handling namely the transmission error threshold. If a
 * transmission error (parity error, crc error, error, data encoding
 * error, or frame timing error)occures during reception of a frame whose overall
 * length is less than or equal to the tranmsission error threshold, then this
 * received frame is ignored and the receiver is reenabled to receive another
 * card response. Also when EMV error handling is enabled then any frame with
 * residual bits will be ignored no matter how long that frame was.
 *
 * This is handled transparent to the caller. I.e. the caller will not be
 * informed that an errornous frame has been ignored. Instead he will either
 * get a timeout error reported if no further frame follows the ingored frame,
 * or he will be informed about the content and error status of any frame
 * received subsequent to the ignored one.
 *
 * The transmission error threshold is compared against the
 * received message length as reported by the AS3911. This length may
 * or may not include the CRC bytes depending on the selected CRC processing
 * mode (see usage of ::AS3911RequestFlags_t in as3911Transceive()).
 *
 * \section sec_as3911RfidCom_5 Modulation Depth Adjustment
 *
 * The AS3911 RFID communication module can adjust the modulation depth for
 * the PCD to PICC communication basedon the
 * the measured RF amplitude (on the RFI pins) or on the phase difference
 * between the RFO and RFI pins. Both of these parameters depend on the coupling
 * between the PCD and PICC antennas. So this provides a method to adjust the
 * modulation depth based on the strength of the coupling between the PCD and
 * PICC.
 * 
 * To adjust the modulation depth, the selected parameter is measured and then
 * a table lookup is performed to derive the apropriate RF modulation on driver
 * level. This lookup table defines a piece wise linear function. I.e. if the
 * measured paramter value is not found in the table then a linear interpolation
 * between the two closest values is performed. If the measured parameter value
 * is out of the table range (either lower than the smallest x table value or
 * higher than the highest x table value), then the RF on modulation driver
 * level associated with the lowest or highest table entry is used.
 *
 * The modulation depth adjustment mode and the lookup table can be configured
 * via
 * as3911SetModulationLevelMode(). An adjustment of the modulation depth can be
 * performed at any time by a call to as3911AdjustModulationLevel(). This
 * adjustment is done  automatically when as3911Transmit() or
 * as3911Transceive() is called.
 *
 * \section sec_as3911RfidCom_6 Data Transmission and Reception
 *
 * Data can be transceived either by using as3911Transmit() and as3911Receive()
 * directly as a two step process or by calling as3911Transceive(). The later
 * of which will
 * perform the complete transmit and receive opertion and also allows to
 * specify some additional options not available when using as3911Transmit() and
 * as3911Receive() separately.
 *
 * If the two step approach is used then care must be taken that as3911Receive()
 * is called in time not to loose any data received from the PICC. Also the user
 * must then take care that any necessary interupts are
 * enabled. The necessary interrupts are ::AS3911_IRQ_MASK_TXE and
 * ::AS3911_IRQ_MASK_WL for as3911Transmit(), and ::AS3911_IRQ_MASK_RXS,
 * ::AS3911_IRQ_MASK_RXE, and ::AS3911_IRQ_MASK_WL for data reception with
 * as3911Receive(). Additionally error interrupts should be enabled as needed.
 */

#ifndef AS3911_COM_H
#define AS3911_COM_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include <stddef.h>

//#include "ams_types.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL DATA TYPES
******************************************************************************
*/

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * Option flags for as3911Transmit and as3911Transceive
 *****************************************************************************
 */
typedef enum _AS3911RequestFlags_
{
    AS3911_IGNORE_CRC = 0x01, /*!< Do not perform a CRC check for received messages. */
    AS3911_CRC_TO_FIFO = 0x02, /*!< Perform a CRC check on received messages and store the received CRC in the FIFO. */

    AS3911_TRANSMIT_WITH_CRC = 0x00, /*!< Append a CRC to the transmitted message. */
    AS3911_TRANSMIT_WITHOUT_CRC = 0x10, /*!< Do not append a CRC to the transmitted message. */
    AS3911_TRANSMIT_REQA = 0x20, /*!< Transmit a REQA ISO14443-A short frame. */
    AS3911_TRANSMIT_WUPA = 0x30 /*!< Transmit a WUPA ISO14443-A short frame. */
} AS3911RequestFlags_t;

/*! \ingroup as3911RfidCom
 * Bitmask for the bits defining the transmission mode inside the AS3911RequestFlags_t type.
 */
#define AS3911_TX_MODE_MASK     0x30

/*
******************************************************************************
* GLOBAL VARIABLE DECLARATIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Enable or disable emv layer 4 exception processing.
 *
 * Enable or disable reception error processing as required by the EMV standard
 * for a PICC that has been activated.
 * 
 * \param [in] enableEmvExceptionProcessing
 *****************************************************************************
 */
void as3911EnableEmvExceptionProcessing(uchar enableEmvExceptionProcessing);


/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Check EMV exception processing status.
 *
 * \return true: EMV layer 4 exception processing is enabled.
 * \return false: EMV layer 4 exception processing is disabled.
 *****************************************************************************
 */
uchar as3911EmvExceptionProcessingIsEnabled();

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Set the transmission error threshold.
 *
 * If EMV exception processing is enabled, then all messages with a transmission
 * error (CRC error, parity error, residual bits error) and an overall length
 * lower than or equal to the transmission error threshold will be ignored.
 * 
 * \param[in] transmissionErrorThreshold Transmission error threshold (bytes).
 *****************************************************************************
 */
void as3911SetTransmissionErrorThreshold(size_t transmissionErrorThreshold);

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Get the transmission error threshold.
 *
 * \return The transmission error threshold.
 *****************************************************************************
 */
size_t as3911GetTransmissionErrorThreshold();

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Set the frame delay time.
 *
 * The frame delay time defines the minimum delay between the last message
 * received from the PICC and the next message send by the PCD. If there
 * was no response to the last message send by the reader then no frame
 * delay time is applied to the next message send by the reader.
 *
 * \param[in] frameDelayTime The frame delay time in carrier cycles.
 *****************************************************************************
 */
void as3911SetFrameDelayTime(u32 frameDelayTime);

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Get the frame delay time.
 *
 * \return The frame delay time currently used.
 *****************************************************************************
 */
u32 as3911GetFrameDelayTime();

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Set the receiver deadtime.
 *
 * The receiver dead time is the time for which the receiver is still disabled
 * after the end of the message send by the AS3911. The as3911 will not process
 * any card response received during the receiver dead time.
 *
 * In NFCIP1 mode the receiver deadtime has a granularity of 512 carrier cycles
 * and ranges from ?? to 130560 carrier cycles (~9.3ms). In all other modes the
 * receiver deadtime has a granularity of 64 carrier cycles and ranges from 256
 * to 16320 carrier cycles (~1.2ms). Using values below the minimum value will
 * lead to the minimum value being used. Using values above the maximum allowed
 * value leads to undefined behavior.
 *
 * \param[in] receiverDeadtime Receiver deadtime in carrier cycles.
 *****************************************************************************
 */
void as3911SetReceiverDeadtime(u32 receiverDeadtime);

void as3911GetReceiveTimeout(u32 receiveTimeout);

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Set the receive timeout.
 *
 * Set the receive timeout (no-response timer) for AS3911 card response
 * reception. The given receive timeout value is rounded up to the next higher
 * value which can be configured in the AS3911.
 *
 * \param[in] receiveTimeout Receive timeout in carrier cycles.
 *****************************************************************************
 */
void as3911SetReceiveTimeout(u32 receiveTimeout);

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Transmit a message with the AS3911.
 *
 * \note The RFID protocol, datarates, and the tx_en bit have to be setup
 * prior to calling this function.
 *
 * \param[in] message The message to transmit.
 * \param[in] messageLength Length of the message in bytes.
 * \param[in] requestFlags If the AS3911_TRANSMIT_WITHOUT_CRC flag is set then
 * the transmission will occure without CRC otherwise an automatically
 * calculated CRC is appended to the transmitted message. All other flags are
 * ignored.
 *****************************************************************************
 */
void as3911Transmit(u8 *message, size_t messageLength, AS3911RequestFlags_t requestFlags);

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Receive a card response.
 *
 * \note The RFID protocol, datarates, and the rx_en bit have to be set prior
 * to calling this function.
 *
 * \param[in] response Buffer to store the received message.
 * \param[in] maxResponseLength Size of the buffer \a response in bytes.
 * \param[out] responseLength Set to the length of the received message in bytes.
 *
 * \return AS3911_TIMEOUT_ERROR: No card response was received during the
 * reception timeout (as3911SetReceiveTimeout).
 * \return COLLISION_ERROR: A bit collision was detected in the card response.
 * \return AS3911_SOFT_FRAMING_ERROR: Card response had a soft framing error.
 * \return AS3911_HARD_FRAMING_ERROR: Card respnose had a hard framing error.
 * \return AS3911_CRC_ERROR: Card response had a crc error.
 * \return AS3911_PARTIY_ERROR: Card response had a parity error.
 * \return AS3911_OVERFLOW_ERROR: AS3911 FIFO or receive buffer overflow.
 * \return AS3911_NO_ERROR: No error, received an error free card response.
 *****************************************************************************
 */
s16 as3911Receive(u8 *response, size_t maxResponseLength, size_t *responseLength);

/*! \ingroup as3911RfidCom
 *****************************************************************************
 * \brief Send an RFID message and receive a card response.
 *
 * \param[in] request Pointer to the request.
 * \param[in] requestLength Length of the request in bytes.
 * \param[out] response Buffer to store the card response.
 * \param[in] maxResponseLength Size of the response buffer \a response in byets.
 * \param[out] responseLength Set to the length of the recieved response in bytes.
 * \param[in] timeout Receive timeout in carrier cycles.
 * \param[in] requestFlags Request flags. \n
 * default: Append CRC to the transmitted message, check the
 * crc of the received message, and do not put the received CRC into the FIFO. \n
 * AS3911_IGNORE_CRC: Do not perform a CRC check on the received messages. The
 * complete received message is stored in the FIFO. \n
 * AS3911_CRC_TO_FIFO: Perform a CRC check on received messages and store the
 * compelte received message (including the CRC bytes) in the FIFO. \n
 * AS3911_TRANSMIT_WITH_CRC: Append a CRC to the transmitted message.
 * AS3911_TRANSMIT_WITHOUT_CRC: Do not append a CRC to the
 * transmitted message. \n
 * AS3911_TRANSMIT_REQA: Transmit a REQA ISO14443-A short frame. The given
 * request message will be ignored. \n
 * AS3911_TRANSMIT_WUPA: Transmit a WUPA ISO14443-A short frame. The given
 * request messag will be ignored.
 *
 * \return See as3911Receive for a description of the return values.
 */
s16 as3911Transceive(u8 *request, size_t requestLength
    , u8 *response, size_t maxResponseLength, size_t *responseLength
    , u32 timeout, AS3911RequestFlags_t requestFlags);


#endif /* AS3911_COM_H */
