/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : adc_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 5/27/2014 3:27:10 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "adc_hw.h"

void dbg_adc_reg_display(const char *title)
{
#if 0
    ADC_TypeDef adcx = ADC0;
    TRACE("\n|---%s--|",title);
    TRACE("\n-|SC1n0:%08b\tSC1n1:%08b",adcx->SC1[0],adcx->SC1[1]);
    TRACE("\n-|CFG1 :%08b\tCFG2 :%08b",adcx->CFG1,adcx->CFG2);
    TRACE("\n-|SC2  :%08b\tSC3  :%08b",adcx->SC2,adcx->SC3);
    TRACE("\n-|PG:%04X  MG:%04X",adcx->PG,adcx->MG);
    TRACE("  OFS:%04X",adcx->OFS);
#endif
}

static int hw_adc_selfcalibrat(ADC_TypeDef adcx)
{
    uint16 cal_var;
/*
For best calibration results:
 1.Set hardware averaging to maximum, that is, SC3[AVGE]=1 and SC3[AVGS]=11 for an average of 32
 2.Set ADC clock frequency fADCK less than or equal to 4 MHz
 3.VREFH =VDDA,Normal power mode
 4.Calibrate at nominal voltage and temperature
The input channel, conversion mode continuous function, compare function, resolution
mode, and differential/single-ended mode are all ignored during the calibration function.
* */
    adcx->CFG1 = ADC_CFG1_InputClk_BUSCLK_DIV2 | ADC_CFG1_ADCLK_DIV8 //4MHz
                |ADC_CFG1_SampleTimeLong 
                |ADC_CFG1_NORMAL_PWR;
    adcx->CFG2 = ADC_CFG2_SampleTimeLong_24Cycles 
                |ADC_CFG2_ConverSeqHighSpeed 
                |ADC_CFG2_AsynClk_Disable;
    adcx->CV1  = 0x1234u; 
    adcx->CV2  = 0x5678u;
    adcx->SC2 = ADC_SC2_TRIGER_SOFTWARE 
               |ADC_SC2_VolRef_Extern;
    adcx->SC3 = ADC_SC3_HARDWARE_AVERAGE_32 | ADC_SC3_HARDWARE_AVERAGE_ENABLE;
    adcx->SC3 |= ADC_SC3_CALIBRAT_START;

    while((adcx->SC1[0] & ADC_SC1_COCO_MASK)== 0x00 );
    if ((adcx->SC3& ADC_SC3_CALF_MASK) == ADC_SC3_CALF_MASK )
    {  
        TRACE("\nSelfCal_err");
        return -1; 
    }
    // Calculate plus-side calibration
    cal_var  = 0x00;
    cal_var  = adcx->CLP0;       
    cal_var += adcx->CLP1;      
    cal_var += adcx->CLP2;      
    cal_var += adcx->CLP3;      
    cal_var += adcx->CLP4;      
    cal_var += adcx->CLPS;      
    cal_var /= 2;
    cal_var |= 0x8000; // Set MSB
    adcx->PG = ADC_PG_PG(cal_var);
    // Calculate minus-side calibration
    cal_var = 0x00;
    cal_var =  adcx->CLM0; 
    cal_var += adcx->CLM1;
    cal_var += adcx->CLM2;
    cal_var += adcx->CLM3;
    cal_var += adcx->CLM4;
    cal_var += adcx->CLMS;
    cal_var /= 2;
    cal_var |= 0x8000; // Set MSB
    adcx->MG   = ADC_MG_MG(cal_var); 

    adcx->SC3 &= ~ADC_SC3_CAL_MASK ;

    return 0;
}

int hw_adc_init(ADC_InitTypeDef *adc_init)
{
    uint8_t mux;
    ADC_TypeDef adcx = adc_init->adcx;
    if(adcx == ADC0) {
        SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    } else if (adcx == ADC1) {
        SIM->SCGC3 |= SIM_SCGC3_ADC1_MASK;
    } else {
        return -1;
    }
    dbg_adc_reg_display("init");
    hw_adc_selfcalibrat(adcx);
    adcx->CFG1 = adc_init->cfg1;
    adcx->CFG2 = adc_init->cfg2;
    adcx->CV1  = 0x1234u ; 
    adcx->CV2  = 0x5678u ;
    mux = ((adc_init->cfg2&ADC_CFG2_Mux_MASK)>>ADC_CFG2_MUXSEL_SHIFT);
    adcx->SC2 = adc_init->sc2;
    adcx->SC3 = adc_init->sc3;
    adcx->SC1[mux] = adc_init->sc1;
    dbg_adc_reg_display("Self");
    return 0;
}

