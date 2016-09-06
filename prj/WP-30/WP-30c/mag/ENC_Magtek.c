/*
********************************************************************************
* descript : 加密磁头 megtek spi magcard
*     
********************************************************************************
*/
#include "wp30_ctrl.h"

#if (defined CFG_MAGCARD)

#if MAG_PRODUCT == EM_MTK211 
#include "bitbuf.h"
/**********************************************************************

                         
                          
***********************************************************************/
int mtk_iIfDav(void)
{
//	if ((PI1 & MTK_SPI_DAV) == MTK_SPI_DAV)
//	{
//		return 1;
//	}
	return 0;
}

void mtk_enable_device(void)
{
}

void mtk_disable_device(void)
{
}

void mtk_enable_VCC(void)
{
    hw_mag_power_write(0);
	delay_ms(10);
}
void mtk_disable_VCC(void)
{	
    hw_mag_power_write(1);
}
void mtk_Init(void)
{
    hw_mag_gpio_output(MAG_POWER);
    hw_mag_gpio_output(MAG_STROBE);
    hw_mag_gpio_output(MAG_DATA);
	mtk_disable_device();
	mtk_disable_VCC();
	MTK_CLR_SPI_CLK();
	MTK_CLR_SPI_MOSI();
}
/*
********************************************************************************
********************************************************************************
*/
void mtk_WriteByte(unsigned char ucByte)
{
	uchar i;
	volatile uchar j;
	MTK_CLR_SPI_CLK();			
	for(i = 0; i < 8; i++)
	{
		if(ucByte & gBitValue[i])
		{
			MTK_SET_SPI_MOSI();
		}
		else
		{
			MTK_CLR_SPI_MOSI();
		}
		MTK_SET_SPI_CLK();		
		++j;
		++j;
		++j;
		++j;
		MTK_CLR_SPI_CLK();
	}	
}	

/*
********************************************************************************
********************************************************************************
*/
unsigned char mtk_ReadByte(void)
{
//	uchar i;
//	vuchar j;
//	uchar ucData = 0;
//	for(i = 0; i < 8; i++)
//	{		
//		MTK_SET_SPI_CLK();
//		if ((PI1 & MTK_SPI_MISO) == MTK_SPI_MISO)
//		{
//			ucData |= gBitValue[i];
//		}
//		else
//		{
//			ucData |= 0;
//		}
//		MTK_CLR_SPI_CLK();	
//	}	
//	return ucData;
    return 0;
}


/*
********************************************************************************
#if 0
	3DES加密
input:  in  --明文  
output: out --密文
#endif
********************************************************************************
*/
void MTK_DES3_K16(uchar *pucKey,uchar ucLen,uchar *in,uchar *out)
{
	des2_encrypt_ecb(in,ucLen,pucKey,out);
}

/*
********************************************************************************
#if 0
	3DES解密
input:  in  --密文 
output: out --明文 
#endif
********************************************************************************
*/
void _MTK_DES3_K16(uchar *pucKey,uchar ucLen,uchar *in,uchar *out)
{
	des2_decrypt_ecb(in,ucLen,pucKey,out);
}

/*
********************************************************************************
#if 0
	DES加密
input:  in  --明文  
output: out --密文
#endif
********************************************************************************
*/
void MTK_DES_K8(uchar *pucKey,uchar ucLen,uchar *in,uchar *out)
{
    des1_encrypt_ecb(in,ucLen,pucKey,out);
}
/*
********************************************************************************
#if 0
	DES解密
input:  in  --密文 
output: out --明文 
#endif
********************************************************************************
*/
void _MTK_DES_K8(uchar *pucKey,uchar ucLen,uchar *in,uchar *out)
{
    des1_decrypt_ecb(in,ucLen,pucKey,out);
}


/*
********************************************************************************
#if 0
	加密
input:  in  --明文 
output: out --密文 
#endif
********************************************************************************
*/
void MTK_DES_K(uchar keylen,uchar *pucKey,uchar ucLen,uchar *in,uchar *out)
{
	if (keylen == 8)
	{
		des1_encrypt_ecb(in,ucLen,pucKey,out);
	}
	else
	{
		des2_encrypt_ecb(in,ucLen,pucKey,out);
	}				
}
/*
********************************************************************************
#if 0
	解密
input:  in  --密文 
output: out --明文 
#endif
********************************************************************************
*/
void _MTK_DES_K(uchar keylen,uchar *pucKey,uchar ucLen,uchar *in,uchar *out)
{
	if (keylen == 8)
	{
		des1_decrypt_ecb(in,ucLen,pucKey,out);
	}
	else
	{
		des2_decrypt_ecb(in,ucLen,pucKey,out);
	}						
}

/*
********************************************************************************
********************************************************************************
*/
int mtk_iRetFromRC(int RC)
{
	/*
    #define RC_SUCCESS        0
    #define RC_FAILURE        1
    #define RC_BAD_PARAMETER  2  
	*/

	if (RC == RC_SUCCESS)
	{
		return MTK_SUCCESS;
	}
	else if (RC == RC_BAD_PARAMETER)
	{
		return MTK_ERRPARAM;
	}
	else
	{
		return MTK_ERROR;
	}
}


/*
********************************************************************************
********************************************************************************
*/

int giDelay=0;

