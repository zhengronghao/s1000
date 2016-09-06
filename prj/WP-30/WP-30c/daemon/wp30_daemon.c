/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30_daemon.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 10/12/2014 3:54:51 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
#include "wp30_daemon.h"
#include "./WP-30c/ctc_protocol/ctc_sys.h"
#include "./WP-30/common/battery/battery.h"


/*------------------------systemtick background module------------------------------ */
static void systick_daemon_beep(void)
{
    if (gwp30SysMemory.SysTickDeamon.bit.beep)
    {
        if (gwp30SysMemory.BeepTimerCnt10ms > 0)
        {
            gwp30SysMemory.BeepTimerCnt10ms--;
            if (gwp30SysMemory.BeepTimerCnt10ms == 0)
            {
                drv_beep_close();  
                gwp30SysMemory.SysTickDeamon.bit.beep = 0;
            }
        }
    }
}

static void systick_daemon_led(void)
{
   if(gled_sys_memory.sys_tick.flash_flag == 1)
   {
       gled_sys_memory.led.time_count--;
       if(gled_sys_memory.led.time_count == (gled_sys_memory.led.flashing_cyc - gled_sys_memory.led.Ontime) )
       {
           hw_led_off(gled_sys_memory.led.status.type & gled_sys_memory.sys_tick.flash_led); //受控灯全部灭
       }
       else if(gled_sys_memory.led.time_count == 0)
       {
           if(gled_sys_memory.led.flashing_times <= 1)
           {
               gled_sys_memory.sys_tick.flash_flag = 0;
           }
           else
           {
               hw_led_on(gled_sys_memory.led.status.type & gled_sys_memory.sys_tick.flash_led); //受控灯全部亮
               gled_sys_memory.led.flashing_times--;
               gled_sys_memory.led.time_count = gled_sys_memory.led.flashing_cyc;
           }
       }
   }
}

static void systick_daemon_power_down(void)
{
    power_wake_up_check();
    power_down_check();
}

const IRQ_CALLBACK  SysTickCallBack[] = {
    systick_daemon_beep,
    systick_daemon_led,
#ifndef PRO_S1000_V100
    systick_daemon_power_down,
#endif
    NULL
};


/*--------------------------kb background module---------------------------------- */
#if !(defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD))
//wp30用
inline static void kb_manully_shutdown_tophalf(void)
{
    if (drv_kb_getkey(0) == KEY_CANCEL)
    {
//        gwp30SysMemory.SysTickDeamon.bit.manuallyshutdown = ON;
        if (gLcdSys.SysOperate.bit.enable)
        {
//            drv_lcd_cls();
//            lcd_display(0,FONT_SIZE12*1+4,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"1-关闭机器");
//            lcd_display(0,FONT_SIZE12*2+8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"2-重启机器");
//            drv_lcd_update_switch(OFF);
            drv_lcd_cls();
            lcd_display(0,64/2-6,FONT_SIZE12|DISP_MEDIACY,"正在关机...");
            s_DelayMs(20);
            drv_beep_stop();
            s_DelayMs(100);
//            beep_powerswitch_nonblock(BEEP_PWM_TIMERCNT_POWER);
            fac_save_log(1);
            if (gLcdSys.SysOperate.bit.enable)
            {
                gLcdSys.SysOperate.bit.enable = 0;
#if defined(CFG_SOFTPWRON)
                hw_lcd_backlight(LCD_BACKLIGHT_ON);
#else
                hw_lcd_backlight(LCD_BACKLIGHT_OFF);
#endif
                drv_lcd_update_switch(ON);
//                drv_lcd_cls();
            }
            if (power_ifcharging() == 1)  //charging
            {
                NVIC_SystemReset();
            } else //using battery
            {
                hw_lcd_backlight(LCD_BACKLIGHT_ON);
#ifdef CFG_LED
                hw_led_off(LED_ALL);
#endif
                power_keep_down();
            }
        }
    }
}
#endif

