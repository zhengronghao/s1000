/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : clock_cycle.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 6/5/2014 5:46:19 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "clock_cycle.h"

#define  DEM_CR   *(volatile unsigned *)0xE000EDFC
#define  DEM_CR_TRCENA	(1 << 24)


void hw_cycle_open(void)
{
	DEM_CR |= DEM_CR_TRCENA;
	DWT_CTRL |= 1;
}

void hw_cycle_close(void)
{
	DEM_CR &= ~DEM_CR_TRCENA;
	DWT_CTRL &= ~1;
}

void hw_clear_cycle(void)
{
	DWT_CYCCNT = 0;
}

uint32_t hw_get_cycle(void)
{
	return DWT_CYCCNT;
}

uint32_t hardclock(void)
{
	return DWT_CYCCNT;
}
