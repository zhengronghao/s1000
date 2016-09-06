#include "wp30_ctrl.h"

#ifdef CFG_SCANER

SCANER_ASY_PARAM g_em3096_param;

void em3096_param_clear(void)
{
    memset(&g_em3096_param, 0, sizeof(SCANER_ASY_PARAM));
}

// 设置超时时间 单位ms
void em3096_timeout_set(uint32_t time)
{
    g_em3096_param.timecount = time;
}

void em3096_timeout_clear(void)
{
    g_em3096_param.timecount = 0;
}

void em3096_timer_isr(void);
void em3096_pit_end(void)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = SCANPIT;
    pit.PIT_Isr = em3096_timer_isr;
    LPLD_PIT_Deinit(&pit);
}

void em3096_timer_isr(void)
{
    if ( g_em3096_param.timecount > 0 ) {
        g_em3096_param.timecount--;
    }
    if ( g_em3096_param.timecount == 0 ) {
        g_em3096_param.scan_timeout = 1;       // 超时标志
        
        em3096_trig_finish();
        em3096_pit_end();
    }
}

void em3096_pit_init(void)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = SCANPIT;
    pit.PIT_Period = 60; 
    pit.PIT_Isr = em3096_timer_isr;
    LPLD_PIT_Init(&pit);
}

void em3096_pit_start(int clk)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = SCANPIT;
    pit.PIT_Isr = em3096_timer_isr;
    pit.PIT_Period = 60*(uint)clk; 
    LPLD_PIT_Restart(&pit);
}

//void dbg_pit(void)
//{
//    s_pit_count = 0;
//    em3096_pit_init();
//    em3096_pit_start(1000); // 1ms定时
//}

static void __com_em3096_init()
{
    // 使能引脚
    gpio_set_output(SCAN_PTxy_EN,GPIO_OUTPUT_SlewRateFast,0); 
    gpio_set_bit(SCAN_PTxy_EN,1);
    
    gpio_set_output(SCAN_PTxy_TRIG,GPIO_OUTPUT_SlewRateFast,0); 
    gpio_set_bit(SCAN_PTxy_TRIG,0);


    // 初始化串口
    drv_uart_open((UARTn)SCAN_COMPORT, SCAN_BAUD, gucBuff, PUBBUFFER_LEN, UartAppCallBack[SCAN_COMPORT]); 

    em3096_pit_init();
    em3096_param_clear();
}

void em3096_trig_begin(void)
{
    gpio_set_bit(SCAN_PTxy_TRIG,1);
}

void em3096_trig_finish(void)
{
    gpio_set_bit(SCAN_PTxy_TRIG,0);
}


uint8_t com_em3096_close(void)
{
    uint8_t ret = RET_OK;
    
    gpio_set_bit(SCAN_PTxy_EN,0);
    drv_uart_close(SCAN_COMPORT);
    
    em3096_pit_end();
    return ret;
}

uint8_t com_em3096_write(uint8_t *data , uint32_t len)
{
    uint8_t ret ;
    
    if (data == NULL) {
        ret = RET_PARAM;
        goto RET;
    }
    
    drv_uart_write(SCAN_COMPORT,data, len);
    ret = RET_OK;
    
RET:
 
    return ret;
}

uint32_t com_em3096_read(uint8_t *data )
{
//    uint8_t ret = RET_TIMEOUT;
    uint32_t len = 0;
    uint8_t i = 0;
    int iRet;

    uint32_t starttime;
    
    if (data == NULL) {
        goto RET;
    }
    
    iRet = drv_uart_check_readbuf(SCAN_COMPORT);
    if(iRet > 0)
    {
        starttime = sys_get_counter();
        while (1) {
            if ( sys_get_counter()-starttime >= 2000 ) {
                return 0;
            }
            i = drv_uart_read(SCAN_COMPORT, &data[len], 1, 200);
            if (i == 1) {
                len++;  
                if (len > 2 && data[len-1] == 0x0a && data[len-2]==0x0d) {
                    //                ret = RET_OK;
                    break;
                } 
            }
        }
    }
    
RET:
    return len;
}

uint16_t com_em3096_scan_start(uint32_t timeout)
{
//    uint32_t len = 0;
   // unsigned char sendbuf[20];
   // unsigned char recv_buf[256];
   // unsigned char send_len;
   // char ret;

#if 0
    //设置超时时间
    memset(sendbuf, 0, sizeof(sendbuf));
    memset(recv_buf, 0, sizeof(recv_buf));
    TRACE("scaner time out:%d",timeout);
    send_len = 11;
    memcpy(sendbuf,"NLS0313000=",send_len);
    ret = sprintf((char *)(sendbuf+send_len),"%d;",timeout);
    TRACE("\r\n%s",sendbuf); 
    if(ret < 0)
    {
        return RET_WRITE_DEVICE; 
    }
    send_len += ret;
    com_em3096_write(sendbuf, send_len);
    ret = com_em3096_open_read(recv_buf, 500);
    if (ret > 0) {
        if (recv_buf[0] == 0x06) {
            ret = RET_OK;
        } else {
            TRACE("\r\nscaner :%d",ret); 
            return RET_WRITE_DEVICE;
        }
    } else {
        TRACE("\r\nscaner 设置失败:%d",ret); 
        return RET_WRITE_DEVICE;
    }
    //软件触发
    sendbuf[0] = 0x1b;
    sendbuf[1] = 0x31;
    com_em3096_write(sendbuf, 2);
#else
    //硬件触发
    em3096_trig_begin();
#endif
//
//    len = com_em3096_read(data, time);
//    return len;
//    设置标志位准备，等待扫描返回
    em3096_param_clear();
    g_em3096_param.scan_flag = 1;
    TRACE("set timeout:%d\r\n",  timeout);
    em3096_timeout_set(timeout);
    // start time count
    em3096_pit_start(1000); // 1ms定时
    return RET_OK;
}

