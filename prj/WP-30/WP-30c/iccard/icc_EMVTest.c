
#include "wp30_ctrl.h"

#ifdef DEBUG_Dx

//#define DEBUG_ICCARD_EMV

#ifdef DEBUG_ICCARD_EMV

#define Dprintk(...)  TRACE(__VA_ARGS__)
#define EMV_VOLTAGE   VCC_3//VCC_5
void DispICCardRet(int iRet);
void Test_TDA8035(void);
void ICCardHardTest(void);
int guiDebugElectricTime=6500;
int guiDebugProtoclTime=2000;
int guiElectricCycleFlg=0;

void display(int line, const char *format, ...)
{
//	int       count=0;
	va_list     marker;
	char        buff[256 + 4];	/* 1K 就够了，8K太浪费 */

	memset(buff, 0, sizeof(buff));
	va_start( marker, format);
    vsnprintf(buff, sizeof(buff)-4, format, marker);
	va_end( marker );
    lcd_display(0,FONT_SIZE12*(line-1),FONT_SIZE12|DISP_CLRLINE,"%s",buff);
}

void displayArry(uchar *pdata, uint len, int row)
{
    int i;
    int n;
    int line;
    char disData[8][44];
    char *ptmpData = (char *)pdata;

    n = 0;
    line = 0;
    memset(disData, 0, sizeof(disData));
    while (len--)
    {
        sprintf(&disData[line][n], "%02x|", *ptmpData);
        n += 3;
        ptmpData++;
        if (n >= 24)
        {
            disData[line][n] = 0;
            n = 0;
            line++;
            if (line >= 8)
            {
                break;
            }
        }
    }

    for (i = 0; i <= line; i++)
    {
        lcd_display(0, row, DISP_FONT | DISP_CLRLINE, disData[i]);
        row += (LCD_WIDE / 5);
        if (row >= LCD_WIDE)
        {
            break;
        }
    }
    for (; row <= (LCD_WIDE - (LCD_WIDE / 5)); row += (LCD_WIDE / 5))
    {
        lcd_display(0, row, DISP_FONT | DISP_CLRLINE, " ");
    }
}

int active_card(void)
{
	uchar atr[256];
	uint AtrLen = 0;
	int ret;

	memset(atr, 0x00, sizeof(atr));
    icc_InitModule(USERCARD, EMV_VOLTAGE, EMVMODE);
	ret = icc_Reset(USERCARD, &AtrLen, atr);
	if(ret)
		return -1;
	else
		return 0;
}

int select_pse(void)
{
	int ret;
	int sendlen = 0, recvlen = 0;
	uchar senddata[300], recvdata[300];

	memset(senddata, 0x00, sizeof(senddata));
	memset(recvdata, 0x00, sizeof(recvdata));
	senddata[0] = 0x00;
	senddata[1] = 0xA4;
  	senddata[2] = 0x04;  // P1=04,文件名选择
   	senddata[3] = 0x00;
   	senddata[4] = 0x0E;
	memcpy(senddata+5,"1PAY.SYS.DDF01", 14); //文件名
   	senddata[19] = 0x00;
	sendlen = 20;

	ret = icc_ExchangeData(USERCARD,sendlen,senddata,&recvlen,recvdata);
	// 6A 81:不支持此功能
	// 6A 82:文件未找到
	// 6A 86:P1 P2参数错误
	// 67 00:长度错误
	if(!ret)
	{
		if((recvlen >= 2)
			&& (recvdata[recvlen-2] == 0x90)
			&& (recvdata[recvlen-1] == 0x00))
		return 0;
	}
	return -1;
}

int get_card_record(void)
{
	int ret;
	int sendlen = 0, recvlen = 0;
	uchar senddata[300], recvdata[300];

	memset(senddata, 0x00, sizeof(senddata));
	memset(recvdata, 0x00, sizeof(recvdata));

	senddata[0] = 0x00;
	senddata[1] = 0xB2;
  	senddata[2] = 0x01; //P1=01,第1条记录
   	senddata[3] = 0x0C; //P2, D7~D3:0000(当前文件),1  D2~D0:100(P1为记录号)
   	senddata[4] = 0x00;
	sendlen = 5;

    do
    {
		recvlen = 0;
		memset(recvdata, 0x00, sizeof(recvdata));
		ret = icc_ExchangeData(USERCARD,sendlen,senddata,&recvlen,recvdata);
		// 67 00:长度错误Lc不存在
		// 69 81:命令与文件结构不相容
		// 6A 81:不支持此功能
		// 6A 82:文件未找到
		// 6A 83:未找到记录
		// 6A 86:P1 P2参数错误

		if(!ret)
		{
			if((recvlen >= 2)
				&& (recvdata[recvlen-2] == 0x90)
				&& (recvdata[recvlen-1] == 0x00))
			{
				senddata[2] = senddata[2] + 0x01;
			}
			else if((recvlen >= 2)
					&& (recvdata[recvlen-2] == 0x6A)
					&& (recvdata[recvlen-1] == 0x83))
			{
				return 0;
			}
			else
				return -1;
		}
		else
		{
			return -1;
		}

		//sys_delay_ms(200);
    }while(guiElectricCycleFlg);
  	return -1;
}
//选择AID应用文件
int select_visa_aid(void)
{
	int ret;
	uchar VisaAid[] = {0xA0,0x00,0x00,0x00,0x03,0x10,0x10};
	int sendlen = 0, recvlen = 0;
	uchar senddata[300], recvdata[300];

	memset(senddata, 0x00, sizeof(senddata));
	memset(recvdata, 0x00, sizeof(recvdata));
	senddata[0] = 0x00;
	senddata[1] = 0xA4;
  	senddata[2] = 0x04;
   	senddata[3] = 0x00;
   	senddata[4] = 0x07;
	memcpy(senddata+5, VisaAid, 7);
   	senddata[12] = 0;
	sendlen = 13;

	ret = icc_ExchangeData(USERCARD,sendlen,senddata,&recvlen,recvdata);
	if(!ret)
	{
		if((recvlen >= 2)
			&& (recvdata[recvlen-2] == 0x90)
			&& (recvdata[recvlen-1] == 0x00))
			return 0;
	}
	return -1;
}

static uchar ComputeLrc(int len, uchar *data)
{
	uchar lrc;
	int i;

	lrc = 0;
	for (i = 0; i < len; i++)
		lrc ^= data[i];
	return lrc;
}

void electric_single_test(void)
{
    int ret;
	display(4,"Contact Active");
	ret = active_card();
	if(!ret)
	{
		display(4,"Select PSE");
		//选择PSE
		ret = select_pse();
		if(!ret)
		{
            display(4,"Read Record");
			//读记录
			ret = get_card_record();
			if(!ret)
			{
                display(4,"Select AID");
				ret = select_visa_aid();
			}//AID
		}//Record
	}//PSE

	display(4,"Contact Deactive");
	icc_Close(USERCARD);
	if (!ret)
	    display(5,"    Success!");
	else
		display(5,"    Failure %d!",ret);

	return;
}

void electric_cycle_test(void)
{
	int i,j=0;
	kb_flush();
	while(1)
	{

		lcd_display(0,DISP_FONT_LINE1,DISP_CLRLINE|DISP_FONT, "current:%d ",++j);
		display(4,"'Cancel' - return");
		if((kb_hit() == YES))
		{
			i = kb_getkey(-1);
			//display(1,"Key:%d",i);
			//kb_getkey(-1);
			if(i == KEY_CANCEL)
			{
				return;
			}
		}
		electric_single_test();
		sys_delay_ms(guiDebugElectricTime);
	}
}

