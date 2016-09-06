/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : system_drv.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/9/2014 8:24:06 PM
 * Description        : 
 *******************************************************************************/

#ifndef __SYSTEM_DRV_H__
#define __SYSTEM_DRV_H__

#define MODULE_SWITCH_ICCARD    ((uint)(0x00000001))
#define MODULE_SWITCH_SAM1      ((uint)(0x00000002))
#define MODULE_SWITCH_SAM2      ((uint)(0x00000004))
#define MODULE_SWITCH_RFC       ((uint)(0x00000008))
#define MODULE_SWITCH_USB       ((uint)(0x00000010))
#define MODULE_SWITCH_BT        ((uint)(0x00000020))
#define MODULE_SWITCH_ENABLE    ((uint)(0x80000000))
typedef union
{
    uint32_t periph;
    struct {
        uint32_t iccard:1;
        uint32_t sam1:1;
        uint32_t sam2:1;
        uint32_t rfc:1;
        uint32_t usb:1;
        uint32_t bt:1;
        uint32_t mtk:1;
        uint32_t exflash:1;
        uint32_t scan:1;
        uint32_t prn:1;
        uint32_t esam:1;
        uint32_t mag:1;
        uint32_t adc:1;
        uint32_t uart2:1;
        uint32_t sys:1;
        uint32_t beep:1;
        uint32_t ctp:1;
        uint32_t rfu:11;
        uint32_t low_en:1;   //是否可以进入低功耗
        uint32_t low_flag:1;  //进入低功耗标志
        uint32_t low_to_normal:1;  //状态改变
        uint32_t enable:1; //禁止节能使能总开关 1-ON 禁止节能
    }bm;
}LPWR_Switch;

typedef struct 
{
    volatile  uint32_t	timercount;
    LPWR_Switch lpwr;
}SYSTTEM_TYPE_DEF;


extern SYSTTEM_TYPE_DEF gSystem;


void drv_sys_init(void);
uint32_t sys_get_counter(void);
void sys_delay_ms(uint32_t duration_ms);
void s_DelayMs(uint32_t CountMs);
void s_DelayUs(uint32_t us);

#endif

