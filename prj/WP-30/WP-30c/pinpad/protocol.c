/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : protocol.c
 * bfief              : 
 * Author             : yehf()  
 * Email              : yehf@itep.com.cn
 * Version            : V0.00
 * Date               : 10/7/2014 4:21:49 PM
 * Description        : 
 *******************************************************************************/
#if 0
#include "wp30_ctrl.h"
#if 0   //defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
#ifdef CFG_DBG_PINPAD
#pragma message("|---->|^_^|->CFG_DBG_PINPAD<-|^_^|<----|")
#endif
extern void parse_v1protocol(uchar stx);
extern void SI_vSendRetStr(uchar ucRet, uint uiLen, uchar *ucBuff);
// PCI PROTOCOL
void PPRT_ManageDealWith(uint mode);
void PPRT_Debug0(uchar *pucData, ushort * pusLen);
void PPRT_SetConfig(uchar *pucData, ushort *pusLen);

#define ASSERT_APP_PCI()   do{if(gAppProcolType!=APP_PROTOCOL_PCI)return 1;}while(0)

// mode=0 TR31 KP ^ 45 = DESK  
// mode=1 TR31 KP ^ 4D = MACK
// mode=2 TR31 KP ^ 4D = DESK ^ 45 ^ 4D = DESK ^ 08 = MACK
void PPRT_GetTR31Key(uint mode, uint keylen, uchar *in, uchar *out)
{
	uint i;
	uchar tmp;
	if(mode==0)
		tmp = 0x45;  //Ke
	else if(mode==1)
		tmp = 0x4D;  //Km
	else 
		tmp = 0x08;
	for(i=0;i<keylen;i++)
	{
		out[i] = in[i]^tmp;
	}
}
// FSK:20+type+Algorithm+00(7B)+keylen_E+key_E(16B)+padding(7B)+Mac(4B)
// MK: 22+type+index+Algorithm+00(7B)+Keylen+Key(24B)+Padding(7B) +Mac(4B)
// WK: Type+index+Mkindex+Algorithm+00(7B)+Keylen+Key(24B)+Padding(7B)+Mac(4B)
//  -> keylen+key
// 0-OK
// 1-Mac Fail
uint ParseTR31KeyBlock(uchar *Kp, uint inlen,uchar *in, uint offset,uint keyblocklen, uchar *keylen, uchar *outkey)
{
	uchar key[24];
	uchar keyblock[8+24];
	uchar mac[4];
	uint ret;
	//descrypt keyblock
	PPRT_GetTR31Key(0, 24, Kp, key);  //Ke
	des_decrypt_ecb(&in[offset], keyblock, keyblocklen, key, 3);
	memcpy(&in[offset],keyblock,keyblocklen);
	PPRT_GetTR31Key(1, 24, Kp, key);  //Km
	cacul_mac_ecb(24,key,inlen-4,in,4,mac);
//    DISPBUF("KP", 24, 0, Kp);
//    DISPBUF("TR31", keyblocklen, 0, keyblock);
//    DISPBUF("MSG",  inlen-4, 0, in);
//    DISPBUF("MAC", 4, 0, mac);
	if(memcmp(mac,&in[offset+keyblocklen],4))
	{
		memset(in,0,inlen);
		ret =  1;
		goto ParseTR31Block_end;
	}
	*keylen = in[offset];
	memcpy(outkey,&in[offset+1],*keylen);	
	ret = 0;
ParseTR31Block_end:	
	CLRBUF(keyblock);
	CLRBUF(key);
	return ret;
}
// FSK:20+type+Algorithm+00(7B)+keylen_E+key_E(16B)+padding(7B)+Mac(4B)
// MK: 22+type+index+Algorithm+00(7B)+Keylen+Key(24B)+Padding(7B) +Mac(4B)
// WK: Type+index+Mkindex+Algorithm+00(7B)+Keylen+Key(24B)+Padding(7B)+Mac(4B)
//  -> keylen+key
// 0-OK
// 1-Mac Fail
uint ParseKeyBlock(uchar *Kp, uchar mode, uchar keylen,uchar *keyblock, uchar *mac, uchar *outkey)
{
	uchar key[24];
	uchar buf1[8],buf2[8];
	uint ret;
	des_decrypt_ecb(keyblock, key, keylen, Kp, 3);
	DISPBUF("KP", 24, 0, Kp);vDispBufTitle("KEY", keylen, 0, key);
	if(mode == 1)
	{
		CLRBUF(buf1);
		des_encrypt_ecb(buf1,buf2,8,key,keylen/8);
		DISPBUF("MAC", 4, 0, buf2);
		if(memcmp(mac,buf2,4))
		{
			ret =  1;
			goto ParseKeyBlock_end;
		}
	}		
	ret = 0;
	memcpy(outkey,key,keylen);
ParseKeyBlock_end:
	CLRBUF(key);
	return ret;
}

void PPRT_SendACK(uchar ack)
{
	UART_Write(&ack, 1);
}
// return data is not seq:  cmd + ucData
// ucData:gucBuff
uchar PPRT_Receive(uchar *ucCmd, uchar *ucData, ushort * usLen)
{
	uchar *buff, pNo = 1;
	ushort len, offset = 0, crc=0;
	uchar cmd = 0;
	int i;	
	buff = gucTmpBuf;
#if 0
	while (!UART_CheckReadbuf())
	{
		s_DelayMs(10);
		return EM_ERROR;
	}
#else	
	i = UART_CheckReadbuf();
	if(i <= 0)
	{
		return EM_ERROR;
	}
#endif
	while (1)
	{
		// STX
		i = UART_Read(&buff[0], 1, 1000);	
		if(!i)
		{
			return EM_ERROR;
		}
#ifdef DEBUG_Dx
#ifdef CFG_DBG_PINPAD
        extern char gDebugFlg;
        if ( gDebugFlg ) {
                lcd_Display(0,DISP_FONT*2,DISP_FONT|DISP_CLRLINE,"%d %d %d %d %d",gDebugFlg,gtCurKey.BeginTime,gtCurKey.ShakehandBeginTime,gtCurKey.PinLimitTime,sys_get_counter()); 	
                if ( gDebugFlg >= 2 ) {
                    if(gtCurKey.appid)
                    {
                        lcd_Display(0,DISP_FONT*3,DISP_FONT|DISP_CLRLINE,"%d-%d-%d-%d-%d",gtCurKey.DlAuthPinkPerHs,gtCurKey.DlAuthDeskPerHs,gtCurKey.DlAuthMackPerHs,gtCurKey.DlAuthMKPerHs,gtCurKey.DlMKPerHs); 
                        lcd_Display(0,DISP_FONT*4,DISP_FONT|DISP_CLRLINE,"%d %d-%d-%d %d-%d-%d ",gtCurKey.HsFlag,gtCurKey.AuthType,gtCurKey.AuthIndex,gtCurKey.AuthFlag,
                                    gtCurKey.MKIndex,gtCurKey.WKType,gtCurKey.AuthIndex);
                    }	
            }
        }
#endif
#endif
		if(buff[0] != STX)
		{
			//DISPERR(buff[0]);
			//PPRT_SendACK(ACK_ERR);
		#ifdef CFG_CMD_V1
			parse_v1protocol(buff[0]);
			LCD_LightOn();
		#endif
			return EM_ERROR;
		}
		// Len:cmd+data+crc16
		i = UART_Read(&buff[1], 1, 1000);
		if(!i)
		{		
			//PPRT_SendACK(ACK_ERR);
			//DISPERR(0);
			return EM_ERROR;
		}		
		len = (ushort) buff[1];
		// read data
		i = UART_Read(buff + 2, len, 2000);
		if (i != len)
		{
		#ifdef EM_DEBUG
			TRACE("i:%d",i);
			vDispBuf(i, 0, buff+2);			
		#endif	
			DISPERR(len);
			//PPRT_SendACK(0x11);	
			PPRT_SendACK(ACK_ERR);			
			return EM_ERROR;
		}

		if (buff[len - 1] != FRAME_STAT_END && buff[len - 1] != FRAME_STAT_NEXT)
		{
			DISPERR(buff[len - 1]);
			//PPRT_SendACK(0x22);	
			PPRT_SendACK(ACK_ERR);		
			return EM_ERROR;
		}
		//check: 02+len+data
		crc = crc_calculate16by8(buff, len);
    #ifndef DEBUG_Dx
		if (memcmp((uchar *) & crc, buff + len, 2))
		{
            DISPERR(crc);
//            PPRT_SendACK(0x33);	
			PPRT_SendACK(ACK_ERR);
//			//TRACE("%02x%02x",buff[len+1],buff[len]);		
			return EM_ERROR;
		}
    #else
//        TRACE("CRC:%02X %02X", LBYTE(crc),HBYTE(crc));
	#endif

		if ((offset + len - FRAME_FLAG_LEN) > (1028+8))
		{
			//DISPERR(offset + len - FRAME_FLAG_LEN);
			//PPRT_SendACK(0x44);	
			PPRT_SendACK(ACK_ERR);
			return EM_ERROR;
		}
		*ucCmd = buff[FRAME_CMD_OFF];
		memcpy(ucData + offset, buff + FRAME_CMD_OFF + 2, len - FRAME_FLAG_LEN);
		offset += (len - FRAME_FLAG_LEN);
		// the end of the frame?
		if (buff[len - 1] == FRAME_STAT_END)
		{
			if (buff[FRAME_CMD_SEQ] == 1)
			{
				// 只有1包
				PPRT_SendACK(ACK_OK);
				break;
			}
			else
			{
				// 多包
				if (buff[FRAME_CMD_SEQ] == pNo + 1 && cmd == buff[FRAME_CMD_OFF])
				{
					PPRT_SendACK(ACK_OK);
					break;
				}
				else
				{
					DISPERR(buff[FRAME_CMD_OFF]);
					DISPERR(buff[FRAME_CMD_SEQ]);
					//PPRT_SendACK(0x55);	
					PPRT_SendACK(ACK_ERR);					
					return EM_ERROR;
				}
			}
		}
		if (pNo == buff[FRAME_CMD_SEQ])
		{
			//命令
			cmd = buff[FRAME_CMD_OFF];
		}
		else if ((pNo + 1 == buff[FRAME_CMD_SEQ]) && (cmd == buff[FRAME_CMD_OFF]))
		{
			pNo = buff[FRAME_CMD_SEQ];
		}
		else
		{
			DISPERR(pNo);
			//PPRT_SendACK(0x66);	
			PPRT_SendACK(ACK_ERR);			
			return EM_ERROR;
		}	
		PPRT_SendACK(ACK_OK);	
	}
	
	*usLen = offset;
//    DISPBUF("cmd",offset,0,ucData);
	return EM_SUCCESS;

}

uchar PPRT_Send(uchar cmd, uchar *ucData, ushort usLen)
{
	uchar buff[256+2];
	uchar num = 1;
	uchar flag = 1;	
	ushort len = usLen, packLen, crc;	
	int offset = 0;
    if ( gInPinEntry == 1 ) {
        //V1协议
        if(cmd == CMD_KEY || (cmd >= CMD_LCD_DISP && cmd <= CMD_BEEP)) {
            buff[0] = V1_CMD_IO;
            buff[1] = cmd;
            memcpy(&buff[2],ucData,usLen);
            len = usLen + 2;
            SI_vSendRetStr(0xFE,len,buff);
            return 0;
        }
    }
//    DISPBUF("ACK",usLen,0,ucData);
	if(usLen == 0)
	{
		return 0;
	}
	while (flag)
	{
		memset(buff, 0, sizeof(buff));
		buff[0] = STX;
		if (len > SEND_PACK_LEN)
		{
			packLen = SEND_PACK_LEN + FRAME_FLAG_LEN;

			buff[FRAME_LEN_OFF] = packLen;
			len = len - SEND_PACK_LEN;
			buff[packLen - 1] = FRAME_STAT_NEXT;
		}
		else
		{
			packLen = len + FRAME_FLAG_LEN;
			buff[FRAME_LEN_OFF] = packLen;
			buff[packLen - 1] = FRAME_STAT_END;
			flag = 0;
		}
		buff[FRAME_CMD_OFF] = cmd;
		buff[FRAME_CMD_OFF + 1] = num++;
		memcpy(buff + FRAME_CMD_OFF + 2, ucData + offset, packLen - FRAME_FLAG_LEN);

		offset += packLen - FRAME_FLAG_LEN;

		crc = crc_calculate16by8(buff, packLen);
		memcpy(buff + packLen, (uchar *) & crc, 2);
		//buff[packLen + 1]  = MakeLrc(buff, packLen + 1);
		UART_Write(buff, packLen + 2);
		// read ACK
		if(cmd <= CMD_DELAY || cmd == CMD_READ_MAGCARD)
		{
			//不需要应答
		}
		else
		{
			if (UART_Read(buff, 1, 2000) != 1)
			{
				return EM_ERROR;
			}
			if (buff[0] != 0x06)
			{
				return EM_ERROR;
			}
		}	
	}
	return EM_SUCCESS;
}


