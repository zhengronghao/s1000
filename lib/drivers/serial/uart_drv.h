/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
* File Name          : uart_drv.h
* Author             : luocs
* Version            : V0.00
* Date               : 2014.01.02
* Description        : 
*******************************************************************************/

#ifndef __UART_DRV_H__
#define __UART_DRV_H__ 

#include "common.h"


#define UART_USED           1
#define UART_UNUSED         0

#define UART_DMATYPE_SEND   0
#define UART_DMATYPE_RECE   1

/*Serial Port struct*/
typedef void (*UART_ISR_CALLBACK)(void);

typedef struct _UART_OPT
{
    S_QUEUE queue;
    UART_ISR_CALLBACK rev_isr;
    uint8_t used;
    uint8_t console;//1-used as console
    uint8_t rfu[2];
}UART_OPT_TYPE;

/*!< 中断及DMA配置 */
typedef enum
{
    UART_IT_Tx,                // 开启每发送一帧传输完成中断 
    UART_DMA_Tx,               // 开启每发送一帧传输完成触发DMA 
    UART_IT_Rx,                // 开启每接收一帧传输完成中断 
    UART_DMA_Rx,               // 开启每接收一帧传输完成触发DMA 
}uart_itdmaconfig_t;

typedef struct 
{
    uint8_t    chl;                              
    uint32_t   s_addr;  
    uint32_t   size; 
    uint32_t   d_addr;  
}dma_config_t;

extern UART_OPT_TYPE  sgSerialOpt[UART_TOTOL_NUM];

#define UART_QUEUE_PTR(uartch)  (&sgSerialOpt[uartch].queue)

/*Serial Port return para*/
#define     EUART_VAL                4101        //  无效的通讯参数
#define     EUART_NOTOPEN            4102        //  串口未打开
#define     EUART_BUSY               4103        //  串口忙

#define     USBSER_SUCCESS            0           //  操作成功
#define     USBSER_VAL                4201        //  无效的通讯参数
#define     USBSER_NOTOPEN            4202        //  USB串口未打开
#define     USBSER_BUSY               4203        //  USB串口忙
#define     USBSER_NOTREADY           4204        //  USB串口未准备好
#define     USBSER_TIMEOUT            4205        //  超时
/********************************************************************/

void drv_uart_init(UARTn uartch);
int drv_uart_open(UARTn uartch, int baud,
                  uint8_t *buffer,uint32_t buf_size,
                  UART_ISR_CALLBACK rev_isr);
int drv_uart_close(UARTn uartch);
int drv_uart_read(UARTn uartch,uint8_t *buffer,
                  uint32_t length,int32_t timout_ms);
int drv_uart_write(UARTn uartch,const uint8_t *buffer,uint32_t length);
int drv_uart_clear(UARTn uartch);
int drv_uart_check_readbuf(UARTn uartch);
int drv_uart_check_used(UARTn uartch);

int drv_uart_open_dma(UARTn uartch, int baud,
                  uint8_t *buffer,uint32_t buf_size,
                  int type, int channel);

int drv_uart_dma_close(UARTn uartch, int channel);

int drv_dma_recv_start(UARTn uartch,int channel,uint8_t *buffer,uint32_t buf_size); 

int uart_dma_check(void);
/********************************************************************/

#endif /* __UART_DRV_H__ */
