/*
 * File:		dryice.h
 * Purpose:		DryIce header file
 *
 */

#ifndef __dryice_H__
#define __dryice_H__

#include "common.h"

#ifdef DEBUG_Dx
#define CFG_DBG_HW_DRYICE
#endif

#ifdef CFG_DBG_HW_DRYICE 
#define DRY_TRACE(...)  printk(__VA_ARGS__)
#else
#define DRY_TRACE(...)
#endif


//void dryice_init();
void dryice_enable_clock_and_prescaler(int);
void dryice_int_tamper_config(int, int);
void dryice_pin_tamper_config(int, int);
void dryice_int_tamper_interrupt_config(int, int);
void dryice_pin_tamper_interrupt_config(int, int);
//void dryice_tamper_pin_pull_config(int, int);
//void dryice_tamper_pin_expect_config(int, int);
//void dryice_tamper_pin_glitch_filter_config(int, int);
//void dryice_tamper_pin_glitch_filter_width_config(int, int);
//void dryice_tamper_pin_glitch_filter_prescaler_config(int, int);
//void dryice_tamper_pin_direction_config(int, int);
//void dryice_tamper_pin_polarity_config(int, int);
void dryice_prescaler_config(int);
void dryice_active_tamper_config(int, int, int);
void dryice_enable_TamperForceSystem(int mode);

//void glitch_filter_prescaler_switch(int tamper_pin, int desired_glitch_filter_setting);
//void glitch_filter_width_select(int tamper_pin, int desired_glitch_filter_width);
//void glitch_filter_prescaler_select(int tamper_pin, int desired_glitch_filter_prescaler);

#endif /* __DRYICE_H__ */

