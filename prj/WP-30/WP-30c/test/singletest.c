/*
 * =====================================================================================
 *
 *       Filename:  wp30_ctrl_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  1/13/2015 4:30:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhangjp (), zhangjp@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#include "wp30_ctrl.h"

#if 0 

int test_iccard_auto(int mode);
int test_sram(int mode);
extern int set_local_time(int mode);
extern int select_menu(int mode ,char *chntitle, char *entitle, uint iHighlight, menu_unit_t *pMenu_unit, uint menuSize, uint timeout_ms);
extern const uchar gApduGet4rand[5];
/*-----------------------------------------------------------------------------}
 *  全局变量
 *-----------------------------------------------------------------------------{*/
#define SERIAL_SELF_TEST_DATA    (const char *)"\r\nUART_TEST"
/*-----------------------------------------------------------------------------}
 *  函数定义
 *-----------------------------------------------------------------------------{*/
void DispData(uchar *title, uint32_t len, uchar *pucData)
{
#if CFG_LCD
	uint32_t i = 0, j = 0, k = 2*8;
	if(title)
	{
		lcd_display(0, 0, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,(char *)title);
		k = FONT_SIZE12;
	}
	for(j = 0; j < len; j++)
	{
		lcd_display(i, k, FONT_SIZE12, "%02x", pucData[j]);
		i = i + 16;
		if(i >= 128)
		{
			i = 0;
			k += FONT_SIZE12;
		}

		if((k+FONT_SIZE12) > 64)
		{
			if(kb_getkey(-1) == KEY_CANCEL)
				return;
			lcd_ClrLine(2*FONT_SIZE12, LCD_WIDE-1);
			k = 2*FONT_SIZE12;
		}
	}
	kb_getkey(-1);
#endif
}
void TestEndHookFail(void)
{
    kb_GetKey(-1);
}

void BeepErr(void)
{
    if ( s_getProduct() == STAR_MK210V301) {
        sys_BeepPro(BEEP_PWM_4KHZ, 1000, NO);
    }else{
        sys_BeepPro(BEEP_PWM_HZ, 1000, NO);
    }
}

// mode: 0-自动 1-手动 
// type: 0-白屏 1-黑屏
int test_display_whole_screen(int mode, int type)
{
#if CFG_LCD
    uint i;
    int key;
	kb_Flush();
	lcd_Cls();
    if ( type == 1 ) {
        lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "液晶黑屏后");
        lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "请检查是否有白点");
    } else {
        lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "液晶清屏后");
        lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "请检查是否有异物");
    }
	sys_DelayMs(1500);
    lcd_SetInv(type); // 反白显示开启
    for ( i=0 ; i<8 ; i++  ) {
        if ( type == 1 ) {
            lcd_Display(0,i*FONT_SIZE12, DISP_FONT|DISP_CLRLINE|DISP_INVLINE, " ");
        }else
            lcd_Display(0,i*FONT_SIZE12, DISP_FONT|DISP_CLRLINE, " ");
    }
    kb_GetKey(3000);
	lcd_SetInv(0); // 反白显示关闭
    if ( mode ) {
        lcd_Cls();
        lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "正常请按[1]");
        //	lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "有异物请按[退格]");
        while (1)
        {
            key = kb_GetKey(-1);
            if ( key == KEY_CANCEL || key == KEY_BACKSPACE) {
                return -1;
            } else if(key == KEY1) {
                break;
            }
        }
    }
    return 0;
}
#endif
// mode:1-自动测试  0-手动测试
int test_keyboard(int mode)
{
    int iRet;
    int keyPushFlag = 0;
    int keyHistory = 0;

    lcd_Cls();
    lcdDispMultiLang(0, 0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "按键测试", "TEST KEYBOARD");
    lcdDispMultiLang(0, DISP_FONT_LINE1, DISP_FONT, "连按[取消]键退出", "Dual[Cancel]to exit");
    kb_Flush();
    while(1)
    {
        iRet = kb_GetKey(-1);
        switch(iRet)
        {
            case KEY0:
            case KEY1:
            case KEY2:
            case KEY3:
            case KEY4:
            case KEY5:
            case KEY6:
            case KEY7:
            case KEY8:
            case KEY9:
                keyPushFlag |= 1 << (iRet - KEY0);
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "[%c]键", "KEY:%c", iRet);
                break;
            case KEY_F1:
                keyPushFlag |= 1 << 10;
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "[F1]键", "KEY: F1");
                break;
            case KEY_ENTER:
                keyPushFlag |= 1 << 11;
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "[确认]键", "KEY: ENTER");
                break;
            case KEY_CANCEL:
                keyPushFlag |= 1 << 12;
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "[取消]键", "KEY: CANCEL");
                break;
            case KEY_CLEAR:
                keyPushFlag |= 1 << 13;
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "[清除]键", "KEY: CLEAR");
                break;
            case KEY_F2:
                keyPushFlag |= 1 << 14;
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "[F2]键", "KEY: F2");
                break;
            case KEY_UP:
                keyPushFlag |= 1 << 15;
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "[上]键", "KEY: UP");
                break;
            case KEY_DOWN:
                keyPushFlag |= 1 << 16;
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "[下]键", "KEY: DOWN");
                break;
            default:
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "无效按键:%02X", "INVALID:%02X", iRet);
                break;
        }

