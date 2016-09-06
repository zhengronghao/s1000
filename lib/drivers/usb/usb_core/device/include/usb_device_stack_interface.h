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
* $FileName: usb_device_stack_interface.h$
* $Version : 
* $Date    : 
*
* Comments:
*
* 
*
*END************************************************************************/
/* Prototypes */
#ifndef __usb_device_stack_interface_h__
#define __usb_device_stack_interface_h__

#include "usb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Informational Request/Set Types */
#define  USB_STATUS_DEVICE_STATE               (0x01)
#define  USB_STATUS_INTERFACE                  (0x02)
#define  USB_STATUS_ADDRESS                    (0x03)
#define  USB_STATUS_CURRENT_CONFIG             (0x04)
#define  USB_STATUS_SOF_COUNT                  (0x05)
#define  USB_STATUS_DEVICE                     (0x06)
#define  USB_STATUS_TEST_MODE                  (0x07)
#ifdef USBCFG_OTG
#define  USB_STATUS_OTG                        (0x08)
#endif
#define  USB_STATUS_ENDPOINT                   (0x10)
#define  USB_STATUS_ENDPOINT_NUMBER_MASK       (0x0F)

#define  USB_TEST_MODE_TEST_PACKET             (0x0400)

/* Available service types */
/* Services 0 through 15 are reserved for endpoints */
#define  USB_SERVICE_EP0                       (0x00)
#define  USB_SERVICE_EP1                       (0x01)
#define  USB_SERVICE_EP2                       (0x02)
#define  USB_SERVICE_EP3                       (0x03)
#define  USB_SERVICE_BUS_RESET                 (0x10)
#define  USB_SERVICE_SUSPEND                   (0x11)
//#define  USB_SERVICE_SOF                                   (0x12)
#define  USB_SERVICE_RESUME                    (0x13)
#define  USB_SERVICE_SLEEP                     (0x14)
#define  USB_SERVICE_SPEED_DETECTION           (0x15)
#define  USB_SERVICE_ERROR                     (0x16)
//#define  USB_SERVICE_STALL                                (0x17)
#define  USB_SERVICE_REQUEST                   (0x18)

#define  USB_CONTROL_ENDPOINT                  (0)
#define  USB_SETUP_PKT_SIZE                    (8)/* Setup Packet Size */
#define  USB_UNINITIALIZED_VAL_32              (0xFFFFFFFF)


#define  USB_DEV_EVENT_BUS_RESET               (0)
#define  USB_DEV_EVENT_CONFIG_CHANGED          (1)
#define  USB_DEV_EVENT_INTERFACE_CHANGED       (2)
#define  USB_DEV_EVENT_ENUM_COMPLETE           (3)
#define  USB_DEV_EVENT_SEND_COMPLETE           (4)
#define  USB_DEV_EVENT_DATA_RECEIVED           (5)
#define  USB_DEV_EVENT_ERROR                   (6)
#define  USB_DEV_EVENT_GET_DATA_BUFF           (7)
#define  USB_DEV_EVENT_EP_STALLED              (8)
#define  USB_DEV_EVENT_EP_UNSTALLED            (9)
#define  USB_DEV_EVENT_GET_TRANSFER_SIZE       (0x10)
#define  USB_DEV_EVENT_TYPE_SET_REMOTE_WAKEUP  (0x11)
#define  USB_DEV_EVENT_TYPE_CLR_REMOTE_WAKEUP  (0x12)
#define  USB_DEV_EVENT_TYPE_SET_EP_HALT        (0x13)
#define  USB_DEV_EVENT_TYPE_CLR_EP_HALT        (0x14)

/* Macros for description of class, configuration, interface */
#define  USB_DESC_INTERFACE(index, ep_cnt, ep) \
{ \
    index, \
    { \
        ep_cnt, \
        ep \
    } \
}
#define  USB_DESC_CONFIGURATION(intf_cnt, intf) \
{ \
    intf_cnt, \
    intf \
}
#define  USB_DESC_CLASS(type, config) \
{ \
    type, \
    config, \
}

