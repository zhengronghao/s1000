#include "wp30_ctrl.h"

#ifdef CFG_SCANER
#if 0
SCANER_ASY_PARAM g_em1395_param;

unsigned int crc_cal_by_bit(unsigned char* ptr, unsigned int len)
{
    unsigned int crc = 0;
    while(len-- != 0)
    {
        for(unsigned char i = 0x80; i != 0; i /= 2)
        {
            crc *= 2;
            if((crc&0x10000) !=0) //上一位 CRC 乘 2 后，若首位是 1，则除以 0x11021
                crc ^= 0x11021;
            if((*ptr&i) != 0) //如果本位是 1，那么 CRC = 上一位的 CRC + 本位/CRC_CCITT
                crc ^= 0x1021;
        }
        ptr++;
    }
    return crc;
}


void em1395_param_clear(void)
{
    memset(&g_em1395_param, 0, sizeof(SCANER_ASY_PARAM));
}

// 设置超时时间 单位ms
void em1395_timeout_set(uint32_t time)
{
    g_em1395_param.timecount = time;
}

void em1395_timeout_clear(void)
{
    g_em1395_param.timecount = 0;
}

//int em1395_check(void)
//{
//    return g_em1395_param.scan_flag ;
//}


void em1395_timer_isr(void);
void em1395_pit_end(void)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = SCANPIT;
    pit.PIT_Isr = em1395_timer_isr;
    LPLD_PIT_Deinit(&pit);
    
}

void em1395_timer_isr(void)
{
    if ( g_em1395_param.timecount > 0 ) {
        g_em1395_param.timecount--;
    }
    if ( g_em1395_param.timecount == 0 ) {
        g_em1395_param.scan_timeout = 1;       // 超时标志
        em1395_trig_finish();
        em1395_pit_end();
    }
}

void em1395_pit_init(void)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = SCANPIT;
    pit.PIT_Period = 60; 
    pit.PIT_Isr = em1395_timer_isr;
    LPLD_PIT_Init(&pit);
}

void em1395_pit_start(int clk)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = SCANPIT;
    pit.PIT_Isr = em1395_timer_isr;
    pit.PIT_Period = 60*(uint)clk; 
    LPLD_PIT_Restart(&pit);
}

static void __com_em1395_init(void)
{
    // 使能引脚
    gpio_set_output(SCAN_PTxy_EN,GPIO_OUTPUT_SlewRateFast,0); 
    gpio_set_bit(SCAN_PTxy_EN,1);

    gpio_set_output(SCAN_PTxy_TRIG,GPIO_OUTPUT_SlewRateFast,0); 
    gpio_set_bit(SCAN_PTxy_TRIG,0);


    // 初始化串口
    drv_uart_open((UARTn)SCAN_COMPORT, SCAN_BAUD, gucBuff, PUBBUFFER_LEN, UartAppCallBack[SCAN_COMPORT]); 

    em1395_pit_init();
    em1395_param_clear();
}

void em1395_trig_begin(void)
{
    gpio_set_bit(SCAN_PTxy_TRIG,1);
}

void em1395_trig_finish(void)
{
    gpio_set_bit(SCAN_PTxy_TRIG,0);
}

uint8_t com_em1395_close(void)
{
    uint8_t ret = RET_OK;
    
    gpio_set_bit(SCAN_PTxy_EN,0);
    drv_uart_close(SCAN_COMPORT);

    em1395_pit_end();
    return ret;
}

uint8_t com_em1395_write(uint8_t *data , uint32_t len)
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

