/*
 * =====================================================================================
 *
 *       Filename:  factory.c
 *
 *    Description:  生产测试相关
 *
 *        Version:  1.0
 *        Created:  1/14/2015 2:17:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yehf (), yehf@itep.com.cn
 *        Company:  START
 *
 * =====================================================================================
 */
#include "wp30_ctrl.h"
#include "../pinpad/pcikeystore.h"
#ifdef CFG_FACTORY 

/*-----------------------------------------------------------------------------}
 *   函数声明
 *----------------------------------------------------------------------------{*/
extern int local_menu_operate(int mode);
extern int iccard_powerup(int cardslot, int *plen, uchar *pData);
extern int fac_mb_main(int mode);
/*-----------------------------------------------------------------------------}
 *  宏定义和结构体定义 
 *----------------------------------------------------------------------------{*/
#define     PRODUCT_VERSION             "V1.0"
#define     FAC_MENU_ETITLE_TOP             "FACTORY"PRODUCT_VERSION
#define     FAC_MENU_CTITLE_TOP             "生产测试"PRODUCT_VERSION

#define     FAC_AUTO_POWEROFF_TIME           30*1000*60  //生产测试界面自动关机超时时间  10 minutes

#define     FAC_CTRL_ASSEMBLE_CHECK  

// 自动测试按键
#define     KEY_TEST_ARRAY_DEBUG      0x3E22  //1、5、9、确认、清除、取消、功能
#define     KEY_TEST_ARRAY_ASSEMBLE   0x3FFF  //全测
#define     KEY_TEST_ARRAY_FINAL      0x3E22  //1、5、9、确认、清除、取消、功能

#define DEFAULT_MODULE_CFG			((1 << AUTO_TEST_SHIFT_SRAM) |\
									 (1 << AUTO_TEST_SHIFT_KEY)	 |\
									 (1 << AUTO_TEST_SHIFT_LCD)  |\
									 (1 << AUTO_TEST_SHIFT_MAG)  |\
									 (1 << AUTO_TEST_SHIFT_RFID)  |\
									 (1 << AUTO_TEST_SHIFT_IC)  |\
									 (1 << AUTO_TEST_SHIFT_RTC)  |\
									 (1 << AUTO_TEST_SHIFT_LED)  |\
									 (1 << AUTO_TEST_SHIFT_BATTERY)\
									)
//                                   装配前下载主板ID和时间设置
//									 (1 << AUTO_TEST_SHIFT_USB) \

#define DEFAULT_MAG_SUCCESS_NUM		4		//默认4次连续刷卡成功就算通过
#define DEFAULT_SAM_NUM				1       //默认测试两张SAM卡
#define DEFAULT_AGING_CHARGE_MV	    3800    //老化默认充电电压(低于此值充电过慢异常) 
#define DEFAULT_AGING_DISCHARGE_MV  3500    //老化默认放电电压(低于此值放电过快异常)
#define DEFAULT_AGING_CHARGE_TIME    90     //第一次充电
#define DEFAULT_AGING_DISCHARGE_TIME 30     //第一次放电
#define DEFAULT_AGING_RECCHARGE_TIME 120    //第二次充电
#define DEFAULT_SIGNS_VALUE   20                //默认信号值
#define DEFAULT_SPEICAL_NO    0                 //默认的老化流程为正常流程
/*-----------------------------------------------------------------------------}
 *  全局变量
 *-----------------------------------------------------------------------------{*/
const char * const gFacPhaseName[FAC_PHASE_NUM] =
{
    "板卡检",
    "装配检",
    "老化检",
    "总检"
};

// 测试全局缓冲区
FAC_STAT_t  *gFacTest=NULL;

// 装配测试菜单
const FAC_TEST_OPT gMenuAssembleTest[] = {
    {AUTO_TEST_SHIFT_LCD,       1,   test_display},
#ifdef CFG_LED
    {AUTO_TEST_SHIFT_LED,       0,   test_led},
#endif
    {AUTO_TEST_SHIFT_SERIAL,    0,   test_serial_self},
#ifdef CFG_BATTERY
    {AUTO_TEST_SHIFT_BATTERY,   1,   test_battery},
#endif
    {AUTO_TEST_SHIFT_RTC,       0,   test_rtc_autocheck},
#ifdef CFG_RFID
    {AUTO_TEST_SHIFT_RFID,      0,   test_rfid_auto},
#endif
#ifdef CFG_MAGCARD
    {AUTO_TEST_SHIFT_MAG,       (1<<17)|(1<<18)|(1<<8),   test_magcard_auto},
#endif
#if PRODUCT_NAME == PRODUCT_MPOS
#ifdef CFG_ICCARD
    {AUTO_TEST_SHIFT_IC,        (1<<1)|(1<<8),   test_iccard_auto_new},
#endif
#endif
    {AUTO_TEST_SHIFT_KEY,       KEY_TEST_ARRAY_ASSEMBLE,   test_keyboard},
#ifdef CFG_USBD_CDC
    {AUTO_TEST_SHIFT_USB,       0,   test_usb_manual},
#endif
    {AUTO_TEST_SHIFT_SRAM,     1,   test_sram_auto},
};
// 总检测试菜单
const FAC_TEST_OPT gMenuFinalTest[] = {
    {AUTO_TEST_SHIFT_SRAM,      0,   test_sram_auto},
    {AUTO_TEST_SHIFT_LCD,       1,   test_display},
#ifdef CFG_LED
    {AUTO_TEST_SHIFT_LED,       0,   test_led},
#endif
    {AUTO_TEST_SHIFT_SERIAL,    0,   test_serial_self},
#ifdef CFG_BATTERY
    {AUTO_TEST_SHIFT_BATTERY,   2,   test_battery},
#endif
#ifdef CFG_RFID
    {AUTO_TEST_SHIFT_RFID,      0,   test_rfid_auto},
#endif
#ifdef CFG_MAGCARD
    {AUTO_TEST_SHIFT_MAG,       (1<<17)|(1<<18)|(1<<8),   test_magcard_auto},
#endif
#if PRODUCT_NAME == PRODUCT_MPOS
#ifdef CFG_ICCARD
    {AUTO_TEST_SHIFT_IC,        (1<<1)|(1<<8),   test_iccard_auto_new},
#endif
#endif
    {AUTO_TEST_SHIFT_KEY,       KEY_TEST_ARRAY_FINAL,   test_keyboard},
#ifdef CFG_USBD_CDC
    {AUTO_TEST_SHIFT_USB,       0,   test_usb_manual},
#endif
    {AUTO_TEST_SHIFT_RTC,       0,   test_rtc_autocheck},
};

/*-----------------------------------------------------------------------------}
 *  函数定义
 *-----------------------------------------------------------------------------{*/
int test_rtc_autostart(uint mode, uchar *time)
{
	sys_GetTime(time);
    if ( time[0] < 0x15 || time[1] < 0x01) {
        lcd_Display(0,DISP_FONT_LINE1,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"时间错误");
        kb_GetKey(-1);
        return 1;
    }
	return 0;
}


int fac_ctrl_tip_backdoor(int new_phase)
{
    int ret;
    ret = backdoor_password_verify("456789");
    if ( ret ) {
        return 1;
    }
    TRACE_M("\r\n **********强制执行:%s 成功**********",gFacPhaseName[new_phase-1]);
    ret = fac_ctrl_set_autotest_res(new_phase-1,FAC_CTRL_FORCE_SUC);
    return ret;
}

/* 
 * fac_ctrl_show_tip - [GENERIC] 装配检 
 * @ 
 */
