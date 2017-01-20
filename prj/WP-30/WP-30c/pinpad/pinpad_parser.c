/***************** (C) COPYRIGHT 2016 START Computer equipment *****************
 * File Name          : pinpad_parser.c
 * bfief              : 
 * Author             : luocs 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 4/24/2016 1:53:32 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
#include "pinpad_parser.h"
#include "pinpad_keystore.h"

static struct PED_SYS sgPedSys;
_OFFLINE_KEYINFO goffline_keyInfo;

//PED:ping entry device

uint8_t ped_algr_blk_size(uint8_t algr)
{
    uint8_t blksize;
    switch ( algr )
    {
    default :
    case PED_ALGR_DES_ENC :
    case PED_ALGR_DES_DEC :
        blksize = 8;
        break;
    case PED_ALGR_AES_ENC :
    case PED_ALGR_AES_DEC :
        blksize = 16;
        break;
    case PED_ALGR_SM4_ENC :
    case PED_ALGR_SM4_DEC :
        blksize = 16;
        break;
    }
    return blksize;
}

int ped_algr_check(uint8_t algr)
{
    if ( algr > PED_ALGR_SM4_DEC ) {
        return RTV_PED_ERR_ALGR;
    }
    return RTV_PED_SUCCE;
}

int ped_crypt_ecb(uint8_t algr, uint8_t *key, uint8_t keylen,
                  int32_t length,uint8_t *input, uint8_t *output)
{
    int iRet = 0;

    switch ( algr )
    {
    case PED_ALGR_DES_ENC :
        des_encrypt_ecb(input,output,length,key,keylen/8);
        break;
    case PED_ALGR_DES_DEC :
        des_decrypt_ecb(input,output,length,key,keylen/8);
        break;
    case PED_ALGR_SM4_ENC :
        iRet = sm4_encrypt(input, length,key, output);
        break;
    case PED_ALGR_SM4_DEC :
        iRet = sm4_decrypt(input, length,key, output);
        break;
    case PED_ALGR_AES_ENC :
    case PED_ALGR_AES_DEC :
    default :
        iRet = RTV_PED_ERR_ALGR;
        break;
    }
    return iRet;
}


int ped_crypt_cbc(uint8_t algr,uint8_t *iv,
                  uint8_t *key, uint8_t keylen,
                  int32_t length,uint8_t *input,
                  uint8_t *output)
{
    int iRet = 0;

    switch ( algr )
    {
    case PED_ALGR_DES_ENC :
        des_encrypt_cbc(input,output,length,key,keylen/8,iv);
        break;
    case PED_ALGR_DES_DEC :
        des_decrypt_cbc(input,output,length,key,keylen/8,iv);
        break;
    case PED_ALGR_SM4_ENC :
        iRet = sm4_encrypt(input, length,key, output);
        break;
    case PED_ALGR_SM4_DEC :
        iRet = sm4_decrypt(input, length,key, output);
        break;
    case PED_ALGR_AES_ENC :
    case PED_ALGR_AES_DEC :
        iRet = RTV_PED_ERR_ALGR;
        break;
    default :
        iRet = RTV_PED_ERR_ALGR;
        break;
    }
    return iRet;
}
int ped_calcu_mac_x99(uint8_t algr, uint8_t *iv,
                      uint8_t klen, uint8_t *key,
                      uint32_t length,uint8_t *input, 
                      uint8_t *output)
{
    uint8_t blk[16],mac[16];
    uint8_t blksize =  ped_algr_blk_size(algr);
    uint32_t i,j;
    int iRet;

    if ((iRet = ped_algr_check(algr)) != 0) {
        return iRet;
    }
    if ( klen == 0 
         || klen%blksize != 0
         || length == 0
         || length%blksize != 0) {
        return RTV_PED_ERR_PARA;
    }
    CLRBUF(blk);
    CLRBUF(mac);
    if (iv != NULL)
    {
        memcpy(mac,iv,blksize);
    }
    for ( i=0 ; i<length ; i+=blksize ) 
    {
        TRACE("\n");
        for ( j=0 ; j<blksize ; j++ ) 
        {
            blk[j] = input[i+j] ^ mac[j];
            TRACE(" %02X",blk[j]);
        }
        ped_crypt_ecb(algr,key,klen,blksize,blk,mac);
        TRACE_BUF("ecb",mac,blksize);
    }
        TRACE("\r\nL:%d",__LINE__);
    memcpy(output,mac,blksize);
    return RTV_PED_SUCCE;
}

int ped_calcu_mac_ecb1(uint8_t algr, uint8_t *iv,
                      uint8_t klen, uint8_t *key,
                      uint32_t length,uint8_t *input, 
                      uint8_t *output)
{
    uint8_t blk[16],mac[16];
    uint8_t blksize =  ped_algr_blk_size(algr);
    uint32_t i,j;

    if ( klen == 0 
         || klen%blksize != 0
         || length == 0
         || length%blksize != 0) {
        return RTV_PED_ERR_PARA;
    }
    CLRBUF(blk);
    CLRBUF(mac);
    if (iv != NULL)
    {
        memcpy(blk,iv,blksize);
    }
    for ( i=0 ; i<length-blksize ; i+=blksize ) 
    {
        for ( j=0 ; j<blksize ; j++ ) 
        {
            blk[j] ^= input[i+j];
        }
    }
    ped_crypt_ecb(algr,key,klen,blksize,blk,mac);
    memcpy(output,mac,blksize);
    return RTV_PED_SUCCE;
}


int ped_calcu_mac_ecb0(uint8_t algr, uint8_t *iv,
                      uint8_t klen, uint8_t *key,
                      uint32_t length,uint8_t *input, 
                      uint8_t *output)
{
    uint8_t blk[32],mac[32];
    uint8_t blksize =  ped_algr_blk_size(algr);
    uint32_t i,j;

    if ( klen == 0 
         || klen%blksize != 0
         || length == 0
         || length%blksize != 0) {
        return RTV_PED_ERR_PARA;
    }
    CLRBUF(blk);
    CLRBUF(mac);
//    DISPBUF("iv0",8,0,iv);
    if (iv != NULL)
    {
        memcpy(mac,iv,blksize);
    }
    TRACE("\r\n blksize:%d",blksize);
//    DISPBUF("iv",8,0,mac);
    for ( i=0 ; i<length; i+=blksize ) 
    {
        for ( j=0 ; j<blksize ; j++ ) 
        {
            mac[j] ^= input[i+j];
        }
//        DISPBUF("b",8,0,mac);
    }
    SI_ucParseStr(2,blksize,mac,blk);
//    DISPBUF("b0",16,0,blk);
    ped_crypt_ecb(algr,key,klen,blksize,blk,mac);
//    DISPBUF("b1",8,0,mac);
    for ( j=0 ; j<blksize ; j++ ) 
    {
        mac[j] ^= blk[blksize+j];
    }
//    DISPBUF("b2",8,0,mac);
    ped_crypt_ecb(algr,key,klen,blksize,mac,blk);
//    DISPBUF("b3",8,0,blk);
    SI_ucParseStr(2,blksize,blk,mac);
//    DISPBUF("b4",8,0,mac);
    memcpy(output,mac,blksize);
    return RTV_PED_SUCCE;
}


int ped_calcu_mac_x919(uint8_t algr, uint8_t *iv,
                       uint8_t klen, uint8_t *key,
                       uint32_t length,uint8_t *input, 
                       uint8_t *output)
{
    int iRet;
    uint8_t mac[16];
    uint8_t blksize;

    TRACE("\r\nx919 algr:%d klen:%d length:%d",algr,klen,length);
    // 加密单位长度
    blksize = ped_algr_blk_size(algr);
    if ((iRet = ped_calcu_mac_x99(algr,iv,blksize,key,length,input,mac)) != 0)
    {
        return iRet;
    }
    if (klen == 2*blksize)
    {
        ped_crypt_ecb((algr+0x01),key+blksize,blksize,blksize,mac,mac);
        ped_crypt_ecb(algr,key,blksize,blksize,mac,mac);
    } else if (klen == 3*blksize)
    {
        ped_crypt_ecb((algr+0x01),key+blksize,blksize,blksize,mac,mac);
        ped_crypt_ecb(algr,key+2*blksize,blksize,blksize,mac,mac);
    }
    memcpy(output,mac,blksize);

    return RTV_PED_SUCCE;
}

//=================================================================================
//=================================================================================
//=================================================================================
int ped_init(uint32_t mode)
{
    memset(&sgPedSys,0,sizeof(struct PED_SYS));
    return RTV_PED_SUCCE;
}

int ped_open(uint32_t mode)
{
    return 0;
}


int ped_close(uint32_t mode)
{
    return 0;
}

int pubk_get_blk_num(uint8_t index)
{
    return (FPOS(struct KEY_ZONE,pubk.TK[index])>>5);
}

int mk_get_blk_num(uint8_t index)
{
    return (FPOS(struct KEY_ZONE,mk.orig[index])>>5);
}

int wk_get_blk_num(uint8_t mk_index,uint8_t type,uint8_t index)
{
    return (FPOS(struct KEY_ZONE,wk[mk_index].pink[WORKINGKEY_NUM*(type-KEY_TYPE_PINK)+index])>>5);
}

int ped_key_read(uint8_t kektype,uint8_t kekindex,uint8_t type, uint8_t index, uint16_t *length,uint8_t *val)
{
    int iRet;

    switch (type)
    {
    case KEY_TYPE_MK:
        if (kektype != KEY_TYPE_SRK 
            && kektype != KEY_TYPE_TK 
            && kektype != KEY_TYPE_PROCESSKEY) {
            DISPERR(kektype);
            iRet = RTV_PED_ERR_USAGE_FAULT;
            break;
        }
        iRet = key_second_level_read(SRK_BLK_NUM,mk_get_blk_num(index),length,val);
        break;
    case KEY_TYPE_TK:
        iRet = key_second_level_read_nobackup(SRK_BLK_NUM,pubk_get_blk_num(index),length,val);
        break;
    case KEY_TYPE_PROCESSKEY:
        *length = sizeof(sgPedSys.rand);
        memcpy(val,sgPedSys.rand,sizeof(sgPedSys.rand));
        break;
    case KEY_TYPE_PINK:
    case KEY_TYPE_MACK:
    case KEY_TYPE_DESK:
        if (kektype != KEY_TYPE_MK) {
            DISPERR(kektype);
            iRet = RTV_PED_ERR_USAGE_FAULT;
            break;
        }
        iRet = key_third_level_read(mk_get_blk_num(kekindex),
                                    wk_get_blk_num(kekindex,type,index),
                                    length,val);
        if ( iRet ) {
            TRACE("\r\nkey_third_level_read err line:%d, %d",__LINE__, iRet);
        }
        break;
    default:
        TRACE("\n-|key type:%02X error",type);
        iRet = RTV_PED_ERR_KEYTYPE;
        break;
    }
    return iRet;
}


void ped_key_extend(uint8_t keylen, uint8_t *input, uint8_t *output)
{
    memcpy(output,input,keylen);
    if(keylen == 8)
    {
        memcpy(output+8,input,8);
        memcpy(output+8+8,input,8);
    }
    else if(keylen == 16)
    {
        memcpy(output+16,input,8);
    }
}

int ped_key_write(uint8_t kektype,uint8_t kekindex,uint8_t type, uint8_t index, uint16_t length,uint8_t *val)
{
    int iRet;
    uchar key[24];

    if (kekindex >= MASTERKEY_NUM) 
    {
        TRACE("\n-|kekindex:%d Error",kekindex);
        return RTV_PED_ERR_PARA;
    }
    if ( type >= KEY_TYPE_AUTHMK && type <= KEY_TYPE_TK)
    {
        ped_key_extend(length,val,key);
        length = 24;
    } else {
        memcpy(key,val,length);
    }
    switch (type)
    {
    case KEY_TYPE_MK:
//        if ( encryptflag ) {
            if (kektype != KEY_TYPE_SRK 
                && kektype != KEY_TYPE_TK 
                && kektype != KEY_TYPE_PROCESSKEY) {
                DISPERR(iRet);
                iRet = RTV_PED_ERR_USAGE_FAULT;
                break;
            }
//        }
        iRet = key_second_level_write(SRK_BLK_NUM,mk_get_blk_num(index),length,key);
        break;
    case KEY_TYPE_TK:
        iRet = key_second_level_write_nobackup(SRK_BLK_NUM,pubk_get_blk_num(index),length,key);
        break;
    case KEY_TYPE_PROCESSKEY:
        memcpy(sgPedSys.rand,key,length);
        iRet = 0;
        break;
    case KEY_TYPE_PINK:
    case KEY_TYPE_MACK:
    case KEY_TYPE_DESK:
        if (kektype != KEY_TYPE_MK) {
            iRet = RTV_PED_ERR_USAGE_FAULT;
            break;
        }
        iRet = key_third_level_write(mk_get_blk_num(kekindex),
                                     wk_get_blk_num(kekindex,type,index),
                                     length,key);
        break;
    default:
        TRACE("\n-|key type:%02X error",type);
        iRet = RTV_PED_ERR_PARA;
        break;
    }
    return iRet;
}

int ped_key_type_check(uint8_t type, uint8_t index)
{
    TRACE("\r\ntype:%02X index:%02d",type,index);
    if (type == KEY_TYPE_MK)
    {
        if ( index >= MASTERKEY_NUM ) {
            return RTV_PED_ERR_KEYINDEX;
        }
    } else if (type >= KEY_TYPE_PINK && type <= KEY_TYPE_DESK)
    {
        if ( index >= WORKINGKEY_NUM ) {
            return  RTV_PED_ERR_KEYINDEX;
        }
    } else if (type == KEY_TYPE_TK 
              || type == KEY_TYPE_PROCESSKEY 
              || type == KEY_TYPE_CURKEY)
    {
        if ( index > 0) {
            return  RTV_PED_ERR_KEYINDEX;
        }
    } else if (type == KEY_TYPE_SRK)
    {
        if ( index != 0) {
            return RTV_PED_ERR_KEYTYPE;
        }
    }
    return RTV_PED_SUCCE;
}

int ped_load_key_check(uint32_t length,const uint8_t *input)
{
    const struct PEDLoadKeyHead *head = (struct PEDLoadKeyHead *)input;
    int iRet;

    if (head->ver != 0) {
        TRACE("\r\nkeycheck %d",__LINE__);
        return RTV_PED_ERR;
    }
    if ((iRet = ped_key_type_check(head->type,head->index)) != 0) {
        TRACE("\r\nkeycheck %d %d-%d",__LINE__,head->type,head->index);
        return iRet;
    }
    if ((iRet = ped_key_type_check(head->kektype,head->kekindex)) != 0) {
        TRACE("\r\nkekcheck %d %d-%d",__LINE__,head->kektype,head->kekindex);
        return iRet;
    }
    if (head->type == KEY_TYPE_MK)
    {
//        if ((head->kektype != KEY_TYPE_SRK) && (head->kektype != head->type)) {
//            TRACE("\r\nkeycheck %d",__LINE__);
//            return iRet;
//        }
        // MK必须MK,TK,过程密钥加密下载或明文下载
        if ( !(head->kektype == KEY_TYPE_SRK || head->kektype == KEY_TYPE_TK 
               || head->kektype == KEY_TYPE_PROCESSKEY || head->kektype == KEY_TYPE_MK) ) {
            TRACE("\r\nkeycheck %d",__LINE__);
            return RTV_PED_ERR_PARA;
        }
    } else if ( head->type >= KEY_TYPE_PINK && head->type <= KEY_TYPE_DESK ) {
        // 工作密钥必须MK加密下载
        if ( head->kektype !=  KEY_TYPE_MK) {
            TRACE("\r\nkeycheck %d",__LINE__);
            return RTV_PED_ERR_PARA;
        }
        if ( head->encmode == 0) {
            TRACE("\r\nkeycheck %d",__LINE__);
            return RTV_PED_ERR_PARA;
        }
    }
    if ( (iRet = ped_algr_check(head->algr)) != 0) {
        return iRet;
    }
    if ( (head->checkmode > 2) 
         || (head->checkmode == 1 && (head->checklen != 4 && head->checklen != 8)) 
         || (head->checkmode == 0 && head->checklen != 0)//不校验,checklen不等于0
         || (head->checkmode == 2 && head->type != KEY_TYPE_MK)//不校验,checklen不等于0
       )
    {
        TRACE("\r\nkeycheck %d",__LINE__);
        return RTV_PED_ERR_PARA;
    }

    if (head->ivlen != 0 
        && head->ivlen != ped_algr_blk_size(head->algr))
    {
        return RTV_PED_ERR_PARA;
    }

    if ( (head->keylen != 8 && head->keylen != 16 && head->keylen != 24) )
    {
        TRACE("\r\nkeycheck %d",__LINE__);
        return RTV_PED_ERR_KEYLEN;
    }

    return RTV_PED_SUCCE;
}

int ped_load_key_mac_8zero(uint8_t keylen,uint8_t *key,uint8_t chklen,const uint8_t *check)
{
    uint8_t chk[8];

    CLRBUF(chk);
	des_encrypt_ecb(chk,chk,8,key,keylen/8);
//    TRACE_BUF("chk", chk, 8);
//    TRACE_BUF("check", check, 8);
    if ( memcmp(chk,check,chklen) != 0 ) {
        TRACE("\r\ndes_encrypt_ecb err");
        return RTV_PED_ERR_MACK;
    }
    return RTV_PED_SUCCE;
}

// X919 check
int ped_load_key_mac_x919(uint8_t algr,uint8_t *iv,
                          uint8_t klen, uint8_t *key,
                          uint8_t length, uint8_t *input, 
                          uint8_t checklen, uint8_t *check)
{
    int iRet;
    uint8_t mac[16];

    CLRBUF(mac);
    iRet = ped_calcu_mac_x919(algr,iv,klen,key,length,input,mac);
    if (iRet == RTV_PED_SUCCE)
    {
        if (memcmp(mac,check,checklen) != 0)
        {
            TRACE("\r\nped_calcu_mac_x919 err");
            iRet = RTV_PED_ERR_MACK;
        }
    }
	return iRet;
}


int ped_load_key(uint16_t length,uint8_t *packet)
{
    struct PEDLoadKeyHead *head = NULL; 
    uint8_t kek[24];
    uint8_t key[24];
    uint16_t keklen=0,keylen;
    int iRet;

    if ( (length < sizeof(struct PEDLoadKeyHead)+2)
        || (length != COMBINE16(packet[1],packet[0])+2)) {
        TRACE("\r\nKEY LEN%d, len:%d",length, COMBINE16(packet[1],packet[0])+2);
        TRACE_BUF("KEY:",  packet, length);
        return RTV_PED_ERR;
    }
    length -= 2;
    packet += 2;
    TRACE_BUF("key packet",packet,length);
    if ( (iRet = ped_load_key_check(length,packet)) != 0 ) {
        TRACE("\r\nloadkey %d",__LINE__);
        return iRet;
    }
    head = (struct PEDLoadKeyHead *)packet;
    if (head->encmode == 1)
    {
        if ( (iRet = ped_key_read(KEY_TYPE_SRK,SRK_BLK_NUM,head->kektype,head->kekindex,&keklen,kek)) != 0) {
            TRACE("\r\nloadkey %d",__LINE__);
            return iRet;
        }
        if ( (iRet = ped_crypt_ecb(PED_ALGR_DES_DEC,kek,keklen,head->keylen,head->keyval,key)) != 0) {
            TRACE("\r\nloadkey %d",__LINE__);
            return iRet;
        }
        TRACE_BUF("KEK",kek,keklen);
        keylen = head->keylen;
    } else 
    {
        keylen = head->keylen;
        memcpy(key,head->keyval,keylen);
    }

    iRet = 0;
    switch (head->checkmode)
    {
    case 0://不校验
        break;
    case 1://
        iRet = ped_load_key_mac_8zero(keylen,key,head->checklen,head->keyval+keylen);
        break;
    case 2:// 校验主密钥
        if ( (iRet = ped_key_read(KEY_TYPE_SRK,SRK_BLK_NUM,KEY_TYPE_MK,head->index,&keklen,kek)) != 0) {
            //        if ( (iRet = ped_key_read(head->kektype,head->kekindex,head->type,head->index,&keklen,kek)) != 0) {
            TRACE("\r\nloadkey %d",__LINE__);
            return iRet;
        }
        TRACE_BUF( "mastkey",kek,keklen );
        iRet = ped_load_key_mac_8zero(keklen,kek,head->checklen,head->keyval+keylen);
        if ( iRet ) {
            TRACE("\r\nMASTERKEY cmp err\r\n"); 
        }
        //用KEK进行x919运算
        //        iRet = ped_load_key_mac_x919(head->algr,
        //                                     (head->ivlen == 0)?NULL:(head->keyval+keylen+head->checklen),//iv
        //                                     keklen,kek,
        //                                     keylen,key,
        //                                     head->checklen,head->keyval+keylen);

        break;
    default:
        TRACE("\r\ncheckmode err\r\n"); 
        iRet = RTV_PED_ERR;
        break;
    }
    if (iRet == RTV_PED_SUCCE) {
        TRACE_BUF("\r\nkey writed",key,keylen);
        iRet = ped_key_write(head->kektype,head->kekindex,head->type,head->index,keylen,key);
    }
    CLRBUF(kek);
    CLRBUF(key);
    return iRet;
}


int ped_load_mk(uint32_t length,uint8_t *protocol)
{
    return ped_load_key(length,protocol);
}


int ped_load_wk(uint32_t length,uint8_t *protocol)
{
    return ped_load_key(length,protocol);
}

int ped_get_rand(uint32_t inlen,uint8_t *input,uint16_t *outlen,uint8_t *output)
{
    uint16_t rng_len = 0;

    if ( inlen != 2 )
    {
        return RTV_PED_ERR_PARA;
    }
    rng_len = COMBINE16(input[1],input[0]);
    if ( (rng_len > 2048) || rng_len < 2 || rng_len%2 != 0 ) {
        TRACE("\r\nrng:%d %d %d",rng_len,input[0],input[1]);
        return RTV_PED_ERR_PARA;
    }
    *outlen = rng_len;
    RNG_FillRandom(output,rng_len);
    return RTV_PED_SUCCE;
}

int string_check(const char *input,uint32_t length)
{
    uint32_t i;

    for ( i=0 ; i<length ; i++ )
    {
        if ( input[i]<'0' || input[i] > '9' ) 
        {
            return -1;
        } 
    }
    return 0;
}
#if 0
int ped_get_pin_online(uint32_t inlen,uint8_t *packet,uint16_t *outlen,uint8_t *output)
{
    struct PEDOnlinePinHead *head = NULL;
    int iRet;
    uint16_t length;
    uint16_t klen;
    uint8_t key[24];
    uint8_t blksize = 0;
    uint16_t lcd_x,lcd_y;
    uint16_t str_len = 0;
    uint32_t lcd_mode =0 ;
    uint32_t pin_len;
    uint8_t pinval[32];

    if ( inlen > 2048
        || (inlen < FPOS(struct PEDOnlinePinHead,string)+2)
        || (inlen != COMBINE16(packet[1],packet[0])+2))
    {
        TRACE("\r\nencdata LEN%d",inlen);
        return RTV_PED_ERR;
    }
    TRACE_BUF("encdata",packet,inlen);
    head = (struct PEDOnlinePinHead *)(packet+2);
    packet[inlen] = '\0';//deal string
    str_len = strlen(head->string);
    if ( head->rfu != 0 ) {
        return RTV_PED_ERR_PARA;
    }
    if (head->type != KEY_TYPE_PINK) {
        return RTV_PED_ERR_USAGE_FAULT;
    }
    if ((iRet = ped_key_type_check(head->type,head->index)) != 0) {
        TRACE("->L:%d",__LINE__);
        return iRet;
    }
    if ( (iRet = ped_algr_check(head->algr)) != 0) {
        return iRet;
    }
    if ( head->minlen < PED_PIN_LEN_MIN 
         || head->maxlen >PED_PIN_LEN_MAX 
         || head->minlen > head->maxlen)
    {
        return RTV_PED_ERR_PARA;
    }
    if (head->timeout == 0) {
        head->timeout = PED_PIN_TOUT_MAX;
    }
    if ( head->timeout > PED_PIN_TOUT_MAX
         || (head->timeout < PED_PIN_TOUT_MIN))
    {
        return RTV_PED_ERR_PARA;
    }
    if (head->encmode > 0)//0-9.8 1-3.92(不支持)
    {
        return RTV_PED_ERR_PARA;
    }
    if (string_check(head->cardno,12) != 0)
    {
        return RTV_PED_ERR_PARA;
    }
    if (str_len != 0)
    {
        if (string_check(head->string,str_len) != 0) {
            return RTV_PED_ERR_PARA;
        }
        lcd_x = COMBINE16(head->x_h,head->x_l);
        lcd_y = COMBINE16(head->y_h,head->y_l);
        lcd_mode = COMBINE32(head->lcd_mode_hh,head->lcd_mode_hl,
                             head->lcd_mode_lh,head->lcd_mode_ll);
//        @todo:check lcd para
#warning "check lcd para"
        /*
        if ( lcd_x || lcd_y || lcd_mode) {
            return RTV_PED_ERR_PARA;
        }
        lcd_display(lcd_x,lcd_y,lcd_mode,head->string)
        */
    }

    if (head->encmode == 0)
    { //9.8
        iRet = KB_GetPINStr(4,head->minlen,head->maxlen,head->timeout*1000,&pin_len,pinval);
        switch (iRet)
        {
        case 0:
//            @todo:read PIN key 
//            PinEncrypt((uint) (mode&0x7F), (uint) keylen, key, (uint) pinLen, 
//                       pucData + 5, pinStr, &len,pucData + 1);
            break;
        case 1:
            iRet = RTV_PED_ERR_INPUT_CANCEL;
            break;
        case 2:
            iRet = RTV_PED_ERR_INPUT_TIMEOUT;
            break;
        default:
            iRet = RTV_PED_ERR;
            break;
        }
    }

    return RTV_PED_SUCCE;
}
#endif

