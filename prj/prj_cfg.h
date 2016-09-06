/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
* File Name          : prj_cfg.h
* Author             : luocs
* Version            : V0.00
* Date               : 2014.01.02
* Description        : Hardware pin assignments
*******************************************************************************/

#ifndef __PRJ_CFG_H__
#define __PRJ_CFG_H__ 

/* ========|Project specific header file|======= */
#ifdef MK210_V3
#include "mk210v3_cfg.h"
#endif

#ifdef WP30_V1 
#include "wp30_cfg.h"
#endif

/* ======|All projects shares header file|====== */
#include "cpu.h"
#include "wdog.h"
#include "mcg.h"
#include "rcm.h"
#include "smc.h"
#include "pmc.h"
#include "llwu.h"
#include "clock_cycle.h"
#include "systick_hw.h"
#include "flash_hw.h"
#include "uart_hw.h"
#include "gpio_hw.h"
#ifdef WP30_V1 
#include "./share/prj_share.h"
#endif
//add here

/*=======|All projects shares debug Exported macro|=======*/
#ifdef DEBUG_Dx 
extern char gDebugFlg;
int printk(const char *fmt, ...);
void assert_failed(char *, int);
int console_check_used(void);
#define ASSERT(expr)      if (!(expr)) assert_failed(__FILE__, __LINE__)
//#define TRACE(...)        do{if(console_check_used() == 0 && gDebugFlg)printk(__VA_ARGS__);}while(0)
#define TRACE(...)        do{if(console_check_used() == 0)printk(__VA_ARGS__);}while(0)
#define DISPPOS(x)	      do{TRACE("\r\n[POS:%s:%d %X-%d]",__func__,__LINE__,x,x);}while(0)  
#define DISPERR(x)        do{TRACE("\r\n[ERR:%s:%d %X-%d]",__func__,__LINE__,x,x);}while(0)
#define STOP(x)           do{TRACE("\r\n[STOP:%s:%d %X-%d]",__func__,__LINE__,x,x);KB_GetKey(-1);}while(0)
#define DISPBUF(title,len,mode,buf) vDispBufTitle(title,len,mode,buf)
#else
#define ASSERT(expr)
#define TRACE(...)
#define DISPPOS(x)	
#define DISPERR(x)  
#define STOP(x)     
#define DISPBUF(title,len,mode,buf)
#endif

#endif

