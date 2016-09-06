/***************** (C) COPYRIGHT 2015 START Computer equipment *****************
 * File Name          : security_alg.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/29/2015 4:29:19 PM
 * Description        : 
 *******************************************************************************/
#include "drv_inc.h"
#include "security_alg.h"

/** Cal ECB MAC
 *
 * @param  keylen: 
 * @param  key   output data
 * @param  inlen    length of data
 * @param  in       DES key(s)
 * @param  out   number of keys to use for decryption, 1, 2, or 3
 */
void cacul_mac_ecb(uint16_t keylen, uint8_t *key, uint32_t inlen, uint8_t *input, uint8_t outlen ,uint8_t *out)
{
	uint32_t i;
	uint8_t j,m,n;
	uint8_t ucIBuf[8];
	uint8_t ucOBuf[8];

	memset(ucIBuf,0,sizeof(ucIBuf));
	m = inlen/8;
	n = inlen%8;
    i = 0;
	while(m--)
	{	
		for(j=0;j<8;j++)
		{
			ucIBuf[j] ^= input[i+j];
		}
		i += 8;		
	}
	if(n)
	{
		for(j=0;j<n;j++)
		{
			ucIBuf[j] ^= input[i+j];
		}	
	}
	des_encrypt_ecb(ucIBuf, ucOBuf, 8, key, keylen/8);	
	memcpy(out,ucOBuf,outlen);
}

uint8_t CalculateMac(uint32_t uiMode,
	uint32_t uiKeyLen,
	uint8_t * pucKey,
	uint32_t uiLenIn, uint8_t * pucDataIn, uint32_t * puiLenOut, uint8_t * pucDataOut)
{
	uint32_t i, j, uiTimes;
	uint8_t ucTempBuf1[8], ucTempBuf2[8];
	uint8_t ucKey1[8], ucKey2[8], ucKey3[8];
    uint8_t buffer[1024];
	uint8_t *pucBuffer;
	switch (uiMode & 0x0000f0)
	{
	case 0x0000000:					// mode 1
	case 0x0000010:					// mode 2
		break;
	default:
		return EM_ERRPARAM;				//not support
	}
	switch (uiMode & 0x000000f)
	{
	case 0x00000000:					// algo 1
	case 0x00000002:					// algo 3
		break;
	default:
		return EM_ERRPARAM;				//not support
	}
	if ((uiKeyLen != 8) && (uiKeyLen != 16) && (uiKeyLen != 24))
	{
		return EM_ERRPARAM;				// len of the key is 8、16、24
	}
	if ((uiLenIn > 1024) || (uiLenIn == 0))
	{
		return EM_ERRPARAM;				// the data of encry not bigger than 4096
	}
	if (pucKey == EM_NULL)
	{
		return EM_ERRPARAM;
	}
	if (pucDataIn == EM_NULL)
	{
		return EM_ERRPARAM;
	}
	if (puiLenOut == EM_NULL)
	{
		return EM_ERRPARAM;
	}
	if (pucDataOut == EM_NULL)
	{
		return EM_ERRPARAM;
	}
	pucBuffer = buffer;
	if (!pucBuffer)
	{
		return EM_ERROR;
	}	
	uiTimes = (uiLenIn - 1) / 8 + 1;
	memset(pucBuffer, 0x00, 1024);		//00   
	memcpy(pucBuffer, pucDataIn, uiLenIn);	
	switch (uiMode & 0x0000f0)
	{
	case 0x00000000:					// mode 1，right filed with 00
		break;
	case 0x00000010:					// mode 2，right filled with one 0x80 and  some 0x00
		if (uiLenIn % 8)
		{
			pucBuffer[uiLenIn] = 0x80;
		}
		break;
	case 0x00000020:					// mode 3，         
	default:
		return EM_ERROR;
	}

	memset(ucTempBuf2, 0x00, sizeof(ucTempBuf2));
	memcpy(ucKey1, pucKey, 8);
	if (uiKeyLen == 16)
	{
		memcpy(ucKey2, pucKey + 8, 8);
		memcpy(ucKey3, ucKey1, 8);
	}
	else if (uiKeyLen == 24)
	{
		memcpy(ucKey2, pucKey + 8, 8);
		memcpy(ucKey3, pucKey + 16, 8);
	}

	//
	switch (uiMode & 0x0000000f)
	{
	case 0x00000000:					// algo 1
		for (i = 0; i < uiTimes; i++)
		{
			for (j = 0; j < 8; j++)
			{
				ucTempBuf1[j] = pucBuffer[8 * i + j] ^ ucTempBuf2[j];
			}
			des_encrypt_ecb(ucTempBuf1, ucTempBuf2, 8, pucKey, uiKeyLen / 8);
		}
		memcpy(pucDataOut, ucTempBuf2, 8);
		break;
	case 0x00000002:					// algo 3
		for (i = 0; i < uiTimes; i++)
		{
			for (j = 0; j < 8; j++)
			{
				ucTempBuf1[j] = pucBuffer[8 * i + j] ^ ucTempBuf2[j];
			}
			des_encrypt_ecb(ucTempBuf1, ucTempBuf2, 8, ucKey1, 1);
		}
		if (uiKeyLen != 0x08)
		{
			des_decrypt_ecb(ucTempBuf2, ucTempBuf1, 8, ucKey2, 1);
			des_encrypt_ecb(ucTempBuf1, ucTempBuf2, 8, ucKey3, 1);
		}
		memcpy(pucDataOut, ucTempBuf2, 8);
		break;
	case 0x00000001:					// algo 2               
	case 0x00000003:					// algo 4
	case 0x00000004:					// algo 5
	case 0x00000005:					// algo 6
	default:
		return EM_ERROR;				//
	}
	*puiLenOut = 8;
	return EM_SUCCESS;
}