void electric_test(void)
{
	int key;

	icc_Close(USERCARD);

    while(1)
    {
    	lcd_cls();
		lcd_display(0,DISP_FONT_LINE0,DISP_INVLINE|DISP_MEDIACY|DISP_CLRLINE|DISP_FONT, "Electric Test");
	    lcd_display(0,DISP_FONT_LINE1,DISP_CLRLINE|DISP_FONT, "1-Signel Test");
		lcd_display(0,DISP_FONT_LINE2,DISP_CLRLINE|DISP_FONT, "2-CyCle Test");
		lcd_display(0,DISP_FONT_LINE3,DISP_CLRLINE|DISP_FONT, "0-Exit");
		key = kb_getkey(-1);
        lcd_ClrLine(16,63);
		switch(key)
		{
		    case KEY1:
			//case KEY_ENTER:
				lcd_cls();
                lcd_display(0,DISP_FONT_LINE0,DISP_CLRLINE|DISP_FONT|DISP_MEDIACY|DISP_INVLINE, "Single Test");
	    		electric_single_test();
				sys_delay_ms(2000);
	    		break;
			case KEY2:
				lcd_cls();
				lcd_display(0,DISP_FONT_LINE0,DISP_CLRLINE|DISP_FONT|DISP_MEDIACY|DISP_INVLINE, "Cycle Test");
	     		electric_cycle_test();
	     		break;
			case KEY_CANCEL:
			case KEY0:
				return;
			default:
				break;
		}
    }//while(1)
}
//协议测试就是测试选择Aid文件
void protocol_single_test(void)
{
	int i, ret;
	uchar lrc;
	uchar VisaAid[] = {0xA0,0x00,0x00,0x00,0x03,0x10,0x10};
	int sendlen = 0, recvlen = 0;
	uchar senddata[300], recvdata[300];
	display(4,"Contact Activing");
   	ret = active_card();
   	if (ret)
   	{
   		icc_Close(USERCARD);
		display(4,"Power Up Error %d!",ret);
   		return;
   	}

	memset(senddata, 0x00, sizeof(senddata));
	memset(recvdata, 0x00, sizeof(recvdata));

	senddata[0] = 0x00;
	senddata[1] = 0xA4;
  	senddata[2] = 0x04;
   	senddata[3] = 0x00;
   	senddata[4] = 0x07;
	memcpy (senddata+5, VisaAid, 7);
   	senddata[12] = 0x00;
	sendlen = 13;

   	while(1)
   	{
		display(4,"APDU Exchange...");
   		ret = icc_ExchangeData(USERCARD, sendlen, senddata, &recvlen, recvdata);
   		if(ret)
   			break;
		if((recvlen >= 2)
			&& (recvdata[recvlen-2] == 0x6A)
			&& (recvdata[recvlen-1] == 0x82))
			break;

   		lrc = ComputeLrc(recvlen, recvdata);
   		if(recvlen<8)
   		{
   			//选中Aid
			senddata[0] = 0x00;
			senddata[1] = 0xA4;
		  	senddata[2] = 0x04;
		   	senddata[3] = lrc;
		   	senddata[4] = 0x07;
			memcpy (senddata+5, VisaAid, 7);
		   	senddata[12] = 0x00;
			sendlen = 13;
   			continue;
   		}
   		else
   		{
   			senddata[0] = recvdata[0];
			senddata[1] = recvdata[1];
		  	senddata[2] = recvdata[2];
   			senddata[3] = lrc;
   			switch(recvdata[3]) //R4
   			{
   				case 1:
   					sendlen = 4;
   					break;

   				case 2:
					senddata[4] = recvdata[5];
   					sendlen = 5;
   					break;

   				case 3:
   					senddata[4] = recvdata[4]; //R5
   					for(i=0; i<recvdata[4]; i++)
   					{
   						senddata[5+i] = i;
   					}
   					sendlen = 5+recvdata[4];
   					break;

   				case 4:
   					senddata[4] = recvdata[4]; //R5
   					for (i=0; i<recvdata[4]; i++)
   					{
   						senddata[5+i] = i;
   					}
					senddata[5+recvdata[4]] = recvdata[5];
					sendlen = 6+recvdata[4];
   					break;
   			}
   		}
   	}

   	icc_Close(USERCARD);
   	if(ret)
   	{
		display(5,"APDU Error %d!",ret);
   	}
   	else
   	{
		display(5,"SW=0x6A82");
   	}
   	return;
}

void protocol_cycle_test(void)
{
	int i,j=0;
	kb_flush();
	while(1)
	{
		lcd_display(0,DISP_FONT_LINE1,DISP_CLRLINE|DISP_FONT,"Current:%d ",++j);
		display(8,"'Cancel' - return");
		if((kb_hit() == YES))
		{
			i = kb_getkey(-1);
			//display(1,"Key:%d",i);
			//kb_getkey(-1);
			if(i == KEY_CANCEL)
			{
				return;
			}
		}
		protocol_single_test();
		sys_delay_ms(guiDebugProtoclTime);
	}
}

void protocol_test(void)
{
	int key;

	icc_Close(USERCARD);
	lcd_cls();
    while(1)
    {
		lcd_display(0,DISP_FONT_LINE0,DISP_INVLINE|DISP_MEDIACY|DISP_CLRLINE|DISP_FONT, "Protocol Test");
		lcd_display(0,DISP_FONT_LINE1,DISP_CLRLINE|DISP_FONT, "1-Single Test");
		lcd_display(0,DISP_FONT_LINE2,DISP_CLRLINE|DISP_FONT, "2-CyCle Test");
		lcd_display(0,DISP_FONT_LINE3,DISP_CLRLINE|DISP_FONT, "0-Exit");
		key = kb_getkey(-1);
        lcd_ClrLine(16,63);
		switch(key)
		{
		    case KEY1:
			//case KEY_ENTER:
				lcd_cls();
                lcd_display(0,DISP_FONT_LINE0,DISP_CLRLINE|DISP_FONT|DISP_MEDIACY|DISP_INVLINE,"Single Test");
	    		protocol_single_test();
				sys_delay_ms(2000);
	    		break;
			case KEY2:
				lcd_cls();
				lcd_display(0,DISP_FONT_LINE0,DISP_CLRLINE|DISP_FONT|DISP_MEDIACY|DISP_INVLINE,"Cycle Test");
	     		protocol_cycle_test();
	     		break;
			case KEY_CANCEL:
			case KEY0:
				return;
			default:
				break;
		}
    }//while(1)
}

void disp_data(int len, uchar *pucData)
{
    int i = 0, j = 0, k = DISP_FONT;

	for(j = 0; j < len; j++)
	{
        lcd_display(i, k, DISP_FONT, "%02x", pucData[j]);
		i = i + DISP_FONT+8;
		if(i >= 128)
		{
		   i = 0;
		   k += DISP_FONT;
		}

		if(k >= 63)
		{
		    kb_getkey(-1);
			lcd_cls();
			k = DISP_FONT;
		}
	}
}

void atr_test(void)
{
	uchar atr[256];
	uint AtrLen = 0;
	int ret;

	icc_Close(USERCARD);

	lcd_cls();
	lcd_display(0,DISP_FONT_LINE0,DISP_MEDIACY|DISP_FONT|DISP_CLRLINE|DISP_INVLINE,"Reset Test");
	icc_InitModule(USERCARD, EMV_VOLTAGE, EMVMODE);
	ret = icc_Reset(USERCARD, &AtrLen, atr);
	if(!ret)
	{
		disp_data(AtrLen, atr);
	}
	else
	{
		lcd_display(0,DISP_FONT_LINE1,DISP_MEDIACY|DISP_FONT|DISP_CLRLINE,"Ret: %d",ret);
	}
	kb_getkey(-1);
}

void select_test(void)
{
	int ret;
	int sendlen = 0, recvlen = 0;
	uchar senddata[300], recvdata[300];

	lcd_cls();
	lcd_display(0,DISP_FONT_LINE0,DISP_MEDIACY|DISP_FONT|DISP_CLRLINE|DISP_INVLINE,"SelectPSE Test");

	memset(senddata, 0x00, sizeof(senddata));
	memset(recvdata, 0x00, sizeof(recvdata));
	senddata[0] = 0x00;
	senddata[1] = 0xA4;
  	senddata[2] = 0x04;
   	senddata[3] = 0x00;
   	senddata[4] = 0x0E;
	memcpy(senddata+5,"1PAY.SYS.DDF01", 14);
   	senddata[19] = 0x00;
	sendlen = 20;

	ret = icc_ExchangeData(USERCARD,sendlen,senddata,&recvlen,recvdata);
	if(!ret)
	{
		disp_data(recvlen, recvdata);
	}
	else
	{
		lcd_display(0,DISP_FONT_LINE1,DISP_MEDIACY|DISP_FONT|DISP_CLRLINE,"Select PSE Error");
	}
	kb_getkey(-1);
}

void read_test(void)
{
	int ret;
	int sendlen = 0, recvlen = 0;
	uchar senddata[300], recvdata[300];

	lcd_cls();
	lcd_display(0,DISP_FONT_LINE0,DISP_MEDIACY|DISP_FONT|DISP_CLRLINE|DISP_INVLINE,"ReadRecord Test");

	memset(senddata, 0x00, sizeof(senddata));
	memset(recvdata, 0x00, sizeof(recvdata));
	senddata[0] = 0x00;
	senddata[1] = 0xB2;
  	senddata[2] = 0x01;
   	senddata[3] = 0x0C;
   	senddata[4] = 0x00;
	sendlen = 5;

 	ret = icc_ExchangeData(USERCARD, sendlen, senddata, &recvlen, recvdata);
	if(!ret)
	{
		disp_data(recvlen, recvdata);
	}
	else
	{
		lcd_display(0,DISP_FONT_LINE1,DISP_MEDIACY|DISP_FONT|DISP_CLRLINE,"Read Error: %d",ret);
	}
	kb_getkey(-1);
}

