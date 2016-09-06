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
  
/*! \file emv_prevalidation.h
 *
 * \author Oliver Regenfelder
 *
 * \brief EMV TTA L1 prevalidation application callback.
 */

#ifndef EMV_PREVALIDATION_H
#define EMV_PREVALIDATION_H

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

/*! \ingroup emvMain
 *****************************************************************************
 * \brief EMV prevalidation test application terminal application callback.
 *
 * \return EMV_ERR_OK No error, prevalidation test application completed
 * successfully.
 * \return EMV_ERR_STOPPED: Prevalidation test application stopped by request from
 * the EMV TTA L1 GUI.
 * \return other: Prevalidation test application stopped due to an errornous
 * card response. See emv_error_codes.h for details on EMV card response
 * error codes.
 *****************************************************************************
 */
s16 emvPrevalidationApplication();

#endif
#endif /* EMV_PREVALIDATION_H */

