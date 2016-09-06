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
* $FileName: usb_descriptor.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief The file contains USB descriptors 
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "usb_descriptor.h"


/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/
/* structure containing details of all the endpoints used by this device */
usb_ep_struct_t cic_ep[CIC_ENDP_COUNT] =  {
        #if CIC_NOTIF_ELEM_SUPPORT
        {
            CIC_NOTIF_ENDPOINT, 
            USB_INTERRUPT_PIPE, 
            USB_SEND,
            CIC_NOTIF_ENDP_PACKET_SIZE
        }
        #endif
};

usb_ep_struct_t dic_ep[DIC_ENDP_COUNT] =  {
        #if DATA_CLASS_SUPPORT
            {
                DIC_BULK_IN_ENDPOINT,
                USB_BULK_PIPE, 
                USB_SEND,
                DIC_BULK_IN_ENDP_PACKET_SIZE
            }, 
            {
                DIC_BULK_OUT_ENDPOINT,
                USB_BULK_PIPE, 
                USB_RECV,
                DIC_BULK_OUT_ENDP_PACKET_SIZE
            } 
        #endif
};

#define USB_CDC_IF_MAX 2
#define USB_CDC_CFG_MAX 1
#define USB_CDC_CLASS_MAX 2
static usb_if_struct_t usb_if[USB_CDC_IF_MAX] = {
    USB_DESC_INTERFACE(0, CIC_ENDP_COUNT, cic_ep),
    USB_DESC_INTERFACE(1, DIC_ENDP_COUNT, dic_ep),
};

static usb_interfaces_struct_t usb_configuration[USB_CDC_CFG_MAX] = {
    USB_DESC_CONFIGURATION(USB_CDC_IF_MAX, usb_if),
};

static usb_class_struct_t usb_dec_class[USB_CDC_CLASS_MAX] =
{
    {
        USB_CLASS_CDC,
        USB_DESC_CONFIGURATION(USB_CDC_IF_MAX, usb_if),
    },
    {
        USB_CLASS_INVALID,
        USB_DESC_CONFIGURATION(0, NULL),
    }
};

#define USBVID_LOW		0xC3
#define USBVID_HIG		0x1B

#define USBPID_HIG		0x20

#define USBPID_VCDL		0x01
#define USBPID_VCDH		USBPID_HIG

#define USBPID_CCIDL	0x02
#define USBPID_CCIDH	USBPID_HIG

#define USBPID_HIDL		0x03
#define USBPID_HIDH		USBPID_HIG

#define USBPID_HIDL_KB		0x04
#define USBPID_HIDH_KB		USBPID_HIG

