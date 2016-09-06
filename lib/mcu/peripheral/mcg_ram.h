/*
 * File:    mcg_ram.h
 * Purpose: mcg_ram specific declarations
 *
 * Notes:
 */
#ifndef __MCG_RAM_H__
#define __MCG_RAM_H__
/********************************************************************/

#include "common.h"

#if (defined(IAR))
__ramfunc void set_sys_dividers
#elif (defined(CW))
__relocate_code__ void set_sys_dividers
#elif (defined(GNU))
void __attribute ((section(".relocate_code"))) set_sys_dividers
#else/*MULTI and KEIL*/
void set_sys_dividers
#endif
(uint32 outdiv1, uint32 outdiv2, uint32 outdiv3, uint32 outdiv4);

/********************************************************************/
#endif /* __MCG_RAM_H__ */
