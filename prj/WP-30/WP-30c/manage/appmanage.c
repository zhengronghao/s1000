/*
 * =====================================================================================
 *
 *       Filename:  authmanage.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  6/20/2016 6:07:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yehf (), yehf@itep.com.cn
 *        Company:  START
 *
 * =====================================================================================
 */


#include "wp30_ctrl.h"

#ifdef CFG_SE_MANAGE
/*-----------------------------------------------------------------------------}
 *  函数说明
 *-----------------------------------------------------------------------------{*/

/* 
 * s_read_pubk - [GENERIC] 读公钥
 *   type: 0-CA公钥 1-读厂商固件公钥 2-读防拆解除公钥 3-读实达应用公钥 4-读定制APK公钥
 *
 * @ 
 */
#define SAPK_PUBKEY_ADD SA_KEYZONE+((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*200)
uint s_save_sys_sapk(uchar *buf)
{
	if(eraseFlashPage(SAPK_PUBKEY_ADD/2))
	{
		return ERROR;
	}
	if(writeFlashPage(SAPK_PUBKEY_ADD/2,buf,sizeof(R_RSA_PUBLIC_KEY)))
	{
		return ERROR;
	}
	return OK;
}

uint s_get_sys_sapk(uchar *buf)
{
    readFlashPage(SAPK_PUBKEY_ADD/2, buf, sizeof(R_RSA_PUBLIC_KEY));
	return OK;
}

/* 
 * s_pubk_decrypt - [GENERIC] 使用公钥解密
 *     type: 0-CA公钥 1-读厂商固件公钥 2-读防拆解除公钥 3-读实达应用公钥 4-读定制APK公钥
 *     uilen:解密数据长度，必须小于公钥长度
 * @ 
 */
int s_pubk_decrypt (unsigned int type, unsigned int uilen, unsigned char *in, unsigned char *out, unsigned int* outlen)
{
    int ret,i;
    R_RSA_PUBLIC_KEY publickey;
    if ( uilen > 256 ) {
        return -1;
    }
    if ( type == 4 ) {
        s_get_sys_sapk((uchar *)(&publickey));
        TRACE_BUF("公钥",(uint8_t *)&publickey,sizeof(R_RSA_PUBLIC_KEY));
        //解密签名区
        TRACE_BUF("decrtpt data",in,uilen);
        ret = Rsa_calt_pub(uilen,in,&publickey,&i,out);
        TRACE_BUF("origin data",out,i);
        *outlen = i;
        memset(&publickey,0,sizeof(publickey));
        if ( ret ) {
            return -3;
        }
    } else {
        ret = s_startpubk_decrypt(type,uilen,in,out,outlen);
    }
    return ret;
}		/* -----  end of function s_pubk_decrypt  ----- */
// sha校验值判断
// return 0 成功
int s_Check_Sha(uint len, ST_newtaginfo *stag)
{
    uint8_t data0[512];
    uint16_t pagenum=0;
    SHA256Context sha;
    int i;
    int datalen=0;

// 计算sha 最后的304(32+256+16)字节不参与sha校验
    pagenum = (len-304+EXFLASH_PAGE_SIZE-1)/EXFLASH_PAGE_SIZE;
    TRACE("sha pagenum:%d\r\n", pagenum);
    sha256_init(&sha);
    for ( i=0 ; i<pagenum ; i++ ) {
        if ( i==(pagenum-1) ) {
            datalen = len-304-((pagenum-1)*EXFLASH_PAGE_SIZE); 
        }else{
            datalen = EXFLASH_PAGE_SIZE;     
        }
        exflash_read(EXFLASH_TMP_BUF_ADDR+EXFLASH_PAGE_SIZE*i, datalen, (uchar *)(&data0[0]));
        sha256_update(&sha, (uchar *)(&data0[0]), datalen);
    }
    sha256_final(&sha, data0);
    TRACE_BUF("sha", data0, 32);
    return s_check_firmware (0, data0 , (uchar *)stag);
}


/* 
 * Descript:读写gpkg sha在外置flash地址块偏移数据
 * 
 *  : 
 * */
