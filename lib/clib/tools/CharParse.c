/*********************************************************************
******************************************************************************/
#include "common.h"
#include "drv_inc.h"
#include "MacroDef.h"

//#define __DEBUG__

unsigned char AscToHex(unsigned char *ucBuffer)
{
	unsigned char tmp;
	if (ucBuffer[0] >= '0' && ucBuffer[0] <= '9')
	{
		tmp = (unsigned char)((ucBuffer[0] - '0') << 4);
	}
	else if (ucBuffer[0] >= 'a' && ucBuffer[0] <= 'f')
	{
		tmp = (unsigned char)((ucBuffer[0] - 'a' + 10) << 4);
	}
	else if (ucBuffer[0] >= 'A' && ucBuffer[0] <= 'F')
	{
		tmp = (unsigned char)((ucBuffer[0] - 'A' + 10) << 4);
	}
	else
	{
		return 0;
	}
	if (ucBuffer[1] >= '0' && ucBuffer[1] <= '9')
	{
		tmp |= (unsigned char)(ucBuffer[1] - '0');
	}
	else if (ucBuffer[1] >= 'a' && ucBuffer[1] <= 'f')
	{
		tmp |= ((ucBuffer[1] - 'a') + 10);
	}
	else if (ucBuffer[1] >= 'A' && ucBuffer[1] <= 'F')
	{
		tmp |= ((ucBuffer[1] - 'A') + 10);
	}
	else
	{
		tmp = 0;
	}
	return tmp;
}
void HexToAsc(unsigned char ucData, unsigned char *pucDataOut)
{
	unsigned char tmp;
	tmp = ucData >> 4;
	if (tmp >= 10)
	{
		tmp += ('A'-10);
	}
	else
	{
		tmp += '0';
	}
	pucDataOut[0] = tmp;
	tmp = ucData & 0x0F;
	if (tmp >= 10)
	{
		tmp += ('A'-10);
	}
	else
	{
		tmp += '0';
	}
	pucDataOut[1] = tmp;
}

int Str2Hex(int in_len, unsigned char *in, int *outlen, unsigned char *out)
{
	int i, j;
	unsigned char tmp, data;
	j = 0;
	tmp = 4;
	for (i = 0; i < in_len; i++)
	{

		if ((in[i] >= '0') && (in[i] <= '9'))
		{
			data = in[i] - '0';
		}
		else if ((in[i] >= 'A') && (in[i] <= 'F'))
		{
			data = in[i] - 'A' + 10;
		}
		else if ((in[i] >= 'a') && (in[i] <= 'f'))
		{
			data = in[i] - 'a' + 10;
		}
		else
		{
			continue;
		}

		data = (data << tmp);
		if (tmp == 4)
		{
			//high half byte
			out[j] = data;
			tmp = 0;
		}
		else
		{
			//low half byte 
			out[j] |= data;
			j++;
			tmp = 4;
		}
	}
	if (tmp == 4)
	{
		*outlen = j;
		return 0;
	}
	else
	{
		*outlen = 0;
		return 1;
	}
}

int Hex2Str(int in_len, unsigned char *in, int *out_len, unsigned char *out)
{
	int i, j;
	unsigned char tmp;
	for (i = 0, j = 0; i < in_len; i++)
	{
		tmp = in[i] >> 4;
		if (tmp >= 10)
		{
			tmp += ('A'-10);
		}
		else
		{
			tmp += '0';
		}
		out[j++] = tmp;
		tmp = in[i] & 0x0F;
		if (tmp >= 10)
		{
			tmp += ('A'-10);
		}
		else
		{
			tmp += '0';
		}
		out[j++] = tmp;
	}
	*out_len = j;
	return 0;
}

long StrToLong(char *buff2)
{
	long tmp = 0;

	tmp = buff2[0];
	tmp <<= 8;
	tmp += buff2[1];
	tmp <<= 8;
	tmp += buff2[2];
	tmp <<= 8;
	tmp += buff2[3];

	return tmp;
}
#if 0
void LongToStr(long LongDat, char *buff2)	// 
{
	buff2[3] = (char)LongDat;
	LongDat >>= 8;

	buff2[2] = (char)LongDat;
	LongDat >>= 8;

	buff2[1] = (char)LongDat;
	LongDat >>= 8;

	buff2[0] = (char)LongDat;
}
#endif

