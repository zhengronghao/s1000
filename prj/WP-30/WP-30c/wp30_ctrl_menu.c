/*
 * =====================================================================================
 *
 *       Filename:  wp30_ctrl_menu.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  1/5/2015 2:48:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhangjp (), zhangjp@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#include "wp30_ctrl.h"
#include "./libdll/libdll.h"

int dl_process(int mode)
{
    struct LocalDlOpt localdl;
//    uint8_t buffer[DL_LOCAL_FRAME_SIZE+64];
    int iRet;

    memset(&localdl,0,sizeof(struct LocalDlOpt));
    localdl.commbuf[0] = DL_COMPORT;
    localdl.serialbuf[0] = gwp30SysBuf_c.pub;
    localdl.seriallen[0] = sizeof(gwp30SysBuf_c.pub);

//    localdl.commbuf[1] = DL_COMUSBD;//SERIL_NOTVALID;
//    localdl.serialbuf[1] = buffer;
//    localdl.seriallen[1] = sizeof(buffer);

    localdl.frame = (struct LocalDlFrame *)gwp30SysBuf_c.work;
    localdl.dllevel = DL_HANDSHAKE_CTRL;
    localdl.callback = &gcLocaldlCallBack;
#ifdef DEBUG_Dx 
    if (CNL_COMPORT == localdl.commbuf[0] 
//        || CNL_COMPORT == localdl.commbuf[1]) {
        ) {
#if defined(CFG_USBD_CDC)
        if (console_get_class() == USBD_CDC)
        {
            while (1)
            {
                if(drv_usbd_cdc_ready() == -USBD_NOTACTIVE)
                {
                    break;
                }
                s_DelayMs(800);
                TRACE("\n-|Plse CLOSE PC Serial tools:SSCOM or others!");
            }
        }
#endif
        console_close();
    }
#endif
    localdl_com_open(&localdl);
    lcd_cls();
    while (1)
    {
        iRet = localdl_process(&localdl);
        if (iRet == LOCALDL_CANCEL)
        {
            break;
        } else if (iRet == OK)
        {
            break;
        }
    }
    localdl_com_close(&localdl);
#ifdef  DEBUG_Dx 
    if (CNL_COMPORT == localdl.commbuf[0] 
        || CNL_COMPORT == localdl.commbuf[1]) {
        s_DelayMs(1000);
        console_init(CNL_COMPORT,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
    }
#endif
    return 0;
}

#if 0
int menu_password_verify(int mode)
{
    int  ret;
    int  pswlen;
    int  pswvalue = 0; //配置区密码是否有效   0-有效 1-无效
    char passWordStr[32];
    MODULE_SYS_INFO  tsysinfo;

    if ( mode < 4 || mode > 32 )
    {
        return -1;
    }
    lcd_cls();
    lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE, "请输入密码", "PASSWORD:");
    lcd_goto(0,DISP_FONT*2);
    CLRBUF(passWordStr);
    ret = kb_getstr(KB_BIG_PWD, 0, mode, -1, passWordStr);
    if(ret < 0)
    {
        if (ret == -KB_CANCEL)
        {
            goto CANCEL;
        }
        goto FAIL;
    }
    s_sysinfo_sysinfo(0,&tsysinfo);
    TRACE("passWordStr:%s\r\n",passWordStr);
    TRACE("tsysinfo.menupassword:%s\r\n",tsysinfo.menupassword);
    pswlen = strlen((char *)tsysinfo.menupassword);
    if (pswlen < 6 || pswlen > sizeof(tsysinfo.menupassword)) 
    {
        pswvalue = 1;
    }
    for(int i=0;i<pswlen;i++)
    {
        if ( tsysinfo.menupassword[i] < '0' || tsysinfo.menupassword[i] > '9' ) 
        {
            pswvalue = 1;
        }
    }
    if(pswvalue)
    {
        if (0 != memcmp(passWordStr,DEFAULT_MENU_PASSWORD,sizeof(tsysinfo.menupassword)))
        {
            lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "密码错误!", "PASSWORD ERROR!");
            sys_delay_ms(1000);
            goto FAIL;
        }
    }
    else
    {
        if (0 != memcmp(passWordStr,tsysinfo.menupassword,sizeof(tsysinfo.menupassword)))
        {
            lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "密码错误!", "PASSWORD ERROR!");
            sys_delay_ms(1000);
            goto FAIL;
        }
    }
    return OK;
FAIL:
    return ERROR;
CANCEL:
    return KB_CANCEL;
}

int show_local_ver(int WaitMode)
{
    char buf[32];
    int i;
    uint page = 0;
    while(1)
    {
        lcd_cls();
        lcd_Display(0,0,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,"%s",k_SonTermName);
        if ( page == 0 ) {
            CLRBUF(buf); sys_read_ver(READ_SN_VER,buf);
            lcd_display(0,DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "SN:%s",buf);
            CLRBUF(buf); sys_read_ver(READ_BOOT_VER,buf);
            lcd_display(0,DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "BOOT:%s",buf);
            CLRBUF(buf); sys_read_ver(READ_CTRL_VER,buf);
#if PRODUCT_NAME == PRODUCT_MPOS
            lcd_display(0,DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "CTRL:%s",buf);
#else
            //MK210提供应用版本V3.0.0.X  监控版本与wp30同步(pc端查看)
            lcd_display(0,DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "APP:%s",buf);
#endif
            CLRBUF(buf); sys_read_ver(READ_MAINB_VER,buf);
            lcd_display(0,DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, "MB:%s",buf);
        } else {
            CLRBUF(buf); sys_read_ver(READ_FONT_VER,buf);
            lcd_display(0,DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "FONT:%s",buf);
            CLRBUF(buf); sys_read_ver(READ_CTRLTIME_VER,buf);
            lcd_display(0,DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "TIME:%s",buf);
            CLRBUF(buf); sys_read_ver(READ_MAINBID_VER,buf);
            lcd_display(0,DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "ID:%s",buf);
//            lcd_display(0,DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "ID:%x %x %x %x %x %x %x %x",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
        }
        i = kb_getkey(-1);
        if ( i == KEY_ENTER) {
#ifdef CFG_FACTORY
//            fac_disp_log(0);
#endif
        } else if ( i == KEY_CANCEL ) {
            return 0;
        } else {
            page++;
            page = page%2;
        }
    }
#if 0
    for ( i=0 ; i<DIM(vername) ; i++  ) {
        CLRBUF(buf);
        sys_read_ver(i,buf);
        //主板ID:产品号(产品平台+产品号,2B)+00+工单号(6B)+时间(6B)+预留(3B)+CRC16(2B)
        if ( i == READ_MAINBID_VER ) {
            // 1行可以显示21B,显示15B=2+1+6+6
            lcd_display(0,DISP_FONT*i, DISP_FONT|DISP_CLRLINE, "%s:",vername[i]);
            for ( j=0 ; j<15 ; j++ ) {
                lcd_display(3*6+j*6,DISP_FONT*i, DISP_FONT|DISP_CLRLINE, "%2X",buf[j]);
            }
        } else {
            lcd_display(0,DISP_FONT*i, DISP_FONT|DISP_CLRLINE, "%s:%s",vername[i],buf);
        }
    }
    kb_flush();
    if ( kb_getkey(-1) == KEY_ENTER ) {
        //            fac_ctrl_disp_res();
    }
    return 0;
#endif
}
int dl_process(int mode)
{
    struct LocalDlOpt localdl;
//    uint8_t buffer[DL_LOCAL_FRAME_SIZE+64];
    int iRet;

    memset(&localdl,0,sizeof(struct LocalDlOpt));
    localdl.commbuf[0] = DL_COMPORT;
    localdl.serialbuf[0] = gwp30SysBuf_c.pub;
    localdl.seriallen[0] = sizeof(gwp30SysBuf_c.pub);

//    localdl.commbuf[1] = DL_COMUSBD;//SERIL_NOTVALID;
//    localdl.serialbuf[1] = buffer;
//    localdl.seriallen[1] = sizeof(buffer);

    localdl.frame = (struct LocalDlFrame *)gwp30SysBuf_c.work;
    localdl.dllevel = DL_HANDSHAKE_CTRL;
    localdl.callback = &gcLocaldlCallBack;
#ifdef DEBUG_Dx 
    if (CNL_COMPORT == localdl.commbuf[0] 
//        || CNL_COMPORT == localdl.commbuf[1]) {
        ) {
#if defined(CFG_USBD_CDC)
        if (console_get_class() == USBD_CDC)
        {
            while (1)
            {
                if(drv_usbd_cdc_ready() == -USBD_NOTACTIVE)
                {
                    break;
                }
                s_DelayMs(800);
                TRACE("\n-|Plse CLOSE PC Serial tools:SSCOM or others!");
            }
        }
#endif
        console_close();
    }
#endif
    localdl_com_open(&localdl);
    lcd_cls();
    while (1)
    {
        iRet = localdl_process(&localdl);
        if (iRet == LOCALDL_CANCEL)
        {
            break;
        } else if (iRet == OK)
        {
            break;
        }
    }
    localdl_com_close(&localdl);
#ifdef  DEBUG_Dx 
    if (CNL_COMPORT == localdl.commbuf[0] 
        || CNL_COMPORT == localdl.commbuf[1]) {
        s_DelayMs(1000);
        console_init(CNL_COMPORT,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
    }
#endif
    return 0;
}
int dup_process(int mode)
{
    return 0;
}
extern int show_run_info(int mode);
int user_manage_interface(int mode)
{
    int useritem=1,manageritem=1,superitem=1,user;
    const menu_unit_t  tBDOption[] =
    {
        // 添加新的菜单需要需要相应的item
        // 普通管理员 USER_NORMAL
#ifdef CFG_FACTORY 
        {"管控信息",    "fac info",        -1,0,fac_disp_log},
#endif
		{"用户管理",    "user manager",    -1,0,user_manage},
//		{"安全认证管理","SA Manager",      -1,0,sys_set_safelevel},
		{"防拆设置",    "set attack",     -1,0, set_attack_menu},
		{"运行信息",    "Run Info",     -1,0, show_run_info},
		{"模块信息","set hw info",    -1,0, sys_set_hardware_info},
//        // 管理员  USER_MANAGE
//#if defined (CFG_TFT)
//		{"显示设置", "DISP SETTING", -1, 0, lcd_color_set},
//#endif
//		{"设置SN",      "set SN",         -1,0, set_sn},
//        {"无线模块识别","get wlm type",-1,0, sys_get_wlm_type},
        {"版本重新识别","Modify Version",-1,0, sys_get_version},
//#ifdef CFG_RSAKEY
////    	{"RSA", "RSA", -1, gManager, sys_del_rsakey},
//#endif
//        // 超级管理员 USER_SUPER
//		{"硬件版本","set hw version", -1,0, set_version},
		{"恢复出厂",  "format syszone ",-1,0, sys_format_syszone},
    };
    const char *c_menu_name = "用户管理";
    const char *e_menu_name = "User Manager";
    user = read_user_flag();
    if (user == USER_MANAGE) {
        manageritem = 5;
        return select_menu(1, (char *)c_menu_name, (char *)e_menu_name, 1, (menu_unit_t * )tBDOption, manageritem, -1);
    } else if(user == USER_SUPER){
        superitem = DIM(tBDOption);
        return select_menu(1, (char *)c_menu_name, (char *)e_menu_name, 1, (menu_unit_t * )tBDOption, superitem, -1);
    } else {
        // 普通管理员
        useritem = 5;
        return select_menu(1, (char *)c_menu_name, (char *)e_menu_name, 1, (menu_unit_t * )tBDOption, useritem, -1);
    }
}
int menu_set_keytone(int mode)
{
    uint8_t curSel = 0;
    MODULE_SYS_INFO tsysinfo;
    kb_flush();
    s_sysinfo_sysinfo(0,&tsysinfo);
    if ( tsysinfo.keytone) {
        curSel = 1;
    }
    while(1)
    {
        lcd_cls();
        lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY,"按键伴音","KEYTONE");

        if (curSel) 
        {
            lcdDispMultiLang(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_INVLINE, "1-开启","1-ON");
            lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "2-关闭","2-OFF");
        }
        else
        {
            lcdDispMultiLang(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "1-开启","1-ON");
            lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_INVLINE, "2-关闭","2-OFF");
        }
        switch(kb_getkey(-1))
        {
        case KEY_CANCEL:
            return 0;
        case KEY2:
        case KEY8:
            curSel = (curSel+1)%2;
            break;
        case KEY_ENTER:
        case KEY5:
            goto SUC;
        default:
            break;
        }
    }
SUC:
    tsysinfo.keytone = curSel;
    s_sysinfo_sysinfo(1,&tsysinfo);
    gwp30SysMemory.SysCtrl.bit.keytone = tsysinfo.keytone;
    return 0;
}
int  menu_set_language(int mode)
{
    uint8_t curSel = 0;
    MODULE_SYS_INFO tsysinfo;
    kb_flush();
    s_sysinfo_sysinfo(0,&tsysinfo);
    if ( tsysinfo.language) {
        curSel = 1;
    }
    while(1)
    {
        lcd_cls();
        lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY,"语言","Language");

        if (curSel) 
        {
            lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "1-English");
            lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_INVLINE, "2-中文");
        }
        else
        {
            lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_INVLINE, "1-English");
            lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "2-中文");
        }
         switch(kb_getkey(-1))
         {
         case KEY_CANCEL:
             return 0;
         case KEY2:
         case KEY8:
             curSel = (curSel+1)%2;
             break;
         case KEY_ENTER:
         case KEY5:
             goto SUC;
         default:
             break;
         }
    }
SUC:
    tsysinfo.language = curSel;
    s_sysinfo_sysinfo(1,&tsysinfo);
    gwp30SysMemory.SysCtrl.bit.language= tsysinfo.language;
    return 0;
}

#ifdef DEBUG_Dx
int menu_set_lightmode(int mode)
{
    kb_flush();
    while(1)
    {
        lcd_cls();
        lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY,"背光","LightMode");
        lcdDispMultiLang(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "1-常亮","1-ALWAYON");
        lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "2-常灭","2-ALWAYOFF");
        lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "3-超时","3-TIMEOFF");
        switch(kb_getkey(-1))
        {
        case KEY_CANCEL:
            return 0;
        case KEY1:
            drv_lcd_setbacklight(LCD_MODE_ALWAYSON);
            return 0;
        case KEY2:
            drv_lcd_setbacklight(LCD_MODE_ALWAYSOFF);
            return 0;
        case KEY3:
            drv_lcd_setbacklight(LCD_MODE_TIMEROUT);
            return 0;
        default:
            break;
        }
    }
}
#endif

/* 
 * menu_set_bps - [GENERIC] 
 * @ 
 */
