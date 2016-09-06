#include "wp30_ctrl.h"
#include "S1000_boardtest.h"
#include "boardtest.h"

extern STRUCT_WORKINFO gFacTesterInfo;
int Android_state;
//prn
static STRUCT_PIN_LIST gPrnPinList[] =
{
	{"PRN_nLAT", 		"TP46",	ID_PRN_LAT, 	PRN_nLAT, 		0 ,0, 0},
	{"PRN_STB1", 		"TP37",	ID_PRN_STB, 	PRN_STB, 		0 ,0, 0},
	//{"PRN_PE", 		    "TP45",	ID_PRN_PAPER, 	PRN_PE, 		0x60 ,0, 0},
	//{"PRN_TM", 		    "TP50",	ID_PRN_TEMP, 	PRN_TM, 		0x60 ,0, 0},
    {"PRN_M_A", 		"TP52",	ID_PRN_MTA, 	PRN_MTA, 		0 ,0, 0},
	{"PRN_M_nA", 		"TP51",	ID_PRN_MTAN, 	PRN_MTAN, 		0 ,0, 0},
	{"PRN_M_B", 		"TP54",	ID_PRN_MTB, 	PRN_MTB, 		0 ,0, 0},
	{"PRN_M_nB", 		"TP53",	ID_PRN_MTBN, 	PRN_MTBN, 		0 ,0, 0},
    {"PRN_MOSI", 		"TP48",	ID_PRN_DATA, 	PRN_MOSI, 		0 ,0, 0},
	{"PRN_CLK", 		"TP47",	ID_PRN_CLK, 	PRN_CLK, 		0 ,0, 0},
};

static STRUCT_PIN_LIST gAndPinList[] =
{
//	{"AND_WHAKE_UP", 		"NULL",	Android_WAKE_UP, 	Android_WAKE_UP, 		0 ,0, 0},
	{"AND_HEARTBEAT", 		"NULL",	(FAC_PIN_ID_t)3, 	Android_HEARTBEAT, 		0 ,0, 0},
};

int fac_function_test_pwr(void *module);
int fac_function_test_prn_pwr(void *module);
int fac_function_test_beep(void *module);
int fac_function_test_magcard(void *module);
int fac_function_test_ic_pull(void *module);
int fac_function_test_io_prn(void *module,void *param);
int fac_function_test_ic_read(void *module);
int fac_function_test_rfcard(void *module);
int fac_function_test_rtc(void *module);
int fac_function_test_rtc_set(void *module);
int fac_function_test_and_uart(void *module);
int fac_function_test_and_gpio(void *module);
int fac_function_test_and_trumpet(void *module);
int fac_function_test_and_charging(void *module);
/*-----------------------------------------------------------------------------}
 *  模块配置
 *-----------------------------------------------------------------------------{*/
const STRUCT_FAC_MODULE gDeviceModuleList[MODULE_AMOUNT] = {
    {MODULE_NAME_PWR,     MODULE_PWR,     0,                  NULL,        NULL,   fac_function_test_pwr      },
    {MODULE_NAME_PRN,     MODULE_PRN,     DIM(gPrnPinList),   gPrnPinList, fac_function_test_io_prn,  NULL  },
    {MODULE_NAME_BEEP,    MODULE_BEEP,    0,                  NULL,        NULL, fac_function_test_beep},
    {MODULE_NAME_PRN_ADC, MODULE_PRN_ADC, 0,                  NULL,        NULL, fac_function_test_prn_pwr},
    {MODULE_NAME_MAGCARD, MODULE_MAGCARD,  0,                 NULL,        NULL, fac_function_test_magcard},
    {MODULE_NAME_RFID,    MODULE_RFID,     0,                 NULL,        NULL, fac_function_test_rfcard},
    {MODULE_NAME_ICCARD,  MODULE_ICCARD,   0,                 NULL,        NULL, fac_function_test_ic_read},
    {MODULE_NAME_ICCARD,  MODULE_ICCARD_PULL,   0,            NULL,        NULL, fac_function_test_ic_pull},
    {MODULE_NAME_RTC,     MODULE_RTC,      0,                 NULL,        NULL, fac_function_test_rtc},
    {MODULE_NAME_RTC_SET, MODULE_RTC_SET,  0,                 NULL,        NULL, fac_function_test_rtc_set},

    {MODULE_NAME_ANDROID_UART,     MODULE_ANDROID_UART,      0,                 NULL,        NULL, fac_function_test_and_uart},
    {MODULE_NAME_ANDROID_GPIO,     MODULE_ANDROID_GPIO,      DIM(gAndPinList),  gAndPinList, NULL, fac_function_test_and_gpio},
    {MODULE_NAME_ANDROID_CHARGING, MODULE_ANDROID_CHARGING,  0,                 NULL,        NULL, fac_function_test_and_charging},
    {MODULE_NAME_ANDROID_TRUMPET,  MODULE_ANDROID_TRUMPET,   0,                 NULL,        NULL, fac_function_test_and_trumpet},

};