void debug_test(void)
{
#if 0
	int key;
	disable_pio_interrupt(PIOB_PIN_(29));
    disable_pio_pullup(PIOB_PIN_(29));
    enable_pio_output(PIOB_PIN_(29));
    enable_pio(PIOB_PIN_(29));
	//set_pio_output(PIOB_PIN_(29),1);
    while(1)
    {
		lcd_cls();
		lcd_display(0,DISP_FONT_LINE0,DISP_MEDIACY|DISP_FONT|DISP_CLRLINE|DISP_INVLINE,"DEBUG TEST");
		lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"1-ATR Test");
		lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"2-Select PSE");
		lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"3-Read Record");
		lcd_display(0,DISP_FONT_LINE4,DISP_FONT|DISP_CLRLINE,"4-Deactive");
		lcd_display(0,DISP_FONT_LINE5,DISP_FONT|DISP_CLRLINE,"CANCEL - Exit");

		key = kb_getkey(-1);
		switch(key)
		{
		    case KEY_CANCEL:
			case KEY0:
				return;
			case KEY1:
			case KEY_ENTER:
				atr_test();
	    		break;
			case KEY2:
				select_test();
	    		break;
			case KEY3:
				read_test();
	    		break;
			case KEY4:
				icc_Close(USERCARD);
	    		lcd_cls();
	    		lcd_display(0,DISP_FONT_LINE1,DISP_MEDIACY|DISP_FONT|DISP_CLRLINE, "PowerDown succ");
	    		kb_getkey(-1);
	    		break;
			case KEY_F2:
				//隐藏菜单
#ifdef DEBUG_Dx
				icc_EMVDebugTest();
#endif
				break;
		}
    }//while(1)
#endif
}


/**********************************************************************************
 *
 * emv test
 *
 * chenf 05152014
 *
 ***********************************************************************************/
#define ICCEMV_TEST
//#define _EMV_PTL_DEBUG__
//#definfe _EMV_SERIAL_DEBUG_
//#define _EMV_DEBUG__
#ifdef ICCEMV_TEST
//T0 最长CLA+INS+P1+P2+LC+DATA+LE  5+255+1
//T1 最长3+254+1
typedef struct
{
	unsigned char data[261] ; // ‘CLA INS P1 P2 [Lc + Lc data] [Le]’ for
	unsigned int length ; // ‘[Le data] SW1 SW2’ for response ( max = 25
} T_APDU ;

T_APDU C_Apdu, R_Apdu ; // C_Apdu sent to the Card, R_Apdu received
//case 4
const unsigned char selectVisaCredit[]={0x00,0xA4,0x04,0x00,7,0xA0,0,0,0,3,0x10,0x10,0} ;
//case 3
const unsigned char selectVisaPPSE[]={0x00,0xA4,0x04,0x00,0x0E,
0x31,0x50,0x41,0x59,0x2E,0x53,0x59,0x53,0x2E,0x44,0x44,0x46,0x30,0x31};//1PAY.SYS.DDF01

const unsigned char selectPSE[]={0x00,0xA4,0x04,0x00,0x0e,
0x31,0x50,0x41,0x59,0x2e,0x53,0x59,0x53,0x2e,0x44,0x44,0x46,0x30,0x31,0x00};//1PAY.SYS.DDF01

const unsigned char selectrecord[]={0x00,0xb2,0x01,0x0c,0x1d};

const unsigned char readRecord1[]={0x00, 0xC0, 0x00, 0x00, 0x86};
const unsigned char readRecord2[]={0x00,0xA4,0x04,0x00,0x80,
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F};

const unsigned char readRecord3[]={0x00, 0xC0, 0x00, 0x00, 0x86};
const unsigned char readRecord4[]={0x00,0xA4,0x04,0x00,0x80,
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F};

const unsigned char readRecord5[]={0x00, 0xC0, 0x00, 0x00, 0x05};

int COLD_WARM_ATR, KEY_OR_SERIAL;
//int ReturnCode,nb_cycles;
#define TypeAPDU R_Apdu.data[3]
#define LC R_Apdu.data[4]
#define LE R_Apdu.data[5]
//#define OK 0

void delay_1ms(int count)
{
	s_DelayMs(count);
}

void vDispICCKey(uint uiLen, uchar *ucBuf)
{
    // 标志 F0~FE
    const uchar gKeybuf[] = {
        0xF2, 0xF4, 0xF5, 0xF6,
        0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC,
        0xFD, 0xFE
    };
	uint i,j,k;
	Dprintk("\r\n--------%d---------\r\n",uiLen);
	for(i=0;i<uiLen;i++)
	{
		k = 0;
		for(j=0;j<sizeof(gKeybuf);j++)
		{
			if(gKeybuf[j] == ucBuf[i] && gKeybuf[j] == ucBuf[i+1])
			{
				k = 1;
				if(gKeybuf[j] == 0xFA)
				{
					Dprintk("\r\n------------");
				}
			}
		}
		if(k)
		{
			Dprintk("\r\n%02X: ",ucBuf[i]);
			++i;
		}
		else
		{
			Dprintk("%02X ",ucBuf[i]);
		}
	}
	Dprintk("\r\n");
}

int EMV_PowerOn (uchar *atr, uint *len_buf)
{
	int ret;
	uint len;
	uchar buf[128];
	icc_InitModule(USERCARD, EMV_VOLTAGE,EMVMODE);
	ret = icc_Reset(USERCARD, &len, buf);
	if(ret)
	{
//		DispICCardRet(ret);
	}
	else
	{
		*len_buf = len;
		memcpy(atr, buf, len);
	}
	return ret;
}

int EMV_apdu (T_APDU *C_Apdu , T_APDU *R_Apdu)
{
	int ret;
	ret = icc_ExchangeData(USERCARD, C_Apdu->length, C_Apdu->data, (int *)&R_Apdu->length, R_Apdu->data);
	if(ret)
	{
		DispICCardRet(ret);
	}
	return ret;
}

unsigned char EMV_PowerOff(void)
{
	int ret;
	ret = icc_Close(USERCARD);
	return ret;
}

static void InitSelectPPSE (T_APDU *apdu)
{
	memcpy(apdu->data , selectVisaPPSE , 0x14) ;
	apdu->length = 0x14 ;
}


static void InitSelectVisa(T_APDU *apdu, int mode)
{
	if(mode == 0)
	{
		memcpy(apdu->data , selectVisaCredit , 13);
		apdu->length = 13;
	}
	else if(mode == 1)
	{
//		memcpy(apdu->data , selectPSE , 19);
//		apdu->length = 19;
		memcpy(apdu->data , selectPSE , 20);
		apdu->length = 20;
	}
	else if(mode == 2)
	{
		memcpy(apdu->data , selectrecord , 5);
		apdu->length = 5;
	}
}

