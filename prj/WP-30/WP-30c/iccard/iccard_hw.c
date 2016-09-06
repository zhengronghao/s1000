/*
 * =====================================================================================
 *
 *       Filename:  iccard_hw.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/24/2014 9:57:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yehf (), yehf@itep.com.cn
 *        Company:  START
 *
 * =====================================================================================
 */
#include "wp30_ctrl.h"
#if (defined CFG_ICCARD)
/*-----------------------------------------------------------------------------}
 *  全局变量
 *-----------------------------------------------------------------------------{*/
// 大小卡输出管脚映射关系
static const _IC_GPIO_MAP_INFO gICGpioMapInfo[] = {
    //         id      initstat low         Pin                   GPIOx
/* 0 */    {(uchar)GPIO_ID_IC_C8,    0,  0, (uchar)IC_C8_PINx,    IC_C8_GPIO,   /*(uint)IC_C8_PORT     */},
/* 1 */    {(uchar)GPIO_ID_IC_C4,    0,  0, (uchar)IC_C4_PINx,    IC_C4_GPIO,   /*(uint)IC_C4_PORT     */},
/* 2 */    {(uchar)GPIO_ID_IC_SEL1,  1,  0, (uchar)IC_SEL1_PINx,  IC_SEL1_GPIO, /*(uint)IC_SEL1_PORT   */},
/* 3 */    {(uchar)GPIO_ID_IC_SEL2,  1,  0, (uchar)IC_SEL2_PINx,  IC_SEL2_GPIO, /*(uint)IC_SEL2_PORT   */},
/* 4 */    {(uchar)GPIO_ID_IC_SEL3,  1,  0, (uchar)IC_SEL3_PINx,  IC_SEL3_GPIO, /*(uint)IC_SEL3_PORT   */},
/* 5 */    {(uchar)GPIO_ID_IC_RST,   1,  1, (uchar)IC_RST_PINx,   IC_RST_GPIO,  /*(uint)IC_RST_PORT    */},
/* 6 */    {(uchar)GPIO_ID_IC_CS,    1,  0, (uchar)IC_CS_PINx,    IC_CS_GPIO,  /*(uint)IC_RST_PORT    */},
/* 7 */    {(uchar)GPIO_ID_IC_IO,    0,  0, (uchar)IC_IO_PINx,    IC_IO_GPIO,   /*(uint)IC_IO_PORT     */},
/* 8 */    {(uchar)GPIO_ID_IC_CLK,   1,  1, (uchar)IC_CLK_PINx,   IC_CLK_GPIO,  /*(uint)IC_CLK_PORT    */},

/* 9 */    {(uchar)GPIO_ID_SAM_CS1,  0,  0, (uchar)SAM_CS1_PINx,  SAM_CS1_GPIO, /*(uint)SAM_CS1_PORT   */},
/* 10 */   {(uchar)GPIO_ID_SAM_CS2,  0,  0, (uchar)SAM_CS2_PINx,  SAM_CS2_GPIO, /*(uint)SAM_CS2_PORT   */},
/* 11 */   {(uchar)GPIO_ID_SAM1_PWR, 0,  0, (uchar)SAM1_PWR_PINx, SAM1_PWR_GPIO, /*(uint)SAM_PWR_PORT   */},
/* 12 */   {(uchar)GPIO_ID_SAM2_PWR, 0,  0, (uchar)SAM2_PWR_PINx, SAM2_PWR_GPIO, /*(uint)SAM_PWR_PORT   */},
/* 13 */   {(uchar)GPIO_ID_SAM1_RST, 1,  1, (uchar)SAM1_RST_PINx, SAM1_RST_GPIO, /*(uint)SAM_IO_PORT    */}, 
/* 14 */   {(uchar)GPIO_ID_SAM2_RST, 1,  1, (uchar)SAM2_RST_PINx, SAM2_RST_GPIO, /*(uint)SAM_IO_PORT    */}, 
/* 15 */   {(uchar)GPIO_ID_SAM_INIT, 0,  0, (uchar)SAM_INIT_PINx, SAM_INIT_GPIO,/*(uint)SAM_INIT_PORT   */},
/* 16 */   {(uchar)GPIO_ID_SAM1_IO,  0,  0, (uchar)SAM1_IO_PINx,  SAM1_IO_GPIO,  /*(uint)SAM_RST_PORT   */},
/* 17 */   {(uchar)GPIO_ID_SAM2_IO,  0,  0, (uchar)SAM2_IO_PINx,  SAM2_IO_GPIO,  /*(uint)SAM_RST_PORT   */},

/* 18 */   {(uchar)GPIO_ID_SAM_CLK,  1,  1, (uchar)SAM_CLK_PINx,  SAM_CLK_GPIO, /*(uint)SAM_CLK_PORT   */},

/* 19 */   {(uchar)GPIO_ID_IC_INT,   0,  0, (uchar)IC_INT_PINx,   IC_INT_GPIO,  /*(uint)IC_INT_PORT    */},
}; 

