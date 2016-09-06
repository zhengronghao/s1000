/***************** (C) COPYRIGHT 2015 START Computer equipment *****************
 * File Name          : crc_hal_drv.c
 * bfief              : 
 * Author             : luocs 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 12/28/2015 3:53:32 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "fsl_crc_driver.h"
#include "crc_hal_drv.h"

#define CRC_HARDWARE_ENGINE   (0)


#define CRC_INSTANCE         (0)

uint32_t drv_crc_hardware(const crc_user_config_t *usrConptr,uint8_t *data,uint32_t bytelen)
{
    uint32_t crcValue=0;

    // Init CRC.
    CRC_DRV_Init(CRC_INSTANCE,usrConptr);
    // Calculates CRC value.
    crcValue = CRC_DRV_GetCrcBlock(CRC_INSTANCE,data,bytelen);
    //Sutdown CRC
    CRC_DRV_Deinit(CRC_INSTANCE);
    return crcValue;
}



//crc32_be
uint32_t calcCrc32(uint32_t* data, uint32_t len, uint32_t startValue)
{
    crc_user_config_t userConfigPtr;

    // Init CRC-16-CCITT
    // polynomial:x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0
    userConfigPtr.crcWidth       = kCrc32Bits;
    userConfigPtr.seed           = startValue;
    userConfigPtr.polynomial     = CRCPOLY_BE ;
    userConfigPtr.writeTranspose = kCrcNoTranspose;
    userConfigPtr.readTranspose  = kCrcNoTranspose;
    userConfigPtr.complementRead = FALSE;
   return drv_crc_hardware(&userConfigPtr,(uint8_t*)data,(len<<2));
}

/*-----------------------------------------------------------------------------}
 * 软件现实计算CRC16，polynom = 0x1021
 * 输入参数:pre_value:初始值
 *      用在下载协议，计算MBID和RTC防拆校验
 *
 *-----------------------------------------------------------------------------{*/
ushort soft_calc_crc16 (const void *in,uint len, ushort start, ushort polynom)
{
    ushort crc;
    uint i;
    uchar *ptr = (uchar *)in;
	crc = start;
	while(len--)
	{
		crc ^= *ptr++;
		for(i=0; i<8;i++)
		{
			if(crc&1)
			{
				crc >>= 1;
				crc ^= polynom;
			}
			else
				crc >>= 1;
		}
	}
    return crc;
}

const uint16_t crc_ta[256] = { /* CRC余式表 */
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

//CRC16 seed:0000 poly:1021 writeTransopse:0 ReadTranspose:0 complement:0
uint16_t calc_crc16(const void *src_buf, uint32_t bytelen, uint16_t pre_value)
{
#if (CRC_HARDWARE_ENGINE)
    crc_user_config_t userConfigPtr;

    // Init CRC-16-CCITT
    // polynomial 1000000100001: x^12+x^5+1
    userConfigPtr.crcWidth       = kCrc16Bits;
    userConfigPtr.seed           = pre_value;
    userConfigPtr.polynomial     = 0x1021;
    userConfigPtr.writeTranspose = kCrcNoTranspose;
    userConfigPtr.readTranspose  = kCrcNoTranspose;
    userConfigPtr.complementRead = FALSE;
   return drv_crc_hardware(&userConfigPtr,(uint8_t*)src_buf,bytelen);
#else
	uint8_t da;
	uint8_t *pT = (uint8_t *)src_buf;
	
    if(src_buf == NULL)
    {
        return(pre_value);
    }
	
	while(bytelen-- != 0)
    {
        da = (uint8_t)( pre_value>>8); /* 以8位二进制数的形式暂存CRC的高8位 */
        pre_value <<= 8;              /* 左移8位，相当于CRC的低8位乘以 */
        pre_value ^= crc_ta[da^*pT];  /* 高8位和当前字节相加后再查表求CRC ，再加上以前的CRC */
        pT++;
    }
	return(pre_value);
#endif
}


/*-----------------------------------------------------------------------------}
 * 计算CRC16，polynom = 0xA001
 * 输入参数:pre_value:初始值
 *      PCI密码键盘通讯协议
 *-----------------------------------------------------------------------------{*/
uint16_t crc_calculate16by8(unsigned char *ptr, uint32_t count)
{
#if (CRC_HARDWARE_ENGINE)
    crc_user_config_t userConfigPtr;

    //CRC16 seed:0000 poly:8005 writeTransopse:1 ReadTranspose:2 complement:0
    // Init CRC-16-CCITT
    // polynomial 1000000100001: x^12+x^5+1
    userConfigPtr.crcWidth       = kCrc16Bits;
    userConfigPtr.seed           = 0;
    userConfigPtr.polynomial     = 0x8005;
    userConfigPtr.writeTranspose = kCrcTransposeBits;
    userConfigPtr.readTranspose  = kCrcTransposeBoth;
    userConfigPtr.complementRead = FALSE;
   return drv_crc_hardware(&userConfigPtr,ptr,count);
#else
#define CRC_POLYNOM	    	0xA001//0x1021		//多项式
#define CRC_PRESET          0x0//0xFFFF		//CRC校验预置值
    return soft_calc_crc16(ptr,count,CRC_PRESET,CRC_POLYNOM);
#endif
}


/********************************************************************/
/**
* crc32_le() - Calculate bitwise little-endian Ethernet AUTODIN II CRC32
* @crc: seed value for computation.  ~0 for Ethernet, sometimes 0 for
*        other uses, or the previous crc32 value if computing incrementally.
* @p: pointer to buffer over which CRC is run
* @len: length of buffer @p
* In fact, the table-based code will work in this case, but it can be
* simplified by inlining the table in ?: form.
*/
uint32_t crc32_le(uint32_t  crc, unsigned char const *p, size_t len)
{
    int i;
    while (len--) {
        crc ^= *p++;
        for (i = 0; i < 8; i++)
            crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
    }
    return crc;
}

/**
* crc32_be() - Calculate bitwise big-endian Ethernet AUTODIN II CRC32
* @crc: seed value for computation.  ~0 for Ethernet, sometimes 0 for
*        other uses, or the previous crc32 value if computing incrementally.
* @p: pointer to buffer over which CRC is run
* @len: length of buffer @p
* In fact, the table-based code will work in this case, but it can be
* simplified by inlining the table in ?: form.
*/

uint32_t crc32_be(uint32_t  crc, unsigned char const *p, size_t len)
{
    int i;
    while (len--) {
        crc ^= *p++ << 24;
        for (i = 0; i < 8; i++)
            crc =
                (crc << 1) ^ ((crc & 0x80000000) ? CRCPOLY_BE :
                              0);
    }
    return crc;
}





