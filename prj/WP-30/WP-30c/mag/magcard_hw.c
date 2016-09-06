/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : magcard_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/21/2014 4:36:04 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
#if (defined CFG_MAGCARD)

void hw_mag_gpio_input(PORTPin_TypeDef pin)
{
    GPIO_InitTypeDef gpio_init;

    //keyboard row init
    gpio_init.GPIOx = MAG_GPIO;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PU;
    gpio_init.PORT_Pin = pin;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(pin);
    hw_gpio_init(&gpio_init);
}

void hw_mag_gpio_output(PORTPin_TypeDef pin)
{
    GPIO_InitTypeDef gpio_init;

    if (pin == PORT_Pin_Invalid) {
        return;
    }
    gpio_init.GPIOx = MAG_GPIO;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
    gpio_init.PORT_Pin = pin;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(pin);
    hw_gpio_init(&gpio_init);
}

//    hw_set_gpio_input(KB_GPIO_COL,(1<<KB_COL1)|(1<<KB_COL2)|(1<<KB_COL3)|(1<<KB_COL4));
uint8_t hw_mag_data_read(void)
{
   return hw_gpio_get_bit(MAG_GPIO,(GPIOPin_TypeDef)(1<<MAG_DATA));
}

void hw_mag_data_write(uint8_t status)
{
    if (status)
        hw_gpio_set_bits(MAG_GPIO,1<<MAG_DATA);
    else
        hw_gpio_reset_bits(MAG_GPIO,1<<MAG_DATA);
}

uint8_t hw_mag_strobe_read(void)
{
   return hw_gpio_get_bit(MAG_GPIO,(GPIOPin_TypeDef)1<<MAG_STROBE);
}

void hw_mag_strobe_write(uint8_t status)
{
    if (status)
        hw_gpio_set_bits(MAG_GPIO,1<<MAG_STROBE);
    else
        hw_gpio_reset_bits(MAG_GPIO,1<<MAG_STROBE);
}

void hw_mag_power_write(uint8_t status)
{
    uint8_t shift_bit;
    if (MAG_POWER == PORT_Pin_Invalid) {
        return;
    } else
    {
        shift_bit = MAG_POWER;
    }
    if (status)
        hw_gpio_set_bits(MAG_GPIO,1<<shift_bit);
    else
        hw_gpio_reset_bits(MAG_GPIO,1<<shift_bit);
}

void hw_mag_power_init(GPIO_Type GPIOx, PORTPin_TypeDef pin)
{
    GPIO_InitTypeDef gpio_init;

    if (pin == PORT_Pin_Invalid) {
        return;
    }
    gpio_init.GPIOx = GPIOx;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
    gpio_init.PORT_Pin = pin;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(pin);
    hw_gpio_init(&gpio_init);
}

void hw_mag_power_open(void)
{
    gpio_set_bit(HCM_POWER_PTx,0);
}

void hw_mag_power_close(void)
{
    gpio_set_bit(HCM_POWER_PTx,1);
}

uint8_t hw_mag_iolevel_ch2read(void)
{
   return hw_gpio_get_bit(HCM_CH2_GPIO,HCM_CH2_PIN);
}

uint8_t hw_mag_iolevel_ch3read(void)
{
   return hw_gpio_get_bit(HCM_CH3_GPIO,HCM_CH3_PIN);
}
#endif





