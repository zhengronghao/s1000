/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
* File Name          : ctrl_isr.h
* Author             : luocs
* Version            : V0.00
* Date               : 2014.01.02
* Description        : Hardware pin assignments
*******************************************************************************/
#include "wp30_ctrl.h"
#include "./daemon/wp30_daemon.h"




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
extern int idcard_scl_isr(void);
extern int idcard_sda_isr(void);
void PORTA_IRQHandler(void)
{
#if (defined CFG_ICCARD)
    IC_IO_IRQHandler();
    IC_INT_IRQHandler();
#endif

#if (defined CFG_RFID_IDCARD)
    if (PORTx_IRQPinx(IDCARD_SCL_GPIO_PORT, IDCARD_SCL_GPIO_PORT_PIN)) {
          PORTx_IRQPinx_Clear(IDCARD_SCL_GPIO_PORT, IDCARD_SCL_GPIO_PORT_PIN);
        idcard_scl_isr();
    }
    if (PORTx_IRQPinx(IDCARD_SDA_GPIO_PORT, IDCARD_SDA_GPIO_PORT_PIN)) {
        PORTx_IRQPinx_Clear(IDCARD_SDA_GPIO_PORT, IDCARD_SDA_GPIO_PORT_PIN);
        idcard_sda_isr();   
    }
#endif
}

void PORTB_IRQHandler(void)
{
//    extern void FIQHandler(void);
//    FIQHandler();
//#if (defined CFG_ICCARD)
//    IC_INT_IRQHandler();
//#endif
//#if (defined CFG_RFID)
//    RFID_IO_IRQHandler();
//#endif
    PORTB_KB_NotMaxtrix_IRQHandler();
    PORTB_PowerChargeNotice_IRQHandler();
}

void PORTC_IRQHandler(void)
{
    PORTC_KeyBoard_IRQHandler();
}

void PORTD_IRQHandler(void)
{
#if (defined CFG_ICCARD)
    SAM_IO_IRQHandler();
//    IC_IO_IRQHandler();
#endif
#if (defined CFG_LOWPWR)
    LowPower_Wakeup_IRQHandler();
#endif
}

void PORTE_IRQHandler(void)
{
#if (defined CFG_RFID)
    RFID_IO_IRQHandler();
#endif
}

/*
 * PIT0--PIT3中断处理函数
 * 与启动文件startup_K60.s中的中断向量表关联
 * 用户无需修改，程序自动进入对应通道中断函数
 */

void PIT0_IRQHandler(void)
{
//    PIT_ISR[0]();
    PIT_TFLG(PIT0) |= PIT_TFLG_TIF_MASK;
#if CFG_SCANER
#ifdef SCANER_EM1395
    extern void em1395_timer_isr(void);
    em1395_timer_isr();
#else
    extern void em3096_timer_isr(void);
    em3096_timer_isr();
#endif
#endif
}
void PIT1_IRQHandler(void)
{
//    PIT_ISR[1]();
    PIT_TFLG(PIT1) |= PIT_TFLG_TIF_MASK;
#if (defined(CFG_TPRINTER))
    extern void tp_printer_handler(void);
    extern void test_tp_forward_step(void);
    tp_printer_handler();
//    test_tp_forward_step();
#endif
}
void PIT2_IRQHandler(void)
{
//    PIT_ISR[2]();
#if (defined CFG_ICCARD)
  extern void icc_etu_timer_isr(void);
    PIT_TFLG(PIT2) |= PIT_TFLG_TIF_MASK;
    //IC卡etu超时中断 需要多次中断
//    PIT_TCTRL(PIT2) &= ~PIT_TCTRL_TIE_MASK;
    icc_etu_timer_isr();
#endif
}
void PIT3_IRQHandler(void)
{
//    PIT_ISR[3]();
    PIT_TFLG(PIT3) |= PIT_TFLG_TIF_MASK;
}

