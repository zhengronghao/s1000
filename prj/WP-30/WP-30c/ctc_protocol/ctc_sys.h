/*
 * =====================================================================================
 *
 *       Filename:  ctc_sys.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/12/2016 3:38:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#ifndef __CTC_SYS_H__
#define __CTC_SYS_H__ 

#include "ParseCmd.h"
#include "wp30_ctrl.h"
#include "..\..\..\..\lib\drivers\time\rtc-lib.h"

#define EXFLASH_NUMBER_MAX     128
#define EXFLASH_DATA_LEN_MAX   512

//#define MDF_MASK 0x7F01FC 
#define MDF_MASK 0x7F00FC 

// download file type
#define DOWNLOAD_TYPE_CTRL 0x00     // 下载监控命令 
#define DOWNLOAD_TYPE_SAPK 0x01     // 下载定制APK公钥 
#define DOWNLOAD_TYPE_GAPK 0x02     // 下载固件APK公钥 

//  safemanage type
#define SAFEMANAGE_TYPE_ORIGIN      0x00       // 切机管理 
#define SAFEMANAGE_TYPE_TAMPERDIS   0x01       // 防拆解除管理 
#define SAFEMANAGE_TYPE_SUPERAUTH   0x02       // 特权模式 
//  safemanage mode 
#define SAFEMANAGE_MODE_IC          0x00       // IC卡模式 
#define SAFEMANAGE_MODE_AUTHEPAG    0x01       // 权限包模式 

//协议 低功耗
#define LOWPOWER_DELAYED 2000

extern uint32 Lowpower_Timer;

typedef struct _SYSTICK_DEAMON_
{
    uint8_t flash_flag;
    uint8_t flash_led;
}sys_tick_deamon;


typedef struct _LED_STATUS
{
    uint8_t type;           //是否控制该灯 1-控制 0-不控制
    uint8_t ctr;            //灯的亮灭 1-亮 0-灭
    //uint8_t flash; //是否闪烁 0-正常 1-闪烁
    uint8_t rfu[1]; //填充
}led_status;


typedef struct _LED_OPT
{
    led_status status;      //灯状态
    uint16_t flashing_times;//闪烁次数 
    uint16_t flashing_cyc;  //闪烁周期 单位10ms
    uint16_t Ontime;        //亮灯时间 单位10ms
    uint16_t time_count;    //时间计数
}led_opt;

typedef struct _CTRL_SYSMEMORY_LED
{
    led_opt led;
    sys_tick_deamon sys_tick;
}MK210_sys_memory;

typedef struct _DL_FILE
{
    int type;             //文件类型
    uchar version[32];    //文件版本
    int len;              //文件长度
    uchar sha[32];        //sha校验
}dl_file;

typedef struct _DL_info
{
    dl_file file_info;      //文件信息
    
    int current_offset;     //当前请求的偏移
    int current_len;     //当前请求的偏移
    SHA256Context sha;

}dl_info;

typedef struct _UPDATE_INFO
{
    int data_len;  //监控大小 单位字节
    int flag;      //升级标志 0xaa-有新的升级文件 0x55-不可升级
}update_info;

extern dl_info sys_dl_info;

extern MK210_sys_memory gled_sys_memory;

//LED
uint16_t sys_manage_led(uint8_t* data);
//BEEP
uint16_t sys_manage_beep(uint16_t time,uint16_t frequency);

uint16_t terminal_info_soft(uint32_t *len,uint8_t* buffer);

uint16_t terminal_info_hard(uint32_t *len,uint8_t* buffer);
//RTC
uint16_t sys_manage_get_rtc(struct rtc_time* tm);

uint16_t sys_manage_set_rtc(struct rtc_time* tm);

//EXFLASH
uint16_t sys_manage_exflash_write(uint8_t mode,uint16_t number,uint16_t data_len,uint8_t* data);
uint16_t sys_manage_exflash_read(uint8_t mode,uint16_t number,uint16_t data_len,uint8_t* data);

//MDF
uint16_t sys_mdf_status_get(int *status, uchar *time);
uint16_t sys_mdf_unlock(void);

//download
uint16_t sys_manage_download_inform(uint8_t *data,uint8_t *buffer,uint32_t *data_len);
uint16_t sys_manage_download_data(uint8_t *data,uint8_t *buffer,uint32_t *buffer_len);
uint16_t sys_manage_download_finish(uint8_t *data);

//cpu manage
uint16_t sys_cpu_get_inform(uint8_t *data,uint8_t *output,uint32_t *data_len);
uint16_t sys_cpu_set_inform(uint8_t *input, int input_len);

//sys state
uint16_t sys_entry_lowpower(uint8_t* data);
void sys_power_state_inform(void);

// sys safe
uint16_t sys_safe_manage(uint8_t* data, int datalen, uint8_t* output, uint32_t *outlen);
#endif /*end __CTC_SYS_H__*/


