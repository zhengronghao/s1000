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
 
/*! \file as3911_interrupt.h
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3911 interrupt handling.
 */

/*! \defgroup as3911IrqHandling AS3911 Interrupt Handling
 * \ingroup as3911
 *
 * \brief This part of the AS3911 module abstracts AS3911 interrupt handling.
 *
 * The AS3911 interrupt logic consists of three byte registers to mask or
 * unmask interupt
 * sources and three additional registers to indicate which interrupts are
 * pending. The content of this pending interrupt registers is automatically
 * cleared on a register read. Additionally a single interrupt line is used
 * to signal any interrupt pending interrupt condition to the microcontroller.
 *
 * This module abstracts this so the user no longer needs to know to which
 * interrupt register an interrupt source belongs to. To achive this a flat
 * hirarchy of interrupt masks is provided. This module also serves any
 * interrupt requests from the AS3911 and accumulates the pending interrupt
 * requests until the user reads out the interrupt status (as3911GetInterrupts(),
 * as3911WaitForInterruptTimed()).
 *
 * \section sec_1 Enabling and disabling processing of interrupt request from the AS3911
 *
 * The macros AS3911_IRQ_ON() and AS3911_IRQ_OFF() enable or disable processing
 * of AS3911 interrupt requests by the PIC controller (and thus by this module).
 * If interrupt processing is disabled via AS3911_IRQ_OFF, then no interrupt
 * request will reach the software even if the interrupt source is enabled via
 * as3911EnableInterrupts().
 *
 * Add startup processing of interrupts by the microcontroller is enabled and
 * therefore AS3911_IRQ_ON() needs to called at least once to enable AS3911
 * interrupt processing.
 *
 * \section sec_2 Enabling and disabling interrupts.
 *
 * The functions as3911EnableInterrupts() and as3911DisableInterrupts()
 * can be used to enable or disable specific AS3911 interrupt sources.
 * 
 * \section sec_3 Retreiving and reseting the interrupt status.
 *
 * The function as3911GetInterrupts() can be used to retreive the interrupt
 * status of any combination of AS3911 interrupt source(s). If an interrupt
 * from a source is pending \em and this interrupt is read out via
 * as3911GetInterrupts() then the penging interrupt is automatically
 * cleared. So a subsequent call to
 * as3911GetInterrupt() will mark that source as not pending (if no additional
 * interrupt happened in between).
 *
 * The function as3911ClearInterrupts() can be used to clear the interrupt
 * status of an interrupt source.
 *
 * The function as3911WaitForInterruptTimed can be used to wait for any
 * interrupt out of a set of interrupts to occure. Additionally a timeout can
 * be specified. This function blocks until at least one of the selected
 * interrupts occure or the timeout expires. This function does not enable
 * or disable interrupts. So any interrupt source to wait for needs to be
 * enabled prior to the call via as3911EnableInterrupts.
 */

#ifndef AS3911_INTERRUPT_H
#define AS3911_INTERRUPT_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

//#include <p24Fxxxx.h>
//
//#include "ams_types.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*! \name Interrupt masks */
/**@{*/
/*! \ingroup as3911IrqHandling */
/* Main interrupt register. */
#define AS3911_IRQ_MASK_ALL             (0xFFFFFF)  /*!< All AS3911 interrupt sources. */
#define AS3911_IRQ_MASK_NONE            (0)        /*!< No AS3911 interrupt source. */
#define AS3911_IRQ_MASK_OSC             (0x80)     /*!< AS3911 oscillator stable interrupt. */
#define AS3911_IRQ_MASK_WL              (0x40)     /*!< AS3911 FIFO water level inerrupt. */
#define AS3911_IRQ_MASK_RXS             (0x20)     /*!< AS3911 start of receive interrupt. */
#define AS3911_IRQ_MASK_RXE             (0x10)     /*!< AS3911 end of receive interrupt. */
#define AS3911_IRQ_MASK_TXE             (0x08)     /*!< AS3911 end of transmission interrupt. */
#define AS3911_IRQ_MASK_COL             (0x04)     /*!< AS3911 bit collision interrupt. */

/* Timer and NFC interrupt register. */
#define AS3911_IRQ_MASK_DCT             (0x8000)   /*!< AS3911 termination of direct command interrupt. */
#define AS3911_IRQ_MASK_NRE             (0x4000)   /*!< AS3911 no-response timer expired interrupt. */
#define AS3911_IRQ_MASK_GPE             (0x2000)   /*!< AS3911 general purpose timer expired interrupt. */
#define AS3911_IRQ_MASK_EON             (0x1000)   /*!< AS3911 external field on interrupt. */
#define AS3911_IRQ_MASK_EOF             (0x0800)   /*!< AS3911 external field off interrupt. */
#define AS3911_IRQ_MASK_CAC             (0x0400)   /*!< AS3911 collision during RF collision avoidance interrupt. */
#define AS3911_IRQ_MASK_CAT             (0x0200)   /*!< AS3911 minimum guard time expired interrupt. */
#define AS3911_IRQ_MASK_NFCT            (0x0100)   /*!< AS3911 initiator bit rate recognized interrupt. */

