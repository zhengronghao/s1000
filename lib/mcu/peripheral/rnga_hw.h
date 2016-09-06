/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : rnga_hw.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/20/2014 3:05:40 PM
 * Description        : 
 *******************************************************************************/

#ifndef __RNGA_HW_H__
#define __RNGA_HW_H__

void hw_rnga_reg_report(void);
void hw_rnga_init(unsigned int seed);
int hw_rnga_getnumber(uint32_t *random_number);
void hw_rnga_stop(void);

#endif

