/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30_ctrl_dbg.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 8/11/2014 4:46:22 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
#include "wp30_ctrl_dbg.h"
#include "libapi_dbg.h"
//#include "../libdll/libdll.h"
#include "../WP-30c/emv/emv.h"


#ifdef CFG_DBG_EXFLASH
void dbg_exflash(void)
{
    uint32_t key,addr;
    uint16_t tmp=0;
    uint8_t byte;
    uint8_t buffer[2048];
    while (1)
    {
        TRACE("\r\n-|************************w25x40************************|-");
        TRACE("\r\n-|1-init 2-readID 3-readSR 4-read 5-write 6-CS  7-reset |-");
        TRACE("\r\n-|******************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            hw_w25x_spi_init();
            hw_w25x_gpio_init();
            break;
        case 2:
            tmp = hw_w25x_flash_read_ID();
            TRACE("\nID:%04X",tmp);
            break;
        case 3:
            byte = hw_w25x_flash_read_SR();
            TRACE("\nSR:%02X",byte);
            break;
        case 4:
            TRACE("\nRead length:");
            memset(buffer,0,sizeof(buffer));
            key = InkeyCount(0);
            if (key > 2048)
                key = 2048;
            TRACE("\nRead Addr");
            addr = InkeyCount(0);
//            hw_w25x_flash_read(buffer,0,key);
            hw_w25x_flash_read(buffer,addr,key);
            TRACE_BUF(NULL,buffer,key); 
            TRACE("\n Read crc16:%x",calc_crc16(buffer,key,0));
            break;
        case 5:
            TRACE("\nwrite length:");
            key = InkeyCount(0);
            if (key > 2048)
                key = 2048;
            RNG_FillRandom(buffer,key);
            TRACE("\nWrite Addr");
            addr = InkeyCount(0);
            hw_w25_flash_erase_sector(addr);
            hw_w25x_flash_write(buffer,addr,key);
            TRACE_BUF(NULL,buffer,key); 
            TRACE("\n Write crc16:%x",calc_crc16(buffer,key,0));
            break;
        case 6:
            TRACE("\n1-low  2-high");
            if (InkeyCount(0) == 1)
                hw_gpio_reset_bits(EXFLASH_GPIO_CS,(1<<EXFLASH_PINx_CS));
            else
                hw_gpio_set_bits(EXFLASH_GPIO_CS,(1<<EXFLASH_PINx_CS));
            break;
#if defined(ExFLASH_SOFT_RESET) 
        case 7:
            TRACE("\n1-low  2-high");
            if (InkeyCount(0) == 1)
                hw_gpio_reset_bits(EXFLASH_GPIO_RESET,(1<<EXFLASH_PINx_RESET));
            else
                hw_gpio_set_bits(EXFLASH_GPIO_RESET,(1<<EXFLASH_PINx_RESET));
            break;
#endif
        case 8:
            hw_w25x_gpio_init();
            break;
        case 99:
            return;
        }
    }
}
#endif

void dbg_buzzer(void)
{
    int key;

    while (1)
    {
        TRACE("\r\n-|*******************Buzzer************************|-");
        TRACE("\r\n-|1-init 2-start 3-stop 4-kb_beep                  |-");
        TRACE("\r\n-|*************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            TRACE("\n-|frequency(Hz):");
            key = InkeyCount(0);
            drv_beep_open(key);
            break;
        case 2:
            drv_beep_start();
            break;
        case 3:
            drv_beep_stop();
            break;
        case 4:
            beep_kb_nonblock();
            break;
        case 99:
            drv_beep_stop();
            return;
        }
    }
}

#ifdef CFG_DBG_LED
void dbg_led(void)
{
    uint32_t i;
    int key;

    hw_led_init();
    while (1)
    {
        TRACE("\r\n-|**********************led debug***********************|-");
        TRACE("\r\n-|1-hight 2-low 3-auto                                  |-");
        TRACE("\r\n-|******************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            TRACE("\n--|all ON");
            hw_led_on(LED_ALL);
            break;
        case 2:
            TRACE("\n--|all OFF 2 seconds");
            hw_led_off(LED_ALL);
            break;
        case 3:
            TRACE("\n--|Red->Green->Blue->Yellow");
            for (i=1; i<=0x08; i<<=1)
            {
                hw_led_on(i);
                s_DelayMs(1000);
                hw_led_off(i);
            }
            break;
        case 99:
            return;
        default:
            break;
        }
    }
}
#endif
/* 
 * Vref*[ad_val/(2^16+1)] = Vbat*[R21/(R20+R21)]
 * Vbat = Vref*(R20+R21)*ad_val/[(2^16+1)*R21]
 *      = 3.3*(47+100)*ad_val/(65536*100)
 *      = (3.3*147/6553600)*ad_val
 *      = (4851/65536000)*ad_val
 *      = (74/10^6)*ad_val
 *      = 74*ad_val/1000 mv
 * */
void dbg_power_charge_adc(void)
{
    int ad_val=0;
    uint32_t times =0;

    TRACE("\n-|ADC Test");
    while (1) 
    {
        times++;
        TRACE("\n\n-|times:%03d",times);
        power_charge_adc_open();
        s_DelayMs(20);
        ad_val = power_charge_adc_caculate();
        TRACE("\n-|battery:%04d-%dmv ",ad_val,(74*ad_val)/1000);
        s_DelayMs(20);
        power_charge_adc_close();
        s_DelayMs(500);
        ad_val = sys_get_version_adc();
        s_DelayMs(20);
        TRACE("\n-|version:%04d ",ad_val);
        if (IfInkey(0)) {
            ad_val = ad_val;
            break;
        }
    }
}

extern void proto_welcome(void);
#ifdef CFG_DBG_LCD
void dbg_app_lcd(void)
{
    int key;
//    int i =0,j;
 //   uint8_t battery;
//    char ch,assii[255];
    
    lcd_init();
    while (1)
    {
        TRACE("\r\n-|*********************lcd gui debug**********************|-");
        TRACE("\r\n-|1-lcd 2-   3-   4- 5- 6- 7-  |-");
        TRACE("\r\n-|8-clr 9-  10- 11-  |-");
        TRACE("\r\n-|*******************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            dbg_lcd();
            break;
        case 2:
            break;
#ifdef CFG_DBG_PROTOTYPE
        case 3:
            proto_welcome();
            break;
#endif
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            drv_lcd_cls();
            break;
        case 9:
            break;
        case 10:
            break;
        case 11:
            break;
        case 12:
            gui_drawbox(2,2,100,60,BoxModel_NoBox,FillType_Clear); //清扫出一片空地
            InkeyCount(0);
            gui_drawbox(2,2,100,60,BoxModel_Draw,FillType_NoFill); //窗体轮廓
            InkeyCount(0);
            gui_drawbox(3,3,99,22,BoxModel_NoBox,FillType_Clear); //反显效果，注意坐标的细节
            break;
        case 99:
            return;
        }
    }
}

#endif

const uint8_t gcFornt[] = {FONT_SIZE12,FONT_SIZE12,FONT_SIZE12};

