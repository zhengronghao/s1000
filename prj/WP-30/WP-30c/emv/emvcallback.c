//#include "../inc/libapi.h"
#include "lib_emv.h"
#include "emv.h"

//typedef struct
//{
//	uchar frametype;
//	uchar button;
//	uchar titlealign;
//	uchar contextalign;
//	ushort rfu;
//	ushort titlelen;
//	uchar* title;
//	ushort contextlen;
//	uchar* context;
//}s_lcd_emv;

// uchar gszOnlinePin[17];
// uchar gucOnlinePinLen = 0;
// uchar glOnlineTrans = 0;
// uchar glSendReverral = 0;
extern unsigned char ST_EMV_GetICPara(unsigned char *pAid, int nAidLen, EMVPARA *pParaStru);
extern int lcd_EmvDisp(s_lcd_emv * lcd_emvdis, int timeout);
extern int ped_get_pin_online(uint32_t inlen,uint8_t *packet,uint16_t *outlen,uint8_t *output);
extern int ped_get_encrpt_data(uint32_t inlen,uint8_t *packet,uint16_t *outlen,uint8_t *output);
extern int ped_get_pin_offline(uint32_t inlen,uint8_t *input,uint16_t *outlen,uint8_t *output);
extern void Debug_ComPrintStr(int nSendLen, char *pvSendData);
extern void Debug_ComPrintInt(int nSendLen, char *pvTitle, int Num);
extern void Debug_ComPrintTitle(int nTitleLen, char *pvTitle, int nSendLen, char *pvSendData);

uint asc2ulong(uchar *asc)
{
	int i, blen;
	uint dwTmp;

	blen = strlen((char *)asc);
	dwTmp = 0;

	for(i=0; i<blen; i++)
	{
		if((asc[i] < '0') || (asc[i] > '9'))
			return 0;//return -1;
		dwTmp = dwTmp*10 + (asc[i] - 0x30);
	}
	return dwTmp;
}

uchar AscConvertHex(uchar *ucBuffer)
{
	uchar Temp[2];
	uchar ucTemp = 0x00;

	memset(Temp, 0x00, sizeof(Temp));

	if((ucBuffer[0] >= '0') && (ucBuffer[0] <= '9'))
	{
		Temp[0] = ucBuffer[0] - 0x30;
	}
	else if((ucBuffer[0] >= 'a') && (ucBuffer[0] <= 'z'))
	{
		Temp[0] = ucBuffer[0] - 0x61 + 0x0a;
	}
	else if((ucBuffer[0] >= 'A') && (ucBuffer[0] <= 'Z'))
	{		
		Temp[0] = ucBuffer[0] - 0x41 + 0x0a;
	}

	if((ucBuffer[1] >= '0') && (ucBuffer[1] <= '9'))
	{		
		Temp[1] = ucBuffer[1] - 0x30;
	}
	else if((ucBuffer[1] >= 'a') && (ucBuffer[1] <= 'z'))
	{
		Temp[1] = ucBuffer[1] - 0x61 + 0x0a;
	}
	else if((ucBuffer[1] >= 'A') && (ucBuffer[1] <= 'Z'))
	{
		Temp[1] = ucBuffer[1] - 0x41 + 0x0a;
	}
	ucTemp = (Temp[0] << 4) + Temp[1];
	return (ucTemp);
}

/*
 * 函数功能：将ASCII字符串转换成十六进制数据，转换后的十六进制数据长度为ASCII
 * 字符串的一半，ASCII长度为奇数的，转换后，右补零。
 * 入口参数：upcAscBuf  ―― 要转换的ASCII字符串
 *           nConvLen   ―― 转换的字符串长度
 *           ucType     ―― 转换后的类型
 * 出口参数：upcHexBuf  ―― 转换后的十六进制数据
 * 返 回 值：NORMAL     ―― 转换成功
 *           EXCEPTION  ―― 失败，pcHexBuf或者pcAscBuf为NULL
 *           ERRSIZE    ―― 不合法的转换范围
 *           ERRCHAR    ―― 存在非法字符
 */
int  AsciiToHex(unsigned char *upcHexBuf, unsigned char *upcAscBuf, int nConvLen, unsigned char ucType)
{
    int nCnt;
    char cChar, cChar1;

    if (upcHexBuf == NULL || upcAscBuf == NULL)
    {
        ;
        return -1;
    }

    if (nConvLen < 0)
    {
        ;
        return -1;
    }
    else if (nConvLen == 0)
    {
        upcHexBuf[0] = 0;
        return 0;
    }

    if (nConvLen & 0x01 && ucType)
        cChar1 = 0;
    else
        cChar1 = 0x55;

    for (nCnt = 0; nCnt < nConvLen; upcAscBuf++, nCnt++)
    {
        if (*upcAscBuf >= 'a')
            cChar = *upcAscBuf - 'a' + 10;
        else if (*upcAscBuf >= 'A')
            cChar = *upcAscBuf - 'A' + 10;
        else if (*upcAscBuf >= '0')
            cChar = *upcAscBuf - '0';
        else
            cChar = 0;

        if (cChar1 == 0x55)
            cChar1 = cChar;
        else
        {
            *upcHexBuf++ = (cChar1 << 4) | cChar;
            cChar1 = 0x55;
        }
    }

    if (cChar1 != 0x55)
        *upcHexBuf = cChar1 << 4;

    return 0;
}

