/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013- 2014 Freescale Semiconductor;
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
* $FileName: hid_descriptor.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief The file contains USB descriptors for the S08 stack
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#if 2
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "hid_descriptor.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/ 
usb_ep_struct_t g_ep[HID_DESC_ENDPOINT_COUNT] = 
{
    HID_ENDPOINT, 
    USB_INTERRUPT_PIPE, 
    USB_SEND,
    HID_ENDPOINT_PACKET_SIZE
};

/* structure containing details of all the endpoints used by this device */ 
usb_endpoints_t g_usb_desc_ep =
{
    HID_DESC_ENDPOINT_COUNT,
    g_ep
};
 
static usb_if_struct_t g_usb_if[1] ;

usb_class_struct_t g_usb_dec_class =
{
   USB_CLASS_HID,
   {
        1,
        g_usb_if
    }
};

uint8_t ghid_device_descriptor[DEVICE_DESCRIPTOR_SIZE] =
{
   /* "Device Dexcriptor Size */
   DEVICE_DESCRIPTOR_SIZE,          /*bLength:0x12 */
   /* "Device" Type of descriptor */
   USB_DEVICE_DESCRIPTOR,           /*bDescriptorType:1*/
   /*  BCD USB version  */  
   USB_uint_16_low(BCD_USB_VERSION), USB_uint_16_high(BCD_USB_VERSION),
   /* Device Class is indicated in the interface descriptors */   
   DEVICE_DESC_DEVICE_CLASS,        /*bcdUSB:0x02->0x00*/
   /*  Device Subclass is indicated in the interface descriptors  */      
   DEVICE_DESC_DEVICE_SUBCLASS,
   /*  Device Protocol  */
   DEVICE_DESC_DEVICE_PROTOCOL,
   /* Max Packet size */
   CONTROL_MAX_PACKET_SIZE,
    /* Vendor ID */
    0x04,0x25,
    /* Product ID */
    0x00,0x01,
    /* BCD Device version */
    0x02,0x00,
    /* Manufacturer string index */
    0x01,
    /* Product string index */  
    0x02,
    /*  Serial number string index */
    0x00,
    /*  Number of configurations */
    DEVICE_DESC_NUM_CONFIG_SUPPOTED
};

uint8_t ghid_config_descriptor[CONFIG_DESC_SIZE] =
{
    /* Configuration Descriptor Size - always 9 bytes*/
    CONFIG_ONLY_DESC_SIZE,  
    /* "Configuration" type of descriptor */
    USB_CONFIG_DESCRIPTOR,  
    /*  Total length of the Configuration descriptor */
    USB_uint_16_low(CONFIG_DESC_SIZE),USB_uint_16_high(CONFIG_DESC_SIZE),
    /*  NumInterfaces */   
    CONFIG_DESC_NUM_INTERFACES_SUPPOTED,
    /*  Configuration Value */
    1,
    /* Configuration Description String Index */   
    0,
    /*  Attributes.support RemoteWakeup and self power */
    (USBCFG_DEV_SELF_POWER << USB_DESC_CFG_ATTRIBUTES_SELF_POWERED_SHIFT) | (USBCFG_DEV_REMOTE_WAKEUP << USB_DESC_CFG_ATTRIBUTES_REMOTE_WAKEUP_SHIFT),
    /*  Current draw from bus */
    CONFIG_DESC_CURRENT_DRAWN, 

    /* Interface Descriptor */
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    0x00,
    0x00,
    HID_DESC_ENDPOINT_COUNT,
    0x03,
    0x01,
    0x02,
    0x00,

    /* HID descriptor */
    HID_ONLY_DESC_SIZE, 
    USB_HID_DESCRIPTOR,
    0x00,0x01,
    0x00,
    0x01,
    0x22,
    0x34,0x00,

    /*Endpoint descriptor */
    ENDP_ONLY_DESC_SIZE, 
    USB_ENDPOINT_DESCRIPTOR,
    HID_ENDPOINT|(USB_SEND << 7),
    USB_INTERRUPT_PIPE, 
    HID_ENDPOINT_PACKET_SIZE, 0x00, 
    0x0A
};