//        lcd_display(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, "%02X",keyPushFlag);
        if ( mode ) {
            // 自动测试
            if ((keyPushFlag & mode) == mode)
            {
                return OK;
            }
        }
        if(iRet != KEY_TIMEOUT)
        {
            if ((keyHistory == KEY_CANCEL) && (iRet == KEY_CANCEL))
            {
                return ERROR;
            }
            keyHistory = iRet;
        }
    }
}
// mode 0-自动 1-手动
int test_display(int mode)
{
    int ret;
	kb_Flush();
	lcd_SetLight(LIGHT_ON_MODE); // 背光常亮
    ret = test_display_whole_screen(mode,1);
    if ( !ret ) {
        ret = test_display_whole_screen(mode,0);
    }
	lcd_SetLight(LIGHT_TIMER_MODE);
    if ( ret ) {
        return ERROR;
    } else {
        return OK;
    }
}
int test_rtc(int mode)
{
    //int result_line = DISP_FONT_LINE1;
    //int state_line = DISP_FONT_LINE4;
	int ret=0;
    char CurTime[64];
    uchar time[7];
    lcd_cls();
    lcdDispMultiLang(0, 0, FONT_SIZE16|DISP_INVLINE|DISP_MEDIACY, "RTC测试", "TEST RTC");
    lcd_display(0, DISP_FONT_LINE1, FONT_SIZE12|DISP_CLRLINE, "[0]设置  [1]快速设置");
    lcd_display(0, DISP_FONT_LINE3, FONT_SIZE12|DISP_CLRLINE, "[取消] 退出");
    while(1)
    {
        sys_GetTime(CurTime);
        lcd_display(0, DISP_FONT_LINE2,
                    DISP_FONT|DISP_CLRLINE, "20%02X-%02X-%02X %02X:%02X:%02X",
                    CurTime[0], CurTime[1], CurTime[2], CurTime[3], CurTime[4], CurTime[5]);
        if (kb_hit())
        {
            switch (kb_getkey(10))
            {
                case KEY_CANCEL:
                    goto FAIL;
                case KEY1:
                    time[0] = 0x10;
                    time[1] = 0x12;
                    time[2] = 0x31;
                    time[3] = 0x23;
                    time[4] = 0x59;
                    time[5] = 0x56;
                    time[6] = 0x1;
					ret = sys_SetTime(time);
                    if(OK != ret)
                    {
                        goto FAIL;
                    }
                    break;
                case KEY0:
                    set_local_time(0);
                    goto SUCCESS;
                default:
                    break;
            }
        }
        sys_DelayMs(50);
    }
SUCCESS:
    return 0;
FAIL:
    return -1;
}

#ifdef CFG_BATTERY
int test_battery(int mode)
{
    int result;
    int mv;
    int state;
    int iRet = 0;

    iRet = iRet;
    lcd_cls();
    lcdDispMultiLang(0, 0, FONT_SIZE16|DISP_INVLINE|DISP_MEDIACY, "电池测试", "");

    if ( mode == 0 )
    {
        while (1)
        {
            state = sys_GetSupplyState();
            switch (state)
            {
            case SUPPLY_VIN_BYOFF_STATE:
                lcd_display(0,  DISP_FONT_LINE1, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "适配器供电,无电池");
                break;
            case SUPPLY_VIN_BYON_STATE:
                result = sys_GetBattery(&mv);
                if (result != OK)
                {
                    goto FAIL;
                }
                lcd_display(0,  DISP_FONT_LINE1, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "适配器供电,有电池");
                lcd_display(0, DISP_FONT_LINE2, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "电池电量为");
                lcd_display(0, DISP_FONT_LINE3, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "%d mv", mv);
                break;
            case SUPPLY_VIN_BYFULL_STATE:
                result =  sys_GetBattery(&mv);
                if (result != OK)
                {
                    goto FAIL;
                }
                lcd_display(0,  DISP_FONT_LINE1, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "适配器供电,有电池");
                lcd_display(0,  DISP_FONT_LINE2, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "电池电量为(满)");
                lcd_display(0,   DISP_FONT_LINE3, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "%d mv", mv);
                break;
            case SUPPLY_BATTERY_NOR_STATE:
            case SUPPLY_BATTERY_LOW_STATE:
                result =   sys_GetBattery(&mv);
                if (result != OK)
                {
                    goto FAIL;
                }
                lcd_display(0,  DISP_FONT_LINE1, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "电池供电");
                lcd_display(0,   DISP_FONT_LINE2, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "电池电量为");
                lcd_display(0,   DISP_FONT_LINE3, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "%d mv", mv);
                break;
            case SUPPLY_USB_STATE:
                lcd_display(0,  DISP_FONT_LINE1, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "USB供电,无电池");
                break;
            default:
                lcd_display(0,  DISP_FONT_LINE1, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
                            "未知状态%d", state);
                break;
            }
            sys_delay_ms(100);
            if (kb_hit())
            {
                switch (kb_getkey(10))
                {
                case KEY_CANCEL:
                    goto SUCCESS;
                    break;
                default:
                    break;
                }
            }
        }

    }
    else if ( mode == 1 ) 
    {
//        //装配检要求插入适配器和电池
//            iRet = sys_GetSupplyState();
//            while (iRet != SUPPLY_VIN_BYON_STATE
//                        && iRet != SUPPLY_VIN_BYFULL_STATE)//没插适配器和电池
//                {
//                    lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "请插入适配器和电池");
//                    iRet = sys_GetSupplyState();
//                    sys_DelayMs(100);
//                }
        return OK;
    }
    else if ( mode == 2 ) 
    {
        while(1)
        {
            iRet  = sys_GetBattery(&mv);
            if ( mv <= 3800 ) {
                lcd_display(0, DISP_FONT_LINE3,FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,"电量不足:%d",mv);
                kb_getkey(1000);
                return ERROR;
            }
            break;
        }
    }
SUCCESS:
    return OK;
FAIL:
    lcd_display(0, DISP_FONT_LINE3,FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,"获取失败,错误%d", result);
    kb_getkey(1000);
    return ERROR;
}
#endif

#ifdef DEBUG_Dx 

