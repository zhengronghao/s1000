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

#ifndef __CTC_MAGCARD_H__
#define __CTC_MAGCARD_H__ 

#include "ParseCmd.h"


uint16_t magcard_open(uint32_t mode);

uint16_t magcard_close(void);


uint16_t magcard_credit_card(uint16_t mode);

uint16_t magcard_cancle_card(void);


#endif /*end __CTC_MAGCARD_H__*/