//0-正常使用 1-不检测按键
#define   EM_KEYPRESS   0
int ifkeyquit(int mode)
{
    if ( mode ) {
        return 0;
    }
    if(KEY_OR_SERIAL == 0){
        if(IfInkey(0))
            return 1;
    }else{
        if(kb_hit() == YES)
        {
            kb_flush();
            return 1;
        }
    }
    return 0;
}
//0-正常使用 1-不检测按键
int TestEle1(int mode)
{
	int RetCode;
//	int DataInSize ;
//	unsigned char *ptr;
	unsigned char nVoltage = 0;
	int times;
//	unsigned char IcProtocol[2];
	uint	wLen;
	unsigned char ats[40];
//	lcd_display(0,4*8, FONT_SIZE16|DISP_CLRLINE|DISP_MEDIACY, "                        ");
//	lcd_display(0,4*8, FONT_SIZE16|DISP_CLRLINE|DISP_MEDIACY, "正在测试。。。");
//    lcd_cls();
//    lcd_display(0,1*12,DISP_CLRLINE | FONT_SIZE12|DISP_MEDIACY,"Single-test");
	Dprintk("\r\nTestEle1\n");
	RetCode = 0 ;
    nVoltage = nVoltage;
	do
	{
//step 1--------------------------------------------------------------
		nVoltage=3;			//5V power up
		RetCode=EMV_PowerOn(ats, &wLen);
#ifdef _EMV_DEBUG__
		Dprintk("\r\n icc power on");
		vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
		guiDebugi=0;
#endif
		if(RetCode != 0)
		{
			EMV_PowerOff();
			Dprintk("\r\npoweron fail ret=%d\n", RetCode);
//			return RetCode;
            break;
		}
//		Dprintk("\r\npoweron suc ret=%d,wlen=%02x atr:\n",RetCode,wLen);
//        DISPBUF(ats, wLen, 0);

//step 2--------------------------------------------------------------
//case 3
		memcpy(C_Apdu.data, selectVisaPPSE, 0x13) ;
		C_Apdu.length = 0x13 ;
		RetCode = EMV_apdu(&C_Apdu, &R_Apdu ); // Proprietary Command/Response function
        if ( ifkeyquit(mode) ) {
            RetCode = -1;
			Dprintk("\r\n step 2 key exit");
            break;
        }
#ifdef _EMV_DEBUG__
		Dprintk("\r\nselectVisaPPSE:");
		vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
		guiDebugi=0;

		Dprintk("\r\nEMV_apdu case3 selectVisaPPSE data:\n");
        DISPBUF(C_Apdu.data, C_Apdu.length, 0);
#endif
//		Dprintk("\r\nEMV_apdu case3 return RetCode=%d,R_Apdu.length=%x,sw=[%02x%02x],\n",RetCode,R_Apdu.length,R_Apdu.data[R_Apdu.length-2],R_Apdu.data[R_Apdu.length-1]);
		if((RetCode!=0) || ((R_Apdu.data[R_Apdu.length-2]==0x6a) && (R_Apdu.data[R_Apdu.length-1]==0x82)))
		{
			EMV_PowerOff();
//			return RetCode;
            break;
		}
//		Dprintk("\r\nEMV_apdu case3 selectVisaPPSE dataout:\n");
//        DISPBUF(R_Apdu.data, R_Apdu.length, 0);


//step 3--------------------------------------------------------------
		memcpy(C_Apdu.data, readRecord1, 0x05) ;
		C_Apdu.length = 0x05 ;
		RetCode = EMV_apdu(&C_Apdu, &R_Apdu ); // Proprietary Command/Response function
        if ( ifkeyquit(mode) ) {
            RetCode = -1;
			Dprintk("\r\n step 3 key exit");
            break;
        }
#ifdef _EMV_DEBUG__
		Dprintk("\r\n调试数据:");
		vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
		guiDebugi=0;

		Dprintk("\r\nEMV_apdu case2 readRecord1 data:\n");
        DISPBUF(C_Apdu.data, C_Apdu.length, 0);
#endif
//		Dprintk("\r\nEMV_apdu case2 return RetCode=%d,R_Apdu.length=%x,sw=[%02x%02x],\n",RetCode,R_Apdu.length,R_Apdu.data[R_Apdu.length-2],R_Apdu.data[R_Apdu.length-1]);
		if((RetCode!=0) || ((R_Apdu.data[R_Apdu.length-2]==0x6a) && (R_Apdu.data[R_Apdu.length-1]==0x82)))
		{
			EMV_PowerOff();
//			return RetCode;
            break;
		}
//		Dprintk("\r\nEMV_apdu case2 readRecord1 dataout:\n");
//        DISPBUF(R_Apdu.data, R_Apdu.length, 0);


//step 4--------------------------------------------------------------
//case 3
		for(times = 2; times<=63; times++)
		{
			memcpy(C_Apdu.data, readRecord2, 133) ;
			C_Apdu.length = 133 ;
			RetCode = EMV_apdu(&C_Apdu, &R_Apdu ); // Proprietary Command/Response function
            if ( ifkeyquit(mode) ) {
                RetCode = -1;
                Dprintk("\r\n step 4 key exit");
                break;
            }
#ifdef _EMV_DEBUG__
			Dprintk("\r\n调试数据:");
			vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
			guiDebugi=0;

			Dprintk("\r\nEMV_apdu case3 readRecord2 data:\n");
            DISPBUF(C_Apdu.data, C_Apdu.length, 0);
#endif
//			Dprintk("\r\nEMV_apdu case3 return RetCode=%d,R_Apdu.length=%x,sw=[%02x%02x],\n",RetCode,R_Apdu.length,R_Apdu.data[R_Apdu.length-2],R_Apdu.data[R_Apdu.length-1]);
			if((RetCode!=0) || ((R_Apdu.data[R_Apdu.length-2]==0x6a) && (R_Apdu.data[R_Apdu.length-1]==0x82)))
			{
				EMV_PowerOff();
//                return RetCode;
                break;
			}
//			Dprintk("\r\nEMV_apdu case3 readRecord2 dataout:\n");
//            DISPBUF(R_Apdu.data, R_Apdu.length, 0);


//step 5--------------------------------------------------------------
//case 2
			memcpy(C_Apdu.data, readRecord3, 5) ;
			C_Apdu.length = 5 ;
			RetCode = EMV_apdu(&C_Apdu, &R_Apdu ); // Proprietary Command/Response function
            if ( ifkeyquit(mode) ) {
                RetCode = -1;
                Dprintk("\r\n step 5 key exit");
                break;
            }
#ifdef _EMV_DEBUG__
			Dprintk("\r\n调试数据:");
			vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
			guiDebugi=0;

			Dprintk("\r\nEMV_apdu case2 readRecord3 data:\n");
            DISPBUF(C_Apdu.data, C_Apdu.length, 0);
#endif
//			Dprintk("\r\nEMV_apdu case2 return RetCode=%d,R_Apdu.length=%x,sw=[%02x%02x],\n",RetCode,R_Apdu.length,R_Apdu.data[R_Apdu.length-2],R_Apdu.data[R_Apdu.length-1]);
			if((RetCode!=0) || ((R_Apdu.data[R_Apdu.length-2]==0x6a) && (R_Apdu.data[R_Apdu.length-1]==0x82)))
			{
				EMV_PowerOff();
//                return RetCode;
                break;
			}
//			Dprintk("\r\nEMV_apdu case2 readRecord3 dataout:\n");
//            DISPBUF(R_Apdu.data, R_Apdu.length, 0);
		}

//        if ( times <= 63 ) {
//            RetCode = -1;
//            Dprintk("\r\n step 4/5 key exit");
//            break;
//        }
        if ( RetCode ) {
            break;
        }

//step 6--------------------------------------------------------------
//case 3
		memcpy(C_Apdu.data, readRecord4, 133) ;
		C_Apdu.length = 133 ;
		RetCode = EMV_apdu(&C_Apdu, &R_Apdu ); // Proprietary Command/Response function
        if ( ifkeyquit(mode) ) {
            RetCode = -1;
            Dprintk("\r\n step 6 key exit");
            break;
        }
#ifdef _EMV_DEBUG__
		Dprintk("\r\n调试数据:");
		vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
		guiDebugi=0;

		Dprintk("\r\nEMV_apdu case3 readRecord4 data:\n");
        DISPBUF(C_Apdu.data, C_Apdu.length, 0);
#endif
//		Dprintk("\r\npoEMV_apdu case3 return RetCode=%d,R_Apdu.length=%x,sw=[%02x%02x],\n",RetCode,R_Apdu.length,R_Apdu.data[R_Apdu.length-2],R_Apdu.data[R_Apdu.length-1]);
		if((RetCode!=0) || ((R_Apdu.data[R_Apdu.length-2]==0x6a) && (R_Apdu.data[R_Apdu.length-1]==0x82)))
		{
			EMV_PowerOff();
//            return RetCode;
            break;
		}
//		Dprintk("\r\npoEMV_apdu case3 readRecord4 dataout:\n");
//        DISPBUF(R_Apdu.data, R_Apdu.length, 0);


//step 7--------------------------------------------------------------
//case 6
		memcpy(C_Apdu.data, readRecord5, 5) ;
		C_Apdu.length = 5 ;
		RetCode = EMV_apdu(&C_Apdu, &R_Apdu ); // Proprietary Command/Response function
        if ( ifkeyquit(mode) ) {
            RetCode = -1;
            Dprintk("\r\n step 7 key exit");
            break;
        }
#ifdef _EMV_DEBUG__
		Dprintk("\r\n调试数据:");
		vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
		guiDebugi=0;

		Dprintk("\r\npoEMV_apdu case2 readRecord5 data:\n");
        DISPBUF(C_Apdu.data, C_Apdu.length, 0);
#endif
//		Dprintk("\r\npoEMV_apdu case2 return RetCode=%d,R_Apdu.length=%x,sw=[%02x%02x],\n",RetCode,R_Apdu.length,R_Apdu.data[R_Apdu.length-2],R_Apdu.data[R_Apdu.length-1]);
		if((RetCode!=0) || ((R_Apdu.data[R_Apdu.length-2]==0x6a) && (R_Apdu.data[R_Apdu.length-1]==0x82)))
		{
			EMV_PowerOff();
//            return RetCode;
            break;
		}
//		Dprintk("\r\npoEMV_apdu case2 readRecord5 dataout:\n");
//        DISPBUF(R_Apdu.data, R_Apdu.length, 0);

	}while (0);
	EMV_PowerOff();
//	lcd_display(0,3*12, FONT_SIZE12|DISP_CLRLINE|DISP_MEDIACY, "one turn over...");
    s_DelayMs(50);
	return (RetCode) ;
}

int TestEle2(void)
{
	int times = 0;
	int RetCode;
//	int DataInSize ;
//	unsigned char *ptr;
	unsigned char nVoltage = 0;

//	unsigned char IcProtocol[2];
	int	i;
//	unsigned char ats[40];

//	Dprintk("\r\nTestEle2\n");
	RetCode = 0 ;
    nVoltage = nVoltage;
    lcd_cls();
    lcd_display(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY| DISP_INVLINE, "Cycle-test:%d",times);
    lcd_display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "Cancel to exit");
