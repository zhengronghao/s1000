/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : dryice_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 5/23/2014 6:12:51 PM
 * Description        : 
 *******************************************************************************/

#include "dryice_functions.h"
#include "common.h"
#include "dryice_hw.h"

void dbg_dryice_reg(void)
{
#ifdef CFG_DBG_HW_DRYICE 
    SIM_SCGC5|=SIM_SCGC5_SECREG_MASK | SIM_SCGC5_DRYICE_MASK;
    DRY_TRACE("\nOnce cleared, these bits are only set by chip reset. They are not affected by VBAT POR or software reset.");
    DRY_TRACE("\nDRY_WAC : %06X ",DRY_WAC); DRY_TRACE("DRY_RAC : %06X ",DRY_RAC);DRY_TRACE("DRY_SWAC : %02X ",DRY_SWAC);   DRY_TRACE("DRY_SRAC : %02X",DRY_SRAC); 
    DRY_TRACE("\n\nDRY_LR:Once cleared, these bits can only be set by VBAT POR or software reset.");
    DRY_TRACE("\nDRY_CR   : %08X  ",DRY_CR); DRY_TRACE("DRY_SR   : %08X  ",DRY_SR);DRY_TRACE("DRY_LR   : %08X  ",DRY_LR); 
    DRY_TRACE("\nDRY_IER  : %08X  ",DRY_IER);DRY_TRACE("DRY_TER  : %08X  ",DRY_TER); DRY_TRACE("DRY_TSR  : %08X  ",DRY_TSR);
    DRY_TRACE("\nDRY_PDR  : %08X  ",DRY_PDR);  DRY_TRACE("DRY_PPR  : %08X  ",DRY_PPR);  DRY_TRACE("DRY_ATR0 : %08X  ",DRY_ATR0); DRY_TRACE("DRY_ATR1 : %08X",DRY_ATR1);    
    DRY_TRACE("\nDRY_PGFR0: %08X  ",DRY_PGFR0);DRY_TRACE("DRY_PGFR1: %08X  ",DRY_PGFR1);DRY_TRACE("DRY_PGFR2: %08X  ",DRY_PGFR2);
    TRACE("\n\nDRY_SKWLR,DRY_SKRLR:Once cleared, this bit can only be set by VBAT POR, software reset,\
          \nor by writing a logic one to the corresponding bit in the secure key valid register.");
    DRY_TRACE("\nDRY_SKVR : %02X ",DRY_SKVR);DRY_TRACE("DRY_SKWLR : %02X ",DRY_SKWLR);DRY_TRACE("DRY_SKRLR : %02X ",DRY_SKRLR);
    DRY_TRACE("\nDRY_SKR0 : %08X  ",DRY_SKR0); DRY_TRACE("DRY_SKR1 : %08X  ",DRY_SKR1); DRY_TRACE("DRY_SKR2 : %08X  ",DRY_SKR2); DRY_TRACE("DRY_SKR3 : %08X  ",DRY_SKR3);
    DRY_TRACE("\nDRY_SKR4 : %08X  ",DRY_SKR4); DRY_TRACE("DRY_SKR5 : %08X  ",DRY_SKR5); DRY_TRACE("DRY_SKR6 : %08X  ",DRY_SKR6); DRY_TRACE("DRY_SKR7 : %08X  ",DRY_SKR7);
#endif
}

int hw_dryice_read_key(uint8_t offset,uint8_t dw_number,uint32_t *output)
{
    ASSERT(offset < 8);
    ASSERT(offset+dw_number <= 8);
    if (DRY_RAC&DRY_RAC_SKRRR_MASK != DRY_RAC_SKRRR_MASK) {
        return KEYERR_ACCESS_BLOCK_READ;
    }
    if (DRY_SKRLR&0xFF != 0xFF) {
        return KEYERR_KEY_LOCK_READ;
    }
    if (DRY_SKVR&0xFF != 0xFF) {
        return KEYERR_VALID;
    }
    while (dw_number--)
    {
        *output = DRY_SKR_REG(DRY_BASE_PTR,offset);
        output++;
        offset++;
    }
    return 0;
}