uint8_t g_device_descriptor[DEVICE_DESCRIPTOR_SIZE] =
{
    /* "Device Dexcriptor Size */
    DEVICE_DESCRIPTOR_SIZE,               
    /* "Device" Type of descriptor */   
    USB_DEVICE_DESCRIPTOR,                
    /*  BCD USB version  */  
    USB_uint_16_low(BCD_USB_VERSION), USB_uint_16_high(BCD_USB_VERSION),
    /* Device Class is indicated in the interface descriptors */   
    DEVICE_DESC_DEVICE_CLASS,
    /*  Device Subclass is indicated in the interface descriptors  */      
    DEVICE_DESC_DEVICE_SUBCLASS,
    /*  Device Protocol  */     
    DEVICE_DESC_DEVICE_PROTOCOL,
    /* Max Packet size */
    CONTROL_MAX_PACKET_SIZE,
    /* Vendor ID */
    USBVID_LOW,USBVID_HIG,
    /* Product ID */
    USBPID_VCDL,USBPID_VCDH,  
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

uint8_t g_config_descriptor[CONFIG_DESC_SIZE] =
{   
    CONFIG_ONLY_DESC_SIZE, /*  Configuration Descriptor Size - always 9 bytes*/   
    USB_CONFIG_DESCRIPTOR, /* "Configuration" type of descriptor */   
    USB_uint_16_low(CONFIG_DESC_SIZE),
    USB_uint_16_high(CONFIG_DESC_SIZE), /*  Total length of the Configuration descriptor */   
    /*  NumInterfaces */   
    CONFIG_DESC_NUM_INTERFACES_SUPPOTED,
    0x01,                      /*  Configuration Value */      
    0x00,                      /*  Configuration Description String Index*/   
    /*  Attributes.support RemoteWakeup and self power */
    (USBCFG_DEV_SELF_POWER << USB_DESC_CFG_ATTRIBUTES_SELF_POWERED_SHIFT) | (USBCFG_DEV_REMOTE_WAKEUP << USB_DESC_CFG_ATTRIBUTES_REMOTE_WAKEUP_SHIFT),
    /*  Current draw from bus */
    CONFIG_DESC_CURRENT_DRAWN, 
    
    /* CIC INTERFACE DESCRIPTOR */  
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    0x00, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    CIC_ENDP_COUNT, /* management and notification(optional)element present */
    CDC_CLASS, /* Communication Interface Class */
    CIC_SUBCLASS_CODE, 
    CIC_PROTOCOL_CODE,
    0x00, /* Interface Description String Index*/
        
    /* CDC Class-Specific descriptor */
    CDC_HEADER_FUNC_DESC_SIZE,/* size of Functional Desc in bytes */
    USB_CS_INTERFACE,  /* descriptor type*/
    HEADER_FUNC_DESC,
    0x10, 0x01,/* USB Class Definitions for CDC spec release number in BCD */
    
    CDC_CALL_MANAG_DESC_SIZE,/* Size of this descriptor */
    USB_CS_INTERFACE, /* descriptor type*/
    CALL_MANAGEMENT_FUNC_DESC,   
    0x01,/*D0(if set): device handales call management itself
           D1(if set): process commands multiplexed over the data interface*/
    0x01,/* Indicates multiplexed commands are handled via data interface */
   
    CDC_ABSTRACT_DESC_SIZE,             /* Size of this descriptor */
    USB_CS_INTERFACE, /* descriptor type*/
    ABSTRACT_CONTROL_FUNC_DESC, 
    0x06,/* Device supports request send break, device supports reuest 
            combination o set_line_coding, set_control_line_state, 
            get_line_coding and the notification serial state */
   
    CDC_UNION_FUNC_DESC_SIZE,             /* size of Functional Desc in bytes */
    USB_CS_INTERFACE,  /* descriptor type*/
    UNION_FUNC_DESC, 
    0x00,   /* Interface Number of Control */
    0x01    /* Interface Number of Subordinate (Data Class) Interface */   
       
#if CIC_NOTIF_ELEM_SUPPORT    /*Endpoint descriptor */
    , /* Comma Added if NOTIF ELEM IS TO BE ADDED */
    ENDP_ONLY_DESC_SIZE,
    USB_ENDPOINT_DESCRIPTOR,
    CIC_NOTIF_ENDPOINT|(USB_SEND << 7),
    USB_INTERRUPT_PIPE, 
    USB_uint_16_low(CIC_NOTIF_ENDP_PACKET_SIZE),
    USB_uint_16_high(CIC_NOTIF_ENDP_PACKET_SIZE),
    0x0A
#endif   
   
#if DATA_CLASS_SUPPORT    
        , /* Comma Added if DATA_CLASS_DESC IS TO BE ADDED */
        IFACE_ONLY_DESC_SIZE,
        USB_IFACE_DESCRIPTOR,
        (uint8_t)(0x00+DATA_CLASS_SUPPORT), /* bInterfaceNumber */
        0x00, /* bAlternateSetting */
        DIC_ENDP_COUNT, /* notification element included */
        0x0A, /* DATA Interface Class */
        0x00, /* Data Interface SubClass Code */
        DIC_PROTOCOL_CODE,
        0x00, /* Interface Description String Index*/
     
        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        DIC_BULK_IN_ENDPOINT|(USB_SEND << 7),
        USB_BULK_PIPE, 
        USB_uint_16_low(DIC_BULK_IN_ENDP_PACKET_SIZE),
        USB_uint_16_high(DIC_BULK_IN_ENDP_PACKET_SIZE),
        0x00,/* This value is ignored for Bulk ENDPOINT */
        
        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        DIC_BULK_OUT_ENDPOINT|(USB_RECV << 7),
        USB_BULK_PIPE, 
        USB_uint_16_low(DIC_BULK_OUT_ENDP_PACKET_SIZE),
        USB_uint_16_high(DIC_BULK_OUT_ENDP_PACKET_SIZE),
        0x00 /* This value is ignored for Bulk ENDPOINT */
#endif
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
        /*Attributes.support RemoteWakeup and self power*/
		(USBCFG_DEV_SELF_POWER << USB_DESC_CFG_ATTRIBUTES_SELF_POWERED_SHIFT) | (USBCFG_DEV_REMOTE_WAKEUP << USB_DESC_CFG_ATTRIBUTES_REMOTE_WAKEUP_SHIFT),
        /*  Current draw from bus */
        CONFIG_DESC_CURRENT_DRAWN, 
       
    /* CIC INTERFACE DESCRIPTOR */  
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    0x00, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    CIC_ENDP_COUNT, /* management and notification(optional)element present */
    CDC_CLASS, /* Communication Interface Class */
    CIC_SUBCLASS_CODE, 
    CIC_PROTOCOL_CODE,
    0x00, /* Interface Description String Index*/
        
    /* CDC Class-Specific descriptor */
    CDC_HEADER_FUNC_DESC_SIZE,/* size of Functional Desc in bytes */
    USB_CS_INTERFACE,  /* descriptor type*/
    HEADER_FUNC_DESC,
    0x10, 0x01,/* USB Class Definitions for CDC spec release number in BCD */
    
    CDC_CALL_MANAG_DESC_SIZE,/* Size of this descriptor */
    USB_CS_INTERFACE, /* descriptor type*/
    CALL_MANAGEMENT_FUNC_DESC,   
    0x01,/*D0(if set): device handales call management itself
           D1(if set): process commands multiplexed over the data interface*/
    0x01,/* Indicates multiplexed commands are handled via data interface */
   
    CDC_ABSTRACT_DESC_SIZE,             /* Size of this descriptor */
    USB_CS_INTERFACE, /* descriptor type*/
    ABSTRACT_CONTROL_FUNC_DESC, 
    0x06,/* Device supports request send break, device supports reuest 
            combination o set_line_coding, set_control_line_state, 
            get_line_coding and the notification serial state */
   
    CDC_UNION_FUNC_DESC_SIZE,             /* size of Functional Desc in bytes */
    USB_CS_INTERFACE,  /* descriptor type*/
    UNION_FUNC_DESC, 
    0x00,   /* Interface Number of Control */
    0x01    /* Interface Number of Subordinate (Data Class) Interface */   
       
    #if CIC_NOTIF_ELEM_SUPPORT    /*Endpoint descriptor */
        , /* Comma Added if NOTIF ELEM IS TO BE ADDED */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        CIC_NOTIF_ENDPOINT|(USB_SEND << 7),
        USB_INTERRUPT_PIPE, 
        USB_uint_16_low(CIC_NOTIF_ENDP_PACKET_SIZE),
        USB_uint_16_high(CIC_NOTIF_ENDP_PACKET_SIZE),
        0x0A
    #endif   
   
    #if DATA_CLASS_SUPPORT    
        , /* Comma Added if DATA_CLASS_DESC IS TO BE ADDED */
        IFACE_ONLY_DESC_SIZE,
        USB_IFACE_DESCRIPTOR,
        (uint8_t)(0x00+DATA_CLASS_SUPPORT), /* bInterfaceNumber */
        0x00, /* bAlternateSetting */
        DIC_ENDP_COUNT, /* notification element included */
        0x0A, /* DATA Interface Class */
        0x00, /* Data Interface SubClass Code */
        DIC_PROTOCOL_CODE,
        0x00, /* Interface Description String Index*/
     
        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        DIC_BULK_IN_ENDPOINT|(USB_SEND << 7),
        USB_BULK_PIPE, 
        USB_uint_16_low(OTHER_SPEED_DIC_BULK_IN_ENDP_PACKET_SIZE),
        USB_uint_16_high(OTHER_SPEED_DIC_BULK_IN_ENDP_PACKET_SIZE),
        0x00,/* This value is ignored for Bulk ENDPOINT */
        
        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        DIC_BULK_OUT_ENDPOINT|(USB_RECV << 7),
        USB_BULK_PIPE, 
        USB_uint_16_low(OTHER_SPEED_DIC_BULK_OUT_ENDP_PACKET_SIZE),
        USB_uint_16_high(OTHER_SPEED_DIC_BULK_OUT_ENDP_PACKET_SIZE),
        0x00 /* This value is ignored for Bulk ENDPOINT */
    #endif
    };
#endif

const uint8_t USB_STR_0[USB_STR_0_SIZE+USB_STR_DESC_SIZE] = {
    sizeof(USB_STR_0),    
    USB_STRING_DESCRIPTOR, 
    0x09,
    0x04/*equiavlent to 0x0409*/ 
};

const uint8_t USB_STR_1[USB_STR_1_SIZE+USB_STR_DESC_SIZE] = {
    sizeof(USB_STR_1),          
    USB_STRING_DESCRIPTOR,
    'S',0,
    't',0,
    'a',0,
    'r',0,
    't',0,
    ' ',0,
    'C',0,
    'o',0,
    'm',0,
    'p',0,
    'u',0,
    't',0,
    'e',0,
    'r',0,
    ' ',0,
    'C',0,
    'O',0,
    '.',0,
    ',',0,
    'L',0,
    't',0,
    'd',0,
    ' ',0,
};


const uint8_t USB_STR_2[USB_STR_2_SIZE+USB_STR_DESC_SIZE] = {
    sizeof(USB_STR_2),          
    USB_STRING_DESCRIPTOR,                                                                                           
    'S',0,
    't',0,
    'a',0,
    'r',0,
    't',0,
    ' ',0,
    'V',0,
    'R',0,
    'T',0,
    'U',0,
    'A',0,
    'L',0,
    ' ',0,
    'C',0,
    'O',0,
    'M',0,
    ' ',0,
};

const uint8_t USB_STR_n[USB_STR_n_SIZE+USB_STR_DESC_SIZE] = {
    sizeof(USB_STR_n),         
    USB_STRING_DESCRIPTOR,                                
    'W',0,
    'P',0,
    '-',0,
    '3',0,
    '0',0,
    ' ',0,
};


uint8_t g_std_desc_size[USB_MAX_STD_DESCRIPTORS+1] = 
{
    0,
    DEVICE_DESCRIPTOR_SIZE,
    CONFIG_DESC_SIZE,
    0, /* string */
    0, /* Interfdace */
    0, /* Endpoint */
    #if HIGH_SPEED_DEVICE
        DEVICE_QUALIFIER_DESCRIPTOR_SIZE,
        OTHER_SPEED_CONFIG_DESCRIPTOR_SIZE
    #else                                       
         0, /* Device Qualifier */
         0 /* other spped config */
    #endif
};   
                                             
const uint8_t * const g_std_descriptors[USB_MAX_STD_DESCRIPTORS+1] = 
 {
      NULL,
      g_device_descriptor,
      g_config_descriptor,
      NULL, /* string */
      NULL, /* Interfdace */
      NULL, /* Endpoint */
      #if HIGH_SPEED_DEVICE
          g_device_qualifier_descriptor,
          g_other_speed_config_descriptor
      #else
          NULL, /* Device Qualifier */
          NULL /* other spped config*/
      #endif
 }; 
   
const uint8_t g_string_desc_size[USB_MAX_STRING_DESCRIPTORS+1] = {
    sizeof(USB_STR_0),
    sizeof(USB_STR_1),
    sizeof(USB_STR_2),
    sizeof(USB_STR_n)
};   

const uint8_t *g_string_descriptors[USB_MAX_STRING_DESCRIPTORS+1] = {
    USB_STR_0,
    USB_STR_1,
    USB_STR_2,
    USB_STR_n
};    

const usb_language_t usb_lang[USB_MAX_LANGUAGES_SUPPORTED] = { 
    (uint16_t)0x0409,
    g_string_descriptors,
    g_string_desc_size
};                                                                                              

const usb_all_languages_t g_languages = {
    USB_STR_0, sizeof(USB_STR_0),
    USB_MAX_LANGUAGES_SUPPORTED,
    usb_lang
};
                                
static uint8_t g_valid_config_values[USB_MAX_CONFIG_SUPPORTED+1]={0,1};

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
 * @name  USB_Desc_Get_Descriptor
 *
 * @brief The function returns the correponding descriptor
 *
 * @param handle:        handle     
 * @param type:          type of descriptor requested     
 * @param str_num:       string index for string descriptor     
 * @param index:         string descriptor language Id     
 * @param descriptor:    output descriptor pointer
 * @param size:          size of descriptor returned
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Get_Descriptor(
     uint32_t handle, 
     uint8_t type,
     uint8_t str_num, 
     uint16_t index,
     uint8_t * *descriptor,
     uint32_t *size) 
{
     UNUSED_ARGUMENT(handle)

    /* string descriptors are handled saperately */
    if (type == USB_STRING_DESCRIPTOR)
    { 
        if(index == 0) 
        {  
            /* return the string and size of all languages */      
            *descriptor = (uint8_t *)g_languages.languages_supported_string;
            *size = g_languages.languages_supported_size;            
        } 
        else 
        {
            uint8_t lang_id=0;
            uint8_t lang_index=USB_MAX_LANGUAGES_SUPPORTED;
            
            for(;lang_id< USB_MAX_LANGUAGES_SUPPORTED;lang_id++) 
            {
                /* check whether we have a string for this language */
                if(index == g_languages.usb_language[lang_id].language_id) 
                {   /* check for max descriptors */
                    if(str_num < USB_MAX_STRING_DESCRIPTORS) 
                    {   /* setup index for the string to be returned */
                        lang_index=str_num;                 
                    }                    
                    break;                    
                }                
            }

            /* set return val for descriptor and size */
            *descriptor = 
              (uint8_t *)g_languages.usb_language[lang_id].lang_desc[str_num];
            *size = 
                  g_languages.usb_language[lang_id].lang_desc_size[lang_index];
        }        
    }
    else if (type < USB_MAX_STD_DESCRIPTORS+1)
    {
        /* set return val for descriptor and size*/
        *descriptor = (uint8_t *)g_std_descriptors [type];
       
        /* if there is no descriptor then return error */
        if(*descriptor == NULL) 
        {
            return USBERR_INVALID_REQ_TYPE;
        }        
        *size = g_std_desc_size[type];                
    }
    else /* invalid descriptor */
    {
        return USBERR_INVALID_REQ_TYPE;
    }
    return USB_OK;  
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Interface
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
uint8_t USB_Desc_Get_Interface(uint32_t handle, 
                              uint8_t interface, 
                              uint8_t * alt_interface)
{   
    UNUSED_ARGUMENT(handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get alternate interface*/
        *alt_interface = g_alternate_interface[interface];
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Interface
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
uint8_t USB_Desc_Set_Interface(uint32_t handle, 
                              uint8_t interface, 
                              uint8_t alt_interface)
{
    UNUSED_ARGUMENT(handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set alternate interface*/
        g_alternate_interface[interface]=alt_interface;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Valid_Configation
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
bool USB_Desc_Valid_Configation(uint32_t handle,
                                   uint16_t config_val)
{
    uint8_t loop_index=0;
 
    UNUSED_ARGUMENT(handle)
 
    /* check with only supported val right now */
    while(loop_index < (USB_MAX_CONFIG_SUPPORTED+1)) 
    {
        if(config_val == g_valid_config_values[loop_index]) 
        {          
            return TRUE;
        }
        loop_index++;
    }    
    return FALSE;    
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Remote_Wakeup
 *
 * @brief The function checks whether the remote wakeup is supported or not
 *
 * @param handle:        handle     
 *
 * @return USBCFG_DEV_REMOTE_WAKEUP (TRUE) - if remote wakeup supported
 *****************************************************************************/
bool USB_Desc_Remote_Wakeup(uint32_t handle) 
{
    UNUSED_ARGUMENT(handle)
    return USBCFG_DEV_REMOTE_WAKEUP;   
}           

/**************************************************************************//*!
 *
 * @name  USB_Set_Configation
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
uint8_t USB_Set_Configation
(
	cdc_handle_t handle, uint8_t config

)
{
    UNUSED_ARGUMENT (handle)
    uint32_t i;
    for(i = 0; USB_CLASS_INVALID != usb_dec_class[i].type; i++)
    {
        usb_dec_class[i].interfaces = usb_configuration[config - 1]; /*config num starts from 1*/
    }
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Entity
 *
 * @brief The function retrieves the entity specified by type.
 *
 * @param handle            handle
 *
 * @return USB_OK  - if success
 *****************************************************************************/
uint8_t USB_Desc_Get_Entity(cdc_handle_t handle,entity_type type, uint32_t * object)
{

    UNUSED_ARGUMENT(handle)
    switch(type)
    {
        case USB_CLASS_INFO:
            *object = (uint32_t)usb_dec_class;
            break;
        default :
            break;
    }/* End Switch */
    return USB_OK;
}

const usb_desc_request_notify_struct_t  desc_callback =
{
   USB_Desc_Get_Descriptor,
   USB_Desc_Get_Interface,
   USB_Desc_Set_Interface,
   USB_Set_Configation,
   USB_Desc_Get_Entity
};


/* EOF */