//    lcd_display(0,0*12,DISP_CLRLINE | FONT_SIZE12|DISP_MEDIACY, "Cycle-test:%d",times);
//    lcd_display(0,4*12, FONT_SIZE12|DISP_CLRLINE, "Cancel to exit");
//    lcd_display(0,4*12, FONT_SIZE12|DISP_CLRLINE, "Anykey to exit");
//    Dprintk("\r\n------------TIMES:%d-----------", times++);
//    lcd_update_buf();
	do
	{
//		lcd_display(0,6*8, FONT_SIZE16|DISP_CLRLINE|DISP_MEDIACY, "                        ");
//		lcd_display(0,6*8, FONT_SIZE16|DISP_CLRLINE|DISP_MEDIACY, "按取消键退出循环");
		for(i = 0; i<100; i++)
		{
			if(KEY_OR_SERIAL == 0)
			{
				if(IfInkey(0))
				{
					if(InkeyCount(0) == 99)
						return 0;
				}

			}
			else
			{
				if(kb_hit() == YES)
	            {
	               if(kb_getkey(10) == KEY_CANCEL)
	               {
                       kb_flush();
	               		return 0;
	               }
//                   return 0;
	            }
			}
			delay_1ms(50);	//delay 5s
		}
        RetCode = TestEle1(0);
        if ( RetCode == -1 ) {
            //按键退出
			Dprintk("\r\npress the key to exit ret=%d",RetCode);
            break;
        }
	}while (1);
	return (RetCode) ;

}

int TestPTL(void)
{
	int times = 0;
	unsigned char RetCode, profil ;
	int DataInSize ;
	unsigned char *ptr;
	unsigned char nVoltage = 0;

//	unsigned char IcProtocol[2];
	uint	wLen,i;
	unsigned char ats[40];

	Dprintk("\r\nTestPTL");
	lcd_cls();
	lcd_display(0,0*12,DISP_CLRLINE | FONT_SIZE12 | DISP_MEDIACY , "Protocol Test");
//	lcd_display(0,4*8, FONT_SIZE16|DISP_CLRLINE|DISP_MEDIACY, "按取消键退出循环");
	lcd_display(0,4*12, FONT_SIZE12|DISP_CLRLINE, "Cancel to exit");
	RetCode = 0 ;
    nVoltage = nVoltage;
	guiDebugi=0;
	do
	{
		Dprintk("\r\n--------------------------TIMES:%d-------------------------------------", times++);
		for(i = 0; i<100; i++)
		{
			if(KEY_OR_SERIAL == 0)
			{
				if(IfInkey(0))
				{
					if(InkeyCount(0) == 99)
						return 0;
				}
			}
			else
			{
				if(kb_hit() == YES)
	            {
	               if(kb_getkey(0) == KEY_CANCEL)
	               {
	               		return 0;
	               }
				   kb_flush();
	            }
			}
			delay_1ms(50);	//delay 5s
		}

		nVoltage=3;			//5V power up
		RetCode=EMV_PowerOn(ats, &wLen);
		if (RetCode != 0)
		{
			EMV_PowerOff();
			Dprintk("\r\npoweron fail ret=%02x",RetCode);
			DispICCardRet(RetCode);
			continue;
		}
		Dprintk("\r\npoweron suc ret=%02x,wlen=%02x\n",RetCode,wLen);


		InitSelectVisa(&C_Apdu, 0) ; // First APDU Command = Select Visa Credit

#ifdef _EMV_PTL_DEBUG__
		Dprintk("\r\n调试数据:");
		vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
		guiDebugi=0;
#endif
		while(1)
		{
			RetCode = EMV_apdu(&C_Apdu, &R_Apdu ); // Proprietary Command/Response function
			Dprintk("\r\napduexchange return RetCode=%d,R_Apdu.length=%x,sw=[%02x%02x],\n",RetCode,R_Apdu.length,R_Apdu.data[R_Apdu.length-2],R_Apdu.data[R_Apdu.length-1]);

#ifdef _EMV_PTL_DEBUG__
			Dprintk("\r\n调试数据:");
			vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
			guiDebugi=0;
#endif
			if((RetCode!=0) || ((R_Apdu.data[R_Apdu.length-2]==0x6a) && (R_Apdu.data[R_Apdu.length-1]==0x82)))
			{
				EMV_PowerOff();
				break;
			}

			Dprintk("\r\npoEMV_apdu dataout=");
			for(i=0;i<R_Apdu.length;i++)
			{
				Dprintk("%02x,",R_Apdu.data[i]);
			}

			if(R_Apdu.length < 8) // Not enough data to create new command
			{
				InitSelectVisa(&C_Apdu, 0); // -> default APDU Command = Select PSE
			}
			else
			{ // R-APDU : R_Apdu.data -> CLA/INS/P1/Type APDU/Lc/Le/[data]/Me1/Me2
				memcpy(C_Apdu.data, R_Apdu.data, 3); // CLA/INS/P1
				C_Apdu.length = 4;
				ptr = C_Apdu.data + 4;
				if((TypeAPDU > 2) && LC) // Case 3 or 4 : Lc field is present
				{
					DataInSize = LC;
					C_Apdu.length += (1+DataInSize);
					*ptr++ = DataInSize;
					profil = 0;
					while(DataInSize--) // Building data string
						*ptr++ = profil++; // -> Lc data = 0,1,2...,Lc-1
				}
				if((TypeAPDU == 2) || (TypeAPDU == 4)) // Le is present
				{
					C_Apdu.length++;
					*ptr++ = LE;
				}
			}
			C_Apdu.data[3] = ComputeLrc(R_Apdu.length, R_Apdu.data); // P2
			Dprintk("\r\n LRC = %02X", C_Apdu.data[3]);
		}
	}while (1);
	EMV_PowerOff();
	return (RetCode) ;
}

/* Main function */
unsigned char TestVisa(void)
{
	unsigned char RetCode, profil ;
	unsigned short DataInSize ;
	unsigned char *ptr , atr[40] ;
	uint len;
//	ReturnCode = 0 ;
	RetCode = EMV_PowerOn(atr, &len);
	if (RetCode != OK)
	{
		return (RetCode) ;
	}
	InitSelectVisa(&C_Apdu, 0) ; // First APDU Command = Select Visa Credit
//	nb_cycles = 0 ;
	while (1)
	{
		RetCode = EMV_apdu(&C_Apdu, &R_Apdu ); // Proprietary Command/Response function
		if ((RetCode != OK) || !memcmp(&R_Apdu.data[R_Apdu.length-2], "\x6A\x82", 2))
		{
			EMV_PowerOff();
			break;
		}
		// Building new APDU command
		if ( R_Apdu.length < 8 ) // Not enough data to create new command
		{
			InitSelectVisa(&C_Apdu, 0) ; // -> default APDU Command = Select PS
		}
		else
		{ // R-APDU : R_Apdu.data -> CLA/INS/P1/Type APDU/Lc/Le/[data]/Me1/
			memcpy(C_Apdu.data , R_Apdu.data , 3); // CLA/INS/P1
			C_Apdu.length = 4 ;
			ptr = C_Apdu.data + 4 ;
			if ((TypeAPDU > 2) && LC) // Case 3 or 4 : Lc field is present
			{
				DataInSize = LC ;
				C_Apdu.length += (1+DataInSize) ;
				*ptr++ = DataInSize ;
				profil = 0 ;
				while(DataInSize--) // Building data string
				{
					*ptr++ = profil++ ; // -> Lc data = 0,1,2...,Lc-1
				}
			}
			if ((TypeAPDU == 2) || (TypeAPDU == 4)) // Le is present
			{
				C_Apdu.length++ ;
				*ptr++ = LE ;
			}
		}
		C_Apdu.data[3] = ComputeLrc (R_Apdu.length, R_Apdu.data) ; // P2
	}
	return (RetCode) ;
}

/* Main function */
//unsigned char NewTestVisa(unsigned short ctn)
int NewTestVisa(unsigned short ctn)
{
//    unsigned char RetCode, profil ;
//    unsigned short DataInSize ;
//    unsigned char *ptr  ;
    int RetCode;

    InitSelectVisa ( &C_Apdu , 1) ; // First APDU Command = Select Visa Credit

    while (1)
    {
        if(KEY_OR_SERIAL == 0)
        {
            if(IfInkey(0))
            {
                Dprintk("\r\n 99-exit");
                if(InkeyCount(0) == 99){
                    RetCode = -1;
                    break;
                }
            }
        }
        else
        {
            if(kb_hit() == YES)
            {
                if(kb_getkey(0) == KEY_CANCEL)
                {
                    RetCode = -1;
                    kb_flush();
                    break;
                }
            }
        }

        //  RetCode = PBOC_apdu(ctn, &C_Apdu, &R_Apdu );
        RetCode = EMV_apdu(&C_Apdu, &R_Apdu );
//        Dprintk("\r\n :%d, %d",i++,RetCode);
        // Proprietary Command/Response function

//		Dprintk("\r\nexchange=%02x",RetCode);
//        InkeyCount(0);
//        DISPBUF(R_Apdu.data, R_Apdu.length, 0);
        if ( (RetCode!=OK) ){
//            Dprintk("\r\n RetCode:%d",RetCode);
//            InkeyCount(0);
            break ;
        }

        if ( R_Apdu.length < 6 ){
            InitSelectVisa ( &C_Apdu , 1) ;
//            Dprintk("\r\nselect visa %d  %d",RetCode,R_Apdu.length);
            Dprintk("\r\n length<6");
        }
        else
        {
            if (R_Apdu.data[1] == 0x70)//INS ==70 结束交易
            {
//                Dprintk("\r\n test ok:%x",R_Apdu.data[1]);
//                InkeyCount(0);
                break;
            }
            memcpy ( C_Apdu.data , R_Apdu.data , R_Apdu.length-2) ;
            C_Apdu.length = R_Apdu.length-2;
            Dprintk("\r\n r-len:%d",R_Apdu.length);
//            DISPBUF(R_Apdu.data, R_Apdu.length,0);
        }
        delay_1ms(200);
    }
    return (RetCode) ;
}

