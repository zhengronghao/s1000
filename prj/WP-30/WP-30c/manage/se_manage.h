/*
 * =====================================================================================
 *
 *       Filename:  authmanage.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  6/20/2016 6:07:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yehf (), yehf@itep.com.cn
 *        Company:  START
 *
 * =====================================================================================
 */

#ifndef __SE_MANAGE__ 
#define __SE_MANAGE__

// 公钥 CA、FSK、SE、START_APK
#define  START_CA_PUBK       0
#define  START_FSK_PUBK      1
#define  START_SE_PUBK       2
#define  START_APK_PUBK      3 
#define  USER_APK_PUBK       4 


typedef struct
{
    unsigned char version[16]; //固件信息，例如：”S-980E_CTRL”
    unsigned int pkcver;        //pkc版本     默认0
    unsigned int ca;            //CA根密钥索引  默认0
    unsigned int ca_start;      //厂商根密钥索引   默认0
    unsigned int codelen;      //tag区前的固件大小
    unsigned int offset;        //固件签名起始偏移，预留
    unsigned int len;           //固件签名长度，预留
    unsigned int issuseindex;       //索引，采用实达应用固件签名密钥时使用，区分不同客户
    unsigned char publisher[16]; //"START IFT CA"
    unsigned char buildtime[16]; //签名时间
    unsigned char deadtime[16];  //签名过期时间
    unsigned char issue[64];      //客户名，给公钥证书时使用
    unsigned char rfu[52];      //预留
    unsigned char sha[32];      //SHA校验码明文, 以上进行SHA计算
    unsigned char RSA[256];     //签名数据密文，用私钥加密
    unsigned char tag[16];       //"CHECK:RSA"
}ST_newtaginfo;


#define LABLE_NRSA256CHECK  "CHECK:RSAN256" //ST_newtaginfo:SHA256+RSA256


#define sys_get_time  sys_GetTime

int se_des_decrypt (unsigned int inlen, void *in, void *out, unsigned int keylen, void *key);
int se_des_encrypt (unsigned int inlen, void *in, void *out, unsigned int keylen, void *key);
int s_DesRSAPubk(uint mode, uint inlen, uchar *in, uchar *out);
/*
 * s_check_tag - [GENERIC] 校验ST_newtaginfo区，并返回解密后的sha值
 *    type: 0-常规固件  1-权限包固件
 * @
 */
int s_check_tag (uint type,ST_newtaginfo *ptag, uchar *sha);
/*
 * s_startpubk_decrypt - [GENERIC] 使用厂商公钥解密
 *     type: 0-CA公钥 1-读厂商固件公钥 2-读防拆解除公钥 3-读实达应用公钥
 *     uilen:解密数据长度，必须小于公钥长度
 * @
 */
int s_startpubk_decrypt (unsigned int type, unsigned int uilen, unsigned char *in, unsigned char *out, unsigned int* outlen);
/*
 * s_check_firmware - [GENERIC] 固件签名
 *  type:固件类型  0-常规固件  1-权限包固件
 *  sha:SHA值
 *  tag:ST_newtaginfo结构体
 *  返回值: < 0: 错误
 *          >=0: 固件类型
 *
 * @
 */
int s_check_firmware (unsigned int type, unsigned char *sha, unsigned char *p);
/*
 * s_scert_authenticate - [GENERIC] 权限包认证
 * 输入:   inlen:权限包长度 
 *         in:权限包
 *
 * 输出:   outlen:输出长度
 *         out:   输出信息: 权限包类型(1B)+新的鉴权码(8B)
 *
 * 返回值: 0-成功     1-包长度错误 2-tag关键字错误 3-签名校验错误   4-包头错误 
 *         5-次数错误 6-ID比较错误 7-读鉴权码错误  8-鉴权码比较错误 9-写鉴权码错误
 * @
 */
int se_cert_authenticate (uint inlen, uchar *in, uint *outlen, uchar *out);
/*
 * s_OperAuthICCard - [GENERIC] IC卡认证
 * 输入:   iCmdId:   命令      0-降级解锁 1-防拆解锁 2-镜像特权恢复 255-获取库版本
 *         iInLen:   输入长度，必须为6位 
 *         pacInData:输入卡片密码
 *
 * 输出:   piOutLen:     输出长度
 *         pacOutData:   输出错误提示，当返回值为0或4~12时输出8位卡号   
 	  0-"操作成功",
    1-"卡不在位",
    2-"上电失败",
    3-"无效卡",
    4-"卡片过期无效",
    5-"卡片认证失败",
    6-"密码错误",
    7-"非法卡",
    8-"卡有效次数用尽",
    9-"卡有效次数不足",
    10-"卡片异常",
    11-"密码长度错误",
    12-"卡片已锁定",
 *
 * 返回值: 见上
 * @
 */
int s_OperAuthICCard(int iCmdId,int iInLen,char *pacInData,int *piOutLen, char *pacOutData);

#endif