unsigned short StrToUshort(char *buff2)
{
	unsigned short tmp = 0;

	tmp = buff2[0];
	tmp <<= 8;
	tmp += buff2[1];
	return tmp;
}

void UshortToStr(unsigned short Num, char *buff2)
{
	buff2[1] = (char)Num;
	Num >>= 8;
	buff2[0] = (char)Num;
}

int UpperCase(char *sourceStr, char *destStr, int MaxLen)
{
	int i;

	for (i = 0; i < MaxLen; i++)
	{
		if ((sourceStr[i] >= 'a') && (sourceStr[i] <= 'z'))
		{
			destStr[i] = sourceStr[i] - 32;
		}
		else
		{
			destStr[i] = sourceStr[i];
			if (destStr[i] == 0x00)
				break;
		}
	}
	return (i);
}

void DeCompressToAscii(unsigned int uiLenIn, unsigned char * pucDataIn, unsigned char * pucDataOut)
{
	SI_ucParseStr(0,(uchar)uiLenIn,pucDataIn,pucDataOut);
}
// mode=1 Compress   2byte->1bytes  0x31 0x3A ->0x1A
//     =0 decompress 1bytes->2byte  0x1A -> 0x31 0x3A 
// mode=3 Compress   2byte->1bytes  0x31 0x41 ->0x1A
//     =2 decompress 1bytes->2byte  0x1A -> 0x31 0x41
uchar SI_ucParseStr(uchar ucMode,uchar ucLen,uchar *pcInStr,uchar *pcOutStr)
{
	int i;
	uchar ucOutLen=0,data;

	if(ucMode == 1)  
	{
		for(i=0;i<ucLen;i+=2)
		{	
			
			pcOutStr[ucOutLen++] = COMBINE8(pcInStr[i]-'0', pcInStr[i+1]-'0');
 		}
	}
	else if(ucMode == 0)        
	{
		for(i=0;i<ucLen;i++)
		{
 			pcOutStr[ucOutLen++] = HHALFB(pcInStr[i])+'0';
			pcOutStr[ucOutLen++] = LHALFB(pcInStr[i])+'0';
 		}
	}
	else if(ucMode == 3)  
	{
		for(i=0;i<ucLen;i+=2)
		{	
			if(pcInStr[i] >= 'A' && pcInStr[i] <= 'F')
			{
				pcInStr[i] = pcInStr[i] - 'A' + 10;
			}
			else if(pcInStr[i] >= 'a' && pcInStr[i] <= 'f')
			{
				pcInStr[i] = pcInStr[i] - 'a' + 10;
			}
			else
			{
				pcInStr[i] = pcInStr[i] - '0';
			}
			if(pcInStr[i+1] >= 'A' && pcInStr[i+1] <= 'F')
			{
				pcInStr[i] = pcInStr[i] - 'A' + 10;
			}
			else if(pcInStr[i+1] >= 'a' && pcInStr[i+1] <= 'f')
			{
				pcInStr[i+1] = pcInStr[i+1] - 'a' + 10;
			}
			else
			{
				pcInStr[i+1] = pcInStr[i+1] - '0';
			}
			pcOutStr[ucOutLen++]=COMBINE8(pcInStr[i], pcInStr[i+1]);
 		}
	}
	else
	{
		for(i=0;i<ucLen;i++)
		{
		 	// 
		 	data = HHALFB(pcInStr[i]);
		 	pcOutStr[ucOutLen++] = data + (((data)> 9) ? ('A' - 10) : '0');
			data = LHALFB(pcInStr[i]);
			pcOutStr[ucOutLen++] = data + (((data)> 9) ? ('A' - 10) : '0');
 		}
	}
	return ucOutLen;
}