int fac_ctrl_show_tip(int new_phase)
{
	uint cur_phase;
    int i;
    int ret;
	cur_phase = fac_ctrl_get_cur_phase();
    TRACE_M("\r\nfac phase:%d\r\n\r\n",cur_phase);
    if ( cur_phase > FAC_PHASE_NUM ) {
        cur_phase = FAC_PHASE_DEBUG;			
    }
#if defined (DEBUG_FAC)
    TRACE_M("\r\n当前:%s准备:%s",gFacPhaseName[cur_phase],gFacPhaseName[new_phase]);
    for ( i=0 ; i<DIM(gFacPhaseName) ; i++ ) {
        ret = fac_ctrl_get_autotest_res(i);
        TRACE("\r\n阶段:%s:%d",gFacPhaseName[i],ret);
    }
#endif
#ifdef FAC_CTRL_ASSEMBLE_CHECK
	if (new_phase == cur_phase || new_phase == FAC_PHASE_DEBUG)
#else
	if (new_phase == cur_phase || new_phase == FAC_PHASE_DEBUG || new_phase == FAC_PHASE_ASSEMBLE)
#endif
	{
		return YES;
	}
	kb_Flush();
#ifdef FAC_CTRL_ASSEMBLE_CHECK
	if (new_phase >= FAC_PHASE_ASSEMBLE && new_phase <= FAC_PHASE_PRODUCT)
#else
	if (new_phase >= FAC_PHASE_AGING && new_phase <= FAC_PHASE_PRODUCT)
#endif
	{
        ret = fac_ctrl_get_autotest_res(new_phase-1);
        // 未测试或测试失败
		if ((ret!=FAC_CTRL_SUCCESS)
            && (ret!=FAC_CTRL_FORCE_SUC) )
		{
            lcd_Cls();
			lcd_Display(0, 0, DISP_FONT, "警告:%s未通过", gFacPhaseName[new_phase-1]);
			lcd_Display(0, DISP_FONT_LINE1, DISP_FONT, "不允许进入%s", gFacPhaseName[new_phase]);
			lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_MEDIACY, "[1]-强制进入");
			lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_MEDIACY, "[取消]-退出");
			while (1)
			{
                i = kb_GetKey(-1);
                if ( i == KEY_CANCEL ) {
                    return NO;
                } else if(i == KEY1){
                    if (!fac_ctrl_tip_backdoor(new_phase))
                    {
                        break;
                    }
                }
			}
		}
	}
    lcd_cls();
	lcd_Display(0,DISP_FONT_LINE0, DISP_FONT|DISP_MEDIACY, "当前为%s", gFacPhaseName[cur_phase]);
	lcd_Display(0,DISP_FONT_LINE1, DISP_FONT, "[确认]-进入%s", gFacPhaseName[new_phase]);
	lcd_Display(0,DISP_FONT_LINE2, DISP_FONT, "[取消]-退出");

	while (1)
	{
		switch (kb_GetKey(-1))
		{
			case KEY_CANCEL:
				return NO;
			case KEY_ENTER:
				fac_ctrl_set_cur_phase(new_phase);
				return YES;
			default:
				continue;
		}
	}
}
/* 
 * test_conf_require - [GENERIC] 判断是否需要测试 
 * //该模块是否需要自动测试()
//返回1:需要, 返回0:不需要
 * @ 
 */
int test_conf_require(int module)
{
    if ( IFBIT(gFacTest->tConfig.module_config,module) ) {
        return 1;
    } else {
		return 0;
	}
}

// 0-不需要重新测试 非0-需要重新测试
uint if_test_again(uint flag, uint config)
{
    return (flag&(1<<config));
}
int test_write_result(int ret, int config, uint *result)
{
    int data = *result;
    if ( config >= DEFAULT_MODULE_CFG) {
        return 1;
    }
    if ( ret ) {
        SETBIT(data,config);
    } else {
        CLRBIT(data,config);
    }
    *result = data;
    return 0;
}


//磁卡刷几次算成功
int test_conf_mag_success(void)
{
	return gFacTest->tConfig.mag_success_num;
}

//测几张SAM卡
int test_conf_sam_num(void)
{
	return gFacTest->tConfig.sam_test_num;
}

int test_conf_aging_charge_mv(void)
{
	return gFacTest->tConfig.aging_charge_mv;
}

int test_conf_aging_discharge_mv(void)
{
	return gFacTest->tConfig.aging_discharge_mv;
}

int test_conf_aging_discharge_time(void)
{
	return gFacTest->tConfig.aging_discharge_time;
}

int Set_conf_aging_dicharge_time(int time)
{
	gFacTest->tConfig.aging_discharge_time = (ushort)time;
	return 0;
}

int test_conf_icsq(void)
{
	return gFacTest->tConfig.icsq;
}