int menu_set_bps (int mode)
{
#if PRODUCT_NAME == PRODUCT_EPP 
    int ret,bps,i;
    const int bpslist[] = {/*1200,*/9600,19200,38400,57600,115200};
    while ( 1 ) {
        bps = uart_get_bps();	
        lcd_Cls();
        lcd_Display(0,DISP_FONT_LINE0,DISP_FONT|DISP_MEDIACY|DISP_INVLINE,"工作波特率");
        lcd_Display(0,DISP_FONT_LINE1,DISP_FONT|DISP_MEDIACY,"当前:%d",bps);
        for ( i=0 ; i<DIM(bpslist) ; i++  ) {
            lcd_Display((LCD_LENGTH/2)*(i%2),DISP_FONT_LINE2+DISP_FONT*(i/2),DISP_FONT,"%d-%d",i+1,bpslist[i]);
        }
        ret = kb_getkey(-1);
        if ( ret >= KEY1 && ret <= (KEY0+DIM(bpslist))) {
            ret -= KEY1;
            if ( bps != bpslist[ret] ) {
                // 不同保存
                s_sysconfig_write(OFFSET_BPS,LEN_BPS,(uchar *)&bpslist[ret]);	
            }
        } else if(ret == KEY_CANCEL){
            break;
        }
    } 
#endif
    return 0;
}		/* -----  end of function menu_set_bps  ----- */

