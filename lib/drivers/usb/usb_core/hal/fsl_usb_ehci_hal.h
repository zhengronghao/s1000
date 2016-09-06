/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __FSL_USB_HAL_H__
#define __FSL_USB_HAL_H__

#include "adapter.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_usb_features.h"
#include "fsl_device_registers.h"
#define NEW_USB_HAL_ENABLE  1
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    #if (defined(CPU_MK21DN51212))
    #include "MK21DN512USB.h"
    #include "MK21DN51212_usb.h"
    #define NEW_USB_HAL_ENABLE  1
#elif (defined(CPU_MK65F18))
    #include "MK65F18.h"
    #define NEW_USB_HAL_ENABLE  0
#elif (defined(CPU_MK70F12))
    #include "MK70F12.h"
    #define NEW_USB_HAL_ENABLE  0
    #endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#if (defined(CPU_MK21DN51212))
#include "MK22F12.h"
#include "MK21DN51212_usb.h"
#define NEW_USB_HAL_ENABLE	1
#elif (defined(CPU_MK70F12))
#include "MK70F12.h"
#define NEW_USB_HAL_ENABLE	0
#elif (defined(CPU_VF65GS10_A5))
#include "MVF50GS10MK50.h"
#define NEW_USB_HAL_ENABLE	0
#endif
#endif

#if NEW_USB_HAL_ENABLE


#else
//#include <stdint.h>
//#include <stdbool.h>
#include <assert.h>

#if (defined(CPU_MK70F12))
#include "MK70F12.h"
#elif (defined(CPU_MK65F18))
#include "MK65F18.h"
#endif
//#include "fsl_usb_features.h"
//#include "device/fsl_device_registers.h"

//! @addtogroup usb_hal
//! @{

//! @file

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////
 