int test_conf_aging_special(void)
{
	return gFacTest->tConfig.ispecial;
}
// 底板测试，调试检，装配检，总检测试结束调用
// 0-退出 1-重新测试
int test_fail_view(unsigned int testResult)
{
    int i,key;
    char message[128] = "测试:";
    for (i = 0; i <= AUTO_TEST_SHIFT_MAX; i++)
    {
        if ((1 << i) & testResult)
        {
            switch (i)
            {
            case AUTO_TEST_SHIFT_SRAM:
                strcat(message, "防拆,");
                break;
            case AUTO_TEST_SHIFT_KEY:
                strcat(message, "按键,");
                break;
            case AUTO_TEST_SHIFT_LCD:
                strcat(message, "液晶,");
                break;
            case AUTO_TEST_SHIFT_PRINT:
                strcat(message, "打印,");
                break;
            case AUTO_TEST_SHIFT_MAG:
                strcat(message, "磁卡,");
                break;
            case AUTO_TEST_SHIFT_IC:
                if (ic_not_in_slot)
                {
                    strcat(message, "IC卡在位,");
                }
                if (gFacTest->iccardret[0])
                {
#if PRODUCT_NAME == PRODUCT_MPOS
                    strcat(message, "IC卡,");
#else
                    strcat(message, "SAM卡0,");
#endif
                }

                if (gFacTest->iccardret[SAM1SLOT])
                {
                    strcat(message, "SAM卡1,");
                }
                break;
            case AUTO_TEST_SHIFT_SERIAL:
                strcat(message, "串口,");
                break;
            case AUTO_TEST_SHIFT_RTC:
                strcat(message, "RTC,");
                break;
            case AUTO_TEST_SHIFT_TEMP:
                strcat(message, "温度,");
                break;
            case AUTO_TEST_SHIFT_RFID:
                strcat(message, "射频卡,");
                break;
            case AUTO_TEST_SHIFT_USB:
                strcat(message, "USB,");
                break;
            case AUTO_TEST_SHIFT_BATTERY:
                if (battery_charge_err)
                    strcat(message, "充电电路故障");
                else
                    strcat(message, "电池,");
                break;
            case AUTO_TEST_SHIFT_LED:
                strcat(message, "LED灯,");
                break;
            default:
                break;
            }
        }
    }
//    TRACE_M("\r\n测试未通过 result=%x, ic_error_array=%x", testResult, ic_error_array);
    strcat(message, "未通过");
    lcd_Display(0, 0, DISP_FONT|DISP_CLRLINE, message);
    kb_Flush();
    if ( gTestStage == STAGE_Assemble || gTestStage == STAGE_Product) {
        sys_BeepPro(BEEP_PWM_HZ, 1000, NO);
        lcd_Display(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, "[确认]-重测未通过项");
        key = kb_GetKey(-1);
        if ( key == KEY_ENTER ) {
            return 1;
        }
    } else {
        kb_GetKey(-1);
    }
    return 0;
}
int show_aging_log_new(int para)
{
	s_aging_log log;

	int page = 0;
	int iRet = 0;
    int i = 0;
	const uchar *step_name[]=
	{
		"无",
		"按键",
		"液晶",
		"串口",
		"磁卡",
		"IC卡",
		"打印",
	};

	int page_need = 4;//总共需要多少页
	memset(&log, 0x00, sizeof(s_aging_log));
	iRet = fac_read_aginglog(&i,&log);
    if ( iRet == 0 ) 
    {
		lcd_Cls();
		lcd_Display(0,0, DISP_FONT | DISP_CLRLINE | DISP_MEDIACY | DISP_INVLINE, "老化结果");
		lcd_Display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "未测试");
		kb_GetKey(-1);
		return -1;
    }

	kb_Flush();
	while (1)
	{
		lcd_Cls();
		lcd_Display(0,0, DISP_FONT | DISP_CLRLINE | DISP_MEDIACY | DISP_INVLINE, "老化结果");
		switch (page)
		{
			case 0:
				if (log.aging_allok== 1)
				{
					lcd_Display(0,DISP_FONT_LINE1, DISP_FONT | DISP_CLRLINE, "成功");
				}
				else
				{
					if (log.fail_step > 0)
					{
						lcd_Display(0,DISP_FONT_LINE1, DISP_FONT | DISP_CLRLINE, "失败环节:%s",
									step_name[log.fail_step]);
					}
					if (log.battery_error_info & (1<<2))
					{
						lcd_Display(0,DISP_FONT_LINE2, DISP_FONT | DISP_CLRLINE,
									"适配器掉电");
					}
					else if (log.battery_error_info & (1<<0))
					{
						lcd_Display(0,DISP_FONT_LINE2, DISP_FONT | DISP_CLRLINE,
									"充电电路故障");
					}
					else if (log.battery_error_info & (1<<1))
					{
						lcd_Display(0,DISP_FONT_LINE3, DISP_FONT | DISP_CLRLINE,
									"电池不良");
					}
					else if (log.battery_error_info & (1<<4))
					{
						lcd_Display(0,DISP_FONT_LINE3, DISP_FONT | DISP_CLRLINE,
									"最后充电未完成");
					}
					else
					{
						lcd_Display(0,DISP_FONT_LINE3, DISP_FONT | DISP_CLRLINE,
									"老化测试未完成");
					}

				}
				lcd_Display(0,DISP_FONT_LINE4, DISP_FONT | DISP_CLRLINE, "翻页查看详细信息:%d",log.battery_error_info);
				break;
			case 1:
				lcd_Display(0,DISP_FONT_LINE1, DISP_FONT | DISP_CLRLINE, "总测试轮数:%d",log.totaltimes);
				lcd_Display(0,DISP_FONT_LINE2, DISP_FONT | DISP_CLRLINE, "是否因失败终止:%s", (log.fail_step == 0) ? "否":"是");
				break;
//			case 2:
//				break;
            case 2:
				lcd_Display(0,DISP_FONT_LINE1, DISP_FONT | DISP_CLRLINE, "总充电时间%dmin", log.charge_time);
				lcd_Display(0,DISP_FONT_LINE2, DISP_FONT | DISP_CLRLINE, "电池老化时间%dmin",log.discharge_time);
				if (log.baterry_mv1 > 0)
				{
					lcd_Display(0,DISP_FONT_LINE3, DISP_FONT | DISP_CLRLINE, "拔电源时%dmv", log.baterry_mv1);
				}
				if (log.baterry_mv2 > 0)
				{
					lcd_Display(0,DISP_FONT_LINE4, DISP_FONT | DISP_CLRLINE, "放电结束%dmv", log.baterry_mv2);
				}
				else
				{
				}
				break;
			case 3:
				lcd_Display(0,DISP_FONT_LINE2, DISP_FONT | DISP_CLRLINE, "充电时长:%dmin",
						log.recharge_time);
				lcd_Display(0,DISP_FONT_LINE3, DISP_FONT | DISP_CLRLINE, "最后记录电压:%dmv",
						log.baterry_mv_actual);
				break;
			default:
				break;
		}

		switch (kb_GetKey(-1))
		{
			case KEY_CANCEL:
				return -2;
			case KEY_ENTER:
				return 0;
			case KEY2:
				if (page <= 0)
				{
					page = page_need - 1;
				}
				else
				{
					page--;
				}
				break;
			case KEY8:
				if (page >= (page_need -1))
				{
					page = 0;
				}
				else
				{
					page++;
				}
				break;
			default:
				break;
		}
	}
}
/* 
 * test_auto_final - [GENERIC] 装配检 
 * @ 
 */
int test_auto_final(int mode)
{
    int result;
//	int icsq = 0;
    uint i;
    uint first=1,again_flag = UINT_MAX;
    uint testResult = 0;
    FAC_TEST_OPT *p;
    uint phase;
	//显示上次老化结果
    s_aging_log log;
	memset(&log, 0x00, sizeof(s_aging_log));
	show_aging_log_new(0);

    phase = FAC_PHASE_PRODUCT;
	if (fac_ctrl_show_tip(phase) != YES)
	{
		return OK;
	}
    TRACE_M("\r\n %s开始",gFacPhaseName[phase]);
    memset(gFacTest->time,0,sizeof(gFacTest->time));
	if(test_rtc_autostart(0, gFacTest->time)) {
        return ERROR;
    }
	fac_ctrl_set_cur_phase(phase);
    gTestStage = phase;
test_auto_final_again:
    p = (FAC_TEST_OPT *)&gMenuFinalTest[0];
    for ( i=0 ; i<DIM(gMenuFinalTest) ; i++,p++  ) {
        if (test_conf_require(p->id)
            && if_test_again(again_flag,p->id))
        {
            result = p->test_func(p->para);
            test_write_result(result,p->id,&testResult);
        }
    }
//    init_sram(0);
    lcd_Cls();
	kb_Flush();
    if (!testResult) {
        fac_ctrl_set_test_detail(phase,testResult);
        fac_ctrl_set_autotest_res(phase, FAC_CTRL_SUCCESS);
        lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "%s通过",gFacPhaseName[phase]);
        kb_GetKey(2000);
        return OK;
    } else {
        fac_ctrl_set_test_detail(phase,testResult);
        fac_ctrl_set_autotest_res(phase, FAC_CTRL_FAIL);
        if(test_fail_view(testResult)) {
            TRACE_M("\r\n 当前result:%x flag:%x",testResult,again_flag);
            if ( first == 1 ) {
                first = 0;
                again_flag = (~again_flag) | testResult;
            } else {
                again_flag = testResult;
            }
            TRACE_M("\r\n 重新result:%x flag:%x",testResult,again_flag);
            goto test_auto_final_again;
        }
        return ERROR;
    }
}

/* 
 * test_auto_assemble - [GENERIC] 装配检 
 * @ 
 */
