/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : security_dbg.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/24/2014 4:35:29 PM
 * Description        : 
 *******************************************************************************/

#ifndef __SECURITY_DBG_H__
#define __SECURITY_DBG_H__ 

#define DBG_SHA1
#define DBG_SHA256

#ifdef  DBG_SHA1 
void sha1_self_test(void);
#endif

#ifdef  DBG_SHA256
void sha256_self_test(void);
#endif

#endif
