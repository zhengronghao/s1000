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

#ifndef __APPMANAGE__
#define __APPMANAGE__ 

// SHA1和SHA256,tag结构体 288B=272+16B=16+256+16B
typedef struct
{
	unsigned char version[16];   //
	unsigned char sha1[32];      //上电
	unsigned char mac1[16];
	unsigned char sha2[32];      //运行 目前无用无效
	unsigned char mac2[16];
	unsigned char reserved[160];
	unsigned char tag[16];       //"CHECK:SHA256"
}ST_taginfo;


#define TAG_GAPK_REPLACE        '0'
#define TAG_GAPK_ADD            '1'
#define TAG_GAPK_DEL_SAME       '2'
#define TAG_GAPK_DEL_ALL        '3'

#define TAG_FIRMWARE_SPKC0      "BA3_SPKC0"
#define TAG_FIRMWARE_GAPK_PKC   "BA3_GPKC0"
#define TAG_FIRMWARE_SCERTP0    "BA3_SECERTP0"
#define TAG_FIRMWARE_BOOT       "S1000_BOOT" 
#define TAG_FIRMWARE_BOOTIMG    "S1000_BOOTIMG" 
#define TAG_FIRMWARE_BOOTLOAD   "S1000_BOOTLOAD" 
#define TAG_FIRMWARE_SYSTEM     "S1000_SYSTEM" 
#define TAG_FIRMWARE_RECOVERY   "S1000_RECOVERY" 
#define TAG_FIRMWARE_MIDWARE    "S1000_MIDWARE" 
#define TAG_FIRMWARE_CTRL       "S1000_CTRL" 

#define TAG_PROJECTID_COMM       "START_ITEP_SMART_POS_0"
#define TAG_PROJECTID_START      "START_ITEP_SMART_POS_SAPK0"


int s_pubk_decrypt (unsigned int type, unsigned int uilen, unsigned char *in, unsigned char *out, unsigned int* outlen);
int app_gapk_authenticate(int len);
int app_sapk_authenticate(int len);

#endif

