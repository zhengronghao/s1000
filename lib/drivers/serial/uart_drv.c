/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
* File Name          : uart_drv.c
* bfief              : The drivers level of UART
* Author             : luocs
* Version            : V0.00
* Date               : 2014.01.02
* Description        : 
*******************************************************************************/

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "drv_inc.h"

UART_OPT_TYPE sgSerialOpt[UART_TOTOL_NUM] UPAREA;

//static int recv_data1[1024];

#define UART_CALLBACK_R(uartch)  (sgSerialOpt[uartch].rev_isr)

void drv_uart_init(UARTn uartch)
{
    if (uartch > UART_TOTOL_NUM) {
        return;
    }
    sgSerialOpt[uartch].queue.pBuffer = NULL;
    sgSerialOpt[uartch].queue.bufSize= 0;
    sgSerialOpt[uartch].queue.tail= 0;
    sgSerialOpt[uartch].queue.semaphore= 0;
    sgSerialOpt[uartch].rev_isr = NULL;
    sgSerialOpt[uartch].used = UART_UNUSED;
    sgSerialOpt[uartch].console = 0;
}

int drv_uart_check_used(UARTn uartch)
{
    if (uartch >= USBD_CDC) {
        return -EUART_VAL;
    }
    return sgSerialOpt[uartch].used;
}

/**  
 * \brief   Initialize the UART for 8N1 operation, interrupts disabled, and 
 *          no hardware flow-control
 * \author   
 * \param 
 *         uartch      UART channel to initialize:UART0~UART3
 *         baud        UART baud rate
 * \return none
 * \warning . 
 */
int drv_uart_open(UARTn uartch, int baud,
                  uint8_t *buffer,uint32_t buf_size,
                  UART_ISR_CALLBACK rev_isr)
{
    if (uartch >= USBD_CDC)
    {
        return -EUART_VAL;
    }
    if (sgSerialOpt[uartch].used == UART_USED)
    {
        return -EUART_BUSY;
    }
    sgSerialOpt[uartch].used = UART_USED;
    QueueInit(&sgSerialOpt[uartch].queue,
              buffer,buf_size);
    hw_uart_init(uartch,baud);
    if (rev_isr != NULL) {
        sgSerialOpt[uartch].rev_isr = rev_isr;
        hw_uart_irq_open(uartch);
    }

    return 0;
}

int drv_uart_close(UARTn uartch)
{
    if (uartch >= USBD_CDC) {
        return -EUART_VAL;
    }
    if (sgSerialOpt[uartch].used == UART_USED) {
        sgSerialOpt[uartch].used = UART_UNUSED;
        hw_uart_irq_close(uartch);
//        drv_uart_init(uartch);
    }

    return 0;
}

inline static int _drv_uart_para_check(UARTn uartch)
{
   if (uartch >= USBD_CDC) {
        return -EUART_VAL;
    }
    if (sgSerialOpt[uartch].used == UART_UNUSED) {
        return -EUART_NOTOPEN;
    }

    return 0;
}

