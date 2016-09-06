/***************** (C) COPYRIGHT 2015 START Computer equipment *****************
 * File Name          : security_alg.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/29/2015 4:29:46 PM
 * Description        : 
 *******************************************************************************/
#ifndef __SECURITY_ALG_H__
#define __SECURITY_ALG_H__


void cacul_mac_ecb(uint16_t keylen, uint8_t *key, uint32_t inlen, uint8_t *input, uint8_t outlen ,uint8_t *out);
uint8_t CalculateMac(uint32_t uiMode, uint32_t uiKeyLen, uint8_t * pucKey, uint32_t uiLenIn,
	uint8_t * pucDataIn, uint32_t * puiLenOut, uint8_t * pucDataOut);
uchar s_CaltMAC(uchar mode, uchar padding, uchar *start, uchar *key, uint inlen, uchar *inbuf, uchar *mac);
int s_CaltMac_ECB(uchar mode, uchar padding, uchar *start, uchar *key, uint inlen, uchar *inbuf, uchar *mac);
int s_CaltMac_X99 (uchar mode, uchar padding, uchar *start, uchar *key, uint inlen, uchar *inbuf, uchar *mac);

#endif

