/*
 * =====================================================================================
 *
 *       Filename:  padmain.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/25/2014 10:25:32 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yehf (), yehf@itep.com.cn
 *        Company:  START
 *
 * =====================================================================================
 */
#include "wp30_ctrl.h"
//#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
#if 0 
/*-----------------------------------------------------------------------------}
 *  外部函数声明
 *-----------------------------------------------------------------------------{*/
extern void PPRT_ManageDealWith(uint mode);
extern void PPRT_ClearDLHs(uint mode);
/*-----------------------------------------------------------------------------}
 *  全局定义
 *-----------------------------------------------------------------------------{*/
ST_KEY_CURRENT gtCurKey;
uchar gWorkMode;    //MODE_INIT ~ MODE_LOCKED
uchar gAppProcolType; //0-V1 1-PCI
uchar gInPinEntry=0;  //
uchar gReadyAppid;  //准备打开应用号
uchar gAppAuthFlag; //多应用管理认证标志
uchar gEnableSelfCheck;  //0-not need check 1-need check

PINPAD_TIME_DEF gPinPadTime;
/*-----------------------------------------------------------------------------}
 *  全局缓冲区gucBuff
 *使用情况: 1.磁卡解析临时缓冲区 gucTmpBuf
 *          2.串口缓冲区
 *-----------------------------------------------------------------------------{*/
uchar *gucTmpBuf;
/*-----------------------------------------------------------------------------}
 *  全局定义
 *-----------------------------------------------------------------------------{*/
void ParaInit(void)
{
    gucTmpBuf = (uchar *)&gucBuff[PUBBUFFER_LEN/2];
    gWorkMode = MODE_INIT;
    memset((char *)&gPinPadTime,0,sizeof(gPinPadTime));
	 //gPinPadTime.self_check_24H = 0;
	 if(get_appenable())
	 {
	 	gEnableSelfCheck = 1;
	 }
	 else
	 {
	 	gEnableSelfCheck = 0;
	 }
}

void app_inkey(uchar key)
{
#if defined(CFG_INSIDE_PINPAD)
    if(KEY_CLEAR == key)
    {
        key = KEY_BACKSPACE;
    }
    PPRT_Send(CMD_KEY,&key,1);
#endif
}


/* 
 * pinpad_getkey - [GENERIC] 获取按键
 *   mode:0-发送键值 1-发送0键值
 * @ 
 */
int pinpad_getkey (int mode,int timeout_ms)
{
    int ret;
    ret = kb_getkey(timeout_ms);
    if ( mode ) {
        app_inkey(0);
    } else {
        app_inkey((uchar)ret);
    }
    return ret;
}		/* -----  end of function pinpad_getkey  ----- */

void LCD_LightOn(void)
{
    lcd_SetLight(LIGHT_TIMER_MODE);	
}

void LCD_LightOff(void)
{
    lcd_SetLight(LIGHT_OFF_MODE);	
}
void LCD_ClearScreen(void)
{
#if defined(CFG_INSIDE_PINPAD)
	uchar data=0;
	PPRT_Send(CMD_LCD_CLS,&data,1);	
#endif		
#if defined(CFG_EXTERN_PINPAD) 
    lcd_cls();
#endif
}

void LCD_Disp(int x,int y,int mode,const char *pcFormat,...)
{
	va_list ap;
	char buf[128];
	CLRBUF(buf);
	va_start(ap, pcFormat);				
	vsprintf((char *)&buf[4*3], pcFormat, ap);
	va_end(ap);							
#if defined(CFG_INSIDE_PINPAD)
    // line(4B)+col(4B)+dispmode(4B)+str
    memcpy(&buf[0],&x,1);
    memcpy(&buf[4],&y,1);
    memcpy(&buf[8],&mode,1);
    PPRT_Send(CMD_LCD_PLUS,(uchar *)buf,(ushort)strlen(&buf[12]));	
#endif
#if defined(CFG_EXTERN_PINPAD) 
//    DISPBUF("disp",12+strlen(&buf[12]),0,buf);
//    TRACE("\r\n %d-[%s]",y,&buf[12]);
    lcd_Display(x,y*DISP_FONT,mode,"%s",&buf[12]);
#endif
}

void pinpad_Display(uint8_t ucLine, const char *pcFormat, ...)
{
	va_list ap;
	char buf[128];
    uint dispmode;
	CLRBUF(buf);
	va_start(ap, pcFormat);				
	vsprintf(buf, pcFormat, ap);
	va_end(ap);							
    dispmode = DISP_FONT|DISP_CLRLINE;
    LCD_Disp(0,ucLine-1,dispmode,"%s",buf);
}


// mode:1-反显
// ucLine:显示范围1~5
void LCD_DispCenter(uint8_t ucLine,uint mode, const char *pcFormat, ...)
{
	va_list ap;
	char buf[128];
    uint dispmode;
	CLRBUF(buf);
	va_start(ap, pcFormat);				
	vsprintf(buf, pcFormat, ap);
	va_end(ap);							
    dispmode = DISP_FONT|DISP_CLRLINE|DISP_MEDIACY;
    if ( mode ) {
        dispmode |= DISP_INVLINE;
    }
    LCD_Disp(0,ucLine-1,dispmode,"%s",buf);
}

void DispTitle(void *title, uint clearFlag)
{
	if (clearFlag)
		LCD_ClearScreen();
    LCD_Disp(0,0,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE,"%s",title);
}

// str: max len = 31B
void DispStr(uchar displine, uint flag, void *str1, void *str2, void *str3)
{
	char buf[128];
	int len;
    uint dispmode;
	CLRBUF(buf);
	strcat(buf,(char *)str1);
	if(str2)
	{
		len = strlen(buf);
		buf[len] = 0x20;
		strcat(buf,(char *)str2);
	}
	if(str3)
	{
		len = strlen(buf);
		buf[len] = 0x20;
		strcat(buf,(char *)str3);
	}
	if(flag & DISP_CLR)
	{
		LCD_ClearScreen();
	}
    dispmode = DISP_FONT|DISP_CLRLINE;
	if(flag & DISP_INVLINE)
	{
		dispmode |= DISP_INVLINE;
	}
	if(flag & DISP_MEDIACY)
	{
		dispmode |= DISP_MEDIACY;
	}
    LCD_Disp(0,(displine-1),dispmode,"%s",buf);
}

