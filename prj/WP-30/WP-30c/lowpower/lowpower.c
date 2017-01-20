/***************** (C) COPYRIGHT 2015 START Computer equipment *****************
 * File Name          : lowpower.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 4/28/2015 16:49:45 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
#include "lowpower.h"

#ifdef CFG_LOWPWR
//31.5
int g_test_pow = 0;
const struct LpwrDevice gcLpwrDevice = {
#if (defined CFG_RFID)
    .rfid = 
    {
        {RFID_PTxy_IO,   LPWR_GPIO_IN,  LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT_UP},
        {RFID_PTxy_MOSI, LPWR_GPIO_IN,  LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT_UP},
        {RFID_PTxy_SCLK, LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {RFID_PTxy_MISO, LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {RFID_PTxy_PCSn, LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {RFID_PTxy_RST,  LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
    },
    .led = 
    {
        {LED_PTxy_BLUE,   LPWR_GPIO_OUT, LPWR_LEVEL_HIGHT, 0, LPWR_MODE_OUTPUT},
        {LED_PTxy_YELLOW, LPWR_GPIO_OUT, LPWR_LEVEL_HIGHT, 0, LPWR_MODE_OUTPUT},
        {LED_PTxy_GREEN,  LPWR_GPIO_OUT, LPWR_LEVEL_HIGHT, 0, LPWR_MODE_OUTPUT},
        {LED_PTxy_RED,    LPWR_GPIO_OUT, LPWR_LEVEL_HIGHT, 0, LPWR_MODE_OUTPUT},
    },
#endif

#if (defined CFG_W25XFLASH)
    .exflash =
    {
        {EXFLASH_PTxy_CS, LPWR_GPIO_OUT, LPWR_LEVEL_HIGHT, 0, LPWR_MODE_OUTPUT},
    },
#endif

#if (defined CFG_ICCARD_USERCARD)
    .icc = 
    {
        {IC_CLK_PTx,  LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {IC_INT_PTx,  LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {IC_SEL3_PTx, LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {IC_IO_PTx ,  LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {IC_SEL2_PTx, LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {IC_SEL1_PTx, LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {IC_RST_PTx,  LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {PTA29,       LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {IC_CS_PTx,   LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {PTC8,        LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
    },
#endif

#if (defined CFG_SCANER)
    .scan = 
    {
        {SCAN_PTxy_EN , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {UART1_RXD,     LPWR_GPIO_IN,  LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT_UP},
        {UART1_TXD ,    LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {SCAN_PTxy_TRIG,LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
    },
#endif
    
#if (defined CFG_TPRINTER)
    .prn = 
    {
        {TP_MOTPWR_PTxy , LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {TP_STB_PTxy ,    LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {TP_MnA_PTxy ,    LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {TP_MA_PTxy ,     LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {TP_MnB_PTxy ,    LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {TP_MB_PTxy ,     LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {TP_WORK_PTxy  ,  LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {TP_LATCH_PTxy ,  LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {TPRINT_PTxy_SCLK , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {TPRINT_PTxy_MOSI , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
    },
#endif

#if (defined CFG_SM2 || defined CFG_SM3 || defined CFG_SM4)
    .esam =
    {

        {IS8U256A_PTxy_RST , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {IS8U256A_PTxy_SHAKEHAND, LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {IS8U256A_PTxy_PCSn, LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {IS8U256A_PTxy_SCLK, LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {IS8U256A_PTxy_MOSI, LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {IS8U256A_PTxy_MISO, LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
    },
#endif

#if (defined CFG_MAGCARD)
    .mag =
    {
        {PTC0 , LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {PTC1 , LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {PTC2 , LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
    },
#endif

#if (defined CFG_ICCARD)
    .sam1 =
    {
        {SAM_CLK_PTx , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {SAM1_RST_PTx , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {SAM1_PWR_PTx , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {SAM1_IO_PTx , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {SAM2_RST_PTx , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {SAM2_PWR_PTx , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {SAM2_IO_PTx  , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
    },
#endif
   
    .ctp =
    {
        {PTC5 , LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {PTC6 , LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {PTC9 , LPWR_GPIO_OUT, LPWR_LEVEL_HIGHT, 0, LPWR_MODE_OUTPUT},
        {PTC10 , LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
        {PTC11 , LPWR_GPIO_IN, LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT},
    },

    .adc = 
    {
        {PTB3 , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
    },
    
    .sys = 
    {
        {PTB6 , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {PTB16 , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        //{PTD0 , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
        {PTD7 , LPWR_GPIO_OUT, LPWR_LEVEL_LOW, 0, LPWR_MODE_OUTPUT},
    },
    
    .beep =
    {
        {BEEP_PTxy,LPWR_GPIO_OUT,0,LPWR_LEVEL_LOW,LPWR_MODE_OUTPUT},
    },
  
    .uart2 =
    {
        {UART2_TXD, LPWR_GPIO_OUT, LPWR_LEVEL_HIGHT, 0, LPWR_MODE_OUTPUT},
        {UART2_RXD, LPWR_GPIO_IN,  LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT_UP},
    },

    .uart4 =
    {
        {UART4_TXD, LPWR_GPIO_OUT, LPWR_LEVEL_HIGHT, 0, LPWR_MODE_OUTPUT},
        {UART4_RXD, LPWR_GPIO_IN,  LPWR_LEVEL_LOW, 0, LPWR_MODE_INPUT_UP},
    
    }

};

void lpwr_gpio_opt(uint8_t term,struct LpwrGpioMode *opt)
{
    for ( ; term >0; term--)
    {
        if (opt->direct == LPWR_GPIO_IN)
        {
            //        TRACE("\n-input%d",term);
            gpio_set_input(opt->ptx,opt->mode); 
        } else
        {
            //        TRACE("\n-output%d",term);
            gpio_set_output(opt->ptx,opt->mode,opt->level); 
        }
        opt++;
//        InkeyCount(0);
    }
}


void lpwr_set_gpio(void)
{
#ifdef CFG_LOWPWR
#if (defined CFG_RFID)
    TRACE("\n-|rfid");
    lpwr_gpio_opt(sizeof(struct LpwrRFID)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.rfid);
#endif
#if (defined CFG_W25XFLASH)
    TRACE("\n-|exflash");
    lpwr_gpio_opt(sizeof(struct LpwrEXFLASH)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.exflash);
#endif

#if (defined CFG_ICCARD_USERCARD)
    TRACE("\n-|icc");
    lpwr_gpio_opt(sizeof(struct LpwrICC)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.icc);
#endif

#if (defined CFG_SCANER)
    TRACE("\n-|scan");
    lpwr_gpio_opt(sizeof(struct LpwrScan)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.scan);
#endif

#if (defined CFG_TPRINTER)
    TRACE("\n-|prn");
    lpwr_gpio_opt(sizeof(struct LpwrPRN)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.prn);
#endif

#if (defined CFG_SM2 || defined CFG_SM3 || defined CFG_SM4)
    TRACE("\n-|esam");
    lpwr_gpio_opt(sizeof(struct LpwrESAM)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.esam);
#endif

#if (defined CFG_MAGCARD)
    TRACE("\n-|mag");
    lpwr_gpio_opt(sizeof(struct LpwrPRN)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.mag);
#endif

#if (defined CFG_ICCARD)
    TRACE("\n-|sam1");
    lpwr_gpio_opt(sizeof(struct LpwrSAM)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.sam1);
#endif
    TRACE("\n-|adc");
    lpwr_gpio_opt(sizeof(struct LpwrAdc)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.adc);

    TRACE("\n-|sys");
    lpwr_gpio_opt(sizeof(struct LpwrSYS)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.sys);
    TRACE("\n-|beep");
    lpwr_gpio_opt(sizeof(struct LpwrBEEP)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.beep);
    TRACE("\n-|ctp");
    lpwr_gpio_opt(sizeof(struct LpwrCTP)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.ctp);
    TRACE("\n-|uart4");
//    ctc_uart_close();
//    drv_dma_stop();
//    lpwr_gpio_opt(sizeof(struct LpwrUART2)/sizeof(struct LpwrGpioMode),
//                  (struct LpwrGpioMode *)&gcLpwrDevice.uart4);
    TRACE("\n-|uart2");
    lpwr_gpio_opt(sizeof(struct LpwrUART2)/sizeof(struct LpwrGpioMode),
                  (struct LpwrGpioMode *)&gcLpwrDevice.uart2);
#endif
}
void wakeup_ctrl_init(void)
{
    power_5v_init();
    power_5v_close();
    power_3v3_init();
    power_3v3_open();

#ifdef CFG_W25XFLASH
    hw_w25x_spi_init();
    hw_w25x_gpio_init();
#endif

#if (defined CFG_MAGCARD)
    mag_init(0);
#endif
#if (defined CFG_ICCARD)
	icc_SamCardInit();
	icc_CpuCardInit();
    icc_CheckInSlot(USERCARD);
#endif

#if defined(CFG_SECURITY_CHIP)
    sc_is8u256a_init();
#endif
}

extern uint16_t rfcard_module_open(void);
void lwpr_device_reinit(void)
{
//    ctc_uart_restart();
//    drv_uart_init(UART4);
//    ctc_uart_open();
#ifdef CFG_LOWPWR
    wakeup_ctrl_init();

#if (defined CFG_SCANER)
     if (gSystem.lpwr.bm.scan) {
         com_em3096_open();
     }
#endif

#if (defined CFG_TPRINTER)
      if (gSystem.lpwr.bm.prn) {
          tp_init();
      }
#endif

#if (defined CFG_RFID)
      if (gSystem.lpwr.bm.rfc) {
//          power_5v_open();
          rfcard_module_open();
      }
#endif

#if (defined CFG_MAGCARD)
      if (gSystem.lpwr.bm.mtk) {
          mag_open();
      }
#endif

#ifdef DEBUG_Dx
    if (drv_uart_check_used(CNL_COMPORT) > 0) {
        hw_uart_init(CNL_COMPORT,CNL_BAUD);
    }
#endif
#endif
}



void lwpr_pherip_reinit(void)
{
    hw_systick_close();
    hw_systick_open();

#ifdef DEBUG_Dx
    if (drv_uart_check_used(CNL_COMPORT) > 0) {
        hw_uart_init(CNL_COMPORT,CNL_BAUD);
    }
#endif

}

void lowpower_wakeup_gpio_init(void)
{
   GPIO_InitTypeDef gpio_init;
   
   gpio_init.GPIOx = POWER_WAKE_UP_GPIO;
   gpio_init.PORT_Pin = POWER_WAKE_UP_PINx;
   gpio_init.PORT_Mode = PORT_Mode_MUX_gpio | PORT_Mode_IN_PU | PORT_Mode_IRQ_EXTI_FEdge;
   set_irq_priority(POWER_WAKE_UP_IRQn,INT_PRI_LOWPOWER_WAKEUP);
   enable_irq(POWER_WAKE_UP_IRQn);
   
   gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(POWER_WAKE_UP_PINx);
   hw_gpio_init(&gpio_init);
   
   return;
}


void lowerpower_init(void)
{
#ifdef CFG_LOWPWR
    /* Need to make sure the clock monitor(s) is disabled if using
     * an external clock source. This assumes OSC0 is used as the 
     * external clock source and that the clock gate for MCG has 
     * already been enabled.
     */
    MCG_C6 &= ~MCG_C6_CME0_MASK; //CME=0 clock monitor disable

    SMC_PMPROT =  SMC_PMPROT_AVLLS_MASK |
                  SMC_PMPROT_ALLS_MASK|
                  SMC_PMPROT_AVLP_MASK;

  lowpower_wakeup_gpio_init();

