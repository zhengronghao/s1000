/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: usb_class_cdc.h$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief The file contains USB stack CDC class layer api header function.
*
*****************************************************************************/

#ifndef _USB_CLASS_CDC_H
#define _USB_CLASS_CDC_H 1

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class.h"

/******************************************************************************
 * Macro's
 *****************************************************************************/
/* Class specific request Codes */
#define SEND_ENCAPSULATED_COMMAND       (0x00)
#define GET_ENCAPSULATED_RESPONSE       (0x01)
#define SET_COMM_FEATURE                (0x02)
#define GET_COMM_FEATURE                (0x03)
#define CLEAR_COMM_FEATURE              (0x04)
#define SET_AUX_LINE_STATE              (0x10)
#define SET_HOOK_STATE                  (0x11)
#define PULSE_SETUP                     (0x12)
#define SEND_PULSE                      (0x13)
#define SET_PULSE_TIME                  (0x14)
#define RING_AUX_JACK                   (0x15)
#define SET_LINE_CODING                 (0x20)
#define GET_LINE_CODING                 (0x21)
#define SET_CONTROL_LINE_STATE          (0x22)
#define SEND_BREAK                      (0x23)
#define SET_RINGER_PARAMS               (0x30)
#define GET_RINGER_PARAMS               (0x31)
#define SET_OPERATION_PARAM             (0x32)
#define GET_OPERATION_PARAM             (0x33)
#define SET_LINE_PARAMS                 (0x34)
#define GET_LINE_PARAMS                 (0x35)
#define DIAL_DIGITS                     (0x36)
#define SET_UNIT_PARAMETER              (0x37)
#define GET_UNIT_PARAMETER              (0x38)
#define CLEAR_UNIT_PARAMETER            (0x39)
#define GET_PROFILE                     (0x3A)
#define SET_ETHERNET_MULTICAST_FILTERS  (0x40)
#define SET_ETHERNET_POW_PATTER_FILTER  (0x41)
#define GET_ETHERNET_POW_PATTER_FILTER  (0x42)
#define SET_ETHERNET_PACKET_FILTER      (0x43)
#define GET_ETHERNET_STATISTIC          (0x44)
#define SET_ATM_DATA_FORMAT             (0x50)
#define GET_ATM_DEVICE_STATISTICS       (0x51)
#define SET_ATM_DEFAULT_VC              (0x52)
#define GET_ATM_VC_STATISTICS           (0x53)
#define MDLM_SPECIFIC_REQUESTS_MASK     (0x7F)

#define GET_ABSTRACT_STATE              (0x23)
#define GET_COUNTRY_SETTING             (0x24)
#define SET_ABSTRACT_STATE              (0x25)
#define SET_COUNTRY_SETTING             (0x26)
/* Class Specific Notification Codes */
#define NETWORK_CONNECTION_NOTIF        (0x00)
#define RESPONSE_AVAIL_NOTIF            (0x01)
#define AUX_JACK_HOOK_STATE_NOTIF       (0x08)
#define RING_DETECT_NOTIF               (0x09)
#define SERIAL_STATE_NOTIF              (0x20)
#define CALL_STATE_CHANGE_NOTIF         (0x28)
#define LINE_STATE_CHANGE_NOTIF         (0x29)
#define CONNECTION_SPEED_CHANGE_NOTIF   (0x2A)
/* Events to the Application */ /* 0 to 4 are reserved for class events */
#define USB_APP_CDC_CARRIER_DEACTIVATED (0x61)
#define USB_APP_CDC_CARRIER_ACTIVATED   (0x62)
#define USB_APP_CDC_DTE_DEACTIVATED     (0x63)
#define USB_APP_CDC_DTE_ACTIVATED       (0x64)
#define USB_APP_GET_LINK_SPEED          (0x65)    
#define USB_APP_GET_LINK_STATUS         (0x66)  

/* Exception */
#define USB_REQ_VAL_INVALID             (0xFFFF)
extern void USB_Class_Periodic_Task(void);
#define USB_CDC_Periodic_Task USB_Class_Periodic_Task                          
/******************************************************************************
 * Types
 *****************************************************************************/
/* Type representing CDC class handle*/ 
typedef uint32_t  cdc_handle_t;
typedef uint32_t  _ip_address;
typedef uint8_t   enet_address_t[6];
typedef struct _cdc_config_struct
{
   usb_application_callback_struct_t        cdc_application_callback;
   usb_vendor_req_callback_struct_t         vendor_req_callback;
   usb_class_specific_callback_struct_t     class_specific_callback;
   usb_desc_request_notify_struct_t*        desc_callback_ptr;
}cdc_config_struct_t;

/* Data structure for app */
typedef struct _cdc_app_data_struct
{
    uint8_t*                                  data_ptr;     /* pointer to buffer       */     
    uint32_t                                  data_size;    /* buffer size of endpoint */
}cdc_app_data_struct_t;

typedef struct _usb_rndis_info_struct
{
    enet_address_t mac_address;
    _ip_address   ip_address;
    uint32_t      rndis_max_frame_size; 
} usb_rndis_info_struct_t;
/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param *cdc_config_ptr[IN]:  This structure contians configuration parameter
 *                              send by APP to configure CDC class.
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the CDC Class layer and layers it is dependednt on 
 *
 *****************************************************************************/
extern uint32_t USB_Class_CDC_Init
(
    uint8_t controller_id,
    cdc_config_struct_t * cdc_config_ptr,
    cdc_handle_t *    cdc_handle_ptr
);
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Deinit
 *
 * @brief The funtion deinitializes the Device and Controller layer 
 *
 * @param cdc_handle:  
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the CDC Class layer and layers it is dependednt on 
 *
 *****************************************************************************/
extern uint32_t USB_Class_CDC_Deinit
(
  cdc_handle_t cdc_handle
);
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Send_Data
 *
 * @brief  This function sends data to Host.
 *
 * @param handle          :   handle returned by USB_Class_CDC_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK         : When Successfull 
 *         Others         : Errors
 *****************************************************************************/
extern usb_status USB_Class_CDC_Send_Data
(
    cdc_handle_t          cdc_handle,
    uint8_t             ep_num,
    uint8_t*            buff_ptr,      /* [IN] buffer to send */      
    uint32_t            size           /* [IN] length of the transfer */
);
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Recv_Data
 *
 * @brief This functions receives Data from Host.
 *
 * @param handle          :   handle returned by USB_Class_CDC_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK         : When Successfull 
 *         Others         : Errors
 *****************************************************************************/
extern usb_status USB_Class_CDC_Recv_Data
(
    cdc_handle_t          cdc_handle,
    uint8_t             ep_num,
    uint8_t*            buff_ptr,      /* [IN] buffer to send */      
    uint32_t            size           /* [IN] length of the transfer */
);

#ifdef __cplusplus
}
#endif
#endif

