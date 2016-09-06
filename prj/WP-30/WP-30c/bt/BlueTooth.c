#include "wp30_ctrl.h"
#include "bluetooth_hw.h"

#ifdef CFG_BLUETOOTH


#define ISSC_PIN_POW_ON              BT_BATIN_ENABLE_PTxy  //PD21
#define ISSC_PIN_RST                 BT_MODULE_RST_N_PTxy   //PD11
#define ISSC_PIN_SELMODE             BT_SYSCONFIG_P20_PTxy  //PD9
#define ISSC_PIN_WAKEUP              BT_CONFIG_CONT_IND_P32_PTxy  //PC30
#define ISSC_PIN_ONLINE_STATE        BT_STATUS_IND_1_PTxy  //PD10

#define BLUETOOTH_COM           BT_COMPORT 
#define EM_bt_POWERON           0   // 打开模块电源
#define EM_bt_POWEROFF          1   // 关闭模块电源

#define ISSC_COMMU_BAUDRATE          BT_BAUD//"115200,8,n,1"
#define BT_DATASTAT_TIMEOUT          5000
#define BT_CMD_TIMEOUT               3000


enum BTCMD_DEFINES
{
    BTCMD_CMD = 0,
    BTCMD_DATA,
    BTCMD_SETNAME,
    BTCMD_GETNAME,
    BTCMD_SETPIN,
    BTCMD_GETPIN,
    BTCMD_SETBAUD,
    BTCMD_GETBTADDR,
    BTCMD_BPAIR,
    BTCMD_CONNECT,
};

int bt_ready(void);
void issc_set_mode(int mode);
void s_bt_setPower(uchar ucFlag);
int s_bt_setRst(void);

#define dbg_trace(fun, line)
#if 0
static uchar *pSerchStr(void *pvMother, void *pvSon, uint uiLen)
{
    uint i;

    for (i = 0; i < uiLen; i++)
    {
        if ( *(char *)(pvMother + i) == *(char *)pvSon)
        {
            if (memcmp(pvMother + i,pvSon,strlen(pvSon)) ==0)
                return pvMother + i;
        }

    }
    return NULL;
}
#endif


//硬件要求蓝牙在未开启的时候所有口线输出低。包括初始化时
int s_bt_gpioctrl(void)
{
    gpio_set_input(BT_BATIN_ENABLE_PTxy,LPWR_MODE_INPUT );
    gpio_set_input(BT_WAKEUP_ENABLE_PTxy,LPWR_MODE_INPUT );
    gpio_set_input(BT_BATIN_ENABLE_PTxy,LPWR_MODE_INPUT );
    gpio_set_input(BT_STATUS_IND_2_PTxy,LPWR_MODE_INPUT );
    gpio_set_input(BT_STATUS_IND_1_PTxy,LPWR_MODE_INPUT );
    gpio_set_input(BT_SYSCONFIG_P20_PTxy,LPWR_MODE_INPUT );
    gpio_set_input(BT_SYSCONFIG_P24_PTxy,LPWR_MODE_INPUT );
    gpio_set_input(BT_SYSCONFIG_EAN_PTxy,LPWR_MODE_INPUT );
    return 0;
}

int s_bt_setRst(void)
{
//    set_pio_output(ISSC_PIN_RST, 1);
    hw_bt_gpio_set(BT_MODULE_RST_N_GPIO,BT_MODULE_RST_N_PINx,1);
    s_DelayMs(30);
//    set_pio_output(ISSC_PIN_RST, 0);
    hw_bt_gpio_set(BT_MODULE_RST_N_GPIO,BT_MODULE_RST_N_PINx,0);
    s_DelayMs(10);
//    set_pio_output(ISSC_PIN_RST, 1);
    hw_bt_gpio_set(BT_MODULE_RST_N_GPIO,BT_MODULE_RST_N_PINx,1);
    return OK;
}

uint8_t bt_status(void)
{
    return ((hw_bt_gpio_get(BT_STATUS_IND_2_GPIO,BT_STATUS_IND_2_PINx)<<1) 
            | hw_bt_gpio_get(BT_STATUS_IND_1_GPIO,BT_STATUS_IND_1_PINx));
}