void PPRT_ClearDLHs(uint mode)
{
	if(mode == 0)
	{
		gtCurKey.HsFlag = HS_STAT_NOT;
	}
	gtCurKey.DlAuthMKPerHs = 0;
	gtCurKey.DlMKPerHs = 0;
	gtCurKey.DlAuthPinkPerHs = 0;
	gtCurKey.DlAuthMackPerHs = 0;
	gtCurKey.DlAuthDeskPerHs = 0;	
	RNG_FillRandom(gtCurKey.aucRand, 4*2);  
}

// mode=MODE_CTRLMANAGE or MODE_APP
uchar PPRT_HandShake(uchar * pucData, ushort * pusLen)
{
	uchar aucTemp[16];
	uchar hsk[8];
	//uchar stat;
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[1] = RESULT_APP_TYPEERR;
		goto PPRT_HandShake_Err;
	}
	switch (*pucData)
	{
	case 0x00:
		//recv: 00 + T_Rand(8B)
		//send: 01 + T_Rand_E(8B) + P_Rand(8B) + 16B Data		
		if(*pusLen != 9)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_HandShake_Err;
		}		
		PCI_GetHSK(0, hsk);
		des_encrypt_ecb(pucData + 1, aucTemp, 8, hsk, 1);
		memcpy(pucData+1, aucTemp, 8);
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
		return EM_SUCCESS;
		
	case 0x02:
		//recv: 02 + Stat + P_Rand_E(8B)
		//send: 03 + Stat 
		if(*pusLen != 10)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_HandShake_Err;
		}
		if (pucData[1] != 0)
		{
			DISPERR(pucData[1]);
			pucData[1] = RESULT_ERROR;
			goto PPRT_HandShake_Err;
		}
		if (gtCurKey.HsFlag != HS_STAT_ING)
		{
			DISPERR(gtCurKey.HsFlag);
			pucData[1] = RESULT_ERROR;
			goto PPRT_HandShake_Err;
		}
		PCI_GetHSK(0, hsk);
		des_encrypt_ecb(gtCurKey.aucRand, aucTemp, 8, hsk, 1);
		if (!memcmp(pucData + 2, aucTemp, 8))
		{
			pucData[0] = 0x03;
			pucData[1] = RESULT_SUCCESS;
			gtCurKey.HsFlag = HS_STAT_SUC;
			gtCurKey.ShakehandBeginTime = sys_get_counter();
			*pusLen = 2;			
			PPRT_ClearDLHs(1);
			return EM_SUCCESS;
		}
		else
		{
			DISPERR(0);
			pucData[1] = RESULT_ERROR;
			goto PPRT_HandShake_Err;
		}		
	default:
		pucData[1] = RESULT_NOT_PERMIT;
		break;		
	}
PPRT_HandShake_Err:
	pucData[0] += 1;
	PPRT_ClearDLHs(0);
	*pusLen = 2;
	return EM_ERROR;
}

// 多应用管理
// 0-
// 1-长度固定2字节 
uchar PPRT_AppManage(uchar *pucData, ushort *pusLen)
{
	uchar aucTemp[48];
	uchar ptk[24];
	uint i,j;
	switch (*pucData)
	{	
	case REQ_APP_OPEN:
		// recv 9B:  01+APPNAME(8B,APP1~APP4)
		// send 10B: 02+Stat+Rand(8B)
		if(*pusLen != 9)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_AppManage_Open;
		}
		if(gWorkMode < MODE_CTRLMANAGE || gWorkMode > MODE_APP)
		{
			pucData[1] = RESULT_NOT_PERMIT;
			goto PPRT_AppManage_Open;
		}	
		if(gWorkMode > MODE_CTRLMANAGE)
		{
			gWorkMode = MODE_IDLE;
		}
		gtCurKey.appid = 0;
		gAppAuthFlag = STAT_APPAUTH_NOT;
		gReadyAppid = app_find(pucData+1);
		if(!gReadyAppid)
		{
			pucData[1] = RESULT_APP_NOTEXIST;
			goto PPRT_AppManage_Open;
		}
		if(app_get_type(gReadyAppid) != APP_PROTOCOL_PCI)
		{
			pucData[1] = RESULT_APP_TYPEERR;
			goto PPRT_AppManage_Open;
		}
	#if 2	
		RNG_FillRandom(gtCurKey.aucRand,4*2);  
	#else
		memset(gtCurKey.aucRand,0x01,8);
	#endif
		pucData[1] = RESULT_SUCCESS;
		memcpy(&pucData[2],gtCurKey.aucRand,8);
		*pusLen = 2+8;
		gAppAuthFlag = STAT_APPAUTH_ING;
	PPRT_AppManage_Open:	
		gWorkMode = MODE_IDLE;
		if(pucData[1])
		{
			*pusLen = 2;			
		}
		pucData[0] = ACK_APP_OPEN;	
		return 0;
		
	case REQ_APP_AUTH:		
		// recv 9B:  03+Rand_E(8B)
		// send 2B:  04+Stat
		if(*pusLen != 9)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_AppManage_Auth;
		}
		if(gWorkMode != MODE_IDLE)
		{
			pucData[1] = RESULT_NOT_PERMIT;
			goto PPRT_AppManage_Auth;
		}		
		if(gAppAuthFlag != STAT_APPAUTH_ING)
		{
			pucData[1] = RESULT_AUTH_ERR;
			goto PPRT_AppManage_Auth;
		}	
		app_set_APPEK(gReadyAppid,1);
		app_read_info(gReadyAppid,INFO_APPK,&i, aucTemp);
		app_set_APPEK(gReadyAppid,0);	
		des_decrypt_ecb(pucData+1, pucData+1+8, 8, aucTemp, MK_UNIT_LEN/8);				
		if (!memcmp(pucData+1+8, gtCurKey.aucRand, 8))
		{
			//app auth is ok
			if(app_open(gReadyAppid))
			{
				pucData[1] = RESULT_APP_LOCKED;
				goto PPRT_AppManage_Auth;
			}
			pucData[1] = RESULT_SUCCESS;
		}
		else
		{
			pucData[1] = RESULT_AUTH_ERR;
		}	
	PPRT_AppManage_Auth:
		RNG_FillRandom(gtCurKey.aucRand,4*2);  
		pucData[0] = ACK_APP_AUTH;
		*pusLen = 2;
		return 0;
		
	case REQ_APP_CREAT:	
		//               0  1           9           17         33
		// 37B  pucData: 05+AppName(8B)+8B(Resverd)+16B(APPAK)+4B(MAC)
		// 2B   pucData: 06+stat
		if(*pusLen != 37)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_AppManage_Create;
		}
		if(gWorkMode < MODE_CTRLMANAGE || gWorkMode > MODE_IDLE)
		{
			pucData[1] = RESULT_NOT_PERMIT;
			goto PPRT_AppManage_Create;
		}
        if (PKS_CheckPbulicKey())
        {
			pucData[1] = RESULT_ERROR;
			goto PPRT_AppManage_Create;
        }
		i = app_find(pucData+1);
		if(i)
		{
			pucData[1] = RESULT_APP_EXIST;
			goto PPRT_AppManage_Create;
		}
		i = app_find_newapp();
		if(!i)
		{
			pucData[1] = RESULT_APP_OVERFLOW;
			goto PPRT_AppManage_Create;
		}
		//copy ciphertext APPAK
		memcpy(aucTemp,pucData+1+8+8,MK_UNIT_LEN);
		//read default PTK
		read_default_tranferkey(ptk);
        TRACE_BUF("ptk(trsk)",ptk,24);
        TRACE_BUF("ciphertext",aucTemp,16);
		//decrypt APPAK
		des_decrypt_ecb(aucTemp,pucData+1+8+8,MK_UNIT_LEN,ptk,3);
		TRACE_BUF("plaintext",pucData+1+8+8,16);
		//check mac
		cacul_mac_ecb(24,ptk,16,pucData+17,4,aucTemp);
        TRACE_BUF("mac",aucTemp,4);
		CLRBUF(ptk);
		if(memcmp(aucTemp,pucData+33,4))
		{
			pucData[1] = RESULT_ERROR_MACK;
			goto PPRT_AppManage_Create;
		}		
		// copy appname
		memcpy(aucTemp,pucData+1,LEN_APP_APPNAME);
		// cooy default psw
		memset(aucTemp+LEN_APP_APPNAME,'1',PSW_LEN);
		memset(aucTemp+LEN_APP_APPNAME+PSW_LEN,'2',PSW_LEN);
		// copy APPAK
		memcpy(aucTemp+LEN_APP_APPNAME+2*PSW_LEN,pucData+17,MK_UNIT_LEN);
		if(app_create(i,INFO_APPNAME|INFO_PSW|INFO_APPK|(1<<4)|APP_TYPE_PCI,aucTemp))
		{
			pucData[1] = RESULT_ERROR_FLASH;
			goto PPRT_AppManage_Create;
		}
		pucData[1] = RESULT_SUCCESS;	
	PPRT_AppManage_Create:
		pucData[0] = ACK_APP_CREAT;
		*pusLen = 2;
		return 0;
									
	case REQ_APP_DEL:
		//               0  1           9    10
		// 18B  pucData: 07+AppName(8B)+mode+Check(8B)
		// 2B   pucData: 08+stat
		if(*pusLen != 18 || pucData[9] == 0 || pucData[9] > 2)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_AppManage_Del;
		}
		if(gWorkMode < MODE_CTRLMANAGE || gWorkMode > MODE_IDLE)
		{
			pucData[1] = RESULT_NOT_PERMIT;
			goto PPRT_AppManage_Del;
		}
		i = app_find(pucData+1);
		if(!i)
		{
			pucData[1] = RESULT_APP_NOTEXIST;
			goto PPRT_AppManage_Del;
		}
		if(app_get_type(i) != APP_PROTOCOL_PCI)
		{
			pucData[1] = RESULT_APP_TYPEERR;
			goto PPRT_AppManage_Open;
		}
		if(pucData[9] == 1)
		{
			app_set_APPEK(i,1);
			app_read_info(i,INFO_APPK,&j, &aucTemp[16]);
			app_set_APPEK(i,0);
			des_encrypt_ecb(gtCurKey.aucRand,aucTemp,8,&aucTemp[16],MK_UNIT_LEN/8);
			if(memcmp(aucTemp,pucData+10,8))
			{
				pucData[1] = RESULT_AUTH_ERR;
				goto PPRT_AppManage_Del;
			}
		}
		else if(pucData[9] == 2)
		{
			//aucTemp   0~7:mac  8~15:APPAK  16~23:zero
			memset(&aucTemp[8],0,8);
			app_set_APPEK(i,1);
			app_read_info(i,INFO_APPK,&j, &aucTemp[16]);
			app_set_APPEK(i,0);
			des_encrypt_ecb(&aucTemp[8],aucTemp,8,&aucTemp[16],MK_UNIT_LEN/8);
			if(memcmp(aucTemp,pucData+10,8))
			{
				pucData[1] = RESULT_AUTH_ERR;
				goto PPRT_AppManage_Del;
			}
		}
		j = app_del(i);
		if(j)
		{
			pucData[1] = RESULT_ERROR_FLASH;
			goto PPRT_AppManage_Del;
		}
		pucData[1] = RESULT_SUCCESS;	
	PPRT_AppManage_Del:
		pucData[0] = ACK_APP_DEL;
		*pusLen = 2;
		RNG_FillRandom(gtCurKey.aucRand,4*2);  
		return 0;	
	default:
		pucData[1] = RESULT_NOT_PERMIT;
		*pusLen = 2;
		return 0;
	}
}

uchar PPRT_GetSn(uchar * pucData, ushort *pusLen)
{
	uchar sn[32];
	uchar snlen;
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[0] = RESULT_APP_TYPEERR;
		goto PPRT_GetSn_over;
	}
	
	if(read_sn(gtCurKey.appid,&snlen,sn))
	{
		pucData[0] = RESULT_KEY_ERROR;
		goto PPRT_GetSn_over;
	}
	memcpy(pucData+1,sn,snlen);
	pucData[0] = RESULT_SUCCESS;
	*pusLen = snlen+1;
PPRT_GetSn_over:
	if(pucData[0])
	{
		*pusLen = 1;
	}	
	return EM_SUCCESS;
}
uchar PPRT_SetSn(uchar *pucData, ushort *pusLen)
{
	// len(1B)+sn
	uchar snlen;
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[0] = RESULT_APP_TYPEERR;
		goto PPRT_SetSn_over;
	}
	snlen = *pucData;
	if(snlen < 4 || snlen > 24 || *pusLen != (snlen+1))
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_SetSn_over;
	}
	if(write_sn(gtCurKey.appid,snlen,pucData+1))
	{
		pucData[0] = RESULT_ERROR_FLASH;
		goto PPRT_SetSn_over;
	}
	pucData[0] = RESULT_SUCCESS;