int mtk_iExchange(int inlen,unsigned char *pinBuf,unsigned int outmaxlen,int *pretlen,unsigned char *poutBuf)
{    
	int i,ret;
//    int cnt;
	unsigned int len;
	volatile  uchar	sof;              
	unsigned char	len_h;	
	unsigned char	len_l;	
	int begintimer,curtimer;
#ifdef MTK_CTRLDEBUG	
	TRACE("in:");
	vDispBuf(inlen, 0, pinBuf);
#endif		
    mtk_enable_device();    
	for (i=0; i<inlen; i++)
	{
		mtk_WriteByte(pinBuf[i]);
	}
#if 1
	begintimer = sys_get_counter();
	while(1)
	{
		curtimer = sys_get_counter();
		if(curtimer - begintimer > 1000)
		{
			ret = MTK_ERR_SOF;
			goto mtk_iExchange_end; 
		}
		if (mtk_iIfDav() == 1 && mtk_iIfDav() == 1)
		{
			break;
		}		
	}
	sof = mtk_ReadByte();
	sof = mtk_ReadByte();
	sof = mtk_ReadByte();
	if (sof != MTK_SOF)
	{
		ret = MTK_ERR_SOF;
		goto mtk_iExchange_end;  
	}
#else	
	cnt = 10;
	while(1)
	{
		sof = mtk_ReadByte();
		if (sof == MTK_SOF)
		{
			break;
		}
		if (cnt-- <= 0)
		{
			ret = MTK_ERR_SOF;
			goto mtk_iExchange_end;  
		}
	}
	SA_vDelayMs(giDelay*1000);
    cnt = 10;
	while(1)
	{
		sof = mtk_ReadByte();
		if (sof == MTK_SOF)
		{
			break;
		}
		if (cnt-- <= 0)
		{
			ret = MTK_ERR_SOF;
			goto mtk_iExchange_end;  
		}
		SA_vDelayMs(10);
	}
#endif	
	len_h = mtk_ReadByte();
	len_l = mtk_ReadByte();
	len = len_h;
	len = len*256+len_l;
	
	if (len > outmaxlen)
	{
		ret = MTK_ERR_LEN;
		goto mtk_iExchange_end;  
	}

	for (i=0; i<len; i++)
	{
		poutBuf[i]=mtk_ReadByte();
	}
	*pretlen = len;  
	ret = 0;
#ifdef MTK_CTRLDEBUG	
	TRACE("out:");
	vDispBuf(len, 0, poutBuf);
#endif		
mtk_iExchange_end:		
	mtk_disable_device(); 
	return ret;
}

/*
********************************************************************************
SOF LEN MTYP CMD PID
********************************************************************************
*/
int mtk_iGetSoftwareID(int *pRetlen,unsigned char *pDest)
{
	unsigned char inBuff[10];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i;
	int planlen;

	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = 3;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = GET_PROPERTY;
	ptPkt->pid   = 0;
	planlen = 11+2 ;  

	iRet = mtk_iExchange(6,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}

	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;  
    }

	if (rc == RC_SUCCESS)
	{
		memcpy(pDest,outBuff+i,retlen-i);
	}

	*pRetlen = retlen-i;
	return mtk_iRetFromRC(rc);
}

/*
********************************************************************************
********************************************************************************
*/
int mtk_iGetDevSerial(int *pRetlen,unsigned char *pDest)
{
	unsigned char inBuff[10];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;
	

	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = 3;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = GET_PROPERTY;
	ptPkt->pid   = 1;
	planlen = 8+2 ;  
	
	iRet = mtk_iExchange(6,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}

	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;  
    }

	if (rc == RC_SUCCESS)
	{
		memcpy(pDest,outBuff+i,retlen-i);
	}
	*pRetlen = retlen-i;
	return mtk_iRetFromRC(rc);
}

int mtk_iSetDevSerial(unsigned char inlen,unsigned char *pinBuff)
{
	unsigned char inBuff[20];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;

	if (inlen != 8)
	{
		return MTK_ERRPARAM;
	}

	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = inlen+3;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = SET_PROPERTY;
	ptPkt->pid   = 1;
	memcpy(inBuff+EM_MTK_INPKT_LEN,pinBuff,inlen);
	planlen = 0+2 ;  
	//TRACE("LEN:%d",EM_MTK_INPKT_LEN);
	iRet = mtk_iExchange(inlen+EM_MTK_INPKT_LEN,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}

	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;   
    }
	
	return mtk_iRetFromRC(rc);
}

/*
********************************************************************************
KEY CHECK VALUE PROPERTY 
Property ID: 0x0A 
Value length: 2 bytes 
Get Property: Yes 
Set Property: No 
Description: 
    This property contains a 2 byte key check value (KCV). 
    This key check value can be used by the host to help verify that 
    the device contains the proper key. The device derives this property 
    by first TDEA encrypting under the device key an eight byte data field 
    that contains all zeros. The KCV property is the first two bytes of 
    this 8 byte encrypted data field
********************************************************************************
*/
int mtk_iGetKCV(int *pRetlen,unsigned char *pDest)
{
	unsigned char inBuff[10];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;

	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = 3;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = GET_PROPERTY;
	ptPkt->pid   = 0x0A;
	planlen = 2+2 ;  

	iRet = mtk_iExchange(6,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}
	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;   
    }

	if (rc == RC_SUCCESS)
	{
		memcpy(pDest,outBuff+i,retlen-i);
	}
	*pRetlen = retlen-i;
	return mtk_iRetFromRC(rc);
}

/*
********************************************************************************
  get 8B random
ENCRYPTED CHALLENGE PROPERTY    
Property ID: 0x04 
Value length: 8 bytes 
Get Property: Yes 
Set Property: No 
Description: 
    This property contains 8 bytes of encrypted random data. 
This data will vary each time it is retrieved. This data is used 
with the External Authenticate Command.
********************************************************************************
*/
int mtk_iGetEncRandomData(int *pRetlen,unsigned char *pDest)
{
	unsigned char inBuff[10];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;

	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = 3;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = GET_PROPERTY;
	ptPkt->pid   = 0x04;
	planlen = 8+2 ;  

	iRet = mtk_iExchange(6,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}
	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;   
    }

	if (rc == RC_SUCCESS)
	{
		memcpy(pDest,outBuff+i,retlen-i);
	}
	*pRetlen = retlen-i;
	return mtk_iRetFromRC(rc);
}

