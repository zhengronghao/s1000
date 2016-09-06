/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
* File Name          : systick_hw.c
* bfief              : The drivers level of systick
* Author             : luocs
* Version            : V0.00
* Date               : 2014.01.02
* Description        : 
*******************************************************************************/
/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "systick_hw.h"


uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1) > SysTick_RVR_RELOAD_MASK)  return (1);      /* Reload value impossible */

  SYST_RVR  = ticks - 1;                                  /* set reload register */
  NVIC_SetPriority (SysTick_IRQn, 14);  /* set Priority for Systick Interrupt */
  SYST_CVR   = 0;                                          /* Load the SysTick Counter Value */
  SYST_CSR  = SysTick_CSR_CLKSOURCE_MASK |
              SysTick_CSR_TICKINT_MASK   |
              SysTick_CSR_ENABLE_MASK;                    /* Enable SysTick IRQ and SysTick Timer */

  return (0);                                             /* Function successful */
}

int hw_systick_open(void)
{
    return SysTick_Config(core_clk_khz*10);
}

void hw_systick_close(void)
{ /* Disable SysTick IRQ and SysTick Timer */
    SYST_CSR  &= ~(SysTick_CSR_CLKSOURCE_MASK |
                   SysTick_CSR_TICKINT_MASK   |
                   SysTick_CSR_ENABLE_MASK);    
    SYST_CVR = 0x00;  
}