PPRT_SetSn_over:	
	*pusLen = 1;	
	return 0;
}
// 0-OK 1-error
uint check_keyindex(uchar type, uchar index)
{
	uint i=gtCurKey.apptype;
    TRACE("\nkeyindex:%d-%x-%x",i,type,index);
	if (type == KEY_TYPE_AUTHPINK || type == KEY_TYPE_PINK)
	{
		if (index >= NUM_AUTHPINK*i)
		{
			return 1;
		}
	}
	else if (type == KEY_TYPE_AUTHMACK || type == KEY_TYPE_MACK)
	{
		if (index >= NUM_AUTHMACK*i)
		{
			return 1;
		}
	}
	else if (type == KEY_TYPE_AUTHDESK || type == KEY_TYPE_DESK)
	{
		if (index >= NUM_AUTHDESK*i)
		{
			return 1;
		}
	}
	else if (type == KEY_TYPE_AUTHMK || type == KEY_TYPE_MK)
	{
		if (index >= NUM_AUTHMK*i)
		{
			return 1;
		}
	}
	else 
	{
		return 1;
	}
	return 0;
}

uchar PPRT_DownloadPTK(uchar *pucData, ushort * pusLen)
{
extern uint Extendcmd_GetLoadMode(uchar mode, uchar *encmode, uchar *checklen, uchar *checkalg);
extern uint LoadKey_Check(uchar keylen, uchar *key,uchar checklen, uchar *check);

	uchar encmode,checklen,checkalg,keylen;
	uchar *tpk,*tmp;
	
	//          0  1        2         3        4        5          6
	//send(33B):24+type(1B)+index(1B)+mode(1B)+预留(1B)+keylen(1B)+key+check(0/4/8B)+RFU(8B)
	//                             
	//recv:24+00(成功)/01(失败) 
	if(gWorkMode != MODE_APP)
	{
		pucData[1] = RESULT_APP_NOTRUN;
		return 1;
	}
	//固定01
	keylen = pucData[5];
	if(pucData[1]!=1 || pucData[2]!=0 || keylen!=24)
	{
		pucData[1] = RESULT_ERROR_PARA;
		return 1;
	}
	if(Extendcmd_GetLoadMode(pucData[3],&encmode,&checklen,&checkalg))
	{
		pucData[1] = RESULT_ERROR_PARA;
		return 1;
	}
	tpk = pucData + 64;
	tmp = pucData + 64 + 24;
	
	if(IFBIT(pucData[3], 0))
	{
		//PTK加密
		if(encmode != 1)
		{
			pucData[1] = RESULT_ERROR_PARA;
			return 1;
		}
		//用旧的TPK密钥加密
		if(read_transferkey(gtCurKey.appid,tpk))
		{
			pucData[1] = (RESULT_KEY_ERROR);
			return 1;
		}
		// tmp:plaintext key  key:transter key
		des_decrypt_ecb(&pucData[6],tmp,24,tpk,3);
	#if 2
		vDispBufTitle("KEY",24,0,tpk);
		vDispBufTitle("PTK",24,0,tmp);
	#endif	
	}
	else
	{
		memcpy(tmp,&pucData[6],24);
	}
	if(checklen)
	{
		if(LoadKey_Check(24,tmp,checklen,&pucData[6+keylen]))
		{
			memset(tpk,0,48);
			pucData[1] = RESULT_ERROR_MACK;
			return 1;
		}
	}
	
	if(write_transferkey(gtCurKey.appid,tmp))
	{
		memset(tpk,0,48);
		pucData[1] = RESULT_ERROR_FLASH;
		return 1;
	}
	memset(tpk,0,48);
	pucData[1] = 0;
	return 0;
}
// workmode=MODE_CTRLMANAGE  DL FSK
// workmode=APP              DL MK/AUTHKEY
uchar PPRT_DownLoadAuthAndMasterKey(uchar * pucData, ushort * pusLen)
{
	uchar keyblock[24],ptk[24];
	uchar keylen,cmd;
	
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[1] = RESULT_APP_TYPEERR;
		goto PPRT_DownLoadAuthAndMasterKey_Over;
	}
	cmd = pucData[0];
	switch (cmd)
	{
	case 0x20:
	    //          0  1    2         3      10       11         27          34          
		//send(38B):20+type+Algorithm+00(7B)+keylen_E+key_E(16B)+padding(7B)+Mac(4B)
		//                             
		//recv:21+00(成功)/01(失败) 
		if(pucData[1] > FSK_APPAK || *pusLen != 38 || pucData[2] > 2)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_DownLoadFSK_Err;
		}
		
		if((pucData[1] == FSK_SMAPP)  
			|| (pucData[1] == FSK_CMCTRL && gWorkMode != MODE_INIT)
			|| (pucData[1] == FSK_CMAPP && gWorkMode != MODE_CTRLMANAGE)
			|| (pucData[1] == FSK_APPAK && gWorkMode != MODE_APP))
		{
			pucData[1] = RESULT_NOT_PERMIT;
			goto PPRT_DownLoadFSK_Err;
		}
		if (gtCurKey.HsFlag != HS_STAT_SUC)
		{
			pucData[1] = RESULT_NOT_HANDSHAKE;
			goto PPRT_DownLoadFSK_Err;
		}		
		if ((sys_get_counter() - gtCurKey.ShakehandBeginTime) > HS_DL_TIMEOUT)
		{
			pucData[1] = RESULT_TIMEOUT_ERR;
			goto PPRT_DownLoadFSK_Err;
		}	
		if(read_transferkey(gtCurKey.appid,ptk))
		{
			pucData[1] = RESULT_KEY_ERROR;
			goto PPRT_DownLoadFSK_Err;
		}
		if(pucData[2] == 0)
		{
			if(ParseTR31KeyBlock(ptk, *pusLen, pucData, 10, 16+8, &keylen, keyblock))
			{
				pucData[1] = RESULT_ERROR_MACK;
				goto PPRT_DownLoadFSK_Err;
			}
			if(keylen != 16)
			{
				pucData[1] = RESULT_ERROR_PARA;
				goto PPRT_DownLoadFSK_Err;
			}
		}
		else
		{
			keylen = pucData[10];
			if(keylen != 16)
			{
				pucData[1] = RESULT_ERROR_PARA;
				goto PPRT_DownLoadFSK_Err;
			}
			if(ParseKeyBlock(ptk, pucData[2], keylen, &pucData[11], &pucData[34], keyblock))
			{
				pucData[1] = RESULT_ERROR_MACK;
				goto PPRT_DownLoadFSK_Err;
			}
		}
		
		if(PCI_SaveFSK(pucData[1],keyblock))
		{
			pucData[1] = RESULT_ERROR_FLASH;
			goto PPRT_DownLoadFSK_Err;
		}
		pucData[1] = RESULT_SUCCESS;
	PPRT_DownLoadFSK_Err:
		break;
		
	case 0x22:
		//           0  1    2     3         4      11     12       36           43
		// recv(47B):22+type+index+Algorithm+00(7B)+Keylen+Key(24B)+Padding(7B) +Mac(4B)
		// send(2B) :23+Stat	
		if(*pusLen != 47 || pucData[1] < KEY_TYPE_AUTHMK || pucData[1] > KEY_TYPE_MK
			|| pucData[3] > 2)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_DownLoadAuthAndMasterKey_Err;
		}
		if(check_keyindex(pucData[1],pucData[2]))
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_DownLoadAuthAndMasterKey_Err;
		}
		if(gWorkMode != MODE_APP)
		{
			pucData[1] = RESULT_APP_NOTRUN;
			DISPERR(gWorkMode);
			goto PPRT_DownLoadAuthAndMasterKey_Err;
		}		
		if (gtCurKey.HsFlag != HS_STAT_SUC)
		{
			pucData[1] = RESULT_NOT_HANDSHAKE;
			DISPERR(gtCurKey.HsFlag);
			goto PPRT_DownLoadAuthAndMasterKey_Err;
		}
		if ((sys_get_counter() - gtCurKey.ShakehandBeginTime) > HS_DL_TIMEOUT)
		{
			pucData[1] = RESULT_TIMEOUT_ERR;
			goto PPRT_DownLoadAuthAndMasterKey_Err;
		}
		if (pucData[1] == KEY_TYPE_AUTHPINK)
		{
			if (++gtCurKey.DlAuthPinkPerHs> NUM_AUTHPINK*gtCurKey.apptype)
			{
				pucData[1] = RESULT_DL_EXCEED;
				goto PPRT_DownLoadAuthAndMasterKey_Err;
			}
		}
		else if (pucData[1] == KEY_TYPE_AUTHMACK)
		{
			if (++gtCurKey.DlAuthMackPerHs > NUM_AUTHMACK*gtCurKey.apptype)
			{
				pucData[1] = RESULT_DL_EXCEED;
				goto PPRT_DownLoadAuthAndMasterKey_Err;
			}
		}
		else if (pucData[1] == KEY_TYPE_AUTHDESK)
		{
			if (++gtCurKey.DlAuthDeskPerHs > NUM_AUTHDESK*gtCurKey.apptype)
			{
				pucData[1] = RESULT_DL_EXCEED;
				goto PPRT_DownLoadAuthAndMasterKey_Err;
			}
		}
		else if (pucData[1] == KEY_TYPE_AUTHMK)
		{
			if (++gtCurKey.DlAuthMKPerHs > NUM_AUTHMK*gtCurKey.apptype)
			{
				pucData[1] = RESULT_DL_EXCEED;
				goto PPRT_DownLoadAuthAndMasterKey_Err;
			}
		}
		else
		{
			if (++gtCurKey.DlMKPerHs > NUM_MK*gtCurKey.apptype)
			{
				pucData[1] = RESULT_DL_EXCEED;
				goto PPRT_DownLoadAuthAndMasterKey_Err;
			}
		}
		if(read_transferkey(gtCurKey.appid,ptk))
		{
			pucData[1] = RESULT_KEY_ERROR;
			goto PPRT_DownLoadAuthAndMasterKey_Err;
		}
		if(pucData[3] == 0)
		{
			if(ParseTR31KeyBlock(ptk, *pusLen, pucData, 11, 24+8, &keylen, keyblock))
			{
				pucData[1] = RESULT_ERROR_MACK;
				goto PPRT_DownLoadAuthAndMasterKey_Err;
			}
			if(keylen != 24)
			{
				pucData[1] = RESULT_ERROR_PARA;
				goto PPRT_DownLoadAuthAndMasterKey_Err;
			}
		}
		else
		{
			keylen = pucData[11];
			if(keylen != 24)
			{
				pucData[1] = RESULT_ERROR_PARA;
				goto PPRT_DownLoadAuthAndMasterKey_Err;
			}
			if(ParseKeyBlock(ptk, pucData[3], keylen, &pucData[12], &pucData[43], keyblock))
			{
				pucData[1] = RESULT_ERROR_MACK;
				goto PPRT_DownLoadAuthAndMasterKey_Err;
			}
		}
		
		if (PCI_WriteCurrentKey(pucData[1], pucData[2], keylen, keyblock))
		{
			// write flash err
			pucData[1] = RESULT_ERROR_FLASH;
			goto PPRT_DownLoadAuthAndMasterKey_Err;
		}
		else
		{
			pucData[1] = RESULT_SUCCESS;
		}
	PPRT_DownLoadAuthAndMasterKey_Err:
		break;
	case 0x24:		
		PPRT_DownloadPTK(pucData, pusLen);
		break;
	default:
		pucData[1] = RESULT_NOT_PERMIT;
		break;		
	}
PPRT_DownLoadAuthAndMasterKey_Over:	
	CLRBUF(ptk);
	if(pucData[1])
	{
		PPRT_ClearDLHs(0);	
	}	
	if(cmd == 0x20)
	{
		pucData[0] = 0x21;
	}
	else if(cmd == 0x22)
	{
		pucData[0] = 0x23;
	}
	else if(cmd == 0x24)
	{
		pucData[0] = 0x25;
	}
	else
	{
		pucData[0] = cmd;
	}
	*pusLen = 2;
	return 0;
}

uchar PPRT_GetRandom(uchar * pucData, ushort * pusLen)
{
    ushort len;
    /*
       if(gAppProcolType!=APP_PROTOCOL_PCI)
       {
       pucData[0] = RESULT_APP_TYPEERR;
     *pusLen = 1;
     return EM_ERROR;
     }
     */
    // recv(2B):LenL+LenH
    len = COMBINE16(pucData[1],pucData[0]);
    if(*pusLen != 2 || len == 0 || len > 1024)
    {
        pucData[0] = RESULT_ERROR_PARA;
        *pusLen = 1;
        return EM_ERROR;
    }	
    RNG_FillRandom(pucData+1, len);
    /*
       if(len == 8)
       {
       memcpy(gtCurKey.aucRand,pucData+1,8);
       }
       */
    pucData[0] = RESULT_SUCCESS;
    *pusLen = 1+len;
    return EM_SUCCESS;
}