/*
* 函数功能：将十六进制数据转换成ASCII字符串。
* 入口参数：upcHexBuf ―― 要转换的十六进制数据
*           nConvLen ―― 转换的字符串长度
*           ucType ―― 要转换的字串类型
* 出口参数：upcAscBuf ―― 转换后的ASCII字符串
* 返 回 值：NORMAL ―― 转换成功
*           EXCEPTION ―― 失败，pcHexBuf或者pcAscBuf为空
*           ERRSIZE － 错误的转换长度
*/
int  HexToAscii(unsigned char *upcAscBuf, unsigned char *upcHexBuf,	int nConvLen, unsigned char ucType)
{
	int nCnt;

	if (upcAscBuf == NULL || upcHexBuf == NULL)
	{
		;
		return -1;
	}

	if (nConvLen < 0)
	{
		;
		return -1;
	}
	else if (nConvLen == 0)
	{
		upcAscBuf[0] = 0;
		return 0;
	}

	if (nConvLen & 0x01 && ucType)
	{
		nCnt = 1;
		nConvLen ++;
	}
	else
		nCnt = 0;
	for (; nCnt < nConvLen; nCnt ++, upcAscBuf ++)
	{
		*upcAscBuf = ((nCnt & 0x01) ? (*upcHexBuf++ & 0x0f) : (*upcHexBuf >> 4));
		*upcAscBuf += ((*upcAscBuf > 9) ? ('A' - 10) : '0');
	}

	return 0;
}