void LCD_ClrLine(uint displine)
{
    LCD_Disp(0,(displine-1),DISP_FONT|DISP_CLRLINE," ");
}

void LCD_ShowPINPrompt(void)
{
#if defined(CFG_EXTERN_PINPAD)
	BuzzerOn(100);
	LCD_LightOn();
	LCD_Disp(0,1,DISP_CLRLINE|DISP_FONT,STR_PIN_PROMPT);
#endif	
}

void LCD_DispPlsReboot(uint line, uint stat)
{
#if defined(CFG_INSIDE_PINPAD)
	uchar *aucData;
	ushort usLen;
	uchar cmd;
	aucData = gucBuff;
#endif
	LCD_DispCenter((uchar)line,DISP_INVLINE, "PLS REBOOT");
	while(1)
	{
		
    #if defined(CFG_INSIDE_PINPAD)
        if(kb_hit())
		{
            pinpad_getkey(0,20);
		}
		if (!PPRT_Receive(&cmd, aucData, &usLen))
		{
			if(cmd == CMD_REQUEST)
			{
				if(stat == 0)
					aucData[0] = PPKB_REQUEST_RECOVERY_OK;
				else if(stat == 1)
					aucData[0] = PPKB_REQUEST_RECOVERY_ERR;
				else if(stat == 2)
					aucData[0] = PPKB_APP_RECOVERY_OK;
				else
					aucData[0] = PPKB_APP_RECOVERY_ERR;
				PPRT_Send(cmd, aucData, 1);
			}
		#if 0
			else if(cmd == PCI_CMD_DEBUG0)
			{
				PPRT_Debug0(aucData, &usLen);
			}
		#endif
		}
	#endif	
#if defined(CFG_INSIDE_PINPAD)
                kb_getkey(-1);
#endif
	}
}
// mode=0-APPAK  1-FSK
void LCD_DispDlFsk(uint mode)
{	
    LCD_ClearScreen();
	if(mode == 0)
	{
        LCD_Disp(0,0,DISP_INVLINE|DISP_MEDIACY|DISP_FONT|DISP_CLRLINE,"DL APPAK");
	}
	else
	{
        LCD_Disp(0,0,DISP_INVLINE|DISP_MEDIACY|DISP_FONT|DISP_CLRLINE,"DL FSK");
	}
    LCD_Disp(0,3,DISP_MEDIACY|DISP_FONT|DISP_CLRLINE,"[CANCEL] - Exit");
}


// 0-input sus 1-cancel 2-timeout 
uint8_t KB_GetNStr(uint8_t ucLine, uint8_t ucMinLen, uint8_t ucMaxLen, uint32_t uiTimeOut,
	uint32_t * puiRetLen, uint8_t * pucGetBuf)
{
	uint8_t ucKey,ucKeyBuf[16], len = 0;
	uint8_t gets_buf[16];
	int curTime = 0;
	int beginTime = 0;
	beginTime = sys_get_counter();	
	KB_Flush();
	gets_buf[0] = '_';
	gets_buf[1] = 0;
    ucLine -= 1;
    LCD_Disp(0,ucLine,DISP_FONT|DISP_CLRLINE,(char const *)gets_buf);
	//TRACE("\r\n timeout:%d-%d",beginTime,uiTimeOut);
	while (1)
	{
		curTime = sys_get_counter();
		if ((uiTimeOut > 0) && ((curTime - beginTime) >= uiTimeOut))
		{
			//TRACE("\r\n curTime:%d",curTime);
			return (2);
		}

		if (uiTimeOut == 0)
		{
			return (2);
		}

		if (kb_hit())
		{
            ucKey = (uchar)pinpad_getkey(1,-1);
			if (ucKey >= '0' && ucKey <= '9')
			{
				if (len >= ucMaxLen)
				{
					BuzzerOn(DEFAULT_WARNING_BEEPTIME);
					continue;
				}

				if (len < ucMaxLen)
				{
					ucKeyBuf[len++] = ucKey;
				}
			}
			else if (ucKey == KEY_CLEAR)
			{
				len = 0;
				memset(ucKeyBuf, 0, sizeof(ucKeyBuf));
				ucKeyBuf[0] = 0;
				beginTime = sys_get_counter();
			}
			else if (ucKey == KEY_ENTER)
			{
				if (len < ucMinLen)
				{
					BuzzerOn(DEFAULT_WARNING_BEEPTIME);
					continue;
				}
				memcpy(pucGetBuf, ucKeyBuf, len);
				*puiRetLen = len;
				return 0;
			}
			else if (ucKey == KEY_CANCEL)
			{
				*puiRetLen = 0;
				return 1;
			}
			memset(gets_buf, '*', len);
			gets_buf[len] = '_';
			gets_buf[len + 1] = 0;
            LCD_Disp(0,ucLine,DISP_FONT|DISP_CLRLINE,(char const *)gets_buf);
		}
	}
}

