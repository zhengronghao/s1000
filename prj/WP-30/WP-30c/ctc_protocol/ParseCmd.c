#include "common.h"
#include "ParseCmd.h"
#include <string.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////     
// CRC16码表     
static unsigned short const wCRC16Table[256] = {      
0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,     
0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,     
0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,      
0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,     
0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,       
0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,     
0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,     
0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,     
0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,     
0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,        
0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,     
0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,     
0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,     
0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,     
0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,        
0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,     
0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,     
0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,     
0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,     
0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,        
0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,     
0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,     
0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,     
0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,     
0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,       
0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,     
0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,     
0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,     
0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,     
0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,       
0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,     
0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040};  

//x:之前的crc16值,初始值为0;y待计算的下一个字节;返回值为计算好的crc16
#define CALC_CRC16(x,y) (((x>>8)&0xff)^wCRC16Table[((x&0xff)^(y & 0xff))])

/* 组一个包头*/
MCUPCK CmdPackReq(uint32_t len,uint8_t  cmdcls,uint8_t  cmdop,uint8_t  sno1,uint8_t  sno2)
{
    MCUPCK req;
    req.len = len + sizeof(MCUPCK) - sizeof(uint32_t);
    req.cmdcls = cmdcls;
    req.cmdop = cmdop;
    req.sno1 = sno1;
    req.sno2 = sno2;
    return req;
}

/* 计算字符串转义后的长度*/
int CmdTranslateLen(const uint8_t *inBuf,int inLen)
{
    int i,count=0;

    for (i=0;i<inLen;i++)
    {
    	switch (inBuf[i])
    	{
    	case CTC_STX:
    	case CTC_ETX:
    	case ESC:
    		count+=2;
    		break;
    	default:
    		count++;
    		break;
    	}
    	
    }
    return count;
}

/* 计算字符串转义前的长度*/
int CmdRestoreLen(const uint8_t *inBuf,int inLen)
{
	int i,count=0;
	
	for (i=0;i<inLen;i++)
	{
		if(inBuf[i]==ESC)
		{
			if((i+1)>=inLen)return RET_PARAM_LEN;//长度不足以解析
			switch (inBuf[i+1])
			{
			case CTC_STX_T:
			case CTC_ETX_T:
			case ESC_T:
				break;
			default:
				return RET_TRANSLATE;//转义字符后面没有标准转义字符
				break;
			}
			i++;//转义字符额外递增1
		}
		count++;
		
	}
	return count;
}

