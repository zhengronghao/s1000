/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : rnga_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/20/2014 2:44:07 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "rnga_hw.h"


/*****************************************************************************************************
 * Code of project wide FUNCTIONS
 *****************************************************************************************************/
void hw_rnga_reg_report(void)
{
    TRACE("\n\nRNG_CR    = 0x%04X,\t",  (RNG_CR)) ;
    TRACE("RNG_SR    = 0x%04X",     (RNG_SR)) ;
//    TRACE("\nRNG_ER    = 0x%04X,\t",  (RNG_ER)) ;
    //    TRACE("RNG_OR    = 0x%04X",     (RNG_OR)) ;
}

/**  
 * \brief   init random number generation module
 * \author   
 * \param    none
 * \return   the 32-bit random number
 * \todo    
 * \warning
 */
void hw_rnga_init(unsigned int seed)
{
    /*FSL: enable RNGB clk*/
    SIM_SCGC6 |= SIM_SCGC6_RNGA_MASK;
//    hw_rnga_reg_report();
    RNG_ER = seed;

    /*FSL: generate randomness*/
    RNG_CR = RNG_CR_GO_MASK;
//    hw_rnga_reg_report();
}

/**  
 * \brief   Wait for the generation of a random number
 * \author   
 * \param    none
 * \return   the 32-bit random number
 * \todo    
 * \warning
 */
int hw_rnga_getnumber(uint32_t *random_number)
{   
    uint32_t i=0;

    *random_number = 0;
    if (RNG_SR | RNG_SR_SLP_MASK) {
        RNG_CR &= ~RNG_SR_SLP_MASK;
    }

    /*FSL: check for FIFO level*/
    while(!(RNG_SR & RNG_SR_OREG_LVL_MASK)) {
        if (i++ > 0xFFFF) {
            TRACE("\nTime out");
            return -1;
        }
    }
    /*FSL: get random number*/
    *random_number = RNG_OR;
    if (RNG_SR & RNG_SR_LRS_MASK) {
        TRACE("\nRead error");
        return -1;
    }

    return 0;
}

/**  
 * \brief   Stop randomness:Enter SLEEP mode
 * \author   
 * \param    none
 * \return   the 32-bit random number
 * \todo    
 * \warning
 */
void hw_rnga_stop(void)
{
//    RNG_CR &= ~RNG_CR_GO_MASK;
    RNG_CR |= RNG_CR_SLP_MASK;
}

/********************************************************************/