/* Error and wake-up interrupt register. */
#define AS3911_IRQ_MASK_CRC             (0x800000) /*!< AS3911 CRC error interrupt. */
#define AS3911_IRQ_MASK_PAR             (0x400000) /*!< AS3911 parity error interrupt. */
#define AS3911_IRQ_MASK_ERR2            (0x200000) /*!< AS3911 soft framing error interrupt. */
#define AS3911_IRQ_MASK_ERR1            (0x100000) /*!< AS3911 hard framing error interrupt. */
#define AS3911_IRQ_MASK_WT              (0x080000) /*!< AS3911 wake-up interrupt. */
#define AS3911_IRQ_MASK_WAM             (0x040000) /*!< AS3911 wake-up due to amplitude interrupt. */
#define AS3911_IRQ_MASK_WPH             (0x020000) /*!< AS3911 wake-up due to phase interrupt. */
#define AS3911_IRQ_MASK_WCAP            (0x010000) /*!< AS3911 wake-up due to capacitance measurement. */

/**@}*/

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/

/*! \ingroup as3911IrqHandling
 *****************************************************************************
 * \brief Enable processing of AS3911 interrupts by the PIC.
 *****************************************************************************
 */
#define AS3911_IRQ_ON()  {s_as3911_SetInt(ON);}//{ s_OpenIsr(); }

/*! \ingroup as3911IrqHandling
 *****************************************************************************
 *  \brief Disable processing of AS3911 interrupts by the PIC.
 *****************************************************************************
 */
#define AS3911_IRQ_OFF() {s_as3911_SetInt(OFF);}//{ s_CloseIsr(); }

/*
******************************************************************************
* GLOBAL DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL VARIABLE DECLARATIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*! \ingroup as3911IrqHandling
 *****************************************************************************
 * \brief Enable certain AS3911 interrupts.
 *
 * \param[in] mask The interrupts to enable.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, the given interrupts are enabled.
 *****************************************************************************
 */
s8 as3911EnableInterrupts(u32 mask);

/*! \ingroup as3911IrqHandling
 *****************************************************************************
 * \brief Disable certain AS3911 interrupts.
 *
 * \param[in] mask The interrupts to enable.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, the given interrupts are disabled.
 *****************************************************************************
 */
s8 as3911DisableInterrupts(u32 mask);

/*! \ingroup as3911IrqHandling
 *****************************************************************************
 * \brief Clear pending AS3911 interrupts.
 *
 * \param[in] mask The interrupts to clear.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, the given interrupts are cleard.
 *****************************************************************************
 */
s8 as3911ClearInterrupts(u32 mask);

/*! \ingroup as3911IrqHandling
 *****************************************************************************
 * \brief Wait for certain AS3911 interrupts.
 *
 * \param[in] mask The interrupts to wait for.
 * \param[in] timeout
 * \li timeout > 0: Wait operation timeout (milliseconds).
 * \li timeout == 0: No timeout, the function call blocks until one
 * of the interrupts defined by \a mask occur.
 * \param[out] irqs Set to the interrupt(s) wich occured.
 *
 * \note Any interrupt that occurs which is not selected by mask will not
 * cause the wait operation to terminate and will also not be reported in
 * \a irqs after the function call returns.
 * \note All reported interrupts are automatically cleared.
 *
 * \return ERR_NONE: No error, one of the defined interrupts occured or
 * the timeout expired.
 *****************************************************************************
 */
s8 as3911WaitForInterruptTimed(u32 mask, u16 timeout, u32 *irqs);

/*! \ingroup as3911IrqHandling
 *****************************************************************************
 * \brief Get interrupt status of certain interrupts of the AS3911.
 *
 * \param[in] mask The interrupts which status has to be checked.
 * \param[out] irqs Set to the interrupts which are currently pending.
 *
 * \note A pending interrupt that is not selected for readout by \a mask
 * will not be reported.
 * \note All reported interrupts are automatically cleared.
 *
 * \return ERR_NONE: No error, interrupt status read out successful.
 *****************************************************************************
 */
s8 as3911GetInterrupts(u32 mask, u32 *irqs);

void as3911Isr(void);

#define AS3911_TIMER        0  //¶¨Ê±Æ÷0
void as3911_InitTimer(uint id);
void as3911_StartTimer(uint id);
void as3911_StopTimer(uint id);
void as3911_UpdateTimer(uint id, ushort timer);
void as3911_SetTimerCount(uint count);
uint as3911_GetTimerCount(void);
void as3911_WaitForTimer(uint id, uint timeus);
#endif /* AS3911_INTERRUPT_H */