uchar PPRT_AuthKey(uchar * pucData, ushort * pusLen)
{
	uchar cmd;
	uchar len;
	uchar aucMac[8], key[24];
	cmd = *pucData;
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[1] = RESULT_APP_TYPEERR;
		goto PPRT_AuthKeyErr;
	}
	switch (cmd)
	{
	case 0x00:
		//recv:00+auth_key_type(1B)+index(1B)
		//send:01+T_Rand(8B)
		if(*pusLen != 3)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_AuthKeyErr;
		}
		if(pucData[1] < KEY_TYPE_AUTHMK || pucData[1] > KEY_TYPE_AUTHDESK)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_AuthKeyErr;
		}
		if(check_keyindex(pucData[1],pucData[2]))
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_AuthKeyErr;
		}
		
		gtCurKey.AuthType = pucData[1];
		gtCurKey.AuthIndex = pucData[2];
		RNG_FillRandom(gtCurKey.aucRand, 4*2);
		memcpy(pucData + 1, (uchar *) & gtCurKey.aucRand[0], 8);
		*pusLen = 9;
		gtCurKey.AuthFlag = STAT_AUTH_ING;
		pucData[0] = 0x01;
		return EM_SUCCESS;
		
	case 0x02:
		//recv:02+T_Rand_E(8B)
		//send:03+Stat(1B)
		if(*pusLen != 9)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_AuthKeyErr;
		}
		if (gtCurKey.AuthFlag != STAT_AUTH_ING)
		{
			pucData[1] = RESULT_AUTH_ERR;
			PCI_WriteAuthErrTimes(gtCurKey.appid,1);
			goto PPRT_AuthKeyErr;
		}
		if (PCI_ReadCurrentKey(gtCurKey.AuthType, gtCurKey.AuthIndex, &len, key))
		{
			pucData[1] = RESULT_KEY_ERROR;
			goto PPRT_AuthKeyErr;
		}
		des_encrypt_ecb(gtCurKey.aucRand, aucMac, 8, key, len / 8);
		CLRBUF(key);
		if (memcmp(aucMac, pucData + 1, 8))
		{
			PCI_WriteAuthErrTimes(gtCurKey.appid,1);
			pucData[1] = RESULT_AUTH_ERR;
			goto PPRT_AuthKeyErr;
		}
		gtCurKey.AuthFlag = STAT_AUTH_SUS;
		pucData[1] = RESULT_SUCCESS;
		*pusLen = 2;
		if (gtCurKey.AuthType == KEY_TYPE_AUTHMK)
		{
			//认证成功设置主密钥
			gtCurKey.MKIndex = gtCurKey.AuthIndex + 1;
		}
		gtCurKey.BeginTime = sys_get_counter();
		PCI_WriteAuthErrTimes(gtCurKey.appid,0);
		pucData[0] = 0x03;
		RNG_FillRandom(gtCurKey.aucRand,4*2); 
		return EM_SUCCESS;
	default:
		pucData[1] = RESULT_NOT_PERMIT;
		break;
	}
	
PPRT_AuthKeyErr:
	RNG_FillRandom(gtCurKey.aucRand,4*2); 
	*pusLen = 2;
	if(pucData[0] == 0)
	{
		pucData[0] = 1;
	}
	else if(pucData[0] == 2)
	{
		pucData[0] = 3;
	}
	gtCurKey.AuthFlag = STAT_AUTH_NOT;
	gtCurKey.AuthType = 0;
	gtCurKey.AuthIndex = 0;
	return 0;
}

uchar PPRT_DownLoadWorkKey(uchar * pucData, ushort * pusLen)
{
	uchar mk[24], keyblock[24];
	uchar type,keylen;
	uint index, MKIndex;
	//          0    1     2       3         4      11     12       36          43
	//recv(47B):Type+index+Mkindex+Algorithm+00(7B)+Keylen+Key(24B)+Padding(7B)+Mac(4B)
	//send(1B):  Stat(1B)	
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[0] = RESULT_APP_TYPEERR;
		goto PPRT_DownLoadWorkKey_Err;
	}
	type = pucData[0];
	if(*pusLen != 47 || type < KEY_TYPE_PINK || type > KEY_TYPE_DESK)
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_DownLoadWorkKey_Err;
	}		
	index = pucData[1];
	MKIndex = pucData[2];	
	if(check_keyindex(type, index) !=0 
		|| check_keyindex(KEY_TYPE_MK, MKIndex) != 0)
	{
		pucData[0] = RESULT_ERROR_PARA;		
		goto PPRT_DownLoadWorkKey_Err;
	}
#if 2	
	if (gtCurKey.MKIndex == 0 || gtCurKey.AuthFlag != STAT_AUTH_SUS)
	{
		pucData[0] = RESULT_NOT_AUTH;
		goto PPRT_DownLoadWorkKey_Err;
	}
	if ((sys_get_counter() - gtCurKey.BeginTime) > AUTH_DL_TIMEOUT)
	{
		pucData[0] = RESULT_TIMEOUT_ERR;
		goto PPRT_DownLoadWorkKey_Err;
	}
	if (gtCurKey.AuthType != (type-4))
	{
		pucData[0] = RESULT_AUTHKEY_TYPEERR;
		PCI_WriteAuthErrTimes(gtCurKey.appid,1);
		goto PPRT_DownLoadWorkKey_Err;
	}
	if ((gtCurKey.MKIndex - 1) != MKIndex || gtCurKey.AuthIndex != index)
	{
		pucData[0] = RESULT_AUTHKEY_DIFF;
		PCI_WriteAuthErrTimes(gtCurKey.appid,1);
		goto PPRT_DownLoadWorkKey_Err;
	}
#endif	
	if(PCI_ReadCurrentKey(KEY_TYPE_MK, MKIndex, &keylen, mk))
	{	
		pucData[0] = RESULT_KEY_ERROR;
		goto PPRT_DownLoadWorkKey_Err;
	}
	if(pucData[3] == 0)
	{
		if(ParseTR31KeyBlock(mk, *pusLen, pucData, 11, 24+8, &keylen, keyblock))
		{
			pucData[0] = RESULT_ERROR_MACK;
			goto PPRT_DownLoadWorkKey_Err;
		}
		if(keylen != 24)
		{
			pucData[0] = RESULT_ERROR_PARA;
			goto PPRT_DownLoadWorkKey_Err;
		}
	}
	else
	{
		keylen = pucData[11];
		if(keylen != 24)
		{
			pucData[0] = RESULT_ERROR_PARA;
			goto PPRT_DownLoadWorkKey_Err;
		}
		if(ParseKeyBlock(mk, pucData[3], keylen, &pucData[12], &pucData[43], keyblock))
		{
			pucData[0] = RESULT_ERROR_MACK;
			goto PPRT_DownLoadWorkKey_Err;
		}
	}
	PCI_WriteCurrentKey(type,index,keylen,keyblock);
	pucData[0] = RESULT_SUCCESS;
PPRT_DownLoadWorkKey_Err:
	CLRBUF(keyblock);
	CLRBUF(mk);
	*pusLen = 1;
	gtCurKey.AuthFlag = STAT_AUTH_NOT;
	gtCurKey.AuthType = 0;
	gtCurKey.MKIndex = 0;
	return 0;
}

uchar PPRT_GetDes(uchar * pucData, ushort * pusLen)
{
	ushort len;
	uchar key[24], *pBuf;
	uchar keylen;
	//      0         1
	//recv: Index(1B)+Mode(1B)+Data(8~1024)
	//send: Stat(1B)+Data_E
	//      Stat(1B)
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[0] = RESULT_APP_TYPEERR;
		goto PPRT_GetDes_Err;
	}
	len = *pusLen;
	if(len<=2)
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetDes_Err;
	}
	len -= 2;	
	if(len%8 != 0 || len > 1024)
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetDes_Err;
	}
	if (!(pucData[1] == 0 || pucData[1] == 1))
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetDes_Err;
	}
	if(check_keyindex(KEY_TYPE_DESK, pucData[0]))
	{
		pucData[0] = RESULT_ERROR_PARA;		
		goto PPRT_GetDes_Err;
	}
	pBuf = gucTmpBuf;
#if 2
	if (gtCurKey.AuthFlag != STAT_AUTH_SUS)
	{
		pucData[0] = RESULT_NOT_AUTH;
		goto PPRT_GetDes_Err;
	}
	if ((sys_get_counter() - gtCurKey.BeginTime) > AUTH_DL_TIMEOUT)
	{
		pucData[0] = RESULT_TIMEOUT_ERR;
		goto PPRT_GetDes_Err;
	}	
	if (gtCurKey.AuthType != KEY_TYPE_AUTHDESK)
	{
		pucData[0] = RESULT_AUTHKEY_TYPEERR;
		PCI_WriteAuthErrTimes(gtCurKey.appid,1);
		goto PPRT_GetDes_Err;
	}
	if (gtCurKey.AuthIndex != pucData[0])
	{
		pucData[0] = RESULT_AUTHKEY_DIFF;
		PCI_WriteAuthErrTimes(gtCurKey.appid,1);
		goto PPRT_GetDes_Err;
	}
#endif
	if (PCI_ReadCurrentKey(KEY_TYPE_DESK, pucData[0], &keylen, key))
	{
		pucData[0] = RESULT_KEY_ERROR;
		goto PPRT_GetDes_Err;
	}
	if (pucData[1] == 1)
	{
		des_encrypt_ecb(pucData + 2, pBuf, len, key, keylen / 8);
	}
	else
	{
		des_decrypt_ecb(pucData + 2, pBuf, len, key, keylen / 8);
	}
	pucData[0] = RESULT_SUCCESS;
	memcpy(pucData + 1, pBuf, len);
	*pusLen = len+1;	
PPRT_GetDes_Err:
	CLRBUF(key);
	if(pucData[0])
		*pusLen = 1;
	gtCurKey.AuthType = 0;
	gtCurKey.AuthIndex = 0;
	gtCurKey.AuthFlag = STAT_AUTH_NOT;
	return 0;
}
uchar PPRT_GetMag(uchar *pucData, ushort *pusLen)
{
	ushort len;
	uchar keylen;
	uchar key[64], *pBuf;
	uint i;
	//       0        1
	//recv: Index(1B)+Data(8~1024)
	//send: Stat(1B)+Data_E
	//      Stat(1B)
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[0] = RESULT_APP_TYPEERR;
		goto PPRT_GetMag_Err;
	}
	len = *pusLen;
	if(len<2)
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetMag_Err;
	}
	len -= 1;
	if(len%8 != 0 || len > 1024)
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetMag_Err;
	}
	if(check_keyindex(KEY_TYPE_DESK, pucData[0]))
	{
		pucData[0] = RESULT_ERROR_PARA;		
		goto PPRT_GetMag_Err;
	}
	pBuf = gucTmpBuf;
	if (gtCurKey.AuthFlag != STAT_AUTH_SUS)
	{
		pucData[0] = RESULT_NOT_AUTH;
		goto PPRT_GetMag_Err;
	}
	if ((sys_get_counter() - gtCurKey.BeginTime) > AUTH_DL_TIMEOUT)
	{
		pucData[0] = RESULT_TIMEOUT_ERR;
		goto PPRT_GetMag_Err;
	}
	if (gtCurKey.AuthType != KEY_TYPE_AUTHDESK)
	{
		pucData[0] = RESULT_AUTHKEY_TYPEERR;
		PCI_WriteAuthErrTimes(gtCurKey.appid,1);
		goto PPRT_GetMag_Err;
	}
	if (gtCurKey.AuthIndex != pucData[0])
	{
		pucData[0] = RESULT_AUTHKEY_DIFF;
		PCI_WriteAuthErrTimes(gtCurKey.appid,1);
		goto PPRT_GetMag_Err;
	}
	app_read_info(gtCurKey.appid,INFO_APPK,&i,key);
	des_decrypt_ecb(pucData+1, pBuf, len, key,16/8);
	if (PCI_ReadCurrentKey(KEY_TYPE_DESK, pucData[0], &keylen, key))
	{
		pucData[0] = RESULT_KEY_ERROR;
		goto PPRT_GetMag_Err;
	}
	des_encrypt_ecb(pBuf, pucData+1, len, key,keylen/8);	
	pucData[0] = RESULT_SUCCESS;
	*pusLen = len+1;		
PPRT_GetMag_Err:
	CLRBUF(key);
	if(pucData[0])
		*pusLen = 1;
	gtCurKey.AuthFlag = STAT_AUTH_NOT;
	return 0;
}


