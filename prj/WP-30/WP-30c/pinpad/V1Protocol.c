
/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : V1Protocol.c
 * bfief              : 
 * Author             : yehf()  
 * Email              : yehf@itep.com.cn
 * Version            : V0.00
 * Date               : 10/7/2014 4:21:49 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
//#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
#if 0 

#ifdef CFG_CMD_V1
//******************************
//
//  extern declare 
//
//******************************
extern uint check_keyindex(uchar type, uchar index);
extern uchar PinEncrypt(uint uiPINFormat,uint uiKeyLen,uchar * pucKey,
	uint uiLenIn, uchar * pvCardNo, uchar * pvDataIn, uint * puiOutLen, uchar * pvDataOut);
extern void __LCD_SetPageCol(uint8_t page, uint8_t col);
extern void __LCD_WriteData(uint8_t ucData);
//******************************
//
//  declare 
//
//******************************
uint LoadKey_Check(uchar keylen, uchar *key,uchar checklen, uchar *check);
uchar ParseExtendcmd(void);
uint create_default_V1APP(void);

//******************************
//
//  macro definition     
//
//******************************
//调试开关
#ifdef EM_PinpadStart_Debug
	#define  DEBUG_V1(x)          do{x}while(0)
#else
	#define  DEBUG_V1(x)          
#endif

// command
#define CMD_STX  0x1B

#define CMD_CR   0x0D
#define CMD_LF   0x0A

#define CMD_ACK  0xAA
#define CMD_NAK  0x55

#define EM_ERR_SENDDATA  0xFF //send data

#define  SI_ucTwo2One(ucHigh,ucLow)  (((ucHigh-'0')<<4) | (ucLow-'0'))


#define  EM_DESKEY_LEN   8
#define  EM_3DESKEY_LEN  16

//******************************
//
//  global variables    
//
//******************************
uint32_t  gcV1lcdTimeout=0;
uchar gucTmpProcessKey[24];
uchar gucStartXorBuf[8];

uchar gucPinLen;
uchar gucLastRet;
#define SETRET(x)   do{if(x)TRACE("\r\n-|L:%d %d",__LINE__,x);gucLastRet=x;}while(0)

uint8_t gucCMDBuf[2048+48];
uint8_t gucSrcBuf[1024+16];


#define OLD_MAX_WORKKEY  8
#define MAX_PACKET_LEN   1024

#define  KEY_TYPE_CURKEY     0xFE  //current work key
#define  KEY_TYPE_PROCESSKEY 0xFF  //process key
typedef struct _stMATRIX
{
	unsigned char page;
	unsigned char col;
}stMATRIX;

typedef struct _stRVSTATUS
{
	uchar high;
	uchar low;
}stRV_STATUS;

//******************************
//
//  function   
//
//******************************
void SI_vSendRetStr(uchar ucRet, uint uiLen, uchar *ucBuff)
{	
    uchar data;
#ifdef EM_PinpadStart_Debug	
    uint8 *p = ucBuff;
    uint32_t tmp = uiLen;
    uint32_t i=0;
#endif
    enter_lowerpower_freq();
	if(ucRet == EM_ERR_SENDDATA)
	{
        data = STX;
        UART_Write(&data,1);
        data = HBYTE(uiLen*2);
        UART_Write(&data,1);//send length
        data = LBYTE(uiLen*2);
        UART_Write(&data,1);
		while (uiLen--) //send data
		{
            data = HHALFB(*ucBuff)+'0';
            UART_Write(&data,1);
            data = LHALFB(*ucBuff)+'0';
            UART_Write(&data,1);
			ucBuff++;
		}
        data = ETX;
        UART_Write(&data,1);
#ifdef EM_PinpadStart_Debug	
		TRACE_BUF("v1 ->",p,tmp);
#endif
	}else if(ucRet == 0xFE){
#ifdef EM_PinpadStart_Debug	
		TRACE_BUF("v1 ->",ucBuff,uiLen);
#endif
        data = STX;
        UART_Write(&data,1);
        data = HBYTE(uiLen);
        UART_Write(&data,1);//send length
        data = LBYTE(uiLen);
        UART_Write(&data,1);
        UART_Write(ucBuff,uiLen);
        data = ETX;
        UART_Write(&data,1);
    }
	else
	{
		//Send ACK/NAK
        UART_Write(&ucRet,1);
	}
}