int emv_select_candidate_application(void)
{
	int Ret = 0;
	uint CandListNum = 0;
	int j = 0, Cursor = 0, nOffset = 0;
	int nPSEType;
	EMV_CANDAPP_LIST CandApp[32];
	uchar ucFlag;
	s_lcd_emv tDispInfo;
    uchar szBuff[256];
    uchar szBuff2[30];

    memset(szBuff,0,sizeof(szBuff));
    memset(szBuff,0,sizeof(szBuff2));
#if 0
	nPSEType = emv_get_pse_type();        // 获取PSE类型
	emv_Get_All_CandAppInfo(&CandListNum, CandApp);    // 获取所有的候选应用信息
	if(CandListNum == 0)
	{
		return EMV_RET_ERROR_NO_APP;
	}

	ucFlag = ST_EMV_GetFirstSelApp();
	if((!ucFlag) && (nPSEType == 0)) //QPBOC不需要持卡人确认选择应用
	{
		lcd_Cls();
		lcd_Display(0, 3*8, FONT_SIZE16|DISP_MEDIACY, "重试?");
		Ret = kb_GetKey(-1);
		if(Ret != KEY_ENTER)
		{
			return EMV_RET_ERROR_USER_CANCEL;  
		}
	}

	if(CandListNum == 1)
	{
		if (nPSEType == 1)
		{
			//QPBOC不需要持卡人确认选择应用
			ST_EMV_SetFirstSelApp(0);
			return 0;
		}
		if(ucFlag)
		{
			if(CandApp[0].cFlgAPID)
			{
				if((CandApp[0].cAPID & 0x80) == 0x80)  // 需要用户确认的
				{
					lcd_Cls();
					lcd_Display(0, 2*8, DISP_MEDIACY|DISP_CLRLINE|FONT_SIZE16, "%s", CandApp[0].tCandAppName);
					lcd_Display(0, 4*8, DISP_MEDIACY|DISP_CLRLINE|FONT_SIZE16, "确认-是");
					lcd_Display(0, 6*8, DISP_MEDIACY|DISP_CLRLINE|FONT_SIZE16, "取消-否");

					Ret = kb_GetKey(-1);
					if(Ret != KEY_ENTER)
					{
						return EMV_RET_ERROR_USER_CANCEL;  // operator reject to execute the current application
					}
				}
			}
			ST_EMV_SetFirstSelApp(0);
		}
		else
		{
			lcd_Cls();
			lcd_Display(0, 2*8, DISP_MEDIACY|DISP_CLRLINE|FONT_SIZE16, "%s", CandApp[0].tCandAppName);
			lcd_Display(0, 4*8, DISP_MEDIACY|DISP_CLRLINE|FONT_SIZE16, "确认-是");
			lcd_Display(0, 6*8, DISP_MEDIACY|DISP_CLRLINE|FONT_SIZE16, "取消-否");
			Ret = kb_GetKey(-1);
			if(Ret != KEY_ENTER)
			{
				return EMV_RET_ERROR_USER_CANCEL;
			}
		}
		return 0;
	}

	if(nPSEType == 1)  //QPBOC不需要持卡人确认选择应用, 应直接选择优先级别最高的应用 2011-9-28
	{
		for(j=0; j<CandListNum; j++)
		{
			if(CandApp[j].cFlgAPID && (CandApp[j].cAPID & 0x80))
			{
				continue;
			}
			else
			{
				ST_EMV_SetFirstSelApp(0);
				return j;
			}
		}
	}

	lcd_Cls();
	kb_Flush();
	if(CandListNum > 1)
	{
		lcd_SetIcon(ICON_DOWN,ON);
	}
	else
	{
		lcd_SetIcon(ICON_DOWN,OFF);
	}
	lcd_SetIcon(ICON_UP,OFF);
	j = 0;
	while(1)
	{
		lcd_Cls();
		lcd_Display(0, 0, DISP_INVLINE|FONT_SIZE16|DISP_MEDIACY, "应用选择");
		if(j < CandListNum)
		{
			if((Cursor % 3) == 0)
			{
				lcd_Display(0, 2*8, DISP_INVLINE|DISP_CLRLINE|FONT_SIZE16, "%s", CandApp[j].tCandAppName);
			}
			else
			{
				lcd_Display(0, 2*8, DISP_CLRLINE|FONT_SIZE16, "%s", CandApp[j].tCandAppName);
			}
		}

		if((j+1) < CandListNum)
		{
			if((Cursor % 3) == 1)
			{
				lcd_Display(0, 4*8, DISP_INVLINE|DISP_CLRLINE|FONT_SIZE16, "%s", CandApp[j+1].tCandAppName);
			}
			else
			{
				lcd_Display(0, 4*8, DISP_CLRLINE|FONT_SIZE16, "%s", CandApp[j+1].tCandAppName);
			}
		}

		if((j+2) < CandListNum)
		{
			if((Cursor % 3) == 2)
			{
				lcd_Display(0, 6*8, DISP_INVLINE|DISP_CLRLINE|FONT_SIZE16, "%s", CandApp[j+2].tCandAppName);
			}
			else
			{
				lcd_Display(0, 6*8, DISP_CLRLINE|FONT_SIZE16, "%s", CandApp[j+2].tCandAppName);
			}
		}

		Ret = kb_GetKey(-1);

		switch(Ret)
		{
		case KEY_DOWN:
			if(Cursor < (CandListNum - 1))
			{
				Cursor++;
				lcd_SetIcon(ICON_UP,ON);
				if(Cursor == (CandListNum - 1))
				{
					lcd_SetIcon(ICON_DOWN,OFF);  
				}
			}

			j = (Cursor/3)*3;
			break;
		case KEY_UP:
			if(Cursor != 0)
			{
				Cursor--;
				lcd_SetIcon(ICON_DOWN,ON);
			}

			if(Cursor == 0)
			{
				lcd_SetIcon(ICON_UP,OFF);
			}

			j = (Cursor/3)*3;
			break;
		case KEY_ENTER:
			ST_EMV_SetFirstSelApp(0);
			return Cursor;
		case KEY_CANCEL:
			return EMV_RET_ERROR_USER_CANCEL;
		default:
			break;
		}
	}

#else
	nPSEType = emv_get_pse_type();        // 获取PSE类型
	emv_Get_All_CandAppInfo(&CandListNum, CandApp);    // 获取所有的候选应用信息
	if(CandListNum == 0)
	{
		return EMV_RET_ERROR_NO_APP;
	}

	ucFlag = ST_EMV_GetFirstSelApp();
	if((!ucFlag) && (nPSEType == 0)) //QPBOC不需要持卡人确认选择应用
	{
		memset(&tDispInfo, 0x00, sizeof(s_lcd_emv));
		tDispInfo.frametype = 1; //选择框
		tDispInfo.button    = 0x03;  //确认键和取消键有效
		tDispInfo.contextalign = 1;  //居中
	
        memcpy(szBuff, "重试?", 5);
        tDispInfo.context = szBuff;
        tDispInfo.contextlen = 5;
		Ret = lcd_EmvDisp(&tDispInfo, 120000);
		if (Ret != 0x01)
		{
			return EMV_RET_ERROR_USER_CANCEL;  
		}
	}

	if(CandListNum == 1)
	{
		if (nPSEType == 1)
		{
			//QPBOC不需要持卡人确认选择应用
			ST_EMV_SetFirstSelApp(0);
			return 0;
		}
		if(ucFlag)
		{
			if(CandApp[0].cFlgAPID)
			{
				if((CandApp[0].cAPID & 0x80) == 0x80)  // 需要用户确认的
				{
					memset(&tDispInfo, 0x00, sizeof(s_lcd_emv));
					tDispInfo.frametype = 1; //选择框
					tDispInfo.button    = 0x03;  //确认键和取消键有效
					tDispInfo.contextalign = 1;  //居中
					memcpy(szBuff, CandApp[0].tCandAppName, strlen((char *)CandApp[0].tCandAppName));
					tDispInfo.context = szBuff;
                    tDispInfo.contextlen = strlen((char *)CandApp[0].tCandAppName);
					Ret = lcd_EmvDisp(&tDispInfo, 120000);
					if (Ret != 0x01)
					{
						return EMV_RET_ERROR_USER_CANCEL;  // operator reject to execute the current application
					}
				}
			}
			ST_EMV_SetFirstSelApp(0);
		}
		else
		{
			memset(&tDispInfo, 0x00, sizeof(s_lcd_emv));
			tDispInfo.frametype = 1; //选择框
			tDispInfo.button    = 0x03;  //确认键和取消键有效
			tDispInfo.contextalign = 1;  //居中
			memcpy(szBuff, CandApp[0].tCandAppName, strlen((char *)CandApp[0].tCandAppName));
			tDispInfo.context = szBuff;
            tDispInfo.contextlen = strlen((char *)CandApp[0].tCandAppName);
			Ret = lcd_EmvDisp(&tDispInfo, 120000);
			if (Ret != 0x01)
			{
				return EMV_RET_ERROR_USER_CANCEL; 
			}
		}
		return 0;
	}

	if(nPSEType == 1)  //QPBOC不需要持卡人确认选择应用, 应直接选择优先级别最高的应用 2011-9-28
	{
		for(j=0; j<CandListNum; j++)
		{
			if(CandApp[j].cFlgAPID && (CandApp[j].cAPID & 0x80))
			{
				continue;
			}
			else
			{
				ST_EMV_SetFirstSelApp(0);
				return j;
			}
		}
	}

	j = 0;
	while(1)
	{
		memset(&tDispInfo, 0x00, sizeof(s_lcd_emv));
		tDispInfo.frametype = 3; //列表选择框
		tDispInfo.button    = 0x07;  //确认键、取消键和方向键有效
		tDispInfo.titlealign = 1;  //居中
		tDispInfo.contextalign = 0;  //左对齐
		memcpy(szBuff2, "应用选择", 8);
		tDispInfo.title = szBuff2;
        tDispInfo.titlelen = 8;
		nOffset = 0;
		for (j = 0; j < CandListNum; j++)
		{
			memcpy(szBuff+nOffset, CandApp[j].tCandAppName, strlen((char *)CandApp[j].tCandAppName));
            nOffset += strlen((char *)CandApp[j].tCandAppName);
			memcpy(szBuff+nOffset, "\n", 1);
			tDispInfo.context = szBuff;
            nOffset++;
		}
		tDispInfo.contextlen = nOffset;
		Ret = lcd_EmvDisp(&tDispInfo, 120000);
		if (Ret == 0x00)
		{
			//取消键
			return EMV_RET_ERROR_USER_CANCEL;
		}
		else if (Ret == 0x01)
		{
			//确认键
			Cursor = 0; //默认第一个
			ST_EMV_SetFirstSelApp(0);
			return Cursor;
		}
		else if (Ret >= '1' && Ret <= '9')
		{
			Cursor = Ret - '1';
			ST_EMV_SetFirstSelApp(0);
			return Cursor;
		}
	}

#endif
}