/* Go through each endpoint in class */
#define  for_each_ep_in_class(epPtr, usbclassPtr, classtype) \
    for(uint32_t class_i = 0; usbclassPtr[class_i].type != USB_CLASS_INVALID; class_i++) \
        if((usbclassPtr[class_i].type == classtype) || (classtype == USB_CLASS_ALL)) \
        for(uint32_t intf_i = 0; intf_i < usbclassPtr[class_i].interfaces.count; intf_i++) \
            for(uint32_t ep_i = 0; \
                ((ep_i < usbclassPtr[class_i].interfaces.interface[intf_i].endpoints.count) && \
                ((epPtr = usbclassPtr[class_i].interfaces.interface[intf_i].endpoints.ep + ep_i) != NULL)); \
                ep_i++)

/* Go through each interface in class */
#define  for_each_if_in_class(ifPtr, usbclassPtr, classtype) \
                for(uint32_t class_i = 0; usbclassPtr[class_i].type != USB_CLASS_INVALID; class_i++) \
                    if((usbclassPtr[class_i].type == classtype) || (classtype == USB_CLASS_ALL)) \
                    for(uint32_t intf_i = 0; ((intf_i < usbclassPtr[class_i].interfaces.count) \
                        && ((ifPtr = usbclassPtr[class_i].interfaces.interface + intf_i) != NULL)); \
                        intf_i++)

typedef enum {
    USB_CLASS_INFO  = 0,
    USB_COMPOSITE_INFO,    
    USB_AUDIO_UNITS,            
    USB_RNDIS_INFO,
    USB_PHDC_QOS_INFO,
    USB_MSC_LBA_INFO,
} entity_type;

typedef enum {
    USB_CLASS_HID  = 0,
    USB_CLASS_CDC,    
    USB_CLASS_MSC,         
    USB_CLASS_AUDIO,          
    USB_CLASS_PHDC,
    USB_CLASS_ALL,
    USB_CLASS_INVALID
} class_type;

typedef struct _usb_ep_struct
{
    uint8_t               ep_num;      /* endpoint number         */
    uint8_t               type;        /* type of endpoint        */
    uint8_t               direction;   /* direction of endpoint   */
    uint32_t              size;        /* buffer size of endpoint */
} usb_ep_struct_t;

/* Strucutre Representing Endpoints and number of endpoints user want*/
typedef struct _usb_endpoints
{
    uint8_t               count;
    usb_ep_struct_t*      ep; 
} usb_endpoints_t;

/* Strucutre Representing interface*/
typedef struct _usb_if_struct
{
    uint8_t               index;
    usb_endpoints_t       endpoints;
} usb_if_struct_t;

/* Strucutre Representing how many interfaces in one class type*/
typedef struct _usb_interfaces_struct
{
    uint8_t               count;
    usb_if_struct_t*      interface;
} usb_interfaces_struct_t;

/* Strucutre Representing class info*/
typedef struct _usb_class_struct
{
    class_type              type;
    usb_interfaces_struct_t interfaces;
} usb_class_struct_t;

/* Strucutre Representing composite info*/
typedef struct _usb_composite_info_struct
{
    uint8_t               count;
    usb_class_struct_t*   class;
} usb_composite_info_struct_t;

/* Common Data Structures */
typedef struct _usb_setup_struct
{
    uint8_t               request_type;
    uint8_t               request;
    uint16_t              value;
    uint16_t              index;
    uint16_t              length;
} usb_setup_struct_t;

/* USB Specs define CONTROL_MAX_PACKET_SIZE for High Speed device as only 64,
   whereas for FS its allowed to be 8, 16, 32 or 64 */
#define CONTROL_MAX_PACKET_SIZE       (64)

#if (HIGH_SPEED_DEVICE && (CONTROL_MAX_PACKET_SIZE != 64))
#error "For High Speed CONTROL_MAX_PACKET_SIZE should be 64"
#endif

typedef struct _usb_event_struct
{
    usb_device_handle     handle;             /* conttroler device handle*/
    uint8_t*              buffer_ptr;         /* void* to buffer       */
    uint32_t              len;                /* the buffer len had been done */
                                              /* special case: 0xFFFFFFFF means transfer cancel
                                              0xFFFFFFFE means tansfer error */
    uint8_t               ep_num;             /* endpoint number */
    uint8_t               type;
    bool                  setup;              /* is setup packet         */
    bool                  direction;          /* direction of endpoint   */
} usb_event_struct_t;