/*
********************************************************************************
ENCRYPT CARD DATA PROPERTY 
Property ID: 0x02 
Value length: 1 byte 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: 0 (false) 
Description: 
    When this property is set to 1 (true), the card data field in the card 
    data notification message is encrypted. If this property is set to 0 
    (false), the card data field is not encrypted.
********************************************************************************
*/
int mtk_iGetEncryptCaraDataSW(int *pRetlen,unsigned char *pDest)
{
	unsigned char inBuff[10];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;

	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = 3;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = GET_PROPERTY;
	ptPkt->pid   = 0x02;
	planlen = 1+2 ; 

	iRet = mtk_iExchange(6,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}
	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;   
    }

	if (rc == RC_SUCCESS)
	{
		memcpy(pDest,outBuff+i,retlen-i);
	}
	*pRetlen = retlen-i;
	return mtk_iRetFromRC(rc);
}


/*
********************************************************************************
ENCRYPT CARD DATA PROPERTY 
Property ID: 0x02 
Value length: 1 byte 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: 0 (false) 
Description: 
    When this property is set to 1 (true), the card data field in the card 
    data notification message is encrypted. If this property is set to 0 
    (false), the card data field is not encrypted.
********************************************************************************
*/
int mtk_iSetEncryptCaraDataSW(unsigned char inlen,unsigned char *pinBuff)
{
	unsigned char inBuff[20];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;

	if (inlen != 1)
	{
		return MTK_ERRPARAM;
	}
	
	if ((*pinBuff != 0)&&(*pinBuff != 1))
	{
		return MTK_ERRPARAM;
	}

	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = inlen+3;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = SET_PROPERTY;
	ptPkt->pid   = 2;
	memcpy(inBuff+EM_MTK_INPKT_LEN,pinBuff,inlen);
	planlen = 0+2 ;  
	
	iRet = mtk_iExchange(inlen+EM_MTK_INPKT_LEN,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}
	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;  
    }
	
	return mtk_iRetFromRC(rc);
}

/*
********************************************************************************
LOCK DEVICE KEY PROPERTY 
Property ID: 0x03 
Value length: 1 byte 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: 0 (false) 
Description: 
    When this property is set to 1 (true), the device key can no longer 
    be changed unless the External Authenticate Command is successfully 
    issued first. If this property is set to 0 (false), the device key 
    can be changed.
********************************************************************************
*/
int mtk_iGetLockDeviceSW(int *pRetlen,unsigned char *pDest)
{
	unsigned char inBuff[10];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;

	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = 3;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = GET_PROPERTY;
	ptPkt->pid   = 0x03;
	planlen = 1+2 ;  

	iRet = mtk_iExchange(6,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}
	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;   
    }

	if (rc == RC_SUCCESS)
	{
		memcpy(pDest,outBuff+i,retlen-i);
	}
	*pRetlen = retlen-i;
	return mtk_iRetFromRC(rc);
}

/*
********************************************************************************
*     set lock 
********************************************************************************
*/
int mtk_iSetLockDeviceSW(unsigned char inlen,unsigned char *pinBuff)
{
	unsigned char inBuff[20];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;

	if (inlen != 1)
	{
		return MTK_ERRPARAM;
	}
	
	if ((*pinBuff != 0)&&(*pinBuff != 1))
	{
		return MTK_ERRPARAM;
	}

	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = inlen+3;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = SET_PROPERTY;
	ptPkt->pid   = 3;
	memcpy(inBuff+EM_MTK_INPKT_LEN,pinBuff,inlen);
	planlen = 0+2 ;  
	
	iRet = mtk_iExchange(inlen+EM_MTK_INPKT_LEN,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}
	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;  
    }
	
	return mtk_iRetFromRC(rc);
}

/*
********************************************************************************
*     load plaintext key, two group load, Xor two group keys
********************************************************************************
*/
int mtk_iLoadDeviceKey(unsigned char part,unsigned char inlen,unsigned char *pinBuff)
{
	unsigned char inBuff[30];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;

	if ((part != 1)&&(part != 2))
	{
		return MTK_ERRPARAM;
	}

    giDelay = 1;

	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = inlen+3;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = LOAD_DEVICE_KEY;
	ptPkt->pid   = part;
	memcpy(inBuff+EM_MTK_INPKT_LEN,pinBuff,inlen);
	planlen = 0+2 ; 

	iRet = mtk_iExchange(inlen+EM_MTK_INPKT_LEN,inBuff,planlen,&retlen,outBuff);

	giDelay = 0;
	
	if (iRet != 0)
	{
		//DISPERR(iRet);
		return iRet;
	}
	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;  
    }
	
	return mtk_iRetFromRC(rc);
}

