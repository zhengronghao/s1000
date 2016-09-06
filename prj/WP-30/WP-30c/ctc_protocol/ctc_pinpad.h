/*
 * =====================================================================================
 *
 *       Filename:  ctc_magcard.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/7/2016 3:02:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */

#ifndef __CTC_PINPAD_H__
#define __CTC_PINPAD_H__ 

#include "ParseCmd.h"

uint16_t ctc_pinpad_open(uint32_t mode);
uint16_t ctc_pinpad_close(uint32_t mode);
uint16_t ctc_pinpad_load_mk(uint32_t len, uint8_t *input);
uint16_t ctc_pinpad_load_wk(uint32_t len, uint8_t *input);
uint16_t ctc_pinpad_get_rand(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output);
uint16_t ctc_pinpad_get_pin_online(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output);
uint16_t ctc_pinpad_get_pin_offline(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output);
uint16_t ctc_pinpad_get_encrpt_data(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output);
uint16_t ctc_pinpad_get_mac(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output);
uint16_t ctc_pinpad_select_key(uint32_t len, uint8_t *input);
uint16_t ctc_pinpad_extern_main(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output);

#endif /*end __CTC_PINPAD_H__*/