int ped_get_pin_online(uint32_t inlen,uint8_t *packet,uint16_t *outlen,uint8_t *output)
{
    struct PEDOnlinePinHead *head = NULL;
    int iRet;
//    uint8_t key[24];
//    uint8_t blksize = 0;
    uint16_t str_len = 0;
//    uint32_t lcd_mode =0 ;
    uint32_t pin_len;
    uint8_t pinval[32];

    uint16_t klen;
    uint8_t key[24];
    uint len;
    uint8_t keymode = 0;

    if ( inlen > 2048
        || (inlen != COMBINE16(packet[1],packet[0])+2))
    {
        TRACE("\r\nencdata LEN:%d",inlen);
        return RTV_PED_ERR;
    }
    TRACE_BUF("encdata",packet,inlen);
    head = (struct PEDOnlinePinHead *)(packet+2);
    packet[inlen] = '\0';//deal string

    TRACE("get the string pos:%d\r\n", FPOS(struct PEDOnlinePinHead,string) );
    //    if (  inlen > (FPOS(struct PEDOnlinePinHead,string)+2)) {
    //        TRACE("get the string pos:%d\r\n", FPOS(struct PEDOnlinePinHead,string) );
    //        str_len = strlen(head->string);
    //    }
//    if ( head->rfu != 0 ) {
//        return RTV_PED_ERR_PARA;
//    }
    if (head->type != KEY_TYPE_PINK) {
        return RTV_PED_ERR_USAGE_FAULT;
    }
    if ((iRet = ped_key_type_check(head->type,head->index)) != 0) {
        TRACE("->L:%d",__LINE__);
        return iRet;
    }
//    if ( (iRet = ped_algr_check(head->algr)) != 0) {
//        return iRet;
//    }
    if ( head->minlen < PED_PIN_LEN_MIN 
         || head->maxlen >PED_PIN_LEN_MAX 
         || head->minlen > head->maxlen)
    {
        return RTV_PED_ERR_PARA;
    }
    if (head->timeout == 0) {
        head->timeout = PED_PIN_TOUT_MAX;
    }
//    if ( head->timeout > PED_PIN_TOUT_MAX
//         || (head->timeout < PED_PIN_TOUT_MIN))
//    {
//        return RTV_PED_ERR_PARA;
//    }
    
    if (string_check(head->cardno,12) != 0)
    {
        return RTV_PED_ERR_PARA;
    }
    if (str_len != 0)
    {
        if (string_check(head->string,str_len) != 0) {
            return RTV_PED_ERR_PARA;
        }
    }

    if ((head->showlenflag > 1) || (head->beepflag > 1)) {
        return RTV_PED_ERR_PARA;
    }

    keymode = ((head->showlenflag)<<2) | (head->beepflag);
    iRet = KB_GetPINStr(keymode,head->minlen,head->maxlen,head->timeout*1000,&pin_len,pinval);
    switch (iRet)
    {
    case 0:
        if ( pin_len ) {
            ped_key_read(KEY_TYPE_MK,sgPedSys.curr_index_mk,KEY_TYPE_PINK, head->index, &klen, (uint8_t *)(&key[0]));
            //            @todo:read PIN key 
            PinEncrypt((uint) (0&0x7F), (uint) klen, key, (uint) pin_len, 
                       (uchar *)(&head->cardno[0]), pinval, &len,output);
            *outlen = (uint16_t)len;
        }else{
            *outlen = 0; 
//            iRet = RTV_PED_ERR_KEYNULL;
        }
//        PinEncrypt(0, (uint) keylen, key, (uint) pinLen, 
//                   pucData + 5, pinStr, &len,pucData + 1);
        break;
    case 1:
        iRet = RTV_PED_ERR_INPUT_CANCEL;
        break;
    case 2:
        iRet = RTV_PED_ERR_INPUT_TIMEOUT;
        break;
    default:
        iRet = RTV_PED_ERR;
        break;
    }

    return iRet;
}