/* 转义字符串并在末尾加上lrc值*/
int CmdTranslate(const uint8_t *inBuf,uint32_t inLen,uint8_t *outBuf,uint32_t *outLen)
{
	uint32_t i,count=0;
#ifdef PCK_CHK_CRC16
	uint16_t crc16=0;
	uint8_t tmp;
#else
	uint8 lrc=0;
#endif
	if(inBuf==NULL || outBuf==NULL || outLen==NULL)return RET_PARAM;
	if(inLen<=0)return RET_PARAM_LEN;

	for (i=0;i<inLen;i++)
	{
	#ifdef PCK_CHK_CRC16
		crc16=CALC_CRC16(crc16,inBuf[i]);
	#else
		lrc^=inBuf[i];
	#endif
		switch (inBuf[i])
		{
		case CTC_STX:
			outBuf[count]=ESC;
			outBuf[count+1]=CTC_STX_T;
			count+=2;
			break;
		case CTC_ETX:
			outBuf[count]=ESC;
			outBuf[count+1]=CTC_ETX_T;
			count+=2;
			break;
		case ESC:
			outBuf[count]=ESC;
			outBuf[count+1]=ESC_T;
			count+=2;
			break;
		default:
			outBuf[count]=inBuf[i];
			count++;
			break;
		}
		
	}
	//校验码转义
#ifdef PCK_CHK_CRC16
	//crc16转义

	tmp=crc16&0xFF;//low bit
	switch (tmp)
	{
	case CTC_STX:
		outBuf[count]=ESC;
		outBuf[count+1]=CTC_STX_T;
		count+=2;
		break;
	case CTC_ETX:
		outBuf[count]=ESC;
		outBuf[count+1]=CTC_ETX_T;
		count+=2;
		break;
	case ESC:
		outBuf[count]=ESC;
		outBuf[count+1]=ESC_T;
		count+=2;
		break;
	default:
		outBuf[count]=tmp;
		count++;
		break;
	}
	
	tmp=(crc16&0xFF00)>>8;//high bit
	switch (tmp)
	{
	case CTC_STX:
		outBuf[count]=ESC;
		outBuf[count+1]=CTC_STX_T;
		count+=2;
		break;
	case CTC_ETX:
		outBuf[count]=ESC;
		outBuf[count+1]=CTC_ETX_T;
		count+=2;
		break;
	case ESC:
		outBuf[count]=ESC;
		outBuf[count+1]=ESC_T;
		count+=2;
		break;
	default:
		outBuf[count]=tmp;
		count++;
		break;
	}
#else
	//lrc转义
	switch (lrc)
	{
	case CTC_STX:
		outBuf[count]=ESC;
		outBuf[count+1]=CTC_STX_T;
		count+=2;
		break;
	case CTC_ETX:
		outBuf[count]=ESC;
		outBuf[count+1]=CTC_ETX_T;
		count+=2;
		break;
	case ESC:
		outBuf[count]=ESC;
		outBuf[count+1]=ESC_T;
		count+=2;
		break;
	default:
		outBuf[count]=lrc;
		count++;
		break;
	}
#endif
	*outLen = count;
	return RET_OK;
}

/* 转义过的字符串还原,有带LRC校*/
int CmdRestore(const uint8_t *inBuf,uint32_t inLen,uint8_t *outBuf,uint32_t *outLen)
{
    uint32_t i,count=0;
    
    if(inBuf==NULL || outBuf==NULL || outLen==NULL)return RET_PARAM;
    if(inLen<=2)return RET_PARAM_LEN;
#ifdef PCK_CHK_CRC16
    uint16_t crc16=0;
#else
    uint8_t lrc=0;
#endif
    outBuf[0]=inBuf[0];
    count++;
    for (i=1;i<inLen-1;i++) {
        if(inBuf[i]==ESC) {
        	if((i+1)>=inLen)return RET_PARAM_LEN;//长度不足以解析
        	switch (inBuf[i+1])
        	{
        	case CTC_STX_T:
        		outBuf[count]=CTC_STX;
        		break;
        	case CTC_ETX_T:
        		outBuf[count]=CTC_ETX;
        		break;
        	case ESC_T:
        		outBuf[count]=ESC;
        		break;
        	default:
        		return RET_TRANSLATE;//转义字符后面没有标准转义字符
        		break;
        	}
        	i++;//转义字符额外递增1
        } else {
        	outBuf[count]=inBuf[i];
        }
	#ifdef PCK_CHK_CRC16
		crc16=CALC_CRC16(crc16,outBuf[count]);
	#else
		lrc^=outBuf[count];	
	#endif
		count++;
    }
    
    outBuf[count]=inBuf[i];
    count++;
    *outLen = count;
#ifdef PCK_CHK_CRC16
    if(crc16 != 0)return RET_LRC;//CRC16错误，中间数据尾部加上crc16运算结果应该是0x0000
#else
    if(lrc != 0)return RET_LRC;//lrc错误，0x55^0xAA=0xFF,中间数据异或加上数据表的lrc正好是00
#endif
    return RET_OK;
}