extern int test_select_cardslot(void);
extern void test_iccard_powerup(int cardslot);
extern void test_iccard_exchange(int cardslot);
extern void test_iccard_powerdown(int cardslot);
extern void test_iccard_loopback(int cardslot);
int test_iccard_manual(int mode)
{
	int cardslot = 0;
	int key = 0;
	int verno,maxslot;
	icc_GetInfo(&verno, &maxslot);
	while(1)
	{
		lcd_cls();
		lcdDispMultiLang(0,DISP_FONT_LINE0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "IC卡手动测试%xH","ICCARD TEST%xH",verno);
		lcdDispMultiLang(0,DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE,"1-选择卡座 2-上电 ","1-Sel  2-Pwrup");
		lcdDispMultiLang(0,DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE,"3-读卡    4-下电",  "3-Exch 4-Pwrdown");
		lcdDispMultiLang(0,DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE,"5-循环测试 ",       "5-LoopBack");
		lcdDispMultiLang(0,DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"[取消]-退出","[Cancel]-Exit");
		key = kb_getkey(-1);
		switch(key)
		{
			case KEY1:
				cardslot = test_select_cardslot();
				if(cardslot == KEY_CANCEL)
				{
					return 0;
				}
				break;
			case KEY2:
				test_iccard_powerup(cardslot);
				break;
			case KEY3:
				test_iccard_exchange(cardslot);
				break;
			case KEY4:
				test_iccard_powerdown(cardslot);
				break;
			case KEY5:
				test_iccard_loopback(cardslot);
				break;
			case KEY_CANCEL:
				return 0;
			default:
				break;
		}
	}
}
#endif

#ifdef CFG_RFID
extern int Test_RFID_Fac(int mode);
int test_rfid(int mode)
{
    return Test_RFID_Fac(0);
}
#endif
#ifdef CFG_BLUETOOTH
extern int fac_test_bt(int mode);
// mode: 0- 生产测试  1-单项测试
int test_bt(int mode)
{
#if 11
    int result = 0;
    int ret = 0;
    unsigned char data[256];
    char name[16];
    char pin[8];
    unsigned char addr[6];
    uchar fac_res = 0;
    int phase;

    phase = fac_ctrl_get_cur_phase();
    lcd_cls();
    lcdDispMultiLang(0, 0, FONT_SIZE12|DISP_INVLINE|DISP_MEDIACY, "蓝牙测试", "TEST BT");
    CLRBUF(data);
    kb_flush();
    bt_close();
    result = bt_open();
    if (result < 0)
    {
        lcd_display(0,DISP_FONT_LINE3,FONT_SIZE12|DISP_CLRLINE,"打开失败:%d",result);
        kb_getkey(-1);
        bt_close();
        return ERROR;
    }
    if (!mode) 
    {
        result = fac_test_bt(0);
        if ( result == OK ) 
        {
            fac_res = 1;
        }
        else
        {
            fac_res = 0;
        }
        fac_ctrl_set_test_res(phase,FAC_CTRL_BLUETOOTH,fac_res);
        bt_close();
        return OK;
    }
    else
    {
        while (1)
        {
            if ( !(bt_status()) ) 
            {
                lcd_display(0,DISP_FONT_LINE4,FONT_SIZE12|DISP_CLRLINE,"状态:已连接");
            }
            else
            {
                lcd_display(0,DISP_FONT_LINE4,FONT_SIZE12|DISP_CLRLINE,"状态:未连接");
            }
            if (bt_check_readbuf() > 0)
            {
                result = bt_read(data, 128, 1000);
                if (result > 0)
                {
                    lcd_ClrLine(FONT_SIZE12,LCD_WIDE-1);
                    lcd_display(0, FONT_SIZE12, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY, "接收:%d", result);
                    ret = bt_write(data, result);
                    lcd_display(0, FONT_SIZE12*2, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY, "发送:%d", ret);
                }
            }
            if (drv_kb_hit())
            {
                memset(name,0,sizeof(name));
                memset(pin,0,sizeof(pin));
                memset(addr,0,sizeof(addr));
                memset(data, 0, sizeof(data));
                switch (kb_getkey(10))
                {
                case KEY_CANCEL:
                    goto SUCCESS;
                case KEY_ENTER:
                    strcpy((char *)data, "Start Bt Test!\n");
                    result = bt_write(data, sizeof("Start Bt Test!\n"));
                    lcd_display(0,DISP_FONT_LINE2, FONT_SIZE12|DISP_CLRLINE, "发送: %d", result);
                    break;
                case KEY1:
                    lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE,
                                "Set Name:");
                    lcd_goto(0, DISP_FONT_LINE2);
                    lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE,
                                " ");
                    lcd_display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE,
                                " ");
                    result = kb_getstr(KB_BIG_NUM, 1, 16, -1, (char *)name);
                    if(result < 0)
                    {
                        break;
                    }
                    result = bt_setname((uchar *)name,result);
                    if (result < 0)
                    {
                        lcd_display(0, DISP_FONT_LINE1,  FONT_SIZE12|DISP_CLRLINE,
                                    "Set Name Fail %d", result);
                        goto END;
                    }
                    else
                    {
                        lcd_display(0, DISP_FONT_LINE1,  FONT_SIZE12|DISP_CLRLINE,
                                    "Set Name Suc");
                        lcd_display(0,DISP_FONT_LINE2,  FONT_SIZE12|DISP_CLRLINE,
                                    "%s", name);
                    }
                    break;
                case KEY2:
                    lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE,
                                "Set Pin:");
                    lcd_goto(0, DISP_FONT_LINE2);
                    lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE,
                                " ");
                    lcd_display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE,
                                " ");
                    result = kb_getstr(KB_BIG_NUM, 1, 4, -1, (char *)pin);
                    if(result < 0)
                    {
                        break;
                    }
                    result = bt_setpin((uchar *)pin,4);
                    if (result < 0)
                    {
                        lcd_display(0,DISP_FONT_LINE1,  FONT_SIZE12|DISP_CLRLINE,
                                    "Set Pin Fail %d", result);
                        goto END;
                    }
                    else
                    {
                        lcd_display(0, DISP_FONT_LINE1,  FONT_SIZE12|DISP_CLRLINE,
                                    "Set Pin Suc");
                        lcd_display(0,DISP_FONT_LINE2,  FONT_SIZE12|DISP_CLRLINE,
                                    "%s", pin);
                    }
                    break;
                case KEY3:
                    result = bt_getname(name);
                    if (result < 0)
                    {
                        lcd_display(0,DISP_FONT_LINE1,  FONT_SIZE12|DISP_CLRLINE,
                                    "Get Name Fail %d", result);
                        goto END;
                    }
                    else
                    {
                        lcd_display(0,DISP_FONT_LINE1,  FONT_SIZE12|DISP_CLRLINE,
                                    "Get Name Suc");
                        lcd_display(0,DISP_FONT_LINE2,  FONT_SIZE12|DISP_CLRLINE,
                                    "%s", name);
                    }
                    break;
                case KEY4:
                    result = bt_getpin(pin);
                    if (result < 0)
                    {
                        lcd_display(0,DISP_FONT_LINE1,  FONT_SIZE12|DISP_CLRLINE,
                                    "Get Pin Fail%d", result);
                        goto END;
                    }
                    else
                    {
                        lcd_display(0,DISP_FONT_LINE1,  FONT_SIZE12|DISP_CLRLINE,
                                    "Get Pin Suc");
                        lcd_display(0,DISP_FONT_LINE2,  FONT_SIZE12|DISP_CLRLINE,
                                    "%s", pin);
                    }
                    break;
                case KEY5:
                    result = bt_getaddr(addr);
                    if (result < 0)
                    {
                        lcd_display(0,DISP_FONT_LINE1,  FONT_SIZE12|DISP_CLRLINE,
                                    "Get Addr Fail %d", result);
                        goto END;
                    }
                    else
                    {
                        lcd_display(0,DISP_FONT_LINE1,  FONT_SIZE12|DISP_CLRLINE,
                                    "Get Addr Suc");
                        lcd_display(0,DISP_FONT_LINE2,  FONT_SIZE12|DISP_CLRLINE,
                                    "%x:%x:%x:%x:%x:%x", addr[0], addr[1], addr[2],
                                    addr[3], addr[4], addr[5]);
                    }
                    break;
                default:
                    break;
                }
            }
            sys_DelayMs(300);
        }

    }
