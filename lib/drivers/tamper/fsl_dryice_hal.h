/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Freescale Confidential and Proprietary - use of this software is
 * governed by the LA_OPT_FSL_OPEN_3RD_PARTY_IP License distributed
 * with this Material.
 */
 
#ifndef __FSL_DRYICE_HAL_H__
#define __FSL_DRYICE_HAL_H__

//#include <assert.h>
//#include <stdint.h>
#include "common.h"
//#include "drv_inc.h"
#include "MacroDef.h"
//#include "dryice_drv.h"
#define  FSL_FEATURE_SOC_DRY_COUNT 1

#if FSL_FEATURE_SOC_DRY_COUNT
/*!
 * @addtogroup dryice_hal
 * @{
 */

/*! @file*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Error codes for the DryIce HAL. */
typedef enum _hal_dry_status_t
{
    kStatus_DRY_Success =  0,
    kStatus_DRY_Failure = -1,
} hal_dry_status_t;

/*! @brief Enable and disable codes for the DryIce HAL. */
typedef enum _hal_dry_enable_t
{
    kDryDisable =  0,
    kDryEnable =  1,
} hal_dry_enable_t;

/*!
 * @brief DryIce Active Tamper Clock Source.
 *
 * These constants define the clock source for Active Tamper Shift Register to configure in a DryIce base.
 */
typedef enum _hal_dry_act_tamper_clock_t {
    kDryClkType_1Hz = 0U,  /*!< clocked by 1 Hz prescaler clock  */
    kDryClkType_512Hz = 1U,  /*!< clocked by 512 Hz prescaler clock */
} hal_dry_act_tamper_clock_t;

/*!
 * @brief DryIce Active Tamper Register selector.
 *
 * These constants define the Active Tamper Register to configure in a DryIce base.
 */
typedef enum _hal_dry_act_tamper_selector_t {
    kDryActiveTamperReg_0 = 1U << 0,  /*!< Active Tamper Shift Register 0 */
    kDryActiveTamperReg_1 = 1U << 1,  /*!< Active Tamper Shift Register 1 */
    kDryActiveTamperReg_All = kDryActiveTamperReg_0 | kDryActiveTamperReg_1, /*!< Selection of all Active Tamper Registers */
} hal_dry_act_tamper_selector_t;

/*!
 * @brief DryIce Tamper Hysteresis Select.
 *
 * These constants define the tamper pin hysteresis range to configure in a DryIce base.
 */
typedef enum _hal_dry_hysteresis_t {
    kDryHystType_305mV = 0U,  /*!< hysteresis set to a range of 305 mV to 440 mV  */
    kDryHystType_490mV = 1U,  /*!< hysteresis set to a range of 490 mV to 705 mV */
} hal_dry_hysteresis_t;

/*!
 * @brief DryIce Tamper Drive Strength.
 *
 * These constants define tamper pin drive strength to configure in a DryIce base.
 */
typedef enum _hal_dry_drive_strength_t {
    kDryDriveStrengthType_Low = 0U,  /*!< tamper pins configured for low drive strength  */
    kDryDriveStrengthType_High = 1U,  /*!< tamper pins configured for high drive strength */
} hal_dry_drive_strength_t;

/*!
 * @brief DryIce Tamper Slew Rate.
 *
 * These constants define tamper pin slew rate to configure in a DryIce base.
 */
typedef enum _hal_dry_slew_rate_t {
    kDrySlewRateType_Slow = 0U,  /*!< tamper pins configured for slow slew rate  */
    kDrySlewRateType_Fast = 1U,  /*!< tamper pins configured for fast slew rate */
} hal_dry_slew_rate_t;

/*!
 * @brief DryIce Secure Register File.
 *
 * These constants define general purpose VBAT Register File to configure in a DryIce base.
 */
typedef enum _hal_dry_secure_register_file_t {
    kDrySecureRegisterFile_General = 0U,  /*!< VBAT Register File is general purpose, reset on VBAT POR only, supervisor and user mode accessible */
    kDrySecureRegisterFile_RstWithTamper = 2U,  /*!< VBAT Register file is secure, reset on DryIce tampering detected, supervisor mode access only */
    kDrySecureRegisterFile_RstWithTamperOrInterrupt = 3U,  /*!< VBAT Register file is secure, reset on DryIce tampering detected or DryIce interrupt assertion, supervisor mode access only */
} hal_dry_secure_register_file_t;

/*!
 * @brief DryIce Glitch Filter Tamper Pin Clock Source.
 *
 * These constants define tamper pin glitch filter clock source to configure in a DryIce base.
 */
typedef enum _hal_dry_glitch_filter_prescaler_t {
	  kDryGlitchFilterClock_512Hz = 0U, /*!< Glitch Filter on tamper pin is clocked by the 512 Hz prescaler clock */
	  kDryGlitchFilterClock_32768Hz = 1U, /*!< Glitch Filter on tamper pin is clocked by the 32768 Hz prescaler clock */
} hal_dry_glitch_filter_prescaler_t;

/*!
 * @brief DryIce Glitch Filter Tamper Pin Sample Width.
 *
 * These constants define tamper pin glitch filter sample width to configure in a DryIce base.
 */
typedef enum _hal_dry_glitch_filter_sample_width_t {
    kDryGlitchFilterSample_Disable = 0U,  /*!<  sampling disabled */
    kDryGlitchFilterSample_2Cycles = 1U,  /*!<  sample width pull enable/input buffer enable=2 cycles/1 cycle */
    kDryGlitchFilterSample_4Cycles = 2U,  /*!<  sample width pull enable/input buffer enable=4 cycles/2 cycles */
    kDryGlitchFilterSample_8Cycles = 3U,  /*!<  sample width pull enable/input buffer enable=8 cycles/4 cycles */
} hal_dry_glitch_filter_sample_width_t;

/*!
 * @brief DryIce Glitch Filter Tamper Pin Sample Frequency.
 *
 * These constants define tamper pin glitch filter sample frequency to configure in a DryIce base.
 */
typedef enum _hal_dry_glitch_filter_sample_freq_t {
    kDryGlitchFilterSamplingEvery_8Cycles = 0U,  /*!<  sample once every 8 cycles */
    kDryGlitchFilterSamplingEvery_32Cycles = 1U,  /*!<  sample once every 32 cycles */
    kDryGlitchFilterSamplingEvery_128Cycles = 2U,  /*!<  sample once every 128 cycles */
    kDryGlitchFilterSamplingEvery_512Cycles = 3U,  /*!<  sample once every 512 cycles */
} hal_dry_glitch_filter_sample_freq_t;

/*!
 * @brief DryIce Glitch Filter Tamper Pin Expected Value.
 *
 * These constants define tamper pin glitch filter expected value to configure in a DryIce base.
 */
typedef enum _hal_dry_glitch_filter_expected_t {
    kDryGlitchFilterExpected_LogicZero = 0U,  /*!<  Expected value is logic zero */
    kDryGlitchFilterExpected_ActTamperOut_0 = 1U,  /*!<  Expected value is active tamper 0 output */
    kDryGlitchFilterExpected_ActTamperOut_1 = 2U,  /*!<  Expected value is active tamper 1 output */
    kDryGlitchFilterExpected_ActTamperOut_XOR = 3U,  /*!<  Expected value is active tamper 0 output XORed with active tamper 1 output */
} hal_dry_glitch_filter_expected_t;

/*!
 * @brief DryIce Glitch Filter Tamper Pull Select.
 *
 * These constants define tamper pin glitch filter pull direction to configure in a DryIce base.
 */
typedef enum _hal_dry_glitch_filter_pull_t {
    kDryGlitchFilterPullType_Assert = 0U,  /*!< Tamper pin pull direction always asserts the tamper pin. */
    kDryGlitchFilterPullType_Negate = 1U,  /*!< Tamper pin pull direction always negates the tamper pin. */
} hal_dry_glitch_filter_pull_t;

/*!
 * @brief DryIce Status Register flags.
 *
 * This provides constants for the DryIce Status Register. 
 */
typedef enum _hal_dry_status_flag_t {
    kDryStatus_TamperFlag = 1U << DRY_SR_DTF_SHIFT, /*!< DryIce Tamper Flag */
    kDryStatus_TamperAckFlag = 1U << DRY_SR_TAF_SHIFT, /*!< DryIce Tamper Acknowledge Flag */
    kDryStatus_TimeOverflowTamper = 1U << DRY_SR_TOF_SHIFT, /*!< DryIce RTC Time Overflow Tamper detected */
    kDryStatus_MonotonicOverflowTamper = 1U << DRY_SR_MOF_SHIFT, /*!< DryIce RTC Monotonic Overflow Tamper detected */
    kDryStatus_VoltageTamper = 1U << DRY_SR_VTF_SHIFT, /*!< DryIce Voltage Tamper detected */
    kDryStatus_ClockTamper = 1U << DRY_SR_CTF_SHIFT, /*!< DryIce Clock Tamper detected */
    kDryStatus_TemperatureTamper = 1U << DRY_SR_TTF_SHIFT, /*!< DryIce Temperature Tamper detected */
    kDryStatus_SecurityTamper = 1U << DRY_SR_STF_SHIFT, /*!< DryIce Security Tamper detected */
    kDryStatus_FlashSecurityTamper = 1U << DRY_SR_FSF_SHIFT, /*!< DryIce Flash Security Tamper detected */
    kDryStatus_TestModeTamper = 1U <<  DRY_SR_TMF_SHIFT, /*!< DryIce Test Mode Tamper detected */
    kDryStatus_TamperPinTamper_0 = 1U << DRY_SR_TPF_SHIFT, /*!< DryIce Tamper Pin 0 Tamper detected */
    kDryStatus_TamperPinTamper_1 = 1U << (DRY_SR_TPF_SHIFT+1), /*!< DryIce Tamper Pin 1 Tamper detected */
    kDryStatus_TamperPinTamper_2 = 1U << (DRY_SR_TPF_SHIFT+2), /*!< DryIce Tamper Pin 2 Tamper detected */
    kDryStatus_TamperPinTamper_3 = 1U << (DRY_SR_TPF_SHIFT+3), /*!< DryIce Tamper Pin 3 Tamper detected */
    kDryStatus_TamperPinTamper_4 = 1U << (DRY_SR_TPF_SHIFT+4), /*!< DryIce Tamper Pin 4 Tamper detected */
    kDryStatus_TamperPinTamper_5 = 1U << (DRY_SR_TPF_SHIFT+5), /*!< DryIce Tamper Pin 5 Tamper detected */
    kDryStatus_TamperPinTamper_6 = 1U << (DRY_SR_TPF_SHIFT+6), /*!< DryIce Tamper Pin 6 Tamper detected */
    kDryStatus_TamperPinTamper_7 = 1U << (DRY_SR_TPF_SHIFT+7), /*!< DryIce Tamper Pin 7 Tamper detected */
    kDryStatus_All = DRY_SR_TPF_MASK | DRY_SR_TMF_MASK | DRY_SR_FSF_MASK | DRY_SR_STF_MASK\
                     | DRY_SR_TTF_MASK | DRY_SR_CTF_MASK | DRY_SR_VTF_MASK | DRY_SR_MOF_MASK\
                     | DRY_SR_TOF_MASK | DRY_SR_TAF_MASK | DRY_SR_DTF_MASK, /*!< Mask for all of the DryIce Status Register bits */
} hal_dry_status_flag_t;

