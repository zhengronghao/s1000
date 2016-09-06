/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30_boot.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/22/2014 11:16:54 AM
 * Description        : 
 *******************************************************************************/

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "wp30_boot.h"

#pragma message("|---->|^_^|->WP-30BOOT<-|^_^|<----|")
#ifdef DEBUG_BOOT 
#pragma message("|---->|^_^|->DebugOpen<-|^_^|<----|")
#endif
#if (CLK0_FREQ_HZ == 12000000)
#pragma message("|---->|>_<|->Crystal_12M<-|>_<|<----|")
#endif
#if (CLK0_FREQ_HZ == 8000000)
#pragma message("|---->|>_<|->Crystal_12M<-|>_<|<----|")
#endif

//-------------------------------------------------------------
volatile __no_init int gBootTmp@0x20000000;
// 40B
const VERSION_INFO_NEW gBootVerInfo={
	"S1000_BOOT",
	S1000_BOOT_VER ,
	__DATE__       //12B
};	
#pragma required=gBootVerInfo

WP30BOOT_BUF_DEF gSysBuf_b;

void boot_power_charge_init(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = POWER_CHARGE_GPIO;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PDIS 
        |PORT_Mode_IN_PFE;
    gpio_init.PORT_Pin = POWER_CHARGE_PINx;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(POWER_CHARGE_PINx);
    hw_gpio_init(&gpio_init);
}

void boot_cpu_init(void)
{
    /* Disable the watchdog timer */
	wdog_disable();
	/* Copy any vector or data sections that need to be in RAM */
	common_startup();
	/* Perform processor initialization */
	cpu_clk_init();
}


void boot_init(void)
{
    drv_sys_init();
    hw_systick_open();
#if defined(DEBUG_BOOT) 
    console_init(CNL_COMPORT,gSysBuf_b.console,CNL_BUFSIZE_B);
#endif
    if(get_wakeup_io_state()) //检测是否有电源键按下
    {
        //没有按wakeup键 芯片复位 或插入适配器 
        power_keep_init();//frist
    }
    s_touchscreen_gpio();
    power_reset_init();
    power_5v_init();
    power_5v_open();
//    drv_kb_open();
#if defined(CFG_LCD)
    drv_lcd_init(LCD_MODE_ALWAYSOFF);
#endif
#if  defined (CFG_LED)
    hw_led_init();//charging 
    hw_led_off(LED_ALL);
#endif
}

void boot_sys_init(void)
{
    boot_cpu_init();
    boot_init();
}




void boot_charging(void)
{
    uint8_t i,j=0,percent,flag=0;

    boot_power_charge_init();
    if (power_ifcharging() == 0)  //charging
    {
        return;
    }
    drv_lcd_init(LCD_MODE_TEMPOFF);
    drv_kb_clear();
    power_charge_full_init();
    power_charge_adc_switch_init();
    dbg_out_srs();
    i = 0;
    while (1)
    {
        if (power_charge_ifFull() || percent==100)
        {
            if (flag == 0) {
                TRACE("\nfull");
                drv_lcd_cls();
                lcd_display(0,8+10,FONT_SIZE12|DISP_MEDIACY|DISP_XORCHAR,"100%%",percent);
                lcd_display(0,48,FONT_SIZE12|DISP_MEDIACY|DISP_XORCHAR,"满电量",4*60*(100-percent)/100);
                flag++;
            }
        } else
        {
            drv_lcd_cls();
            percent = sys_get_batter_percentum();
            lcd_display(0,8+10,FONT_SIZE12|DISP_MEDIACY|DISP_XORCHAR,"%3d%%",percent);
            lcd_display(0,48,FONT_SIZE12|DISP_MEDIACY|DISP_XORCHAR,"约%d分钟充满",4*60*(100-percent)/100);
        }
        i++;
        i %= 7;
        if (i==j)
        {
            drv_lcd_setbacklight(LCD_MODE_ALWAYSOFF);
        }
        s_DelayMs(1200);
        if (drv_kb_hit())
        {
            drv_lcd_setbacklight(LCD_MODE_TIMEROUT);
            j = (i+5)%7;
            if (drv_kb_getkey(100) == KEY_CANCEL)
            {
                drv_lcd_cls();
                return;
            }
        }
        if (power_ifcharging() == 0)  //charging
        {
            drv_lcd_cls();
            power_keep_down();
        }
    }
}