extern void s_android_com_gpio(char mode);
int fac_init(int mode)
{

//    gpio_set_output(PTD10,GPIO_OUTPUT_SlewRateFast,1);
//    s_DelayMs(3000);
//    gpio_set_output(PTD10,GPIO_OUTPUT_SlewRateFast,0);
//    
    //miniUSB供电
    s_android_com_gpio(ON);
    s_DelayMs(10);// 
    gpio_set_output(TP_WORK_PTxy,GPIO_OUTPUT_SlewRateFast,0);
    gpio_set_output(TP_MOTPWR_PTxy,GPIO_OUTPUT_SlewRateFast,0);
    MOTOR_PWR_ON(); //打印机电机供电
    power_5v_open();
    //二维码模块供电
    gpio_set_output(SCAN_PTxy_EN,GPIO_OUTPUT_SlewRateFast,0); 
    gpio_set_bit(SCAN_PTxy_EN,1);
   return OK;
}


int test_err(int mode)
{
    uint8_t snbuf[16];
    uint8_t *backbuf = NULL;
    int iRet=0;

    iRet = iRet;
    memset(snbuf,0,16);
    backbuf = malloc(FLASH_SECTOR_SIZE);
    if (NULL != backbuf) 
    {
        //擦除系统信息区破坏ctrl重新下载 偏移地址需要修改11022015
        iRet = s_write_syszone(backbuf, OFFSET_BSV_BOOTINFO  , LEN_BSV_BOOTINFO,snbuf);
        free(backbuf);
    }
    return 0;
}		/* -----  end of function fac_writeinfo  ----- */


int fac_dl_boot(void *param)
{
	dl_process(0);
	return 0;
}


int fac_dl_ctrl(void *param)
{
	test_err(0);
    sys_Shutdown();
	
	return 0;
}

int fac_dl_font(void *param)
{
	dl_process(0);
	return 0;
}

/*************************************************************
功能: 锂电池的功能测试函数

注释:

这个功能函数必须要。
*************************************************************/
int fac_function_test_battery(void *module)
{
#ifdef CFG_BATTERY
    int i,mv;
    uchar buf[4];
    i = 0;
    memset(buf, 0, sizeof(buf));
    sys_GetBattery(&mv);
    buf[i++] = 0x00;
    if ( mv >= 3000 ) {
        //硬件设置大于等于3000mv为充电电路正常
        buf[i++] = 0x00;
    }else{
        buf[i++] = 0x01;
    }
    fac_send(buf, i);
//    sys_beep_pro(DEFAULT_SOUNDFREQUENCY, 1000, 1);
#endif
	return 0;
}

int fac_function_test_prn_pwr(void *module)
{
   uint16_t temp,new_val;
   uint16_t pe;

   s_DelayMs(10);
   tp_ad_init();

   new_val = (uint16_t)(drv_adcx_caculate(TP_PAP_TEM_ADC, TP_TEM_ADC_CHANEL));
   //根据恒等式 VPR  == AD/ADmax * 3.3 可得出VPR
    temp = ((new_val*3300)/TPRINT_ADCMAX_VALUE);
    new_val = (uint16_t)(drv_adcx_caculate(TP_PAP_TEM_ADC, TP_PAP_ADC_CHANEL));
    pe = ((new_val*3300)/TPRINT_ADCMAX_VALUE);
    if(temp > 3000 && temp < 3500)
    {
        if(pe > 3000 && pe < 3500)
        {
            return OK;
        } 
    }
    return ERROR;

}