// input:   flag-  0-input psw and out psw 1-input psw and check 
//          line-  disp line 
//          psw -  
//          pswLen-
//          time-  psw check times
// 输出:    psw-
//          pswlen-
// return:  0-OK  1-PSW ERROR 2-Cancel
uint8_t InputOrCheckPsw(uint8_t flag, uint8_t line, uint8_t * psw, uint8_t * pswLen, uint8_t time)
{
	uint8_t str[24],ret;
	uint32_t len,i;
	i = 0;
	while (1)
	{
		pinpad_Display((uchar)(line + 1), "_");
		pinpad_Display(line, "INPUT PSW<NO.%d>:", (i + 1));
		// timeout= 12 S
		CLRBUF(str);
		ret = KB_GetNStr((uchar)(line + 1), *pswLen, *pswLen, PSW_INPUT_TIMER, &len, str);
//        DISPBUF("input psw",len,0,str);
		//vDispBuf(len, 0, str);
		if(ret == 1)
		{
			// cancel
			if (flag == 1)
			{
			#if CFG_MODE == CFG_MODE_FAC
				return 2;
			#else
				if(i == 0)
				{
					return 2;
				}
				else
				{
					goto InputOrCheckPsw_again;
				}
			#endif				
			}
			else
			{
				goto InputOrCheckPsw_again;
			}
		}
		else if(ret == 2)
		{
			// timeout
			if (flag == 0)
			{
				return 1;
			}
			else
			{
				goto InputOrCheckPsw_again;
			}
		}

		if (*pswLen != len)
		{
			if (flag == 0)
				return 1;
			else
			{
				goto InputOrCheckPsw_again;
			}
		}
		if (flag == 0)
		{
			memcpy(psw, str, len);
			return EM_SUCCESS;
		}
		//pinpad_Display(3, "%d:%s-%s", len, str,psw);		
		if (memcmp(psw, str, len))
		{
		InputOrCheckPsw_again:
			if (++i >= time)
			{
				return 1;
			}
			continue;
		}
		else
		{
			return EM_SUCCESS;
		}	
	}
}

char get_ascii_key(char old_key)
{
	char i = 0, c;
	const char *keya = "2ABC3DEF";
	const char *keyb = "ABC2DEF3";
	while (1)
	{
		c = keya[i];
		if (c == old_key)
		{
			return (keyb[i]);
		}
		if (!c)
		{
			return (0);
		}
		i++;
	}
}

// PTK
uint8_t KB_GetAStr(uint8_t ucLine, uint8_t ucMaxLen, char *pucGetBuf)
{
#if 0
	unsigned char c,len=0,old_key=0;
	char gets_buf[52];
	if (ucMaxLen > 48)
		return -1;
	KB_Flush();
	pinpad_Display(ucLine, " ");
	pinpad_Display(ucLine - 1, 17, "[%02d]", len);	
    LCD_Disp(0,ucLine,DISP_MEDIACY|DISP_FONT|DISP_CLRLINE,gets_buf);
	for (;;)
	{
		//pinpad_Display(ucLine - 1, 14, "[%x-%02d]", c,len);
        c = pinpad_getkey(1,-1);
		switch (c)
		{
		case KEY_CLEAR:
			if (len)
			{
				len--;
				gets_buf[len] = 0;
				old_key = c;
			#if PRODUCT_TYPE == PRODUCT_TYPE_F12	
				if(len == 21 || len == 21*2)
				{
					gets_buf[len] = 0x20;
					gets_buf[len+1] = 0;
				}
			#endif	
				goto KB_GetAStr_Disp;				
			}
			break;

		case KEY_ENTER:
			memcpy(pucGetBuf,gets_buf,len);
			return (len);
		case KEY_CANCEL:
			return (-1);
		default:
			if (((c >= '0') && (c <= '9')) || (c == '.'))
			{
				if (len < ucMaxLen)
				{
					gets_buf[len++] = c;
					old_key = c;
					goto KB_GetAStr_Disp;
				}
				else
				{
					BuzzerOn(DEFAULT_WARNING_BEEPTIME);
					break;
				}
			}	
		#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
			else if (c == KEY_F1)
		#else
			else if (c == KEY_ALPHA)
		#endif
			{
				if((old_key >= '2' && old_key <= '3') 
					|| (old_key >= 'A' && old_key <= 'F'))
				{
					c = get_ascii_key(old_key);
					gets_buf[len-1] = c;
					old_key = c;
					goto KB_GetAStr_Disp;
				}
			}
			break;
		KB_GetAStr_Disp:
			pinpad_Display(ucLine - 1, 17, "[%02d]", len);			
		#if 0
			memset(disp_buf,0,sizeof(disp_buf));
			if (len > 20)
			{
				memcpy(disp_buf, gets_buf + len - 20, 20);
				//disp_buf[20] = '_';
			}
			else
			{
				memcpy(disp_buf, gets_buf, len);
				//disp_buf[len] = '_';
			}
			pinpad_Display(ucLine, "%s", disp_buf);
		#else
			pinpad_Display(ucLine, "%s", gets_buf);
		#endif			
			break;
		}
	}
#endif
    return 0;
}


// appno=0   主控
// appno!=0  应用
// 0-OK 1-check psw err 2-cancel 3-flash err
uint InputPtk(uint appno)
{
	uint len,i;
	uchar ptka[50];
	uchar ptkb[50];
	uchar ptk[48];
	uchar str[24];  //psw
	uchar ret;
	if(appno)
	{
		DispTitle(STR_USERA,1);
		app_read_info(appno,INFO_PSW,&len,str);	
	}
	else
	{
		DispTitle(STR_ADMINA,1);
		read_syspsw(PSW_USER_A,&len,str);
	}
	len = 8;
#ifndef __DEBUG_AUTOINPUT__
	// check psw A
	ret = InputOrCheckPsw(1, 2, str, (uint8_t *)&len, 1);
	if (ret)
	{
		return ret;
	}
	DispStr(2,0,STR_Input, STR_PTK, "A(48)");	
	#ifndef __AUTOINPUT_PTK__
	while(1)
	{
		CLRBUF(ptka);
		if (KB_GetAStr(3, 48, (char *)ptka) == 48)
		{
			break;
		}
	}
	#else
	memset(ptka,'1',48);
	KB_GetKey(-1);
	#endif
#else
	memset(ptka,'1',48);
	KB_GetKey(-1);
#endif	
	if(appno)
	{
		DispTitle(STR_USERB,1);
		app_read_info(appno,INFO_PSW,&len,str);
		memcpy(str,&str[8],8);				
	}	
	else
	{
		DispTitle(STR_ADMINB,1);
		read_syspsw(PSW_USER_B,&len,str);
	}
	len = 8;
#ifndef __DEBUG_AUTOINPUT__		
	// check psw B
	ret = InputOrCheckPsw(1, 2, str, (uint8_t *)&len, 1);
	if (ret)
	{
		return ret;
	}
	DispStr(2,0,STR_Input, STR_PTK, "B(48)");
	#ifndef __AUTOINPUT_PTK__
	while(1)
	{
		CLRBUF(ptkb);
		if (KB_GetAStr(3, 48, (char *)ptkb) == 48)
		{
			break;
		}
	}
	#else
	memset(ptkb,'2',48);
	KB_GetKey(-1);
	#endif
#else
	memset(ptkb,'2',48);
	KB_GetKey(-1);
#endif
	Str2Hex(48, ptka, (int *)&len, ptk);
	Str2Hex(48, ptkb, (int *)&len, ptka);
	for (i = 0; i < 24; i++)
	{
		ptk[i] ^= ptka[i];
	}
#ifdef EM_DEBUG		
	vDispBufTitle("PTK", sizeof(ptk), 0, ptk);
#endif		
	i = write_transferkey(appno,ptk);
	CLRBUF(ptka);
	CLRBUF(ptkb);
	CLRBUF(ptk);
	if(i)
	{		
		return 3;
	}
	return 0;
}



