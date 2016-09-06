/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : dryice_drv.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 5/26/2014 11:43:27 AM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "drv_inc.h"
#include "dryice_drv.h"

void drv_dryice_clock_init(void)
{
    SIM_SCGC5|=SIM_SCGC5_SECREG_MASK | SIM_SCGC5_DRYICE_MASK;
}

int drv_dryice_open(uint8_t tamperpin0,uint8_t tamperpin1,uint8_t tamperpin2,uint8_t trig_level,uint8_t reset)
{
    uint32_t gitch_filter=0;

//    gitch_filter = FILTER_TAMPER_PIN_LOGIC_0|FILTER_PullResistor_ENABLE|FILTER_CLK_512_HZ
//        | FILTER_GLITCH_FILTER_WITH(10);
    gitch_filter = FILTER_TAMPER_PIN_LOGIC_0;//|FILTER_CLK_512_HZ;//  | FILTER_GLITCH_FILTER_WITH(10);
    DisableInterrupts;
    hw_dryice_init();
    DRY_HAL_Init(DRY);
    DRY_HAL_Enable(DRY);
    if (tamperpin0) {
        hw_dryice_tamper_cfg(TAMPER_PIN0,trig_level,gitch_filter);
    }
    if (tamperpin1) {
        hw_dryice_tamper_cfg(TAMPER_PIN1,trig_level,gitch_filter);
    }
    if (tamperpin2) {
        hw_dryice_tamper_cfg(TAMPER_PIN2,trig_level,gitch_filter);
    }
    dryice_int_tamper_config(TIME_OVERFLOW,ENABLE);
    dryice_int_tamper_config(MONOTONIC_OVERFLOW,ENABLE);
    dryice_int_tamper_config(VBAT_TAMPER,ENABLE);
    dryice_int_tamper_config(RTCOSC_TAMPER,ENABLE);
    dryice_int_tamper_config(TEMPERATURE_TAMPER,ENABLE);
//    dryice_int_tamper_interrupt_config(DRYICE_TAMPER,ENABLE);
//    dryice_int_tamper_interrupt_config(TIME_OVERFLOW,ENABLE);
//    dryice_int_tamper_interrupt_config(MONOTONIC_OVERFLOW,ENABLE);
//    dryice_int_tamper_interrupt_config(VBAT_TAMPER,ENABLE);
//    dryice_int_tamper_interrupt_config(RTCOSC_TAMPER,ENABLE);
//    dryice_int_tamper_interrupt_config(TEMPERATURE_TAMPER,ENABLE);
    dryice_enable_TamperForceSystem(reset);
//    enable_irq(Tamper_IRQn); 
    EnableInterrupts;
    return 0;
}

int drv_dryice_init(hal_dry_tamper_t bm_tamp_sources,
                    hal_dry_interrupt_t bm_interrupt,
                    uint8_t bm_trig_level,
                    uint8_t reset)
{
    hal_dry_pin_polarity_t bm_polarity_level;
    hal_dry_tamper_pin_selector_t bm_pin;

    TRACE("\n-|t_src:%04X irq:%04X trig_level:%04X ",bm_tamp_sources,bm_interrupt,bm_trig_level);
    bm_pin = (hal_dry_tamper_pin_selector_t)((bm_tamp_sources&DRY_TER_TPE_MASK)>>DRY_TER_TPE_SHIFT);
    bm_polarity_level = (hal_dry_pin_polarity_t)((bm_trig_level^bm_pin)&bm_pin);
    bm_interrupt &= (hal_dry_interrupt_t)(bm_tamp_sources|0x01);
    TRACE("\n-|t_src:%04X irq:%04X trig_level:%04X pin:%02X",bm_tamp_sources,bm_interrupt,bm_polarity_level,bm_pin);
    hw_dryice_init();
    DRY_HAL_Init(DRY);
    // add cr_reg
    DRY_HAL_SetHysteresis(DRY, kDryHystType_490mV);
    DRY_HAL_SetPassiveFilter(DRY, kDryEnable);

    DRY_HAL_Enable(DRY);
    DRY_HAL_SetTamperPinPolarity(DRY,&bm_pin,bm_polarity_level);
    DRY_HAL_SetTamperPinDirection(DRY,&bm_pin,( hal_dry_pin_direction_t)0);//±»¶¯´¥·¢input:kDryTamperPinDirection_In
    DRY_HAL_SetGlitchFilterExpected(DRY,bm_pin,kDryGlitchFilterExpected_LogicZero);
//    DRY_HAL_SetGlitchFilterClock(DRY,&bm_pin,kDryGlitchFilterClock_32768Hz,10);
//    DRY_HAL_GlitchFilterEnable(DRY,&bm_pin);
    DRY_HAL_EnableTamper(DRY,bm_tamp_sources);
    DRY_HAL_EnableInterrupt(DRY,bm_interrupt);
    if (reset == ENABLE)
    {
        DRY_HAL_SetForceSystemReset(DRY);
    }
    if (bm_interrupt) {
        enable_irq(Tamper_IRQn); 
    }

    TRACE("\r\nget dry_cr_reg:%x\r\n", DRY_CR_REG(DRY));
    return 0;
}