/*!
 * @brief DryIce Secure Key Registers.
 *
 * This provides constants for the DryIce Secure Key Registers selection.
 */
typedef enum _hal_dry_secure_key_reg_selector_t {
    kDrySecureKeyReg_0 = 1U << 0, /*!< Secure Key Register 0 */
    kDrySecureKeyReg_1 = 1U << 1, /*!< Secure Key Register 1 */
    kDrySecureKeyReg_2 = 1U << 2, /*!< Secure Key Register 2 */
    kDrySecureKeyReg_3 = 1U << 3, /*!< Secure Key Register 3 */
    kDrySecureKeyReg_4 = 1U << 4, /*!< Secure Key Register 4 */
    kDrySecureKeyReg_5 = 1U << 5, /*!< Secure Key Register 5 */
    kDrySecureKeyReg_6 = 1U << 6, /*!< Secure Key Register 6 */
    kDrySecureKeyReg_7 = 1U << 7, /*!< Secure Key Register 7 */
    kDrySecureKeyReg_All = 0xffU, /*!< Mask to select all DryIce Secure Key Registers */
} hal_dry_secure_key_reg_selector_t;

/*!
 * @brief DryIce Lock Register.
 *
 * This provides constants for the DryIce Lock Register.
 */
typedef enum _hal_dry_reg_t {
    kDryReg_KeyValid = 1U << DRY_LR_KVL_SHIFT, /*!< Secure Key Valid Register lock */
    kDryReg_KeyWrite = 1U << DRY_LR_KWL_SHIFT, /*!< Secure Key Write Register lock */
    kDryReg_KeyRead = 1U << DRY_LR_KRL_SHIFT, /*!< Secure Key Read Register lock */
    kDryReg_Control = 1U << DRY_LR_CRL_SHIFT, /*!< Control Register lock */
    kDryReg_Status = 1U << DRY_LR_SRL_SHIFT, /*!< Status Register lock */
    kDryReg_Lock = 1U << DRY_LR_LRL_SHIFT, /*!< Lock Register lock */
    kDryReg_InterruptEnbl = 1U << DRY_LR_IEL_SHIFT, /*!< Interrupt Enable Register lock */
    kDryReg_TamperSeconds = 1U << DRY_LR_TSL_SHIFT, /*!< Tamper Seconds Register lock */
    kDryReg_TamperEnbl = 1U << DRY_LR_TEL_SHIFT, /*!< Tamper Enable Register lock */
    kDryReg_PinDirection = 1U << DRY_LR_PDL_SHIFT, /*!< Pin Direction Register lock */
    kDryReg_PinPolarity = 1U << DRY_LR_PPL_SHIFT, /*!< Pin Polarity Register lock */
    kDryReg_ActiveTamper_0 = 1U << DRY_LR_ATL_SHIFT, /*!< Active Tamper Register 0 lock */
    kDryReg_ActiveTamper_1 = 1U << (DRY_LR_ATL_SHIFT+1), /*!< Active Tamper Register 1 lock */
    kDryReg_ActiveTamper_All = DRY_LR_ATL_MASK, /*!< Mask to select all register bits for Active Tamper configuration */
    kDryReg_GlitchFilter_0 = 1U << DRY_LR_GFL_SHIFT, /*!< Glitch Filter Register 0 lock */
    kDryReg_GlitchFilter_1 = 1U << (DRY_LR_GFL_SHIFT+1), /*!< Glitch Filter Register 1 lock */
    kDryReg_GlitchFilter_2 = 1U << (DRY_LR_GFL_SHIFT+2), /*!< Glitch Filter Register 2 lock */
    kDryReg_GlitchFilter_3 = 1U << (DRY_LR_GFL_SHIFT+3), /*!< Glitch Filter Register 3 lock */
    kDryReg_GlitchFilter_4 = 1U << (DRY_LR_GFL_SHIFT+4), /*!< Glitch Filter Register 4 lock */
    kDryReg_GlitchFilter_5 = 1U << (DRY_LR_GFL_SHIFT+5), /*!< Glitch Filter Register 5 lock */
    kDryReg_GlitchFilter_6 = 1U << (DRY_LR_GFL_SHIFT+6), /*!< Glitch Filter Register 6 lock */
    kDryReg_GlitchFilter_7 = 1U << (DRY_LR_GFL_SHIFT+7), /*!< Glitch Filter Register 7 lock */
    kDryReg_GlitchFilter_All = DRY_LR_GFL_MASK, /*!< Mask to select all register bits for Glitch Filters configuration */
    kDryReg_All = DRY_LR_GFL_MASK | DRY_LR_ATL_MASK | DRY_LR_PPL_MASK | DRY_LR_PDL_MASK\
                   | DRY_LR_TEL_MASK | DRY_LR_TSL_MASK | DRY_LR_IEL_MASK | DRY_LR_LRL_MASK\
                   | DRY_LR_SRL_MASK | DRY_LR_CRL_MASK | DRY_LR_KRL_MASK | DRY_LR_KWL_MASK\
                   | DRY_LR_KVL_MASK, /*!< Mask to select all DryIce Lock Register bits */
} hal_dry_reg_t;

/*!
 * @brief DryIce Interrupt Enable Register.
 *
 * This provides constants for the DryIce Interrupt Enable Register.
 */
typedef enum _hal_dry_interrupt_t {
    kDryInterrupt_Tamper = 1U << DRY_IER_DTIE_SHIFT, /*!< DryIce Tamper Interrupt  */
    kDryInterrupt_TimeOverflow = 1U << DRY_IER_TOIE_SHIFT, /*!< DryIce Time Overflow Interrupt */
    kDryInterrupt_MonotonicOverflow = 1U << DRY_IER_MOIE_SHIFT, /*!< DryIce Monotonic Overflow Interrupt */
    kDryInterrupt_VoltageTamper = 1U << DRY_IER_VTIE_SHIFT, /*!< DryIce Voltage Tamper Interrupt */
    kDryInterrupt_ClockTamper = 1U << DRY_IER_CTIE_SHIFT, /*!< DryIce Clock Tamper Interrupt */
    kDryInterrupt_TemperatureTamper = 1U << DRY_IER_TTIE_SHIFT, /*!< DryIce Temperature Tamper Interrupt */
    kDryInterrupt_SecurityTamper = 1U << DRY_IER_STIE_SHIFT, /*!< DryIce Security Tamper Interrupt */
    kDryInterrupt_FlashSecurityTamper = 1U << DRY_IER_FSIE_SHIFT, /*!< DryIce Flash Security Tamper Interrupt */
    kDryInterrupt_TestModeTamper = 1U << DRY_IER_TMIE_SHIFT, /*!< DryIce Test Mode Tamper Interrupt */
    kDryInterrupt_TamperPinTamper_0 = 1U << DRY_IER_TPIE_SHIFT, /*!< DryIce Tamper Pin Tamper 0 Interrupt */
    kDryInterrupt_TamperPinTamper_1 = 1U << (DRY_IER_TPIE_SHIFT+1), /*!< DryIce Tamper Pin Tamper 1 Interrupt */
    kDryInterrupt_TamperPinTamper_2 = 1U << (DRY_IER_TPIE_SHIFT+2), /*!< DryIce Tamper Pin Tamper 2 Interrupt */
    kDryInterrupt_TamperPinTamper_3 = 1U << (DRY_IER_TPIE_SHIFT+3), /*!< DryIce Tamper Pin Tamper 3 Interrupt */
    kDryInterrupt_TamperPinTamper_4 = 1U << (DRY_IER_TPIE_SHIFT+4), /*!< DryIce Tamper Pin Tamper 4 Interrupt */
    kDryInterrupt_TamperPinTamper_5 = 1U << (DRY_IER_TPIE_SHIFT+5), /*!< DryIce Tamper Pin Tamper 5 Interrupt */
    kDryInterrupt_TamperPinTamper_6 = 1U << (DRY_IER_TPIE_SHIFT+6), /*!< DryIce Tamper Pin Tamper 6 Interrupt */
    kDryInterrupt_TamperPinTamper_7 = 1U << (DRY_IER_TPIE_SHIFT+7), /*!< DryIce Tamper Pin Tamper 7 Interrupt */
    kDryInterrupt_TamperPinTamper_All = DRY_IER_TPIE_MASK, /*!< DryIce All Tamper Pins Interrupt */
    kDryInterrupt_All = DRY_IER_TPIE_MASK | DRY_IER_TMIE_MASK | DRY_IER_FSIE_MASK | DRY_IER_STIE_MASK\
                        | DRY_IER_TTIE_MASK | DRY_IER_CTIE_MASK | DRY_IER_VTIE_MASK\
                        | DRY_IER_MOIE_MASK | DRY_IER_TOIE_MASK | DRY_IER_DTIE_MASK, /*!< Mask to select all DryIce Interrupt Enable Register bits */
} hal_dry_interrupt_t;