#if defined(__cplusplus)
extern "C" {
#endif
#define HW_USB_INSTANCE_COUNT (1U)

#if (defined(CPU_VF65GS10_A5))
static USB_MemMapPtr _usb_hal_ehci_get_base_addr(uint32_t instance)
{
    USB_MemMapPtr base;	
	switch(instance)
	{
		case 0:
		    base = USB0_BASE_PTR;
		    break;
		case 1:
			base = USB1_BASE_PTR;
			break;
		default:
			break;
	}
	return base;
}
#endif

#if (defined(CPU_MK70F12)) || (defined(CPU_MK65F18))
static void usb_hal_ehci_set_controller_host_mode(uint32_t instance)
{
	USBHS_USBMODE = (USBHS_USBMODE & ~USBHS_USBMODE_CM_MASK) | USBHS_USBMODE_CM(0x3); 
}

static void usb_hal_ehci_set_controller_device_mode(uint32_t instance)
{
	USBHS_USBMODE = (USBHS_USBMODE & ~USBHS_USBMODE_CM_MASK) | USBHS_USBMODE_CM(0x2); 
}

static void usb_hal_ehci_set_big_endian(uint32_t instance)
{
	USBHS_USBMODE = (USBHS_USBMODE & ~USBHS_USBMODE_ES_MASK)|(1 << USBHS_USBMODE_ES_SHIFT);
}

static void usb_hal_ehci_set_little_endian(uint32_t instance)
{
	USBHS_USBMODE = (USBHS_USBMODE & ~USBHS_USBMODE_ES_MASK)|(0 << USBHS_USBMODE_ES_SHIFT);
}

static void usb_hal_ehci_disable_setup_lock(uint32_t instance)
{
	USBHS_USBMODE = (USBHS_USBMODE & ~USBHS_USBMODE_SLOM_MASK)|(1 << USBHS_USBMODE_SLOM_SHIFT);
}

static void usb_hal_ehci_enable_setup_lock(uint32_t instance)
{
	USBHS_USBMODE = (USBHS_USBMODE & ~USBHS_USBMODE_SLOM_MASK)|(0 << USBHS_USBMODE_SLOM_SHIFT);
}

static void usb_hal_ehci_set_usb_mode(uint32_t instance, uint32_t mode)
{
   USBHS_USBMODE = mode;
}

static uint32_t usb_hal_ehci_get_dcc_params(uint32_t instance)
{
	return USBHS_DCCPARAMS;
}

static void usb_hal_ehci_clear_ep_setup_status(uint32_t instance, uint32_t epSetupStatus)
{	
	USBHS_EPSETUPSR |= (epSetupStatus << USBHS_EPSETUPSR_EPSETUPSTAT_SHIFT) & USBHS_EPSETUPSR_EPSETUPSTAT_MASK;
}

static void usb_hal_ehci_clear_setup_stat_ep_num(uint32_t instance, uint32_t ep_num)
{
    USBHS_EPSETUPSR = ep_num;
}

static uint32_t usb_hal_ehci_get_ep_setup_status(uint32_t instance)
{	
	return USBHS_EPSETUPSR;
}

static uint32_t usb_hal_ehci_get_ep_complete_status(uint32_t instance)
{
	return USBHS_EPCOMPLETE;
}

static void usb_hal_ehci_clear_ep_complete_status(uint32_t instance, uint32_t epCompleteStatus)
{
	USBHS_EPCOMPLETE = epCompleteStatus;
}

static void usb_hal_ehci_set_eplist_address(uint32_t instance, uint32_t epQHAddress)
{
	USBHS_EPLISTADDR = epQHAddress;
}

static uint32_t usb_hal_ehci_get_eplist_address(uint32_t instance)
{
	return USBHS_EPLISTADDR;
}

static uint32_t usb_hal_ehci_get_interrupt_status(uint32_t instance)
{
    return USBHS_USBINTR;
}


static void usb_hal_ehci_enable_interrupts(uint32_t instance, uint32_t intrType)
{
	USBHS_USBINTR |= intrType;
}

static void usb_hal_ehci_interrupts_set(uint32_t instance, uint32_t intrType)
{
   USBHS_USBINTR = intrType;
}

static void usb_hal_ehci_disable_interrupts(uint32_t instance, uint32_t intrType)
{
	USBHS_USBINTR &= ~intrType;
}

static void usb_hal_ehci_enable_pull_up(uint32_t instance)
{
	USBHS_USBCMD = EHCI_CMD_RUN_STOP;
}

static void usb_hal_ehci_initiate_attach_event(uint32_t instance)
{
	USBHS_USBCMD |= EHCI_CMD_RUN_STOP;
}

static void usb_hal_ehci_initiate_detach_event(uint32_t instance)
{
	USBHS_USBCMD &= ~EHCI_CMD_RUN_STOP;
}

static void usb_hal_ehci_reset_controller(uint32_t instance)
{
	USBHS_USBCMD = EHCI_CMD_CTRL_RESET;
}

static void usb_hal_ehci_set_dTD_TripWire(uint32_t instance)
{
	USBHS_USBCMD |= EHCI_CMD_ATDTW_TRIPWIRE_SET;
}

static void usb_hal_ehci_clear_dTD_TripWire(uint32_t instance)
{
	USBHS_USBCMD &= ~EHCI_CMD_ATDTW_TRIPWIRE_SET;
}

static void usb_hal_echi_clear_usb_cmd_async_sched_enable(uint32_t instance)
{
    USBHS_USBCMD &= ~EHCI_USBCMD_ASYNC_SCHED_ENABLE;
}

static void usb_hal_ehci_set_usb_cmd_async_sched_enable(uint32_t instance)
{
    USBHS_USBCMD |= EHCI_USBCMD_ASYNC_SCHED_ENABLE;
}
static void usb_hal_ehci_clear_usb_cmd_periodic_sched_enable(uint32_t instance)
{
    USBHS_USBCMD &= ~EHCI_USBCMD_PERIODIC_SCHED_ENABLE;
}

static uint32_t usb_hal_ehci_is_TripWire_set(uint32_t instance)
{
	return (USBHS_USBCMD & EHCI_CMD_ATDTW_TRIPWIRE_SET);
}


static void usb_hal_ehci_clear_usb_cmd_setup_trip_wire(uint32_t instance)
{
    USBHS_USBCMD &= ~EHCI_CMD_SETUP_TRIPWIRE_SET;
}

static void usb_hal_ehci_set_usb_cmd_setup_trip_wire(uint32_t instance)
{
    USBHS_USBCMD |= EHCI_CMD_SETUP_TRIPWIRE_SET;
}


static void usb_hal_ehci_set_usb_cmd(uint32_t instance, uint32_t value)
{
    USBHS_USBCMD = value;
}

static uint32_t  usb_hal_ehci_get_usb_cmd(uint32_t instance)
{
	return USBHS_USBCMD;
}



static uint8_t usb_hal_ehci_get_cap_length(uint32_t instance)
{
    return USBHS_CAPLENGTH;
}

static void usb_hal_ehci_set_usb_config(uint32_t instance, uint32_t value)
{
    //USBHS_CONFIGFLAG = value;
}


static void usb_hal_ehci_set_endpoint_prime(uint32_t instance, uint32_t value)
{	
	USBHS_EPPRIME = value;
}

static uint32_t usb_hal_ehci_get_endpoint_prime(uint32_t instance)
{
	return USBHS_EPPRIME;
}
static uint32_t usb_hal_echi_get_endpoint_status(uint32_t instance)
{
	return USBHS_EPSR;
}

static void usb_hal_ehci_flush_endpoint_buffer(uint32_t instance, uint32_t epNumber)
{
	USBHS_EPFLUSH = epNumber;
}

static uint32_t usb_hal_ehci_is_endpoint_transfer_flushed(uint32_t instance, uint32_t epNumber)
{
	return (USBHS_EPFLUSH & epNumber);
}

static uint32_t usb_hal_ehci_get_frame_index(uint32_t instance)
{
	return USBHS_FRINDEX;
}

static uint32_t usb_hal_ehci_get_port_status(uint32_t instance)
{
	return USBHS_PORTSC1;
}

static void usb_hal_ehci_set_port_status(uint32_t instance, uint32_t status)
{
	USBHS_PORTSC1 = status;
}

static uint32_t usb_hal_ehci_get_usb_status(uint32_t instance)
{
	return USBHS_USBSTS;
}

static void usb_hal_ehci_clear_usb_status(uint32_t instance, uint32_t status)
{
	USBHS_USBSTS = status;
}

static uint32_t usb_hal_ehci_get_hcsparams(uint32_t instance)
{
	return USBHS_HCSPARAMS;
}

static void usb_hal_ehci_clear_device_address(uint32_t instance)
{
	USBHS_DEVICEADDR &= ~0xFE000000;
}

static void usb_hal_ehci_set_qh_to_curr_async_list(uint32_t instance, uint32_t qh_addr)
{
    USBHS_ASYNCLISTADDR = qh_addr;
}

static uint32_t usb_hal_ehci_get_curr_async_list(uint32_t instance)
{
   return USBHS_ASYNCLISTADDR;
}

static void usb_hal_ehci_set_periodic_list_base_addr(uint32_t instance, uint32_t base_addr)
{
    USBHS_PERIODICLISTBASE = base_addr;
}
static void usb_hal_ehci_enable_endpoint(uint32_t instance, uint32_t epNumber, uint32_t direction, uint32_t type)
{
	if (epNumber == 0)
		USBHS_EPCR0 |= ((direction ? (EHCI_EPCTRL_TX_ENABLE |
            EHCI_EPCTRL_TX_DATA_TOGGLE_RST) :
            (EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST)) |
            (type << (direction ?
            EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT)));
	else
		USBHS_EPCR_REG(USBHS_BASE_PTR,epNumber - 1) |= ((direction ? (EHCI_EPCTRL_TX_ENABLE |
            EHCI_EPCTRL_TX_DATA_TOGGLE_RST) :
            (EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST)) |
            (type << (direction ?
            EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT)));
}

static void usb_hal_ehci_disable_endpoint(uint32_t instance, uint32_t epNumber, uint32_t direction)
{
    USBHS_EPCR(epNumber) &= ~(direction ? (EHCI_EPCTRL_TX_ENABLE|EHCI_EPCTRL_TX_TYPE):(EHCI_EPCTRL_RX_ENABLE|EHCI_EPCTRL_RX_TYPE));
}

static uint32_t usb_hal_ehci_get_endpoint_control(uint32_t instance, uint32_t epNumber)
{
	return (epNumber == 0 ? USBHS_EPCR0 : USBHS_EPCR_REG(USBHS_BASE_PTR,epNumber - 1));
}	


static void usb_hal_ehci_clear_endpoint_stall(uint32_t instance, uint32_t epNumber, uint32_t direction)
{
	if (epNumber == 0)
		USBHS_EPCR0 &= ~(direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
	else
		USBHS_EPCR_REG(USBHS_BASE_PTR,epNumber - 1) &= ~(direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
}

static void usb_hal_ehci_stall_both_directions(uint32_t instance, uint32_t epNumber)
{
	if (epNumber == 0)
		USBHS_EPCR0 |= (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
	else
		USBHS_EPCR_REG(USBHS_BASE_PTR,epNumber - 1) |= (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
}

static void usb_hal_ehci_stall_specific_direction(uint32_t instance, uint32_t epNumber, uint32_t direction)
{
	if (epNumber == 0)
		USBHS_EPCR0 |= (direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
	else
		USBHS_EPCR_REG(USBHS_BASE_PTR,epNumber - 1) |= (direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
		
}



static void usb_hal_ehci_set_device_address(uint32_t instance, uint32_t address)
{
	USBHS_DEVICEADDR = (uint32_t)(address << USBHS_ADDRESS_BIT_SHIFT);
}

static void usb_hal_ehci_clear_max_packet_length(uint32_t instance,usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
	epQueueHeadAddr->MAX_PKT_LENGTH = 0;
}

static void usb_hal_ehci_set_max_packet_length(uint32_t instance,usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t maxPacketSize)
{
	epQueueHeadAddr->MAX_PKT_LENGTH = ((uint32_t)maxPacketSize) ;
}

static void usb_hal_ehci_set_max_packet_length_for_non_iso(uint32_t instance,usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t value)
{
    epQueueHeadAddr->MAX_PKT_LENGTH = value;
}

static uint32_t usb_hal_ehci_is_ios_set(uint32_t instance, usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
	return (epQueueHeadAddr->MAX_PKT_LENGTH & VUSB_EP_QUEUE_HEAD_IOS);
}

static void usb_hal_ehci_set_next_dtd_terminate(uint32_t instance,usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
	epQueueHeadAddr->NEXT_DTD_PTR = VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;
}

static void usb_hal_ehci_set_dtd_next_tr_elem_terminate(uint32_t instance, usb_ehci_dev_dtd_struct_t * dTD_ptr)
{
   dTD_ptr->NEXT_TR_ELEM_PTR = VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;
}

static void usb_hal_ehci_set_dtd_next_tr_elem_ptr(uint32_t instance, usb_ehci_dev_dtd_struct_t * temp_dTD_ptr, uint32_t dTD_ptr)
{
   temp_dTD_ptr->NEXT_TR_ELEM_PTR = dTD_ptr;
}

static void usb_hal_ehci_set_qh_next_dtd(uint32_t instance,usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t dtdAddr)
{
	epQueueHeadAddr->NEXT_DTD_PTR = dtdAddr;
}

static void usb_hal_ehci_set_next_dtd_invalid(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	dtdAddr->NEXT_TR_ELEM_PTR = USBHS_TD_NEXT_TERMINATE;
}

static void usb_hal_ehci_clear_SIZE_IOC_STS(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	dtdAddr->SIZE_IOC_STS = 0;
}

static void usb_hal_ehci_clear_IOC_reserved_fields(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	dtdAddr->SIZE_IOC_STS &= ~USBHS_TD_RESERVED_FIELDS;
}

static void usb_hal_ehci_clear_dtd(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	dtdAddr->NEXT_TR_ELEM_PTR = 0;
	dtdAddr->SIZE_IOC_STS = 0;
	dtdAddr->BUFF_PTR0 = 0;
	dtdAddr->BUFF_PTR1 = 0;
	dtdAddr->BUFF_PTR2 = 0;
	dtdAddr->BUFF_PTR3 = 0;
	dtdAddr->BUFF_PTR4 = 0;
}

static void usb_hal_ehci_set_dtd_buffer_address(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr, uint32_t addr)
{
	dtdAddr->BUFF_PTR0 = addr;
	dtdAddr->BUFF_PTR1 = addr + 4096;
	dtdAddr->BUFF_PTR2 = addr + (4096*2);
	dtdAddr->BUFF_PTR3 = addr + (4096*3);
	dtdAddr->BUFF_PTR4 = addr + (4096*4);
}

static void usb_hal_ehci_set_dtd_size_ioc_status(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr, uint32_t value)
{
	dtdAddr->SIZE_IOC_STS = value;
}

static uint32_t usb_hal_ehci_get_dtd_size_ioc_status(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	return dtdAddr->SIZE_IOC_STS;
}


static uint32_t usb_hal_ehci_get_next_dtd_address(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	return (dtdAddr->NEXT_TR_ELEM_PTR & USBHS_TD_ADDR_MASK);
}

static uint32_t usb_hal_ehci_get_dtd_error_status(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	return (dtdAddr->SIZE_IOC_STS & USBHS_TD_ERROR_MASK);
}



static void usb_hal_ehci_clear_qh_error_status(uint32_t instance, usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t errors)
{
	epQueueHeadAddr->SIZE_IOC_INT_STS &= ~errors;
}

static uint32_t usb_hal_ehci_get_tr_packet_size(uint32_t instance,usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	return ((dtdAddr->SIZE_IOC_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);
}

static uint32_t usb_hal_ehci_get_xd_for_this_dtd(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	return (uint32_t)dtdAddr->xd_for_this_dtd;
}


static void usb_hal_ehci_clear_SIZE_IOC_INT_STS(uint32_t instance, usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
	epQueueHeadAddr->SIZE_IOC_INT_STS = 0;
}



static uint32_t usb_hal_ehci_get_fstn_normal_path_link_ptr(uint32_t instance,ehci_fstn_struct_t* FSTN_ptr)
{
   return FSTN_ptr->normal_path_link_ptr;
}

static void  usb_hal_ehci_set_fstn_normal_path_link_ptr(uint32_t instance,ehci_fstn_struct_t* FSTN_ptr, uint32_t next)
{
   FSTN_ptr->normal_path_link_ptr = next;
}


static void usb_hal_ehci_add_frame_list_pointer_ITD(uint32_t instance, uint32_t *transaction_descriptor, ehci_itd_struct_t* itd_ptr)
{
   *transaction_descriptor = (uint32_t) itd_ptr | (EHCI_FRAME_LIST_ELEMENT_TYPE_ITD << EHCI_FRAME_LIST_ELEMENT_TYPE_BIT_POS);
}

static void usb_hal_ehci_add_frame_list_pointer_SITD(uint32_t instance, uint32_t *transaction_descriptor, ehci_sitd_struct_t* sitd_ptr)
{
   *transaction_descriptor = (uint32_t) sitd_ptr | (EHCI_FRAME_LIST_ELEMENT_TYPE_SITD << EHCI_FRAME_LIST_ELEMENT_TYPE_BIT_POS);
}


static uint32_t usb_hal_ehci_get_periodic_list_addr(uint32_t instance, volatile uint32_t*  transfer_data_struct_ptr)
{
   return (*transfer_data_struct_ptr);
}

static void usb_hal_ehci_set_periodic_list_addr(uint32_t instance, volatile uint32_t*  transfer_data_struct_ptr, ehci_qh_struct_t* qh_ptr)
{
   *transfer_data_struct_ptr = ((uint32_t)qh_ptr | (EHCI_FRAME_LIST_ELEMENT_TYPE_QH << EHCI_QH_ELEMENT_TYPE_BIT_POS));
}


static void usb_hal_ehci_set_transfer_data_struct(uint32_t instance, volatile uint32_t*  transfer_data_struct_ptr, uint32_t temp_transfer_struct_ptr)
{
  *transfer_data_struct_ptr = temp_transfer_struct_ptr;
}

//static void usb_hal_ehci_store_device_address(uint32_t instance, ehci_itd_struct_t* itd_ptr)
//{
//   itd_ptr->buffer_page_ptr_list[0] |= (uint32_t)(pipe_descr_ptr->G.DEVICE_ADDRESS);
//}

static void usb_hal_ehci_set_itd_next_link_pointer(uint32_t instance,ehci_itd_struct_t* itd_ptr, uint32_t transaction_descriptor )
{
    itd_ptr->next_link_ptr = (uint32_t)transaction_descriptor;
}

static uint32_t usb_hal_ehci_get_itd_next_link_pointer(uint32_t instance,ehci_itd_struct_t* itd_ptr)
{
   return itd_ptr->next_link_ptr;
}


static void usb_hal_ehci_set_ITD_terminate_bit(uint32_t instance,ehci_itd_struct_t* itd_ptr)
{
   itd_ptr->next_link_ptr = EHCI_ITD_T_BIT;
}


static void usb_hal_ehci_store_endpoint_number_and_device_addr(uint32_t instance, ehci_itd_struct_t* itd_ptr, uint32_t value)
{
    itd_ptr->buffer_page_ptr_list[0] = value;
}

static void usb_hal_ehci_store_max_packet_size_and_direction(uint32_t instance, ehci_itd_struct_t* itd_ptr, uint32_t value)
{
		itd_ptr->buffer_page_ptr_list[1] = value;
}

static void usb_hal_ehci_store_direction(uint32_t instance, ehci_itd_struct_t* itd_ptr, uint32_t direction)
{
    itd_ptr->buffer_page_ptr_list[1] |= (uint32_t)(direction << EHCI_ITD_DIRECTION_BIT_POS);
}

static void usb_hal_ehci_set_transaction_number_per_micro_frame(uint32_t instance, ehci_itd_struct_t* itd_ptr, uint32_t number)
{
   itd_ptr->buffer_page_ptr_list[2] = number;
}

static void usb_hal_ehci_set_buffer_page_pointer(uint32_t instance, ehci_itd_struct_t* itd_ptr, uint32_t page_number, uint32_t buffer_ptr)
{
   itd_ptr->buffer_page_ptr_list[page_number] = buffer_ptr;
}

static uint32_t usb_hal_ehci_get_buffer_page_pointer(uint32_t instance, ehci_itd_struct_t* itd_ptr, uint32_t page_number)
{
   return itd_ptr->buffer_page_ptr_list[page_number];
}


static void usb_hal_ehci_set_transcation_status_and_control_bit(uint32_t instance, ehci_itd_struct_t* itd_ptr, uint32_t number, uint32_t value)
{
  itd_ptr->tr_status_ctl_list[number] = value;
}

static void usb_hal_ehci_set_transcation_status_active_bit(uint32_t instance, ehci_itd_struct_t* itd_ptr, uint32_t number)
{
   itd_ptr->tr_status_ctl_list[number] = itd_ptr->tr_status_ctl_list[number] | EHCI_ITD_ACTIVE_BIT;
}

static uint32_t usb_hal_ehci_get_transcation_status_ctl_list(uint32_t instance, ehci_itd_struct_t* itd_ptr, uint32_t transaction_number)
{
   return itd_ptr->tr_status_ctl_list[transaction_number];
}


static void usb_hal_ehci_set_itd_length_scheduled(uint32_t instance, ehci_itd_struct_t* itd_ptr, uint32_t length_scheduled)
{
  itd_ptr->reserved[MEM_SIZE_INDEX] = length_scheduled;
}



static void usb_hal_ehci_set_sitd_next_link_pointer(uint32_t instance,ehci_sitd_struct_t* sitd_ptr, uint32_t transaction_descriptor )
{
    sitd_ptr->next_link_ptr =  (uint32_t)transaction_descriptor;
}

static uint32_t usb_hal_ehci_get_sitd_next_link_pointer(uint32_t instance,ehci_sitd_struct_t* sitd_ptr)
{
   return sitd_ptr->next_link_ptr;
}

static uint32_t usb_hal_ehci_get_sitd_transfer_state(uint32_t instance,ehci_sitd_struct_t* sitd_ptr)
{
    return sitd_ptr->transfer_state;
}


static void usb_hal_ehci_set_sitd_next_link_terminate_bit(uint32_t instance, ehci_sitd_struct_t* sitd_ptr)
{
	sitd_ptr->next_link_ptr = EHCI_SITD_T_BIT;
}

static void usb_hal_ehci_set_sitd_back_pointer_terminate_bit(uint32_t instance, ehci_sitd_struct_t* sitd_ptr)
{
    sitd_ptr->back_link_ptr = EHCI_SITD_T_BIT;
}

static void usb_hal_ehci_set_sitd_ep_capab_charac(uint32_t instance, ehci_sitd_struct_t* sitd_ptr, uint32_t value)
{
   sitd_ptr->ep_capab_charac = value;
}

static void usb_hal_ehci_set_sitd_uframe_sched_ctl(uint32_t instance, ehci_sitd_struct_t* sitd_ptr, uint32_t value)
{
   sitd_ptr->uframe_sched_ctl= value;
}

static void usb_hal_ehci_set_sitd_transfer_state(uint32_t instance, ehci_sitd_struct_t* sitd_ptr, uint32_t value)
{
   sitd_ptr->transfer_state = value;
}

static void usb_hal_ehci_set_sitd_length_scheduled(uint32_t instance, ehci_sitd_struct_t* sitd_ptr, uint32_t length_scheduled)
{
   sitd_ptr->reserved[MEM_SIZE_INDEX] = length_scheduled;
}

static uint32_t usb_hal_ehci_get_sitd_length_scheduled(uint32_t instance, ehci_sitd_struct_t* sitd_ptr)
{
  return sitd_ptr->reserved[MEM_SIZE_INDEX];
}


static uint32_t usb_hal_ehci_get_sitd_buffer_ptr_0(uint32_t instance, ehci_sitd_struct_t* sitd_ptr)
{
  return sitd_ptr->buffer_ptr_0;
}

static void usb_hal_ehci_set_sitd_buffer_ptr_0(uint32_t instance, ehci_sitd_struct_t* sitd_ptr, uint32_t buffer_ptr)
{
   sitd_ptr->buffer_ptr_0 = buffer_ptr;
}

static void usb_hal_ehci_set_sitd_buffer_ptr_1(uint32_t instance, ehci_sitd_struct_t* sitd_ptr, uint32_t buffer_ptr)
{
   sitd_ptr->buffer_ptr_1 = buffer_ptr;
}

static void usb_hal_ehci_set_qh_next_alt_qtd_link_terminate(uint32_t instance, ehci_qh_struct_t* qh_ptr)
{
   qh_ptr->alt_next_qtd_link_ptr = EHCI_QTD_T_BIT;
}

static void usb_hal_ehci_set_qh_next_qtd_link_ptr(uint32_t instance, ehci_qh_struct_t* qh_ptr, uint32_t first_qtd_ptr)
{

   qh_ptr->next_qtd_link_ptr = (uint32_t)first_qtd_ptr;
}

static void usb_hal_ehci_set_qh_next_qtd_link_terminate(uint32_t instance, ehci_qh_struct_t* qh_ptr)

{
   qh_ptr->next_qtd_link_ptr = EHCI_QTD_T_BIT;
}
static uint32_t usb_hal_ehci_get_next_qtd_link_ptr(uint32_t instance, ehci_qh_struct_t* qh_ptr)
{
  return (uint32_t)qh_ptr->next_qtd_link_ptr;
}



static void usb_hal_ehci_clear_qh_status(uint32_t instance, ehci_qh_struct_t* qh_ptr)
{
  qh_ptr->status = 0;
}

static uint32_t usb_hal_ehci_get_qh_status(uint32_t instance, ehci_qh_struct_t* qh_ptr)
{
   return qh_ptr->status;
}

static void usb_hal_ehci_set_qh_status(uint32_t instance, ehci_qh_struct_t* qh_ptr, uint32_t status)
{
   qh_ptr->status = status;
}

static uint32_t usb_hal_ehci_get_qh_horiz_link_ptr(uint32_t instance, ehci_qh_struct_t* qh_ptr)
{
  return qh_ptr->horiz_link_ptr;
}

static void usb_hal_ehci_set_qh_horiz_link_ptr(uint32_t instance, ehci_qh_struct_t* qh_ptr, uint32_t link_ptr)
{

   qh_ptr->horiz_link_ptr = link_ptr;
}

static void usb_hal_ehci_init_qh(uint32_t instance,  ehci_qh_struct_t* qh_ptr)
{
    qh_ptr->curr_qtd_link_ptr = EHCI_QTD_T_BIT;
    qh_ptr->alt_next_qtd_link_ptr = EHCI_QTD_T_BIT;
    qh_ptr->status = 0;
	qh_ptr->buffer_ptr_0 = 0;
	qh_ptr->buffer_ptr_1 = 0;
	qh_ptr->buffer_ptr_2 = 0;
    qh_ptr->buffer_ptr_3 = 0;
	qh_ptr->buffer_ptr_4 = 0;
}


static void usb_hal_ehci_set_ep_capab_charac1(uint32_t instance,  ehci_qh_struct_t* qh_ptr, uint32_t temp_ep_capab)
{
   qh_ptr->ep_capab_charac1 = temp_ep_capab;
}

static uint32_t usb_hal_ehci_get_ep_capab_charac1(uint32_t instance,  ehci_qh_struct_t* qh_ptr)
{
   return qh_ptr->ep_capab_charac1;
}

static void usb_hal_ehci_set_ep_capab_charac2(uint32_t instance,  ehci_qh_struct_t* qh_ptr, uint32_t temp_ep_capab)
{
   qh_ptr->ep_capab_charac2 = temp_ep_capab;
}

static uint32_t usb_hal_ehci_get_ep_capab_charac2(uint32_t instance,  ehci_qh_struct_t* qh_ptr)
{
   return qh_ptr->ep_capab_charac2;
}

static void usb_hal_ehci_set_qh_horiz_link_ptr_head_pointer_terminate(uint32_t instance,  ehci_qh_struct_t* qh_ptr)
{
   qh_ptr->horiz_link_ptr = EHCI_QUEUE_HEAD_POINTER_T_BIT;
}


static void usb_hal_ehci_set_qtd_buffer_page_pointer(uint32_t instance, ehci_qtd_struct_t* qtd_ptr, uint32_t buffer_start_address)
{
   qtd_ptr->buffer_ptr_0 = buffer_start_address;
   qtd_ptr->buffer_ptr_1 = qtd_ptr->buffer_ptr_0 + 4096;
   qtd_ptr->buffer_ptr_2 = qtd_ptr->buffer_ptr_1 + 4096;
   qtd_ptr->buffer_ptr_3 = qtd_ptr->buffer_ptr_2 + 4096;
   qtd_ptr->buffer_ptr_4 = qtd_ptr->buffer_ptr_3 + 4096;
}


static void usb_hal_ehci_set_qtd_token(uint32_t instance, ehci_qtd_struct_t* qtd_ptr, uint32_t token)
{

   qtd_ptr->token = token;
}

static void usb_hal_ehci_clear_qtd_token_bits(uint32_t instance, ehci_qtd_struct_t* qtd_ptr, uint32_t token_bits)
{
   qtd_ptr->token &= ~token_bits;
}

static uint32_t usb_hal_ehci_get_qtd_token(uint32_t instance, ehci_qtd_struct_t* qtd_ptr)
{

   return qtd_ptr->token;
}

static void usb_hal_ehci_set_qtd_terminate_bit(uint32_t instance, ehci_qtd_struct_t* qtd_ptr)
{
	qtd_ptr->next_qtd_ptr = EHCI_QTD_T_BIT;
}

static uint32_t usb_hal_ehci_get_next_qtd_ptr(uint32_t instance,ehci_qtd_struct_t* qtd_ptr)
{
    return qtd_ptr->next_qtd_ptr;
}

static void usb_hal_ehci_set_alt_next_qtd_terminate_bit(uint32_t instance, ehci_qtd_struct_t* qtd_ptr)
{
  qtd_ptr->alt_next_qtd_ptr = EHCI_QTD_T_BIT;
}

static void usb_hal_ehci_link_qtd(uint32_t instance, ehci_qtd_struct_t* prev_qtd_ptr,uint32_t qtd_ptr)
{
   prev_qtd_ptr->next_qtd_ptr = qtd_ptr;
}



#elif (defined(CPU_VF65GS10_A5))
static void usb_hal_ehci_set_controller_host_mode(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBMODE = (usb_reg_base_ptr->USBMODE & ~USB_USBMODE_CM_MASK) | USB_USBMODE_CM(0x3); 
}

static void usb_hal_ehci_set_controller_device_mode(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBMODE = (usb_reg_base_ptr->USBMODE & ~USB_USBMODE_CM_MASK) | USB_USBMODE_CM(0x2); 
}

static void usb_hal_ehci_set_big_endian(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBMODE = (usb_reg_base_ptr->USBMODE & ~USB_USBMODE_ES_MASK)|(1 << USB_USBMODE_ES_SHIFT);
}

static void usb_hal_ehci_set_little_endian(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBMODE = (usb_reg_base_ptr->USBMODE & ~USB_USBMODE_ES_MASK)|(0 << USB_USBMODE_ES_SHIFT);
}

static void usb_hal_ehci_disable_setup_lock(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBMODE = (usb_reg_base_ptr->USBMODE & ~USB_USBMODE_SLOM_MASK)|(1 << USB_USBMODE_SLOM_SHIFT);
}

static void usb_hal_ehci_enable_setup_lock(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBMODE = (usb_reg_base_ptr->USBMODE & ~USB_USBMODE_SLOM_MASK)|(0 << USB_USBMODE_SLOM_SHIFT);
}

static uint32_t usb_hal_ehci_get_dcc_params(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return usb_reg_base_ptr->DCCPARAMS;
}

static void usb_hal_ehci_clear_ep_setup_status(uint32_t instance, uint32_t epSetupStatus)
{	
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->ENDPTSETUPSTAT |= (epSetupStatus << USB_ENDPTSETUPSTAT_ENDPTSETUPSTAT_SHIFT) & USB_ENDPTSETUPSTAT_ENDPTSETUPSTAT_MASK;
}

static uint32_t usb_hal_ehci_get_ep_setup_status(uint32_t instance)
{	
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return usb_reg_base_ptr->ENDPTSETUPSTAT;
}

static uint32_t usb_hal_ehci_get_ep_complete_status(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return usb_reg_base_ptr->ENDPTCOMPLETE;
}

static void usb_hal_ehci_clear_ep_complete_status(uint32_t instance, uint32_t epCompleteStatus)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->ENDPTCOMPLETE = epCompleteStatus;
}

static void usb_hal_ehci_set_eplist_address(uint32_t instance, uint32_t epQHAddress)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->ENDPTLISTADDR = epQHAddress;
}

static uint32_t usb_hal_ehci_get_eplist_address(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return usb_reg_base_ptr->ENDPTLISTADDR;
}


static void usb_hal_ehci_enable_interrupts(uint32_t instance, uint32_t intrType)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBINTR = intrType;
}

static void usb_hal_ehci_disable_interrupts(uint32_t instance, uint32_t intrType)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBINTR &= ~intrType;
}
static void usb_hal_ehci_enable_pull_up(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBCMD = EHCI_CMD_RUN_STOP;
}

static void usb_hal_ehci_initiate_attach_event(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBCMD |= EHCI_CMD_RUN_STOP;
}

static void usb_hal_ehci_initiate_detach_event(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBCMD &= ~EHCI_CMD_RUN_STOP;
}

static void usb_hal_ehci_reset_controller(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBCMD = EHCI_CMD_CTRL_RESET;
}

static void usb_hal_ehci_set_dTD_TripWire(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBCMD |= EHCI_CMD_ATDTW_TRIPWIRE_SET;
}

static void usb_hal_ehci_clear_dTD_TripWire(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->USBCMD &= ~EHCI_CMD_ATDTW_TRIPWIRE_SET;
}
static uint32_t usb_hal_ehci_is_TripWire_set(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return (usb_reg_base_ptr->USBCMD & EHCI_CMD_ATDTW_TRIPWIRE_SET);
}

static void usb_hal_ehci_set_endpoint_prime(uint32_t instance, uint32_t value)
{	
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->ENDPTPRIME = value;
}

static uint32_t usb_hal_ehci_get_endpoint_prime(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return usb_reg_base_ptr->ENDPTPRIME;
}
static uint32_t usb_hal_echi_get_endpoint_status(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return usb_reg_base_ptr->ENDPTSTAT;
}

static void usb_hal_ehci_flush_endpoint_buffer(uint32_t instance, uint32_t epNumber)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->ENDPTFLUSH = epNumber;
}

static uint32_t usb_hal_ehci_is_endpoint_transfer_flushed(uint32_t instance, uint32_t epNumber)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return (usb_reg_base_ptr->ENDPTFLUSH & epNumber);
}

static uint32_t usb_hal_ehci_get_frame_index(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return usb_reg_base_ptr->FRINDEX;
}

static uint32_t usb_hal_ehci_get_port_status(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return usb_reg_base_ptr->PORTSC1;
}

static void usb_hal_ehci_set_port_status(uint32_t instance, uint32_t status)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->PORTSC1 = status;
}

static uint32_t usb_hal_ehci_get_usb_status(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return usb_reg_base_ptr->USBSTS;
}

static void usb_hal_ehci_set_device_address(uint32_t instance, uint32_t address)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->DEVICEADDR = (uint32_t)(address << USBHS_ADDRESS_BIT_SHIFT);
}