END:
    lcd_ClrLine(DISP_FONT,LCD_WIDE-1);
    kb_GetKey(2000);
SUCCESS:
    bt_close();
    return 0;
#endif
}
#endif

#ifdef CFG_LED
// mode: 0-自动测试  1-单项测试
int test_led(int mode)
{
        //int i,j;
	//int iRet;
	lcd_Cls();
	lcd_Display(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE, "LED 测试");
	lcd_Display(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, "  蓝  黄    绿  红 ");
#if 0
    for ( i=0x01 ,j = 1; i < 0x10 ; i<<=1,j++ ) {
        hw_led_on(i);
        lcd_display(0,DISP_FONT_LINE0+DISP_FONT*j,FONT_SIZE24|DISP_CLRLINE|DISP_MEDIACY,"LED%d亮",j);
        s_DelayMs(600);
    }
    for ( i=0x08 ,j=4; i > 0; i>>=1,j--) {
        s_DelayMs(600);
        hw_led_off(i);
        lcd_display(0,DISP_FONT_LINE0+DISP_FONT*j,FONT_SIZE24|DISP_CLRLINE|DISP_MEDIACY,"LED%d灭",j);
    }
#else
    hw_led_on(LED_ALL);
#endif
    if ( mode == 0 ) {
        lcd_ClrLine(DISP_FONT_LINE1,LCD_WIDE-1);
        lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "正常请按[2]");
        kb_Flush();
        switch(kb_GetKey(-1))
        {
        case KEY2:
             hw_led_off(LED_ALL);
             return OK;
        default:
             hw_led_off(LED_ALL);
             return ERROR;
        }
    } else {
        kb_GetKey(3000);
        hw_led_off(LED_ALL);
        return OK;
    }
}
#endif

/*
	mode=0:自动测试 失败直接返回, 用于自动测试
	mode=1:手动测试 失败等待按键返回
*/
int test_serial_self(int mode)
{
    int ret;
    char readBuffer[32];
    int comPort = UART_COM1;
    int results = 0;

    lcd_Cls();
    lcd_Display(0, 0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "串口测试");
    lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_MEDIACY, "串口[%d]自发自收",comPort);
    uart_Close(comPort);
    ret = uart_Open(comPort, "115200,8,n,1");
    if ( ret ) {
        results = ret;
        goto END;
    }
    CLRBUF(readBuffer);
    uart_Printf(comPort, SERIAL_SELF_TEST_DATA);
    ret = uart_Read(comPort, readBuffer, sizeof(SERIAL_SELF_TEST_DATA) - 1, 1000);
    if ((ret != (sizeof(SERIAL_SELF_TEST_DATA) - 1)) ||
        (memcmp(readBuffer, SERIAL_SELF_TEST_DATA, sizeof(SERIAL_SELF_TEST_DATA) - 1) != 0))
    {
        results = 0;
    } else {
        results = 1;
    }
END:
    lcd_Display(0, DISP_FONT_LINE2+DISP_FONT, DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY,
                "%s", results ? "成功" : "失败");
    uart_Close(comPort);
    if (!results)
    {
        if (mode)
        {
            TestEndHookFail();
        }
        return ERROR;
    }
	else
    {
		if (mode)
		{
        	kb_GetKey(1000);
		}
        return OK;
    }
}
int test_rtc_autocheck(int mode)
{
	char curTime[32];
	memset(curTime, 0, sizeof(curTime));
	sys_GetTime(curTime);
	if (curTime[0] != gFacTest->time[0] ||
		curTime[1] != gFacTest->time[1] ||
		curTime[2] != gFacTest->time[2] ||
		curTime[3] != gFacTest->time[3] ||
		curTime[4] != gFacTest->time[4] ||
		curTime[5] != gFacTest->time[5] )
	{
        TRACE_M("\r\nRTC自动测试成功");
        return OK;
	}
	else
	{
        TRACE_M("\r\nRTC自动测试失败");
        return ERROR;
	}
}