void lcd_dbg_display(void)
{
	uint8_t x,y,i;
	uint8_t buf[256];
	int mode,reverse;
	
	
	TRACE("\r\n中文:1-12dot 2-16dot 3-24");
	switch (InkeyCount(0))
	{
		case 2:
			mode = FONT_SIZE16;
			y = 3;
			break;
		case 3:
			y = 1;
			mode = FONT_SIZE24;
			break;
		default:
			mode = FONT_SIZE12;
			y = 5;
			break;
	}
	TRACE("\r\n0-normal 1-reserver char 2-reserver line");
    reverse = InkeyCount(0);
	if (reverse==1)
	{
		reverse = DISP_INVCHAR;
		mode |= DISP_INVCHAR;
	} else if (reverse ==2)
    {
		reverse = DISP_INVLINE;
		mode |= DISP_INVLINE;
    } else
    {
        reverse = 0;
    }
	TRACE("\r\nDraw the dot:0-Auto 1-manual");
	if (InkeyCount(0) == 0x00)
	{		
		TRACE("\r\nASCII table:");
		for (x=0x21; x<128; x++)
		{
			buf[x-0x21] = x;
		}
		buf[x-0x21] = '\0';
		lcd_display(0,0,FONT_SIZE12|DISP_CLRLINE|reverse,"%s",buf);
		for (i=0; i<3; i++)
		{
			mode =gcFornt[i];
			mode |= reverse;
            mode |= DISP_CLRLINE;
			TRACE("\r\n中文:");	
			InkeyCount(0);
			drv_lcd_cls();
			y = 5-(i<<1);
			for (x=0; x<y; x++)
			{
				memcpy(buf+x*20,"中国早上好是该醒了吧",20);
			}
			buf[y*20] = '\0';
            TRACE("\nmode:%04X",mode);
            TRACE("\n%s",buf);
			lcd_display(0,0,mode,"%s",buf);
			InkeyCount(0);
			TRACE("\r\n混全:");
//			drv_lcd_cls();
			if ((mode&0xFF) == FONT_SIZE24)
				lcd_display(0,0,mode,"中国China");	
			else
				lcd_display(0,0,mode,"中国早上好(Good\r\nmorning China)");
			TRACE("\r\n居中");		
			InkeyCount(0);
//			drv_lcd_cls();
			lcd_display(0,0,mode|DISP_MEDIACY,"中国China");			
			lcd_display(17,27,mode|DISP_MEDIACY,"中国China");			
		}	
		TRACE("\r\n8-16-24");
		InkeyCount(0);
		drv_lcd_cls();
		lcd_display(0,0,FONT_SIZE12|DISP_CLRLINE,"Good morning!");
		lcd_display(0,12,FONT_SIZE12|DISP_CLRLINE,"早上好Goodmorning");
//		lcd_display(0,20,FONT_SIZE16|DISP_INVCHAR,"早上好Goodmornin");
//		lcd_display(0,36,FONT_SIZE24|DISP_INVCHAR,"早上好Good");
	} else
	{
		TRACE("\r\n\t-x:[0-131]");
		x = InkeyCount(0);
		TRACE("\r\n\t-y:[0- 63]");
		y = InkeyCount(0);
		x=0; y=0;
		lcd_display(x,y,mode|DISP_CLRLINE,"V E");
		lcd_display(x,y,FONT_SIZE12|DISP_CLRLINE,"abcdefghijkl");
		TRACE("\r\nChinese characters:");
		InkeyCount(0);
		drv_lcd_cls();
		lcd_display(x,y,mode,"中国可\r\n耳中国");
		InkeyCount(0);
//		drv_lcd_cls();
		lcd_display(0,20,mode|DISP_CLRLINE,"早上好Goodmorning");
	}
}

//void dbg_font_lab(void)
//{
//    int key;
//    uint16_t code;
//    uint32_t addr;
//    uint8_t encode[24],deconde[24];
//    uint8_t i;
//    while (1)
//    {
//        TRACE("\r\n-|*****************font lab debug***********************|-");
//        TRACE("\r\n-|1-Chinese characters            2-western character   |-");
//        TRACE("\r\n-|******************************************************|-\t");
//        key = InkeyCount(0);
//        switch (key)
//        {
//        case 1://啊
//            TRACE("\n-|Input Chinese characters interior code(HEX):");
//            code = InkeyHex(0);
//            addr = (((code>>8)- 0xB0) * 94 + (code&0xFF) - 0xA1) * 18;
//            TRACE("\n-|code:0x%04X offset:0x%04X-%4d",code,addr,addr);
//            addr += SA_ZK +0x12;
//            TRACE("\n-|SAZK:%05X addr: 0x%05X - %04d",SA_ZK,addr,addr);
//            memcpy(encode,(uint8_t *)addr,18);
//            TRACE_BUF("HZ encode dot:",encode,18);
//            memcpy(deconde,encode,12);
//            for(i=12; i<18; i++)
//            {
//                deconde[i]   = (encode[i]<<4)&0xF0;
//                deconde[i+6] = (encode[i]&0xF0);
//            }
//            TRACE_BUF("HZ decode dot:",deconde,24);
//            drv_lcd_cls();
//            axisymmetric_x(deconde,24);
//            drv_lcd_FillVertMatrix1(48,0,12,16,deconde);
//            drv_lcd_update_switch(ON);
//            drv_lcd_update();
//            break;
//        case 2://!
//            TRACE("\n-|Input Western characters interior code(HEX):");
//            code = InkeyHex(0);
//            addr = SA_ZK +0x1DBF2 + ((code & 0xFF) - 0x20) * 12;
//            TRACE("\n-|addr: 0x%04X - %04d",addr,addr);
//            memcpy(encode,(uint8_t *)addr,12);
//            TRACE_BUF("CHAR encode dot:",encode,12);
//            for(i=0; i<6; i++)
//            {
//                deconde[i] = (encode[i]<<1)|(encode[i+6]>>7);
//                deconde[i+6] = (encode[i+6]<<1);
//            }
//            TRACE_BUF("CHAR decode dot:",deconde,12);
//            drv_lcd_cls();
//            axisymmetric_x(deconde,12);
//            drv_lcd_FillVertMatrix1(48,0,6,16,deconde);
//            drv_lcd_update();
//            break;
//        case 3:
//            drv_lcd_cls();
//            InkeyCount(0);
//            lcd_display(0,8,FONT_SIZE12,"!啊");
//            lcd_display(0,FONT_SIZE12,FONT_SIZE12|DISP_CLRLINE,"!啊");
//            break;
//        case 4:
//            lcd_dbg_display();
//            break;
//        case 5:
//            drv_lcd_cls();
//            lcd_display(0,0,FONT_SIZE12|DISP_CLRLINE,"V E");
//            break;
//        case 6:
//            TRACE_BUF("FONT",(uint8_t *)SA_ZK,4094);
//            break;
//        default:
//            break;
//        case 99:
//            return;
//        }
//    }
//}


#ifdef CFG_DBG_BT
extern int bt_getname(char *name);
extern int bt_getpin(char *pin);
extern int bt_getaddr(unsigned char *addr);
extern uint8_t bt_status(void);
void dbg_bluetooth(void)
{
    int key;
    int iRet;
    char pin[32];
    uint8_t buffer[64];

    while (1)
    {
        TRACE("\r\n-|*******************Bluetooth debug***********************|-");
        TRACE("\r\n-|1-Open 2-Close  3-getpin 4-read 5-write 6-name 7-addr    |-");
        TRACE("\r\n-|*********************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            bt_open();
            break;
        case 2:
            bt_close();
            break;
        case 3:
            iRet = bt_getpin(pin);
            TRACE("\n-|ret:0x%02X-%d",iRet,iRet);
            break;
        case 4:
            memset(buffer,0x00,sizeof(buffer));
            iRet = bt_read(buffer,10,100);
            TRACE("\n-|ret:%d-0x%d",iRet,iRet);
            TRACE_BUF("",buffer,iRet );
            break;
        case 5:
            iRet = bt_write("\x55\xAA", 2);
            TRACE("\nret:%d",iRet);
            break;
        case 6:
            iRet = bt_getname(pin);
            TRACE("\n-|ret:0x%02X-%d",iRet,iRet);
            break;
        case 7:
            iRet = bt_getaddr((unsigned char *)pin);
            TRACE("\n-|ret:0x%02X-%d",iRet,iRet);
            break;
        case 8:
            TRACE("\n-|status:%02b",bt_status());
            break;
        case 99:
            return;
        default:
            break;
        }
    }
 

}
#endif

#ifdef CFG_DBG_MAGTECK 
void dbg_magtek(void)
{
    int magret = 0x70;
    uchar track[3][128];
    mag_init(0);
    TRACE("\r\n magret test");
    mag_open();
    while ( 1 ) {
        if ( IfInkey(0) ) {
            break;
        }
        if ( mag_check(0) == 0 ) {
            memset(track,0,sizeof(track));
            TRACE("\r\n magret read");
            magret = mag_read(track[0], track[1], track[2]);
            TRACE("\r\n magret:%x",magret);
//            DISPBUF("track1",(magret>>8)&0xFF,0,track[0]);
            DISPBUF("track2",(magret>>16)&0xFF,2,track[1]);
            DISPBUF("track3",(magret>>24)&0xFF,2,track[2]);
        }
    }
    mag_close();
}
#endif