/*
********************************************************************************
*     extern auth 
********************************************************************************
*/
int mtk_iExternalAuthCmd(unsigned char inlen,unsigned char *pinDevkey)
{
	unsigned char inBuff[30];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;
	
	unsigned char random_Enc[8];
	unsigned char random_Dec[8];
	
    // only len = 16B
	if (inlen != 16)
	{
		return MTK_ERRPARAM;
	}
	iRet = mtk_iGetEncRandomData(&retlen,random_Enc);
	if (iRet != MTK_SUCCESS)
	{
		return iRet;
	}
	_MTK_DES_K(inlen,pinDevkey,retlen,random_Enc,random_Dec);
	
	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = 6;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = EXTERNAL_AUTHENTICATE_COMMAND;
	
    //No PID, PID is 4B random   
	memcpy(inBuff+EM_MTK_INPKT_LEN-1,random_Dec,4);
	planlen = 0+2 ;  

	iRet = mtk_iExchange(4+EM_MTK_INPKT_LEN-1,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}
	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;   //返回包类型错
    }
	
	return mtk_iRetFromRC(rc);
}

/*
********************************************************************************
  save rom
SOF LEN MTYP CMD 
01 00 02 00 02
********************************************************************************
*/
int mtk_SaveDataCmd(void)
{
	unsigned char inBuff[30];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;
	
	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = 2;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = SAVE_PROPERTY;

	planlen = 0+2 ;  
	iRet = mtk_iExchange(EM_MTK_INPKT_LEN-1,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}
	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;   
    }
	
	return mtk_iRetFromRC(rc);
}


/*
********************************************************************************
   device restart
   SOF LEN   MTYP CMD 
    01 00 02 00   03
********************************************************************************
*/
int mtk_RstDeviceCmd(void)
{
	unsigned char inBuff[30];
	unsigned char outBuff[15];
	unsigned char mtyp;
	unsigned char rc; 
	ET_MTK_INPKT *ptPkt;
	int iRet=0;
	int retlen=0;
	int i,planlen;
	
	ptPkt = (ET_MTK_INPKT *)inBuff;
	ptPkt->sof   = MTK_SOF;
	ptPkt->len_h = 0;
	ptPkt->len_l = 2;
	ptPkt->mtyp  = REQUEST_MESSAGE;
	ptPkt->cmd   = RST_DEVICE;


	planlen = 0+2 ;  //data + mtye + rc 

	iRet = mtk_iExchange(EM_MTK_INPKT_LEN-1,inBuff,planlen,&retlen,outBuff);
	if (iRet != 0)
	{
		return iRet;
	}
	i = 0;
	mtyp = outBuff[i++];
	rc   = outBuff[i++];
    if (mtyp != RESPONSE_MESSAGE)
    {
		return MTK_ERR_ACK;   
    }
	
	return mtk_iRetFromRC(rc);
}


/*
-----------------------------------------------
SOFTWARE ID PROPERTY 
Property ID: 0x00 
Value length: 11 bytes 
Get Property: Yes 
Set Property: No
-----------------------------------------------
DEVICE SERIAL NUMBER 
PROPERTY Property ID: 0x01 
Value length: 8 bytes 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: (Hex) 00 00 00 00 00 00 00 00
-----------------------------------------------
ENCRYPT CARD DATA 
PROPERTY Property ID: 0x02 
Value length: 1 byte 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: 0 (false)
-----------------------------------------------
LOCK DEVICE KEY PROPERTY 
Property ID: 0x03 
Value length: 1 byte 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: 0 (false)
-----------------------------------------------
ENCRYPTED CHALLENGE PROPERTY 
Property ID: 0x04 
Value length: 8 bytes 
Get Property: Yes 
Set Property: No
-----------------------------------------------
RECEIVE INTER CHARACTER TIMEOUT PROPERTY 
Property ID: 0x05 
Value length: 1 byte 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: 20 (2 seconds)
-----------------------------------------------
DATA AVAILABLE TIMEOUT PROPERTY 
Property ID: 0x06 
Value length: 1 byte 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: 0 (disabled)
-----------------------------------------------
TRANSMIT INTER CHARACTER TIMEOUT PROPERTY 
Property ID: 0x07 
Value length: 1 byte 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: 20 (2 seconds)
-----------------------------------------------
SPI CLOCK PHASE AND POLARITY PROPERTY 
Property ID: 0x08 
Value length: 1 byte 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: 0 (Clock Phase = 0, Clock Polarity = 0)
-----------------------------------------------
CLOCK CHARACTER TIMEOUT PROPERTY 
Property ID: 0x09 
Value length: 1 byte 
Get Property: Yes 
Set Property: Yes 
Non-volatile: Yes 
Default value: 20 (2 seconds)
-----------------------------------------------
KEY CHECK VALUE PROPERTY 
Property ID: 0x0A 
Value length: 2 bytes 
Get Property: Yes 
Set Property: No
-----------------------------------------------


-----------------------------------------------

-----------------------------------------------


-----------------------------------------------

*/



/*
********************************************************************************
********************************************************************************
*/
uchar mtk_ucIfDataRdy(void)
{	
	uchar ret;   
	if (mtk_iIfDav() && mtk_iIfDav()) 
	{
	    ret = 0;
    }
	else
	{
	    ret = 1;
    }	
	return ret;
}



