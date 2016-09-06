/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : misc.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 10/7/2014 4:21:49 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
#if PRODUCT_NAME == PRODUCT_MPOS
#pragma message("|---->|^_^|->S1000<-|^_^|<----|")
#elif PRODUCT_NAME == PRODUCT_EPP 
#pragma message("|---->|^_^|->MK-210CTRL<-|^_^|<----|")
#endif
#ifdef DEBUG_Dx
#pragma message("|---->|^_^|->DebugOpen<-|^_^|<----|")
#endif
#if (CLK0_FREQ_HZ == 12000000)
#pragma message("|---->|>_<|->Crystal_12M<-|>_<|<----|")
#endif
#if (CLK0_FREQ_HZ == 8000000)
#pragma message("|---->|>_<|->Crystal_12M<-|>_<|<----|")
#endif

/*-----------------------------------------------------------------------------}
 *  全局变量
 *-----------------------------------------------------------------------------{*/
static int gManager = USER_NORMAL;

#if PRODUCT_NAME == PRODUCT_MPOS
const VERSION_INFO_NEW gCtrlVerInfo={
	"S1000_CTRL",
	S1000_CTRL_VER,
	__DATE__       //12B
};
// 子产品名
const char *const k_SonTermName = "S1000";
#elif PRODUCT_NAME == PRODUCT_EPP
const VERSION_INFO_NEW gCtrlVerInfo={
	"MK-210 CTRL",
	MK210_APP_VER,
	__DATE__       //12B
};
const char *const k_SonTermName = "MK-210";
#endif

/*--------------------------beep module---------------------------------- */
void beep_run(void)
{
    //根据pcb版本默认开启不同频率的蜂鸣器
//        drv_beep_open(BEEP_PWM_4KHZ);
    drv_beep_open(BEEP_PWM_HZ);
    drv_beep_start();
}


void beep_kb_nonblock(void)
{
    if (gwp30SysMemory.SysCtrl.bit.keytone)
    {
        beep_run();
        gwp30SysMemory.BeepTimerCnt10ms = BEEP_PWM_TIMERCNT_KB;
        gwp30SysMemory.SysTickDeamon.bit.beep = 1;
    }
    if (gLcdSys.SysOperate.bit.enable)
    {
        gLcdSys.LcdBackLightTimerCnt10ms = gLcdSys.LcdBackLightTimerOut10ms;
        if (gLcdSys.LcdBackLightMode != LCD_MODE_ALWAYSOFF) {
            hw_lcd_backlight(LCD_BACKLIGHT_ON);
        }
    }
}

void beep_powerswitch_nonblock(uint8_t time_10ms)
{
    beep_run();
    gwp30SysMemory.BeepTimerCnt10ms = time_10ms;
    gwp30SysMemory.SysTickDeamon.bit.beep = 1;
}

/*--------------------------power keep module---------------------------------- */
int sys_get_version_adc(void)
{
    int val;
    drv_adc_open(SYS_VERSION_ADC_CTRLR,ADC_SC2_TRIGER_SOFTWARE,ADC_SC1n_SingleEnded,
                 ADC_SC3_HARDWARE_AVERAGE_ENABLE|ADC_SC3_HARDWARE_AVERAGE_32);
    val = drv_adc_caculate(SYS_VERSION_ADC_CHANEL);
    drv_adc_close(SYS_VERSION_ADC_CTRLR);
    return val;
}

static void power_charge_update(void)
{
    if (power_ifcharging() == 1)
    {  //charging
//        auto_poweroff_update_time(0);
        auto_poweroff_update_time(gwp30SysMemory.PowerOffTimerTotalTm);
    } else 
    { //using battery
        auto_poweroff_update_time(gwp30SysMemory.PowerOffTimerTotalTm);
    }
}

void power_charge_init(void)
{
    GPIO_InitTypeDef gpio_init;

    gwp30SysMemory.SysTickDeamon.bit.charge = 0; //stop the systick charge demon
    gpio_init.GPIOx = POWER_CHARGE_GPIO;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PDIS 
        |PORT_Mode_IN_PFE|PORT_Mode_IRQ_EXTI_EEdge;
    gpio_init.PORT_Pin = POWER_CHARGE_PINx;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(POWER_CHARGE_PINx);
    hw_gpio_init(&gpio_init);
    PORTx_IRQPinx_Clear(POWER_CHARGE_PORT,POWER_CHARGE_PINx);
    enable_irq(PORTB_IRQn);		

    power_charge_update();
    gwp30SysMemory.SysTickDeamon.bit.charge = 1; //run the systick power demon
}


void PORTB_PowerChargeNotice_IRQHandler(void)
{
    if (PORTx_IRQPinx(POWER_CHARGE_PORT,POWER_CHARGE_PINx)) {
        power_charge_update();
        PORTx_IRQPinx_Clear(POWER_CHARGE_PORT,POWER_CHARGE_PINx);
    }
}

void power_module_init(void)
{
    power_keep_init();//frist
    power_charge_adc_switch_init();
    power_charge_full_init();
    power_charge_init();//last
}

int read_user_flag(void)
{
    return gManager;
}
int set_user_flag(int mode)
{
    gManager = mode;
    return 0;
}
int user_manage(int mode)
{
    int user;
    int key = 0;
    int len,ret;
    uchar psw[SYS_SUPER_PSW_SIZE];
    uchar passWordStr[32];

//    int result;
//    password_t password;
    lcd_cls();
    lcd_display(0,DISP_FONT_LINE0,DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY,"用户管理");
    user = read_user_flag();
    if ( user == USER_NORMAL )
    {
        lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"当前用户:普通");
    }
    else if(user == USER_MANAGE)
    {
        lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"当前用户:管理员");
    }
    else if(user == USER_SUPER)
    {
        lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"当前用户:超级管理员");
    }
    lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"[1]-切换");
    while(1)
    {
        key = kb_getkey(-1);
        if ( key == '1') {
            break;
        } else {
            goto FAIL;
        }
    }
   lcd_cls();
   lcd_display(0,DISP_FONT_LINE0,DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY,"用户管理");
   lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"请选择需要切换的用户:");
   lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"1-普通");
