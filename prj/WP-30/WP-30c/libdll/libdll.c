/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : libdll.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 11/14/2014 10:26:50 AM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"

//*********************************************************************
//                         系统模块定义
//*********************************************************************
void sys_ReadSn(void *sn)
{
    s_localdl_get_sn(sn);
//    uchar *psn = (uchar *)sn;
//
//    if(sn == NULL)
//    {
//        return;
//    }
//    s_read_syszone(SYS_SN_DEV_ADDR, 9, psn);
//    if(memcmp(psn, (uchar *)"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8) == 0)
//    {
//        memset(psn, 0, 8);
//    }
//    psn[8] = 0;
//    return;
}

int sys_WriteSn(void *sn)
{
    return s_localdl_save_sn(sn,strlen((char *)sn));
//    int i;
//    uchar *psn = (uchar *)sn;
//    int ret;
//    uchar len;
//    uchar *backbuf;
//    
//    if (sn == NULL)
//    {
//        return ERROR;
//    }
//
//    len = strlen(sn);
//    if (len > SYS_SN_DEV_SIZE)
//    {
//        return ERROR;
//    }
//    for (i = 0; i < len; i++)
//    {
//        if (!((*psn >= '0' && *psn <= '9') || (*psn >= 'a' && *psn <= 'z') || (*psn >= 'A' && *psn <= 'Z')))
//        {
//            return ERROR;
//        }
//        psn++;
//    }
//
//    backbuf = malloc(FLASH_SECTOR_SIZE);
//    if ( backbuf ) {
//        ret = s_write_syszone(backbuf, SYS_SN_DEV_ADDR, len, sn);
//        free(backbuf);
//    }else{
//        ret = ERROR;
//    }
//    if(ret)
//        return ERROR;
//    else
//        return OK;
}

void sys_ReadSn_new(void *sn)
{
    int i;
    int flag = 1;

    uchar   *psn = (uchar *)sn;
    if(sn == NULL)
    {
        return;
    }
    s_read_syszone(SYS_SN_DEV_ADDR, SYS_SN_DEV_SIZE, psn);
    for (i = 0; i < SYS_SN_DEV_SIZE; i++)
    {
        if (psn[i] != 0xFF)
        {
            flag = 0;
            break;
        }
    }

    if (flag)
    {
        memset(psn, 0, SYS_SN_DEV_SIZE);
    }
    psn[SYS_SN_DEV_SIZE] = 0;
    return ;
}

int sys_WriteSn_new(void *sn)
{
    int i;
    uchar *psn = (uchar *)sn;
    int ret;
    uchar len;
    uchar *backbuf;
    
    if (sn == NULL)
    {
        return ERROR;
    }

    len = SYS_SN_DEV_SIZE;
    for (i = 0; i < len; i++)
    {
        if ( *psn == '\0' )
        {
            break;
        }
        if (!((*psn >= '0' && *psn <= '9') || (*psn >= 'a' && *psn <= 'z') || 
              (*psn >= 'A' && *psn <= 'Z') || (*psn == '-')))
        {
            return ERROR;
        }
        psn++;
    }

    backbuf = malloc(FLASH_SECTOR_SIZE);
    if ( backbuf ) {
        ret = s_write_syszone(backbuf, SYS_SN_DEV_ADDR, len, sn);
        free(backbuf);
    }else{
        ret = ERROR;
    }
    if(ret)
        return ERROR;
    else
        return OK;
}

int sys_FirstRunExp(void)
{
    uint flag = 0;
    s_read_syszone(OFFSET_APPUPDATE_FLAG, 4, (uchar *)&flag);
    return (flag == SYSZONE_APPUPDATE_FLAG)?YES:NO;
}

int sys_FirstRun(void)
{
    uint flag = 0;
    uchar *backbuf;
    s_read_syszone(OFFSET_APPUPDATE_FLAG, 4, (uchar *)&flag);
    if ( flag == SYSZONE_APPUPDATE_FLAG) {
        backbuf = malloc(FLASH_SECTOR_SIZE);
        if ( backbuf ) {
            flag = 0xFFFFFFFF;
            s_write_syszone(backbuf,OFFSET_APPUPDATE_FLAG, 4, (uchar *)&flag);
            free(backbuf);
        }
        flag = YES;
    }else
    {
        flag = NO;
    }
    return flag;
}


void sys_Beep(void)
{
    sys_beep();
    return;
}