extern uint16_t sys_manage_exflash_write(uint8_t mode,uint16_t number,uint16_t data_len,uint8_t* data);
uint16_t s_gapk_flash_write(uint16_t number, uint32_t data_len,uint8_t* data)
{
    return sys_manage_exflash_write(1, number+EXFLASH_GSHA_ADDR/EXFLASH_PAGE_SIZE, (uint16_t)data_len, data);
}
extern uint16_t sys_manage_exflash_read(uint8_t mode,uint16_t number,uint16_t data_len,uint8_t* data);
uint16_t s_gapk_flash_read(uint16_t number,uint32_t data_len,uint8_t* data)
{
    return sys_manage_exflash_read(1, number+EXFLASH_GSHA_ADDR/EXFLASH_PAGE_SIZE, (uint16_t)data_len, data);
}
/*
 * Descript: replace 覆盖白名单操作
 * 
 * */
uint32_t s_gapk_replace_sha(uint32_t len)
{
    uint16_t pagenum=0;
    int i;
    int shalen=0, datalen=0;
    uint32_t savenum=0;
    uint8_t data0[512];
    uint8_t data1[512];
    int ret;
    shalen = len-sizeof(ST_newtaginfo)-16;
    pagenum = (shalen + EXFLASH_PAGE_SIZE - 1)/EXFLASH_PAGE_SIZE;
    // 准备写入操作,以512为单位，最后一包填充有效数据
    for ( i=0 ; i<pagenum ; i++ ) {
        if ( i==(pagenum-1) ) {
            datalen = shalen-((pagenum-1)*EXFLASH_PAGE_SIZE); 
        }else{
            datalen = EXFLASH_PAGE_SIZE;
        }
        exflash_read(EXFLASH_TMP_BUF_ADDR+16+EXFLASH_PAGE_SIZE*i, datalen, (uchar *)(&data1[0]));
        s_DesRSAPubk(1, datalen, &data1[0], &data0[0]);
        TRACE_BUF("data0", data0, datalen);
        // 不调用exflash_write，需加密
        //            ret = sys_manage_exflash_write(1, i+EXFLASH_GSHA_ADDR/EXFLASH_PAGE_SIZE, (uint16)datalen, data0);
        ret = s_gapk_flash_write(i, datalen, data0);
        if ( ret != 0 ) {
            TRACE("write exflash err\r\n");
            return RET_WRITE_DEVICE; 
        }
        //            sys_manage_exflash_read(1,pagenum-1+EXFLASH_GSHA_ADDR/EXFLASH_PAGE_SIZE, datalen, data1);
        //            TRACE_BUF("data1", data1, datalen);
    }
    // 成功后写入标志
    //        savenum += optnum;
    savenum = shalen/32;
    TRACE("\ngpkg savenum:%d\n", savenum);
    return s_sysinfo_gpakshainfo(1, &savenum);
}
/*Descript:通过读取flash页,是否需要插入或删除的sha在读取的flash页中
 *return  : 0 无命中，非0 表示命中sha值所处位置
 * */
int s_page_hit_sha(uint8_t *shavalue, uint8_t *comparebuf, int len)
{
    int i; 
    int shanum;
    if ( len%32 != 0 ) {
        //  
        return -1;
    }
    shanum = len/32;
    for ( i=1 ; i<=shanum ; i++ ) {
        if ( memcmp(shavalue, &comparebuf[32*(i-1)], 32) ) {
            continue;
        }
        return i;
    }
    return 0;
}
/* Descript:
 * 在末尾偏移处增加sha数据；
 * 如果增加的sha未满足一页，暂不写入flash保存，而是返回待保存的数据待后续处理
 * 如果增加的sha满足一页，写入flash保存
 *
 * */
