/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : adc_hw.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 5/27/2014 3:27:13 PM
 * Description        : 
 *******************************************************************************/

#ifndef __ADC_HW_H__
#define __ADC_HW_H__


typedef enum
{
    ADC_SC1n_SingleEnded  = (0<<ADC_SC1_DIFF_SHIFT),
    ADC_SC1n_Differential = (1<<ADC_SC1_DIFF_SHIFT),
    ADC_SC1n_DIFF_MASK = ADC_SC1_DIFF_MASK,

    ADC_SC1n_IrqDisable = (0<<ADC_SC1_AIEN_SHIFT),
    ADC_SC1n_IrqEnable  = (1<<ADC_SC1_AIEN_SHIFT),
    ADC_SC1n_IrqMASK    = ADC_SC1_AIEN_MASK,

    ADC_SC1n_ConvComplte_MASK = ADC_SC1_COCO_MASK,
}ADC_SC1n_BitMapDef;

typedef enum _ChnBitMap
{
    ADC_SC1n_ChnSelt_SEDP0_DiffDAD0  = ADC_SC1_ADCH(0),
    ADC_SC1n_ChnSelt_SEDP1_DiffDAD1  = ADC_SC1_ADCH(1),
    ADC_SC1n_ChnSelt_SEDP2_DiffDAD2  = ADC_SC1_ADCH(2),//Reserved
    ADC_SC1n_ChnSelt_SEDP3_DiffDAD3  = ADC_SC1_ADCH(3),
    ADC_SC1n_ChnSelt_SE4a_b = ADC_SC1_ADCH(4),
    ADC_SC1n_ChnSelt_SE5a_b = ADC_SC1_ADCH(5),
    ADC_SC1n_ChnSelt_SE6a_b = ADC_SC1_ADCH(6),
    ADC_SC1n_ChnSelt_SE7a_b = ADC_SC1_ADCH(7),
    ADC_SC1n_ChnSelt_SE8    = ADC_SC1_ADCH(8),
    ADC_SC1n_ChnSelt_SE9    = ADC_SC1_ADCH(9),
    ADC_SC1n_ChnSelt_SE10   = ADC_SC1_ADCH(10),
    ADC_SC1n_ChnSelt_SE11   = ADC_SC1_ADCH(11),
    ADC_SC1n_ChnSelt_SE12   = ADC_SC1_ADCH(12),
    ADC_SC1n_ChnSelt_SE13   = ADC_SC1_ADCH(13),
    ADC_SC1n_ChnSelt_SE14   = ADC_SC1_ADCH(14),
    ADC_SC1n_ChnSelt_SE15   = ADC_SC1_ADCH(15),
    ADC_SC1n_ChnSelt_SE16   = ADC_SC1_ADCH(16),//Reserved
    ADC_SC1n_ChnSelt_SE17   = ADC_SC1_ADCH(17),//Reserved
    ADC_SC1n_ChnSelt_SE18   = ADC_SC1_ADCH(18),//Reserved
    ADC_SC1n_ChnSelt_DM0    = ADC_SC1_ADCH(19),
    ADC_SC1n_ChnSelt_DM1    = ADC_SC1_ADCH(20),
    ADC_SC1n_ChnSelt_SE21   = ADC_SC1_ADCH(21),
    ADC_SC1n_ChnSelt_SE22   = ADC_SC1_ADCH(22),
    ADC_SC1n_ChnSelt_SE23   = ADC_SC1_ADCH(23),
    ADC_SC1n_ChnSelt_SE24   = ADC_SC1_ADCH(24),//Reserved
    ADC_SC1n_ChnSelt_SE25   = ADC_SC1_ADCH(25),//Reserved
    ADC_SC1n_ChnSelt_TempSensor = ADC_SC1_ADCH(26),
    ADC_SC1n_ChnSelt_Bandgap    = ADC_SC1_ADCH(27),
    ADC_SC1n_ChnSelt_SE28  = ADC_SC1_ADCH(28),//Reserved
    ADC_SC1n_ChnSelt_VREFH = ADC_SC1_ADCH(29),
    ADC_SC1n_ChnSelt_VREFL = ADC_SC1_ADCH(30),
    ADC_SC1n_ChnDisable    = ~ADC_SC1_ADCH(31)
}ADC_Channel_BitMapDef;

