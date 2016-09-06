/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
* File Name          : wp30_boot_isr.h
* Author             : luocs
* Version            : V0.00
* Date               : 2014.01.02
* Description        : Hardware pin assignments
*******************************************************************************/
#include "../wp30_boot.h"
#include "wp30_boot_isr.h"

typedef void (*IRQ_CALLBACK)(void);

const IRQ_CALLBACK  SysTickCallBack[] = {
    drv_kb_systick_irq,
    NULL
};

void SysTick_IRQCallBack(void)
{
    uint8_t i=0;

    while (SysTickCallBack[i] != NULL)
    {
        SysTickCallBack[i]();
        i++;
    }
}


void SysTick_Handler(void)
{
    gSystem.timercount += 10;
    SysTick_IRQCallBack();
//    if (gSystem.timercount%(1*100) == 0) {
//        TRACE("\ncore_clk_khz:%d\n",core_clk_khz);
//    }
}

void PORTB_IRQHandler(void)
{
    PORTB_KB_NotMaxtrix_IRQHandler();
}

void PORTC_IRQHandler(void)
{
    PORTC_KeyBoard_IRQHandler();
}


