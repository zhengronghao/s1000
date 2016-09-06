/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : beep_drv.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 10/12/2014 4:18:51 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "beep_drv.h"

void drv_beep_open(uint32_t freq_hz)
{
    FTM_InitTypeDef ftm_init_struct;

    memset((char *)&ftm_init_struct,0,sizeof(ftm_init_struct));
    ftm_init_struct.FTM_Ftmx = BEEP_PWM_TIMER;
    ftm_init_struct.FTM_Mode = FTM_MODE_PWM;
    ftm_init_struct.FTM_PwmFreq = freq_hz;
    LPLD_FTM_Init(ftm_init_struct);
}

void drv_beep_close(void)
{
    FTM_InitTypeDef ftm_init_struct;

    drv_beep_stop();
    memset((char *)&ftm_init_struct,0,sizeof(ftm_init_struct));
    ftm_init_struct.FTM_Ftmx = BEEP_PWM_TIMER;
    LPLD_FTM_Deinit(ftm_init_struct);
}

void drv_beep_start(void)
{
    LPLD_FTM_PWM_Enable(BEEP_PWM_TIMER, //使用FTM0
                        BEEP_PWM_CHANNEL, //使能Ch0通道
                        FTM_PWM_DUTY_MAX/2, //占空比
                        BEEP_PTxy, //使用Ch0通道的PTC1引脚
                        ALIGN_LEFT        //脉宽左对齐
                       );    
}

void drv_beep_stop(void)
{
    LPLD_FTM_DisableChn(BEEP_PWM_TIMER,BEEP_PWM_CHANNEL); 
}