int test_auto_assemble(int mode)
{
    int result;
    uint i;
    uint first=1,again_flag = UINT_MAX;
    uint testResult = 0;
    FAC_TEST_OPT *p;
    uint phase;

#if PRODUCT_NAME == PRODUCT_EPP 
        s_aging_log log;
        memset(&log, 0, sizeof(s_aging_log));	//清空log缓存
#endif

    //调试检强制成功
//    fac_ctrl_set_autotest_res(FAC_PHASE_DEBUG,FAC_CTRL_SUCCESS);

    phase = FAC_PHASE_ASSEMBLE;
	if (fac_ctrl_show_tip(phase) != YES)
	{
		return OK;
	} 
    TRACE_M("\r\n %s开始",gFacPhaseName[phase]);
    memset(gFacTest->time,0,sizeof(gFacTest->time));
	if(test_rtc_autostart(0, gFacTest->time)) {
        return ERROR;
    }
	fac_ctrl_set_cur_phase(phase);
    gTestStage = phase;
test_auto_assemble_again:
    p = (FAC_TEST_OPT *)&gMenuAssembleTest[0];
    for ( i=0 ; i<DIM(gMenuAssembleTest) ; i++,p++  ) {
        if (test_conf_require(p->id)
            && if_test_again(again_flag,p->id))
        {
            result = p->test_func(p->para);
            test_write_result(result,p->id,&testResult);
        }
    }
//    init_sram(0);
    lcd_Cls();
	kb_Flush();
    if (!testResult) {
        fac_ctrl_set_test_detail(phase,testResult);
		fac_ctrl_set_autotest_res(phase, FAC_CTRL_SUCCESS);
        lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "%s通过",gFacPhaseName[phase]);
        kb_GetKey(2000);

#if PRODUCT_NAME == PRODUCT_EPP 
        //mk210当通过装配检时 老化总检直接成功
        log.aging_allok = 1;
        fac_write_aginglog(&log);
        fac_ctrl_set_autotest_res(FAC_PHASE_AGING, FAC_CTRL_SUCCESS);
        fac_ctrl_set_autotest_res(FAC_PHASE_PRODUCT, FAC_CTRL_SUCCESS);
#endif

        return OK;
    } else {
        fac_ctrl_set_test_detail(phase,testResult);
        fac_ctrl_set_autotest_res(phase, FAC_CTRL_FAIL);
        if(test_fail_view(testResult)) {
            TRACE("\r\n 当前result:%x flag:%x",testResult,again_flag);
            if ( first == 1 ) {
                first = 0;
                again_flag = (~again_flag) | testResult;
            } else {
                again_flag = testResult;
            }
            TRACE_M("\r\n 重新result:%x flag:%x",testResult,again_flag);
            goto test_auto_assemble_again;
        }

#if PRODUCT_NAME == PRODUCT_EPP 
        //mk210当通过装配检时 老化总检直接成功否则失败
        log.aging_allok = 0;
        fac_write_aginglog(&log);
        fac_ctrl_set_autotest_res(FAC_PHASE_AGING, FAC_CTRL_FAIL);
        fac_ctrl_set_autotest_res(FAC_PHASE_PRODUCT, FAC_CTRL_FAIL);
#endif

        return ERROR;
    }
}


int aging_charge_step(s_aging_log *log,char step)	//老化第一次充电阶段
{
    int iRet = 0;
    int time = DEFAULT_AGING_CHARGE_TIME ;//充电时间
    static uint charge_start_counter = 0;
    static int charge_time_tmp = 0;
    uint tmp_counter2 = 0;
    int mv = 0;
    int j = 0;
    int endflag = 0;
    lcd_Cls();
    //供电状态
    iRet = sys_GetSupplyState();
    if (iRet == SUPPLY_VIN_BYON_STATE
        || iRet == SUPPLY_VIN_BYFULL_STATE)//适配器供电,有电池
    {
        if(!step)//记录开始充电时的时间点
        {
            charge_time_tmp = 0;
            charge_start_counter = sys_GetCounter();
        }
        log->battery_error_info  |= (1<<2);
        log->charge_time = (sys_GetCounter() - charge_start_counter)/1000/60 + charge_time_tmp;
        if(log->charge_time >= time)//充电时间超过3.5小时
        {
            lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "已充电满%d分钟",time);
        }
        else
        {
            lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "已充电%d分钟",
                        log->charge_time);
        }
        sys_GetBattery(&mv);
        log->baterry_mv_actual = mv;
        if (iRet == SUPPLY_VIN_BYFULL_STATE)
        {
            lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池电压:%dmv(满)", mv);
        }
        else
        {
            lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池电压:%dmv", mv);
        }
        sys_DelayMs(2000);

    }
    else if (iRet == SUPPLY_BATTERY_NOR_STATE)//电池供电
    {
        sys_GetBattery(&mv);

        log->baterry_mv_actual = mv;
        if(log->charge_time >= time)//充电时间超过3.5小时
        {
            log->baterry_mv1 = mv;
            if (mv < test_conf_aging_charge_mv())
            {
                log->battery_error_info |=  (1<<0);
            }
            log->battery_error_info &= (~(1<<2));
            endflag = 1;	//第一阶段充电结束
        }
        else
        {
            lcd_Display(0,  0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "充电时间不足!");
            lcd_Display(0,DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "请继续充电!");
            lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池电压:%dmv", mv);
            tmp_counter2 = sys_GetCounter();
            charge_time_tmp = log->charge_time;
            log->battery_error_info |=  (1<<2);
            while (1)
            {
                if (sys_GetCounter() - tmp_counter2 >= (60*1000*30))	//提示30分钟
                {
                    log->battery_error_info |=  (1<<0);
                    log->battery_error_info |=  (1<<2);
                    endflag = 1;
                    break;
                }

                j = sys_GetSupplyState();

                if (j == SUPPLY_VIN_BYON_STATE
                    || j == SUPPLY_VIN_BYFULL_STATE)
                {
                    //插入了适配器
                    tmp_counter2 = 0;
                    charge_start_counter = sys_GetCounter();
                    break;
                }
                lcd_SetLight(LIGHT_ON_MODE);
                sys_DelayMs(1000);
                sys_Beep();
                lcd_SetLight(LIGHT_OFF_MODE);
                sys_DelayMs(1000);
                sys_Beep();
            }
            lcd_SetLight(LIGHT_ON_MODE);
            //				goto BATTERY_OUT;
        }

    }
    if (endflag == 0)
    {
        return  -1;
    }
    else
    {
        return OK;
    }
}
int aging_discharge_step(s_aging_log *log,int step)//老化电池放电阶段
{
	int endflag = 0;
	int mv = 0;
	static int tmp_counter1 = 0;
	int iRet = 0;
	int j = 0;
	static uint discharge_start_counter = 0;

		iRet = sys_GetSupplyState();
		if (!step)
		{
			tmp_counter1 = 0;
			discharge_start_counter = sys_GetCounter();//记录开始放电时的时间点
		}
		lcd_Cls();
		if (iRet == SUPPLY_BATTERY_NOR_STATE)//电池供电
		{
			sys_GetBattery(&mv);
			log->baterry_mv_actual = mv;
			if (log->battery_error_info & (1<<0))
			{
				sys_BeepPro(BEEP_PWM_HZ, 2000, 0);
				lcd_Display(0,  0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "充电电路故障!");
				lcd_Display(0,  DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "battery_error_info:%d\r\n",log->battery_error_info);
				sys_DelayMs(2000);
			}

			log->discharge_time = (sys_GetCounter()-discharge_start_counter)/1000/60;

			lcd_Display(0,DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池老化%d分钟",
					   				log->discharge_time);
			lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池电压:%dmv", mv);
			sys_DelayMs(2000);
		}
		else if (iRet == SUPPLY_VIN_BYON_STATE
				|| iRet == SUPPLY_VIN_BYFULL_STATE)//适配器供电,有电池)
		{
			sys_GetBattery(&mv);
			log->baterry_mv_actual = mv;
			log->discharge_time = (sys_GetCounter()-discharge_start_counter)/1000/60;

			lcd_Display(0,  0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "正在进行电池老化");
			lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "请拔除适配器!");
			lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,
								"已老化%d分钟", log->discharge_time);
			lcd_Display(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池电压:%dmv", mv);

			if (tmp_counter1 == 0)
			{
				tmp_counter1 = sys_GetCounter();
				while (1)
				{
					if (sys_GetCounter() - tmp_counter1 >= (60*1000*10))//报警10分钟
					{
						//超过10分钟,不再报警,仍继续老化,
						break;
					}
					j = sys_GetSupplyState();
					if (j == SUPPLY_BATTERY_NOR_STATE)	//适配器拔除
					{
						tmp_counter1 = 0;
						break;
					}
					lcd_SetLight(LIGHT_ON_MODE);
				    sys_DelayMs(1000);
				    sys_Beep();
				    lcd_SetLight(LIGHT_OFF_MODE);
				    sys_DelayMs(1000);
				    sys_Beep();
				}
	//						tmp_counter = 0;

				lcd_SetLight(LIGHT_ON_MODE);
			//	goto BATTERY_OUT;
			}
			else
			{
				sys_BeepPro(BEEP_PWM_HZ, 2000, 0);
				sys_DelayMs(1000);
			}
		}

		if (log->discharge_time > test_conf_aging_discharge_time())		//默认放电3.5个小时,可根据配置文件修改
		{
			log->baterry_mv2 = mv;
			endflag = 1;
			if(mv < test_conf_aging_discharge_mv())//电压待定值
			{
				log->battery_error_info |= (1<<1);
			}
			else
			{

			}

		}

		if (endflag == 0)
		{
			return -1;
		}
		else
		{
			return OK;
		}
}

