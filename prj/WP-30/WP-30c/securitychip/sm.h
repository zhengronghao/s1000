/*
 * sm.h
 *
 *  Created on: 2013-4-26
 *      Author: zhengj
 *      Modified: huanglf 2015-1-5
 */

#ifndef SM_H_
#define SM_H_

#include "wp30_ctrl.h"
//#include <utility/debug.h>
//#include <openssl/ec.h>
//#include <openssl/bn.h>
//#include <openssl/ec_lcl.h>
//#include <openssl/err.h>
//#include <openssl/obj_mac.h>
//#include "test_data_param.h"

#if 2//def CFG_OPENSSL
// #define OPENSSL_TEST        // 测试函数开关

/*
 * @Block:      Defines
 * @{ */
// #define SM_DEBUG

#if (defined(SM_DEBUG) && defined(DEBUG_SERIAL))
#define dbgsm_printf(...)                 _DbgSPrintf(__VA_ARGS__) //函数原形：dbg_printf(Level,pcFormat,...)

#define DEFINE_SHOW_BIGNUM(x) \
    dbgsm_printf(#x":\r\n"); \
show_bignum(x, ecp->point_byte_length);\
dbgsm_printf("\r\n")

#define DEFINE_SHOW_STRING(x, length1) \
    dbgsm_printf(#x":\r\n"); \
sm_msg_show(x, length1);\
dbgsm_printf("\r\n")

#else
#define dbgsm_printf(...)

#define DEFINE_SHOW_BIGNUM(...)
#define DEFINE_SHOW_STRING(...)

#endif /* (defined(SM_DEBUG) && defined(DEBUG_SERIAL)) */

typedef struct {
    int field_type,         /* either NID_X9_62_prime_field or
                             * NID_X9_62_characteristic_two_field
                             */
        seed_len,
        param_len;
    unsigned int cofactor;  /* promoted to BN_ULONG */
} EC_CURVE_DATA;

#define HASH_BYTE_LENGTH 32
#define HASH_BIT_LENGTH 256
#define ABORT printf("error: line: %d function: %s\n", __LINE__, __FUNCTION__);
#define TYPE_GFp 0      // 包含p个元素的素域
#define TYPE_GF2m 1     // 包含2m 个元素的二元扩域。
//#define SUCCESS 1
//#define FAIL 0

#define SM_SUCC         0               // 成功
#define SM_FAIL         -1              // 失败
#define SM_PNULL        -101            // 为空指针
#define SM_PARMERR      -102            // 参数错误

#define OPENSSL_malloc(num) CRYPTO_malloc((int)num,__FILE__,__LINE__)
#define ORDER_A_B 0
#define ORDER_B_A 1

#define HASH_BYTE_LENGTH 32
#define HASH_BIT_LENGTH 256

#define MAX_POINT_BYTE_LENGTH 64  //点中x, y的最大字节长度

#define BUFFER_APPEND_BIGNUM(buffer1, pos1, point_byte_length, x) \
    BN_bn2bin(x, &buffer1[pos1 + point_byte_length - BN_num_bytes(x)]); \
pos1 = pos1 + point_byte_length

#define BUFFER_APPEND_STRING(buffer1, pos1, length1, x) \
    memcpy(&buffer1[pos1], x, length1); \
pos1 = pos1 + length1
/* @} */
/*
 * @Block:      SM2
 * @{ */
#if ((defined(CFG_SM2) && defined(CFG_SM3)) \
            || (defined(CFG_SM2_H) && defined(CFG_SM3_H)))
/*-------------------------SIGN-------------------------------------------*/
int sm2_sign(uchar *message_digest, int length, uchar *id,  int idlength,
        uchar *privatekey, uchar *rs, uchar *publickey);
int  sm2_verify(uchar *message_digest, int length, uchar *key,
        uchar *id, int idlength);
/*------------------------ENCIPHER---------------------------------------*/
int sm2_encrypt(char **sm2_param, int type, int point_bit_length,
        uchar* message, uchar* encipher, uchar* public_x, uchar* public_y);
int sm2_decrypt(char **sm2_param, int type, int point_bit_length,
        uchar* private, uchar* encryptmessage,
        int decrymessage_length, uchar* decrymessage);
int sm2_generate_pubk(char **sm2_param, int type, int point_bit_length,
        uchar* private, uchar* public_x, uchar* public_y);

#endif /* ((defined(CFG_SM2) && defined(CFG_SM3)) \
            || (defined(CFG_SM2_H) && defined(CFG_SM3_H))) */
/* @} */
/*
 * @Block:      SM3
 * @{ */
#if defined(CFG_SM3) || defined(CFG_SM3_H)
void sm3(unsigned char *input, int ilen, unsigned char output[32]);
#endif /* defined(CFG_SM3) || defined(CFG_SM3_H) */
/* @} */
/*
 * @Block:      SM4
 * @{ */
#if defined(CFG_SM4) || defined(CFG_SM4_H)

#define SM4_ENCRYPT     1
#define SM4_DECRYPT     0
int sm4_encrypt(unsigned char *encrymessage, int length,
        unsigned char *key, unsigned char *output);
int sm4_decrypt(unsigned char *decrymessage, int length,
        unsigned char *key, unsigned char *output);
#endif /* defined(CFG_SM4) || defined(CFG_SM4_H) */
/**
 * \brief       国密算法自检
 * \return      0:Success   <0:Error
 */
int sm_selftest(void);

#endif /* CFG_OPENSSL */
#endif /* SM_H_ */