static void usb_hal_ehci_clear_device_address(uint32_t instance)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	usb_reg_base_ptr->DEVICEADDR &= ~0xFE000000;
}

static void usb_hal_ehci_enable_endpoint(uint32_t instance, uint32_t epNumber, uint32_t direction, uint32_t type)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	
	if (epNumber == 0)
		usb_reg_base_ptr->ENDPTCTRL0 |= ((direction ? (EHCI_EPCTRL_TX_ENABLE |
            EHCI_EPCTRL_TX_DATA_TOGGLE_RST) :
            (EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST)) |
            (type << (direction ?
            EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT)));
	else
		usb_reg_base_ptr->ENDPTCTRL[epNumber - 1] |= ((direction ? (EHCI_EPCTRL_TX_ENABLE |
            EHCI_EPCTRL_TX_DATA_TOGGLE_RST) :
            (EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST)) |
            (type << (direction ?
            EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT)));
}
static uint32_t usb_hal_ehci_get_endpoint_control(uint32_t instance, uint32_t epNumber)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	return (epNumber == 0 ? usb_reg_base_ptr->ENDPTCTRL0 : usb_reg_base_ptr->ENDPTCTRL[epNumber - 1]);
}	


static void usb_hal_ehci_clear_endpoint_stall(uint32_t instance, uint32_t epNumber, uint32_t direction)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	if (epNumber == 0)
		usb_reg_base_ptr->ENDPTCTRL0 &= ~(direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
	else
		usb_reg_base_ptr->ENDPTCTRL[epNumber - 1] &= ~(direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
}

static void usb_hal_ehci_stall_both_directions(uint32_t instance, uint32_t epNumber)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	if (epNumber == 0)
		usb_reg_base_ptr->ENDPTCTRL0 |= (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
	else
		usb_reg_base_ptr->ENDPTCTRL[epNumber - 1] |= (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
}

static void usb_hal_ehci_stall_specific_direction(uint32_t instance, uint32_t epNumber, uint32_t direction)
{
	USB_MemMapPtr usb_reg_base_ptr;
	usb_reg_base_ptr = _usb_hal_ehci_get_base_addr(instance);
	if (epNumber == 0)
		usb_reg_base_ptr->ENDPTCTRL0 |= (direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
	else
		usb_reg_base_ptr->ENDPTCTRL[epNumber - 1] |= (direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);
		
}

static void usb_hal_ehci_clear_max_packet_length(uint32_t instance,usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
	epQueueHeadAddr->MAX_PKT_LENGTH = 0;
}

static void usb_hal_ehci_set_max_packet_length(uint32_t instance,usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t maxPacketSize)
{
	epQueueHeadAddr->MAX_PKT_LENGTH = ((uint32_t)maxPacketSize << VUSB_EP_QUEUE_HEAD_MAX_PKT_LEN_POS);
}

static uint32_t usb_hal_ehci_is_ios_set(uint32_t instance, usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
	return (epQueueHeadAddr->MAX_PKT_LENGTH & VUSB_EP_QUEUE_HEAD_IOS);
}



static void usb_hal_ehci_set_next_dtd_terminate(uint32_t instance,usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
	epQueueHeadAddr->NEXT_DTD_PTR = VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;
}


static void usb_hal_ehci_set_qh_next_dtd(uint32_t instance,usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t dtdAddr)
{
	epQueueHeadAddr->NEXT_DTD_PTR = dtdAddr;
}
static void usb_hal_ehci_set_next_dtd_invalid(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	dtdAddr->NEXT_TR_ELEM_PTR = USBHS_TD_NEXT_TERMINATE;
}

static void usb_hal_ehci_clear_SIZE_IOC_STS(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	dtdAddr->SIZE_IOC_STS = 0;
}

static void usb_hal_ehci_clear_IOC_reserved_fields(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	dtdAddr->SIZE_IOC_STS &= ~USBHS_TD_RESERVED_FIELDS;
}

static void usb_hal_ehci_clear_dtd(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	dtdAddr->NEXT_TR_ELEM_PTR = 0;
	dtdAddr->SIZE_IOC_STS = 0;
	dtdAddr->BUFF_PTR0 = 0;
	dtdAddr->BUFF_PTR1 = 0;
	dtdAddr->BUFF_PTR2 = 0;
	dtdAddr->BUFF_PTR3 = 0;
	dtdAddr->BUFF_PTR4 = 0;
}

static void usb_hal_ehci_set_dtd_buffer_address(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr, uint32_t addr)
{
	dtdAddr->BUFF_PTR0 = addr;
	dtdAddr->BUFF_PTR1 = addr + 4096;
	dtdAddr->BUFF_PTR2 = addr + (4096*2);
	dtdAddr->BUFF_PTR3 = addr + (4096*3);
	dtdAddr->BUFF_PTR4 = addr + (4096*4);
}

static void usb_hal_ehci_set_dtd_size_ioc_status(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr, uint32_t value)
{
	dtdAddr->SIZE_IOC_STS = value;
}

static uint32_t usb_hal_ehci_get_dtd_size_ioc_status(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	return dtdAddr->SIZE_IOC_STS;
}


static uint32_t usb_hal_ehci_get_next_dtd_address(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	return (dtdAddr->NEXT_TR_ELEM_PTR & USBHS_TD_ADDR_MASK);
}

static uint32_t usb_hal_ehci_get_dtd_error_status(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	return (dtdAddr->SIZE_IOC_STS & USBHS_TD_ERROR_MASK);
}

static void usb_hal_ehci_clear_qh_error_status(uint32_t instance, usb_ehc_dev_qh_struct_t * epQueueHeadAddr, uint32_t errors)
{
	epQueueHeadAddr->SIZE_IOC_INT_STS &= ~errors;
}

static uint32_t usb_hal_ehci_get_tr_packet_size(uint32_t instance,usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	return ((dtdAddr->SIZE_IOC_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);
}

static uint32_t usb_hal_ehci_get_xd_for_this_dtd(uint32_t instance, usb_ehci_dev_dtd_struct_t * dtdAddr)
{
	return (uint32_t)dtdAddr->xd_for_this_dtd;
}

static void usb_hal_ehci_clear_SIZE_IOC_INT_STS(uint32_t instance, usb_ehc_dev_qh_struct_t * epQueueHeadAddr)
{
	epQueueHeadAddr->SIZE_IOC_INT_STS = 0;
}
#endif



#endif

#endif
