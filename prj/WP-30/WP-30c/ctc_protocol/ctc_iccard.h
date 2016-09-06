/*
 * =====================================================================================
 *
 *       Filename:  ctc_iccard.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/7/2016 3:34:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */

#ifndef __CTC_ICCARD_H__
#define __CTC_ICCARD_H__ 

#include "ParseCmd.h"


uint16_t iccard_slot_open(uint8_t slot);

uint16_t iccard_type_select(uint8_t slot,uint8_t vol,uint8_t mode);

uint16_t iccard_slot_close(uint8_t slot);

uint16_t iccard_card_open(uint8_t slot,uint32_t* len,uint8_t *buffer);

uint16_t iccard_exchange_apdu(uint8_t slot,uint32_t sed_len,uint8_t* data,uint32_t *rev_len, uint8_t* rev_buf);

uint16_t iccard_card_close(uint8_t slot);

uint16_t iccard_card_check(uint8_t slot,uint32_t *len,uint8_t *buffer);

#endif /*end __CTC_ICCARD_H__*/