int drv_dryice_active_init(hal_dry_tamper_t bm_tamp_sources,
                           hal_dry_interrupt_t bm_interrupt,
                           struct DryiceActive atr[DRY_ATR_COUNT],
                           uint8_t reset)
{
    hal_dry_tamper_pin_selector_t bm_pin;
    hal_dry_glitch_filter_expected_t  filter_expected;
    uint32_t i;

    bm_interrupt &= (hal_dry_interrupt_t)(bm_tamp_sources|0x01);
    TRACE("\n-|t_src:%04X irq:%04X",bm_tamp_sources,bm_interrupt);
    hw_dryice_init();
    DRY_HAL_Init(DRY);
    DRY_HAL_SetActiveTamperClock(DRY,kDryActiveTamperReg_All,kDryClkType_1Hz,0);
    DRY_HAL_Enable(DRY);
    for ( i=0; i<DRY_ATR_COUNT; i++,atr++)
    {
        bm_pin = (hal_dry_tamper_pin_selector_t)atr->bm_pin;
        if (bm_pin == 0) {
            continue;
        }
        bm_tamp_sources &= ((~atr->bm_pin_direction)<<DRY_IER_TPIE_SHIFT);
        filter_expected = (i==0)?(kDryGlitchFilterExpected_ActTamperOut_0):(kDryGlitchFilterExpected_ActTamperOut_1);
        TRACE("\n-|[ATR%d]pin:%02X direction:%02X poly:%04X shift:%04X tamper src:%04X",i,bm_pin,
              atr->bm_pin_direction,atr->polynomial,atr->shift,bm_tamp_sources);
        DRY_HAL_SetActiveTamperShift(DRY,(hal_dry_act_tamper_selector_t)(1U<<i),atr->shift);
        DRY_HAL_SetActiveTamperPolynomial(DRY,(hal_dry_act_tamper_selector_t)(1U<<i),atr->polynomial);
        DRY_HAL_SetTamperPinPolarity(DRY,&bm_pin,atr->bm_pin_polarity);
        DRY_HAL_SetTamperPinDirection(DRY,&bm_pin,atr->bm_pin_direction);
        DRY_HAL_SetGlitchFilterExpected(DRY,bm_pin,filter_expected);
        DRY_HAL_SetGlitchFilterClock(DRY,bm_pin,atr->prescaler,atr->width);
        DRY_HAL_SetGlitchFilterPull(DRY,bm_pin,kDryEnable,kDryGlitchFilterPullType_Assert);
        DRY_HAL_GlitchFilterEnable(DRY,bm_pin);
    }
    DRY_HAL_EnableTamper(DRY,bm_tamp_sources);
    DRY_HAL_EnableInterrupt(DRY,bm_interrupt);
    if (reset == ENABLE) {
        DRY_HAL_SetForceSystemReset(DRY);
    }
    if (bm_interrupt) {
        enable_irq(Tamper_IRQn); 
    }
    return 0;
}



