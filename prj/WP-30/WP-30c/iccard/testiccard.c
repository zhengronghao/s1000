#include "wp30_ctrl.h"

#ifdef CFG_ICCARD
extern void DispData(uchar *title, uint32_t len, uchar *pucData);
//获取4个随机数
const uchar gApduGet4rand[5] = {0x00,0x84,0x00,0x00,0x04};
//选择主目录
const uchar gApduSelMaindir[8] = {0x00,0xA4,0x00,0x00,0x02,0x3F,0x00,0x00};
#ifdef DEBUG_Dx 
int test_select_cardslot(void)
{
	int key = 0;
	int maxslot,verno;
	icc_GetInfo(&verno,&maxslot);
	lcd_cls();
	while(1)
	{
		lcdDispMultiLang(0, FONT_SIZE12*0, FONT_SIZE12|DISP_INVLINE|DISP_MEDIACY, "选择卡座","Select CardSlot");
		lcd_display(0, FONT_SIZE12*1,FONT_SIZE12, "1-UserCard 2-SAM1");
		if(maxslot >= SAM2SLOT)
		{
			lcd_display(0, FONT_SIZE12*2,FONT_SIZE12,"3-SAM2");
		}
		if(maxslot >= SAM3SLOT)
		{
			lcd_display(63,FONT_SIZE12*2,FONT_SIZE12,"4-SAM3");
		}
		if(maxslot >= SAM4SLOT)
		{
			lcd_display(0, FONT_SIZE12*3,FONT_SIZE12,"5-SAM4");
		}
		if(maxslot >= SAM5SLOT)
		{
			lcd_display(63,FONT_SIZE12*3,FONT_SIZE12,"6-SAM5");
			lcd_display(0, FONT_SIZE12*4,FONT_SIZE12,"7-SAM6");
			lcd_display(63,FONT_SIZE12*4,FONT_SIZE12,"8-SAM7");
		}

		key = drv_kb_getkey(-1);
		switch(key)
		{
			case KEY1:
				return USERCARD;
			case KEY2:
				return SAM1SLOT;
			case KEY3:
				if(maxslot >= SAM2SLOT)
					return SAM2SLOT;
				break;
			case KEY4:
				if(maxslot >= SAM3SLOT)
					return SAM3SLOT;
				break;
			case KEY5:
				if(maxslot >= SAM4SLOT)
					return SAM4SLOT;
				break;
			case KEY6:
				if(maxslot >= SAM5SLOT)
					return SAM5SLOT;
				break;
			case KEY7:
				if(maxslot >= SAM5SLOT)
					return SAM6SLOT;
				break;
			case KEY8:
				if(maxslot >= SAM5SLOT)
					return SAM7SLOT;
				break;
			case KEY_CANCEL:
				return KEY_CANCEL;
			default:
				break;
		}
	}
	//return KEY_CANCEL;
}

void test_iccard_powerup(int cardslot)
{
	int len = 0;
	uchar atrbuf[100];
	int ret = 0;

	lcd_cls();
	lcd_display(0, 0, FONT_SIZE12|DISP_INVLINE|DISP_MEDIACY, "PowerUp Card");
	ret = icc_InitModule(cardslot, VCC_5, ISOMODE);
	if(ret)
	{
		lcd_display(0,4*8, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY, "Init Ret:%d",ret);
		drv_kb_getkey(-1);
		return;
	}

	memset(atrbuf, 0x00, sizeof(atrbuf));
	ret = icc_Reset(cardslot,(uint *)&len,(uchar *)atrbuf);
	if(ret)
	{
		lcd_display(0,4*8, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY, "Reset Ret:%d",ret);
		drv_kb_getkey(-1);
		return;
	}

	DispData(NULL,len, atrbuf);
}

