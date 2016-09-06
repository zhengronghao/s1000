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
  
/*! \file emv_response_buffer.h
 *
 * \author Oliver Regenfelder
 *
 * \brief Global Bufer to store the PICC response (Shall not be used outside the
 * EMV code).
 *
 * A single shared buffer is used by the digital and prevalidation test
 * application to store PICC RAPDUs. The required size of this buffer is
 * not specified.
 *
 * The selected 1024 bytes are sufficiently large to pass the certification.
 */

#ifndef EMV_RESPONSE_BUFFER_H
#define EMV_RESPONSE_BUFFER_H

#if defined(EMV_AS3911)
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include "ams_types.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*! Size of the (module) global EMV card response buffer. */
#define EMV_RESPONSE_BUFFER_SIZE    1024

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

/*! The (module) global EMV card response buffer. */
extern u8 emvResponseBuffer[];

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

#endif
#endif /* EMV_RESPONSE_BUFFER_H */

