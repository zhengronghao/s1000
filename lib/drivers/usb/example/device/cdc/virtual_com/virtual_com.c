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
* $FileName: virtual_com.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief  The file emulates a USB PORT as RS232 PORT.
*****************************************************************************/ 

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "virtual_com.h"
#include "usbd_serial.h"

#if USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile usbd with this option.
#endif

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
extern uint32_t sys_get_counter(void);
/****************************************************************************
 * Global Variables
 ****************************************************************************/
extern usb_desc_request_notify_struct_t  desc_callback;

struct USBD_SYS_STRUCT_T gUSBDSys;

typedef union
{
    uint32_t periph;
    struct {
        uint32_t iccard:1;
        uint32_t sam1:1;
        uint32_t sam2:1;
        uint32_t rfc:1;
        uint32_t usb:1;
        uint32_t bt:1;
        uint32_t rfu:26;
    }bm;
}LPWR_Switch;

typedef struct 
{
    volatile  uint32_t	timercount;
    LPWR_Switch lpwr;
}SYSTTEM_TYPE_DEF;

extern SYSTTEM_TYPE_DEF gSystem;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void* arg);
uint8_t USB_Notif_Callback(uint8_t event, uint16_t value, uint8_t ** data, uint32_t* size, void* arg); 
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
uint8_t g_line_coding[LINE_CODING_SIZE] = 
{
 /*e.g. 0x00,0x10,0x0E,0x00 : 0x000E1000 is 921600 bits per second */
	(LINE_CODE_DTERATE_IFACE>> 0) & 0x000000FF,
	(LINE_CODE_DTERATE_IFACE>> 8) & 0x000000FF,
	(LINE_CODE_DTERATE_IFACE>>16) & 0x000000FF, 		 
	(LINE_CODE_DTERATE_IFACE>>24) & 0x000000FF,
	 LINE_CODE_CHARFORMAT_IFACE,
	 LINE_CODE_PARITYTYPE_IFACE,
	 LINE_CODE_DATABITS_IFACE
};

uint8_t g_abstract_state[COMM_FEATURE_DATA_SIZE] = 
{
	 (STATUS_ABSTRACT_STATE_IFACE>>0) & 0x00FF,
	 (STATUS_ABSTRACT_STATE_IFACE>>8) & 0x00FF																		
};

uint8_t g_country_code[COMM_FEATURE_DATA_SIZE] = 
{
 (COUNTRY_SETTING_IFACE>>0) & 0x00FF,
 (COUNTRY_SETTING_IFACE>>8) & 0x00FF															  
};

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
 