int menu_setpara(int mode)
{
     
	const menu_unit_t  t_menu[] = {
        {"语言", "Language",     -1,0, menu_set_language},
        {"按键伴音", "Key Tone", -1,0, menu_set_keytone},
#if PRODUCT_NAME == PRODUCT_EPP 
        {"波特率",   "BPS",      -1,0, menu_set_bps},
#endif
#ifdef DEBUG_Dx
        {"背光", "LightMode",     -1,0, menu_set_lightmode},
#endif
	};
    const char * const c_menu_name = "系统参数";
    const char * const e_menu_name = "System Para";
    return select_menu(1,(char *)c_menu_name, (char *)e_menu_name, 1, (menu_unit_t * )t_menu, DIM(t_menu), -1);
}
int  menu_password_modify(int mode)
{
    int     ret;
    char    passWordStr1[16];
    char    passWordStr2[16];
    MODULE_SYS_INFO tsysinfo;
    
    while (1)
    {
        lcd_cls();
        lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,
                "请输入新密码:", "NEW PASSWORD:");
        kb_flush();

        memset(passWordStr1, 0, sizeof(passWordStr1));

        lcd_goto(0,DISP_FONT_LINE2);

        ret = kb_getstr(KB_BIG_PWD, 0, 6, -1, passWordStr1);
        if(ret < 0)
        {
            goto FAIL;
        }

        lcd_cls();
        lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,
                "请确认密码:", "CONFIRM PASSWORD");

        lcd_goto(0,DISP_FONT_LINE2);

        memset(passWordStr2, 0, sizeof(passWordStr2));
        ret = kb_getstr(KB_BIG_PWD, 0, 6, -1, passWordStr2);
        if(ret < 0)
        {
            goto FAIL;
        }

        if (memcmp(passWordStr1, passWordStr2, sizeof(passWordStr2)) != 0)
        {
            lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,
                    "密码不一致", "CONFIRM ERROR!");
            sys_delay_ms(2000);
            continue;
        }
        s_sysinfo_sysinfo(0,&tsysinfo);
        memcpy(tsysinfo.menupassword,passWordStr2,sizeof(passWordStr2));
        s_sysinfo_sysinfo(1,&tsysinfo);
        break;
    }

    lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "密码修改成功", "SUCCESS");
    sys_delay_ms(1000);

    return OK;
