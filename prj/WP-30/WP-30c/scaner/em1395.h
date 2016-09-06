/*
 * =====================================================================================
 *
 *       Filename:  em1095.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  5/20/2016 10:31:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#ifndef __COM_em1095_H
#define __COM_em1095_H
#if 0
//typedef struct _SCANER_EM1095{
//    short   prefix;         // 两个字节的前导 
//    char    types;          // 命令类型 
//    char    len;            // 数据长度,表示该命令中 Datas 的字节数，这里建议设置 0x01
//    short   address;        // 超时标志
//    char*   data;           // 数据
//    short   fsc;            // fsc校验
//}SCANER_EM1095;
#ifdef em1395_DEBUG
   #define em1395_print(s) printf(s)
#else
   #define em1395_print(s)
#endif

uint8_t com_em1395_open(void);

uint8_t com_em1395_close(void);

uint8_t com_em1395_write(uint8_t *data , uint32_t len);

uint32_t com_em1395_read(uint8_t *data , uint32_t time);

uint16_t com_em1395_scan_start(uint32_t timeout);

uint32_t com_em1395_read_result(uint8_t *data);

uint32_t com_em1395_scan(void);

void com_em1395_scan_cancel(void);

void em1395_trig_finish(void);

#endif


#endif