uchar PPRT_GetMac(uchar * pucData, ushort * pusLen)
{
	uint len;
	uchar key[24], Mack[8];
	uchar keylen;
	//uchar *pdata;
	//      0         1        2            10
	//recv: Index(1B)+Mode(1B)+StartXor(8B)+Data(1024)
	//send: Stat(1B)+Mack(8B)
	//      Stat(1B)
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[0] = RESULT_APP_TYPEERR;
		goto PPRT_GetMac_Err;
	}
	len = (uint)(*pusLen);
	if(len>(1024+2+8) || len<=(2+8))
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetMac_Err;
	}
	if(check_keyindex(KEY_TYPE_MACK, pucData[0]))
	{
		pucData[0] = RESULT_ERROR_PARA;		
		goto PPRT_GetMac_Err;
	}
	if(LHALFB(pucData[1]) > 4)
	{
		pucData[0] = RESULT_ERROR_PARA;		
		goto PPRT_GetMac_Err;
	}
#if 2
	if (gtCurKey.AuthFlag != STAT_AUTH_SUS)
	{
		pucData[0] = RESULT_NOT_AUTH;
		goto PPRT_GetMac_Err;
	}
	if ((sys_get_counter() - gtCurKey.BeginTime) > AUTH_DL_TIMEOUT)
	{
		pucData[0] = RESULT_TIMEOUT_ERR;
		goto PPRT_GetMac_Err;
	}
	if (gtCurKey.AuthType != KEY_TYPE_AUTHMACK)
	{
		pucData[0] = RESULT_AUTHKEY_TYPEERR;
		PCI_WriteAuthErrTimes(gtCurKey.appid,1);
		goto PPRT_GetMac_Err;
	}
	if (gtCurKey.AuthIndex != pucData[0])
	{
		pucData[0] = RESULT_AUTHKEY_DIFF;
		PCI_WriteAuthErrTimes(gtCurKey.appid,1);
		goto PPRT_GetMac_Err;
	}
#endif
	// read data    
	if (PCI_ReadCurrentKey(KEY_TYPE_MACK, pucData[0], &keylen, key))
	{
		pucData[0] = RESULT_KEY_ERROR;
		goto PPRT_GetMac_Err;
	}
	/*
	CalculateMac((uint) pucData[1], (uint)keylen, key, len-2, pucData + 2, (ulong *)&len, Mack);	
	*/
	len -= (2+8);
	s_CaltMAC(LHALFB(pucData[1]),HHALFB(pucData[1]),pucData+2,key,len,pucData+10,Mack);
	pucData[0] = RESULT_SUCCESS;
	memcpy(pucData + 1, Mack, 8);
	*pusLen = 1 + 8;
PPRT_GetMac_Err:
	CLRBUF(key);
	if(pucData[0])
		*pusLen = 1;
	gtCurKey.AuthType = 0;
	gtCurKey.AuthFlag = STAT_AUTH_NOT;
	return 0;
}
// uiPINFormat=0 8byte pinblock -> 16 pinblock
// uiPINFormat=1 8byte pinblock
uchar PinEncrypt(uint uiPINFormat,
	uint uiKeyLen,uchar * pucKey,
	uint uiLenIn, uchar * pvCardNo, uchar * pvDataIn, uint * puiOutLen, uchar * pvDataOut)
{
	uchar ucBlock[20], ucTemp[26];
	uchar *pucDataIn, *pucDataOut;
	uint i, j, uiLen;

	pucDataIn = pvDataIn;
	pucDataOut = pvDataOut;

	uiLen = uiLenIn;

	for (i = 0; i < uiLen; i++)
	{
		ucBlock[i] = pucDataIn[i];
	}
	for (i = uiLen; i < 16; i++)
	{
		ucBlock[i] = 0xff;				// not enough 16 filled with 0XFF;
	}
	
	//To Form BLOCK1
	ucTemp[0] = '0';
	ucTemp[1] = '0' + uiLen;
	memcpy(ucTemp + 2, ucBlock, 14);
	for (i = 0, j = 0; i < 8; i++, j += 2)
	{
		ucBlock[i] = (ucTemp[j] << 4) + (ucTemp[j + 1] & 0x0f);
	}
#ifdef EM_DEBUG	
	vDispBufTitle("BLOCK1", 8, 0, ucBlock);
#endif
	//To Form BLOCK2    
	ucTemp[0] = ucTemp[1] = 0;
	for (i = 2, j = 0; i < 8; i++, j += 2)
	{
		ucTemp[i] = (pvCardNo[j] << 4) + (pvCardNo[j + 1] & 0x0f);
	}
#ifdef EM_DEBUG	
	vDispBufTitle("BLOCK2", 8, 0, ucTemp);
#endif
	//To get PIN-BLOCK
	for (i = 0; i < 8; i++)
	{
		ucTemp[i] ^= ucBlock[i];
	}
#ifdef EM_DEBUG	
	vDispBufTitle("BLOCK3", 8, 0, ucTemp);
#endif	
	des_encrypt_ecb(ucTemp, ucBlock, 8, pucKey, uiKeyLen / 8);
#ifdef EM_DEBUG	
	vDispBufTitle("BLOCK4", 8, 0, ucBlock);
#endif
	if(!uiPINFormat)
	{
		SI_ucParseStr(0,8,ucBlock,pucDataOut);
		*puiOutLen = 16;
	}
	else
	{
		memcpy(pucDataOut,ucBlock,8);
		*puiOutLen = 8;
	}
#ifdef EM_DEBUG	
	vDispBufTitle("BLOCK5", *puiOutLen, 0, pucDataOut);
#endif	
	return EM_SUCCESS;
}


uchar PPRT_GetPIN(uchar * pucData, ushort * pusLen)
{
	uint len, ucRet, pinLen;
	uchar lenMin, lenMax, mode, timeOut;
	uchar pinStr[16], key[KEY_UNIT_LEN];
	uchar keylen; 
	//            0      1     2      3    4       5           17          
	// recv(36B): Index+MinLen+MaxLen+Mode+Timeout+CardNo(12B)+Pinstr+00(padding 19B) 
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[0] = RESULT_APP_TYPEERR;
		goto PPRT_GetPIN_Err1;
	}
	if(*pusLen != 36)
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetPIN_Err;
	}
	lenMin = pucData[1];
	lenMax = pucData[2];
	mode = pucData[3];
	timeOut = pucData[4];
	if (timeOut == 0)
	{
		timeOut = 120;
	}
	if (timeOut < 30 || timeOut > 120)
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetPIN_Err1;
	}
	if ((lenMin < MIN_PCIPIN_LEN || lenMin > MAX_PIN_LEN) 
		|| (lenMax < MIN_PCIPIN_LEN || lenMax > MAX_PIN_LEN) 
		|| (lenMin > lenMax))
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetPIN_Err1;
	}
	if (mode)
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetPIN_Err1;
	}
	if(check_keyindex(KEY_TYPE_PINK, pucData[0]))
	{
		pucData[0] = RESULT_ERROR_PARA;		
		goto PPRT_GetPIN_Err1;
	}	
	// check cardno
    if ( s_CheckStr(12,&pucData[5],(int *)&pinLen,&pucData[5]) ) {
        pucData[0] = RESULT_ERROR_PARA;
        goto PPRT_GetPIN_Err1;
    }
#if 2
	if(sys_get_counter() - gtCurKey.PinLimitTime < PIN_INPUT_LIMITED_TIME)
	{
		pucData[0] = RESULT_INPUT_NOTALLOW;
		goto PPRT_GetPIN_Err;
	}
	if (gtCurKey.AuthFlag != STAT_AUTH_SUS)
	{
		pucData[0] = RESULT_NOT_AUTH;
		goto PPRT_GetPIN_Err;
	}
	if ((sys_get_counter() - gtCurKey.BeginTime) > AUTH_DL_TIMEOUT)
	{
		pucData[0] = RESULT_TIMEOUT_ERR;
		goto PPRT_GetPIN_Err;
	}
	if (gtCurKey.AuthType != KEY_TYPE_AUTHPINK)
	{
		pucData[0] = RESULT_AUTHKEY_TYPEERR;
		PCI_WriteAuthErrTimes(gtCurKey.appid,1);
		goto PPRT_GetPIN_Err;
	}
	if (gtCurKey.AuthIndex != pucData[0])
	{
		pucData[0] = RESULT_AUTHKEY_DIFF;
		PCI_WriteAuthErrTimes(gtCurKey.appid,1);
		goto PPRT_GetPIN_Err;
	}
#endif	
	if (PCI_ReadCurrentKey(KEY_TYPE_PINK, pucData[0], &keylen, key))
	{
		pucData[0] = RESULT_KEY_ERROR;
		goto PPRT_GetPIN_Err;
	}
	memset(pinStr, 0, sizeof(pinStr));	
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
	LCD_ShowPINPrompt();	
	ucRet = KB_GetPINStr(3, lenMin, lenMax, timeOut * 1000, (uint32_t *)&pinLen, pinStr);
#elif PRODUCT_TYPE == PRODUCT_TYPE_F12	
    s_CheckStr(14,&pucData[17],(int *)&pinLen,pinStr);
	if(pinLen == 0)
	{
		//S980		
		LCD_ShowPINPrompt();	
		ucRet = KB_GetPINStr(4, lenMin, lenMax, timeOut * 1000, (uint32_t *)&pinLen, pinStr);		
	}
	else if(pinLen >= lenMin && pinLen <= lenMax)
	{
		//S970
		ucRet = 0;
	}
	else
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetPIN_Err1;
	}
#endif
    s_DelayUs(500);
	if (ucRet)
	{
		if (ucRet == 2)
			pucData[0] = RESULT_INPUT_TIMEOUT;
		if (ucRet == 1)
			pucData[0] = RESULT_INPUT_CANCEL;
		goto PPRT_GetPIN_Err;
	}
//    vDispBufTitle("PINK",keylen,0,key);
//    vDispBufTitle("PIN明文",pinLen,0,pinStr);
	if(pinLen == 0) 
	{		
		pucData[0] = RESULT_INPUT_NONE;
	}
	else
	{
		PinEncrypt((uint) (mode&0x7F), (uint) keylen, key, (uint) pinLen, 
			pucData + 5, pinStr, &len,pucData + 1);
		*pusLen = len + 1;
//        vDispBufTitle("PIN密文",len,0,pucData);
		pucData[0] = RESULT_SUCCESS;
	}	
PPRT_GetPIN_Err:
	gtCurKey.PinLimitTime = sys_get_counter();
PPRT_GetPIN_Err1:	
	CLRBUF(key);
	CLRBUF(pinStr);
	if(pucData[0])
		*pusLen = 1;
	gtCurKey.AuthType = 0;
	gtCurKey.AuthFlag = STAT_AUTH_NOT;	
	return 0;
}

uchar PPRT_LCD(uchar * pucData, ushort *pusLen)
{
	uchar line;
	uchar str[30];
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[1] = RESULT_APP_TYPEERR;
		goto PPRT_LCD_Err;
	}
	if(pucData[0] == 0x00)
	{
		//cls
		if(*pusLen == 1)
		{
			LCD_ClearScreen();
		}
		else
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_LCD_Err;
		}
		
	}
	else if(pucData[0] == 0x01)
	{
		//display
		if (pucData[1] == 0 || pucData[1] > 4)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_LCD_Err;
		}
		if (*pusLen > 23)
		{
			pucData[1] = RESULT_ERROR_PARA;
			goto PPRT_LCD_Err;
		}
		line = pucData[1];
		memset(str, 0, sizeof(str));
		memcpy(str, pucData + 2, *pusLen - 2);
		pinpad_Display(line, (char *)str);
	}
	else 
	{
		DispScreen(0);
	}
	pucData[1] = RESULT_SUCCESS;
	*pusLen = 2;
	return EM_SUCCESS;
PPRT_LCD_Err:
	*pusLen = 2;
	return EM_ERROR;
}

