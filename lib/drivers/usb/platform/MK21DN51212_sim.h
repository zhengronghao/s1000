/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL FREESCALE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */
/*
 * WARNING! DO NOT EDIT THIS FILE DIRECTLY!
 *
 * This file was generated automatically and any changes may be lost.
 */
#ifndef __HW_SIM_REGISTERS_H__
#define __HW_SIM_REGISTERS_H__

#include "regs.h"

/*
 * MK22F51212 SIM
 *
 * System Integration Module
 * Registers defined in this header file:
 */

//! @name Module base addresses
//@{
#ifndef REGS_SIM_BASE
#define HW_SIM_INSTANCE_COUNT (1U) //!< Number of instances of the SIM module.
#define REGS_SIM_BASE (0x40047000U) //!< Base address for SIM.
#endif
//@}

//-------------------------------------------------------------------------------------------
// HW_SIM_SOPT1 - System Options Register 1
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_SIM_SOPT1 - System Options Register 1 (RW)
 *
 * Reset value: 0x80000000U
 *
 * The SOPT1 register is only reset on POR or LVD.
 */
typedef union _hw_sim_sopt1
{
    uint32_t U;
    struct _hw_sim_sopt1_bitfields
    {
        uint32_t RESERVED0 : 12;       //!< [11:0]
        uint32_t RAMSIZE : 4;          //!< [15:12] RAM size
        uint32_t OSC32KOUT : 2;        //!< [17:16] 32K Oscillator Clock Output
        uint32_t OSC32KSEL : 2;        //!< [19:18] 32K oscillator clock select
        uint32_t RESERVED1 : 9;        //!< [28:20]
        uint32_t USBVSTBY : 1;         //!< [29] USB voltage regulator in standby
                                       //! mode during VLPR and VLPW modes
        uint32_t USBSSTBY : 1;         //!< [30] USB voltage regulator in standby
                                       //! mode during Stop, VLPS, LLS and VLLS modes.
        uint32_t USBREGEN : 1;         //!< [31] USB voltage regulator enable
    } B;
} hw_sim_sopt1_t;
#endif

/*!
 * @name Constants and macros for entire SIM_SOPT1 register
 */
//@{
#define HW_SIM_SOPT1_ADDR        (REGS_SIM_BASE + 0x0U)

#ifndef __LANGUAGE_ASM__
#define HW_SIM_SOPT1             (*(__IO hw_sim_sopt1_t *) HW_SIM_SOPT1_ADDR)
#define HW_SIM_SOPT1_RD()        (HW_SIM_SOPT1.U)
#define HW_SIM_SOPT1_WR(v)       (HW_SIM_SOPT1.U = (v))
#define HW_SIM_SOPT1_SET(v)      (HW_SIM_SOPT1_WR(HW_SIM_SOPT1_RD() |  (v)))
#define HW_SIM_SOPT1_CLR(v)      (HW_SIM_SOPT1_WR(HW_SIM_SOPT1_RD() & ~(v)))
#define HW_SIM_SOPT1_TOG(v)      (HW_SIM_SOPT1_WR(HW_SIM_SOPT1_RD() ^  (v)))
#endif
//@}

/*!
 * @name Register SIM_SOPT1, field USBVSTBY[29] (RW)
 *
 * Controls whether the USB voltage regulator is placed in standby mode during
 * VLPR and VLPW modes.
 *
 * Values:
 * - 0 - USB voltage regulator not in standby during VLPR and VLPW modes.
 * - 1 - USB voltage regulator in standby during VLPR and VLPW modes.
 */
//@{
#define BP_SIM_SOPT1_USBVSTBY (29U)        //!< Bit position for SIM_SOPT1_USBVSTBY.
#define BM_SIM_SOPT1_USBVSTBY (0x20000000U) //!< Bit mask for SIM_SOPT1_USBVSTBY.
#define BS_SIM_SOPT1_USBVSTBY (1U)         //!< Bit field size in bits for SIM_SOPT1_USBVSTBY.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the SIM_SOPT1_USBVSTBY field.
#define BR_SIM_SOPT1_USBVSTBY (BITBAND_ACCESS32(HW_SIM_SOPT1_ADDR, BP_SIM_SOPT1_USBVSTBY))
#endif