//   lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"2-管理员");
   lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"2-超级管理员");
    while(1)
    {
        key = kb_getkey(-1);
        switch(key)
        {
        case KEY1:
            user = USER_NORMAL;
            goto PASS_CHECK;
        case KEY2:
            user = USER_SUPER;
            goto PASS_CHECK;
//        case KEY3:
//        case KEY_CANCEL:
//            goto FAIL;
        default:
            goto FAIL;
        }
    }
PASS_CHECK:
//    menu_password_verify(12);

    CLRBUF(psw);
    read_syspsw(PSW_SYS, (uint*)&len, psw);
    lcd_cls();
    lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE, "请输入密码", "PASSWORD:");
    lcd_goto(0,DISP_FONT*2);
    CLRBUF(passWordStr);
    ret = kb_getstr(KB_BIG_PWD, 0, SYS_SUPER_PSW_SIZE, -1, (char*)passWordStr);
    if(ret < 0)
    {
        if (ret == -KB_CANCEL)
        {
            goto FAIL;
        }
        goto FAIL;
    }
    if (0 != memcmp(passWordStr,psw,SYS_SUPER_PSW_SIZE))
    {
        lcdDispMultiLang(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "密码错误!", "PASSWORD ERROR!");
        sys_delay_ms(1000);
        goto FAIL;
    }
    set_user_flag(user);

    lcd_cls();
//    lcd_clrline(DISP_FONT,LCD_HEIGHT-1);
    lcd_display(0,DISP_FONT_LINE0,DISP_FONT|DISP_CLRLINE|DISP_INVLINE|DISP_MEDIACY,"用户管理");
    user = read_user_flag();
    lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"当前用户: (%d)",user);
    if ( user == 0 )
    {
        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"普通");
    }
    else if(user == 1)
    {
        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"管理员");
    }
    else if(user == 2)
    {
        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"超级管理员");
    }
    s_DelayMs(1000);
    return OK;
FAIL:
    return ERROR;
}

void s_sys_error(uchar *ptr)
{
	lcd_cls();
	lcd_display(0, 12, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,
				"System Error!!!");
	lcd_display(0, 24, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,
				"%s",ptr);
    kb_flush();
	while(1)
	{
		sys_beep();
        lcd_SetLight(LIGHT_ON_MODE);
		s_DelayUs(300000);
		lcd_SetLight(LIGHT_OFF_MODE);
		if(kb_hit()){
            if ( kb_getkey(-1) == KEY_CANCEL ) {
                break;
            }
		}
		s_DelayUs(300000);
	}
}

#define  GET_ADC_NUM              50 //AD采用总次数
#define  MAX_MIN_NUM              15 //过滤最大最小次数
// WP70版本识别误差位为3位,需要进行较为准确的采样值,此处采用采样50次,分别去除最大最小15位处理
int s_ReadVersion_Adc(int adc_no,int *pAdcValue)
{
    int adc[GET_ADC_NUM];
    int adc_total = 0;
//    int adc_value= 0;
    int i = 0,j = 0;
    int temp = 0;

    CLRBUF(adc);

    for ( i = 0 ; i < GET_ADC_NUM; i++)
    {
        adc[i] = sys_get_version_adc();
        s_DelayMs(1);
    }
    for(i = 0 ; i< GET_ADC_NUM; ++i)
    {
        for(j = 0; j < GET_ADC_NUM-i-1; ++j)
        {
            if(adc[j] > adc[j+1])
            {
                 temp =  adc[j] ;
                  adc[j] =  adc[j+1] ;
                  adc[j+1] = temp;
            }
        }
    }
#ifdef DEBUG_Dx
    for(i=0;i< GET_ADC_NUM;i++)
    {
        TRACE("(%x) ",adc[i]);
    }
    TRACE("\r\n");
#endif
    for(i=MAX_MIN_NUM;i<GET_ADC_NUM-MAX_MIN_NUM;i++)
    {
        adc_total +=  adc[i];
    }
    *pAdcValue = adc_total/(GET_ADC_NUM-2*MAX_MIN_NUM);
    return OK;
}

#define S_PCB_BITNUM     8 
#define S_MODULE_BITNUM  11
//输出参数已是pcb版本号和模块配置
int s_getAdc_hardversion(int *ADCpcb, int *ADCmodule)
{
    int adcVaule = 0;
    s_ReadVersion_Adc(0, &adcVaule);
    *ADCpcb = (adcVaule>>S_PCB_BITNUM)&0x07;
    *ADCmodule = (adcVaule>>S_MODULE_BITNUM)&0x1F;
    TRACE("\r\n pcb:%x module:%x \r\n",*ADCpcb,*ADCmodule);
    return OK;
}

//根据adc采样值列表查询是否存在该模块或模块类型
//输入参数是pcb版本号和模块配置部分
int s_getModuleType_list(int pcb, int adcVaule, module_type_t * list)
{
#if PRODUCT_NAME == PRODUCT_MPOS 
    switch(adcVaule)
    {
    case 0:
        /*
         *蓝牙：BM77   
         *RFID：不支持 
         *ICC:  74AHCT00
         *PSAM: 74AHCT0
         *磁卡：magtek
         */
        list->bluetooth = 0x01;
        list->rfid = 0x00;
        list->magcard = 0x03;//对应磁卡 EM_MAGTEK;
        list->iccard = 0x01; //ic卡IO反向
        break;
    case 1:
        /*
         *蓝牙：BM77  
         *RFID：不支持  
         *ICC:  直连
         *PSAM: 直连 
         *磁卡：HCM4003  
         */
        list->bluetooth = 0x01;
        list->rfid = 0x00;
        list->magcard = 0x04;//对应磁卡 EM_HCM4003;
        list->iccard = 0x02; //ic卡cpu直连
        break;
    case 16:
        /*
         *蓝牙：BM77    
         *RFID：AS3911  
         *ICC:  74AHCT00 
         *PSAM: 74AHCT00
         *磁卡：magtek 
         */
        list->bluetooth = 0x01;
        list->rfid = 0x01;
        list->magcard = 0x03;//对应磁卡 EM_MAGTEK;
        list->iccard = 0x01;
        break;
    default:
        //失败配置则全0
        return -1;
    }
#endif

#if PRODUCT_NAME == PRODUCT_EPP
    switch(adcVaule)
    {
    case 1:
        /*
         *蓝牙：不支持
         *RFID：不支持  
         *ICC:  74AHCT00 
         *PSAM: 74AHCT00
         *磁卡：不支持
         */
        list->bluetooth = 0x00;
        list->rfid = 0x00;
        list->magcard = 0x00;
        list->iccard = 0x01; //ic卡IO反向
        break;
    case 2:
        /*
         *蓝牙：不支持
         *RFID：不支持  
         *ICC:  直连
         *PSAM: 直连
         *磁卡：不支持
         */
        list->bluetooth = 0x00;
        list->rfid = 0x00;
        list->magcard = 0x00;
        list->iccard = 0x02; //ic卡cpu直连
        break;
    default:
        //失败配置则全0
        return -1;
    }
#endif
    return OK;
}