int emv_account_type_select(void)
{
	int Ret = 0x00;
	uchar szBuff[128];
	s_lcd_emv tDispInfo;
    uchar szBuff2[30];
#if 0
	lcd_Cls();
	lcd_Display(0,0,FONT_SIZE16|DISP_MEDIACY,"账户类型");
	lcd_Display(0,2*8,FONT_SIZE16,"1-默认  2-储蓄");
	lcd_Display(0,4*8,FONT_SIZE16,"3-支票/借记");
	lcd_Display(0,6*8,FONT_SIZE16,"4-信用");

	Ret = kb_GetKey(-1);
	switch(Ret)
	{
		case KEY_CANCEL:
			return EMV_RET_ERROR_USER_CANCEL;
		case KEY1:
			return 0x00;
		case KEY2:
			return 0x10;
		case KEY3:
			return 0x20;
		case KEY4:
			return 0x30;
		default:
			return 0x00;
	}	
#else
	memset(&tDispInfo, 0x00, sizeof(s_lcd_emv));
	memset(szBuff2, 0x00, sizeof(szBuff2));
	tDispInfo.frametype = 3; //列表选择框
	tDispInfo.button    = 0x07;  //确认键、取消键和方向键有效
	tDispInfo.titlealign = 1;  //居中
	tDispInfo.contextalign = 0;  //左对齐
	
    
    memcpy(szBuff2, "账户类型", 8);
	tDispInfo.title = szBuff2;
    tDispInfo.titlelen = 8;
	sprintf((char *)szBuff, "%s\n%s\n%s\n%s\n", "1-默认", "2-储蓄", "3-支票/借记", "4-信用");
    tDispInfo.context = szBuff;
	tDispInfo.contextlen = strlen((char *)szBuff);
	Ret = lcd_EmvDisp(&tDispInfo, 120000);
	if (Ret == 0x00)
	{
		//取消键
		return EMV_RET_ERROR_USER_CANCEL;
	}
	else if (Ret == '1')
	{
		return 0x00;
	}
	else if (Ret == '2')
	{
		return 0x10;
	}
	else if (Ret == '3')
	{
		return 0x20;
	}
	else if (Ret == '4')
	{
		return 0x30;
	}
	else
	{
		return 0x00;
	}

#endif
}

int emv_get_transaction_amount(void)
{
	uchar ucBCD[10], ucAsc[20], ucRet;
	int  nAmount = 0;
	unsigned short nLen = 0;

	memset(ucBCD, 0x00, sizeof(ucBCD));
	memset(ucAsc, 0x00, sizeof(ucAsc));
	ucRet = ST_EMV_GetAppData((uchar *)"\x9F\x02", 2, ucBCD, &nLen);
	if (ucRet != EMV_RET_OK)
	{
		return 0;
	}
    HexToAscii(ucAsc, ucBCD, 12, 0);
	nAmount = atoi((char *)ucAsc);

	return nAmount;
}