//! @brief Format value for bitfield SIM_SOPT1_USBVSTBY.
#define BF_SIM_SOPT1_USBVSTBY(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_SIM_SOPT1_USBVSTBY), uint32_t) & BM_SIM_SOPT1_USBVSTBY)

#ifndef __LANGUAGE_ASM__
//! @brief Set the USBVSTBY field to a new value.
#define BW_SIM_SOPT1_USBVSTBY(v) (BITBAND_ACCESS32(HW_SIM_SOPT1_ADDR, BP_SIM_SOPT1_USBVSTBY) = (v))
#endif
//@}

/*!
 * @name Register SIM_SOPT1, field USBSSTBY[30] (RW)
 *
 * Controls whether the USB voltage regulator is placed in standby mode during
 * Stop, VLPS, LLS and VLLS modes.
 *
 * Values:
 * - 0 - USB voltage regulator not in standby during Stop, VLPS, LLS and VLLS
 *     modes.
 * - 1 - USB voltage regulator in standby during Stop, VLPS, LLS and VLLS modes.
 */
//@{
#define BP_SIM_SOPT1_USBSSTBY (30U)        //!< Bit position for SIM_SOPT1_USBSSTBY.
#define BM_SIM_SOPT1_USBSSTBY (0x40000000U) //!< Bit mask for SIM_SOPT1_USBSSTBY.
#define BS_SIM_SOPT1_USBSSTBY (1U)         //!< Bit field size in bits for SIM_SOPT1_USBSSTBY.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the SIM_SOPT1_USBSSTBY field.
#define BR_SIM_SOPT1_USBSSTBY (BITBAND_ACCESS32(HW_SIM_SOPT1_ADDR, BP_SIM_SOPT1_USBSSTBY))
#endif

//! @brief Format value for bitfield SIM_SOPT1_USBSSTBY.
#define BF_SIM_SOPT1_USBSSTBY(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_SIM_SOPT1_USBSSTBY), uint32_t) & BM_SIM_SOPT1_USBSSTBY)

#ifndef __LANGUAGE_ASM__
//! @brief Set the USBSSTBY field to a new value.
#define BW_SIM_SOPT1_USBSSTBY(v) (BITBAND_ACCESS32(HW_SIM_SOPT1_ADDR, BP_SIM_SOPT1_USBSSTBY) = (v))
#endif
//@}

/*!
 * @name Register SIM_SOPT1, field USBREGEN[31] (RW)
 *
 * Controls whether the USB voltage regulator is enabled.
 *
 * Values:
 * - 0 - USB voltage regulator is disabled.
 * - 1 - USB voltage regulator is enabled.
 */
//@{
#define BP_SIM_SOPT1_USBREGEN (31U)        //!< Bit position for SIM_SOPT1_USBREGEN.
#define BM_SIM_SOPT1_USBREGEN (0x80000000U) //!< Bit mask for SIM_SOPT1_USBREGEN.
#define BS_SIM_SOPT1_USBREGEN (1U)         //!< Bit field size in bits for SIM_SOPT1_USBREGEN.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the SIM_SOPT1_USBREGEN field.
#define BR_SIM_SOPT1_USBREGEN (BITBAND_ACCESS32(HW_SIM_SOPT1_ADDR, BP_SIM_SOPT1_USBREGEN))
#endif

//! @brief Format value for bitfield SIM_SOPT1_USBREGEN.
#define BF_SIM_SOPT1_USBREGEN(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_SIM_SOPT1_USBREGEN), uint32_t) & BM_SIM_SOPT1_USBREGEN)

#ifndef __LANGUAGE_ASM__
//! @brief Set the USBREGEN field to a new value.
#define BW_SIM_SOPT1_USBREGEN(v) (BITBAND_ACCESS32(HW_SIM_SOPT1_ADDR, BP_SIM_SOPT1_USBREGEN) = (v))
#endif
//@}

//-------------------------------------------------------------------------------------------
// HW_SIM_SOPT1CFG - SOPT1 Configuration Register
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_SIM_SOPT1CFG - SOPT1 Configuration Register (RW)
 *
 * Reset value: 0x00000000U
 *
 * The SOPT1CFG register is reset on System Reset not VLLS.
 */