/*-----------------------------------------------------------------------------}
 *  函数定义
 *-----------------------------------------------------------------------------{*/
/* 
 * hw_iccard_gpio_write - [GENERIC] 
 * @ 
 */
int hw_iccard_gpio_write (int id, int status)
{
//    TRACE("[wr:%d-%d]",id,status);
    if (gICGpioMapInfo[id].GPIOx == NULL)
        return 0;
    if (status)
        hw_gpio_set_bits(gICGpioMapInfo[id].GPIOx,1<<(gICGpioMapInfo[id].PORT_Pin));
    else
        hw_gpio_reset_bits(gICGpioMapInfo[id].GPIOx,1<<(gICGpioMapInfo[id].PORT_Pin));
    return 0;
}		/* -----  end of function hw_iccard_gpio_write  ----- */

/* 
 * hw_iccard_gpio_write - [GENERIC] 
 * @ 
 */
int hw_iccard_gpio_read (int id)
{
    if (gICGpioMapInfo[id].GPIOx == NULL)
        return 0;
    return hw_gpio_get_bit(gICGpioMapInfo[id].GPIOx,(GPIOPin_TypeDef)(1<<gICGpioMapInfo[id].PORT_Pin));
}		/* -----  end of function hw_iccard_gpio_write  ----- */

/* 
 * hw_iccard_gpio_ioctl - [GENERIC] 
 *    id:    PIN ID号
 *    mode:  0-置为输入   value:1-打开中断 0-不开启中断  2-上下边沿触发
 *           1-置为输出   value:输出时的IO电平 0-低电平 1-高电平 2-不配置
 *           2-置为外设   value:1-开启
 * @ 
 */
int hw_iccard_gpio_ioctl (int id, int mode, int value)
{
    GPIO_InitTypeDef gpio_init;
    PORTPin_TypeDef pin;
    GPIO_Type tgpio;

    if (gICGpioMapInfo[id].GPIOx == NULL)
        return 0;
    tgpio = gICGpioMapInfo[id].GPIOx;
    pin = (PORTPin_TypeDef)gICGpioMapInfo[id].PORT_Pin;
    gpio_init.GPIOx = tgpio;
    gpio_init.PORT_Pin = pin;
//    TRACE("[ioctl:%d-%d-%d]",id,mode,value);
    if ( mode == 0 ) 
    {
        gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PU;
        if ( value ) {
            if (value == 1) {
                gpio_init.PORT_Mode |= PORT_Mode_IRQ_EXTI_FEdge;
            } else {
//                gpio_init.PORT_Mode |= PORT_Mode_IRQ_EXTI_FEdge;
                gpio_init.PORT_Mode |= PORT_Mode_IRQ_EXTI_EEdge | PORT_Mode_IN_PFE;
            }
            if ( tgpio ==  GPIOA) {
                set_irq_priority (PORTA_IRQn, INT_PRI_ICCARD_IO);
                enable_irq(PORTA_IRQn);
            } else if( tgpio ==  GPIOB){
                set_irq_priority (PORTB_IRQn, INT_PRI_ICCARD_IO);
                enable_irq(PORTB_IRQn);
            } else if( tgpio ==  GPIOC){
                set_irq_priority (PORTC_IRQn, INT_PRI_ICCARD_IO);
                enable_irq(PORTC_IRQn);
            } else if( tgpio ==  GPIOD){
                set_irq_priority (PORTD_IRQn, INT_PRI_ICCARD_IO);
                enable_irq(PORTD_IRQn);
            }
        }
        gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(pin);
        hw_gpio_init(&gpio_init);
    } else if (mode == 1)
    {
        gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
        gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(pin);
        hw_gpio_init(&gpio_init);
        if (value == 1) {
            hw_gpio_set_bits(gpio_init.GPIOx,1<<(pin));
        } else if (value == 0) {
            hw_gpio_reset_bits(gpio_init.GPIOx,1<<(pin));
        }
    } else 
    {
        //外设口
    }
    return 0;
}		/* -----  end of function hw_iccard_gpio_ioctl  ----- */