#ifdef CFG_MAGCARD
//mode: D0~D7:   0-自动 1-手动 
//      D8~D15:  1-磁道都有数据才算通过   0-有任意1个磁道有数据就算通过 
//      D16D17D18:  需要测试的磁道n 
int test_magcard_auto(int mode)
{
    int    MagRet;
    char   Track[3][256];
    int     ErrTime, AllTime, ContiOkTime;
    //int NeedOkTime;
    int     trklen[3],i;
    int     ErrFlag;
    int		iRet;
    //NeedOkTime = 0;
    ErrTime = 0;
    AllTime = 0;
    ContiOkTime = 0;
    kb_Flush();
    lcd_Cls();
    lcdDispMultiLang(0, 0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "磁卡测试", "TEST MAGCARD");
    lcdDispMultiLang(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "请刷卡<<<", "Swipe<<<");
	mag_Open();
    while(1)
    {
//        sys_SleepWaitForEvent(EVENT_MAGSWIPED|EVENT_KEYPRESS,0,0);
        while(mag_Check() == NO)
        {
//            if(AllTime != 0)
//            {
///*                 lcd_Display(0, 5*8, DISP_FONT|DISP_CLRLINE, "Succeed: %d%%", (AllTime-ErrTime)*100/AllTime); */
//            }
            lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "TOTAL:%d, ERR:%d", AllTime, ErrTime);
            lcd_Display(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, "ContiOK: %d", ContiOkTime);

            while(2)
            {
                if(mag_Check() == YES)
                {
                    break;
                }
                iRet = kb_GetKey(10);
                if(iRet == KEY_CANCEL)
                {
                    goto FAIL;
                }
            }
        }
        lcd_Cls();
        lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "测试磁卡", "TEST MAGCARD");
        lcdDispMultiLang(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "已刷卡", "Swipe OK");
        lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "读取数据", "Reading...");
        sys_Beep();
        memset(Track, 0x00, sizeof(Track));
        memset(trklen, 0x00, sizeof(trklen));
        MagRet = mag_Read(Track[0], Track[1], Track[2]);
        ErrFlag = OFF;
        lcd_Cls();
        for ( i=0 ; i<3 ; i++ ) {
            if ( IFBIT(mode,16+i) ) {
                // 需要测试磁道
                if ( IFBIT(MagRet,i) ) {
                    trklen[i] = (MagRet >> (8*(i+1)))&0xFF ;
                    lcdDispMultiLang(0, DISP_FONT*i, DISP_FONT|DISP_CLRLINE, "磁道%d正确:%d", "Trk%d OK:%d",i+1,trklen[i]);
                }
                if ( IFBIT(MagRet,4+i) ) {
                    lcdDispMultiLang(0, DISP_FONT*i, DISP_FONT|DISP_CLRLINE, "磁道%d无数据", "Trk%d NONE",i+1);
                }
            }
        }
        switch (HBYTE(mode))
        {
        case 0:		//三个磁道都没数据才记录错误
            if ((MagRet & 0x10) &&
                    (MagRet & 0x20) &&
                    (MagRet & 0x40))
            {
                ErrFlag = ON;
            }
            break;
        case 1:	
            //二、三个磁道都有数据才算成功
            for ( i=0 ; i<3 ; i++  ) {
                if ( IFBIT(mode,16+i) && trklen[i] == 0 ) {
                    ErrFlag = ON;
                }
            }
            break;
        default:
            break;
        }
        AllTime++;
        if(ErrFlag == ON)
        {
            ContiOkTime = 0;
            ErrTime++;
            BeepErr();
        }
        else
        {
            ContiOkTime++;
        }

        lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "TOTAL:%d,ERR:%d", AllTime, ErrTime);
        lcd_Display(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, "ContiOK:%d", ContiOkTime);
        //成功条件修改,默认最多刷8次,连续成功4次就算通过
//		if (fac_ctrl_get_cur_phase() == FAC_PHASE_DEBUG)
//		{
//			if (ContiOkTime >= 1)
//			{
//				goto SUCCESS;
//			}
//			if (AllTime >= 3)
//			{
//				goto FAIL;
//			}
//		} else {
            if ( LBYTE(mode) == 0) {
                // 自动测试
                if (ContiOkTime >= test_conf_mag_success())
                {
                    goto SUCCESS;
                }
                if (AllTime > 8)
                {
                    goto FAIL;
                }
            }
//        }
    }
SUCCESS:
    mag_Close();
    return OK;
FAIL:
    mag_Close();
    return ERROR;
}
#endif
#ifdef CFG_RFID
extern int fac_poll_rfid_test(int mode ,int total,int sus);
int test_rfid_auto(int mode)
{
	int ret;
    if (sys_GetModuleType(MODULE_RF_TYPE) == MODULE_NOTEXIST)
    {
        return 0;
    }
    if(rfid_open(0))
    {
	    return 0;
    }
    ret =  fac_poll_rfid_test(1,20, 5);
	rfid_close();
	return ret;
}
#endif
#ifdef CFG_ICCARD
int iccard_powerup(int cardslot, int *plen, uchar *pData)
{
    int ret = 0;
    ret = icc_InitModule(cardslot, VCC_5, ISOMODE);
    if(ret)
    {
        return ret;
    }
    ret = icc_Reset(cardslot,(uint *)plen,(uchar *)pData);
    if(ret)
    {
        return ret;
    }
    return 0;
}


int iccard_exchange(int cardslot, int *plen1, uchar *pData1, int *plen2, uchar *pData2)
{
    int ret;
    ret = icc_ExchangeData(cardslot,sizeof(gApduGet4rand),(uchar *)gApduGet4rand, plen1,pData1);
    return ret;
}

int iccard_powerdown(int cardslot)
{
    icc_Close(cardslot);
    return 0;
}