void sys_BeepPro(uint freq, uint duration_ms, int choke)
{
    sys_beep_pro(freq, duration_ms, choke);
    return;
}

int sys_SetTime(const void  *intime)
{
    unsigned long second=0;
    struct rtc_time tm;
    uint8_t *time = (uint8_t *)intime;

    tm.tm_year = (ushort)BCD2BIN(time[0])+2000-1900;
    tm.tm_mon  = (ushort)BCD2BIN(time[1])-1;
    tm.tm_mday = (ushort)BCD2BIN(time[2]);
    tm.tm_hour = (ushort)BCD2BIN(time[3]);
    tm.tm_min = (ushort)BCD2BIN(time[4]);
    tm.tm_sec = (ushort)BCD2BIN(time[5]);
    if (rtc_valid_tm(&tm))
    {
        // Something went wrong encoding the date/time
        return ERROR;
    }
    else
    {
        rtc_tm_to_time(&tm,&second);
        hw_rtc_init(second,0);
        return OK;
    }

}

void sys_GetTime(void *outtime)
{
    unsigned long second=0;
    struct rtc_time tm;
    uint8_t *time = (uint8_t *)outtime;

    if (hw_rtc_read_second((uint32_t *)&second) == 0) 
    {
        rtc_time_to_tm(second,&tm);
        time[0] = BIN2BCD(tm.tm_year+1900-2000);
        time[1] = BIN2BCD(tm.tm_mon+1);
        time[2] = BIN2BCD(tm.tm_mday);
        time[3] = BIN2BCD(tm.tm_hour);
        time[4] = BIN2BCD(tm.tm_min);
        time[5] = BIN2BCD(tm.tm_sec);
        time[6] = BIN2BCD(tm.tm_wday);
    } else
    {
        TRACE("\nRead second reg error!");
        time[0] = 0x00;
        time[1] = 0x00;
        time[2] = 0x00;
        time[3] = 0x00;
        time[4] = 0x00;
        time[5] = 0x00;
        time[6] = 0x00;
    }
}

uint sys_GetCounter(void)
{
    return sys_get_counter();
}

void sys_DelayMs(uint duration_ms)
{
    s_DelayMs(duration_ms);
}

void sys_Reset(void)
{
    NVIC_SystemReset();
}

int sys_GetBattery(int *pMV)
{
    int ad_val;
    power_charge_adc_open();
    ad_val = power_charge_adc_caculate();
    power_charge_adc_close();
//    ad_val = ((74*ad_val)/10/4220);
    *pMV = (74*ad_val)/1000;//mv
    return 0;
//    sys_get_batter_percentum();
}


int sys_GetBatteryPercentum(void)
{
    return sys_get_batter_percentum();
}

int sys_GetSupplyState(void)
{
    if (power_ifcharging())
    {
        if (power_charge_ifFull())
        {
            return SUPPLY_VIN_BYFULL_STATE;
        } else
        {
            return SUPPLY_VIN_BYON_STATE;
        }
    } else 
    {
        return SUPPLY_BATTERY_NOR_STATE;
    }
}

void sys_Shutdown(void)
{
    if (power_ifcharging() == 1) //charging
    {
        NVIC_SystemReset();
    } else //using battery
    {
        power_keep_down();
    }
}

//*********************************************************************
//                         电源模块定义
//*********************************************************************
#define POWEROFF_OPEN          0
#define POWEROFF_CLOSE         1
int sys_auto_poweroff(int mode,uint32_t timeout_ms)
{
    int iRet = 0;
    switch(mode)
    {
    case POWEROFF_OPEN:
        auto_poweroff_open(timeout_ms);
        break;
    case POWEROFF_CLOSE:
        auto_poweroff_close();
        break;
    default:
        iRet = -1;
        break;
    }
    return iRet;
}

//*********************************************************************
//                         LED模块定义
//*********************************************************************
int sys_SetLed(int led, int value)
{
    //int ret=0;
    uint i;
    switch (led)
    {
    case LED_BLUE:
        i = S_LED_BLUE;
        break;
    case LED_YELLOW:
        i = S_LED_YELLOW;
        break;
    case LED_GREEN:
        i = S_LED_GREEN;
        break;
    case LED_RED:
        i = S_LED_RED;
        break;
    default:
        return ERROR;
    }
    if ( value ) {
        hw_led_on(i);
    } else {
        hw_led_off(i);
    }
    return 0;
}

