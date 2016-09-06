/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2010, 2013 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 ******************************************************************************
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
 **************************************************************************//*!
 *
 * @file usb_composite.h
 *
 * @author
 *
 * @version
 *
 * @date
 *
 * @brief The file contains USB stack Video class layer API header function.
 *
 *****************************************************************************/


#ifndef _USB_CLASS_COMPOSITE_H
#define _USB_CLASS_COMPOSITE_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "usb_class.h"


/******************************************************************************
 * Macro's
 *****************************************************************************/
/* Audio class type */ 


 typedef uint32_t comosite_handle_t;
/******************************************************************************
 * Types
 *****************************************************************************/

 /* Structure used to configure composite class by APP*/
 typedef struct _class_config_struct
 {
     usb_application_callback_struct_t          composite_application_callback;
     usb_vendor_req_callback_struct_t           vendor_req_callback;
     usb_class_specific_callback_struct_t       class_specific_callback;
     usb_desc_request_notify_struct_t*          desc_callback_ptr;
     class_type                                 type;
 }class_config_struct_t;
 
 typedef struct _composite_config_struct
 {
     uint8_t                                    count;               /* Number of class support */   
     class_config_struct_t*                     class_app_callback;  /* Array of Endpoints Structures */
 }composite_config_struct_t;
/******************************************************************************
 * Global function prototypes
 *****************************************************************************/
extern usb_status USB_Composite_Init(
    uint8_t                    controller_ID,                /* [IN] Controller ID */
    composite_config_struct_t* composite_callback_ptr,       /* [IN] Poiter to class info */
    comosite_handle_t*         compositeHandle   
);

extern usb_status USB_Composite_DeInit(
    comosite_handle_t          compositeHandle               /* [IN] Controller ID */
);
extern usb_status USB_Composite_Get_Class_Handle(comosite_handle_t handle, class_type type, void *   class_handle_ptr);
#endif
/* EOF */