//根据模块采样adc得到moduletype初始化
int s_sysinfo_rfid_para_init(int moduletype, MODULE_RFID_INFO *t)
{
#ifdef CFG_RFID
    //1-有射频模块
    t->type = (uchar)moduletype;
    t->rfid_powerfield_time = RFID_DEFAULT_PWRSETTIME;
    t->field_strength = RFID_DEFAULT_PWR;
#endif
    return 0;
}		/* -----  end of function syszone_rfid_para_init  ----- */

int s_sysinfo_bt_para_init(int moduletype, MODULE_BLUET_INFO *t)
{
    //1-有蓝牙模块
    t->bluetooth = (uchar)moduletype;
    return 0;
}		/* -----  end of function syszone_rfid_para_init  ----- */

int s_sysinfo_mag_para_init(int moduletype, MODULE_MAGCARD_INFO *t)
{
    //磁卡模块
    t->type = (uchar)moduletype;
    return 0;
}		/* -----  end of function syszone_rfid_para_init  ----- */

int s_sysinfo_iccard_para_init(int moduletype, MODULE_ICCARD_INFO *t)
{
    //CPU卡模块
    t->type = (uchar)moduletype;
    return 0;
}		/* -----  end of function syszone_rfid_para_init  ----- */

//清硬件配置重新AD采样获取硬件
int32_t sys_check_version(int mode)
{
//    int ADCpcb = 0,ADCmodule = 0;
    module_type_t list;

    memset((uchar *)&list, 0x00, sizeof(module_type_t));
//    s_getAdc_hardversion(&ADCpcb, &ADCmodule);
//    s_getModuleType_list(ADCpcb, ADCmodule, &list);

    //k21f临时模块配置
    list.bluetooth = 0x00;
    list.rfid = 0x02;
    list.magcard = 0x03;//对应磁卡 EM_MAGTEK;
    list.iccard = 0x02; //ic卡IO反向

    //pcb主板版本号
    hardware_verion_t mainversion;
    mainversion.flag = 0;
#if PRODUCT_NAME == PRODUCT_MPOS 
    mainversion.main = 0x01;
#else
    mainversion.main = 0x03;
#endif
    mainversion.secondary = 0x00;
//    mainversion.revise = (uchar)ADCpcb;
    mainversion.revise = 0; //k21f临时配置
    s_sysinfo_mbVersioninfo(1, &mainversion);

#ifdef CFG_RFID
    //rfid
    MODULE_RFID_INFO rfidinfo;
    memset((uchar*)&rfidinfo, 0xFF, sizeof(MODULE_RFID_INFO));
    s_sysinfo_rfid_para_init(list.rfid, &rfidinfo);
//    rfidinfo.type = 1;
    s_sysinfo_rfidinfo(1, &rfidinfo);
#endif

#ifdef CFG_BLUETOOTH
    //bluetooth
    MODULE_BLUET_INFO bluetoothinfo;
    memset((uchar*)&bluetoothinfo, 0xFF, sizeof(MODULE_BLUET_INFO));
    s_sysinfo_bt_para_init(list.bluetooth, &bluetoothinfo);
    s_sysinfo_bluetoothinfo (1, &bluetoothinfo);
#endif

#ifdef CFG_MAGCARD
    MODULE_MAGCARD_INFO tmagcardinfo;
    memset((uchar*)&tmagcardinfo, 0xFF, sizeof(tmagcardinfo));
    s_sysinfo_mag_para_init(list.magcard, &tmagcardinfo);
    s_sysinfo_magcardinfo (1, &tmagcardinfo);
#endif

#ifdef CFG_ICCARD 
    MODULE_ICCARD_INFO ticcardinfo;
    memset((uchar*)&ticcardinfo, 0xFF, sizeof(ticcardinfo));
    s_sysinfo_iccard_para_init(list.iccard, &ticcardinfo);
    s_sysinfo_iccardinfo (1, &ticcardinfo);
#endif

    // sysinfo
    MODULE_SYS_INFO sysinfo;
    memset((uchar*)&sysinfo, 0xFF, sizeof(MODULE_SYS_INFO));
    sysinfo.language = 1; 
    sysinfo.lcdcontrast = 0;
    sysinfo.lcdlighttime = 0;
    sysinfo.keytone = 1;
    memset(&sysinfo.menupassword,0,16);
    memcpy(&sysinfo.menupassword,DEFAULT_MENU_PASSWORD,6);
    s_sysinfo_sysinfo(1,&sysinfo);

    return 0;
}
int backdoor_password_verify(char *psw)
{
    int     ret;
	unsigned char password[16] = {0};
    char    passWordStr[16];

    lcd_Cls();
    lcdDispMultiLang(0,  0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE, "请输入密码", "PASSWORD:");
    kb_Flush();

	memcpy(password,psw,6);
    memset(passWordStr, 0, sizeof(passWordStr));

    lcd_Goto(0, 12 * 2);
    ret = kb_GetStr(KB_BIG_PWD, 0, 6, -1, passWordStr);
    if(ret < 0)
    {
        if (ret == -KB_CANCEL)
        {
            goto CANCEL;
        }
        goto FAIL;
    }

    if (0 != memcmp(passWordStr, password, sizeof(password)))
    {
        lcdDispMultiLang(0,  DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "密码错误!", "PASSWORD ERROR!");
        sys_DelayMs(1000);
        goto FAIL;
    }

//SUCCESS:
    return OK;
FAIL:
    return ERROR;
CANCEL:
    return KB_CANCEL;
}