//蜂鸣器测试
int fac_function_test_beep(void *module)
{
    sys_beep_pro(2000,30,YES);
    return OK;
}
//磁条卡测试
int fac_function_test_magcard(void *module)
{
    int ret;
    mag_open();

//    return OK;
    
    while(1)
    {
       ret = mag_check(0);
       if(ret == EM_SUCCESS)
       {
           return OK;
       }
    }
//    return OK;
}
//射频卡测试
int fac_function_test_rfcard(void *module)
{
    int ret;
    uint type;
    power_5v_open();
    ret = rfid_open(0);  //模块上电

    //TRACE("\r\nRFID open:%d",ret);    
    
    if(ret != RFID_SUCCESS)
    {
        return ERROR;
    }
   // while(1)
    {
        ret = rfid_poll(RFID_MODE_ISO,&type); 
        
      //  TRACE("\r\nRFID poll:%d",ret);    
        
        if(ret != RFID_SUCCESS)
        {
            ret = rfid_poll(RFID_MODE_ISO,&type); 
            if(ret != RFID_SUCCESS)
            {   
                return ERROR;
            }
        }
        else
        {
            return OK;
        }
    }
    return OK;
}

//IC 卡读卡测试
int fac_function_test_ic_read(void *module)
{
   int ret,i; 
   int cardslot;
   
   uchar buf[64];
   
   cardslot = USERCARD;
   
   icc_InitIccModule();
   icc_InitModule(USERCARD, VCC_5,ISOMODE);
	
   ret = icc_Reset(0, (uint *)&i, buf);
   
   if(!ret)
    {
        ret = icc_ExchangeData(cardslot,sizeof(gApduGet4rand), (uchar *)gApduGet4rand,&i,buf);
    }	

	return ret;
}


//IC 卡拔卡测试
int fac_function_test_ic_pull(void *module)
{
	int ret ;

   icc_InitModule(USERCARD, VCC_5,ISOMODE);
   
   while(1)
	{
        ret = icc_CheckInSlot(USERCARD);
		if(ret == -ICC_CARDOUT)
        {
			//sys_beep();
            //testcom_write(tester_com,"ICOUT",5);
            return OK;
        }	
	}
//   return OK;
}
int boardtest_prn_pwr_strobe (int mode)
{
    int i;
    for (i = 0; i < 20; i++) {
        PRN_POWER_CHARGE();
        s_DelayUs(600);
        PRN_POWER_DISCHARGE();
        s_DelayUs(600);
    }
    return 0;
}
int fac_function_test_io_prn(void *module,void *param)
{
	STRUCT_PIN_LIST *pinlist;
	int mode;

	mode = *(int *)param;
    pinlist = (STRUCT_PIN_LIST  *)module;

	if(mode == 0) //开始测试
    {
        boardtest_prn_pwr_strobe(0);          //打开激励
        if(pinlist->pin == PRN_MTB)
        {
            boardtest_set_pio_output(PRN_MTB, 0);
            boardtest_set_pio_output(PRN_MTBN, 1);
        }
        else if(pinlist->pin == PRN_MTBN)
        {
            boardtest_set_pio_output(PRN_MTBN, 0);
            boardtest_set_pio_output(PRN_MTB, 1);
        }
        else if(pinlist->pin == PRN_MTA)
        {
            boardtest_set_pio_output(PRN_MTA, 0);
            boardtest_set_pio_output(PRN_MTAN, 1);
        }
        else if(pinlist->pin == PRN_MTAN)
        {
            boardtest_set_pio_output(PRN_MTAN, 0);
            boardtest_set_pio_output(PRN_MTA, 1);
        }
    }else if(mode == 1) //结束测试
	{
		if(pinlist->pin == PRN_MTB)
    	{
       		 boardtest_set_pio_output(PRN_MTB, 0);
       		 boardtest_set_pio_output(PRN_MTBN, 0);
    	}
		else if(pinlist->pin == PRN_MTBN)
    	{
           boardtest_set_pio_output(PRN_MTBN, 0);
       		 boardtest_set_pio_output(PRN_MTB, 0);
    	}
    	else if(pinlist->pin == PRN_MTA)
    	{
        	boardtest_set_pio_output(PRN_MTA, 0);
        	boardtest_set_pio_output(PRN_MTAN, 0);
    	}
    	else if(pinlist->pin == PRN_MTAN)
    	{
        	boardtest_set_pio_output(PRN_MTAN, 0);
        	boardtest_set_pio_output(PRN_MTA, 0);
    	}
        
	}
	return 0;
}

