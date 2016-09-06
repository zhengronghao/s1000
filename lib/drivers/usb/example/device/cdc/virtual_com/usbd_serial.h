/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : usbd_serial.h
 * bfief              : 
 * Author             :   
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/6/2015 16:21:21 PM
 * Description        : 
 *******************************************************************************/

#ifndef __USBD_SERIAL_H__
#define __USBD_SERIAL_H__

#ifdef WP30_V1
#include "../../../../queue/queue.h"
#endif

typedef union _USBD_STATUS_UNION_T
{
    uint8_t byte;
    struct {
        uint8_t enum_succ:1;
        uint8_t start_transfer:1;
        uint8_t schedule_recv:1;
        uint8_t send_allow:1;
    }bit;
}USBD_STATUS_UNION_T;

struct USBD_SYS_STRUCT_T
{
    uint32_t  g_app_handle;
    uint8_t g_curr_recv_buf[64];//DATA_BUFF_SIZE
    S_QUEUE queue;
    uint8_t used;
    uint8_t console;//1-used as console
    USBD_STATUS_UNION_T status;
};

#define     USBD_USED           1
#define     USBD_UNUSED         0

#define     USBD_SUCCESS            0           //  操作成功
#define     USBD_VAL                4201        //  无效的通讯参数
#define     USBD_NOTOPEN            4202        //  USB串口未打开
#define     USBD_BUSY               4203        //  USB串口忙
#define     USBD_NOTREADY           4204        //  USB串口未准备好
#define     USBD_TIMEOUT            4205        //  超时
#define     USBD_NOTACTIVE          4206        //  USB串口未准备好


extern struct USBD_SYS_STRUCT_T gUSBDSys;
int drv_usbd_cdc_open(uint8_t *buffer,uint32_t buf_size);
int drv_usbd_cdc_close(void);
int drv_usbd_cdc_read(uint8_t *buffer,uint32_t length,int32_t timout_ms);
int drv_usbd_cdc_write(const uint8_t *buffer,uint32_t length);
int drv_usbd_cdc_clear(void);
int drv_usbd_cdc_check_readbuf(void);
int drv_usbd_cdc_ready(void);

#endif

