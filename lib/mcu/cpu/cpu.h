/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : cpu.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/22/2014 3:28:23 PM
 * Description        : 
 *******************************************************************************/

#ifndef __CPU_H__
#define __CPU_H__ 

extern int mcg_clk_hz;
extern int mcg_clk_khz;
extern int core_clk_khz;
extern int core_clk_mhz;
extern int periph_clk_khz;


// function prototypes
void start(void);
void cpu_identify(void);

//sysinit
void cpu_clk_init(void);
void trace_clk_init(void);
void clkout_init(void);
void enable_abort_button(void);
/********************************************************************/

void mcg_pee_fei(void);
void cpu_clk_refresh(void);

#endif