int check_if_online(void)
{
    return hw_bt_gpio_get(BT_STATUS_IND_1_GPIO,BT_STATUS_IND_1_PINx);
}
void s_bt_setPower(uchar ucFlag)
{
    if (ucFlag == EM_bt_POWERON)
    {
//        set_pio_output(ISSC_PIN_POW_ON,1);
        hw_bt_gpio_set(BT_BATIN_ENABLE_GPIO,BT_BATIN_ENABLE_PINx,1);
//        set_pio_output(ISSC_PIN_WAKEUP,1);
        hw_bt_gpio_set(BT_WAKEUP_ENABLE_GPIO,BT_WAKEUP_ENABLE_PINx,1);
        s_DelayMs(50);
    }
    else
    {
//        set_pio_output(ISSC_PIN_POW_ON,0);
        hw_bt_gpio_set(BT_BATIN_ENABLE_GPIO,BT_BATIN_ENABLE_PINx,0);
//        set_pio_output(ISSC_PIN_WAKEUP,0);
        hw_bt_gpio_set(BT_WAKEUP_ENABLE_GPIO,BT_WAKEUP_ENABLE_PINx,0);
//        set_pio_output(ISSC_PIN_SELMODE,0);
        hw_bt_gpio_set(BT_SYSCONFIG_P20_GPIO,BT_SYSCONFIG_P20_PINx,0);
        hw_bt_gpio_set(BT_SYSCONFIG_P24_GPIO,BT_SYSCONFIG_P24_PINx,1);
    }
}
// 0- application
// 1- hci_uart
//P20/P24: 
//HH:Application 
//LL:Boot mode 
//LH:HCI UART mode for testing and system configuration. 
void issc_set_mode(int mode)
{
//    s_bt_setPower(1);
    if ( mode == 0 )
    {
//        set_pio_output(ISSC_PIN_SELMODE,1);
        hw_bt_gpio_set(BT_SYSCONFIG_P20_GPIO,BT_SYSCONFIG_P20_PINx,1);//Normal operation
        TRACE("\nmode:App\n");
    }
    else
    {
//        set_pio_output(ISSC_PIN_SELMODE,0);
//HCI UART mode for testing and system configuration
        hw_bt_gpio_set(BT_SYSCONFIG_P20_GPIO,BT_SYSCONFIG_P20_PINx,0);//Test (Write EEPROM)
        TRACE("\nmode:hci uart\n");
    }
//    s_bt_setPower(0);
    s_bt_setRst();
    s_DelayMs(1000);
}

static int bt_uart_read(UARTn uartch,uint8_t *buffer,uint32_t length,int32_t timout_ms)
{
    uint32_t begin_time;
    uint32_t rev_length,tmp;
    int iRet;

    rev_length = 0;
    begin_time = sys_get_counter();
    tmp = 0;
    while (1)
    { 
        iRet = drv_uart_check_readbuf(BLUETOOTH_COM);
        if(iRet > 0)
        {
            if ( (tmp+iRet) >= length ) {
                iRet = length-tmp;
            }
            rev_length = drv_uart_read(BLUETOOTH_COM,buffer+tmp,iRet,0);
            tmp += rev_length;

            if ( tmp >= length ) {
                break;
            }
            timout_ms = 100;
            begin_time = sys_get_counter();
        }
        if ( (timout_ms >= 0) && (sys_get_counter() - begin_time > timout_ms)) {
            break;
        }
    }
    
    return tmp;
}


