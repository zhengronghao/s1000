/*
 * =====================================================================================
 *
 *       Filename:  ctc_rfcard.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/7/2016 7:50:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#ifndef __CTC_RFCARD_H__
#define __CTC_RFCARD_H__ 

#include "ParseCmd.h"



uint16_t rfcard_open(uint32_t mode);

uint16_t rfcard_close(void);

uint16_t rfcard_module_open(void);

uint16_t rfcard_module_close(void);

uint16_t rfcard_type_select(uint8_t type);

uint16_t rfcard_poll_powerup(uint32_t* len, uint8_t* buffer);

uint16_t rfcard_exchange_apdu(uint8_t sed_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer);

uint16_t rfcard_poll(void);

uint16_t rfcard_card_open(uint32_t* len, uint8_t* buffer);

uint16_t rfcard_card_close(void);

uint16_t rfcard_mifare (uint8_t sed_len,uint8_t* data,uint32_t *rev_len,uint8_t* buffer);

#endif /*end __CTC_RFCARD_H__*/