/*!
 * @brief DryIce Tamper Enable Register.
 *
 * This provides constants for the DryIce Tamper Enable Register.
 */
typedef enum _hal_dry_tamper_t {
    kDryTamper_TimeOverflow = 1U << DRY_TER_TOE_SHIFT, /*!< Time Overflow Tamper Enable */
    kDryTamper_MonotonicOverflow = 1U << DRY_TER_MOE_SHIFT, /*!< Monotonic Overflow Tamper Enable */
    kDryTamper_Voltage = 1U << DRY_TER_VTE_SHIFT, /*!< Voltage Tamper Enable */
    kDryTamper_Clock = 1U << DRY_TER_CTE_SHIFT, /*!< Clock Tamper Enable */
    kDryTamper_Temperature = 1U << DRY_TER_TTE_SHIFT, /*!< Temperature Tamper Enable */
    kDryTamper_Security = 1U << DRY_TER_STE_SHIFT, /*!< Security Tamper Enable */
    kDryTamper_FlashSecurity = 1U << DRY_TER_FSE_SHIFT, /*!< Flash Security Tamper Enable */
    kDryTamper_TestMode = 1U << DRY_TER_TME_SHIFT, /*!< Test Mode Tamper Enable */
    kDryTamper_TamperPin_0 = 1U << DRY_TER_TPE_SHIFT, /*!< Tamper Pin 0 Tamper Enable */
    kDryTamper_TamperPin_1 = 1U << (DRY_TER_TPE_SHIFT+1), /*!< Tamper Pin 1 Tamper Enable */
    kDryTamper_TamperPin_2 = 1U << (DRY_TER_TPE_SHIFT+2), /*!< Tamper Pin 2 Tamper Enable */
    kDryTamper_TamperPin_3 = 1U << (DRY_TER_TPE_SHIFT+3), /*!< Tamper Pin 3 Tamper Enable */
    kDryTamper_TamperPin_4 = 1U << (DRY_TER_TPE_SHIFT+4), /*!< Tamper Pin 4 Tamper Enable */
    kDryTamper_TamperPin_5 = 1U << (DRY_TER_TPE_SHIFT+5), /*!< Tamper Pin 5 Tamper Enable */
    kDryTamper_TamperPin_6 = 1U << (DRY_TER_TPE_SHIFT+6), /*!< Tamper Pin 6 Tamper Enable */
    kDryTamper_TamperPin_7 = 1U << (DRY_TER_TPE_SHIFT+7), /*!< Tamper Pin 7 Tamper Enable */
    kDryTamper_TamperPin_All = DRY_TER_TPE_MASK, /*!< All Tamper Pin Tamper Enable */
    kDryTamper_All = DRY_TER_TPE_MASK | DRY_TER_TME_MASK | DRY_TER_FSE_MASK | DRY_TER_STE_MASK\
                     | DRY_TER_TTE_MASK | DRY_TER_CTE_MASK | DRY_TER_VTE_MASK\
                     | DRY_TER_MOE_MASK | DRY_TER_TOE_MASK, /*!< Mask to select all Tamper Enable Register bits */
} hal_dry_tamper_t;

/*!
 * @brief DryIce Tamper Pins.
 *
 * This provides constants for the DryIce Tamper Pin selection.
 */
typedef enum _hal_dry_tamper_pin_selector_t {
    kDryTamperPin_0 = 1U << 0, /*!< Tamper Pin 0 */
    kDryTamperPin_1 = 1U << 1, /*!< Tamper Pin 1 */
    kDryTamperPin_2 = 1U << 2, /*!< Tamper Pin 2 */
    kDryTamperPin_3 = 1U << 3, /*!< Tamper Pin 3 */
    kDryTamperPin_4 = 1U << 4, /*!< Tamper Pin 4 */
    kDryTamperPin_5 = 1U << 5, /*!< Tamper Pin 5 */
    kDryTamperPin_6 = 1U << 6, /*!< Tamper Pin 6 */
    kDryTamperPin_7 = 1U << 7, /*!< Tamper Pin 7 */
    kDryTamperPin_All = 0xffU, /*!< Mask to select all DryIce Tamper Pins */
} hal_dry_tamper_pin_selector_t;

/*!
 * @brief DryIce Tamper Pin Direction.
 *
 * These constants define tamper pin direction to configure in a DryIce base.
 */
typedef enum _hal_dry_pin_direction_t {
    kDryTamperPinDirection_In = 0U,  /*!< tamper pins configured as input  */
    kDryTamperPinDirection_Out = 1U,  /*!< tamper pins configured as output, drives inverse of expected value */
} hal_dry_pin_direction_t;

/*!
 * @brief DryIce Tamper Pin Polarity.
 *
 * These constants define tamper pin polarity to configure in a DryIce base.
 */
