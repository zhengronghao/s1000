/*
 * =====================================================================================
 *
 *       Filename:  iccard_hw.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/24/2014 9:57:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yehf (), yehf@itep.com.cn
 *        Company:  START
 *
 * =====================================================================================
 */

#ifndef __ICCARD_HW__
#define __ICCARD_HW__ 


/*-----------------------------------------------------------------------------}
 *  宏定义
 *-----------------------------------------------------------------------------{*/
typedef enum 
{
    GPIO_ID_IC_C8 = 0,  
    GPIO_ID_IC_C4,  
    GPIO_ID_IC_SEL1,
    GPIO_ID_IC_SEL2,
    GPIO_ID_IC_SEL3,
    GPIO_ID_IC_RST, 
    GPIO_ID_IC_CS, 
    GPIO_ID_IC_IO,  
    GPIO_ID_IC_CLK, 
    GPIO_ID_SAM_CS1,
    GPIO_ID_SAM_CS2,
    GPIO_ID_SAM1_PWR,
    GPIO_ID_SAM2_PWR,
    GPIO_ID_SAM1_RST,
    GPIO_ID_SAM2_RST,
    GPIO_ID_SAM_INIT,
    GPIO_ID_SAM1_IO, 
    GPIO_ID_SAM2_IO,
    GPIO_ID_SAM_CLK,
    GPIO_ID_IC_INT, 
}GPIO_ID_Def;
/** 
  * @brief  GPIO Init structure definition  
  */

typedef struct
{
    uchar id;  // ic卡驱动gpio的id
    uchar initstat;  //初始状态
    uchar lowerstat; //低功耗前的状态
    uchar PORT_Pin;  //PORTPin_TypeDef类型
    GPIO_Type GPIOx;     
//    uint port;
}_IC_GPIO_MAP_INFO;
/*-----------------------------------------------------------------------------}
 *  变量 
 *-----------------------------------------------------------------------------{*/
/*-----------------------------------------------------------------------------}
 *  函数 
 *-----------------------------------------------------------------------------{*/
int hw_iccard_gpio_write (int id, int status);
int hw_iccard_gpio_read (int id);
int hw_iccard_gpio_ioctl (int id, int mode, int value);
void hw_ic_gpio_init (int mode);
void hw_sam_gpio_init (int mode);
void IC_INT_IRQHandler(void);
void IC_IO_IRQHandler(void);
void SAM_IO_IRQHandler(void);

#endif
