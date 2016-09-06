/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30_boot.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/17/2014 11:46:31 AM
 * Description        : 
 *******************************************************************************/

#ifndef __BOOT_H__
#define __BOOT_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "wp30_cfg.h"
#include "drv_inc.h"
#include "app_common.h"
#include "wp30_boot_dbg.h"
//------------------------------------------------------------------------------
//         Product Define
//------------------------------------------------------------------------------

#define CNL_BUFSIZE_B   (64+4)
#define DL_BUFSIZE_B    (DL_FRAME_SIZE+32)

typedef struct _BOOT_BUF
{
    uint8_t console[CNL_BUFSIZE_B];
    uint8_t download[DL_LOCAL_FRAME_SIZE+32];
    uint8_t usbbuffer[DL_LOCAL_FRAME_SIZE+32];
}WP30BOOT_BUF_DEF;

typedef struct _UPDATE_INFO
{
    int data_len;  //监控大小 单位字节
    int flag;      //升级标志 0xaa-有新的升级文件 0x55-不可升级
}update_info;

extern WP30BOOT_BUF_DEF gSysBuf_b;
extern const VERSION_INFO_NEW gBootVerInfo;

//extern funtion
void enter_ctrl(void);

#endif

