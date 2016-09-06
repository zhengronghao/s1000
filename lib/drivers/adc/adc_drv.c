/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : adc_drv.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 5/28/2014 4:33:06 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "adc_drv.h"

int drv_adc_open(uint8_t mux_ab,uint8_t trig,uint8_t mode_diff, uint8_t hw_avg)
{
    ADC_InitTypeDef adc_init;

    memset(&adc_init,0x00,sizeof(ADC_InitTypeDef));
    adc_init.adcx = ADC0;
    adc_init.cfg1 = ADC_CFG1_InputClk_BUSCLK_DIV2 |ADC_CFG1_Single16BIT_Diff16BIT
        | ADC_CFG1_SampleTimeLong;
    adc_init.cfg2 = ADC_CFG2_SampleTimeLong_10Cycles | ADC_CFG2_ConverSeqHighSpeed | ADC_CFG2_AsynClk_Disable 
        | (mux_ab&ADC_CFG2_Mux_MASK);
    adc_init.sc1 = mode_diff&ADC_SC1n_DIFF_MASK;
    adc_init.sc2 = (trig&ADC_SC2_TRIGER_MASK) | ADC_SC2_VolRef_Extern
        | ADC_SC2_CMP_RANGE_ENABLE | ADC_SC2_CMP_GREATER_ENABLE;
    adc_init.sc3 = hw_avg;
//    TRACE("\nCFG1 %08b",adc_init.cfg1);
    return hw_adc_init(&adc_init);
}

int drv_adc_caculate(ADC_Channel_BitMapDef chn)
{
    if (hw_adc_pin_channel_enable(ADC0,chn)) {
        return -1;
    }
    return hw_adc_softtrig_adval(ADC0,chn);
}

int drv_adc_close(uint8_t mux_ab)
{
    return hw_adc_close(ADC0,mux_ab);
}


uint drv_adcx_open(ADC_TypeDef adcx, uint8_t mux_ab, uint8_t trig,uint8_t mode_diff, uint8_t hw_avg)
{
    ADC_InitTypeDef adc_init;

    memset(&adc_init,0x00,sizeof(ADC_InitTypeDef));
    adc_init.adcx = adcx;
    adc_init.cfg1 = ADC_CFG1_InputClk_BUSCLK_DIV2 |ADC_CFG1_Single16BIT_Diff16BIT
        | ADC_CFG1_SampleTimeLong;
    adc_init.cfg2 = ADC_CFG2_SampleTimeLong_10Cycles | ADC_CFG2_ConverSeqHighSpeed | ADC_CFG2_AsynClk_Disable 
        | (mux_ab&ADC_CFG2_Mux_MASK);
    adc_init.sc1 = mode_diff&ADC_SC1n_DIFF_MASK;
    adc_init.sc2 = (trig&ADC_SC2_TRIGER_MASK) | ADC_SC2_VolRef_Extern
        | ADC_SC2_CMP_RANGE_ENABLE | ADC_SC2_CMP_GREATER_ENABLE;
    adc_init.sc3 = hw_avg;
//    TRACE("\nCFG1 %08b",adc_init.cfg1);
    return hw_adc_init(&adc_init);
}

uint drv_adcx_caculate(ADC_TypeDef adcx, ADC_Channel_BitMapDef chn)
{
    if (hw_adc_pin_channel_enable(adcx, chn)) {
        return -1;
    }
    return hw_adc_softtrig_adval(adcx, chn);
}

uint drv_adcx_close(ADC_TypeDef adcx, uint8_t mux_ab)
{
    return hw_adc_close(adcx, mux_ab);
}



