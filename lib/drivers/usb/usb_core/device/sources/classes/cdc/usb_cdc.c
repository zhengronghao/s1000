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
* $FileName: usb_cdc.c$
* $Version : 
* $Date    : 
*
*
* @brief The file contains CDC layer implimentation.
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"


#if USBCFG_DEV_CDC
#include "usb_class_internal.h"
#include "usb_class_cdc.h"
#include "usb_cdc.h"
#include "usb_cdc_pstn.h"
/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/

extern usb_status usb_device_postinit(
                                      /* [IN] the USB device controller to initialize */
                                      uint8_t controller_id,
                                      /* [OUT] the USB_USB_dev_initialize state structure */
                                      usb_device_handle  handle
                                     );
/****************************************************************************
 * Global Variables
 ****************************************************************************/
cdc_device_struct_t   cdc_device_array[MAX_CDC_DEVICE];

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
uint8_t USB_Map_Ep_To_Struct_Index(cdc_device_struct_t * cdc_obj_ptr, uint8_t ep_num);  
static usb_status USB_Cdc_Allocate_Handle(cdc_handle_t* handle);
static usb_status USB_Cdc_Free_Handle(cdc_handle_t handle);

/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/
  /*************************************************************************//*!
 *
 * @name  USB_Cdc_Allocate_Handle
 *
 * @brief The funtion reserves entry in device array and returns the index.
 *
 * @param none.
 * @return returns the reserved handle or if no entry found device busy.      
 *
 *****************************************************************************/
static usb_status USB_Cdc_Allocate_Handle(cdc_handle_t* handle)
{
    uint32_t cnt = 0;
    for (;cnt< MAX_CDC_DEVICE;cnt++)
    {
        if (cdc_device_array[cnt].controller_handle == NULL)
        {
            *handle = (cdc_handle_t)&cdc_device_array[cnt];
            return USB_OK;
        }
    }
    return USBERR_DEVICE_BUSY;
}
 /*************************************************************************//*!
 *
 * @name  USB_Cdc_Free_Handle
 *
 * @brief The funtion releases entry in device array .
 *
 * @param handle  index in device array to be released..
 * @return returns and error code or USB_OK.      
 *
 *****************************************************************************/

static usb_status USB_Cdc_Free_Handle(cdc_handle_t handle)
{
    int32_t cnt = 0;
    for (;cnt< USBCFG_DEV_MAX_CLASS_OBJECT;cnt++)
    {
        if (((cdc_handle_t)&cdc_device_array[cnt]) == handle)
        {
            OS_Mem_zero((void *)handle, sizeof(cdc_device_struct_t));
            return USB_OK;
        }
    }

    return USBERR_INVALID_PARAM;
}

 /*****************************************************************************
 * Local Functions
 *****************************************************************************/
 
 /*************************************************************************//*!
 *
 * @name  USB_Cdc_Get_Desc_Info
 *
 * @brief The funtion gets the info of the descriptors. .
 *
 * @param handle  index in device array.
 * @param type     descriptor type.
 * @param object   store the returned value.
 * @return returns USB_OK if successful.      
 *
 *****************************************************************************/
 static usb_status USB_Cdc_Get_Desc_Info(cdc_device_struct_t * cdc_obj_ptr,USB_DESC_INFO_T type, uint32_t * object)
 {
	 usb_class_struct_t* usbclassPtr;
	 /* Get class info */
#if USBCFG_DEV_COMPOSITE
	 usb_composite_info_struct_t* usbcompinfoPtr;
	 cdc_obj_ptr->desc_callback.get_desc_entity((uint32_t)cdc_obj_ptr->controller_handle,
													USB_COMPOSITE_INFO,
													(uint32_t *)&usbcompinfoPtr);
	 usbclassPtr = usbcompinfoPtr->class;
#else
     cdc_obj_ptr->desc_callback.get_desc_entity((uint32_t)cdc_obj_ptr->controller_handle,
													USB_CLASS_INFO,
													(uint32_t *)&usbclassPtr);
#endif
	 switch(type)
	 {
	     case USB_EP_COUNT:
	     {
	         usb_if_struct_t *if_ptr;
	         uint32_t ep_cnt = 0;
	         for_each_if_in_class(if_ptr, usbclassPtr, USB_CLASS_ALL)
	         {
	             ep_cnt += if_ptr->endpoints.count;
	         }
	         *object = ep_cnt;
	         break;
	     }
	     case USB_CDC_EP_COUNT:
	     {
	         usb_if_struct_t *if_ptr;
	         uint32_t ep_cnt = 0;
	         for_each_if_in_class(if_ptr, usbclassPtr, USB_CLASS_CDC)
	         {
	             ep_cnt += if_ptr->endpoints.count;
	         }
	         *object = ep_cnt;
	         break;
	     }
	     case USB_INTERFACE_COUNT:
	     {
	         usb_if_struct_t *if_ptr = NULL;
	         uint32_t if_cnt = 0;
             if_ptr = if_ptr;
	         for_each_if_in_class(if_ptr, usbclassPtr, USB_CLASS_ALL)
	         {
	             if_cnt++;
	         }
	         *object = if_cnt;
	         break;
	     }
	     case USB_CDC_INTERFACE_COUNT:
	     {
	         usb_if_struct_t *if_ptr = NULL;
	         uint32_t if_cnt = 0;
             if_ptr = if_ptr;
	         for_each_if_in_class(if_ptr, usbclassPtr, USB_CLASS_CDC)
	         {
	             if_cnt++;
	         }
	         *object = if_cnt;
	         break;
	     }
	     default :
	         break;
	 } 
     return USB_OK;
}
/**************************************************************************//*!
 *
 * @name  USB_Map_Ep_To_Struct_Index
 *
 * @brief The funtion maps the endpoint num to the index of the ep data 
 *           structure
 *
 * @param handle          handle to identify the controller
 * @param ep_num          endpoint num
 *
 * @return index          mapped index       
 *
 *****************************************************************************/
