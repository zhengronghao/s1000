/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : rtc_hw.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/18/2014 2:08:17 PM
 * Description        : 
 *******************************************************************************/

#ifndef __RTC_HW__
#define __RTC_HW__

void hw_rtc_init( uint32_t seconds, uint32_t alarm);
void hw_rtc_colse(void);
void hw_rtc_reg_report(void);
uint32_t hw_rtc_IfValid(void);
int hw_rtc_read_second(uint32_t *second);
void hw_rtc_irq_second_enable(void);
void hw_rtc_irq_enable(void);
#endif