/**************************************************************************//*!
 *
 * @name  USB_Get_Line_Coding
 *
 * @brief The function returns the Line Coding/Configuraion
 *
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param coding_data:   output line coding data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Get_Line_Coding(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *coding_data)
{   
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *coding_data = g_line_coding;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Line_Coding
 *
 * @brief The function sets the Line Coding/Configuraion
 *
 * @param handle: handle     
 * @param interface:     interface number     
 * @param coding_data:   output line coding data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Set_Line_Coding(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *coding_data)
{   
    uint8_t count;

    UNUSED_ARGUMENT(handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set line coding data*/
        for (count = 0; count < LINE_CODING_SIZE; count++) 
        {          
            g_line_coding[count] = *((*coding_data+USB_SETUP_PKT_SIZE) + count);
        }
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Get_Abstract_State
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (ABSTRACT_STATE)
 * @param handle:        handle
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Get_Abstract_State(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *feature_data)
{   
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *feature_data = g_abstract_state;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Get_Country_Setting
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (COUNTRY_CODE)
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Get_Country_Setting(uint32_t handle, 
                                    uint8_t interface, 
                                    uint8_t * *feature_data)
{   
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *feature_data = g_country_code;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Abstract_State
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (ABSTRACT_STATE)
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Set_Abstract_State(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *feature_data)
{   
    uint8_t count;
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set Abstract State Feature*/
        for (count = 0; count < COMM_FEATURE_DATA_SIZE; count++) 
        {          
            g_abstract_state[count] = *(*feature_data + count);
        }
        return USB_OK; 
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Country_Setting
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (COUNTRY_CODE)
 * @param handle: handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Set_Country_Setting(uint32_t handle, 
                                    uint8_t interface, 
                                    uint8_t * *feature_data)
{   
    uint8_t count;
    UNUSED_ARGUMENT (handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        for (count = 0; count < COMM_FEATURE_DATA_SIZE; count++) 
        {          
            g_country_code[count] = *(*feature_data + count);
        }
        return USB_OK; 
    }
    
    return USBERR_INVALID_REQ_TYPE;
}
/*****************************************************************************
* 
*	@brief		 This function do initialization for APP.
* 
*	@param		 None
* 
*	@return 	 None
**				  
*****************************************************************************/
void USBD_CDC_APP_init(void)
{
    cdc_config_struct_t cdc_config;
    cdc_config.cdc_application_callback.callback = USB_App_Callback;
    cdc_config.cdc_application_callback.arg = &gUSBDSys.g_app_handle;
    cdc_config.vendor_req_callback.callback = NULL;
    cdc_config.vendor_req_callback.arg = NULL;
    cdc_config.class_specific_callback.callback = USB_Notif_Callback;
    cdc_config.class_specific_callback.arg = &gUSBDSys.g_app_handle;
    cdc_config.desc_callback_ptr =  &desc_callback;
    /* Always happend in control endpoint hence hard coded in Class layer*/
    
    /* Initialize the USB interface */
    USB_Class_CDC_Init(CONTROLLER_ID, &cdc_config, &gUSBDSys.g_app_handle);
}
/******************************************************************************
 * 
 *    @name        USB_App_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void* arg) 
{
    uint32_t handle;
    handle = *((uint32_t *)arg);
    if(event_type == USB_DEV_EVENT_BUS_RESET)
    {
        gUSBDSys.status.bit.enum_succ = FALSE;    
        gUSBDSys.status.bit.schedule_recv = 0;
//        UTRACE("\n-|reset");
    }
    else if(event_type == USB_DEV_EVENT_CONFIG_CHANGED)
    {
        /* Schedule buffer for receive */
        if (gUSBDSys.status.bit.schedule_recv == 0)
        {
            USB_Class_CDC_Recv_Data(handle, DIC_BULK_OUT_ENDPOINT, gUSBDSys.g_curr_recv_buf, DIC_BULK_OUT_ENDP_PACKET_SIZE); 
            gUSBDSys.status.bit.schedule_recv = 1;
        }
        gUSBDSys.status.bit.enum_succ = TRUE;
        gUSBDSys.status.bit.send_allow = TRUE;
//        UTRACE("\n-|enum_ok");
    }
    else if(event_type == USB_DEV_EVENT_ERROR)
    {
        /* add user code for error handling */
    }
    return;
}

/******************************************************************************
 * 
 *    @name        USB_Notif_Callback
 *    
 *    @brief       This function handles the callback for Get/Set report req  
 *                  
 *    @param       request  :  request type
 *    @param       value    :  give report type and id
 *    @param       data     :  pointer to the data 
 *    @param       size     :  size of the transfer
 *
 *    @return      status
 *                  USB_OK  :  if successful
 *                  else return error
 *
 *****************************************************************************/
 
uint8_t USB_Notif_Callback
(
    uint8_t event, 
    uint16_t value, 
    uint8_t ** data, 
    uint32_t* size,
    void* arg
) 
{
    cdc_handle_t handle;
    uint8_t error = USB_OK;
    handle = *((cdc_handle_t *)arg);
//    UTRACE("\n-|event:%02X",event);
    switch(event)
    {
		case GET_LINE_CODING:
			error = USB_Get_Line_Coding(handle, value, data);
		break;
		case GET_ABSTRACT_STATE:
			error = USB_Get_Abstract_State(handle, value, data);
		break;
		case GET_COUNTRY_SETTING:
			error = USB_Get_Country_Setting(handle, value, data);
		break;
		case SET_LINE_CODING:
			error = USB_Set_Line_Coding(handle, value, data);
		break;
		case SET_ABSTRACT_STATE:
			error = USB_Set_Abstract_State(handle, value, data);
		break;
		case SET_COUNTRY_SETTING:
			error = USB_Set_Country_Setting(handle, value, data);
		break;
		case USB_APP_CDC_DTE_ACTIVATED:
			if(gUSBDSys.status.bit.enum_succ == TRUE)
			{
				gUSBDSys.status.bit.start_transfer = TRUE; 
//                 UTRACE("\n-|activated");
			}
		break;
		case USB_APP_CDC_DTE_DEACTIVATED:
			if(gUSBDSys.status.bit.enum_succ == TRUE)
			{
				gUSBDSys.status.bit.start_transfer = FALSE; 
//                 UTRACE("\n-|dte deactivated");
			}
		break;
		case USB_DEV_EVENT_DATA_RECEIVED:
		{
            if((gUSBDSys.status.bit.enum_succ == TRUE) && (gUSBDSys.status.bit.start_transfer == TRUE))
            {
                if (gUSBDSys.used == USBD_USED) {
                    QueueWrite(&gUSBDSys.queue,*data,*size);
                }
                USB_Class_CDC_Recv_Data(handle, DIC_BULK_OUT_ENDPOINT, gUSBDSys.g_curr_recv_buf, DIC_BULK_OUT_ENDP_PACKET_SIZE); 
                gUSBDSys.status.bit.schedule_recv = 1;
            }
//            UTRACE("\nrecv event");
        }
		break;
		case USB_DEV_EVENT_SEND_COMPLETE:
		{
	        if ((size != NULL) && (*size != 0) && !(*size % DIC_BULK_IN_ENDP_PACKET_SIZE))
	        {
				/* If the last packet is the size of endpoint, then send also zero-ended packet,
				** meaning that we want to inform the host that we do not have any additional
				** data, so it can flush the output.
		             */
	            USB_Class_CDC_Send_Data(gUSBDSys.g_app_handle, DIC_BULK_IN_ENDPOINT, NULL, 0);
//                UTRACE(">sd_zero_len");
	        } else if((gUSBDSys.status.bit.enum_succ == TRUE) && (gUSBDSys.status.bit.start_transfer == TRUE))
            {
				 if((size != NULL) || ((size == NULL) && (*size == 0)))
				 {
					 /* User: add your own code for send complete event */ 
					 /* Schedule buffer for next receive event */
                     if (gUSBDSys.status.bit.schedule_recv == 0)
                     {
                         USB_Class_CDC_Recv_Data(handle, DIC_BULK_OUT_ENDPOINT, gUSBDSys.g_curr_recv_buf, DIC_BULK_OUT_ENDP_PACKET_SIZE); 
                         gUSBDSys.status.bit.schedule_recv = 1;
                     }
				 }
//                 UTRACE(">sd_len");
                 gUSBDSys.status.bit.send_allow = TRUE;
            }
		}
		break;
        default:
        error = USBERR_INVALID_REQ_TYPE;
        UTRACE(" not defined");
        break;
    }

    return error;
}



//=============================================================
//=============================================================


void drv_usbd_cdc_init(void)
{
    gUSBDSys.queue.pBuffer = NULL;
    gUSBDSys.queue.bufSize= 0;
    gUSBDSys.queue.tail= 0;
    gUSBDSys.queue.semaphore= 0;
    gUSBDSys.used = USBD_UNUSED;
    gUSBDSys.console = 0;
}

/**  
 * \brief   Initialize the UART for 8N1 operation, interrupts disabled, and 
 *          no hardware flow-control
 * \author   
 * \param 
 * \return none
 * \warning . 
 */
int drv_usbd_cdc_open(uint8_t *buffer,uint32_t buf_size)
{
    if (gUSBDSys.used == USBD_USED)
    {
        return -USBD_BUSY;
    }
    memset(&gUSBDSys,0x00,sizeof(gUSBDSys));
    gUSBDSys.used = USBD_USED;
    QueueInit(&gUSBDSys.queue,
              buffer,buf_size);
    USBD_CDC_APP_init();

    gSystem.lpwr.bm.usb = 1;
    return USBD_SUCCESS;
}

int drv_usbd_cdc_close(void)
{
    gSystem.lpwr.bm.usb = 0;
    if (gUSBDSys.used == USBD_USED)
    {
        gUSBDSys.used = USBD_UNUSED;
        USB_Class_CDC_Deinit(gUSBDSys.g_app_handle);
        drv_usbd_cdc_init();
        return USBD_SUCCESS;
    } else 
    {
        return -USBD_NOTOPEN;
    }
}

inline static int _drv_usbd_cdc_para_check(void)
{
    if (gUSBDSys.used == USBD_UNUSED) {
        return -USBD_NOTOPEN;
    }
    if (gUSBDSys.status.bit.enum_succ ==FALSE) 
    {
        return -USBD_NOTREADY;
    }
    if (gUSBDSys.status.bit.start_transfer == FALSE)
    {
        return -USBD_NOTACTIVE;
    }

    return USBD_SUCCESS;
}

int drv_usbd_cdc_read(uint8_t *buffer,uint32_t length,int32_t timout_ms)
{
    uint32_t begin_time;
    uint32_t rev_length,tmp;
    int iRet;

    iRet = _drv_usbd_cdc_para_check();
    if (iRet < 0 && iRet != -USBD_NOTACTIVE) {
        return iRet;
    }
    if (NULL == buffer) {
        return -USBD_VAL;
    }
    if (length == 0) {
        return length;
    }
    rev_length = 0;
    begin_time = sys_get_counter();
    while (1)
    {
        tmp = QueueRead(&gUSBDSys.queue,
                         buffer+rev_length,
                         length);
        rev_length += tmp;
        if (rev_length == length) {
            break;
        }
        if ( (timout_ms >= 0) && (sys_get_counter() - begin_time > timout_ms)) {
            break;
        }
    }
    
    return rev_length;
}

int drv_usbd_cdc_write(const uint8_t *buffer,uint32_t length)
{
    int iRet;
    uint8_t usb_pack[DIC_BULK_IN_ENDP_PACKET_SIZE];
    uint32_t index = 0;
    uint8_t tmp=0;

    iRet = _drv_usbd_cdc_para_check();
    if (iRet != 0) {
        return iRet;
    }
    if (gUSBDSys.status.bit.start_transfer ==FALSE) {
        return -USBD_NOTACTIVE;
    }
    if (gUSBDSys.status.bit.send_allow == FALSE) {
        return -USBD_BUSY;
    }
    if (NULL == buffer) {
        return -USBD_VAL;
    }
    if (length == 0) {
        return length;
    }

    while (1)
    {
        if (length >= DIC_BULK_IN_ENDP_PACKET_SIZE) {
            tmp = DIC_BULK_IN_ENDP_PACKET_SIZE;
        } else {
            tmp = length;
        }
        memcpy(usb_pack,buffer+index,tmp);
        gUSBDSys.status.bit.send_allow = FALSE;
        if (USB_Class_CDC_Send_Data(gUSBDSys.g_app_handle,
                                    DIC_BULK_IN_ENDPOINT,
                                    usb_pack,tmp) == 0)
        {
            uint32_t begin_time,timeout_ms;
            begin_time = sys_get_counter();
            timeout_ms = length/32+1500;
            while (gUSBDSys.status.bit.send_allow == FALSE)
            {
                if ( sys_get_counter() - begin_time > timeout_ms ) {
                    return -USBD_TIMEOUT;
                }
            }
            index += tmp;
            length -= tmp;
            if (length == 0) {
                return index;
            }
//            return length;
        } else
        {
            return -USBD_BUSY;
        }
    }
}


int drv_usbd_cdc_clear(void)
{
    int iRet;

    iRet = _drv_usbd_cdc_para_check();
    if (iRet != 0) {
        return iRet;
    }
    
    QueueReset(&gUSBDSys.queue);   

    return 0;
}

int drv_usbd_cdc_check_readbuf(void)
{
    int iRet;

    iRet = _drv_usbd_cdc_para_check();
    if (iRet != 0) {
        return iRet;
    }

    return QueueDataSize(&gUSBDSys.queue);
}

int drv_usbd_cdc_ready(void)
{
    return _drv_usbd_cdc_para_check();
}

/* EOF */