// 在规定的时间内是否接收到数据请求后，回送数据的完整性
uint8_t em1395_read_check(int timeout)
{
    uint32_t starttime ;
    uchar recvbuf[255];
    int ret, len=0;
    
    memset(recvbuf, 0, sizeof(recvbuf));
    starttime = sys_get_counter();
    
    // found Prefix1 0x0200
    starttime = sys_get_counter();
    while (1) {
        if ( sys_get_counter()-starttime >= timeout ) {
            return RET_TIMEOUT;
        }
        ret = drv_uart_read(SCAN_COMPORT, &recvbuf[len], 1, 200);
        if (ret == 1) {
            TRACE("recv[%d]=%x", len, recvbuf[len]);
            len++;  
            if (len >= 2 && recvbuf[len-1] == 0x00 && recvbuf[len-2]==0x02) {
                // ret = RET_OK;
                TRACE("found the prefix2\r\n");
                break;
            } 
        }
    }
    len = 0;
    while(1)
    {
        if ( sys_get_counter()-starttime >= timeout ) {
            return RET_TIMEOUT;
        }
        ret = drv_uart_read(SCAN_COMPORT, recvbuf, 1, 200);
        if (ret == 1) {
            if ( recvbuf[0] == 0 ) {
                TRACE("racv respond succ\r\n");
                break; 
            }
            return RET_UNKNOWN;
        }
    }
    // get data len
    while(1)
    {
        if ( sys_get_counter()-starttime >= timeout ) {
            return RET_TIMEOUT;
        }
        ret = drv_uart_read(SCAN_COMPORT, &recvbuf[1], 1, 200);
        if (ret == 1) {
            TRACE("data len:%d\r\n", ret);
            break; 
        }
    }
    // get the rest
    ret = drv_uart_read(SCAN_COMPORT, &recvbuf[2], recvbuf[1]+2, sys_get_counter()-starttime);
    if (ret == (recvbuf[1]+2)) {
        return RET_OK; 
    }
    return RET_TIMEOUT; 
}

uint32_t com_em1395_read_result(uint8_t *data )
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

uint32_t com_em1395_read(uint8_t *data , uint32_t time)
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

uint8_t com_em1395_scan_end(void);
uint8_t em1395_cmd_get( short Address, uchar sendlen, uchar* reclen, char* recdata);
uint8_t em1395_cmd_set(short Address, uchar len, char* data);
uint8_t com_em1395_open(void)
{
   // char sendbuf[10];
//    uchar recelen;
  //  int ret;
    __com_em1395_init();
    // 防止打开太快失败，scan_start可能造成打开失败
 //   sys_DelayMs(1000);
//    com_em1395_scan_end();
//    ret = em1395_cmd_get(0x0000, 1, &recelen, sendbuf);
//    if ( ret != RET_OK ) {
//        TRACE("get data err\r\n"); 
//    }

//    sys_DelayMs(2000);
//    memset(sendbuf, 0, sizeof(sendbuf));
//    sendbuf[0] = 0xf5;
//    // 设置“命令触发识读”命令
//    ret = em1395_cmd_set( 0x0000,  0x01,  (char *)sendbuf);
//    if ( ret != RET_OK ) {
//        TRACE("set trigger cmd err\r\n"); 
//        return ret ;
//    }
    TRACE("open succ\r\n"); 
    return RET_OK;
} 

/*---------------------------------------------------
 *  send {Prefix1} {Types} {Lens} {Address} {Datas} {FCS}
 *         0x7e00   0xxx   0xxx   0xxxxx    len     crc
 *  recv {Prefix1} {Types} {Lens} {Address} {Datas} {FCS}
 *         0x0200   0xxx   0xxx   0xxxxx    len     crc
 *  Descript:  cmd write
 * ------------------------------------------------*/