// mode=0 public check error
// mode=1 app    check error
uchar PPRT_Systemrecover(uchar *pucData, ushort *pusLen)
{
#if 0
	uchar aucTemp[16];
	uchar hsk[8];
	switch (*pucData)
	{
	case 0x00:
		//recv: 00 + T_Rand(8B)
		//send: 01 + T_Rand_E(8B) + P_Rand(8B) + 16B Data		
		if(*pusLen != 9)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_Systemrecover_Err;
		}		
		if(gWorkMode != MODE_SYSRECOVER)
		{
			pucData[1] = RESULT_ERROR;
			goto PPRT_Systemrecover_Err;
		}
		PCI_GetHSK(2, hsk);
		des_encrypt_ecb(pucData + 1, aucTemp, 8, hsk, 1);
		memcpy(pucData+1, aucTemp, 8);
		//memset(gtCurKey.aucRand,0,8);
		RNG_FillRandom(gtCurKey.aucRand, 4*2);  		
		memcpy(pucData+1+8, gtCurKey.aucRand, 8);
		//                    0123456789abcdef
		//memcpy(pucData + 17, "START MK-210V2.0.0 101021", 16);
	#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
		//11B:STAR MK-210
		memcpy(pucData+1+8+8, gCtrlVerInfo.product, 11);
		//5B:V2.00
		memcpy(pucData+1+8+8+11, gCtrlVerInfo.version, 5);
	#else
		//11B:STAR S980PP
		memcpy(pucData+1+8+8, gCtrlVerInfo.product, 11);
		//5B:V1.00
		memcpy(pucData+1+8+8+11, gCtrlVerInfo.version, 5);
	#endif
		*pusLen = 33;
		pucData[0] = 0x01;
	#ifdef EM_DEBUG
		vDispBuf(33, 0, pucData);
	#endif	
		gtCurKey.HsFlag = HS_STAT_ING;
		return EM_ERROR;
		
	case 0x02:
		//recv: 02 + Stat + P_Rand_E(8B)
		//send: 03 + Stat 
		if(*pusLen != 10)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_Systemrecover_Err;
		}
		if (pucData[1] != 0)
		{
			pucData[1] = RESULT_ERROR;
			goto PPRT_Systemrecover_Err;
		}
		if (gtCurKey.HsFlag != HS_STAT_ING)
		{
			DISPERR(gtCurKey.HsFlag);
			pucData[1] = RESULT_ERROR;
			goto PPRT_Systemrecover_Err;
		}
		PCI_GetHSK(2, hsk);
		des_encrypt_ecb(gtCurKey.aucRand, aucTemp, 8, hsk, 1);
		if (!memcmp(pucData + 2, aucTemp, 8))
		{			
			if(PKS_CheckPbulicKey())
			{
				if(sys_recover(1))
				{
					pucData[1] = RESULT_ERROR_FLASH;
					goto PPRT_Systemrecover_Err;
				}
			}
			else
			{
				if(app_recover())
				{
					pucData[1] = RESULT_ERROR_FLASH;
					goto PPRT_Systemrecover_Err;
				}
			}			
			pucData[0] = 0x03;
			pucData[1] = RESULT_SUCCESS;
			gtCurKey.HsFlag = HS_STAT_NOT;
			*pusLen = 2;			
			return EM_SUCCESS;
		}
		else
		{
			DISPERR(0);
			pucData[1] = RESULT_ERROR;
			goto PPRT_Systemrecover_Err;
		}		
	default:
		pucData[1] = RESULT_NOT_PERMIT;
		break;		
	}
PPRT_Systemrecover_Err:
	pucData[0] += 1;
	gtCurKey.HsFlag = HS_STAT_NOT;
	*pusLen = 2;
#endif
	return EM_ERROR;
}