extern void ctc_scaner_report(int type, uint8_t *data , int len);
// 检测是否准备扫描， 等待扫描完成，或超时
uint32_t com_em3096_scan(void)
{
    uint32_t len=0;
    uint8_t  data[520]; 
    if ( g_em3096_param.scan_timeout ) {
        memset(data, 0, sizeof(data));
        TRACE("\r\nscan timeout\r\n");
        ctc_scaner_report(SCANER_TYPE_EM3096, data, 0);
        
        em3096_trig_finish();      
        em3096_pit_end();
        em3096_param_clear();
    }
    if ( g_em3096_param.scan_flag ) {
        memset(data, 0, sizeof(data));
        len = com_em3096_read(data);
        if ( len ) {
//            TRACE("\r\nscan result report\r\n");
            TRACE_BUF("\r\nscan result report\r\n", data, len);
            ctc_scaner_report(SCANER_TYPE_EM3096, data, len);
            // close PIT and clear flag
            
            em3096_trig_finish();      
            em3096_pit_end();
            em3096_param_clear();
        }
    }
    return len;
}

uint32_t com_em3096_open_read(uint8_t *data , uint32_t time)
{
    uint8_t ret = 0;
    int len = 0;

    uint32_t starttime;
    
    if (data == NULL) {
        goto RET;
    }
    
    starttime = sys_get_counter();
    while (1) {
        if ( sys_get_counter()-starttime >= time ) {
           break; 
        }
        ret = drv_uart_read(SCAN_COMPORT, &data[len], 1, 200);
        if (ret == 1) {
            len++;  
//            data++;
        }
    }
    
RET:    
    return len;
}

uint8_t com_em3096_open(void)
{
//    uint32_t ret;
//    unsigned char sendbuf[10];
//    unsigned char recvbuf[256];

 #if 1
//    sendbuf[0] = 0x7e;
//    sendbuf[1] = 0x00;
//    sendbuf[2] = 0x00;
//    sendbuf[3] = 0x05;
//    sendbuf[4] = 0x33;
//    sendbuf[5] = 0x48;
//    sendbuf[6] = 0x30;
//    sendbuf[7] = 0x32;    
//    sendbuf[8] = 0x30;   
//    sendbuf[9] = 0xb3;  
    __com_em3096_init();
//    s_DelayMs(800);
//    hal_delay_ms(800);
  //  com_em3096_write(sendbuf, 10);
 #else
    sendbuf[0] = 0x7e;
    sendbuf[1] = 0x00;
    sendbuf[2] = 0x08;
    sendbuf[3] = 0x01;
    sendbuf[4] = 0x00;
    sendbuf[5] = 0x00;
    sendbuf[6] = 0xf5;
    sendbuf[7] = 0xcb;    
    sendbuf[8] = 0x23;   
    hal_delay_init();
    __com_em3096_init();
    hal_delay_ms(800);
    com_em3096_write(sendbuf, 9);

 #endif
//    ret = com_em3096_open_read(recvbuf, 500);
//    if (ret > 0) {
//        TRACE_BUF("open buf ret", recvbuf, ret);
//        if (recvbuf[0] == 0x02) {
////            TRACE_BUF("open read:", recvbuf, ret);
//            ret = RET_OK;
//        } else {
//            ret = RET_NOT_OPEN;
//        }
//    } else {
//        TRACE("\r\nscaner 通信失败"); 
//        return RET_NOT_OPEN;
//    }
//    memset(recvbuf,0,sizeof(recvbuf)); 
//    TRACE("\r\n开启二维扫描头设置"); 
//    com_em3096_write("nls0006010;", 11); //开启设置
//    ret = com_em3096_open_read(recvbuf, 500);
//    if (ret > 0) {
//        TRACE_BUF("open set ret", recvbuf, ret);
//        if (recvbuf[0] == 0x06) {
//            ret = RET_OK;
//        } else {
//            ret = RET_WRITE_DEVICE;
//        }
//    } else {
//        TRACE("\r\nscaner 设置失败"); 
//        return RET_WRITE_DEVICE;
//    }
//
//    TRACE("\r\nscaner ret:%d",ret); 
    return 0;
}

void com_em3096_scan_cancel(void)
{
    memset(&g_em3096_param, 0, sizeof(SCANER_ASY_PARAM));
    em3096_trig_finish();
    em3096_pit_end();
}
#endif