int issc_first_run_init(void)
{
     int result;
     int i;
     uint8_t buf[128];
//     const uint8_t  read_paire_cmd[]={0x01,0x29,0xFC,0x03,0x00,0xDC,0X04};
     const uint8_t write_paire_cmd[]={0x01,0x27,0xFC,0x04,0x00,0xB1,0x01,0x04};
     // ISO 提速参数
     // 0xDC  LE_CONNECTION_INTERVALE_MAX   DEFAULT:0x0190   MODIFY:0x0010
     const uint8_t cmd_LE_connect_intervale_max[] = {0x01,0x27,0xFC,0x05,0x00,0xDC,0x02,0x00,0x10};
     // 0xDE  LE_SLAVE_LATENCY  DEFAULT:0x0002  MODIFY:0x0000
     const uint8_t cmd_LE_slave_latency[] = {0x01,0x27,0xFC,0x05,0x00,0xDE,0x02,0x00,0x00};

     // Write Success Echo
     const uint8_t suc_echo[7] = {0x04,0x0E,0x04,0x01,0x27,0xFC,0x00};

//    char read_buf[] = {0x04,0x0E,0x0B,0x01,0x29,0xFC,0x00,0x00,0xB1,0x01};
//    const char init_config[][8] = {
//        // led config
//                         {0x01,0x27,0xFC,0x04,0x00,0xF1,0x01,0x02},
//                         {0x01,0x27,0xFC,0x04,0x00,0xF7,0x01,0x0A},
//                         {0x01,0x27,0xFC,0x04,0x00,0xFF,0x01,0x0A},
//                         {0x01,0x27,0xFC,0x04,0x01,0x07,0x01,0x00},
//                         {0x01,0x27,0xFC,0x04,0x01,0x0F,0x01,0x00},
       //disable retry to connect when link lose addr:0x00c8
//                         {0x01,0x27,0xFC,0x04,0x00,0xC8,0x01,0x00},
       //disable link back function addr:0x00BD
//                         {0x01,0x27,0xFC,0x04,0x00,0xBD,0x01,0x00},

//     };


     issc_set_mode(1);
     /*
     read addr 0x00b1 value  01-SIMPLE PAIRING  02-ENABLE AUTH 04-SM2
     cmd:  01 29 FC 03 00 B1 01
     */

     // 修改配对认证方式
     CLRBUF(buf);
     drv_uart_clear(BLUETOOTH_COM);
     drv_uart_write(BLUETOOTH_COM,write_paire_cmd,sizeof(write_paire_cmd));
     result =  bt_uart_read(BLUETOOTH_COM,buf,100,3000);
     if (result < 7) 
     {
         TRACE("FAIL:%s %d\r\n",__func__,__LINE__);
         return -1;
     }
     TRACE("\r\n");
     for(i=0;i < result;i++)
     {
         if ( buf[i] == suc_echo[4] && buf[i+1] == suc_echo[5] && buf[i+2] == suc_echo[6] ) {
             break;
         }
         TRACE("%x ",buf[i]);
     }
     if ( i == result ) 
     {
         TRACE("FAIL:%s %d\r\n",__func__,__LINE__);
         return -1;
     }

     // 修改 IOS连接提速参数 0xDC 0xDE
     drv_uart_clear(BLUETOOTH_COM);
     drv_uart_write(BLUETOOTH_COM,cmd_LE_connect_intervale_max,sizeof(cmd_LE_connect_intervale_max));
     result =  bt_uart_read(BLUETOOTH_COM,buf,100,3000);
     if (result < 7) 
     {
         TRACE("FAIL:%s %d\r\n",__func__,__LINE__);
         return -1;
     }
     TRACE("\r\n");
     for(i=0;i < result;i++)
     {
         if ( buf[i] == suc_echo[4] && buf[i+1] == suc_echo[5] && buf[i+2] == suc_echo[6] ) {
             break;
         }
         TRACE("%x ",buf[i]);
     }
     if ( i == result ) 
     {
         TRACE("FAIL:%s %d\r\n",__func__,__LINE__);
         return -1;
     }

     // 修改 IOS连接提速参数 0xDC 0xDE
     drv_uart_clear(BLUETOOTH_COM);
     drv_uart_write(BLUETOOTH_COM,cmd_LE_slave_latency,sizeof(cmd_LE_slave_latency));
     result =  bt_uart_read(BLUETOOTH_COM,buf,100,3000);
     if (result < 7) 
     {
         TRACE("FAIL:%s %d\r\n",__func__,__LINE__);
         return -1;
     }
     TRACE("\r\n");
     for(i=0;i < result;i++)
     {
         if ( buf[i] == suc_echo[4] && buf[i+1] == suc_echo[5] && buf[i+2] == suc_echo[6] ) {
             break;
         }
         TRACE("%x ",buf[i]);
     }
     if ( i == result ) 
     {
         TRACE("FAIL:%s %d\r\n",__func__,__LINE__);
         return -1;
     }

     drv_uart_clear(BLUETOOTH_COM);
     return 0;
}

