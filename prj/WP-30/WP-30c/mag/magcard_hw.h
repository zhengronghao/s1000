/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : magcard_hw.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/25/2014 5:40:39 PM
 * Description        : 
 *******************************************************************************/

#ifndef __MAGCARD_HW__
#define __MAGCARD_HW__

void hw_mag_gpio_input(PORTPin_TypeDef pin);
void hw_mag_gpio_output(PORTPin_TypeDef pin);
uint8_t hw_mag_data_read(void);
void hw_mag_data_write(uint8_t status);
uint8_t hw_mag_strobe_read(void);
void hw_mag_strobe_write(uint8_t status);
void hw_mag_power_write(uint8_t status);
void hw_mag_power_init(GPIO_Type GPIOx, PORTPin_TypeDef pin);
void hw_mag_power_open(void);
void hw_mag_power_close(void);
uint8_t hw_mag_iolevel_ch2read(void);
uint8_t hw_mag_iolevel_ch3read(void);

#endif