typedef enum _hal_dry_pin_polarity_t {
    kDryTamperPinPolarity_ExpectValNormal = 0U,  /*!< tamper pin expected value is not inverted  */
    kDryTamperPinPolarity_ExpectValInverted = 1U,  /*!< tamper ping expected value is inverted */
} hal_dry_pin_polarity_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name DryIce Common Configurations
 * @{
 */

/*!
 * @brief Initializes DryIce.
 *
 * This function initializes the module to a known state whenever possible.
 * 
 * Write reset values to the following DryIce registers, if writing is possible (writing is controlled by DRY_LR, DRY_WAC and DRY_RAC):
 *   DRY_CR, DRY_PGFRn, DRY_IER, DRY_TER, DRY_PDR, DRY_PPR, DRY_ATR.
 * The function does not change DryIce module registers that control read/write-ability:
 *   DRY_SKWLR, DRY_SKRLR, DRY_LR, DRY_WAC, DRY_RAC, DRY_SWAC and DRY_SRAC.
 * The function does not change DryIce module Secure Key related registers:
 *   DRY_SKVR, DRY_SKRn
 * The function does not change DryIce module status related registers:
 *   DRY_SR, DRY_TSR
 * @param   base DryIce module base pointer.
 */
void DRY_HAL_Init(DRY_Type * base);

/*!
 * @brief Enables the DryIce clock and prescaler.
 *
 * This function allows the user to enable the DryIce clock and prescaler.
 *
 * @param   base DryIce module base pointer.
 */
static inline void DRY_HAL_Enable(DRY_Type * base)
{
    DRY_CR_REG(base) |= DRY_CR_DEN(1U);
}

/*!
 * @brief Disables the DryIce clock and prescaler.
 *
 * This function allows the user to disable the DryIce clock and prescaler.
 * If the second argument is given as TRUE, the function, prior to disabling
 * the DryIce clock and prescaler, calls other HAL functions to disable
 * all Glitch Filters and Active Tampers.
 *
 * @param   base DryIce module base pointer.
 * @param   force Disable all Glitch Filters and Active Tampers prior to clock and prescaler disable.
 */
void DRY_HAL_Disable(DRY_Type * base, bool force);

/*!
 * @brief Get enable status of DryIce clock and prescaler.
 *
 * This function allows the user to test if DryIce clock and prescaler is enabled.
 *
 * @param   base DryIce module base pointer.
 * @return  true DryIce clock and prescaler is enabled.
 * @return  false DryIce clock and prescaler is disabled.
 */
static inline bool DRY_HAL_IsEnabled(DRY_Type * base)
{
    return (bool) (DRY_CR_REG(base) & DRY_CR_DEN_MASK);
}

/*!
 * @brief Performs a software reset of the DryIce module.
 *
 * This function allows the user to set a software reset of the DryIce module.
 * The software reset resets all DryIce registers except the read and write access registers 
 * WAC, RAC, SWAC, and SRAC. The read and write access registers are reset only by the chip reset.
 *
 * Note that the software reset remains asserted after this function completes and the DryIce module
 * is kept in a software reset. User software should negate the software reset request to be able
 * to further use the DryIce module:
 * 
 * DRY_HAL_SoftwareReset(base);
 * while (false == DRY_HAL_GetStatusFlag(base, kDryStatus_TamperFlag)) {}
 * DRY_HAL_SoftwareResetRelease(base);
 *
 * @param   base DryIce module base pointer.
 */
static inline void DRY_HAL_SoftwareReset(DRY_Type * base)
{
    DRY_CR_REG(base) = DRY_CR_SWR(1U);
}

/*!
 * @brief Releases a software reset of the DryIce module.
 *
 * This function allows the user to negate a software reset of the DryIce module.
 * The software reset resets all DryIce registers except the read and write access registers 
 * WAC, RAC, SWAC, and SRAC. The read and write access registers are reset only by the chip reset.
 * 
 * @param   base DryIce module base pointer.
 */
static inline void DRY_HAL_SoftwareResetRelease(DRY_Type * base)
{
    DRY_CR_REG(base) &= ~DRY_CR_SWR(1U);
}

/*!
 * @brief Configures DryIce to force the chip reset when tampering is detected.
 *
 * This function allows the user to configure the DryIce module to assert
 * the chip reset upon tamper detection.
 *
 * @param   base DryIce module base pointer.
 */
static inline void DRY_HAL_SetForceSystemReset(DRY_Type * base)
{
    DRY_CR_REG(base) |= DRY_CR_TFSR(1U);
}

/*!
 * @brief Configures DryIce not to force chip reset when tampering is detected.
 *
 * This function allows the user to configure the DryIce module not to drive
 * the chip reset upon tamper detection.
 *
 * @param   base DryIce module base pointer.
 */
static inline void DRY_HAL_ClrForceSystemReset(DRY_Type * base)
{
    DRY_CR_REG(base) &= ~(DRY_CR_TFSR(1U));
}

/*!
 * @brief Prevents the DryIce Tamper Flag from being cleared.
 *
 * This function allows the user to configure the DryIce module to prevent
 * the Tamper Flag (SR[DTF]) from being cleared after it is set.
 *
 * @param   base DryIce module base pointer.
 */
static inline void DRY_HAL_SetUpdateMode(DRY_Type * base)
{
    DRY_CR_REG(base) |= DRY_CR_UM(1U);
}

/*!
 * @brief Allows the DryIce Status Register to be cleared when tampering has been detected.
 *
 * This function allows the user to configure the DryIce module to allow
 * the Tamper Flag (SR[DTF]) to be cleared after it is set. 
 *
 * @param   base DryIce module base pointer.
 */
static inline void DRY_HAL_ClrUpdateMode(DRY_Type * base)
{
    DRY_CR_REG(base) &= ~(DRY_CR_UM(1U));
}

/*!
 * @brief In DryIce Control Register, selects the source prescaler clock for the Active Tamper Shift Register.
 *
 * Selects between 512 Hz and 1 Hz prescaler clocks as the source for the specified
 * Active Tamper Shift Register. reg_name argument can select one or multiple
 * Active Tamper Shift Registers to be configured with the clock source defined by the clock argument.
 * The function also sets the initial value for the DryIce prescaler, unless init_count is -1. 
 * This function is called only when DryIce is disabled, if init_count argument is passed 
 * as greater than or equal to zero. If init_count is set to -1, the function does not write 
 * to the DryIce prescaler and can be called when DryIce is enabled. Although, it is not called 
 * when the corresponding active tamper register is enabled.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Selection of Active Tamper Shift Register(s) as a bit mask, pass kDryActiveTamperShift_All to select all Active Tamper Shift registers.
 * @param   clock Source clock select for the selected (by reg argument) Active Tamper Shift Register(s).
 * @param   init_count DryIce Prescaler Register value. When set to -1, writing the Prescaler Register is bypassed.
 */
void DRY_HAL_SetActiveTamperClock(DRY_Type * base, hal_dry_act_tamper_selector_t reg_name, hal_dry_act_tamper_clock_t clock, 
                                  int32_t init_count);

/*!
 * @brief Reads the DryIce Prescaler Register.
 *
 * This function allows the user to read the DryIce Prescaler Register
 * and the current value of the prescaler.
 *
 * @param   base DryIce module base pointer.
 */                                  
static inline uint32_t DRY_HAL_GetPrescaler(DRY_Type * base)
{
    return (DRY_CR_REG(base) & DRY_CR_DPR_MASK) >> DRY_CR_DPR_SHIFT;
}

/*!
 * @brief Writes to the DryIce Prescaler Register.
 *
 * This function allows the user to set an actual value of the DryIce Prescaler Register
 * and the current value of the prescaler.
 * This register can only be written to when the DryIce clocks are disabled (when writing logic zero to the DryIce Enable bit).
 * DryIce Enable bit can be cleared by function void DRY_HAL_Disable(DRY_Type * base, bool force); 
 *
 * @param   base DryIce module base pointer.
 */                                  
static inline void DRY_HAL_SetPrescaler(DRY_Type * base, uint32_t prescaler_count)
{
    DRY_CR_REG(base) &= ~DRY_CR_DPR_MASK;
    DRY_CR_REG(base) |= DRY_CR_DPR(prescaler_count);    
}

/*!
 * @brief Configures the Tamper Pin Hysteresis.
 *
 * This function allows the user to configure the hysteresis voltage range.
 *
 * @param   base DryIce module base pointer.
 * @param   type Hysteresis range type, one of 305 mV to 440 mV or 490 mV to 705 mV.
 */
static inline void DRY_HAL_SetHysteresis(DRY_Type * base, hal_dry_hysteresis_t type)
{
    if (kDryHystType_490mV == type)
    {
        DRY_CR_REG(base) |= DRY_CR_THYS(1U);
    }
    else
    {
        DRY_CR_REG(base) &= ~(DRY_CR_THYS(1U));
    }
}

/*!
 * @brief Configures the Tamper Passive Filter.
 *
 * This function allows the user to configure the optional low-pass filter (10 MHz to 30 MHz bandwidth)
 * and to disable the Input Filter when supporting the high-speed interfaces (greater than 2 MHz) on the pin.
 *
 * @param   base DryIce module base pointer.
 * @param   enable kDryEnable: enable, kDryDisable: disable the low pass filter on digital input path.
 */
static inline void DRY_HAL_SetPassiveFilter(DRY_Type * base, hal_dry_enable_t enable)
{
    if (kDryEnable == enable)
    {
        DRY_CR_REG(base) |= DRY_CR_TPFE(1U);
    }
    else
    {
        DRY_CR_REG(base) &= ~(DRY_CR_TPFE(1U));
    }
}

/*!
 * @brief Configures the tamper drive strength.
 * 
 * Selects tamper pin's drive strength.
 *
 * @param   base DryIce module base pointer.
 * @param   type kDryDriveStrengthType_Low: low, kDryDriveStrengthType_High: high drive strength.
 */
static inline void DRY_HAL_SetDriveStrength(DRY_Type * base, hal_dry_drive_strength_t type)
{
    if (kDryDriveStrengthType_High == type)
    {
        DRY_CR_REG(base) |= DRY_CR_TDSE(1U);
    }
    else
    {
        DRY_CR_REG(base) &= ~(DRY_CR_TDSE(1U));
    }
}

/*!
 * @brief Configures the tamper slew rate.
 * 
 * Selects tamper pin's slew rate.
 *
 * @param   base DryIce module base pointer.
 * @param   type kDrySlewRateType_Slow: slow, kDrySlewRateType_Fast: fast slew rate.
 */
static inline void DRY_HAL_SetSlewRate(DRY_Type * base, hal_dry_slew_rate_t type)
{
    if (kDrySlewRateType_Fast == type)
    {
        DRY_CR_REG(base) |= DRY_CR_TSRE(1U);
    }
    else
    {
        DRY_CR_REG(base) &= ~(DRY_CR_TSRE(1U));
    }
}

/*!
 * @brief Configures the DryIce Secure Register file.
 * 
 * Selects the reset behavior and supervisor versus the user mode access for the VBAT Register file.
 *
 * @param   base DryIce module base pointer.
 * @param   type kDrySecureRegisterFile_General, kDrySecureRegisterFile_RstWithTamper, kDrySecureRegisterFile_RstWithTamperOrInterrupt
 */
static inline void DRY_HAL_SetSecureRegisterFile(DRY_Type * base, hal_dry_secure_register_file_t type)
{
    DRY_CR_REG(base) &= ~DRY_CR_SRF_MASK;
    DRY_CR_REG(base) |= DRY_CR_SRF(type);
}

/*@}*/

/*!
 * @name DryIce Glitch Filter Configurations
 * @{
 */

/*!
 * @brief Configures the glitch filter prescaler and width.
 * 
 * Configures the glitch filter prescaler and width.
 *
 * @param   base DryIce module base pointer.
 * @param   pin  Selects the glitch filter for the corresponding tamper pin. 
 *               Multiple pins can be selected as a bit mask, example: 
 *               pin = (hal_dry_tamper_pin_selector_t) (kDryTamperPin_1 | kDryTamperPin_5);
 *               Pass kDryTamperPin_All to select all pins. 
 * @param   prescaler source clock for the glitch filter prescaler. kDryGlitchFilterClock_512Hz or kDryGlitchFilterClock_32768Hz.
 * @param   width Integer number in range between 0 to 63. The value (width+1)*2 determines number of clock edges the input must remain stable
 *                to be passed through the glitch filter for a tamper pin.
 */ 
void DRY_HAL_SetGlitchFilterClock(DRY_Type * base, 
                                  hal_dry_tamper_pin_selector_t pin, 
                                  hal_dry_glitch_filter_prescaler_t prescaler, 
                                  uint32_t width);

/*!
 * @brief Enables the glitch filter.
 * 
 * Enables the glitch filter on a tamper pin.
 *
 * @param   base DryIce module base pointer.
 * @param   pin  Selects the glitch filter for the corresponding tamper pin.
 *               Multiple pins can be selected as a bit mask, example: 
 *               pin = (hal_dry_tamper_pin_selector_t) (kDryTamperPin_1 | kDryTamperPin_5);
 *               Pass kDryTamperPin_All to select all pins. 
 */
void DRY_HAL_GlitchFilterEnable(DRY_Type * base, hal_dry_tamper_pin_selector_t pin);

/*!
 * @brief Bypasses the glitch filter.
 * 
 * Disables the glitch filter on a tamper pin.
 *
 * @param   base DryIce module base pointer.
 * @param   pin  Selects the glitch filter for the corresponding tamper pin.
 *               Multiple pins can be selected, as a bit mask, pass kDryTamperPin_All to select all pins.
 */
void DRY_HAL_GlitchFilterDisable(DRY_Type * base, hal_dry_tamper_pin_selector_t pin);

/*!
 * @brief Configures the glitch filter pin sampling.
 * 
 * Configures the glitch filter tamper pin sampling parameters.
 *
 * @param   base DryIce module base pointer.
 * @param   pin  Selects the glitch filter for the corresponding tamper pin.
 *               Multiple pins can be selected as a bit mask, example: 
 *               pin = (hal_dry_tamper_pin_selector_t) (kDryTamperPin_1 | kDryTamperPin_5);
 *               Pass kDryTamperPin_All to select all pins. 
 * @param   width Tamper Pin sample width.
 * @param   freq Tamper Pin sampling frequency.
 */
void DRY_HAL_SetGlitchFilterSampling(DRY_Type * base, hal_dry_tamper_pin_selector_t pin, 
                                     hal_dry_glitch_filter_sample_width_t width, 
                                     hal_dry_glitch_filter_sample_freq_t freq);

/*!
 * @brief Configures the glitch filter pin expected value.
 * 
 * Configures the glitch filter tamper pin expected value.
 *
 * @param   base DryIce module base pointer.
 * @param   pin  Selects the glitch filter for the corresponding tamper pin.
 * @param   value Glitch Filter expected value.
 */                                     
void DRY_HAL_SetGlitchFilterExpected(DRY_Type * base, hal_dry_tamper_pin_selector_t pin, 
                                     hal_dry_glitch_filter_expected_t value);

/*!
 * @brief Configures the glitch filter pin pull device.
 * 
 * Configures the tamper pin pull resistor.
 *
 * @param   base DryIce module base pointer.
 * @param   pin  Selects the glitch filter for the corresponding tamper pin.
 *               Multiple pins can be selected as a bit mask, example: 
 *               pin = (hal_dry_tamper_pin_selector_t) (kDryTamperPin_1 | kDryTamperPin_5);
 *               Pass kDryTamperPin_All to select all pins. 
 * @param   enable kDryEnable/kDryDisable for enable/disable pull resistor. 
 * @param   type Direction of tamper pin pull resistor. 
 */
void DRY_HAL_SetGlitchFilterPull(DRY_Type * base, hal_dry_tamper_pin_selector_t pin, 
                                 hal_dry_enable_t enable, hal_dry_glitch_filter_pull_t type);
 
/*@}*/

/*!
 * @name DryIce Status Register
 * @{
 */

/*!
 * @brief Gets the DryIce Status Register bit.
 * 
 * Reads the status flag from the DryIce Status Register.
 * If the flag argument selects more than a single bit in the Status Register, the returned result 
 * is logical OR of the actual logical states of the selected bits.
 *
 * @param   base DryIce module base pointer.
 * @param   flag  Selects the flag bit(s) to be read. 
 * @return  In case a single bit is selected, returns TRUE if the bit is set or FALSE if the bit is clear.
 * @return  In case multiple bits are selected, returns result of logical OR of get single bit status of all selected bits.
 */ 
static inline bool DRY_HAL_GetStatusFlag(DRY_Type * base, hal_dry_status_flag_t flag)
{
    return (bool) (((uint32_t)flag & kDryStatus_All) & DRY_SR_REG(base));
}

/*!
 * @brief Gets the DryIce Status Register bit(s).
 * 
 * Reads the status flag(s) (specified by the specifier) from the DryIce Status Register.
 * In the specifier argument, pass a bit mask of the Status Register bits to be read. The output is written to destination
 * as the value of the DryIce Status Register masked by the specifier argument.
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Bit mask for the Status Register read. Pass kDryStatus_All to mask all Status Register bits.
 * @param   dest Masked Status register.
 * @return  Return zero if successful, return -1 if specified flag bit does not exist.   
 */
int32_t DRY_HAL_GetStatusFlagMasked(DRY_Type * base, const hal_dry_status_flag_t * specifier, hal_dry_status_flag_t * dest);

/*!
 * @brief Clears the DryIce Status Register bit(s).
 * 
 * Clears the status flags in the DryIce Status Register.
 * The function clears the specified (by the flag argument) bit or bits in the DryIce Status Register.
 *
 * @param   base DryIce module base pointer.
 * @param   flag  Selects the flag bit(s) to be cleared. Pass kDryStatus_All to clear all Status Register bits.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_ClearStatusFlag(DRY_Type * base, hal_dry_status_flag_t flag);

/*@}*/ 

/*!
 * @brief Gets the DryIce Secure Key Valid Register bit.
 * 
 * Reads the Secure Key Valid Register and returns sets/clears logical state of the secure key valid bit for the given Secure Key Register. 
 * If the key_register argument selects more than a single bit in the Secure Key Valid Register, the returned result 
 * is logical OR of the actual logical states of the selected bits in the Secure Key Valid Register.
 *
 * @param   base DryIce module base pointer.
 * @param   key_register  Selects the bit(s) in the Secure Key Valid Register to be read. 
 * @return  In case a single bit is selected, returns TRUE if the bit is set or FALSE if the bit is clear.
 * @return  In case multiple bits are selected, returns result of logical OR of get single bit state of all selected bits.
 */
static inline bool DRY_HAL_GetSecureKeyValid(DRY_Type * base, hal_dry_secure_key_reg_selector_t key_register)
{
    return (bool) ((((uint32_t)key_register) & kDrySecureKeyReg_All) & DRY_SKVR_REG(base));
}

/*!
 * @brief Gets the DryIce Secure Key Valid Register bit(s).
 * 
 * Reads bits (specified by the specifier) from the DryIce Secure Key Valid Register.
 * In the specifier argument, pass a bit mask of the Secure Key Valid Register bits to be read. The output is written to destination
 * as the value of the DryIce Secure Key Valid Register masked by the specifier argument.
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Bit mask for the Secure Key Valid Register read. Pass kDrySecureKeyReg_All to mask all Secure Key Valid Register bits.
 * @param   dest Masked Secure Key Valid register.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.   
 */
int32_t DRY_HAL_GetSecureKeyValidMasked(DRY_Type * base, const hal_dry_secure_key_reg_selector_t * specifier, hal_dry_secure_key_reg_selector_t * dest);

/*!
 * @brief Clears the DryIce Secure Key Valid Register bit(s).
 * 
 * Clears bit(s) in DryIce Secure Key Valid Register.
 * The function clears the specified (by the key_register argument) bit or bits in the DryIce Secure Key Valid Register.
 *
 * @param   base DryIce module base pointer.
 * @param   key_register  Selects the bit(s) to be cleared. Pass kDrySecureKeyReg_All to clear all Secure Key Valid Register bits. 
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_ClrSecureKeyValid(DRY_Type * base, hal_dry_secure_key_reg_selector_t key_register);

/*!
 * @brief Gets the DryIce Secure Key Write Lock Register bit.
 * 
 * Reads the Secure Key Write Lock Register and return set/clear logical state of the secure key write lock bit for the given Secure Key Register. 
 * If the key_register argument selects more than a single bit in the Secure Key Write Lock Register, the returned result 
 * is logical OR of the actual logical states of the selected bits in the Secure Key Write Lock Register.
 *
 * If a lock bit is set, the corresponding Secure Key Register can be written.
 *
 * @param   base DryIce module base pointer.
 * @param   key_register  Selects the bit(s) in the Secure Key Write Lock Register to be read. 
 * @return  In case a single bit is selected, returns TRUE if the bit is set or FALSE if the bit is clear.
 * @return  In case multiple bits are selected, returns result of logical OR of get single bit state of all selected bits.
 */
static inline bool DRY_HAL_GetSecureKeyWriteLock(DRY_Type * base, hal_dry_secure_key_reg_selector_t key_register)
{
    return (bool) ((((uint32_t)key_register) & kDrySecureKeyReg_All) & DRY_SKWLR_REG(base));
}

/*!
 * @brief Gets the DryIce Secure Key Write Lock Register bit(s).
 * 
 * Reads bits (specified by specifier) from the DryIce Secure Key Write Lock Register.
 * In the specifier argument, pass a bit mask of the Secure Key Write Lock Register bits to be read. The output is written to destination
 * as the value of the DryIce Secure Key Write Lock Register masked by the specifier argument.
 *
 * If a lock bit is set, the corresponding Secure Key Register can be written.
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Bit mask for the Secure Key Write Lock Register read. Pass kDrySecureKeyReg_All to mask all Secure Key Write Lock Register bits.
 * @param   dest Masked Secure Key Write lock register.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.   
 */
int32_t DRY_HAL_GetSecureKeyWriteLockMasked(DRY_Type * base, const hal_dry_secure_key_reg_selector_t * specifier, hal_dry_secure_key_reg_selector_t * dest);

/*!
 * @brief Locks the DryIce Secure Key Register(s).
 * 
 * Clears bit(s) in the DryIce Secure Key Write Lock Register.
 * The function clears the specified (by the key_register argument) bit or bits in the DryIce Secure Key Write Lock Register.
 * When a bit is cleared, the corresponding Secure Key Register is locked and cannot be written to by software.
 *
 * @param   base DryIce module base pointer.
 * @param   key_register  Selects the bit(s) to be cleared. Pass kDrySecureKeyReg_All to clear all Secure Key Write Lock Register bits. 
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_LockSecureKeyWrite(DRY_Type * base, hal_dry_secure_key_reg_selector_t key_register);

/*!
 * @brief Gets the DryIce Secure Key Read Lock Register bit.
 * 
 * Reads the Secure Key Read Lock Register and return set/clear logical state of the secure key read lock bit for the given Secure Key Register. 
 * If the key_register argument selects more than a single bit in the Secure Key Read Lock Register, the returned result 
 * is logical OR of the actual logical states of the selected bits in the Secure Key Read Lock Register.
 *
 * @param   base DryIce module base pointer.
 * @param   key_register  Selects the bit(s) in the Secure Key Read Lock Register to be read. 
 * @return  In case a single bit is selected, returns TRUE if the bit is set or FALSE if the bit is clear.
 * @return  In case multiple bits are selected, returns result of logical OR of get single bit state of all selected bits.
 */
static inline bool DRY_HAL_GetSecureKeyReadLock(DRY_Type * base, hal_dry_secure_key_reg_selector_t key_register)
{
    return (bool) ((((uint32_t)key_register) & kDrySecureKeyReg_All) & DRY_SKRLR_REG(base));
}

/*!
 * @brief Gets the DryIce Secure Key Read Lock Register bit(s).
 * 
 * Reads bits (specified by specifier) from DryIce Secure Key Read Lock Register.
 * In the specifier argument, pass a bit mask of the Secure Key Read Lock Register bits to be read. The output is written to destination
 * as the value of the DryIce Secure Key Read Lock Register masked by the specifier argument.
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Bit mask for the Secure Key Read Lock Register read. Pass kDrySecureKeyReg_All to mask all Secure Key Read Lock Register bits.
 * @param   dest Masked Secure Key Read lock register.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.   
 */
int32_t DRY_HAL_GetSecureKeyReadLockMasked(DRY_Type * base, const hal_dry_secure_key_reg_selector_t * specifier, hal_dry_secure_key_reg_selector_t * dest);

/*!
 * @brief Locks the DryIce Secure Key Register(s).
 * 
 * Clears bit(s) in the DryIce Secure Key Read Lock Register.
 * The function clears the specified (by the key_register argument) bit or bits in the DryIce Secure Key Read Lock Register.
 * When a bit is cleared, the corresponding Secure Key Register is locked and cannot be read by software.
 *
 * @param   base DryIce module base pointer.
 * @param   key_register  Selects the bit(s) to be cleared. Pass kDrySecureKeyReg_All to clear all Secure Key Read Lock Register bits. 
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_LockSecureKeyRead(DRY_Type * base, hal_dry_secure_key_reg_selector_t key_register);
 
/*!
 * @brief Gets the DryIce Lock Register bit.
 * 
 * Reads the Lock Register and returns set/clear logical state of the lock bit for the given DryIce Register. 
 * If the lock argument selects more than a single bit in the DryIce Lock Register, the returned result is logical OR 
 * of the actual logic states of the selected bits in the Lock Register.
 * If a bit is set in the Lock Register, the corresponding register can be written to (unlocked).
 *
 * @param   base DryIce module base pointer.
 * @param   lock Selects the bit(s) in the DryIce Lock Register to be read. 
 * @return  In case a single bit is selected, returns TRUE if the bit is set or FALSE if the bit is clear.
 * @return  In case multiple bits are selected, returns result of logical OR of get single bit state of all selected bits.
 */
static inline bool DRY_HAL_GetRegisterLockStatus(DRY_Type * base, hal_dry_reg_t lock)
{
    return (bool) (((uint32_t)lock & kDryReg_All) & DRY_LR_REG(base));
}

/*!
 * @brief Gets the DryIce Lock Register bit(s).
 * 
 * Reads bits (specified by the specifier) from the DryIce Lock Register.
 * In the specifier argument, pass a bit mask of the Lock Register bits to be read. The output is written to destination
 * as the value of the DryIce Lock Register masked by the specifier argument.
 * If a bit is set in the Lock Register, the corresponding register can be written (unlocked).
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Bit mask for the Lock Register read. Pass kDryReg_All to mask all Lock Register bits.
 * @param   dest Masked lock register (out).
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.   
 */
int32_t DRY_HAL_GetRegisterLockStatusMasked(DRY_Type * base, const hal_dry_reg_t * specifier, hal_dry_reg_t * dest);

/*!
 * @brief Locks the DryIce Register(s).
 * 
 * Clears bit(s) in the DryIce Lock Register.
 * The function clears the specified (by the lock argument) bit or bits in the DryIce Lock Register.
 * When a bit is cleared, the corresponding DryIce Register is locked and writes are ignored.
 * After it is cleared, the bits can only be set by VBAT POR or software reset.
 *
 * @param   base DryIce module base pointer.
 * @param   lock Selects the bit(s) to be cleared. Pass kDryReg_All to clear all Lock Register bits. 
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_LockRegister(DRY_Type * base, hal_dry_reg_t lock);

/*!
 * @brief Reads the DryIce Interrupt Enable Register.
 * 
 * Reads the Interrupt Enable Register and returns set/clear logical state of the interrupt enable bit for the interrupt source. 
 * If the source argument selects more than a single bit in the DryIce Interrupt Enable Register, the returned result is logical OR 
 * of the actual logic states of the selected bits in the Interrupt Enable Register.
 *
 * @param   base DryIce module base pointer.
 * @param   source Selects the bit(s) in the DryIce Interrupt Enable Register to be read. 
 * @return  In case a single bit is selected, returns TRUE if the bit is set or FALSE if the bit is clear.
 * @return  In case multiple bits are selected, returns result of logical OR of get single bit state of all selected bits.
 */
static inline bool DRY_HAL_GetInterruptEnable(DRY_Type * base, hal_dry_interrupt_t source)
{
    return (bool) (((uint32_t)source & kDryInterrupt_All) & DRY_IER_REG(base));
}

/*!
 * @brief Gets the DryIce Interrupt Enable Register bit(s).
 * 
 * Reads bits (specified by specifier) from the DryIce Interrupt Enable Register.
 * In the specifier argument, pass a bit mask of the Interrupt Enable Register bits to be read. The output is written to destination
 * as the value of the DryIce Interrupt Enable Register masked by the specifier argument.
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Bit mask for the Interrupt Enable Register read. Pass kDryInterrupt_All to mask all Interrupt Enable Register bits.
 * @param   dest Masked Interrupt Enable register (out).
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.   
 */
int32_t DRY_HAL_GetInterruptEnableMasked(DRY_Type * base, const hal_dry_interrupt_t * specifier, hal_dry_interrupt_t * dest);

/*!
 * @brief Enables the DryIce Interrupt.
 * 
 * Sets bit(s) in the DryIce Interrupt Enable Register.
 * The function sets the specified (by the source argument) bit or bits in the DryIce Interrupt Enable Register.
 * When a bit is set, the corresponding DryIce Interrupt source is enabled to generate an interrupt when the corresponding
 * status flag(s) is(are) set.
 *
 * @param   base DryIce module base pointer.
 * @param   source Selects the bit(s) to be set. Pass kDryInterrupt_All to set all Interrupt Enable Register bits. 
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_EnableInterrupt(DRY_Type * base, hal_dry_interrupt_t source);

/*!
 * @brief Disables the DryIce Interrupt.
 * 
 * Clears bit(s) in the DryIce Interrupt Enable Register.
 * The function clears the specified (by the source argument) bit or bits in the DryIce Interrupt Enable Register.
 * When a bit is clear, the corresponding DryIce Interrupt source is disabled to generate an interrupt when the corresponding
 * status flag(s) is(are) set.
 *
 * @param   base DryIce module base pointer.
 * @param   source Selects the bit(s) to be clear. Pass kDryInterrupt_All to clear all Interrupt Enable Register bits. 
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_DisableInterrupt(DRY_Type * base, hal_dry_interrupt_t source);

/*!
 * @brief Reads the DryIce Tamper Seconds Register.
 * 
 * Returns the Tamper Seconds Register value.
 *
 * @param   base DryIce module base pointer.
 * @return  Read value of Tamper Seconds Register.
 */
static inline uint32_t DRY_HAL_GetTamperSeconds(DRY_Type * base)
{
    return DRY_TSR_REG(base);
}

/*!
 * @brief Reads the DryIce Tamper Enable Register.
 * 
 * Reads the Tamper Enable Register and returns set/clear logical state of the tamper enable bit for the tamper source. 
 * If the tamper argument selects more than a single bit in the DryIce Tamper Enable Register, the returned result is logical OR 
 * of the actual logic states of the selected bits in the Tamper Enable Register.
 *
 * @param   base DryIce module base pointer.
 * @param   tamper Selects the bit(s) in the DryIce Tamper Enable Register to be read. 
 * @return  In case a single bit is selected, returns TRUE if the bit is set or FALSE if the bit is clear.
 * @return  In case multiple bits are selected, returns result of logical OR of get single bit state of all selected bits.
 */
static inline bool DRY_HAL_GetTamperEnable(DRY_Type * base, hal_dry_tamper_t tamper)
{
    return (bool) (((uint32_t)tamper & kDryTamper_All) & DRY_TER_REG(base));
}

/*!
 * @brief Gets the DryIce Tamper Enable Register bit(s).
 * 
 * Reads bits (specified by specifier) from the DryIce Tamper Enable Register.
 * In the specifier argument, pass a bit mask of the Tamper Enable Register bits to be read. The output is written to destination
 * as the value of the DryIce Tamper Enable Register masked by the specifier argument.
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Bit mask for the Tamper Enable Register read. Pass kDryTamper_All to mask all Tamper Enable Register bits.
 * @param   dest Masked Tamper Enable register (out).
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.   
 */
int32_t DRY_HAL_GetTamperEnableMasked(DRY_Type * base, const hal_dry_tamper_t * specifier, hal_dry_tamper_t * dest);

/*!
 * @brief Enables the DryIce Tamper.
 * 
 * Sets bit(s) in the DryIce Tamper Enable Register.
 * The function sets the specified (by the tamper argument) bit or bits in the DryIce Tamper Enable Register.
 * When a bit is set, the corresponding DryIce Tamper source is enabled to generate the tamper detected event when the corresponding
 * status flag(s) is(are) set.
 *
 * @param   base DryIce module base pointer.
 * @param   tamper Selects the bit(s) to be set. Pass kDryTamper_All to select all Tamper Enable Register bits. 
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_EnableTamper(DRY_Type * base, hal_dry_tamper_t tamper);

/*!
 * @brief Disables the DryIce Tamper.
 * 
 * Clears bit(s) in the DryIce Tamper Enable Register.
 * The function clears the specified (by the tamper argument) bit or bits in the DryIce Tamper Enable Register.
 * When a bit is clear, the corresponding DryIce Tamper source is disabled to generate the tamper detected event when the corresponding
 * status flag(s) is(are) set.
 *
 * @param   base DryIce module base pointer.
 * @param   tamper Selects the bit(s) to be clear. Pass kDryTamper_All to select all Tamper Enable Register bits. 
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_DisableTamper(DRY_Type * base, hal_dry_tamper_t tamper);

/*!
 * @brief Reads the DryIce Pin Direction Register.
 * 
 * Returns the tamper pin output logic zero (FALSE) or logic one (TRUE).
 * If the pin argument selects more than a single tamper pin, the returned result is logical OR
 * of the selected tamper pin outputs.
 *
 * @param   base DryIce module base pointer.
 * @param   pin Selects the tamper pin to be read. 
 * @return  FALSE if the tamper pin output data is logic zero.
 * @return  TRUE if the tamper pin output data is logic one.
 * @return  logical OR of the selected tamper pin outputs.
 */
 /*!
 * @brief Reads the DryIce Pin Direction Register.
 * 
 * Returns the tamper pin output data as a logical zero (FALSE) or logical one (TRUE).
 * If the tamper pin 0 output is a logical one, the kDryTamperPin_0 is set in the destination word.
 * If the tamper pin n output is a logical one, kDryTamperPin_n is set in the destination word.
 *
 * @param   base DryIce module base pointer.
 * @param   pin Selects the tamper pin or pins to be read. 
 * @param   dest Destination for the TPOD field masked by pin, right justified.
 * @return  -1 if selected pin does not exists, 0 otherwise.
 */
int32_t DRY_HAL_GetTamperPinOutput(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_tamper_pin_selector_t *dest);

/*!
 * @brief Writes the DryIce Pin Direction Register.
 * 
 * Sets the tamper pin direction.
 * When the tamper pin is output, it drives an inverse of the expected value (the tamper pin is asserted).
 *
 * @param   base DryIce module base pointer.
 * @param   pin Selects the tamper pin(s) (one or multiple) to be configured.
 *               Multiple pins can be selected as a bit mask, example: 
 *               pin = (hal_dry_tamper_pin_selector_t) (kDryTamperPin_1 | kDryTamperPin_5);
 *               Pass kDryTamperPin_All to select all pins. 
 * @param   direction The tamper pin direction to configure: kDryTamperPinDirection_In or kDryTamperPinDirection_Out.
 */
void DRY_HAL_SetTamperPinDirection(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_pin_direction_t direction);

/*!
 * @brief Reads the DryIce Pin Direction Register.
 * 
 * Gets the tamper pin direction.
 *
 * @param   base DryIce module base pointer.
 * @param   pin Selects the tamper pin(s) (one or multiple) to be read. 
 * @param   dest Destination where the Pin Direction for each selected pin is written.
 * @return  -1 if selected pin does not exists, 0 otherwise.
 */
int32_t DRY_HAL_GetTamperPinDirection(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_tamper_pin_selector_t *dest);

/*!
 * @brief Reads the DryIce Pin Polarity Register.
 * 
 * Returns the tamper pin input data (before glitch filter) as a logical zero (FALSE) or logical one (TRUE).
 * If the tamper pin 0 input data is logical one, the kDryTamperPin_0 is set in the destination word.
 * If the tamper pin n input data is logical one, the kDryTamperPin_n is set in the destination word.
 *
 * @param   base DryIce module base pointer.
 * @param   pin Selects the tamper pin or pins to be read. 
 * @param   dest Destination for the TPID field masked by pin, right justified.
 * @return  -1 if selected pin does not exists, 0 otherwise.
 */
 int32_t DRY_HAL_GetTamperPinInput(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_tamper_pin_selector_t *dest);

/*!
 * @brief Writes the DryIce Pin Polarity Register.
 * 
 * Sets the tamper pin polarity.
 * kDryTamperPinPolarity_ExpectValNormal for the tamper pin expected value is not inverted.
 * kDryTamperPinPolarity_ExpectValInverted for tamper pin expected value is inverted.
 *
 * @param   base DryIce module base pointer.
 * @param   pin Selects the tamper pin(s) (one or multiple) to be configured.
 * @param   polarity The tamper pin polarity to configure: kDryTamperPinPolarity_ExpectValNormal or kDryTamperPinPolarity_ExpectValInverted
 */
void DRY_HAL_SetTamperPinPolarity(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_pin_polarity_t polarity);

/*!
 * @brief Reads the DryIce Pin Polarity Register.
 * 
 * Gets the tamper pin polarity.
 *
 * @param   base DryIce module base pointer.
 * @param   pin Selects the tamper pin(s) (one or multiple) to be read. 
 * @param   dest Destination where the Pin Polarity for each selected pin will be written.
 * @return  -1 if selected pin does not exists, 0 otherwise.
 */
int32_t DRY_HAL_GetTamperPinPolarity(DRY_Type * base, const hal_dry_tamper_pin_selector_t *pin, hal_dry_tamper_pin_selector_t *dest);

/*!
 * @brief Reads the DryIce Active Tamper Register.
 * 
 * Returns the Active Tamper Polynomial from the Active Tamper Register 0 or 1.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Selects the Active Tamper Register to be read. One or multiple registers can be selected.
 * @param   dest Output where Active Tamper Polynomial is written. If multiple registers are selected, it must be a pointer to an array of uint16_t short words.
 * @param   num Number of uint16_t elements in the dest array.
 *              Pass 1 if single register is selected by reg_name argument.
 * @return  -1 if None Active Tamper Register is selected, 0 otherwise.
 */
int32_t DRY_HAL_GetActiveTamperPolynomial(DRY_Type * base, const hal_dry_act_tamper_selector_t *reg_name, uint16_t *dest, int32_t num);

/*!
 * @brief Reads the DryIce Active Tamper Register.
 * 
 * Returns the Active Tamper Shift Register for the Active Tamper Register 0 or 1.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Selects the Active Tamper Register to be read. One or multiple registers can be selected.
 * @param   dest Output where Active Tamper Shift is written. If multiple registers are selected, it must be a pointer to an array of uint16_t short words.
 * @param   num Number of uint16_t elements in the destination array.
 *              Pass 1 if single register is selected by reg_name argument.
 * @return  -1 If none Active Tamper Registers is selected, 0 otherwise.
 */
int32_t DRY_HAL_GetActiveTamperShift(DRY_Type * base, const hal_dry_act_tamper_selector_t *reg_name, uint16_t *dest, int32_t num);

/*!
 * @brief Writes the DryIce Active Tamper Register.
 * 
 * Sets the Active Tamper Polynomial in the Active Tamper Register 0 or 1.
 *
 * @param   base DryIce module base pointer.
 * @param   reg Selects the Active Tamper Register to be written. Pass kDryActiveTamperReg_All to select all Active Tamper Registers.
 * @param   polynomial Active Tamper Polynomial.
 */
int32_t DRY_HAL_SetActiveTamperPolynomial(DRY_Type * base, const hal_dry_act_tamper_selector_t reg_name, uint16_t polynomial);

/*!
 * @brief Writes the DryIce Active Tamper Register.
 * 
 * Sets the Active Tamper Shift Register in the Active Tamper Register 0 or 1.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Selects the Active Tamper Register to be written. Pass kDryActiveTamperReg_All to select all Active Tamper Registers.
 * @param   shift Active Tamper Shift Register.
 */
int32_t DRY_HAL_SetActiveTamperShift(DRY_Type * base, const hal_dry_act_tamper_selector_t reg_name, uint16_t shift);

/*!
 * @brief Disables the DryIce Active Tamper.
 * 
 * Clears the Active Tamper Polynomial in the Active Tamper Register 0 or 1. 
 * When polynomial is set to zero, the active tamper shift register is disabled.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Selects the Active Tamper Register to be disabled. Pass kDryActiveTamperReg_All to select all Active Tamper Registers.
 */
static inline int32_t DRY_HAL_DisableActiveTamper(DRY_Type * base, const hal_dry_act_tamper_selector_t reg_name)
{
    return DRY_HAL_SetActiveTamperPolynomial(base, reg_name, ((uint16_t)0x0U));
}

/*!
 * @name DryIce Access Control Registers
 * @{
 */

/*!
 * @brief Reads the DryIce Write Access Control Register.
 * 
 * Returns the write access status from the DryIce Write Access Control Register. 
 * Returns TRUE if register, specified by reg_name, can be written. Returns FALSE if writes to register, specified by reg_name, are ignored.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Select the DryIce access control register to be read.
 * @return  TRUE if writes to specified register complete as normal.
 * @return  FALSE if writes to specified register are ignored.
 * @return  logical OR of specifed register bits if multiple bits are selected in reg_name.
 */
static inline bool DRY_HAL_GetRegisterWriteAccess(DRY_Type * base, hal_dry_reg_t reg_name)
{
    return (bool)(((uint32_t)reg_name & kDryReg_All) & DRY_WAC_REG(base));
}

/*!
 * @brief Reads the DryIce Write Access Control Register.
 * 
 * Returns the write access status from the DryIce Write Access Control Register. 
 * Returns the DryIce Write Access Control Register masked by the specifier argument.
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Select the DryIce registers to be read.
 * @param   dest Destination for the masked register output.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise..
 */
int32_t DRY_HAL_GetRegisterWriteAccessMasked(DRY_Type * base, const hal_dry_reg_t * specifier, hal_dry_reg_t * dest);

/*!
 * @brief Writes the DryIce Write Access Control Register.
 * 
 * Configures a write access status in the DryIce Write Access Control Register to writes ignored (clear specified bits).
 * Reset value of register bits is 0b1. After it is cleared, the bits can only be set by chip reset.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Select the DryIce access control register bit or bits to be cleared.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_SetIgnoreRegisterWrite(DRY_Type * base, hal_dry_reg_t reg_name);

/*!
 * @brief Reads the DryIce Read Access Control Register.
 * 
 * Returns a read access status from the DryIce Read Access Control Register. 
 * Returns TRUE if register, specified by reg_name, can be read. Returns FALSE if reads from register, specified by reg_name, are ignored.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Select the DryIce access control register.
 * @return  TRUE if reads from specified register complete as normal.
 * @return  FALSE if reads from specified register are ignored.
 * @return  logical OR of specified register bits if multiple bits are selected in reg_name.
 */
static inline bool DRY_HAL_GetRegisterReadAccess(DRY_Type * base, hal_dry_reg_t reg_name)
{
    return (bool)(((uint32_t)reg_name & kDryReg_All) & DRY_RAC_REG(base));
}

/*!
 * @brief Reads the DryIce Read Access Control Register.
 * 
 * Returns a read access status from the DryIce Read Access Control Register. 
 * Returns DryIce Read Access Control Register masked by the specifier argument.
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Select the DryIce registers to read.
 * @param   dest Destination for the masked register output.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_GetRegisterReadAccessMasked(DRY_Type * base, const hal_dry_reg_t * specifier, hal_dry_reg_t * dest);

/*!
 * @brief Writes the DryIce Read Access Control Register.
 * 
 * Configures a read access status in DryIce Read Access Control Register to reads ignored (clear specified bits).
 * Reset value of register bits is 0b1. After it is cleared, the bits can only be set by chip reset.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Select the DryIce access control register bit or bits to be cleared.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_SetIgnoreRegisterRead(DRY_Type * base, hal_dry_reg_t reg_name);

/*!
 * @brief Reads the DryIce Secure Write Access Control Register.
 * 
 * Returns a write access status from the DryIce Secure Write Access Control Register. 
 * Returns TRUE if register, specified by reg_name, can be written. Returns FALSE if writes to register, specified by reg_name, are ignored.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Select the DryIce secure key register to be read.
 * @return  TRUE if writes to specified register complete as normal.
 * @return  FALSE if writes to specified register are ignored.
 * @return  logical OR of specifed register bits if multiple bits are selected in reg_name.
 */
static inline bool DRY_HAL_GetSecureWriteAccess(DRY_Type * base, hal_dry_secure_key_reg_selector_t reg_name)
{
    return (bool)(((uint32_t)reg_name & kDrySecureKeyReg_All) & DRY_SWAC_REG(base));
}

/*!
 * @brief Reads the DryIce Secure Write Access Control Register.
 * 
 * Return a write access status from the DryIce Secure Write Access Control Register. 
 * Returns DryIce Secure Write Access Control Register masked by the specifier argument.
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Select the DryIce secure key registers to be read.
 * @param   dest Destination for the masked register output.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_GetSecureWriteAccessMasked(DRY_Type * base, const hal_dry_secure_key_reg_selector_t * specifier, hal_dry_secure_key_reg_selector_t * dest);

/*!
 * @brief Writes the DryIce Secure Write Access Control Register.
 * 
 * Configures the write access status in the DryIce Secure Write Access Control Register to writes ignored (clear specified bits).
 * Reset value of register bits is 0b1. Once cleared, the bits can only be set by chip reset.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Select the DryIce Secure Write Access Control register bit or bits to clear.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_SetIgnoreSecureWrite(DRY_Type * base, hal_dry_secure_key_reg_selector_t reg_name);

/*!
 * @brief Reads the DryIce Secure Read Access Control Register.
 * 
 * Returns a read access status from the DryIce Secure Read Access Control Register. 
 * Returns TRUE if register, specified by reg_name, can be read. Returns FALSE if reads from register, specified by reg_name, are ignored.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Select the DryIce secure key register to be read.
 * @return  TRUE reads from writes to specified register complete as normal.
 * @return  FALSE if reads from to specified register are ignored.
 * @return  logical OR of specifed register bits if multiple bits are selected in reg_name.
 */
static inline bool DRY_HAL_GetSecureReadAccess(DRY_Type * base, hal_dry_secure_key_reg_selector_t reg_name)
{
    return (bool)((((uint32_t)reg_name) & kDrySecureKeyReg_All) & DRY_SRAC_REG(base));
}

/*!
 * @brief Reads the DryIce Secure Read Access Control Register.
 * 
 * Returns a read access status from the DryIce Secure Read Access Control Register. 
 * Returns DryIce Secure Read Access Control Register masked by the specifier argument.
 *
 * @param   base DryIce module base pointer.
 * @param   specifier Select the DryIce secure key registers to be read.
 * @param   dest Destination for the masked register output.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_GetSecureReadAccessMasked(DRY_Type * base, const hal_dry_secure_key_reg_selector_t * specifier, hal_dry_secure_key_reg_selector_t * dest);

/*!
 * @brief Writes the DryIce Secure Read Access Control Register.
 * 
 * Configures a read access status in the DryIce Secure Read Access Control Register to reads ignored (clear specified bits).
 * Reset value of register bits is 0b1. Once cleared, the bits can only be set by chip reset.
 *
 * @param   base DryIce module base pointer.
 * @param   reg_name Select the DryIce Secure Read Access Control register bit or bits to clear.
 * @return  Return -1 if specified bit does not exist in the register, return 0 otherwise.
 */
int32_t DRY_HAL_SetIgnoreSecureRead(DRY_Type * base, hal_dry_secure_key_reg_selector_t reg_name);

/*@}*/

/*!
 * @brief Writes the DryIce Secure Key Register.
 * 
 * Writes a 32-bit key to the Secure Key Register. 
 *
 * @param  base DryIce module base pointer.
 * @param  reg_name Select Secure Key Register.
 * @param  key array of 32-bit values to be written to selected Secure Key Registers.
 *             For each set bit in reg_name argument (Secure Key Register selector), the corresponding member of key[] array is read 
 *             and the read value is written to the corresponding Secure Key Register. 
 *             To write all Secure Key Registers in one function call, user can call:
 *             DRY_HAL_SetSecureKey(base, kDrySecureKeyReg_All, key, DRY_SKR_COUNT);
 *             where key is declared as int32_t key[DRY_SKR_COUNT];
 * @param  num Number of elements in key[] array.
 * @return  Return -1 if register does not exist, return 0 otherwise.
 */
int32_t DRY_HAL_SetSecureKey(DRY_Type * base, hal_dry_secure_key_reg_selector_t reg_name, int32_t *key, int32_t num);

/*!
 * @brief Reads the DryIce Secure Key Register.
 * 
 * Read 32-bit value from Secure Key Register.
 *
 * @param  base DryIce module base pointer.
 * @param  reg_name Select Secure Key Register.
 * @param  key Destination pointer where the read value will be written to.
 *             For each set bit in reg_name argument (Secure Key Register selector), the corresponding Secure Key Register is read
 *             and the read value is written to memory location specified by key argument. 
 *             To read all Secure Key Registers in one function call, user can call:
 *             DRY_HAL_GetSecureKey(base, kDrySecureKeyReg_All, key, DRY_SKR_COUNT);
 *             where key is declared as int32_t key[DRY_SKR_COUNT];
 * @param  num Number of elements in key[] array.
 * @return  Return -1 if register does not exist, return 0 otherwise.
 */
int32_t DRY_HAL_GetSecureKey(DRY_Type * base, hal_dry_secure_key_reg_selector_t reg_name, int32_t *key, int32_t num);

/*!
 * @brief Tests whether Lock and Write Access Register allows for writing.
 * 
 * This function allows the user to test whether writing to a specified register(s) is possible
 * due to the write access control and register lock restrictions. 
 *
 * @param  base DryIce module base pointer.
 * @param  reg_name Select DryIce module register or registers to check whether the register can be written to.
 * @return true Registers specified by the reg_name can be written.
 * @return false Registers specified by the reg_name cannot be written.
 */
bool DRY_HAL_IsRegWritePossible(DRY_Type *base, const hal_dry_reg_t reg_name);

/*!
 * @brief Tests whether the Read Access Register allows for reading.
 * 
 * This function allows the user to test whether reading of the specified register(s) is possible
 * due to the read access control. 
 *
 * @param  base DryIce module base pointer.
 * @param  reg_name Select DryIce module register or registers to check for readability.
 * @return true Registers specified by the reg_name can be read.
 * @return false Registers specified by the reg_name cannot be read.
 */
bool DRY_HAL_IsRegReadPossible(DRY_Type *base, const hal_dry_reg_t reg_name);

/*!
 * @brief Tests whether the Secure Key Write Lock and Secure Key Write Access allows for writing.
 * 
 * This function allows the user to test whether writing to the specified Secure Key register(s) is possible
 * due to the write access control and write register lock restrictions. 
 *
 * @param  base DryIce module base pointer.
 * @param  reg_name Select DryIce module register or registers to check whether the register can be written to.
 * @return true Registers specified by the reg_name can be written.
 * @return false Registers specified by the reg_name cannot be written.
 */
bool DRY_HAL_IsSecureKeyWritePossible(DRY_Type *base, const hal_dry_secure_key_reg_selector_t reg_name);

/*!
 * @brief Test whether the Secure Key Read Lock and Secure Key Read Access allows for reading.
 * 
 * This function allows the user to test whether reading of the specified Secure Key register(s) is possible
 * due to the read access control and read register lock restrictions. 
 *
 * @param  base DryIce module base pointer.
 * @param  reg_name Select DryIce module register or registers to check for readability.
 * @return true Registers specified by the reg_name can be read.
 * @return false Registers specified by the reg_name cannot be read.
 */
bool DRY_HAL_IsSecureKeyReadPossible(DRY_Type *base, const hal_dry_secure_key_reg_selector_t reg_name);

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* FSL_FEATURE_SOC_DRY_COUNT */

#endif /* __FSL_DRYICE_HAL_H__ */
/*******************************************************************************
 * EOF
 ******************************************************************************/