#ifdef CFG_DBG_RFID 
void vDispBufKey(uint uiLen, uchar *ucBuf)
{
    const uchar gKeybuf[] = {
        0x11, 0x22, 0x33, 0x55,
        0x66, 0x77, 0x88, 0x99, 0xAA,
        0xBB, 0xCC, 0xDD, 0xEE
    };
	uint i,j,k;
	TRACE("\r\n--------%d---------\r\n",uiLen);
	for(i=0;i<uiLen;i++)
	{
		k = 0;
		for(j=0;j<sizeof(gKeybuf);j++)
		{
			if(gKeybuf[j] == ucBuf[i] && gKeybuf[j] == ucBuf[i+1])
			{
				k = 1;
				if(gKeybuf[j] == 0xCC)
				{
					TRACE("\r\n------------");
				}
			}
		}
		if(k)
		{
			TRACE("\r\n%02X: ",ucBuf[i]);
			++i;
		}
		else
		{
			TRACE("%02X ",ucBuf[i]);
		}
	}
	TRACE("\r\n");
}
uchar TI_GetCardType(uchar Type)
{
	uchar CardType = 0;
	
	if(Type == EM_mifs_TYPEA)
	{
		if (EG_mifs_tWorkInfo.ucUIDLen == 4)
		{
			//b5=1时符合ISO14443-3协议
			if ((EG_mifs_tWorkInfo.ucSAK1 & 0x20) != 0x20)
			{
				if ((EG_mifs_tWorkInfo.ucSAK1 & 0x18) == 0x08)
				{
					CardType = EM_mifs_S50;
				}
				else if ((EG_mifs_tWorkInfo.ucSAK1 & 0x18) == 0x18)
				{
					CardType = EM_mifs_S70;
				}
			}
			else
			{
				if ((EG_mifs_tWorkInfo.ucSAK1 & 0x18) == 0x08)
				{
					CardType = EM_mifs_PRO_S50;
				}
				else if ((EG_mifs_tWorkInfo.ucSAK1 & 0x18) == 0x18)
				{
					CardType = EM_mifs_PRO_S70;
				}
				else
				{
					CardType = EM_mifs_PROCARD;
				}
			}
		}
	}
	else if(Type == EM_mifs_TYPEB)
	{
		CardType = EM_mifs_TYPEBCARD;
	}

	return CardType;
}
void dbg_rfid(void)
{
#ifdef CFG_DBG_RFID
    uchar ucVal,result,ucATQA[2];
    uchar buf[32];
    int i,key;
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = GPIOC;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL;
    gpio_init.PORT_Pin = PORT_Pin_2;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(gpio_init.PORT_Pin);
    hw_gpio_init(&gpio_init);
    gpio_init.PORT_Pin = PORT_Pin_3;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(gpio_init.PORT_Pin);
    hw_gpio_init(&gpio_init);
    hw_gpio_reset_bits(GPIOC,GPIO_Pin_2);
    hw_gpio_reset_bits(GPIOC,GPIO_Pin_3);

    TRACE("\r\n---rfid open:%d",EA_ucMifsOpen(EM_mifs_TYPEA)); 

    while ( 1 ) {
        TRACE("\r\n----rfid test----\r\n");
        TRACE("\r\n1-wupa  2-active a  3-wupb  4-active b----\r\n");
        key = InkeyCount(0);
        switch ( key )
        {
        case 1:
            EI_paypass_vInit();
            EG_mifs_tWorkInfo.ucAnticollFlag = 1;
            EI_paypass_vResetPICC();
            EI_paypass_vDelay(2000);
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                TRACE("\r\n----WUPA----\r\n");
                EI_paypass_vResetPICC();
                EI_paypass_vDelay(2000);
                s_DelayMs(500);
                result = EI_paypass_ucWUPA(ucATQA);
                if ( !result ) {
                    sys_beep();
                    DISPBUF("type a",2,0,ucATQA);		
                }
                EI_paypass_vHALTA();
            }
            break;
        case 2 :
            TRACE("\r\n----type a----\r\n");

            EI_paypass_vInit();
            EG_mifs_tWorkInfo.ucAnticollFlag = 1;
            EI_paypass_vResetPICC();
            EI_paypass_vDelay(2000);
            EI_mifs_vFlushFIFO();				// empty FIFO

            // 初始化相关变量
            EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEA;
            EG_mifs_tWorkInfo.ucMifActivateFlag = 0;

            CLRBUF(ucATQA);
#ifdef CFG_DBG_ICCARD 
            guiDebugFlg = 1;
            guiDebugi = 0;
#endif
            result = EI_paypass_ucWUPA(ucATQA);
//            EI_paypass_vHALTA();
            if (!result)
            {
                TRACE("\r\nTYPE A 寻卡成功 \r\n");
                DISPBUF("type a",2,0,ucATQA);		
                sys_beep();
                result = EI_paypass_ucAnticoll(0x93,buf);
                if ( !result ) {
                    TRACE("\r\n Anticoll success\r\n");
                    result = EI_paypass_ucSelect(0x93, buf, &ucVal);
                    if ( !result ) {
                        TRACE("\r\n SAK success\r\n");
                        memcpy(EG_mifs_tWorkInfo.ucUIDCL1,buf,5);
                        result = EI_paypass_ucRATS(&ucVal, buf);
                        if ( !result ) {
                            EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_OK;
                            CLRBUF(buf);
                            EG_mifs_tWorkInfo.ucCurPCB = 0;
                            EG_mifs_tWorkInfo.ucMifCardType = TI_GetCardType(EG_mifs_tWorkInfo.ucCurType);
                            result = EA_ucMIFAPDU(0,5,"\x00\x84\x00\x00\x04",(uint*)&i,buf);
                            EG_mifs_tWorkInfo.ucMifActivateFlag = 0;

                            if((result != EM_mifs_SUCCESS) && (result != EM_mifs_SWDIFF))
                            {
                                TRACE("\r\nTYPE A [%x].......\t随机数失败 %x\r\n",EG_mifs_tWorkInfo.ucMifCardType,result);				
                            }
                            else
                            {
                                sys_beep();
                                TRACE("\r\nTYPE A [%x].......\t随机数成功\r\n",EG_mifs_tWorkInfo.ucMifCardType);
                                DISPBUF("type a Random",i,0,buf);
                            }
                        }else{
                            TRACE("\r\n rats err %x",result);
#ifdef CFG_DBG_ICCARD 
                            vDispBufKey(guiDebugi,gcDebugBuf);
#endif
                            DISP_DEBUGBUF();
                        }
                    }else{
                        TRACE("\r\n select err %x",result);
#ifdef CFG_DBG_ICCARD 
                        vDispBufKey(guiDebugi,gcDebugBuf);
#endif
                        DISP_DEBUGBUF();
                    }
                }else{
                    TRACE("\r\n Anticoll err %x",result);
#ifdef CFG_DBG_ICCARD 
                    vDispBufKey(guiDebugi,gcDebugBuf);
#endif
                    DISP_DEBUGBUF();
                }
            }
            else
            {
                TRACE("\r\n--------未找到卡:0x%02X\r\n", result);
                //TRACE("\r\n 1-退出  ");

            }
            break;
        case 3 :
            TRACE("\r\n----WUPB----\r\n");
            CLRBUF(buf);
            EI_paypass_vResetPICC();
            EI_paypass_vDelay(2000);
            s_DelayMs(50);
#ifdef CFG_DBG_ICCARD 
            guiDebugFlg = 0;
#endif
            INIT_DEBUGBUF();
            EG_mifs_tWorkInfo.ucAnticollFlag = 1;
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                s_DelayMs(500);
                result = EI_paypass_ucWUPB(&buf[0], &buf[1]);
                if (!result)
                {
                    TRACE("\r\nTYPE B 寻卡成功 len:%d\r\n",buf[0]);
                    DISPBUF("type b",buf[0],0,&buf[1]);		
                    sys_beep();
                }
                else
                {
                    TRACE("\r\n--------未找到卡:0x%02X\r\n", result);
                    //TRACE("\r\n 1-退出  ");

                }
            }
#ifdef CFG_DBG_ICCARD 
            vDispBufKey(guiDebugi,gcDebugBuf);
#endif
            DISP_DEBUGBUF();
            break;
        case 4:
           TRACE("\r\n----type b----\r\n");
#ifdef CFG_DBG_ICCARD 
            guiDebugFlg = 0;
#endif
            INIT_DEBUGBUF();

            EI_paypass_vInit();
            EG_mifs_tWorkInfo.ucAnticollFlag = 1;
            EI_paypass_vResetPICC();
            EI_paypass_vDelay(2000);
            s_DelayMs(50);

            EI_mifs_vFlushFIFO();				// empty FIFO

            // 初始化相关变量
            EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEB;
            EG_mifs_tWorkInfo.ucMifActivateFlag = 0;

            CLRBUF(buf);
            result = EI_paypass_ucWUPB(&buf[0],&buf[1]);
            if(result == EM_mifs_SUCCESS)
            {
                //pollAflg = 1;
                TRACE("\r\nTYPE B 寻卡成功 \r\n");
                DISPBUF("type b",buf[0],0,&buf[1]);
                sys_beep();
                INIT_DEBUGBUF();
                CLRBUF(buf);
                result = EI_paypass_ucAttrib(buf);
                if(result == EM_mifs_SUCCESS)
                {
                    TRACE("\r\nSucceed Attrib:%02X",result);
                    TRACE("\r\nbuf:%02X  %02X  %02X  %02X  %02X",buf[0], buf[1], buf[2], buf[3], buf[4]);
                    sys_beep();

#ifdef CFG_DBG_ICCARD 
                    DISPBUF("data",guiDebugi,0,gcDebugBuf);
                    TRACE("\r\n\r\n");
                    INIT_DEBUGBUF();
#endif

                    TRACE("\r\nPress to get the Random");
                    EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_OK;
                    CLRBUF(buf);
                    EG_mifs_tWorkInfo.ucCurPCB = 0;
                    EG_mifs_tWorkInfo.ucMifCardType = TI_GetCardType(EG_mifs_tWorkInfo.ucCurType);
                    result = EA_ucMIFAPDU(0,5,"\x00\x84\x00\x00\x04",(uint*)&i,buf);
                    EG_mifs_tWorkInfo.ucMifActivateFlag = 0;

                    if((result != EM_mifs_SUCCESS) && (result != EM_mifs_SWDIFF))
                    {
                        TRACE("\r\nTYPE B [%x].......\t随机数失败 %x\r\n",EG_mifs_tWorkInfo.ucMifCardType,result);
                    }
                    else
                    {
                        sys_beep();
                        TRACE("\r\nTYPE B [%x].......\t随机数成功\r\n",EG_mifs_tWorkInfo.ucMifCardType);
                        DISPBUF("type b Random",i,0,buf);
                    }
                }
                else
                {
                    TRACE("\r\nFail Anticoll:%02X",result);
                }
            }
            else
            {
                TRACE("\r\nPoll B:%02X",result);
            }

#ifdef CFG_DBG_ICCARD 
            DISPBUF("data",guiDebugi,0,gcDebugBuf);
            TRACE("\r\n\r\n");
            INIT_DEBUGBUF();
#endif
            break;
        case 99:
            return;
        default :
            break;
        }
    }