/* 
 * hw_ic_gpio_init - [GENERIC] 初始化
 *    mode:  D0: 0-大卡异步模式初始化 1-大卡同步模式初始化
 * @ 
 */
void hw_ic_gpio_init (int mode)
{
    int i,pin,max;
    GPIO_InitTypeDef gpio_init;
//    DISPPOS(mode);
    if ( mode == 1 ) {
        max = GPIO_ID_IC_IO;
    } else {
        max = GPIO_ID_IC_CLK;
    }
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
    for ( i=GPIO_ID_IC_C8; i<= max ; i++  ) {
        if (gICGpioMapInfo[i].GPIOx == NULL){
            continue;
        }
        gpio_init.GPIOx = gICGpioMapInfo[i].GPIOx;
        pin = gICGpioMapInfo[i].PORT_Pin;
        gpio_init.PORT_Pin = (PORTPin_TypeDef)pin;
        gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(pin);
        hw_gpio_init(&gpio_init);
        if (gICGpioMapInfo[i].initstat)
            hw_gpio_set_bits(gpio_init.GPIOx,1<<pin);
        else
            hw_gpio_reset_bits(gpio_init.GPIOx,1<<pin);
    }
    // 设置插卡中断优先级
    set_irq_priority (IC_INT_IRQn, INT_PRI_ICCARD_INSERT);
    enable_irq(IC_INT_IRQn);
}		/* -----  end of function hw_ic_gpio_init  ----- */


/* 
 * hw_sam_gpio_init - [GENERIC] 小卡初始化
 *    mode:  
 * @ 
 */
void hw_sam_gpio_init (int mode)
{
    int i,pin;
    GPIO_InitTypeDef gpio_init;
//    DISPPOS(mode);
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
    for ( i=GPIO_ID_SAM_CS1 ; i<= GPIO_ID_SAM_CLK ; i++  ) {
        if (gICGpioMapInfo[i].GPIOx == NULL){
            continue;
        }
        gpio_init.GPIOx = gICGpioMapInfo[i].GPIOx;
        pin = gICGpioMapInfo[i].PORT_Pin;
        gpio_init.PORT_Pin = (PORTPin_TypeDef)pin;
        gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(pin);
        hw_gpio_init(&gpio_init);
        if (gICGpioMapInfo[i].initstat)
            hw_gpio_set_bits(gpio_init.GPIOx,1<<pin);
        else
            hw_gpio_reset_bits(gpio_init.GPIOx,1<<pin);
    }
}		/* -----  end of function hw_ic_gpio_init  ----- */

void IC_IO_IRQHandler(void)
{
    if (PORTx_IRQPinx(IC_IO_PORT,IC_IO_PINx)) {
        PORTx_IRQPinx_Clear(IC_IO_PORT,IC_IO_PINx);
        icc_CardIOIsr();
    }
}

void IC_INT_IRQHandler(void)
{
    if (PORTx_IRQPinx(IC_INT_PORT,IC_INT_PINx)) {
        PORTx_IRQPinx_Clear(IC_INT_PORT,IC_INT_PINx);
        usercard_insert_int();
    }
}
void SAM_IO_IRQHandler(void)
{
    if (PORTx_IRQPinx(SAM1_IO_PORT,SAM1_IO_PINx)) {
        icc_CardIOIsr();
        PORTx_IRQPinx_Clear(SAM1_IO_PORT,SAM1_IO_PINx);
    }
    if (PORTx_IRQPinx(SAM2_IO_PORT,SAM2_IO_PINx)) {
        icc_CardIOIsr();
        PORTx_IRQPinx_Clear(SAM2_IO_PORT,SAM2_IO_PINx);
    }
}
#endif