// mode=0  单个应用故障
//     =1  整体故障
void HardFault(uint mode, char *str1, char *str2)
{
#if defined(CFG_INSIDE_PINPAD)
	ushort usLen;
	uchar cmd;
	uchar ret;
        uchar *aucData = gucBuff;
#endif
#if 2
	if(mode == 0)
		PCI_InitPublicKey(2);
	else
		PCI_InitPublicKey(1);
        LCD_ClearScreen();
	LCD_DispCenter(2,DISP_INVLINE, "DEVICE LOCKED");
	if(str2)
	{
		LCD_DispCenter(3,0,"%s %s",str1,str2);
	}
	else
	{
		LCD_DispCenter(3,0,"%s", str1);
	}
	gWorkMode = MODE_LOCKED;
	while(1)
	{
    #if defined(CFG_EXTERN_PINPAD)
        BuzzerOn(DEFAULT_WARNING_BEEPTIME);
		s_DelayMs(2000);
	#endif	
    #if defined(CFG_INSIDE_PINPAD)
		if(kb_hit())
		{
            pinpad_getkey(0,100);
		}
		if (!PPRT_Receive(&cmd, aucData, &usLen))
		{
			if(cmd == CMD_REQUEST)
			{
				aucData[0] = gWorkMode;
				PPRT_Send(cmd, aucData, 1);
			}
			else if(cmd == CMD_SYSCOVERY)
			{
				ret = PPRT_Systemrecover(aucData,&usLen);
				PPRT_Send(cmd, aucData, usLen);				
				if(!ret)
				{
					gWorkMode = MODE_INIT;
					#ifdef CFG_CMD_V1
				    open_default_V1APPP();
					#endif	
					break;
				}
				
			}
			else if(cmd == CMD_CHECK_SYSPSW)
			{
                if ( aucData[0] != 1 ) {
                    aucData[0] = CheckPsw(0,PSW_SYS);
                } else {
                    aucData[0] = 0;
                }
				PPRT_Send(cmd, aucData, 1);
				if(aucData[0] == 0)
				{
					gWorkMode = MODE_SYSRECOVER;
				}
			}
			else if(cmd == CMD_FACTEST && aucData[0] == 0)
			{
				PPRT_FactoryTest(aucData,&usLen);
				PPRT_Send(cmd, aucData, 1);
			}
			else if(cmd == CMD_TAMPER && aucData[0] == 0)
			{
				PPRT_TamperPinStatus(aucData,&usLen);
				PPRT_Send(cmd, aucData, usLen);
			}
            else if(cmd == CMD_GETTIME)
            {
                app_get_time(aucData, &usLen);
				PPRT_Send(cmd, aucData, usLen);
            }
            else if(cmd == CMD_SETTIME)
            {
                app_set_time(aucData, &usLen);
				PPRT_Send(cmd, aucData, usLen);
            }
		}
	#endif	
	}
#endif	
}

uint CalCodeSha(uchar *out, uchar *mac)
{
	// 0x00000~0x18000
	uchar fsk[FSK_LEN];
//	if (!sha_createHash_multi_new(SA_BOOT,(LEN_BOOT+72*1024),out,SHA_MODE))
    if(sha256((const uint8_t *)SA_BOOT,LEN_BOOT,out))
	{		
		if(!PCI_ReadFSK(FSK_SMAPP,fsk))
		{
			cacul_mac_ecb(FSK_LEN,fsk,LEN_BSV_SHA,out,LEN_BSV_MAC,mac);
			CLRBUF(fsk);	
			return 0;
		}				
	}	
	return 1;
}



uint check_BSV(void)
{
	uchar str[LEN_BSV];
	uchar check[LEN_BSV_SHA+LEN_BSV_MAC];
	s_read_syszone(OFFSET_BSV,LEN_BSV,str);
	if(mymemcmp(str, LABLE_BOOTINFO, LABLE_BOOTINFO_LEN))
	{
		HardFault(0,STR_BSV,STR_VER);
	}
	if(!CalCodeSha(check,&check[LEN_BSV_SHA]))
	{
		if(!mymemcmp(&str[OFFSET_BSV_SHA],check, LEN_BSV_SHA))
		{
			if (!mymemcmp(&str[OFFSET_BSV_MAC], &check[LEN_BSV_SHA], LEN_BSV_MAC))
			{
				CLRBUF(str);
				return 0;
			}
			else
			{
				HardFault(0,STR_BSV,STR_MAC);
			}
		}
	}
	HardFault(0,STR_BSV,STR_SHA);
	return 1;
}

uint check_ASV(void)
{
#if 0
	uint codeLen;
	uchar SHA256Check[LEN_APPSYS_SHA+LEN_APPSYS_MAC];
	uchar str[LEN_APPSYS_SHA+LEN_APPSYS_MAC];
	uchar fsk[FSK_LEN];
	s_read_syszone(OFFSET_APP_CHECK+LABLE_SYSZONE_LEN,4,(uchar *)&codeLen);	
	readFlashPage((SA_CTRL+codeLen-OFFSET_APPSYS_APPINFO)/2,str, LEN_APPSYS_APPINFO);			
	if (mymemcmp(str, LABLE_APPINFO, LABLE_APPINFO_LEN))
	{	
        TRACE("app0:%s",APP_INFO);
        vDispBufTitle("app1",LEN_APPSYS_APPINFO,0,str);	
		HardFault(0,STR_ASV,STR_VER);
	}	
	sha_createHash_multi_new(SA_CTRL/2, codeLen-OFFSET_APPSYS_SHA, SHA256Check,SHA_MODE);
	readFlashPage((SA_CTRL+codeLen-OFFSET_APPSYS_SHA)/2,str, LEN_APPSYS_SHA+LEN_APPSYS_MAC);	
	if (mymemcmp(SHA256Check, str, LEN_APPSYS_SHA))
	{
        TRACE("Len:%X",codeLen);			
        vDispBufTitle("SHA0",LEN_APPSYS_SHA+LEN_APPSYS_MAC,0,str);	
        vDispBufTitle("SHA1",LEN_APPSYS_SHA+LEN_APPSYS_MAC,0,SHA256Check);	
		HardFault(0,STR_ASV,STR_SHA);
	}
	PCI_ReadFSK(FSK_SMAPP,fsk);	
	cacul_mac_ecb(FSK_LEN,fsk,LEN_APPSYS_SHA,SHA256Check,LEN_APPSYS_MAC,&SHA256Check[LEN_APPSYS_SHA]);
	CLRBUF(fsk);
	if (mymemcmp(&SHA256Check[LEN_APPSYS_SHA], &str[LEN_APPSYS_SHA], LEN_APPSYS_MAC))
	{
		HardFault(0,STR_ASV,STR_MAC);
	}
	CLRBUF(SHA256Check);
	CLRBUF(str);
#endif       
	return 0;
}