void mtk_decrypt_data(uint keylen, uchar *key,uint inlen, uchar *in, uchar *out)
{
	uint j,k;
	uchar block[8];
	k = inlen/8;
	while(1)
	{
		_MTK_DES_K((uchar)keylen,key,8,&in[8*k-8],block);		
		if(k > 1)
		{
			for(j=0;j<8;j++)
			{
				out[8*k-8+j] = block[j] ^ in[8*k-8-8+j];
			}
		#if 0
			TRACE("k:%d",k);
			vDispBuf(8, 0, &out[8*k-8]);
		#endif
		}
		else
		{
			memcpy(out,block,8);
		#if 0
			TRACE("over");
			vDispBuf(8, 0, out);
		#endif
			break;
		}
		--k;
	}
}
// out:  SN(8B) + random(6B) + head(2B) + trk3(88B)+ trk2(88B) + trk1(88B)
uchar mtk_ReadMagCard(uint *outlen, uchar *out)
{
	uchar len_h,len_l;
	uint i,len;	
	uchar ret=0;    
	i = 10;
	while(1)
	{
		if (mtk_ReadByte() == MTK_SOF)
		{
			break;
		}
		if (!i--)
		{
			return MTK_ERR_SOF;
		}
	}

	len_h = mtk_ReadByte();
	len_l = mtk_ReadByte();
	len = (uint)len_h*256+(uint)len_l;
	
	if (len != 282)
	{
		for (i=0; i<len; i++)
		{
			len_h = mtk_ReadByte();
		}	
		return MTK_ERR_LEN;
	}
	//read MTYP=02 NID=00
	if(mtk_ReadByte() != NOTIFICATION_MESSAGE)
	{
		ret = MTK_ERR_ACK;
	}
	if(mtk_ReadByte() != 0)
	{
		ret = MTK_ERR_ACK;
	}
	len -= 2;
	for (i=0; i<len; i++)
	{
		out[i]=mtk_ReadByte();
	}	
    *outlen = len;	
#if 0
	TRACE("---src---");
	vDispBufTitle("SN",8,0,out);
	vDispBufTitle("ran",8,0,out+8);
	vDispBufTitle("trk1",88,0,out+8+6+2+88+88);  //q+8+6+2
	vDispBufTitle("trk2",88,0,out+8+6+2+88);
	vDispBufTitle("trk3",88,0,out+8+6+2);  //q+8+6+2+88+88
#endif	
	return ret;
	
}

uchar mtk_ScanMagCard(uint *b1Len,uint *b2Len,uint *b3Len,uchar *b1, uchar * b2, uchar * b3)
{
	uchar *p,*q;	
	uchar ret;  
	//uchar key[KEY_UNIT_LEN];
	uint i,len;	
	p = pMagBufTmp0;
	ret = mtk_ReadMagCard(&len,p);
#ifdef MTK_CTRLDEBUG	
	TRACE("--%d flg:%d",ret,gMagInfo.mode);	
#endif		
	if(!ret)
	{
		if(IFBIT(gMagInfo.mode,0))
		{								
			//mtk_decrypt_data(16,key,len,p,p+len);
			//memcpy(p+len,p,8);
			mtk_decrypt_data(16,gMagInfo.MagKey,len-8,p+8,p+8+len);
			//q = p+len;
			memcpy(p+8,p+8+len,len-8);
			q = p;
		}
		else
		{
			q = p;
		}
#if 0
		TRACE("---obj---");
		vDispBufTitle("Key",sizeof(gMagInfo),0,&gMagInfo);
		vDispBufTitle("SN",8,0,q);
		vDispBufTitle("ran",8,0,q+8);
		vDispBufTitle("trk1",88,0,q+8+6+2+88+88);  //q+8+6+2
		vDispBufTitle("trk2",88,0,q+8+6+2+88);
		vDispBufTitle("trk3",88,0,q+8+6+2);  //q+8+6+2+88+88
#endif
		//trk3/trk2/trk1
		*b3Len = 0;
		q += 16;
		for(i=0;i<88;i++)
		{
			if(*(q+i) != 0)
			{
				*b3Len = 8*88;
				memcpy(b3,q,88);
				break;
			}
		}
		q += 88;
		*b2Len = 0;
		for(i=0;i<88;i++)
		{
			if(*(q+i) != 0)
			{
				*b2Len = 8*88;
				memcpy(b2,q,88);
				break;
			}
		}
		q += 88;
		*b1Len = 0;
		for(i=0;i<88;i++)
		{
			if(*(q+i) != 0)
			{
				*b1Len = 8*88;
				memcpy(b1,q,88);
				break;
			}
		}
	}
	return ret;
}

int mag_MTK211_init(int mode)
{
    uint i=0;
    uchar len=0;
    uchar magkey[KEY_UNIT_LEN];
    uchar mmk_magkey[KEY_UNIT_LEN];
    SETBIT(gMagInfo.mode,0);
    mtk_enable_VCC();
    CLRBUF(magkey);
    while(1)
    {
        if(!mtk_iLoadDeviceKey(1,sizeof(magkey),magkey))
            break;
        if(i++>10)
        {
            return 1;
        }				
    }
    PCI_ReadKeyFromFlash(0,KEY_TYPE_MAGK,0,len,mmk_magkey)
    while(mtk_iLoadDeviceKey(2,sizeof(mmk_magkey),mmk_magkey));
    while(mtk_iSetEncryptCaraDataSW(1,(uchar *)"\x01"));
    mtk_SaveDataCmd();
    //mtk_iGetKCV(&i,magkey);	
    //vDispBufTitle("KCV", i, 0, magkey);
    mtk_disable_VCC();	
	return 0;
}
int mag_MTK211_open(int mode)
{
    mtk_enable_VCC();
    mtk_enable_device(); 
	return 0;
}

int mag_MTK211_close(int mode)
{
    mtk_disable_device();
    mtk_disable_VCC();	
	return 0;
}

