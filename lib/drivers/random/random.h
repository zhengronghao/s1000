/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : random.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/20/2014 3:17:52 PM
 * Description        : 
 *******************************************************************************/
#ifndef __RANDOM_H__
#define __RANDOM_H__

int drv_rand(uint32_t seed,uint32_t length,uint32_t *rand);
void RNG_FillRandom(uint8_t * buffer, int numToFill);
#endif

