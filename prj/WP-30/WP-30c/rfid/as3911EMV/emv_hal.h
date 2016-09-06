/*
 *****************************************************************************
 * Copyright @ 2009                                 *
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
 
/*! \file emv_hal.h
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV module RFID hardware abstraction layer.
 */

#ifndef EMV_HAL_H
#define EMV_HAL_H

/*! \defgroup emvHal EMV RFID Hardware Abstraction Layer.
 *****************************************************************************
 * \ingroup emv
 * 
 * \brief This module abstracts the RFID hardware in use from the EMV L1
 * contactless comunication protocol stack.
 *
 * This moduel abstracts the RFID hardware in use from the EMV L1 contactless
 * communication protocol stack. Any access to the RFID hardware from
 * the EMV module is done via the API provided by this HAL. This allows to
 * adapt to future RFID chips from AMS with only minimum changes to the EMV
 * software.
 *****************************************************************************
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include <stddef.h>
//#include "ams_types.h"
//#include "as3911_modulation_adjustment.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*! \name EMV HAL error codes. */
/**@{*/
/*! \ingroup emvHal */
#define EMV_HAL_ERR_OK        0 /*!< No error. */
#define EMV_HAL_ERR_TIMEOUT  -1 /*!< Timeout error. */
#define EMV_HAL_ERR_OVERFLOW -2 /*!< Buffer overflow error. */
#define EMV_HAL_ERR_ENCODING -3 /*!< Encoding error (CRC, parity, bit encoding, or framing). */
#define EMV_HAL_ERR_PARAM    -4 /*!< Invalid function call parameter. */
#define EMV_HAL_ERR_COLLISION -5 /*!< Encoding collision error */
/**@}*/

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

/*! \ingroup emvHal
 *  RFID data transmission standard.
 */
typedef enum
{
    EMV_HAL_TYPE_A, /*!< ISO14443-A. */
    EMV_HAL_TYPE_B /*!< ISO14443-B. */
} EmvHalStandard_t;

/*! \ingroup emvHal
 * EMV error handling to use.
 */
typedef enum
{
    EMV_HAL_PREACTIVATION_ERROR_HANDLING, /*!< Error handling as required prior to card activation. */
    EMV_HAL_LAYER4_ERROR_HANDLING /*!< Error handling as required after card activation. */
} EmvHalErrorHandling_t;

/*! \ingroup emvHal
 * Tansceive mode flags.
 */
typedef enum
{
    EMV_HAL_TRANSCEIVE_WITH_CRC, /*!< Transmit and receive with CRC. */
    EMV_HAL_TRANSCEIVE_WITHOUT_CRC, /*!< Transmit and receive without CRC. */
    EMV_HAL_TRANSCEIVE_WUPA /*!< Transmit an ISO14443-A WUPA short frame. */
} EmvHalTransceiveMode_t;

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

/*! \ingroup emvHal
 *****************************************************************************
 * \brief Wait for \a millseconds milliseconds.
 *
 * \param milliseconds Number of milliseconds to wait.
 *****************************************************************************
 */
void emvHalSleepMilliseconds(u16 milliseconds);

/*! \ingroup emvHal
 *****************************************************************************
 * \brief Set the RFID standard to be used by the EMV HAL layer.
 *
 * \param[in] standard RFID standard to use for any subsequent data
 * transmission.
 *
 * \return EMV_HAL_ERR_OK: No error, operation successfull.
 *****************************************************************************
 */
s8 emvHalSetStandard(EmvHalStandard_t standard);

/*! \ingroup emvHal
 *****************************************************************************
 * \brief Set the EMV error handling mode.
 *
 * \param[in] errorHandling EMV error handling mode.
 *
 * \return EMV_HAL_ERR_OK: No error, operation successfull.
 *****************************************************************************
 */
s8 emvHalSetErrorHandling(EmvHalErrorHandling_t errorHandling);

/*! \ingroup emvHal
 *****************************************************************************
 * \brief Activate or deactivate the reader field.
 *
 * \param[in] activateField
 *   TRUE: Activate the reader field. \n
 *   FALSE: Deactivate the reader field.
 *
 * \return EMV_HAL_ERR_OK: No error, operation successful.
 * \return EMV_HAL_ERR_IO: Error during communication.
 *****************************************************************************
 */
s8 emvHalActivateField(uchar activateField);

/*! \ingroup emvHal
 *****************************************************************************
 * \brief Check whether the reader field is activated or not.
 *
 * \return TRUE: The reader field is activated.
 * \return FALSE: The reader field is deactivated.
 *****************************************************************************
 */
uchar emvHalFieldIsActivated();

/*! \ingroup emvHal
 *****************************************************************************
 * \brief Reset the reader field.
 *
 * Reset the reader operating field according to requirement PCD 3.2.6.1 .
 *
 * \note The field will be active after the reset operation, even
 * if it was inactive prior to the function call.
 *
 * \return EMV_HAL_ERR_OK: No error, operation successful.
 * \return EMV_HAL_ERR_IO: Error during communication.
 *****************************************************************************
 */
s8 emvHalResetField();

/*! \ingroup emvHal
 *****************************************************************************
 * \brief Send a request to the PICC and store the response.
 *
 * Transmission and reception are done using the currently active standard
 * (see emvHalSetStandard()) and error handling mode (see
 * emvHalSetErrorHandling()).
 *
 * \param[in] request Buffer holding the request data.
 * \param[in] requestLength Length of the request data in bytes.
 * \param[out] response Buffer to store the PICCs response.
 * \param[in] maxResponseLength Length of the response buffer in bytes.
 * \param[out] responseLength Set to the length of the received repsonse in
 * bytes.
 * \param[in] timeout Timeout in carrier cycles.
 * \param[in] transceiveMode
 *   EMV_HAL_TRANSCEIVE_WITH_CRC: A crc is appended to the transmit data during
 *   transmission, and the crc of the received data is checked.
 *   EMV_HAL_TRANSCEIVE_WITHOUT_CRC: No crc is appended to the transmit data. No
 *   crc check is performed on the received data.
 *   EMV_HAL_TRANSCEIVE_WUPA: A wupa request is transmitted (the content of the
 *   request buffer is ignored). No crc check is performed on the received
 *   data.
 *
 * \return EMV_ERR_OK: A response from the PICC has been correctly received.
 * \return EMV_ERR_TIMEOUT: No response has been received.
 * \return EMV_ERR_ENCODING: A transmission error (parity, crc,
 * bit collision...) occured.
 *****************************************************************************
 */
s8 emvHalTransceive(u8 *request, size_t requestLength,
        u8 *response, size_t maxResponseLength, size_t *responseLength,
        u32 timeout, EmvHalTransceiveMode_t transceiveMode);
		
/*! \ingroup emvHal
 *****************************************************************************
 * \brief Set AS3911 type B modulation mode.
 *
 * \param[in] modulationLevelMode AS3911 modulation leve adjustment mode for
 * type B communication.
 * \param[in] modulationLevelModeData Additional data for the AS3911 type B 
 * modulation level adjustment mode.
 *****************************************************************************
 */
s8 emvHalSetAs3911TypeBModulationMode(AS3911ModulationLevelMode_t modulationLevelMode, const void *modulationLevelModeData);
 
void s_as3911_init(void);
void s_as3911_SetInt(uint mode);
void sleepMilliseconds(unsigned int milliseconds);
uchar s_as3911errorCode(int error);

#endif /* EMV_HAL_H */