typedef enum
{
    ADC_CFG1_InputClk_BUSCLK_DIV1 = ADC_CFG1_ADICLK(0),
    ADC_CFG1_InputClk_BUSCLK_DIV2 = ADC_CFG1_ADICLK(1),
    ADC_CFG1_InputClk_ALTCLK      = ADC_CFG1_ADICLK(2),
    ADC_CFG1_InputClk_ASYNCLK     = ADC_CFG1_ADICLK(3),

    ADC_CFG1_Single8BIT_Diff9BIT   = ADC_CFG1_MODE(0),
    ADC_CFG1_Single12BIT_Diff13BIT = ADC_CFG1_MODE(1),
    ADC_CFG1_Single10BIT_Diff11BIT = ADC_CFG1_MODE(2),
    ADC_CFG1_Single16BIT_Diff16BIT = ADC_CFG1_MODE(3),

    ADC_CFG1_SampleTimeShort = (0<<ADC_CFG1_ADLSMP_SHIFT),
    ADC_CFG1_SampleTimeLong  = (1<<ADC_CFG1_ADLSMP_SHIFT),

    ADC_CFG1_ADCLK_DIV1 = ADC_CFG1_ADIV(0),
    ADC_CFG1_ADCLK_DIV2 = ADC_CFG1_ADIV(1),
    ADC_CFG1_ADCLK_DIV4 = ADC_CFG1_ADIV(2),
    ADC_CFG1_ADCLK_DIV8 = ADC_CFG1_ADIV(3),

    ADC_CFG1_NORMAL_PWR = (0<<ADC_CFG1_ADLPC_SHIFT),
    ADC_CFG1_LOWER_PWR  = (1<<ADC_CFG1_ADLPC_SHIFT),
}ADC_CFG1_BitMapDef;

typedef enum
{
    ADC_CFG2_SampleTimeLong_24Cycles = ADC_CFG2_ADLSTS(0),
    ADC_CFG2_SampleTimeLong_16Cycles = ADC_CFG2_ADLSTS(1),
    ADC_CFG2_SampleTimeLong_10Cycles = ADC_CFG2_ADLSTS(2),
    ADC_CFG2_SampleTimeLong_6Cycles  = ADC_CFG2_ADLSTS(3),

    ADC_CFG2_ConverSeqNormal     = (0<<ADC_CFG2_ADHSC_SHIFT),
    ADC_CFG2_ConverSeqHighSpeed  = (1<<ADC_CFG2_ADHSC_SHIFT),

    ADC_CFG2_AsynClk_Disable = (0<<ADC_CFG2_ADACKEN_SHIFT),
    ADC_CFG2_AsynClk_Enable  = (1<<ADC_CFG2_ADACKEN_SHIFT),

    ADC_CFG2_Mux_ADxxa = (0<<ADC_CFG2_MUXSEL_SHIFT),//ADxxa channels are selected.
    ADC_CFG2_Mux_ADxxb = (1<<ADC_CFG2_MUXSEL_SHIFT),//ADxxb channels are selected.
    ADC_CFG2_Mux_MASK = ADC_CFG2_MUXSEL_MASK,
}ADC_CFG2_BitMapDef;