void drv_dryice_irq_open(void)
{
    SIM_SCGC5|=SIM_SCGC5_SECREG_MASK | SIM_SCGC5_DRYICE_MASK;
    enable_irq(Tamper_IRQn); 
}

/**  
 * \brief   Configure the active tamper register.
 * \author   
 * \param 
 *         active_tamper_n   ACTIVE_TAMPER_0/ACTIVE_TAMPER_1
 * \return none
 * \warning . 
 */
void drv_dryice_active_tamper_config(int active_tamper_n, int desired_polynomial, int desired_shift_reg_value)
{
    int32 atr_reg_contents1,atr_reg_contents2,atr_reg_contents3;


    DRY_TRACE("\n-------------------------------------------\n");
    DRY_TRACE("  DryIce Pin Active Tamper Config Utility\n");
    DRY_TRACE("-------------------------------------------\n");
    DRY_TRACE("\n");

    if ((DRY_CR & DRY_CR_DEN_MASK) != DRY_CR_DEN_MASK)
    {
        DRY_TRACE("DryIce prescaler and clock are disabled. Please enable them first before modifying the active tamper registers.\n\n");
        DRY_TRACE("\n");
        return;
    }  

    switch (active_tamper_n)
    {
    case 0:
        if (((DRY_RAC & DRY_RAC_ATR(0x1))>>(DRY_RAC_ATR_SHIFT+0) == 0x0))
        {
            DRY_TRACE("Cannot read DRY_ATR0 because its read access is blocked. Remove the read access block and try again.\n");
            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
            DRY_TRACE("\n");
            return;
        }    
        if (((DRY_WAC & DRY_WAC_ATW(0x1))>>(DRY_WAC_ATW_SHIFT+0) == 0x0))
        {
            DRY_TRACE("Cannot write DRY_ATR0 because its write access is blocked. Remove the write access block and try again.\n");
            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
            DRY_TRACE("\n");
            return;
        }
        if (((DRY_LR & DRY_LR_ATL(0x1))>>(DRY_LR_ATL_SHIFT+0) == 0x0))
        {
            DRY_TRACE("Cannot write DRY_ATR0 because it is locked. Remove the lock and try again.\n");
            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
            DRY_TRACE("\n");
            return;
        }            
        DRY_TRACE("\nCurrent DRY_ATR0 is %08X\n",DRY_ATR0);
        atr_reg_contents1 = DRY_ATR_ATP(desired_polynomial);
        atr_reg_contents2 = DRY_ATR_ATSR(desired_shift_reg_value);
        atr_reg_contents3 = atr_reg_contents1 | atr_reg_contents2;    
        DRY_TRACE("Writing new values to DRY_ATR0.\n");    
        DRY_ATR0 = atr_reg_contents3;    
        DRY_TRACE("New DRY_ATR0 is %32b",DRY_ATR0);    
        DRY_TRACE(" (%08X)\n",DRY_ATR0);
        break;

    case 1:
        if (((DRY_RAC & DRY_RAC_ATR(0x2))>>(DRY_RAC_ATR_SHIFT+1) == 0x0))
        {
            DRY_TRACE("Cannot read DRY_ATR1 because its read access is blocked. Remove the read access block and try again.\n");
            DRY_TRACE("DRY_RAC is \t%08X\n",DRY_RAC);    
            DRY_TRACE("\n");
            return;
        }
        if (((DRY_WAC & DRY_WAC_ATW(0x2))>>(DRY_WAC_ATW_SHIFT+1) == 0x0))
        {
            DRY_TRACE("Cannot write DRY_ATR1 because its write access is blocked. Remove the write access block and try again.\n");
            DRY_TRACE("DRY_WAC is \t%08X\n",DRY_WAC);    
            DRY_TRACE("\n");
            return;
        }
        if (((DRY_LR & DRY_LR_ATL(0x2))>>(DRY_LR_ATL_SHIFT+1) == 0x0))
        {
            DRY_TRACE("Cannot write DRY_ATR1 because it is locked. Remove the lock and try again.\n");
            DRY_TRACE("DRY_LR is \t%08X\n",DRY_LR);    
            DRY_TRACE("\n");
            return;
        }        
        DRY_TRACE("\n Current DRY_ATR1 is %08X\n",DRY_ATR1);
        atr_reg_contents1 = DRY_ATR_ATP(desired_polynomial);
        atr_reg_contents2 = DRY_ATR_ATSR(desired_shift_reg_value);
        atr_reg_contents3 = atr_reg_contents1 | atr_reg_contents2;    
        DRY_TRACE("Writing new values to DRY_ATR1.\n");    
        DRY_ATR1 = atr_reg_contents3;    
        DRY_TRACE("New DRY_ATR1 is %32b",DRY_ATR1); 
        DRY_TRACE(" (%08X)\n",DRY_ATR1);    
        break;
    }
    DRY_TRACE("\n");  
    return;
}
/**  
 * \brief   Then configure the tamper pin expected value to the desired active tamper output,
 *      set the tamper pin polarity to inverted and set the tamper pin direction to output.
 * Do not enable that tamper pin as an input tamper source, since the output data
 * is always the inverse of the expected value which would trigger a tamper event.
 * \author   
 * \param 
 *         tamperpin         TAMPER_PIN0/TAMPER_PIN1/TAMPER_PIN2
 *         expect_value      FILTER_TAMPER_PIN_ACTIVE_0/FILTER_TAMPER_PIN_ACTIVE_1/FILTER_TAMPER_PIN_ACTIVE_0_XOR_1/
 * \return none
 * \warning . 
 */