// 0-check 1-set sram 2-IO test
void PPRT_FactoryTest(uchar *pucData, ushort *pusLen)
{
//    uint32_t ret=1;
//
//    if(gEnableSelfCheck)
//    {
//        ret = PKS_CheckPbulicKey();
//    } else 
//    {
//        if (pucData[0] <= 1)
//        {
//            ret = Fac_SRAM(pucData[0]);
//        } else if (pucData[0] == 2)
//        {
//            ret = Factory_IO_Test(pucData[1]);   
//        }
//    }
//    
//    *pucData = ret;
//    *pusLen = 1;
}

void PPRT_TamperPinStatus(uchar *pucData,ushort *pusLen)
{
#if (defined CFG_TAMPER)
    pucData[0] = 0x07;//tamper pin normal status mask:0000 0111
    pucData[1] = hw_dryice_read_pin();
    *pusLen = 2;
    TRACE("\n-|Tamper PIN:%02X",pucData[1]);
#else
    pucData[0] = 0x00;//tamper pin normal status mask:0000 0111
    pucData[1] = 0x00;
    *pusLen = 2;
#endif
}


//mode = 0 check now     =1 1s check
void SelfCheck(uint mode)
{
#if (defined CHECK_SELF)
	if(mode == 0 || sys_get_counter()-gPinPadTime.self_check>1000)
	{
		if(!gEnableSelfCheck)
		{
			return;
		}
		// public key checking
		if(PKS_CheckPbulicKey())
		{
			HardFault(0,"PUBLIC",STR_KEY);
		}					
		// work key checking
//		if (PKS_CheckWorkKey())
//		{
//			HardFault(1,"WORK",STR_KEY);
//		}
		if(PCI_ReadAuthErrTimes(gtCurKey.appid) >= MAXNUM_AUTH)
		{
			HardFault(1,STR_AUTHERR,NULL);
}
	#if 0	
		if(sys_get_counter() - gPinPadTime.self_check_24H > 1000*3600*24)
		{
			check_BSV();
			check_ASV();
			gPinPadTime.self_check_24H = sys_get_counter();
		}
	#endif
		gPinPadTime.self_check = sys_get_counter();
	}		
#endif		
}


//        appno==0      appno!=0
//pswType=0:SYS PSW
//       =1:admin A      user A
//       =2:admin B      user B
// return: 0-OK  1-cancel exit  2-check err
uchar CheckPsw(uint appno, uchar pswType)
{
	uchar str[24];
	uint len = 8,i;
	CLRBUF(str);
	switch(pswType)
	{
		case PSW_SYS:
			read_syspsw(PSW_SYS,&len,str);				
			DispStr(1,DISP_CLR|DISP_INVLINE,STR_Input,STR_PSW,STR_SYS);
		#ifdef __DEBUG_AUTOINPUT__
			return OK;
		#endif
			break;
		case PSW_USER_A:
			if(appno == 0)
			{
				read_syspsw(PSW_USER_A,&len,str);	
				DispStr(1,DISP_CLR|DISP_INVLINE,STR_Input,STR_PSW,STR_ADMINA);
			}
			else
			{
				if(app_read_info(appno,INFO_PSW,&len,str))
				{
					return ERROR;
				}
				DispStr(1,DISP_CLR|DISP_INVLINE,STR_Input,STR_PSW,STR_USERA);
			}
			
			break;
		case PSW_USER_B:
			if(appno == 0)
			{
				read_syspsw(PSW_USER_B,&len,str);		
				DispStr(1,DISP_CLR|DISP_INVLINE,STR_Input,STR_PSW,STR_ADMINB);
			}
			else
			{	
				if(app_read_info(appno,INFO_PSW,&len,str))
				{
					return ERROR;
				}
				memcpy(str,&str[8],len);
				DispStr(1,DISP_CLR|DISP_INVLINE,STR_Input,STR_PSW,STR_USERB);
			}			
			
			break;
	}
//	DISPBUF("psw", PSW_LEN, 0, str);
	len = PSW_LEN;
	i = (uint)InputOrCheckPsw(1, 2, str, (uchar *)&len, 1);
	CLRBUF(str);
	if (i == 1)
	{
		return 2;
	}
	else if(i == 2)
	{
		return 1;
	}	
	return OK;
}

uchar InputPsw(uchar * psw)
{
	uchar str[8];
	uchar len = 8;
	if(1)
	{
		while (1)
		{
			len = 8;
            pinpad_Display(2, "INPUT NEW PSW");
			if (InputOrCheckPsw(0, 3, str, &len, 0))
			{
				return EM_ERROR;
			}
            pinpad_Display(2, "INPUT AGAIN");
			if (InputOrCheckPsw(1, 3, str, &len, 1))
			{
				pinpad_Display(3, "NOT MATCH");
				pinpad_Display(4, "");
				KB_GetKey(-1);
				continue;
			}
			break;
		}
		memcpy(psw, str, 8);
	}	
	return EM_SUCCESS;
}