int s_gapk_sha_addtail(uint32_t exist_lastpage, uint32_t tail_off_num, uint8_t *shadata, uint8_t *savedata, uint32_t *savelen)
{
    uint16_t savepage;
    uint32_t exist_shanum=0;    // 已经保存在系统里的sha值数目
    int exist_datalen=0;
    savepage = (tail_off_num+15) / 16; 
    if ( savepage > 16 ) {
       TRACE("\r\nexceed the max store\r\n");
       return -1;
    }
    // 说明此前数据已经保存
    if ( savepage != exist_lastpage ) {
       TRACE("exist_lastpage is not same as savepage\r\n");
       // 保存一组sha
       memcpy(&savedata[*savelen], shadata, 32); 
       *savelen += 32;
    }else {
    // 当前保存的数据位置为同一页
       if ( 0 == *savelen ) {
           // 读取最后页已经保存的数据
           s_sysinfo_gpakshainfo(0, &exist_shanum);
           TRACE("\r\n&&&&&&:%d\r\n", exist_shanum);
           exist_datalen = (exist_shanum*32)%EXFLASH_PAGE_SIZE;
           TRACE("&&&&&&exist_datalen:%d\r\n", exist_datalen);
           s_gapk_flash_read(exist_lastpage-1, exist_datalen, savedata);
           memcpy(&savedata[exist_datalen], shadata, 32); 
           *savelen = exist_datalen+32;
       }else{
           memcpy(&savedata[*savelen], shadata, 32); 
           *savelen += 32;
       }
    }
    // 如果数据填充正好够满一页，写入页保存数据
    if ( *savelen == EXFLASH_PAGE_SIZE ) {
        s_gapk_flash_write(savepage-1, EXFLASH_PAGE_SIZE, savedata);
        memset(savedata, 0, 512);
        *savelen = 0;
    }
    TRACE("save len:%d\r\n", *savelen);
    return 0;
}
/* Descript:
 * 处理未写入flash的数据；
 * 如果增加的sha未满足一页，暂不写入flash保存，而是返回待保存的数据待后续处理
 * 如果增加的sha满足一页，写入flash保存
 *
 * */
int s_gapk_sha_dealtail( uint32_t tail_off_num, uint8_t *savedata, uint32_t *savelen)
{
    uint16_t savepage;
    savepage = (tail_off_num+15) / 16;
    if ( *savelen != 0 ) {
        s_gapk_flash_write(savepage-1, *savelen, savedata);
    }
    return 0;
}
uint32_t s_gapk_add_sha(uint32_t len)
{
    int ret;
    int i,j,z;
    uint8_t data0[512];
    uint8_t data1[512];
    uint8_t exist_data[512];
    uint8_t save_data[512];
    uint16_t pagenum=0, exist_pagenum;
    uint32_t exist_shanum=0;    // 已经保存在系统里的sha值
    uint32_t add_shanum=0;      // 增加的sha值
    uint32_t savelen=0;
    int shalen=0, datalen=0, exist_datalen=0;
    shalen = len-sizeof(ST_newtaginfo)-16;
    pagenum = (shalen + EXFLASH_PAGE_SIZE - 1)/EXFLASH_PAGE_SIZE;
    s_sysinfo_gpakshainfo(0, &exist_shanum);
    // 如果没有检测到有下载过gpkg包，不需要比较直接写入
    if ( (exist_shanum == 0) || (exist_shanum == 0xffffffff) ) {
       return s_gapk_replace_sha(len); 
    }
    exist_pagenum = (exist_shanum + 15)/16;  
    TRACE("\r\n@get the pagenum:%d\r\n", pagenum);
    for ( i=0 ; i<pagenum ; i++ ) {
        if ( i==(pagenum-1) ) {
            datalen = shalen-((pagenum-1)*EXFLASH_PAGE_SIZE); 
        }else{
            datalen = EXFLASH_PAGE_SIZE;
        }
        TRACE("@@@@@@@@@get the datalen:%d\r\n", datalen);
        exflash_read(EXFLASH_TMP_BUF_ADDR+16+EXFLASH_PAGE_SIZE*i, datalen, (uchar *)(&data1[0]));
        s_DesRSAPubk(1, datalen, &data1[0], &data0[0]);
//        TRACE_BUF("data0", data0, datalen);
        // 比较判断是否添加sha
        for ( j=0 ; j<datalen/32 ; j++ ) {
            for ( z=0 ; z<exist_pagenum; z++ ) {
                if ( z==(exist_pagenum-1) ) {
                    exist_datalen = (exist_shanum*32)%EXFLASH_PAGE_SIZE ; 
                    if ( 0 == exist_datalen ) {
                        exist_datalen = EXFLASH_PAGE_SIZE;
                    }
                }else{
                    exist_datalen = EXFLASH_PAGE_SIZE;
                }
                memset( exist_data, 0, sizeof(exist_data) );
                s_gapk_flash_read(z, exist_datalen, exist_data);
                ret = s_page_hit_sha(&data0[j*32], exist_data, exist_datalen);  
                // ret >0 说明命中，舍弃
                if ( ret > 0 ) {
                    TRACE("*****hit the data:%d\r\n", i*16+j+1);
                    break;     
                }
            }
            // 没有命中需要向原存放的尾部增加记录
            if ( z == exist_pagenum ) {
                add_shanum ++;
//                TRACE("*****add_shanum:%d\r\n", add_shanum);
                ret = s_gapk_sha_addtail(exist_pagenum, add_shanum + exist_shanum, &data0[j*32], save_data, &savelen);
                if ( ret != 0 ) {
                    return -1;
                }
                TRACE_BUF("savedata", save_data, savelen);
            }
        }
    }
    // 处理未写入flash进行保存的数据
    s_gapk_sha_dealtail(add_shanum + exist_shanum, save_data, &savelen);
    // 操作完成写入当前保存的有效sha列表值
    exist_shanum = exist_shanum + add_shanum;
    TRACE("add_shanum:%d, exist_shanum:%d\r\n", add_shanum, exist_shanum);
    s_sysinfo_gpakshainfo(1, &exist_shanum);
    return 0;
}
/* Descript:
 * 删除指定位置数据，并把存在最末尾处sha值拷贝删除位置进行填充；
 * parrm: delete_shanum(1~256)    existlast_shanum(1~256)
 * */