//AD检测
int fac_function_test_pwr(void *module)
{
   // while(1)
    {
    boardtest_prn_pwr_strobe(0);          //激励
    boardtest_prn_pwr_strobe(0);          //激励
    boardtest_prn_pwr_strobe(0);          //激励
    }  
    return OK; 
}



//与RTC设置的时间进行比较
/*************************************************************/
extern uint16_t sys_manage_get_rtc(struct rtc_time* tm);
extern uint16_t sys_manage_set_rtc(struct rtc_time* tm);
int fac_function_test_rtc(void *module)
{
// mode=0 设置RTC mode=1比较RTC

    // 设置时间:20160101 00:00:00
    struct rtc_time tm;
    struct rtc_time tm1;
    uint16_t ret;
   
    tm.tm_sec =  50;
    tm.tm_min = 10;
    tm.tm_hour = 10;
    tm.tm_mday = 11;
    tm.tm_mon = 6;
    tm.tm_year = 116;
    tm.tm_wday = 1;

    TRACE("\r\nRTC测试...");

    ret = sys_manage_get_rtc(&tm1);
    if(ret != CMDST_OK)
    {
        return 1;
    }

	if ((tm.tm_year == tm1.tm_year)&& (tm.tm_mon == tm1.tm_mon) && (tm.tm_mday == tm1.tm_mday) && (tm.tm_hour == tm1.tm_hour)) // 年月日时必须一样
		{
    	   if ( (tm.tm_min != tm1.tm_min)|| (tm.tm_sec != tm1.tm_sec))
			{
    	       return 0;
    	    }
    	}
    return 1;

}

int fac_function_test_rtc_set(void *module)
{
    // 设置时间:20160710 10:10:50
    struct rtc_time tm;
    
    tm.tm_sec =  50;
    tm.tm_min = 10;
    tm.tm_hour = 10;
    tm.tm_mday = 11;
    tm.tm_mon = 6;
    tm.tm_year = 116;
    tm.tm_wday = 1;
    if(sys_manage_set_rtc(&tm) != CMDST_OK)
    {
        return 1;
    }
    return 0;
}

int test_ctc_recev_frame(uint8_t *cmd, uint32_t cmd_len,uchar* response)
{
    MCUPCK res;
    uint32_t outlen;
    uint8_t outbuf[2048];  //协议中的数据
    uint32_t ret = 0;

    TRACE_BUF("recev",cmd,cmd_len);

    ret = unpackCmd(cmd, cmd_len, &res, outbuf, &outlen);
    if(ret != RET_OK)
    {
        return ret;
    }

    if(res.cmdcls != CMD_FAC) 
    {
        return RET_UNKNOWN;
    }

    switch ( res.cmdop )
    {
    case  CMD_FAC_TEST_UART:
          return RET_OK;      
    case  CMD_FAC_TEST_GPIO:
         return RET_OK; 
     case  CMD_FAC_TEST_CHARGING:
         memcpy(response,outbuf,6);
         return RET_OK; 
     case  CMD_FAC_TEST_TRUMPET:
         return RET_OK;
     default :
        break;
    }
 return RET_OK;
}

int test_frame_receive(uchar *response,uint timeout)
{
    uint data_vail_len=0;
    int s_pos=0, e_pos=0, pos=0;
    int len,ret,i;
    uint time_cur;
    
    time_cur = sys_get_counter();
    while(sys_get_counter() < time_cur + timeout)
    {
        if (ctc_uart_dma_check()) {
            TRACE("\r\ncheck OK"); 
            for(i = 0; i < 3; i++)
            {
                pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
                data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
                ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
                if (ret == RET_OK) {
                    drv_dma_stop();
                    len = e_pos - s_pos + 1;
                    ret = test_ctc_recev_frame(gwp30SysBuf_c.work, len,response);
                    break; 
                } 
              s_DelayMs(5); 
            }
            data_vail_len = 0;
            memset(gwp30SysBuf_c.work, sizeof(gucBuff), 0);
            ctc_uart_restart();
            return ret;
        }
    }
    return RET_TIMEOUT;  
}