#endif
}



//void dbg_lowerpower(void)
void enter_lowerpower_wait(void)
{
#ifdef CFG_LOWPWR
    enter_wait();
#endif
}

void lwpr_pherip_close(void)
{
    ctc_uart_close();
}

void lwpr_pherip_reopen(void)
{
    ctc_uart_open();
}

void enter_lowerpower_freq(void)
{
#ifdef CFG_LOWPWR
    if(gSystem.lpwr.bm.low_flag == 1)//已经进入低功耗
    {
        return;
    }
    if (PEE != what_mcg_mode()) {
        return;
    }
//    lwpr_pherip_close();
    mcg_clk_hz  = pee_pbe(CLK0_FREQ_HZ);
    mcg_clk_hz  = pbe_blpe(CLK0_FREQ_HZ);
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(CLK_VLPR_DIV1)|SIM_CLKDIV1_OUTDIV2(CLK_VLPR_DIV2)|SIM_CLKDIV1_OUTDIV4(CLK_VLPR_DIV4);
    cpu_clk_refresh();
    lwpr_pherip_reinit();
//    enter_vlpr(0);
    lwpr_pherip_reinit();
//    lpwr_set_gpio();
//    hw_led_on(S_LED_RED);
    gSystem.lpwr.bm.low_flag = 1;
//    g_test_pow = 0;
//    TRACE("\r\n-----------------------");
//    while ( g_test_pow == 0 ) {
//    }

#endif
}