FAIL:
    return ERROR;
}
int  menu_toolset(int mode)
{
    return 0;
}

int vTwoOne1(uchar *in, uchar *out, int in_len)
{
	int   i;

	for(i=0; i<in_len; i++)
	{
        if((in[i]>='0') && (in[i]<='9'))
        {
            out[i/2] = (out[i/2] & 0xF0) | ((in[i] - '0') & 0x0F);
        }
        else if((in[i]>='A') && (in[i]<='F'))
        {
            out[i/2] = (out[i/2] & 0xF0) | ((in[i] - 'A' + 0x0A) & 0x0F);
        }
        else if((in[i]>='a') && (in[i]<='f'))
        {
            out[i/2] = (out[i/2] & 0xF0) | ((in[i] - 'a' + 0x0A) & 0x0F);
        }
        else
        {
            return(ERROR);
        }
        if(i%2 == 0)
        {
            out[i/2] = out[i/2] << 4;
        }
	}
    return(OK);
}
int  set_local_time(int mode)
{
	int32_t     iRet;
    char     CurTime[64], InputTime[64];
	uint8_t     str[16], Cursor, DefCursor;
    uint32_t    BeginTime, EndTime;

    while(1)
    {
    	lcd_cls();
    	lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "设置系统时间", "Set System Time:");
        lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "YYMMDDHHMMSS:");
        kb_flush();

        memset(InputTime, 0x00, sizeof(InputTime));
        DefCursor = '_';
        Cursor = DefCursor;
        while(1)
        {
            sys_GetTime(CurTime);
            lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "20%02X-%02X-%02X %02X:%02X:%02X",
                CurTime[0], CurTime[1], CurTime[2], CurTime[3], CurTime[4], CurTime[5]);
            lcd_display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "20%s%c", InputTime, Cursor);
            if ( kb_hit() ) 
            {
                iRet = kb_getkey(10);
                switch(iRet)
                {
                case KEY_CANCEL:
                    return 0;
                case KEY_TIMEOUT:
                    Cursor = (Cursor==' ') ? DefCursor : ' ';
                    break;
                case KEY_ENTER:
                    if(strlen(InputTime) < 12)
                    {
//                        sys_beep_pro(DEFAULT_SOUNDFREQUENCY, DEFAULT_WARNING_BEEPTIME, 0);
                        sys_beep();
                    }
                    break;
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
                    if(strlen(InputTime) >= 12)
                    {
//                        sys_beep_pro(DEFAULT_SOUNDFREQUENCY, DEFAULT_WARNING_BEEPTIME, 0);
                        sys_beep();
                    }
                    else
                    {
                        InputTime[strlen(InputTime)] = iRet;
                        Cursor = DefCursor;
                    }
                    break;
                case KEY_BACKSPACE:
                    if(strlen(InputTime) == 0)
                    {
//                        sys_beep_pro(DEFAULT_SOUNDFREQUENCY, DEFAULT_WARNING_BEEPTIME, 0);
                        sys_beep();
                    }
                    else
                    {
                        InputTime[strlen(InputTime)-1] = 0;
                        Cursor = DefCursor;
                    }
                    break;
                case KEY_CLEAR:
                    if(strlen(InputTime) == 0)
                    {
//                        sys_beep_pro(DEFAULT_SOUNDFREQUENCY, DEFAULT_WARNING_BEEPTIME, 0);
                        sys_beep();
                    }
                    else
                    {
                        memset(InputTime, 0x00, sizeof(InputTime));
                        Cursor = DefCursor;
                    }
                    break;
                default:
//                    sys_beep_pro(DEFAULT_SOUNDFREQUENCY, DEFAULT_WARNING_BEEPTIME, 0);
                    sys_beep();
                    break;
                }
            }
            if(strlen(InputTime) >= 12 && iRet == KEY_ENTER)
            {
                break;
            }
            sys_DelayMs(50);
        }

    	memset(str, 0x00, sizeof(str));
        iRet = vTwoOne1((uchar *)InputTime, (uchar*)str, strlen(InputTime));
        if(iRet != OK)
        {
            lcdDispMultiLang(0, 48, DISP_FONT|DISP_CLRLINE, "输入错误", "Input Error!");
        }
        else
        {
            lcd_ClrLine(FONT_SIZE12,LCD_WIDE-1);
            iRet = sys_SetTime(str);
            if(iRet == OK)
            {
                sys_GetTime(str);
                BeginTime = sys_get_counter();
                lcd_ClrLine(FONT_SIZE12,LCD_WIDE-1);
                lcdDispMultiLang(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "设置完成", "Set OK");
                lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "请查看时间跳变!", "Pls Check Time!");
                while(1)
                {
                    sys_GetTime(CurTime);
                    lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "20%02X-%02X-%02X %02X:%02X:%02X",
                        CurTime[0], CurTime[1], CurTime[2], CurTime[3], CurTime[4], CurTime[5]);
                    EndTime = sys_get_counter();
                    if(EndTime - BeginTime > 1500)
                    {
                        sys_GetTime(CurTime);
                        if(!memcmp(CurTime, str, 6))
                        {
                            lcd_ClrLine(FONT_SIZE12,LCD_WIDE-1);
                            lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "不能正常工作!", "RTC Error!");
                            break;
                        }
                    }
                    if(kb_hit() || (EndTime - BeginTime > 5000))
                    {
                        kb_flush();
                        return 0;
                    }
                }
            }
            else
            {
                lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "设置错误", "Set Time Error!");
            }
        }
        sys_beep();
        kb_flush();
        iRet = kb_getkey(-1);
        if(iRet == KEY_CANCEL)
        {
            return 0;
        }
    }
}

