/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : sha.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/24/2014 4:26:52 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "drv_inc.h"
#include "sha.h"

//SHA-1, SHA-224 and SHA-256
static void sha_padding(const uint8_t *input,uint32_t length,uint8_t output[128],uint32_t *out_length)
{
    uint32_t bit_length = (length <<3 );
    uint32_t remainder = length & (SHA1_BLOCK_LENGTH - 1);
    uint32_t pad_length;
    uint32_t tmp_length;

//    TRACE("\n-|sha padding data:");
//    vDispBuf(length,3,input);
//    TRACE("\n-|bit_length:%d remainder:%d",bit_length,remainder);
    memset(output,0x00,128);
    pad_length = ( remainder < 56 ) ? ( 56 - remainder ) : ( 120 - remainder );
//    TRACE("-|pad_length:%d",pad_length);
    tmp_length = length - remainder;
//    TRACE("-|tmp_len:%d",tmp_length);
    memcpy(output,input+tmp_length,remainder);
    /*add padding*/
    tmp_length = remainder;
    output[tmp_length] = 0x80;/*first bit enabled*/
    /*add length(get number of bits):big endian*/
    tmp_length += (pad_length+4);
    output[tmp_length++] = bit_length>>24 & 0xFF;
    output[tmp_length++] = bit_length>>16 & 0xFF;
    output[tmp_length++] = bit_length>>8  & 0xFF;
    output[tmp_length++] = bit_length     & 0xFF;
    *out_length = tmp_length;
//    TRACE("\n-|Output padding data:");
//    vDispBuf(tmp_length,3,output);
}

int sha1(const uint8_t *input,uint32_t length,uint8_t output[20])
{
    uint8_t padding_buffer[64*2];
    uint32_t padding_length=0;

    if (output == NULL)
        return -1;
    sha_padding(input,length,padding_buffer,&padding_length);
    cau_sha1_initialize_output((uint32_t *)output);//init state variables  
    if (length >= SHA1_BLOCK_LENGTH)
        cau_sha1_hash_n(input,length/SHA1_BLOCK_LENGTH,(uint32_t *)output);
    cau_sha1_hash_n(padding_buffer,padding_length/SHA1_BLOCK_LENGTH,(uint32_t *)output);
    return 0;
}

int sha256(const uint8_t *input,uint32_t length,uint8_t output[32])
{
    uint8_t padding_buffer[64*2];
    uint32_t padding_length=0;

    if (output == NULL)
        return -1;
    sha_padding(input,length,padding_buffer,&padding_length);
    cau_sha256_initialize_output((uint32_t *)output);//init state variables  
    if (length >= SHA256_BLOCK_LENGTH)
        cau_sha256_hash_n(input,length/SHA256_BLOCK_LENGTH,(uint32_t *)output);
    cau_sha256_hash_n(padding_buffer,padding_length/SHA256_BLOCK_LENGTH,(uint32_t *)output);
    return 0;
}