#endif
}
#endif

void dbg_console(void)
{
    int key;
  //  int time;
    while (1)
    {
        TRACE("\r\n-|*****************console debug**********************|-");
        TRACE("\r\n-|1-usb 2-uart                                        |-");
        TRACE("\r\n-|****************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            console_init(USBD_CDC,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
//            while (1)
//            {
//                s_DelayMs(400);
//                if (drv_usbd_cdc_ready() == 0)
//                    break;
//            }
            TRACE("\nUSB console OK1");
//            time = sys_get_counter();
//            while (1)
//            {
//                TRACE("\nUSB console OK");
//                if (sys_get_counter() - time > 8*1000)
//                    break;
//                s_DelayMs(30);
//            }
//            console_close();
//            console_init(CNL_COMPORT,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
            break;
        case 2:
            console_close();
            console_init(CNL_COMPORT,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
            break;
        case 99:
            return;
        default:
            break;
        }
    }
     
}

#define  CFG_DBG_DOWNLOAD_BOOT
#ifdef CFG_DBG_DOWNLOAD_BOOT
typedef struct _VERSION_INFO_
{
	char product[20];				
	char version[16];  // "255.255.255.255"			
	char time[12];					
}VERSION_INFO;

extern int dl_open(uint32_t com, uint8_t *buf_dl, uint32_t buf_size);
extern int dl_boot(uint32_t *code_length,int t_out_ms);
extern int dl_close(void);
const VERSION_INFO gBootVerInfo={
	"STAR PPKB BOOT",
	"2.0.0.0" ,
	"__DATE__"       //12B
};	

void dbg_boot_download(int com)
{
    uint8_t download[DL_BUFSIZE_C];
    uint32_t length;

    TRACE("\nboot dl main");
    s_DelayMs(500);
    console_close();
    dl_open(com,download,DL_BUFSIZE_C);
    dl_boot(&length,-1);
    dl_close();
    console_init(CNL_COMPORT,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
    TRACE("\npress any key to continue");
    InkeyCount(0);
    TRACE("\ncode length:%dKB %d 0x%02X",length/1024,length,length);
}
#endif

void dbg_local_down(void)
{
#ifdef CFG_DBG_DOWNLOAD_BOOT
    dbg_boot_download(CNL_COMPORT);
#endif
//    int iRet;
//    struct LocalDlOpt localdl;
//
//    TRACE("\n-|local down");
//    if (CNL_COMPORT == DL_COMPORT) {
//        console_close();
//    }
//    memset(&localdl,0,sizeof(struct LocalDlOpt));
//    localdl.serialbuf = gwp30SysBuf_c.pub;
//    localdl.seriallen = sizeof(gwp30SysBuf_c.pub);
//    localdl.frame = (struct LocalDlFrame *)gwp30SysBuf_c.work;
//    localdl.dllevel = DL_HANDSHAKE_CTRL;
//    localdl.callback =  &gcLocaldlCallBack;
//
//    localdl_com_open(&localdl);
//    lcd_cls();
//    while (1)
//    {
//        iRet = localdl_process(&localdl);
//        if (iRet == LOCALDL_CANCEL)
//        {
//            break;
//        } else if (iRet == OK)
//        {
//            break;
//        }
//        if (iRet ==  LOCALDL_CANCEL)
//        { 
//            break;
//        }
//    }
//    localdl_com_close(&localdl);
//    if (CNL_COMPORT == DL_COMPORT) {
//        console_init(CNL_COMPORT,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
//    }
//    InkeyCount(0);
//    if (iRet == OK)
//    {
//        TRACE("\nDownOK");
//    }
//    else
//        TRACE("\n-|cancel out");
}


void dbg_version(void)
{
    extern const VERSION_INFO_NEW gCtrlVerInfo;
//    extern uint32_t __code_start__[];
//    extern int localdl_get_boot_version(VERSION_INFO_NEW *ver);

    const VERSION_INFO_NEW *version;

//    TRACE("\n-|VER:%p  %04X icf:%p-%04x-%04x-%04x ",&gCtrlVerInfo,*((uint32_t*)(SA_CTRL+0x410)),__code_start__,__code_start__,(uint32_t)__code_start__,*__code_start__);
//    TRACE("\n-|version:%s",*((VERSION_INFO_NEW*)&__code_start__)->product);
//    TRACE("\n-|version:%s",*((VERSION_INFO_NEW*)*__code_start__)->product);
    localdl_get_boot_version(&version);
    TRACE("\n-|product:%s",version->product);
    TRACE("\n-|version:%s",version->version);
    TRACE("\n-|complietime:%s",version->time);
    //extern VERSION_INFO_NEW  *__code_start__;
    //    TRACE("\n-|version:%s",__code_start__->product);
}


void dbg_gpio(void)
{
    int key;
    GPIOPin_Def pin=PTD0;


    while (1)
    {
        TRACE("\r\n-|*****************gpio  debug***************|-");
        TRACE("\r\n-|1-setoutput 2-out_1 3-out_0 4-square wave  |-");
        TRACE("\r\n-|5-setinput  6-input_level                  |-");
        TRACE("\r\n-|*******************************************|-\t");
        TRACE("\n-|clk:%d",core_clk_mhz);
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            gpio_set_output(pin,GPIO_OUTPUT_OpenDrainDisable,0);
            break;
        case 2:
            gpio_set_bit(pin,1);
            break;
        case 3:
            gpio_set_bit(pin,0);
            break;
        case 4:
            TRACE("\nInput cn:");
            key = InkeyCount(0);
            gpio_set_bit(pin,0);
            s_DelayMs(key);
            gpio_set_bit(pin,1);
            s_DelayMs(key);
            gpio_set_bit(pin,0);
            break;
        case 5:
            gpio_set_input(pin,GPIO_INPUT_PULLDISABLE);
            break;
        case 6:
            TRACE("\n-|level:%d",gpio_get_bit(pin));
            break;
        case 7:
            TRACE("\nus Input cn:");
            key = InkeyCount(0);
            gpio_set_bit(pin,0);
            s_DelayUs(key);
            gpio_set_bit(pin,1);
            s_DelayUs(key);
            gpio_set_bit(pin,0);
            break;
        case 99:
            return;
        default:
            break;
        }
    }
}

#ifdef CFG_DBG_LPWR
void dbg_lpwr(void)
{
    int key;
    uint32_t  waitevent=0,time_ms;

    while (1)
    {
        TRACE("\r\n-|*****************lowerpoer debug**********************|-");
        TRACE("\r\n-|1-cpu mode lowerpoer 2-lpw 3-vlpr 4-exit vlpr 5-gpio  |-");
        TRACE("\r\n-|6-wait                                                |-");
        TRACE("\r\n-|******************************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            dbg_lowerpower();
            break;
#if defined(CFG_LOWPWR)
        case 2:
#if (defined CFG_MAGCARD)
            TRACE("\n-|Magnetic Stripe Reader:1-open other-close");
            if (InkeyCount(0) == 1) {
                mag_open();
                waitevent |= EVENT_MAGSWIPED;
            }
#endif
#if (defined CFG_ICCARD)
            TRACE("\n-|IC Card Reader:1-open other-close");
            if (InkeyCount(0) == 1) {
                waitevent |= EVENT_ICCIN;
            }
#endif
            TRACE("\n-|timeout");
            time_ms = InkeyCount(0);
            waitevent |= EVENT_TIMEOUT;
            waitevent |= EVENT_UARTRECV;
            waitevent |= EVENT_KEYPRESS;
            TRACE("\n-|1-lpwr run");
            sys_SleepWaitForEvent(waitevent,InkeyCount(0),time_ms);
            break;
        case 3:
            enter_lowerpower_freq();
            break;
        case 4:
            exit_lowerpower_freq();
            break;
        case 5:
            lpwr_set_gpio();
            break;
        case 6:
            while (1) {
                enter_lowerpower_wait();
                if (IfInkey(0))
                    break;
            }
            break;
        case 7:
            lcd_cls();
            lcd_display(0,12, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY, "Lowerpowermode");
            break;
        case 8:
            lwpr_device_reinit();
            break;
#endif
        case 99:
            return;
        default:
            break;
        }
    }
}
#endif

#ifdef CFG_DBG_CRC
void dbg_crc32_le(void)
{
    crc_user_config_t userConfigPtr;
    uint32_t crcValue=0,srcSoft=0;
    uint8_t w,r,compr;

    srcSoft = crc32_le(0,"123456789",8);
    userConfigPtr.crcWidth       = kCrc32Bits;
    userConfigPtr.polynomial     = CRCPOLY_BE;
    userConfigPtr.seed           = 0;
    for (w =0;w<8;w++ ) 
    {
        if (w>3) {
            userConfigPtr.seed           = ~0;
        }
        for (r=0;r<4;r++) 
        {
            for (compr=0; compr<2; compr++ ) 
            {
                userConfigPtr.writeTranspose = (crc_transpose_t )(w%4);
                userConfigPtr.readTranspose  = (crc_transpose_t )r;
                userConfigPtr.complementRead = (bool)compr;
                crcValue = drv_crc_hardware(&userConfigPtr,"123456789",8);
                TRACE("\r\nCRC%d seed:%08X poly:%04X writeTransopse:%d ReadTranspose:%d complement:%d soft:%08X VAL:%08X",
                      (userConfigPtr.crcWidth==kCrc16Bits)?16:32,
                      userConfigPtr.seed,
                      userConfigPtr.polynomial,
                      userConfigPtr.writeTranspose,
                      userConfigPtr.readTranspose,
                      userConfigPtr.complementRead,
                      srcSoft,
                      crcValue);
                if (srcSoft == crcValue) {
                    TRACE("  CRC THE SAME!");
                    if (InkeyCount(0) == 99)
                        return;
                }
            }
        }
    }
}

void dbg_crc32_be(void)
{
    crc_user_config_t userConfigPtr;
    uint32_t crcValue=0,srcSoft=0;
    uint8_t w,r,compr;

    TRACE("\r\n-|=============BE============");
    srcSoft = crc32_be(0,"123456789",8);
    userConfigPtr.crcWidth       = kCrc32Bits;
    userConfigPtr.polynomial     = CRCPOLY_BE;
    userConfigPtr.seed           = 0;
    for (w =0;w<8;w++ ) 
    {
        if (w>3) {
            userConfigPtr.seed           = ~0;
        }
        for (r=0;r<4;r++) 
        {
            for (compr=0; compr<2; compr++ ) 
            {
                userConfigPtr.writeTranspose = (crc_transpose_t )(w%4);
                userConfigPtr.readTranspose  = (crc_transpose_t )r;
                userConfigPtr.complementRead = (bool)compr;
                crcValue = drv_crc_hardware(&userConfigPtr,"123456789",8);
                TRACE("\r\nCRC%d seed:%08X poly:%04X writeTransopse:%d ReadTranspose:%d complement:%d soft:%08X VAL:%08X",
                      (userConfigPtr.crcWidth==kCrc16Bits)?16:32,
                      userConfigPtr.seed,
                      userConfigPtr.polynomial,
                      userConfigPtr.writeTranspose,
                      userConfigPtr.readTranspose,
                      userConfigPtr.complementRead,
                      srcSoft,
                      crcValue);
                if (srcSoft == crcValue) {
                    TRACE("  CRC THE SAME!");
                    if (InkeyCount(0) == 99)
                        return;
                }
            }
        }
    }
}

#define MAX_CHARS   256
void crc_selft_test(void)
{
    crc_user_config_t userConfigPtr;
    uint32_t crcValue;
    int iRet;
    uint8_t  strMsg[MAX_CHARS];
    uint32_t  bByteCount;
    uint32   crc;

    TRACE("\r\n====== CRC Lab ==========\r\n");
    TRACE("\r\n-|1-Auto");
    if ( InkeyCount(0) == 1)
    {
        dbg_crc32_le();
        dbg_crc32_be();
    } else
    { 
        TRACE("\r\nPlease select type of Transpose for input:");
        TRACE("\r\n1. No Transposition");
        TRACE("\r\n2. Only transpose bits in a byte");
        TRACE("\r\n3. Transpose both bits and bytes");
        TRACE("\r\n4. Only transpose bytes");
        CLRBUF(strMsg);
        strcpy((char *)strMsg,"12345678");
        bByteCount = strlen((char *)strMsg);
        iRet = InkeyCount(0);
        switch (iRet)
        {
        case 1:
            userConfigPtr.writeTranspose = kCrcNoTranspose;
            break;
        case 2:
            userConfigPtr.writeTranspose = kCrcTransposeBits;
            break;
        case 3:
            userConfigPtr.writeTranspose = kCrcTransposeBoth;
            break;
        case 4:
            userConfigPtr.writeTranspose = kCrcTransposeBytes;
            break;
        }

        TRACE("\r\nPlease select type of Transpose for Read:");
        TRACE("\r\n1. No Transposition");
        TRACE("\r\n2. Only transpose bits in a byte");
        TRACE("\r\n3. Transpose both bits and bytes");
        TRACE("\r\n4. Only transpose bytes");
        TRACE("select:");
        iRet = InkeyCount(0);
        switch (iRet)
        {
        case 1:
            userConfigPtr.readTranspose = kCrcNoTranspose;
            break;
        case 2:
            userConfigPtr.readTranspose = kCrcTransposeBits;
            break;
        case 3:
            userConfigPtr.readTranspose = kCrcTransposeBoth;
            break;
        case 4:
            userConfigPtr.readTranspose = kCrcTransposeBytes;
            break;
        }

        TRACE("\r\nXOR final checksum: 1-NO  2-YES");
        if (InkeyCount(0) == 1) {
            userConfigPtr.complementRead = FALSE;
        } else {
            userConfigPtr.complementRead = TRUE;
        }

        TRACE("\r\nPlease enter SEED in hex:");
        userConfigPtr.seed = InkeyHex(0);
        TRACE("\r\nPlease select CRC polynomial:");
        TRACE("\r\nCRC16:1.0x1021(CRC-CCITT) 2-0x8408 (XMODEM) 3-0x8005 (ARC)");
        TRACE("\r\nCRC16:4.0xA001()          5-0xxxxx (xxxxxx) 6-0xxxxx (xxx)");
        TRACE("\r\nCRC32:7.0xEDB88320(LE)    8-0x04C11DB7(BE) 9-0x8005 (ARC)");
        TRACE("\r\n10. others\r\n");
        iRet = InkeyCount(0);
        switch (iRet)
        {
        case 1:
            userConfigPtr.crcWidth = kCrc16Bits;
            userConfigPtr.polynomial = 0x1021;
            //CRC16 seed:0000 poly:1021 writeTransopse:0 ReadTranspose:0 complement:0
            crc =  calc_crc16(strMsg,bByteCount,userConfigPtr.seed);
            break;
        case 2:
            userConfigPtr.crcWidth = kCrc16Bits;
            userConfigPtr.polynomial = 0x8408;
            break;
        case 3:
            userConfigPtr.crcWidth = kCrc16Bits;
            userConfigPtr.polynomial = 0x8005;
            break;
        case 4:
            userConfigPtr.crcWidth = kCrc16Bits;
//            userConfigPtr.polynomial = 0xA001;
            userConfigPtr.polynomial = 0x8005;
            //CRC16 seed:0000 poly:8005 writeTransopse:1 ReadTranspose:2 complement:0
            crc = crc_calculate16by8(strMsg,bByteCount);
            break;
        case 5:
            userConfigPtr.crcWidth = kCrc16Bits;
            userConfigPtr.polynomial = 0x04C11DB7;
            break;
        case 6:
            userConfigPtr.crcWidth = kCrc16Bits;
            userConfigPtr.polynomial = 0x04C11DB7;
            break;
        case 7:
            userConfigPtr.crcWidth = kCrc32Bits;
//            userConfigPtr.polynomial = CRCPOLY_LE;
            userConfigPtr.polynomial = CRCPOLY_BE;
            //CRC32 seed:00000000 poly:4C11DB7 writeTransopse:1 ReadTranspose:2 complement:0 soft:FFC205C6 VAL:FFC205C6  CRC THE SAME![IN]:99
            crc = crc32_le(userConfigPtr.seed,strMsg,bByteCount);
            break;
        case 8:
            userConfigPtr.crcWidth = kCrc32Bits;
            userConfigPtr.polynomial = CRCPOLY_BE;
            crc = crc32_be(userConfigPtr.seed,strMsg,bByteCount);
            crcValue = calcCrc32((uint32_t *)strMsg,bByteCount/4,0);
            TRACE("\r\nCRC result HAL0: = %#04.4x",crcValue);
            break;
        case 10:
            TRACE("Please enter a polynomial in hex format:"); 
            userConfigPtr.polynomial = InkeyHex(0);
            break;
        }
        TRACE("\r\n\nCRC%d seed:%04X poly:%04X writeTransopse:%d ReadTranspose:%d complement:%d",
              (userConfigPtr.crcWidth==kCrc16Bits)?16:32,
              userConfigPtr.seed,
              userConfigPtr.polynomial,
              userConfigPtr.writeTranspose,
              userConfigPtr.readTranspose,
              userConfigPtr.complementRead );
        crcValue = drv_crc_hardware(&userConfigPtr,strMsg, bByteCount);
        TRACE("\r\nCRC result HAL : = %#04.4x",crcValue);
        TRACE("\r\nCRC result soft: = %#04.4x",crc);
    }
}

void dbg_crc_main(void)
{
    crc_selft_test();
}
#endif
#if 0
void dbg_uartdma()
{
    extern int ctc_uart_dma_check1(void);
    uint data_vail_len=0;
    int s_pos=0, e_pos=0, pos=0;
    int len;
    int key;
    int ret;
    uchar buff[1024];
    int i;
    memset(buff, 0, sizeof(buff));
    while ( 1 ) {
        TRACE("\r\n-|*****************uart dma**********************|-");
        TRACE("\r\n-|1-open 2-check 3-receive 4-close               |-");
        TRACE("\r\n-|***********************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
//            ret = ctc_uart_open();
            ret = ctc_uart_open_nodma();
            TRACE("get the open ret:%d\r\n", ret);
            break;
        case 2:
            ctc_uart_dma_check1();
            break;
        case 3:
            while (1) {
REJUDGE:
                if (IfInkey(0)) {
                    break;
                }
                if ( drv_uart_check_readbuf(CTC_COMPORT) ) {
                    i = drv_uart_read(CTC_COMPORT, &buff[0], 1, 1000);	
                    if(!i)
                    {
                        TRACE("rec buff data err\r\n");
                        goto REJUDGE;
                    }
                    if(buff[0] != CTC_STX)
                    {
                      TRACE("data head err:%2X\r\n", buff[0]);
                        goto REJUDGE;
                    }
                    // Len:cmd+data+crc16
                    i = drv_uart_read(CTC_COMPORT, &buff[1], 4, 1000);
                    if(!i)
                    {		
                        TRACE("rec len err\r\n");
                        goto REJUDGE;
                    }		
                    len = (uint) buff[1];
                    TRACE("get the data len:%d\r\n", len);
                    // read data
                    i = drv_uart_read(CTC_COMPORT, &buff[5] , len+3, 20000);
                    if (i != len+3)
                    {
                        TRACE("read data fail\r\n");
                        goto REJUDGE;
                    }
                    ret = ctc_recev_frame(buff, len+8);
                    if ( ret != RET_OK ) {
                        TRACE("rec fail\r\n");
                    }
                    memset(buff, 0, sizeof(buff));
                }
            }
            break;
        case 4:
            ctc_uart_close_nodma();
            break;
        case 99:
            return ;
        }

    }
}
#endif
#if 2
typedef struct _DMA_BUF{
    int prepos;    // 当前位置
    int recodepos;
    int len;
    uchar dmabuf[1024];
}DMA_BUF; 

DMA_BUF g_SDMA_BUF;

// 认为一帧数据不超过1024

void dmabuf_get_frame_init(void)
{
    g_SDMA_BUF.prepos = 0;
    g_SDMA_BUF.recodepos = 0;
    g_SDMA_BUF.len = 0;
    memset( g_SDMA_BUF.dmabuf, 0, 1024 );
}

int dmabuf_check_frame(void)
{
    int pos;
    pos = drv_dma_get_daddr(UART4_DMA_CHANNEL)-(uint32)&gwp30SysBuf_c.work[0];
    if ( g_SDMA_BUF.prepos == (pos%sizeof(gwp30SysBuf_c.work))) {
        return NO;      
    }
    return YES; 
}

void dmabuf_get_frame(void)
{
    // 检测dma是否开启
    int pos;
    pos = drv_dma_get_daddr(UART4_DMA_CHANNEL)-(uint32)&gwp30SysBuf_c.work[0];
    memset( g_SDMA_BUF.dmabuf, 0, 1024);
    if ( g_SDMA_BUF.prepos > pos ) {
        memcpy(g_SDMA_BUF.dmabuf, &gwp30SysBuf_c.work[g_SDMA_BUF.prepos], sizeof(gwp30SysBuf_c.work)-g_SDMA_BUF.prepos);
        memcpy(&g_SDMA_BUF.dmabuf[sizeof(gwp30SysBuf_c.work)-g_SDMA_BUF.prepos], &gwp30SysBuf_c.work[0], pos);
        g_SDMA_BUF.len = sizeof(gwp30SysBuf_c.work)-g_SDMA_BUF.prepos+pos; 
    }
    else{
        memcpy(g_SDMA_BUF.dmabuf, &gwp30SysBuf_c.work[g_SDMA_BUF.prepos], pos-g_SDMA_BUF.prepos);
        g_SDMA_BUF.len = pos-g_SDMA_BUF.prepos; 
    }
    g_SDMA_BUF.recodepos = pos;
}

void dmabuf_dealend(void)
{
    g_SDMA_BUF.prepos = (g_SDMA_BUF.recodepos)%(sizeof(gwp30SysBuf_c.work)); 
}

void dbg_uartdma()
{
    extern int ctc_uart_dma_check1(void);
    uint data_vail_len=0;
    int s_pos=0, e_pos=0, pos=0;
    int len = 0;
    int key;
    int ret;

//    uchar log_buf[256] = {0};
    while ( 1 ) {
        TRACE("\r\n-|*****************uart dma**********************|-");
        TRACE("\r\n-|1-open 2-check 3-receive 4-close 5-emv_log     |-");
        TRACE("\r\n-|***********************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            ret = ctc_uart_open();
            TRACE("get the open ret:%d\r\n", ret);
            break;
        case 2:
            ctc_uart_dma_check1();
            break;
        case 3:
            dmabuf_get_frame_init();
            while (1) {
                if (IfInkey(0)) {
                    break;
                }
#if 0
                if (dmabuf_check_frame()) {
                    dmabuf_get_frame();
                    TRACE("get len:%d\r\n", g_SDMA_BUF.len);
                    ret = ctc_frame_check(g_SDMA_BUF.dmabuf, g_SDMA_BUF.len, &s_pos, &e_pos);
                    if (ret == RET_OK) {
                        len = e_pos - s_pos + 1;
                        ret = ctc_recev_frame(g_SDMA_BUF.dmabuf, len);
                        dmabuf_dealend();
                    }
                }
#endif 
#if 2
                if (ctc_uart_dma_check()) {
                    pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
                    data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
                    ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
                    if (ret == RET_OK) {
                        TRACE("check integriate ok spos:%d, epos:%d\r\n",s_pos,e_pos);
//                        ctc_uart_close();
//                        drv_dma_disable( UART4_DMA_CHANNEL);
                        drv_dma_stop();
                        len = e_pos - s_pos + 1;
                        ret = ctc_recev_frame(1,gwp30SysBuf_c.work, len);
                        data_vail_len = 0;
                        memset(gwp30SysBuf_c.work, 0, sizeof(gwp30SysBuf_c.work));
                        ctc_uart_restart(); 

//                        drv_uart_dam_recv_init(CTC_COMPORT,(uint32)(&gwp30SysBuf_c.work[0]),sizeof(gwp30SysBuf_c.work), UART4_DMA_CHANNEL);
//                        ctc_uart_close();
//                        ctc_uart_open();
                    } 
//                    else {
//                        sys_DelayMs(5);
//
//                        pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
//                        data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
//                        ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
//                        if (ret == RET_OK) {
//                            drv_dma_stop();
//                            len = e_pos - s_pos + 1;
//                            ctc_recev_frame(gwp30SysBuf_c.work, len);
//                        } else {
//                            sys_DelayMs(5);
//                            pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
//                            data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
//                            ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
//                            if (ret == RET_OK) {
//                                drv_dma_stop();
//                                len = e_pos - s_pos + 1;
//                                ctc_recev_frame(gwp30SysBuf_c.work, len);
//                                TRACE("len3 = %d\r\n", data_vail_len);
//                            }
//                        }
//                    }
                }
#endif
                //            data_vail_flag = 0;
            }
            break;
        case 4:
            ctc_uart_close(); 
            break;
//        case 5:
//            memset(log_buf,0,sizeof(log_buf));
//            emv_get_transerr_log(log_buf,&len);
//            TRACE("log:%s",log_buf);
//            break;
//        case 6:
//             ret = ST_EMV_ContinueTransaction((PINSTATUS*)log_buf);
//             TRACE("continue_ret:%x",ret); 
//             break;
        case 99:
            ctc_uart_close();
            return ;
        }

    }
}
#endif


extern uint8_t com_em3096_open(void);
//extern uint16_t test_em1395(uchar *sendbuf, uint32_t len);
//extern int test_crc(char *buf, int len);
extern uint16_t com_em3096_scan_start(uint32_t timeout);
extern uint8_t com_em3096_close(void);
//extern uint8_t com_em1395_open(void);
//extern uint16_t com_em1395_scan_start(uint32_t timeout);
//extern uint32_t com_em1395_scan(void);
//extern uint8_t com_em1395_close(void);
int test_scan(void)
{
    int key;
    int ret;
    uchar tempbuff[300];
    while (1)
    {
        TRACE("\r\n-|*****************wp30 ctrl debug**********************|-");
        TRACE("\r\n-|-------core %02dMHz periph %05dKHz mcg %05dKHz--------|-",core_clk_mhz,periph_clk_khz,mcg_clk_khz);
        TRACE("\r\n-|1-3096open 2-3096_scan_start 3-3096_scan 4-3096_close |-");
        TRACE("\r\n-|5-crc  6-16进制输入 7-字符窜输入                      |-");
        TRACE("\r\n-|8-1395open 9-1395_scan_start 10-1395_scan 11-1395_close|-");
        TRACE("\r\n-|******************************************************|-\t");
        memset(tempbuff, 0, sizeof(tempbuff));
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            com_em3096_open();
            break;
        case 2:
            com_em3096_scan_start(10000);
            break;
        case 3:
            com_em3096_scan();
            break;
        case 4:
            com_em3096_close();
            break;
        case 5:
            ret = InkeyStrToHex((char *)tempbuff);
            TRACE_BUF("HEX value", tempbuff, ret);
//            test_crc((char *)tempbuff, ret);
            break;
//        case 6:
//            ret = InkeyStrToHex((char *)tempbuff);
//            TRACE_BUF("HEX value", tempbuff, ret);
//            test_em1395(tempbuff, ret);
//            break;
//        case 7:
//            ret = InkeyStr((char *)tempbuff);
//            TRACE("input str:%s\r\n", tempbuff);
//            test_em1395(tempbuff, ret);
//            break;
//        case 8:
//            com_em1395_open();
//            break;
//        case 9:
//            com_em1395_scan_start(10000);
//            break;
//        case 10:
//            com_em1395_scan();
//            break;
//        case 11:
//            com_em1395_close();
//            break;
        case 99:
            return 0;
        default:
            break;
        }
    }
}


