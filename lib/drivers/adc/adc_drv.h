/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : adc_drv.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 5/28/2014 4:33:56 PM
 * Description        : 
 *******************************************************************************/
#ifndef __ADC_DRV_H__
#define __ADC_DRV_H__

#include "adc_hw.h"
int drv_adc_open(uint8_t mux_ab,uint8_t trig,uint8_t mode_diff, uint8_t hw_avg);
int drv_adc_caculate(ADC_Channel_BitMapDef chn);
int drv_adc_close(uint8_t mux_ab);

uint drv_adcx_open(ADC_TypeDef adcx, uint8_t mux_ab,uint8_t trig,uint8_t mode_diff, uint8_t hw_avg);
uint drv_adcx_caculate(ADC_TypeDef adcx,ADC_Channel_BitMapDef chn);
uint drv_adcx_close(ADC_TypeDef adcx,uint8_t mux_ab);
#endif