/* 组包*/
int packCmd(MCUPCK req,const uint8_t *inBuf, uint8_t *outBuf, uint32_t *outLen)
{
    if(inBuf==NULL || outBuf==NULL || outLen==NULL)return RET_PARAM;

    uint8_t tmp[MAX_CMD_TEMP_SIZE];
    uint32_t tmpLen=req.len+sizeof(uint32_t);//sizeof(MCUPCK)+req.len;

    memcpy(tmp,&req,sizeof(MCUPCK));
    memcpy(tmp+sizeof(MCUPCK),inBuf,req.len+sizeof(uint32_t)-sizeof(MCUPCK));

    int ret = CmdTranslate(tmp,tmpLen,outBuf+1,&tmpLen);
    if(ret != RET_OK)return ret;

    outBuf[0]=CTC_STX;
    outBuf[tmpLen+1]=CTC_ETX;

    *outLen = tmpLen + 2;
    return RET_OK;
}

/* 解包*/
int unpackCmd(const uint8_t *inBuf,uint32_t inLen, MCUPCK *res, uint8_t *outBuf, uint32_t *outLen)
{
    if(inBuf==NULL || outBuf==NULL || outLen==NULL || res==NULL)return RET_PARAM;
    if(inLen<=0)return RET_PARAM_LEN;

    uint8_t tmp[MAX_CMD_TEMP_SIZE];
    uint32_t tmpLen;
    int ret;
    ret = CmdRestore(inBuf,inLen,tmp,&tmpLen);
    if(ret != RET_OK)return ret;

    if(tmpLen < PACK_MINSIZE)return RET_UNPACK_LEN;//解包长度出错

    if(tmp[0]!=CTC_STX)return RET_NOCTC_STX;
    if(tmp[tmpLen-1]!=CTC_ETX)return RET_NOCTC_ETX;

    memcpy(res,tmp+1,sizeof(MCUPCK));

    if((tmpLen-2-sizeof(uint32_t)) - PCK_CHK_LEN != res->len)return RET_CHECK_LEN;//长度校验出错

    *outLen = res->len + sizeof(uint32_t) - sizeof(MCUPCK);
    if(*outLen>0)memcpy(outBuf,tmp+1+sizeof(MCUPCK),*outLen);


    return RET_OK;
}

int CmdFindCTC_STX(uint8_t *data, uint32_t size, int *s_pos)
{
    uint32_t i;

    if(size < PACK_MINSIZE) return RET_PARAM_LEN;//数据过短
    for (i=0;i<size;i++)//找包头
    {
    	if(data[i]==CTC_STX)
    	{
    		*s_pos=i;
    		break;
    	}
    }
    if(*s_pos==0 && data[0]!=CTC_STX && size==0)return RET_NOCTC_STX;
    return RET_OK;
}

int CmdFindCTC_ETX(uint8_t *data, uint32_t size, int *e_pos)
{
    uint32_t i;

    if(size < PACK_MINSIZE)return RET_PARAM_LEN;//数据过短
    for (i=0;i<size;i++)//找包尾
    {
        if(data[i]==CTC_ETX)
        {
        	*e_pos=i;
        	break;
        }
    }
    if(*e_pos==0 && data[0]!=CTC_ETX && size==0)return RET_NOCTC_ETX;
    return RET_OK;
} 