typedef union _hw_sim_sopt1cfg
{
    uint32_t U;
    struct _hw_sim_sopt1cfg_bitfields
    {
        uint32_t RESERVED0 : 24;       //!< [23:0]
        uint32_t URWE : 1;             //!< [24] USB voltage regulator enable write enable
        uint32_t UVSWE : 1;            //!< [25] USB voltage regulator VLP standby write
                                       //! enable
        uint32_t USSWE : 1;            //!< [26] USB voltage regulator stop standby
                                       //! write enable
        uint32_t RESERVED1 : 5;        //!< [31:27]
    } B;
} hw_sim_sopt1cfg_t;
#endif

/*!
 * @name Constants and macros for entire SIM_SOPT1CFG register
 */
//@{
#define HW_SIM_SOPT1CFG_ADDR     (REGS_SIM_BASE + 0x4U)

#ifndef __LANGUAGE_ASM__
#define HW_SIM_SOPT1CFG          (*(__IO hw_sim_sopt1cfg_t *) HW_SIM_SOPT1CFG_ADDR)
#define HW_SIM_SOPT1CFG_RD()     (HW_SIM_SOPT1CFG.U)
#define HW_SIM_SOPT1CFG_WR(v)    (HW_SIM_SOPT1CFG.U = (v))
#define HW_SIM_SOPT1CFG_SET(v)   (HW_SIM_SOPT1CFG_WR(HW_SIM_SOPT1CFG_RD() |  (v)))
#define HW_SIM_SOPT1CFG_CLR(v)   (HW_SIM_SOPT1CFG_WR(HW_SIM_SOPT1CFG_RD() & ~(v)))
#define HW_SIM_SOPT1CFG_TOG(v)   (HW_SIM_SOPT1CFG_WR(HW_SIM_SOPT1CFG_RD() ^  (v)))
#endif
//@}

/*
 * Constants & macros for individual SIM_SOPT1CFG bitfields
 */

/*!
 * @name Register SIM_SOPT1CFG, field URWE[24] (RW)
 *
 * Writing one to the URWE bit allows the SOPT1 USBREGEN bit to be written. This
 * register bit clears after a write to USBREGEN.
 *
 * Values:
 * - 0 - SOPT1 USBREGEN cannot be written.
 * - 1 - SOPT1 USBREGEN can be written.
 */
//@{
#define BP_SIM_SOPT1CFG_URWE (24U)         //!< Bit position for SIM_SOPT1CFG_URWE.
#define BM_SIM_SOPT1CFG_URWE (0x01000000U) //!< Bit mask for SIM_SOPT1CFG_URWE.
#define BS_SIM_SOPT1CFG_URWE (1U)          //!< Bit field size in bits for SIM_SOPT1CFG_URWE.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the SIM_SOPT1CFG_URWE field.
#define BR_SIM_SOPT1CFG_URWE (BITBAND_ACCESS32(HW_SIM_SOPT1CFG_ADDR, BP_SIM_SOPT1CFG_URWE))
#endif

//! @brief Format value for bitfield SIM_SOPT1CFG_URWE.
#define BF_SIM_SOPT1CFG_URWE(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_SIM_SOPT1CFG_URWE), uint32_t) & BM_SIM_SOPT1CFG_URWE)

#ifndef __LANGUAGE_ASM__
//! @brief Set the URWE field to a new value.
#define BW_SIM_SOPT1CFG_URWE(v) (BITBAND_ACCESS32(HW_SIM_SOPT1CFG_ADDR, BP_SIM_SOPT1CFG_URWE) = (v))
#endif
//@}


//-------------------------------------------------------------------------------------------
// HW_SIM_SOPT2 - System Options Register 2
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_SIM_SOPT2 - System Options Register 2 (RW)
 *
 * Reset value: 0x00001000U
 *
 * SOPT2 contains the controls for selecting many of the module clock source
 * options on this device. See the Clock Distribution chapter for more information
 * including clocking diagrams and definitions of device clocks.
 */