int offlinekey_des_encrypt(uchar *in, uchar *out, uint * outlen)
{
//    int ret;
    uint len;
    len = (goffline_keyInfo.indatalen+7)/8*8;//长度凑足8字节
    des_encrypt_ecb(in, out, len, goffline_keyInfo.keybuf, goffline_keyInfo.keylen/8);
    *outlen = len;
    return 0;
}

int offlinekey_des_decrypt(uchar *in, uchar *out, uint * outlen)
{
//    int ret;
    uint len;
    len = (goffline_keyInfo.indatalen+7)/8*8;//长度凑足8字节
    des_decrypt_ecb(in, out, len, goffline_keyInfo.keybuf, goffline_keyInfo.keylen/8);
    *outlen = goffline_keyInfo.indatalen;
//    return ret;
    return 0;
}

int ped_get_pin_offline(uint32_t inlen,uint8_t *input,uint16_t *outlen,uint8_t *output)
{
//    int keylen,ret;
    int ret;
    uint outputlen;
    uchar buf[32];
    struct PEDOfflinePinHead *head;
    uint8_t keymode = 0;

    memset(buf, 0, sizeof(buf));//8字节对齐补0

    if ( inlen > 2048
         || (inlen != COMBINE16(input[1],input[0])+2))
    {
        TRACE("\r\noffline LEN:%d",inlen);
        return RTV_PED_ERR;
    }


//    TRACE_BUF("encdata",input+2,inlen-2);
    head = (struct PEDOfflinePinHead *)(input+2);
    TRACE("----------------offline timeout:%d---------------\r\n", head->timeout);
//    if (head->timeout == 0) {
//        head->timeout = PED_PIN_TOUT_MAX;
//    }
    //    keylen = kb_getstr(0x25, head->minlen, head->maxlen, -1, (char *)buf);
    if ((head->showlenflag > 1) || (head->beepflag > 1)) {
        return RTV_PED_ERR_PARA;
    }

    keymode = ((head->showlenflag)<<2) | (head->beepflag);
    ret = KB_GetPINStr(keymode,head->minlen,head->maxlen,head->timeout*1000, &outputlen, buf);
    switch (ret)
    {
    case 0:
        //            @todo:read PIN key 
        //            PinEncrypt((uint) (mode&0x7F), (uint) keylen, key, (uint) pinLen, 
        //                       pucData + 5, pinStr, &len,pucData + 1);
        if ( 0 == outputlen ) {
            *outlen = 0; 
//            ret = RTV_PED_ERR_KEYNULL;           
        }else{
            if ( 0 == head->mode ) {
                *outlen = outputlen & 0xffff;
                memcpy(output, (uchar *)buf, *outlen);
            }else{
                *outlen = outputlen & 0xffff;
                memset(&goffline_keyInfo, 0, sizeof(_OFFLINE_KEYINFO));
                goffline_keyInfo.keylen = 16;
                goffline_keyInfo.indatalen = outputlen;
                RNG_FillRandom(goffline_keyInfo.keybuf, goffline_keyInfo.keylen);//获取随机数
                ret = offlinekey_des_encrypt((uchar *)buf, output, (uint *)outlen);
                if ( ret == 0 ) {
                    return RTV_PED_SUCCE;
                }
            }
        }
        break;
    case 1:
        ret = RTV_PED_ERR_INPUT_CANCEL;
        break;
    case 2:
        ret = RTV_PED_ERR_INPUT_TIMEOUT;
        break;
    default:
        ret = RTV_PED_ERR;
        break;
    }
    
    return ret;
}