int s_gapk_sha_deletail(uint32_t delete_shanum, uint32_t existlast_shanum)
{
    uint32_t deletepos_page;
    uint32_t existpos_page;
    uint8_t data_delete[512];
    uint8_t data_existlast[512];
    if ( existlast_shanum <= delete_shanum ) {
        // 最后一个sha即为需要删除的sha
        TRACE("param err or del sha is the last\r\n"); 
        return -1;
    }
    deletepos_page = (delete_shanum+15) / 16;   
    existpos_page = (existlast_shanum+15) / 16;   
    memset(data_delete, 0, sizeof(data_delete));
    memset(data_existlast, 0, sizeof(data_existlast));
    //读取删除页数据
    s_gapk_flash_read(deletepos_page-1, EXFLASH_PAGE_SIZE, data_delete);
    // 在同一页只要读取页继而通过拷贝替换的方式结束操作
    TRACE("delete_shanum:%d, existlast_shanum:%d\r\n", delete_shanum, existlast_shanum);
    if ( deletepos_page == existpos_page ) {
        memcpy(&data_delete[((delete_shanum-1)%16)*32],&data_delete[((existlast_shanum-1)%16)*32], 32);
    }else{
        // 不在同一页，读取尾页，继而通过拷贝最后一条数据替换需要删除的sha结束操作
        s_gapk_flash_read(existpos_page-1, 512, data_existlast);
        memcpy(&data_delete[((delete_shanum-1)%16)*32],&data_existlast[((existlast_shanum-1)%16)*32], 32);
//        memcpy(&data_delete[(delete_shanum%32-1)*32],&data_existlast[(existlast_shanum-1)*32], 32);
    }
    return s_gapk_flash_write(deletepos_page-1, 512, data_delete);
}
uint32_t s_gapk_delete_sha(uint32_t len)
{
    int ret;
    int i,j,z;
    uint8_t data0[512];
    uint8_t data1[512];
    uint8_t exist_data[512];
    uint16_t pagenum=0, exist_pagenum;
    uint32_t exist_shanum=0;    // 已经保存在系统里的sha值
    int shalen=0, datalen=0, exist_datalen=0;
    shalen = len-sizeof(ST_newtaginfo)-16;
    pagenum = (shalen + EXFLASH_PAGE_SIZE - 1)/EXFLASH_PAGE_SIZE;
    s_sysinfo_gpakshainfo(0, &exist_shanum);
    // 如果没有检测到有下载过gpkg包,直接返回
    TRACE("get the pagenum:%d\r\n", pagenum);
    if ( (exist_shanum == 0) || (exist_shanum == 0xffffffff) ) {
       return 0; 
    }
    for ( i=0 ; i<pagenum ; i++ ) {
        if ( i==(pagenum-1) ) {
            datalen = shalen-((pagenum-1)*EXFLASH_PAGE_SIZE); 
        }else{
            datalen = EXFLASH_PAGE_SIZE;
        }
        exflash_read(EXFLASH_TMP_BUF_ADDR+16+EXFLASH_PAGE_SIZE*i, datalen, (uchar *)(&data1[0]));
        s_DesRSAPubk(1, datalen, &data1[0], &data0[0]);
        TRACE_BUF("data0", data0, datalen);
        for ( j=0 ; j<datalen/32 ; j++ ) {
            exist_pagenum = (exist_shanum +15)/16;  
            for ( z=0 ; z<exist_pagenum; z++ ) {
                if ( z==(exist_pagenum-1) ) {
                    exist_datalen = (exist_shanum*32)%EXFLASH_PAGE_SIZE ; 
                    if ( 0 == exist_datalen ) {
                        exist_datalen = EXFLASH_PAGE_SIZE;
                    }
//                    TRACE("\r\nreadlen:%d", exist_datalen);
                }else{
                    exist_datalen = EXFLASH_PAGE_SIZE;
                }
                memset( exist_data, 0, sizeof(exist_data) );
                s_gapk_flash_read(z, exist_datalen, exist_data);
//                TRACE("z:%d, exist_datalen:%d\r\n", z,exist_datalen);
//                TRACE_BUF("compare origin data", &data0[j*32], 32);
                ret = s_page_hit_sha(&data0[j*32], exist_data, exist_datalen);  
                if ( ret > 0 ) {
                    // 说明命中，准备删除，并且通过最后一个有效数据覆盖
                    TRACE("\r\n*****hit the data:%d\r\n", z*16+ret);
                    s_gapk_sha_deletail(z*16+ret, exist_shanum);
                    exist_shanum --;
                    break;     
                }
            }
        }
        // 写入当前剩余的sha个数
        s_sysinfo_gpakshainfo(1, &exist_shanum);
    }
    return 0;
}
/* 
 * app_gapk_authenticate - 
 *  Descript: GAPK authenticate and save the legal data in the spiflash
 *  param： 传入的长度，为exflash缓冲区的长度值，数据读取起始地址统一为exflash地址
 *  sha:SHA值
 *  tag:ST_newtaginfo结构体 
 * @ 
域	   长度	            说明
VER     4       默认为0
TYPE	4	    SHA类型，默认0-SHA256
NUM	    4	    后续公钥证书SHA值数量，支持多GAPK公钥证书
SHA1    32	    根据SHA类型确定长度，一般为32	GAPK1公钥证书SHA值
SHA2	32      根据SHA类型确定长度，一般为32	GAPK2公钥证书SHA值
….		…..
SHAn	32      根据SHA类型确定长度，一般为32	GAPKn公钥证书SHA值
TAG	    512	    签名信息区，关键字“ GAPK_PKC0”
注意：issuseindex域区别
1. issuseindex=‘0’:表示替换所有公钥证书
2. issuseindex=‘1’:表示增加公钥证书
3. issuseindex=‘2’:表示删除指定公钥证书
4. issuseindex=‘3’表示清空所有公钥证书
*/
int app_gapk_authenticate(int len)
{
    int ret; 
    ST_newtaginfo stag;
//    SHA256Context sha;
    uint8_t data0[512];
    uint8_t data1[512];
    uint8_t project_id[32];
    uint32_t savenum=0; 
//    uint32_t optnum=0, savenum=0; 
//    uint16_t pagenum=0;
//    int shalen=0, datalen=0;
    int shalen=0;
    int i;
    
    // 至少要满足ver+type+num+rfu+tag
    if ( len < sizeof(ST_newtaginfo)+16 ) {
        TRACE("len err\r\n");
        return RET_PARAM;
    }
    // 必须要满足sha组数的倍数 
    shalen = len-sizeof(ST_newtaginfo)-16;
    if ( (shalen%32) != 0 ) {
        TRACE("len err sha not 32 mod %d\r\n", len);
        return RET_PARAM;
    }
    // 取出TAG区域 get the tag
    memset(data0, 0, sizeof(data0));
    memset(data1, 0, sizeof(data1));
    exflash_read(EXFLASH_TMP_BUF_ADDR+len-sizeof(ST_newtaginfo), sizeof(ST_newtaginfo), (uint8_t *)(&stag.version[0]));
//    ptag = (ST_newtaginfo *)(&data[0]);

//    if ( memcmp(s_ptag->version, "GAPK_PKC", strlen("GAPK_PKC")) ) {
    if ( memcmp(&stag.version[0], TAG_FIRMWARE_GAPK_PKC, strlen(TAG_FIRMWARE_GAPK_PKC)) ) {
        TRACE("version err :%s\r\n", &stag.version[0]);
        return RET_PARAM;
    }
    
//  比较项目ID为合法ID ：START_ITEP_SMART_POS_0	通用项目ID 
//                       START_ITEP_SMART_POS_SAPK0	实达项目ID    
//                       其他	定制项目ID
    if ( memcmp(&stag.issue[0], TAG_PROJECTID_COMM, strlen(TAG_PROJECTID_COMM)) ) {
        memset(project_id, 0, sizeof(project_id));
        ret = (uint8_t)s_read_syszone(SYS_PROJIECT_ID_ADDR, SYS_PROJIECT_ID_SIZE, project_id);
        for ( i=0 ; i<32 ; i++ ) {
            if ( project_id[i] == 0xff  ) {
                continue; 
            }  
            break;
        }
        // 从未写入过项目ID
        if ( i==32 ) {
            TRACE("not be writern project id\r\n");
            return RET_ILLEAGLE_SERVICE;
        }
        if ( memcmp(&stag.issue[0], (char *)project_id, strlen((char *)project_id))) {
            TRACE("not correct project id\r\n");
            return RET_ILLEAGLE_SERVICE;
        }
        TRACE("get the project id:%s\r\n", project_id);
    }
    TRACE("download the project id:%s\r\n", &stag.issue[0]);
    
    // 计算sha 最后的304(32+256+16)字节不参与sha校验
    ret = s_Check_Sha(len, &stag);
    if ( ret != 0 ) {
        TRACE("check tag err\r\n");
        return RET_UNPACK_LEN;
    }

    // 解密数据区域
    exflash_read(EXFLASH_TMP_BUF_ADDR, 16, (uchar *)(&data1[0]));

    s_DesRSAPubk(1, 16, &data1[0], &data0[0]);
//    optnum = data0[8]|(data0[9]<<8)|(data0[10]<<16)|(data0[11]<<24); 
    TRACE("get the operate num:%d\r\n", data0[8]|(data0[9]<<8)|(data0[10]<<16)|(data0[11]<<24));
    // 保存数据开始, 根据不同操作进行数据保存
    switch ( stag.issuseindex )
    {
    case  TAG_GAPK_REPLACE:
        TRACE("\r\n********TAG_GAPK_REPLACE**********\r\n");
        ret = s_gapk_replace_sha(len);
        break;
    case  TAG_GAPK_ADD:
        TRACE("\r\n********TAG_GAPK_ADD**********\r\n");
        ret = s_gapk_add_sha(len);
        break;
    case  TAG_GAPK_DEL_SAME:
        TRACE("\r\n********TAG_GAPK_DEL_SAME**********\r\n");
        ret = s_gapk_delete_sha(len);
        break;
    case  TAG_GAPK_DEL_ALL:
        TRACE("\r\n********TAG_GAPK_DEL_ALL**********\r\n");
        savenum = 0;
        s_sysinfo_gpakshainfo(1, &savenum);
        break;
    default :
        return RET_PARAM;
    }
    return ret; 
}