// 增加公钥加密PIN
uchar PPRT_GetUserPin(uchar * pucData, ushort * pusLen)
{
	//recv:        0    1    2       3  4     
	// 02 09 BD 01 Min  Max  Timeou  00 pinstr 
    //                               3  4     12    12+256
	//             Min  Max  Timeout 01 rand  pubk  pinstr 
	//send:  Pinlen+Pin                              
	//  
    //    0   1   2         3    5
    //PCI:Min+Max+mode(0/1)+Time+[rand(8B)+RSA包(264B)]+pin包
	uchar pinStr[32],key[24];
	uchar ucRet;
	uint pinLen,i;	
	R_RSA_PUBLIC_KEY *pubk;	
	uchar *charp;
	if(gAppProcolType!=APP_PROTOCOL_PCI)
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetUserPin_Err;
	}
	if (pucData[2] == 0)
	{
		pucData[2] = 120;
	}
	if (pucData[2] < 30 || pucData[2] > 120)
	{
		pucData[0] = RESULT_TIMEOUT_ERR;
		goto PPRT_GetUserPin_Err;
	}
	if ((pucData[0] < MIN_PCIPIN_LEN || pucData[0] > MAX_PIN_LEN) 
		|| (pucData[1] < MIN_PCIPIN_LEN || pucData[1] > MAX_PIN_LEN) 
		|| (pucData[0] > pucData[1])
		|| (pucData[3] > 1))
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_GetUserPin_Err;
	}
	if(pucData[3] == 0)
	{
        // 明文返回
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210		
		if(*pusLen != 4)
		{
			pucData[0] = RESULT_ERROR_PARA;
			goto PPRT_GetUserPin_Err;
		}
#else
        *pusLen -= 4;
        s_CheckStr(*pusLen,&pucData[4],(int *)&pinLen,pinStr);
        if ( pinLen ) {
            //S970
            if ( pinLen < pucData[0] || pinLen > pucData[1]) {
                //S970 err
                pucData[0] = RESULT_ERROR_PARA;
                goto PPRT_GetUserPin_Err;
            }
            goto PPRT_GetUserPin_0;
        }
#endif
	}
	else 
	{
        // RSA加密
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210			
		if(*pusLen != (4+sizeof(R_RSA_PUBLIC_KEY)+8))
		{
			pucData[0] = RESULT_ERROR_PARA;
			goto PPRT_GetUserPin_Err;
		}
#endif
		pubk = (R_RSA_PUBLIC_KEY *)&pucData[4+8];
		if(pubk->bits%8 != 0 || pubk->bits < 512 || pubk->bits > 2048)
		{
			pucData[0] = RESULT_ERROR_PARA;
			goto PPRT_GetUserPin_Err;
		}
#if PRODUCT_TYPE == PRODUCT_TYPE_F12					
        *pusLen-=(4+sizeof(R_RSA_PUBLIC_KEY)+8);
        s_CheckStr(*pusLen,&pucData[4+sizeof(R_RSA_PUBLIC_KEY)+8],(int *)&pinLen,pinStr);
        if ( pinLen ) {
            //S970
            if ( pinLen < pucData[0] || pinLen > pucData[1]) {
                //S970 err
                pucData[0] = RESULT_ERROR_PARA;
                goto PPRT_GetUserPin_Err;
            }
            goto PPRT_GetUserPin_0;
        }
#endif		
	}
	memset(pinStr, 0, sizeof(pinStr));
	LCD_ShowPINPrompt();	
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210		
	ucRet = KB_GetPINStr(3, pucData[0], pucData[1], pucData[2] * 1000, (uint32_t *)&pinLen, pinStr);
#elif PRODUCT_TYPE == PRODUCT_TYPE_F12	
	ucRet = KB_GetPINStr(4, pucData[0], pucData[1], pucData[2] * 1000, (uint32_t *)&pinLen, pinStr);
#endif
    s_DelayUs(500);
	if (ucRet)
	{
		memset(pinStr, 0, sizeof(pinStr));
		if (ucRet == 2)
			pucData[0] = RESULT_INPUT_TIMEOUT;
		if (ucRet == 1)
			pucData[0] = RESULT_INPUT_CANCEL;
		goto PPRT_GetUserPin_Err;
	}
	if(pinLen == 0)
	{
		pucData[0] = RESULT_INPUT_NONE;
		goto PPRT_GetUserPin_Err;
	}
PPRT_GetUserPin_0:
    DISPBUF("PIN明文",pinLen,0,pinStr);
	if(pucData[3] == 0)
	{	
		//用APPAK加密
		if(app_read_info(gtCurKey.appid,INFO_APPK,&i,key))
		{
			pucData[0] = RESULT_KEY_ERROR;
            DISPERR(gtCurKey.appid);
			goto PPRT_GetUserPin_Err;
		}
        DISPBUF("APPAK",i,0,key);
		des_encrypt_ecb(pinStr, &pucData[1], 16, key, 2);
        DISPBUF("PIN密文",16,0,&pucData[1]);
		pucData[0] = RESULT_SUCCESS;
		*pusLen = 16+1;
		CLRBUF(key);
		CLRBUF(pinStr);
        DISPPOS(0);
		return EM_SUCCESS;
	}
	else
	{	
		//公钥加密
		charp = &gucTmpBuf[RSA_UNIT_LEN*0]; 
		// 0      1            9             17
		// 7F(1B)+Pinblock(8B)+IccRandom(8B)+PinpadRandom(Nic-17)
		memset(charp,0,8);
		charp[0] = 0x7F;
		charp[1] = COMBINE8(0x02,(uchar)pinLen);
		for(;pinLen<14;pinLen++)
		{
			pinStr[pinLen] = 'F';
		}
		Str2Hex((int)pinLen,pinStr,(int *)&pinLen,&charp[2]);
		RNG_FillRandom(&charp[17],(pubk->bits/8-17)*2);
		memcpy(&charp[9],&pucData[4],8);  //icc random
		//pubk->bits =pubk->bits/8;	
		//DISPBUF("明文", pubk->bits/8, 0, charp);
		if(!rsa_pub_dec(charp,&pinLen,charp,pubk->bits/8,pubk))	
//		if(!Rsa_calt_pub(pubk->bits/8, charp, pubk, (int *)&pinLen, charp))	
		{
			//DISPBUF("密文",pinLen, 0, charp);
			*pusLen = pinLen;
			pucData[0] = RESULT_SUCCESS;
			memcpy(pucData+1,(char *)charp,*pusLen);
			*pusLen += 1;
			CLRBUF(pinStr);
			return EM_SUCCESS;
		}
		pucData[0] = RESULT_ERROR;
	}	
PPRT_GetUserPin_Err:
	CLRBUF(pinStr);
	*pusLen = 1;
	return ERROR;
}

uchar PPRT_GetVer(uchar *pucData, ushort *pusLen)
{
	uint len;
	if(*pusLen == 0)
	{
		pucData[0] = RESULT_ERROR_PARA;
		*pusLen = 1;
		return 1;
	}
	else 
	{
		
		if(sys_get_ver(pucData[0],&len,pucData+1))
		{
			pucData[0] = RESULT_ERROR_PARA;
			*pusLen = 1;
			return 1;
		}
		pucData[0] = 0;
		*pusLen = 1+len;
		return 0;
	}
}

uchar PPRT_CheckSHA(uchar *pucData, ushort * pusLen)
{
	//            0         1                33            
	// recv(37B): 1B mode(00-ctrl 01~04-app)+32B(明文)+4B(密文Mack)
	// send: 00/01    
	uchar aucMac[4];
	uchar fsk[FSK_LEN];
	if(*pusLen != 37 || pucData[0] > 4)
	{
		*pusLen = 1;
		pucData[0] = RESULT_ERROR_PARA;
		return 1;
	}
	*pusLen = 1;
	if(pucData[0] == 0)
	{
		PCI_ReadFSK(FSK_CMCTRL,fsk);
	}
	else
	{
		PCI_ReadFSK(pucData[0]+FSK_CMAPP-1,fsk);  //01~04 ->02~05
	}
	cacul_mac_ecb(FSK_LEN,fsk,32,&pucData[1],4,aucMac);
	CLRBUF(fsk);
	//vDispBufTitle(NULL, 36,0,&pucData[1]);
	//vDispBufTitle(NULL, 4,0,aucMac);
	if(!memcmp(aucMac,pucData+33, 4))
	{
		if(pucData[0] == 0)
		{
			//check cm-ctrl is ok
			gWorkMode = MODE_CTRLMANAGE;
		}
		else
		{
			gWorkMode = MODE_IDLE;
		}
		pucData[0] = RESULT_SUCCESS;		
		return 0;
	}
	else
	{
		pucData[0] = RESULT_ERROR;
		return 1;
	}	
}
// 1-enable 0-disable
void PPRT_SetAttackflag(uchar *pucData, ushort *pusLen)
{
	if(*pusLen != 1 || *pucData > 1)
	{
		*pusLen = 1;
		*pucData = 1;
		return;
	}
	if(*pucData != gEnableSelfCheck)
	{
		if(set_appenable(*pucData))
		{
			*pucData = 1;
		}
		else
		{
			*pucData = 0;
			if(gEnableSelfCheck == 0)
				gEnableSelfCheck = 1;
			else
				gEnableSelfCheck = 0;
		}
	}
	else
	{
		*pucData = 0;
	}
	*pusLen = 1;
}

void PPRT_DESDATA(uchar *pucData, ushort *pusLen)
{
	ushort len;
	uchar key[24], *pBuf;
	uchar keylen;
	//      0    1    2     4  
	//recv: mode+3B 00(预留)+data(8~1024) 
	//send: Stat(1B)+Data_E
	//      Stat(1B)
	len = *pusLen-4;	
	if(len%8 != 0 || len > 1024)
	{
		pucData[0] = RESULT_ERROR_PARA;
		goto PPRT_DESDATA_Err;
	}
	if (PCI_ReadMCKey(0, 0, &keylen, key))
	{
		pucData[0] = RESULT_KEY_ERROR;
		goto PPRT_DESDATA_Err;
	}
	pBuf = gucTmpBuf;
	if (pucData[0] == 0)
	{
		des_decrypt_ecb(pucData + 4, pBuf, len, key, keylen / 8);
	}
	else
	{
		des_encrypt_ecb(pucData + 4, pBuf, len, key, keylen / 8);
	}
	CLRBUF(key);
	pucData[0] = RESULT_SUCCESS;
	memcpy(pucData + 1, pBuf, len);
	*pusLen = len+1;
	return;
PPRT_DESDATA_Err:
	CLRBUF(key);
	*pusLen = 1;	
}

uchar PPRT_Extend_Setbps(uchar *data,ushort *pusLen)
{
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210		
    uint i;
    switch(data[1])
    {
    case 1:
        i = 9600;
        break;
    case 2:
        i = 19200;
        break;
    case 3:
        i = 57600;
        break;
//    case 4:
//        i = 1200;
//        break;
    case 7:
        i = 38400;
        break;
    case 8:
        i = 115200;
        break;
    default:
        *data = RESULT_ERROR_PARA;
        goto PPRT_Extend_Setbps;
    }
    s_sysconfig_write(OFFSET_BPS,LEN_BPS,(uchar *)&i);	
    *data = 0;
PPRT_Extend_Setbps:
#else
    *data = RESULT_ERROR;
#endif
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	        
	if(!*data)
	{
		UART_Init(i);
	}
#endif        
	*pusLen = 0;
    return 0;
}

/* 
 * PPRT_Extend_Ic - [GENERIC] 
 *  IC卡操作 接收格式:  0-open ic卡模块  1-close ic卡模块
 *                                 2-设置卡座属性   3-判断卡座状态  00-卡在位  01-卡上电
 *                                 4-卡复位         5-读卡          6-卡下电
 *           发送格式:  返回值(4B) + 数据
 * @ 
 */
uchar PPRT_Extend_Ic (uchar *pin, ushort *inlen)
{
#ifdef CFG_ICCARD
    uchar out[256+16];
    uint outlen=4,i,j;
    uchar *in = pin+1;
    int ret=0,slot;
    switch ( in[0] )
    {
    case PCI_CMD_EXTEND_IC_OPEN :
        memcpy(out,&ret,sizeof(int));
        break;
    case PCI_CMD_EXTEND_IC_CLOSE :
        memcpy(out,&ret,sizeof(int));
        break;
    case PCI_CMD_EXTEND_IC_INIT :
        memcpy(&slot,&in[1],sizeof(int));
        memcpy(&i,&in[1+4],sizeof(int)); //CardVol
        memcpy(&j,&in[1+4+4],sizeof(int)); //CardMode
        ret = icc_InitModule(slot,i,j);
        memcpy(out,&ret,sizeof(int));
        break;
    case PCI_CMD_EXTEND_IC_CHECK :
        memcpy(&slot,&in[1],sizeof(int));
        if ( in[1+4] == 0) {
            ret = icc_CheckInSlot(slot);
        } else {
            ret = icc_CheckVcc(slot);
        }
        memcpy(out,&ret,sizeof(int));
        break;
    case PCI_CMD_EXTEND_IC_POWERUP :
        memcpy(&slot,&in[1],sizeof(int));
        ret = icc_Reset(slot,&i,&out[4]);
        if (ret == ICC_SUCCESS) {
            gSystem.lpwr.bm.iccard = 1;
        }
        memcpy(out,&ret,sizeof(int));
        outlen = 4+i;
        break;
    case PCI_CMD_EXTEND_IC_EXCHANGE :
        memcpy(&slot,&in[1],sizeof(int));
        i = *inlen-4-2;
        ret = icc_ExchangeData(slot,(int)i,&in[1+4],(int *)&j,&out[4]);
//        vDispBufTitle("out",j,0,&out[4]);
        memcpy(out,&ret,sizeof(int));
        outlen = 4+j;
        break;
    case PCI_CMD_EXTEND_IC_POWERDOWN :
        memcpy(&slot,&in[1],sizeof(int));
        ret = icc_Close(slot);
        if (ret == ICC_SUCCESS) {
            gSystem.lpwr.bm.iccard = 0;
        }
        memcpy(out,&ret,sizeof(int));
        break;
    default :
        out[0] = RESULT_ERROR_PARA;
        outlen = 1;
        break;
    }
    memcpy(pin,out,outlen);
    *inlen = outlen;
#else
    *pin = RESULT_ERROR;
	*inlen = 1;
#endif
    return 0;
}		/* -----  end of function PPRT_Extend_Ic  ----- */

