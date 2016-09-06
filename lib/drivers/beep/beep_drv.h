/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : beep_drv.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 10/12/2014 4:17:12 PM
 * Description        : 
 *******************************************************************************/
#ifndef __BEEP_DRV_H__
#define __BEEP_DRV_H__

void drv_beep_open(uint32_t freq_hz);
void drv_beep_close(void);
void drv_beep_start(void);
void drv_beep_stop(void);

#endif