int bsv_check(void)
{
    uint8_t sha[32];
    uint8_t mac[4];

	if(memcmp(gpSys->boot_info, LABLE_BOOTINFO, LABLE_BOOTINFO_LEN))
	{
#ifdef CHECK_BSV	
		HardFault(0,STR_BSV,STR_VER);
#endif
        TRACE("\n-|bsv:%d",__LINE__);
        TRACE_BUF("flash",gpSys->boot_info, LABLE_BOOTINFO_LEN);
        TRACE_BUF("label",LABLE_BOOTINFO, LABLE_BOOTINFO_LEN);
        return -1;
	}
	if(OK == GetCodeSignature(sha,mac,sys_get_ctrllen(0)))
	{
		if(OK == memcmp(sha,gpSys->boot_sha2,LEN_BSV_SHA))
		{
			if (OK == memcmp(mac,gpSys->boot_mac,LEN_BSV_MAC))
			{
				CLRBUF(sha);
				CLRBUF(mac);
                TRACE("\n-|bsv OK");
				return 0;
			} else
            {
                CLRBUF(sha);
                CLRBUF(mac);
#ifdef CHECK_BSV	
                HardFault(0,STR_BSV,STR_MAC);
#endif
                TRACE("\n-|bsv:%d",__LINE__);
            }
		}
        TRACE("\n-|bsv:%d",__LINE__);
	}
    CLRBUF(sha);
    CLRBUF(mac);
#ifdef CHECK_BSV	
	HardFault(0,STR_BSV,STR_SHA);
#endif
	return -1;
}



const struct LocalDlCallBack gcLocaldlCallBack_boot = 
{
    .open_comport = dlcom_open,
    .close_comport = dlcom_close,
    .check_buffer = dlcom_check_readbuf,
    .clear_buffer = dlcom_clear,
    .read_data = dlcom_read,
    .write_data = dlcom_write,
    .save_data = s_localdl_save_data,
    .save_sn = s_localdl_save_sn,
    .get_terminal_info = s_localdl_get_terminal_info,
    .after_done = localdl_boot_make_check,
//    .cancel = s_localdl_cancle,
    .cancel = NULL,
    .display = s_localdl_display,
    .fac_ctrl = NULL 
};

int dlboot_process(int t_out_ms)
{
    struct LocalDlOpt localdl;
    uint8_t frame[DL_LOCAL_FRAME_SIZE+32];
    int iRet;
    uint32_t timeout;

#if defined (CFG_LCD)
    drv_lcd_setbacklight(LCD_MODE_TIMEROUT);
    lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "%s",gBootVerInfo.product);
    lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "%s",gBootVerInfo.version);
    lcd_display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "%s",gBootVerInfo.time);
    s_DelayMs(1500);
    drv_lcd_cls();
#endif
    memset(&localdl,0,sizeof(struct LocalDlOpt));
    localdl.commbuf[0] = DL_COMPORT ;
    localdl.serialbuf[0] = gSysBuf_b.download;
    localdl.seriallen[0] = sizeof(gSysBuf_b.download);

//    localdl.commbuf[1] = DL_COMUSBD ;
//    localdl.serialbuf[1] = gSysBuf_b.usbbuffer;
//    localdl.seriallen[1] = sizeof(gSysBuf_b.usbbuffer);

    localdl.frame = (struct LocalDlFrame *)frame;
    localdl.dllevel = DL_HANDSHAKE_BOOT;
    localdl.callback = &gcLocaldlCallBack_boot;

#ifdef DEBUG_Dx
    TRACE("\n-|local down");
    if (CNL_COMPORT == localdl.commbuf[0] 
//       || CNL_COMPORT == localdl.commbuf[1]
        ) {
#if defined(CFG_USBD_CDC)
        if (console_get_class() == USBD_CDC)
        {
            while (1)
            {
                if(drv_usbd_cdc_ready() == -USBD_NOTACTIVE)
                {
                    break;
                }
                s_DelayMs(800);
                TRACE("\n-|Plse CLOSE PC Serial tools:SSCOM or others!");
            }
        }
#endif
        console_close();
    }
