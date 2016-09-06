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

/*! \file as3911.h
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 module main header file.
 *
 * Include this header file to include all header files provided by the AS3911
 * module.
 */

/*! \defgroup as3911 AS3911
 */

#ifndef AS3911_H
#define AS3911_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "as3911_def.h"
//#include "as3911_io.h"
//#include "as3911_modulation_adjustment.h"
//#include "as3911_gain_adjustment.h"
//#include "as3911_interrupt.h"
//#include "as3911_com.h"
//#include "as3911_errno.h"

//typedef enum _BOOL { FALSE = 0, TRUE } BOOL;    /* Undefined size */
//typedef enum _BIT { CLEAR = 0, SET } BIT;

//typedef enum {FALSE = 0, TRUE = !FALSE} bool;
//typedef uchar BOOL;    /* Undefined size */
//typedef uchar bool_t;    /* Undefined size */

//#define U8_C(x)     (x) /*!< \ingroup datatypes
//                         Define a constant of type u8 */
//#define S8_C(x)     (x) /*!< \ingroup datatypes
//                         Define a constant of type s8 */
//#define U16_C(x)    (x) /*!< \ingroup datatypes
//                         Define a constant of type u16 */
//#define S16_C(x)    (x) /*!< \ingroup datatypes
//                         Define a constant of type s16 */
#define U32_C(x)    ((uint)(x))/*(x##UL)*/ /*!< \ingroup datatypes
                             Define a constant of type u32 */
#define S32_C(x)    ((int)(x))/*(x##L)*/ /*!< \ingroup datatypes
                            Define a constant of type s32 */
//#define U64_C(x)    (x)/*(x##ULL)*/ /*!< \ingroup datatypes
//                              Define a constant of type u64 */
//#define S64_C(x)    (x)/*(x##LL)*/ /*!< \ingroup datatypes
//                             Define a constant of type s64 */
//#define UMWORD_C(x) (x) /*!< \ingroup datatypes
//                         Define a constant of type umword */
//#define MWORD_C(x)  (x) /*!< \ingroup datatypes
//                         Define a constant of type mword */

//#define EMV_AS3911

#include "./rfid/as3911/as3911_def.h"
#include "./rfid/as3911/as3911_io.h"
#include "./rfid/as3911/as3911_modulation_adjustment.h"
#include "./rfid/as3911/as3911_gain_adjustment.h"
#include "./rfid/as3911/as3911_interrupt.h"
#include "./rfid/as3911/as3911_com.h"
#include "./rfid/as3911/as3911_errno.h"
#include "./rfid/as3911/emv_gui.h"

#include "./rfid/as3911EMV/emv_error_codes.h"
#include "./rfid/as3911EMV/emv_display.h"
#include "./rfid/as3911EMV/emv_hal.h"
#include "./rfid/as3911EMV/emv_standard.h"
#include "./rfid/as3911EMV/emv_picc.h"
#include "./rfid/as3911EMV/emv_typeA.h"
#include "./rfid/as3911EMV/emv_typeB.h"
#include "./rfid/as3911EMV/emv_prelayer4.h"
#include "./rfid/as3911EMV/emv_main.h"
#include "./rfid/as3911EMV/emv_layer4.h"
#include "./rfid/as3911EMV/emv_poll.h"
#include "./rfid/as3911EMV/emv_prevalidation.h"
#include "./rfid/as3911EMV/emv_digital.h"
#include "./rfid/as3911EMV/emv_response_buffer.h"

#include "./rfid/as3911Mifare/mifare.h"

/*
******************************************************************************
* PUBLIC DEFINES
******************************************************************************
*/

/*! \defgroup errorcodes Basic application error codes
 * Error codes to be used within the application.
 * They are represented by an s8
 */
#define ERR_NONE   0 /*!< \ingroup errorcodes
               no error occured */
#define ERR_NOMEM -1 /*!< \ingroup errorcodes
               not enough memory to perform the requested
               operation */
#define ERR_BUSY  -2 /*!< \ingroup errorcodes
               device or resource busy */
#define ERR_IO    -3 /*!< \ingroup errorcodes
                generic IO error */
//#define ERR_TIMEOUT -4 /*!< \ingroup errorcodes error due to timeout */

#define ERR_REQUEST -5 /*!< \ingroup errorcodes
                invalid request or requested
                function can't be executed at the moment */
#define ERR_NOMSG -6 /*!< \ingroup errorcodes
                       No message of desired type */
#define ERR_PARAMS -7 /*!< \ingroup errorcodes
                       Parameter error */

#define ERR_LAST_ERROR -32


//as3911 error
//#define ERR_CRC -50        /*!< crc error */
#define ERR_COLLISION -51  /*!< collision error */
#define ERR_NOTFOUND -52   /*!< transponder not found */
#define ERR_NOTUNIQUE -53  /*!< transponder not unique - more than one transponder in field */
#define ERR_NOTSUPP -54    /*!< requested operation not supported */
#define ERR_WRITE -55      /*!< write error */
#define ERR_FIFO  -56      /*!< fifo over or underflow error */
#define ERR_PAR   -57      /*!< parity error */
#define ERR_FRAMING -58    /*!< framing error */
#define ERR_DONE -59      /*!< transfer has already finished */
#define ERR_INTERNAL -59     /*!< internal error */
#define ERR_RF_COLLISION -60 /*!< \ingroup error codes RF collision during RF collision avoidance occured */
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
#define AS3911_TYPEB_MODULATION     233
#define AS3911_TYPEA_RECEIVE        0xD8
#define AS3911_TYPEB_RECEIVE        0xD8 

/*
******************************************************************************
* GLOBAL DATA TYPES
******************************************************************************
*/

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

#endif /* AS3911_H */
