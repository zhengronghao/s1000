#include "wp30_ctrl.h"
//#include "drv_is8u256a.h"

#if (defined(CFG_OPENSSL) && defined(CFG_SECURITY_CHIP))
#if ((defined(CFG_SM2) && defined(CFG_SM3)) \
            || (defined(CFG_SM2_H) && defined(CFG_SM3_H)))
/**
 * \brief       硬件生成密钥对
 * \param       sm2_param           不使用
 * \param       type                不使用
 * \param       point_bit_length    不使用
 * \param [out] private             私钥输出
 * \param [out] public_x            公钥前0x20字节
 * \param [out] public_y            公钥后0x20字节
 * \return      0:Success       <0:Error
 */
int sm2_generate_pubk(char **sm2_param, int type, int point_bit_length,
        uchar* private, uchar* public_x, uchar* public_y)
{
    int ret = sc_is8u_sm2_gen_keypair();
    if(ret < 0) {
        return ret;
    }
    unsigned char pubk[0x40];
    ret = sc_is8u_sm2_export_keypair((unsigned char *)private, pubk);
    memmove(public_x, pubk, 0x20);
    memmove(public_y, pubk + 0x20, 0x20);
    return ret;
}
/**
 * \brief       硬件SM2签名
 * \param       message_digest      明文数据
 * \param       length              明文数据长度
 * \param       id                  ID
 * \param       idlength            ID长度
 * \param       privatekey          私钥
 * \param [out] rs                  签名数据输出(0x40 byte)
 * \param       publickey           不使用
 * \return      0:Success       <0:Error
 */
int sm2_sign(uchar *message_digest, int length, uchar *id,  int idlength,
        uchar *privatekey, uchar *rs, uchar *publickey)
{
    int ret = sc_is8u_sm2_import_keypair(privatekey, NULL);
    if(ret < 0) {
        return ret;
    }
    return sc_is8u_sm2_sign(idlength, id, length, message_digest, rs);
}
/**
 * \brief       硬件SM2校验签名
 * \param       message_digest      明文数据 + 签名(0x40 byte)
 * \param       length              明文数据 + 签名长度
 * \param       key                 公钥
 * \param       id                  ID
 * \param       idlength            ID长度
 * \return      0:Success       <0:Error
 */
int  sm2_verify(uchar *message_digest, int length, uchar *key,
        uchar *id, int idlength)
{
    int ret = sc_is8u_sm2_import_keypair(NULL, key);
    if(ret < 0) {
        return ret;
    }
    unsigned char sign[0x40];
    length -= 0x40;
    memmove(sign, message_digest + length, 0x40);
    return sc_is8u_sm2_verify(idlength, id, length, message_digest, sign);
}
/**
 * \brief       硬件SM2加密
 * \param       sm2_param           不使用
 * \param       type                不使用
 * \param       point_bit_length    不使用
 * \param       message             明文数据
 * \param [out] encipher            加密结果
 * \param  [in] public_x            公钥前0x20字节
 * \param  [in] public_y            公钥后0x20字节
 * \return      0:Success       <0:Error
 */
int sm2_encrypt(char **sm2_param, int type, int point_bit_length,
        uchar* message, uchar* encipher, uchar* public_x, uchar* public_y)
{
    unsigned char pubk[0x40];
    memmove(pubk, public_x, 0x20);
    memmove(pubk + 0x20, public_y, 0x20);
    int ret = sc_is8u_sm2_import_keypair(NULL, pubk);
    if(ret < 0) {
        return ret;
    }
    return sc_is8u_sm2_enc(strlen((char *)message), message, encipher);
}
/**
 * \brief       硬件SM2解密
 * \param       sm2_param           不使用
 * \param       type                不使用
 * \param       point_bit_length    不使用
 * \param  [in] private             私钥
 * \param  [in] encryptmessage      密文数据
 * \param  [in] decrymessage_length 密文数据长度
 * \param [out] decrymessage        解密结果
 * \return      0:Success       <0:Error
 */
int sm2_decrypt(char **sm2_param, int type, int point_bit_length, uchar* private,
        uchar* encryptmessage, int decrymessage_length, uchar* decrymessage)
{
    int ret = sc_is8u_sm2_import_keypair(private, NULL);
    if(ret < 0) {
        return ret;
    }
    return sc_is8u_sm2_dec(decrymessage_length, encryptmessage, decrymessage);
}
#endif /* ((defined(CFG_SM2) && defined(CFG_SM3)) \
            || (defined(CFG_SM2_H) && defined(CFG_SM3_H))) */
#if defined(CFG_SM3) || defined(CFG_SM3_H)
/**
 * \brief       硬件SM3计算摘要值
 * \param       input               数据
 * \param       ilen                数据长度
 * \param [out] output[32]          摘要值输出
 */
void sm3(unsigned char *input, int ilen, unsigned char output[32])
{
    sc_is8u_sm3(ilen, input, output);
}
#endif /* defined(CFG_SM3) || defined(CFG_SM3_H) */
#if defined(CFG_SM4) || defined(CFG_SM4_H)
/**
 * \brief       硬件SM4加密
 * \param  [in] encrymessage        明文数据
 * \param       length              明文数据长度, 必须为16*n, n=[1,15]
 * \param       key                 密钥
 * \param [out] output              密文输出
 * \return      0:Success       <0:Error
 */
int sm4_encrypt(unsigned char *encrymessage, int length,
        unsigned char *key, unsigned char *output)
{
    return sc_is8u_sm4_enc(SM_ECB_MODE, encrymessage, length, key, NULL, output);
}
/**
 * \brief       硬件SM4解密
 * \param       decrymessage        密文数据
 * \param       length              密文数据长度, 必须为16*n, n=[1,15]
 * \param       key                 密钥
 * \param [out] output              明文输出
 * \return      0:Success       <0:Error
 */
int sm4_decrypt(unsigned char *decrymessage, int length,
        unsigned char *key, unsigned char *output)
{
    return sc_is8u_sm4_dec(SM_ECB_MODE, decrymessage, length, key, NULL, output);
}
#endif /* defined(CFG_SM4) || defined(CFG_SM4_H) */
#endif /* CFG_OPENSSL */