/* 
 * test_sn_id - [GENERIC] 
 * @ 
 */
int test_sn_id (int mode)
{
    int ret;
    uchar buf[32];
    ushort crc16;
    const char *sn[] = {"START123456","123456789123456789"};
    //产品号（产品平台+产品号 2B）+ 00 + 工单号（6B）+ 时间（6B）+ 预留(3B)+CRC16(2B)
    uchar id[20] = {0x30,0x00,0x00,'S','T','A','R','T','0',0x16,0x07,0x28,0x15,0x50,0x00,0,0,0};
    CLRBUF(buf);sys_ReadSn_new(buf);
    TRACE("\r\n SN:%s",buf);
    CLRBUF(buf);fac_ctrl_get_MBID(buf);
    TRACE_BUF("主板ID",buf,sizeof(buf));
    TRACE("\r\n 1-SN 2-主板ID");
    ret = InkeyCount(0);
    if ( ret == 1 ) {
        TRACE("\r\n 1-%s 2-%s",sn[0],sn[1]);
        ret = InkeyCount(0);
        if ( ret == 1 ) {
            ret = sys_WriteSn_new((char *)sn[0]);
        } else if(ret == 2){
            ret = sys_WriteSn_new((char *)sn[1]);
        }
    } else if(ret == 2){
        TRACE("\r\n 主板ID:");
        ret = InkeyCount(0);
        if ( ret == 99 ) {
            return ret; 
        }
        id[14] = (char)ret;
        crc16 = calc_crc16(id,18,0);
        memcpy((char *)&id[18],(char *)&crc16,2);
        TRACE_BUF("主板ID:",(uchar *)id,sizeof(id));
        ret = fac_ctrl_set_MBID(1,(uchar *)id);
        DISPPOS(ret);
    }
    return ret;
}		/* -----  end of function test_sn_id  ----- */