/*
 * int  ConvertData(char cLen, char *cInBuf, char *cOutBuf)
 * {
 * char i,j,k;
 * long sum=0,tmp=0;
 * if(cLen == 1 || cLen == 2 || cLen == 4)
 * {
 * i = cLen * 8;
 * k = i - 1;
 * memcpy((char *)&tmp,cInBuf,cLen);
 * for(j=0;j<i/2;j++)
 * {
 * sum |= (MOVELBIT(tmp,j,k-2*j) | MOVERBIT(tmp,k-j,k-2*j));
 * }
 * memcpy(cOutBuf,(char *)&sum,cLen);
 * return 0;
 * }
 * else
 * {
 * return 1;
 * }
 * }
 */

unsigned char MakeLrc(unsigned char *ucData, unsigned short usLen)
{
	unsigned char lrc = 0xaa;
	int i;
	for (i = 0; i < usLen; i++)
	{
		lrc ^= ucData[i];
	}
	return lrc;
}

unsigned char CheckLrc(unsigned char *ucData, unsigned short usLen, unsigned char ucLrc)
{
	unsigned char lrc;
	lrc = MakeLrc(ucData, usLen);
	if (lrc == ucLrc)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void hexdump(unsigned char * b, unsigned short len)
{
  /*
	int x, c;
	int line;

	TRACE("ADDR|  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F | ASCII %p\r\n", b);
	TRACE("------------------------------------------------------------------------\r\n");
	for (line = 0; line < ((len % 16) ? (len / 16) + 1 : (len / 16)); line++)
	{
		TRACE("%04X| ", line * 16);
		for (x = 0; x < 16; x++)
		{
			if (x + (line * 16) < len)
			{
				c = b[x + line * 16];
				TRACE("%02X ", c);
			}
			else
			{
				TRACE("   ");
			}
		}
		TRACE("| ");
		for (x = 0; x < 16; x++)
		{
			if (x + (line * 16) < len)
			{
				c = b[x + line * 16];
				if ((c > 0x1f) && (c < 0x7f))
				{
					TRACE("%c", c);
				}
				else
				{
					TRACE(".");
				}
			}
			else
			{
				TRACE(" ");
			}
		}
		TRACE("\r\n");
	}
	TRACE("------------------------------------------------------------------------\r\n");
  */
}

void debug_hexdump(unsigned char * b, unsigned short len)
{
#ifdef __DEBUG__
	hexdump(b, len);
#endif
}

void Buf8ByteXor(unsigned int inlen, unsigned char *in, unsigned char *out)
{
	uint i=0;
	uchar j,m,n;
	uchar ucIBuf[8];
	memset(ucIBuf,0,sizeof(ucIBuf));
	m = inlen/8;
	n = inlen%8;
	while(m--)
	{
		for(j=0;j<8;j++)
		{
			ucIBuf[j] ^= in[i+j];
		}
		i += 8;
	}
	if(n)
	{
		for(j=0;j<n;j++)
		{
			ucIBuf[j] ^= in[i+j];
		}							
	}
	memcpy(out,ucIBuf,8);
}



void Str8ByteXor(unsigned int len, unsigned char *str1, unsigned char *str2, unsigned char *out)
{
	uint i;
	for(i=0;i<len;i++)
	{
		out[i] = str1[i]^str2[i];
	}
}

//int mymemcmp(const void *s1, const void *s2, uint n)
//{
//	int ret=0;
//	uint i;
//    char *p,*q;
//    p = (char *)s1;
//    q = (char *)s2;
//	for(i=0;i<n;i++)
//	{
//		if(p[i] != q[i]) 
//		{
//			ret |= (1<<i);
//		}
//		if(p[i] == q[i])
//		{
//			ret |= (0<<i);
//		}
//	}
//	return ret;
//}

unsigned int make_crc(unsigned int len, void *buf)
{
	//²ÉÓÃLRC
	uint *p,i,check=0;
	p = (uint *)buf;
 	for(i=0;i<len/sizeof(uint);i++)
	{
#if 0
	if(i%8==0)
	{
		TRACE("");
	}
	TRACE("[%08X-%08X]",check,p[i]);
#endif
		check ^= p[i];
	}
	return check;
}
// mode=1  str tail add ":"
//     =2  str tail add "!"
void str2link(int mode, void *str1, void *str2, void *out)
{
	int len;
	char *p;
	p = (char *)out;
	strcat(p,(char *)str1);
	len = strlen(p);
	p[len] = 0x20;
	strcat(p,str2);
	if(mode==1)
	{
		strcat(p,":");
	}
	else if(mode==2)
	{
		strcat(p,"!");
	}
}

// mode=1  str tail add ":"
//     =2  str tail add "!"
void str3link(int mode, void *str1, void *str2, void *str3, void *out)
{
	int len;
	char *p;
	p = (char *)out;
	strcat(p,(char *)str1);
	len = strlen(p);
	p[len] = 0x20;
	strcat(p,str2);
	len +=(1+strlen(str2)); 
	p[len] = 0x20;
	strcat(p,str3);
	if(mode==1)
	{
		strcat(p,":");
	}
	else if(mode==2)
	{
		strcat(p,"!");
	}
}


/* 
 * s_CheckStr - [GENERIC] check str is ascii code
 * @   0-OK  1-Err 
 */
int s_CheckStr (int inlen, unsigned char *in, int *outlen, unsigned char *out)
{
    int i;
    for ( i=0 ; i<inlen ; i++ ) {
        if ( in[i]<'0' || in[i] > '9' ) {
            *outlen = i; 
            return 1;
        } else {
            out[i] = in[i];
        }
    }
    *outlen = i; 
    return 0;
}		/* -----  end of function s_CheckStr  ----- */

uint32_t msb_byte4_to_uint32(const uint8_t byte[4])
{
    uint32_t  tmp = 0;

	tmp = byte[0];
	tmp <<= 8;
	tmp += byte[1];
	tmp <<= 8;
	tmp += byte[2];
	tmp <<= 8;
	tmp += byte[3];

	return tmp;
}

uint32_t lsb_byte4_to_uint32(const uint8_t byte[4])
{
    uint32_t  tmp = 0;

	tmp = byte[3];
	tmp <<= 8;
	tmp += byte[2];
	tmp <<= 8;
	tmp += byte[1];
	tmp <<= 8;
	tmp += byte[0];

	return tmp;
}

uint16_t msb_byte2_to_uint16(const uint8_t byte[2])
{
	return ((byte[0]<<8)+byte[1]);
}

void msb_uint32_to_byte4(uint32_t byte4, uint8_t byte[4])
{
    byte[3] = (uint8_t)byte4;
    byte4 >>= 8;
    byte[2] = (uint8_t)byte4;
    byte4 >>= 8;
    byte[1] = (uint8_t)byte4;
    byte4 >>= 8;
    byte[0] = (uint8_t)byte4;
}

void lsb_uint32_to_byte4(uint32_t byte4, uint8_t byte[4])
{
    byte[0] = (uint8_t)byte4;
    byte4 >>= 8;
    byte[1] = (uint8_t)byte4;
    byte4 >>= 8;
    byte[2] = (uint8_t)byte4;
    byte4 >>= 8;
    byte[3] = (uint8_t)byte4;
}

void msb_uint16_to_byte2(uint16_t byte2, uint8_t byte[2]) 
{
    byte[1] = (uint8_t)byte2;
    byte2 >>= 8;
    byte[0] = (uint8_t)byte2;
}

uint8_t ascii2_to_dec1(uint8_t ascii[2]) 
{
    uint8_t dec1=0,i;

    for (i=0; i<2; i++)
    {
        dec1 = dec1*10;
        if ((ascii[i]>='0') && (ascii[i]<='9'))
        {
            dec1 += ((ascii[i] - '0') & 0x0F);
        } else if ((ascii[i]>='A') && (ascii[i]<='F'))
        {
            dec1 += ((ascii[i] - 'A' + 0x0A) & 0x0F);
        } else if ((ascii[i]>='a') && (ascii[i]<='f'))
        {
            dec1 += ((ascii[i] - 'a' + 0x0A) & 0x0F);
        }
    }
    return dec1;
}