int set_attack_menu(int mode)
{
#ifdef CFG_TAMPER       
#if PRODUCT_NAME == PRODUCT_MPOS
    return OK;
#else
    int ret=0;

    kb_flush();
    lcd_cls();
    while (1)
    {
        lcd_Cls();
        lcd_display(0,0,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,"防拆状态:");
        ret = Fac_SRAM(0);
        if ( ret == OK  ) 
        {
            lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"正常");
        }
        else
        {
            lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"异常");
        }
        if ( read_user_flag() >= USER_MANAGE) 
            lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"1-重置防拆");
        switch(kb_getkey(-1))
        {
        case KEY1:
            if ( read_user_flag() >= USER_MANAGE) {
                ret = Fac_SRAM(1);
                //        lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"防拆状态:");
                if ( ret == OK ) {
                    lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"防拆重置成功");
                }
                else
                {
                    lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"防拆重置失败");
                }
                kb_getkey(-1);
            }
            break;
        case KEY_CANCEL:
            return OK;
        default:
            break;

        }
    }
#endif
#else
    return OK;
#endif
}

int sys_get_version(int mode)
{
//    int ret;
//    ret = backdoor_password_verify("456789");
//    if ( ret ) {
//        return ERROR;
//    }
    lcd_cls();
    lcd_display(0, DISP_FONT_LINE0, DISP_FONT | DISP_CLRLINE | DISP_MEDIACY | DISP_INVLINE, "版本重新识别");
    lcd_display(0, DISP_FONT_LINE1, DISP_FONT | DISP_CLRLINE , "1-确认");
    if ( kb_getkey(-1) == KEY1 ) {
        sys_check_version(0);
        lcd_display(0, DISP_FONT_LINE1, DISP_FONT | DISP_CLRLINE , "识别成功");
    }else{
        lcd_display(0, DISP_FONT_LINE1, DISP_FONT | DISP_CLRLINE , "取消识别");
    }
    kb_getkey(2000);
//    lcd_clrline(DISP_FONT_LINE0,LCD_HEIGHT-1);
//    lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"正在获取");
//    sys_CheckModule(1);
//    lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"识别成功");
//    s_DelayMs(2000);
    return OK;
}

int sys_do_set_hardinfo(int module)
{
    typedef struct
    {
        int module;
        char name[32];
        int  offset;
    }module_info_t;

    typedef struct
    {
        int module_type;
        char name[32];
    }module_name_t;

    int i=0,j=0;
   // module_type_t module_type;
    int type_value,module_value = 0;
    //int modify_value;
    int key,len,user;
    const uchar BTmodule[] = {0,1,2};
    const uchar RFmodule[] = {0,1,2};
    const uchar Magmodule[] = {0,3,4};
    const uchar ICmodule[] = {0,1,2};
    const uchar nomodule[] = {0,0,0};
    uchar *ptr = NULL;

    ptr = (uchar *)nomodule;
    module_info_t tModuleInfo[] =
    {
//        {MODULE_GPRS_TYPE,"GPRS",FPOS(module_type_t,gprs)},
//        {MODULE_CDMA_TYPE,"CDMA",FPOS(module_type_t,cdma)},
//        {MODULE_MODEM_TYPE,"Modem",FPOS(module_type_t,modem)},
        {MODULE_BLUETOOTH_TYPE,"蓝牙",0},
        {MODULE_RF_TYPE,"射频",0},
        {MODULE_MAGNETIC_TYPE,"磁卡",0},
        {MODULE_MODE_ICCARD,"CPU卡",0},
//        {MODULE_LCD_TYPE,"液晶",FPOS(module_type_t,lcd)},
//        {MODULE_PRINT_TYPE,"打印",FPOS(module_type_t,printer)},
    };
    const module_name_t tModuleName[] =
    {
//        {MODULE_GPRS_MG323,"MG323"},
//        {MODULE_CDMA_MC323,"MC323"},
//        {MODULE_MODEM_CX_MODEM,"CX Modem"},
        {MODULE_BLUETOOTH_BM77SPP,"BM77SPP05"},
//        {MODULE_BLUETOOTH_BLK,"BT_BLK"},
        {MODULE_RF_AS3911,"AS3911"},
        {MODULE_RF_PN512_BASE,"PN512"},
        {MODULE_RF_FM17550,"FM17550"},
        {MODULE_MAGNETIC_MAGTEK,"MAGTEK"},
        {MODULE_MAGNETIC_HCM4003,"HCM4003"},
        {MODULE_CPUCARD_IOREV,"IOREV"},
        {MODULE_CPUCARD_IO,"IO"},
        {MODULE_CPUCARD_8035,"8035"},
//        {MODULE_MAGNETIC_IDTECH,"IDTECH"},
//        {MODULE_PRINT_FUJITSU,"FUJITSU"},
//        {MODULE_PRINT_SEIKO,"SEIKO"},
    };
//    ReadHardWareModuleType(&module_type);
//    ReadHardWareModuleType((module_type_t *)nValue);
    for(i=0;i<DIM(tModuleInfo);i++)
    {
        if ( tModuleInfo[i].module == module)
        {
            break;
        }
    }
    if ( i == DIM(tModuleInfo))
    {
        return -1;
    }
    type_value = sys_get_module_type(module);
//    type_value = nValue[tModuleInfo[i].offset / 4];
    lcd_cls();
    lcdDispMultiLang(0, DISP_FONT_LINE0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "硬件信息:%x", "Hardware Info:%x", type_value);
//    lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_MEDIACY, "%s", tModuleInfo[i].name);
    if ( type_value == MODULE_NOTEXIST)
    {
        lcd_display(0, DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"当前配置:无");
    }else{
        for(j=0;j<DIM(tModuleName);j++)
        {
            if ( tModuleName[j].module_type == type_value)
            {
                break;
            }
        }
        if ( j == DIM(tModuleName))
        {
            lcd_display(0, DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"%s:未知:0x%4x",tModuleInfo[i].name,type_value);
        }
        else
        {
            lcd_display(0, DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"%s:%s",tModuleInfo[i].name,tModuleName[j].name);
        }
    }

    user = read_user_flag();
    if (user < USER_MANAGE) {
        //普通用户按键退出
        kb_getkey(-1);
        return OK;
    }
    lcd_display(0,  DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, "[确认]修改 [取消]退出");
    while(1)
    {
        switch(kb_getkey(-1))
        {
        case KEY_ENTER:
            goto Modify_Version;
        case KEY_CANCEL:
            goto CANCEL;
        default:
            break;
        }
    }
Modify_Version:
//    lcd_clrline(DISP_FONT_LINE2,LCD_HEIGHT-1);
    switch ( module )
    {
    case MODULE_GPRS_TYPE :
//        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"1-无");
//        lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"2-MG323");
        break;
    case MODULE_CDMA_TYPE :
//        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"1-无");
//        lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"2-MC323");
        break;
    case MODULE_MODEM_TYPE :
//        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"1-无");
//        lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"2-CX Modem");
        break;
    case MODULE_BLUETOOTH_TYPE :
        lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"1-无");
        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"2-BM77SPP05");
        lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"3-BLK");
        len = FSIZE(MODULE_BLUET_INFO,bluetooth);
        //系统配置基地址+射频模块类型地址
        tModuleInfo[i].offset = FPOS(SYSZONE_DEF,bluetoothinfo) 
                              + FPOS(MODULE_BLUET_INFO,bluetooth);
        ptr = (uchar *)BTmodule;
        break;
    case MODULE_RF_TYPE :
        lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"1-无");
        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"2-AS3911");
        lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"3-FM17550");

        len = FSIZE(MODULE_RFID_INFO,type);
        //系统配置基地址+射频模块类型地址
        tModuleInfo[i].offset = FPOS(SYSZONE_DEF,trfidinfo) 
                              + FPOS(MODULE_RFID_INFO,type);
        ptr = (uchar *)RFmodule;
        break;
    case MODULE_MAGNETIC_TYPE :
        lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"1-无");
        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"2-MAGTEK");
        lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"3-HCM4003");
        len = FSIZE(MODULE_MAGCARD_INFO,type);
        //系统配置基地址+射频模块类型地址
        tModuleInfo[i].offset = FPOS(SYSZONE_DEF,tmagcardinfo) 
                              + FPOS(MODULE_MAGCARD_INFO,type);
        ptr = (uchar *)Magmodule;
        break;
    case MODULE_PRINT_TYPE :