#ifdef CFG_CURRENCY_DETECT
int currencyDetect_test(void)
{
    int key;
    hw_currencyDetect_init();
    while (1)
    {
        TRACE("\r\n-|*****************currencyDetect***************|-");
        TRACE("\r\n-|1-open 2-close                                |-");
        TRACE("\r\n-|**********************************************|-\t");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            hw_currencyDetect_open();
            break;
        case 2:
            hw_currencyDetect_close();
            break;
        case 99:
            return 0;
        default:
            break;
        }
    }
}
#endif

#ifdef CFG_RFID_IDCARD
extern uint16_t ctc_idcard_open(char *output);   
extern uint16_t ctc_idard_close(void);
extern uint16_t ctc_idcard_poll(char *output, uint *datalen);
extern uint16_t ctc_idcard_active(char *output, uint *datalen);
extern uint16_t ctc_idcard_read(char *output, uint *datalen);

int idcard_test(void)
{
    uint32_t key;
    uint datalen=0;
    int ret;
    uchar output[1500];
    while(1){
        datalen = 0;
        TRACE("\r\n-|************************idcard************************|-");
        TRACE("\r\n-|1.idcard_open 2-idcard_close 3-idcard_search. 4.idcard_read_info 5.idcard_active-");
        TRACE("\r\n-|******************************************************|-");
        key = InkeyCount(0);
#if 0
        idcard_control(key, output, &datalen);
#else 
        switch (key){
        case 1:
            ret = ctc_idcard_open((char *)output);
            if(ret){
                DISPPOS(ret);
                // return ret;
            }
            break;
        case 2:
            datalen = 0;
            ret = ctc_idard_close();
            //return ret;
            if(ret){
                DISPPOS(ret);
                // return ret;
            }
            break;
        case 3:
            ret = ctc_idcard_poll((char *)output, &datalen);
            if(ret){
                DISPPOS(ret);
            }
            break;
        case 4:
            ret = ctc_idcard_read((char *)output, &datalen);
            if(ret){
                DISPPOS(ret);
                // return ret;
            }
            break;
        case 5:
            ret = ctc_idcard_active((char *)output, &datalen);
            if(ret){
                DISPPOS(ret);
            }
            break;
        default:    
            return 0;
            break;
        }
        if(datalen){
            sys_beep();
            TRACE("data len is %d \r\n",datalen);   
            sys_beep();
//            TRACE("\r\n------ ------------------------------\r\n");
            TRACE_BUF("id info", output, datalen);
//            for(i = 0; i < datalen; i++){
//                TRACE("%2X  ",(int)*(output+i) );
//            }
//            TRACE("\r\n-------------------------------------\r\n");
        }
#endif
    }
}
#endif