typedef enum
{
    ADC_SC2_VolRef_Extern = ADC_SC2_REFSEL(0),//Default voltage reference pin pair, that is, external pins VREFH and VREFL
    ADC_SC2_VolRef_Altern = ADC_SC2_REFSEL(1),//Alternate reference pair, that is, VALTH and VALTL. This pair may be additional external pins or
                                              //internal sources depending on the MCU configuration.
    ADC_SC2_VolRef_Res1 = ADC_SC2_REFSEL(2),
    ADC_SC2_VolRef_Res2 = ADC_SC2_REFSEL(3),

    ADC_SC2_DMA_DISABLE = (0<<ADC_SC2_DMAEN_SHIFT),
    ADC_SC2_DMA_ENABLE  = (1<<ADC_SC2_DMAEN_SHIFT),

    ADC_SC2_CMP_RANGE_DISABLE = (0<<ADC_SC2_ACREN_SHIFT),
    ADC_SC2_CMP_RANGE_ENABLE  = (1<<ADC_SC2_ACREN_SHIFT),

    ADC_SC2_CMP_GREATER_DISABLE = (0<<ADC_SC2_ACFGT_SHIFT),
    ADC_SC2_CMP_GREATER_ENABLE  = (1<<ADC_SC2_ACFGT_SHIFT),

    ADC_SC2_CMP_DISABLE = (0<<ADC_SC2_ACFE_SHIFT),
    ADC_SC2_CMP_ENABLE  = (1<<ADC_SC2_ACFE_SHIFT),

    ADC_SC2_TRIGER_SOFTWARE = (0<<ADC_SC2_ADTRG_SHIFT),
    ADC_SC2_TRIGER_HARDWARE = (1<<ADC_SC2_ADTRG_SHIFT),
    ADC_SC2_TRIGER_MASK = ADC_SC2_ADTRG_MASK,
}ADC_SC2_BitMapDef;

typedef enum
{
    ADC_SC3_HARDWARE_AVERAGE_4 = ADC_SC3_AVGS(0),
    ADC_SC3_HARDWARE_AVERAGE_8 = ADC_SC3_AVGS(1),
    ADC_SC3_HARDWARE_AVERAGE_16 = ADC_SC3_AVGS(2),
    ADC_SC3_HARDWARE_AVERAGE_32 = ADC_SC3_AVGS(3),
    ADC_SC3_HARDWARE_AVERAGE_MASK = ADC_SC3_AVGS_MASK,

    ADC_SC3_HARDWARE_AVERAGE_DISABLE = (0<<ADC_SC3_AVGE_SHIFT),
    ADC_SC3_HARDWARE_AVERAGE_ENABLE  = (1<<ADC_SC3_AVGE_SHIFT),

    ADC_SC3_ContineCouversion_DISABLE = (0<<ADC_SC3_ADCO_SHIFT),
    ADC_SC3_ContineCouversion_ENABLE  = (0<<ADC_SC3_ADCO_SHIFT),

    ADC_SC3_CALIBRAT_START = ADC_SC3_CAL_MASK,
}ADC_SC3_BitMapDef;



typedef ADC_MemMapPtr  ADC_TypeDef;

typedef struct
{    
  ADC_TypeDef adcx;
  uint32_t cfg1;
  uint32_t cfg2;
  uint32_t sc1;
  uint32_t sc2;
  uint32_t sc3;
} ADC_InitTypeDef;


/** Peripheral ADC0 base pointer */
#define ADC0                        ((ADC_TypeDef)0x4003B000u)
#define ADC1                        ((ADC_TypeDef)0x400BB000u)


int hw_adc_init(ADC_InitTypeDef *adc_init);
int hw_adc_close(ADC_TypeDef adcx,uint8_t mux_ab );
int hw_adc_pin_channel_enable(ADC_TypeDef adcx,ADC_Channel_BitMapDef chn);
void hw_adc_start_convert(ADC_TypeDef adcx,ADC_Channel_BitMapDef chn,uint8_t mux_ab);
int hw_adc_irq_enable(ADC_TypeDef adcx);
int hw_adc_irq_disable(ADC_TypeDef adcx);
uint16_t hw_adc_softtrig_adval(ADC_TypeDef adcx,ADC_Channel_BitMapDef chn);
uint16_t hw_adc_hardtrig_adval(ADC_TypeDef adcx);

void dbg_adc_reg_display(const char *title);
#endif

