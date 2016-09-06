/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : console.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/10/2014 2:58:44 PM
 * Description        : 
 *******************************************************************************/

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <stdint.h>

char in_char (void);
void out_char (char ch);
int char_present (void);

int console_init(uint8_t comport,uint8_t *buffer,uint32_t buf_size);
int console_close(void);
int console_check_buffer(void);
int console_clear(void);
int console_read_buffer(uint8_t *output,uint32_t length,int32_t timeout_ms);
int console_check_used(void);
int console_get_class(void);

#endif