typedef union _hw_sim_sopt2
{
    uint32_t U;
    struct _hw_sim_sopt2_bitfields
    {
        uint32_t RESERVED0 : 4;        //!< [3:0]
        uint32_t RTCCLKOUTSEL : 1;     //!< [4] RTC clock out select
        uint32_t CLKOUTSEL : 3;        //!< [7:5] CLKOUT select
        uint32_t FBSL : 2;             //!< [9:8] FlexBus security level
        uint32_t RESERVED1 : 2;        //!< [11:10]
        uint32_t TRACECLKSEL : 1;      //!< [12] Debug trace clock select
        uint32_t RESERVED2 : 3;        //!< [15:13]
        uint32_t PLLFLLSEL : 2;        //!< [17:16] PLL/FLL clock select
        uint32_t USBSRC : 1;           //!< [18] USB clock source select
        uint32_t RESERVED3 : 7;        //!< [25:19]
        uint32_t LPUARTSRC : 2;        //!< [27:26] LPUART clock source select
        uint32_t RESERVED4 : 4;        //!< [31:28]
    } B;
} hw_sim_sopt2_t;
#endif

/*!
 * @name Constants and macros for entire SIM_SOPT2 register
 */
//@{
#define HW_SIM_SOPT2_ADDR        (REGS_SIM_BASE + 0x1004U)

#ifndef __LANGUAGE_ASM__
#define HW_SIM_SOPT2             (*(__IO hw_sim_sopt2_t *) HW_SIM_SOPT2_ADDR)
#define HW_SIM_SOPT2_RD()        (HW_SIM_SOPT2.U)
#define HW_SIM_SOPT2_WR(v)       (HW_SIM_SOPT2.U = (v))
#define HW_SIM_SOPT2_SET(v)      (HW_SIM_SOPT2_WR(HW_SIM_SOPT2_RD() |  (v)))
#define HW_SIM_SOPT2_CLR(v)      (HW_SIM_SOPT2_WR(HW_SIM_SOPT2_RD() & ~(v)))
#define HW_SIM_SOPT2_TOG(v)      (HW_SIM_SOPT2_WR(HW_SIM_SOPT2_RD() ^  (v)))
#endif

/*!
 * @name Register SIM_SOPT2, field USBSRC[18] (RW)
 *
 * Selects the clock source for the USB 48 MHz clock.
 *
 * Values:
 * - 0 - External bypass clock (USB_CLKIN).
 * - 1 - MCGFLLCLK , or MCGPLLCLK , or IRC48M clock as selected by
 *     SOPT2[PLLFLLSEL], and then divided by the USB fractional divider as configured by
 *     SIM_CLKDIV2[USBFRAC, USBDIV].
 */
//@{
#define BP_SIM_SOPT2_USBSRC  (18U)         //!< Bit position for SIM_SOPT2_USBSRC.
#define BM_SIM_SOPT2_USBSRC  (0x00040000U) //!< Bit mask for SIM_SOPT2_USBSRC.
#define BS_SIM_SOPT2_USBSRC  (1U)          //!< Bit field size in bits for SIM_SOPT2_USBSRC.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the SIM_SOPT2_USBSRC field.
#define BR_SIM_SOPT2_USBSRC  (BITBAND_ACCESS32(HW_SIM_SOPT2_ADDR, BP_SIM_SOPT2_USBSRC))
#endif

//! @brief Format value for bitfield SIM_SOPT2_USBSRC.
#define BF_SIM_SOPT2_USBSRC(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_SIM_SOPT2_USBSRC), uint32_t) & BM_SIM_SOPT2_USBSRC)

#ifndef __LANGUAGE_ASM__
//! @brief Set the USBSRC field to a new value.
#define BW_SIM_SOPT2_USBSRC(v) (BITBAND_ACCESS32(HW_SIM_SOPT2_ADDR, BP_SIM_SOPT2_USBSRC) = (v))
#endif
//@}

//-------------------------------------------------------------------------------------------
// HW_SIM_SCGC4 - System Clock Gating Control Register 4
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_SIM_SCGC4 - System Clock Gating Control Register 4 (RW)
 *
 * Reset value: 0xF0100030U
 */
typedef union _hw_sim_scgc4
{
    uint32_t U;
    struct _hw_sim_scgc4_bitfields
    {
        uint32_t RESERVED0 : 1;        //!< [0]
        uint32_t EWMb : 1;             //!< [1] EWM Clock Gate Control
        uint32_t RESERVED1 : 4;        //!< [5:2]
        uint32_t I2C0b : 1;            //!< [6] I2C0 Clock Gate Control
        uint32_t I2C1b : 1;            //!< [7] I2C1 Clock Gate Control
        uint32_t RESERVED2 : 2;        //!< [9:8]
        uint32_t UART0b : 1;           //!< [10] UART0 Clock Gate Control
        uint32_t UART1b : 1;           //!< [11] UART1 Clock Gate Control
        uint32_t UART2b : 1;           //!< [12] UART2 Clock Gate Control
        uint32_t RESERVED3 : 5;        //!< [17:13]
        uint32_t USBOTG : 1;           //!< [18] USB Clock Gate Control
        uint32_t CMP : 1;              //!< [19] Comparator Clock Gate Control
        uint32_t VREFb : 1;            //!< [20] VREF Clock Gate Control
        uint32_t RESERVED4 : 11;       //!< [31:21]
    } B;
} hw_sim_scgc4_t;
#endif

