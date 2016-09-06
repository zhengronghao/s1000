/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : sha.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/24/2014 4:28:49 PM
 * Description        : 
 *******************************************************************************/

#ifndef __SHA__H_
#define __SHA__H_


#define SHA1_BLOCK_LENGTH                   64
#define SHA1_RESULT_LENGTH                  20

#define SHA256_BLOCK_LENGTH                 64
#define SHA256_RESULT_LENGTH                32

int sha1(const uint8_t *input,uint32_t length,uint8_t output[20]);
int sha256(const uint8_t *input,uint32_t length,uint8_t output[32]);

#endif


