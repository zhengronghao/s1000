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
* $FileName: hid_descriptor.h$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief 
*
*****************************************************************************/

#ifndef _HID_DESCRIPTOR_H
#define _HID_DESCRIPTOR_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "usb_class_hid.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define BCD_USB_VERSION                  (0x0200)

/* Various descriptor sizes */
#define DEVICE_DESCRIPTOR_SIZE              (18)
#define CONFIG_DESC_SIZE                    (34)
#define REPORT_DESC_SIZE                    (52)
#define CONFIG_ONLY_DESC_SIZE               (9)
#define IFACE_ONLY_DESC_SIZE                (9)
#define HID_ONLY_DESC_SIZE                  (9)
#define ENDP_ONLY_DESC_SIZE                 (7)

#if HIGH_SPEED_DEVICE
    #define DEVICE_QUALIFIER_DESCRIPTOR_SIZE    (10)
    #define OTHER_SPEED_CONFIG_DESCRIPTOR_SIZE  (CONFIG_DESC_SIZE)  
#endif

/* HID buffer size */
#define HID_BUFFER_SIZE                         (8)
/* Max descriptors provided by the Application */
#define USB_MAX_STD_DESCRIPTORS                 (8)
#define USB_MAX_CLASS_SPECIFIC_DESCRIPTORS      (2)
/* Max configuration supported by the Application */
#define USB_MAX_CONFIG_SUPPORTED                (1)

/* Max string descriptors supported by the Application */
#define USB_MAX_STRING_DESCRIPTORS              (3)

/* Max language codes supported by the USB */
#define USB_MAX_LANGUAGES_SUPPORTED             (1)

#define HID_DESC_ENDPOINT_COUNT                 (1)
#define HID_ENDPOINT                            (1)
#define HID_ENDPOINT_PACKET_SIZE                (8)
#define HID_DESC_INTERFACE_COUNT                (1)

/* string descriptors sizes */
#define USB_STR_DESC_SIZE                       (2)
#define USB_STR_0_SIZE                          (2)
#define USB_STR_1_SIZE                          (56)
#define USB_STR_2_SIZE                          (34)
#define USB_STR_n_SIZE                          (32)

/* descriptors codes */
#define USB_DEVICE_DESCRIPTOR                   (1)
#define USB_CONFIG_DESCRIPTOR                   (2)
#define USB_STRING_DESCRIPTOR                   (3)
#define USB_IFACE_DESCRIPTOR                    (4)
#define USB_ENDPOINT_DESCRIPTOR                 (5)

#if HIGH_SPEED_DEVICE
    #define USB_DEVQUAL_DESCRIPTOR              (6)
    #define USB_OTHER_SPEED_DESCRIPTOR          (7)
#endif

#define USB_HID_DESCRIPTOR                      (0x21)
#define USB_REPORT_DESCRIPTOR                   (0x22)

#define USB_MAX_SUPPORTED_INTERFACES            (1)

#define DEVICE_DESC_DEVICE_CLASS                (0x00)
#define DEVICE_DESC_DEVICE_SUBCLASS             (0x00)
#define DEVICE_DESC_DEVICE_PROTOCOL             (0x00)
#define DEVICE_DESC_NUM_CONFIG_SUPPOTED         (0x01)
/* Keep the following macro Zero if you dont Support Other Speed Configuration
   If you suppoort Other Speeds make it 0x01 */
#define DEVICE_OTHER_DESC_NUM_CONFIG_SUPPOTED   (0x00) 
#define CONFIG_DESC_NUM_INTERFACES_SUPPOTED     (0x01)
#define CONFIG_DESC_CURRENT_DRAWN               (0x32)
#define CONTROL_MAX_PACKET_SIZE                 (64)
#define USB_MAX_SUPPORTED_LANGUAGES             (1)
/******************************************************************************
 * Types
 *****************************************************************************/ 

/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern uint8_t USB_Desc_Get_Descriptor( 
     hid_handle_t handle, 
     uint8_t type,
     uint8_t str_num, 
     uint16_t index,
     uint8_t * *descriptor,
     uint32_t *size);
      
extern uint8_t USB_Desc_Get_Interface(
                              hid_handle_t handle, 
                              uint8_t interface, 
                              uint8_t * alt_interface);

extern uint8_t USB_Desc_Set_Interface(
                              hid_handle_t handle, 
                              uint8_t interface, 
                              uint8_t alt_interface);

extern bool USB_Desc_Valid_Configation(hid_handle_t handle, uint16_t config_val); 
extern bool USB_Desc_Remote_Wakeup(hid_handle_t handle); 

extern usb_endpoints_t *USB_Desc_Get_Endpoints(hid_handle_t handle); 

#endif
/* EOF */
