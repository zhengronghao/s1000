#ifndef PARITY_DATA_T_H__
#define PARITY_DATA_T_H__

#if (defined(EM_AS3911_Module))
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
//#include "platform.h"
#include "wp30_ctrl.h"

/*
******************************************************************************
* GLOBAL DATATYPES
******************************************************************************
*/
/*!
 * This type holds data bytes with associated parity bits.
 * The data is in the low byte while the associated parity bit
 * is in the least-significant bit of the high byte.
 */
typedef u16 parity_data_t;

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/
/*! Calculate the odd parity bit for one byte */
#define ODD_PARITY(i) (( (i) ^ (i)>>1 ^ (i)>>2 ^ (i)>>3 ^ (i)>>4 ^ (i)>>5 ^ (i)>>6 ^ (i)>>7 ^ 1) & 0x01)

/*!
 * Like UINT32_TO_ARRAY, but put the correct parity in the 8th bit of each array
 * entry (thus the array entries must at least be 16 bit wide)
 */
#define UINT32_TO_ARRAY_WITH_PARITY(i, a) do{ \
	(a)[0] = ((i)>>24) & 0xff; (a)[0] |= ODD_PARITY((a)[0])<<8; \
	(a)[1] = ((i)>>16) & 0xff; (a)[1] |= ODD_PARITY((a)[1])<<8; \
	(a)[2] = ((i)>> 8) & 0xff; (a)[2] |= ODD_PARITY((a)[2])<<8; \
	(a)[3] = ((i)>> 0) & 0xff; (a)[3] |= ODD_PARITY((a)[3])<<8; \
} while(0);

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/
/*!
 *****************************************************************************
 * \brief Calculate the parity for a parity_data_t array.
 *
 * Calculates the parity of each byte of \a data and updates the parity
 * information inside \a data accordingly.
 *****************************************************************************
 */
void calculateParity(parity_data_t *data, u32 length);

#endif /* PARITY_DATA_T_H__ */
#endif