//*********************************************************************
//                         液晶模块定义
//*********************************************************************
int lcd_SetLight(int light_mode)
{
#ifdef CFG_LCD
    if(light_mode > LIGHT_OFF_MODE || light_mode < LIGHT_OFF_TEMP)
    {
        return ERROR;
    }
    return (drv_lcd_setbacklight(light_mode) != 0)?ERROR:OK;
#else
    return OK;
#endif
}

int lcd_SetLighttime(int duration_ms)
{
	int tmp = 0;

#ifdef CFG_LCD

	tmp = gLcdSys.LcdBackLightTimerOut10ms*10;
	gLcdSys.LcdBackLightTimerOut10ms = duration_ms/10;
#endif
	return tmp;
}

void lcd_Cls(void)
{
#ifdef CFG_LCD
    lcd_cls();
#endif
}

void lcd_ClrLine(int begline, int endline)
{
#ifdef CFG_LCD
    int i;

//    TRACE("\r\n beg:%d  end:%d",begline,endline);
    if((begline < 0)
        || (begline >= LCD_WIDE)
        || (endline < 0)
        || (endline >= LCD_WIDE)
        || (begline > endline))
    {
        return;
    }
//    for ( i = 0 ; i < LCD_LENGTH; i++ ) {
//        drv_lcd_drawline(i, begline,i, endline,0);
//    }
    //速度快一点
    for ( i = begline ; i <= endline; i++ ) {
        drv_lcd_drawline(0, i, 127, i,0);
    }
    gLcdSys.glcd_x = 0;
    gLcdSys.glcd_y = begline;
#endif
}

void lcd_Goto(int x, int y)
{
#ifdef CFG_LCD
    lcd_goto(x, y);
#endif
}

int lcd_SetInv(int inv_mode)
{
#ifdef CFG_LCD
    uint8_t mode;

    mode = gLcdSys.SysOperate.bit.reverse;
    if (inv_mode)
    {
        gLcdSys.SysOperate.bit.reverse = ON;
    } else
    {
        gLcdSys.SysOperate.bit.reverse = OFF;
    }
    return mode;
#else
    return 0;
#endif
}

void lcd_DrawLine(uint8_t sx,uint8_t sy,uint8_t ex,uint8_t ey,uint8_t val)
{
    drv_lcd_drawline(sx,sy,ex,ey,val);
}

void lcd_Plot(int x, int y, int color)
{
#ifdef CFG_LCD
    drv_lcd_drawdot((uint8_t)x, (uint8_t)y,(uint8_t)color);
    drv_lcd_update();
#endif
}

void lcd_GetPos(int *x, int *y)
{
#ifdef CFG_LCD
    if (x == NULL || y ==NULL)
    {
        return;
    }
    *x = gLcdSys.glcd_x;
    *y = gLcdSys.glcd_y;
#endif
}

int lcd_SetIcon(int icon_no, int icon_mode)
{
    return 0;
}


void lcd_BmpDisp(int x, int y, const void *bmpdata)
{
#ifdef CFG_LCD
    lcd_bmp_disp(x,y,bmpdata);
#endif
}

void lcd_BmpmapDisp(int x, int y, int width, int hight, void *scrbmp)
{
#ifdef CFG_LCD
    if (scrbmp == NULL)
    {
        return;
    }
    drv_lcd_clear_matrix(x,y,width,hight,0);
    drv_lcd_FillVertMatrixBigEndBits(x,y,width,hight,scrbmp);
    drv_lcd_update();
#endif
}

/*
 *********************************************************************
 *       function:  lcd_EmvDisp.c
 *    Description:  
 *        Version:  1.0
 *          param:  s_lcd_emv结构体
 *           （1）   frametype：0-文本框 1-选择框 2-列表框 3-列表选择框
 *           （2）   button: 1有效 D0-选择键 D1-取消键 D2-方向键 D3-功能键
 *           （3）   titleAlige: 0-左对齐 1-中间对齐 2-右对齐
 *           （4）   contentAlige：0-左对齐 1-中间对齐 2-右对齐
 *           （5）   title：框标题
 *           （6）   content：框内显示内容，不同行用\n分割。
 *           （7）   rfu：预留。
 *           ret :   0xff   超时
 *                   0x01   确认
 *                   0x00   取消
 *                   ASCII  对应ASCII键值
 *********************************************************************
 */
