/*
 *****************************************************************************
 * Copyright @ 2009 by austriamicrosystems AG                                *
 * All rights are reserved.                                                  *
 *                                                                           *
 * Reproduction in whole or in part is prohibited without the written consent*
 * of the copyright owner. Austriamicrosystems reserves the right to make    *
 * changes without notice at any time. The software is provided as is and    *
 * Austriamicrosystems makes no warranty, expressed, implied or statutory,   *
 * including but not limited to any implied warranty of merchantability or   *
 * fitness for any particular purpose, or that the use will not infringe any *
 * third party patent, copyright or trademark. Austriamicrosystems should    *
 * not be liable for any loss or damage arising from its use.                *
 *****************************************************************************
 */

/*
 * PROJECT: AS3911 firmware
 * $Revision: $
 * LANGUAGE: ANSI C
 */

/*! \file as3911_interrupt.c
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 interrupt handling and ISR
 */

#include "wp30_ctrl.h"

#if (defined(EM_AS3911_Module))
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*
******************************************************************************
* MACROS
******************************************************************************
*/

/*!
 *****************************************************************************
 * \brief Clear the interrupt flag associated with the as3911 interrupt.
 *****************************************************************************
 */
//#define AS3911_IRQ_CLR() { _IC1IF = 0; }

/*!
 *****************************************************************************
 * \brief Evaluates to true if there is a pending interrupt request from the
 * AS3911.
 *****************************************************************************
 */
#define AS3911_IRQ_IS_SET() ( _RB9 != 0)

/*
******************************************************************************
* LOCAL DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/

/*! AS3911 interrutp mask. */
static volatile u32 as3911InterruptMask = 0;
/*! Accumulated AS3911 interrupt status. */
static volatile u32 as3911InterruptStatus = 0;