void test_iccard_exchange(int cardslot)
{
	int ret = 0;
	int sendlen = 0;
	int recvlen = 0;
	uchar sendbuf[256];
	uchar recvbuf[256];

	memset(sendbuf, 0x00, sizeof(sendbuf));
	memset(recvbuf, 0x00, sizeof(recvbuf));

#if 0
	if(cardslot == USERCARD)
	{
		sendbuf[0] = 0x00;
		sendbuf[1] = 0xA4;
		sendbuf[2] = 0x04;
		sendbuf[3] = 0x00;
		sendbuf[4] = 0x0E;
		memcpy(sendbuf+5, "1PAY.SYS.DDF01", 14);
		sendbuf[19] = 0x00;
		sendlen = 20;
	}
	else
	{
        sendbuf[0] = 0x00;
        sendbuf[1] = 0xA4;
        sendbuf[2] = 0x00;
        sendbuf[3] = 0x00;
        sendbuf[4] = 0x02;
        memcpy(sendbuf+5, "\x3F\x00", 2);
        sendlen = 7;
	}
#endif
/*     sendbuf[0] = 0x00; */
/*     sendbuf[1] = 0x84; */
/*     sendbuf[2] = 0x00; */
/*     sendbuf[3] = 0x00; */
/*     sendbuf[4] = 0x04; */
/*     sendlen = 5;       */
    sendbuf[0] = 0x00;
    sendbuf[1] = 0xA4;
    sendbuf[2] = 0x00;
    sendbuf[3] = 0x00;
    sendbuf[4] = 0x02;
    memcpy(sendbuf+5, "\x3F\x00", 2);
    sendlen = 7;

	lcd_cls();
	lcd_display(0, 0, FONT_SIZE12|DISP_INVLINE|DISP_MEDIACY, "APDU Exchange");
	ret = icc_ExchangeData(cardslot,sendlen,sendbuf,&recvlen,recvbuf);
	if(ret)
	{
		lcd_display(0,4*8, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,"APDU Ret:%d",ret);
		drv_kb_getkey(-1);
		return;
	}

	DispData(NULL,recvlen, recvbuf);

#if 0
	if(recvbuf[recvlen-2] == 0x9F)
	{
		sendbuf[0] = 0xA0;
		sendbuf[1] = 0xC0;
		sendbuf[2] = 0x00;
		sendbuf[3] = 0x00;
		sendbuf[4] = recvbuf[recvlen-1];
		sendlen = 5;
		lcd_clrline(16, 63);
		ret = icc_ExchangeData(cardslot,sendlen,sendbuf,&recvlen,recvbuf);
		if(ret)
		{
			lcd_display(0,4*8, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,"APDU Ret:%d",ret);
			drv_kb_getkey(-1);
			return;
		}
		else
		{
			DispData(NULL,recvlen, recvbuf);
		}
	}
#endif
}

void test_iccard_powerdown(int cardslot)
{
	icc_Close(cardslot);
}

