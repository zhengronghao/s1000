/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : battery.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 11/10/2014 7:25:38 PM
 * Description        : 
 *******************************************************************************/

#ifndef __BATTERY_H__
#define __BATTERY_H__

void power_charge_full_init(void);
void power_charge_adc_switch_init(void);
void power_charge_init(void);
uint8_t sys_get_batter_percentum(void);
void power_charge_adc_open(void);
void power_charge_adc_close(void);
int power_charge_adc_caculate(void);
void power_keep_init(void);
void power_keep_down(void);
void power_keep_on(void);
void power_keep_off(void);
void power_reset_init(void);
void power_reset(void);
void power_5v_init(void);
void power_5v_open(void);
void power_5v_close(void);
void power_3v3_init(void);
void power_3v3_open(void);
void power_3v3_close(void);


void power_down_switch_set(uchar value);
void power_down_check_init(void);
void power_down_check(void);
void power_wake_up_init(void);
void power_wake_up_check(void);
uint8_t get_wakeup_io_state(void);

//1-charging 0-using battery
inline uint8_t power_ifcharging(void)
{
    return hw_gpio_get_bit(POWER_CHARGE_GPIO,1<<POWER_CHARGE_PINx);
}
inline uint8_t power_charge_ifFull(void)
{
    return (hw_gpio_get_bit(POWER_CHARGE_FULL_GPIO,(1<<POWER_CHARGE_FULL_PINx))?TRUE:FALSE);
}

#endif


