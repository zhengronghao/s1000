/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : clock_cycle.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 6/5/2014 5:49:38 PM
 * Description        : 
 *******************************************************************************/

#ifndef __CLOCK_CYCLE_H__
#define __CLOCK_CYCLE_H__
void hw_cycle_open(void);
void hw_cycle_close(void);
void hw_clear_cycle(void);
uint32_t hw_get_cycle(void);
uint32_t hardclock(void);
#endif

