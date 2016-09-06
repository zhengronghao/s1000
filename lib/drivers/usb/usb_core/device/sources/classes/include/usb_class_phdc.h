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
* $FileName: usb_class_phdc.h$
* $Version : 3.8.2.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
* @brief The file contains USB stack PHDC class layer api header function.
*
*****************************************************************************/

#ifndef _USB_CLASS_PHDC_H
#define _USB_CLASS_PHDC_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define USB_METADATA_SUPPORTED                  (0)
#define USB_DEV_EVENT_META_DATA_PARAMS_CHANGED  (0xF2)
#define USB_DEV_EVENT_FEATURE_CHANGED           (0xF3)

#if USB_METADATA_SUPPORTED
    #define META_DATA_MSG_PRE_IMPLEMENTED       (1)/*TRUE:1; FALSE:0*/
#else
    #define META_DATA_MSG_PRE_IMPLEMENTED       (0)/*TRUE:1; FALSE:0*/
#endif
/* Exception */
#define USB_REQ_VAL_INVALID             (0xFFFF)
/*****************************************************************************
 * Local Functions
 *****************************************************************************/

 /******************************************************************************
 * Types
 *****************************************************************************/
typedef uint32_t phdc_handle_t;

/* event structures */
typedef struct _phdc_app_data_struct 
{
    uint8_t   qos;
    uint8_t*  buffer_ptr;
    uint32_t  size;
} phdc_app_data_struct_t;

/* Structures used to configure PHDC class by  APP*/
typedef struct _phdc_config_struct
{
    usb_application_callback_struct_t           phdc_application_callback;
    usb_vendor_req_callback_struct_t            vendor_req_callback; 
    usb_class_specific_callback_struct_t        class_specific_callback;
    usb_desc_request_notify_struct_t*           desc_callback_ptr;  
} phdc_config_struct_t;

/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_PHDC_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param  phdc_config_ptr[IN]  : Phdc configuration structure pointer
 * @return phdc_handle      : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 * This function initializes the PHDC Class layer and layers it is dependednt on 
 *****************************************************************************/                          
extern usb_status USB_Class_PHDC_Init
(
    uint8_t                 controller_id,   /*[IN]*/
    phdc_config_struct_t*   phdc_config_ptr, /*[IN]*/
    phdc_handle_t *           phdcHandle /*[OUT]*/
);

/**************************************************************************//*!
 *
 * @name  USB_Class_PHDC_Deinit
 *
 * @brief 
 *
 * @param handle          :   handle returned by USB_Class_PHDC_Deinit   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
extern usb_status USB_Class_PHDC_Deinit
(
    phdc_handle_t   phdcHandle
);

/**************************************************************************//*!
 *
 * @name  USB_PHDC_Class_Recv_Data
 *
 * @brief This fucntion is used by Application to receive data through PHDC class
 *
 * @param handle     :   Handle returned by USB_Class_PHDC_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 * This fucntion is used by Application to send data through PHDC class 
 *****************************************************************************/  
extern usb_status USB_Class_PHDC_Recv_Data
(
    phdc_handle_t       handle,
    uint8_t             qos, 
    uint8_t*            buff_ptr,      /* [IN] buffer to send */      
    uint32_t            size           /* [IN] length of the transfer */
);
/**************************************************************************//*!
 *
 * @name  USB_Class_PHDC_Send_Data
 *
 * @brief This fucntion is used by Application to send data through PHDC class
 *
 * @param handle          :   handle returned by USB_Class_PHDC_Init
 * @param meta_data       :   packet is meta data or not
 * @param num_tfr         :   no. of transfers
 * @param qos             :   current qos of the transfer
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 * This fucntion is used by Application to send data through PHDC class 
 *****************************************************************************/  
extern usb_status USB_Class_PHDC_Send_Data
(
    phdc_handle_t    handle,    
    bool             meta_data,    /* opaque meta data in app buffer */
    uint8_t          num_tfr,      /* no. of transfers to follow with given 
                                                            channel--only valid if meta data is 
                                                            true */
    uint8_t          current_qos,  /* qos of the transfers to follow--only 
                                                             valid if meta data is true */
    uint8_t*         app_buff,     /* buffer holding application data */
    uint32_t         size          /* [IN] length of the transfer */
);
extern void USB_Class_Periodic_Task(void);                               
#define USB_PHDC_Periodic_Task USB_Class_Periodic_Task
#define USB_Class_PHDC_Periodic_Task USB_Class_Periodic_Task

#endif

/* EOF */