int aging_recharge_step(s_aging_log *log)//老化结束后再充电阶段
{
//	int iRet = 0;
	int supply_state = 0;
	int mv = 0;
	int count = 0;
//	int charge_time = 0;
	int start_recharge_counter = 0;
	int full = 0;	//检测到电池充满
	uint full_counter = 0;//充满时的计数值
	int charge_time_tmp = 0;
	int pause_flag = 0;	//充电期间适配器被拔出,置该变量1
	int iflag = 0;
	int icount = 0;
    char step = 0;
	kb_Flush();
		while (1)
		{
			lcd_Cls();

			supply_state = sys_GetSupplyState();
			if (supply_state == SUPPLY_BATTERY_NOR_STATE)//电池供电
			{
				sys_GetBattery(&mv);
				log->baterry_mv_actual = mv;
				if (pause_flag == 0)
				{
					full = 0;
					pause_flag = 1;
					charge_time_tmp = log->recharge_time;
				}
//				lcd_Display(0, (LCD_HEIGHT / 5) * 0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池老化已结束");
				lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "请插入适配器充电");
				lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "当前电池电压:%dmv", mv);
				if (log->recharge_time < DEFAULT_AGING_RECCHARGE_TIME && mv < DEFAULT_AGING_CHARGE_MV)
				{
					lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池未充满!");
				}

			}
			else if (supply_state == SUPPLY_VIN_BYON_STATE
					|| supply_state == SUPPLY_VIN_BYFULL_STATE)//适配器供电,有电池)
			{
				sys_GetBattery(&mv);
				log->baterry_mv_actual = mv;
				if (!step)	//第一次进入充电程序
				{
//					sys_GetTime(log->recharge_start_rtc);
					start_recharge_counter = sys_GetCounter();
                    fac_write_aginglog(log);
                    step = 1;
				}
				if (pause_flag == 1)
				{
					pause_flag = 0;
					start_recharge_counter = sys_GetCounter();
				}
				log->recharge_time = (sys_GetCounter() - start_recharge_counter)/1000/60 + charge_time_tmp;
				lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "已充电%d分钟",
					   			log->recharge_time);
				if (supply_state == SUPPLY_VIN_BYFULL_STATE)
				{
					if (full == 0)
					{
						lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "充满");
						full_counter = sys_GetCounter();
						full = 1;
					}
					lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池电压:%dmv(满)", mv);
				}
				else
				{
					if (full == 1)
					{
						lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "没充满");
						full = 0;
						if (sys_GetCounter() - full_counter < (10*60*1000))//充满后10分钟之内,检测是否重新开始充电
						{
							log->battery_error_info |= (1<<4);
							log->aging_allok = 0;
							fac_write_aginglog(log);
							fac_ctrl_set_test_detail(FAC_PHASE_AGING, 1<<1);//记录电池不良
							fac_ctrl_set_autotest_res(FAC_PHASE_AGING, FAC_CTRL_FAIL);//老化失败(总检时会提示更换电池)
						}
					}
					lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池电压:%dmv", mv);
				}
			}
			lcd_Display(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "按[确认]查看老化结果");

//			sys_GetTime(log->actual_rtc);
			if (count % 100 == 0)
			{
				fac_write_aginglog(log);
			}
			count++;
			if (kb_Hit())
			{
				if (kb_GetKey(100) == KEY_ENTER)
				{
					show_aging_log_new(0);
				}
			}
#if 0
			if ( (log->recharge_time >= DEFAULT_AGING_RECCHARGE_TIME) && (iflag == 0))
			{
				iflag = 1;
				itime = log->recharge_time;
			}
#endif
            if ( log->recharge_time >= DEFAULT_AGING_RECCHARGE_TIME ) 
            {
                iflag = 1;
            }

//			if(iflag == 1 && (log->recharge_time > itime + 10))
            if ( iflag == 1) 
			{
				if(mv >= test_conf_aging_charge_mv())
				{
					log->aging_allok = 1;
                    fac_write_aginglog(log);
				}
				else
				{
					lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "失败");
					kb_GetKey(-1);
					//时间到了没充满
					log->battery_error_info |= (1<<4);
					log->aging_allok = 0;
                    fac_write_aginglog(log);
					goto FAIL;
				}
				icount++;
				if((icount > 20) || (full == 1))//20次后，或成功就退出
				{
//					sys_DelayMs(500);
                    lcd_Cls();
					lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE, "老化成功");
					lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "电池电压:%dmv",mv);
                    sys_DelayMs(500);
                    return 0;
				}
			}
			sys_DelayMs(2000);
		}

FAIL:
	return ERROR;
}
/* 
 * test_aging_new - [GENERIC]  老化测试
 * @ 
 */
#if PRODUCT_NAME == PRODUCT_MPOS
int test_aging_new(int mode)
{
	int result = 0;
	int iRet = 0;
	uint begcounter = 0;
	int testTimer = 0;
	int lcdCnt = 0;
    int keyCnt = 0;
	int icCnt = 0;
	int magCnt = 0;
	int keyBegCounter = 0;
	int step = 0;
	int battery_step = 0; //老化时电池的阶段,0-开始时充电阶段,1-放电阶段,2-第二次充电阶段
	int ic_len = 0;
	uchar ic_buff[256];
    int id = 0;
    char chargestep = 0;
    char dischargestep = 0;

	s_aging_log log;
	int log_size = sizeof(s_aging_log);
	memset(&log, 0, log_size);	//清空log缓存

    fac_read_aginglog(&id,&log);
	if (log.aging_allok == 1)
	{
        lcd_Cls();
		lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE,
                "老化测试已经成功!");
        lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE,
                "[1]重测 [2]退出");
        while (1)
        {
            switch (kb_GetKey(-1))
            {
                case KEY1:
					iRet = backdoor_password_verify("456789");
					if(iRet == OK)
					{
						break;
					}
					else
					{
						lcd_cls();
						lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE,
                					"老化测试已经成功!");
        				lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE,
                					"[1]重测 [2]退出");
						continue;
					}
                    break;
                case KEY2:
                    return 0;
                case KEY_CANCEL:
                    return 0;
                default:
                    continue;
            }
            break;
        }
	}
	result = fac_ctrl_show_tip(FAC_PHASE_AGING);
	if (result != YES)
		return -1;

    iRet = sys_GetSupplyState();
	memset(&log, 0, log_size);	//清空log缓存
    while (iRet != SUPPLY_VIN_BYON_STATE
           && iRet != SUPPLY_VIN_BYFULL_STATE)//没插适配器
    {
        iRet = sys_GetSupplyState();
        lcd_Display(0,DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE,
                    "请插入适配器!");
        sys_BeepPro(BEEP_PWM_HZ, 500, 0);
        sys_DelayMs(1000);
    }
	begcounter = sys_GetCounter();
    while(1)
    {
        testTimer++;
        if(battery_step == 1)
		{
			lcd_Cls();
            hw_led_on(LED_ALL);
            lcd_SetLight(LIGHT_ON_MODE);
            lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"正在打开蓝牙");
#ifdef CFG_BLUETOOTH
            bt_open();
#endif
            if (sys_GetModuleType(MODULE_RF_TYPE) != MODULE_NOTEXIST)
            {
#ifdef CFG_RFID
//                rfid_open(0); //老化不测射频
#endif
            }
		}
        lcdCnt++;

		lcd_Cls();