int drv_dryice_active_output(uint8_t tamperpin,uint32_t expect_value)
{
    hw_dryice_pgfr_cfg(tamperpin,expect_value);
    hw_dryice_tamper_pin_polarity_config(tamperpin,TAMPER_PIN_INVERTED);//PPR:TPP
    hw_dryice_tamper_pin_direction_config(tamperpin,TAMPER_PIN_SET_OUTPUT_INVERSE);//PDR:TPD
    return 0;
}

/**  
 * \brief   To configure a tamper pin as an active tamper input, first configure the active tamper
 *register and enable any active tamper outputs. Then configure the tamper pin expected
 *value to the desired active tamper output and enable the pull-resistor if desired. The glitch
 *filter must be enabled to account for the propagation delay through the pads and on the
 *board. Finally, enable the tamper pin as either a tamper source or an interrupt source.
 * \author   
 * \param 
 *         width             0~63
 *         prescaler_clk     FILTER_CLK_512_HZ/FILTER_CLK_32_KHZ
 *         active_tamper_n   ACTIVE_TAMPER_0/ACTIVE_TAMPER_1/ACTIVE_TAMPER_0_1
 * \return none
 * \warning . 
 */
int drv_dryice_active_input(uint8_t tamperpin,uint8_t atr_n,uint8_t width,uint8_t prescaler_clk)
{
    uint32_t pgfr=0;
    int iRet;
    if (atr_n == ACTIVE_TAMPER_0)
    {
        pgfr = FILTER_TAMPER_PIN_ACTIVE_0;
    } else if (atr_n == ACTIVE_TAMPER_1)
    {
        pgfr = FILTER_TAMPER_PIN_ACTIVE_1;
    } else
    {
        pgfr = FILTER_TAMPER_PIN_ACTIVE_0_XOR_1;
    }
    pgfr |= (  FILTER_PullResistor_ENABLE 
             | FILTER_GLITCH_FILTER_WITH(width)
             | prescaler_clk
             | FILTER_GLITCH_ENABLE);
    iRet = hw_dryice_pgfr_cfg(tamperpin,pgfr);
    dryice_pin_tamper_config(tamperpin,ENABLE);
//    dryice_int_tamper_interrupt_config(DRYICE_TAMPER,ENABLE);
    return iRet;
}


