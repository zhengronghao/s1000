/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : map.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/13/2014 7:11:11 PM
 * Description        : 
 *******************************************************************************/

#ifndef __MAP_H__
#define __MAP_H__
//*******************************************************
//
//   空间分配
//
//*******************************************************
//*******************************************************
//   空间分配
//   BOOT 64K
//   CTRL 320K
//   KEY ZONE (200+20K)
//   RFU 92K
//   CTRL BACK (320K)
//   SYS INFO (4K)
//   SYS BACK INFO (4K)
//*******************************************************
#define BOOT_LEN            ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*64 )
#define CTRL_LEN            ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*320)
#define KEYZONE_LEN         ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*220) //2*4(MK)+48*4(WK) = 200
#define RFU_LEN             ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*92)
#define CTRL_BACK_LEN       ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*320) //FLASH_SECTOR_SIZE
#define SYSZONE_LEN         ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*4  ) //FLASH_SECTOR_SIZE
#define SYSZONE_BACK_LEN    ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*4  ) //FLASH_SECTOR_SIZE
typedef struct _FLASH_ROM
{
    uint8_t boot[BOOT_LEN];   // 64K
    uint8_t ctrl[CTRL_LEN];   // 320K 
    uint8_t keyzone[KEYZONE_LEN]; //220K (MK-210密钥区)
    uint8_t rfu[RFU_LEN]; // 92 (预留)
    uint8_t ctrl_back[CTRL_BACK_LEN]; //320K
    uint8_t syszone[SYSZONE_LEN]; //4K
    uint8_t syszone_bck[SYSZONE_BACK_LEN]; //4K
}FLASH_ROM;

#define SA_FLASH_BASE         0
// Flash空间分配
#define SA_BOOT          (SA_FLASH_BASE+FPOS(FLASH_ROM,boot))
	#define LEN_BOOT     FSIZE(FLASH_ROM,boot) 
#define SA_CTRL          (SA_FLASH_BASE+FPOS(FLASH_ROM,ctrl))
	#define LEN_CTRL     FSIZE(FLASH_ROM,ctrl) 
#define SA_KEYZONE       (SA_FLASH_BASE+FPOS(FLASH_ROM,keyzone))
	#define LEN_KEYZONE  FSIZE(FLASH_ROM,keyzone) 
#define SA_RFU           (SA_FLASH_BASE+FPOS(FLASH_ROM,rfu)) 
	#define LEN_RFU      FSIZE(FLASH_ROM,rfu)
#define SA_CTRL_BACK      (SA_FLASH_BASE+FPOS(FLASH_ROM,ctrl_back))
	#define LEN_CTRL_BACK FSIZE(FLASH_ROM,ctrl_back) 
//#define SA_ZK            (SA_FLASH_BASE+FPOS(FLASH_ROM,fontlab))  //字库2K
//	#define LEN_ZK       FSIZE(FLASH_ROM,fontlab) 
#define SA_SYSZONE       (SA_FLASH_BASE+FPOS(FLASH_ROM,syszone))  //系统信息区4K=2K+2K
	#define LEN_SYSZONE  FSIZE(FLASH_ROM,syszone) 