int fac_function_test_and_uart(void *module)
{
    MCUPCK res;
    uint8_t buffer[2]; 
     
//    ctc_uart_restart();
    res.cmdcls = CMD_FAC;
    res.cmdop = CMD_FAC_TEST_UART;
    res.len = MIN_CMD_RESPONSE - 2;
    res.sno1 = 0; 
    res.sno2 = 1; 
    ctc_send_frame(res, buffer);
    
    Android_state = test_frame_receive(NULL,500);
    return Android_state; 
}

int fac_function_test_and_gpio(void *module)
{
	STRUCT_PIN_LIST *pinlist;
    STRUCT_FAC_MODULE *fac_module;
    MCUPCK res;
	int i = 0,j = 0;
    int ret = 0;
    uint8_t buffer[3]; 
    fac_module = (STRUCT_FAC_MODULE *)module; 
    pinlist = fac_module->pinList;
    if(Android_state == RET_OK)
    {
        for(i = 0; i < fac_module->listnum; i++)
        {
            for(j = 0; j < 2; j++)
            {
                res.cmdcls = CMD_FAC;
                res.cmdop = CMD_FAC_TEST_GPIO;
                res.len = MIN_CMD_RESPONSE + 1;
                res.sno1 = 0; 
                res.sno2 = 1;
                buffer[0] = pinlist[i].pinid;
                buffer[1] = 1;            //输出
                buffer[2] = j;            //0-为低电平 1-为高电平
                ctc_send_frame(res, buffer);
                if(test_frame_receive(NULL,500) != RET_OK)
                {
                    ret |= 1<<i;
                    break;
                }

                if(gpio_set_input((GPIOPin_Def)pinlist[i].pin,PORT_Mode_IN_PDIS) != j) //实际电路反向
                {
                    ret |= 1<<i;
                }
            }
        }
        TRACE("\r\nret:%d",ret);
    }
    return ret;
}
int fac_function_test_and_trumpet(void *module)
{
     MCUPCK res;
    uint8_t buffer[2],i; 
      
    if(Android_state == RET_OK)
    { 
        res.cmdcls = CMD_FAC;
        res.cmdop = CMD_FAC_TEST_TRUMPET;
        res.len = MIN_CMD_RESPONSE - 2;
        res.sno1 = 0; 
        res.sno2 = 1; 
        ctc_send_frame(res, buffer);
        s_DelayMs(500);
        for(i = 0; i < 2; i++)
        {
            sys_beep_pro(2700,250,YES);
            sys_beep_pro(3060,250,YES);
        }

        return test_frame_receive(NULL,8000);
    }
    return OK;
}

int fac_function_test_and_charging(void *module)
{
     MCUPCK res;
    uint8_t buffer[2]; 
    uint8_t response[8];    
    uint8_t ret = 0;
    
    if(Android_state == RET_OK)
    {res.cmdcls = CMD_FAC;
        res.cmdop = CMD_FAC_TEST_CHARGING;
        res.len = MIN_CMD_RESPONSE - 2;
        res.sno1 = 0; 
        res.sno2 = 1; 
        ctc_send_frame(res, buffer);
        ret = test_frame_receive(response,500);
        if(ret != RET_OK)
        {
            return ret;
        }
        if(response[2] == 1) //协议规定 1 正常
        {
            return ret;
        }
        else
        {
            ret |= response[2] & 0x0f;  //充电错误
            ret |= (response[3] & 0x0f) << 4;
        }
    }
    return ret;
}

void test_android(void)
{

    int key,ret;
    ctc_uart_open();
    while(1)
    {
        TRACE("\r\n|--------android test---------------------|");
        TRACE("\r\n|1-uart 2-gpio 3-trumpet 4-charging       |");

        key = InkeyCount(0);
        switch(key)
        {
        case 1:
            fac_function_test_and_uart(0);
            break;
        case 2:
            fac_function_test_and_gpio((void *)&gDeviceModuleList[11]);
            break;
         case 3:
            fac_function_test_and_trumpet(0);
            break;
         case 4:
           ret =  fac_function_test_and_charging(0);
           TRACE("\r\ncharging:%d",ret);  
           break;
 
        
        
        case 5:
            
            test_frame_receive(NULL,5000);
          
            break;





        case 99:
            return;
        default :
            break;
        }
    }
}


