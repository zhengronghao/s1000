/*
 * =====================================================================================
 *
 *       Filename:  ctc_authent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  6/30/2016 8:23:43 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#ifndef __CTC_AUTHENT_H__
#define __CTC_AUTHENT_H__ 

#include "ParseCmd.h"
#include "wp30_ctrl.h"

typedef struct{
    uint8_t type;
    uint8_t hash[32];
    uint8_t rfu[32];
    ST_newtaginfo tag;
}ST_AUTH_VERTIFYIMG;

#define VERIFY_TYPE_MDDEM       1
#define VERIFY_TYPE_SBLL        2
#define VERIFY_TYPE_SBL1BAK     3 
#define VERIFY_TYPE_ABOOT       4 
#define VERIFY_TYPE_ABOOTBAK    5 
#define VERIFY_TYPE_RPM         6 
#define VERIFY_TYPE_rPMBAK      7 
#define VERIFY_TYPE_TZ          8 
#define VERIFY_TYPE_TZBAK       9 
#define VERIFY_TYPE_PAD         10 
#define VERIFY_TYPE_MODEMST1    11 
#define VERIFY_TYPE_MODEMST2    12 
#define VERIFY_TYPE_MISC        13 
#define VERIFY_TYPE_FSC         14 
#define VERIFY_TYPE_SSD         15
#define VERIFY_TYPE_SPLASH      16 
#define VERIFY_TYPE_DDR         17
#define VERIFY_TYPE_FSG         18
#define VERIFY_TYPE_SEC         19 
#define VERIFY_TYPE_BOOT        20
#define VERIFY_TYPE_SYSTEM      21 
#define VERIFY_TYPE_PERISIS     22
#define VERIFY_TYPE_CACHE       23 
#define VERIFY_TYPE_RECOVERY    24
#define VERIFY_TYPE_DEVINFO     25
#define VERIFY_TYPE_KEYSTORE    26 
#define VERIFY_TYPE_OEM         27 
#define VERIFY_TYPE_CONFIG      28 
#define VERIFY_TYPE_USERDATA    29

typedef struct{
    uint8_t type;
    uint8_t rfu[3];
    uint8_t data[256];
}ST_AUTH_PUBDECT;

#define AUTH_PUBDECRYPT_TYPE_START 0
#define AUTH_PUBDECRYPT_TYPE_USER  1
#define AUTH_PUBDECRYPT_TYPE_MAX 2

// GAPK
uint8_t authent_get_gapkhash_req(void);
uint16_t authent_get_gapkhash_num(uint8 num, uint8_t *outbuf, uint32_t *outlen);
uint16_t authent_sign_verifyimg(uint8_t *data, int len);
uint16_t authent_pub_decrpt(uint8_t *input, int inlen, uint8_t* output);

#endif
