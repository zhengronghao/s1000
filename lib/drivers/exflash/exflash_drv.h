/*
 * =====================================================================================
 *
 *       Filename:  exflash_drv.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  5/17/2016 10:29:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#ifndef __EXFLASH_DRV_H__
#define __EXFLASH_DRV_H__

/**
    区号	起始地址	大小（字节）	说明
        0	0x0000	    4096	        配置区
        1	0x1000	    4096*2	应用原生签名授权的应用签名Hash采用SHA256，每段32字节，
                                每页可以存储16组，一共可以存储256组，即最大支持免验签256种预置应用签名
        2	0x3000	    4096	        实达预置签名公钥，暂定8组，1页1组。
        3	0x4000	    4096	        部分应用需要使用到的银联参数
        4	0x5000	    4096*4	        缓存区，用于SPI FLASH写操作时缓存区
        5	0x9000	    4096*503	    预留
 */
#define EXFLASH_SECTOR_SIZE     4096 
#define EXFLASH_PAGE_SIZE       512

#define EX_CFG_LEN              EXFLASH_SECTOR_SIZE
#define EX_GSHA_LEN             EXFLASH_SECTOR_SIZE*2
#define EX_START_PUB_LEN        EXFLASH_SECTOR_SIZE 
#define EX_YINLIAN_LEN          EXFLASH_SECTOR_SIZE 
#define EX_TMP_BUF_LEN          EXFLASH_SECTOR_SIZE*4 

typedef struct _EXFLASH_ROM
{
    uint8_t cfg[EX_CFG_LEN];            // 4096K
    uint8_t gsha[EX_GSHA_LEN];          //  
    uint8_t start_pub[EX_START_PUB_LEN];// (MK-210密钥区)
    uint8_t yilian[EX_YINLIAN_LEN];     // 92 (预留)
    uint8_t tmp_buf[EX_TMP_BUF_LEN];    //320K
}EXFLASH_R0M;

#define EXFLASH_CFGINFO_ADDR            (FPOS(EXFLASH_R0M,cfg))
#define EXFLASH_CFGINFO_SIZE            (FSIZE(EXFLASH_R0M,cfg))

#define EXFLASH_GSHA_ADDR               (FPOS(EXFLASH_R0M,gsha))
#define EXFLASH_GSHA_SIZE               (FSIZE(EXFLASH_R0M,gsha))

#define EXFLASH_START_PUB_ADDR          (FPOS(EXFLASH_R0M,start_pub))
#define EXFLASH_START_PUB_SIZE          (FSIZE(EXFLASH_R0M,start_pub))

#define EXFLASH_YINLIAN_ADDR            (FPOS(EXFLASH_R0M,yilian))
#define EXFLASH_YINLIAN_SIZE            (FSIZE(EXFLASH_R0M,yilian))

#define EXFLASH_TMP_BUF_ADDR            (FPOS(EXFLASH_R0M,tmp_buf))
#define EXFLASH_TMP_BUF_SIZE            (FSIZE(EXFLASH_R0M,tmp_buf))

unsigned int exflash_write(unsigned int addr,unsigned int data_len,unsigned char* data);

unsigned int exflash_read(unsigned int addr,unsigned int data_len,unsigned char* data);
#endif

