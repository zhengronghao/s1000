/***************** (h) COPYRIGHT 2015 START Computer equipment *****************
 * File Name          : crc_hal_drv.h
 * bfief              : 
 * Author             : luocs 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 12/29/2015 4:14:25 PM
 * Description        : 
 *******************************************************************************/
#ifndef __CRC_HAL_DRV_H__
#define __CRC_HAL_DRV_H__ 

#include "fsl_crc_driver.h"
uint32_t drv_crc_hardware(const crc_user_config_t *usrConptr,uint8_t *data,uint32_t bytelen);
uint32_t calcCrc32(uint32_t* data, uint32_t len, uint32_t startValue);

/* 
* There are multiple 16-bit CRC polynomials in common use, but this is
* *the* standard CRC-32 polynomial, first popularized by Ethernet.
* x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0
*/
#define CRCPOLY_LE 0xEDB88320
#define CRCPOLY_BE 0x04C11DB7
uint32_t crc32_le(uint32_t  crc, unsigned char const *p, size_t len);
uint32_t crc32_be(uint32_t  crc, unsigned char const *p, size_t len);

uint16_t calc_crc16(const void *src_buf, uint32_t bytelen, uint16_t pre_value);
ushort soft_calc_crc16 (const void *in,uint len, ushort start, ushort polynom);
uint16_t crc_calculate16by8(unsigned char *ptr, uint32_t count);
#endif