int emv_get_cardholder_pin(PINSTATUS *pEncryptInfo, uchar *CardPin)
{    
	TermMCK tMck;
	int Ret, nInLen = 0;
// 	s_lcd_emv tDispInfo;
	unsigned short usOutLen = 0;
    uchar szBuff[128];

#if 0
	memset(szBuff,0x00, sizeof(szBuff));
    TRACE("\r\nget cardholer_pin 1");
	memset(&tDispInfo, 0x00, sizeof(s_lcd_emv));
	tDispInfo.frametype = 0; //文本框
	tDispInfo.button    = 0x00;  //仅显示
	tDispInfo.contextalign = 0;  //左对齐
	memcpy(szBuff,"请输入密码:", strlen((char *)"请输入密码:"));
	tDispInfo.context = szBuff;
    tDispInfo.contextlen = strlen((char *)szBuff);
    TRACE("\r\nget cardholer_pin 2");

    Ret = lcd_EmvDisp(&tDispInfo, 120000);
#endif

	memset(szBuff,0x00, sizeof(szBuff));
	szBuff[2] = 0x00;  //明文上送脱机PIN
	szBuff[3] = pEncryptInfo->cMinLen;
	szBuff[4] = pEncryptInfo->cMaxLen;
	szBuff[5] = pEncryptInfo->cTimeout;
	szBuff[6] = pEncryptInfo->cShowlenflag;
	szBuff[7] = pEncryptInfo->cBeepflag;
	szBuff[10] = pEncryptInfo->cInfoLen;
	memcpy(szBuff+11, pEncryptInfo->acInfo, pEncryptInfo->cInfoLen);
	nInLen = pEncryptInfo->cInfoLen+9;
	szBuff[0] = nInLen & 0xff;
	szBuff[1] = (nInLen>>8) & 0xff;
	nInLen+=2;

	emv_get_mck(&tMck);
	while(1)
	{
// 		if(tMck.cSupBypassPIN)
// 		{
			Debug_ComPrintStr(9, "AppDebug3");
			//Ret = kb_GetStr(KB_BIG_PWD,0,12,120000,(char *)CardPin);
			Ret = ped_get_pin_offline(nInLen, szBuff, &usOutLen, CardPin);
			Debug_ComPrintInt(9, "AppDebug4", Ret);
			if (Ret == RTV_PED_ERR_INPUT_CANCEL)
			{
				return EMV_RET_ERROR_USER_CANCEL;
			}
			else if (Ret)
			{
				return EMV_RET_ERROR_DATA;
			}
			Ret = usOutLen;
// 		}
// 		else
// 		{
// 			Ret = kb_GetStr(KB_BIG_PWD,4,12,120000,(char *)CardPin);
// 		}
		//密码长度控制在4-12位
		if (Ret>0 && Ret<4)
		{
			memset(CardPin, 0x00,sizeof(CardPin));	
            continue;
		}
		else if (Ret>12)
		{
			memset(CardPin, 0x00,sizeof(CardPin));	
			continue;
		}
		else
		{
            TRACE("\r\nget cardholer_pin 3");
			break;
		}
	}

    TRACE("\r\nget cardholer_pin 4=%d",Ret);
	//Debug_ComPrintInt(9, "AppDebug6", Ret);
	if(Ret == -KB_CANCEL)
		return EMV_RET_ERROR_USER_CANCEL;
	if(Ret == -KB_ERROR|| Ret == -KB_TIMEOUT)
		return EMV_RET_ERROR_DATA;
	if(Ret == 0)
		return EMV_RET_ERROR_BYPASSPIN;
// 	if(Ret>0 && Ret<4)
// 		return EMV_RET_ERROR_DATA;
	if(Ret>=4 && Ret<=12)
		return Ret;
	return EMV_RET_ERROR_DATA;
}

int emv_show_pin_verify_result(uchar TryCounter)
{
	int Ret, nLen = 0;
	s_lcd_emv tDispInfo;
	uchar szBuff[128];
    uchar szBuff2[30];

	if(TryCounter==0)
	{
		memset(&tDispInfo, 0x00, sizeof(s_lcd_emv));
		memset(szBuff2, 0x00, sizeof(szBuff2));
		tDispInfo.frametype = 0; //文本框
		tDispInfo.button    = 0x0F;  //各按键都支持
		tDispInfo.contextalign = 0;  //左对齐
        memcpy(szBuff2, "密码正确", strlen((char *)"密码正确"));
        tDispInfo.context = szBuff2;
        tDispInfo.contextlen = strlen((char *)"密码正确");
		Ret = lcd_EmvDisp(&tDispInfo, 10000);
		return EMV_RET_ERROR_OK;
	}

	memset(szBuff, 0x00, sizeof(szBuff));
	sprintf((char *)szBuff, "%s\n", "密码错误");
	nLen = strlen((char *)szBuff);
	if (TryCounter == 1)
	{
		// 最后一次才显示剩余次数,BCTC修改_ruanmd20130830
		sprintf((char *)szBuff+nLen, "%s%d%s\n", "剩余 ",TryCounter, " 次");
		nLen = strlen((char *)szBuff);
	}
	sprintf((char *)szBuff+nLen, "%s\n", "重试?");
	nLen = strlen((char *)szBuff);

	memset(&tDispInfo, 0x00, sizeof(s_lcd_emv));
	tDispInfo.frametype = 2; //列表框
	tDispInfo.button    = 0x03;  //确认键、取消键有效
	tDispInfo.contextalign = 1;  //居中
	tDispInfo.context = szBuff;
	tDispInfo.contextlen = nLen;
	Ret = lcd_EmvDisp(&tDispInfo, 120000);
	if (Ret == 0x01)
	{
		//确认键
		return EMV_RET_ERROR_OK;                    
	}
	else
	{
		return EMV_RET_ERROR_USER_CANCEL;
	}
}