void exit_lowerpower_freq(void)
{
#ifdef CFG_LOWPWR
    
//    gSystem.lpwr.bm.low_en = 0; //有按检测到按键 则不可进入休眠 等待休眠指令
//    TRACE("gpio lever trigger\r\n");
   
    if(gSystem.lpwr.bm.low_flag == 0) //未进入休眠
    {
        return;
    }

    if (BLPE != what_mcg_mode()) {
        return;
    }
//    exit_vlpr();

    SIM_CLKDIV1 = ( 0
                    | SIM_CLKDIV1_OUTDIV1(0)    //Core/system       120 Mhz
                    | SIM_CLKDIV1_OUTDIV2(1)    //Busclk            60 Mhz         
                    | SIM_CLKDIV1_OUTDIV3(2)    //FlexBus           40 Mhz
                    | SIM_CLKDIV1_OUTDIV4(4) ); //Flash             24 Mhz
    
    mcg_clk_hz = blpe_pbe(CLK0_FREQ_HZ, PLL0_PRDIV,PLL0_VDIV);
    mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
    cpu_clk_refresh();
    lwpr_pherip_reinit();
    TRACE("-1");
    lwpr_device_reinit();
    gSystem.lpwr.bm.low_flag = 0;
    gSystem.lpwr.bm.low_to_normal = 1;
    TRACE("-2");
    lwpr_pherip_reopen();
    TRACE("-3");
//    hw_led_off(S_LED_RED);
#endif
}

