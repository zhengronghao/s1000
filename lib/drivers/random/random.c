/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : random.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/20/2014 3:17:45 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "random.h"

int _get_rand(uint32_t *rand)
{
    int i;
    for (i=0; i<3; i++) 
    {
        if (hw_rnga_getnumber(rand) == 0) {
            return 0;
        }
    }
    return -1;
}

int drv_rand(uint32_t seed,uint32_t length,uint32_t *rand)
{
    int iRet=0;

    hw_rnga_init(seed);
    while (length--)
    {
        if (_get_rand(rand)) {
            iRet = -1;
            break;
        }
        rand++;
    }
    hw_rnga_stop();

    return iRet;
}

void RNG_FillRandom(uint8_t * buffer, int numToFill)
{
    static uint32_t seed=0x11223344;
    uint32_t rand;
    uint32_t tmp;

    if (numToFill >= 4)
    {
        drv_rand(seed,numToFill>>2,(uint32_t *)buffer);
        seed |= *buffer;
        seed <<= 8;
    }
    tmp = numToFill%4;
    if (tmp) 
    {
        drv_rand(seed,1,&rand);
        numToFill -= tmp;
        memcpy(buffer+numToFill,&rand,tmp);
        seed = rand;
    }
}

