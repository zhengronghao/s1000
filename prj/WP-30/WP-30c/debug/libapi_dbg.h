/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : libapi_dbg.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 11/26/2014 5:43:12 PM
 * Description        : 
 *******************************************************************************/
#ifndef __LIBAPI_DBG_H__
#define __LIBAPI_DBG_H__ 
#include "wp30_ctrl.h"

#define CFG_LIBAPI_DBG_LCD
#define CFG_LIBAPI_DBG_MAG
#if (defined CFG_RFID)
#define CFG_LIBAPI_DBG_RFID
#endif


void dbg_libapi_main(void);


#endif

