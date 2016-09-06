/*
 * =====================================================================================
 *
 *       Filename:  ctc_authent.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  6/30/2016 8:23:33 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#include "ctc_authent.h"
#include "ctc_sys.h"
#include "wp30_ctrl.h"

// 获取GAPK HASH请求包
uint8_t authent_get_gapkhash_req(void)
{		
    int num ;
    s_read_syszone(SYS_GAPKSHA_NUM_ADDR , 4, (uchar *)(&num));
    TRACE("get the sha num:%d\r\n", num);
    if ( num == 0xffffffff ) {
        return 0; 
    }
    return (num+(EXFLASH_PAGE_SIZE/32)-1)/(EXFLASH_PAGE_SIZE/32);
}

/*
 *Description: 获取gapk数据包数据，要求每包为存储页的一页即512bytes，最后一包为剩余页求模剩余数据
 *
 * */
uint16_t authent_get_gapkhash_num(uint8 num, uint8_t *outbuf, uint32_t *outlen)
{
    uint16_t ret ;
    uint8_t maxnum;
    uint32_t count;

    maxnum = authent_get_gapkhash_req();
    if ( num > maxnum || num < 1 ) {
       return CMDST_OK;
    }

    if ( num != maxnum ) {
        ret = sys_manage_exflash_read(1, num +EXFLASH_GSHA_ADDR/EXFLASH_PAGE_SIZE-1, EXFLASH_PAGE_SIZE, outbuf);

//            sys_manage_exflash_read(1, pagenum+EXFLASH_GSHA_ADDR/EXFLASH_PAGE_SIZE, datalen, data1);
        if ( ret  ) {
            TRACE("read exflash err\r\n"); 
            return CMDST_OTHER_ER;
        }
        *outlen = EXFLASH_PAGE_SIZE;
    }else{
        s_read_syszone(SYS_GAPKSHA_NUM_ADDR , 4, (uchar *)(&count));
        // the last data 
//        if ( count == 0 || count == 0xffffffff  ) {
//            *outlen = 0;
//            return CMDST_OK;
//        }
        *outlen = ((count-1)%(EXFLASH_PAGE_SIZE/32))*32 + 32; 
        ret = sys_manage_exflash_read(1, num +EXFLASH_GSHA_ADDR/EXFLASH_PAGE_SIZE-1, (uint16_t)*outlen, outbuf);
        TRACE("get the num id:%d, datalen:%d\r\n", num +EXFLASH_GSHA_ADDR/EXFLASH_PAGE_SIZE-1, *outlen);
        if ( ret ) {
            TRACE("read exflash err\r\n"); 
            return CMDST_OTHER_ER;
        }
    }
    TRACE_BUF("gapk", outbuf, *outlen);
    return CMDST_OK;
}

/**********************************************************************
 * Description: 鉴权文件，根据文件的鉴权后续操作
 * 说明：
ID	数据域	长度	备注
1	镜像id	1	
    1-modem     2-sbl1      3-sbl1bak   4-aboot     5-abootbak
    6-rpm       7-rpmbak    8-tz        9-tzbak     10-pad
    11-modemst1 12-modemst2 13-misc     14-fsc      15-ssd
    16-splash   17-DDR      18-fsg      19-sec      20-boot
    21-system   22-persist  23-cache    24-recovery 25-devinfo
    26-keystore 27-oem      28-config   29-userdata
    255-通用
2	HASH数据	32	SHA256值 
3	RFU	32	预留
4	tag	512	签名信息区
***********************************************************************/
uint16_t authent_sign_verifyimg(uint8_t *data, int len)
{
    //uint16_t ret;
    ST_AUTH_VERTIFYIMG *s_authentinfo;
//    ST_newtaginfo *ptag;

    if ( len != sizeof(ST_AUTH_VERTIFYIMG) ) {
       return CMDST_PARAM_ER;  
    }

    s_authentinfo = (ST_AUTH_VERTIFYIMG *)data; 

    // compare the sha value
    if ( !memcmp( s_authentinfo->hash, &s_authentinfo->tag.sha[0], 32) ) {
       return CMDST_SIGN_ER ;
    }
    // accroding the type and deal with the diff ways,set security level
    switch ( s_authentinfo->type )
    {
    case VERIFY_TYPE_ABOOT :
        break;
    case VERIFY_TYPE_BOOT :
        break;
    case VERIFY_TYPE_PERISIS :
        break;
    default :
        break;
    }
    
//    ptag = &data[len-sizeof(ST_newtaginfo)];
    return CMDST_OK;
}

uint16_t authent_pub_decrpt(uint8_t *input, int inlen, uint8_t* output)
{
    uint32_t result;
    uint32_t outlen;
    uint8_t project_id[32];
    int i;
    ST_AUTH_PUBDECT *s_authentinfo;

    s_authentinfo = (ST_AUTH_PUBDECT *)input;

    if ( inlen != sizeof(ST_AUTH_PUBDECT) ) {
       return CMDST_PARAM_ER;  
    }
    if ( s_authentinfo->type >= AUTH_PUBDECRYPT_TYPE_MAX ) {
       return CMDST_PARAM_ER;  
    }

    s_read_syszone(SYS_PROJIECT_ID_ADDR, SYS_PROJIECT_ID_SIZE, project_id);
    TRACE_BUF("decrypt", project_id, 32);
    for ( i=0 ; i<32 ; i++ ) {
       if ( project_id[i] == 0xff  ) {
           continue; 
       }  
       break;
    }
    TRACE("i:%d\r\n", i);
    if ( i==32 ) {
        // 说明无项目ID, 取得第一组为项目ID,并保存本组项目ID
        return CMDST_PUBKEY_FORMAT_ER;
    }

    if ( s_authentinfo->type == AUTH_PUBDECRYPT_TYPE_START ) {
        result = s_pubk_decrypt (1, 256, s_authentinfo->data, output, &outlen);
    }else if(s_authentinfo->type == AUTH_PUBDECRYPT_TYPE_USER){
        result = s_pubk_decrypt (4, 256, s_authentinfo->data, output, &outlen);
    }else{
       return CMDST_PARAM_ER;  
    }
    if ( result != 0 ) {
        TRACE("get the s_pubk_decrypt result:%d\r\n", result);
        return CMDST_OTHER_ER;  
    }
    return CMDST_OK;  
}