int emv_process_online_pin(PINSTATUS *pEncryptInfo)
{
#if 0
	int Ret = 0;
	uchar KeyBuff[16] = {0xF0,0xE1,0xD2,0xC3,0xB4,0xA5,0x96,0x87,0x0F,0x1E,0x2D,0x3C,0x4B,0x5A,0x69,0x78};    
 	uchar CardNo[16] = "6228482310541906";
	uchar CardPin[16];
	uchar TempBuf[16];
	uchar FmtCardNo[8];
	uchar FmtUserPin[8];
	uchar FmtPinBlock[8];
	int i = 0;

	memset(CardPin, 0x00,sizeof(CardPin));	
	Ret = emv_get_cardholder_pin(CardPin);
	if(Ret < 4
		|| Ret > 12)
	{
		if (Ret == EMV_RET_ERROR_BYPASSPIN)
		{
			pEncryptInfo->cPINInputFlag = 2; //BYPASSPIN
		}
		else
		{
			pEncryptInfo->cPINInputFlag = 3; //未输密
		}
		return Ret;
	}

	memset(TempBuf, 0x00, sizeof(TempBuf));
	memcpy(TempBuf, "0000", 4);
 	memcpy(TempBuf+4, CardNo+4, 12);
	memcpy(TempBuf+4, pEncryptInfo->acCardNo, 12);
	memset(FmtCardNo, 0x00, sizeof(FmtCardNo));
	for(i=0; i<sizeof(TempBuf);)
	{
		FmtCardNo[i/2] = AscConvertHex(TempBuf+i);
		i+=2;
	}    

	memset(TempBuf, 'F', 16);
	sprintf((char *)TempBuf, "%02X", (uchar)Ret);
	memcpy(TempBuf+2, CardPin, Ret);
	memset(FmtUserPin, 0x00, sizeof(FmtUserPin));
	for(i=0; i<sizeof(TempBuf);)
	{
		FmtUserPin[i/2] = AscConvertHex(TempBuf+i);
		i+=2;
	}

	memset(FmtPinBlock, 0x00, sizeof(FmtPinBlock));
	for(i=0; i<8; i++)
		FmtPinBlock[i] = FmtCardNo[i]^FmtUserPin[i];

	memset(gszOnlinePin, 0x00, sizeof(gszOnlinePin));
	calc_tdea(FmtPinBlock,gszOnlinePin,KeyBuff,16,TDEA_ENCRYPT);
	gucOnlinePinLen = 8;

	memcpy(pEncryptInfo->acEncryptPin, gszOnlinePin, gucOnlinePinLen);
	pEncryptInfo->cEncryptPinLen = gucOnlinePinLen;
	pEncryptInfo->cPINInputFlag = 1; //已输密
	lcd_Cls();		
	return EMV_RET_ERROR_OK;
#else
	int Ret = 0, nInLen = 0, i, nTmpLen;
	unsigned short usOutLen = 0;
	uchar ucBuff[128], ucOutData[128];
	s_lcd_emv tDispInfo;
    uchar szBuff[30];


    memset(&tDispInfo, 0x00, sizeof(s_lcd_emv));
	tDispInfo.frametype = 4; //密码框
	tDispInfo.button    = 0x00;  //仅显示
	tDispInfo.contextalign = 0;  //左对齐
	memcpy(szBuff,"请输入密码:", strlen((char *)"请输入密码:"));
	tDispInfo.context = szBuff;
    tDispInfo.contextlen = strlen((char *)szBuff);
    TRACE("\r\nget cardholer_pin 3");
    lcd_EmvDisp(&tDispInfo, 120000);


	memset(ucBuff, 0x00, sizeof(ucBuff));
	memset(ucOutData, 0x00, sizeof(ucOutData));
	memcpy(ucBuff+2, pEncryptInfo, pEncryptInfo->cInfoLen+21);
	nInLen = pEncryptInfo->cInfoLen+21;
	ucBuff[0] = nInLen & 0xff;
	ucBuff[1] = (nInLen>>8) & 0xff;
	nInLen+=2;
	Debug_ComPrintStr(9, "AppDebug7");
	Ret = ped_get_pin_online(nInLen, ucBuff, &usOutLen, ucOutData);
	Debug_ComPrintInt(9, "AppDebug8", Ret);
	Debug_ComPrintTitle(9, "AppDebug9", usOutLen, (char *)ucOutData);
	if (Ret)
	{
		Debug_ComPrintInt(9, "OnlinePin", Ret);
		pEncryptInfo->cPINInputFlag = 3; //未输密
		return EMV_RET_ERROR_USER_CANCEL;
	}
	if (usOutLen == 0)
	{
		pEncryptInfo->cPINInputFlag = 2; //BYPASSPIN
		//无密码直接按确认键，不返回BYPASSPIN，防止某些IC卡交易拒绝
// 		return EMV_RET_ERROR_BYPASSPIN;
	}
	else
	{
		memcpy(pEncryptInfo->acEncryptPin, ucOutData, usOutLen);
		pEncryptInfo->cEncryptPinLen = usOutLen;
		pEncryptInfo->cPINInputFlag = 1; //已输密
	}

	//磁道数据加密
	memset(ucBuff, 0x00, sizeof(ucBuff));
	memset(ucOutData, 0x00, sizeof(ucOutData));
	ST_EMV_GetAppData((uchar *)"\x57", 1, ucOutData, &usOutLen);  // Track 2 Equivalent Data
	nInLen = usOutLen;
	HexToAscii(ucBuff, ucOutData, nInLen*2, 0);
	for (i = 0; i < nInLen*2; i++)
	{
		//二磁道数据，读出来数据有D，但上送要改成=
		if (ucBuff[i] == 'D')
		{
			ucBuff[i] = 0x3D;  // =号
		}
		//二磁道数据，读出来数据有F，但上送不能有F
		if (ucBuff[i] == 'F')
		{
			break;
		}
	}

	memset(ucOutData, 0x00, sizeof(ucOutData));
	AsciiToHex(ucOutData, ucBuff, i, 0);
	if (i%2)
	{
		nInLen = i/2 + 1;
	}
	else
	{
		nInLen = i/2;
	}
	//加密磁道数据
	memset(ucBuff, 0x00, sizeof(ucBuff));
	ucBuff[2] = 0x08; //磁道加密密钥的密钥类型
	ucBuff[3] = pEncryptInfo->cIndex;
	ucBuff[4] = 0x01; //DES/3DES加密
	ucBuff[8] = 0x08;  //Len
	ucBuff[9] = 0x00;  //Len
	memcpy(ucBuff+10, ucOutData + nInLen - 9, 8);
	nTmpLen = 16;
	ucBuff[0] = nTmpLen & 0xff;
	ucBuff[1] = (nTmpLen>>8) & 0xff;
	nTmpLen += 2;

	Debug_ComPrintTitle(7, "encrpt1", 8, (char *)ucOutData + nInLen - 9);
	usOutLen = 0;
	Ret = ped_get_encrpt_data(nTmpLen, ucBuff, &usOutLen, ucOutData + nInLen - 9);
	Debug_ComPrintInt(7, "encrpt2", Ret);
	if (Ret == 0)
	{
		Debug_ComPrintInt(7, "encrpt3", usOutLen);
		Debug_ComPrintTitle(7, "encrpt4", usOutLen, (char *)ucOutData + nInLen - 9);
		HexToAscii(pEncryptInfo->acEncryptTrack2, ucOutData, i, 0);
		pEncryptInfo->cTrack2Len = i;
	}

	return EMV_RET_ERROR_OK;

#endif
}