int drv_uart_read(UARTn uartch,uint8_t *buffer,
                  uint32_t length,int32_t timout_ms)
{
    uint32_t begin_time;
    uint32_t rev_length,tmp;
    int iRet;

    iRet = _drv_uart_para_check(uartch);
    if (iRet != 0) {
        return iRet;
    }
    if (NULL == buffer) {
        return -EUART_VAL;
    }
    if (length == 0) {
        return length;
    }
    rev_length = 0;
    begin_time = sys_get_counter();
    while (1)
    {
        tmp = QueueRead(&sgSerialOpt[uartch].queue,
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

int drv_uart_write(UARTn uartch,const uint8_t *buffer,uint32_t length)
{
    uint32_t i;
    int iRet;

    iRet = _drv_uart_para_check(uartch);
    if (iRet != 0) {
        return iRet;
    }
    if (NULL == buffer) {
        return -EUART_VAL;
    }
    if (length == 0) {
        return length;
    }
    for ( i=0; i<length; i++ ) {
        if (hw_uart_putchar(uartch,(char)buffer[i]) != 0)
        {
            return i;
        }
    }
    hw_uart_tx_check(uartch);
    
    return length;
}


int drv_uart_clear(UARTn uartch)
{
    int iRet;

    iRet = _drv_uart_para_check(uartch);
    if (iRet != 0) {
        return iRet;
    }
    
    QueueReset(&sgSerialOpt[uartch].queue);   

    return 0;
}

int drv_uart_check_readbuf(UARTn uartch)
{
    int iRet;

    iRet = _drv_uart_para_check(uartch);
    if (iRet != 0) {
        return iRet;
    }

    return QueueDataSize(&sgSerialOpt[uartch].queue);
}


void UART0_IRQHandler(void)
{
    if (UART_CALLBACK_R(UART0) != NULL) {
        UART_CALLBACK_R(UART0)();
    }
}

void UART1_IRQHandler(void)
{
    if (UART_CALLBACK_R(UART1) != NULL) {
        UART_CALLBACK_R(UART1)();
    }
}

void UART2_IRQHandler(void)
{
    if (UART_CALLBACK_R(UART2) != NULL) {
        UART_CALLBACK_R(UART2)();
    }
//    ch = hw_uart_getchar(UART2);
}

void UART3_IRQHandler(void)
{
    if (UART_CALLBACK_R(UART3) != NULL) {
        UART_CALLBACK_R(UART3)();
    }
}

void UART4_IRQHandler(void)
{
    if (UART_CALLBACK_R(UART4) != NULL) {
        UART_CALLBACK_R(UART4)();
    }
}
/********************************************************************/
void drv_uart_itdmaconfig(uint32_t instance, uart_itdmaconfig_t config, bool status)
{
    UART_ITDMAConfig(instance, (UART_ITDMAConfig_Type)config, status);
}

void drv_uart_dma_init(uint32_t instance, uart_itdmaconfig_t config, dma_init_t *dma_init)
{
    drv_uart_itdmaconfig(instance, config, true);
    drv_dma_init(dma_init);
}

void drv_uart_dma_start(uint32_t instance, dma_config_t *dma_config)
{
    drv_dma_set_daddr(dma_config->chl, dma_config->d_addr);
    drv_dma_set_saddr(dma_config->chl, dma_config->s_addr);
    drv_dma_set_major_counter(dma_config->chl, dma_config->size);
    UART_Disable_Tx_Rx( instance);
    drv_dma_enable(dma_config->chl);
    UART_Enable_Tx_Rx(instance);
}

//串口DMA初始化
extern struct UART_MemMap *const UARTx[5];
/**  
 * \brief   Initialize the UART for dma operation, 
 * \author  zhengj   
 * \param   instance: the num of uart
 *          config:   the param about dma channel type
 *          dma_init: the param about dma
 *         
 *         baud        UART baud rate
 * \return none
 * \warning . 
 */
//void drv_uart_dma_init(uint32_t instance, uart_itdmaconfig_t config, dma_init_t *dma_init)
//{
//
//    drv_uart_itdmaconfig(instance, config, true);
//    drv_dma_init(dma_init);
//}

int drv_uart_dam_recv_init(int uart,uint32 daddr,int buf_len, int channel)
{
    int sourece;
    dma_init_t dma_init = {0};
    dma_config_t dma_config;

    switch(uart)
    {
    case UART0:
        sourece = UART0_REV_DMAREQ; //触发源
        break; 
    case UART1:
        sourece = UART1_REV_DMAREQ; //触发源
        break; 
    case UART2:
        sourece = UART2_REV_DMAREQ; //触发源
        break; 
    case UART3:
        sourece = UART3_REV_DMAREQ; //触发源
        break; 
    case UART4:
        sourece = UART4_REV_DMAREQ; //触发源
        break;
    default:
        return ERROR;
    }
//    dma_init.majorLoopCnt = 1;
//    dma_init.dLastAddrAdj = 0;

    dma_init.chl = channel;
    dma_init.chlTriggerSource = sourece; 
    dma_init.triggerSourceMode = DMA_TriggerSource_Normal;
    dma_init.minorLoopByteCnt = 1;                       //次循环次数 一次DMA中断循环次数
    dma_init.majorLoopCnt = buf_len;                     //主循环次数
//    dma_init.majorLoopCnt = 1;                     //主循环次数
    dma_init.sAddr = (uint32_t)(&UART_D_REG(UARTx[uart]));             //源地址 串口数据寄存器地址
    dma_init.sLastAddrAdj = 0; 
    dma_init.sAddrOffset = 0;       //每次读取后 地址不偏移
    dma_init.sDataWidth = DMA_DataWidthBit_8;
    dma_init.sMod = DMA_ModuloDisable;
    dma_init.dAddr = daddr;            //目的地址 
    dma_init.dLastAddrAdj =  -buf_len;                       //一次主循环完成，目的地址偏移量 
//    dma_init.dLastAddrAdj =  0;                       //一次主循环完成，目的地址偏移量 
    dma_init.dAddrOffset = 1;                         //每存入1次数据 目的地址偏移量+1
    dma_init.dDataWidth = DMA_DataWidthBit_8;
    dma_init.dMod = DMA_ModuloDisable;
    drv_uart_dma_init(uart, UART_DMA_Rx, &dma_init);

    dma_config.chl = channel;
    dma_config.s_addr = (uint32)(&UART_D_REG(UARTx[uart]));
    dma_config.d_addr = daddr;
    dma_config.size = buf_len;
    drv_uart_dma_start(uart, &dma_config);
 
    return 0;
}


int drv_uart_dam_send_init(int uart,uint32 daddr,int buf_len, int channel)
{
    return 0;
}

void hw_uart_errirq_enable(UARTn uartch)
{
    /* Enable uart revcive interrupts*/
//    UART_C3_REG(UARTx[UART4]) |= UART_C3_FEIE_MASK|UART_C3_ORIE_MASK;
//    UART_C3_REG(UARTx[UART4]) |= UART_C3_FEIE_MASK;
    if ( uartch > USBD_CDC ) {
       return ; 
    }
    UART_C3_REG(UARTx[uartch]) |= (UART_C3_ORIE_MASK|UART_C3_FEIE_MASK);
    /* Enable revcive pin irq interrupts*/
    if ( uartch <  UART4) {
        enable_irq((uartch << 1) + UART0_ERR_IRQn);		
    }else{
        enable_irq(((uartch-UART4) << 1) + UART4_ERR_IRQn); 
    }
}

void UART_ERR_IRQHandler(UARTn uartch)
{
    volatile uint32_t dummy;
//    if(UART_S1_REG(UARTx[uartch])& (UART_S1_OR_MASK|UART_S1_FE_MASK) )
//    if(UART_S1_REG(UARTx[uartch])& UART_S1_FE_MASK )

    volatile uint32_t status ;
    status = UART_S1_REG(UARTx[uartch]); 
    if(status & UART_S1_OR_MASK )
    {
        dummy = UART_S1_REG(UARTx[uartch]); 
        dummy = UART_D_REG(UARTx[uartch]);
        TRACE("or err\r\n");
//        NVIC_SystemReset();
    }
    if(status & UART_S1_FE_MASK)
    {
        dummy = UART_S1_REG(UARTx[uartch]); 
        dummy = UART_D_REG(UARTx[uartch]);
        TRACE("fr err\r\n");
//        NVIC_SystemReset();
    }
}


void UART4_ERR_IRQHandler(void)
{
    UART_ERR_IRQHandler(UART4);
}
/**  
 * \brief   Initialize the UART for 8N1 operation, interrupts disabled, and 
 *          no hardware flow-control
 * \author   
 * \param 
 *         uartch      UART channel to initialize:UART0~UART3
 *         baud        UART baud rate
 *         type        Send or Receive
 * \return none
 * \warning . 
 */
int drv_uart_open_dma(UARTn uartch, int baud,
                  uint8_t *buffer,uint32_t buf_size,
                  int type, int channel)
{
    if (uartch >= USBD_CDC)
    {
        return -EUART_VAL;
    }
    if (sgSerialOpt[uartch].used == UART_USED)
    {
        return -EUART_BUSY;
    }
    sgSerialOpt[uartch].used = UART_USED;
//    QueueInit(&sgSerialOpt[uartch].queue,
//              buffer,buf_size);
    hw_uart_init(uartch,baud);     //串口初始化
    hw_uart_errirq_enable(uartch);
//    hw_uart_init1(uartch,baud);     //串口初始化
    if (type == UART_DMATYPE_SEND) 
    {
        // drv_uart_dam_send_init(int uart,uint32 daddr,int buf_len);
    }
    else if(type == UART_DMATYPE_RECE)
    {
//        TRACE("dma rece init add:%x\r\n", (uint32)(&buffer[0]));
        drv_uart_dam_recv_init(uartch,(uint32)(&buffer[0]),buf_size, channel);
    }

    return 0;
}

int drv_uart_dma_close(UARTn uartch, int channel)
{
    if (uartch >= USBD_CDC) {
        return -EUART_VAL;
    }
    if (sgSerialOpt[uartch].used == UART_USED) {
        sgSerialOpt[uartch].used = UART_UNUSED;
//        hw_uart_irq_close(uartch);
    }

//    drv_dma_stop();
    drv_dma_disable( channel);
    drv_dma_clock_disable();
    UART_C2_REG(UARTx[uartch]) &= ~(UART_C2_TE_MASK
                                    | UART_C2_RE_MASK );
    /* disable the clock to the UARTx */    
    if ( uartch <  UART4) {
        SIM_SCGC4 &= ~(SIM_SCGC4_UART0_MASK<<uartch);
    }else{
        SIM_SCGC1 &= ~(SIM_SCGC1_UART4_MASK<<uartch);
    }
    return 0;
}

int drv_dma_recv_start(UARTn uartch,int channel,uint8_t *buffer,uint32_t buf_size) 
{
    if (uartch >= USBD_CDC)
    {
        return -EUART_VAL;
    }
    if (sgSerialOpt[uartch].used == UART_USED)
    {
        return -EUART_BUSY;
    }
    dma_config_t dma_config;
    dma_config.chl = channel;
    dma_config.s_addr = (uint32)(&UART_D_REG(UARTx[uartch]));
    dma_config.d_addr = (uint32)(&buffer[0]);
    dma_config.size = buf_size;
    drv_uart_dma_start(uartch, &dma_config);
    return 0;
}

#if 0
void uart_dam_test(UARTn uart)
{
    int i=0;
    int key;
    char buffer[5];

    drv_uart_close(uart);   

//    lcd_display(0,0, 12|0x100|4, "串口初始化....");
    kb_getkey(-1);
    drv_uart_open_dma(uart,DL_BAUD,buffer,sizeof(buffer),NULL);

    TRACE("串口3test.....\r\n");
    TRACE("buf_len:%d.....\r\n",2048);
    
    memset(gwp30SysBuf_c.work,'\0',2048);
    
    while(1)
    {
        key = kb_getkey(100);
        if(key != KEY_ENTER)
        {
              while(gwp30SysBuf_c.work[0] != '\0')
              {
                  TRACE("i:%d--%d\r\n",i,gwp30SysBuf_c.work[i]);
                  gwp30SysBuf_c.work[i] = '\0';
                  i++; 
                  if(i == 2048)
                  {
                      i = 0;
                  }
              }
//              TRACE("DADDR:%x\r\n",DMA_BASE_PTR->TCD[HW_DMA_CH1].DADDR);
//              TRACE("CITER:%d\r\n",DMA_BASE_PTR->TCD[HW_DMA_CH1].CITER_ELINKNO);
//              sys_delay_ms(600);
        }
        else
        {
            break;
        }
    }
    drv_uart_close(uart);
}
#endif