//void lcd_EmvDisp(uchar frametype, uchar button, uchar titlealign, uchar contextalign, uchar *title, 
//                 uchar* context, int contextlen, int timeout)
int lcd_EmvDisp(s_lcd_emv * lcd_emvdis, int timeout) 
{
    int ret ;
    // 该函数的返回值需要特别注意，和其他函数有不同，返回值存在负
    ret = lcd_emvdisp( lcd_emvdis,  timeout); 
    if ( ret < 0 ) {
        return 0; // 表示取消       
    }

    return ret;
}
//*********************************************************************
//                         按键模块定义
//*********************************************************************
void kb_Sound(int freq, int duration_ms)
{

}

int kb_Hit(void)
{
    return kb_hit();
}

int kb_GetKey(int timeout_ms)
{
    return kb_getkey(timeout_ms);
}

void kb_Flush(void)
{
    kb_flush();
}

int kb_GetStr(uint input_mode, int minlen, int maxlen, int timeout_ms, char *str_buf)
{
    return (kb_getstr(input_mode, minlen, maxlen, timeout_ms, str_buf));
}

int kb_GetHz(uint input_mode, int minlen, int maxlen, int timeout_ms, char *str_buf)
{
    return 0;
}

int kb_InKey(int key)
{
    return drv_kb_inkey(key);
}

int kb_ReadKey(uchar *keybuf)
{
    if (keybuf == NULL)
        return -KB_ERROR;
    return drv_kb_read_buffer(keybuf);
}


//*********************************************************************
//                         打印模块定义
//*********************************************************************
#if (defined CFG_TPRINTER)
int prn_Reset(void)
{
    return 0;
}

int prn_SetFont(int prnfont)
{
    return 0;
}

void prn_SetZoom(int x_zoom, int y_zoom)
{
    return;
}

void prn_SetSpace(int text_space, int line_space)
{
    return;
}

int prn_SetTab(int Width)
{
    return 0;
}

int prn_SetIndent(int indent)
{
    return 0;
}

int prn_GetCurline(void)
{
    return 0;
}

void prn_Move(int line_offset)
{
    return;
}
int prn_GetTemperature(void)
{
    return 25;
}

int prn_Printf(const char *format, ...)
{
    return 0;
}

int prn_Bmp(int rotate, const void *bmpbuf)
{
    return 0;
}

int prn_Pic(int Width, int Height, const uchar *bmpbuf)
{
    return 0;
}

int prn_Start(void)
{
    return 0;
}

int prn_GetStatus(void)
{
    return 0;
}

int prn_GetAttribute(int attri, int *attrivalue)
{
    return 0;
}

int prn_SetAttribute(int attri, int attrivalue)
{
    return 0;
}

int prn_Open(void)
{
    return 0;
}

int prn_Close(void)
{
    return 0;
}
#endif

//*********************************************************************
//                         IC卡模块定义
//*********************************************************************
#if (defined CFG_ICCARD)
//在模块中已完成
#endif

//*********************************************************************
//                         磁卡模块定义
//*********************************************************************
#if (defined CFG_MAGCARD)
void mag_Open(void)
{
    mag_open();
    return;
}

void mag_Close(void)
{
    mag_close();
}

void mag_Clear(void)
{
    mag_clear();
    return;
}

int mag_Check(void)
{
    return (mag_check(0) == EM_mag_SUCCESS);
}

int mag_Read(void *track1, void *track2, void *track3)
{
    return(mag_read(track1, track2, track3));
}

#endif

//*********************************************************************
//                         射频模块定义
//*********************************************************************
#if (defined CFG_RFID)
//在模块中已完成
#endif

