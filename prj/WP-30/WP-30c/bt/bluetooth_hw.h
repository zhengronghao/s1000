/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : bluetooth_hw.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 11/12/2014 5:11:53 PM
 * Description        : 
 *******************************************************************************/
#ifndef __BLUETOOTH_HW_H__
#define __BLUETOOTH_HW_H__

void hw_bt_gpio_init(void);
uint8_t hw_bt_gpio_get(GPIO_Type gpiox,PORTPin_TypeDef pinx);
void hw_bt_gpio_set(GPIO_Type gpiox,PORTPin_TypeDef pinx,uint8_t level);

#endif