//        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"1-无");
//        lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"2-FUJITSU");
//        lcd_display(0,DISP_FONT_LINE5,DISP_FONT|DISP_CLRLINE,"3-SEIKO");
        break;
    case MODULE_MODE_ICCARD :
        lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"1-无");
        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"2-IOREV");
        lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"3-IO");
        len = FSIZE(MODULE_ICCARD_INFO,type);
        //系统配置基地址+射频模块类型地址
        tModuleInfo[i].offset = FPOS(SYSZONE_DEF,ticcardinfo) 
                              + FPOS(MODULE_ICCARD_INFO,type);
        ptr = (uchar *)ICmodule;
        break;
    default :
        break;
    }

//    lcd_display(0,DISP_FONT_LINE7,DISP_FONT|DISP_CLRLINE,"[确认]-修改  [取消]-退出");

    while(1)
    {
        key = kb_getkey(-1);
        switch(key)
        {
        case KEY_CANCEL:
            goto CANCEL;
        case KEY2:
            lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"当前选择:2");
            if ( module == tModuleInfo[i].module)
            {
                type_value = 1;
            }
            break;
        case KEY3:
            lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"当前选择:3");
            if ( module == tModuleInfo[i].module)
            {
                type_value = 2;
            }
             break;
        case KEY1:
            lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"当前选择:1");
            if ( module == tModuleInfo[i].module)
            {
                type_value = 0x00;
            }
            break;
        case KEY_ENTER:
            module_value = ptr[type_value];
            key = s_sysconfig_write(tModuleInfo[i].offset,len,&module_value);
            return key;
        default:
            break;
        }
    }
//    return OK;
CANCEL:
    return ERROR;

}

//int sys_set_hardware_info(int para)
//{
//    const menu_unit_t  tHardInfo[] =
//    {
////    		{"GPRS"              , "GPRS"           , -1, MODULE_GPRS_TYPE,     sys_do_set_hardinfo},
////    		{"CDMA"              , "CDMA"           , -1, MODULE_CDMA_TYPE,     sys_do_set_hardinfo},
////    		{"Modem"             , "Modem"          , -1, MODULE_MODEM_TYPE,  sys_do_set_hardinfo},
////    		{"以太网"            , "Ethernet"       , -1, MODULE_LAN_TYPE,  sys_do_set_hardinfo},
//    		{"蓝牙"              , "BT"             , -1, MODULE_BLUETOOTH_TYPE,  sys_do_set_hardinfo},
//            {"射频"              , "RF"             , -1, MODULE_RF_TYPE,        sys_do_set_hardinfo},
////            {"NAND"              , "NAND"           , -1, MODULE_NAND_TYPE,      sys_do_set_hardinfo},
////    		{"扫描"              , "SCANNER"        , -1, MODULE_SCANNER_TYPE,   sys_do_set_hardinfo},
//    		{"磁卡"              , "MAGCARD"        , -1, MODULE_MAGNETIC_TYPE,  sys_do_set_hardinfo},
//    		{"CPU卡"             , "CPUCARD"        , -1, MODULE_MODE_ICCARD,  sys_do_set_hardinfo},
////    		{"液晶"              , "LCD"            , -1, MODULE_LCD_TYPE,       sys_do_set_hardinfo},
////    		{"打印"              , "PRINT"            , -1, MODULE_PRINT_TYPE,       sys_do_set_hardinfo},
//    };
//
////    int ret;
////    ret = backdoor_password_verify("456789");
////    if ( ret ) {
////        return ERROR;
////    }
//    select_menu(1, "设置硬件信息", "HARDWARE INFO", 1, (menu_unit_t * )tHardInfo, DIM(tHardInfo), -1);
//    return 0;
//}