void test_lowpower(void)
{
    int key;
    TRACE("\ntest lowpower");
    while (1)
    {
        TRACE("\n-|1-enter lowpower 2-exit lowpower");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            enter_lowerpower_freq();
            break;
        case 2:
            exit_lowerpower_freq();
            break;
        case 99:
            return ;
        default:
            break;
        }
    }
}

extern int ped_get_rand(uint32_t inlen,uint8_t *input,uint16_t *outlen,uint8_t *output);
extern uint8_t ctc_recev_frame_debug(uint8_t* buff, uint32_t cmd_len);
extern void dbg_tprint(void);
extern void sys_GetTime(void *outtime);
//extern uint16_t calc_crc16(const void *src_buf, uint32_t bytelen, uint16_t pre_value);
void dbg_s1000_ctrl(void)
{
#ifdef DEBUG_Dx 
    extern void dbg_test(void);
    extern int test_se_cert (int mode);
    extern void dbg_fac_log(void);
    int key;
//    int flg = 0;
    int i,ret;
    ushort randlen;
    uchar tempbuff[300];
    memset(tempbuff, 0, sizeof(tempbuff));

    drv_uart_close(WORK_COMPORT); 
    console_init(CNL_COMPORT,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
    lcd_SetLight(LIGHT_ON_MODE);
    lcd_cls();
    lcd_display(0,0, FONT_SIZE12|DISP_CLRLINE, "dbg test");
    cpu_identify();
    dbg_out_srs();
#ifdef CFG_DBG_TIME
    dbg_time_read();
#endif
//    lcd_init();
//    test2();
    TRACE("\r\n\nWelcome to Enter CTRL!%s %s.",__DATE__, __TIME__);
    TRACE("\r\n CTRL:%x-%x",SA_CTRL,LEN_CTRL);
//    TRACE("\r\n ZK:%x-%x SYSZONE:%x-%x",SA_ZK,LEN_ZK,SA_SYSZONE,LEN_SYSZONE);
//void hw_bt_gpio_init(void)
    while (1)
    {
        TRACE("\r\n-|*****************wp30 ctrl debug**********************|-");
        TRACE("\r\n-|-------core %02dMHz periph %05dKHz mcg %05dKHz--------|-",core_clk_mhz,periph_clk_khz,mcg_clk_khz);
        TRACE("\r\n-|1-uart 2-mcg 3-flash 4-rtc 5-rnga 6-security 7-time   |-");
        TRACE("\r\n-|8-crc  9-dryice 10-keyboard 11-magtek 12-delay 13-led |-");
        TRACE("\r\n-|14-lowpower 15-ksr 16-work com 17-keyzone 18-ic       |-");
        TRACE("\r\n-|19-exflash 20-pinpad 21-adc 22-rsa 23-usb 24-LCD      |-");
        TRACE("\r\n-|25-buzzer 26-proto 27-font 28-bt 29-rfid 30-dlboot    |-");
        TRACE("\r\n-|31-version 32-console 33-pinpad 34-gpio 36-uartdma    |-");
        TRACE("\r\n-|37-key_new 38-get_rand 39-gothrouth_rec 40-fac_log    |-");
        TRACE("\r\n-|41-IC卡解锁 42-权限包 43-写SN/主板ID 50-libapi        |-");
        TRACE("\r\n-|51-yanchao 52-idcard 53-fingerprint 54-lowpower       |-");
        TRACE("\r\n-|66-localdl 67-scan 80-reset_ini 81-reset_hard 88-reset|-");
        TRACE("\r\n-|82-get_boot_ver 83-dryice status 84-crc16             |-");
        TRACE("\r\n-|******************************************************|-\t");
//        if ( flg == 0 ) {
//            key = 68;
//            flg = 1;
//        }else
            key = InkeyCount(0);
        switch (key)
        {
#ifdef CFG_DBG_UART
        case 1:
            dbg_uart();
            break;
#endif
#ifdef CFG_DBG_MCG
        case 2:
            dbg_mcg(gSysBuf_c.console,CNL_BUFSIZE_C);
            break;
#endif
#ifdef CFG_DBG_FLASH
        case 3:
            dbg_flash();
            break;
#endif
#ifdef CFG_DBG_RTC
        case 4:
            dbg_rtc();
            break;
#endif
        case 4:
            sys_GetTime(tempbuff);
            TRACE_BUF("rtc", tempbuff, 8);
            break;

#ifdef CFG_DBG_RNG
        case 5:
            dbg_rnga();
            break;
#endif
#ifdef CFG_DBG_SECURYTY
        case 6:
            dbg_security();
            break;
#endif
#ifdef CFG_DBG_TIME
        case 7:
            dbg_time();
            break;
#endif
#ifdef CFG_DBG_CRC
        case 8:
            dbg_crc_main();
            break;
#endif
#ifdef CFG_DBG_DRYICE
        case 9:
            dbg_dryice();
            break;
#endif
#ifdef CFG_DBG_KEYBOARD
        case 10:
            drv_kb_open();
            drv_kb_NotMaxtrix_clear_irq();
            gwp30SysMemory.SysTickDeamon.bit.keybrd = 1;
            dbg_key();
            break;
#endif
#ifdef CFG_DBG_MAGTECK 
        case 11:
            dbg_magtek();
            break;
#endif
#ifdef CFG_DBG_DELAY
        case 12:
            dbg_delay();
            break;
#endif
#ifdef CFG_DBG_LED
        case 13:
            dbg_led();
            break;
#endif
#ifdef CFG_DBG_LPWR
        case 14:
            dbg_lpwr();
            break;
#endif
#ifdef CFG_DBG_CTRL_KSR 
        case 15:
            dbg_ksr();
            break;
#endif
#ifdef CFG_DBG_CTRL_WORK_COMPORT 
        case 16:
            dbg_work_uart();
            break;
#endif
#ifdef CFG_DBG_CTRL_KSR 
        case 17:
            dbg_keyzone();
            break;
#endif
        case 18:
#ifdef CFG_DBG_ICCARD 
#endif
            vTest_Icc();
            break;
#ifdef CFG_DBG_EXFLASH
        case 19:
            dbg_exflash();
            break;
#endif
#ifdef CFG_DBG_PINPAD
        case 20:
            dbg_pinpad();
            break;
#endif
#ifdef CFG_DBG_ADC
        case 21:
//            dbg_adc();
            dbg_power_charge_adc();
            break;
#endif
#ifdef CFG_DBG_RSA
        case 22:
            dbg_rsa();
            break;
#endif
#ifdef CFG_DBG_USB
#warning "yes usb"
        case 23:
            dbg_usb();
            break;
#endif
        case 24:
#ifdef CFG_DBG_LCD
            dbg_app_lcd();
#endif
            break;
        case 25:
            dbg_buzzer();
            break;
#ifdef CFG_DBG_PROTOTYPE
        case 26:
            proto_main();
            break;
#endif
        case 27:
//            dbg_font_lab();
            break;
#ifdef CFG_DBG_BT
        case 28:
            dbg_bluetooth();
            break;
#endif
#ifdef CFG_DBG_RFID
        case 29:
            dbg_rfid();
            break;
#endif
        case 30:
            dbg_local_down();
            break;
        case 31:
            dbg_version();
            break;
        case 32:
            dbg_console();
            break;
#if (defined CFG_EXTERN_PINPAD) || (defined CFG_INSIDE_PINPAD)
        case 33:
            PadMain();
            break;
#endif
        case 34:
            dbg_gpio();
            break;

        case 35:
            dbg_tprint();
            break;
        case 36:
            dbg_uartdma();
            break;
        case 37:
//            kb_getstr1(0, 0, 20, -1, (char *)tempbuff);
            break;
        case 38:
            memset(tempbuff, 0, sizeof(tempbuff));
            tempbuff[0]=0x08;
            ped_get_rand(2, tempbuff, &randlen, &tempbuff[0]);
            TRACE("\r\nget the rand len:%d\r\n",randlen);
            TRACE("get the rand:%02X,%02X,%02X\r\n",tempbuff[0],tempbuff[1],tempbuff[2]);
            break;
        case 39:
            ret = InkeyStrToHex((char *)tempbuff+4);
            TRACE_BUF("HEX value", tempbuff+4, ret);
            tempbuff[0] = (ret>>0 )&0xff;
            tempbuff[1] = (ret>>8 )&0xff;
            tempbuff[2] = (ret>>16)&0xff;
            tempbuff[3] = (ret>>24)&0xff;
            ctc_recev_frame_debug(tempbuff, ret+4);
            break;
        case 40:
            dbg_fac_log();
            break;
        case 41:
            CLRBUF(tempbuff);
            TRACE("\r\n 请输入密码:");
            InkeyStr((char *)tempbuff);
            TRACE("\r\n 0-降级解锁 1-防拆解锁 2-镜像特权恢复 255-版本");
            ret = s_OperAuthICCard(InkeyCount(0),strlen((char *)tempbuff),(char *)tempbuff,&i,(char *)&tempbuff[128]);
            TRACE("\r\n ret:%d %d [%s]",ret,i,&tempbuff[128]);
            break;
        case 42:
            test_se_cert(0);
            break;
        case 43:
            test_sn_id(0);
            break;
#if defined (CFG_LIBAPI_DEBUG)
        case 50:
            dbg_libapi_main();
            break;
#endif
        case 51:
#ifdef CFG_CURRENCY_DETECT
            currencyDetect_test();
#endif
            break;
        case 52:
#ifdef CFG_RFID_IDCARD
            idcard_test();
#endif
            break;
        case 54:
            test_lowpower();
            break;

        case 66:
            dl_process(0);
            break;
        case 67:
            test_scan();
            break;
        case 68:
            dbg_test();
            break;
        case 69:
            com_em3096_open();
            break;
        case 80:
            power_reset_init();
            break;
        case 81:
            power_reset();
            break;
        case 82:
            sys_read_ver(READ_BOOT_VER,(char *)tempbuff);
            TRACE("BOOT:%s",tempbuff);
//            power_reset_input();
//            power_reset_get();
            break;
        case 83:
            DRY_SR_REG(DRY) = 0x3f0030;
//            sys_read_ver(READ_CTRL_VER,(char *)tempbuff);
//            TRACE("CTRL:%s",tempbuff);
            break;
        case 84:
            randlen = calc_crc16("123456789012345678", 18, 0);
            TRACE("get result:%x\r\n", randlen);
            break;
        case 88:
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
            NVIC_SystemReset();
            break;
        case 99:
            TRACE("\r\n");
            return;
        default:
            break;
        }
    }
#endif
}


