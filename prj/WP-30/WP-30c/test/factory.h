/*
 * =====================================================================================
 *
 *       Filename:  factory.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  1/14/2015 3:17:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yehf (), yehf@itep.com.cn
 *        Company:  START
 *
 * =====================================================================================
 */
#ifndef __FACTORY_H__
#define __FACTORY_H__ 
// 测试项


enum {
    STAGE_Debuging = FAC_PHASE_DEBUG,  //板卡检
    STAGE_Assemble = FAC_PHASE_ASSEMBLE,      //装配检
    STAGE_Aging = FAC_PHASE_AGING,         //老化检
    STAGE_Product = FAC_PHASE_PRODUCT,
    STAGE_Individual,    //单项检
    STAGE_Bottomboard,   //底板测试
    STAGE_OQC,           //OQC抽检
    STAGE_Err
};

typedef struct
{
	uint module_config;     //对应模块是否在自动测试中测试
	uchar mag_success_num;  //磁卡连续刷几次算通过
	uchar sam_test_num;     //sam应测几张卡
	uchar icsq;             //无线信号值
	uchar ispecial;         //是否为特殊制造工单的老化流程
	ushort aging_charge_mv;   //老化测试时,电池应充电到多少电压
	ushort aging_discharge_mv;//老化测试时,电池放电完电压应大于多少
	ushort aging_discharge_time;//老化测试电池应放电多久
    uchar rfu[2];
}S_TEST_CONFIG;


typedef struct __FAC_STAT_t {
    uchar stage;         // 测试阶段
    uchar iccardinslot;  // 1-卡在位  0-卡不在位
    uchar charge_err;
    uchar rfu;
    uchar time[8];
    int   iccardret[5];
    S_TEST_CONFIG tConfig;
}FAC_STAT_t;				/* ----------  end of struct FAC_STAT_t  ---------- */

typedef int (*FAC_TEST_CALLBACK)(int param);
typedef struct _FAC_TEST_OPT
{
    uint id;
    uint para;
    int (*test_func)(int);   //测试方法
}FAC_TEST_OPT;


/*-----------------------------------------------------------------------------}
 *  调试宏
 *-----------------------------------------------------------------------------{*/
#define DEBUG_FAC
#if defined (DEBUG_FAC)
#define TRACE_M(...)							TRACE(__VA_ARGS__)
#define DISPBUF_M(ptitle, len, mode,pBuf)		DebugDispbuf(ptitle, len, mode,pBuf)
#else
#define TRACE_M(...)							do{}while(0)
#define DISPBUF_M(ptitle, len, mode,pBuf)		do{}while(0)
#endif

/*-----------------------------------------------------------------------------}
 *  全部变量声明
 *-----------------------------------------------------------------------------{*/
extern FAC_STAT_t  *gFacTest;
#define gTestStage gFacTest->stage
#define ic_not_in_slot gFacTest->iccardinslot
#define battery_charge_err gFacTest->charge_err


/*-----------------------------------------------------------------------------}
 *  函数声明
 *-----------------------------------------------------------------------------{*/
extern int fac_main (int mode);
extern int test_serial_self(int mode);
extern int test_rtc_autocheck(int mode);
extern int test_battery(int mode);
extern int test_rtc(int mode);
extern int test_usb_manual(int mode);
extern int test_keyboard(int mode);
extern int test_display(int mode);
extern int test_led(int mode);
extern int test_bt(int mode);
extern int test_magcard_auto(int mode);
extern int test_rfid_auto(int mode);
extern int test_iccard_auto_new(int mode);
extern int test_rfid(int mode);
extern int local_test_menu(int mode);
extern int fac_menu_top(int mode);
extern int init_sram(int mode);
extern int test_sram_auto(int mode);
extern void BeepErr(void);
extern void TestEndHookFail(void);
int test_conf_init(void);
int test_conf_require(int module);
int test_conf_mag_success(void);
int test_conf_sam_num(void);
int test_conf_disp(int mode);
int test_conf_aging_discharge_mv(void);
int test_conf_aging_charge_mv(void);
int test_conf_aging_discharge_time(void);
int Set_conf_aging_dicharge_time(int time);
int test_conf_ic_retest_count(void);
int test_conf_icsq(void);
int test_conf_aging_special(void);
int fac_disp_log(int mode);
int test_oqc(int mode);
#endif