int mag_MTK211_ioctl(int mode,int value)
{
    return (int)mtk_ucIfDataRdy();
}
int mag_MTK211_read(void *Track1, void *Track2, void *Track3)
{
	uchar ret;
	ET_MAGCARD *ptTrk1, *ptTrk2, *ptTrk3;	
	uchar *b1,*b2,*b3;
	uint b1bitlen, b2bitlen, b3bitlen;
	BitBuf SrcBitBuf;	
    int magret = 0x00;
    int maglen = 0;
    uchar *ptr;
	ptTrk1 = (ET_MAGCARD *)&gucBuff[512];  
	ptTrk2 = (ET_MAGCARD *)&gucBuff[512+sizeof(ET_MAGCARD)*1]; 
	ptTrk3 = (ET_MAGCARD *)&gucBuff[512+sizeof(ET_MAGCARD)*2];
	memset((char *)ptTrk1, 0, sizeof (ET_MAGCARD));
	memset((char *)ptTrk2, 0, sizeof (ET_MAGCARD));
	memset((char *)ptTrk3, 0, sizeof (ET_MAGCARD));
	b1 = pMagBufTmp1;
	b2 = b1+MAG_UINT_LEN;
	b3 = b2+MAG_UINT_LEN;	
	memset(b1,0,MAG_UINT_LEN);
	memset(b2,0,MAG_UINT_LEN);
	memset(b3,0,MAG_UINT_LEN);
	ret = mtk_ScanMagCard(&b1bitlen,&b2bitlen,&b3bitlen,b1,b2,b3);
#ifdef MTK_CTRLDEBUG		
	TRACE("%d %d-%d-%d",ret,b1bitlen,b2bitlen,b3bitlen);
#endif
	if(ret)
	{
        magret = 0x70;
		goto app_read_magcard_end;
	}
	// Trk1 decode
	ret = EM_mag_NULL;
	if(b1bitlen)
	{		
		bitbufInit(&SrcBitBuf, b1, MAG_UINT_LEN * 8);
		ret = SI_mag_ucDecodeTrk1(&SrcBitBuf, b1bitlen, &ptTrk1->ucTrkLength, ptTrk1->aucTrkBuf, 1);	// forward
	#ifdef MTK_CTRLDEBUG	
		TRACE("Trk1 forward:%X", ret);
	#endif
		if (ret != EM_mag_SUCCESS)
		{
			ret = SI_mag_ucDecodeTrk1(&SrcBitBuf, b1bitlen, &ptTrk1->ucTrkLength, ptTrk1->aucTrkBuf, 0);	// backward
		#ifdef MTK_CTRLDEBUG	
			TRACE("Trk1 backward:%X", ret);
		#endif
		}
		if (ret == EM_mag_SUCCESS)
		{
			ptTrk1->ucTrkFlag = EM_mag_SUCCESS;
		}
	}
	if(ret)
	{
		memset((uchar *)ptTrk1,0,sizeof(ET_MAGCARD));
		ptTrk1->ucTrkFlag = ret;
        magret |= 0x10;
    }else{
        magret |= 0x01;
        maglen = ptTrk1->ucTrkLength;
        magret = magret|(maglen<<8);
        ptr = (uchar *)Track1;
        memcpy(ptr, ptTrk1->aucTrkBuf, ptTrk1->ucTrkLength);
    }
	
	// Trk2 decode
	ret = EM_mag_NULL;
	if(b2bitlen)
	{
		bitbufInit(&SrcBitBuf, b2, MAG_UINT_LEN * 8);
		ret = EI_mag_vProcTrk23(&SrcBitBuf, b2bitlen, &ptTrk2->ucTrkLength, ptTrk2->aucTrkBuf, 1);	// forward
	#ifdef MTK_CTRLDEBUG	
		TRACE("Trk2 forward:%X", ret);
	#endif
		if (ret != EM_mag_SUCCESS)
		{
			ret = EI_mag_vProcTrk23(&SrcBitBuf, b2bitlen, &ptTrk2->ucTrkLength, ptTrk2->aucTrkBuf, 0);	// backward
		#ifdef MTK_CTRLDEBUG	
			TRACE("Trk2 backward:%X", ret);
		#endif
		}
		if (ret == EM_mag_SUCCESS)
		{
			ptTrk2->ucTrkFlag = EM_mag_SUCCESS;
		}
	}
	if(ret)
	{
		memset((uchar *)ptTrk2,0,sizeof(ET_MAGCARD));
		ptTrk2->ucTrkFlag = ret;
        magret |= 0x20;
    }else{
        magret |= 0x02;
        maglen = ptTrk2->ucTrkLength;
        magret = magret|(maglen<<16);
        ptr = (uchar *)Track2;
        memcpy(ptr, ptTrk2->aucTrkBuf, ptTrk2->ucTrkLength);
    }
	
	// Trk3 decode
	ret = EM_mag_NULL;
	if(b3bitlen)
	{
		bitbufInit(&SrcBitBuf, b3, MAG_UINT_LEN * 8);
		ret = EI_mag_vProcTrk23(&SrcBitBuf, b3bitlen, &ptTrk3->ucTrkLength, ptTrk3->aucTrkBuf, 1);	// forward
	#ifdef MTK_CTRLDEBUG	
		TRACE("Trk3 forward:%X", ret);
	#endif
		if (ret != EM_mag_SUCCESS)
		{
			ret = EI_mag_vProcTrk23(&SrcBitBuf, b3bitlen, &ptTrk3->ucTrkLength, ptTrk3->aucTrkBuf, 0);	// backward
		#ifdef MTK_CTRLDEBUG	
			TRACE("\r\n Trk3 backward:%X", ret);
		#endif
		}
		if (ret == EM_mag_SUCCESS)
		{
			ptTrk3->ucTrkFlag = EM_mag_SUCCESS;
		}
	}
	if(ret)
	{
		memset((uchar *)ptTrk3,0,sizeof(ET_MAGCARD));
		ptTrk3->ucTrkFlag = ret;
        magret |= 0x40;
    }else{
        magret |= 0x04;
        maglen = ptTrk3->ucTrkLength;
        magret = magret|(maglen<<24);
        ptr = (uchar *)Track3;
        memcpy(ptr, ptTrk3->aucTrkBuf, ptTrk3->ucTrkLength);
    }
#ifdef MTK_CTRLDEBUG	
	vDispBufTitle("trk",sizeof(ET_MAGCARD)*3,0,(uchar *)ptTrk1);
	vDispBufTitle("trk2",sizeof(ET_MAGCARD),0,(uchar *)ptTrk2);
	vDispBufTitle("trk3",sizeof(ET_MAGCARD),0,(uchar *)ptTrk3);
#endif
	mag_close();
	PPRT_Send(CMD_READ_MAGCARD,(uchar *)ptTrk1,sizeof(ET_MAGCARD)*3);
	return magret;
app_read_magcard_end:
	mag_close();
	PPRT_Send(CMD_READ_MAGCARD,&ret,1);
	return magret;
}
int mag_MTK211_main(int mode)
{
    if (mtk_ucIfDataRdy() == EM_SUCCESS){
        mag_MTK211_read(0);
        mag_MTK211_open(0);
    }
    return 0;
}