int bt_open(void)
{
    int result;

    if (sys_get_module_type(MODULE_BLUETOOTH_TYPE) == MODULE_NOTEXIST)
    {
        result = -BT_NOTEXIST;
        goto EXIT;
    }
    result = drv_uart_open(BLUETOOTH_COM,ISSC_COMMU_BAUDRATE,
                           gwp30SysBuf_c.bt,BT_BUFSIZE_C,UartAppCallBack[BT_COMPORT]);
    switch (result)
    {
    case -EUART_VAL:
        result = -BT_VAL;
        goto EXIT;
    case -EUART_NOTOPEN:
        result = -BT_NOTOPEN;
        goto EXIT;
    case -EUART_BUSY:
        result = -BT_BUSY;
        goto EXIT;
    default:
        hw_bt_gpio_init();
        //todo:add GPIO init level
        hw_bt_gpio_set(BT_SYSCONFIG_EAN_GPIO,BT_SYSCONFIG_EAN_PINx,0);
        hw_bt_gpio_set(BT_SYSCONFIG_P24_GPIO,BT_SYSCONFIG_P24_PINx,1);
        s_bt_setPower(EM_bt_POWERON);
        break;
    }
    if(gwp30SysMemory.SysCtrl.bit.factoryisok != 1) 
    {
        //生产测试没完成,需要判断模块初次运行
        issc_first_run_init();
    }
    issc_set_mode(0);
    drv_uart_clear(BLUETOOTH_COM);
    TRACE("bt_open\n");
    gSystem.lpwr.bm.bt = 1;
EXIT:
    return result;
}

int bt_close(void)
{
    int result;

    result = drv_uart_close(BLUETOOTH_COM);
    switch (result)
    {
    case -EUART_VAL:
        result = -BT_VAL;
        break;
    case -EUART_NOTOPEN:
        result = -BT_NOTOPEN;
        break;
    case -EUART_BUSY:
        result = -BT_BUSY;
        break;
    default:
//        set_pio_output(PIN_WL_RTS0, 0);
//        set_pio_output(PIN_WL_CTS0, 0);
//        set_pio_output(ISSC_PIN_RST, 0);
        hw_bt_gpio_set(BT_MODULE_RST_N_GPIO,BT_MODULE_RST_N_PINx,0);
//        set_pio_output(PIN_WL_TXD0,0);
        s_DelayMs(10);
        s_bt_setPower(EM_bt_POWEROFF);
        gSystem.lpwr.bm.bt = 0;
        break;
    }
    s_bt_gpioctrl();
    return result;
}

int bt_read(void *outbuf, int bytelen, int timeOut_ms)
{
    int result;

    if (bt_ready() != OK)
    {
        return -BT_NOTOPEN;
    }

    if ( outbuf == NULL ) {
        return -BT_VAL; 
    }

    result =  bt_uart_read(BLUETOOTH_COM, outbuf, bytelen, timeOut_ms);
    switch (result)
    {
    case -EUART_VAL:
        result = -BT_VAL;
        break;
    case -EUART_NOTOPEN:
        result = -BT_NOTOPEN;
        break;
    case -EUART_BUSY:
        result = -BT_BUSY;
        break;
    default:
        break;
    }
    return result;
}

int bt_write(const void *inbuf, int bytelen)
{
    int result;

    if ( check_if_online() )
    {
        return -BT_NOTCONNECT;
    }

    result = drv_uart_write(BLUETOOTH_COM, inbuf, bytelen);

    switch (result)
    {
    case -EUART_VAL:
        result = -BT_VAL;
        break;
    case -EUART_NOTOPEN:
        result = -BT_NOTOPEN;
        break;
    case -EUART_BUSY:
        result = -BT_BUSY;
        break;
    default:
        break;
    }
    return result;
}

//int bt_printf(const char *format, ...)
//{
//    int       count=0;
//    va_list     marker;
//    unsigned char buff[512+1];	/* 1K 就够了，8K太浪费 */
//
//    if ( check_if_online() )
//    {
//        return -BT_NOTCONNECT;
//    }
//
//    if ( format == NULL ) {
//        return -BT_VAL;
//    }
//
//    memset(buff, 0, sizeof(buff));
//    va_start( marker, format);
//    count = vsnprintf((char *)buff, sizeof(buff)-1, format, marker);
//    va_end( marker );
//    if(count < 0 || count > sizeof(buff)-1)
//    {
//        count = sizeof(buff)-1;
//    }
//
//    return bt_write(buff, count);
//}

