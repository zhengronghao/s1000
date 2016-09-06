/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : rtc_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/18/2014 2:07:58 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "rtc_hw.h"

static void _rtc_clk_open(void)
{
    if ((SIM_SCGC6 & SIM_SCGC6_RTC_MASK) == 0) {
        TRACE("\nRtc clk open.SIM SCGC6:%02X",SIM_SCGC6);
        /*enable the clock to SRTC module register space*/
        SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;
    }
}

void hw_rtc_init( uint32_t seconds, uint32_t alarm)
{
    uint32_t i;

    /*enable the clock to SRTC module register space*/
    _rtc_clk_open();
    if (RTC_SR & RTC_SR_TIF_MASK){
        TRACE("\nRTC Invalid flag was set RTC_SR =  %#02X \n",  (RTC_SR) )  ;
    }
    TRACE("\n-|RTC 32.768OSC:%d",RTC_CR & RTC_CR_OSCE_MASK);
//    hw_rtc_reg_report();
    if ((RTC_CR & RTC_CR_OSCE_MASK) != RTC_CR_OSCE_MASK )
    {
        TRACE(" ->ENABLE");
        /*Only VBAT_POR has an effect on the SRTC, RESET to the part does not, 
         *so you must manually reset the SRTC to make sure everything is in a known state*/
        //Resets all RTC registers except for the SWR bit and the RTC_WAR and RTC_RAR registers . 
        //The SWR bit is cleared by POR and by software explicitly clearing it.
        RTC_CR  = RTC_CR_SWR_MASK;
        RTC_CR  &= ~RTC_CR_SWR_MASK;//software explicitly clearing  
        if (RTC_SR & RTC_SR_TIF_MASK){
            RTC_TSR = 0x00000000;   //  this action clears the TIF
            TRACE("\nRTC Invalid flag was set - Write to TSR done to clears RTC_SR =  %#02X \n",  (RTC_SR) )  ;
        }
//        hw_rtc_reg_report();

        /*Set time compensation parameters*/
        RTC_TCR = RTC_TCR_CIR(0) | RTC_TCR_TCR(0);

        RTC_CR  |= (RTC_CR_SC16P_MASK | RTC_CR_SC4P_MASK);
        /*Enable the oscillator*/
        RTC_CR |= RTC_CR_OSCE_MASK;
        /*Wait to all the 32 kHz to stabilize, refer to the crystal startup time in the crystal datasheet*/
        for(i=0;i<0x00FFFFFF;i++) {
            asm("nop");
        }
    }
    RTC_SR &= ~RTC_SR_TCE_MASK;
    /*Configure the timer seconds and alarm registers*/
    RTC_TSR = seconds;
    /*Configure the timer seconds and alarm registers*/
    RTC_TAR = alarm;
    RTC_TPR = 0;//reset prescaler register 

    /*Enable the counter*/
    RTC_SR |= RTC_SR_TCE_MASK;
    hw_rtc_reg_report();
}

void hw_rtc_colse(void)
{
    _rtc_clk_open();
    disable_irq(RTC_IRQn);
    disable_irq(RTC_Seconds_IRQn);
    RTC_SR &= (~RTC_SR_TCE_MASK);
    SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;  
}

//retrun: 1-rtc runing 0-rtc not runing
uint32_t hw_rtc_IfValid(void)
{
    _rtc_clk_open();
    return ((RTC_SR & RTC_SR_TIF_MASK) !=  RTC_SR_TIF_MASK);
//    return ((RTC_SR & RTC_SR_TCE_MASK) ==  RTC_SR_TCE_MASK);
}

void hw_rtc_irq_second_enable(void)
{
    _rtc_clk_open();
    RTC_IER |= RTC_IER_TSIE_MASK; //Time Seconds Interrupt Enable
    enable_irq(RTC_Seconds_IRQn);		
}

void hw_rtc_irq_enable(void)
{
    _rtc_clk_open();
    RTC_IER |= RTC_IER_TAIE_MASK; //Time Alarm Interrupt Enable
    enable_irq(RTC_IRQn);		
}

void hw_rtc_reg_report(void)
{
    _rtc_clk_open();
    TRACE("\n\nRTC_TSR    = 0x%08X,",  (RTC_TSR)) ;
    TRACE("RTC_TPR    = 0x%04X",     (RTC_TPR)) ;
    TRACE("\nRTC_TAR    = 0x%04X,\t",  (RTC_TAR)) ;
    TRACE("RTC_TCR    = 0x%04X",     (RTC_TCR)) ;
    TRACE("\nRTC_CR     = 0x%04X,\t",  (RTC_CR)) ;
    TRACE("RTC_SR     = 0x%04X",     (RTC_SR)) ;
    TRACE("\nRTC_CR     = %016b,\t",  (RTC_CR)) ;
    TRACE("\nRTC_LR     = 0x%04X,\t",  (RTC_LR)) ;
    TRACE("RTC_IER    = 0x%04X",     (RTC_IER)) ;
    TRACE("\nRTC_WAR    = 0x%04X,\t",  (RTC_WAR)) ;
    TRACE("RTC_RAR    = 0x%04X",     (RTC_RAR)) ;
    TRACE("\nRTC_TTSR    = 0x%04X,\t",  (RTC_TTSR)) ;
    TRACE("RTC_TER    = 0x%04X",     (RTC_TER)) ;
    TRACE("\nRTC_TDR    = 0x%04X,\t",  (RTC_TDR)) ;
}

int hw_rtc_read_second(uint32_t *second)
{
    int iRet = -1;
    if (hw_rtc_IfValid() == 1) {
        iRet = 0;
        *second = RTC_TSR;
    } 
    return iRet;
}

void RTC_IRQHandler(void)
{
    GPIOB_PSOR = 0x00080000;           // set Port B 19 indicate wakeup

    if((RTC_SR & RTC_SR_TIF_MASK)== 0x01)
    {
        hw_rtc_reg_report();
        TRACE("\nSRTC time invalid interrupt entered...\r\n");
        RTC_SR &= 0x07;  //clear TCE, or RTC_TSR can  not be written
        RTC_TSR = 0x00000000;  //clear TIF 
        //RTC_IER &= ~RTC_IER_TIIE_MASK;

    }	
    if((RTC_SR & RTC_SR_TOF_MASK) == 0x02)
    {
        TRACE("\nSRTC time overflow interrupt entered...\r\n");
        RTC_SR &= 0x07;  //clear TCE, or RTC_TSR can  not be written
        RTC_TSR = 0x00000000;  //clear TOF
    }	 	
    if((RTC_SR & RTC_SR_TAF_MASK) == 0x04)
    {
        TRACE("\nSRTC alarm interrupt entered...\r\n");
        TRACE("Time Seconds Register value is: %i\n", RTC_TSR);
        RTC_TAR += 11;// Write new alarm value, to generate an alarm every second add 1
    }
}

void RTC_Seconds_IRQHandler(void)
{
    TRACE("\nsystem seconds:%d", RTC_TSR);
}


