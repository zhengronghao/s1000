/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : keyboard_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/21/2014 4:36:04 PM
 * Description        : 
 *******************************************************************************/
#include "keyboard_hw.h"

struct KB_OptDef {
    GPIO_MemMapPtr gpiox;
    PORTPin_TypeDef pinx;
};

const struct KB_OptDef gcKbOpt[] = {
    {KB_GPIO_COL0,KB_PINx_COL0},
    {KB_GPIO_COL1,KB_PINx_COL1},
    {KB_GPIO_COL2,KB_PINx_COL2},
    {KB_GPIO_COL3,KB_PINx_COL3},
    {KB_GPIO_COL4,KB_PINx_COL4},
    {KB_GPIO_ROW0,KB_PINx_ROW0},
    {KB_GPIO_ROW1,KB_PINx_ROW1},
    {KB_GPIO_ROW2,KB_PINx_ROW2},
    {KB_GPIO_ROW3,KB_PINx_ROW3},
    {KB_GPIO_INDEPENDENT0,KB_PINx_INDEPENDENT0},
};


void hw_kb_col_irq_open(KB_IndexDef index)
{
    GPIO_InitTypeDef gpio_init;

    if (gcKbOpt[index].gpiox == NULL) {
        return;
    }
    gpio_init.GPIOx = gcKbOpt[index].gpiox;
//    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PU 
//        |PORT_Mode_IN_PFE|PORT_Mode_IRQ_EXTI_FEdge;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PU
        |PORT_Mode_IN_PFE|PORT_Mode_IRQ_EXTI_FEdge;
    gpio_init.PORT_Pin = gcKbOpt[index].pinx;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(gcKbOpt[index].pinx);
    hw_gpio_init(&gpio_init);
}
 
//colse col pinx irq,and set the pinx to input mode
void hw_kb_col_irq_close(KB_IndexDef index)
{
    GPIO_InitTypeDef gpio_init;

    if (gcKbOpt[index].gpiox == NULL) {
        return;
    }
    gpio_init.GPIOx = gcKbOpt[index].gpiox;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PU 
        |PORT_Mode_IN_PFE;
    gpio_init.PORT_Pin =  gcKbOpt[index].pinx;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(gcKbOpt[index].pinx);
    hw_gpio_init(&gpio_init);
}

static void hw_kb_row_init(KB_IndexDef index)
{
    GPIO_InitTypeDef gpio_init;

    if (gcKbOpt[index].gpiox == NULL) {
        return;
    }
    gpio_init.GPIOx = gcKbOpt[index].gpiox;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
    gpio_init.PORT_Pin =  gcKbOpt[index].pinx;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(gcKbOpt[index].pinx);
    hw_gpio_init(&gpio_init);
}

void hw_kb_col_irq_hight_open(KB_IndexDef index)
{
    GPIO_InitTypeDef gpio_init;

    if (gcKbOpt[index].gpiox == NULL) {
        return;
    }
    gpio_init.GPIOx = gcKbOpt[index].gpiox;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PDIS 
        |PORT_Mode_IN_PFE|PORT_Mode_IRQ_EXTI_REdge;
    gpio_init.PORT_Pin = gcKbOpt[index].pinx;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(gcKbOpt[index].pinx);
    hw_gpio_init(&gpio_init);
}


void hw_kb_gpio_init(void)
{
    //keyboard row init
    hw_kb_col_irq_open(KB_COL1);
    hw_kb_col_irq_open(KB_COL2);
    hw_kb_col_irq_open(KB_COL3);
    hw_kb_col_irq_open(KB_COL4);
    hw_kb_col_irq_hight_open(KB_INDEPENDENT0);
    //keyboard col init
    hw_kb_row_init(KB_ROW0);
    hw_kb_row_init(KB_ROW1);
    hw_kb_row_init(KB_ROW2);
    hw_kb_row_init(KB_ROW3);
}