//
//int ped_get_pin_offline(uint32_t inlen,uint8_t *input,uint16_t *outlen,uint8_t *output)
//{
//
//    return RTV_PED_SUCCE;
//}
//
int ped_get_encrpt_data(uint32_t inlen,uint8_t *packet,uint16_t *outlen,uint8_t *output)
{
    struct PEDEncrDataHead *head = NULL;
    int iRet;
    uint16_t length;
    uint16_t klen;
    uint8_t key[24];
    uint8_t blksize = 0;

    if ( (inlen < sizeof(struct PEDEncrDataHead)+2)
        || (inlen != COMBINE16(packet[1],packet[0])+2)) {
        TRACE("\r\nencdata LEN%d",inlen);
        return RTV_PED_ERR;
    }
    TRACE_BUF("encdata",packet,inlen);
    head = (struct PEDEncrDataHead *)(packet+2);
    if ( (iRet = ped_algr_check(head->algr)) != 0) {
        return iRet;
    }
    blksize = ped_algr_blk_size(head->algr);
    length = COMBINE16(head->len_h,head->len_l);
    if ( length == 0
         || length > 1024
         || length%blksize != 0) {
        TRACE("->L:%d",__LINE__);
        return RTV_PED_ERR_PARA;
    }
    if ((iRet = ped_key_type_check(head->type,head->index)) != 0) {
        TRACE("->L:%d",__LINE__);
        return iRet;
    }
    if (head->type != KEY_TYPE_DESK && head->type != KEY_TYPE_CURKEY) {
        TRACE("->L:%d",__LINE__);
        return RTV_PED_ERR_KEYTYPE;
    }
    if (head->scatter > 1 
        || head->rfu[0] != 0
        || head->rfu[1] != 0) {
        TRACE("->L:%d",__LINE__);
        return RTV_PED_ERR_PARA;
    }
    if ( (iRet = ped_key_read(KEY_TYPE_MK,sgPedSys.curr_index_mk,
                              head->type,head->index,&klen,key)) != 0) {
        TRACE("->L:%d",__LINE__);
        return iRet;
    }
    TRACE_BUF("key",key,klen);
    TRACE_BUF("plaintext",head->val,length);
    if ( (iRet = ped_crypt_ecb(head->algr,key,klen,length,head->val,output)) != 0) {
        TRACE("->L:%d",__LINE__);
        return iRet;
    }
    *outlen = length;
    return RTV_PED_SUCCE;
}