/* 
 * app_gapk_authenticate - 
 *  Descript: GAPK authenticate and save the legal data in the spiflash
 *  sha:SHA值
 *  tag:ST_newtaginfo结构体 
 * @ 
域	   长度	            说明
VER	    4	            版本，默认0
TYPE	4	            预留，默认0
NUM	    4	            后续公钥证书数量，目前只支持1
RFU	    4	            预留
项目1公钥	            公钥1结构体	厂商1公钥
项目1 ID	32B	        项目ID
项目n公钥	            公钥n结构体	厂商n公钥
项目n ID	32B	        项目ID
TAG	512	签名信息区，关键字" PKC0”
*/
//typedef struct
//{
//    uint32_t modlen;         // 公钥模长度
//    uint8_t mod[256];        // 公钥模
//    uint8_t pubindex;        // 公钥指数
//}s_publickey;

typedef struct
{
    R_RSA_PUBLIC_KEY pubkey;             // 公钥
    uint8_t projectid[32];          // 项目ID
}s_pubinfo;
//typedef struct
//{
//    uint32_t version;       // 版本，默认0
//    uint32_t type;        //预留，默认0
//    uint32_t num;            //后续公钥证书数量，目前只支持1
////    s_pubinfo* ppubinfo;      //厂商公钥
//}s_sapk_head;