int sys_format_syszone(int mode)
{
//    extern int32_t  s_sysconfig_format();
    int ret = OK;
//    lcd_cls();
//    lcd_display(0,0,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,"恢复出厂");
//    ret = s_sysconfig_format();
    ret = backdoor_password_verify("456789");
    if ( ret ) {
        return ERROR;
    }
    write_syspsw(0,8,DEFAULT_SUPERPSW_SYS);
    sys_check_version(0);
    sys_beep();
    lcd_cls();
    lcd_display(0,0,DISP_FONT|DISP_CLRLINE,"恢复成功");
    kb_getkey(2000);
    return ret;
}

// Ctrl first run
// 0-first run   1-not first run  2-write flash err
// 如果系统信息区在结构体中改变原某个字段大小（末尾添加无影响），
// 需要修改运行标志，重新初始化，否则将造成某些模块无法正常运行
#define FIRST_RUN_FLAG 2  
uint ctrl_first_run(void)
{
//    uchar backbuf[SYSZONE_LEN];
    uchar *backbuf;
    uint i;
	s_read_syszone(OFFSET_FIRST_FLAG,LEN_FIRST_FLAG,(uchar *)&i);
	if(i == FIRST_RUN_FLAG)
	{
		return 1;
	}
    backbuf = malloc(SYSZONE_LEN);
    if ( backbuf == NULL ) {
        return 2;
    }
//    i = 0;
//    s_write_syszone(backbuf,OFFSET_FIRST_FLAG,LEN_FIRST_FLAG,(uchar *)&i);
    write_syspsw(0,8,DEFAULT_SUPERPSW_SYS);

    sys_check_version(0);

    // 清空生产测试信息区
#ifdef CFG_FACTORY
    FAC_TESTINFO  facinfo;
    memset((uchar*)&facinfo,0,sizeof(FAC_TESTINFO));
    fac_ctrl_set_fac_testinfo(&facinfo);
#endif
    //初始化完写入监控初次运行标识
    i = FIRST_RUN_FLAG;
    s_write_syszone(backbuf,OFFSET_FIRST_FLAG,LEN_FIRST_FLAG,(uchar *)&i);

    free(backbuf);

	return 0;
}

/**********************************************************************
*
***********************************************************************/
uint sys_start(void)
{
	uint i,j;
    uchar buf[SYSZONE_LEN];
	if(!ctrl_first_run())
	{
        DISPPOS(0);
//        TRACE("\n\n---first run");
		return OK;
	}
	if(check_syszone(0,buf))
	{
		// syszone is err
//        TRACE("\n\n---sys err");
		if(check_syszone(1,buf))
		{
			// two syszone is err
//            TRACE("\n\n---sysback err");
			return ERROR;
		}
		else
		{
			// recover syszone
//            TRACE("\n\n---sysback recover");
			readFlashPage(SA_SYSZONE_BACK/2, buf, SYSZONE_LEN);
			return resume_syszone(buf);
		}
	}
	else
	{
		// syszone is ok , buf is syszone data
		// read syszone check
		memcpy((uchar *)&i,&buf[OFFSET_SYSZONE_CHECK],LEN_SYSZONE_CHECK);
		// read syszoneback check
		readFlashPage((SA_SYSZONE_BACK+OFFSET_SYSZONE_CHECK)/2, (uchar *)&j, 4);
		if(i == j)
		{
//            TRACE("\n\n---crc sam");
			if(check_syszone(1,buf) == 0)
			{
				// syszoneback is OK
//                TRACE("\n\n---sys check ok");
				return OK;
			}
		}
		//备份信息区错误或不同步,则重新备份区
//        TRACE("\n\n---sys back err recover");
		return save_syszone_back(buf);
	}
}

int get_hardware_version(void)
{
    hardware_verion_t mainversion;
    memset((uchar *)&mainversion, 0, sizeof(hardware_verion_t));
    s_sysinfo_mbVersioninfo(0, &mainversion);

    k_MainVer.main = mainversion.main;
    k_MainVer.secondary = mainversion.secondary;
    k_MainVer.revise = mainversion.revise;
    return OK;
}

// mode: 0- PCB版本(AD采样获得)
//       1- MB主板版本号 (软件固化)
//int sys_read_mb_pcb_version(int mode)
//{
//    if ( mode == 0 ) 
//    {
//        return k_MainVer.revise;
//    }
//    else if (mode == 1)
//    {
//        return k_MainVer.main;
//    }
//    return -1;
//}

// 获取机器类型  0-WP-30 1-MK-210
int s_getProduct(void)
{
    return STAR_S1000V100; //s1000 无版本识别直接返回 涉及蜂鸣器 ic卡模块 06132016 chenf
//#if PRODUCT_NAME == PRODUCT_EPP 
//    if ( k_MainVer.revise == 0) {
//        return STAR_MK210V300;
//    } else if( k_MainVer.revise == 1){
//        return STAR_MK210V301;
//    }  
//#else
//    if ( k_MainVer.revise == 0) {
//        return STAR_WP30V100;
//    } else if( k_MainVer.revise == 1){
//        return STAR_WP30V101;
//    }  
//#endif
//    return -1;
}

int s_setProduct(uchar type)
{
    return 0;
}
int s_InitProduct(void)
{
    return 0;
}

