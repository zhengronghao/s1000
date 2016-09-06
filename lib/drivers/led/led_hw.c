/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : led_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/21/2014 4:36:04 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "led_hw.h"

void hw_led_gpio_output(GPIO_Type GPIOx,PORTPin_TypeDef pin)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = GPIOx;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_DSH;
    gpio_init.PORT_Pin = pin;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(pin);
    hw_gpio_init(&gpio_init);
}

void hw_led_init(void)
{

//    hw_led_gpio_output(LED_GPIO_CTRL,LED_CTRL_PINx);
//    hw_gpio_set_bits(LED_GPIO_CTRL,1<<LED_CTRL_PINx);

    hw_led_gpio_output(LED_BLUE_GPIO,LED_BLUE_PINx);
    hw_led_gpio_output(LED_YELLOW_GPIO,LED_YELLOW_PINx);
    hw_led_gpio_output(LED_GREEN_GPIO,LED_GREEN_PINx );
    hw_led_gpio_output(LED_RED_GPIO,LED_RED_PINx);
    hw_led_off(LED_ALL);
}

void hw_led_off(uint32_t ledbit)
{
    if (ledbit & S_LED_BLUE) {
        hw_gpio_set_bits(LED_BLUE_GPIO,1<<LED_BLUE_PINx);
    }
    if (ledbit & S_LED_YELLOW) {
        hw_gpio_set_bits(LED_YELLOW_GPIO,1<<LED_YELLOW_PINx);
    }
    if (ledbit & S_LED_GREEN) {
        hw_gpio_set_bits(LED_GREEN_GPIO,1<<LED_GREEN_PINx);
    }
    if (ledbit & S_LED_RED) {
        hw_gpio_set_bits(LED_RED_GPIO,1<<LED_RED_PINx );
    }
}

void hw_led_on(uint32_t ledbit)
{
    if (ledbit & S_LED_BLUE) {
        hw_gpio_reset_bits(LED_BLUE_GPIO,1<<LED_BLUE_PINx);
    }
    if (ledbit & S_LED_YELLOW) {
        hw_gpio_reset_bits(LED_YELLOW_GPIO,1<<LED_YELLOW_PINx);
    }
    if (ledbit & S_LED_GREEN) {
        hw_gpio_reset_bits(LED_GREEN_GPIO,1<<LED_GREEN_PINx);
    }
    if (ledbit & S_LED_RED) {
        hw_gpio_reset_bits(LED_RED_GPIO,1<<LED_RED_PINx );
    }
}