//协议B流程
//timer*100ms
int TestPTL_protocolB(uint timer)
{
//	int times = 0;
    int mode = 0;
//	unsigned char RetCode, profil ;
//	int DataInSize ;
    int RetCode;
//	unsigned char *ptr;
//	unsigned char nVoltage;
    uchar testover = 0;

//	unsigned char IcProtocol[2];
	uint	wLen,i;
	unsigned char ats[40];

	Dprintk("\r\nTestPTL_protocolB");
	lcd_cls();
    lcd_display(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "protocol test:%d",timer/10);
    lcd_display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE, "Cancel to exit");
//	lcd_display(0,0*12,DISP_CLRLINE | FONT_SIZE12 | DISP_MEDIACY, "protocol test:%d",timer/10);
//	lcd_display(0,4*12, FONT_SIZE12|DISP_CLRLINE, "Cancel to exit");
//    lcd_display(0,4*12, FONT_SIZE12|DISP_CLRLINE, "Anykey to exit");
	RetCode = 0 ;
	guiDebugi=0;
    testover = 0;
//    lcd_update_buf();
	do
	{
//        if ( icc_checkCardInsert() ) {
        if ( icc_CheckInSlot(USERCARD) ) {
            s_DelayMs(50);
            if ( ifkeyquit(0) ) {
                Dprintk("\r\n key to exit");
                break;
            }
            continue;
        }
//		Dprintk("\r\n---TIMES:%d", times++);
		for(i = 0; i<timer; i++)
		{
			if(KEY_OR_SERIAL == 0)
			{
				if(IfInkey(0))
				{
                    Dprintk("\r\n 99-exit");
					if(InkeyCount(0) == 99){
                        testover = 1;
                        break;
                    }
				}
			}
			else
			{
				if(kb_hit() == YES)
	            {
	               if(kb_getkey(0) == KEY_CANCEL)
	               {
                        testover = 1;
//                        kb_flush();
                        break;
	               }
//                    testover = 1;
//                    kb_flush();
//                    break;
	            }
			}
			delay_1ms(100);	//delay 
		}
        if ( testover == 1 ) {
            RetCode = -1;
            break;
        }

        INIT_DEBUGBUF();

//		nVoltage=3;			//5V power up
		RetCode=EMV_PowerOn(ats, &wLen);
//        ICCard_ATRAnalyse(wLen, ats);
//        DISP_DEBUGBUF();
        INIT_DEBUGBUF();
		if (RetCode != 0)
		{
			EMV_PowerOff();
			Dprintk("\r\npoweron fail ret=%d",RetCode);
			DispICCardRet(RetCode);
			continue;
		}
//		Dprintk("\r\npoweron suc ret=%02x,wlen=%d",RetCode,wLen);

        //mode 0-表示按照loop back流程 1-exchange失败直接下电
        mode = 1;
        while ( 1 ) {
            INIT_DEBUGBUF();
            RetCode = NewTestVisa(1);
            if ( RetCode == -1 ) {
                //按键退出
                DISP_DEBUGBUF();
                INIT_DEBUGBUF();
                Dprintk("\r\npress the key to exit ret=%d",RetCode);
                EMV_PowerOff();
                return (RetCode) ;
            }
            if ( (RetCode == OK) )
            {
                EMV_PowerOff();
                Dprintk("\r\n[70]exchange ok end %d",RetCode);
//                DispICCardRet(RetCode);
                break;
            }
            if ( (RetCode!=OK) )
            {
                if ( mode == 1 ) {
//                    EMV_PowerOff();
//mode为1时 交互失败下电
//icc_ExchangeData 失败会自动下电
//                    vDispICCKey(guiDebugi, gcDebugBuf);
                    DISP_DEBUGBUF();
                    INIT_DEBUGBUF();
                    Dprintk("\r\nexchange fail ret=%d",RetCode);
                    DispICCardRet(RetCode);
                    break;
                }
//                DISP_DEBUGBUF();
//                INIT_DEBUGBUF();
            }
        }

	}while (1);
	EMV_PowerOff();
	return (RetCode) ;
}

void icc_emv_atr(void)
{
    uint i,len;
	uchar buf[256];
	uchar ucBuff[256];
//	uint AtrLen = 0;
	int ret;

    while(1)
    {
        lcd_cls();
        lcd_display(0,0*12,DISP_CLRLINE | FONT_SIZE12 | DISP_INVLINE | DISP_MEDIACY,"功能测试");
        lcd_display(0,1*12,DISP_CLRLINE | FONT_SIZE12,"1.冷复位");
        lcd_display(0,2*12,DISP_CLRLINE | FONT_SIZE12,"2.热复位");
        lcd_display(0,3*12,DISP_CLRLINE | FONT_SIZE12,"3.取随即数");
        lcd_display(0,4*12,DISP_CLRLINE | FONT_SIZE12,"4.下电");
        switch(kb_getkey(-1))
        {
        case '1':
            guiDebugi=0;
            COLD_WARM_ATR = 0;
            ret = icc_Reset(USERCARD, &len, buf);
            if(!ret)
            {
                displayArry(buf, len, (LCD_WIDE / 5) * 1 + 0);
            }
            else
            {
                lcd_display(0,4*8, FONT_SIZE16|DISP_CLRLINE|DISP_MEDIACY, "Init Ret:%d",ret);
            }
            kb_getkey(-1);
            break;
        case '2':
            COLD_WARM_ATR = 1;
            ret = icc_Reset(USERCARD, &len, buf);
            if(!ret)
            {
                displayArry(buf, len, (LCD_WIDE / 5) * 1 + 0);
            }
            else
            {
                lcd_display(0,4*8, FONT_SIZE16|DISP_CLRLINE|DISP_MEDIACY, "Init Ret:%d",ret);
            }
            kb_getkey(-1);
            break;
        case '3':
            lcd_cls();
            if(!icc_ExchangeData(USERCARD,5,(uchar *)"\x00\xa4\x04\x00\x07"/*"\x00\x84\x00\x00\x04"*/,(int *)&i,ucBuff))
            {
                lcd_display(0,1*12,DISP_CLRLINE | FONT_SIZE12,"读卡成功!");
                lcd_display(0,2*12,DISP_CLRLINE | FONT_SIZE12,"%02X %02X %02X %02X",ucBuff[0],ucBuff[1],ucBuff[2],ucBuff[3]);
            }
            else
            {
                lcd_display(0,2*12,DISP_CLRLINE | FONT_SIZE12,"读卡失败!");
            }
            kb_getkey(-1);
            break;
        case '4':
            lcd_cls();
            ret = icc_Close(USERCARD);
            if(ret)
            {
                lcd_display(0,4*8, FONT_SIZE16|DISP_CLRLINE|DISP_MEDIACY,"CLOSE Ret:%d",ret);
            }
            else
            {
                lcd_display(0,4*8, FONT_SIZE16|DISP_CLRLINE|DISP_MEDIACY,"下电成功");
            }
            kb_getkey(-1);
            break;
        case KEY_CANCEL:
            return;
        default:
            break;
        }
        Dprintk("\r\n调试数据:");
        vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
        guiDebugi=0;
    }
}

void icc_emv_electric(void)
{
	int key;
    int ret = 0;
    ret = ret;

    while(1)
    {
        lcd_cls();
        lcd_display(0,0*12,DISP_CLRLINE | FONT_SIZE12 | DISP_INVLINE | DISP_MEDIACY,"Electric Test");
        lcd_display(0,1*12,DISP_CLRLINE | FONT_SIZE12,"1.Single-test");
        lcd_display(0,2*12,DISP_CLRLINE | FONT_SIZE12,"2.Cycle-test");
        lcd_display(0,3*12,DISP_CLRLINE | FONT_SIZE12,"3.exit");
        key = kb_getkey(-1);
        if(key == '1')
        {
            ret = TestEle1(0);
            lcd_display(0,4*12, FONT_SIZE12|DISP_CLRLINE, "Cancel to exit");
            kb_getkey(-1);
        }
        else if(key == '2')
        {
            ret = TestEle2();
        }
        else
        {
            break;
        }
    }
}