int hw_adc_close(ADC_TypeDef adcx,uint8_t mux_ab )
{
    mux_ab  = (mux_ab)?1:0;
    adcx->SC1[mux_ab] &= ADC_SC1n_ChnDisable;    //¸´Î»SC1
    if(adcx == ADC0) {
        SIM->SCGC6 &= ~SIM_SCGC6_ADC0_MASK;
    } else if (adcx == ADC1) {
        SIM->SCGC3 &= ~SIM_SCGC3_ADC1_MASK;
    } else {
        return -1;
    }
    return 0;
}

int hw_adc_pin_channel_enable(ADC_TypeDef adcx,ADC_Channel_BitMapDef chn)
{
    uint8_t mux_ab = (adcx->CFG2 & ADC_CFG2_MUXSEL_MASK)>>ADC_CFG2_MUXSEL_SHIFT;
    if (adcx == ADC0)
    {
        switch (chn)
        {
        case ADC_SC1n_ChnSelt_SEDP0_DiffDAD0:
        case ADC_SC1n_ChnSelt_SEDP1_DiffDAD1:
        case ADC_SC1n_ChnSelt_SEDP2_DiffDAD2:
        case ADC_SC1n_ChnSelt_SEDP3_DiffDAD3:
            break;
        case ADC_SC1n_ChnSelt_SE4a_b:
            if(mux_ab == 1)
                PORTC->PCR[2] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE5a_b:
            if(mux_ab == 1)
                PORTD->PCR[1] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE6a_b:
            if(mux_ab == 1)
                PORTD->PCR[5] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE7a_b:
            if(mux_ab == 1)
                PORTD->PCR[6] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE8:
            if(mux_ab == 1)
                PORTB->PCR[0] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE9:
            if(mux_ab == 1)
                PORTB->PCR[1] =  PORT_PCR_MUX(0);
            break;
//        case ADC_SC1n_ChnSelt_SE10:
//            if(mux_ab == 1)
//                PORTA->PCR[7] =  PORT_PCR_MUX(0);
//            break;
//        case ADC_SC1n_ChnSelt_SE11:
//            if(mux_ab == 1)
//                PORTA->PCR[8] =  PORT_PCR_MUX(0);
//            break;
        case ADC_SC1n_ChnSelt_SE12:
            if(mux_ab == 1)
                PORTB->PCR[2] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE13:
            if(mux_ab == 1)
                PORTB->PCR[3] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE14:
            if(mux_ab == 1)
                PORTC->PCR[0] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE15:
            if(mux_ab == 1)
                PORTC->PCR[1] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE16:
            break;
//        case ADC_SC1n_ChnSelt_SE17:
//            if(mux_ab == 1)
//                PORTE->PCR[24] =  PORT_PCR_MUX(0);
//            break;
//        case ADC_SC1n_ChnSelt_SE18:
//            if(mux_ab == 1)
//                PORTE->PCR[25] =  PORT_PCR_MUX(0);
//            break;
        case ADC_SC1n_ChnSelt_DM0:
        case ADC_SC1n_ChnSelt_DM1:
        case ADC_SC1n_ChnSelt_SE21:
        case ADC_SC1n_ChnSelt_SE22:
        case ADC_SC1n_ChnSelt_SE23:
//        case ADC_SC1n_ChnSelt_SE24:
//        case ADC_SC1n_ChnSelt_SE25:
        case ADC_SC1n_ChnSelt_TempSensor:
        case ADC_SC1n_ChnSelt_Bandgap:
//        case ADC_SC1n_ChnSelt_SE28:
        case ADC_SC1n_ChnSelt_VREFH:
        case ADC_SC1n_ChnSelt_VREFL:
            break;
        default:
            return -1;
        }
    } else if (adcx == ADC1)
    {
        switch (chn)
        {
        case ADC_SC1n_ChnSelt_SEDP0_DiffDAD0:
        case ADC_SC1n_ChnSelt_SEDP1_DiffDAD1:
//        case ADC_SC1n_ChnSelt_SEDP2_DiffDAD2:
        case ADC_SC1n_ChnSelt_SEDP3_DiffDAD3:
            break;
        case ADC_SC1n_ChnSelt_SE4a_b:
            if(mux_ab == 1)
                PORTE->PCR[0] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE5a_b:
            if(mux_ab == 1)
                PORTE->PCR[1] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE6a_b:
            if(mux_ab == 1)
                PORTE->PCR[2] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE7a_b:
            if(mux_ab == 1)
                PORTE->PCR[3] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE8:
            if(mux_ab == 1)
                PORTB->PCR[0] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE9:
            if(mux_ab == 1)
                PORTB->PCR[1] =  PORT_PCR_MUX(0);
            break;
//        case ADC_SC1n_ChnSelt_SE10:
//            if(mux_ab == 1)
//                PORTA->PCR[7] =  PORT_PCR_MUX(0);
//            break;
//        case ADC_SC1n_ChnSelt_SE11:
//            if(mux_ab == 1)
//                PORTA->PCR[8] =  PORT_PCR_MUX(0);
//            break;
        case ADC_SC1n_ChnSelt_SE12:
            if(mux_ab == 1)
                PORTB->PCR[6] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE13:
            if(mux_ab == 1)
                PORTB->PCR[7] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE14:
            if(mux_ab == 1)
                PORTB->PCR[10] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE15:
            if(mux_ab == 1)
                PORTB->PCR[11] =  PORT_PCR_MUX(0);
            break;
        case ADC_SC1n_ChnSelt_SE16:
            break;
//        case ADC_SC1n_ChnSelt_SE17:
//            if(mux_ab == 1)
//                PORTE->PCR[24] =  PORT_PCR_MUX(0);
//            break;
//        case ADC_SC1n_ChnSelt_SE18:
//            if(mux_ab == 1)
//                PORTE->PCR[25] =  PORT_PCR_MUX(0);
//            break;
        case ADC_SC1n_ChnSelt_DM0:
        case ADC_SC1n_ChnSelt_DM1:
        case ADC_SC1n_ChnSelt_SE21:
        case ADC_SC1n_ChnSelt_SE22:
        case ADC_SC1n_ChnSelt_SE23:
//        case ADC_SC1n_ChnSelt_SE24:
//        case ADC_SC1n_ChnSelt_SE25:
        case ADC_SC1n_ChnSelt_TempSensor:
        case ADC_SC1n_ChnSelt_Bandgap:
//        case ADC_SC1n_ChnSelt_SE28:
        case ADC_SC1n_ChnSelt_VREFH:
        case ADC_SC1n_ChnSelt_VREFL:
            break;
        default:
            return -1;
        }
    }
    return 0;
}