#if 0
/* 指令完整性校验*/
int frame_integrity_check(uint8_t *data, uint32_t size, int *s_pos, int *e_pos)
{
	uint32_t start = 0,end = 0;
	uint32_t i;
	
	if(size < PACK_MINSIZE) return RET_PARAM_LEN;//数据过短
	for (i=0;i<size;i++)//找包头
	{
		if(data[i]==CTC_STX)
		{
			start=i;
			break;
		}
	}
	if(start==0 && data[0]!=CTC_STX && size==0)return RET_NOCTC_STX;

	

	*s_pos=start;//赋值包头位置

	uint32_t headlencount = 0,headlentarns = 0;
	for (i=start+1;headlentarns<sizeof(uint32_t) && headlencount<8 && i<size;i++)//读长度4字节，转义长度会增长
	{
		headlencount++;
		headlentarns++;
		if (data[i] == ESC)
		{
			i++;
			headlencount++;
		}	
	}

	uint8_t headlen[sizeof(uint32_t)];
	uint32_t trueheadlencount;
	int ret;
	ret = CmdRestore(&data[start+1],headlencount,headlen,&trueheadlencount);//转义长度还原
	if(ret != RET_OK && ret != RET_LRC)
	{
		return RET_UNPACK_LEN;//ret;//此段数据末尾不带LRC
	}
    
	uint32_t len;
	memcpy(&len,headlen,sizeof(uint32_t));//包长度转换成uint32_t

	headlentarns = 0;
	for ( i = start+headlencount+1;headlentarns<(len+PCK_CHK_LEN) && i<size;i++)//按照长度遍历一遍查找最后一个字母
	{
		headlentarns++;
		if (data[i] == ESC)
		{
			i++;
		}	
	}
	end=i;

	if(end>=size || data[end]!=CTC_ETX)
	{
		if(CmdFindCTC_ETX(data,size,e_pos)!= RET_OK)
			return RET_NOCTC_ETX;//检查是不是CTC_ETX包尾
		else return RET_PACKET_INTEGRITY;
	}
	
	*e_pos=end;//赋值包尾位置

	return RET_OK;
}
#endif

#if 1
/* 指令完整性校验*/
extern uint32 g_Heartbeat_Timer;
extern uint32_t sys_get_counter(void);
int frame_integrity_check(uint8_t *data, uint32_t size, int *s_pos, int *e_pos)
{
	uint32_t start = 0,end = 0;
	uint32_t i;
	
	if (size < PACK_MINSIZE) return RET_PARAM_LEN;//数据过短
    for (i = 0; i < size; i++) {
        if (data[i] == CTC_STX) {
            start = i;
            break;
        }
    }

	if (start==0 && data[start]!=CTC_STX) return RET_NOCTC_STX;
     *s_pos=start;
     
	for (i = start+1;i < size; i++) {
		if (data[i] == CTC_ETX){
			end=i;
			break;
		}
	}
    
	if(end==0 && data[end]!= CTC_ETX) return RET_NOCTC_ETX;
	*e_pos=end;
#ifdef CFG_LOWPWR
// 说明接收数据的完整性已经通过，记录心跳包的时间
    g_Heartbeat_Timer = sys_get_counter();
#endif
	return RET_OK;
}
#endif
/* 会自动整理缓冲区的解包函 */
int unpackCmdEx(uint8_t *data, uint32_t size, MCUPCK *res,uint8_t *outBuf,uint32_t *outLen, uint32_t *restLen)
{
    int s_pos=0,e_pos=0;
    int ret;
    ret = frame_integrity_check(data,size,&s_pos,&e_pos);

    if(ret != RET_OK)
    {
    	if(ret == RET_NOCTC_STX)
    	{
    		*restLen=0;
    		//TODO: 无包头，清空缓冲区
    	}
    	else if(ret == RET_PACKET_INTEGRITY && s_pos != 0)
    	{
                    *restLen = size-s_pos;
    		memcpy(data,data+s_pos+1,*restLen);
                    dumpByteArray(data,*restLen);
    		//TODO: 无包尾，且包头偏移
    	}
    	else
    	{
    		*restLen = size-1;
    		memcpy(data,data+1,*restLen);
                   dumpByteArray(data,*restLen);
    	}
    	return ret;
    }

    ret = unpackCmd(data+s_pos,e_pos-s_pos+1,res,outBuf,outLen);
    *restLen = size-e_pos-1;
    if((*restLen)>0)
    {
    	memcpy(data,data+e_pos+1,*restLen);
    }

    if(ret == RET_OK)
    {
    	dumpByteArray(data,*outLen);
    	return ret;
    }

    return RET_OK;
}

void dumpByteArray(uint8_t *data, uint32_t len)
{
    uint32_t i;

    TRACE("dump len:%d\n",len);
    for (i=0;i<len;i++)
    {
    	TRACE("%02X ",data[i]);
    	if(i%16==15)TRACE("\n");
    }
    TRACE("\n");
}