#define SA_SYSZONE_BACK  (SA_FLASH_BASE+FPOS(FLASH_ROM,syszone_bck)) 

	
///*-------------------------------SYS MAP--------------------------------*/
//#define BOOT_LEN            ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*64 )
//#define CTRL_LEN            ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*384)
//#define FILESYS_LEN         ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*32 )
//#define FONTLIB_LEN         ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*264)  //FLASH_SECTOR_SIZE
//#define RFU_LEN             ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*272)
//#define SYSZONE_LEN         ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*4   ) //FLASH_SECTOR_SIZE
//#define SYSZONE_BACK_LEN    ((FLASH_SECTOR_SIZE/FLASH_SECTOR_NbrOf1024)*4   ) //FLASH_SECTOR_SIZE
//typedef struct _FLASH_ROM
//{
//    uint8_t boot[BOOT_LEN];   // 64K
//    uint8_t ctrl[CTRL_LEN];   // 384K 
//    uint8_t filesys[FILESYS_LEN]; //32K (MK-210密钥区)
//    uint8_t fontlab[FONTLIB_LEN]; //148K
//    uint8_t rfu[RFU_LEN]; //12K-4 (预留)
//    uint8_t syszone[SYSZONE_LEN]; //4K
//    uint8_t syszone_bck[SYSZONE_BACK_LEN]; //4K
//}FLASH_ROM;
//
//#define SA_FLASH_BASE         0
//// Flash空间分配
//#define SA_BOOT          (SA_FLASH_BASE+FPOS(FLASH_ROM,boot))
//	#define LEN_BOOT     FSIZE(FLASH_ROM,boot) 
//#define SA_CTRL          (SA_FLASH_BASE+FPOS(FLASH_ROM,ctrl))
//	#define LEN_CTRL     FSIZE(FLASH_ROM,ctrl) 
//#define SA_APP          (SA_FLASH_BASE+FPOS(FLASH_ROM,app))
//	#define LEN_APP     FSIZE(FLASH_ROM,app) 
//#define SA_FILESYS      (SA_FLASH_BASE+FPOS(FLASH_ROM,filesys))
//	#define LEN_FILESYS FSIZE(FLASH_ROM,filesys) 
//#define SA_ZK            (SA_FLASH_BASE+FPOS(FLASH_ROM,fontlab))  //字库2K
//	#define LEN_ZK       FSIZE(FLASH_ROM,fontlab) 
//#define SA_SYSZONE       (SA_FLASH_BASE+FPOS(FLASH_ROM,syszone))  //系统信息区4K=2K+2K
//	#define LEN_SYSZONE  FSIZE(FLASH_ROM,syszone) 
//#define SA_SYSZONE_BACK  (SA_FLASH_BASE+FPOS(FLASH_ROM,syszone_bck)) 
	

//*******************************************************
//  SHA模式
//*******************************************************
#define LABLE_SYSZONE    "STAR"       
	#define LABLE_SYSZONE_LEN    (sizeof(LABLE_SYSZONE)-1)
#define LABLE_SHA1FLG    "CHECK_MODE:HASH"   
#define LABLE_SHA256FLG  "CHECK:SHA256"
#define LABLE_APPINFO    APP_INFO
	#define LABLE_APPINFO_LEN     (sizeof(LABLE_APPINFO)-1)
#define LABLE_BOOTINFO  "WP-30BOOT" 
	#define LABLE_BOOTINFO_LEN    (sizeof(LABLE_BOOTINFO)-1)
#define LABLE_NEEDDL     "NEEDDLV1" 

// SHA1和SHA256,tag结构体 288B=272+16B=16+256+16B
//typedef struct
//{
//	unsigned char version[16];   //
//	unsigned char sha1[32];      //上电
//	unsigned char mac1[16];
//	unsigned char sha2[32];      //运行 目前无用无效
//	unsigned char mac2[16];
//	unsigned char reserved[160];
//	unsigned char tag[16];       //"CHECK:SHA256"
//}ST_taginfo;
//// RSA1和RSA256,tag结构体 640B=624+16B=16+48+48+256+256+16B
//typedef struct
//{
//	unsigned char version[16];   //
//	unsigned char sha1[32];      //上电SHA校验码明文
//	unsigned char mac1[16];      //目前无效
//	unsigned char sha2[32];      //运行SHA校验码明文，目前无效
//	unsigned char mac2[16];      //目前无效
//	unsigned char RSA1[256];     //上电检测SHA校验码密文，用公钥加密
//	unsigned char RSA2[256];     //运行检测SHA校验码密文，用公钥加密，目前无效
//	unsigned char tag[16];       //"CHECK:RSA1"
//}ST_extaginfo;

#endif