void icc_emv_serial(void)
{
    int i,ret,flg;
    uint len;
	uchar buf[128];
	uchar ucBuff[256];

    KEY_OR_SERIAL = 0;
    while(1)
    {
        Dprintk("\r\n-------------------------串口测试--------------");
        Dprintk("\r\n 1-单步电气测试 2-循环电气测试 3-协议 4-单步测试");
        switch(InkeyCount(0))
        {
        case 1:
            TestEle1(0);
            break;
        case 2:
            TestEle2();
            break;
        case 3:
//            TestPTL();
            Dprintk("\r\n----------enter the time------------");
            i = InkeyCount(0);
            if ( i == 0 ) {
                i = 5;
            }
            if ( i>100 ) {
                i = 100; //max 100s
            }
            i *= 10;
            guiDebugFlg = 1;
            ret = TestPTL_protocolB(i);
            guiDebugFlg = 0;
            vDispICCKey(guiDebugi, gcDebugBuf);
            DISP_DEBUGBUF();
            INIT_DEBUGBUF();
            Dprintk("\r\n------test over %d-------",ret);
            break;
        case 4:
            flg = 0;
            while(1)
            {
                if ( flg ) {
                    break;
                }
                Dprintk("\r\n-------------------------串口测试--------------");
                Dprintk("\r\n 1-冷复位 2-热复位 3-取随即数 4-下电");
                switch(InkeyCount(0))
                {
                case 1:
                    COLD_WARM_ATR = 0;
                    ret = icc_Reset(USERCARD, &len, buf);
                    if(!ret)
                    {
                        Dprintk("\r\nATR:");
                        for(i=0; i<len; i++)
                        {
                            Dprintk("0x%02X ", buf[i]);
                        }
                    }
                    DispICCardRet(ret);
                    break;
                case 2:
                    COLD_WARM_ATR = 1;
                    ret = icc_Reset(USERCARD, &len, buf);
                    if(!ret)
                    {
                        Dprintk("\r\nATR:");
                        for(i=0; i<len; i++)
                        {
                            Dprintk("0x%02X ", buf[i]);
                        }
                    }

                    DispICCardRet(ret);
                    break;
                case 3:
                    ret = icc_ExchangeData(USERCARD,5,(uchar *)"\x00\x84\x00\x00\x04",(int *)&i,ucBuff);
                    if(!ret)
                    {
                        Dprintk("\r\nATR:");
                        Dprintk("0x%02X 0x%02X 0x%02X 0x%02X",ucBuff[0],ucBuff[1],ucBuff[2],ucBuff[3]);
                    }
                    DispICCardRet(ret);
                    break;
                case 4:
                    ret = icc_Close(USERCARD);
                    DispICCardRet(ret);
                    break;
                case 99:
                    flg = 1;
                    break;
                default:
                    break;
                }
            }
            break;
        case 99:
            return;
        default:
            break;

        }
    }
}

void icc_emv_test(void)
{
	int ret = 0,key;
//    int flg,fileno;
	uint i;
	uchar buf[128];
//	guiRamLen = (sys_get_counter()%sizeof(gucRamBuf))/3;
	icc_InitModule(USERCARD, EMV_VOLTAGE, EMVMODE);
	KEY_OR_SERIAL = 0;
	COLD_WARM_ATR = 0;

//	uart_close(UART_COM1);
//	uart_open(UART_COM1, "115200,8,n,1");
    guiDebugFlg = 0;
    INIT_DEBUGBUF();
    ret = ret;
	while(1)
	{
		lcd_cls();
        lcd_display(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY| DISP_INVLINE, "EMV TEST");
        lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE, "1.Electric Test");
        lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE, "2.Protocol Test");
//		lcd_display(0,0*12,DISP_CLRLINE | FONT_SIZE12 | DISP_MEDIACY | DISP_INVLINE ,"EMV TEST");
//		lcd_display(0,1*12,DISP_CLRLINE | FONT_SIZE12,"1.Electric Test");
//		lcd_display(0,2*12,DISP_CLRLINE | FONT_SIZE12,"2.Protocol Test");
#ifdef _EMV_SERIAL_DEBUG_
		lcd_display(0,2*12,DISP_CLRLINE | FONT_SIZE12,"3.Reset Test");
		lcd_display(0,3*12,DISP_CLRLINE | FONT_SIZE12,"4.Serial Test");
#endif
#if 0
        if ( flg == 1 ) {
            flg = 1;
            key = 1;
        }else
            key = InkeyCount(0);
#else
        key = kb_getkey(-1);
#endif
		switch(key)
		{
			case '1':
			{
				KEY_OR_SERIAL = 1;
//                icc_emv_electric();
//lcd_set_auto_refresh(0);
                ret = TestEle2();
//lcd_set_auto_refresh(1);
				KEY_OR_SERIAL = 0;
				break;
			}
			case '2':
			{
                lcd_cls();
//                lcd_display(0,0*12,DISP_CLRLINE | FONT_SIZE12 | DISP_MEDIACY | DISP_INVLINE ,"EMV TEST");
                lcd_display(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE, "enter the loop time");
//                lcd_display(0,1*8,DISP_CLRLINE | FONT_SIZE12,"enter the loop time");
//                lcd_display(0,2*8,DISP_CLRLINE | FONT_SIZE12," ");

                memset(buf,0,sizeof(buf));
                lcd_goto(0,48);
				i = kb_getstr(KB_SMALL_NUM, 1, 2, -1, (char *)buf);
                i = atoi((char *)buf);
                Dprintk("\r\n time==:%d",i);
                if ( i == 0 ) {
                    break;
                }
                if ( i<3 ) {
                    i = 3; //min 5s
                }
                if ( i>100 ) {
                    i = 100; //max 100s
                }
				KEY_OR_SERIAL = 0;
				#if 0
				TestVisa();
				#else
//				ret = TestPTL();
                i *= 10;
                guiDebugFlg = 0;
//lcd_set_auto_refresh(0);
                ret = TestPTL_protocolB(i);
//lcd_set_auto_refresh(1);
                guiDebugFlg = 0;
				#endif
				KEY_OR_SERIAL = 0;
				break;
			}
			//
#ifdef _EMV_SERIAL_DEBUG_
			case '3':
                icc_emv_atr();
                break;
			case '4':
                icc_emv_serial();
			break;
#endif
			case KEY_CANCEL:
            return;

			default:
				break;
		}
	}
}
#endif

void icc_EMVTest(void)
{
	int key = 0;
	while(1)
	{
		lcd_cls();
		lcd_display(0,DISP_FONT_LINE0,DISP_INVLINE|DISP_MEDIACY|DISP_FONT,"EMV L1 TEST");
		lcd_display(0,DISP_FONT_LINE1,DISP_FONT|DISP_CLRLINE,"1-Electric Test");
		lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE,"2-Protocol Test");
		lcd_display(0,DISP_FONT_LINE3,DISP_FONT|DISP_CLRLINE,"3-Debug Test");
		//lcd_display(0,40,DISP_FONT|DISP_CLRLINE,"4-Set Cycle Time");
		kb_flush();

		key = kb_getkey(-1);
		switch(key)
		{
			case KEY1:
				electric_test();
				break;
			case KEY2:
				protocol_test();
				break;
			case KEY3:
				debug_test();
				break;
			/*
			case KEY4:
				lcd_cls();
				lcd_display(0,0,DISP_INVLINE|DISP_MEDIACY|DISP_FONT,"Set Cycle Time");
				lcd_display(0,16,DISP_FONT|DISP_CLRLINE,"1-Electric Cycle Time");
				lcd_display(0,24,DISP_FONT|DISP_CLRLINE,"2-Electric Cycle Flag");
				lcd_display(0,32,DISP_FONT|DISP_CLRLINE,"3-Protocol Time");
				kb_flush();
				key = kb_getkey(-1);
				lcd_cls();
				switch(key)
				{
					case KEY1:
						lcd_display(0,16,DISP_FONT|DISP_CLRLINE,"Current Cycle Time:");
						lcd_display(0,24,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"%d ms",guiDebugElectricTime);
						key = InkeyCount(1);
						if(key>=500)
							guiDebugElectricTime = key;
						break;
					case KEY2:
						lcd_display(0,16,DISP_FONT|DISP_CLRLINE,"guiDebugk:");
						lcd_display(0,24,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"%d",guiDebugk);
						key = InkeyCount(1);
						guiDebugk = key;
						break;
					case KEY3:
						lcd_display(0,16,DISP_FONT|DISP_CLRLINE,"Current Cycle Time:");
						lcd_display(0,24,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"%d ms",guiDebugProtoclTime);
						key = InkeyCount(1);
						if(key>=500)
							guiDebugProtoclTime = key;
						break;
				}
				break;
			*/
			default:
				break;
		}
	}
}

#define  Debug_SIGNAL0_Port      LED_GREEN_GPIO  //lcd
#define  Debug_SIGNAL1_Port      LED_BLUE_GPIO 
#define  Debug_SIGNAL2_Port      LED_YELLOW_GPIO 

#define  Debug_SIGNAL0_Pin       LED_GREEN_PINx  //lcd
#define  Debug_SIGNAL1_Pin       LED_BLUE_PINx 
#define  Debug_SIGNAL2_Pin       LED_YELLOW_PINx 

