/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 4* File Name          : ctrl_wp30.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 8/11/2014 3:10:20 PM
 * Description        : 
 *******************************************************************************/

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "wp30_ctrl.h"
extern uint16_t sys_manage_led(uint8_t* data);
extern uint16_t sys_manage_beep(uint16_t time,uint16_t frequency);
void main_process(void)
{
    //uchar key,stat=0;
//extern int s_InitProduct(void);
//    int i;
    uint data_vail_len=0;
    int s_pos=0, e_pos=0, pos=0;
    int len;
    int ret;
    uint8_t led_flash[10] = {0x0f,0x0f,0x0f,0x01,0,50,0,25,0};  
    
#ifdef CFG_LOWPWR
    lowerpower_init();
#endif
//    UART_Init(uart_get_bps());

#ifdef DEBUG_Dx
    drv_uart_close(WORK_COMPORT); 
    console_init(CNL_COMPORT,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
#endif
   sys_manage_led(led_flash);

   while (1)
	{		

#ifdef DEBUG_Dx
        if (IfInkey(0))
        {
            break;
        }
#endif
#ifdef CFG_LOWPWR
   sys_power_state_inform();
   enter_lowerpower_wait();
#endif

#ifdef CFG_MAGCARD
        magcard_main(0);
#endif
#ifdef CFG_SCANER
        scaner_2d_scan();
#endif
        //		PPRT_DealWith();
        if (ctc_uart_dma_check()) {
#ifdef CFG_LOWPWR
            Lowpower_Timer = sys_get_counter(); //低功耗用计时   
#endif
            pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
            data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
            ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
            if (ret == RET_OK) {
                drv_dma_stop();
                len = e_pos - s_pos + 1;
                ret = ctc_recev_frame(0, gwp30SysBuf_c.work, len);
            } else {
                sys_DelayMs(5);
                
                pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
                data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
                ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
                if (ret == RET_OK) {
                    drv_dma_stop();
                    len = e_pos - s_pos + 1;
                    ctc_recev_frame(0, gwp30SysBuf_c.work, len);
                } else {
                    sys_DelayMs(5);
                    pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
                    data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
                    ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
                    if (ret == RET_OK) {
                        drv_dma_stop();
                        len = e_pos - s_pos + 1;
                        ctc_recev_frame(0, gwp30SysBuf_c.work, len);
                        TRACE("len3 = %d\r\n", data_vail_len);
                    }
                }
            }
            data_vail_len = 0;
            memset(gwp30SysBuf_c.work, sizeof(gucBuff), 0);
            ctc_uart_restart();
        }
	}
#ifdef DEBUG_Dx
    ctc_uart_close(); 
    dbg_s1000_ctrl();
#endif
}

/**
 * \brief   main
 */  
int main (void)
{
    extern void PadMain(void);
#ifdef DEBUG_Dx
//    dbg_s1000_ctrl();
#else
#ifdef CFG_DBG_PROTOTYPE
    proto_main();
#endif
#endif
#ifdef CFG_DBG_TIME
    TRACE("\r\n\nWelcome to Enter CTRL!%s %s.",__DATE__, __TIME__);
//    dbg_time_read();
#endif
#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
    PadMain();
#endif
    main_process();
    lcd_cls();
    lcd_display(0,DISP_FONT*2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,
                "No App");
    while (1);
}

/********************************************************************/