#ifdef  CFG_ICCARD
		lcdDispMultiLang(0, 0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "IC卡,磁卡上电", "IC,mag Powerup");
		lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_MEDIACY, "IC卡正在上电");
		iRet = iccard_powerup(USERCARD, &ic_len, ic_buff);
#endif
#ifdef CFG_MAGCARD
		lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_MEDIACY, "磁卡正在上电");
		mag_Open();
		lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_MEDIACY, "磁卡上电成功");
#endif
		kb_GetKey(1000);
		icCnt++;
		magCnt++;

        keyCnt++;
		lcd_Cls();
        kb_Flush();

        keyBegCounter = sys_GetCounter();
        lcdDispMultiLang(0, 0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "按键测试", "TEST SERIAL");
        while (1)
        {
            if (kb_Hit())
            {
                kb_GetKey(20);
                goto FAIL;
            }
            if ((sys_GetCounter() - keyBegCounter) > 1000)
            {
                break;
            }
        }
        switch (battery_step)
        {
        case 0:
            iRet = aging_charge_step(&log,chargestep);
            if (iRet == OK)
            {
                battery_step = 1;
            }
            chargestep = 1;
            break;
        case 1:
            iRet = aging_discharge_step(&log,dischargestep);
            if (iRet == OK)
            {
                battery_step = 2;
            }
            dischargestep = 1;
            break;
        default:
            break;
        }
		lcd_Cls();
		log.totaltimes = testTimer;
		log.total_time = (sys_GetCounter() - begcounter) / 1000/ 60;
		fac_write_aginglog (&log);
		if ((log.battery_error_info & (1<<0)))
		{
			//充电电路故障,直接结束老化
			goto FAIL;
		}
		if ((log.battery_error_info & (1<<1)))
		{
			//电池放电测试失败
			fac_ctrl_set_test_detail(FAC_PHASE_AGING, 1);//电池有故障就记录下来
			goto FAIL;
		}
		if (battery_step == 2)
		{
			//放电时间结束,
			goto SUCCESS;
		}
    }
SUCCESS:
	fac_ctrl_set_autotest_res(FAC_PHASE_AGING, FAC_CTRL_SUCCESS);
    lcd_SetLight(LIGHT_TIMER_MODE);
#ifdef CFG_BLUETOOTH
    bt_close();
#endif
	if (sys_GetModuleType(MODULE_RF_TYPE) != MODULE_NOTEXIST)
	{
#ifdef CFG_RFID
//	   rfid_close(); //老化不测射频
#endif
	}
    hw_led_off(LED_ALL);
    iRet = aging_recharge_step(&log);
    if (iRet == ERROR)
    {
        goto FAIL;
    }
    while(1)
    {
        if ( sys_GetSupplyState() ==  SUPPLY_BATTERY_NOR_STATE ) {
            sys_Shutdown();
        }
        s_DelayMs(500);
    }
//    return 0;
FAIL:
	lcd_SetLight(LIGHT_TIMER_MODE);
    hw_led_off(LED_ALL);
#ifdef CFG_BLUETOOTH
    bt_close();
#endif
	if (sys_GetModuleType(MODULE_RF_TYPE) != MODULE_NOTEXIST)
	{
#ifdef CFG_RFID
//	   rfid_close(); //老化不测射频
#endif
	}
	lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "第%4d轮测试失败", testTimer);
	lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "失败原因:%d",log.battery_error_info);
    lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "实时电压:%d",log.baterry_mv_actual);
    lcd_Display(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, "程序终止!");

	log.error_timer = testTimer;
	log.fail_step = step;
    fac_write_aginglog(&log);

	while (1)
	{
		lcd_SetLight(LIGHT_ON_MODE);
	    sys_DelayMs(1000);
	    sys_Beep();
	    lcd_SetLight(LIGHT_OFF_MODE);
	    sys_DelayMs(1000);
	    sys_Beep();
		if (kb_Hit())
		{
			kb_GetKey(50);
			break;
		}
	}
	lcd_SetLight(LIGHT_TIMER_MODE);

	while (1)
	{
		show_aging_log_new(0);
	}
}
#else
//MK210老化
int test_aging_new(int mode)
{
	uint begcounter = 0;
	uint curcounter = 0;
	int testTimer = 0;
	int lcdCnt = 0;
    int keyCnt = 0;
	int icCnt = 0;
	int magCnt = 0;
	int keyBegCounter = 0;
	int ic_len = 0;
	uchar ic_buff[256];

	s_aging_log log;
	memset(&log, 0, sizeof(s_aging_log));	//清空log缓存
    log.aging_allok = 1;
    fac_write_aginglog(&log);
	fac_ctrl_set_autotest_res(FAC_PHASE_AGING, FAC_CTRL_SUCCESS);

	begcounter = sys_GetCounter();
    lcd_Cls();
    while(1)
    {
        testTimer++;
//        hw_led_on(LED_ALL);
        lcd_SetLight(LIGHT_ON_MODE);
        lcdCnt++;


#ifdef  CFG_ICCARD
		lcdDispMultiLang(0, 0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY|DISP_CLRLINE, "IC卡上电", "IC,mag Powerup");
		lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_MEDIACY|DISP_CLRLINE, "SAM0卡正在上电");
		iccard_powerup(USERCARD, &ic_len, ic_buff);
		lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_MEDIACY|DISP_CLRLINE, "SAM1卡正在上电");
		iccard_powerup(SAM1SLOT, &ic_len, ic_buff);
#endif
		s_DelayMs(1000);
		icCnt++;
		magCnt++;

        keyCnt++;
		lcd_Cls();
        kb_Flush();

        keyBegCounter = sys_GetCounter();
        lcdDispMultiLang(0, 0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "按键测试", "TEST SERIAL");
        curcounter = sys_GetCounter();
        lcdDispMultiLang(0, DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE, 
                         "时间:%d分钟", "Time:%dmins",(curcounter-begcounter)/1000/60);
        while (1)
        {
            if (kb_Hit())
            {
                kb_GetKey(20);
                goto SUCCESS;
            }
            if ((sys_GetCounter() - keyBegCounter) > 1000)
            {
                break;
            }
        }

    }
SUCCESS:
	lcd_SetLight(LIGHT_TIMER_MODE);
    hw_led_off(LED_ALL);
    return OK;
}