uint Check_user_psw(uint appno)
{
	if (CheckPsw(appno,PSW_USER_A))
	{
		return ERROR;
	}
	if (CheckPsw(appno,PSW_USER_B))
	{
		return ERROR;
	}
	return OK;
}
// mode=0  modify syspsw
// mode=1  modify userpsw
// mode=2  check and modify userpsw
// 0-OK  1-flash err 2-input err 3-cancel
uint ChangePsw(uint appno, uint mode)
{
	uchar psw[16];
	uint ret;
	if(mode == 0)
	{
		DispStr(1,DISP_CLR|DISP_INVLINE,STR_Change,STR_PSW,STR_SYS);	
		if (InputPsw(psw))
		{
			return 2;
		}
		ret = write_syspsw(PSW_SYS,PSW_LEN, psw);
		CLRBUF(psw);
		return ret;
	}
	else
	{
		if(mode == 2)
		{
			ret = CheckPsw(appno,PSW_USER_A);
			if (ret == 1)
			{
				return 3;
			}
			else if(ret == 2)
			{
				return 2;
			}
		}
		
		DispStr(1,DISP_CLR|DISP_INVLINE,STR_Change,STR_PSW,STR_USERA);	
	#ifndef __DEBUG_AUTOINPUT__
		if (InputPsw(&psw[0]))
		{
			return 2;
		}
	#else
		memset(psw,'1',8);
		KB_GetKey(-1);
	#endif
		if(mode == 2)
		{
			ret = CheckPsw(appno,PSW_USER_B);
			if (ret == 1)
			{
				CLRBUF(psw);
				return 3;
			}
			else if(ret == 2)
			{
				CLRBUF(psw);
				return 2;
			}
		}	
		DispStr(1,DISP_CLR|DISP_INVLINE,STR_Change,STR_PSW,STR_USERB);
	#ifndef __DEBUG_AUTOINPUT__
		if (InputPsw(&psw[PSW_LEN]))
		{
			CLRBUF(psw);
			return 2;
		}
	#else
		memset(&psw[PSW_LEN],'2',8);
		KB_GetKey(-1);
	#endif	
		ret = app_write_info(appno,INFO_PSW, psw);
		CLRBUF(psw);
		return ret;
	}
}
// mode=0 初始显示
// mode=1 屏保显示
void DispScreen(uint mode)
{
#if defined(CFG_EXTERN_PINPAD)
	static uchar displine;
	if(mode==0)
	{
		displine = 1;
	}
	else
	{
		if(gLcdSys.LcdBackLightTimerCnt10ms == 0)
		{
			if(sys_get_counter()-gPinPadTime.idle >= 5000)
			{
				mode = 0;
				++displine;
			}
		}	
	}
    if ( !mode ) {
        LCD_ClearScreen();
        LCD_DispCenter(displine%4+1,0,"==WELCOME==");
        LCD_DispCenter(displine%4+2,0,"欢迎光临");
//        LCD_DispCenter(displine%4+2,0,"欢迎光临%d-%d",displine,gPinPadTime.idle);
        gPinPadTime.idle = sys_get_counter();
    }
#endif
}

#ifdef CFG_DBG_PINPAD
char gDebugFlg = 0;
/* 
 * menu_dbg_pinpad - [GENERIC] 
 * @ 
 */
int menu_dbg_pinpad (int mode)
{
    int ret = 0;
    lcd_cls();
    lcd_Display(0,DISP_FONT*0,DISP_FONT|DISP_INVLINE,"Flg:%d-%x %d",gDebugFlg,ret,sys_get_counter());
    lcd_Display(0,DISP_FONT*1,DISP_FONT,"0~3-gDebugFlg");
    lcd_Display(0,DISP_FONT*2,DISP_FONT,"9-dbg_pinpad");
    ret = KB_GetKey(-1);
    switch ( ret )
    {
    case '0':
    case '1':
    case '2' :
    case '3' :
        gDebugFlg = ret - '0';
        break;
    case '9' :
        ret = gDebugFlg;
        gDebugFlg = 1;
        dbg_pinpad();
        gDebugFlg = ret;
        break;
    default :
        break;
    }
    lcd_cls();
    return ret;
}
#endif

// item:0-modify user psw (F12)
//      1-modif  user psw (F17)
uchar menu_app(uchar item)
{
#ifdef CFG_DBG_PINPAD
    uint ret;
    ret = menu_dbg_pinpad(0);
    return ret;
#else
    uchar key,stat;
    uchar filename[16],dispfilename[16];
    uint len,i,j,ret;
    if(item != 1)
    {
        if(CheckPsw(0,PSW_SYS))
            return 1;
    }

    while(1)
    {
        lcd_cls();
        //                      123456
        // 半行 64/6=10个字符(1.abcdef..) 文件名最多显示6个字符
        lcd_Display(0,DISP_FONT*0,DISP_FONT|DISP_MEDIACY|DISP_INVLINE,"应用列表");
        j = 0;
        i = 1;
        stat = 0;
        while ( i<=NUM_APP ) {
            CLRBUF(filename);
            CLRBUF(dispfilename);
            if(app_read_info(i,INFO_APPNAME,&len,filename))
            {
                strcat((char *)dispfilename,"NONE");				
            }
            else
            {
                stat |= (1<<(i-1));
                memcpy(dispfilename,filename,6);
                strcat((char *)dispfilename,"..");				
            }
            LCD_Disp(j%2*64,1+j/2,DISP_FONT, "%d.%s",i,dispfilename);
            ++i;
            ++j;
        }
        key = pinpad_getkey(1,-1);
        if(key == KEY_CANCEL)
        {
            return 0;
        }
        else if(key < '1' || key > (NUM_APP+'0'))
        {
            continue;
        }	
        key -= '1';
        if(!(stat&(1<<key)))
        {
            continue;
        }
        i = key+1; 
        CLRBUF(filename);
        app_set_APPEK(i,1);
        app_read_info(i,INFO_APPNAME,&len,filename);
        while(1)
        {
            lcd_cls();
            lcd_Display(0,DISP_FONT*0,DISP_FONT|DISP_MEDIACY|DISP_INVLINE,(char const *)filename);
            lcd_Display(0,DISP_FONT*1,DISP_FONT,"1-CHANGE USER PSW");
            lcd_Display(0,DISP_FONT*2,DISP_FONT,"2-CHANGE PTK");
//            lcd_Display(0,DISP_FONT*3,DISP_FONT,"3-INIT APP");
            key = pinpad_getkey(1,-1);
            switch ( key )
            {
            case '1' :
                ret = ChangePsw(i,2);
                break;
            case '2' :
                ret = InputPtk(i);				
                break;
            case '3' :
//                if ( app_get_type(i) == APP_PROTOCOL_V1 ) {
//                    if ( !CheckPsw(i,PSW_USER_A)) {
//                        if ( !CheckPsw(i,PSW_USER_B) ) {
//                            ret = app_create_again(i);
//                        }
//                    }
//                }
                break;
            default :
                goto menu_app_again;
            }
            lcd_cls();
            if ( ret == 0) {
                lcd_Display(0,DISP_FONT*2,DISP_FONT|DISP_MEDIACY,"OK");
            } else {
                lcd_Display(0,DISP_FONT*2,DISP_FONT|DISP_MEDIACY,"ERR");
            }
            pinpad_getkey(0,3000);
            continue;
        }
menu_app_again:
        app_set_APPEK(i,0);
    }
#endif    
}
// item=1 modify syspsw
// item=0 modify userpsw
uchar menu_sys(uchar item)
{
	//uint ret=1;
	//uchar key;	
	//uchar str[24];	
	if(item == 1)
	{
		if(CheckPsw(0,PSW_SYS))
		{
			return 1;
		}
		return ChangePsw(0,PSW_SYS);
	}
	else if(item == 2)
	{
		return menu_app(0);
	}
	else if(item == 3)
	{
		return menu_app(1);
	}
	return 1;
}

