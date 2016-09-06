/*
 * File:		uart_hw.h
 * Purpose:     Provide common ColdFire UART routines for polled serial IO
 *
 * Notes:
 */

#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

typedef enum  _UARTn
{
    UART0,
    UART1,
    UART2,
    UART3,
    UART4,

    USBD_CDC,
    SERIL_NOTVALID
}UARTn;

typedef enum
{
    kUART_IT_Tx,                // 开启每发送一帧传输完成中断 
    kUART_DMA_Tx,               // 开启每发送一帧传输完成触发DMA 
    kUART_IT_Rx,                // 开启每接收一帧传输完成中断 
    kUART_DMA_Rx,               // 开启每接收一帧传输完成触发DMA 
}UART_ITDMAConfig_Type;

//#define IP_CLK_ENABLE(x)        (*((uint32_t*) ClkTbl[x].addr) |= ClkTbl[x].mask)
//#define IP_CLK_DISABLE(x)       (*((uint32_t*) ClkTbl[x].addr) &= ~ClkTbl[x].mask)

/* interrupt handler table */
static const IRQn_Type UART_IRQnTable[] = 
{
    UART0_RX_TX_IRQn,
    UART1_RX_TX_IRQn,
    UART2_RX_TX_IRQn,
#ifdef UART3
    UART3_RX_TX_IRQn,
#endif
#ifdef UART4
    UART4_RX_TX_IRQn,
#endif
#ifdef UART5
    UART5_RX_TX_IRQn,
#endif
};


/********************************************************************/

void hw_uart_init(UARTn uartch, uint32_t baud);
void hw_uart_irq_open(UARTn uartch);
void hw_uart_irq_close(UARTn uartch);
char hw_uart_getchar (UARTn uartch);
int hw_uart_putchar (UARTn  uartch, char ch);
int hw_uart_getchar_present (UARTn  uartch);
void hw_uart_tx_check(UARTn uartch);

void UART_ITDMAConfig(uint32_t instance, UART_ITDMAConfig_Type config, bool status);
void UART_Disable_Tx_Rx(uint32_t instance);
void UART_Enable_Tx_Rx(uint32_t instance);
/********************************************************************/

#endif /* __UART_H__ */