#endif
#if 0
uint Fac_SRAM(uint mode)
{
#if  defined(CFG_TAMPER)
    ST_MMAP_KEY mmk;
    CLRBUF(mmk.facchk);
    hw_dryice_read_key((FPOS(ST_MMAP_KEY,facchk)>>2),1,(uint32_t *)&mmk.facchk);
    TRACE_BUF("fack",mmk.facchk,4);
    if(mode == 0)
    {
        if(memcmp(mmk.facchk,STR_SRAM,FUN_LEN) == 0)
        {
            //正常
            return 0;
        }
        else
        {
            //防拆异常
            return 1;
        }
    }
    else
    {
        if(memcmp(mmk.facchk,STR_SRAM,FUN_LEN) != 0)
        {
            sys_tamper_open(mode);
            RNG_FillRandom((uint8_t *)&mmk, MMAP_KEY_LEN);
            memcpy(mmk.facchk,STR_SRAM,FUN_LEN);
            hw_dryice_write_key((FPOS(ST_MMAP_KEY,aucMMK)>>2),
                                ((MK_UNIT_LEN+ELRCK_LEN+CHECK_LEN+FUN_LEN)>>2),
                                (uint32_t *)&mmk);
            hw_dryice_read_key((FPOS(ST_MMAP_KEY,facchk)>>2),1,(uint32_t *)&mmk.facchk);
            return (memcmp(mmk.facchk,STR_SRAM,FUN_LEN) != 0);
        }
        return 0;
    }
#else
    return 0;
#endif
}
#endif

int test_conf_init(void)
{
	//默认值
	gFacTest->tConfig.module_config = DEFAULT_MODULE_CFG;
	gFacTest->tConfig.mag_success_num = DEFAULT_MAG_SUCCESS_NUM;
	gFacTest->tConfig.sam_test_num = DEFAULT_SAM_NUM;
	gFacTest->tConfig.aging_charge_mv = DEFAULT_AGING_CHARGE_MV;
	gFacTest->tConfig.aging_discharge_mv = DEFAULT_AGING_DISCHARGE_MV;
	gFacTest->tConfig.aging_discharge_time = DEFAULT_AGING_DISCHARGE_TIME;
	gFacTest->tConfig.icsq = DEFAULT_SIGNS_VALUE;
	gFacTest->tConfig.ispecial = DEFAULT_SPEICAL_NO;
	TRACE_M("\r\n默认配置为0x%x", gFacTest->tConfig.module_config);
    return 0;
}
/* 
 * test_oqc - OQC检测
 * @ 
 */
int test_oqc(int mode)
{
    int result;
//	int icsq = 0;
    uint i;
    uint first=1,again_flag = UINT_MAX;
    uint testResult = 0;
    FAC_TEST_OPT *p;
//    uint phase;
	//显示上次老化结果
    s_aging_log log;
	memset(&log, 0x00, sizeof(s_aging_log));
    uchar time[8];
//	show_aging_log_new(0);

//    phase = FAC_PHASE_PRODUCT;
//	if (fac_ctrl_show_tip(phase) != YES)
//	{
//		return OK;
//	}
    lcd_cls();
    lcd_Display(0,DISP_FONT_LINE0,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"OQC检(和总检流程一致)");
    lcd_Display(0,DISP_FONT_LINE1,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"按任意键继续");
    kb_GetKey(-1);

//    lcd_cls();
//    lcd_Display(0,DISP_FONT_LINE0,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"1-进入低功耗");
//    if ( kb_GetKey(-1) == KEY1 ) {
//        gSystem.lpwr.bm.enable = 0;
//        sys_SleepWaitForEvent(EVENT_KEYPRESS,1,0);
//        gSystem.lpwr.bm.enable = 1;
//    }

    lcd_cls();
    gFacTest = malloc(sizeof(FAC_STAT_t));
    if ( gFacTest == NULL ) {
        return 1;
    }
    memset((uchar *)gFacTest,0,sizeof(FAC_STAT_t));
    test_conf_init();
    gTestStage = STAGE_OQC;

//    TRACE_M("\r\n %s开始",gFacPhaseName[phase]);
    memset(time,0,sizeof(time));
	if(test_rtc_autostart(0, time)) {
        free(gFacTest);
        return ERROR;
    }
//	fac_ctrl_set_cur_phase(phase);
//    gTestStage = phase;
test_auto_final_again:
    p = (FAC_TEST_OPT *)&gMenuFinalTest[0];
    for ( i=0 ; i<DIM(gMenuFinalTest) ; i++,p++  ) {
        if (test_conf_require(p->id)
            && if_test_again(again_flag,p->id))
        {
            result = p->test_func(p->para);
            test_write_result(result,p->id,&testResult);
        }
    }
//    init_sram(0);
    lcd_Cls();
	kb_Flush();
    if (!testResult) {
//        fac_ctrl_set_test_detail(phase,testResult);
//        fac_ctrl_set_autotest_res(phase, FAC_CTRL_SUCCESS);
        lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "%s通过","OQC");
        kb_GetKey(-1);
        free(gFacTest);
        return OK;
    } else {
//        fac_ctrl_set_test_detail(phase,testResult);
//        fac_ctrl_set_autotest_res(phase, FAC_CTRL_FAIL);
        if(test_fail_view(testResult)) {
            TRACE_M("\r\n 当前result:%x flag:%x",testResult,again_flag);
            if ( first == 1 ) {
                first = 0;
                again_flag = (~again_flag) | testResult;
            } else {
                again_flag = testResult;
            }
            TRACE_M("\r\n 重新result:%x flag:%x",testResult,again_flag);
            goto test_auto_final_again;
        }
        free(gFacTest);
        return ERROR;
    }
}

#if PRODUCT_NAME == PRODUCT_EPP 
int fac_openApp(void)
{
    int ret;

    lcd_Cls();
    ret = fac_ctrl_get_autotest_res(FAC_PHASE_ASSEMBLE);
    if ((ret!=FAC_CTRL_SUCCESS)
        && (ret!=FAC_CTRL_FORCE_SUC) )
    {
        lcd_Display(0,DISP_FONT_LINE1,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"装配检未通过");
        kb_getkey(-1);
    }else{
        if ( test_sram_auto(0) == OK ) {
            lcd_Display(0,DISP_FONT_LINE1,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"1-开启应用");
            if ( kb_getkey(-1) == KEY1 ) {
                if ( set_appenable(1) == 0 ) {
                    fac_ctrl_set_fac_ctrl_flag(1);
                    sys_Reset();
                    while(1);
                }else
                    lcd_Display(0,DISP_FONT_LINE1,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"开启失败");
                kb_getkey(-1);
            }
        }else{
            lcd_Display(0,DISP_FONT_LINE1,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"防拆异常");
            kb_getkey(-1);
        }
    }
    return 1;
}
#endif
/* 
 * fac_menu_top - [GENERIC] 第1级菜单 
 *    mode: 0-生产测试菜单  1-单项测试菜单
 * @ 
 */
