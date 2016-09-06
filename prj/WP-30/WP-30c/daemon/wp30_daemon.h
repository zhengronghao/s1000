/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30_daemon.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 10/12/2014 3:54:56 PM
 * Description        : 
 *******************************************************************************/
#ifndef __WP30_DAEMON_H__
#define __WP30_DAEMON_H__


typedef void (*IRQ_CALLBACK)(void);
extern const IRQ_CALLBACK  SysTickCallBack[];
extern const IRQ_CALLBACK UartAppCallBack[];

#endif