#if HIGH_SPEED_DEVICE
    uint8_t  g_device_qualifier_descriptor[DEVICE_QUALIFIER_DESCRIPTOR_SIZE] =
    {
        /* Device Qualifier Descriptor Size */
        DEVICE_QUALIFIER_DESCRIPTOR_SIZE, 
        /* Type of Descriptor */
        USB_DEVQUAL_DESCRIPTOR,
        /*  BCD USB version  */
        USB_uint_16_low(BCD_USB_VERSION), USB_uint_16_high(BCD_USB_VERSION),
        /* bDeviceClass */
        DEVICE_DESC_DEVICE_CLASS,
        /* bDeviceSubClass */
        DEVICE_DESC_DEVICE_SUBCLASS,
        /* bDeviceProtocol */
        DEVICE_DESC_DEVICE_PROTOCOL,
        /* bMaxPacketSize0 */
        CONTROL_MAX_PACKET_SIZE,
        /* bNumConfigurations */
        DEVICE_OTHER_DESC_NUM_CONFIG_SUPPOTED,  
        /* Reserved : must be zero */ 
        0x00
    };

    uint8_t  g_other_speed_config_descriptor[OTHER_SPEED_CONFIG_DESCRIPTOR_SIZE] =
    {
        /* Length of this descriptor */
        CONFIG_ONLY_DESC_SIZE,     
        /* This is a Other speed config descr */
        USB_OTHER_SPEED_DESCRIPTOR,
        /*  Total length of the Configuration descriptor */
        USB_uint_16_low(CONFIG_DESC_SIZE), USB_uint_16_high(CONFIG_DESC_SIZE),
        CONFIG_DESC_NUM_INTERFACES_SUPPOTED,
        /*value used to selct this configuration : Configuration Value */
        1, 
        /*  Configuration Description String Index*/   
        0, 
        /*  Attributes.support RemoteWakeup and self power */
        (USBCFG_DEV_SELF_POWER << USB_DESC_CFG_ATTRIBUTES_SELF_POWERED_SHIFT) | (USBCFG_DEV_REMOTE_WAKEUP << USB_DESC_CFG_ATTRIBUTES_REMOTE_WAKEUP_SHIFT),    
        /*  Current draw from bus */
        CONFIG_DESC_CURRENT_DRAWN, 

        /* Interface Descriptor */
        IFACE_ONLY_DESC_SIZE,
        USB_IFACE_DESCRIPTOR,
        0x00,
        0x00,
        HID_DESC_ENDPOINT_COUNT,
        0x03,
        0x01,
        0x02,
        0x00,

        /* HID descriptor */
        HID_ONLY_DESC_SIZE, 
        USB_HID_DESCRIPTOR,
        0x00,0x01,
        0x00,
        0x01,
        0x22,
        0x34,0x00,
         
        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE, 
        USB_ENDPOINT_DESCRIPTOR,
        HID_ENDPOINT|(USB_SEND << 7),
        USB_INTERRUPT_PIPE, 
        HID_ENDPOINT_PACKET_SIZE, 0x00, 
        0x0A
    };
#endif

uint8_t g_report_descriptor[REPORT_DESC_SIZE] =
{
    0x05, 0x01,   /* Usage Page (Generic Desktop)*/
    0x09, 0x02,   /* Usage (Mouse) */
    0xA1, 0x01,   /* Collection (Application) */
    0x09, 0x01,   /* Usage (Pointer) */

    0xA1, 0x00,   /* Collection (Physical) */
    0x05, 0x09,   /* Usage Page (Buttons) */
    0x19, 0x01,   /* Usage Minimun (01) */
    0x29, 0x03,   /* Usage Maximum (03) */

    0x15, 0x00,   /* logical Minimun (0) */
    0x25, 0x01,   /* logical Maximum (1) */
    0x95, 0x03,   /* Report Count (3) */
    0x75, 0x01,   /* Report Size (1) */

    0x81, 0x02,   /* Input(Data, Variable, Absolute) 3 button bits */
    0x95, 0x01,   /* Report count (1) */
    0x75, 0x05,   /* Report Size (5) */
    0x81, 0x01,   /* Input (Constant), 5 bit padding */

    0x05, 0x01,   /* Usage Page (Generic Desktop) */
    0x09, 0x30,   /* Usage (X) */
    0x09, 0x31,   /* Usage (Y) */
    0x09, 0x38,   /* Usage (Z) */

    0x15, 0x81,   /* Logical Minimum (-127) */
    0x25, 0x7F,   /* Logical Maximum (127) */
    0x75, 0x08,   /* Report Size (8) */
    0x95, 0x03,   /* Report Count (2) */

    0x81, 0x06,   /* Input(Data, Variable, Relative), 2 position bytes (X & Y)*/
    0xC0,         /* end collection */
    0xC0          /* end collection */
};

/* number of strings in the table not including 0 or n. */
//const uint8_t USB_STR_NUM = 3;

uint8_t g_usb_str_0[USB_STR_0_SIZE+USB_STR_DESC_SIZE] = 
{ 
    sizeof(g_usb_str_0),    
    USB_STRING_DESCRIPTOR, 
    0x09,
    0x04/*equiavlent to 0x0409*/ 
};