int bt_check_writebuf(void)
{
    return 0;
}

int bt_check_readbuf(void)
{
    int result;
    result = drv_uart_check_readbuf(BLUETOOTH_COM);

    switch (result)
    {
    case -EUART_VAL:
        result = -BT_VAL;
        break;
    case -EUART_NOTOPEN:
        result = -BT_NOTOPEN;
        break;
    case -EUART_BUSY:
        result = -BT_BUSY;
        break;
    default:
        break;
    }
    return result;
}

int bt_clear(void)
{
    int result;
    result = drv_uart_clear(BLUETOOTH_COM);
    switch (result)
    {
    case -EUART_VAL:
        result = -BT_VAL;
        break;
    case -EUART_NOTOPEN:
        result = -BT_NOTOPEN;
        break;
    case -EUART_BUSY:
        result = -BT_BUSY;
        break;
    default:
        break;
    }
    return result;
}

int issc_reade2prom(int cmd,uchar *read_buf)
{

    int result;
    uint8_t read_cmd[100];
    uint8_t respond_buf[100];
    int data_len = 0;
    int i = 0;
    int j = 0;
    int flag = 0;
    int HighAddr = 0;
    int LowAddr = 0;

    issc_set_mode(1);

    read_cmd[0] = 0x01;
    read_cmd[1] = 0x29;
    read_cmd[2] = 0xfc;
    read_cmd[3] = 0x03;
    switch(cmd)
    {
    case BTCMD_GETNAME:
        HighAddr = 0x00;
        LowAddr = 0x0B;
        data_len = 0x10;
        break;
    case BTCMD_GETPIN:
        HighAddr = 0x00;
        LowAddr = 0x4B;
        data_len = 0x04;
        break;
    case BTCMD_GETBTADDR:
        HighAddr = 0x00;
        LowAddr = 0x00;
        data_len = 0x06;
        break;
    default:
        break;
    }

    read_cmd[4] = HighAddr;
    read_cmd[5] = LowAddr;
    read_cmd[6] = data_len;

    s_DelayMs(10);

    drv_uart_clear(BLUETOOTH_COM);
    result = drv_uart_write(BLUETOOTH_COM,read_cmd,7);
    if( result != 7 )
    {
        TRACE("uart_write ret=%d\r\n",result);
        return -1;
    }
    vDispBufTitle("BT Write",result,0,read_cmd);

    memset(respond_buf,0x00,sizeof(respond_buf));
    result =  bt_uart_read(BLUETOOTH_COM,respond_buf,100,30000);
    if( result <= 7 )
    {
        TRACE("\nuart_read ret=%d\r\n",result);
        return -1;
    }
    vDispBufTitle("BT Read",result,0,respond_buf);

    for( i=0 ; i<=result ;i++ ){
        if( respond_buf[i] == 0x29 && respond_buf[i+1] == 0xFC && respond_buf[i+2] == 0x00 ){

            flag = 1;
            break;
        }
    }

    if( flag == 0 ){
        return -1;
    }
    data_len = respond_buf[i+5];
    TRACE("\r\ndata_len:%d\r\n",data_len);
    for(j=0;j<data_len;j++)
    {
        if(cmd == BTCMD_GETBTADDR)
        {
            // BT 地址反向
            read_buf[j] = respond_buf[i+6+data_len-1];
            i--;
        }
        else
        {
            read_buf[j] = respond_buf[i+6];
            i++;
        }
    }
//    strncpy((char *)read_buf,(char *)(respond_buf+i+6),data_len);
//    issc_set_mode(0);
    vDispBufTitle("BT Read",data_len,0,read_buf);

    return 0;
}
int issc_reade2prom_offset(uint HighAddr,uint LowAddr,uint len,uchar *read_buf)
{
    int result;
    uint8_t read_cmd[100];
    uint8_t respond_buf[2048];
    int i = 0;
    int flag = 0;


    issc_set_mode(1);

    read_cmd[0] = 0x01;
    read_cmd[1] = 0x29;
    read_cmd[2] = 0xfc;
    read_cmd[3] = 0x03;
    read_cmd[4] = HighAddr;
    read_cmd[5] = LowAddr;
    read_cmd[6] = len;

    s_DelayMs(10);

    drv_uart_clear(BLUETOOTH_COM);

    result = drv_uart_write(BLUETOOTH_COM,read_cmd,7);
    if( result != 7 )
    {
        TRACE("uart_write ret=%d\r\n",result);
        return -1;
    }
    memset(respond_buf,0x00,sizeof(respond_buf));

    result =  bt_uart_read(BLUETOOTH_COM,respond_buf,2048,3000);
    if( result <= 7 )
    {
        TRACE("uart_read ret=%d\r\n",result);
        return -1;
    }
    vDispBufTitle("BT Read",result,0,respond_buf);

    for( i=0 ; i<=result ;i++ ){
        if( respond_buf[i] == 0x29 && respond_buf[i+1] == 0xFC && respond_buf[i+2] == 0x00 ){

            flag = 1;
            break;
        }
    }

    if( flag == 0 ){
        return -1;
    }
    len = respond_buf[i+5];

    strncpy((char *)read_buf,(char *)(respond_buf+i+6),len);

//    TRACE("read_buf:%s\r\n",read_buf);
    vDispBufTitle("BT Read",len,0,read_buf);
    return 0;
}
int issc_write2prom_offset(uint HighAddr,uint LowAddr,uint len,uchar *write_buf)
{
    int result;
    uint8_t write_cmd[1024];
    uint8_t respond_buf[100];
    int data_total;
    int i = 0;
    int flag = 0;

    memset(write_cmd,0x00,sizeof(write_cmd));
    data_total = len+3;
    issc_set_mode(1);

    write_cmd[0] = 0x01;
    write_cmd[1] = 0x27;
    write_cmd[2] = 0xfc;

    for(i=7;i<=7+len;i++)
    {
        write_cmd[i] = *write_buf;
        write_buf++;
    }
    write_cmd[3] = data_total;
    write_cmd[4] = HighAddr;
    write_cmd[5] = LowAddr;
    write_cmd[6] = len;
    vDispBufTitle("Write",write_cmd[3]+4,0,write_cmd);
    s_DelayMs(10);
    drv_uart_clear(BLUETOOTH_COM);
    result = drv_uart_write(BLUETOOTH_COM,write_cmd,write_cmd[3]+4);
    if( result <= 0  )
    {
        return -1;
    }
    memset(respond_buf,0x00,sizeof(respond_buf));

    result =  bt_uart_read(BLUETOOTH_COM,respond_buf,100,3000);
    if( result <= 0 )
    {
        return -1;
    }
    vDispBufTitle("Read",result,0,respond_buf);
    for( i=0 ; i<=result ;i++ ){
        if( respond_buf[i] == 0x27 && respond_buf[i+1] == 0xFC && respond_buf[i+2] == 0x00 ){

            flag = 1;
            break;
        }
    }

    if( flag == 0 ){
        return -1;
    }
    issc_set_mode(0);
    return 0;
}
int issc_write2prom(int cmd, uchar *write_buf,int len)
{
    int result;
    uint8_t write_cmd[100];
    uint8_t respond_buf[100];
    int data_len;
    int data_total;
    int i = 0;
    int flag = 0;
    uint HighAddr = 0;
    uint LowAddr = 0;

    memset(write_cmd,0x00,sizeof(write_cmd));
    data_len = len;
    data_total = data_len+3;
    issc_set_mode(1);

    TRACE("data_len:%d\r\n",data_len);
    write_cmd[0] = 0x01;
    write_cmd[1] = 0x27;
    write_cmd[2] = 0xfc;

    for(i=7;i<=7+data_len;i++)
    {
        write_cmd[i] = *write_buf;
        write_buf++;
    }
    switch(cmd)
    {
    case BTCMD_SETNAME:
        HighAddr = 0x00;//由于模块地址存储空间为固定的0x10字节，当设置数据长度小于0x10的末尾补0X00
        LowAddr = 0x0B;
        if ( data_len < 16 )
        {
            data_total += 1;
            data_len += 1;
            write_cmd[i] = 0x00;
        }
        break;
    case BTCMD_SETPIN:
        HighAddr = 0x00;
        LowAddr = 0x4b;
        break;
    case BTCMD_SETBAUD:
        HighAddr = 0x00;
        LowAddr = 0x21;
        break;
    default:
        break;
    }
    write_cmd[3] = data_total;
    write_cmd[4] = HighAddr;
    write_cmd[5] = LowAddr;
    write_cmd[6] = data_len;
    vDispBufTitle("Write",write_cmd[3]+4,0,write_cmd);
    s_DelayMs(10);

    drv_uart_clear(BLUETOOTH_COM);
    result = drv_uart_write(BLUETOOTH_COM,write_cmd,write_cmd[3]+4);
    if( result <= 0  ){
        return -1;
    }
    memset(respond_buf,0x00,sizeof(respond_buf));

    result =  bt_uart_read(BLUETOOTH_COM,respond_buf,100,3000);
    if( result <= 0 ){
        return -1;
    }
    vDispBufTitle("Read",result,0,respond_buf);
    for( i=0 ; i<=result ;i++ ){
        if( respond_buf[i] == 0x27 && respond_buf[i+1] == 0xFC && respond_buf[i+2] == 0x00 ){

            flag = 1;
            break;
        }
    }

    if( flag == 0 ){
        return -1;
    }
    return 0;
}
int bt_setname(uchar *pucBuff, int len)
{

    int result;
    int datalen;
    uchar buf[16];

    if (bt_ready() != OK)
    {
        return -BT_NOTOPEN;
    }

    if ( len > 16 || len == 0 || pucBuff == NULL )
    {
        return -BT_VAL;
    }
    
    result  = strlen((char *)pucBuff);
    if (result < len) {
        datalen = result;
    }
    else
    {
        datalen = len;
    }
    CLRBUF(buf);
    memcpy((uchar *)buf,(uchar *)pucBuff,datalen);

    result = issc_write2prom(BTCMD_SETNAME,buf,0x10);
    issc_set_mode(0);

    return result;
}

