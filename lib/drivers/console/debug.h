/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : debug.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/22/2014 4:41:51 PM
 * Description        : 
 *******************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

void assert_failed(char *, int);
void vDispBuf(uint32_t length, uint32_t mode, const uchar *const buffer);
uint32_t IfInkey(uint32_t mode);
int InkeyCount(int mode);
uint32_t InkeyHex(uint32_t Flag);
void vDispBufTitle(char *title, uint uiLen, uint Mode, void *p);
void TRACE_BUF(const char *title,const uint8_t *const buffer,uint32_t length);
void vDispLable(uint mode, uint num);
int InkeyStrToHex(char * outbuf);
int InkeyStr(char * outbuf);

#endif