//pucData:input and output para
//length: input and output para
uchar PPRT_Extend_Func(uchar *pucData, ushort *length)
{
    uchar *p = pucData+1;

    switch ( p[0] )
    {
#ifdef CFG_LED 
    case PCI_EXTCMD_FUNC_LED :
        pucData[0] = RESULT_SUCCESS;
        if (p[2] == 0) {
            pucData[0] = RESULT_ERROR_PARA;
            *length = 1;
            break;
        }
        if (p[1] == OFF) {
            hw_led_off(p[2]);
        } else if (p[1] == ON) {
            hw_led_on(p[2]);
        } else {
            pucData[0] = RESULT_ERROR_PARA;
        }
        *length = 1;
        break;
#endif
    default :
        pucData[0] = RESULT_ERROR_PARA;
        *length = 1;
        break;
    }
    return 0;
}

/*-----------------------------------------------------------------------------}
 *  扩展指令  格式: id+str
 *  1. id=0   设置波特率
 *  2. id=1   IC卡操作 str区格式:  0-open ic卡模块  1-close ic卡模块
 *                                 2-设置卡座属性   3-判断卡座状态  00-卡在位  01-卡上电
 *                                 4-卡复位         5-读卡          6-卡下电
 *                                 
 *
 *-----------------------------------------------------------------------------{*/
uchar PPRT_Extend(uchar *pucData, ushort *pusLen)
{
    switch ( pucData[0] )
    {
    case PCI_CMD_EXTEND_SETBPS :
        PPRT_Extend_Setbps(pucData,pusLen);
        break;
    case PCI_CMD_EXTEND_IC :
        PPRT_Extend_Ic(pucData,pusLen);
        break;
    case PCI_CMD_EXTEND_FUNC:
        PPRT_Extend_Func(pucData,pusLen);
        break;
    default :
		pucData[0] = RESULT_ERROR_PARA;
        *pusLen = 1;
        break;
    }
	return 0;
}

/* 
 * PPRT_CheckAuthkey - [GENERIC] 
 * @ 
 */
int PPRT_CheckAuthkey (uchar *pucData, ushort *pusLen)
{
    // type+index
    *pusLen = 1;
    if(pucData[0] < KEY_TYPE_AUTHMK || pucData[0] > KEY_TYPE_AUTHDESK)
    {
        pucData[0] = RESULT_ERROR_PARA;
        return 1;
    }
    if(check_keyindex(pucData[0],pucData[1]))
    {
        pucData[0] = RESULT_ERROR_PARA;
        return 1;
    }
	if (gtCurKey.AuthFlag != STAT_AUTH_SUS)
	{
		pucData[0] = RESULT_NOT_AUTH;
        return 1;
	}
    if ( gtCurKey.AuthType != pucData[0] ) {
        pucData[0] = RESULT_AUTHKEY_TYPEERR;
        return 1;
    }
    if (gtCurKey.AuthIndex != pucData[1]) {
        pucData[0] = RESULT_AUTHKEY_DIFF;
        return 1;
    }
    pucData[0] = 0;
    return 0;
}		/* -----  end of function PPRT_CheckAuthkey  ----- */
// cmd:0 无限制
// cmd 不等于0，这限制接受命令
void PPRT_DealWith(void)
{
	uchar cmd;
	ushort usLen;
	uchar *aucData = gucBuff;
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
#ifdef CFG_DBG_PINPAD
extern char gDebugFlg;
    if ( gDebugFlg ) {
        if ( gDebugFlg >= 2 ) {
            lcd_Display(0,DISP_FONT*2,DISP_FONT|DISP_CLRLINE,"%d %d %d %d %d",gDebugFlg,gtCurKey.BeginTime,gtCurKey.ShakehandBeginTime,gtCurKey.PinLimitTime,sys_get_counter()); 	
            if(gtCurKey.appid)
            {
                lcd_Display(0,DISP_FONT*3,DISP_FONT|DISP_CLRLINE,"%d-%d-%d-%d-%d",gtCurKey.DlAuthPinkPerHs,gtCurKey.DlAuthDeskPerHs,gtCurKey.DlAuthMackPerHs,gtCurKey.DlAuthMKPerHs,gtCurKey.DlMKPerHs); 
                lcd_Display(0,DISP_FONT*4,DISP_FONT|DISP_CLRLINE,"1]%d %d-%d-%d %d-%d-%d ",gtCurKey.HsFlag,gtCurKey.AuthType,gtCurKey.AuthIndex,gtCurKey.AuthFlag,
                               gtCurKey.MKIndex,gtCurKey.WKType,gtCurKey.AuthIndex);
            }	
        }
    }
#endif
#endif
	if (PPRT_Receive(&cmd, aucData, &usLen))
	{
		return;
	}
    exit_lowerpower_freq();
#ifdef DEBUG_Dx
//    TRACE("\r\n-|cmd:%02X",cmd);
#endif
//    TRACE_BUF(NULL,aucData,usLen);
//    TRACE("\t|WorkMode=%d",gWorkMode);
	LCD_LightOn();
#ifdef _DEBUG_OUTPUT_	
	if(cmd == PCI_CMD_DEBUG0)
	{
#ifdef CFG_DBG_PINPAD
        dbg_pinpad();
#endif
        usLen = 1;
	}
#endif	
	if(cmd == PCI_CMD_APPMANAGE)
	{
		PPRT_AppManage(aucData, &usLen);
	}
	else if(cmd == PCI_CMD_VER)
	{
		PPRT_GetVer(aucData, &usLen);
	}
	else if(cmd == PCI_CMD_RND)
	{
		PPRT_GetRandom(aucData, &usLen);
	}
	else if(cmd == PCI_CMD_MK && gWorkMode != MODE_APP)
	{
		PPRT_DownLoadAuthAndMasterKey(aucData, &usLen);	
	}
	else if(cmd == PCI_CMD_EXTEND)
	{
		PPRT_Extend(aucData, &usLen);
	}
	else if(cmd == CMD_REQUEST)
	{
		aucData[0] = gWorkMode;
		usLen = 1;
	}
#if PRODUCT_TYPE == PRODUCT_TYPE_F12
	else if(cmd == CMD_GETTIME)
	{
		app_get_time(aucData, &usLen);
	}
	else if(cmd == CMD_SETTIME)
	{
		app_set_time(aucData, &usLen);
	}	
	else if(cmd == CMD_SYSMENU)
	{
		aucData[0] = menu_sys(*aucData);
		usLen = 1;
	}
	else if(cmd == CMD_OPEN_MAGCARD)
	{
		app_magcard_parse(aucData, &usLen);
	}
	else if(cmd == CMD_GetSecuLevel)
	{
		*aucData = gEnableSelfCheck;
		usLen = 1;
	}
	else if(cmd == CMD_SetSecuLevel)
	{
		PPRT_SetAttackflag(aucData, &usLen);
	}
	else if(cmd == CMD_CHECK_SHA)
	{
		PPRT_CheckSHA(aucData, &usLen);
	}
	else if(cmd == CMD_DESDATA)
	{
		PPRT_DESDATA(aucData, &usLen);
	}
	else if(cmd == CMD_FACTEST)
	{
		PPRT_FactoryTest(aucData, &usLen);
	}
	else if(cmd == CMD_SET_CONFIG)
	{
		PPRT_SetConfig(aucData, &usLen);
	}
    else if(cmd == CMD_TAMPER && aucData[0] == 0)
    {
        PPRT_TamperPinStatus(aucData,&usLen);
        PPRT_Send(cmd, aucData, usLen);
    }
#endif
	else if(gWorkMode == MODE_APP)
	{
		switch (cmd)
		{		
		case PCI_CMD_HS:
			PPRT_HandShake(aucData, &usLen);
			break;
		case PCI_CMD_SN:
			PPRT_GetSn(aucData, &usLen);
			break;
		case PCI_CMD_SETSN:
			PPRT_SetSn(aucData, &usLen);	
			break;
		case PCI_CMD_WK:
			PPRT_DownLoadWorkKey(aucData, &usLen);
			break;
		case PCI_CMD_MK:
			PPRT_DownLoadAuthAndMasterKey(aucData, &usLen);	
			break;
		case PCI_CMD_CK:
			PPRT_AuthKey(aucData, &usLen);
			break;				
		case PCI_CMD_GET_MAC:
			PPRT_GetMac(aucData, &usLen);
			break;				
		case PCI_CMD_GET_PIN:
			PPRT_GetPIN(aucData, &usLen);
			break;				
		case PCI_CMD_GET_DES:
			PPRT_GetDes(aucData, &usLen);
			break;				
		case PCI_CMD_GET_MAG:
			PPRT_GetMag(aucData, &usLen);
			break;			
		case PCI_CMD_LCD:
			PPRT_LCD(aucData, &usLen);
			break;
		case PCI_CMD_PIN:
            TRACE("\r\n uslen:%d %x",usLen,&usLen);
			PPRT_GetUserPin(aucData, &usLen);
            TRACE("\r\n uslen:%d %x",usLen,&usLen);
            TRACE("\r\n uslen:%d %x",usLen,&usLen);
			break;
        case CMD_CHECKAUTH:
            PPRT_CheckAuthkey(aucData,&usLen);
            break;
		default:
			aucData[0] = RESULT_NOT_PERMIT;
			usLen = 1;
			break;			
		}
	}
	else
	{
		if(cmd == PCI_CMD_MK || cmd == PCI_CMD_CK || cmd == PCI_CMD_HS)
		{
			aucData[1] = RESULT_APP_NOTRUN;
			aucData[0] += 1;
			usLen = 2;
		}
		else
		{
			aucData[0] = RESULT_APP_NOTRUN;
			usLen = 1;
		}	
	}
    enter_lowerpower_freq();
	PPRT_Send(cmd, aucData, usLen);		
}
// mode=0   fsk or appak
//     =1   mk or authkey 
//     
// workmode=MODE_CTRLMANAGE
void PPRT_ManageDealWith(uint mode)
{
	uchar cmd;
	ushort usLen;
	uchar *aucData = gucBuff;
	KB_Flush();
	LCD_DispDlFsk(mode);
	while(1)
	{
	#if PRODUCT_TYPE == PRODUCT_TYPE_MK210		
		if(KB_Hit())
		{
			if(KB_GetKey(20) == KEY_CANCEL)
				return;
		}
	#endif
		if (PPRT_Receive(&cmd, aucData, &usLen))
		{
			continue;
		}	
		if(cmd == PCI_CMD_HS)
		{
			PPRT_HandShake(aucData, &usLen);
		}
		else if(cmd == PCI_CMD_SN)
		{
			PPRT_GetSn(aucData, &usLen);
		}
		else if(cmd == PCI_CMD_MK)
		{
			PPRT_DownLoadAuthAndMasterKey(aucData, &usLen);	
		}
	#if PRODUCT_TYPE == PRODUCT_TYPE_F12	
		else if(cmd == CMD_EXIT)
		{
			aucData[0] = 0;
			PPRT_Send(cmd, aucData, usLen);
		}
	#endif
		else
		{
			continue;
		}		
		PPRT_Send(cmd, aucData, usLen);
	}
	
}