int emv_process_pboc_certificate(void)
{
	uchar CertType, Cert[41];
	uchar  szBuff[128];
	int CertLen, nLen = 0;
//	int key;
        int Ret;
	s_lcd_emv tDispInfo;
	uchar  szBuff2[30];
	
    if (emv_get_data_element(0x9F62, &CertLen, &CertType) != EMV_RET_ERROR_OK)
		return EMV_RET_ERROR_DATA;

	if (emv_get_data_element(0x9F61, &CertLen, Cert) != EMV_RET_ERROR_OK)
		return EMV_RET_ERROR_DATA;

	if (CertLen > 40)
		return EMV_RET_ERROR_DATA;

#if 0
	lcd_Cls();
	lcd_Display(0, 0, DISP_INVLINE|FONT_SIZE16, "请核对证件");
	switch (CertType) 
	{
	case 0x00:
		lcd_Display(0, 2*8, FONT_SIZE16, "身份证:");
		break;
	case 0x01:
		lcd_Display(0, 2*8, FONT_SIZE16, "军官证:");
		break;
	case 0x02:
		lcd_Display(0, 2*8, FONT_SIZE16, "护照:");
		break;
	case 0x03:
		lcd_Display(0, 2*8, FONT_SIZE16, "入境证:");
		break;
	case 0x04:
		lcd_Display(0, 2*8, FONT_SIZE16, "临时身份证:");
		break;
	case 0x05:
		lcd_Display(0, 2*8, FONT_SIZE16, "其它证件:");
		break;
	}
	Cert[CertLen] = 0;
	lcd_Display(0, 4*8, FONT_SIZE16, "%s",Cert);
	while (1)
	{
		key = kb_GetKey(-1);
		if ((key != KEY_ENTER) && (key != KEY_CANCEL))
			continue;
		break;
	}

	if (key == KEY_ENTER)
		return EMV_RET_ERROR_OK;

	return EMV_RET_ERROR_DATA;
#else
	memset(szBuff, 0x00, sizeof(szBuff));
	switch (CertType) 
	{
	case 0x00:
		sprintf((char *)szBuff, "%s\n", "身份证:");
		break;
	case 0x01:
		sprintf((char *)szBuff, "%s\n", "军官证:");
		break;
	case 0x02:
		sprintf((char *)szBuff, "%s\n", "护照:");
		break;
	case 0x03:
		sprintf((char *)szBuff, "%s\n", "入境证:");
		break;
	case 0x04:
		sprintf((char *)szBuff, "%s\n", "临时身份证:");
		break;
	case 0x05:
		sprintf((char *)szBuff, "%s\n", "其它证件:");
		break;
	}
	Cert[CertLen] = 0;
	nLen = strlen((char *)szBuff);
	sprintf((char *)(szBuff+nLen), "%s\n", Cert);
	nLen = strlen((char *)szBuff);

	memset(&tDispInfo, 0x00, sizeof(s_lcd_emv));
	tDispInfo.frametype = 2; //列表框
	tDispInfo.button    = 0x03;  //确认键、取消键有效
	tDispInfo.titlealign = 1;  //居中
    memcpy(szBuff2, "请核对证件", 10);
    tDispInfo.title = szBuff2;
    tDispInfo.titlelen = 10;
	tDispInfo.contextalign = 0;  //左对齐
	tDispInfo.context = szBuff;
	tDispInfo.contextlen = nLen;
	Ret = lcd_EmvDisp(&tDispInfo, 120000);
	if (Ret == 0x01)
	{
		//确认键
		return EMV_RET_ERROR_OK;
	}
	else
	{
		return EMV_RET_ERROR_DATA;
	}

#endif
}

int emv_process_advice(void)
{    
// 	if(glGenAdvice)
// 	{
// 		return EMV_RET_ERROR_OK;
// 	}
// 
// 	glGenAdvice = 1;    
// 	emvBatchDataCapture(MSG_OFFLINEADVICE); // store offline advice or batch data record    
	return EMV_RET_ERROR_OK;
}

int  emv_process_online(uchar *RspCode,
			                           uchar *AuthCode, int *AuthCodeLen,
			                           uchar  *IAuthData, int *IAuthDataLen,
			                           uchar *script, int *ScriptLen)
{
// 	int Ret;   
// 
// 	glOnlineTrans = 0;
// 	glSendReverral = 0;    
// 
// 	Ret = emvSendOnlineAuth(RspCode,AuthCodeLen,AuthCode,IAuthDataLen,IAuthData,ScriptLen,script);
// 	if(Ret != EMV_RET_ERROR_OK)
// 	{
// 		Ret = emvSendOnlineAuth(RspCode,AuthCodeLen,AuthCode,IAuthDataLen,IAuthData,ScriptLen,script);
// 	}    
// 
// 	if(Ret == EMV_RET_ERROR_OK)
// 	{
// 		if (!memcmp(RspCode, "00", 2))
// 		{
// 			glOnlineTrans = 1;
// 			return ONLINE_APPROVE;   // Host approve the transaction
// 		}
// 		else if ((!memcmp(RspCode, "05", 2)) || (!memcmp(RspCode, "51", 2)))
// 		{
// 			glOnlineTrans = 0;
// 			return ONLINE_DENIAL;    // Host decline the transaction
// 		}
// 		else if (!memcmp(RspCode, "01", 2))
// 		{
// 			glOnlineTrans = 2;
// 			return ONLINE_REFER;    // need refer
// 		}
// 		else
// 		{
// 			glOnlineTrans = 3; // Invalid ARC
// 		}
// 	}
// 
// 	if(Ret == ONLINE_TIMEOUT)  // Timeout, can not goto online
// 	{
// 		lcd_Cls();		
// 		lcd_Display(0,3*8,FONT_SIZE16|DISP_MEDIACY,"无法联机");	
// 		return Ret;
// 	} 
// 
// 	glOnlineTrans = 5;
// 	lcd_Cls();	
// 	lcd_Display(0,3*8,FONT_SIZE16|DISP_MEDIACY,"联机失败");		
// 	return ONLINE_OTHER_ERROR;
    return EMV_RET_ERROR_OK;
}