void test_iccard_showret(int col, int line, int iRet, int mode)
{
    if ( mode == 0 ) {
        switch (iRet)
        {
        case ICC_SUCCESS:
            lcdDispMultiLang(col,line,DISP_FONT,"成功","OK");
            break;
        case -ICC_TIMEOUT:
            lcdDispMultiLang(col,line,DISP_FONT,"超时:%d","TIMEOUT:%d",iRet);
            break;
        case -ICC_COMMERR:
            lcdDispMultiLang(col,line,DISP_FONT,"校验:%d","CHECKERR:%d",iRet);
            break;
        case -ICC_RESPERR:
            lcdDispMultiLang(col,line,DISP_FONT,"交互错误:%d","EXCHERR:%d", iRet);
            break;
        case -ICC_CARDOUT:
            lcdDispMultiLang(col,line,DISP_FONT,"卡不在位:%d","CARDOUT:%d", iRet);
            break;
        case -ICC_ERRORSLOT:
            lcdDispMultiLang(col,line,DISP_FONT,"卡座错误:%d","SLOTERR:%d", iRet);
            break;
        case -ICC_ERRPARA:
            lcdDispMultiLang(col,line,DISP_FONT,"参数错误:%d","PARAERR:%d", iRet);
            break;
        case -ICC_CARDNOPOWER:
        case -ICC_NOPOWER:
            lcdDispMultiLang(col,line,DISP_FONT,"卡未上电:%d","NOPOWER:%d", iRet);
            break;
        default:
            lcdDispMultiLang(col,line,DISP_FONT,"其他错误:%d","OTHERERR:%d", iRet);
            break;
        }
    } else {
        switch (iRet)
        {
        case ICC_SUCCESS:
            lcdDispMultiLang(col,line,DISP_FONT,"成功","OK");
            break;
        case -ICC_TIMEOUT:
            lcdDispMultiLang(col,line,DISP_FONT,"超时",    "TIMEOUT");
            break;
        case -ICC_COMMERR:
            lcdDispMultiLang(col,line,DISP_FONT,"校验",    "CHECKERR");
            break;
        case -ICC_RESPERR:
            lcdDispMultiLang(col,line,DISP_FONT,"交互错误","EXCHERR");
            break;
        case -ICC_CARDOUT:
            lcdDispMultiLang(col,line,DISP_FONT,"卡不在位","CARDOUT");
            break;
        case -ICC_ERRORSLOT:
            lcdDispMultiLang(col,line,DISP_FONT,"卡座错误","SLOTERR");
            break;
        case -ICC_ERRPARA:
            lcdDispMultiLang(col,line,DISP_FONT,"参数错误","PARAERR");
            break;
        case -ICC_CARDNOPOWER:
        case -ICC_NOPOWER:
            lcdDispMultiLang(col,line,DISP_FONT,"卡未上电","NOPOWER");
            break;
        default:
            lcdDispMultiLang(col,line,DISP_FONT,"其他错误","OTHERERR");
            break;
        }
    }
}
int test_iccard_getname(int slot, char *name)
{
#if PRODUCT_NAME == PRODUCT_MPOS
    if ( slot == USERCARD ) {
        strcat(name,"大卡");
    } else {
        sprintf(name,"SAM%d",slot);
    }
#else
        sprintf(name,"SAM%d",slot);
#endif
    return 0;
}

int test_iccard_getslotnum(int mode)
{
	int verno,maxslot,i;
    icc_GetInfo(&verno, &maxslot);
    if ( mode ) {
        for (i = USERCARD; i < maxslot; i++)
        {
            if ( IFBIT(mode,i) ) {
                maxslot++;
            }
        }
    }
    return maxslot;
}

// mode: D0~D7: 需要测试的卡座 1-需要测试 如果都是0,默认配置
// mode: D8~D15: 0-自动测试 1-手动测试
// mode: D16~D23: 0-生产测试 1-单项测试
int test_iccard_auto(int mode)
{
    int cardslot;
    int ret;
    int col = 0;
    char name[16];
    int result[5];
    int stat[5];  //1-上电 2-读卡
    int atrlen[5];
    uchar atr[5][64];
    uchar buffer[5][8];
    char title[64];
    int len[8];
	int verno,maxslot;
	icc_GetInfo(&verno, &maxslot);
    maxslot = test_iccard_getslotnum(LBYTE(mode));
    lcd_cls();
    lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "IC测试%XH", "IC TEST%XH",verno);
    CLRBUF(atrlen);
    CLRBUF(len);
    CLRBUF(result);
    CLRBUF(stat);
    for (cardslot = USERCARD; cardslot <= maxslot; cardslot++)
    {
        CLRBUF(name);test_iccard_getname(cardslot,name);
        lcd_display(0, DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"%s上电",name);
		result[cardslot] = iccard_powerup(cardslot, &atrlen[cardslot],atr[cardslot]);
        if (result[cardslot])
        {
            if(!H2BYTE(mode))
            {
                gFacTest->iccardret[cardslot] = result[cardslot];
            }
            continue;
        }
        ++stat[cardslot];
        lcd_display(0, DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"%s读卡",name);
        result[cardslot] = iccard_exchange(cardslot, &len[cardslot], buffer[cardslot],
                                           &len[cardslot], buffer[cardslot]);
        if (!result[cardslot])
        {
            ++stat[cardslot];
        }
        lcd_display(0, DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"%s下电",name);
        iccard_powerdown(cardslot);
        if(!H2BYTE(mode))
        {
            gFacTest->iccardret[cardslot] = result[cardslot];
        }
    }
    DISPPOS(mode);
    lcd_cls();
    lcd_display(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "正在显示结果");
    lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "按[确认]查看数据");
    ret = kb_getkey(500);
    lcd_cls();
    for (cardslot = USERCARD; cardslot <= maxslot; cardslot++)
    {
        CLRBUF(name);test_iccard_getname(cardslot,name);
        if (col + 12 >= LCD_WIDE)
        {
            kb_getkey(3000);
            lcd_cls();
            col = 0;
        }
        lcd_display(0, col,DISP_FONT|DISP_CLRLINE,"%s", name);
        test_iccard_showret(8*5, col, result[cardslot],1);
        col += 12;
    }
    ret = kb_getkey(3000);
    if (ret == KEY_ENTER)
    {
        for (cardslot = USERCARD; cardslot <= maxslot; cardslot++)
        {
            CLRBUF(name);test_iccard_getname(cardslot,name);
            lcd_cls();
            CLRBUF(title);sprintf(title,"%s上电",name);
            if ( stat[cardslot] >= 1) {
                strcat(title,"数据");
                DispData((uchar *)title,atrlen[cardslot],atr[cardslot]);
            } else {
                lcd_display(0, 0,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,
                            "%s", name);
                test_iccard_showret(0,DISP_FONT_LINE1,result[cardslot],0);
                kb_getkey(-1);
                continue;
            }
            lcd_cls();
            CLRBUF(title);sprintf(title,"%s读卡",name);
            if ( stat[cardslot] >= 2) {
                strcat(title,"数据");
                DispData((uchar *)title,len[cardslot],buffer[cardslot]);
            } else {
                lcd_display(0, 0,
                            DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,
                            "%s", name);
                test_iccard_showret(0,DISP_FONT_LINE1,result[cardslot],0);
                kb_getkey(-1);
                continue;
            }
        }
    }
    for (cardslot = USERCARD; cardslot <= maxslot; cardslot++)
    {
        if(!H2BYTE(mode))
        {
            if ( gFacTest->iccardret[cardslot] != 0 ) {
                return ERROR;
            }
        }
    }
    return OK;
}
// 装配检或总检中调用,包含拔卡测试，自动设置主板ID
// mode: D0~D7: 需要测试的卡座 1-需要测试 如果都是0,默认配置
//       D8~D15:1-需要测试拔卡 
int test_iccard_auto_new(int mode)
{
	int iRet;
	iRet = test_iccard_auto(LBYTE(mode));
	//只要大卡测试没问题,就要再进行插拔卡测试
	if (iRet && (gFacTest->iccardret[USERCARD] != OK))
	{
		goto FAIL;
	}
    // 读配置是否需要测试插拔卡
    kb_Flush();
#if PRODUCT_NAME == PRODUCT_EPP 
    //mk210不进行插拔卡测试
    ic_not_in_slot = 0;
    if ((gFacTest->iccardret[USERCARD] != OK) 
        || (gFacTest->iccardret[SAM1SLOT] != OK)) {
        return ERROR;
    }else
        return OK;

#else
	int count;
    count = 1;
	while (count > 0)
	{
		lcd_Cls();
		lcd_Display(0, DISP_FONT_LINE1, DISP_CLRLINE | DISP_FONT, "请拔出IC卡");
		lcd_Display(0, DISP_FONT_LINE3, DISP_CLRLINE | DISP_FONT | DISP_MEDIACY, "[取消]-退出");
		while (1)
		{
			iRet = icc_CheckInSlot(USERCARD);
			if (iRet == -ICC_CARDOUT)
			{
                //生产测试拔卡后成功返回
                iRet = OK;
				break;
			}
            if ( kb_Hit() ) {
                if ( kb_GetKey(10) == KEY_CANCEL) {
                    ic_not_in_slot = 1;
                    goto FAIL;
                }
            }
		}
		ic_not_in_slot = 0;
		count--;
	}
    if (gFacTest->iccardret[SAM1SLOT] != OK) 
    {
        iRet = ERROR;
    }
	return iRet;

#endif

FAIL:
    return ERROR;
}
#endif