int hw_dryice_write_key(uint8_t offset,uint8_t dw_number,uint32_t *input)
{
    ASSERT(offset < 8);
    ASSERT(offset+dw_number <= 8);
    if (DRY_RAC&DRY_RAC_SKWRR_MASK != DRY_RAC_SKWRR_MASK) {
        return KEYERR_ACCESS_BLOCK_WRITE;
    }
    if (DRY_SKWLR&0xFF != 0xFF) {
        return KEYERR_KEY_LOCK_WRITE;
    }
    while (dw_number--)
    {
       DRY_SKR_REG(DRY_BASE_PTR,offset) = *input;
        input++;
        offset++;
    }
    return 0;
}


static void hw_dryice_rtc_init(void)
{
    SIM_SCGC6|=SIM_SCGC6_RTC_MASK;
    if ((RTC_CR & RTC_CR_OSCE_MASK) != RTC_CR_OSCE_MASK )
    {
        hw_rtc_init(946684800U,0);
    }
}

static void hw_dryice_reset_init(void)
{
    /* Enable clocks to DryIce Secure Registers and to DryIce  Module */
    SIM_SCGC5|=SIM_SCGC5_SECREG_MASK | SIM_SCGC5_DRYICE_MASK;
    DRY_CR &= DRY_CR_SWR_MASK;
    //Clear DTF
    DRY_SR |= DRY_SR_DTF_MASK;
    //Clear TAF
    DRY_SR |= DRY_SR_TAF_MASK;
    if ((FTFE_FSEC & 0x3) == 0x2)
    {
        DRY_TRACE("\nFlash is unsecured.");
    } else
    {
        DRY_TRACE("\nFlash is secured.");
    }
}

void hw_dryice_init(void)
{
    hw_dryice_rtc_init();
    hw_dryice_reset_init();
}


int hw_dryice_tamper_pin_polarity_config(uint8_t pin_number, uint8_t polarity)
{
    if (pin_number >= TAMPER_MAX_NUM)
    {
        DRY_TRACE("Invalid tamper pin source:\n");    
        return -1;
    }
    if ((DRY_RAC & DRY_RAC_PPR_MASK)>>DRY_RAC_PPR_SHIFT == 0)
    { //Cannot read the pin polarity setting. The DRY_PPR register's read access is blocked. Remove the read access block and try again.
        return KEYERR_ACCESS_BLOCK_READ;
    }         
    if ((DRY_WAC & DRY_WAC_PPW_MASK)>>DRY_WAC_PPW_SHIFT == 0)
    {//Cannot write the pin polarity setting. The DRY_PPR register's write access is blocked. Remove the write access block and try again.
        return KEYERR_ACCESS_BLOCK_WRITE;
    }
    if ((DRY_LR & DRY_LR_PPL_MASK)>>DRY_LR_PPL_SHIFT == 0)
    {//Cannot write the pin polarity setting. The DRY_PPR register is locked or its write access is blocked. Remove the lock or its write access block and try again.
        return KEYERR_REG_LOCK;    
    }   
    if (polarity == TAMPER_PIN_NOT_INVERTED) {
        DRY_PPR &= ~DRY_PPR_TPP(1<<pin_number);
        DRY_TRACE("Tamper Pin %d expected value is not inverted.\n",pin_number);
    } else
    {
        DRY_PPR |= DRY_PPR_TPP(1<<pin_number);
        DRY_TRACE("Tamper Pin %d expected value is inverted.\n",pin_number);            
    }
    return 0;
}

int hw_dryice_tamper_pin_direction_config(uint8_t pin_number, uint8_t direction)
{
    if (pin_number >= TAMPER_MAX_NUM) {
        DRY_TRACE("Invalid tamper pin source:\n");    
        return -1;
    }
    if ((DRY_RAC & DRY_RAC_PDR_MASK)>>DRY_RAC_PDR_SHIFT == 0)
    { //Cannot read the pin direction. The DRY_PDR register's read access is blocked. Remove the read access block and try again.
        return KEYERR_ACCESS_BLOCK_READ;
    }          
    if ((DRY_LR & DRY_LR_PDL_MASK)>>DRY_LR_PDL_SHIFT == 0)
    {//Cannot write the pin direction. The DRY_PDR register is locked. Remove the lock and try again.\n");
        return KEYERR_REG_LOCK;    
    }  
    if (((DRY_LR & DRY_LR_PDL_MASK)>>DRY_LR_PDL_SHIFT == 0) || ((DRY_WAC & DRY_WAC_PDW_MASK)>>DRY_WAC_PDW_SHIFT == 0))
    {//Cannot write the pin direction. The DRY_PDR register,s write access is blocked. Remove the write access blocked and try again.\n");
        return KEYERR_ACCESS_BLOCK_WRITE;
    }
    if (direction == TAMPER_PIN_SET_INPUT) {
        DRY_PDR &= ~DRY_PDR_TPD(1<<pin_number);
        DRY_TRACE("Tamper Pin %d pin direction set to input.\n",pin_number);
    } else
    {
        DRY_PDR |= DRY_PDR_TPD(1<<pin_number);
        DRY_TRACE("Tamper Pin %d pin direction set to output and drives inverse of expected value.\n",pin_number);
    }
    return 0;
}

