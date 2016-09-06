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
* $FileName: usb_class_msc.h$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief 
*
*****************************************************************************/

#ifndef _USB_CLASS_MSC_H
#define _USB_CLASS_MSC_H 1

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
#define MAX_MSC_CLASS_EP_NUM   2

#define USB_MSC_DEVICE_READ_REQUEST        (0x81)
#define USB_MSC_DEVICE_WRITE_REQUEST       (0x82) 
#define USB_MSC_DEVICE_FORMAT_COMPLETE     (0x83)
#define USB_MSC_DEVICE_REMOVAL_REQUEST     (0x84)
#define USB_MSC_DEVICE_GET_INFO            (0x85)
#define USB_MSC_START_STOP_EJECT_MEDIA     (0x86) 
#define USB_MSC_DEVICE_GET_SEND_BUFF_INFO  (0x87)
#define USB_MSC_DEVICE_GET_RECV_BUFF_INFO  (0x88)

/* macros for queuing */
 #define MSD_MAX_QUEUE_ELEMS  (4)

 #define USB_REQ_VAL_INVALID             (0xFFFF)
 
typedef uint32_t msd_handle_t;

/* structure to hold a request in the endpoint queue */
typedef struct _msc_app_data_struct
{
    uint8_t*                data_ptr;         /* pointer to buffer       */     
    uint32_t                data_size;        /* buffer size of endpoint */
}msc_app_data_struct_t;

typedef struct _device_lba_info_struct
{
    uint32_t                total_lba_device_supports;/* lab : LOGICAL ADDRESS BLOCK */ 
    uint32_t                length_of_each_lab_of_device;
    uint8_t                 num_lun_supported; 
}device_lba_info_struct_t;

typedef struct _msd_buffers_info
{
     uint8_t*               msc_bulk_in_ptr;
     uint8_t*               msc_bulk_out_ptr;
     uint32_t               msc_bulk_in_size;
     uint32_t               msc_bulk_out_size;
}msc_buff_info_t;

/* MSD Configuration structure to be passed by APP*/
typedef struct _msc_config_struct
{
    /* SCSI related initialization data. To be moved to SCSI layer.*/
     
     usb_application_callback_struct_t          msc_application_callback;
     usb_vendor_req_callback_struct_t           vendor_req_callback;
     usb_class_specific_callback_struct_t       class_specific_callback;
     usb_desc_request_notify_struct_t*          desc_callback_ptr; 
}msc_config_struct_t;

typedef struct _lba_app_struct
{
    uint32_t             offset;
    uint32_t             size;
    uint8_t*             buff_ptr;
}lba_app_struct_t;
extern void USB_Class_Periodic_Task(void);
#define USB_MSC_Periodic_Task USB_Class_Periodic_Task 
/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_MSC_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param msd_config_ptr    : Configuration paramemter strucutre pointer
 *                            passed by APP.
 * @return status       
 *         MSD Handle           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the MSC Class layer and layers it is dependednt on 
 ******************************************************************************/
extern usb_status USB_Class_MSC_Init
(
    uint8_t controller_id,
    msc_config_struct_t* msd_config_ptr,
    msd_handle_t *  msd_handle
); 


/**************************************************************************//*!
 *
 * @name  USB_Class_MSC_Deinit
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param cdc_handle
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the MSC Class layer and layers it is dependednt on 
 *
 *****************************************************************************/
extern usb_status USB_Class_MSC_Deinit
(
    msd_handle_t msd_handle
);

#endif