uint8_t USB_Map_Ep_To_Struct_Index(cdc_device_struct_t * cdc_obj_ptr,uint8_t ep_num) 
{
    uint8_t index = 0;
    uint32_t ep_count;
    
	USB_Cdc_Get_Desc_Info(cdc_obj_ptr, USB_EP_COUNT, &ep_count);
    /* map the endpoint num to the index of the endpoint structure */
    for(index = 0; index < ep_count; index++)
    {
        if(cdc_obj_ptr->ep[index].endpoint == ep_num)
            break;
    }    
    return index;     
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Cdc_Notif
 *
 * @brief The funtion ic callback function of CIC Notification endpoint 
 *
  * @param event
 *
 * @return None       
 *
 *****************************************************************************/
void USB_Service_Cdc_Notif(usb_event_struct_t* event,void* arg)
{    
    cdc_device_struct_t * cdc_obj_ptr = (cdc_device_struct_t *)arg;
    #if CDC_IMPLEMENT_QUEUING
        uint8_t index;
        uint8_t producer, consumer;
//        usb_endpoints_t *usb_ep_data = cdc_obj_ptr->usb_ep_data;
        
        cdc_queue_struct_t queue;
    #endif
    
    UNUSED_ARGUMENT (event)
    
    #if CDC_IMPLEMENT_QUEUING
        /* map the endpoint num to the index of the endpoint structure */
        index = USB_Map_Ep_To_Struct_Index(cdc_obj_ptr, event->ep_num); 

        producer = cdc_obj_ptr->ep[index].bin_producer;
            
        /* if there are no errors de-queue the queue and decrement the no. of 
           transfers left, else send the same data again */
        cdc_obj_ptr->ep[index].bin_consumer++;              
        consumer = cdc_obj_ptr->ep[index].bin_consumer;
            
        if(consumer != producer) 
        {/*if bin is not empty */                           
            queue = cdc_obj_ptr->ep[index].queue[consumer%CDC_MAX_QUEUE_ELEMS];                         
            (void)USB_Class_Send_Data(cdc_obj_ptr->class_handle, queue.channel, 
                queue.app_data.data_ptr, queue.app_data.data_size);
        }        
    #endif
    
	cdc_obj_ptr->has_sent_state = FALSE;
    if(cdc_obj_ptr->class_specific_callback.callback != NULL) 
    {
        uint8_t event_type = USB_DEV_EVENT_SEND_COMPLETE;
		cdc_obj_ptr->class_specific_callback.callback(event_type,
				USB_REQ_VAL_INVALID,
				NULL,
				NULL,
				cdc_obj_ptr->class_specific_callback.arg);
    }   
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Dic_Bulk_In
 *
 * @brief The funtion ic callback function of DIC Bulk In Endpoint 
 *
 * @param event
 *
 * @return None       
 *
 *****************************************************************************/
void USB_Service_Dic_Bulk_In(usb_event_struct_t* event,void* arg)
{
    uint8_t event_type;
    cdc_device_struct_t * cdc_obj_ptr = (cdc_device_struct_t *)arg;

    if(USB_UNINITIALIZED_VAL_32 == event->len) return;
    #if CDC_IMPLEMENT_QUEUING
        uint8_t index;
        uint8_t producer, consumer;
//        usb_endpoints_t *usb_ep_data = cdc_obj_ptr->usb_ep_data;
        
        cdc_queue_struct_t queue;

        /* map the endpoint num to the index of the endpoint structure */
        index = USB_Map_Ep_To_Struct_Index(cdc_obj_ptr, event->ep_num); 
        producer = cdc_obj_ptr->ep[index].bin_producer;         
        /* if there are no errors de-queue the queue and decrement the no. of 
           transfers left, else send the same data again */
        cdc_obj_ptr->ep[index].bin_consumer++;                  
        consumer = cdc_obj_ptr->ep[index].bin_consumer;
            
        if(consumer != producer) 
        {/*if bin is not empty */
                            
            queue = cdc_obj_ptr->ep[index].queue[consumer%CDC_MAX_QUEUE_ELEMS];
                            
            (void)USB_Class_Send_Data(cdc_obj_ptr->class_handle, queue.channel, 
                queue.app_data.data_ptr, queue.app_data.data_size);
        }          
    #endif
    
    if(cdc_obj_ptr->class_specific_callback.callback != NULL) 
    {
        event_type = USB_DEV_EVENT_SEND_COMPLETE;
		cdc_obj_ptr->class_specific_callback.callback(event_type,
				USB_REQ_VAL_INVALID,
				&(event->buffer_ptr),
				&(event->len),
				cdc_obj_ptr->class_specific_callback.arg);
    }
}

/**************************************************************************//*!
 *
 * @name  USB_Service_Dic_Bulk_Out
 *
 * @brief The funtion ic callback function of DIC Bulk Out Endpoint 
 *
 * @param event
 *
 * @return None       
 *
 *****************************************************************************/
void USB_Service_Dic_Bulk_Out(usb_event_struct_t* event,void* arg)
{
    uint8_t event_type; 
    cdc_device_struct_t * cdc_obj_ptr = (cdc_device_struct_t *)arg;
      
    if(USB_UNINITIALIZED_VAL_32 == event->len) return;
    event_type = USB_DEV_EVENT_DATA_RECEIVED;
    if(cdc_obj_ptr->class_specific_callback.callback != NULL) 
    {
	  cdc_obj_ptr->class_specific_callback.callback(event_type,
			  USB_REQ_VAL_INVALID,
			  &(event->buffer_ptr),
			  &(event->len),
			  cdc_obj_ptr->class_specific_callback.arg);
    }
 }

/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Event
 *
 * @brief The funtion initializes CDC endpoints 
 *
 * @param handle   handle to Identify the controller
 * @param event           pointer to event structure
 * @param val             gives the configuration value 
 *
 * @return None       
 *
 *****************************************************************************/
 void USB_Class_CDC_Event(uint8_t event, void* val,void* arg) 
{  
    usb_class_struct_t* usbclassPtr;
#if USBCFG_DEV_COMPOSITE
	usb_composite_info_struct_t* usbcompinfoPtr;
#endif
    cdc_device_struct_t * cdc_obj_ptr = NULL;
    cdc_obj_ptr = (cdc_device_struct_t *)arg;
    uint8_t index;
    if(event == USB_DEV_EVENT_CONFIG_CHANGED)
    {
        uint32_t ep_count, max_if_count;
        usb_ep_struct_t* ep_struct_ptr = NULL;
        /* Set configuration according to config number*/
        cdc_obj_ptr->desc_callback.set_configuration((uint32_t)cdc_obj_ptr->controller_handle, (uint8_t)(*(uint16_t*)val));
        /* Get class info */
#if USBCFG_DEV_COMPOSITE
		cdc_obj_ptr->desc_callback.get_desc_entity((uint32_t)cdc_obj_ptr->controller_handle,
													USB_COMPOSITE_INFO,
													(uint32_t*)&usbcompinfoPtr);
		usbclassPtr = usbcompinfoPtr->class;
#else
        cdc_obj_ptr->desc_callback.get_desc_entity((uint32_t)cdc_obj_ptr->controller_handle,
													USB_CLASS_INFO,
													(uint32_t*)&usbclassPtr);
#endif
        /* Get count of endpoints for a specific configuration */
		USB_Cdc_Get_Desc_Info(cdc_obj_ptr, USB_CDC_EP_COUNT, &ep_count);
        /* Get count of interfaces for a specific configuration */
		USB_Cdc_Get_Desc_Info(cdc_obj_ptr, USB_CDC_INTERFACE_COUNT, &max_if_count);
        cdc_obj_ptr->max_supported_interfaces = max_if_count;
        if(NULL == cdc_obj_ptr->ep)
        {
            index = 0;
             
            for_each_ep_in_class(ep_struct_ptr, usbclassPtr, USB_CLASS_CDC)
            {
                cdc_obj_ptr->ep[index].endpoint = ep_struct_ptr->ep_num;
                cdc_obj_ptr->ep[index].type = ep_struct_ptr->type;
#if CDC_IMPLEMENT_QUEUING
                cdc_obj_ptr->ep[index].bin_consumer = 0x00;
                cdc_obj_ptr->ep[index].bin_producer = 0x00;
#endif
                index++;
            }

        }
        /* intialize all non control endpoints */            
        for_each_ep_in_class(ep_struct_ptr, usbclassPtr, USB_CLASS_CDC)
        {
            (void)usb_device_init_endpoint(cdc_obj_ptr->controller_handle,
             ep_struct_ptr,TRUE);
  
            /* register callback service for Non Control EndPoints */
            switch(ep_struct_ptr->type) 
            {
                case USB_INTERRUPT_PIPE :
                    (void)usb_device_register_service(cdc_obj_ptr->controller_handle,
                        (uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                        USB_Service_Cdc_Notif,(void *)cdc_obj_ptr);
                    cdc_obj_ptr->cic_recv_endpoint = USB_CONTROL_ENDPOINT;
                    cdc_obj_ptr->cic_send_endpoint = ep_struct_ptr->ep_num;
                    cdc_obj_ptr->cic_send_pkt_size = ep_struct_ptr->size;
                    break;                              
                case USB_BULK_PIPE :
                    if(ep_struct_ptr->direction == USB_RECV) 
                    {
                        (void)usb_device_register_service(cdc_obj_ptr->controller_handle,
                            (uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                            USB_Service_Dic_Bulk_Out,(void *)cdc_obj_ptr);
                        cdc_obj_ptr->dic_recv_endpoint = ep_struct_ptr->ep_num;
                        cdc_obj_ptr->dic_recv_pkt_size = ep_struct_ptr->size;
                    } 
                    else
                    {
                        (void)usb_device_register_service(cdc_obj_ptr->controller_handle,
                            (uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                            USB_Service_Dic_Bulk_In,(void *)cdc_obj_ptr);
                        cdc_obj_ptr->dic_send_endpoint = ep_struct_ptr->ep_num;
                        cdc_obj_ptr->dic_send_pkt_size = ep_struct_ptr->size;
                    }
                    break;
                default : break;        
            }
            
        }
	}
	else if(event == USB_DEV_EVENT_ENUM_COMPLETE)
    {
        /* To Do */
        UTRACE("\nenum complete");
    }
    #if USBCFG_DEV_RNDIS_SUPPORT
    else if(event == USB_DEV_EVENT_BUS_RESET)
    {
        uint8_t * data; 
        uint32_t size;
        RNDIS_Reset_Command(cdc_obj_ptr, &data, &size);     
    }
    #endif        
    if(cdc_obj_ptr->cdc_application_callback.callback != NULL) 
    {
        cdc_obj_ptr->cdc_application_callback.callback(event,
            val,cdc_obj_ptr->cdc_application_callback.arg);
    } 
}

/**************************************************************************//*!
 *
 * @name  USB_CDC_Other_Requests
 *
 * @brief The funtion provides flexibilty to add class and vendor specific
 *        requests 
 *
 * @param handle
 * @param setup_packet:     setup packet recieved      
 * @param data:             data to be send back
 * @param size:             size to be returned    
 *
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
usb_status USB_CDC_Other_Requests
(   
    usb_setup_struct_t * setup_packet, 
    uint8_t * *data, 
    uint32_t *size,
    void* arg
) 
{
    usb_status status;
    cdc_device_struct_t * cdc_obj_ptr = NULL;
    cdc_obj_ptr = (cdc_device_struct_t *)arg;
    status = USBERR_INVALID_REQ_TYPE;
    if((setup_packet->request_type & USB_DEV_REQ_STD_REQUEST_TYPE_TYPE_POS) == 
        USB_DEV_REQ_STD_REQUEST_TYPE_TYPE_CLASS) 
    {  /* class request so handle it here */     
        status = USB_OK;
        /* call for class/subclass specific requests */
        switch(setup_packet->request) 
        {
            case SEND_ENCAPSULATED_COMMAND :
                #if USBCFG_DEV_RNDIS_SUPPORT               
                    /* Pass the Remote NDIS Control Message supoported by 
                       802.3 connectionless device to PSTN Layer */
                    status = PSTN_Rndis_Message_Set(cdc_obj_ptr,setup_packet,data,size); 
                #endif
                *size = 0;              
                break;
            case GET_ENCAPSULATED_RESPONSE :
                #if USBCFG_DEV_RNDIS_SUPPORT               
                    /* Get the Remote NDIS Control Message supoported by 
                       802.3 connectionless device from  PSTN Layer */
                    status = PSTN_Rndis_Message_Get(cdc_obj_ptr,setup_packet,data,size); 
                #else
                    *size = 0;
                    /* protocol says to return zero byte data instead of stalling 
                       the command if you don't have data to return */
                    status = USB_OK;                
                #endif
                break; 
            case SET_COMM_FEATURE :
                status = PSTN_Set_Comm_Feature(cdc_obj_ptr,setup_packet,data,size);  
                break;
            case GET_COMM_FEATURE :
                status = PSTN_Get_Comm_Feature(cdc_obj_ptr,setup_packet,data,size); 
                break;
            case CLEAR_COMM_FEATURE : /* Verify this implementation */
                *size = cdc_obj_ptr->comm_feature_data_size;
                **data = 0x00; *(++(*data)) = 0x00;/*clear both feature bytes*/
                status = PSTN_Set_Comm_Feature(cdc_obj_ptr,
                setup_packet,data,size);  
                break; 
            case GET_LINE_CODING :               
                status = PSTN_Get_Line_Coding(cdc_obj_ptr,
                setup_packet,data,size); 
                break;
            case SET_LINE_CODING :               
                status = PSTN_Set_Line_Coding(cdc_obj_ptr,setup_packet,data,size); 
                break;    
            case SET_CONTROL_LINE_STATE : 
                status = PSTN_Set_Ctrl_Line_State(cdc_obj_ptr,
                setup_packet,data,size); 
                break;
            case SEND_BREAK : 
                status = PSTN_Send_Break(cdc_obj_ptr,setup_packet,data,size);
                break;
            default :  *size=0;break;        
        }
    } 
    else if((setup_packet->request_type & USB_DEV_REQ_STD_REQUEST_TYPE_TYPE_POS) == 
        USB_DEV_REQ_STD_REQUEST_TYPE_TYPE_VENDOR) 
    {   /* vendor specific request  */    
        if(cdc_obj_ptr->vendor_req_callback.callback != NULL) 
        {
            status = cdc_obj_ptr->vendor_req_callback.callback(setup_packet,
            data,size,cdc_obj_ptr->vendor_req_callback.arg);
        }
    }
        
    return status;
}


/*****************************************************************************
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
usb_status USB_Class_CDC_Init
(
    uint8_t controller_id,
    cdc_config_struct_t * cdc_config_ptr,
    cdc_handle_t *  cdc_handle_ptr
)
{
    usb_status error = USB_OK;
    cdc_handle_t cdc_handle;
    cdc_device_struct_t * devicePtr = NULL;
    
    if (NULL == (void *)cdc_config_ptr)
        return USBERR_ERROR;
    
    if (USBERR_DEVICE_BUSY == USB_Cdc_Allocate_Handle(&cdc_handle)) 
    {
        return USBERR_INIT_FAILED;
    }

    devicePtr = (cdc_device_struct_t *)cdc_handle;
    /* initialize the Global Variable Structure */
    OS_Mem_zero(devicePtr, sizeof(cdc_device_struct_t));
    devicePtr->mutex = OS_Mutex_create();
    //devicePtr->ep = NULL;
#if USBCFG_DEV_COMPOSITE
    devicePtr->class_handle = USB_Class_Get_Class_Handle();
    devicePtr->controller_handle =(usb_device_handle)USB_Class_Get_Ctrler_Handle(devicePtr->class_handle);
    if(NULL == devicePtr->controller_handle)
    {
        goto error1;
    }
#else
	/* Initialize the device layer*/
	error = usb_device_init(controller_id, &devicePtr->controller_handle);
	/* +1 is for Control Endpoint */ 
	if(error != USB_OK)
	{
	  goto error1;  
	}

	/* Initialize the generic class functions */
	devicePtr->class_handle = USB_Class_Init(devicePtr->controller_handle,
	USB_Class_CDC_Event,USB_CDC_Other_Requests,(void *)cdc_handle,
	cdc_config_ptr->desc_callback_ptr);

#endif
    /* Initialize the generic class functions */
    #if PSTN_SUBCLASS_NOTIF_SUPPORT
    /* Initialize the pstn subclass functions */
    error = USB_Pstn_Init(devicePtr,&cdc_config_ptr->cdc_application_callback);
    if(error != USB_OK)
    {
      goto error2;  
    }
    #endif
    /* Save the desc callback to ask application for class specific params*/
    OS_Mem_copy(cdc_config_ptr->desc_callback_ptr,
    &devicePtr->desc_callback ,sizeof(usb_desc_request_notify_struct_t));
    /* save the callback pointer */
    devicePtr->cdc_application_callback.callback = cdc_config_ptr->cdc_application_callback.callback;
    devicePtr->cdc_application_callback.arg = cdc_config_ptr->cdc_application_callback.arg;         
    /* save the callback pointer */
    devicePtr->vendor_req_callback.callback = 
    cdc_config_ptr->vendor_req_callback.callback;
    devicePtr->vendor_req_callback.arg = cdc_config_ptr->vendor_req_callback.arg; 
    /* save the callback pointer */
    devicePtr->class_specific_callback.callback = cdc_config_ptr->class_specific_callback.callback;
    devicePtr->class_specific_callback.arg = cdc_config_ptr->class_specific_callback.arg; 
    devicePtr->comm_feature_data_size =  COMM_FEATURE_DATA_SIZE;
      
    devicePtr->cdc_handle = cdc_handle;
   
    *cdc_handle_ptr = cdc_handle;
#if USBCFG_DEV_RNDIS_SUPPORT
    devicePtr->desc_callback.get_desc_entity((uint32_t)devicePtr->controller_handle,
	   USB_RNDIS_INFO,
	   (uint32_t*)&devicePtr->rndis_info);
#endif
    usb_device_postinit(controller_id,devicePtr->controller_handle);
   return USB_OK;
 error2:
  /* Implement Denit class and invoke here*/    
 error1: 
    USB_Cdc_Free_Handle(cdc_handle);
    //OS_Mem_free(devicePtr);
    devicePtr = NULL;    
    return error;    
}

/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Deinit
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
 *This function initializes the CDC Class layer and layers it is dependednt on 
 *
 *****************************************************************************/
usb_status USB_Class_CDC_Deinit
(
  cdc_handle_t cdc_handle
)
{
    usb_status error = USB_OK;
    cdc_device_struct_t * devicePtr = NULL;
    devicePtr = (cdc_device_struct_t *)cdc_handle;

    #if PSTN_SUBCLASS_NOTIF_SUPPORT
    /* deinitialize the pstn subclass functions */
    error = USB_Pstn_Deinit(devicePtr);
    #endif
#if !USBCFG_DEV_COMPOSITE
    if(error == USB_OK)
        /* deinitialize the generic class functions */
        error = USB_Class_Deinit(devicePtr->controller_handle,devicePtr->class_handle);
    if(error == USB_OK)
        /* deinitialize the device layer*/
        error = usb_device_deinit(devicePtr->controller_handle);
#endif
	OS_Mutex_destroy(devicePtr->mutex);

#if 0
    if(NULL != devicePtr->ep)
    {
        OS_Mem_free(devicePtr->ep);
    }
#endif
    if(error == USB_OK)
        error = USB_Cdc_Free_Handle(cdc_handle);

    devicePtr = NULL;
    return error;    
}
/**************************************************************************//*!
 *
 * @name  USB_Class_CDC_Send_Data
 *
 * @brief 
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
usb_status USB_Class_CDC_Send_Data
(
    cdc_handle_t handle,
    uint8_t ep_num,
    uint8_t * app_buff,
    uint32_t size
) 
{
    #if CDC_IMPLEMENT_QUEUING
        uint8_t index;
        uint8_t producer, consumer; 
//        usb_endpoints_t *usb_ep_data; 
    #endif    
        
    cdc_device_struct_t * cdc_obj_ptr;
    usb_status status = USB_OK;
    cdc_obj_ptr = (cdc_device_struct_t *)handle;
    if (NULL == cdc_obj_ptr)
      return USBERR_ERROR;
    
    #if CDC_IMPLEMENT_QUEUING
//        usb_ep_data = cdc_obj_ptr->usb_ep_data;
      
        /* map the endpoint num to the index of the endpoint structure */
        index = USB_Map_Ep_To_Struct_Index(cdc_obj_ptr, ep_num); 
                
        producer = cdc_obj_ptr->ep[index].bin_producer;
		OS_Mutex_lock(cdc_obj_ptr->mutex);
        consumer = cdc_obj_ptr->ep[index].bin_consumer;
                                
        if(((uint8_t)(producer - consumer)) != (uint8_t)(CDC_MAX_QUEUE_ELEMS))  
        {   /* the bin is not full*/        
            uint8_t queue_num = (uint8_t)(producer % CDC_MAX_QUEUE_ELEMS);
            
            /* put all send request parameters in the endpoint data structure*/
            cdc_obj_ptr->ep[index].queue[queue_num].channel = ep_num;
            cdc_obj_ptr->ep[index].queue[queue_num].app_data.data_ptr = app_buff;
            cdc_obj_ptr->ep[index].queue[queue_num].app_data.data_size = size; 
            cdc_obj_ptr->ep[index].queue[queue_num].handle = 
            cdc_obj_ptr->controller_handle;         
            /* increment producer bin by 1*/       
            cdc_obj_ptr->ep[index].bin_producer = ++producer;

            if((uint8_t)(producer - consumer) == (uint8_t)1)         
            {          
    #endif      
                status = USB_Class_Send_Data(cdc_obj_ptr->class_handle,
                 ep_num, app_buff,size);
    #if CDC_IMPLEMENT_QUEUING
            }
        }
        else /* bin is full */
        {
            status = USBERR_DEVICE_BUSY; 
        }    
		OS_Mutex_unlock(cdc_obj_ptr->mutex);
    #endif 
    return status;
}
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
usb_status USB_Class_CDC_Recv_Data
(
    cdc_handle_t          cdc_handle,
    uint8_t              ep_num,
    uint8_t *          buff_ptr,      /* [IN] buffer to recv */      
    uint32_t             size           /* [IN] length of the transfer */
)
{
    cdc_device_struct_t * cdc_obj_ptr;
    usb_status status = USB_OK;

    cdc_obj_ptr = (cdc_device_struct_t *)cdc_handle;
    if (NULL == cdc_obj_ptr)
    {
        return USBERR_ERROR;    
    }
//    UTRACE("\n<-");
    status = usb_device_recv_data(cdc_obj_ptr->controller_handle,
    ep_num,buff_ptr,size);
    
    return status;
    
}

#endif /*CDC_CONFIG*/
/* EOF */