#define SETSIGNAL_H()     hw_gpio_set_bits(Debug_SIGNAL0_Port,(1<<Debug_SIGNAL0_Pin))
#define SETSIGNAL_L()     hw_gpio_reset_bits(Debug_SIGNAL0_Port,(1<<Debug_SIGNAL0_Pin))
//#define SIGNAL_ROLL()     if(guiDebugm++%2){SETSIGNAL_L();}else{SETSIGNAL_H();}

//#define SETSIGNAL1_H()    hw_gpio_set_bits(Debug_SIGNAL1_Port,(1<<Debug_SIGNAL1_Pin))  
//#define SETSIGNAL1_L()    hw_gpio_reset_bits(Debug_SIGNAL1_Port,(1<<Debug_SIGNAL1_Pin))
////#define SIGNAL1_ROLL()    if(guiDebugm++%2){SETSIGNAL1_L();}else{SETSIGNAL1_H();}
//
//#define SETSIGNAL2_H()    hw_gpio_set_bits(Debug_SIGNAL2_Port,(1<<Debug_SIGNAL2_Pin))  
//#define SETSIGNAL2_L()    hw_gpio_reset_bits(Debug_SIGNAL2_Port,(1<<Debug_SIGNAL2_Pin))
////#define SIGNAL2_ROLL()    if(guiDebugm++%2){SETSIGNAL2_L();}else{SETSIGNAL2_H();}

int test_ioinit(void)
{
//    GPIO_InitTypeDef gpio_init;

//    gpio_init.GPIOx = Debug_SIGNAL0_Port;
//    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL;
//    gpio_init.PORT_Pin = Debug_SIGNAL0_Pin;
//    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(Debug_SIGNAL0_Pin);
//    hw_gpio_init(&gpio_init);
//    hw_gpio_reset_bits(Debug_SIGNAL0_Port,(1<<Debug_SIGNAL0_Pin));
    return 0;
}

extern volatile UINT64 gl_char_wait_etu;
#define EMV_COMM   UART4 //UART3
int test_cpucard(int mode)
{
    int i,flg = 0;
    int stat;
    volatile int j;

    test_ioinit();
	SETSIGNAL_L();
	SETSIGNAL1_L();
//	SETSIGNAL2_L();
//    s_DelayMs(100);

//    lcd_set_auto_refresh(0);
    lcd_SetLight(LIGHT_ON_MODE);
    lcd_cls();
    kb_flush();
    lcd_display(0,FONT_SIZE12*1,FONT_SIZE12|DISP_CLRLINE,"k1-test k2-debug");

    drv_uart_close(WORK_COMPORT);
#if 0

    console_close();
    console_init(EMV_COMM,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
    flg = 1;

#else
    if ( kb_getkey(-1) == KEY1 ) {
        flg = 1;
        console_close();
        s_DelayMs(500);
        console_init(EMV_COMM,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
    }else{
        flg = 0;
        lcd_cls();
        lcd_display(0,FONT_SIZE12*2,FONT_SIZE12|DISP_CLRLINE,"k1-usb k2-com ");

#if 0
            console_close();
            console_init(USBD_CDC,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
#else
        if ( kb_getkey(-1) == KEY1 ) {
            console_close();
            s_DelayMs(500);
            console_init(USBD_CDC,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
            kb_getkey(3000);
        }else{
            console_close();
            s_DelayMs(500);
            console_init(EMV_COMM,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
        }
#endif
    }
#endif

    guiDebugFlg = 0;
    INIT_DEBUGBUF();
    icc_InitIccModule();
	icc_InitModule(USERCARD, EMV_VOLTAGE,ISOMODE);
	icc_InitModule(SAM1SLOT, EMV_VOLTAGE,ISOMODE);
    while(1) {
        lcd_cls();
        lcd_display(0,FONT_SIZE12*4,FONT_SIZE12|DISP_CLRLINE,"%s %s ",__DATE__,__TIME__);
        TRACE("\r\n\r\n---ic type :");
        TRACE("\r\n\r\n1-icc  2-8035  3-emv test  4-emv serial  5-hard");
        TRACE("\r\n99-return");
        if ( flg == 1 ) {
            flg = 2;
            i = 3;
        }else
            i = InkeyCount(0);

        switch(i) {
        case 1:
            vTest_Icc();
            break;
        case 2:
            Test_TDA8035();
            break;
        case 3:
            icc_emv_test();
//			if ( InkeyCount(0) == 0 ) {
//                icc_open_clk();
//                sys_beep();
//            }else
//                icc_close_clk();
            break;
        case 4:
            icc_emv_serial();
            break;
        case 5:
            if ( InkeyCount(0) == 1 ) {
//                icc_control_clk(ON);
                icc_open_clk();        // 打开时钟
                s_DelayMs(20);
            }
            if ( InkeyCount(0) == 1 ) {
                icc_SetVCC(5);
                s_DelayMs(20);
            }
            ICCardHardTest();
            break;

        case 10:
            icc_init_etu_timer();
            guiDebugFlg = 1;
            INIT_DEBUGBUF();
            gl_char_wait_etu = 10;
            i = InkeyCount(0);
            TRACE("\r\n ---clock delay");
            icc_start_etu_timer(i);      // 等待TS字节时间
            s_DelayMs(100);
            icc_close_etu_timer();
            DISP_DEBUGBUF();
            break;
        case 11:
            TRACE("\r\n ---clock delay");
            icc_init_inter_timer();
            i = InkeyCount(0);
            SETSIGNAL1_H(); 
            icc_delay_clock(i);
            SETSIGNAL1_L(); 
            break;
        case 12:
            TRACE("\r\n ---etu delay");
            i = InkeyCount(0);
            SETSIGNAL1_H(); 
            icc_delay_etu(372, i);
            SETSIGNAL1_L(); 
            break;
        case 13:
            i = 0;
            TRACE("\r\n ---pc2");
            InkeyCount(0);
            while ( i<5 ) {
                SETSIGNAL_H(); 
                s_DelayUs(1000);
                SETSIGNAL_L(); 
                s_DelayUs(1000);
                i++;
            }
            i = 0;
            TRACE("\r\n ---pc3");
            InkeyCount(0);
            while ( i<5 ) {
                SETSIGNAL1_H(); 
                s_DelayUs(1000);
                SETSIGNAL1_L(); 
                s_DelayUs(1000);
                i++;
            }
            InkeyCount(0);
            break;
        case 14:
            //软延时时间
            j = 0;
            icc_DisableOtherInt();
            SETSIGNAL_H();
            while ( 1 ) {
                if ( j++>1000000 ) {
                    break;
                }
            }
            SETSIGNAL_L();
            icc_EnableOtherInt();
            break;
        case 15:
            //测试定时器标志不能置位的问题
//            i = 15*372;
//            j = 0;
//            stat = 5;
//            while ( 1 ) {
//                j++;
//                icc_DisableOtherInt();
//                icc_init_etu_timer();
//                icc_init_inter_timer();
//                SETSIGNAL_H();
//                icc_delay_clock(i);
//                icc_delay_etu(372, stat);
//                if ( stat++>266 ) {
//                    stat = 5;
//                }
//                SETSIGNAL_L();
////                icc_StartCharTimer();
//                Enable_TC_Clk(TIMER2_ID);
//                TC_CCR_WRITE(TIMER2_ID,TC_CLKDIS);
//                TC_CMR_WRITE(TIMER2_ID,TC_TCCLKS_TIMER_CLOCK3|TC_CLKI|TC_BURST_INTER);
//                TC_SR_READ(TIMER2_ID);
//                TC_CCR_WRITE(TIMER2_ID,TC_CLKEN|TC_SWTRG);
//                icc_EnableOtherInt();
////                s_DelayMs(30);
//                if ( IfInkey(0) ) {
//                    if ( InkeyCount(0) == 1 ) {
//                        break;
//                    }
//                }
////                Dprintk("\r\n--:%d",j);
////                s_DelayMs(30);
//            }
            break;
        case 16:
            InkeyCount(0);
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                SETSIGNAL_H(); 
                s_DelayUs(1000);
                SETSIGNAL_L(); 
                s_DelayUs(1000);
            }

            InkeyCount(0);
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                SETSIGNAL1_H(); 
                s_DelayUs(1000);
                SETSIGNAL1_L(); 
                s_DelayUs(1000);
            }

            InkeyCount(0);
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                SETSIGNAL2_H(); 
                s_DelayUs(1000);
                SETSIGNAL2_L(); 
                s_DelayUs(1000);
            }
            break;
        case 98:
            TRACE("\r\n\r\n---set the com 1-com  ff-usb");
            i = InkeyHex(0);
            if ( i == UART_COM1 ) {
                console_close();
                console_init(EMV_COMM,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
            }
            else if ( i == 0xFF ) {
                console_close();
                console_init(USBD_CDC,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
            }
            break;
        case 99:
            lcd_display(0,FONT_SIZE12*4,FONT_SIZE12|DISP_CLRLINE," no-----");
            if ( kb_getkey(-1) == KEY1 ) {
                return 0;
            }
            break;
        default:
            break;
        }
    }

    return 0;
}
#endif
#endif