uint8_t em1395_cmd_set(short Address, uchar len, char* data)
{
    char sendbuf[20];
//    char recvbuf[20];
    int sendlen;
    uint crc;
    int ret;
    //int i;
    memset(sendbuf, 0, sizeof(sendbuf));
//    memset(recvbuf, 0, sizeof(recvbuf));
    // prefix1
    sendbuf[0] = 0x7e;
    sendbuf[1] = 0x00;
    // type len 
    sendbuf[2] = 0x08;
    sendbuf[3] = len;
    // Address 
    sendbuf[4] = (Address>>8) & 0xff;
    sendbuf[5] = Address & 0xff;
    // fill data 
    memcpy(&sendbuf[6], data, len);

    crc = crc_cal_by_bit((uchar *)&sendbuf[2], len+4);

    sendlen = 6+len;

    sendbuf[sendlen] = (crc>>8) & 0xff; 
    sendlen++;
    sendbuf[sendlen] = crc & 0xff; 
    sendlen++;
    // send cmd data
    com_em1395_write((uchar *)sendbuf, sendlen);
    // waiting the rece datas 
    ret = em1395_read_check(1000);
    if ( ret != RET_OK ) {
       TRACE("read check err:%d\r\n", ret); 
    }
    return ret;
//    ret = com_em1395_read((uchar *)recvbuf, 1000);
//    if ( ret ) {
//        for ( i=0 ; i<ret-1 ; i++ ) {
//            if ( (recvbuf[i] == 0x02) && (recvbuf[i+1] == 0x00) ) {
//                break;
//            }
//        }
//        // not found the prefix2 0x02 0x00
//        if ( i==ret-1 ) {
//            return RET_NOT_FOUND;
//        }
//        // crc checkout ignore 
//        return RET_OK;
//    }
//    return RET_NOT_FOUND;
}
/*---------------------------------------------------
 *  send {Prefix1} {Types} {Lens} {Address} {Datas} {FCS}
 *         0x0200   0xxx   0xxx   0xxxxx    len     crc
 *  recv {Prefix1} {Types} {Lens} {Address} {Datas} {FCS}
 *         0x0200   0xxx   0xxx   0xxxxx    len     crc
 *  Descript:  cmd write
 * ------------------------------------------------*/

uint8_t em1395_cmd_get( short Address, uchar sendlen, uchar* reclen, char* recdata)
{
    char sendbuf[20];
    char recvbuf[20];
    uint crc;
    int ret;
    int i;
    memset(sendbuf, 0, sizeof(sendbuf));
    memset(recvbuf, 0, sizeof(recvbuf));
    // prefix1
    sendbuf[0] = 0x7e;
    sendbuf[1] = 0x00;
    // type len 
    sendbuf[2] = 0x07;
    sendbuf[3] = 0x01;
    // Address 
    sendbuf[4] = (Address>>8) & 0xff;
    sendbuf[5] = Address & 0xff;
    // fill data 

    sendbuf[6] = sendlen;

    crc = crc_cal_by_bit((uchar *)&sendbuf[2], 5);

    sendbuf[7] = (crc>>8) & 0xff; 
    sendbuf[8] = crc & 0xff; 
    // send cmd data
    com_em1395_write((uchar *)sendbuf, 9);
    // waiting the rece datas 
    ret = com_em1395_read((uchar *)recvbuf, 1000);
    if ( ret ) {
        for ( i=0 ; i<ret-1 ; i++ ) {
            if ( (recvbuf[i] == 0x02) && (recvbuf[i+1] == 0x00) ) {
                break;
            }
        }
        // not found the prefix2 0x02 0x00
        if ( i==ret-1 ) {
            return RET_NOT_FOUND;
        }
        // crc checkout ignore 

        // response is ok
        if ( 0 == recvbuf[i+2] ) {
            *reclen = recvbuf[i+2]; 
            memcpy(recdata, (char *)(&recvbuf[i+3]), recvbuf[i+2]);
        }
        return RET_OK;
    }
    return RET_NOT_FOUND;
}

uint8_t com_em1395_scan_end(void)
{
    uint32_t ret;
    unsigned char sendbuf[10];

    sendbuf[0] = 0x02;
    // 设置“命令触发识读”命令
    ret = em1395_cmd_set( 0x0000,  0x01,  (char *)sendbuf);
    if ( ret != RET_OK ) {
        TRACE("set trigger cmd err\r\n"); 
        return ret ;
    }
    return RET_OK;
}

