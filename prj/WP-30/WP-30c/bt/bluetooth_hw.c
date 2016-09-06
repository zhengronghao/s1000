/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : bluetooth_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 11/12/2014 5:11:49 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "drv_inc.h"
#include "bluetooth_hw.h"

#define BT_GPIO_INPUT   0
#define BT_GPIO_OUTPUT  1
//mode:1-output or 0-input
static void _bt_gpio_init(GPIO_Type gpiox,PORTPin_TypeDef pinx,uint8_t mode)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = gpiox;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL;
    gpio_init.PORT_Pin = pinx;
    gpio_init.GPIO_Pinx_Mode = (mode == BT_GPIO_INPUT)?GPIO_INPUT(pinx):GPIO_OUTPUT(pinx);
    hw_gpio_init(&gpio_init);
}

void hw_bt_gpio_init(void)
{
    //output init
    _bt_gpio_init(BT_BATIN_ENABLE_GPIO,BT_BATIN_ENABLE_PINx,BT_GPIO_OUTPUT);
    _bt_gpio_init(BT_WAKEUP_ENABLE_GPIO,BT_WAKEUP_ENABLE_PINx,BT_GPIO_OUTPUT);
    _bt_gpio_init(BT_SYSCONFIG_P20_GPIO,BT_SYSCONFIG_P20_PINx,BT_GPIO_OUTPUT);
    _bt_gpio_init(BT_SYSCONFIG_P24_GPIO,BT_SYSCONFIG_P24_PINx,BT_GPIO_OUTPUT);
    _bt_gpio_init(BT_SYSCONFIG_EAN_GPIO,BT_SYSCONFIG_EAN_PINx,BT_GPIO_OUTPUT);
    _bt_gpio_init(BT_MODULE_RST_N_GPIO,BT_MODULE_RST_N_PINx,BT_GPIO_OUTPUT);
    //input init
    _bt_gpio_init(BT_STATUS_IND_2_GPIO,BT_STATUS_IND_2_PINx,BT_GPIO_INPUT);
    _bt_gpio_init(BT_STATUS_IND_1_GPIO,BT_STATUS_IND_1_PINx,BT_GPIO_INPUT);

    hw_bt_gpio_set(BT_BATIN_ENABLE_GPIO,BT_BATIN_ENABLE_PINx,0);
    hw_bt_gpio_set(BT_WAKEUP_ENABLE_GPIO,BT_WAKEUP_ENABLE_PINx,0);
    hw_bt_gpio_set(BT_SYSCONFIG_P20_GPIO,BT_SYSCONFIG_P20_PINx,0);
    hw_bt_gpio_set(BT_SYSCONFIG_P24_GPIO,BT_SYSCONFIG_P24_PINx,0);
    hw_bt_gpio_set(BT_SYSCONFIG_EAN_GPIO,BT_SYSCONFIG_EAN_PINx,0);
    hw_bt_gpio_set(BT_MODULE_RST_N_GPIO,BT_MODULE_RST_N_PINx,0);
}

void hw_bt_gpio_set(GPIO_Type gpiox,PORTPin_TypeDef pinx,uint8_t level)
{
    if (level == 0)
        hw_gpio_reset_bits(gpiox,(1<<pinx));
    else
        hw_gpio_set_bits(gpiox,(1<<pinx));
}

uint8_t hw_bt_gpio_get(GPIO_Type gpiox,PORTPin_TypeDef pinx)
{
    return hw_gpio_get_bit(gpiox,(1<<pinx));
}