/*!
 * @name Constants and macros for entire SIM_SCGC4 register
 */
//@{
#define HW_SIM_SCGC4_ADDR        (REGS_SIM_BASE + 0x1034U)

#ifndef __LANGUAGE_ASM__
#define HW_SIM_SCGC4             (*(__IO hw_sim_scgc4_t *) HW_SIM_SCGC4_ADDR)
#define HW_SIM_SCGC4_RD()        (HW_SIM_SCGC4.U)
#define HW_SIM_SCGC4_WR(v)       (HW_SIM_SCGC4.U = (v))
#define HW_SIM_SCGC4_SET(v)      (HW_SIM_SCGC4_WR(HW_SIM_SCGC4_RD() |  (v)))
#define HW_SIM_SCGC4_CLR(v)      (HW_SIM_SCGC4_WR(HW_SIM_SCGC4_RD() & ~(v)))
#define HW_SIM_SCGC4_TOG(v)      (HW_SIM_SCGC4_WR(HW_SIM_SCGC4_RD() ^  (v)))
#endif
//@}

/*
 * Constants & macros for individual SIM_SCGC4 bitfields
 */
/*!
 * @name Register SIM_SCGC4, field USBOTG[18] (RW)
 *
 * This bit controls the clock gate to the USB module.
 *
 * Values:
 * - 0 - Clock disabled
 * - 1 - Clock enabled
 */
//@{
#define BP_SIM_SCGC4_USBOTG  (18U)         //!< Bit position for SIM_SCGC4_USBOTG.
#define BM_SIM_SCGC4_USBOTG  (0x00040000U) //!< Bit mask for SIM_SCGC4_USBOTG.
#define BS_SIM_SCGC4_USBOTG  (1U)          //!< Bit field size in bits for SIM_SCGC4_USBOTG.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the SIM_SCGC4_USBOTG field.
#define BR_SIM_SCGC4_USBOTG  (BITBAND_ACCESS32(HW_SIM_SCGC4_ADDR, BP_SIM_SCGC4_USBOTG))
#endif

//! @brief Format value for bitfield SIM_SCGC4_USBOTG.
#define BF_SIM_SCGC4_USBOTG(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_SIM_SCGC4_USBOTG), uint32_t) & BM_SIM_SCGC4_USBOTG)

#ifndef __LANGUAGE_ASM__
//! @brief Set the USBOTG field to a new value.
#define BW_SIM_SCGC4_USBOTG(v) (BITBAND_ACCESS32(HW_SIM_SCGC4_ADDR, BP_SIM_SCGC4_USBOTG) = (v))
#endif
//@}



//-------------------------------------------------------------------------------------------
// HW_SIM_CLKDIV2 - System Clock Divider Register 2
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_SIM_CLKDIV2 - System Clock Divider Register 2 (RW)
 *
 * Reset value: 0x00000000U
 */
typedef union _hw_sim_clkdiv2
{
    uint32_t U;
    struct _hw_sim_clkdiv2_bitfields
    {
        uint32_t USBFRAC : 1;          //!< [0] USB clock divider fraction
        uint32_t USBDIV : 3;           //!< [3:1] USB clock divider divisor
        uint32_t RESERVED0 : 28;       //!< [31:4]
    } B;
} hw_sim_clkdiv2_t;
#endif

/*!
 * @name Constants and macros for entire SIM_CLKDIV2 register
 */
//@{
#define HW_SIM_CLKDIV2_ADDR      (REGS_SIM_BASE + 0x1048U)