int bt_getname(char *name)
{
    int result;

    if (bt_ready() != OK)
    {
        return -BT_NOTOPEN;
    }

    result = issc_reade2prom(BTCMD_GETNAME,(uchar *)name);
    issc_set_mode(0);
    TRACE("name:%s\r\n",(uchar *)name);
    return result ;
}

int bt_setpin(uchar *pucBuff, int len)
{
    int result;
    if (bt_ready() != OK)
    {
        return -BT_NOTOPEN;
    }

    if ( len != 4 || pucBuff == NULL )
    {
        return -BT_VAL;
    }
    result = issc_write2prom(BTCMD_SETPIN,pucBuff,len);
    issc_set_mode(0);

    return result;
}

int bt_getpin(char *pin)
{

    int result;

    if (bt_ready() != OK)
    {
        return -BT_NOTOPEN;
    }

    result = issc_reade2prom(BTCMD_GETPIN,(uchar *)pin);
    issc_set_mode(0);

    TRACE("pin:%s\r\n",(uchar *)pin);
    return result ;
}

int bt_getaddr(unsigned char *addr)
{

    int result;

    if (bt_ready() != OK)
    {
        return -BT_NOTOPEN;
    }

    result = issc_reade2prom(BTCMD_GETBTADDR,addr);
    issc_set_mode(0);
    return result ;
}