/* callback function pointer structure for Application to handle events */
typedef void(_CODE_PTR_ usb_device_notify_t)(uint8_t event, void* val, void* arg);


/* callback function pointer structure to handle USB framework request */
typedef usb_status (_CODE_PTR_ usb_request_notify_t)(usb_setup_struct_t *,
                                          uint8_t **,
                                          uint32_t*,void* arg);

typedef void(_CODE_PTR_ usb_event_service_t)(usb_event_struct_t*, void*);


typedef struct _usb_desc_request_notify_struct
{
#ifdef USBCFG_OTG
        uint32_t handle;
#endif
    uint8_t (_CODE_PTR_ get_desc)(uint32_t handle,uint8_t type,uint8_t desc_index,
        uint16_t index,uint8_t * *descriptor,uint32_t *size);  
    uint8_t (_CODE_PTR_ get_desc_interface)(uint32_t handle,uint8_t interface,
        uint8_t * alt_interface);
    uint8_t (_CODE_PTR_ set_desc_interface)(uint32_t handle,uint8_t interface,
        uint8_t alt_interface);
    uint8_t (_CODE_PTR_ set_configuration)(uint32_t handle, uint8_t config);  
    uint8_t (_CODE_PTR_ get_desc_entity)(uint32_t handle, entity_type type, uint32_t * object);
} usb_desc_request_notify_struct_t;

/* Structure application request class callback */
typedef struct usb_application_callback_struct
{
    usb_device_notify_t     callback;
    void*                 arg;
}usb_application_callback_struct_t;

/* Structure vendor request class callback */
typedef struct usb_vendor_req_callback_struct
{
    usb_request_notify_t  callback;
    void*                 arg;
}usb_vendor_req_callback_struct_t;

extern usb_status usb_device_init(uint8_t, usb_device_handle * );
extern usb_status usb_device_deinit(usb_device_handle);
extern usb_status usb_device_recv_data(usb_device_handle, uint8_t, uint8_t *, uint32_t);
extern usb_status usb_device_send_data(usb_device_handle, uint8_t, uint8_t *, uint32_t);
#if USBCFG_DEV_ADVANCED_CANCEL_ENABLE
extern usb_status usb_device_cancel_transfer(usb_device_handle, uint8_t, uint8_t);
#endif
extern usb_status usb_device_register_service(usb_device_handle, uint8_t, usb_event_service_t, void* arg);
extern usb_status usb_device_unregister_service(usb_device_handle, uint8_t);
#if USBCFG_DEV_ADVANCED_SUSPEND_RESUME
extern usb_status usb_device_assert_resume(usb_device_handle);
#endif
extern usb_status usb_device_init_endpoint(usb_device_handle, usb_ep_struct_t*, uint8_t);
extern usb_status usb_device_stall_endpoint(usb_device_handle, uint8_t, uint8_t);
extern usb_status usb_device_unstall_endpoint(usb_device_handle, uint8_t, uint8_t);
extern usb_status usb_device_deinit_endpoint(usb_device_handle, uint8_t, uint8_t);
extern usb_status usb_device_register_application_notify(usb_device_handle, usb_device_notify_t, void*);
extern usb_status usb_device_register_vendor_class_request_notify(usb_device_handle, usb_request_notify_t, void*);
extern usb_status usb_device_register_desc_request_notify(usb_device_handle,usb_desc_request_notify_struct_t*, void*);
extern usb_status usb_device_get_status(usb_device_handle, uint8_t, uint16_t*);
extern usb_status usb_device_set_status(usb_device_handle, uint8_t, uint16_t);
#ifdef USBCFG_OTG
extern usb_status usb_device_otg_init(usb_device_handle, uint8_t);
extern usb_status usb_device_otg_get_hnp_support(usb_device_handle, uint8_t*);
extern usb_status usb_device_otg_set_hnp_enable(usb_device_handle);
#endif
#ifdef __cplusplus
}
#endif

#endif