/*
********************************************************************************
********************************************************************************
*/
#ifdef EM_DEBUG
void mtk_id_test(void)
{
	//unsigned int uiKey;	
	unsigned char buff[20];
	int ilen=0;
	int iRet;

	CLRBUF(buff);

	iRet = mtk_iGetSoftwareID(&ilen,buff);
	TRACE("ID=%2X",iRet);
}

void mtk_serial_test(void)
{
	unsigned int uiKey;	
	unsigned char buff[20];
	int ilen=0;
	int iRet;
	
	while (1)
	{ 
		TRACE("1-read 2-write");
		uiKey = InkeyCount(0);
		
		switch (uiKey)
		{
			case 1:
				CLRBUF(buff);				
				iRet = mtk_iGetDevSerial(&ilen,buff);
				break;	
			case 2:
				CLRBUF(buff);
				memcpy(buff,"20100518",8);
				iRet = mtk_iSetDevSerial(8,buff);
				break;
			case 3:
				CLRBUF(buff);
				memcpy(buff,"20110101",8);
				iRet = mtk_iSetDevSerial(8,buff);
				break;
			case 4:
				CLRBUF(buff);
				iRet = mtk_iSetDevSerial(8,buff);
				break;
			case 99:
				return;
		} 
		TRACE("%X",iRet);
	}		
}


void mtk_encryptcarddata_test(void)
{
	unsigned int uiKey;	
	unsigned char buff[20];
	int ilen=0;
	int iRet;
	
	while (1)
	{ 
		TRACE("1-read 2-close 3-open");
		uiKey = InkeyCount(0);		
		switch (uiKey)
		{
			case 1:
				iRet = mtk_iGetEncryptCaraDataSW(&ilen,buff);
				TRACE("%X",iRet);
				if(!iRet)
				{
					TRACE("flg:%d",buff[0]);
						
				}
				break;	
			case 2:
			case 3:
				uiKey -= 2;
				iRet = mtk_iSetEncryptCaraDataSW(1,(unsigned char*)&uiKey);
				if(!iRet)
				{
                    if ( uiKey ) {
                        SETBIT(gMagInfo.mode,0);
                    } else {
                        CLRBIT(gMagInfo.mode,0);
                    }
				}
				TRACE("%X",iRet);
				break;
			case 99:
				return;
		}  
	}		
}


void mtk_lockdevice_test(void)
{
	unsigned int uiKey;	
	unsigned char buff[20];
	int ilen=0;
	int iRet;
	
	while (1)
	{ 
		TRACE("1-read 2-unlock 3-lock");
		uiKey = InkeyCount(0);
		switch (uiKey)
		{
			case 1:
				iRet = mtk_iGetLockDeviceSW(&ilen,buff);
				TRACE("%X",iRet);
				if(!iRet)
				{
					TRACE("flg:%d",buff[0]);
						
				}
				break;	
			case 2:
			case 3:
				uiKey -= 2;
				iRet = mtk_iSetLockDeviceSW(1,(unsigned char*)&uiKey);
				TRACE("%X",iRet);
				break;
			case 99:
				return;
		}  
	}		
}