extern uint sys_write_project_id(uchar *id, int len);
int app_sapk_authenticate(int len)
{
    int ret; 
//    s_sapk_head *pspk_head;
//    s_pubinfo *ppubinfo;
//    uint8_t project_id[32];
    ST_newtaginfo stag;
//    SHA256Context sha;
    uint8_t data0[512];
    uint8_t data1[512];
    uint32_t optnum=0 ; 
//    uint16_t pagenum=0;
//    int pubkeylen, datalen=0;;
    int pubkeylen;
//    int i;

    if ( len < sizeof(ST_newtaginfo)+312 ) {
        TRACE("len err\r\n");
        return RET_PARAM;
    }
    // 必须要满足pubkey组数的倍数 
    pubkeylen = len-sizeof(ST_newtaginfo)-16;
    if ( (pubkeylen%sizeof(s_pubinfo)) != 0 ) {
        TRACE("len err pubinfo not correct mod\r\n");
        return RET_PARAM;
    }
    // 取出TAG区域 get the tag
    memset(data0, 0, sizeof(data0));
    memset(data1, 0, sizeof(data1));
    exflash_read(EXFLASH_TMP_BUF_ADDR+len-sizeof(ST_newtaginfo), sizeof(ST_newtaginfo), (uint8_t *)(&stag.version[0]));
//    ptag = (ST_newtaginfo *)(&data[0]);
//    if ( memcmp(s_ptag->version, "GAPK_PKC", strlen("GAPK_PKC")) ) {
    if ( memcmp(&stag.version[0], TAG_FIRMWARE_SPKC0, strlen(TAG_FIRMWARE_SPKC0)) ) {
        TRACE("version err :%s\r\n", &stag.version[0]);
        return RET_PARAM;
    }

    ret = s_check_tag (0,&stag , stag.sha);
    if ( ret != 0 ) {
        TRACE("check tag err\r\n");
        return RET_UNPACK_LEN;
    }
// 计算sha 最后的304(32+256+16)字节不参与sha校验
    ret = s_Check_Sha(len, &stag);
    if ( ret != 0 ) {
        TRACE("check tag err\r\n");
        return RET_UNPACK_LEN;
    }
//    pagenum = (len-304+EXFLASH_PAGE_SIZE-1)/EXFLASH_PAGE_SIZE;
//    TRACE("sha pagenum:%d\r\n", pagenum);
//    sha256_init(&sha);
//    for ( i=0 ; i<pagenum ; i++ ) {
//        if ( i==(pagenum-1) ) {
//            datalen = len-304-((pagenum-1)*EXFLASH_PAGE_SIZE); 
//        }else{
//            datalen = EXFLASH_PAGE_SIZE;     
//        }
//        exflash_read(EXFLASH_TMP_BUF_ADDR+EXFLASH_PAGE_SIZE*i, datalen, (uchar *)(&data0[0]));
//        sha256_update(&sha, (uchar *)(&data0[0]), datalen);
//    }
//    sha256_final(&sha, data0);
//    TRACE_BUF("sha", data0, 32);
//    ret = s_check_firmware (0xff, data0 , (uchar *)&stag);
//    if ( ret != 0 ) {
//        TRACE("check tag err\r\n");
//        return RET_UNPACK_LEN;
//    }

    // 解密数据区域
    exflash_read(EXFLASH_TMP_BUF_ADDR, 16, (uchar *)(&data1[0]));

    s_DesRSAPubk(1, 16, &data1[0], &data0[0]);
//    TRACE_BUF("head en", data1, 16);
//    TRACE_BUF("head de", data0, 16);
    optnum = data0[8]|(data0[9]<<8)|(data0[10]<<16)|(data0[11]<<24); 
    TRACE("get the operate num:%d\r\n", optnum);
    // not allow bypass the 32 
    if ( optnum > 32 ) {
        return RET_PARAM; 
    }
/////////////////////////////////目前均允许写项目ID/////////////////////////////
    // 说明从未写入项目ID, 取得第一组为项目ID,并保存本组项目ID 
    exflash_read(EXFLASH_TMP_BUF_ADDR+16, sizeof(s_pubinfo), (uchar *)(&data1[0]));
    //        TRACE_BUF("data1", data1, 296);
    s_DesRSAPubk(1, sizeof(s_pubinfo), &data1[0], &data0[0]);
    //        TRACE_BUF("data0", data0, 296);
    ret = s_save_sys_sapk(data0);
    if ( ret ) {
        TRACE("save sapk err\r\n");
    }
    //        s_get_sys_sapk(data1);
    //        TRACE_BUF("data1", data1, 296);
    // 保存项目ID
    ret = sys_write_project_id(&data0[FPOS(s_pubinfo, projectid)], 32);
    if ( ret ) {
        TRACE("sys_write_project_id err\r\n");
    }
///////////////////////////////////////////////////////////////////////////////
    /*
    // judge project ID  
    ret = (uint8_t)s_read_syszone(SYS_PROJIECT_ID_ADDR, SYS_PROJIECT_ID_SIZE, project_id);
    TRACE_BUF("ID", project_id, 32);
    for ( i=0 ; i<32 ; i++ ) {
       if ( project_id[i] == 0xff  ) {
           continue; 
       }  
       break;
    }
    TRACE("i:%d\r\n", i);
    if ( i==32 ) {
        // 说明从未写入项目ID, 取得第一组为项目ID,并保存本组项目ID 
        exflash_read(EXFLASH_TMP_BUF_ADDR+16, sizeof(s_pubinfo), (uchar *)(&data1[0]));
//        TRACE_BUF("data1", data1, 296);
        s_DesRSAPubk(1, sizeof(s_pubinfo), &data1[0], &data0[0]);
//        TRACE_BUF("data0", data0, 296);
        ret = s_save_sys_sapk(data0);
        if ( ret ) {
            TRACE("save sapk err\r\n");
        }
//        s_get_sys_sapk(data1);
//        TRACE_BUF("data1", data1, 296);
        // 保存项目ID
        ret = sys_write_project_id(&data0[FPOS(s_pubinfo, projectid)], 32);
        if ( ret ) {
            TRACE("sys_write_project_id err\r\n");
        }
    }else{
        for ( i=0; i<optnum ; i++ ) {
            exflash_read(EXFLASH_TMP_BUF_ADDR+16+sizeof(s_pubinfo)*i, sizeof(s_pubinfo), (uchar *)(&data1[0]));
            s_DesRSAPubk(1, sizeof(s_pubinfo), &data1[0], &data0[0]);
            if ( memcmp(&data0[FPOS(s_pubinfo,projectid)], project_id, 32) ) {
                continue;
            }
            // 匹配项目ID, 保存数据  
            ret = s_save_sys_sapk(data0);
            if ( ret ) {
                TRACE("save sapk err\r\n");
            }
            break;
        }
        // 判断是否匹配 返回未找到对应数据
        if ( i==optnum ) {
            TRACE("can not find correct project id\r\n");
            return RET_NOT_FOUND;
        }
    }
*/    
    return ret;
}

/* 
 * app_gapk_authenticate -
 *  Descript: 公钥解密，根据公钥数据 
 *  sha:SHA值
 *  tag:ST_newtaginfo结构体 
 * @ 
域	   长度	            说明
VER	    4	            版本，默认0
TYPE	4	            预留，默认0
NUM	    4	            后续公钥证书数量，目前只支持1
项目1公钥	            公钥1结构体	厂商1公钥
项目1 ID	32B	        项目ID
项目n公钥	            公钥n结构体	厂商n公钥
项目n ID	32B	        项目ID
TAG	512	签名信息区，关键字" PKC0”
*/

#endif


