/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : console.c
 * bfief              : 
 * Author             :   
 * Email              : xxxxx@itep.com.cn
 * Version            : V0.00
 * Date               : 1/10/2014 2:59:21 PM
 * Description        : 
 *******************************************************************************/

/*****************************************************************************************************
 * Include files
 *****************************************************************************************************/
#include "common.h"
#include "drv_inc.h"

static UARTn sgConsolePort = SERIL_NOTVALID;

static void console_isr(void)
{
    if (sgConsolePort == SERIL_NOTVALID)
    {
        return;
    }
    char ch;
    ch = hw_uart_getchar(sgConsolePort);
    QueueWrite(&sgSerialOpt[sgConsolePort].queue,
               (unsigned char *)&ch,
               1);
}

int console_init(uint8_t comport,uint8_t *buffer,uint32_t buf_size)
{
    if (comport == USBD_CDC)
    {
#if defined(CFG_USBD_CDC)
        if (drv_usbd_cdc_open(buffer,buf_size) == 0) {
            sgConsolePort = (UARTn)comport;
            return 0;
        } else  {
            return -1;
        }
#else
        return -1;
#endif
    } else
    {
        if (drv_uart_open((UARTn)comport,CNL_BAUD,
                          buffer,buf_size,
                          console_isr) == 0) {
            sgConsolePort = (UARTn)comport;
            return 0;
        } else {
            return -1;
        }
    }
}

int console_close(void)
{
    if (sgConsolePort == USBD_CDC)
    {
        sgConsolePort  = SERIL_NOTVALID;
#if defined(CFG_USBD_CDC)
        return drv_usbd_cdc_close();
#else
        return 0;
#endif
    } else
    {
        int iRet;
        iRet = drv_uart_close(sgConsolePort);
        if (iRet == 0) {
            sgConsolePort = SERIL_NOTVALID;
        }
        return iRet;
    }
}

int console_check_buffer(void)
{
    int iRet=0;
    switch (sgConsolePort)
    {
    case SERIL_NOTVALID:
        break;
#if defined(CFG_USBD_CDC)
    case USBD_CDC:
        iRet = drv_usbd_cdc_check_readbuf();
        break;
#endif
    default:
        iRet = drv_uart_check_readbuf(sgConsolePort);
        break;
    }
    return iRet;
}

int console_clear(void)
{
    int iRet=0;
    switch (sgConsolePort)
    {
    case SERIL_NOTVALID:
        break;
#if defined(CFG_USBD_CDC)
    case USBD_CDC:
        iRet = drv_usbd_cdc_clear();
        break;
#endif
    default:
        iRet = drv_uart_clear(sgConsolePort);
        break;
    }
    return iRet;
}

int console_read_buffer(uint8_t *output,uint32_t length,int32_t timeout_ms)
{
    int iRet=-1;
    switch (sgConsolePort)
    {
    case SERIL_NOTVALID:
        break;
#if defined(CFG_USBD_CDC)
    case USBD_CDC:
        iRet = drv_usbd_cdc_read(output,length,timeout_ms);
        break;
#endif
    default:
        iRet = drv_uart_read(sgConsolePort,output,length,timeout_ms);
        break;
    }
    return iRet;
}

int console_check_used(void)
{
    int iRet = 0;
    switch (sgConsolePort )
    {
#if defined(CFG_USBD_CDC)
    case USBD_CDC:
        if (drv_usbd_cdc_ready() != 0)
        {
            iRet = -1;
            s_DelayMs(1000);
        }
        break;
#endif
    case SERIL_NOTVALID:
        iRet = -1;
        break;
    default:
        if ( drv_uart_check_used(sgConsolePort) <= 0 ) {
            iRet = -1;
        }
        break;
    }
    return iRet;
}

int console_get_class(void)
{
    return sgConsolePort;
}


/********************************************************************/
char in_char (void)
{
    char  ch=0;

    switch (sgConsolePort)
    {
    case SERIL_NOTVALID:
        break;
#if defined(CFG_USBD_CDC)
    case USBD_CDC:
        drv_usbd_cdc_read((uint8_t *)&ch,1,-1);
        break;
#endif
    default:
        ch = hw_uart_getchar(sgConsolePort);
        break;
    }
    return ch;
}
/********************************************************************/
void out_char (char ch)
{
    switch (sgConsolePort)
    {
    case SERIL_NOTVALID:
        break;
#if defined(CFG_USBD_CDC)
    case USBD_CDC:
        drv_usbd_cdc_write((uint8_t *)&ch,1);
        break;
#endif
    default:
        hw_uart_putchar(sgConsolePort, ch);
        hw_uart_tx_check(sgConsolePort);
        break;
    }
}
/********************************************************************/
int char_present(void)
{
    int iRet =0;
    switch (sgConsolePort)
    {
    case SERIL_NOTVALID:
        break;
#if defined(CFG_USBD_CDC)
    case USBD_CDC:
        iRet = drv_usbd_cdc_check_readbuf();
        break;
#endif
    default:
       iRet = hw_uart_getchar_present(sgConsolePort);
       break;
    }
    return iRet;
}
/********************************************************************/