//*********************************************************************
//                         文件系统模块定义
//*********************************************************************
int fs_Open(const char *file_name, int open_mode)
{
#ifdef CFG_FS
    return fs_open(file_name,open_mode);
#else
    return 0;
#endif
}
int fs_Close(int fileno)
{
#ifdef CFG_FS
    return fs_close(fileno);
#else
    return 0;
#endif
}
int fs_Seek(int fileno, int offset, int whence)
{
#ifdef CFG_FS
    return fs_seek(fileno,offset,whence);
#else
    return 0;
#endif
}
int fs_Read(int fileno, void *outbuf, int bytelen)
{
#ifdef CFG_FS
    return fs_read(fileno, outbuf, bytelen);
#else
    return 0;
#endif
}
int fs_Write(int fileno, const void *inbuf, int bytelen)
{
#ifdef CFG_FS
    return fs_write(fileno, inbuf, bytelen);
#else
    return 0;
#endif
}
int fs_Size(const char *file_name)
{
#ifdef CFG_FS
    return fs_size(file_name);
#else
    return 0;
#endif
}
int fs_Exist(const char *file_name)
{
#ifdef CFG_FS
    return fs_exist(file_name);
#else
    return 0;
#endif
}
int fs_Truncate(int fileno)
{
#ifdef CFG_FS
    return fs_truncate(fileno);
#else
    return 0;
#endif
}
int fs_Remove(const char *file_name)
{
#ifdef CFG_FS
    return fs_remove(file_name);
#else
    return 0;
#endif
}
int fs_Rename(const char *oldname, const char *newname)
{
#ifdef CFG_FS
    return fs_rename(oldname,newname);
#else
    return 0;
#endif
}
int fs_Freespace(void)
{
#ifdef CFG_FS
    return fs_freespace();
#else
    return 0;
#endif
}
int fs_List(int begno, file_attr_t * fileattr)
{
#ifdef CFG_FS
    return fs_list( begno, fileattr);
#else
    return 0;
#endif
}
//*********************************************************************
//                         串口模块定义
//*********************************************************************
const UARTn gcUartPortSwitch[] = {
    SERIL_NOTVALID, /*UART invalid*/
    UART2,     /*UART_COM1*/
    SERIL_NOTVALID, /*UART invalid*/
    SERIL_NOTVALID, /*UART invalid*/
    SERIL_NOTVALID, /*UART invalid*/
};


//static void uart2_isr(void)
//{
//    char ch;
//
//    ch = hw_uart_getchar(CNL_COMPORT);
//    QueueWrite(&sgSerialOpt[CNL_COMPORT].queue,
//               (unsigned char *)&ch,
//               1);
//}

int _uart_para_check(int uart_id)
{
    if ( uart_id < UART_COM1 || uart_id > UART_COM4) {
        return (-EUART_VAL);
    }
    return 0;
}

int uart_Open(int uart_id, const char *uart_para)
{
    char tmps[16];
    uint baud_rate;
    //    uint port;
    //	ushort data_bits,stop_bits,parity;
    ushort i,j, AttrLen = 0;
    if(uart_para == NULL)
    {
        return (-EUART_VAL);
    }
    if (_uart_para_check(uart_id) != 0) {
        return (-EUART_VAL);
    }
    AttrLen = strlen(uart_para);
    for(i = 0, j = 0; i < AttrLen; i++)
    {
        if(uart_para[i] == ',')
        {
            break;
        }

        if(uart_para[i] != 0x20)
        {
            tmps[j++] = uart_para[i];
        }
        if(j >= 7)
        {
            return (-EUART_VAL);
        }
    }
    if(i >= AttrLen)
    {
        return (-EUART_VAL);
    }

    tmps[j] = 0;
    baud_rate = atol(tmps);
    if((baud_rate < 4800) || (baud_rate > 460800))
    {
        return (-EUART_VAL);
    }
    switch (uart_id)
    {
    case UART_COM1:
        return drv_uart_open(gcUartPortSwitch[uart_id],
                            baud_rate,
                            gwp30SysBuf_c.work,/*receive buffer */
                            WRK_BUFSIZE_C,
                            UartAppCallBack[gcUartPortSwitch[uart_id]]);
//        break;
//    case UART_COM2 :
//        return drv_uart_open(gcUartPortSwitch[uart_id],
//                            baud_rate,
//                            gwp30SysBuf_c.work,/*receive buffer */
//                            WRK_BUFSIZE_C,
//                            uart2_isr);
//        break;
    default :
        return (-EUART_VAL);
    }
}

int uart_Read(int uart_id, void *outbuf, int bytelen, int timeout_ms)
{
    if (_uart_para_check(uart_id) != 0) {
        return (-EUART_VAL);
    }
    return(drv_uart_read(gcUartPortSwitch[uart_id],(uint8_t *)outbuf,bytelen,timeout_ms));
}

int uart_Write(int uart_id, const void *inbuf, int bytelen)
{
    if (_uart_para_check(uart_id) != 0) {
        return (-EUART_VAL);
    }
    return(drv_uart_write(gcUartPortSwitch[uart_id],(const uint8_t *)inbuf,bytelen));
}

int uart_Close(int uart_id)
{
    if (_uart_para_check(uart_id) != 0) {
        return (-EUART_VAL);
    }
    return(drv_uart_close(gcUartPortSwitch[uart_id]));
}

