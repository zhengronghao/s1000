/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : dryice_drv.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 5/26/2014 11:55:18 AM
 * Description        : 
 *******************************************************************************/
#ifndef __DRYICE_DRV_H__
#define __DRYICE_DRV_H__

#include "fsl_dryice_hal.h"

struct TamperEvent {
    uint32_t time_s;//Tamper Time Seconds
    uint32_t tsr;   //DRY_SR
    uint32_t sources;//DryIce Tamper Enable Register
    uint32_t level;//DryIce Pin Polarity Register
};

struct DryiceActive {
    hal_dry_glitch_filter_prescaler_t prescaler;
    hal_dry_pin_direction_t bm_pin_direction;
    hal_dry_pin_polarity_t bm_pin_polarity;
    uint16_t polynomial;
    uint16_t shift;
    uint8_t width;
    uint8_t bm_pin;
};

void drv_dryice_clock_init(void);
int drv_dryice_open(uint8_t tamperpin0,uint8_t tamperpin1,uint8_t tamperpin2,uint8_t trig_level,uint8_t reset);
void drv_dryice_irq_open(void);

int drv_dryice_init(hal_dry_tamper_t bm_tamp_sources,
                    hal_dry_interrupt_t bm_interrupt,
                    uint8_t bm_trig_level,
                    uint8_t reset);

void drv_dryice_active_tamper_config(int active_tamper_n, int desired_polynomial, int desired_shift_reg_value);
int drv_dryice_active_output(uint8_t tamperpin,uint32_t expect_value);
int drv_dryice_active_input(uint8_t tamperpin,uint8_t atr_n,uint8_t width,uint8_t prescaler_clk);
int drv_dryice_active_init(hal_dry_tamper_t bm_tamp_sources,
                           hal_dry_interrupt_t bm_interrupt,
                           struct DryiceActive atr[DRY_ATR_COUNT],
                           uint8_t reset);

int drv_dryice_get_reg(struct TamperEvent *event);
#endif