#ifndef __LANGUAGE_ASM__
#define HW_SIM_CLKDIV2           (*(__IO hw_sim_clkdiv2_t *) HW_SIM_CLKDIV2_ADDR)
#define HW_SIM_CLKDIV2_RD()      (HW_SIM_CLKDIV2.U)
#define HW_SIM_CLKDIV2_WR(v)     (HW_SIM_CLKDIV2.U = (v))
#define HW_SIM_CLKDIV2_SET(v)    (HW_SIM_CLKDIV2_WR(HW_SIM_CLKDIV2_RD() |  (v)))
#define HW_SIM_CLKDIV2_CLR(v)    (HW_SIM_CLKDIV2_WR(HW_SIM_CLKDIV2_RD() & ~(v)))
#define HW_SIM_CLKDIV2_TOG(v)    (HW_SIM_CLKDIV2_WR(HW_SIM_CLKDIV2_RD() ^  (v)))
#endif
//@}

/*
 * Constants & macros for individual SIM_CLKDIV2 bitfields
 */

/*!
 * @name Register SIM_CLKDIV2, field USBFRAC[0] (RW)
 *
 * This field sets the fraction multiply value for the fractional clock divider
 * when the MCGFLLCLK/MCGPLLCLK clock is the USB clock source (SOPT2[USBSRC] =
 * 1). Divider output clock = Divider input clock * [ (USBFRAC+1) / (USBDIV+1) ]
 */
//@{
#define BP_SIM_CLKDIV2_USBFRAC (0U)        //!< Bit position for SIM_CLKDIV2_USBFRAC.
#define BM_SIM_CLKDIV2_USBFRAC (0x00000001U) //!< Bit mask for SIM_CLKDIV2_USBFRAC.
#define BS_SIM_CLKDIV2_USBFRAC (1U)        //!< Bit field size in bits for SIM_CLKDIV2_USBFRAC.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the SIM_CLKDIV2_USBFRAC field.
#define BR_SIM_CLKDIV2_USBFRAC (BITBAND_ACCESS32(HW_SIM_CLKDIV2_ADDR, BP_SIM_CLKDIV2_USBFRAC))
#endif

//! @brief Format value for bitfield SIM_CLKDIV2_USBFRAC.
#define BF_SIM_CLKDIV2_USBFRAC(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_SIM_CLKDIV2_USBFRAC), uint32_t) & BM_SIM_CLKDIV2_USBFRAC)

#ifndef __LANGUAGE_ASM__
//! @brief Set the USBFRAC field to a new value.
#define BW_SIM_CLKDIV2_USBFRAC(v) (BITBAND_ACCESS32(HW_SIM_CLKDIV2_ADDR, BP_SIM_CLKDIV2_USBFRAC) = (v))
#endif
//@}

/*!
 * @name Register SIM_CLKDIV2, field USBDIV[3:1] (RW)
 *
 * This field sets the divide value for the fractional clock divider when the
 * MCGFLLCLK/MCGPLLCLK clock is the USB clock source (SOPT2[USBSRC] = 1). Divider
 * output clock = Divider input clock * [ (USBFRAC+1) / (USBDIV+1) ]
 */
//@{
#define BP_SIM_CLKDIV2_USBDIV (1U)         //!< Bit position for SIM_CLKDIV2_USBDIV.
#define BM_SIM_CLKDIV2_USBDIV (0x0000000EU) //!< Bit mask for SIM_CLKDIV2_USBDIV.
#define BS_SIM_CLKDIV2_USBDIV (3U)         //!< Bit field size in bits for SIM_CLKDIV2_USBDIV.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the SIM_CLKDIV2_USBDIV field.
#define BR_SIM_CLKDIV2_USBDIV (HW_SIM_CLKDIV2.B.USBDIV)
#endif

//! @brief Format value for bitfield SIM_CLKDIV2_USBDIV.
#define BF_SIM_CLKDIV2_USBDIV(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_SIM_CLKDIV2_USBDIV), uint32_t) & BM_SIM_CLKDIV2_USBDIV)

#ifndef __LANGUAGE_ASM__
//! @brief Set the USBDIV field to a new value.
#define BW_SIM_CLKDIV2_USBDIV(v) (HW_SIM_CLKDIV2_WR((HW_SIM_CLKDIV2_RD() & ~BM_SIM_CLKDIV2_USBDIV) | BF_SIM_CLKDIV2_USBDIV(v)))
#endif
//@}


#endif // __HW_SIM_REGISTERS_H__
// v22/130726/0.9
// EOF