int bt_ioctl(int cmd, union bt_ioctl_arg *arg)
{
    int result;
    int i;

    if (bt_ready() != OK)
    {
        return -BT_NOTOPEN;
    }

    if (arg == NULL)
    {
        result = -BT_VAL;
        goto FAIL;
    }
    switch (cmd)
    {
    case BT_IOCTL_SET_NAME:
        for (i = 0; i < sizeof(arg->name); i++)
        {
            if (!arg->name[i])
            {
                break;
            }
        }
        if (i >= sizeof(arg->name))
        {
            result = -BT_VAL;
        }
        else
        {
            result = bt_setname((uchar *)arg->name, strlen(arg->name));
        }
        break;
    case BT_IOCTL_GET_NAME:
        result = bt_getname(arg->name);
        break;
    case BT_IOCTL_GET_PIN:
        result = bt_getpin(arg->pin);
        TRACE("\n-|re:%d",result);
        break;
    case BT_IOCTL_SET_PIN:
        for (i = 0; i < sizeof(arg->pin); i++)
        {
            if (!arg->pin[i])
            {
                break;
            }
        }
        if (i >= sizeof(arg->pin))
        {
            result = -BT_VAL;
        }
        else
        {
            result = bt_setpin((uchar *)arg->pin, strlen(arg->pin));
        }
        break;
    case BT_IOCTL_GET_BTADDR:
        result = bt_getaddr(arg->addr);
        break;
    case BT_IOCTL_CHECK_CONNECTED:
        result =  check_if_online();
        break;
    default:
        result = -BT_VAL;
        goto FAIL;
    }
FAIL:
    return result;
}