#define DEBUG1(x) //do{x}while(0)


/*
 * s_CaltMac_ECB - [GENERIC] 1-ECB_1  3-ECB
 * @
 */
int s_CaltMac_ECB(uchar mode, uchar padding, uchar *start, uchar *key, uint inlen, uchar *inbuf, uchar *mac)
{
    uint i;
	uchar j,m,n;
	uchar buf0[16],buf1[16];
	m = inlen/8;
	n = inlen%8;
    i = 8*m;
	if(m) {
        Buf8ByteXor(i,inbuf,buf0);
		for(j=0;j<8;j++)
		{
			buf0[j] ^= start[j];
		}
	} else {
        memcpy(buf0,start,8);
    }
    if ( n ) {
        if(IFBIT(padding,0)){
            inbuf[i+n] = 0x80;
        } else {
            inbuf[i+n] = 0x00;
        }
        for ( j=1 ; j<(8-n) ; j++ ) {
            inbuf[i+n+j] = 0x00;
        }
		for(j=0;j<8;j++) {
			buf0[j] ^= inbuf[i+j];
		}
    }
    DEBUG1(vDispBufTitle("last",8,0,buf0););
//    TRACE("1:%x",!IFBIT(padding,7),IFBIT(padding,7));
    if ( !IFBIT(padding,3) ) {
        if ( mode == 1 ) {
            // ECB_1
            des_encrypt_ecb(buf0,buf1,8,key,3);//K
        } else {
            // ECB
            // 1. 8bytes Xor -> 16 bytes Xor
            SI_ucParseStr(2,8,buf0,buf1);
            DEBUG1(vDispBufTitle("1",8,0,buf1););
            // 2. encrypt 1st-8-bytes
            des_encrypt_ecb(buf1,buf0,8,key,3);//K
            DEBUG1(vDispBufTitle("2",8,0,buf0););
            // 3. Xor  2rd-8-bytes
            for(j=0;j<8;j++)
            {
                buf1[j] = buf0[j]^buf1[8+j];
            }
            DEBUG1(vDispBufTitle("3",8,0,buf1););
            // 4. encrypt Xor result
            des_encrypt_ecb(buf1,buf0,8,key,3);//K
            DEBUG1(vDispBufTitle("4",8,0,buf0););
            // 5. 8bytes Xor -> 16 bytes Xor
            SI_ucParseStr(2,8,buf0,buf1);
            DEBUG1(vDispBufTitle("5",8,0,buf1););
        }
        memcpy(mac,buf1,8);
    } else {
        memcpy(mac,buf0,8);
    }
    return 0;
}		/* -----  end of function s_CaltMac_ECB  ----- */

/*
 * s_CaltMac_X99 - [GENERIC] 0-X99 2-X99_1
 * @
 */