void wp30_sys_ctrl_init(void)
{
    MODULE_SYS_INFO tsysinfo;
    s_sysinfo_sysinfo(0,&tsysinfo);
    //菜单语言
    gwp30SysMemory.SysCtrl.bit.language = tsysinfo.language;
    //按键伴音
    gwp30SysMemory.SysCtrl.bit.keytone = tsysinfo.keytone;
    //生产测试完成表标识
    if(fac_ctrl_get_fac_ctrl_flag() == 1 )
    {
        gwp30SysMemory.SysCtrl.bit.factoryisok = 1;
    }
    else
    {
        gwp30SysMemory.SysCtrl.bit.factoryisok = 0;
    }
    s_InitProduct();
} 

int s_lcdgetMenuLang(void)
{
    return gwp30SysMemory.SysCtrl.bit.language;
}
/*--------------------------lcd module---------------------------------- */
void lcd_init(void)
{
    drv_lcd_init(LCD_MODE_TIMEROUT);
}

//=========================================
//#if defined(CFG_SOFTPWRON)
#if 2 //defined(CFG_SOFTPWRON)
static int system_ifpwron(uint32_t start_time,uint32_t timeout_ms)
{
    int iRet=NO;
    uint16_t hight=0,low=0;

    gpio_set_input(KB_PTx_INDEPENDENT0,GPIO_INPUT_FILTERENABLE|GPIO_INPUT_PULLDISABLE);
    if (gpio_get_bit(KB_PTx_INDEPENDENT0))
    {
        while (1)
        {
//            enter_lowerpower_wait();
            if (gpio_get_bit(KB_PTx_INDEPENDENT0))
            {
                hight++;
                if (sys_get_counter() - start_time > timeout_ms) {
                    iRet = YES;    
                    TRACE("\r\nYES PWRON");
                    break;
                }
            }
            else
            {
                if (++low > 4) {
                    iRet = NO;    
                    TRACE("\r\nNO PWRON");
                    break;
                }
            }
        }
    } 
    return iRet;
}

void sys_poweron_charge(void)
{
    uint32_t start_time = sys_get_counter();
    uint32_t current_time = 0;
    uint8_t bitmap = 1;

#if  defined(CFG_LCD)
    lcd_init();
    lcd_display(0, 2*DISP_FONT, DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"正在充电...");
#endif
    while (1)
    {
//        enter_lowerpower_wait();
        current_time = sys_get_counter();
        if (current_time - start_time > 1200)
        {
            bitmap <<= 1;
            if ( (bitmap& (uint8_t)(~LED_ALL)) != 0) {
                bitmap = 1;
            }
            if (power_charge_ifFull()) {
                bitmap = LED_ALL;
#if  defined(CFG_LCD)
                lcd_display(0, 2*DISP_FONT, DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"充电完成");
#endif
            } else {
#if  defined(CFG_LCD)
                lcd_display(0, 2*DISP_FONT, DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"正在充电...");
#endif
            }
            hw_led_on(bitmap);
            start_time = current_time;
        } else if (current_time - start_time > 600)
        {
#if  defined(CFG_LCD)
            lcd_cls();
#endif
            hw_led_off(LED_ALL);
        }
        if (power_ifcharging() == 0)  //适配器拨出
        {
            TRACE("\n-|pwrdown");
#if  defined(CFG_LCD)
            lcd_cls();
#endif
            power_keep_init();//frist
            power_keep_down();
            s_DelayMs(5000);
        }
        if (system_ifpwron(current_time,300) == YES)
        {
            TRACE("\n-|pwron..");
            break;
        }
    }
}

void sys_poweron_ifvalid(uint32_t pretime,uint32_t timeout_ms)
{

//    if (RCM_SRS1 & RCM_SRS1_SW_MASK)
//    {
//        TRACE("\n-|SoftReset");
//        return;
//    }
    if (system_ifpwron(pretime,timeout_ms) == NO)
    {
        if (power_ifcharging() == 1)  //charging
        {
            TRACE("\n-|charning");
            //todo:charging,auto start
            sys_poweron_charge();
        } else //using battery
        {
            TRACE("\n-|pwrdown");
            power_keep_init();//frist
            power_keep_down();
            s_DelayMs(15000);
        }
    }
    TRACE("\n-|pwron");
}
#endif

extern void MakeKeyCheck(uchar *in,uint len,uchar *check); 
int sys_tamper_init(uint32_t mode)
{
//    int flag = 0;
//    uchar *backbuf;
//    // 临时使用标志位
//    s_read_syszone(OFFSET_APPUPDATE_FLAG, 4, (uchar *)&flag);
//    if ( flag != SYSZONE_APPUPDATE_FLAG) {
//        backbuf = malloc(FLASH_SECTOR_SIZE);
//        if ( backbuf ) {
//            flag = SYSZONE_APPUPDATE_FLAG;
//            s_write_syszone(backbuf,OFFSET_APPUPDATE_FLAG, 4, (uchar *)&flag);
//            free(backbuf);
//        }
//        drv_dryice_init((hal_dry_tamper_t)(DRY_TER_TPE(0x3F)|kDryTamper_Voltage|kDryTamper_Clock),
//                    (hal_dry_interrupt_t)0,
//                    0x2C,ENABLE);
//        flag = YES;
//    }else
//    {
//        flag = NO;
//    }
#if defined(CFG_TAMPER) 
    ST_MMAP_KEY ramkey; 

    TRACE("\n-|pinpad init:%02X",mode);
    drv_dryice_init((hal_dry_tamper_t)mode,
                    (hal_dry_interrupt_t)0,
                    0x2C,ENABLE);
    RNG_FillRandom((uint8_t *)&ramkey, MMAP_KEY_LEN);
    MakeKeyCheck((uchar *)&ramkey,MMAP_KEY_LEN,ramkey.check);
    memcpy(ramkey.facchk,STR_SRAM,FUN_LEN);
    TRACE_BUF("dryice mmk",ramkey.aucMMK,32);
    hw_dryice_write_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),
                        ((MK_UNIT_LEN+ELRCK_LEN+CHECK_LEN+FUN_LEN)>>2),
                        (uint32_t *)&ramkey);
#endif
    return 0;
}

