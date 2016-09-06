/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Freescale Confidential and Proprietary - use of this software is
 * governed by the LA_OPT_FSL_OPEN_3RD_PARTY_IP License distributed
 * with this Material.
 */

#include "fsl_dryice_hal.h"

#if FSL_FEATURE_SOC_DRY_COUNT

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_Init
 * Description: Write reset values to the following DryIce registers, if writing is possible:
 *              DRY_CR, DRY_PGFRn, DRY_IER, DRY_TER, DRY_PDR, DRY_PPR, DRY_ATR.
 *              The function does not touch:
 *              DRY_SKVR, DRY_SKWLR, DRY_SKRLR, DRY_SR, DRY_LR, DRY_TSR, DRY_WAC, DRY_RAC, DRY_SKRn, DRY_SWAC and DRY_SRAC.
 *END*********************************************************************/
void DRY_HAL_Init(DRY_Type * base)
{
    int i;
    uint32_t reg_select;
    
    if (DRY_HAL_IsRegWritePossible(base, kDryReg_Control))
    {
        DRY_CR_REG(base) = 0x0u;
    }
    
    i=0;
    for (reg_select = kDryReg_GlitchFilter_0; reg_select <= kDryReg_GlitchFilter_7; reg_select = reg_select << 1)
    {
        if (DRY_HAL_IsRegWritePossible(base, (hal_dry_reg_t)reg_select))
        {
            DRY_PGFR_REG(base, i) = 0x0u;
        }
        i++;
    }
    
    if (DRY_HAL_IsRegWritePossible(base, kDryReg_InterruptEnbl))
    {
        DRY_IER_REG(base) = 0x1u;
    }
    
    if (DRY_HAL_IsRegWritePossible(base, kDryReg_TamperEnbl))
    {
        DRY_TER_REG(base) = 0x1u;
    }
    
    if (DRY_HAL_IsRegWritePossible(base, kDryReg_PinDirection))
    {
        DRY_PDR_REG(base) = 0x0u;
    }    
    
    if (DRY_HAL_IsRegWritePossible(base, kDryReg_PinPolarity))
    {
        DRY_PPR_REG(base) = 0x0u;
    }
    
    i=0;
    for (reg_select = kDryReg_ActiveTamper_0; reg_select <= kDryReg_ActiveTamper_1; reg_select = reg_select << 1)
    {
        if (DRY_HAL_IsRegWritePossible(base, (hal_dry_reg_t)reg_select))
        {
            DRY_ATR_REG(base, i) = 0x0u;
        }
        i++;
    }
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_Disable
 * Description: Disable the DryIce module. If force argument is TRUE,
 *              the function will also call other DryIce HAL functions to make sure
 *              all Glitch Filters and Active Tampers are disabled in advance.
 *END*********************************************************************/
void DRY_HAL_Disable(DRY_Type * base, bool force)
{
    if (force)
    {
        /* Disable all Glitch Filters */
        DRY_HAL_GlitchFilterDisable(base, kDryTamperPin_All);
        
        /* Disable all Active Tampers */
        DRY_HAL_DisableActiveTamper(base, kDryActiveTamperReg_All);
    }
    
    DRY_CR_REG(base) &= ~(DRY_CR_DEN(1U));
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetActiveTamperClock
 * Description: In DryIce Control Register, configure Active Tamper Clock Source
 *              and optionally Prescaler.
 * Note: Do not change the active tamper clock source when the corresponding active
 *       tamper register is enabled.
 * Note: Prescaler Register will only be written if init_count argument is greater than
 *       or equal zero AND DryIce module is disabled (Control Register DEN bit is zero).
 *END*********************************************************************/
void DRY_HAL_SetActiveTamperClock(DRY_Type * base, 
                                  hal_dry_act_tamper_selector_t reg_name, 
                                  hal_dry_act_tamper_clock_t clock, 
                                  int32_t init_count)
{
    uint32_t atcs_field;
        
    atcs_field = (uint32_t)reg_name & ((uint32_t)clock | ((uint32_t)clock << 1));
    
    DRY_CR_REG(base) &= ~DRY_CR_ATCS(reg_name);
    DRY_CR_REG(base) |= DRY_CR_ATCS(atcs_field);
    
    /* Write also Prescaler field in Control Register, unless the argument is negative. 
     * The Prescler can only be written when DryIce module is disabled (DEN bit is zero).
     */
    if ((init_count >= 0) && (FALSE   == DRY_HAL_IsEnabled(base)))
    {
        DRY_HAL_SetPrescaler(base, init_count);
    }
}

/*FUNCTION****************************************************************
 *
 * Function Name: dry_hal_set_glitch_filter_clock
 * Description: Static function to configure one Glitch Filter Register.
 *              
 *END*********************************************************************/
static void dry_hal_set_glitch_filter_clock(DRY_Type * base, 
                                            int32_t index, 
                                            hal_dry_glitch_filter_prescaler_t prescaler, 
                                            uint32_t width)
{
    uint32_t reg_field;
    
    DRY_PGFR_REG(base, index) &= ~(DRY_PGFR_GFP_MASK | DRY_PGFR_GFW_MASK);
    reg_field = DRY_PGFR_GFW(width) | DRY_PGFR_GFP(prescaler);
    DRY_PGFR_REG(base, index) |= reg_field;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetGlitchFilterClock
 * Description: In selected DryIce Glitch Filter Register(s), configure 
 *              Glitch Filter Prescaler and Glitch Filter Width.
 *END*********************************************************************/
void DRY_HAL_SetGlitchFilterClock(DRY_Type * base, 
                                  hal_dry_tamper_pin_selector_t pin, 
                                  hal_dry_glitch_filter_prescaler_t prescaler, 
                                  uint32_t width)
{
    /* for each selected pin (multiple can be selected) 
     * configure Glitch Filter Prescaler and Glitch Filter Width
     */
    uint32_t pins;
    int32_t index;
    uint32_t onepin;

    pins = pin & kDryTamperPin_All;
    index = 0;
    for ( onepin = kDryTamperPin_0; onepin <= kDryTamperPin_7; onepin = (onepin << 1))
    {
        if (pins & onepin)
        {
            dry_hal_set_glitch_filter_clock(base, index, prescaler, width);
            pins &= ~onepin;
            if (0u == pins)
            {
                break;
            }
        }
        index++;
    }
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GlitchFilterEnable
 * Description: Enable glitch filter for a tamper pin or pins.
 *              
 *END*********************************************************************/
void DRY_HAL_GlitchFilterEnable(DRY_Type * base, hal_dry_tamper_pin_selector_t pin)
{
    uint32_t pins;
    int32_t index;
    uint32_t onepin;
    
    pins = pin & kDryTamperPin_All;
    index = 0;
    for ( onepin = kDryTamperPin_0; onepin <= kDryTamperPin_7; onepin = (onepin << 1))
    {
        if (pins & onepin)
        {
            DRY_PGFR_REG(base, index) |= DRY_PGFR_GFE(1u);
        }
        index++;
    }
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GlitchFilterDisable
 * Description: Disable glitch filter for a tamper pin or pins.
 *              
 *END*********************************************************************/
void DRY_HAL_GlitchFilterDisable(DRY_Type * base, hal_dry_tamper_pin_selector_t pin)
{
    uint32_t pins;
    int32_t index;
    uint32_t onepin;

    pins = pin & kDryTamperPin_All;
    index = 0;
    for ( onepin = kDryTamperPin_0; onepin <= kDryTamperPin_7; onepin = (onepin << 1))
    {
        if (pins & onepin)
        {
            DRY_PGFR_REG(base, index) &= ~(DRY_PGFR_GFE(1u));
        }
        index++;
    }
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetGlitchFilterSampling
 * Description: Configure parameters of Glitch Filter sampling.
 *              
 *END*********************************************************************/
void DRY_HAL_SetGlitchFilterSampling(DRY_Type * base, hal_dry_tamper_pin_selector_t pin, 
                                     hal_dry_glitch_filter_sample_width_t width, 
                                     hal_dry_glitch_filter_sample_freq_t freq)
{
    uint32_t pins;
    int32_t index;
    uint32_t onepin;
    
    uint32_t reg_field;
    reg_field = DRY_PGFR_TPSW(width) | DRY_PGFR_TPSF(freq);

    pins = pin & kDryTamperPin_All;
    index = 0;
    for ( onepin = kDryTamperPin_0; onepin <= kDryTamperPin_7; onepin = (onepin << 1))
    {
        if (pins & onepin)
        {
            DRY_PGFR_REG(base, index) &= ~(DRY_PGFR_TPSF_MASK | DRY_PGFR_TPSW_MASK);
            DRY_PGFR_REG(base, index) |= reg_field;
        }
        index++;
    }
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetGlitchFilterExpected
 * Description: Configure Glitch Filter expected values.
 *              
 *END*********************************************************************/
void DRY_HAL_SetGlitchFilterExpected(DRY_Type * base, 
                                     hal_dry_tamper_pin_selector_t pin, 
                                     hal_dry_glitch_filter_expected_t value)
{
    uint32_t pins;
    int32_t index;
    uint32_t onepin;
    
    uint32_t reg_field;
    reg_field = DRY_PGFR_TPEX(value);

    pins = pin & kDryTamperPin_All;
    index = 0;
    for ( onepin = kDryTamperPin_0; onepin <= kDryTamperPin_7; onepin = (onepin << 1))
    {
        if (pins & onepin)
        {
            DRY_PGFR_REG(base, index) &= ~DRY_PGFR_TPEX_MASK;
            DRY_PGFR_REG(base, index) |= reg_field;
        }
        index++;
    }
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetGlitchFilterPull
 * Description: Configure Glitch Filter pull enable and direction.
 *              
 *END*********************************************************************/
void DRY_HAL_SetGlitchFilterPull(DRY_Type * base, 
                                 hal_dry_tamper_pin_selector_t pin, 
                                 hal_dry_enable_t enable, 
                                 hal_dry_glitch_filter_pull_t type)
{
    uint32_t pins;
    int32_t index;
    uint32_t onepin;
    
    uint32_t reg_field;
    reg_field = DRY_PGFR_TPE(enable) | DRY_PGFR_TPS(type);

    pins = pin & kDryTamperPin_All;
    index = 0;
    for ( onepin = kDryTamperPin_0; onepin <= kDryTamperPin_7; onepin = (onepin << 1))
    {
        if (pins & onepin)
        {
            DRY_PGFR_REG(base, index) &= ~(DRY_PGFR_TPE_MASK | DRY_PGFR_TPS_MASK);
            DRY_PGFR_REG(base, index) |= reg_field;
        }
        index++;
    }
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetStatusFlagMasked
 * Description: Read and mask Status Register DRY_SR.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetStatusFlagMasked(DRY_Type * base, const hal_dry_status_flag_t * specifier, hal_dry_status_flag_t * dest)
{
    uint32_t getmask;
    
    getmask = ((uint32_t)*specifier) & kDryStatus_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_status_flag_t)(getmask & DRY_SR_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_ClearStatusFlag
 * Description: Clear specified flags in DRY_SR.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_ClearStatusFlag(DRY_Type * base, hal_dry_status_flag_t flag)
{
    uint32_t setmask;

    setmask = (uint32_t)flag & kDryStatus_All;
    if (0 == setmask)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    DRY_SR_REG(base) = setmask;
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetSecureKeyValidMasked
 * Description: Read and mask Secure Key Valid register DRY_SKVR.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetSecureKeyValidMasked(DRY_Type * base, 
                                        const hal_dry_secure_key_reg_selector_t * specifier, 
                                        hal_dry_secure_key_reg_selector_t * dest)
{
    uint32_t getmask;
    
    getmask = *specifier & kDrySecureKeyReg_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_secure_key_reg_selector_t)(getmask & DRY_SKVR_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_ClrSecureKeyValid
 * Description: Clear a secure key valid bit or bits in DRY_SKVR register.
 *              This also invalidates correcponding secure key register.
 *END*********************************************************************/
int32_t DRY_HAL_ClrSecureKeyValid(DRY_Type * base, hal_dry_secure_key_reg_selector_t key_register)
{
    uint32_t setmask;

    setmask = key_register & kDrySecureKeyReg_All;
    if (0 == setmask)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    DRY_SKVR_REG(base) = setmask;
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetSecureKeyWriteLockMasked
 * Description: Read and mask Secure Key Write Lock register DRY_SKWLR.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetSecureKeyWriteLockMasked(DRY_Type * base, 
                                            const hal_dry_secure_key_reg_selector_t * specifier, 
                                            hal_dry_secure_key_reg_selector_t * dest)
{
    uint32_t getmask;
    
    getmask = *specifier & kDrySecureKeyReg_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_secure_key_reg_selector_t)(getmask & DRY_SKWLR_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_LockSecureKeyWrite
 * Description: Lock specified secure key register in DRY_SKWLR.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_LockSecureKeyWrite(DRY_Type * base, hal_dry_secure_key_reg_selector_t key_register)
{
    uint32_t setmask;

    setmask = key_register & kDrySecureKeyReg_All;
    if (0 == setmask)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    DRY_SKWLR_REG(base) &= ~setmask;
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetSecureKeyReadLockMasked
 * Description: Read and mask Secure Key Read Lock Register DRY_SKRLR.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetSecureKeyReadLockMasked(DRY_Type * base, const hal_dry_secure_key_reg_selector_t * specifier, hal_dry_secure_key_reg_selector_t * dest)
{
    uint32_t getmask;
    
    getmask = *specifier & kDrySecureKeyReg_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_secure_key_reg_selector_t)(getmask & DRY_SKRLR_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_LockSecureKeyRead
 * Description: Lock specified secure key register or registers for read in DRY_SKRLR.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_LockSecureKeyRead(DRY_Type * base, hal_dry_secure_key_reg_selector_t key_register)
{
    uint32_t setmask;

    setmask = key_register & kDrySecureKeyReg_All;
    if (0 == setmask)
    {
        return kStatus_DRY_Failure;
    }
    DRY_SKRLR_REG(base) &= ~setmask;
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetRegisterLockStatusMasked
 * Description: Read and mask Lock Register DRY_LR.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetRegisterLockStatusMasked(DRY_Type * base, const hal_dry_reg_t * specifier, hal_dry_reg_t * dest)
{
    uint32_t getmask;
    
    getmask = (uint32_t)*specifier & kDryReg_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_reg_t)(getmask & DRY_LR_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_LockRegister
 * Description: Lock register for writing in DRY_LR.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_LockRegister(DRY_Type * base, hal_dry_reg_t lock)
{
    uint32_t setmask;

    setmask = (uint32_t)lock & kDryReg_All;
    if (0 == setmask)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    DRY_LR_REG(base) &= ~setmask;
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetInterruptEnableMasked
 * Description: Read and mask Interrupt Enable Register DRY_IER.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetInterruptEnableMasked(DRY_Type * base, const hal_dry_interrupt_t * specifier, hal_dry_interrupt_t * dest)
{
    uint32_t getmask;
    
    getmask = (uint32_t)*specifier & kDryInterrupt_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_interrupt_t)(getmask & DRY_IER_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_EnableInterrupt
 * Description: Enable interrupt source(s) in DRY_IER.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_EnableInterrupt(DRY_Type * base, hal_dry_interrupt_t source)
{
    uint32_t setmask;

    setmask = (uint32_t)source & kDryInterrupt_All;
    if (0 == setmask)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    DRY_IER_REG(base) |= setmask;
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_DisableInterrupt
 * Description: Disable interrupt source(s) in DRY_IER.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_DisableInterrupt(DRY_Type * base, hal_dry_interrupt_t source)
{
    uint32_t setmask;

    setmask = (uint32_t)source & kDryInterrupt_All;
    if (0 == setmask)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    DRY_IER_REG(base) &= ~setmask;
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetTamperEnableMasked
 * Description: Read and mask Tamper Enable Register DRY_TER.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetTamperEnableMasked(DRY_Type * base, const hal_dry_tamper_t * specifier, hal_dry_tamper_t * dest)
{
    uint32_t getmask;
    
    getmask = (uint32_t)*specifier & kDryTamper_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_tamper_t)(getmask & DRY_TER_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_EnableTamper
 * Description: Enable tamper source(s) in DRY_TER.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_EnableTamper(DRY_Type * base, hal_dry_tamper_t tamper)
{
    uint32_t setmask;

    setmask = (uint32_t)tamper & kDryTamper_All;
    if (0 == setmask)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    DRY_TER_REG(base) |= setmask;
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_DisableTamper
 * Description: Disable tamper source(s) in DRY_TER.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_DisableTamper(DRY_Type * base, hal_dry_tamper_t tamper)
{
    uint32_t setmask;

    setmask = (uint32_t)tamper & kDryTamper_All;
    if (0 == setmask)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    DRY_TER_REG(base) &= ~setmask;
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetTamperPinOutput
 * Description: Read and mask Tamper Pin Output Register DRY_PDR[TPOD].
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetTamperPinOutput(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_tamper_pin_selector_t *dest)
{
    uint32_t pins;
    
    pins = *pin & kDryTamperPin_All;
    
    if (0 == pins)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_tamper_pin_selector_t)(((pins << DRY_PDR_TPOD_SHIFT) & DRY_PDR_REG(base))>>DRY_PDR_TPOD_SHIFT);
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetTamperPinDirection
 * Description: Configure Tamper Pin Direction Register DRY_PDR[TPD].
 *              
 *END*********************************************************************/
void DRY_HAL_SetTamperPinDirection(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_pin_direction_t direction)
{
    uint32_t pins;
    uint32_t onepin;
    uint32_t reg_field;
    
    reg_field = 0u;
    pins = *pin & kDryTamperPin_All;    
    for ( onepin = kDryTamperPin_0; onepin <= kDryTamperPin_7; onepin = (onepin << 1))
    {
        if ((pins & onepin) && (direction&onepin))
        {
            reg_field |= onepin;// kDryTamperPinDirection_Out
        }        
    }
    TRACE("\n-|Direction pins:%02X reg:%02X",pins,reg_field);
    DRY_PDR_REG(base) &= ~pins;
    DRY_PDR_REG(base) |= reg_field;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetTamperPinDirection
 * Description: Read and mask Tamper Pin Direction Register DRY_PDR[TPD].
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetTamperPinDirection(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_tamper_pin_selector_t *dest)
{
    uint32_t pins;
    
    pins = *pin & kDryTamperPin_All;
    if (0 == pins)
    {
        return kStatus_DRY_Failure;
    }
    *dest = (hal_dry_tamper_pin_selector_t)(pins & DRY_PDR_REG(base));
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetTamperPinInput
 * Description: Read and mask Tamper Pin Input Register DRY_PPR[TPID].
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetTamperPinInput(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_tamper_pin_selector_t *dest)
{
    uint32_t pins;
    
    pins = *pin & kDryTamperPin_All;
    
    if (0 == pins)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_tamper_pin_selector_t)(((pins << DRY_PPR_TPID_SHIFT) & DRY_PPR_REG(base))>>DRY_PPR_TPID_SHIFT);
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetTamperPinPolarity
 * Description: Configure Tamper Pin Polarity Register DRY_PPR[TPP].
 *              
 *END*********************************************************************/
void DRY_HAL_SetTamperPinPolarity(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_pin_polarity_t polarity)
{
    uint32_t pins;
    uint32_t onepin;
    uint32_t reg_field;
    
    reg_field = 0u;
    pins = *pin & kDryTamperPin_All;    
    for ( onepin = kDryTamperPin_0; onepin <= kDryTamperPin_7; onepin = (onepin << 1))
    {
        if ((pins & onepin) && (polarity & onepin))//== kDryTamperPinPolarity_ExpectValInverted))
        {
            reg_field |= onepin;
        }        
    }
    TRACE("\n-|Polarity pins:%02X reg:%02X",pins,reg_field);
    DRY_PPR_REG(base) &= ~pins;
    DRY_PPR_REG(base) |= reg_field;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetTamperPinPolarity
 * Description: Read and mask Tamper Tamper Pin Polarity Register DRY_PPR[TPP].
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetTamperPinPolarity(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_tamper_pin_selector_t *dest)
{
    uint32_t pins;
    
    pins = *pin & kDryTamperPin_All;
    if (0 == pins)
    {
        return kStatus_DRY_Failure;
    }
    *dest = (hal_dry_tamper_pin_selector_t)(pins & DRY_PPR_REG(base));
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetActiveTamperPolynomial
 * Description: Read Active Tamper Polynomial DRY_ATRn[ATP].
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetActiveTamperPolynomial(DRY_Type * base, const hal_dry_act_tamper_selector_t *reg_name, uint16_t *dest, int32_t num)
{
    uint32_t selector;
    int32_t i;
    uint32_t one_reg;
    
    selector = *reg_name & kDryActiveTamperReg_All;
    if(0u == selector)
    {
        return kStatus_DRY_Failure;
    }
    
    one_reg = 1u;
    for ( i = 0; i < DRY_ATR_COUNT; i++)
    {
        if (selector & one_reg)
        {
            *dest = (uint16_t)((DRY_ATR_ATP_MASK & DRY_ATR_REG(base, i)) >> DRY_ATR_ATP_SHIFT);
            dest++;
            num--;
        }
        if (num <= 0)
        {
            break;
        }
        one_reg <<= 1;
    }
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetActiveTamperShift
 * Description: Read Active Tamper Shift Register DRY_ATRn[ATSR].
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetActiveTamperShift(DRY_Type * base, const hal_dry_act_tamper_selector_t *reg_name, uint16_t *dest, int32_t num)
{
    uint32_t selector;
    int32_t i;
    uint32_t one_reg;
    
    selector = *reg_name & kDryActiveTamperReg_All;
    if (0u == selector)
    {
        return kStatus_DRY_Failure;
    }
    
    one_reg = 1u;
    for ( i = 0; i < DRY_ATR_COUNT; i++)
    {
        if (selector & one_reg)
        {
            *dest = (uint16_t)((DRY_ATR_ATSR_MASK & DRY_ATR_REG(base, i)));
            dest++;
            num--;
        }
        if (num <= 0)
        {
            break;
        }
        one_reg <<= 1;
    }
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetActiveTamperPolynomial
 * Description: Configure Active Tamper Polynomial DRY_ATRn[ATP].
 *              
 *END*********************************************************************/
int32_t DRY_HAL_SetActiveTamperPolynomial(DRY_Type * base, const hal_dry_act_tamper_selector_t reg_name, uint16_t polynomial)
{
    uint32_t selector;
    int32_t i;
    uint32_t one_reg;
    
    selector = reg_name & kDryActiveTamperReg_All;
    if(0u == selector)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    
    one_reg = 1u;
    for ( i = 0; i < DRY_ATR_COUNT; i++)
    {
        if (selector & one_reg)
        {
            DRY_ATR_REG(base, i) &= ~DRY_ATR_ATP_MASK;
            DRY_ATR_REG(base, i) |= ((uint32_t)polynomial << DRY_ATR_ATP_SHIFT);
        }
        one_reg <<= 1;
    }
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetActiveTamperShift
 * Description: Configure Active Tamper Shift Register DRY_ATRn[ATSR].
 *              
 *END*********************************************************************/
int32_t DRY_HAL_SetActiveTamperShift(DRY_Type * base, const hal_dry_act_tamper_selector_t reg_name, uint16_t shift)
{
    uint32_t selector;
    int32_t i;
    uint32_t one_reg;
    
    selector = reg_name & kDryActiveTamperReg_All;
    if(0u == selector)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    
    one_reg = 1u;
    for ( i = 0; i < DRY_ATR_COUNT; i++)
    {
        if (selector & one_reg)
        {
            DRY_ATR_REG(base, i) &= ~DRY_ATR_ATSR_MASK;
            DRY_ATR_REG(base, i) |= shift;
        }
        one_reg <<= 1;
    }
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetRegisterWriteAccessMasked
 * Description: Read and mask Write Access Control Register DRY_WAC.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetRegisterWriteAccessMasked(DRY_Type * base, const hal_dry_reg_t * specifier, hal_dry_reg_t * dest)
{
    uint32_t getmask;
    
    getmask = (uint32_t)*specifier & kDryReg_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_reg_t)(getmask & DRY_WAC_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetIgnoreRegisterWrite
 * Description: Configure Write Access Control Register DRY_WAC.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_SetIgnoreRegisterWrite(DRY_Type * base, hal_dry_reg_t reg_name)
{
    uint32_t setmask;
    
    setmask = (uint32_t)reg_name & kDryReg_All;
    if (0 == setmask)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    
    DRY_WAC_REG(base) &= ~setmask;
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetRegisterReadAccessMasked
 * Description: Read and mask Read Access Control Register DRY_RAC.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetRegisterReadAccessMasked(DRY_Type * base, const hal_dry_reg_t * specifier, hal_dry_reg_t * dest)
{
    uint32_t getmask;
    
    getmask = (uint32_t)*specifier & kDryReg_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_reg_t)(getmask & DRY_RAC_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetIgnoreRegisterRead
 * Description: Configure Read Access Control Register DRY_RAC.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_SetIgnoreRegisterRead(DRY_Type * base, hal_dry_reg_t reg_name)
{
    uint32_t setmask;
    
    setmask = (uint32_t)reg_name & kDryReg_All;
    if (0 == setmask)
    {
        TRACE("\n-|Err:%d",__LINE__);
        return kStatus_DRY_Failure;
    }
    
    DRY_RAC_REG(base) &= ~setmask;
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetSecureWriteAccessMasked
 * Description: Read and mask Secure Key Write Access Control Register DRY_SWAC.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetSecureWriteAccessMasked(DRY_Type * base, const hal_dry_secure_key_reg_selector_t * specifier, hal_dry_secure_key_reg_selector_t * dest)
{
    uint32_t getmask;
    
    getmask = *specifier & kDrySecureKeyReg_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_secure_key_reg_selector_t)(getmask & DRY_SWAC_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetIgnoreSecureWrite
 * Description: Configure Secure Key Write Access Control Register DRY_SWAC.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_SetIgnoreSecureWrite(DRY_Type * base, hal_dry_secure_key_reg_selector_t reg_name)
{
    uint32_t setmask;
    
    setmask = reg_name & kDrySecureKeyReg_All;
    if (0 == setmask)
    {
        return kStatus_DRY_Failure;
    }
    
    DRY_SWAC_REG(base) &= ~setmask;
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetSecureReadAccessMasked
 * Description: Read and mask Secure Key Read Access Control Register DRY_SRAC.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetSecureReadAccessMasked(DRY_Type * base, const hal_dry_secure_key_reg_selector_t * specifier, hal_dry_secure_key_reg_selector_t * dest)
{
    uint32_t getmask;
    
    getmask = *specifier & kDrySecureKeyReg_All;
    if (0 == getmask)
    {
        return kStatus_DRY_Failure;
    }
    
    *dest = (hal_dry_secure_key_reg_selector_t)(getmask & DRY_SRAC_REG(base));
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetIgnoreSecureRead
 * Description: Configure Secure Key Read Access Control Register DRY_SRAC.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_SetIgnoreSecureRead(DRY_Type * base, hal_dry_secure_key_reg_selector_t reg_name)
{
    uint32_t setmask;
    
    setmask = reg_name & kDrySecureKeyReg_All;
    if (0 == setmask)
    {
        return kStatus_DRY_Failure;
    }
    
    DRY_SRAC_REG(base) &= ~setmask;
    
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_SetSecureKey
 * Description: Write Secure Key Register(s) DRY_SKRn.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_SetSecureKey(DRY_Type * base, hal_dry_secure_key_reg_selector_t reg_name, int32_t *key, int32_t num)
{
    uint32_t key_regs;
    int32_t index;
    uint32_t one_key_reg;

    key_regs = reg_name & kDrySecureKeyReg_All;
    if (0u == key_regs)
    {
        /* no key register selected */
        return kStatus_DRY_Failure;
    }
    index = 0;
    for ( one_key_reg = kDrySecureKeyReg_0; one_key_reg <= kDrySecureKeyReg_7; one_key_reg = (one_key_reg << 1))
    {
        if (key_regs & one_key_reg)
        {
            DRY_SKR_REG(base, index) = key[index];
        }
        index++;
        num--;
        if (num <= 0)
        {
            break;
        }
    }
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_GetSecureKey
 * Description: Read Secure Key Register(s) DRY_SKRn.
 *              
 *END*********************************************************************/
int32_t DRY_HAL_GetSecureKey(DRY_Type * base, hal_dry_secure_key_reg_selector_t reg_name, int32_t *key, int32_t num)
{
    uint32_t key_regs;
    int32_t index;
    uint32_t one_key_reg;

    key_regs = reg_name & kDrySecureKeyReg_All;
    if (0u == key_regs)
    {
        /* no key register selected */
        return kStatus_DRY_Failure;
    }
    index = 0;
    for ( one_key_reg = kDrySecureKeyReg_0; one_key_reg <= kDrySecureKeyReg_7; one_key_reg = (one_key_reg << 1))
    {
        if (key_regs & one_key_reg)
        {
            key[index] = DRY_SKR_REG(base, index);
        }
        index++;
        if (num <= 0)
        {
            break;
        }
    }
    return kStatus_DRY_Success;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_IsRegWritePossible
 * Description: Test if Lock Register DRY_LR and Write Access Control Register DRY_WAC allow writing.
 *              
 *END*********************************************************************/
bool DRY_HAL_IsRegWritePossible(DRY_Type *base, const hal_dry_reg_t reg_name)
{
    hal_dry_reg_t masked_out;
    
    /* specified DRY_LR bit(s) must be set */
    if(kStatus_DRY_Success != DRY_HAL_GetRegisterLockStatusMasked(base, &reg_name, &masked_out))
    {
        return FALSE  ;
    }    
    
    if (reg_name != masked_out)
    {
        return FALSE ;
    }
    
    /* specified DRY_WAC bit(s) must be set */
    if(kStatus_DRY_Success != DRY_HAL_GetRegisterWriteAccessMasked(base, &reg_name, &masked_out))
    {
        return FALSE ;
    }    
    
    if (reg_name != masked_out)
    {
        return FALSE ;
    }
    
    return TRUE;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_IsRegReadPossible
 * Description: Test if Read Access Control Register DRY_RAC allow reading.
 *              
 *END*********************************************************************/
bool DRY_HAL_IsRegReadPossible(DRY_Type *base, const hal_dry_reg_t reg_name)
{
    hal_dry_reg_t masked_out;
    
    /* specified DRY_RAC bit(s) must be set */
    if(kStatus_DRY_Success != DRY_HAL_GetRegisterReadAccessMasked(base, &reg_name, &masked_out))
    {
        return FALSE ;
    }    
    
    if (reg_name != masked_out)
    {
        return FALSE ;
    }
            
    return TRUE;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_IsSecureKeyWritePossible
 * Description: Test if Secure Key Write Lock DRY_SKWLR and Write Access Registers DRY_SWAC allow writing.
 *
 *              
 *END*********************************************************************/
bool DRY_HAL_IsSecureKeyWritePossible(DRY_Type *base, const hal_dry_secure_key_reg_selector_t reg_name)
{
    hal_dry_secure_key_reg_selector_t masked_out;
    
    /* specified DRY_SKWLR bit(s) must be set */
    if(kStatus_DRY_Success != DRY_HAL_GetSecureKeyWriteLockMasked(base, &reg_name, &masked_out))
    {
        return FALSE ;
    }    
    
    if (reg_name != masked_out)
    {
        return FALSE ;
    }
        
    /* specified DRY_SWAC bit(s) must be set */
    if(kStatus_DRY_Success != DRY_HAL_GetSecureWriteAccessMasked(base, &reg_name, &masked_out))
    {
        return FALSE ;
    }    
    
    if (reg_name != masked_out)
    {
        return FALSE ;
    }
    
    return TRUE;
}

/*FUNCTION****************************************************************
 *
 * Function Name: DRY_HAL_IsSecureKeyReadPossible
 * Description: Test if Secure Key Read Lock DRY_SKRL and Read Access Registers DRY_SRAC allow reading.
 *
 *              
 *END*********************************************************************/
bool DRY_HAL_IsSecureKeyReadPossible(DRY_Type *base, const hal_dry_secure_key_reg_selector_t reg_name)
{
    hal_dry_secure_key_reg_selector_t masked_out;
    
    /* specified DRY_SKRLR bit(s) must be set */
    if(kStatus_DRY_Success != DRY_HAL_GetSecureKeyReadLockMasked(base, &reg_name, &masked_out))
    {
        return FALSE ;
    }    
    
    if (reg_name != masked_out)
    {
        return FALSE ;
    }
    
    /* specified DRY_SRAC bit(s) must be set */
    if(kStatus_DRY_Success != DRY_HAL_GetSecureReadAccessMasked(base, &reg_name, &masked_out))
    {
        return FALSE ;
    }    
    
    if (reg_name != masked_out)
    {
        return FALSE ;
    }
    
    return TRUE;
}

#endif /* FSL_FEATURE_SOC_DRY_COUNT */
