/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : sys_variate.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 10/12/2014 4:01:48 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
#include "wp30c_sys_variate.h"

WP30BufDefCtrl  gwp30SysBuf_c UPAREA;
WP30SysMemory   gwp30SysMemory UPAREA;
uint8_t *gucBuff = gwp30SysBuf_c.pub;   
volatile  __no_init uint8_t gCtrlTmp[4]@0x20000000;
#pragma required=gCtrlTmp

void wp30c_sys_variate_init(void)
{
    memset((char *)gCtrlTmp,0x00,sizeof(gCtrlTmp));
    memset(&gwp30SysMemory,0x00,sizeof(gwp30SysMemory));
}