int sys_tamper_open(uint32_t mode)
{
//#if PRODUCT_NAME == PRODUCT_MPOS
//    //wp30
//    mode = (1<<0);//tamper0
//#else
//    //mk210
//    mode = (1<<1);//tamper1
//#endif
    return sys_tamper_init(mode);
}


void pinpad_open(uint32_t mode)
{
    sys_tamper_open(mode);
//#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
//    uint8_t length;
//    uint8_t tmp[32];
//
//    sys_tamper_open(mode);
//    ksr_read_mmk(&length,gMKey.aucMMK);
//    DISPBUF("MMK",32,0,gMKey.aucMMK);
//    ksr_read_elk(&length,gMKey.aucELRCK);
//    RNG_FillRandom(tmp,MK_UNIT_LEN);
//    PCI_WriteKeyToFlash(0,KEY_TYPE_MAGK,0,MK_UNIT_LEN,tmp);
//    RNG_FillRandom(tmp,MK_UNIT_LEN);
//    PCI_WriteKeyToFlash(0,KEY_TYPE_MCK,0,MK_UNIT_LEN,tmp);
////    DISPBUF("MMK",32,0,gMKey.aucMMK);
//#endif
}

/*--------------------------Sysinit module---------------------------------- */


void cpu_init(void)
{
//    asm("nop");
    /* Disable the watchdog timer */
//    wdog_disable();
    /* Copy any vector or data sections that need to be in RAM */
    common_startup();
    /* Perform processor initialization */
    cpu_clk_init();
    EnableInterrupts;
}
//S1000 mini usb供电控制
void s_android_com_gpio(char mode)
{
    gpio_set_output(MINI_USB_PTxy_COM_EN,GPIO_OUTPUT_SlewRateFast,0); 
    if(mode == ON)
    {
        gpio_set_bit(MINI_USB_PTxy_COM_EN,1);
    }
    else 
    {
        gpio_set_bit(MINI_USB_PTxy_COM_EN,0);
    }
}

//static void ctrl_kb_init(void)
//{
//    drv_kb_open();
//    drv_kb_NotMaxtrix_clear_irq();
//    gwp30SysMemory.SysTickDeamon.bit.keybrd = 1;
//}

void ctrl_init(void)
{
    uint sysflg = 0;
    power_keep_init();
    power_reset_init();
    wp30c_sys_variate_init();
    power_down_check_init();
    power_wake_up_init();
    hw_systick_open();
#ifdef CFG_LOWPWR
    lowerpower_init();
#endif
    s_android_com_gpio(OFF);
    // 影响上位机触屏
    s_touchscreen_gpio();
#if  defined (CFG_LED)
    hw_led_init();//charging 
#endif
//    power_module_init();
    drv_sys_init();
//#ifdef DEBUG_Dx
//    console_init(CNL_COMPORT,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
//#endif
#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
#else
//    sys_poweron_ifvalid(sys_get_counter(),500);
#endif
    sysflg = sys_start();
    get_hardware_version();
    power_5v_init();
//    power_5v_open();
    power_5v_close();
    power_3v3_init();
    power_3v3_open();
    wp30_sys_ctrl_init();
//    beep_powerswitch_nonblock(BEEP_PWM_TIMERCNT_POWER);
//    ctrl_kb_init();
//    drv_uart_open(WORK_COMPORT,WORK_BAUD,
//                  gwp30SysBuf_c.work,WRK_BUFSIZE_C,
//                  UartAppCallBack[WORK_COMPORT]);
//    ctc_uart_open_nodma();
#ifdef CFG_W25XFLASH
    hw_w25x_spi_init();
    hw_w25x_gpio_init();
#endif
#if  defined(CFG_LCD)
    lcd_init();
#endif
    if ( sysflg ) {
        //系统信息区有错
        s_sys_error("Check sysInfo!");
    }

#ifdef CFG_TAMPER 
    drv_dryice_clock_init();
#endif
#if (defined CFG_MAGCARD)
    mag_init(0);
#endif
#if (defined CFG_ICCARD)
    icc_InitIccModule();
#endif
#if (defined CFG_RFID)
    rfid_open(2);
    rfid_close();
#endif
#ifdef CFG_FACTORY
    fac_save_log(0);
#endif
#ifdef CFG_FS
    fs_init();
#endif
#if defined(CFG_SECURITY_CHIP)
    sc_is8u256a_init();
#endif
    // 待所有相关外设开启后才允许接收数据处理
    ctc_uart_open();

//    sys_tamper_init(0);
}

int check_attack(void)
{
    if(fac_ctrl_get_fac_ctrl_flag() == 0 ){
        //生产管控未结束 不判断防拆
        return OK;
    }
    return OK;
#ifdef CFG_FACTORY
//    if ( test_sram_auto(0) == OK ) {
//
//        return OK;
//    }
#else
#endif
//    while ( 1 ) {
//        if ( kb_hit() ) {
//            if ( backdoor_password_verify("737247") == 0 ) {
//                drv_lcd_setbacklight(LCD_MODE_TIMEROUT);
//                local_menu_operate(1);
//                return ERROR;
//            }
//        }
//        lcd_cls();
//        lcd_Display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,"机器被攻击!!!");
//        sys_beep();
//        drv_lcd_setbacklight(LCD_MODE_ALWAYSON);
//        s_DelayMs(300);
//        drv_lcd_setbacklight(LCD_MODE_ALWAYSOFF);
//        s_DelayMs(300);
//    }
}

void ctrl_main(void)
{
    extern int fac_mb_main (int mode);
    extern int local_menu_operate(int mode);
    ctrl_init();
    check_attack();
//    local_menu_operate(0);
#ifdef DEBUG_Dx
//    dbg_s1000_ctrl();
#endif
#ifdef CFG_FACTORY
    fac_main(0);
#endif
}

void sys_init(void)
{
    cpu_init();
    ctrl_main();
}

void start(void)
{
    sys_init();
	/* Jump to main process */
	main();
	/* No actions to perform after this so wait forever */
	while(1);
}

/*--------------------------xxx module---------------------------------- */

/* ------------------------------------end of file---------------------------------------- */