#define SI_vSendACK()       SI_vSendRetStr(CMD_ACK,0,0)  
#define SI_vSendNAK()       SI_vSendRetStr(CMD_NAK,0,0)  
/**********************************************************************
* name 
*     SI_ucParseOldProtocolData
* function 
*   parse cmd from uiLen-length string, and check 0D and 0A 
*	1B cmd x y Z.. 0D 0A from Z parse command 
*   	
* in parameter:
*      uiLen: length of command except 0D and 0A
*      gucCMDBuf[0]=cmd	
*      gucCMDBuf[1]=x 		
*	   gucCMDBuf[2]=y	
* out parameter:
*      from gucCMDBuf[3]
*
* return value
*    0-OK  1-Err
*
***********************************************************************/
uchar SI_ucParseStartCMDData0(uint uiLen)
{
	uint i;
	i = uiLen+2;
	if(UART_Read(gucCMDBuf+3,i,2000) < i)
	{
		return 1;
	}	
    exit_lowerpower_freq();
#ifdef EM_PinpadStart_Debug	
	vDispBufTitle("cmd0",i+3,0,gucCMDBuf);
#endif
	if(gucCMDBuf[3+i-2] == CMD_CR && gucCMDBuf[3+i-1] == CMD_LF)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
/**********************************************************************
* name 
*     SI_ucParseStartCMDData
* function 
*   parse correct commad from max-length string
*	1B cmd x y Z.. 0D 0A from Z parse command 
*   	
* in parameter:
*      uiMaxLen: length of command including 0D and 0A
*      gucCMDBuf[0]=cmd	
*      gucCMDBuf[1]=x 		
*	   gucCMDBuf[2]=y	
* out parameter:
*      from gucCMDBuf[3]
*
* return value
*    0-OK command is 1b+cid+x+y+..+0d+0a			 
*    1-Err 
*    2-command is    1b+cid+x+0d+0a   
***********************************************************************/
uchar SI_ucParseStartCMDData(uint uiMaxLen, uint *uiOutLen)
{
	uint i;
	uchar ucRet= 1,data;
	*uiOutLen = 0;	
	for(i=0;i<uiMaxLen;i++)
	{
		if(!UART_Read(&data,1,100))
		{
			break;
		}
		if(data == CMD_LF)
		{
			if(i == 0)
			{
				if(gucCMDBuf[2] == CMD_CR)
				{
					//1b+cid+x+0d+0a
					ucRet = 2;						
					break;
				}
			}
			else
			{
				if(gucCMDBuf[3+i-1] == CMD_CR)
				{					
					if(i>=uiMaxLen)
					{
						ucRet = 3;
					}
					else
					{
						if(!UART_Read(&data,1,100))
						{
							//1b+cid+x+y+..+0d+0a					
							*uiOutLen = i - 1;	
							ucRet = 0;
							break;
						}
						else
						{
							gucCMDBuf[3+i] = CMD_LF;
							i++;
						}
//                        //解析到1b+cid+x+y+..+0d+0a					
//                        *uiOutLen = i - 1;	
//                        ucRet = 0;
                    }
//                    break;
				}
			}
		}
		gucCMDBuf[3+i] = data;
	}	
    exit_lowerpower_freq();
#ifdef EM_PinpadStart_Debug	
	TRACE("cmd:%d-%d-%d",ucRet,uiMaxLen,*uiOutLen);
	vDispBuf(*uiOutLen+3,0,gucCMDBuf);
#endif	
	return ucRet;
}

uchar SI_ucIfETXErr(uchar ucData1, uchar ucData2)
{
	if(ucData1 == CMD_CR && ucData2 == CMD_LF)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


/*
// 8bytes DES
#define  SA_vDES(key,text,mtext)   des_encrypt_ecb(text,mtext,8,key,1)
#define  SA__vDES(key,text,mtext)  des_decrypt_ecb(text,mtext,8,key,1)
// 24bytes 3DES
#define  SA_vTDESEncrypt(key,len,text,mtext)   des_encrypt_ecb(text,mtext,len,key,3)
#define  SA_vTDESDecrypt(key,len,text,mtext)   des_decrypt_ecb(text,mtext,len,key,3)
*/

#define ASSERT_APP_OPEN() do{if(!gtCurKey.appid){gucLastRet=RESULT_APP_NOTRUN;return 1;}}while(0)
#define ASSERT_APP_V1()   do{if(gAppProcolType!=APP_PROTOCOL_V1){gucLastRet=RESULT_APP_TYPEERR;return 1;}}while(0)
/**********************************************************************
* name 
*     SI_ucEncrypt
* function 
*     使用当前密钥加解密
* in parameter:
*      key:   24 bytes key
*      uiLen: length of data
*      ucInBuf: in data
* out parameter:
*      ucOutBuf: out data
*
* return value
*    0-OK 			 
***********************************************************************/
uchar SI_ucEncrypt(uchar *key, uint inlen, uchar *ucInBuf,uchar *ucOutBuf)
{
	uchar *p=key;
#if 0
	uchar newkey[24];
	if(gtCurKey.scatter)
	{
		des_encrypt_ecb(gucTmpProcessKey, newkey, sizeof(gucTmpProcessKey), key, 3);
#ifdef EM_PinpadStart_Debug		
		vDispBuf(24, 0,key);
		vDispBuf(24, 0,gucTmpProcessKey);
		vDispBuf(24, 0,newkey);
#endif
		keyextend(16,newkey,newkey);
		p = newkey;	
	}
	else
	{
		p = key;
	}
#endif	
	if(gtCurKey.encflg)
	{
		des_decrypt_ecb(ucInBuf,ucOutBuf,inlen,p,3);
	}
	else
	{
		des_encrypt_ecb(ucInBuf,ucOutBuf,inlen,p,3);
	}
#ifdef EM_PinpadStart_Debug			
	vDispBufTitle(NULL,24,0, p);
	vDispBufTitle("text",inlen,0, ucInBuf);
	vDispBufTitle("mtext",inlen,0, ucOutBuf);
#endif		
	return 0;
}
/**********************************************************************
* name 
*     SI_ucEncrypt
* function 
*  display
*   	
*               0               1
* in parameter: line(0x30~0x33)+str
* out parameter:
*
* return value
*    0-OK 		 
***********************************************************************/
uchar SI_ucParseDisplay(uchar inlen, uchar *in)
{
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(in[0] < '0' || in[0] > '4')
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}	
	if(inlen == 0)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
    if ( inlen > 21 ) {
        inlen  = 21;
    }
	in[inlen+1] = 0;
	in[inlen+2] = 0;
 	pinpad_Display(in[0]-'0'+1, (char *)&in[1]);
#endif	
	return 0;
}

uchar SI_ucTimeoutDisplay(void)
{
    uchar ucRet = 0;
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
    uchar j;
    uint32_t i=0;
    // 超时显示 ESC+L+行数Row+超时时间T（秒）+ST
    // 1b 4C row T1 T2 st 0d 0a
    //     0   1  2 3  4
    // row st不拆  T拆分
    ucRet  = SI_ucParseStartCMDData(2+21-1+2,&i);
    if(!ucRet)
    {
		if(i <= 1)
		{
			//收到 1b 4c row T1 T2 0d 0a
			return 1;
		}
		gucCMDBuf[1] -= '0';
		if(gucCMDBuf[1] > 4)
		{
			return 1;
		}
		if(gucCMDBuf[2]< 0x30 || gucCMDBuf[2] > 0x3F)
		{
			return 1;
		}
		if(gucCMDBuf[3]< 0x30 || gucCMDBuf[3] > 0x3F)
		{
			return 1;
		}
		++gucCMDBuf[1];
		//超时显示时间
		j = SI_ucTwo2One(gucCMDBuf[2],gucCMDBuf[3]);
		if(j == 0)
		{
			return 1;
		}
		//字符数
		--i;
		//结束符
		gucCMDBuf[4+i] = 0;
        pinpad_Display(gucCMDBuf[1], (char *)(gucCMDBuf+4));
		gcV1lcdTimeout = j*1000+sys_get_counter();
    }
    else
    {
        SETRET(RESULT_ERROR_PARA);
        ucRet = 1;
    }
#endif
    return ucRet;
}


//mode:1-超时显示立即返回待机界面 0-超时显示等待超时
void v1protocol_timeout_daemon(uint32_t mode)
{
    if (gcV1lcdTimeout == 0)
    {
        DispScreen(1);
    } else 
    {
        if (mode == 1) {
            gcV1lcdTimeout = 0;
        }
        if (gcV1lcdTimeout <= sys_get_counter()) 
        {
            DispScreen(0);
            gcV1lcdTimeout = 0;
        }
    }
}


// workkey map                       
// 
// mode=0  only ID:0~7
// index  0~3        4~5         6~7    
// type   PINK0~3    DESK0~1     MACK0~1
uchar workkey_map(uint mk, uchar id, uchar *type, uchar *index)
{
	if(mk == 15 && id == 0)
	{
		*type = KEY_TYPE_PINK;
		*index = 4;
		return 0;
	}
	else
	{
		if(id <= 3)
		{
			*type = KEY_TYPE_PINK;
			*index = id;
			return 0;
		}
		else if(id <= 5)
		{
			*type = KEY_TYPE_DESK;
			*index = id-4;
			return 0;
		}
		else if(id <= 7)
		{
			*type = KEY_TYPE_MACK;
			*index = id-6;
			return 0;
		}
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
}

uchar s_check_keyindex(uchar type, uchar index)
{
	if(check_keyindex(type,index))
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	return 0;
}

uchar ActivateSecretGroup(void)
{
	uint iLen;
	uchar ucMkNum,ucWkNum;
	uchar type,index;
	if(!SI_ucParseStartCMDData(2+2,&iLen))
	{	
		ASSERT_APP_V1();
		ASSERT_APP_OPEN();
		if(iLen == 2)
		{
			ucMkNum = SI_ucTwo2One(gucCMDBuf[1],gucCMDBuf[2]);
			ucWkNum = SI_ucTwo2One(gucCMDBuf[3],gucCMDBuf[4]);
			if(s_check_keyindex(KEY_TYPE_MK,ucMkNum))
			{
				return 1;
			}
			if(workkey_map(ucMkNum,ucWkNum,&type,&index))
			{
				return 1;
			}
			gtCurKey.MKIndex = ucMkNum;
			gtCurKey.AuthType = type;
			gtCurKey.AuthIndex = index;
			return 0;
		}
		else
		{
			SETRET(RESULT_ERROR_PARA);
		}
	}
	return 1;
}
//read key(including processkey and current work key(scatter))
uint s_PCI_ReadKey(uchar type,uchar index, uchar *outkey)
{
	uchar len;
	uchar key[24];

	if(type == KEY_TYPE_MK)
	{
		if (PCI_ReadCurrentKey(type, index, &len, outkey))
		{
			SETRET(RESULT_KEY_ERROR);
			return 1;
		}
	}
	else 
	{
		if(index == KEY_TYPE_CURKEY)
		{
			if (PCI_ReadCurrentKey(gtCurKey.AuthType, gtCurKey.AuthIndex, &len, key))
			{
				SETRET(RESULT_KEY_ERROR);
				return 1;
			}
			if(gtCurKey.scatter)
			{
				des_encrypt_ecb(gucTmpProcessKey, outkey, sizeof(gucTmpProcessKey), key, 3);
#ifdef EM_PinpadStart_Debug		
				vDispBuf(24, 0,key);
				vDispBuf(24, 0,gucTmpProcessKey);
				vDispBuf(24, 0,outkey);
#endif
			} else {
                memcpy(outkey,key,len);
            }
			CLRBUF(key);
		}
		else if(index == KEY_TYPE_PROCESSKEY)
		{
			memcpy(outkey,gucTmpProcessKey,sizeof(gucTmpProcessKey));
		}
		else
		{
			if (PCI_ReadCurrentKey(type, index, &len, outkey))
			{
				SETRET(RESULT_KEY_ERROR);
				return 1;
			}
		}				
	}
	DEBUG_V1(		
		TRACE("\nkey:%d-%d",type,index);
		vDispBufTitle(NULL, 24, 0, outkey);
	);
	return 0;
}

uint32_t s_get_scatter_key(uint8_t type,uint8_t index,uint8_t *key)
{
    if (gtCurKey.scatter 
        && gtCurKey.AuthType ==  type 
        && gtCurKey.AuthIndex == index)
    {
        index = KEY_TYPE_CURKEY;
    }
    return s_PCI_ReadKey(type, index, key);
}

uchar get_curworkkey(uchar *key)
{
	uchar tmpkey[24];
	if(gtCurKey.AuthType < KEY_TYPE_PINK || gtCurKey.AuthType > KEY_TYPE_DESK)
	{
		return 1;
	}
	if (s_PCI_ReadKey(gtCurKey.AuthType, gtCurKey.AuthIndex, key))
	{
		return 1;
	}
	if(gtCurKey.scatter)
	{
 		des_encrypt_ecb(gucTmpProcessKey,tmpkey, sizeof(gucTmpProcessKey),key,3);
		memcpy(key,tmpkey,24);
		CLRBUF(tmpkey);
	}
	DEBUG_V1(		
		TRACE("Cur:%d-%d",gtCurKey.AuthType,gtCurKey.AuthIndex);
		vDispBufTitle(NULL, 24, 0, key);
	);
	return 0;
}
/**********************************************************************
* name 
*     EnCryptData
* function 
*     fixed 16bytes data encrypt 
* 	  1b + H + ST(16B) + 0d + 0a 
*               
* in parameter: 0               1
*               line(0x30~0x33)+str
* out parameter:
*
* return value
*    0xff-OK   1-ERR		 
***********************************************************************/
uchar EnCryptData(void)
{
	uchar key[24];
	if(!SI_ucParseStartCMDData0(14))
	{
		ASSERT_APP_V1();
		ASSERT_APP_OPEN();	
		if(get_curworkkey(key))
		{
			return 1;
		}	
		SI_ucEncrypt(key, 16, &gucCMDBuf[1], gucSrcBuf);
		SI_vSendRetStr(EM_ERR_SENDDATA,16,gucSrcBuf);
		CLRBUF(key);
		return EM_ERR_SENDDATA;
	}
	return 1;
}
/**********************************************************************
* name 
*     UionPayII_EnCryptMac
* function 
*     calculate Mac (Algorithm 0)
*               
* in parameter: 
*    ucMode:0-DES  1-3DES
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar SI_GetMacX99(uchar ucMode,uchar  *ucKey, uint uiInLen,uchar  *ucInbuf,uchar  *ucOutBuf)
{
#if 2
	uint i=0;
	uchar j,m,n;
    uchar ucIBuf[8],ucOBuf[8];
	memset(ucIBuf,0,sizeof(ucIBuf));
	memset(ucOBuf,0,sizeof(ucOBuf));
	memcpy(ucOBuf, gucStartXorBuf, 8);
	m = uiInLen/8;
	n = uiInLen%8;
	while(m--)
	{
		for(j=0;j<8;j++)
		{
			ucIBuf[j] = ucInbuf[i+j] ^ ucOBuf[j];
		}
		i += 8;
		if(!ucMode)
		{
			des_encrypt_ecb(ucIBuf,ucOBuf,8,ucKey,1);
		}
		else
		{
			des_encrypt_ecb(ucIBuf,ucOBuf,8,ucKey,3);
		}
	}
	vDispBufTitle("01", 8, 0, ucOBuf);
	if(n)
	{
		for(j=0;j<n;j++)
		{
			ucIBuf[j] = ucInbuf[i+j] ^ ucOBuf[j];
		}
		for(;j<8;j++)
		{
			ucIBuf[j] = ucOBuf[j];
		}
		if(!ucMode)
		{
			des_encrypt_ecb(ucIBuf,ucOBuf,8,ucKey,1);
		}
		else
		{
			des_encrypt_ecb(ucIBuf,ucOBuf,8,ucKey,3);
		}
	}
	vDispBufTitle("02", 8, 0, ucOBuf);
	memcpy(ucOutBuf,ucOBuf,8);
	return 0;
#else
	// 算法1 模式1
	ulong len; 
	return CalculateMac(0, 24, ucKey, uiInLen, ucInbuf, &len, ucOutBuf);
#endif
} 
/**********************************************************************
* name 
*     SI_GetAlgorithmTwo
* function 
*     calculate Mac (Algorithm 2)
*               
* in parameter: 
*    
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar SI_GetAlgorithmTwo(uchar  *ucKey, uint uiInLen,uchar  *ucInbuf,uchar  *ucOutBuf)
{
	uchar ucTemBuf[8];
	SI_GetMacX99(0,ucKey,uiInLen,ucInbuf,ucOutBuf);
	vDispBufTitle("04", 8, 0, ucOutBuf);
	des_decrypt_ecb(ucOutBuf,ucTemBuf,8,ucKey,1);
	des_encrypt_ecb(ucTemBuf,ucOutBuf,8,ucKey,3);
	return 0;
} 
/**********************************************************************
* name 
*     SI_GetMacECB
* function 
*     calculate Mac (Algorithm 1 and 3)
*               
* in parameter: 
*     mode=0   standard ECB Algorithm 1
*     mode=1   extended ECB Algorithm 3
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar SI_GetMacECB(uchar ucMode,uchar  *ucKey, uint uiInLen,uchar  *ucInbuf,uchar  *ucOutBuf)
{
	uint i=0;
	uchar j,m,n;
	 uchar ucIBuf[16],ucOBuf[16];
	memset(ucIBuf,0,sizeof(ucIBuf));
	memset(ucOBuf,0,sizeof(ucOBuf));
	m = uiInLen/8;
	n = uiInLen%8;
#ifdef EM_PinpadStart_Debug	
	TRACE("ECB %d %d",m,n);
	TRACE("Len:%d",uiInLen);
	vDispBuf(24,0,ucKey);
#endif		
	while(m--)
	{
		for(j=0;j<8;j++)
		{
			ucIBuf[j] ^= ucInbuf[i+j];
		}
#ifdef EM_PinpadStart_Debug	
		TRACE("%d",i);
		vDispBuf(8, 0,&ucInbuf[i]);
		vDispBuf(8, 0,ucIBuf);
#endif	
		i += 8;
	}
	if(n)
	{
		for(j=0;j<n;j++)
		{
			ucIBuf[j] ^= ucInbuf[i+j];
		}
#ifdef EM_PinpadStart_Debug	
		TRACE("%d",i);
		vDispBuf(n, 0,&ucInbuf[i]);
		TRACE("Xor:");
		vDispBuf(8, 0,gucStartXorBuf);
#endif							
	}
	vDispBuf(8, 0,gucStartXorBuf);
	for(j=0; j<8; j++)
	{
		ucIBuf[j]^=gucStartXorBuf[j];
	}
	vDispBufTitle("03", 8, 0, ucIBuf);
	if(ucMode == 0)
	{
		des_encrypt_ecb(ucIBuf,ucOBuf,8,ucKey,3);
		memcpy(ucOutBuf,ucOBuf,8);
	}
	else
	{
		// 1. 8bytes Xor -> 16 bytes Xor
		SI_ucParseStr(2,8,ucIBuf,ucOBuf);
		// 2. encrypt 1st-8-bytes 
		des_encrypt_ecb(ucOBuf,ucIBuf,8,ucKey,3);
		// 3. Xor  2rd-8-bytes
		for(j=0;j<8;j++)
		{
			ucIBuf[j] ^= ucOBuf[8+j];
		}
		// 4. encrypt Xor result
		des_encrypt_ecb(ucIBuf,ucOBuf,8,ucKey,3);
		// 5. 8bytes Xor -> 16 bytes Xor
		SI_ucParseStr(2,8,ucOBuf,ucIBuf);
		// 6. 1st-8-bytes is MAC
		memcpy(ucOutBuf,ucIBuf,8);
	}
	return 0;
}
/**********************************************************************
* name 
*     SI_vGetMacX919
* function 
*     calculate Mac (Algorithm 4 and 5)
*               
* in parameter: 
*     mode=4   Calculate 1st step
*     mode=5   Calculate 2rd step
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar SI_GetMacX919(uchar ucMode,uchar  *ucKey, uint uiInLen,uchar  *ucInbuf,uchar  *ucOutBuf)
{
	uint i,j;
	uchar ucIBuf[16];	
#ifdef EM_PinpadStart_Debug	
	TRACE("X919 %d-%d",ucMode,uiInLen);
	vDispBuf(uiInLen, 0, ucInbuf);
#endif		
	for(i=0;i<(uiInLen/8);i++)
	{
		for(j=0;j<8;j++)
		{
			ucIBuf[j] = gucStartXorBuf[j] ^ ucInbuf[i*8+j]; 
		}
		des_encrypt_ecb(ucIBuf,gucStartXorBuf,8,ucKey,1);
	#ifdef EM_PinpadStart_Debug	
		vDispBufTitle("In",8, 0, ucIBuf);
		vDispBufTitle("Out",8, 0, gucStartXorBuf);
	#endif		
	}
	if(ucMode == 5)
	{
		// 1. result descrypted by later 8 bytes 
		des_decrypt_ecb(gucStartXorBuf,ucIBuf,8,&ucKey[8],1);
	#ifdef EM_PinpadStart_Debug		
		vDispBufTitle("In1",8, 0, ucIBuf);
	#endif		
		// 2. reslut encrypted by front 8 bytes 
	#ifdef EM_PinpadStart_Debug	
		vDispBufTitle("Out1",8, 0, gucStartXorBuf);
	#endif
		des_encrypt_ecb(ucIBuf,gucStartXorBuf,8,ucKey,1);			
	}
	memcpy(ucOutBuf,gucStartXorBuf,8);	
	return 0;
}
/**********************************************************************
* name 
*     UionPayII_EnCryptMac
* function 
*     calculate Mac 
*     1b + '\x22'+ ID + Mode + Len(2B) + Data + 0D + 0A  
* 
* in parameter: 
*    
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar UionPayII_EnCryptMac(void)
{
	uchar ucRet,key[24];
	uint uiLen;
    //           0     1    2      3         5  
    //Mac:1b + '\x22'+ ID + Mode + Len(2B) + Data + 0D + 0A
    if(UART_Read(&gucCMDBuf[3], 2, 200) < 2)
    {
		return 1;
    }
	uiLen = COMBINE16(gucCMDBuf[3],gucCMDBuf[4]);
	if(UART_Read(&gucCMDBuf[5],uiLen+2,2000) < (uiLen+2))
	{
		return 1;
	}
    exit_lowerpower_freq();
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if (uiLen > MAX_PACKET_LEN || uiLen == 0 
		|| gucCMDBuf[5+uiLen] != CMD_CR || gucCMDBuf[5+uiLen+1] != CMD_LF)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	if(gucCMDBuf[1] != KEY_TYPE_CURKEY && gucCMDBuf[1] != KEY_TYPE_PROCESSKEY)
	{
		if(s_check_keyindex(KEY_TYPE_MACK,gucCMDBuf[1]))
		{
			return 1;
		} 
	}
	if (s_get_scatter_key(KEY_TYPE_MACK, gucCMDBuf[1], key))
	{
		return 1;
	}
	switch (gucCMDBuf[2])	
	{
		case 0:
			ucRet = SI_GetMacX99(1,key,uiLen,&gucCMDBuf[5],gucSrcBuf);	
			break;
		case 1:
			ucRet = SI_GetMacECB(0,key,uiLen,&gucCMDBuf[5],gucSrcBuf);
			break;
		case 2:
			ucRet = SI_GetAlgorithmTwo(key,uiLen,&gucCMDBuf[5],gucSrcBuf);
			break;
		case 3:
			ucRet = SI_GetMacECB(1,key,uiLen,&gucCMDBuf[5],gucSrcBuf);
			break;
		case 4:
		case 5:
			if (uiLen%8 == 0)
			{
				ucRet = SI_GetMacX919(gucCMDBuf[2],key,uiLen,&gucCMDBuf[5],gucSrcBuf);	
			}		
			else
			{
				SETRET(RESULT_ERROR_PARA);
				ucRet = 1;
			}
			break;
		default:
			SETRET(RESULT_ERROR_PARA);
			ucRet = 1;
			break;
	}
	CLRBUF(key);
	if (ucRet == 0)
	{
		SI_vSendRetStr(EM_ERR_SENDDATA,8,gucSrcBuf);
		return EM_ERR_SENDDATA;		
	}
//EnCryptMac_Out:
	return 1;
}
/**********************************************************************
* name 
*     UionPayII_EncryptKeyWithCardNO
* function 
*     inline PIN entry with cardno,timeout and display
*     1b+'\x20'+ID+Min+Max+EncryMode+Time+CardNo(12B)+DispMode+Str(0~21)+0D+0A
* 
* in parameter: 
*    
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar UionPayII_EncryptKeyWithCardNO(void)
{
	uchar ucRet,pinStr[16],pink[24];
        //uchar type,index,tempkey[24];
	uint uiLen;
    //PINK 1b+'\x20'+ID+Min+Max+EncryMode+Time+CardNo(12B)+DispMode+Str(0~21)+0D+0A
    //         [0]  [1] [2] [3]  [4]       [5]  [6]~[17]   [18]     [19]
    //                       7 - 4 + 12B+(0~21)B+2B=17B~38B 
    //1+18+21+2=42-3=39
    if(SI_ucParseStartCMDData(39,&uiLen) == 0)
    {
    	ASSERT_APP_V1();
		ASSERT_APP_OPEN();
    	if(uiLen >= 14)
    	{
    		if(gucCMDBuf[5] == 0)
			{
				gucCMDBuf[5] = 120;
			}
    		if(gucCMDBuf[3] > MAX_PIN_LEN || gucCMDBuf[2] > gucCMDBuf[3] 
				|| gucCMDBuf[2] < MIN_PCIPIN_LEN || gucCMDBuf[3] < MIN_PCIPIN_LEN 
				|| (gucCMDBuf[4]&0x7F)!=0 || gucCMDBuf[18]>1 
				|| gucCMDBuf[5]>120 || gucCMDBuf[5]<30)
    		{
    			//TRACE("%d-%d %d %d %d",gucCMDBuf[2],gucCMDBuf[3],gucCMDBuf[4],gucCMDBuf[5],gucCMDBuf[18]);
    			SETRET(RESULT_ERROR_PARA);
    			return 1;
    		}  
			if(gucCMDBuf[1] != KEY_TYPE_CURKEY && gucCMDBuf[1] != KEY_TYPE_PROCESSKEY)
			{
				if(s_check_keyindex(KEY_TYPE_PINK,gucCMDBuf[1]))
				{
					return 1;
				}
			}
            
		#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
			if(gucCMDBuf[18])
			{
				gucCMDBuf[uiLen+3] = 0;
				pinpad_Display(1, (char *)&gucCMDBuf[19]);
			}
			LCD_ShowPINPrompt();							
			ucRet = KB_GetPINStr(3, gucCMDBuf[2], gucCMDBuf[3], (uint32_t)(gucCMDBuf[5]*1000),(uint32_t *)&uiLen, pinStr);
		#elif PRODUCT_TYPE == PRODUCT_TYPE_F12	
			gInPinEntry = 1;
			LCD_ShowPINPrompt();							
			ucRet = KB_GetPINStr(4, gucCMDBuf[2], gucCMDBuf[3], (uint32_t)(gucCMDBuf[5]*1000),(uint32_t *)&uiLen, pinStr);
			gInPinEntry = 0;
		#endif
           
			if(ucRet == 1)
			{
				// 1
				SETRET(RESULT_INPUT_CANCEL);
				return 1;
			}
			else if (ucRet == 2)
			{
				// 2
				SETRET(RESULT_INPUT_TIMEOUT);
				return 1;
			}
            if(uiLen == 0)
            {
                CLRBUF(gucSrcBuf);
                SI_vSendRetStr(EM_ERR_SENDDATA, 0, gucSrcBuf);
                return EM_ERR_SENDDATA;
            }    

			if (s_get_scatter_key(KEY_TYPE_PINK, gucCMDBuf[1], pink))
			{
				return 1;
			}
			PinEncrypt(1,(uint)24,pink,uiLen,&gucCMDBuf[6],pinStr,&uiLen,gucSrcBuf);
            if (gtCurKey.encflg)//解密
            {
                des_decrypt_ecb(gucSrcBuf,gucSrcBuf,uiLen,pink,24/8);
                des_decrypt_ecb(gucSrcBuf,gucSrcBuf,uiLen,pink,24/8);
            }
			CLRBUF(pinStr);
			CLRBUF(pink);
			SI_vSendRetStr(EM_ERR_SENDDATA,8,gucSrcBuf);
			return EM_ERR_SENDDATA;		
    	}				
    }
	SETRET(RESULT_ERROR_PARA);
	return 1;
}
/**********************************************************************
* name 
*     UionPayII_EncryptTrack
* function 
*    
*     1b + '\x21'+ ID + Len(2B) + Data + 0D + 0A
* 
* in parameter: 
*    
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar UionPayII_EncryptTrack(void)
{
	uchar key[24];
	uint uiLen;
    
    // 1b + '\x21'+ ID + Len(2B) + Data + 0D + 0A
    if(UART_Read(gucCMDBuf+3,1,200))
    {
    	uiLen = COMBINE16(gucCMDBuf[2], gucCMDBuf[3]);
		if (uiLen > MAX_PACKET_LEN || uiLen%8!=0 || uiLen == 0)
		{
			UART_Read(gucCMDBuf,uiLen,2000);
			return 1;
		}
		else
		{
		    // read data and 0d 0a
		    // 21 00 00 10 11 22 33 44 55 66 77 88 99 00 11 22 33 44 55 66 0d 0a
		    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21
			if(UART_Read(gucCMDBuf+4,uiLen+2,2000) < (uiLen+2))
			{
				return 1;
			}	
            exit_lowerpower_freq();
			ASSERT_APP_V1();
			ASSERT_APP_OPEN();
			if(gucCMDBuf[4+uiLen] != CMD_CR || gucCMDBuf[4+uiLen+1] != CMD_LF)
			{
				SETRET(RESULT_ERROR_PARA);
				return 1;
			}		
			if(gucCMDBuf[1] != KEY_TYPE_CURKEY && gucCMDBuf[1] != KEY_TYPE_PROCESSKEY)
			{
				if(s_check_keyindex(KEY_TYPE_DESK,gucCMDBuf[1]))
	    		{
	    			return 1;
	    		} 
			} 
			if (s_get_scatter_key(KEY_TYPE_DESK, gucCMDBuf[1], key))
			{
				return 1;
			}		
            //vDispBufTitle("in", uiLen, 0, &gucCMDBuf[4]);
//			des_encrypt_ecb(&gucCMDBuf[4],gucSrcBuf,uiLen,key,3);
            SI_ucEncrypt(key,uiLen,&gucCMDBuf[4],gucSrcBuf);
            //vDispBufTitle("out", uiLen, 0, gucSrcBuf);
			CLRBUF(key);
			SI_vSendRetStr(EM_ERR_SENDDATA,uiLen,gucSrcBuf);
			return EM_ERR_SENDDATA;
		}
    }
	return 1;
}

/**********************************************************************
* name 
*     UionPayII_CreateProSecretKey
* function 
*     make process key
*     1b + 2D + Mode + Len + Data + 0D + 0A
* 
* in parameter: 
*     Len:8/16 bytes
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar UionPayII_CreateProSecretKey(void)
{
	uint j;
	uchar key[24];
	//  
	// 1b + 2D + Mode + Len(8/16) + Data + 0D + 0A
	//      0     1     2	        3			
	if(SI_ucParseStartCMDData0(gucCMDBuf[2]) == 0)
	{
		if(gucCMDBuf[2] != 8 && gucCMDBuf[2] != 16)
		{
			SETRET(RESULT_ERROR_PARA);
			return 1;
		}
		if(get_curworkkey(key))
		{
			return 1;
		}		
		if(gucCMDBuf[2] == 8)
		{
			memcpy(&gucCMDBuf[3+8],&gucCMDBuf[3],8);
			SI_ucEncrypt(key,16,&gucCMDBuf[3],gucTmpProcessKey);
			CLRBUF(key);
			keyextend(16,gucTmpProcessKey,gucTmpProcessKey);
			return 0;				
		} 
		else
		{	
			SI_ucEncrypt(key,16,&gucCMDBuf[3],gucTmpProcessKey);
			CLRBUF(key);
			if(gucCMDBuf[1] == 1)
			{
				for(j=0;j<8;j++)
				{
					gucTmpProcessKey[j] = gucTmpProcessKey[8+j] ^ gucTmpProcessKey[j];
				}
				memcpy(&gucTmpProcessKey[8],&gucTmpProcessKey[0],8);
			}
			keyextend(16,gucTmpProcessKey,gucTmpProcessKey);
			return 0;
		}								
	}	
	return 1;
}

/**********************************************************************
* name 
*     UionPayII_ActivScatter
* function 
*     Scatter activate work key
*     1b+\x28+MK+WK+Mode+0d+0a
* 
* in parameter: 
*     Mode: D0 1-scatter D1:0-encrypt 1-descrypt
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar UionPayII_ActivScatter(void)
{
	uchar type,index;
	uint i;
	// 1b+\x28+MK+WK+Mode+0d+0a
	if (SI_ucParseStartCMDData(1+2,&i) == 0)
	{
		ASSERT_APP_V1();
		ASSERT_APP_OPEN();		
		if (i != 1)
		{
			SETRET(RESULT_ERROR_PARA);
			return 1;
		}
		if(s_check_keyindex(KEY_TYPE_MK,gucCMDBuf[1]))
		{
			return 1;
		}
		if(workkey_map(gucCMDBuf[1],gucCMDBuf[2],&type,&index))
		{
			return 1;
		}
		gtCurKey.MKIndex = gucCMDBuf[1];
		gtCurKey.AuthType = type;
		gtCurKey.AuthIndex = index;
		gtCurKey.scatter = GETBIT(gucCMDBuf[3], 0);
		gtCurKey.encflg = GETBIT(gucCMDBuf[3], 1);
		return 0;
	}
	return 1;
}
/**********************************************************************
* name 
*     SI_LoadRandom
* function 
*     1b+\x29+Len(8/16)+str+0d+0a	
* 
* in parameter: 
*
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar SI_LoadRandom(void)
{
	uint i;
	if(SI_ucParseStartCMDData0(gucCMDBuf[1]-1) == 0)
	{
		if(gucCMDBuf[1] == 8)
		{
			for(i=0;i<8;i++)
			{
				gucTmpProcessKey[i] = gucCMDBuf[2+i];
				gucTmpProcessKey[i+16] = gucCMDBuf[2+i];
				gucTmpProcessKey[i+8] = ~gucCMDBuf[2+i];
			}
			return 0;					
		}
		else if(gucCMDBuf[1] == 16)
		{
			memcpy(gucTmpProcessKey,&gucCMDBuf[2],16);
			memcpy(&gucTmpProcessKey[16],gucTmpProcessKey,8);
			return 0;	
		}
		else
		{
			SETRET(RESULT_ERROR_PARA);
		}
	}	
	return 1;
}
/**********************************************************************
* name 
*     SI_ucPinpadLoadTMK
* function 
*     1b+\x29+Len(8/16)+str+0d+0a	
* 
* in parameter: 
*
* out parameter:
*
* return value
*    0-OK  1-para err 2-flash err 	 
***********************************************************************/
uchar SI_ucPinpadLoadTMK(uchar ucNo, uchar ucLen, uchar *ucInBuf)
{
	uchar key[KEY_UNIT_LEN];
    if(s_check_keyindex(KEY_TYPE_MK,ucNo))
    {
    	return 1;
    }
	if(ucLen != 8 && ucLen !=16 && ucLen !=24)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	keyextend(ucLen, ucInBuf, key);		
#ifdef EM_PinpadStart_Debug
	vDispBufTitle("MK",KEY_UNIT_LEN,0,key);				
#endif	
	if(PCI_WriteCurrentKey(KEY_TYPE_MK, ucNo, KEY_UNIT_LEN,key))
	{
		SETRET(RESULT_ERROR_FLASH);
		return 2;
	}
	return 0;
}

/**********************************************************************
* name 
*     SI_ucPinpadLoadKey
* function 
* 
* in parameter: 
*       ucKeyType:1-need check 0-not check
*		ucTMKNo:  mk index
*       ucNo:     work key index
*       ucLen:    work key len
*       ucInBuf:  work key
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
uchar SI_ucPinpadLoadKey(uchar checklen, uchar ucTMKNo, uchar type, uchar index, uchar ucLen,uchar  *ucInBuf)
{	
	uchar  ucKey[24],mk[24];
	if(s_check_keyindex(KEY_TYPE_MK,ucTMKNo) || s_check_keyindex(type, index))
	{
		return 1;
	}
 
	if (s_PCI_ReadKey(KEY_TYPE_MK, ucTMKNo,  mk))
	{
		return 1;
	}
	des_decrypt_ecb(ucInBuf,ucKey,ucLen,mk,3);
#ifdef EM_PinpadStart_Debug
	TRACE("%d %d %d-%d %d",checklen,ucTMKNo,type,index,ucLen);
	vDispBufTitle("MK",KEY_UNIT_LEN,0,mk);
	vDispBufTitle("WK",ucLen,0,ucKey);
#endif		
	if(checklen)
	{
		if(LoadKey_Check(ucLen,ucKey,checklen,&ucInBuf[ucLen]))
		{
			return 1;
		}
	}
	keyextend(ucLen, ucKey, ucKey);	
	PCI_WriteCurrentKey(type, index, KEY_UNIT_LEN, ucKey);
	return 0;	
}

void SI_vResetPinpad(void)
{
	gtCurKey.AuthType = KEY_TYPE_PINK;
	gtCurKey.AuthIndex = 0;
	gtCurKey.MKIndex = 1;
	gtCurKey.encflg = 0;
	gtCurKey.scatter = 0;
	gtCurKey.pinlen = 0;
	memset(gucTmpProcessKey,0,sizeof(gucTmpProcessKey));
	memset(gucStartXorBuf,0,sizeof(gucStartXorBuf));
	memset(gtCurKey.aucRand,0,sizeof(gtCurKey.aucRand));
}
uchar SI_ucRenewDefault(void)
{
#if 0
	uint i,j;
	uchar key[KEY_UNIT_LEN];
#else
    uchar ret;
#endif
    TRACE("\nCreate Default key");
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
    exit_lowerpower_freq();
	SI_vResetPinpad();	
#if 0
	memset(key,'8',KEY_UNIT_LEN);	
	for(i=0,j=0;i<NUM_MK;i++)
//	for(i=0;i<1;i++)
	{	
        PCI_WriteCurrentKey(KEY_TYPE_MK,i,KEY_UNIT_LEN,key);
	}	
	memset(key,0,KEY_UNIT_LEN);
	for(j=KEY_TYPE_PINK;j<=KEY_TYPE_DESK;j++)
	{
		for(i=0;i<NUM_PINK;i++)
//        for(i=0;i<1;i++)
		{	
			PCI_WriteCurrentKey(j, i,KEY_UNIT_LEN,key);
		}
	}
#else
    ret = (uchar)fast_ksr_write_key(gtCurKey.appid,0);
    if ( !ret ) {
        ret = (uchar)fast_ksr_write_key(gtCurKey.appid,1);
    }
#endif
	return 0;
}

uchar EncryptKeyWithoutCardNO(void)
{
	uchar ucRet,keylen,pinStr[16],pink[24];
	uint uiLen;
    //1b 56 TW EW 0D 0A
    if(SI_ucParseStartCMDData(2,&uiLen))
	{
		return 1;
	}
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	//TRACE("%d-%x-%x",uiLen,gucCMDBuf[1],gucCMDBuf[2]);
	if(uiLen != 0 || gucCMDBuf[1] != 0x30 || gucCMDBuf[2] < 0x30 || gucCMDBuf[2] > 0x33)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}  
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
	LCD_ShowPINPrompt();	
	ucRet = KB_GetPINStr(3, gtCurKey.pinlen, MAX_PIN_LEN, -1,(uint32_t *)&uiLen, pinStr);
#elif PRODUCT_TYPE == PRODUCT_TYPE_F12		
	gInPinEntry = 1;
	LCD_ShowPINPrompt();	
	ucRet = KB_GetPINStr(4, gtCurKey.pinlen, MAX_PIN_LEN, -1,(uint32_t *)&uiLen, pinStr);
	gInPinEntry = 0;
#endif
	if(!ucRet)
	{
		if(uiLen == 0)
		{
			SI_vSendRetStr(EM_ERR_SENDDATA,0,gucCMDBuf);
			return 0xff;
		}
		else
		{
						
			if (get_curworkkey(pink))
			{
				CLRBUF(pinStr);
				return 1;
			}
			memset(gucSrcBuf,0,16);		
			memcpy(gucSrcBuf,pinStr,uiLen);
			keylen = ((uiLen + 7)>>3)<<3;
			SI_ucEncrypt(pink, 16, gucSrcBuf, &gucSrcBuf[16]);
			CLRBUF(pinStr);
			CLRBUF(pink);
			SI_vSendRetStr(EM_ERR_SENDDATA,keylen,&gucSrcBuf[16]);
			return 0xff;
		}
	}	   
	return 1;
}

uchar DownLoadWorkSecretKey(void)
{
	uint iLen;
	uchar ucMkNum,ucWkNum,type,index;
	//     0   1 2 3 4 5
	//ESC+0x53+MNO+WNO+WK(8/16)+0D+0A
 	if(!SI_ucParseStartCMDData(2*2+EM_3DESKEY_LEN*2+2,&iLen))
	{
		ASSERT_APP_V1();
		ASSERT_APP_OPEN();
		iLen -= 2;
		if(iLen == EM_DESKEY_LEN*2 || iLen == EM_3DESKEY_LEN*2)
		{
			ucMkNum = SI_ucTwo2One(gucCMDBuf[1],gucCMDBuf[2]);
			ucWkNum = SI_ucTwo2One(gucCMDBuf[3],gucCMDBuf[4]);
			if(workkey_map(ucMkNum,ucWkNum,&type,&index))
			{				
				return 1;
			}
			iLen = SI_ucParseStr(1,iLen,&gucCMDBuf[5],gucSrcBuf);
			return SI_ucPinpadLoadKey(0,ucMkNum,type,index,iLen,gucSrcBuf);
		}
		else
		{
			SETRET(RESULT_ERROR_PARA);
		}
	}
	return 1;
}

uchar SI_cls(void)
{
	if(!SI_ucIfETXErr(gucCMDBuf[1],gucCMDBuf[2]))	
	{
		ASSERT_APP_V1();
		ASSERT_APP_OPEN();
		LCD_ClearScreen();	
		return 0;
	}	
	else
	{
		return 1;
	}	
}
uchar SI_SetDesFlg(void)
{
	uint j;
	if(SI_ucParseStartCMDData(1,&j) == 2)
	{
		ASSERT_APP_V1();
		ASSERT_APP_OPEN();		
		return 0;
	}
	else
	{
		return 1;
	}
}

uchar SI_GetSN(void)
{
	//read SN 
	//1b 23 0d 0a  
	uint8_t j;
	if(!SI_ucIfETXErr(gucCMDBuf[1],gucCMDBuf[2]))	
	{
		ASSERT_APP_V1();
		ASSERT_APP_OPEN();
		if(!read_sn(gtCurKey.appid,(uchar *)&j,gucSrcBuf))
		{
			SI_vSendRetStr(EM_ERR_SENDDATA,j,gucSrcBuf);
			return EM_ERR_SENDDATA;
		}	
		SETRET(RESULT_KEY_ERROR);
	}	
	return 1;	
}

uchar SI_SetSN(void)
{
	//load SN  0  1         2
	//      1b 24 len(4~16) data 0d 0a 
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(gucCMDBuf[1] <3 || gucCMDBuf[1] > 16)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
    //从gucCMDBuf[3]开始接收,已经接收一个字节data
	if(SI_ucParseStartCMDData0(gucCMDBuf[1])-1)	
	{
		return 1;
	}
	if(!write_sn(gtCurKey.appid,gucCMDBuf[1],&gucCMDBuf[2]))
	{
		return 0;
	} 
	SETRET(RESULT_KEY_ERROR);	
	return 1;	
}

uchar SI_SetBps(void)
{
	uint j,bps;
	if(SI_ucParseStartCMDData(3,&j))	
	{
		return 1;
	}
//	TRACE("LEN=%d", j);
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(j!=1)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
//	TRACE("%x-%x-%x", gucCMDBuf[1],gucCMDBuf[2],gucCMDBuf[3]);
	if(gucCMDBuf[1]!=0x02 || gucCMDBuf[2]!=0x00)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	switch(gucCMDBuf[3])
	{
		case 1:
			bps = 9600;
			break;
		case 2:
			bps = 19200;
			break;
		case 3:
			bps = 57600;
			break;
//		case 4:
//			bps = 1200;
//			break;
		case 7:
			bps = 38400;
			break;
		case 8:
			bps = 115200;
			break;
		default:
			SETRET(RESULT_ERROR_PARA);
			return 1;
	}
    s_sysconfig_write(OFFSET_BPS,LEN_BPS,(uchar *)&bps);	
    SETRET(RESULT_SUCCESS);
	SI_vSendACK();
    UART_Init(uart_get_bps());
	return 0;
}
/* 
 * SI_ucLoadNewMK - [GENERIC] 旧主密钥下载新主密钥
 * in:  ID+Len+MKLen+MK+CKLen  +  CK
 *      0  1   2     3  3+MKlen   4+MKLen
 * @ 
 */
uchar SI_ucLoadNewMK (uchar inlen, uchar *in)
{
    uchar ret,keylen,MKLen;
    uchar key[KEY_UNIT_LEN],newmk[KEY_UNIT_LEN];
    MKLen = in[2];
#ifdef EM_PinpadStart_Debug
    vDispBufTitle("IN",inlen,0,in);
#endif		
    if ( MKLen != EM_DESKEY_LEN && MKLen != EM_3DESKEY_LEN) {
        SETRET(RESULT_ERROR_PARA);
        return 1;
    } 
    if(PCI_ReadCurrentKey(KEY_TYPE_MK,in[0],&keylen,key)){
        SETRET(RESULT_KEY_ERROR);
        return 1;
    }
    des_decrypt_ecb(&in[3],newmk,MKLen,key,keylen/8);
#ifdef EM_PinpadStart_Debug
	vDispBufTitle("MK",MKLen,0,&in[3]);
	vDispBufTitle("OMK",KEY_UNIT_LEN,0,key);
	vDispBufTitle("NMK",KEY_UNIT_LEN,0,newmk);
#endif
    CLRBUF(key);
    keylen = in[3+MKLen];
    if ( keylen ) {
        des_encrypt_ecb(&key[0],&key[8],8,newmk,MKLen/8);
#ifdef EM_PinpadStart_Debug
        vDispBufTitle("CK0",keylen,0,&in[4+MKLen]);
        vDispBufTitle("CK1",8,0,&key[8]);
#endif		
        if ( memcmp(&key[8],&in[4+MKLen],keylen) ) {
            SETRET(RESULT_ERROR_MACK);
            return 1;
        }
    }
    ret = SI_ucPinpadLoadTMK(in[0],MKLen,newmk);
    CLRBUF(newmk);
    return ret;
}		/* -----  end of function SI_ucLoadNewMK  ----- */

/**********************************************************************
* name 
*     SI_ucParseCMD
* function 
*     parase command whose format is
*        1b + cid + data + 0d + 0a
* in parameter: 
*   gucCMDBuf[0]:CID
*   gucCMDBuf[1] and gucCMDBuf[2]
*
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/
void SI_vParseCMD(void)
{
	uchar ucRet=1,type,index;
	uint j;
//    pinpad_Display(0,0,DISP_FONT|DISP_CLRLINE,"%x-%x-%x-%x",gucCMDBuf[0],gucCMDBuf[1],gucCMDBuf[2],gucCMDBuf[3]);
//    TRACE(" prtl=%d appid:%d",gAppProcolType,gtCurKey.appid);
	switch(gucCMDBuf[0])
	{
		case 'F':
		{	
			// 46
			ucRet = EncryptKeyWithoutCardNO();
			DispScreen(0);
			break;
		}
		case 'N':
		{
			// 4E
			if(!SI_ucParseStartCMDData(2,&j))				
			{
				if(j == 0)
				{
					j = SI_ucTwo2One(gucCMDBuf[1],gucCMDBuf[2]);
					if(j <= MAX_PIN_LEN)
					{
						if(gtCurKey.appid)
						{
							gtCurKey.pinlen = j;
							ucRet = 0;
							break;
						}
						else
						{
							SETRET(RESULT_APP_NOTRUN);
							break;
						}
					}
				}
				SETRET(RESULT_ERROR_PARA);
			}	
			break;
		}
		case 'S':
		{	
			// 53
			ucRet = DownLoadWorkSecretKey();
			break;
		}
		case 'D':
		{
			// 44
			ucRet = SI_ucParseStartCMDData(40,&j);
			if(!ucRet)
			{
//                vDispBufTitle(NULL,j+1,0,gucCMDBuf);
				ucRet = SI_ucParseDisplay(j+1,&gucCMDBuf[1]);
			}
			else
			{
				SETRET(RESULT_ERROR_PARA);
				ucRet = 1;
			}
			break;
		}
		case 'L':
		{
            ucRet = SI_ucTimeoutDisplay();
			break;
		}
		case 'A':
		{
			//41
			ucRet = ActivateSecretGroup();
			break;
		}
		case 'R':
		{
			//52
			if(!SI_ucIfETXErr(gucCMDBuf[1],gucCMDBuf[2]))			
			{
				ucRet = SI_ucRenewDefault();
				DispScreen(0);
			}
			else
			{
				SETRET(RESULT_ERROR_PARA);
			}
			break;
		}
		case 'C':
		{
			//43
			ucRet = SI_cls();
			break;
		}
		case 'V':
		{
			//1B 56 31/32 0D 0A
			ucRet = SI_SetDesFlg();
			break;
		}
		case 'H':
		{	
			//48
			ucRet = EnCryptData();
			break;
		}
		case 0x71:
		{
			ucRet = SI_SetBps();
			return;
		}	
		case 0x20:
        {
			ucRet = UionPayII_EncryptKeyWithCardNO();
            DispScreen(0);
			break;
        }
		case 0x21:
        {
			ucRet = UionPayII_EncryptTrack();
			break;
        }
		case 0x22:
        {
			ucRet = UionPayII_EnCryptMac();
			break;
        }
		case 0x23:
		{
			ucRet = SI_GetSN();
			break;
		}
		case 0x24:
		{
		    ucRet = SI_SetSN();
			break;
		}
		case 0x25:
		{
			//1b 25 0d 0a reset Pinpad 
			if(!SI_ucIfETXErr(gucCMDBuf[1],gucCMDBuf[2])) {
                if(gAppProcolType!=APP_PROTOCOL_V1){
                    SETRET(RESULT_APP_TYPEERR);
                } else {
                    if(!gtCurKey.appid){
                        SETRET(RESULT_APP_NOTRUN);
                    } else {
                        SI_vResetPinpad();	
                        DispScreen(0);
                        ucRet = 0;
                    }
                }
			} else {
				SETRET(RESULT_ERROR_PARA);
			}
			break;
		}		

		case 0x27:
		{
			// load text MK   ESC+'\x27'+ Id + Len + Data+0D+0A
			//                     [0]    [1]  [2]
			if(!SI_ucParseStartCMDData0(gucCMDBuf[2]))
			{
                ucRet = SI_ucPinpadLoadTMK(gucCMDBuf[1],gucCMDBuf[2],&gucCMDBuf[3]);
			} else
			{
				SETRET(RESULT_ERROR_PARA);
			}
			break;			
		}
		case 0x28:
		{		
			ucRet = UionPayII_ActivScatter();
			break;
		}
		case 0x29:
		{	
			ucRet = SI_LoadRandom();					
			break;
		}	
		case 0x2A:
		{
			// load initial xor value 1b 2A 08 str 0d 0a
			if(SI_ucParseStartCMDData0(gucCMDBuf[1]-1) == 0)
			{
				if(gucCMDBuf[1] == 8)
				{
					memcpy(gucStartXorBuf,&gucCMDBuf[2],8);
					ucRet = 0;
				}
				else
				{
					SETRET(RESULT_ERROR_PARA);
				}
			}	
			break;
		}
		case 0x2B:
		{
			//load work key with check 
			//      0       1 2       3      4      5             13/21
			//1b + '\x2B' + Len(2B) + MKNo + WKNo + WK(8B或16B) + Check(4B) + 0d + 0a
			j = gucCMDBuf[1] * 256 + gucCMDBuf[2];
			if(SI_ucParseStartCMDData0(j) == 0)
			{
				if(j != 22 && j != 14)
				{
					SETRET(RESULT_ERROR_PARA);
					break;
				}
				if(workkey_map(gucCMDBuf[3],gucCMDBuf[4],&type,&index))
				{
					break;
				}
				ucRet = SI_ucPinpadLoadKey(4,gucCMDBuf[3],type,index,(uchar)(j-6),&gucCMDBuf[5]);
			}
			break;
		}
		case 0x2C:
		{
			//load mtext MK   ESC+'\x2C'+ Id + Len + Data+0D+0A
			//						[0]  [1]  [2]
            //                                 Len+MKLen+NMK+CKLen+CK+0D+0A 
			if(SI_ucParseStartCMDData0(gucCMDBuf[2]) == 0 )
			{
                if(gucCMDBuf[1] >= 0x80 && gucCMDBuf[1] <= 0x8F)
                {
                    gucCMDBuf[1] -= 0x80;
                    ucRet = SI_ucLoadNewMK(gucCMDBuf[2]+2,&gucCMDBuf[1]);
                    break;
                }
                else if(gucCMDBuf[1] <= 0x0F)
				{
                    if(gucCMDBuf[2] == EM_DESKEY_LEN || gucCMDBuf[2] == EM_3DESKEY_LEN){
                        des_decrypt_ecb(&gucCMDBuf[3],&gucCMDBuf[64],gucCMDBuf[2],gucTmpProcessKey,1);
                        ucRet = SI_ucPinpadLoadTMK(gucCMDBuf[1],gucCMDBuf[2],&gucCMDBuf[64]);
                        break;
                    }
				}
                SETRET(RESULT_ERROR_PARA);
			}
			break;
		}
		case 0x2D:
		{				
			ucRet = UionPayII_CreateProSecretKey();
			break;
		}
		case 0x4B:
		{
			ucRet = ParseExtendcmd();
//            TRACE("\n-|EX cmd");
			return;
		}
		case 0xFF:
		{
			break;
		}
		default:
			SETRET(RESULT_INVALIDCMD);
			break;
	}	
	if(ucRet == 1)
	{
		SI_vSendNAK();
	}
	else 
	{
		SETRET(RESULT_SUCCESS);
		if(ucRet == 0)
		{
			SI_vSendACK();
		}
	}			
}

uchar SI_ucMakeLrc0(uchar ucStart,uchar *ucBuf, uint uiLen)
{
	uint i;
	uchar ucLrc = ucStart;
	for(i=0;i<uiLen;i++)
	{
		ucLrc ^= ucBuf[i];
	}
	return ucLrc;
	
}

void SI_vSendMasterKey(uchar ucCmd, uchar ucLen, uchar *ucBuf)
{
	uchar ucLrc;
	gucCMDBuf[0] = ucCmd;
	gucCMDBuf[1] = ucLen;
	memcpy(&gucCMDBuf[2], ucBuf, ucLen);	
	ucLrc = SI_ucMakeLrc0(0,gucCMDBuf, ucLen+2);
	gucCMDBuf[ucLen+2] = ucLrc;
	UART_Write(gucCMDBuf, ucLen+3);
}
//*********************************
//
//         MPOS Command
//
//*********************************
#define MPOS_ERR_INVALID  0x91
#define MPOS_ERR_CHECK    0x92
#define MPOS_ERR_LEN      0x93
#define MPOS_ERR_PARA     0x94
#define MPOS_ERR_MAC      0x95
#define MPOS_ERR_FAIL     0x96
#define MPOS_ERR_OTHER    0x97

void SI_ucMasterKeyParse(uchar ucCmd)
{
	uchar i,ucRet;
	gucCMDBuf[0] = ucCmd;
	if(!UART_Read(&gucCMDBuf[1], 1, 200))
	{
		return;
	}	
	for(i=0;i<(gucCMDBuf[1]+1);i++)
	{
		if(!UART_Read(&gucCMDBuf[2+i], 1, 200))
		{
			return;
		}
	}
    exit_lowerpower_freq();
	//gucCMDBuf [0]   [1]   [2]    [2+Len]
	// 			cmd + len + data + Lrc
#ifdef EM_PinpadStart_Debug
	vDispBufTitle("MPOS",gucCMDBuf[1]+3,0,gucCMDBuf);
#endif	
	if(SI_ucMakeLrc0(0,&gucCMDBuf[0], gucCMDBuf[1]+3))
	{
		ucRet = MPOS_ERR_CHECK;
		goto SI_ucMasterKeyParseErr;
	}
	if(!gtCurKey.appid)
	{
		SETRET(RESULT_APP_NOTRUN);
		ucRet = MPOS_ERR_OTHER;
		goto SI_ucMasterKeyParseErr;
	}
	if(gAppProcolType!=APP_PROTOCOL_V1)
	{
		SETRET(RESULT_APP_TYPEERR);
		ucRet = MPOS_ERR_OTHER;
		goto SI_ucMasterKeyParseErr;
	}
	ucRet = 0;
	switch(ucCmd)
	{
		case 0x81:
			// reset
			if(gucCMDBuf[1] == 0)
			{
				memset(gucSrcBuf,0xFF,16);
				//04:厂家编码  01:键盘型号(A命令集) 09:键盘版本
				memcpy(gucSrcBuf,"\x04\x01\x01",3);
				SI_vResetPinpad();
				SI_vSendMasterKey(ucCmd,16,gucSrcBuf);
				return;
			}
			else
			{
				SETRET(RESULT_ERROR_PARA);
				ucRet = MPOS_ERR_LEN;				
			}
			break;
		case 0xD0:
			// update SN
			if(gucCMDBuf[1] >= 4 && gucCMDBuf[1] <= 16)
			{
				if(write_sn(gtCurKey.appid,gucCMDBuf[1], &gucCMDBuf[2]))
				{
					SETRET(RESULT_ERROR_FLASH);
					ucRet = MPOS_ERR_FAIL;					
				}
			}
			else
			{
				SETRET(RESULT_ERROR_PARA);
				ucRet = MPOS_ERR_LEN;
			}
			break;
		case 0xD1:
			//mingwen load transferkey
			if(gucCMDBuf[1] == 16)
			{
				memcpy(&gucCMDBuf[2+16],&gucCMDBuf[2],8);
				if(write_transferkey(gtCurKey.appid,&gucCMDBuf[2]))
				{
					SETRET(RESULT_ERROR_FLASH);
					ucRet = MPOS_ERR_FAIL;
				}
			}
			else
			{
				SETRET(RESULT_ERROR_PARA);
				ucRet = MPOS_ERR_LEN;
			}				
			break;
		case 0xD2:
			//            0     1       2    3       19          
			//load MK     D2 11/15/19 MKNo MK(16B) Check(0/4/8B) Lrc
			if(gucCMDBuf[1] == 0x19 || gucCMDBuf[1] == 0x15 || gucCMDBuf[1] == 0x11)
			{
				if(s_check_keyindex(KEY_TYPE_MK,gucCMDBuf[2]))
				{
					SETRET(RESULT_ERROR_PARA);
					ucRet = MPOS_ERR_PARA;
					goto SI_ucMasterKeyParseErr;
				}
				if(read_transferkey(gtCurKey.appid, &gucSrcBuf[64]))
				{
					SETRET(RESULT_KEY_ERROR);
					ucRet = MPOS_ERR_FAIL;
					goto SI_ucMasterKeyParseErr;
				}
				// check length (0/4/8) 
				i = gucCMDBuf[1] - 16 - 1;
				des_decrypt_ecb(&gucCMDBuf[3],gucSrcBuf,16,&gucSrcBuf[64],2);	
			#ifdef EM_PinpadStart_Debug
				vDispBufTitle(NULL,16, 0,&gucSrcBuf[64]);
				vDispBufTitle(NULL,16, 0,gucSrcBuf);
			#endif
				if(i)
				{
					memset(&gucCMDBuf[32],0,8);
					des_encrypt_ecb(&gucCMDBuf[32],&gucSrcBuf[16],8,gucSrcBuf,2);
				#ifdef EM_PinpadStart_Debug
					vDispBuf(8, 0, &gucSrcBuf[16]);
				#endif
					if(memcmp(&gucSrcBuf[16],&gucCMDBuf[19],i) == 0)
					{
						SI_ucPinpadLoadTMK(gucCMDBuf[2],16,gucSrcBuf);
					}
					else
					{
						//校验失败
						SETRET(RESULT_ERROR_MACK);
						ucRet = MPOS_ERR_MAC;
					}
				}
				else
				{
					if(SI_ucPinpadLoadTMK(gucCMDBuf[2],16,gucSrcBuf))
					{
						SETRET(RESULT_ERROR_FLASH);
						ucRet = MPOS_ERR_FAIL;
					}
				}						
			}
			else
			{
				SETRET(RESULT_ERROR_PARA);
				ucRet = MPOS_ERR_LEN;
			}	
			break;
		case 0xD3:
			// 2011-01-06 邮储银行POS调试增加密文下载传输密钥
			//0     1         2    18 
			//D3 + 10/14/18 + Str +Check(0/4/8) +Lrc
			if(gucCMDBuf[1] == 0x18 || gucCMDBuf[1] == 0x14 || gucCMDBuf[1] == 0x10)
			{
				if(read_transferkey(gtCurKey.appid, &gucSrcBuf[64]))
				{
					SETRET(RESULT_KEY_ERROR);
					ucRet = MPOS_ERR_FAIL;
					goto SI_ucMasterKeyParseErr;
				}
				des_decrypt_ecb(&gucCMDBuf[2],gucSrcBuf,16,&gucSrcBuf[64],2);	
				i = gucCMDBuf[1] - 16;
			#ifdef EM_PinpadStart_Debug
				vDispBuf(16, 0,&gucSrcBuf[64]);
				vDispBuf(16, 0, gucSrcBuf);
			#endif
				if(i)
				{
					memset(&gucCMDBuf[32],0,8);
					des_encrypt_ecb(&gucCMDBuf[32],&gucSrcBuf[16],8,gucSrcBuf,2);
				#ifdef EM_PinpadStart_Debug
					vDispBuf(8, 0, &gucSrcBuf[16]);
				#endif
					if(memcmp(&gucSrcBuf[16],&gucCMDBuf[18],i) == 0)
					{
						memcpy(&gucSrcBuf[16],gucSrcBuf,8);
						if(write_transferkey(gtCurKey.appid,gucSrcBuf))
						{
							SETRET(RESULT_ERROR_FLASH);
							ucRet = MPOS_ERR_FAIL;
						}
					}
					else
					{
						SETRET(RESULT_ERROR_MACK);
						ucRet = MPOS_ERR_MAC;
					}
				}
				else
				{
					memcpy(&gucSrcBuf[16],gucSrcBuf,8);
					if(write_transferkey(gtCurKey.appid,gucSrcBuf))
					{
						SETRET(RESULT_ERROR_FLASH);
						ucRet = MPOS_ERR_FAIL;
					}
				}			
			}
			else
			{
				SETRET(RESULT_ERROR_PARA);
				ucRet = MPOS_ERR_LEN;
			}	
			break;
		default:
			SETRET(RESULT_NOT_PERMIT);
			ucRet = MPOS_ERR_INVALID;
			break;
	}	
SI_ucMasterKeyParseErr:	
	SI_vSendMasterKey(ucCmd,1,&ucRet);		
}


void parse_v1protocol(uchar stx)
{
	if(stx == CMD_STX)
	{
		if(UART_Read(gucCMDBuf, 3, 500) >= 3)
		{
//            TRACE("\r\n-|V1[%02X-%02X]",gucCMDBuf[0],gucCMDBuf[1]);
			SI_vParseCMD();
//            TRACE("\nV1 out");
//            TRACE("\r\n###################");
		}
	}
	else if(stx == 0x81 || stx >= 0xD0 && stx <= 0xD3)
	{
		SI_ucMasterKeyParse(stx);
	} else {
//        DISPPOS(stx);
    }
}

//****************************************************
//
//    extend commad
//
//*************************************************** 
uchar Extendcmd_getstat(uint inlen, uchar *inbuf)
{
	if(inlen)
	{
		SETRET(RESULT_ERROR_PARA);
		SI_vSendNAK();
		return 0;
	}
    TRACE("gucLastRet:%d", gucLastRet);
	SI_vSendRetStr(EM_ERR_SENDDATA,1,&gucLastRet);
	return EM_ERR_SENDDATA;
}
uint creat_V1APP(uint appno, uchar *appname, uchar *appak, uchar space)
{
	uchar buf[128];
	uint ret;

    TRACE("\n ==V1 create app%d space:%d==",appno,space);
	CLRBUF(buf);
	// copy appname
	memcpy(buf,appname,LEN_APP_APPNAME);
	// cooy default psw
	memset(buf+LEN_APP_APPNAME,'1',PSW_LEN);
	memset(buf+LEN_APP_APPNAME+PSW_LEN,'2',PSW_LEN);
	// copy APPAK
	memcpy(buf+LEN_APP_APPNAME+2*PSW_LEN,appak,MK_UNIT_LEN);	
	ret = app_create(appno,APP_TYPE_V1|INFO_APPNAME|(space<<4)|INFO_PSW|INFO_APPK,buf);
	if(ret)
	{
        TRACE("\n-|app create error");
		return 1;
	}	
    gtCurKey.appid = appno;
	ret = SI_ucRenewDefault();
    if ( ret ) {
        app_del(appno);
    }
//	app_open(appno);
//	app_close(appno);
	return 0;
}

uint create_default_V1APP(void)
{
	uchar appak[16];
	uchar appname[8];
	CLRBUF(appname);
	CLRBUF(appak);
	strcat((char *)appname,DEFAULT_V1_APP);
	return creat_V1APP(1,appname,appak,1);
}
//0-open default app ok 
//1-not default app
//2-open default app fail
uchar open_default_V1APPP(void)
{
	// V1 SMAPP1 space=1
	uint ret,i,flag=0;
	uchar appname[8],matchappname[8];
    TRACE("\nOpen default V1 app");
	ret = app_read_info(1,INFO_FLAG,&i,(uchar *)&flag);
	if(flag & APP_TYPE_PCI)
	{
		return 1;
	}
	ret = app_read_info(1,INFO_APPNAME,&i,(uchar *)appname);
	if(ret)
	{
		return 1;
	}
	CLRBUF(matchappname);
	strcat((char *)matchappname,DEFAULT_V1_APP);
	if(memcmp(matchappname,appname,LEN_APP_APPNAME))
	{
        TRACE("\n name err");
		return 1; 
	}		
	if(app_open(1))
	{
        TRACE("\n open");
		return 2;
	}
	SI_vResetPinpad();
	return 0;
}

uchar Extendcmd_CreateApp(uint32_t inlen, uint8_t *inbuf)
{
	uint32_t i,ret;
	//uchar buf[64];
	uint8_t ptk[24];
	uint8_t appak[24];
	uint8_t mac[8];

    TRACE_BUF("CreatApp",inbuf,inlen);
	//                0         8       9       10
	//(8+1+1+16+4=34) Name(8B)	Type	Space	APPAK(16)	RFU(4B)
	if(inlen != 30 || inbuf[8] != 0 || inbuf[9] != 0)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
    }
    if (PKS_CheckPbulicKey())
    {
		SETRET(RESULT_ERROR);
		return 1;
    }
    app_close(gtCurKey.appid);
	i = app_find(inbuf);
	if(i)
	{
		SETRET(RESULT_APP_EXIST);
		return 1;
	}	
    i = app_find_newapp();
	if(!i)
	{
		SETRET(RESULT_APP_OVERFLOW);
		return 1;
	}	
    //copy APPAK ciphertext 
	memcpy(appak,inbuf+10,MK_UNIT_LEN);
	//read default PTK
	read_default_tranferkey(ptk);
#ifdef EM_DEBUG	
	vDispBufTitle("default trsk",LEN_TRANSFERKEY_UNIT, 0, ptk);
	vDispBufTitle("appak cipher", 16, 0, inbuf+10);
#endif	
	des_decrypt_ecb(appak,appak,MK_UNIT_LEN,ptk,3);
#ifdef EM_DEBUG	
	vDispBufTitle("appak text", 16, 0, appak);
	vDispBufTitle("default trsk",LEN_TRANSFERKEY_UNIT, 0, ptk);
#endif
	//check mac
	cacul_mac_ecb(24,ptk,16,appak,4,mac);
#ifdef EM_DEBUG	
	vDispBufTitle("mac1", 4, 0, mac);
	vDispBufTitle("mac2", 4, 0, inbuf+26);
#endif	
	CLRBUF(ptk);
	if(memcmp(mac,inbuf+26,4))
	{
		SETRET(RESULT_ERROR_MACK);
		return 1;
	}	
//    TRACE("\ncreat");
	ret = creat_V1APP(i,inbuf,appak,inbuf[9]);
	if(ret)
	{
		SETRET(RESULT_ERROR_FLASH);
	}	
	return 0;
}

uchar Extendcmd_OpenApp(uint inlen, uchar *inbuf)
{
	//          0         8       
	//(8+8=16) Name(8B) check
	uint i;
	if(inlen != 16)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
    gWorkMode = MODE_IDLE;
	app_close(gtCurKey.appid);
	i = app_find(inbuf);
	if(!i)
	{
		SETRET(RESULT_APP_NOTEXIST);
		return 1;
	}
	if(app_get_type(i) != APP_PROTOCOL_V1)
	{
		SETRET(RESULT_APP_TYPEERR);
		return 1;
	}
	if(app_open(i))
	{
		SETRET(RESULT_APP_LOCKED);
		return 1;
	}
	SI_vResetPinpad();
	return 0;
}

uchar Extendcmd_DelApp(uint inlen, uchar *inbuf)
{
	//          0         8       
	//(8+1+8=17) Name(8B) check
	uint i;
	if(inlen != 17 || inbuf[8]!=0)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	app_close(gtCurKey.appid);
	i = app_find(inbuf);
	if(!i)
	{
		SETRET(RESULT_APP_NOTEXIST);
		return 1;
	}
	if(app_get_type(i) != APP_PROTOCOL_V1)
	{
		SETRET(RESULT_APP_TYPEERR);
		return 1;
	}
	if(app_del(i))
	{
		SETRET(RESULT_ERROR_FLASH);
		return 1;
	}
	return 0;
}


uchar Extendcmd_Active(uint inlen, uchar *inbuf)
{
	//     0    1       2       
	//(4B) Type	Index	Mode	RFU
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(inlen != 4 || inbuf[0] > 2)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	inbuf[0] += KEY_TYPE_PINK;
	if(s_check_keyindex(inbuf[0],inbuf[1]))
	{
		return 1;
	}
	//gtCurKey.MKIndex = gucCMDBuf[1];
	gtCurKey.AuthType = inbuf[0];
	gtCurKey.AuthIndex = inbuf[1];
	gtCurKey.scatter = GETBIT(inbuf[2], 0);
	gtCurKey.encflg = GETBIT(inbuf[2], 1);
	return 0;
}
// encmode:D1~D2
// checklen:D3~D4
uint Extendcmd_GetLoadMode(uchar mode, uchar *encmode, uchar *checklen, uchar *checkalg)
{
	uchar data;
	//D1~D2
	*encmode = (mode & 0x06)>>1;
	//D3~D4
	data = (mode & 0x18)>>3;
	if(data == 0)
	{
		*checklen = 0;
	}
	else if(data == 1)
	{
		*checklen = 4;
	}
	else if(data == 2)
	{
		*checklen = 8;
	}
	else
	{
		return 1;
	}
	//D5~D6
	*checkalg = (mode & 0x60)>>5;
	//TRACE("Mode:%d-%d-%d-%d",mode,*encmode,*checklen,*checkalg);
	if(*checkalg > 1)
	{
		return 1;
	}
	DEBUG_V1(TRACE("Mode:%d-%d-%d-%d",mode,*encmode,*checklen,*checkalg););
	return 0;
}
// 8byte0x00 check
uint LoadKey_Check(uchar keylen, uchar *key,uchar checklen, uchar *check)
{
	uchar buf1[8],buf2[8];
	CLRBUF(buf1);
	des_encrypt_ecb(buf1,buf2,8,key,keylen/8);
#ifdef EM_PinpadStart_Debug
	TRACE("LoadKey");
	vDispBufTitle(NULL,keylen,0,key);
	vDispBufTitle(NULL,checklen,0,check);
	vDispBufTitle(NULL,8,0,buf2);
#endif	
	if(memcmp(buf2,check,checklen))
	{
		SETRET(RESULT_ERROR_MACK);
		return 1;
	}
	return 0;
}

// X919 check
// 
uint LoadKey_X919Check(uchar inlen, uchar *in,
                       uchar *key, uchar *plus, 
                       uchar checklen, uchar *check)
{
	uchar buf1[32],buf2[8];
	
	memcpy(buf1,in,inlen);
	memcpy(&buf1[inlen],plus,8);
	CLRBUF(buf2);
	s_CaltMAC(4, 0, buf2, key, inlen+8, buf1, buf2);
#if 2	
	vDispBufTitle("in", inlen+8, 0, buf1);
	vDispBufTitle("ck0", checklen, 0, check);
	vDispBufTitle("key", 24, 0, key);
	vDispBufTitle("ck1", checklen, 0, buf2);
#endif	
	if(memcmp(buf2,check,checklen))
	{
		SETRET(RESULT_ERROR_MACK);
		return 1;
	}
	return 0;
}



uchar Extendcmd_Loadkey(uint inlen, uchar *inbuf)
{
 	//     0    1       2       3       4   5   5+len
	//(4B) Type	Index	Mode	RFU(1B)	Len	Key	MAC	RFU(8B)
	uchar key[24],tmp[24],encmode,checklen,checkalg,keylen;
    TRACE_BUF("loadkey",inbuf,inlen);
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(inbuf[0] > 1 || (inbuf[0] == 1 && inbuf[1] != 0))
	{
		TRACE("%d-%d",inbuf[0],inbuf[1]);
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	keylen = inbuf[4];
	if(keylen != 8 && keylen != 16 && keylen != 24)
	{
		TRACE("keylen:%d",keylen);
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
    if(inlen != (5+keylen+((inbuf[2]&0x18)>>3)*4+8))
    {
        TRACE("inlen:%d len:%d",inlen,(5+keylen+((inbuf[2]&0x18)>>3)*4+8));
        SETRET(RESULT_ERROR_PARA);
		return 1;
    }    
	if(!IFBIT(inbuf[2], 0))
	{
		//明文下载
		if(inbuf[0] == 1)
		{
			keyextend(keylen,&inbuf[5],key);
			if(write_transferkey(gtCurKey.appid,key))
			{
				SETRET(RESULT_ERROR_FLASH);
				return 1;
			}
		}
		else
		{
			if(SI_ucPinpadLoadTMK(inbuf[1],keylen,&inbuf[5]))
			{
				return 1;
			}
		}	
	}
	else
	{	
#ifdef EM_PinpadStart_Debug
        vDispBufTitle("MK_CipherText",inbuf[4],0,inbuf+5);
#endif
		if(Extendcmd_GetLoadMode(inbuf[2],&encmode,&checklen,&checkalg))
		{
			SETRET(RESULT_ERROR_PARA);
			return 1;
		}
		
		if(inbuf[0] == 1)
		{
			//传输密钥只支持传输密钥加密
			if(encmode != 1)
			{
				SETRET(RESULT_ERROR_PARA);
				return 1;
			}
			if(read_transferkey(gtCurKey.appid,key))
			{
				SETRET(RESULT_KEY_ERROR);
				return 1;
			}
			// tmp:plaintext key  key:transter key
			des_decrypt_ecb(&inbuf[5],tmp,keylen,key,3);
		#ifdef EM_PinpadStart_Debug
			vDispBufTitle("TK0",24,0,key);
			vDispBufTitle("TK1",24,0,tmp);
		#endif	
			if(checklen)
			{
				if(checkalg == 0)
				{
					if(LoadKey_Check(keylen,tmp,checklen,&inbuf[5+keylen]))
					{
						CLRBUF(key);
						return 1;
					}
				}
				else
				{
					if(LoadKey_X919Check(keylen,&inbuf[5],key,
                                         &inbuf[5+keylen+checklen],checklen,&inbuf[5+keylen]))
					{
						CLRBUF(key);
						return 1;
					}
				}				
			}		
			keyextend(keylen,tmp,key);
			CLRBUF(tmp);
			if(write_transferkey(gtCurKey.appid,key))
			{
				SETRET(RESULT_ERROR_FLASH);
				return 1;
			}
		}
		else
		{
			// MK support encrypt with random, transfer key and old MK
			if(encmode == 0)
			{
                TRACE("->random:%d",sizeof(gucTmpProcessKey));
				memcpy(key,gucTmpProcessKey,sizeof(gucTmpProcessKey));
			}
			else if(encmode == 1)
			{
				if(read_transferkey(gtCurKey.appid,key))
				{
					SETRET(RESULT_KEY_ERROR);
					return 1;
				}
			}
//            else if(encmode == 2){
//                if(PCI_ReadCurrentKey(KEY_TYPE_MK,inbuf[1],&i,key)){
//					SETRET(RESULT_KEY_ERROR);
//					return 1;
//                }
//            }
			else
			{
				SETRET(RESULT_ERROR_PARA);
				return 1;
			}
			// tmp:plaintext key  key:transter key
			des_decrypt_ecb(&inbuf[5],tmp,keylen,key,3);
		#ifdef EM_PinpadStart_Debug
			vDispBufTitle("KEY",24,0,key);
			vDispBufTitle("MK_PlainText",24,0,tmp);
		#endif		
			if(checklen)
			{				
				if(checkalg == 0)
				{
					if(LoadKey_Check(keylen,tmp,checklen,&inbuf[5+keylen]))
					{
						CLRBUF(key);
						return 1;
					}
				}
				else
				{
					if(LoadKey_X919Check(keylen,&inbuf[5],
                                         key,&inbuf[5+keylen+checklen],
                                         checklen,&inbuf[5+keylen]))
					{
						CLRBUF(key);
						return 1;
					}
				}
			}
			keyextend(keylen,tmp,key);
			CLRBUF(tmp);
			if(SI_ucPinpadLoadTMK(inbuf[1],keylen,key))
			{
				CLRBUF(key);
				return 1;
			}
		}	
	}
	CLRBUF(key);
	return 0;
}

uchar Extendcmd_Loadworkkey(uint inlen, uchar *inbuf)
{
	//     0    1       2       3       4   5   6      6+Len  6+Len+Checklen
	//(4B) Type	Index	MKNo	Mode	RFU	Len	Key	   MAC	  RFU
	uchar encmode,checklen,checkflag,keylen;
	uchar mk[24],key[24];
	keylen = inbuf[5];
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(inbuf[0] > 2 || (keylen != 8 && keylen != 16 && keylen != 24) 
		|| ((inbuf[3]&0x01)==0))
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
    if(inlen != (keylen + 6 + 8 + ((inbuf[3]&0x18)>>3) *4))
    {
        TRACE("inlen:%d keylen:%d", inlen, keylen);
        SETRET(RESULT_ERROR_PARA);
		return 1;
    }
  	inbuf[0] += 6;
	if(s_check_keyindex(KEY_TYPE_MK,inbuf[2]) || s_check_keyindex(inbuf[0], inbuf[1]))
	{
		return 1;
	}	
	if(Extendcmd_GetLoadMode(inbuf[3],&encmode,&checklen,&checkflag))
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}	
	if (s_PCI_ReadKey(KEY_TYPE_MK, inbuf[2],  mk))
	{
		return 1;
	}
	des_decrypt_ecb(&inbuf[6],key,keylen,mk,3);
#if 2//def EM_PinpadStart_Debug
	TRACE("%d %d %d-%d %d",checklen,inbuf[2],inbuf[0],inbuf[1],keylen);
	vDispBufTitle("MK",KEY_UNIT_LEN,0,mk);
	vDispBufTitle("WK",keylen,0,key);
#endif		
	if(checklen)
	{
		if(checkflag==0)
		{
			if(LoadKey_Check(keylen,key,checklen,&inbuf[6+keylen]))
			{
				CLRBUF(key);
				CLRBUF(mk);
				return 1;
			}
		}
		else
		{
			if(LoadKey_X919Check(keylen,&inbuf[6],mk,&inbuf[6+keylen+checklen],checklen,&inbuf[6+keylen]))
			{
				CLRBUF(key);
				CLRBUF(mk);
				return 1;
			}
		}	
	}
	keyextend(keylen, key, key);	
	PCI_WriteCurrentKey(inbuf[0], inbuf[1], KEY_UNIT_LEN, key);
	CLRBUF(key);
	CLRBUF(mk);
	return 0;
}

uchar Extendcmd_Random(uint inlen, uchar *inbuf)
{
	//     0     
	//(2B) len
	uint len;
	ASSERT_APP_V1();
	if(inlen != 2)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	len = COMBINE16(inbuf[0], inbuf[1]);
	if(len == 0 || len > MAX_PACKET_LEN)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
//    TRACE("\nrandom len:%d",len);
	RNG_FillRandom(gucCMDBuf, len);
//    TRACE(" ok");
	SI_vSendRetStr(EM_ERR_SENDDATA,len,gucCMDBuf);
//    TRACE(" --");
	return EM_ERR_SENDDATA;
}

uchar Extendcmd_Loadprocesskey(uint inlen, uchar *inbuf)
{
	//     0    1               2       
	//(4B) Mode	Len(8/16/24)	Random
	uchar key[24],tmp[24],buf[24],mode,len;
	uint i;
	mode = inbuf[0];
	len = inbuf[1];
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(mode > 3 || len != 8 && len != 16 && len != 24) 
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	if(mode == 3)
	{
		//only copy 8/16 byte -> 24 byte
		keyextend(len, &inbuf[2], gucTmpProcessKey);
	}
	else
	{
        //读取当前密钥
        if (s_PCI_ReadKey(gtCurKey.AuthType, gtCurKey.AuthIndex, key))
        {
            return 1;
        }
		if(mode == 0)
		{
			//1. encryptd with current work key
			SI_ucEncrypt(key,len,&inbuf[2],buf);
			//2. copy extend 24 byte
			keyextend(len, buf, gucTmpProcessKey);	
		}
		else if(mode == 1)
		{
			CLRBUF(tmp);
			memcpy(tmp,&inbuf[2],len);
			//1. encryptd with current work key
			SI_ucEncrypt(key,len,tmp,buf);
			DEBUG_V1(vDispBufTitle("B0", len, 0, buf););	
			//2. Xor per 8 bytes
			Buf8ByteXor(len,buf,tmp);
			DEBUG_V1(vDispBufTitle("B1", 8, 0, tmp););	
			//3. copy extend 24 byte
			keyextend(8, tmp, gucTmpProcessKey);		
		}
		else if(mode == 2)
		{
			if(len != 8)
			{
				CLRBUF(key);
				SETRET(RESULT_ERROR_PARA);
				return 1;
			}
			// 前8字节取反
			for(i=0;i<8;i++)
			{
				inbuf[2+8+i] = ~inbuf[2+i];
			}
            keyextend(16, &inbuf[2], gucTmpProcessKey);	
		}
		CLRBUF(key);
	}
	DEBUG_V1(vDispBufTitle("PK", sizeof(gucTmpProcessKey), 0, gucTmpProcessKey););	
	return 0;
}


uchar Extendcmd_CaltMAC(uint inlen, uchar *inbuf)
{
	//     0    1           2       3   5       5+Len
	//(4B)Index	Algorithm	Padding	Len	Data	Xor
	// padding: D0: 0-fill with 00 1-fill with 80
	//          D2(算法4时有效):   0-不分散 1-分散
    //          D3: 1-还有后续块   0-最后1块

	uint len;
	uchar key[24],mac[8];	
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(inbuf[0] != KEY_TYPE_CURKEY && inbuf[0] != KEY_TYPE_PROCESSKEY)
	{
		if(s_check_keyindex(KEY_TYPE_MACK,inbuf[0]))
		{
			return 1;
		}
	}	
	len = COMBINE16(inbuf[3], inbuf[4]);
	DEBUG_V1(TRACE("\r\nMAC:%x-%d-%d-%d",inbuf[0],inbuf[1],inbuf[2],len););
	if(inbuf[1] > 4 || len == 0 || len > MAX_PACKET_LEN)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}

    TRACE("inlen:%d keylen:%d", inlen, len);
    if(inlen != (len + 5 + 8))
    {
        SETRET(RESULT_ERROR_PARA);
		return 1;
    }

	if (s_PCI_ReadKey(KEY_TYPE_MACK, inbuf[0], key))
	{
		return 1;
	}
//    TRACE_BUF("mac_key",key,24);
	s_CaltMAC(inbuf[1],inbuf[2],&inbuf[5+len],key,len,&inbuf[5],mac);
	CLRBUF(key);
	SI_vSendRetStr(EM_ERR_SENDDATA,8,mac);
	return EM_ERR_SENDDATA;
}

uchar Extendcmd_Des(uint inlen, uchar *inbuf)
{

	//            0     1       2    4    6+Len
	//(4B) 1b 'K' Index	Mode	Len	 Data 0D 0A
	//                  D0:1-解密 D1:1-分散
	uint len;
	uchar key[24],newkey[24];
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(inbuf[0] != KEY_TYPE_CURKEY && inbuf[0] != KEY_TYPE_PROCESSKEY)
	{
		if(s_check_keyindex(KEY_TYPE_DESK,inbuf[0]))
		{
			return 1;
		}
	}
	len = COMBINE16(inbuf[2], inbuf[3]);
	if(inbuf[1] > 3 || len == 0 || len > MAX_PACKET_LEN || len%8 != 0)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
    if(inlen != (len + 4))
    {
        SETRET(RESULT_ERROR_PARA);
		return 1;
    }

	if (s_PCI_ReadKey(KEY_TYPE_DESK, inbuf[0], key))
	{
		return 1;
	}		
	if(inbuf[0] == KEY_TYPE_CURKEY)
	{
		 SI_ucEncrypt(key, len, &inbuf[4], gucSrcBuf);
	}
    else if(inbuf[0] == KEY_TYPE_PROCESSKEY)
    {
#ifdef EM_PinpadStart_Debug	
		TRACE_BUF("key",key,24);
#endif
        if(IFBIT(inbuf[1],0))
        {
            des_decrypt_ecb(&inbuf[4], gucSrcBuf, len, key, 3);
        }
        else
        {
            des_encrypt_ecb(&inbuf[4], gucSrcBuf, len, key, 3); 
        }    
    }    
    else
    {    
    	if(IFBIT(inbuf[1],1))
    	{
    		//scatter
    		des_encrypt_ecb(gucTmpProcessKey, newkey, sizeof(gucTmpProcessKey), key, 3);
#ifdef EM_PinpadStart_Debug		
			vDispBuf(24, 0,key);
			vDispBuf(24, 0,gucTmpProcessKey);
			vDispBuf(24, 0,newkey);			
#endif
    	}
        else {
            memcpy(newkey,key,sizeof(key));
        }
#ifdef EM_PinpadStart_Debug	
		TRACE_BUF("key",newkey,24);
#endif
		if(IFBIT(inbuf[1],0))
        {
            des_decrypt_ecb(&inbuf[4], gucSrcBuf, len, newkey, 3);
        }
        else
        {
            des_encrypt_ecb(&inbuf[4], gucSrcBuf, len, newkey, 3); 
        }
		CLRBUF(newkey);
    }    
	CLRBUF(key);
	SI_vSendRetStr(EM_ERR_SENDDATA,len,gucSrcBuf);
	return EM_ERR_SENDDATA;
}

uchar Extendcmd_OnlinePIN(uint inlen, uchar *inbuf)
{
	//   0    1   2   3    4    5           17
	//V1:pink+Min+Max+mode+Time+CardNO(12B)+str包/pin包	
	uint pinLen,i;	
 	uchar dispbuf[42],pinStr[16],pink[24];
	uchar ucRet;
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(inbuf[0] != KEY_TYPE_CURKEY && inbuf[0] != KEY_TYPE_PROCESSKEY)
	{
		if(s_check_keyindex(KEY_TYPE_PINK,inbuf[0]))
		{
			return 1;
		}
	}
    if ( inbuf[3] >= 1 ) {
		SETRET(RESULT_ERROR_PARA);
		return 1;
    }
	if(inbuf[4] == 0)
	{
		inbuf[4] = 120;
	}
	if(inbuf[2] > MAX_PIN_LEN || inbuf[1] > inbuf[2] || inbuf[1] < 4
		|| inbuf[4] < 30 || inbuf[4] > 120)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}	
	//check cardno
    if ( s_CheckStr(12,&inbuf[5],(int *)&pinLen,&inbuf[5]) ) {
        SETRET(RESULT_ERROR_PARA);
        return 1;
    }
	inlen -= 17;
	//check str or pinstr
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210			
    LCD_ClearScreen();
	if(inlen)
	{
		CLRBUF(dispbuf);
		memcpy(dispbuf,&inbuf[17],inlen);
		pinpad_Display(1,(char *)dispbuf);
	}
	LCD_ShowPINPrompt();		
	ucRet = KB_GetPINStr(3, inbuf[1], inbuf[2], inbuf[4] * 1000, (uint32_t *)&pinLen, pinStr);
#elif PRODUCT_TYPE == PRODUCT_TYPE_F12	
	if(inlen)
	{
		if(inlen < inbuf[1] || inlen > inbuf[2])
		{
			SETRET(RESULT_ERROR_PARA);
		}
		//S970
        if ( s_CheckStr(inlen,&inbuf[17],(int *)&pinLen,pinStr)) {
            SETRET(RESULT_ERROR_PARA);
            return 1;
        } 
		ucRet = 0;
	}
	else
	{
		//S980
		gInPinEntry = 1;
		LCD_ShowPINPrompt();
		ucRet = KB_GetPINStr(4, inbuf[1], inbuf[2], inbuf[4] * 1000, (uint32_t *)&pinLen, pinStr);
		gInPinEntry = 0;
	}
#endif
    s_DelayUs(500);
	if (ucRet)
	{
		if (ucRet == 2)
		{
			// 2
			SETRET(RESULT_INPUT_TIMEOUT);
			return 1;
		}
		else
		{
			// 1
			SETRET(RESULT_INPUT_CANCEL);
			return 1;
		}
	}
    if(pinLen == 0)
    {
        SI_vSendRetStr(EM_ERR_SENDDATA, 0, dispbuf);
        return EM_ERR_SENDDATA;
    }    
	if (s_PCI_ReadKey(KEY_TYPE_PINK, inbuf[0], pink))
	{
		CLRBUF(pinStr);
		return 1;
	}
	PinEncrypt(1,24,pink,pinLen,&inbuf[5],pinStr,&i,dispbuf);
	CLRBUF(pinStr);
	CLRBUF(pink);
	SI_vSendRetStr(EM_ERR_SENDDATA,8,dispbuf);
	return EM_ERR_SENDDATA;		
}



uchar Extendcmd_OfflinePIN(uint inlen, uchar *inbuf)
{
	//             0    1   2       3   4       
	//            Min	Max	Timeout	0	Str
//1B 4B 00 05 0B   04   0E  78      00  7C 0D 0A 
	//             0    1   2       3   4       12      12+264
	//            Min	Max	Timeout	1	Random	PubKey	Str  
	uint pinLen,i;	
	R_RSA_PUBLIC_KEY *pubk;	
	uchar *charp;
	uchar dispbuf[42],pinStr[16];
	uchar ucRet;
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	if(inbuf[2] == 0)
	{
		inbuf[2] = 120;
	}
	if(inbuf[0] < MIN_PCIPIN_LEN || inbuf[1] > MAX_PIN_LEN || inbuf[0] > inbuf[1] 
		|| inbuf[2] < 30 || inbuf[2] > 120)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	CLRBUF(dispbuf);
	if(inbuf[3] == 0)
	{
		if(inlen < 4)
		{
			SETRET(RESULT_ERROR_PARA);
			return 1;
		}
		inlen -= 4;
	#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
		if(inlen > 40)
			inlen = 40;	
		memcpy(dispbuf,&inbuf[4],inlen);
	#else
        pinLen = 0;
        if ( inlen ) {
            //S970
            if ( inlen < inbuf[0] || inlen > inbuf[1]) {
                SETRET(RESULT_ERROR_PARA);
                return 1;
            }
            if (s_CheckStr(inlen,&inbuf[4],(int *)&pinLen,pinStr)) {
                SETRET(RESULT_ERROR_PARA);
                return 1;
            }
        }
	#endif	
	}
	else
	{
		if(inlen < (4+8+264))
		{
			SETRET(RESULT_ERROR_PARA);
			return 1;
		}
		pubk = (R_RSA_PUBLIC_KEY *)&inbuf[12];
//		DEBUG_V1(
        memcpy((char *)&i,pubk->exponent,4);
            TRACE("\r\nPUBK bit:%d exp:%08x",pubk->bits,i);
            DISPBUF("PUBK",sizeof(R_RSA_PUBLIC_KEY),0,pubk);
//        );
//        msb_uint32_to_byte4(i,pubk->exponent);
		if(pubk->bits<512 || pubk->bits>2048 || pubk->bits%8!=0)
		{
			SETRET(RESULT_ERROR_PARA);
			return 1;
		}
		inlen -= (4+8+264);
	#if PRODUCT_TYPE == PRODUCT_TYPE_MK210		
		if(inlen > 40)
			inlen = 40;
		memcpy(dispbuf,&inbuf[12+264],inlen);
	#else
        pinLen = 0;
        if ( inlen ) {
            if ( inlen < inbuf[0] || inlen > inbuf[1]) {
                SETRET(RESULT_ERROR_PARA);
                return 1;
            }
            if (s_CheckStr(inlen,&inbuf[12+264],(int *)&pinLen,pinStr)) {
                SETRET(RESULT_ERROR_PARA);
                return 1;
            }
        }
	#endif
	}    
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
	if(inlen)
	{
		pinpad_Display(1,(char *)dispbuf);
	}
	LCD_ShowPINPrompt();		
	ucRet = KB_GetPINStr(3, inbuf[0], inbuf[1], inbuf[2] * 1000, (uint32_t *)&pinLen, pinStr);
#elif PRODUCT_TYPE == PRODUCT_TYPE_F12	
	if(pinLen)
	{
		//S970
		ucRet = 0;
	}
	else
	{
		//S980
		gInPinEntry = 1;
		LCD_ShowPINPrompt();		
		ucRet = KB_GetPINStr(4, inbuf[0], inbuf[1], inbuf[2] * 1000, (uint32_t *)&pinLen, pinStr);
		gInPinEntry = 0;
	}	
#endif
    s_DelayUs(500);
	if (ucRet)
	{
		if (ucRet == 2)
		{
			// 2
			SETRET(RESULT_INPUT_TIMEOUT);
			return 1;
		}
		else
		{
			// 1
			SETRET(RESULT_INPUT_CANCEL);
			return 1;
		}
	}
    if(pinLen == 0)
    {
        CLRBUF(dispbuf);
        SI_vSendRetStr(EM_ERR_SENDDATA, 0, dispbuf);
        return EM_ERR_SENDDATA;
    }    
	if(inbuf[3] == 0)
	{
		//用APPAK加密
		if(app_read_info(gtCurKey.appid,INFO_APPK,&i,dispbuf))
		{
			SETRET(RESULT_KEY_ERROR);
			return 1;
		}
		des_encrypt_ecb(pinStr, dispbuf+16, 16, dispbuf, 2);	
		CLRBUF(pinStr);
		SI_vSendRetStr(EM_ERR_SENDDATA,16,dispbuf+16);
		CLRBUF(dispbuf);		
		return EM_ERR_SENDDATA;
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
		memcpy(&charp[9],&inbuf[4],8);  //icc random
		if(!rsa_pub_dec(charp,&pinLen,charp,pubk->bits/8,pubk))	
//		if(!Rsa_calt_pub(pubk->bits/8, charp, pubk, (int *)&pinLen, charp))	
		{
			SI_vSendRetStr(EM_ERR_SENDDATA,pinLen,charp);
			CLRBUF(pinStr);
			return EM_ERR_SENDDATA;
		}
		else
		{
			SETRET(RESULT_ERROR);
			return 1;
		}
	}
}

uchar Extendcmd_version(uint inlen, uchar *inbuf)
{
	//     0       
	//(4B) Type 0-Hard Version 1-BOOT 2-APP 3-	
	uint len;
	uchar version[64];
	ASSERT_APP_V1();
	if(inlen != 1)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	CLRBUF(version);
	if(sys_get_ver(inbuf[0],&len,version))
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}
	SI_vSendRetStr(EM_ERR_SENDDATA,len,version);	
	return EM_ERR_SENDDATA;
}
#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
#if 0
uchar  LCD_ConvertPage(uchar *ucpPage)
{
	//范围:[0,7]
	if (*ucpPage>7)
	{
		return 1;
	}
	*ucpPage ^= 0x03;
	return 0;
}

void LCD_FillDotMatrix(const stMATRIX  matrix,const uchar *pData, const uchar ucDotType)
{
	unsigned char i;
	unsigned char ucPage;

	ucPage = matrix.page;	
	LCD_ConvertPage(&ucPage);
	for (i=0; i<ucDotType; i++)
	{		
		__LCD_SetPageCol(ucPage,matrix.col+i+1);//set_lcd_page_col中:col范围[1,128],page:[0,7]
		__LCD_WriteData(pData[i]);		
	}	
}

uchar LCD_DisplayOnePageDotMatrix(stMATRIX matrix,const uchar *pDot,const unsigned short usSize)
{
	unsigned short i;
	unsigned short usTmp;
#ifdef EM_PinpadStart_Debug
	vDispBuf(usSize, 0, pDot);
#endif
	usTmp = usSize>>3;	//usSize/8	
	for (i=0; i<usTmp; i++)
	{
		LCD_FillDotMatrix(matrix,&pDot[i*8],8);
		matrix.col += 8;
	}
	usTmp = usSize&0x07;//usSize%8
	LCD_FillDotMatrix(matrix,&pDot[i*8],(uchar)usTmp);
	
	return 0;	
}

uchar LCD_DisplayTwoPageDotMatrix(stMATRIX matrix,
							const uchar *pDotOne,
							const uchar *pDotTwo,
							const unsigned short usSize)
{
	unsigned short i;
	unsigned short usTmp;
	stMATRIX NextMatrix;
	
	usTmp = usSize>>3;	//usSize/8
	NextMatrix.col = matrix.col;
	NextMatrix.page = matrix.page+1;
	for (i=0; i<usTmp; i++)
	{
		LCD_FillDotMatrix(matrix,&pDotOne[i<<3],8);	//i<<3 -> i*8
		LCD_FillDotMatrix(NextMatrix,&pDotTwo[i<<3],8);
		matrix.col += 8;
		NextMatrix.col += 8;
	}
	usTmp = usSize&0x07;//usSize%8
	LCD_FillDotMatrix(matrix,&pDotOne[i*8],(uchar)usTmp);
	LCD_FillDotMatrix(NextMatrix,&pDotTwo[i*8],(uchar)usTmp);
	
	return 0;	
}
static uchar Extendcmd_SetBitMap(uint inlen, uchar *inbuf)	
{
	stRV_STATUS status;
	stMATRIX matrix;
	struct _Disp
	{
		unsigned char page;
		unsigned char col;
		unsigned char rfu1;
		unsigned char rfu2;
	}*pDisp;
	unsigned char *pDot;
	ASSERT_APP_V1();
	ASSERT_APP_OPEN();
	memset(&status,0x00,sizeof(stRV_STATUS));
	//LEN
	inlen -= 4;
	if (inlen < 1 || inlen > 128*2)
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	} 	
	pDisp = (struct _Disp *)inbuf;
	pDot = &inbuf[4];

	
	if ( pDisp->page >7		// page 
		|| pDisp->col >127	// col
		||(pDisp->rfu1 != 0	|| pDisp->rfu2 != 0))	//RFU two bytes
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}	
	TRACE("inlen:%d pDisp->col %d pDisp->page %d",inlen,pDisp->col,pDisp->page);
	if ((inlen>>1)+pDisp->col <= 128)	//128*64
	{
		matrix.col = pDisp->col;
		matrix.page = pDisp->page;
		if (inlen & 0x01)
		{
			if (inlen+pDisp->col <= 128)
			{
				LCD_DisplayOnePageDotMatrix(matrix,pDot,inlen);
			}
			else
			{
				SETRET(RESULT_ERROR_PARA);
				return 1;
			}
		}
		else
		{
			if (matrix.page<=0x06)
			{
				LCD_DisplayTwoPageDotMatrix(matrix,	pDot,
								&pDot[inlen>>1],
								(inlen>>1));			
			} 
			else
			{
				SETRET(RESULT_ERROR_PARA);
				return 1;
			}				
		}				
	} 
	else
	{
		SETRET(RESULT_ERROR_PARA);
		return 1;
	}		
	return 0;
}
#endif
#endif

#if PRODUCT_TYPE == PRODUCT_TYPE_F12	

/* 
 * Extendcmd_SysCmd - [GENERIC] 
 * @    0+ID:DL app ID:   
 *                     0-DL request 1- DL Ack
 *                     2-DL Auth    3- DL Auth Ack 
 *    
 */
int Extendcmd_SysCmd (uint inlen, uchar *in)
{
    int len;
	uchar buf[64],hsk[8];
    if ( in[0] == 0 ) {
        if ( in[1] == 0 ) {
            //      0    1    2              10
            //recv: 00 + 00 + T_Rand(8B)
            //send: 00 + 01 + T_Rand_E(8B) + P_Rand(8B) + 40B Data		
            len = 0;
            buf[len++] = 0;
            buf[len++] = 1;
            PCI_GetHSK(3, hsk);
            des_encrypt_ecb(&in[2],&buf[len],8,hsk,1);
            len += 8;
            RNG_FillRandom(gtCurKey.aucRand, 4*2);  
            memcpy(&buf[len],gtCurKey.aucRand,8);
            len += 8;
            memcpy(&buf[len],(uchar *)&gCtrlVerInfo,sizeof(gCtrlVerInfo));
            len += sizeof(gCtrlVerInfo);
            gtCurKey.HsFlag = HS_STAT_ING;
            SI_vSendRetStr(EM_ERR_SENDDATA,len,buf);	
            return EM_ERR_SENDDATA;
        } else if (in[1] == 2){
            //      0    1    2      3
            //recv: 00 + 02 + Stat + P_Rand_E(8B)
            //send: 00 + 03 + Stat 
           if ( gtCurKey.HsFlag != HS_STAT_ING || in[2] != 0) {
               RNG_FillRandom(gtCurKey.aucRand, 8);  
               return 1;
           } 
           PCI_GetHSK(3, hsk);
           des_encrypt_ecb(gtCurKey.aucRand, buf, 8, hsk, 1);
           if (!memcmp(&in[3], buf, 8))
           {
               len = 0;
               buf[len++] = 0;
               buf[len++] = 3;
               buf[len++] = 0;
               gtCurKey.HsFlag = HS_STAT_SUC;
               RNG_FillRandom( gtCurKey.aucRand, 4*2);  
               SI_vSendRetStr(EM_ERR_SENDDATA,len,buf);	
               return EM_ERR_SENDDATA;
           }
        }
    } else {

    }
    return 1;
}		/* -----  end of function Extendcmd_SysCmd  ----- */

uint8_t v1_excmd_enter_lowpower(uint32_t length,uint8_t *input)
{
    uint8_t ret = 0;

    if (length != 1)
    {
        SETRET(RESULT_ERROR_PARA);
        return 1;
    }
    switch (input[0])
    {
    case 0:
        SI_vSendACK();
        s_DelayMs(300);
        enter_lowerpower_llwu();
        break;
    default:
        ret = 1;
        break;
    }
    return ret;
}

#endif
/**********************************************************************
* name 
*     SI_ucParseCMD
* function 
*      pase extend commad
*           0   1    2    3  4    
*       1b+	'K'+LENH+LENL+ID+DATA+LRC+0D+0A
* in parameter: 
*   gucCMDBuf[0]:CID
*   gucCMDBuf[1] and gucCMDBuf[2]:LENH+LENL
*
* out parameter:
*
* return value
*    0-OK   	 
***********************************************************************/ 
uchar ParseExtendcmd(void)
{
	uint i;
	uchar ret;
	i = COMBINE16(gucCMDBuf[1], gucCMDBuf[2]);
	ret = SI_ucParseStartCMDData0(i+1);
//    TRACE_BUF("Excmd",gucCMDBuf+1,2+i+1);
	if(ret)
	{
		TRACE("data ret:%d", ret);
        SI_vSendNAK();
		return 1;
	}
	// check lrc
	ret = SI_ucMakeLrc0(0,&gucCMDBuf[1],2+i+1);
	if(ret)
	{
        TRACE("lrc ret:%d", ret);
		SETRET(RESULT_FRAME_ERRLRC);
		SI_vSendNAK();
        return 1;
	}
    DISPBUF("ecmd",2+i+1,0,&gucCMDBuf[1]); 
//    TRACE(" subcmd:%02X",gucCMDBuf[3]);
	switch(gucCMDBuf[3])
	{
		case 0x00:
			ret = Extendcmd_getstat(i-1,&gucCMDBuf[4]);
			return 0;
		case 0x01:
			ret = Extendcmd_CreateApp(i-1,&gucCMDBuf[4]);
			break;
		case 0x02:
			ret = Extendcmd_OpenApp(i-1,&gucCMDBuf[4]);
			break;
		case 0x03:
			ret = Extendcmd_DelApp(i-1,&gucCMDBuf[4]);
			break;
		case 0x04:
			ret = Extendcmd_Active(i-1,&gucCMDBuf[4]);
			break;
		case 0x05:
			ret = Extendcmd_Loadkey(i-1,&gucCMDBuf[4]);
			break;
		case 0x06:
			ret = Extendcmd_Loadworkkey(i-1,&gucCMDBuf[4]);
			break;
		case 0x07:
			ret = Extendcmd_Random(i-1,&gucCMDBuf[4]);
			break;
		case 0x08:
			ret = Extendcmd_Loadprocesskey(i-1,&gucCMDBuf[4]);
			break;
		case 0x09:
			ret = Extendcmd_CaltMAC(i-1,&gucCMDBuf[4]);
			break;
		case 0x0a:
			ret = Extendcmd_Des(i-1,&gucCMDBuf[4]);
			break;
		case 0x0b:
			ret = Extendcmd_OfflinePIN(i-1,&gucCMDBuf[4]);
            DispScreen(0);
            break;
		case 0x0C:
			ret = Extendcmd_version(i-1,&gucCMDBuf[4]);
			break;
		case 0x0D:
		#if PRODUCT_TYPE == PRODUCT_TYPE_MK210	
//			ret = Extendcmd_SetBitMap(i-1,&gucCMDBuf[4]);
		#else
			ret = 0;
		#endif
			break;
		case 0x0E:
			ret = Extendcmd_OnlinePIN(i-1,&gucCMDBuf[4]);
            DispScreen(0);
			break;
        case 0x20:
            ret = v1_excmd_enter_lowpower(i-1,gucCMDBuf+4);
            break;
        case 0x80:
		#if PRODUCT_TYPE == PRODUCT_TYPE_F12	
            ret = Extendcmd_SysCmd(i-1,&gucCMDBuf[4]);
        #endif
            break;
		default:
			SETRET(RESULT_INVALIDCMD);
			ret = 1;
			break;
	}
#ifdef DEBUG_Dx
    TRACE("\nret:%d gucLastRet:%d",ret,gucLastRet);
    s_DelayMs(5);
#endif
	if(ret == 1)
	{
		SI_vSendNAK();
	}
	else 
	{
		SETRET(RESULT_SUCCESS);
		if(ret == 0)
		{
			SI_vSendACK();
		}
	}
        return ret;
}
#endif
#endif