int test_sram_auto(int mode)
{
#ifdef CFG_TAMPER       
#if PRODUCT_NAME == PRODUCT_MPOS
    return OK;
#else
    return Fac_SRAM(mode);
#endif
//    int ret;
//    while(1)
//    {
//        lcd_Cls();
//        lcd_display(0,0,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,"防拆");
//        lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"1-开启防拆");
//        lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"2-查看防拆");
//        switch(kb_getkey(-1))
//        {
//        case KEY1:
//            ret = Fac_SRAM(1);
//            //        lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"防拆状态:");
//            if ( ret == OK ) {
//                lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"防拆开启成功");
//            }
//            else
//            {
//                lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"防拆开启失败");
//            }
//            kb_getkey(-1);
//            break;
//        case KEY2:
//            ret = Fac_SRAM(0);
//            if ( ret == OK  ) 
//            {
//                lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"防拆状态:");
//                lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"正常");
//            }
//            else
//            {
//                lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"防拆状态:");
//                lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"异常");
//            }
//            kb_getkey(-1);
//            break;
//        case KEY_CANCEL:
//            return 0;
//        default:
//            break;
//
//        }
//    }
#else 
    return OK;
#endif
}

// mode : 0 - 生产测试
// mode ：1 - 单项测试
int test_usb_manual(int mode)
{
    unsigned char usbBuffer[1024];
    unsigned char iBuffer[1024];
    int result;
    int usbstate;
    lcd_cls();
    lcdDispMultiLang(0, 0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "USB测试", "USB TEST");

    memset(iBuffer, 0x0, sizeof(iBuffer));
    memset(usbBuffer, 0x0, sizeof(usbBuffer));
    strcpy((char *)iBuffer, "USB TEST!\n");

    kb_flush();
    usbdserial_Close();
    result = usbdserial_Open();
    if (result != OK)
    {
        lcd_display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,
                    "失败,错误%d", result);
        TestEndHookFail();
        return ERROR;
    }
    while (1)
    {
        usbstate = usbdserial_Ready();
        if ( usbstate == -USBD_NOTACTIVE || usbstate == USBD_SUCCESS ) 
        {
            usbstate = USBD_SUCCESS;
        }
        lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE,
                "枚举%s 接收 %d", (!usbstate ? "成功" : "失败"), result);
        if ( !mode ) 
        {
            if ( usbstate == USBD_SUCCESS )
            {
                goto SUCCESS;
            }
        }
        if (kb_hit())
        {
            switch (kb_getkey(10))
            {
                case KEY_CANCEL:
                    goto FAIL;
                case KEY_ENTER:
                    result = usbdserial_Write(iBuffer, sizeof("USB TEST!\n"));
                    lcd_display(0,DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "发送 %d", result);
                    result = 0;
                    break;
                case KEY1:
                    usbdserial_Clear();
                    break;
                default:
                    break;
            }
        }

        if (usbdserial_CheckReadBuf() > 0)
        {
            result = usbdserial_Read(usbBuffer, 1024, 1000);
            if (result > 0)
            {
                usbdserial_Write(usbBuffer, result);
            }
        }
    }