#endif

    localdl_com_open(&localdl);
    timeout = sys_get_counter();
    while (1)
    {
        if (t_out_ms >= 0 && sys_get_counter() - timeout > t_out_ms)
        {
            break;
        }
        iRet = localdl_process(&localdl);
        if (iRet == LOCALDL_CANCEL)
        {
            break;
        } else if (iRet == OK)
        {
            break;
        }
    }
    localdl_com_close(&localdl);
#ifdef DEBUG_Dx
    if (CNL_COMPORT == localdl.commbuf[0] 
//        || CNL_COMPORT == localdl.commbuf[1]
        ) {
        s_DelayMs(1000);
        console_init(CNL_COMPORT,gSysBuf_b.console,CNL_BUFSIZE_B);
    }
#endif
    TRACE("\n-out1");
#if defined(CFG_LCD)
    drv_lcd_cls();
    drv_lcd_setbacklight(LCD_MODE_ALWAYSOFF);
#endif
    return iRet;
}

int boot_if_blackchip(void)
{
    uint32_t i;
    uint32_t *p = (uint32_t *)SA_CTRL;

    for (i=0; i<FLASH_SECTOR_SIZE/sizeof(uint32_t); i++)
    {
        if (p[i] != (uint32_t)(~0))
        {
            return NO;
        }
    }
    return YES;
}

uint boot_start(void)
{
	uint i,j;
    uchar buf[SYSZONE_LEN];
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

void boot_check_dowload(void)
{
    uint32_t prev_time,time_out;
    int key;
    uint8_t flag = 1;
    uint8_t dwn = 0;

  
    prev_time = sys_get_counter();
    if (drv_kb_ifprocessing() == 1)
    {
        time_out = 3000; //开机按住不放
        TRACE("\nkeyin");
    } else
    {
        time_out = 80;
        TRACE("\nnotkey");
    }
    while (1)
    {
        if (sys_get_counter() - prev_time > time_out)
        {
            TRACE("\n-|TimeOUT|-");
            break;
        }
        if (drv_kb_hit())
        {
            key = drv_kb_getkey(30);
            TRACE("\nkey:%x",key);
            switch (key)
            {
            case KEY_CANCEL:
                flag = 0;
                break;
            case KEY1:
            case KEY2:
                boot_power_charge_init();
                if (power_ifcharging() == 0) {
                    time_out = 10;
                    break;
                }
            case KEY0:
                dlboot_process(60*10*1000);//下载等待时间10分钟
                return;
            default:
                break;
            }
            if (drv_kb_hit() == 0) {
                break;
            }
        }
        s_DelayMs(11);
    }
//    if (boot_if_blackchip() == YES || bsv_check() != 0)
    if ((boot_if_blackchip() == YES ))
    {
        dwn = 1;
    } else 
    {
        boot_start();
        if (bsv_check() != 0)
        {
            dwn = 1;
        }
    }

    if (dwn)
    {
#ifndef DEBUG_BOOT 
        while (1)
        {
            lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "CTRL ERROR");
            if (dlboot_process(-1) == OK) {
                break;
            }
        }
#endif
        return;
    }
    if (flag)
    {
//        boot_charging();
    }
}


static void boot_power_down(void)
{
    boot_power_charge_init();
    if (power_ifcharging() == NO) 
    {
        drv_lcd_cls();
        lcd_display(0,FONT_SIZE12*2+4,FONT_SIZE12|DISP_MEDIACY,"关机...");
        s_DelayMs(500);
        drv_lcd_cls();
        drv_lcd_setbacklight(LCD_MODE_ALWAYSOFF);
        drv_lcd_update_switch(OFF);
        power_keep_down();
    }
}

//static void boot_daemon_longkey(void)
//{
//    if (drv_kb_getkey(0) == KEY_CANCEL)
//    {
//        boot_power_down();
//    }
//}



const KB_CALLBACK gcKbCallBack = 
{
    .normalkey = NULL,
    .longkey = NULL,
};