// mode:单行显示列数 1-1列  2-2列
int select_menu(int mode ,char *chntitle, char *entitle, uint iHighlight, menu_unit_t *pMenu_unit, uint menuSize, uint timeout_ms)
{
	int menuBegLine;
    int menuEndLine=0;
    int menuBegItem;
    int menuEndItem=0;
    int menuCursor;
    int preCursor=0;
    uint menuDistance;
    uint menuDisplayMode;
    uint line;
    uint item;
	uint key;
	uint starttime,curtime;
    uint dispmode;//反显模式
    uint freshflag; //刷新标志
	menu_unit_t *pMenu;
	pMenu = pMenu_unit;
    char menuPage = 0; 
    char TotalPage = 0; 

    uint col;
    uint  maxItem = 0;  // 1页最大显示项数

    menuDistance = DISP_FONT + 1;
	if(iHighlight == 0)
	{
		iHighlight = 1;
	}
    if (pMenu_unit == NULL || iHighlight > menuSize || iHighlight < 1)
    {
        return -MENU_ERRPARAM;
    }
    if (chntitle != NULL || entitle != NULL)
    {
        menuBegLine = DISP_FONT + 1;
    }
	else
    {
        menuBegLine = 0;
    }
    menuEndLine = LCD_WIDE - (menuDistance - 1);

	starttime = sys_get_counter();

    menuBegItem = iHighlight;
    menuCursor = iHighlight;

    menuDisplayMode = DISP_FONT;
	kb_flush();
    freshflag = 1;
    if ( mode == 2 ) 
    {
        dispmode = DISP_INVCHAR;
        maxItem = 8;
    }
    else
    {
        dispmode = DISP_INVLINE;
        maxItem = 4;
    }
    TotalPage = menuSize/maxItem;
    menuPage = 0;
    while (1)
    {
    	curtime = sys_get_counter();
    	if((curtime - starttime) >=  timeout_ms)
    	{
    		return -MENU_TIMEOUT;
    	}
        line = menuBegLine;
        item = menuBegItem;

        if ( freshflag ) {
            freshflag = 0;
			lcd_cls();
			if (chntitle != NULL || entitle != NULL)
			{
                if ( gwp30SysMemory.SysCtrl.bit.language == 1 ) 
                {
                    lcd_display(0, 0, DISP_MEDIACY|DISP_FONT|DISP_INVLINE, (char *)chntitle);
                }
                else
                {
                    lcd_display(0, 0, DISP_MEDIACY|DISP_FONT|DISP_INVLINE, (char *)entitle);
                }
			}
            col = 0;
            TRACE("menuSize:%d menuCursor:%d menuBegItem:%d menuEndItem:%d\r\n",menuSize,menuCursor,menuBegItem,menuEndItem);
            while(1)
            {
                if ( mode == 2 )
                {
                    if ( !(item % 2)) 
                    {
                        col = LCD_LENGTH/2;
                    }
                    else
                    {
                        col = 0;
                    }
                }
                else
                {
                    col = 0;
                }
                if (item == menuCursor)
                {
                    menuDisplayMode |= dispmode ;
                }
                else
                {
                    menuDisplayMode &= ~(dispmode);
                }
                if ( gwp30SysMemory.SysCtrl.bit.language == 1 ) 
                {
                    lcd_display(col, line, menuDisplayMode, "%d.%s",item,(char *)pMenu_unit[item - 1].prompt_chn);
                }
                else
                {
                    lcd_display(col, line, menuDisplayMode, "%d.%s",item,(char *)pMenu_unit[item - 1].prompt_en);
                }
                if ( mode != 2 ) 
                {
                    line += menuDistance;
                }
                else
                {
                    if ( !(item % 2) )
                    {
                        line += menuDistance;
                    }
                }
                item++;

	            if (line > menuEndLine || item > menuSize)
	            {
	                menuEndItem = item - 1;
	                break;
	            }
	        }

		}

		if(kb_hit())
		{
			key = kb_getkey(10);
			switch (key)
	        {
	            case KEY_ENTER:
                case KEY5:
					kb_flush();
					if (pMenu[menuCursor-1].menu_func != NULL)
						pMenu[menuCursor-1].menu_func(pMenu[menuCursor-1].para);
					kb_flush();
                    freshflag = 1;
					break;
	            case KEY_CANCEL:
	                return -MENU_CANCEL;
	            case KEY2:
                case KEY_UP:
	                menuCursor = menuCursor - mode;
                    if ( menuCursor <= 0 )
                    {
                        menuCursor = menuSize;
                        menuPage = TotalPage;
                        menuBegItem = TotalPage*maxItem+1; 
                        menuEndItem = menuSize - menuBegItem;
                    }
                    if ( menuCursor < menuBegItem )
                    {
                        menuPage--;
                        if ( menuPage < 0) {
                            menuPage = TotalPage;
                            menuCursor = menuSize;
                        }
                    }
                    menuBegItem = menuPage*maxItem + 1;
                    freshflag = 1;
	                break;
                case KEY4:
	                menuCursor = menuCursor - 1;
                    if ( menuCursor <= 0 )
                    {
                        menuCursor = menuSize;
                        menuPage = TotalPage;
                        menuBegItem = TotalPage*maxItem+1; 
                        menuEndItem = menuSize - menuBegItem;
                    }
                    if ( menuCursor < menuBegItem )
                    {
                        menuPage--;
                        if ( menuPage < 0) {
                            menuPage = TotalPage;
                            menuCursor = menuSize;
                        }
                    }
                    menuBegItem = menuPage*maxItem + 1;
                    freshflag = 1;
	                break;
                case KEY6:
	                menuCursor = menuCursor + 1;
                    if ( menuCursor > menuEndItem )
                    {
                        menuPage++;
                        if ( menuPage > TotalPage ) {
                            menuPage = 0;
                        }
                    }
                    if ( menuCursor > menuSize ) {
                        menuCursor = 1;
                        menuBegItem = 1;
                    }
                    menuBegItem = menuPage*maxItem + 1;
                    freshflag = 1;
	                break;
                case KEY8:
                case KEY_DOWN:
                    preCursor = menuCursor;
	                menuCursor = menuCursor + mode;
                    if ( menuCursor > menuSize ) 
                    {
                        if ( (menuSize-preCursor) > 0) 
                        {
                            menuPage = TotalPage;
                            menuEndItem = menuSize;
                            menuCursor = menuSize;
                        }
                        else
                        {
                            menuCursor = 1;
                            menuEndItem = maxItem;
                            menuPage = 0;
                        }
                    }
                    if ( menuCursor > menuEndItem )
                    {
                        menuPage++;
                        if ( menuPage > TotalPage ) {
                            menuPage = 0;
                        }
                        menuCursor = menuPage*maxItem+1;
                    }
                    menuBegItem = menuPage*maxItem + 1;
                    freshflag = 1;
	                break;
	            default :
	                break;
	        }
		}
    }
}
// mode: 0-需要密码校验 1-不需要密码校验
int local_menu_operate(int mode)
{
    const menu_unit_t  t_menu[] = {
        {"查看版本",     "Show Version",  -1, 0, show_local_ver},
        {"本地下载",     "Local Download", -1, 0,dl_process},
        {"机器信息",     "Machine Info",  -1, 0, user_manage_interface},
        {"系统参数",     "System Set",    -1, 0, menu_setpara},
        {"修改菜单密码", "Modify PassWord",  -1, 0, menu_password_modify},
        //        {"工具",         "Tools",         -1, 0, menu_toolset},
        {"系统时间",     "Set Time",      -1, 0, set_local_time},
#ifdef CFG_FACTORY
//        {"生产测试",     "Fac Test",      -1, 0, fac_main},
        {"单项测试",     "Single Test",          -1, 0, local_test_menu}
#endif
    };
    char iRet = 0xFF;
    const char * const c_menu_name = "主菜单";
    const char * const e_menu_name = "Main Menu";
    char retryTimes = 3;

    TRACE("%s\r\n",__func__);
    if(mode == 1)
    {
        goto PASSWORD_CHECK_OK;
    }
    kb_flush();
    if (sizeof(SYSZONE_DEF) != FLASH_SECTOR_SIZE)
    {
        lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "Loading%d...",sizeof(SYSZONE_DEF));
    } else
    {
        lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "Loading...");
    }
    if (drv_kb_ifprocessing() == 1)
    {
        iRet = kb_getkey(3000);
    } else
    {
        if (power_ifcharging() == 0)
        {
            iRet = kb_getkey(500);
        }else
        {
            iRet = kb_getkey(1500);
        }
    }
    if(iRet != KEY_F1)
    {
        return iRet;
    }
    while (1)
    {
        iRet = menu_password_verify(6);
//        iRet = OK;
        switch (iRet)
        {
        case KB_CANCEL:
            return iRet;
        case OK:
            goto PASSWORD_CHECK_OK;
            break;
        default:
            retryTimes--;
            if (retryTimes == 0)
            {
                return iRet;
            }
            break;
        }
    }
PASSWORD_CHECK_OK:
    return select_menu(1,(char *)c_menu_name, (char *)e_menu_name, 1, (menu_unit_t * )t_menu, DIM(t_menu), -1);
}
#endif