/*
******************************************************************************
* LOCAL TABLES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL VARIABLE DEFINITIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/

s8 as3911EnableInterrupts(u32 mask)
{
    s8 error = ERR_NONE;
    u32 irqMask = 0;

    AS3911_IRQ_OFF();

    error |= as3911ContinuousRead(AS3911_REG_IRQ_MASK_MAIN, (u8*) &irqMask, 3);
    irqMask &= ~mask;
    as3911InterruptMask |= mask;
    error |= as3911ContinuousWrite(AS3911_REG_IRQ_MASK_MAIN, (u8*) &irqMask, 3);

    AS3911_IRQ_ON();

    if (ERR_NONE == error)
        return ERR_NONE;
    else
        return ERR_IO;
}

s8 as3911DisableInterrupts(u32 mask)
{
    s8 error = ERR_NONE;
    u32 irqMask = 0;

    AS3911_IRQ_OFF();

    error |= as3911ContinuousRead(AS3911_REG_IRQ_MASK_MAIN, (u8*) &irqMask, 3);
    irqMask |= mask;
    as3911InterruptMask &=  ~mask;
    error |= as3911ContinuousWrite(AS3911_REG_IRQ_MASK_MAIN, (u8*) &irqMask, 3);

    AS3911_IRQ_ON();

    if (ERR_NONE == error)
        return ERR_NONE;
    else
        return ERR_IO;
}

s8 as3911ClearInterrupts(u32 mask)
{
    s8 error = ERR_NONE;
    u32 irqStatus = 0;
    u32 irqMasktmp = 0;

    AS3911_IRQ_OFF();

    error |= as3911ContinuousRead(AS3911_REG_IRQ_MAIN, (u8*) &irqStatus, 3);
//    as3911InterruptStatus |= irqStatus & as3911InterruptMask;
    irqMasktmp = irqStatus & as3911InterruptMask;
    as3911InterruptStatus |= irqMasktmp;
    as3911InterruptStatus &= ~mask;

    AS3911_IRQ_ON();

    if (ERR_NONE == error)
        return ERR_NONE;
    else
        return ERR_IO;
}

uint as3911_timerout(uint timer0,uint timerout)
{
	uint timer1=sys_get_counter();
	if(timer1 >= timer0)
	{
		if(timer1 - timer0 >= timerout)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		timer0 = UINT_MAX - timer0;
		if(timer0+timer1 >= timerout)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

s8 as3911WaitForInterruptTimed(u32 mask, u16 timeout, u32 *irqs)
{
    uchar timerExpired = OFF;
    volatile uint irqStatus = 0;
    uint irqMasktmp = 0;
    uint timer0 = 0;

    if (timeout > 0)
        timer0 = sys_get_counter();

#if AS3911_IRQMODE
    do
    {
        irqStatus = as3911InterruptStatus & mask;

        if ( timeout > 0 ) {
            if(as3911_timerout(timer0,timeout))
            {
                timerExpired = ON;

//                //08262013 chenf
//                DISPBUF(gucDebugBuf, guiDebugS3, 0);
//                vDispBufKey(guiDebugS3,gucDebugBuf);
//                TRACE("\r\n timeout :%x - mask:%x",as3911InterruptStatus,mask);
//                InkeyCount(0);
            }
        }
    } while (!irqStatus && !timerExpired);
#else
    uint irqStatusISR,tmp;
    do
    {
        if (1 == hw_gpio_get_bit(RFID_IO_GPIO,(1<<RFID_IO_PINx)))
        {

            as3911ContinuousRead(AS3911_REG_IRQ_MAIN, (u8*) &irqStatusISR, 3);

            D2(LABLE(0xAA);//1us
               DATAIN((irqStatusISR));
               DATAIN((irqStatusISR>>8));
               DATAIN((irqStatusISR>>16));
              );

            tmp = irqStatusISR & as3911InterruptMask;
            as3911InterruptStatus |= tmp;
        }
        irqStatus = as3911InterruptStatus & mask;

        if ( timeout > 0 ) {
            if(as3911_timerout(timer0,timeout))
            {
                timerExpired = ON;
            }
        }
    } while (!irqStatus && !timerExpired);
#endif

    irqMasktmp = irqStatus;
    AS3911_IRQ_OFF();
//    as3911InterruptStatus &= ~irqStatus;
    as3911InterruptStatus &= ~irqMasktmp;
    AS3911_IRQ_ON();

    *irqs = irqStatus;

    return ERR_NONE;
}

s8 as3911GetInterrupts(u32 mask, u32 *irqs)
{
    AS3911_IRQ_OFF();

    *irqs = as3911InterruptStatus & mask;
    as3911InterruptStatus &= ~mask;

    AS3911_IRQ_ON();

    return ERR_NONE;
}

//void __attribute__((interrupt, no_auto_psv)) _IC1Interrupt(void)
void as3911Isr(void)
{
//    volatile uint count = 0;
    uint irqStatus = 0;
    uint tmp = 0;
    volatile uint timer;
    static u8 lastIntHadNRE = 0;

//    GPIO_ResetBits(GPIOB,GPIO_Pin_1);
//    s_DelayUs(20);
//    TRACE("\r\n--------------------i\r\n");
//    count = 0;
//    SETSIGNAL_H();
#if 11
    while (1 == hw_gpio_get_bit(RFID_IO_GPIO,(1<<RFID_IO_PINx)))
    {
        as3911ContinuousRead(AS3911_REG_IRQ_MAIN, (u8*) &irqStatus, 3);
        D2(LABLE(0xAA);//1us
           DATAIN((irqStatus));
           DATAIN((irqStatus>>8));
           DATAIN((irqStatus>>16));
        );
//        as3911InterruptStatus |= irqStatus & as3911InterruptMask;
        tmp = irqStatus & as3911InterruptMask;
        as3911InterruptStatus |= tmp;
       	if (irqStatus & AS3911_IRQ_MASK_NRE)
       	{
       		if (lastIntHadNRE == 2)
       		{
       			u8 reg;
       			as3911ReadRegister(AS3911_REG_GPT_CONTROL, &reg);
       			as3911WriteRegister(AS3911_REG_GPT_CONTROL, reg & ~AS3911_REG_GPT_CONTROL_nrt_emv);
       			as3911ExecuteCommand(AS3911_CMD_CLEAR_FIFO);
       			as3911WriteRegister(AS3911_REG_GPT_CONTROL, reg);
       		}
			else if (lastIntHadNRE == 1)
       			lastIntHadNRE = 2;
       		else
       			lastIntHadNRE = 1;
    	}
       	else
       	{
       		lastIntHadNRE = 0;
       	}
    }

#else

    //在一次外部中断产生两次中断 如接收开始和no-response 此时有可能产生两个
    //外部中断 cpu退出后再响应 但此时irq已经为低 寄存器值全零
    //例子
    /*
        ===========================
        EE: 00 02 
        EE: 30 00 00 00 
        EE: 00 
        EE: 00 
        AA: 08 00 00 FE FE 
        DD: 08 00 00 
        AA: 20 00 00        //这里在一次中断处理中产生了两次中断
        AA: 02 40 00 FE FE 
        AA: 00 00 00 FE FE  //此时又响应了一次中断
        ------------
        CC: 20 40 00 F1 F1 F2 F2 
        55: 00 00 00 
        AA: 10 00 00 FE FE 
        ------------
        CC: 10 00 00 F3 F3 00 02 00 00 00 00 
        88: 00 00 00 
        66: 02 00 
        BB: 00 02 02 04 00 
        ===========================
        EE: 00 00 
        EE: 02 00 00 02 50 00 
        EE: 00 
        EE: 00 
        AA: 08 00 00 FE FE 
        DD: 08 00 00 
        AA: 02 40 00 FE FE 
     */
    for ( timer = 0 ; timer < 10 ; timer++ ) {
        //3.62us
        ;
    }
    while (1 == hw_gpio_get_bit(RFID_IO_GPIO,(1<<RFID_IO_PINx)))
    {
//        AS3911_IRQ_CLR();

        as3911ContinuousRead(AS3911_REG_IRQ_MAIN, (u8*) &irqStatus, 3);

//        SETSIGNAL2_H();
        for ( timer = 0 ; timer < 10 ; timer++ ) {
            //20 - 3.62us   10 - 1.8us
            //TB305_12 只能10  TA304_2必须延时
            ;
        }
        
        D2(LABLE(0xAA);//1us
           DATAIN((irqStatus));
           DATAIN((irqStatus>>8));
           DATAIN((irqStatus>>16));
        );
//        SETSIGNAL2_L();

//        as3911InterruptStatus |= irqStatus & as3911InterruptMask;
        tmp = irqStatus & as3911InterruptMask;
        as3911InterruptStatus |= tmp;
//        count++;
//        if ( count >= 10000) {
//            count--;
//            TRACE("\r\n ============== irq error!!");
//        }
//        if ( IfInkey(0) ) {
//            TRACE("\r\n ============== irq error:%d!!",count);
//            InkeyCount(0);
//        }
    }
