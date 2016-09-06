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
 
/*! \file emv_standard.h
 *
 * \author Oliver Regenfelder
 *
 * \brief Values defined by the EMV standard.
 *
 * Definition of values defined in the EMV standard which are used by
 * the EMV implementation.
 * Minimum values have the prefix \c _MIN and
 * maximum values have the prefix \c _MAX. For certain values only min
 * and max values are defined by the standard. In these cases the value
 * without any prefix defines the value used by the EMV stack.
 *
 * Certain parameters have different values for the PCD and the PICC. In
 * these cases the PCD value is prefixed with \c _PCD and the PICC value
 * is prefixed with \c _PICC. These prefixes are placed after the min/max
 * prefixes. E.g. \a FSCI_MIN_PCD defines the minimum value for FSCI for
 * the PCD.
 */

#ifndef EMV_STANDARD_H
#define EMV_STANDARD_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*!
 *****************************************************************************
 * Minimum value of t_p (milliseconds).
 * \note The standard defines t_p min to be 5.1 milliseconds.
 *****************************************************************************
 */
#define EMV_T_P_MIN             5
/*! Maximum value of t_p (milliseconds). */
#define EMV_T_P_MAX             10
/*! Value of t_p used (in milliseconds). */
#define EMV_T_P                 6

/*!
 *****************************************************************************
 * Minimum value of t_reset (operating field reset time) (milliseconds).
 * \note The standard defines t_reset min to be 5.1 milliseconds.
 *****************************************************************************
 */
#define EMV_T_RESET_MIN         5
/*! Maximum value of t_reset (milliseconds). */
#define EMV_T_RESET_MAX         10
/*! Value of t_reset used (in milliseconds). */
#define EMV_T_RESET             6

/*! Frame wait time of RATS for the PCD (microseconds). */
#define EMV_FWT_ACTIVATION_PCD  71680UL

/*! Frame wait time of RATS for the PICC (microseconds). */
#define EMV_FWT_ACTIVATION_PICC 65536UL

/*! Minimum allowed extra guard time for the PCD (carrier cycles). */
#define EMV_EGT_PCD_MIN         0

/*! Maximum allowed extra guard time for the PCD (carrier cycles). */
#define EMV_EGT_PCD_MAX         752

/*! Minimum allowed extra guard time for the PICC (carrier cycles). */
#define EMV_EGT_PICC_MIN        0

/*! Maximum allowed extra guard time for the PICC (carrier cycles). */
#define EMV_EGT_PICC_MAX        272

/*! Frame wait time of ATQB for the PCD (carrier cycles). */
#define EMV_FWT_ATQB_PCD        7680

/*! Frame wait time of ATQB for the PICC (carrier cycles). */
#define EMV_FWT_ATQB_PICC       7296

/*! Minimum frame delay time for the PCD (carrier cycles). */
#define EMV_FDT_PCD_MIN         6780

/*! Minimum PCD frame size integer supported by the PCD */
#define EMV_FSDI_MIN_PCD        8

/*! Minimum PCD frame size integer supported by the PICC */
#define EMV_FSDI_MIN_PICC       8

/*! Minimum PCD frame size supported by the PCD */
#define EMV_FSD_MIN_PCD        256

/*! Minimum PCD frame size supported by the PICC */
#define EMV_FSD_MIN_PICC       256


/*! Minimum PICC frame size integer supported by the PCD */
#define EMV_FSCI_MIN_PCD        0

/*! Minimum PICC frame size integer supporte by the PICC */
#define EMV_FSCI_MIN_PICC       2

/*! Default PICC frame size integer. This value has to be used if the ATS has no T0 byte. */
#define EMV_FSCI_DEFAULT        2

/*! Minimum PICC frame size supported by the PCD */
#define EMV_FSC_MIN_PCD        16

/*! Minimum PICC frame size supported by the PICC */
#define EMV_FSC_MIN_PICC       32


/*! Maximum frame wait integer supported by the PCD */
#define EMV_FWI_MAX_PCD         14

/*! Maximum frame wait integer allowed for the PICC */
#define EMV_FWI_MAX_PICC        7

/*! Maximum frame wait time supported by the PCD (carrier cycles). */
#define EMV_FWT_MAX_PCD         67108864UL

/*! Maximum frame wait time allowed for the PICC (carrier cycles).*/
#define EMV_FWT_MAX_PICC        524288UL


/*! Maximum frame guard time integer supported by the PCD */
#define EMV_SFGI_MAX_PCD        8

/*! Maximum frame guard time integer supported by the PICC */
#define EMV_SFGI_MAX_PICC       8


/*!
 *****************************************************************************
 * Default value for the PICC->PCD datarate bits if TA(1) is not transmitted
 * in the ATS (ISO14443-A).
 *****************************************************************************
 */
#define EMV_D_PICC_PCD_DEFAULT  0

/*!
 *****************************************************************************
 * Default value for the PCD->PICC datarate bits if TA(1) is not transmitted
 * in the ATS (ISO14443-A).
 *****************************************************************************
 */
#define EMV_D_PCD_PICC_DEFAULT  0

/*!
 *****************************************************************************
 * Default value for the frame guard time integer if TB(1) is not transmitted
 * in the ATS (ISO14443-A).
 *****************************************************************************
 */ 
#define EMV_SFGI_DEFAULT        0

/*!
 *****************************************************************************
 * Default value for the frame wait integer if TB(1) is not transmitted
 * in the ATS (ISO14443-A).
 *****************************************************************************
 */
#define EMV_FWI_DEFAULT         4

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

#endif /* EMV_STANDARD_H */