void hw_adc_start_convert(ADC_TypeDef adcx,ADC_Channel_BitMapDef chn,uint8_t mux_ab)
{
    adcx->SC1[mux_ab] &=  ADC_SC1n_ChnDisable;
    adcx->SC1[mux_ab] |= chn;
}

int hw_adc_irq_enable(ADC_TypeDef adcx)
{
    uint8_t mux_ab = (adcx->CFG2 & ADC_CFG2_MUXSEL_MASK)>>ADC_CFG2_MUXSEL_SHIFT;
    if(adcx == ADC0) {
        adcx->SC1[mux_ab] |= (ADC_SC1_AIEN_MASK);
        enable_irq(ADC0_IRQn);
    } else if (adcx == ADC1) {
        adcx->SC1[mux_ab] |= (ADC_SC1_AIEN_MASK);
        enable_irq(ADC1_IRQn);
    } else 
        return -1;

    return 0;
}

int hw_adc_irq_disable(ADC_TypeDef adcx)
{
    if(adcx == ADC0)
    {
        disable_irq(ADC0_IRQn);
    }  else if (adcx == ADC1)
    {
        disable_irq(ADC1_IRQn);
    } else 
        return -1;

    return 0;
}

uint16_t hw_adc_softtrig_adval(ADC_TypeDef adcx,ADC_Channel_BitMapDef chn)
{
//    uint32 count = 0;
    adcx->SC1[0] &= ~(ADC_SC1_AIEN_MASK);
    adcx->SC1[0] &= ADC_SC1n_ChnDisable;
    adcx->SC1[0] |= chn;
    dbg_adc_reg_display("SoftTrg");
    while((adcx->SC1[0]&ADC_SC1n_ConvComplte_MASK) == 0);
//    {
//        count ++;
//        if(count == 0x1000)
//        {
//            count = 0;
//            adcx->SC1[0] &= ~(ADC_SC1_AIEN_MASK);
//            adcx->SC1[0] &= ADC_SC1n_ChnDisable;
//            adcx->SC1[0] |= chn;
//        }
//    }
//    
    return adcx->R[0];
}

//hardware tirger or irq
uint16_t hw_adc_hardtrig_adval(ADC_TypeDef adcx)
{
    if (adcx->SC1[0] & ADC_SC1n_ConvComplte_MASK)
    {
        return adcx->R[0];
    } else if (adcx->SC1[1] & ADC_SC1n_ConvComplte_MASK) 
    {
        return adcx->R[1];
    } else
    {
        ASSERT(0);
    }
    return -1;
}