int uart_Printf(int uart_id, const char *format, ...)
{
    va_list ap;
    int count;
	char  buff[256 + 1]; //注意缓冲区大小

    if (_uart_para_check(uart_id) != 0) {
        return (-EUART_VAL);
    }
    if (format == NULL)
    {
        return -EUART_VAL;
    }
	memset(buff, 0, sizeof(buff));
	va_start( ap, format);
	count = vsnprintf(buff, sizeof(buff)-1, format, ap);
	va_end( ap );
    if (count < 0 || count > sizeof(buff)-1)
	{
		count = sizeof(buff)-1;
	}
    return(drv_uart_write(gcUartPortSwitch[uart_id], (uint8_t *)buff, count));
}

int uart_Flush(int uart_id)
{
    if (_uart_para_check(uart_id) != 0) {
        return (-EUART_VAL);
    }
    return(drv_uart_clear(gcUartPortSwitch[uart_id]));
}

//int uart_CheckWriteBuf(int uart_id);
//{
//
//}

int uart_CheckReadBuf(int uart_id)
{
    return(drv_uart_check_readbuf(gcUartPortSwitch[uart_id]));
}

//int uart_CheckWriteBufAvailable(int uart_id);
//{
//
//}

//*********************************************************************
//                         USB CDC模块定义
//*********************************************************************
#if defined(CFG_USBD_CDC)
int usbdserial_Open(void)
{
    return drv_usbd_cdc_open(gwp30SysBuf_c.work,sizeof(gwp30SysBuf_c.work));
}

int usbdserial_Close(void)
{
    return drv_usbd_cdc_close();
}

int usbdserial_Read(void *data, unsigned int size, int timeout_ms)
{
    return drv_usbd_cdc_read(data,size,timeout_ms);
}

int usbdserial_Write(void *data, unsigned int size)
{
    return drv_usbd_cdc_write(data,size);
}

int usbdserial_Printf(const char *format, ...)
{
    va_list ap;
    int count;
	char  buff[256 + 1]; //注意缓冲区大小

    if (format == NULL)
    {
        return -USBSER_VAL;
    }
	memset(buff, 0, sizeof(buff));
	va_start( ap, format);
	count = vsnprintf(buff, sizeof(buff)-4, format, ap);
	va_end( ap );
    if (count < 0 || count > sizeof(buff)-1)
	{
		count = sizeof(buff)-1;
	}
    return (drv_usbd_cdc_write((uint8_t *)buff, count));
}

int usbdserial_Clear(void)
{
    return drv_usbd_cdc_clear();
}

int usbdserial_CheckReadBuf(void)
{
    return drv_usbd_cdc_check_readbuf();
}

int usbdserial_Ready(void)
{
    return drv_usbd_cdc_ready();
}
#endif


//*********************************************************************
//                         蓝牙模块定义
//*********************************************************************
#if defined(CFG_BLUETOOTH)    
int bt_Open(void)
{
    return bt_open();
}

int bt_Close(void)
{
    return bt_close();
}

int bt_Write(uchar *pucBuff, uint uiLen)
{
    return bt_write(pucBuff,uiLen);
}

int bt_Printf(const char *format, ...)
{
    int       count=0;
    va_list     marker;
    unsigned char buff[512+1];	/* 1K 就够了，8K太浪费 */

    if ( check_if_online() )
    {
        return -BT_NOTCONNECT;
    }

    if ( format == NULL ) {
        return -BT_VAL;
    }

    memset(buff, 0, sizeof(buff));
    va_start( marker, format);
    count = vsnprintf((char *)buff, sizeof(buff)-1, format, marker);
    va_end( marker );
    if (count < 0 || count > sizeof(buff)-1)
    {
        count = sizeof(buff)-1;
    }

    return bt_write(buff, count);
}

int bt_Read(void *outbuf, int bytelen, int timeOut_ms)
{
    return bt_read(outbuf,bytelen,timeOut_ms);
}

int bt_Clear(void)
{
    return bt_clear();
}

int bt_CheckWriteBuf(void)
{
    return  bt_check_writebuf();
}

int bt_CheckReadBuf(void)
{
    return  bt_check_readbuf();
}

int bt_SetName(uchar *pucBuff, uchar ucLen)
{
    return bt_setname(pucBuff,ucLen);
}

int bt_SetPin(uchar *pucBuff, uchar ucLen)
{
    return bt_setpin(pucBuff,ucLen);
}

int bt_IoCtl(int cmd, union bt_ioctl_arg *arg)
{
    return bt_ioctl(cmd,arg);
}

#endif


