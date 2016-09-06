/*
 * =====================================================================================
 *
 *       Filename:  ctc_tprinter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/8/2016 9:43:53 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#ifndef __CTC_TPRINTER_H__
#define __CTC_TPRINTER_H__ 

#include "ParseCmd.h"


uint16_t ctc_tprinter_open(uchar mode);

uint16_t ctc_tprinter_close(void);

uint16_t ctc_tprinter_line_set(uint32_t dot_len);

uint16_t ctc_tprinter_desity_set(uint8_t data);

uint16_t ctc_tprinter_feed_paper(uint32_t dot_len);

uint16_t ctc_tprinter_bitmap(uint16_t witdth,uint16_t high,uint16_t x_offset,uint16_t y_offset,uint8_t* data);

uint8_t ctc_tprinter_get_status(void);

uint16_t ctc_tprinter_bkmark_locate(void);


#endif /*end __CTC_TPRINTER_H__*/