void DryIce_Tamper_IRQHandler(void) 
{
//    uint8_t i,j;
    DRY_TRACE("\n--|Entered DryIce ISR\n");
#ifdef CFG_DBG_HW_DRYICE 
    dbg_dryice_reg();
#endif
    TRACE("\n");
    if((((DRY_SR & DRY_SR_TOF_MASK) == 0x04) && (DRY_IER & DRY_IER_TOIE_MASK)==DRY_IER_TOIE_MASK))
    {
        DRY_TRACE("DryIce Time Overflow in RTC detected...\r\n");
        DRY_TRACE("Disabling RTC time counter.\n");
        RTC_SR &= ~RTC_SR_TCE_MASK;
        DRY_TRACE("Writing to RTC_TSR to clear RTC_SR[TOF].\n");
        RTC_TSR |= 0xAAAA5555;
        DRY_TRACE("Writing to DRY_SR[TOF] to clear DRY_SR[TOF].\n");
        DRY_SR |= DRY_SR_TOF_MASK;
        DRY_TRACE("DRY_SR is %08X\n",DRY_SR);
    }	 	
    if((((DRY_SR & DRY_SR_MOF_MASK) == 0x08)&& (DRY_IER & DRY_IER_MOIE_MASK)==DRY_IER_MOIE_MASK))
    {
        DRY_TRACE("DryIce Monotonic Overflow in RTC detected...\r\n");
        DRY_TRACE("Disabling RTC monotonic counter.\n");           
        RTC_MER &= ~RTC_MER_MCE_MASK; // Disable monotonic counter
        DRY_TRACE("Writing to RTC_MCHR to clear RTC_SR[MOF].\n");           
        RTC_MCHR |= 0xFFFFFFFF; // Write to monotonic counter register high
        DRY_TRACE("Writing to DRY_SR[MOF] to clear DRY_SR[MOF].\n");
        DRY_SR |= DRY_SR_MOF_MASK;
        DRY_TRACE("DRY_SR is %08X\n",DRY_SR);
    }	
    if((((DRY_SR & DRY_SR_VTF_MASK) == 0x10)&& (DRY_IER & DRY_IER_VTIE_MASK)==DRY_IER_VTIE_MASK))
    {
        DRY_TRACE("DryIce Voltage Tamper detected...\r\n");
        DRY_TRACE("Please bring VBAT voltage to within the acceptable range and press any key to continue\n");
        // Wait to ensure VBAT is within proper range first                      
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
        DRY_TRACE("Clearing VTF by writing 1 to it.\n");
        DRY_SR |= DRY_SR_VTF_MASK; // clear VTF
        DRY_TRACE("DRY_SR is %08X\n",DRY_SR);           
    }	
    if((((DRY_SR & DRY_SR_CTF_MASK) == 0x20)&& (DRY_IER & DRY_IER_CTIE_MASK)==DRY_IER_CTIE_MASK))
    {
        DRY_TRACE("DryIce Clock Tamper detected...\r\n");
        DRY_TRACE("Please bring EXTAL32 clock frequency to within the acceptable range and press any key to continue\n");
        // Ensure clock is within proper range first
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
        DRY_TRACE("Clearing CTF by writing 1 to it.\n");
        DRY_SR |= DRY_SR_CTF_MASK; // clear CTF
        DRY_TRACE("DRY_SR is %08X\n",DRY_SR);                      
    }	
    if((((DRY_SR & DRY_SR_TTF_MASK) == 0x40)&& (DRY_IER & DRY_IER_TTIE_MASK)==DRY_IER_TTIE_MASK))
    {
        DRY_TRACE("DryIce Temperature Tamper detected...\r\n");
        DRY_TRACE("Please bring temperature to within the acceptable range and press any key to continue\n");           
        // Ensure temperature is within proper range first
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
        DRY_TRACE("Clearing TTF by writing 1 to it.\n");           
        DRY_SR |= DRY_SR_TTF_MASK; // clear TTF
        DRY_TRACE("DRY_SR is %08X\n",DRY_SR);                      
    }
    if((((DRY_SR & DRY_SR_FSF_MASK) == 0x100)&& (DRY_IER & DRY_IER_FSIE_MASK)==DRY_IER_FSIE_MASK))
    {
        DRY_TRACE("DryIce Flash Security Tamper detected...\r\n");
        DRY_TRACE("Flash security byte is: %08X\n",FTFE_FSEC);
        // Ensure flash security has been enabled first
        if ((FTFE_FSEC & 0x2) == 0x2)
        {
            DRY_TRACE("Flash is unsecured. Cannot clear FSF bit. Need to secure the flash first before \n");
            DRY_TRACE("Please issue a reset and unsecure the flash.\n");
        }
        else
        {
            DRY_TRACE("Flash is secured. Attempting to clear FSF bit.\n");
            DRY_SR |= DRY_SR_FSF_MASK; // clear FSF       
        }
    }	
    if((((DRY_SR & DRY_SR_TMF_MASK) == 0x200)&& (DRY_IER & DRY_IER_TMIE_MASK)==DRY_IER_TMIE_MASK))
    {
        DRY_TRACE("DryIce Test Mode detected...\r\n");
        DRY_TRACE("DryIce Test Mode must be exited before TMF can be cleared.\n");
        // Ensure test mode has been exited first
        DRY_TRACE("Exit test mode and enter any key to continue.\n");
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
        DRY_SR &= DRY_SR_TMF_MASK; // clear VTF
    }	
    if((((DRY_SR & DRY_SR_TPF(0x1)) == DRY_SR_TPF(0x1))&& (DRY_IER & DRY_IER_TPIE(0x1))==DRY_IER_TPIE(0x1)))
    {
        DRY_TRACE("DryIce Tamper Pin 0 tamper event detected...\r\n");
        // Ensure tamper pin equals its expected value
        DRY_TRACE("Change Tamper Pin 0 to its expected value and press any key to continue.\n");
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
        DRY_SR |= DRY_SR_TPF(0x1); // clear TPF for associated pin
    }	
    if((((DRY_SR & DRY_SR_TPF(0x2)) == DRY_SR_TPF(0x2))&& (DRY_IER & DRY_IER_TPIE(0x2))==DRY_IER_TPIE(0x2)))
    {
        DRY_TRACE("DryIce Tamper Pin 1 tamper event detected...\r\n");
        // Ensure tamper pin equals its expected value
        DRY_TRACE("Change Tamper Pin 1 to its expected value and press any key to continue.\n");
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
        DRY_SR |= DRY_SR_TPF(0x02); // clear TPF for associated pin

    }	
    if ((((DRY_SR & DRY_SR_TPF(0x4)) == DRY_SR_TPF(0x4))&& (DRY_IER & DRY_IER_TPIE(0x4))==DRY_IER_TPIE(0x4)))
    {
        DRY_TRACE("DryIce Tamper Pin 2 tamper event detected...\r\n");
        // Ensure tamper pin equals its expected value
        DRY_TRACE("Change Tamper Pin 2 to its expected value and press any key to continue.\n");
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
        DRY_SR |= DRY_SR_TPF(0x04); // clear TPF for associated pin
    }
    if(((DRY_SR & DRY_SR_DTF_MASK)== DRY_SR_DTF_MASK) && ((DRY_IER & DRY_IER_DTIE_MASK) == DRY_IER_DTIE_MASK))
    {
        DRY_TRACE("DryIce Tamper Flag tamper event detected...\r\n");
        DRY_TRACE("\nDRY_TSR is %08X\n\n",DRY_TSR);

        if((((DRY_SR & DRY_SR_TOF_MASK) == 0x04) && (DRY_TER & DRY_TER_TOE_MASK)==DRY_TER_TOE_MASK))
        {
            DRY_TRACE("DryIce Time Overflow in RTC detected...\r\n");
            DRY_TRACE("Disabling RTC time counter.\n");
            RTC_SR &= ~RTC_SR_TCE_MASK;
            DRY_TRACE("Writing to RTC_TSR to clear RTC_SR[TOF].\n");
            RTC_TSR |= 0xAAAA5555;
            DRY_TRACE("Writing to DRY_SR[TOF] to clear DRY_SR[TOF].\n");
            DRY_SR |= DRY_SR_TOF_MASK;
            DRY_TRACE("DRY_SR is %08X\n",DRY_SR);
        }	 	
        if((((DRY_SR & DRY_SR_MOF_MASK) == 0x08)&& (DRY_TER & DRY_TER_MOE_MASK)==DRY_TER_MOE_MASK))
        {
            DRY_TRACE("DryIce Monotonic Overflow in RTC detected...\r\n");
            DRY_TRACE("Disabling RTC monotonic counter.\n");           
            RTC_MER &= ~RTC_MER_MCE_MASK; // Disable monotonic counter
            DRY_TRACE("Writing to RTC_MCHR to clear RTC_SR[MOF].\n");           
            RTC_MCHR |= 0xFFFFFFFF; // Write to monotonic counter register high
            DRY_TRACE("Writing to DRY_SR[MOF] to clear DRY_SR[MOF].\n");
            DRY_SR |= DRY_SR_MOF_MASK;
            DRY_TRACE("DRY_SR is %08X\n",DRY_SR);
        }	
        if((((DRY_SR & DRY_SR_VTF_MASK) == 0x10)&& (DRY_TER & DRY_TER_VTE_MASK)==DRY_TER_VTE_MASK))
        {
            DRY_TRACE("DryIce Voltage Tamper detected...\r\n");
            DRY_TRACE("Please bring VBAT voltage to within the acceptable range and press any key to continue\n");
            // Wait to ensure VBAT is within proper range first                      
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
            DRY_TRACE("Clearing VTF by writing 1 to it.\n");
            DRY_SR |= DRY_SR_VTF_MASK; // clear VTF
            DRY_TRACE("DRY_SR is %08X\n",DRY_SR);           
        }	
        if((((DRY_SR & DRY_SR_CTF_MASK) == 0x20)&& (DRY_TER & DRY_TER_CTE_MASK)==DRY_TER_CTE_MASK))
        {
            DRY_TRACE("DryIce Clock Tamper detected...\r\n");
            DRY_TRACE("Please bring EXTAL32 clock frequency to within the acceptable range and press any key to continue\n");
            // Ensure clock is within proper range first
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
            DRY_TRACE("Clearing CTF by writing 1 to it.\n");
            DRY_SR |= DRY_SR_CTF_MASK; // clear CTF
            DRY_TRACE("DRY_SR is %08X\n",DRY_SR);                      
        }	
        if((((DRY_SR & DRY_SR_TTF_MASK) == 0x40)&& (DRY_TER & DRY_TER_TTE_MASK)==DRY_TER_TTE_MASK))
        {
            DRY_TRACE("DryIce Temperature Tamper detected...\r\n");
            DRY_TRACE("Please bring temperature to within the acceptable range and press any key to continue\n");           
            // Ensure temperature is within proper range first
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
            DRY_TRACE("Clearing TTF by writing 1 to it.\n");           
            DRY_SR |= DRY_SR_TTF_MASK; // clear TTF
            DRY_TRACE("DRY_SR is %08X\n",DRY_SR);                      
        }
        if((((DRY_SR & DRY_SR_FSF_MASK) == 0x100)&& (DRY_TER & DRY_TER_FSE_MASK)==DRY_TER_FSE_MASK))
        {
            DRY_TRACE("DryIce Flash Security Tamper detected...\r\n");
            DRY_TRACE("Flash security byte is: %08X\n",FTFE_FSEC);
            // Ensure flash security has been enabled first
            if ((FTFE_FSEC & 0x2) == 0x2)
            {
                DRY_TRACE("Flash is unsecured. Cannot clear FSF bit. Need to secure the flash first before \n");
                DRY_TRACE("Please issue a reset and unsecure the flash.\n");
            }
            else
            {
                DRY_TRACE("Flash is secured. Attempting to clear FSF bit.\n");
                DRY_SR |= DRY_SR_FSF_MASK; // clear FSF       
            }
        }	
        if((((DRY_SR & DRY_SR_TMF_MASK) == 0x200)&& (DRY_TER & DRY_TER_TME_MASK)==DRY_TER_TME_MASK))
        {
            DRY_TRACE("DryIce Test Mode detected...\r\n");
            DRY_TRACE("DryIce Test Mode must be exited before TMF can be cleared.\n");
            // Ensure test mode has been exited first
            DRY_TRACE("Exit test mode and enter any key to continue.\n");
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
            DRY_SR &= DRY_SR_TMF_MASK; // clear VTF
        }	
        if((((DRY_SR & DRY_SR_TPF(0x1)) == DRY_SR_TPF(0x1))&& (DRY_TER & DRY_TER_TPE(0x1))==DRY_TER_TPE(0x1)))
        {
            DRY_TRACE("DryIce Tamper Pin 0 tamper event detected...\r\n");
            // Ensure tamper pin equals its expected value
            DRY_TRACE("Change Tamper Pin 0 to its expected value and press any key to continue.\n");
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
            DRY_SR |= DRY_SR_TPF(0x1); // clear TPF for associated pin
        }	
        if((((DRY_SR & DRY_SR_TPF(0x2)) == DRY_SR_TPF(0x2))&& (DRY_TER & DRY_TER_TPE(0x2))==DRY_TER_TPE(0x2)))
        {
            DRY_TRACE("DryIce Tamper Pin 1 tamper event detected...\r\n");
            // Ensure tamper pin equals its expected value
            DRY_TRACE("Change Tamper Pin 1 to its expected value and press any key to continue.\n");
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
            DRY_SR |= DRY_SR_TPF(0x02); // clear TPF for associated pin

        }	
        if ((((DRY_SR & DRY_SR_TPF(0x4)) == DRY_SR_TPF(0x4))&& (DRY_TER & DRY_TER_TPE(0x4))==DRY_TER_TPE(0x4)))
        {
            DRY_TRACE("DryIce Tamper Pin 2 tamper event detected...\r\n");
            // Ensure tamper pin equals its expected value
            DRY_TRACE("Change Tamper Pin 2 to its expected value and press any key to continue.\n");
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
            DRY_SR |= DRY_SR_TPF(0x04); // clear TPF for associated pin
        }	     
        if ((DRY_SR & (DRY_SR_TAF_MASK | DRY_SR_DTF_MASK)) == (DRY_SR_TAF_MASK | DRY_SR_DTF_MASK))
        {
            DRY_TRACE("DTF and TAF bits both set.\n");
            DRY_TRACE("Clearing DTF first.\n");
            DRY_SR |= DRY_SR_DTF_MASK;
            DRY_TRACE("DRY_SR is %08X\n\n",DRY_SR); 
            DRY_TRACE("Clearing TAF next.\n");
            DRY_SR |= DRY_SR_TAF_MASK;
            DRY_TRACE("DRY_SR is %08X\n\n",DRY_SR); 
            DRY_TRACE("Press any key to continue\n");
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
        }
        else if((DRY_SR & DRY_SR_DTF_MASK) == DRY_SR_DTF_MASK)
        {
            DRY_TRACE("DTF bit is set but TAF bit is clear. To clear DTF, TAF must also be set. A chip reset must be performed to set TAF before DTF can be cleared.\n");
            DRY_TRACE("Please issue a chip reset to exit the DryIce ISR.\n");
#ifdef CFG_DBG_HW_DRYICE 
        in_char();           
#endif
        }
    }
    dbg_dryice_reg();
    DRY_TRACE("\n--|Exiting DryIce ISR\n");
    return;
}





int drv_dryice_get_reg(struct TamperEvent *event)
{
    if (event == NULL) {
        return -1;
    }
    drv_dryice_clock_init();
    event->time_s = DRY_TSR_REG(DRY);
    event->tsr =  DRY_SR_REG(DRY);
    event->sources = DRY_TER_REG(DRY);
    event->level =  DRY_PPR_REG(DRY);
    TRACE("-|second:%02X sr:%02X src:%02X ppr:%02X",
          event->time_s,event->tsr,
          event->sources,event->level);
    return 0;
}