int bt_ready(void)
{
    if (gSystem.lpwr.bm.bt)
    {
        return OK;
    }
    else
    {
        return -BT_NOTREADY;
    }
}
// 通过ISSC上位机USB DONGLE工具去测试
#define  BT_TEST_STRING     "start_issc_fac_test"
//生产测试
int fac_test_bt(int mode)
{
    unsigned char data[256];
    int ret;
    int result = ERROR;
    uint uiStartTime,uiCurTime;
    int data_len = 0;
    int time;

    CLRBUF(data);
    uiStartTime = sys_get_counter();
    while(1)
    {
        uiCurTime = sys_get_counter();
        time = uiCurTime-uiStartTime;
        data_len = bt_check_readbuf();
        if (data_len >= strlen(BT_TEST_STRING)) 
        {
            if (data_len > 256) {
                data_len = 256;
            }
            ret = bt_read(data,data_len,1000);
//            bt_write(data,ret);
            drv_uart_write(BLUETOOTH_COM, data,ret);
            ret = memcmp(data,BT_TEST_STRING,strlen(BT_TEST_STRING));
            if (ret == 0) 
            {
                lcd_display(0, DISP_FONT_LINE1, DISP_FONT| DISP_CLRLINE,"测试成功");
                result = OK;
                break;
            }
        }
        else
        {
            lcd_display(0, DISP_FONT_LINE1, DISP_FONT| DISP_CLRLINE,
                        "蓝牙正在连接...(%d)",time/1000);
        }
        if ( kb_hit()) 
        {
            if ( kb_getkey(100) == KEY_CANCEL )
            {
                result  = ERROR;
                lcd_display(0, DISP_FONT_LINE1, DISP_FONT| DISP_CLRLINE,
                            "中止测试");
                break;
            }
        }
        sys_DelayMs(100);
    }
    kb_flush();
    kb_getkey(1000);
    return result;
}

int fac_boardtest_bt(int mode)
{
    int result;
    uchar pin[4];
    if (sys_get_module_type(MODULE_BLUETOOTH_TYPE) == MODULE_NOTEXIST)
    {
        result = -BT_NOTEXIST;
        goto EXIT;
    }
    drv_uart_close(BLUETOOTH_COM);
    result = drv_uart_open(BLUETOOTH_COM,ISSC_COMMU_BAUDRATE,
                           gwp30SysBuf_c.bt,BT_BUFSIZE_C,UartAppCallBack[BT_COMPORT]);
    switch (result)
    {
    case -EUART_VAL:
        result = -BT_VAL;
        goto EXIT;
    case -EUART_NOTOPEN:
        result = -BT_NOTOPEN;
        goto EXIT;
    case -EUART_BUSY:
        result = -BT_BUSY;
        goto EXIT;
    default:
        hw_bt_gpio_init();
        hw_bt_gpio_set(BT_SYSCONFIG_EAN_GPIO,BT_SYSCONFIG_EAN_PINx,0);
        hw_bt_gpio_set(BT_SYSCONFIG_P24_GPIO,BT_SYSCONFIG_P24_PINx,1);
        s_bt_setPower(EM_bt_POWERON);
        break;
    }

    result = issc_reade2prom(BTCMD_GETPIN,pin);
    if ( result !=  0 ) {
        return -BT_ERROR;
    }
    drv_uart_clear(BLUETOOTH_COM);
    drv_uart_close(BLUETOOTH_COM);
    hw_bt_gpio_set(BT_MODULE_RST_N_GPIO,BT_MODULE_RST_N_PINx,0);
    s_DelayMs(10);
    s_bt_setPower(EM_bt_POWEROFF);
    TRACE("bt_open\n");
EXIT:
    return result;
}

#endif