int ped_get_mac(uint32_t inlen,uint8_t *packet,uint16_t *outlen,uint8_t *output)
{
    struct PEDCalcuMACHead *head = (struct PEDCalcuMACHead *)packet;
    int iRet;
    uint16_t length;
    uint16_t klen;
    uint8_t key[24];
    uint8_t blksize = 0;
    uint8_t *iv = NULL;
    uint8_t mac[16];

    TRACE_BUF("indata",packet,inlen);
    if ( (inlen < sizeof(struct PEDCalcuMACHead )+2)
        || (inlen != COMBINE16(packet[1],packet[0])+2)) {
        TRACE("\r\nencdata LEN%d",inlen);
        return RTV_PED_ERR;
    }
    head = (struct PEDCalcuMACHead *)(packet+2);
    if ( (iRet = ped_algr_check(head->algr)) != 0) {
        return iRet;
    }
    blksize = ped_algr_blk_size(head->algr);
    length = COMBINE16(head->len_h,head->len_l);
    if ( length > 1024
         || length == 0
         || length%blksize != 0) {
        return RTV_PED_ERR_PARA;
    }
    if ((iRet = ped_key_type_check(head->type,head->index)) != 0) {
        return iRet;
    }
    if (head->type != KEY_TYPE_MACK && head->type != KEY_TYPE_CURKEY) {
        return RTV_PED_ERR_KEYTYPE;
    }
    if (head->scatter > 1 
        || head->mode > 2
        || head->flag > 1
        || (head->ivlen != 0 && head->ivlen != blksize)
        || head->rfu[0] != 0
        || head->rfu[1] != 0
        || head->rfu[2] != 0) {
        return RTV_PED_ERR_PARA;
    }
    if ( (iRet = ped_key_read(KEY_TYPE_MK,sgPedSys.curr_index_mk,
                              head->type,head->index,&klen,key)) != 0) {
        return iRet;
    }
    TRACE_BUF("key",key,klen);
    iv = NULL;
    switch ( head->mode )
    {
    case 0: //x99
        if (head->ivlen != 0) {
            iv = head->val;
        }
        TRACE("\r\n X99");
        iRet = ped_calcu_mac_x99(head->algr,iv,klen,key,length,head->val+head->ivlen,output);
        break;
    case 1 ://ECB 
        if (head->ivlen != 0) {
            iv = head->val;
        }
        TRACE("\r\n ECB : %d-%d",head->ivlen,length);
        iRet = ped_calcu_mac_ecb0(head->algr,iv,klen,key,length,head->val+head->ivlen,output);
        break;
    case 2: //x919
        TRACE("\r\n X919 : %d-%d-%d",head->ivlen,length,head->flag);
        TRACE_BUF("data",head->val+head->ivlen,length);
        CLRBUF(mac);
        if (head->ivlen != 0) {
            iv = head->val;
        } else {
            iv = mac;
        }
        iRet = ped_calcu_mac_x919(head->algr,iv,blksize,key,length,head->val+head->ivlen,mac);
        TRACE_BUF("mac0",mac,blksize);
        //Flag：1-还有后续块 0-最后1块 
        if (head->flag == 0)  //last data block
        {
            if (klen == 2*blksize)
            {
                ped_crypt_ecb((head->algr+0x01),key+blksize,blksize,blksize,mac,mac);
                ped_crypt_ecb(head->algr,key,blksize,blksize,mac,mac);
            } else if (klen = 3*blksize)
            {
                ped_crypt_ecb((head->algr+0x01),key+blksize,blksize,blksize,mac,mac);
                ped_crypt_ecb(head->algr,key+2*blksize,blksize,blksize,mac,mac);
            }
        } 
        TRACE_BUF("mac",mac,blksize);
        memcpy(output,mac,blksize);
        CLRBUF(mac);
        break;
    default :
        break;
    }
    *outlen = blksize;
    return RTV_PED_SUCCE;
}

