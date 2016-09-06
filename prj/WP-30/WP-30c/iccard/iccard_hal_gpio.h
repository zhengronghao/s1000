/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : iccard_hal_gpio.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 10/15/2014 6:17:56 PM
 * Description        : 
 *******************************************************************************/
#ifndef __ICCARD_HAL_GPIO_H__
#define __ICCARD_HAL_GPIO_H__

#include "iccard_hal.h"
int icc_gpioctrl_init (ICC_GpioInfo_t *p,int mode);
int icc_gpioctrl_read (ICC_GpioInfo_t *p,int mode, int *param);
int icc_gpioctrl_write (ICC_GpioInfo_t *p,int mode,int value);
#endif