void mtk_loaddevicekey_test(void)
{
	unsigned int uiKey;	
	unsigned char buff[20];
	//int ilen=0;
	int iRet;
	
	while (1)
	{ 
		TRACE("LOAD KEY:"); 	
		switch (InkeyCount(0))
		{
			case 1:
				CLRBUF(buff);								
/*				
				buff[0] = 0x46;
				buff[1] = 0xEB;
				buff[2] = 0x17;
				buff[3] = 0xC3;
				buff[4] = 0x27;
				buff[5] = 0xD0;
				buff[6] = 0xB6;
				buff[7] = 0xA3;
				buff[8] = 0x95;
				buff[9] = 0x27;
				buff[10] = 0x21;
				buff[11] = 0x52;
				buff[12] = 0x14;
				buff[13] = 0x6D;
				buff[14] = 0x08;
				buff[15] = 0xA3;
*/	
				iRet = 16;	
				iRet = mtk_iLoadDeviceKey(1,iRet,buff);
				TRACE("%X",iRet);
				break;	
			case 2:
				CLRBUF(buff);								
/*
				buff[0] = 0x46;
				buff[1] = 0xFA;
				buff[2] = 0x35;
				buff[3] = 0xF0;
				buff[4] = 0x63;
				buff[5] = 0x85;
				buff[6] = 0xD0;
				buff[7] = 0xD4;
				buff[8] = 0x1D;
				buff[9] = 0xBE;
				buff[10] = 0x8B;
				buff[11] = 0xE9;
				buff[12] = 0xD8;
				buff[13] = 0xB0;
				buff[14] = 0xE6;
				buff[15] = 0x5C;
				buff[16] = 1;
				iRet = 17;					
*/		
				buff[0] = 0x00;
				buff[1] = 0x11;
				buff[2] = 0x22;
				buff[3] = 0x33;
				buff[4] = 0x44;
				buff[5] = 0x55;
				buff[6] = 0x66;
				buff[7] = 0x77;
				buff[8] = 0x88;
				buff[9] = 0x99;
				buff[10] = 0xAA;
				buff[11] = 0xBB;
				buff[12] = 0xCC;
				buff[13] = 0xDD;
				buff[14] = 0xEE;
				buff[15] = 0xFF;

/*
buff[8] = 0x00;
buff[9] = 0x11;
buff[10] = 0x22;
buff[11] = 0x33;
buff[12] = 0x44;
buff[13] = 0x55;
buff[14] = 0x66;
buff[15] = 0x77;
*/

				iRet = 16;			
				iRet = mtk_iLoadDeviceKey(2,iRet,buff);
				TRACE("%X",iRet);	
				break;
			case 3:
				memset(buff,0x11,16);
				iRet = mtk_iLoadDeviceKey(1,16,buff);
				TRACE("%X",iRet);
				memset(buff,0x22,16);
				iRet = mtk_iLoadDeviceKey(2,16,buff);
				TRACE("%X",iRet);
				break;
			case 4:
				memset(buff,0,16);
				iRet = mtk_iLoadDeviceKey(1,16,buff);
				iRet = mtk_iLoadDeviceKey(2,16,buff);
				break;
			case 99:
				return;
		}  
	}
}
void mtk_GPIOTest(void)
{
	mtk_Init();
	while(1)
	{
		//TRACE("[All low]");				
		MTK_CLR_SPI_CLK();	
		MTK_CLR_SPI_MOSI();
		mtk_enable_device();		
		mtk_disable_VCC();
		InkeyCount(0);
				
		//TRACE("[All high]");
		MTK_SET_SPI_CLK();	
		MTK_SET_SPI_MOSI();
		mtk_disable_device();
		mtk_enable_VCC();
		if (InkeyCount(0) == 99)
		{
			break;
		}
	}
}




void mtk_test(void)
{
	unsigned int uiKey;	
	unsigned char buff[20];
	int ilen=0;
	int iRet;

	CLRBUF(buff);  
	while (1)
	{ 
		mtk_Init();	
		mtk_enable_VCC();
		TRACE("1-ID 2-SN 3-KCV 4-random 5-wipe 6-Encryption");
		TRACE("7-lock 8-loadkey 9-auth 10-restart");
		TRACE("11-save 12-gpio 13-testMagcard");
		uiKey = InkeyCount(0);
		//uiKey = 5;
		switch (uiKey)
		{
			case 1:
				mtk_id_test();
				 break;	
			case 2:
	 			mtk_serial_test();
				 break;
			case 3: 
				CLRBUF(buff);
				iRet = mtk_iGetKCV(&ilen,buff);
				TRACE("GetKCV=%2X",iRet);
				break;
			case 4:
				iRet = mtk_iGetEncRandomData(&ilen,buff);
				TRACE("GetEncRandom=%2X",iRet);
			    break;
			case 5:
				TRACE("wipe...");
				ilen = 0;
				while(1)
				{
					if(mtk_ucIfDataRdy())
					{
						mtk_ReadMagCard((uint *)&ilen,gucBuff);
						break;
					}
					if(IfInkey(0))
						break;
				}	
				if(ilen)
					vDispBuf(ilen, 0, gucBuff);
				break;
			case 6:
				mtk_encryptcarddata_test();
				break;
			case 7:
				mtk_lockdevice_test();
				break;
			case 8: 
				mtk_loaddevicekey_test();
				break;
			case 9:
				CLRBUF(buff);	
				uiKey = InkeyCount(0);
				if(uiKey == 1)
				{
					buff[0] = 0x00;
					buff[1] = 0x11;
					buff[2] = 0x22;
					buff[3] = 0x33;
					buff[4] = 0x44;
					buff[5] = 0x55;
					buff[6] = 0x66;
					buff[7] = 0x77;

					buff[8] = 0x88;
					buff[9] = 0x99;
					buff[10] = 0xAA;
					buff[11] = 0xBB;
					buff[12] = 0xCC;
					buff[13] = 0xDD;
					buff[14] = 0xEE;
					buff[15] = 0xFF;
				}
				else if(uiKey == 2)
				{
					memset(buff,0x30,16);
				}
				TRACE("Authk");
				vDispBuf(16, 0, buff);
				iRet = mtk_iExternalAuthCmd(16,buff);
				break;
			case 10:
				TRACE("recover");				
				iRet = mtk_RstDeviceCmd();
				break;
			case 11:
				TRACE("save setup");					
				iRet = mtk_SaveDataCmd();			
				break;
			case 12:
				mtk_GPIOTest();
				break;
			case 13:
				//testMagcard();
				break;
			case 14:
				break;
			case 15:
				mtk_disable_VCC();
				delay_ms(100);
				break;
			case 99:
		    	return;		
		}	
		
		if (iRet == EM_SUCCESS)
		{
		    TRACE("OK");
	    }
	    else
	    {
			TRACE("ERR=%x",iRet);
	    }	
	}	
}
#endif


#endif
#endif
/*
********************************************************************************
*                                  END FILE
********************************************************************************
*/