int s_CaltMac_X99 (uchar mode, uchar padding, uchar *start, uchar *key, uint inlen, uchar *inbuf, uchar *mac)
{
    uint i;
	uchar k,j,m,n;
	uchar buf0[8],buf1[8];
	m = inlen/8;
	n = inlen%8;
    i = 0;
	memcpy(buf1,start,8);
	if(m)
	{
        for(k=0;k<m;k++)
        {
            for(j=0;j<8;j++)
            {
                buf0[j] = inbuf[i+j] ^ buf1[j];
            }
             DEBUG1(vDispBufTitle("m0",8,0,buf0););
            if(!mode)
            {
                //X99
                des_encrypt_ecb(buf0,buf1,8,key,3);//K
            }
            else
            {
                des_encrypt_ecb(buf0,buf1,8,key,1);//K1
            }
            DEBUG1(vDispBufTitle("m1",8,0,buf1););
            i += 8;
        }
	}
    if ( n ) {
        if(IFBIT(padding,0)){
            inbuf[i+n] = 0x80;
        } else {
            inbuf[i+n] = 0x00;
        }
        for ( j=1 ; j<(8-n) ; j++ ) {
            inbuf[i+n+j] = 0x00;
        }
		for (j=0;j<8;j++) {
			buf0[j] = buf1[j] ^ inbuf[i+j];
		}
        DEBUG1(vDispBufTitle("n0",8,0,buf0););
        if(!mode)
        {
            //X99
            des_encrypt_ecb(buf0,buf1,8,key,3); //K
        }
        else
        {
            des_encrypt_ecb(buf0,buf1,8,key,1); //K1
        }
        DEBUG1(vDispBufTitle("n1",8,0,buf1););
    }
    if ( mode ) {
        //X99_1
        if ( !IFBIT(padding,3) ) {
            //最后1块
            des_decrypt_ecb(buf1,buf0,8,&key[8],1);//K2
            DEBUG1(vDispBufTitle("L1",8,0,buf0););
            des_encrypt_ecb(buf0,buf1,8,&key[16],1);//K3
            DEBUG1(vDispBufTitle("L2",8,0,buf1);); 
        }
    }
	DEBUG1(vDispBufTitle("MAC",8,0,buf1););
    memcpy(mac,buf1,8);
    return 0;

}		/* -----  end ------- */
// mode:0-X99 1-ECB 2-X99_1 3-ECB_1 4-X919
// padding:0-padding with 0x00 1-padding with 0x80 
// start: 8bytes xor init value
uchar s_CaltMAC(uchar mode, uchar padding, uchar *start, uchar *key, uint inlen, uchar *inbuf, uchar *mac)
{
#if 2
	DEBUG1(
		TRACE("%d-%x-%d",mode,padding,inlen);
		vDispBufTitle("st",8,0,start);	
		vDispBufTitle("in",inlen,0,inbuf);
		vDispBufTitle("Ke",24,0,key);
	);
    switch ( mode )
    {
    case 0 :
    case 2 :
        s_CaltMac_X99(mode,padding,start,key,inlen,inbuf,mac);
        break;
    case 1 :
    case 3 :
        s_CaltMac_ECB(mode,padding,start,key,inlen,inbuf,mac);
        break;
    case 4 :
//        s_CaltMAC_X919(padding,start,key,inlen,inbuf,mac);
        s_CaltMac_X99(2,padding,start,key,inlen,inbuf,mac);
        break;
    default :
        break;
    }
    return 0;
#else
	uint i;
	uchar k,j,m,n;
	uchar buf0[16],buf1[16],fillblock[8];
	m = inlen/8;  
	n = inlen%8;	
	memcpy(buf1,start,8);
	DEBUG1(
		TRACE("%d-%x-%d",mode,padding,inlen);
		vDispBufTitle("st",8,0,start);	
		vDispBufTitle("in",inlen,0,inbuf);
		vDispBufTitle("Ke",24,0,key);
	);
	i = 0;
	if(m)
	{
		if(mode == 1 || mode == 3)
		{
			i = (uint)m*8;
			Buf8ByteXor(i,inbuf,buf0);
			Str8ByteXor(8,buf1,buf0,buf1);
		}
		else
		{
			for(k=0;k<m;k++)
			{
				for(j=0;j<8;j++)
				{
					buf0[j] = inbuf[i+j] ^ buf1[j];
				}
				DEBUG1(
					vDispLable(1,32);
					TRACE("%d",k);
					vDispBufTitle("O1",8,0,buf1);	
					vDispBufTitle("Xor",8,0,&inbuf[i]);	
					vDispBufTitle("I1",8,0,buf0);
				);
				if(mode == 0)
				{
					des_encrypt_ecb(buf0,buf1,8,key,3);
				}
				else
				{
					des_encrypt_ecb(buf0,buf1,8,key,1);
				}
				i += 8;		
			}
		}
	}
	CLRBUF(fillblock);
	if((padding&0x01)==0x01)
	{
		fillblock[0] = 0x80;
	}
	DEBUG1(vDispBufTitle("O1",8,0,buf1););
	if(n)
	{
		memcpy(&inbuf[i+n],fillblock,8-n);
		DEBUG1(vDispBufTitle("last",8,0,&inbuf[i]););
		for(j=0;j<8;j++)
		{
			buf0[j] = inbuf[i+j] ^ buf1[j];
		}
		if(mode == 0)
		{
			//标准X99最后1块
            des_encrypt_ecb(buf0,buf1,8,key,3);
			goto caltMAC_X99;
		}
        else if(mode == 1){
			//非标准ECB 最1后1块
			goto caltMAC_ECB;
        }
		else if(mode == 2)
		{
			//非标准X99
			goto caltMAC_X99_1;
		}
		else if(mode == 3)
		{
			//标准ECB
			goto caltMAC_ECB_1;
		}
		else
		{
			//X919
			des_encrypt_ecb(buf0,buf1,8,key,1);
			goto caltMAC_X919;
		}
	}
	else
	{
		memcpy(buf0, buf1, 8);
		if(mode == 0)
		{
	caltMAC_X99:
		}
		else if(mode == 1)
		{
	caltMAC_ECB:
			DEBUG1(vDispBufTitle("ECB",8,0,buf0););	
			if((padding&0x08)==0){
                //last block
                des_encrypt_ecb(buf0,buf1,8,key,3);
            }
		}
		else if(mode == 2)
		{
			//解密最后1块
			des_decrypt_ecb(buf1,buf0,8,key,1);
	caltMAC_X99_1:
			DEBUG1(vDispBufTitle("X99_1",8,0,buf0););	
			des_encrypt_ecb(buf0,buf1,8,key,3);
			// result is MAC
		}
		else if(mode == 3)
		{	
	caltMAC_ECB_1:
			DEBUG1(vDispBufTitle("ECB_1",8,0,buf0););	
			if((padding&0x08)==0)
			{
				// 1. 8bytes Xor -> 16 bytes Xor
				SI_ucParseStr(2,8,buf0,buf1);
				DEBUG1(vDispBufTitle("1",16,0,buf1););	
				// 2. encrypt 1st-8-bytes 
				des_encrypt_ecb(buf1,buf0,8,key,3);
				DEBUG1(vDispBufTitle("2",8,0,buf0););
				// 3. Xor  2rd-8-bytes
				for(j=0;j<8;j++)
				{
					buf0[j] ^= buf1[8+j];
				}
				DEBUG1(vDispBufTitle("3",8,0,buf0););
				// 4. encrypt Xor result
				des_encrypt_ecb(buf0,buf1,8,key,3);
				DEBUG1(vDispBufTitle("4",8,0,buf1););
				// 5. 8bytes Xor -> 16 bytes Xor
				SI_ucParseStr(2,8,buf1,buf0);
				DEBUG1(vDispBufTitle("5",16,0,buf0););
			}		
			// 6. 1st-8-bytes is MAC
			memcpy(buf1,buf0,8);
		}
		else if(mode == 4)
		{
			//X919
 //2012-12-07 X919 not fillblock when lenght is 8-bytes
//			D1(vDispBufTitle("last",8,0,fillblock););
//			for(j=0;j<8;j++)
//			{
//				buf0[j] = fillblock[j] ^ buf1[j];
//			}	
	caltMAC_X919:	
				DEBUG1(vDispBufTitle("X919",8,0,buf1););	
				if((padding&0x08)==0)
				{	
					// 1.decrypt K2 
					des_decrypt_ecb(buf1,buf0,8,&key[8],1);
					DEBUG1(vDispBufTitle("M1",8,0,buf0););
					// 2.encrypt K1(16B)或K3(24B)
                    des_encrypt_ecb(buf0,buf1,8,&key[16],1);
				}	
		}
	}
	DEBUG1(vDispBufTitle("MAC",8,0,buf1););	
	memcpy(mac,buf1,8);
	return 0;
#endif
}