//inline static void device_shutdown_deinit(void)
//{
//    hw_led_off(LED_ALL);
//    hw_mag_gpio_output(MAG_DATA);
//    hw_mag_gpio_output(MAG_STROBE);
//    hw_mag_data_write(0);
//    hw_mag_strobe_write(0);
//    sam_set_io(0,0);
//}


//inline static void kb_manully_shutdown_bottomhalf(void)
//{
//    uint8_t reset=1;
//    if (gwp30SysMemory.SysTickDeamon.bit.manuallyshutdown == ON)
//    {
//#if 2
//        switch (drv_kb_getkey(0))
//        {
//        case KEY1:
//            reset = 0;
//        case KEY2:
//            device_shutdown_deinit();
//            beep_powerswitch_nonblock(BEEP_PWM_TIMERCNT_POWER);
//            if (gLcdSys.SysOperate.bit.enable)
//            {
//                gLcdSys.SysOperate.bit.enable = 0;
//#if defined(CFG_SOFTPWRON)
//                hw_lcd_backlight(LCD_BACKLIGHT_ON);
//#else
//                hw_lcd_backlight(LCD_BACKLIGHT_OFF);
//#endif
//                drv_lcd_update_switch(ON);
//                drv_lcd_cls();
//            }
//            if (power_ifcharging() == 1  //charging
//                || reset == 1)
//            {
//                NVIC_SystemReset();
//            } else //using battery
//            {
//                power_keep_down();
//#ifdef CFG_LED
//                hw_led_off(LED_ALL);
//#endif
//                s_DelayMs(20);
//                drv_beep_stop();
//            }
//            break;
//        case KEY_CANCEL:
//            drv_lcd_update_switch(ON);
//            drv_lcd_cls();
//            gwp30SysMemory.SysTickDeamon.bit.manuallyshutdown = OFF;
//            break;
//        default:
//            break;
//        }
//#endif
//    }
//}
static void kb_daemon_normalkey(void)
{
    beep_kb_nonblock();
#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
#else
    auto_poweroff_update_time(gwp30SysMemory.PowerOffTimerTotalTm);
#endif
//    kb_manully_shutdown_bottomhalf();
}

//KEY_CANCEL:Long press to turn the power off
void kb_daemon_longkey(void)
{
    beep_kb_nonblock();
#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
#else
    auto_poweroff_update_time(gwp30SysMemory.PowerOffTimerTotalTm);
    kb_manully_shutdown_tophalf();
#endif
}

const KB_CALLBACK gcKbCallBack = 
{
    .normalkey = kb_daemon_normalkey,
    .longkey = kb_daemon_longkey,
};

/*------------------------UART background module------------------------------ */
void uart0_app_callback(void)
{
    char ch;
    ch = hw_uart_getchar(UART0);
    QueueWrite(&sgSerialOpt[UART0].queue,(unsigned char *)&ch,1);
}

void uart1_app_callback(void)
{
    char ch;
    ch = hw_uart_getchar(UART1);
    QueueWrite(&sgSerialOpt[UART1].queue,(unsigned char *)&ch,1);
}

void uart2_app_callback(void)
{
    char ch;
    ch = hw_uart_getchar(UART2);
    QueueWrite(&sgSerialOpt[UART2].queue,(unsigned char *)&ch,1);
}

void uart3_app_callback(void)
{
    char ch;
    ch = hw_uart_getchar(UART3);
    QueueWrite(&sgSerialOpt[UART3].queue,(unsigned char *)&ch,1);
}

void uart4_app_callback(void)
{
    char ch;
    ch = hw_uart_getchar(UART4);
    QueueWrite(&sgSerialOpt[UART4].queue,(unsigned char *)&ch,1);
}
const IRQ_CALLBACK UartAppCallBack[] = {
    uart0_app_callback,
    uart1_app_callback,
    uart2_app_callback,
    uart3_app_callback,
    uart4_app_callback,
    NULL
};


