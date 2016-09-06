/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : misc.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 10/7/2014 4:40:04 PM
 * Description        : 
 *******************************************************************************/
#ifndef __MISC_H__
#define __MISC_H__

// 管理员等级
#define     USER_NORMAL        0x00
#define     USER_MANAGE        0x01
#define     USER_SUPER         0x02

void beep_run(void);
void beep_kb_nonblock(void);
void beep_powerswitch_nonblock(uint8_t time_10ms);

void power_module_init(void);
void PORTB_PowerChargeNotice_IRQHandler(void);

int sys_get_version_adc(void);

void lcd_init(void);

void s_com_gpio(char mode);
/*--------------------------poweroff module---------------------------------- */
inline void auto_poweroff_update_time(uint32_t timeout_ms)
{
    //powerdown timeout counter(unit:10ms):MAX 10.9 minutes
//    if (timeout_ms >= 0xFFFF*10)//uint16_t
//    {
//        gwp30SysMemory.PowerOffTimerCnt10ms = 0xFFFF;
//    } else
//    {
//        gwp30SysMemory.PowerOffTimerCnt10ms =  timeout_ms/10;
//    }
    gwp30SysMemory.PowerOffTimerCnt10ms =  (timeout_ms&0xFFFF);
}

inline void auto_poweroff_open(uint32_t timeout_ms)
{
    //powerdown timeout counter(unit:10ms):MAX 10.9 minutes
    if (timeout_ms >= 0xFFFF*10)//uint16_t
    {
        gwp30SysMemory.PowerOffTimerTotalTm = 0xFFFF;
    } else
    {
        gwp30SysMemory.PowerOffTimerTotalTm = timeout_ms/10;
    }
    auto_poweroff_update_time(gwp30SysMemory.PowerOffTimerTotalTm);
    gwp30SysMemory.SysTickDeamon.bit.poweroff = TRUE;
}

inline void auto_poweroff_close(void)
{
    gwp30SysMemory.SysTickDeamon.bit.poweroff = FALSE;
}


extern const char * const k_TermName;
extern const char * const k_PinpadName;
extern const VERSION_INFO_NEW gCtrlVerInfo;
extern const char *const k_SonTermName;

void cpu_init(void);
void ctrl_init(void);
int s_lcdgetMenuLang(void);
int s_getProduct(void);
uint sys_start(void);
#if defined(CFG_SOFTPWRON)
void sys_poweron_ifvalid(uint32_t pretime,uint32_t timeout_ms);
#endif
int32_t sys_check_version(int mode);
int set_attack_menu(int mode);
int sys_get_version(int mode);
//int sys_set_hardware_info(int para);
int sys_format_syszone(int mode);
int read_user_flag(void);
int set_user_flag(int mode);
int user_manage(int mode);
int get_hardware_version(void);
int backdoor_password_verify(char *psw);
int sys_tamper_init(uint32_t mode);
int sys_tamper_open(uint32_t mode);
void pinpad_open(uint32_t mode);

#endif