uint8_t g_usb_str_1[USB_STR_1_SIZE+USB_STR_DESC_SIZE] =
{  
    sizeof(g_usb_str_1),
    USB_STRING_DESCRIPTOR,
    'F',0,
    'R',0,
    'E',0,
    'E',0,
    'S',0,
    'C',0,
    'A',0,
    'L',0,
    'E',0,
    ' ',0,
    'S',0,
    'E',0,
    'M',0,
    'I',0,
    'C',0,
    'O',0,
    'N',0,
    'D',0,
    'U',0,
    'C',0,
    'T',0,
    'O',0,
    'R',0,
    ' ',0,
    'I',0,
    'N',0,
    'C',0,
    '.',0
};


uint8_t g_usb_str_2[USB_STR_2_SIZE+USB_STR_DESC_SIZE] =
{  
    sizeof(g_usb_str_2),
    USB_STRING_DESCRIPTOR,
    'M',0,
    'C',0,
    'U',0,
    ' ',0,
    'M',0,
    'O',0,
    'U',0,
    'S',0,
    'E',0,
    ' ',0,
    'D',0,
    'E',0,
    'M',0,
    'O',0
};

uint8_t g_usb_str_n[USB_STR_n_SIZE+USB_STR_DESC_SIZE] =
{  
    sizeof(g_usb_str_n),
    USB_STRING_DESCRIPTOR,
    'B',0,
    'A',0,
    'D',0,
    ' ',0,
    'S',0,
    'T',0,
    'R',0,
    'I',0,
    'N',0,
    'G',0
};

uint8_t ghid_std_desc_size[USB_MAX_STD_DESCRIPTORS+1] =
{ 
    0,
    DEVICE_DESCRIPTOR_SIZE,
    CONFIG_DESC_SIZE,
    0, /* string */
    0, /* Interfdace */
    0, /* Endpoint */
    #if HIGH_SPEED_DEVICE
        DEVICE_QUALIFIER_DESCRIPTOR_SIZE,
        OTHER_SPEED_CONFIG_DESCRIPTOR_SIZE,
    #else
        0, /* Device Qualifier */
        0, /* other spped config */
    #endif
    REPORT_DESC_SIZE
};   

uint8_t *ghid_std_descriptors[USB_MAX_STD_DESCRIPTORS+1] = 
{
    NULL,
    ghid_device_descriptor,
    ghid_config_descriptor,
    NULL, /* string */
    NULL, /* Interfdace */
    NULL, /* Endpoint */
    #if HIGH_SPEED_DEVICE
        g_device_qualifier_descriptor,
        g_other_speed_config_descriptor,
    #else
        NULL, /* Device Qualifier */
        NULL, /* other spped config*/
    #endif
    g_report_descriptor
}; 
   
uint8_t ghid_string_desc_size[USB_MAX_STRING_DESCRIPTORS+1] = 
{
    sizeof(g_usb_str_0),
    sizeof(g_usb_str_1),
    sizeof(g_usb_str_2),
    sizeof(g_usb_str_n)
};   

uint8_t *ghid_string_descriptors[USB_MAX_STRING_DESCRIPTORS+1] = 
{ 
    g_usb_str_0,
    g_usb_str_1,
    g_usb_str_2,
    g_usb_str_n
};
usb_language_t g_usb_lang[USB_MAX_SUPPORTED_LANGUAGES] = 
{
    { 
        (uint16_t)0x0409,
        ghid_string_descriptors,
        ghid_string_desc_size
    }
};
usb_all_languages_t ghid_languages = 
{
    g_usb_str_0, sizeof(g_usb_str_0),
    USB_MAX_SUPPORTED_LANGUAGES,
    g_usb_lang
};

//uint8_t g_valid_config_values[USB_MAX_CONFIG_SUPPORTED+1]={0,1};

/****************************************************************************
 * Global Variables
 ****************************************************************************/
static uint8_t g_alternate_interface[USB_MAX_SUPPORTED_INTERFACES];

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes 
 *****************************************************************************/