#endif
//        D2(LABLE(0xFE););//1us
//    SETSIGNAL_L();
}

//volatile uint gas3911timeCount = 0;
//volatile ushort gas3911referencetime = 50;
//void as3911_timerISR(void)
//{
////    static int tflg = 0;
////
////    if ( tflg == 0 ) {
////        tflg = 1;
////        SETSIGNAL_H();
////    }else{
////        tflg = 0;
////        SETSIGNAL_L();
////    }
//    if ( gas3911timeCount ) {
//        gas3911timeCount--;
//    }
//    return;
//}
//
//static __inline__ TIM_TypeDef *s_as3911_Id2TIM(uint id)
//{
//	switch(id)
//	{
//		case 0:
//		default:
//			return TIM1;
//		case 1:
//			return TIM2;
//		case 2:
//			return TIM3;
//		case 3:
//			return TIM4;
//#if Product_Type == Product_F05_2 || Product_Type == Product_F14 || Product_Type == Product_F11_2
//		case 4:
//			return TIM5;
//		case 5:
//			return TIM6;
//		case 6:
//			return TIM7;
//		case 7:
//			return TIM8; 
//#endif
//	}
//}
//
//void as3911_CloseTimer(uint id)
//{
//	s_HardTimer_Close(id);
//}
//
//void as3911_InitTimer(uint id)
//{
//	s_HardTimer_Close(id);
//    //优先级比射频外部中断高   初始500us
//    s_HardTimer_Open(id, (4<<1)|INT_SUBPRI_MF,gas3911referencetime,as3911_timerISR);
//}
//
//void as3911_StartTimer(uint id)
//{
//	TIM_TypeDef *TIMx;
//
//	TIMx = s_as3911_Id2TIM(id);
//	TIM_SetCounter(TIMx,0);
//	s_HardTimer_vStart(id);    //启动
//}
//
//void as3911_StopTimer(uint id)
//{
//    s_HardTimer_vStop(id);  //关闭
//}
//
//void as3911_UpdateTimer(uint id, ushort timer)
//{
//    //50 -- 500us
//    gas3911referencetime = timer/10;
//    s_HardTimer_vUpdate(id, gas3911referencetime);
//}
//
//void as3911_SetTimerCount(uint timeus)
//{
//    //time == gas3911timeCount*updatetime   e.g 2*50 (2*500 == 1000us)
//    timeus /= 10;
//    gas3911timeCount = timeus/gas3911referencetime;
//}
//
//uint as3911_GetTimerCount(void)
//{
//    return gas3911timeCount;
//}
//
//void as3911_WaitForTimer(uint id, uint timeus)
//{
//	TIM_TypeDef *TIMx;
//
//    as3911_StopTimer(id);
//    gas3911timeCount = 0;
//    if ( !timeus ) {
//        return;
//    }
//	TIMx = s_as3911_Id2TIM(id);
//	TIM_SetCounter(TIMx,0);
//    timeus /= 10;
//    gas3911timeCount = timeus/gas3911referencetime;
//    as3911_StartTimer(id);
//    while(gas3911timeCount);
//    as3911_StopTimer(id);
//    return;
//}
//
//void as3911_DelayForTimer(uint id)
//{
//    while(gas3911timeCount);
//    as3911_StopTimer(id);
//    return;
//}
/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

#endif