SUCCESS:
    usbdserial_Close();
    return OK;
FAIL:
    usbdserial_Close();
    return ERROR;
}

int test_lowper(int mode)
{
    uint32_t periph;
    lcd_cls();
    lcd_Display(0,DISP_FONT_LINE0,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"1-进入低功耗");
    kb_Flush();
    if ( kb_GetKey(-1) == KEY1 ) {
        periph = gSystem.lpwr.periph;
        gSystem.lpwr.periph = 0;
        sys_SleepWaitForEvent(EVENT_KEYPRESS,1,0);
        gSystem.lpwr.periph = periph;
    }
    return 0;
}

int ShowMenuMatrix(menu_unit_t menu_unit[])
{
    int32_t SelectIndex;
    int32_t col;
    int32_t line;
    int maxItem = 8;  // 1页最大显示项数
    int colItem = 2;  // 1行显示列数

    lcd_cls();
    SelectIndex = 0;
    col = 0;
    line = 0;
    lcdDispMultiLang(0, 0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE, "单项测试", "SIGNAL TEST");
    line += DISP_FONT;
    while (menu_unit[SelectIndex].menu_func != NULL)
    {
        if ((SelectIndex + 1) == 11)
        {
            col += (LCD_LENGTH/ colItem);
        }
        if ( s_lcdgetMenuLang() ) {
            lcd_display(col, line, DISP_FONT, "%d.%s ", ((SelectIndex + 1) % 10), menu_unit[SelectIndex].prompt_chn);
        } else {
            lcd_display(col, line, DISP_FONT, "%d.%s ", ((SelectIndex + 1) % 10), menu_unit[SelectIndex].prompt_en);
        }
        if (((col + (LCD_LENGTH / colItem)) + 40) >= LCD_LENGTH)
        {
            col = 0;
            line += FONT_SIZE12;
        } else
        {
            col += (LCD_LENGTH / colItem);
        }
        SelectIndex++;
        if (SelectIndex == maxItem)
        {
            break;
        }
    }
    return 0;
}
int SelectIndexMenu(int TimeOutMs, menu_unit_t menu_unit[])
{
    int index;
    int iRet;

    if(menu_unit == NULL)
    {
        return(-KB_ERROR);
    }

    while(1)
    {
//        sys_SleepWaitForEvent(EVENT_KEYPRESS,0,0);
        iRet = kb_getkey(TimeOutMs);
        switch(iRet)
        {
            case KEY1:
            case KEY2:
            case KEY3:
            case KEY4:
            case KEY5:
            case KEY6:
            case KEY7:
            case KEY8:
                index = iRet - KEY0;
                return index;
            case KEY_TIMEOUT:
                return KB_TIMEOUT;
            case KEY_ENTER:
                return KEY_ENTER;
            case KEY_CANCEL:
                return KEY_CANCEL;
            default :
                return KB_TIMEOUT;
        }
    }
}
    // 单项测试
const menu_unit_t gMenuSingleTest[] = {
    {"按键"              , "KEY"            , -1, 0,      test_keyboard},
    {"液晶"              , "LCD"            , -1, 0,      test_display},
#if CFG_LED
    {"LED"              , "LED"            , -1, 1,       test_led},
#endif
    {"时间"              , "RTC"            , -1, 0,      test_rtc},
#ifdef CFG_BATTERY
    {"电池"              , "BATTERY"        , -1, 0,      test_battery},
#endif
    {"串口"              , "UART"           , -1, 1,      test_serial_self},
    {"防拆"              , "SRAM"           , -1, 0,      set_attack_menu},
#ifdef CFG_MAGCARD
    {"磁卡"              , "MAG"            , -1, 1|(1<<16)|(1<<17)|(1<<18),      test_magcard_auto},
#endif
#ifdef CFG_BLUETOOTH
    {"蓝牙"              , "BT"             , -1, 1,      test_bt},
#endif
#ifdef CFG_ICCARD
    {"IC卡自"            , "ICCARD Auto"    , -1,0|(1<<8)|(1<<16),      test_iccard_auto},
#endif
#ifdef DEBUG_Dx 
    {"IC卡手"            , "ICCARD Manual"  , -1, 0,      test_iccard_manual},
#endif
#ifdef CFG_RFID
    {"射频卡"            , "RFID"           , -1, 1,      test_rfid},
#endif
#ifdef CFG_USBD_CDC
    {"USB "              , "USB "           , -1, 1,      test_usb_manual},
#endif
    {"OQC "              , "OQC "           , -1, 0,      test_oqc},
#ifdef CFG_LOWPWR
    {"低功耗"            , "lowpower "      , -1, 0,      test_lowper},
#endif
    {NULL                , NULL             , -1, 0,      NULL},
};

int local_test_menu(int mode)
{
    int32_t SelectIndex = 0;
    int index;

    menu_unit_t  *pMenu;

    pMenu = (menu_unit_t *)&gMenuSingleTest[0];
    index = 0;

    while(1)
    {
        ShowMenuMatrix(pMenu);
//        sys_SleepWaitForEvent(EVENT_KEYPRESS,0,0);

        SelectIndex = SelectIndexMenu(-1, pMenu);
        switch(SelectIndex)
        {
            case KEY_CANCEL:
                return(OK);
            case KB_TIMEOUT:
                continue;
            case KB_ERROR:
                return(OK);
            case KEY_ENTER:
                if ((index + 8) < (ARRAY_SIZE(gMenuSingleTest) - 1))
                {
                    index += 8;
                }
                else
                {
                    index = 0;
                }
                pMenu = (menu_unit_t *)&gMenuSingleTest[index];
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                if (SelectIndex > 8)
                {
                    continue;
                }
                if (SelectIndex >= 1)
                {
                    SelectIndex -= 1;
                }
                if((SelectIndex + index) < (ARRAY_SIZE(gMenuSingleTest) - 1))
                {
                    pMenu[SelectIndex].menu_func(pMenu[SelectIndex].para);
                    kb_flush();
                }
            default :
                break;
        }
    }
}

#endif