/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/
 
 /*****************************************************************************
 * Local Functions - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  HID_Desc_Get_Descriptor
 *
 * @brief The function returns the correponding descriptor
 *
 * @param handle:        handle     
 * @param type:          type of descriptor requested     
 * @param sub_type:      string index for string descriptor     
 * @param index:         string descriptor language Id     
 * @param descriptor:    output descriptor pointer
 * @param size:          size of descriptor returned
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t HID_Desc_Get_Descriptor 
(
    hid_handle_t handle, 
    uint8_t type,
    uint8_t str_num, 
    uint16_t index,
    uint8_t * *descriptor,
    uint32_t *size
) 
{
    UNUSED_ARGUMENT (handle)
    
    switch(type)  
    {
        case USB_REPORT_DESCRIPTOR: 
            {
                type = USB_MAX_STD_DESCRIPTORS;
                *descriptor = (uint8_t *)ghid_std_descriptors [type];
                *size =  ghid_std_desc_size[type]; 
            }
            break;
        case USB_HID_DESCRIPTOR: 
            {
                type = USB_CONFIG_DESCRIPTOR ; 
                *descriptor = (uint8_t *)(ghid_std_descriptors [type]+
                    CONFIG_ONLY_DESC_SIZE+IFACE_ONLY_DESC_SIZE);
                *size = HID_ONLY_DESC_SIZE;
            }
        break;
        case USB_STRING_DESCRIPTOR: 
            {
                if (index == 0) 
                {  
                    /* return the string and size of all languages */      
                    *descriptor = 
                        (uint8_t *)ghid_languages .languages_supported_string;
                    *size = ghid_languages .languages_supported_size;            
                } 
                else 
                {
                    uint8_t lang_id=0;
                    uint8_t lang_index=USB_MAX_LANGUAGES_SUPPORTED;
                
                    for (;lang_id< USB_MAX_LANGUAGES_SUPPORTED;lang_id++) 
                    {
                        /* check whether we have a string for this language */
                        if(index == 
                            ghid_languages .usb_language[lang_id].language_id) 
                        {   /* check for max descriptors */
                            if(str_num < USB_MAX_STRING_DESCRIPTORS) 
                            {   /* setup index for the string to be returned */
                                lang_index=str_num;
                            }
                            break;
                        }
                    }
                    /* set return val for descriptor and size */
                    *descriptor = (uint8_t *)
                        ghid_languages .usb_language[lang_id].lang_desc[lang_index];
                    *size = 
                        ghid_languages .usb_language[lang_id].
                            lang_desc_size[lang_index];
                }
            }
            break;
        default :
            if (type < USB_MAX_STD_DESCRIPTORS)
            {
                /* set return val for descriptor and size*/
                *descriptor = (uint8_t *)ghid_std_descriptors [type];

                /* if there is no descriptor then return error */
                *size = ghid_std_desc_size[type];

                if(*descriptor == NULL) 
                {
                    return USBERR_INVALID_REQ_TYPE;
                }
            }
            else /* invalid descriptor */
            {
                return USBERR_INVALID_REQ_TYPE;
            }
            break; 
    }/* End Switch */
    return USB_OK;  
}

/**************************************************************************//*!
 *
 * @name HID_Desc_Get_Interface 
 *
 * @brief The function returns the alternate interface
 *
 * @param handle:         handle     
 * @param interface:      interface number     
 * @param alt_interface:  output alternate interface     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t HID_Desc_Get_Interface 
(
    hid_handle_t handle, 
    uint8_t interface, 
    uint8_t * alt_interface
)
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get alternate interface*/
        *alt_interface = g_alternate_interface[interface];  
        return USB_OK;  
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name HID_Desc_Set_Interface 
 *
 * @brief The function sets the alternate interface
 *
 * @param handle:         handle     
 * @param interface:      interface number
 * @param alt_interface:  input alternate interface
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t HID_Desc_Set_Interface 
(
    hid_handle_t handle, 
    uint8_t interface, 
    uint8_t alt_interface
)
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set alternate interface*/
        g_alternate_interface[interface]=alt_interface;
        return USB_OK;  
    }    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name HID_Set_Configation 
 *
 * @brief The function checks whether the configuration parameter 
 *        input is valid or not
 *
 * @param handle          handle     
 * @param config_val      configuration value     
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************/
uint8_t HID_Set_Configation 
(
    hid_handle_t handle, uint8_t config
) 
{
    UNUSED_ARGUMENT (handle)

    return USB_OK;
}

//usb_if_struct_t usb_qq[5] = {1,usb_desc_ep};
/**************************************************************************//*!
 *
 * @name  USB_Desc_Remote_Wakeup
 *
 * @brief The function checks whether the remote wakeup is supported or not
 *
 * @param handle            handle     
 *
 * @return REMOTE_WAKEUP_SUPPORT (TRUE) - if remote wakeup supported
 *****************************************************************************/
uint8_t HID_Desc_Get_Entity(hid_handle_t handle, entity_type type, uint32_t * object)
{
    switch (type)  
    {
        case USB_CLASS_INFO:
            g_usb_if[0].index = 1;
            g_usb_if[0].endpoints = g_usb_desc_ep;
            *object = (unsigned long)&g_usb_dec_class;
            break;
        default :
            break; 
    }/* End Switch */
    return USB_OK;  
}

usb_desc_request_notify_struct_t  g_desc_callback = 
{
    HID_Desc_Get_Descriptor,
    HID_Desc_Get_Interface,
    HID_Desc_Set_Interface,
    HID_Set_Configation,
    HID_Desc_Get_Entity
};
#endif
/* EOF */