#ifdef CFG_DBG_PINPAD
// =1 MMK  =2 Workkey  =3 AuthWorkkey =4 MK/AuthMK =5 appinfo
void PPRT_Debug0(uchar *pucData, ushort *pusLen)
{
#if 2
    uint j,k,ret;
    uchar key[128];
    //uchar appek[24];
    char title[32];
    if(*pucData == 1 || *pucData == 0xFF)
    {
        vDispBufTitle("MMK", MMAP_KEY_LEN+CHECK_LEN, 0, &gMKey);	
    }
    if(*pucData == 2 || *pucData == 0xFF)
    {	
        for(j=0;j<NUM_PINK*gtCurKey.apptype;j++)
        {
            k = 0;
            ret = PCI_ReadCurrentKey(KEY_TYPE_PINK,(uchar)j,(uchar *)&k,key);
            if(!ret)
            {
                CLRBUF(title);sprintf(title,"Pink%d",j);
                vDispBufTitle(title,k,0,key);
            }
        }
        for(j=0;j<NUM_MACK*gtCurKey.apptype;j++)
        {
            k = 0;
            ret = PCI_ReadCurrentKey(KEY_TYPE_MACK,(uchar)j,(uchar *)&k,key);
            if(!ret)
            {
                CLRBUF(title);sprintf(title,"Mack%d",j);
                vDispBufTitle(title,k,0,key);
            }
        }
        for(j=0;j<NUM_DESK*gtCurKey.apptype;j++)
        {
            k = 0;
            ret = PCI_ReadCurrentKey(KEY_TYPE_DESK,(uchar)j,(uchar *)&k,key);
            if(!ret)
            {
                CLRBUF(title);sprintf(title,"Desk%d",j);
                vDispBufTitle(title,k,0,key);
            }
        }
    }
    if(*pucData == 3 || *pucData == 0xFF)
    {
        for(j=0;j<NUM_PINK*gtCurKey.apptype;j++)
        {
            k = 0;
            ret = PCI_ReadCurrentKey(KEY_TYPE_AUTHPINK,(uchar)j,(uchar *)&k,key);
            if(!ret)
            {
                CLRBUF(title);sprintf(title,"AuthPink%d",j);
                vDispBufTitle(title,k,0,key);
            }
        }
        for(j=0;j<NUM_MACK*gtCurKey.apptype;j++)
        {
            k = 0;
            ret = PCI_ReadCurrentKey(KEY_TYPE_AUTHMACK,(uchar)j,(uchar *)&k,key);
            if(!ret)
            {
                CLRBUF(title);sprintf(title,"AuthMack%d",j);
                vDispBufTitle(title,k,0,key);
            }
        }
        for(j=0;j<NUM_DESK*gtCurKey.apptype;j++)
        {
            k = 0;
            ret = PCI_ReadCurrentKey(KEY_TYPE_AUTHDESK,(uchar)j,(uchar *)&k,key);
            if(!ret)
            {
                CLRBUF(title);sprintf(title,"AuthDesk%d",j);
                vDispBufTitle(title,k,0,key);
            }
        }	
        for(j=0;j<NUM_AUTHMK*gtCurKey.apptype;j++)
        {
            k = 0;
            ret = PCI_ReadCurrentKey(KEY_TYPE_AUTHMK,(uchar)j,(uchar *)&k,key);
            if(!ret)
            {
                CLRBUF(title);sprintf(title,"AuthMK%d",j);
                vDispBufTitle(title,k,0,key);
            }
        }
    }
    if(*pucData == 4 || *pucData == 0xFF)
    {
        for(j=0;j<NUM_AUTHMK*gtCurKey.apptype;j++)
        {
            k = 0;
            ret = PCI_ReadCurrentKey(KEY_TYPE_MK,(uchar)j,(uchar *)&k,key);
            if(!ret)
            {
                CLRBUF(title);sprintf(title,"MK%d",j);
                vDispBufTitle(title,k,0,key);
            }
        }
    }
    if(*pucData == 5 || *pucData == 0xFF)
    {
        for(j=0;j<NUM_AUTHMK*gtCurKey.apptype;j++)
        {
            k = 0;
            ret = PCI_ReadCurrentKey(KEY_TYPE_AUTHMK,(uchar)j,(uchar *)&k,key);
            if(!ret)
            {
                CLRBUF(title);sprintf(title,"AuthMK%d",j);
                vDispBufTitle(title,k,0,key);
            }
        }

    }
    if(*pucData == 6 || *pucData == 0xFF)
    {
        s_read_syszone(0,SYSZONE_LEN,gucBuff);
        vDispBufTitle("syszone",SYSZONE_LEN, 0, gucBuff);		
    }
//    if(*pucData == 0xFE)
//    {
//        //sys_recover(0);
//        //TRACE("Mode[%d-%d]",gWorkMode,pucData[1]);
//        gWorkMode = pucData[1];
//    }
    *pusLen = 1;
#endif
}

void DispAppInfo(uint app)
{
    uchar buf[64];
    //char str[20];
    uint len;
    // name(8B)+appk(24B)+psw(16B)
    TRACE("\r\n ====应用:%d信息====",app);
    CLRBUF(buf);
    app_read_info(app,INFO_FLAG,&len,buf);
    vDispBufTitle("flag",len,0,buf);
    app_read_info(app,INFO_APPNAME,&len,buf);
    vDispBufTitle("appname",len,0,buf);
    app_read_info(app,INFO_APPK,&len,buf);
    vDispBufTitle("appak",len,0,buf);
    app_read_info(app,INFO_PSW,&len,buf);
    vDispBufTitle("PSW",len,0,buf);
    read_transferkey(app,buf);
    vDispBufTitle("传输密钥",24,0,buf);
    vDispLable(0,12);
}
void dbg_pinpad(void)
{
    extern void dbg_syszone(void);
    extern void dbg_keyzone(void);
    uint i,j,ret;
    //uchar key[32];
    uchar buf[64];
    struct KEY_APP *q;
    lcd_cls();
    lcd_Display(0,DISP_FONT*0,DISP_FONT|DISP_INVLINE,"dbg_pinpad");
    TRACE("\r\n boot:%x %x ctrl:%x %x",SA_BOOT,LEN_BOOT,SA_CTRL,LEN_CTRL);
    TRACE("\r\n font:%x %x syszone:%x %x ",SA_ZK,LEN_ZK,SA_SYSZONE,SYSZONE_LEN);
    TRACE("\r\n keyzone:%x len:%x",KEY_ZONE_ADDR,sizeof(struct APP_ZONE));
    for ( i=0 ; i< APP_TOTAL_NUM; i++) {
       TRACE("\r\n appzone:%d-%x-%x",i+1,KEY_ZONE_APP_ADDR(i),sizeof(struct KEY_APP)); 
    }
    while(1)
    {		
        vDispLable(0,12);
        TRACE("\r\n *******工作状态:%x 当前应用:%d*******",gWorkMode,gtCurKey.appid);
        TRACE("\r\n 1-syszone 2-读应用信息 3-读公共密钥 4-手动设置认证状态 5-手动设置工作状态");
        TRACE("\r\n 6-当前工作状态 7-set_appenable 8-应用密钥 9-读应用flash区 10-PCI_InitPublicKey");
        TRACE("\r\n 11-dbg_keyzone  12-IC 13-默认应用V1APP 14-删除应用 15-dbg_dryice");
        switch(InkeyCount(0))
        {
        case 1:
#ifdef CFG_DBG_SYSZONE
            dbg_syszone();
#endif
            break;
        case 2:
            TRACE("应用号(1~6 0-全部):");
            j = InkeyCount(0);
            if ( j == 0 ) {
                for ( i=1 ; i<6 ; i++  ) {
                    DispAppInfo(i);				
                }
            } else {
                DispAppInfo(j);				
            }
            break;
        case 3:
            vDispBufTitle("MMK",sizeof(gMKey.aucMMK),0,gMKey.aucMMK);
            vDispBufTitle("ELRCK",sizeof(gMKey.aucELRCK),0,gMKey.aucELRCK);
//            vDispBufTitle("MagKey",sizeof(gMKey.aucMagKey),0,gMKey.aucMagKey);
//            vDispBufTitle("MCKey",sizeof(gMKey.aucMCKey),0,gMKey.aucMCKey);
            break;
        case 4:
            TRACE("当前:HsFlag:%d AuthFlag %d:",gtCurKey.HsFlag,gtCurKey.AuthFlag);
            gtCurKey.AuthFlag = InkeyCount(0);
            gtCurKey.HsFlag = InkeyCount(0);
            break;
        case 5:
            gWorkMode = InkeyCount(0);
            break;
        case 6:
//            vDispBufTitle("gtCurKey",sizeof(gtCurKey),0,&gtCurKey);
            TRACE("\r\n ======gtCurKey===== ");
            TRACE("\r\n appid:%x apptype:%d Mkindex:%x WKtype:%d",gtCurKey.appid,gtCurKey.apptype,gtCurKey.MKIndex,gtCurKey.WKType);
            TRACE("\r\n auth:%x-%d-%x  time:%d",gtCurKey.AuthType,gtCurKey.AuthIndex,gtCurKey.AuthFlag,gtCurKey.BeginTime);
            TRACE("\r\n HsFlag:%d %d-%d-%d-%d time:%d",gtCurKey.HsFlag,gtCurKey.DlAuthMKPerHs,gtCurKey.DlAuthPinkPerHs
                  ,gtCurKey.DlAuthDeskPerHs,gtCurKey.DlAuthMackPerHs,gtCurKey.DlAuthDeskPerHs,gtCurKey.ShakehandBeginTime);
            TRACE("\r\n scatter:%d encflg:%d",gtCurKey.scatter,gtCurKey.encflg);
            TRACE("\r\n pinlen:%d time:%d",gtCurKey.pinlen,gtCurKey.PinLimitTime);
            vDispBufTitle("rand",sizeof(gtCurKey.aucRand),0,gtCurKey.aucRand);
            break;
        case 7:
            TRACE("\r\n 1-enable 0-disable");
            set_appenable(InkeyCount(0));
            break;
        case 8:
            TRACE("\r\n =====当前应用密钥======");
            TRACE("\r\n 1-MMK 2-WK 3-认证WK 4-MK 5-应用信息 6-系统信息区 255-全部");
            buf[0] = InkeyCount(0);
            i = 1;
            PPRT_Debug0(buf,(ushort *)&i);
            break;
        case 9:
            TRACE("\r\n 应用(0~5):");
            i = InkeyCount(0);
            q = (struct KEY_APP *)(KEY_ZONE_ADDR+sizeof(struct KEY_APP)*i);
            TRACE("\r\n keyapp:%x",(uint)q);
            TRACE("\r\n 1-公共密钥 2-工作密钥 3-认证密钥 4-主密钥 255-全部");
            j = InkeyCount(0);
            if ( j == 1 || j == 0xFF) {
                vDispBufTitle("pub",KEY_BLK_LEN*9,0,&q->pub);
            } 
            if (j == 2 || j == 0xFF){
                TRACE("\r\n pink:%x",q->wsk.pink);
                vDispBufTitle("pink",KEY_BLK_LEN*10,0,q->wsk.pink);
                TRACE("\r\n mack:%x",q->wsk.mack);
                vDispBufTitle("mack",KEY_BLK_LEN*10,0,q->wsk.mack);
                TRACE("\r\n desk:%x",q->wsk.desk);
                vDispBufTitle("desk",KEY_BLK_LEN*10,0,q->wsk.desk);
            } 
            if (j == 3 || j == 0xFF){
                TRACE("\r\n auth_mk:%x",q->auth_mk);
                vDispBufTitle("auth_mk",KEY_BLK_LEN*64,0,  q->auth_mk);
                TRACE("\r\n auth_pink:%x",q->auth_wk.pink);
                vDispBufTitle("auth_pink",KEY_BLK_LEN*10,0,q->auth_wk.pink);
                TRACE("\r\n auth_mack:%x",q->auth_wk.mack);
                vDispBufTitle("auth_mack",KEY_BLK_LEN*10,0,q->auth_wk.mack);
                TRACE("\r\n auth_desk:%x",q->auth_wk.desk);
                vDispBufTitle("auth_desk",KEY_BLK_LEN*10,0,q->auth_wk.desk);
            } 
            if (j == 4 || j == 0xFF){
                TRACE("\r\n msk:%x",q->msk);
                vDispBufTitle("msk",KEY_BLK_LEN*64,0,q->msk);
            } 
            if (j == 5 || j == 0xFF){
            }
            break;
        case 10:				
// mode=0  初始化指针
//     =1  清空单个应用密钥区
//     =2  清空公共密钥区+全部应用密钥区
//     =3  初始公共密钥区和应用密钥区
            TRACE("\r\n 0-初始化指针 1-清空单个应用密钥区 2-清空公共密钥区+全部应用密钥区 3-初始公共密钥区和应用密钥区");
            PCI_InitPublicKey(InkeyCount(0));
            break;
        case 11:
#ifdef CFG_DBG_CTRL_KSR 
            dbg_keyzone();
#endif
            break;
        case 12:			
#ifdef CFG_DBG_ICCARD
            vTest_Icc();
#endif
            break;
        case 13:
            while ( 1 ) {
                TRACE("\r\n **********%s********",DEFAULT_V1_APP);
                TRACE("\r\n 1-建立 2-打开 3-删除");
                i = InkeyCount(0); 
                if ( i == 99 ) {
                    break;
                }
                if ( i == 1 ) {
                    ret = create_default_V1APP();
                } else if(i == 2){
                    ret = open_default_V1APPP();
                } else if(i == 3){
                    extern uchar Extendcmd_DelApp(uint inlen, uchar *inbuf);
                    Extendcmd_DelApp(17,DEFAULT_V1_APP);
                }
                TRACE("\r\n ret:%d",ret);
            }
            break;
        case 14:
            TRACE("\r\n 应用号(1~6):");
            i = InkeyCount(0);
            ret = app_del(i);
            TRACE("\r\n ret:%d",ret);
            break;
        case 15:
#ifdef CFG_DBG_DRYICE
            dbg_dryice();
#endif
            break;
        case 99:
            return;
        }
    }
}
#endif
#endif
#endif