void start(void)
{
//	/* Disable the watchdog timer */
//	wdog_disable();
//        
//	/* Copy any vector or data sections that need to be in RAM */
//	common_startup();
//        
//	/* Perform processor initialization */
//	cpu_clk_init();

        /* Determine the last cause(s) of reset */
//        outSRS();
	/* Determine specific Kinetis device and revision */
//	cpu_identify();

#ifndef KEIL	
	/* Jump to main process */
	main();

	/* No actions to perform after this so wait forever */
	while(1);
#endif
}

#define UPDATE_ERROR  -1
#define UPDATE_NONE      0
#define UPDATE_OK      1

char update_ctrl(void)
{
    int i,ret;
    update_info sys_update; 
    int offset = 0;
    uchar temp_buf[1024]; 

    memcpy((char*)&sys_update,(char *)SA_CTRL_BACK-sizeof(sys_update),sizeof(sys_update));
    TRACE("\r\n升级标志:%d",sys_update.flag);
    if(sys_update.flag == 0xaa){

        hw_led_on(LED_ALL);
        //校验

        hw_flash_init();
        sys_update.flag = 0x55;
        TRACE("\r\ndata len:%d",sys_update.data_len);
        if(sys_update.data_len > LEN_CTRL){
            TRACE("\r\n长度错误");
        }
        else{
            for(i = 0;i < sys_update.data_len/sizeof(temp_buf); i++){
                memcpy((char *)temp_buf,(char *)SA_CTRL_BACK+offset,sizeof(temp_buf));
                ret = flash_write(SA_CTRL+offset,sizeof(temp_buf),temp_buf);
                if(ret != Flash_OK)
                {
                    return UPDATE_ERROR; 
                }
                offset += sizeof(temp_buf); 
            }
            memcpy((char *)temp_buf,(char *)SA_CTRL_BACK+offset,sys_update.data_len - offset);
            ret = flash_write(SA_CTRL+offset,sizeof(temp_buf),temp_buf);
            if(ret != Flash_OK)
            {
                return UPDATE_ERROR; 
            }

        }
        ret = flash_write(SA_CTRL_BACK-sizeof(sys_update),sizeof(sys_update),(uchar *)&sys_update);
        if(ret != Flash_OK)
        {
            return UPDATE_ERROR;
        }

        hw_led_off(LED_ALL);
        TRACE("update_ctrl OK !");
        return UPDATE_OK; 
    }
    return UPDATE_NONE;
}

typedef void (*pFunction)(void);
void enter_ctrl(void)
{
    uint32_t CtrlEntryAddress;
    pFunction vCtrlEntry;

#if defined(CFG_LCD)
    drv_lcd_setbacklight(LCD_MODE_ALWAYSOFF);
#endif
    while (1) 
    {
        CtrlEntryAddress = *(volatile uint32_t*) (SA_CTRL + 4);
        TRACE("SA_CTRL:%d PC:%d 0x%02X\r\n",SA_CTRL,CtrlEntryAddress,CtrlEntryAddress);
        if(CtrlEntryAddress >= SA_CTRL && CtrlEntryAddress <= SA_CTRL+LEN_CTRL)
        {
            TRACE("PC:%d 0x%02X\r\n",CtrlEntryAddress,CtrlEntryAddress);
            #ifdef DEBUG_BOOT 
            s_DelayMs(300);
            #endif
            vCtrlEntry = (pFunction) CtrlEntryAddress;	
            DisableInterrupts;
#ifdef DEBUG_BOOT 
            console_close();
#endif
            hw_systick_close();
            mcg_pee_fei();
            __set_MSP(*(volatile uint32_t*) SA_CTRL);
            vCtrlEntry();
        } else
        {
            dlboot_process(60*10*1000);//下载等待时间10分钟
            boot_power_down();
        }
    }
}

/**
 * \brief   main
 */  
int main (void)
{
    extern void dbg_boot(void);
    
    boot_sys_init();
//    dlboot_process(1*1000);       
//    boot_check_dowload();
#ifdef DEBUG_BOOT 
    dbg_boot();
#endif
    
    dlboot_process(1500);       

    power_keep_init();//frist
    
    update_ctrl(); //监控升级
    
    enter_ctrl();
    while (1)
    {
#ifdef DEBUG_BOOT 
        dbg_boot();
#endif
        dlboot_process(5*1000);       
        boot_power_down();
    }
}

/********************************************************************/


