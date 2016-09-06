/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : des.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/21/2014 11:42:43 AM
 * Description        : 
 *******************************************************************************/
#ifndef __DES_H__
#define __DES_H__

#define DES_BLOCK_LENGTH              8
#define DES_KEY_LENGTH                8
#define DES_ENCRYPT     1
#define DES_DECRYPT     0


void cau_des3_encrypt(const uint8_t *in, const uint8_t key[24],uint8_t *out);
void cau_des3_decrypt(const uint8_t *in, const uint8_t *key,uint8_t *out);
void cau_des2_encrypt(const uint8_t *in, const uint8_t key[16],uint8_t *out);
void cau_des2_decrypt(const uint8_t *in, const uint8_t key[16],uint8_t *out);

int des1_encrypt_ecb(const uint8_t *input, uint32_t input_length,
                    const uint8_t key[8], uint8_t *output);
int des1_decrypt_ecb(const uint8_t *input, uint32_t input_length,
                    const uint8_t key[8], uint8_t *output);
int des2_encrypt_ecb(const uint8_t *input, uint32_t input_length,
                    const uint8_t key[16], uint8_t *output);
int des2_decrypt_ecb(const uint8_t *input, uint32_t input_length,
                    const uint8_t key[16], uint8_t *output);
int des3_encrypt_ecb(const uint8_t *input, uint32_t input_length,
                     const uint8_t key[24], uint8_t *output);
int des3_decrypt_ecb(const uint8_t *input, uint32_t input_length,
                     const uint8_t key[24], uint8_t *output);


int des1_encrypt_cbc(const uint8_t key[8], uint8_t iv[8],
                    const uint8_t *input,uint32_t length, 
                    uint8_t *output );
int des1_decrypt_cbc(const uint8_t key[8], uint8_t iv[8],
                    const uint8_t *input, uint32_t length,
                    uint8_t *output );
int des2_encrypt_cbc(const uint8_t key[16], uint8_t iv[8],
                    const uint8_t *input,uint32_t length, 
                    uint8_t *output );
int des2_decrypt_cbc(const uint8_t key[16], uint8_t iv[8],
                    const uint8_t *input, uint32_t length,
                    uint8_t *output );

int des3_encrypt_cbc(const uint8_t key[24], uint8_t iv[8],
                     const uint8_t *input, uint32_t input_length,
                     uint8_t *output );
int des3_decrypt_cbc(const uint8_t key[24], uint8_t iv[8],
                     const uint8_t *input,  uint32_t input_length,
                     uint8_t *output );

//-------------------------------------------------------------------

/** Encrypt a block of data (CBC)
 *  Uses Triple DES with two keys
 *
 * @param  inData    input data
 * @param  outData   output data
 * @param  length    length of data
 * @param  key       array of two DES keys
 * @param  numKeys   number of keys to use for decryption, 1, 2, or 3
 * @param  iv        IV (0 if NULL)
 *
 */
void des_encrypt_cbc(uint8_t * inData, uint8_t * outData, int32_t length, uint8_t * key,
	uint8_t numKeys, uint8_t * iv);

/** Decrypt a block of data (CBC) Triple DES.
 *  Uses Triple DES with two keys 
 *
 * @param  inData    input data
 * @param  outData   output data
 * @param  length    length of data
 * @param  key       array of two DES key(s)
 * @param  numKeys   number of keys to use for decryption, 1, 2, or 3
 * @param  iv        IV (0 if NULL)
 * Note:   For CBC Decrypt, inData and outData must not be the same array
 */
void des_decrypt_cbc(uint8_t * inData, uint8_t * outData, int32_t length, uint8_t * key,
	uint8_t numKeys, uint8_t * iv);

/** Single-DES encrypt blocks of data (ECB)..
 * Uses one key
 *
 * @param  inData    input data
 * @param  outData   output data
 * @param  length    length of data
 * @param  key       DES key(s)
 * @param  numKeys   number of keys to use for decryption, 1, 2, or 3
 */
void des_encrypt_ecb(uint8_t * inData, uint8_t * outData, int32_t length, uint8_t * key,
	uint8_t numKeys);

/** DES decrypt blocks of data (ECB).
 *
 * @param  inData    input data
 * @param  outData   output data
 * @param  length    length of data
 * @param  key       DES key(s)
 * @param  numKeys   number of keys to use for decryption, 1, 2, or 3
 */
void des_decrypt_ecb(const uint8_t * inData, uint8_t * outData, int32_t length, uint8_t * key,
	uint8_t numKeys);





#endif

