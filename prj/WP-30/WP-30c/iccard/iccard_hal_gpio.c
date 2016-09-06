/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : iccard_hal_gpio.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 10/15/2014 5:02:47 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
#include "iccard_hal.h"
#if (defined CFG_ICCARD)

extern void icc_EnableCardInsertInt(void);
static void hw_ic_pwr_gpio_init (void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
    gpio_init.GPIOx = IC_PWR_GPIO;
    gpio_init.PORT_Pin = IC_PWR_PINx;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(IC_PWR_PINx);
    hw_gpio_init(&gpio_init);
    hw_gpio_reset_bits(IC_PWR_GPIO,1<<IC_PWR_PINx);
}

int icc_gpioctrl_init (ICC_GpioInfo_t *p,int mode)
{
    ICC_HalInfo_t *haltinfo;

    haltinfo = &gIccHalInfo;
    hw_ic_gpio_init(mode);
    hw_ic_pwr_gpio_init();
    icc_EnableCardInsertInt(); 	// 打开插拔卡中断
//    requst_pio_interrupt(p->icc_gpio_io,INT_EDGE, 6,icc_CardIOIsr); // IO中断
//    disable_pio_interrupt(p->icc_gpio_io);
    haltinfo->usercard_stat = 0;
    return 0;
}

int icc_gpioctrl_read (ICC_GpioInfo_t *p,int mode, int *param)
{
    int ret=0;
    switch ( mode )
    {
    case CARD_DETECT :
//        if (hw_iccard_gpio_read(GPIO_ID_IC_INT))
//            *param = 0;
//        else
//            *param = 1;

        if (hw_iccard_gpio_read(GPIO_ID_IC_INT)){
            s_DelayMs(1);
            if (hw_iccard_gpio_read(GPIO_ID_IC_INT))
                *param = 0;
            else
                *param = 1;
        }else{
            s_DelayMs(1);
            if (hw_iccard_gpio_read(GPIO_ID_IC_INT))
                *param = 0;
            else
                *param = 1;
        }
        break;
    case ASYNCLK_RD_IO :
        *param = (uint)hw_iccard_gpio_read(GPIO_ID_IC_IO);
        break;
    default :
        break;
    }
    return ret;
}


int icc_gpioctrl_write (ICC_GpioInfo_t *p,int mode,int value)
{
    switch(mode)
    {
    case VCC_MODE:
        if (value == 0)
            hw_gpio_reset_bits(IC_PWR_GPIO,1<<IC_PWR_PINx);
        else
            hw_gpio_set_bits(IC_PWR_GPIO,1<<IC_PWR_PINx);
        break;
    case WRITE_RST:
        hw_iccard_gpio_write(p->icc_gpio_rst,value);
        break;
    case SET_IO_OUTPUT:
        hw_iccard_gpio_ioctl(p->icc_gpio_io,1,0); 
        break;
    case SET_IO_INPUT:
        hw_iccard_gpio_ioctl(p->icc_gpio_io,0,0); 
        hw_iccard_gpio_write(p->icc_gpio_io,0);
        break;
    case WRITE_IO:
        hw_iccard_gpio_write(p->icc_gpio_io,value);
        break;
    default:
        break;
    }
    return 0;
}

#endif