int emv_process_issuer_refer(void)
{
// 	int len;
// 	uchar pan[100], buff[200];
// 	int rc;
// 
// 	memset(pan, 0x00, sizeof(pan));
// 	memset(buff, 0x00, sizeof(buff));
// 	len = 0;
// 	emv_get_data_element(0x5A, &len, buff);
// 	bcd_to_asc(pan, buff, len * 2);
// 
// 	lcd_Cls();	
// 	lcd_Display(0,0,FONT_SIZE16|DISP_MEDIACY,"呼叫银行,确认键继续");
// 
// 	lcd_Display(0,3*8,DISP_CLRLINE|FONT_SIZE8,"APP PAN:");
// 	lcd_Display(0,5*8,DISP_CLRLINE|FONT_SIZE8,"%s", pan);	
// 
// 	kb_GetKey(-1);
// 	lcd_Cls();	
// 	lcd_Display(0,3*8,FONT_SIZE16|DISP_MEDIACY,"是否批准交易?");
// 
// 	while(1)
// 	{
// 		rc = kb_GetKey(-1);
// 		if (rc == KEY_ENTER)
// 		{
// 			return EMV_RET_ERROR_OK; // Approve the transaction
// 		}
// 
// 		if(rc == KEY_CANCEL)
// 		{
// 			return EMV_RET_ERROR_DECLINE;  // Decline the transaction
// 		}
// 	}
	return EMV_RET_ERROR_DECLINE;
}

int emv_set_transaction_data(void)
{
	int   nAidLen = 0, nLen = 0;
	uchar ucAID[50], ucRet, ucBuff[10];
	EMVPARA tParaInfo;
	AppInitData tInitInfo;

	//获取当前应用的AID
	memset(ucAID, 0x00, sizeof(ucAID));
	emv_get_data_element(0x4F,&nAidLen,ucAID);

	//获取对应的参数
	memset(&tParaInfo, 0x00, sizeof(EMVPARA));
	ucRet = ST_EMV_GetICPara(ucAID, nAidLen, &tParaInfo);
	if (ucRet)
	{
		return EMV_RET_ERROR_NOT_ACCEPTED;
	}

	//设置应用数据
	memset(&tInitInfo, 0x00, sizeof(AppInitData));
	memcpy(tInitInfo.tAppVerNo, tParaInfo.tAppVerNo, 2);
	memcpy(tInitInfo.tTAC_Denial, tParaInfo.tTAC_Denial, 5);
	memcpy(tInitInfo.tTAC_Online, tParaInfo.tTAC_Online, 5);
	memcpy(tInitInfo.tTAC_Default, tParaInfo.tTAC_Default, 5);
    memcpy(tInitInfo.tTDOL, "\x9F\x02\x06\x5F\x2A\x02\x9A\x03\x9C\x01\x95\x05\x9F\x37\x04", 15); // 终端缺省TDOL
	tInitInfo.cLenTDOL = 15;
	memcpy(tInitInfo.tDDOL, tParaInfo.tDDOL, tParaInfo.cLenDDOL);
	tInitInfo.cLenDDOL = tParaInfo.cLenDDOL;
	memcpy(tInitInfo.tFloorLmt, tParaInfo.tFloorLmt, sizeof(tParaInfo.tFloorLmt));
	memcpy(tInitInfo.tLmt, tParaInfo.tLmt, sizeof(tParaInfo.tLmt));
	memcpy(tInitInfo.tPercent, tParaInfo.tPercent, sizeof(tParaInfo.tPercent));
	memcpy(tInitInfo.tMaxPercent, tParaInfo.tMaxPercent, sizeof(tParaInfo.tMaxPercent));
	memcpy(tInitInfo.tECTransLimit, tParaInfo.tECTransLimit, sizeof(tParaInfo.tECTransLimit));
	memcpy(tInitInfo.tQPBOCTransLimit, tParaInfo.tQPBOCLimit, sizeof(tParaInfo.tQPBOCLimit));
	memcpy(tInitInfo.tQPBOCFloorLimit, tParaInfo.tQPBOCFloorLimit, sizeof(tParaInfo.tQPBOCFloorLimit));
	memcpy(tInitInfo.tQPBOCCVMLimit, tParaInfo.tCVMLimit, sizeof(tParaInfo.tCVMLimit));
	ST_EMV_InitiateEMV(&tInitInfo);

	memset(ucBuff, 0x00, sizeof(ucBuff));
	memcpy(ucBuff, "\x9F\x33\x03", 3);	
	ST_EMV_GetAppData((uchar *)"\x9F\x33", 2, ucBuff+3, (unsigned short *)&nLen);
	if (tParaInfo.tOnline == 0x00)
	{
		//不支持联机PIN
		ucBuff[4] &= 0xBF;
	}
	else
	{
		//支持联机PIN
		ucBuff[4] |= 0x40;
	}
	ST_EMV_SetData(ucBuff, nLen+3);

	return EMV_RET_ERROR_OK;
}

int emv_ec_account_select(void)
{
	return 1;
}

