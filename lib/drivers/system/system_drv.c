/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : system_drv.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/9/2014 8:23:28 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "drv_inc.h"

SYSTTEM_TYPE_DEF gSystem;

static void dry_sys_attr_init(void)
{
    memset(&gSystem,0x00,sizeof(gSystem));
    gSystem.timercount = 0;
}

void drv_sys_init(void)
{
    dry_sys_attr_init();
    drv_uart_init(UART0);
    drv_uart_init(UART1);
    drv_uart_init(UART2);
//    drv_uart_init(UART3);
    drv_uart_init(UART4);
}

uint32_t sys_get_counter(void)
{
    return gSystem.timercount;
}





void s_DelayUs(uint32_t us)
{
#if 0
    uint32_t total;

    if (us <= 4)
    {
        return;
    }
    total = (us-4)*core_clk_mhz; 
    if (total >= 28)
    {
        total -= 28;
    } 
    hw_cycle_open();
    hw_clear_cycle();
    while (1)
    {
        if (hw_get_cycle() >= total)
            break;
    }
    hw_cycle_close();
#else
    if (core_clk_mhz == 48)
    {
        //48M调试完成01112016
        int i;
        i = us;
        if (us <= 10)//10us
        {
            us *= 10;
            us += i;
            while ( us-- ) {
                asm("nop");
            }
            return;
        }
        us -= 2;
        us *= 11;
        if (i < 500)//500us
        {
//            i -= 800;
            if ( i < 100 ) {
                i = i*13/10;
            }else{
                i = i*11/10;
            }
        }
        us += i;//us >= 14
        us -= 10;
        while (us--)
        {
            asm("nop");asm("nop");
        }
    } else if (core_clk_mhz == 50)
    {
        if (us <= 1) 
        {
            return;
        }
        asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");//0.1us
        us -= 2;
        us *= 5;
        us += 2;
        while (us--)
        {
            asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");//0.1us
        }
    } else //120M
    {
        if (us < 3) {
            us = (((us-1)*1000+100)>>5);
        } else if (us < 2000){
            us *= 24;
            us -= 15;
        } else {
            us = us*24+us/20;
        }
        while (us--)
        {
            asm("nop");
        }
    }
#endif
}


void s_DelayMs(uint32_t CountMs)
{
//    uint32_t t;
//    while (CountMs--)
//    {
//        for (t=9996; t>0; t--);
//    }
    s_DelayUs(CountMs*1000);
}

void sys_delay_ms(uint duration_ms)
{
	uint tmp = 0;
	tmp = sys_get_counter();
	while(1)
	{
        // 进入休眠，需要增加
        enter_wait();
		if((sys_get_counter() - tmp) >= duration_ms)
		{
			break;
		}
	}
}

