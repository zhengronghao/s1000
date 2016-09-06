/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30_boot_dbg.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/22/2014 11:17:28 AM
 * Description        : 
 *******************************************************************************/

#ifndef __WP30_BOOT_DBG_H__
#define __WP30_BOOT_DBG_H__ 

/*-----------------------------------------------------------------------------}
 *  boot层调试开关
 *-----------------------------------------------------------------------------{*/
#ifdef DEBUG_Dx
#define DEBUG_BOOT        // BOOT层调试开关宏
#endif

#ifdef DEBUG_BOOT
#define CFG_DBG_FLASH
//#define CFG_DBG_USB
#endif

void dbg_boot(void);

#endif