void test_iccard_loopback(int cardslot)
{
	int len = 0;
	uchar atrbuf[100];
	int ret = 0;
	int sendlen = 0;
	int recvlen = 0;
	uchar sendbuf[256];
	uchar recvbuf[256];

	lcd_cls();
    if ( cardslot == USERCARD ) {
        lcd_display(0, 0, FONT_SIZE12|DISP_INVLINE|DISP_MEDIACY, "User Slot Test");
    } else {
        lcd_display(0, 0, FONT_SIZE12|DISP_INVLINE|DISP_MEDIACY, "SAM%d Slot Test",cardslot);
    }
	while(1)
	{
		lcd_display(0, 2*8, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY, "PowerUp Card...");
		icc_InitModule(cardslot, VCC_5, ISOMODE);
		memset(atrbuf, 0x00, sizeof(atrbuf));
		ret = icc_Reset(cardslot,(uint *)&len,(uchar *)atrbuf);
		lcd_display(0,4*8, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY, "Ret:%d",ret);
		if(ret)
		{
			drv_kb_getkey(-1);
			return;
		}
		else
		{
			sys_delay_ms(100);
		}

		lcd_display(0, 2*8, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY, "Exchange Data...");
		memset(sendbuf, 0x00, sizeof(sendbuf));
		memset(recvbuf, 0x00, sizeof(recvbuf));

		if(cardslot == USERCARD)
		{
			sendbuf[0] = 0x00;
			sendbuf[1] = 0xA4;
			sendbuf[2] = 0x04;
			sendbuf[3] = 0x00;
			sendbuf[4] = 0x0E;
			memcpy(sendbuf+5, "1PAY.SYS.DDF01", 14);
			sendbuf[19] = 0x00;
			sendlen = 20;
		}
		else
		{
			sendbuf[0] = 0x00;
			sendbuf[1] = 0xA4;
			sendbuf[2] = 0x00;
			sendbuf[3] = 0x00;
			sendbuf[4] = 0x02;
			memcpy(sendbuf+5, "\x3F\x00", 2);
			sendbuf[7] = 0x00;
			sendlen = 8;
		}
		ret = icc_ExchangeData(cardslot,sendlen,sendbuf,&recvlen,recvbuf);
		lcd_display(0,4*8, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,"Ret:%d",ret);
		if(ret)
		{
			drv_kb_getkey(-1);
			return;
		}

		sys_delay_ms(100);
		if(recvbuf[recvlen-2] == 0x9F)
		{
			sendbuf[0] = 0xA0;
			sendbuf[1] = 0xC0;
			sendbuf[2] = 0x00;
			sendbuf[3] = 0x00;
			sendbuf[4] = recvbuf[recvlen-1];
			sendlen = 5;
			ret = icc_ExchangeData(cardslot,sendlen,sendbuf,&recvlen,recvbuf);
			lcd_display(0,4*8, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY,"Ret:%d",ret);
			if(ret)
			{
				drv_kb_getkey(-1);
				return;
			}
		}

		sys_delay_ms(100);
		icc_Close(cardslot);

		if(kb_hit() == YES)
		{
			if(drv_kb_getkey(0) == KEY_CANCEL)
			{
				lcd_cls();
				return;
			}
		}
		sys_delay_ms(100);
	}
}

int test_iccard(int mode)
{
	int cardslot = 0;
	int key = 0;
	int verno,maxslot;
	icc_GetInfo(&verno, &maxslot);
	while(1)
	{
		lcd_cls();
		lcd_display(0, 0, FONT_SIZE16|DISP_INVLINE|DISP_MEDIACY, "ICCARD TEST%d",verno);
		lcd_display(0,2*8, FONT_SIZE8|DISP_CLRLINE,"1-Select CardSlot");
		lcd_display(0,3*8, FONT_SIZE8|DISP_CLRLINE,"2-PowerUp Card");
		lcd_display(0,4*8, FONT_SIZE8|DISP_CLRLINE,"3-Exchange APDU");
		lcd_display(0,5*8, FONT_SIZE8|DISP_CLRLINE,"4-Power Down");
		lcd_display(0,6*8, FONT_SIZE8|DISP_CLRLINE,"5-LoopBack");
		lcd_display(0,7*8, FONT_SIZE8|DISP_CLRLINE,"Cancel - Exit");

		key = drv_kb_getkey(-1);
		switch(key)
		{
			case KEY1:
				cardslot = test_select_cardslot();
				if(cardslot == KEY_CANCEL)
				{
					return 0;
				}
				break;
			case KEY2:
				test_iccard_powerup(cardslot);
				break;
			case KEY3:
				test_iccard_exchange(cardslot);
				break;
			case KEY4:
				test_iccard_powerdown(cardslot);
				break;
			case KEY5:
				test_iccard_loopback(cardslot);
				break;
			case KEY_CANCEL:
				return 0;
			default:
				break;
		}
	}
	//return 0;
}

#endif

#endif