uint16_t com_em1395_scan_start(uint32_t timeout)
{
   // uint32_t ret;
  //  unsigned char sendbuf[10];

//    sendbuf[0] = 0xf5;
//    // 设置“命令触发识读”命令
//    TRACE("\r\n------------------begin trigger------------\r\n");
//    ret = em1395_cmd_set( 0x0000,  0x01,  (char *)sendbuf);
//    if ( ret != RET_OK ) {
//        TRACE("set trigger cmd err\r\n"); 
//        return ret ;
//    }
//    TRACE("\r\n------------------trigger succ------------\r\n");
    // 开始模拟触发”可以启动设备触发
#if 0
    sendbuf[0] = 0x01;
    ret = em1395_cmd_set( 0x0002,  0x01,  (char *)sendbuf);
    if ( ret != RET_OK ) {
        TRACE("set module cmd err\r\n"); 
        return ret ;
    }
    em1395_param_clear();
#else
    //硬件触发
    em1395_trig_begin();
#endif

    g_em1395_param.scan_flag = 1;
    TRACE("set timeout:%d\r\n",  timeout);
    em1395_timeout_set(timeout);
    // start time count
    em1395_pit_start(1000); // 1ms定时

    TRACE("\r\nscaner succ\r\n"); 
    return RET_OK;
}

extern void ctc_scaner_report(int type, uint8_t *data , int len);
uint32_t com_em1395_scan(void)
{
    uint32_t len=0;
    uint8_t  data[520]; 

    if ( g_em1395_param.scan_timeout ) {
        memset(data, 0, sizeof(data));
        TRACE("\r\nscan timeout\r\n");
        ctc_scaner_report(SCANER_TYPE_EM1395, data, 0);
        em1395_trig_finish();
        em1395_pit_end();
        em1395_param_clear();
//        com_em1395_scan_end();
    }
    if ( g_em1395_param.scan_flag ) {
        memset(data, 0, sizeof(data));

        len = com_em1395_read_result(data);
        if ( len ) {
            //            TRACE("\r\nscan result report\r\n");
            TRACE_BUF("\r\nscan result report\r\n", data, len);
            
            ctc_scaner_report(SCANER_TYPE_EM1395, data, len);
            // close PIT and clear flag
            em1395_trig_finish();
            em1395_pit_end();
            em1395_param_clear();
//            com_em1395_scan_end();
//            if ( ret == RET_OK ) {
//                return RET_OK; 
//            }
        }

    }
    return len;
}

void com_em1395_scan_cancel(void)
{
    memset(&g_em1395_param, 0, sizeof(SCANER_ASY_PARAM));

    em1395_trig_finish();
//    com_em1395_scan_end();
    em1395_pit_end();
}

uint16_t test_em1395(uchar *sendbuf, uint32_t sendlen)
{
//    uint32_t len = 0;
//    unsigned char sendbuf[20];
    unsigned char recv_buf[100];
    char ret;

    memset(recv_buf, 0, sizeof(recv_buf));
    com_em1395_write(sendbuf, sendlen);

//    sys_DelayMs(100);
    ret = com_em1395_read(recv_buf, 500);
    if (ret > 0) {
        TRACE_BUF("\r\nscaner suc:",recv_buf, ret); 
        return 0;
    }
//    sys_DelayMs(500);
//    TRACE("second\r\n");
//    com_em1395_write(sendbuf, sendlen);
////    sys_DelayMs(100);
//    ret = com_em1395_read(recv_buf, 500);
//    if (ret > 0) {
//        TRACE_BUF("\r\nscaner suc:",recv_buf, ret); 
//        return 0;
//    } 
    TRACE("\r\nscaner fail :%d",ret); 

//    com_em1395_close();
    return ret;
}

int test_crc(char *buf, int len)
{
    int ret;
    ret = crc_cal_by_bit((uchar *)buf, len); 
    TRACE("get the crc result:%x\r\n", ret);
    return ret;
}
#endif 
#endif