int ped_select_key(uint32_t inlen, uint8_t *input)
{
    struct PEDSelectKeyCHead *head = (struct PEDSelectKeyCHead *)input;
    int iRet;
    uint16_t mode;

    mode = COMBINE16(head->mode_h,head->mode_l);
    if (mode == 0)
    {
        if ((iRet = ped_key_type_check(head->type,head->index)) != 0) {
            return iRet;
        }
        if (head->type == KEY_TYPE_MK)
        {
            sgPedSys.curr_index_mk = head->index;
        } else if (head->type >= KEY_TYPE_PINK && head->type <= KEY_TYPE_DESK)
        {
            sgPedSys.curr_type_wk = head->type;
            sgPedSys.curr_index_wk = head->index;
        } else 
        {
            iRet = RTV_PED_ERR_KEYTYPE;
        }
    } else if (mode == 1)
    {
        iRet = RTV_PED_ERR;
    } else 
    {
        iRet = RTV_PED_ERR_PARA;
    }
    return iRet;
}

int ped_extern_main(uint32_t inlen,uint8_t *input,uint16_t *outlen,uint8_t *output)
{

    *outlen = 0;
    return RTV_PED_SUCCE;
}

int ped_parse_cmd(uint32_t inlen,uint8_t *input,uint32_t *outlen,uint8_t *output)
{
    int iRet;
    uint16_t length=0;

    TRACE_BUF("ped input",input,inlen);
    inlen--;
    switch (input[0])
    {
    case PED_CMD_OPEN:
        iRet = ped_open(0);
        break;
    case PED_CMD_CLOSE:
        iRet = ped_close(0);
        break;
    case PED_CMD_LOADMK:
        iRet = ped_load_mk(inlen,input+1);
        break;
    case PED_CMD_LOADWK:
        iRet = ped_load_wk(inlen,input+1);
        break;
    case PED_CMD_RANDOM:
        iRet = ped_get_rand(inlen,input+1,&length,output);
        break;
    case PED_CMD_ONLINE:
        iRet = ped_get_pin_online(inlen,input+1,&length,output);
        break;
    case PED_CMD_OFFLINE:
        iRet = ped_get_pin_offline(inlen,input+1,&length,output);
        break;
    case PED_CMD_ENCRDATA:
        iRet = ped_get_encrpt_data(inlen,input+1,&length,output);
        break;
    case PED_CMD_MAC:
        iRet = ped_get_mac(inlen,input+1,&length,output);
        break;
    case PED_CMD_SELECTKEY:
        iRet = ped_select_key(inlen,input+1);
        break;
    case PED_CMD_EXTEND:
        iRet = ped_extern_main(inlen,input+1,&length,output);
        break;
    default:
        iRet = RTV_PED_ERR_CMD;
        break;
    }
    *outlen = length;
    return iRet;
}