int fac_menu_top(int mode)
{
    extern const VERSION_INFO_NEW gCtrlVerInfo;
    gFacTest = malloc(sizeof(FAC_STAT_t));
    if ( gFacTest == NULL ) {
        return 1;
    }
    memset((uchar *)gFacTest,0,sizeof(FAC_STAT_t));
    test_conf_init();
    if ( mode ) {
        local_test_menu(0);
    } else {
        while ( 1 ) {
            kb_flush();
            lcd_Cls();
            lcd_Display(0,DISP_FONT_LINE0,DISP_FONT|DISP_MEDIACY|DISP_INVLINE,"%s",FAC_MENU_CTITLE_TOP);
            lcd_Display(0,DISP_FONT_LINE1,DISP_FONT,"1-调试 2-装配 3-总检");
            lcd_Display(0,DISP_FONT_LINE2,DISP_FONT,"4-老化 5-单项 6-蓝牙");
#if PRODUCT_NAME == PRODUCT_MPOS
            lcd_Display(0,DISP_FONT_LINE3,DISP_FONT,"7-管控 ");
#else

            lcd_Display(0,DISP_FONT_LINE3,DISP_FONT,"7-管控 8-开启应用");
#endif
            sys_auto_poweroff(POWEROFF_OPEN,FAC_AUTO_POWEROFF_TIME);
            switch ( kb_GetKey(-1) )
            {
            case KEY1 :
//                debugging(0);
                break;
            case KEY2 :
                test_auto_assemble(0);
                break;
            case KEY3 :
                test_auto_final(0);
                break;
            case KEY4 :
                sys_auto_poweroff(POWEROFF_CLOSE,FAC_AUTO_POWEROFF_TIME);
                test_aging_new(0);
                sys_auto_poweroff(POWEROFF_OPEN,FAC_AUTO_POWEROFF_TIME);
                break;
            case KEY5 :
                local_test_menu(0);
                break;
            case KEY6 :
#ifdef CFG_BLUETOOTH
                test_bt(0);
#endif
                break;
            case KEY7:
                dl_process(0);
                break;
            case KEY8:
#if PRODUCT_NAME == PRODUCT_MPOS
                show_aging_log_new(0);
#else
                fac_openApp();
#endif
                break;
            case KEY_CANCEL:
                break;
            default :
                break;
            }
        }
    }
    free(gFacTest);
    gFacTest = NULL;
    sys_auto_poweroff(POWEROFF_CLOSE,FAC_AUTO_POWEROFF_TIME);
    return 0;
}
//int show_test_auto_res(char *title, int phase,FAC_TESTINFO facinfo)
//{
//    const char *prompt[] = {"未测试","成功","失败","强制成功"};
//    int j,k,flg;
//    lcd_cls();
//    lcd_display(0, DISP_FONT_LINE0, DISP_FONT | DISP_CLRLINE | DISP_MEDIACY | DISP_INVLINE, "%s",title);
//    if (phase == 0)
//    {
//        lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"主板号:%s",facinfo.mbno);
//        lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"流水号:%s",facinfo.voucherno);
//        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"模块:%x:%x:%x:%x",facinfo.module_res[0],facinfo.module_res[1],facinfo.module_res[2],facinfo.module_res[3]);
//        lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"管控信息:%d",facinfo.fac_ctrl_flag);
//    }
//    else
//    {
//        phase -= 1;
//        j = facinfo.auto_res[phase];
//        k = (j>>30)&0x03;
//        lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_MEDIACY,"%s-%04XH",prompt[k],j);
//        if ( phase > FAC_PHASE_ASSEMBLE 
//             && (j == FAC_CTRL_FAIL)) {
//            flg = 0;
//            lcd_Goto(0,DISP_FONT_LINE2);
//            if ( !IFBIT(k,AUTO_TEST_SHIFT_SRAM) ) {
//                if ( phase == FAC_PHASE_PRODUCT) {
//                    lcd_Printf("防拆 ");
//                    flg = 1;
//                }
//            }
//            if ( !IFBIT(k,AUTO_TEST_SHIFT_KEY) ) {
//                lcd_Printf("按键 ");
//                flg = 1;
//            }
//            if ( !IFBIT(k,AUTO_TEST_SHIFT_LCD) ) {
//                lcd_Printf("按键 ");
//                flg = 1;
//            }
//            if ( !IFBIT(k,AUTO_TEST_SHIFT_MAG) ) {
//                lcd_Printf("磁卡 ");
//                flg = 1;
//            }
//            if ( !IFBIT(k,AUTO_TEST_SHIFT_IC) ) {
//                lcd_Printf("IC卡 ");
//                flg = 1;
//            }
//            if ( flg ) {
//                lcd_Printf("故障 ");
//            } else {
//                lcd_Printf("成功 ");
//            }
//        }
//    }
//    kb_getkey(-1);
//    return 0;
//}

//后台设置生产管控检成功标识 0-失败 1-成功
//int fac_set_facctrl_mode(char mode)
//{
//    int ret;
//    const char *prompt[]={"成功","失败"};
//    ret = backdoor_password_verify("456789");
//    if ( ret ) {
//        return ERROR;
//    }
//    while(1)
//    {
//        lcd_cls();
//        lcd_display(0,DISP_FONT_LINE0,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,"管控检设置");
//        lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"状态:%s",fac_ctrl_get_fac_ctrl_flag()?prompt[0]:prompt[1]);
//        lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"1-强制成功");
//        lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"2-强制失败");
//        switch(kb_getkey(-1))
//        {
//        case KEY1:
//            fac_ctrl_set_fac_ctrl_flag(1);
//            return OK;
//        case KEY2:
//            fac_ctrl_set_fac_ctrl_flag(0);
//            return OK;
//        default:
//            break;
//        }
//    }
//}
/* 
 * fac_main - [GENERIC] 管控信息查询 
 * @ 
 */
//int fac_disp_log(int mode)
//{
//	FAC_TESTINFO facinfo;
//    const char *prompt[]={" ","未"};
//    const char *title[]={"生产信息","调试","装配","总检","老化"};
//    int i,key;
//	while (1)
//	{
//        memset(&facinfo, 0, sizeof(FAC_TESTINFO));
//
//        fac_ctrl_get_fac_testinfo(&facinfo);
//		lcd_cls();
//		lcd_Display(0, DISP_FONT_LINE0, DISP_FONT | DISP_CLRLINE | DISP_MEDIACY | DISP_INVLINE, "生产管控信息");
//        lcd_Display(0, DISP_FONT_LINE1, DISP_FONT | DISP_CLRLINE, "1-%s-%s通过",title[0],
//                    (facinfo. fac_ctrl_flag == 1)?prompt[0]:prompt[1]);
//        for ( i=1 ; i<DIM(title) ; i++ ) {
//            lcd_Display(LCD_LENGTH/2*((i-1)%2), DISP_FONT_LINE2+DISP_FONT*((i-1)/2), DISP_FONT, "%d-%s",i+1,title[i]);
//        }
//        key = kb_getkey(-1);
//        if ( key >= KEY1 && key <= KEY4 ) {
//            key -= KEY1;
//            show_test_auto_res((char *)title[key],key,facinfo);
//        } else if(key == KEY5){
//            show_aging_log_new(0);
//        } else if(key == KEY0){
////            fac_ctrl_set_cur_phase(FAC_PHASE_DEBUG);
////            fac_ctrl_set_autotest_res(FAC_PHASE_DEBUG,0);
//        } else if(key == KEY_F1){
//            if ( read_user_flag() == USER_SUPER) {
//                fac_set_facctrl_mode(0);
//            }
//        } else if(key == KEY_CANCEL){
//            return -1;
//        }
//	}
//}
/* 
 * fac_main - [GENERIC] 生产测试主流程
 * @ 
 */
int fac_main (int mode)
{
    // 判断管控检是否成功,进入生产测试
    uint32_t lwpr_back;
    
    
    lwpr_back = gSystem.lpwr.periph;
        // 判断管控检是否成功,进入生产测试
    if(gwp30SysMemory.SysCtrl.bit.factoryisok == 1)
    {
        return 0;
    }
    else
    {
        gSystem.lpwr.bm.enable = 1;
    }
    
    
    hw_led_on(LED_ALL);
    power_down_switch_set(OFF);  //关闭关机电源检测
    fac_mb_main(0);
 //   fac_menu_top(0);
    gSystem.lpwr.periph = lwpr_back;
    power_down_switch_set(ON);  //开启关机电源检测
    hw_led_off(LED_ALL);
    
    return 0;
}		/* -----  end of function fac_main  ----- */


#endif



