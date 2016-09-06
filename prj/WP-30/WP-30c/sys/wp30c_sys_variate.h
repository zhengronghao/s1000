/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30c_sys_variate.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 10/12/2014 4:01:51 PM
 * Description        : 
 *******************************************************************************/
#ifndef __WP30C_SYS_VARIATE_H__
#define __WP30C_SYS_VARIATE_H__ 
/*-----------------------------------------------------------------------------}
 * 结构体 
 *-----------------------------------------------------------------------------{*/
#define CNL_BUFSIZE_C   (32+4)
#define WRK_BUFSIZE_C   (2048+32+4)
#define DL_BUFSIZE_C    (2*1024+4)
#define BT_BUFSIZE_C    (512+4)
#define PUBBUFFER_LEN  (1024*4+64)
typedef struct _CTRL_BUF
{
//    struct LCD_SYS_DEF lcd;
    unsigned char  console[CNL_BUFSIZE_C];
    unsigned char  work[WRK_BUFSIZE_C];
    unsigned char  bt[BT_BUFSIZE_C];
    unsigned char  pub[PUBBUFFER_LEN];
//    unsigned char  stack_heap[8*1024];
}WP30BufDefCtrl;
extern WP30BufDefCtrl gwp30SysBuf_c;
typedef union _SYSTICK_DEAMON
{
    uint8_t byte;
    struct {
        uint8_t beep:1;   //beep
        uint8_t keybrd:1;//keyboard
        uint8_t charge:1;  //power charging 
        uint8_t poweroff:1;  //power off switch 
        uint8_t manuallyshutdown:1;  //keyboard daemon
    }bit;
}SYSTICK_DEAMON;
typedef union _SYS_CTRL
{
    uint8_t byte;
    struct {
        uint8_t language:1;  //菜单语言
        uint8_t keytone:1;   //按键伴音
        uint8_t rfub:2;      // 
        uint8_t factoryisok:1; //生产测试完成标识
    }bit;
}SYS_CTRL;
typedef struct _CTRL_SYSMEMORY
{
    volatile uint16_t BeepTimerCnt10ms; //unit:10ms
    volatile uint16_t PowerOffTimerCnt10ms; //powerdown timeout counter(unit:10ms):MAX 10.9 minutes
    volatile uint8_t  ChargeADTimerCnt10ms; //unit:10ms:max 2550ms=2.55s
    volatile SYSTICK_DEAMON SysTickDeamon;
    volatile SYS_CTRL SysCtrl;
    volatile uint16_t PowerOffTimerTotalTm; // powerdown total timeout  
//    volatile uint8_t  ChargeDetectionFlag;//0-using battery 1-charging
}WP30SysMemory;
extern WP30SysMemory gwp30SysMemory;

extern uint8_t *gucBuff;

void wp30c_sys_variate_init(void);

#endif