void DispVer(void)
{
#if defined(CFG_EXTERN_PINPAD)
	uchar key;
    lcd_cls();
    lcd_Display(0,DISP_FONT*1,DISP_FONT|DISP_MEDIACY,"%s %s",(char *)k_SonTermName,(char *)gCtrlVerInfo.version);
	lcd_Display(0,DISP_FONT*2,DISP_FONT|DISP_MEDIACY,"%s",gCtrlVerInfo.time);
	key = KB_GetKey(2000);
	if(key == KEY_F2)
	{
		menu_app(0);
	}
	else if(key == KEY_CANCEL)
	{
		//SA_vFactoryTest(0);
	}
#endif	
}

void Pad_recover(void)
{	
#ifdef SYS_RECOVER	
	uchar ret;
	uint stat=0;
	if(PKS_CheckPbulicKey())
	{
		DispTitle(STR_SYSRECOVER,1);
		ret = sys_recover(0);
		if(!ret)
			stat = 0;
		else 
			stat = 1;
	}
	else
	{
		DispTitle("APP RECOVER",1);
		ret = app_recover();
		if(!ret)
			stat = 2;
		else 
			stat = 3;
	}
	if(!ret)
	{
		LCD_DispCenter(2,0, "RECOVER OK");		
	}
	else
	{
		LCD_DispCenter(2,0, "RECOVER Err:%d",ret);
	}
	LCD_DispPlsReboot(4,stat);
#endif
}


void PadMain(void)
{
    //uint32_t i=0;
    //uchar key,stat=0;
//extern int s_InitProduct(void);
//    int i;
    uint data_vail_len=0;
    int s_pos=0, e_pos=0, pos=0;
    int len;
    int ret;
//    TRACE("\r\n ====PINPAD MAIN====");
//    TRACE("\r\n 密钥区起始地址:%x 大小:%x 应用数:%d 单应用大小:%x",KEY_ZONE_ADDR,sizeof(struct APP_ZONE),APP_TOTAL_NUM,sizeof(struct KEY_APP));
//    for ( i=0 ; i<APP_TOTAL_NUM ; i++ ) {
//        TRACE("\r\n 应用%d起始地址:%x",i+1,KEY_ZONE_APP_ADDR(i));
//    }
    
    lowerpower_init();
//    UART_Init(uart_get_bps());

//	ParaInit();
//	PCI_InitPublicKey(0);
//	Pad_recover();
//	gWorkMode = MODE_CTRLMANAGE;
#if defined(CFG_EXTERN_PINPAD)	
	gWorkMode = MODE_IDLE;
	LCD_LightOn();
	BuzzerOn(100);
	DispVer();
	DispScreen(0);
#endif	
//#if defined(CFG_EXTERN_PINPAD)	
//#ifdef CFG_CMD_V1
//	gucBuff[0] = open_default_V1APPP();
//	if(gucBuff[0] == 2)
//	{
//		LCD_DispCenter(3,0,"APP OPEN ERR");
//		KB_GetKey(3000);
//	}
//#endif
//#endif
//	SelfCheck(0);
//    enter_lowerpower_freq();
//    lcd_SetLighttime(25000); //

#ifdef DEBUG_Dx
    drv_uart_close(WORK_COMPORT); 
    console_init(CNL_COMPORT,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
#endif

	while (1)
	{		
        if (IfInkey(0))
        {
            break;
        }
//        sys_power_save(0,20);
//		if (kb_hit())
//		{
//            if (i == LED_ALL) {
//                hw_led_on(LED_ALL);
//                i = 0;
//            } else {
//                hw_led_off(LED_ALL);
//                i = LED_ALL;
//            }
//        #ifdef CFG_CMD_V1
//            v1protocol_timeout_daemon(1);
//        #endif
//            gucBuff[0] = (uchar)pinpad_getkey(1,30);
//            if ( gucBuff[0] == KEY_F2) {
//                menu_app(0);
//            }
//		}	
    #if (defined(CFG_MAGCARD)) && (defined(CFG_INSIDE_PINPAD))
        magcard_main(0);
	#endif
        //		PPRT_DealWith();
        if (ctc_uart_dma_check()) {
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
//            data_vail_flag = 0;
            data_vail_len = 0;
            memset(gwp30SysBuf_c.work, sizeof(gucBuff), 0);
            ctc_uart_restart();
//            drv_uart_dma_start(CTC_COMPORT, &dma_config);
        }

#ifdef CFG_CMD_V1
        v1protocol_timeout_daemon(0);
#else
		DispScreen(1);
#endif
#ifdef CFG_DBG_PINPAD
//        if ( IfInkey(0) ) {
//            exit_lowerpower_freq();
//            dbg_pinpad();
//            enter_lowerpower_freq();
//        }
#endif
#if 0//def DEBUG
        if (IfInkey(0)) {
            TRACE("\n99-reset 88-llwu 2-gWorkMode");
            i = InkeyCount(0);
            if (i == 99)
                NVIC_SystemReset();
            else if (i == 88) 
                enter_lowerpower_llwu();
            else if (i == 2){
                TRACE("\ngWorkMode");
                gWorkMode = InkeyCount(0);
            }
            TRACE("\nRun");
        }	
#endif
//        SelfCheck(1);
	}
    ctc_uart_close(); 
    dbg_s1000_ctrl();
}
#endif