int hw_dryice_pgfr_cfg(uint8_t tamper_pin,uint32_t pgfr)
{
    if (tamper_pin >= TAMPER_MAX_NUM) {
        return -1;
    }
    if (((DRY_RAC & DRY_RAC_GFR(0x1<<tamper_pin))>>(DRY_RAC_GFR_SHIFT+tamper_pin) == 0x0))
    {//Cannot read DRY_PGFR0 because its read access is blocked. Remove the read access block and try again.
        DRY_TRACE("\nread blcok");
        return KEYERR_ACCESS_BLOCK_READ;
    }    
    if (((DRY_WAC & DRY_WAC_GFW(0x1<<tamper_pin))>>(DRY_WAC_GFW_SHIFT+tamper_pin) == 0x0))
    {//Cannot write DRY_PGFR0 because its write access is blocked. Remove the write access block and try again
        DRY_TRACE("\nwrite block");
        return KEYERR_ACCESS_BLOCK_WRITE;
    }
    if (((DRY_LR & DRY_LR_GFL(0x1<<tamper_pin))>>(DRY_LR_GFL_SHIFT+tamper_pin) == 0x0))
    {//Cannot write DRY_PGFR0 because it is locked. Remove the lock and try again.
        DRY_TRACE("\nLock");
        return KEYERR_KEY_LOCK_WRITE;
    }
//    DRY_TRACE("\nTamper Pin %d glitch filter disabled.",tamper_pin);
//    DRY_PGFR_REG(DRY_BASE_PTR,tamper_pin) &= ~FILTER_GLITCH_ENABLE; 
    DRY_PGFR_REG(DRY_BASE_PTR,tamper_pin) = pgfr; 
//    DRY_TRACE("\nTamper Pin %d glitch filter enabled.",tamper_pin);
//    DRY_PGFR_REG(DRY_BASE_PTR,tamper_pin) |= FILTER_GLITCH_ENABLE; 
    return 0;
}


int hw_dryice_tamper_cfg(uint8_t pin,uint8_t trig_level,uint32_t glitch_filter)
{
    if (pin >= TAMPER_MAX_NUM) {
        return -1;
    }
//    dryice_tamper_pin_polarity_config(pin,(trig_level==0)?INVERTED:NOT_INVERTED);//PPR:TPP
    hw_dryice_tamper_pin_polarity_config(pin,(trig_level==0)?TAMPER_PIN_INVERTED:TAMPER_PIN_NOT_INVERTED);//PPR:TPP
    hw_dryice_tamper_pin_direction_config(pin,TAMPER_PIN_SET_INPUT);
    hw_dryice_pgfr_cfg(pin,glitch_filter);
    dryice_pin_tamper_config(pin,ENABLE);
//    dryice_int_tamper_interrupt_config(DRYICE_TAMPER,ENABLE);
    return 0;
}

uint8_t hw_dryice_read_pin(void)
{
    SIM_SCGC5|=SIM_SCGC5_SECREG_MASK | SIM_SCGC5_DRYICE_MASK;
    return (uint8_t)((DRY_PPR & DRY_PPR_TPID_MASK)>>DRY_PPR_TPID_SHIFT);
}

uint8_t hw_dryice_read_trigger(void)
{
    SIM_SCGC5|=SIM_SCGC5_SECREG_MASK | SIM_SCGC5_DRYICE_MASK;
    return (uint8_t)(((DRY_SR & DRY_SR_TPF_MASK)>>DRY_SR_TPF_SHIFT));
}