void  enter_lowerpower_llwu(void)
{
#ifdef CFG_LOWPWR
//    dbg_llwu();
    llwu_configure(1<<11,0x02,0); //SIM_SCGC4_LLWU_MASK
    enable_irq(LLW_IRQn);
    enter_lls(); 
    if(what_mcg_mode()==PBE)
    {
        mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
    }
#endif
}
uint32_t sys_power_save(uint32_t Event, uint32_t TimeOutMs)
{
#ifdef CFG_LOWPWR
    uint32_t BeginCount = 0;

    BeginCount = sys_get_counter();
    while(1)
    {
        enter_lowerpower_wait();
        if((sys_get_counter() - BeginCount) >= TimeOutMs)
        {
            break;
        }
		if (KB_Hit()) 
        {
            break;
        }
        if(uart_CheckReadBuf(UART_COM1) > 0)
        {
            break;
        }
    }
#endif
    return 0;
}


int sys_SleepWaitForEvent(uint waitevent, uint mode,uint timeout_ms)
{
#ifdef CFG_LOWPWR
    uint32_t BeginCount = 0;

#if defined(CFG_LED)
    hw_led_off(LED_ALL);
#endif
    BeginCount = sys_get_counter();
    enter_lowerpower_freq();
    while(1)
    {
        enter_lowerpower_wait();
        if ((timeout_ms > 0)  
            && (waitevent&EVENT_TIMEOUT)
            && ((sys_get_counter() - BeginCount) >=  timeout_ms))
        {
            waitevent = EVENT_TIMEOUT;
            break;
        }
		if ((waitevent&EVENT_KEYPRESS) && KB_Hit()) 
        {
            s_DelayMs(BEEP_PWM_TIMERCNT_KB*10);
            waitevent = EVENT_KEYPRESS;
            break;
        }
        if ((waitevent&EVENT_UARTRECV) )
        {
            if (drv_uart_check_readbuf(CNL_COMPORT) > 0) {
                waitevent = EVENT_UARTRECV;
                break;
            }
            if (drv_uart_check_readbuf(BT_COMPORT) > 0) {
                waitevent = EVENT_UARTRECV;
                break;
            }
//            if (drv_uart_check_readbuf(WORK_COMPORT) > 0) {
//                waitevent = EVENT_UARTRECV;
//                break;
//            }
        }
#if (defined CFG_ICCARD)
        if ((waitevent&EVENT_ICCIN) && (icc_CheckInSlot(USERCARD) == ICC_SUCCESS) > 0)
        {
            waitevent = EVENT_ICCIN;
            break;
        }
#endif
#if (defined CFG_MAGCARD)
        if ((waitevent&EVENT_MAGSWIPED) && (mag_check(0) == EM_mag_SUCCESS) > 0)
        {
            waitevent = EVENT_MAGSWIPED;
            break;
        }
#endif
    }
    exit_lowerpower_freq();
    return waitevent;
#else
    return 0;   
#endif

}

void LowPower_Wakeup_IRQHandler(void)
{
    if (PORTx_IRQPinx(POWER_WAKE_UP_PORT,POWER_WAKE_UP_PINx)) {
        PORTx_IRQPinx_Clear(POWER_WAKE_UP_PORT,POWER_WAKE_UP_PINx);
        exit_lowerpower_freq(); 
        TRACE("\r\n****");
        g_test_pow = 1;
    }

}

#endif


