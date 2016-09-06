/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : battery.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 11/10/2014 7:25:34 PM
 * Description        : 
 *******************************************************************************/
#include "app_common.h"
#include "battery.h"

#define POWER_DOWN_CHECK_TIME     300     //3s(系统定时器每10ms触发一次)
#define POWER_RESET_CHECK_TIME     30     //300ms(系统定时器每10ms触发一次)
#define POWER_START_CHECK_TIME     5000     //5s
#define POWER_WAKE_UP_CHECK_TIME     1500 //15s 强制关机 长按关机按键


volatile int power_down_check_count;  //检测关机口线 计数
uchar power_down_check_switch = ON;        //是否开启3秒关机检测
volatile int power_wake_up_count;     //唤醒口线（关机用）计数


/*-------------------------power charging module--------------------------------- */
void power_charge_full_init(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = POWER_CHARGE_FULL_GPIO;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PDIS 
        |PORT_Mode_IN_PFE;
    gpio_init.PORT_Pin =  POWER_CHARGE_FULL_PINx;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(POWER_CHARGE_FULL_PINx);
    hw_gpio_init(&gpio_init);
}


static void power_charge_adc_switch(uint8_t sw)
{
    if (sw == ON)
        hw_gpio_set_bits(POWER_CHARGE_ADC_GPIO,(1<<POWER_CHARGE_ADC_PINx));
    else
        hw_gpio_reset_bits(POWER_CHARGE_ADC_GPIO,(1<<POWER_CHARGE_ADC_PINx));
}

void power_charge_adc_switch_init(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = POWER_CHARGE_ADC_GPIO;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
    gpio_init.PORT_Pin =  POWER_CHARGE_ADC_PINx;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(POWER_CHARGE_ADC_PINx);
    hw_gpio_init(&gpio_init);
//    hw_gpio_set_bits(POWER_CHARGE_ADC_GPIO,(1<<POWER_CHARGE_ADC_PINx));
    power_charge_adc_switch(OFF);
}

void power_charge_adc_open(void)
{
    power_charge_adc_switch(ON);
    drv_adc_open(POWER_CHARGE_ADC_CTRLR,ADC_SC2_TRIGER_SOFTWARE,ADC_SC1n_SingleEnded,
                 ADC_SC3_HARDWARE_AVERAGE_ENABLE|ADC_SC3_HARDWARE_AVERAGE_32);
}

int power_charge_adc_caculate(void)
{
    return drv_adc_caculate(POWER_CHARGE_ADC_CHANEL);
}

void power_charge_adc_close(void)
{
    drv_adc_close(POWER_CHARGE_ADC_CTRLR);
    power_charge_adc_switch(OFF);
}


uint8_t sys_get_batter_percentum(void)
{
    int ad_val;
    power_charge_adc_open();
    ad_val = power_charge_adc_caculate();
    power_charge_adc_close();
    ad_val = ((74*ad_val)/10/4220);
    if (ad_val > 100)
        ad_val = 100;
    return (uint8_t)ad_val;
}
/*--------------------------5V power supply module---------------------------------- */
void power_5v_init(void)
{
    gpio_set_output(POWER_SUPPLY_5V_PTx,GPIO_OUTPUT_SlewRateFast,0); 
}

void power_5v_open(void)
{
    gpio_set_bit(POWER_SUPPLY_5V_PTx,1);
}


void power_5v_close(void)
{
    gpio_set_bit(POWER_SUPPLY_5V_PTx,0);
}

void power_3v3_init(void)
{
    gpio_set_output(POWER_SUPPLY_3V3_PTx,GPIO_OUTPUT_SlewRateFast,0); 
}

void power_3v3_open(void)
{
    gpio_set_bit(POWER_SUPPLY_3V3_PTx,1);
}

void power_3v3_close(void)
{
    gpio_set_bit(POWER_SUPPLY_3V3_PTx,0);
}
/*--------------------------power keep module---------------------------------- */

void power_keep_init(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = POWER_KEEP_GPIO;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
    gpio_init.PORT_Pin =  POWER_KEEP_PINx;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(POWER_KEEP_PINx);
    hw_gpio_init(&gpio_init);
    hw_gpio_set_bits(POWER_KEEP_GPIO,(1<<POWER_KEEP_PINx));
}

// init reset low level
void power_reset_init(void)
{
//    GPIO_InitTypeDef gpio_init;
//
//    gpio_init.GPIOx = POWER_RESET_GPIO;
//    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
//    gpio_init.PORT_Pin =  POWER_RESET_PINx;
//    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(POWER_RESET_PINx);
//    hw_gpio_init(&gpio_init);
//    hw_gpio_reset_bits(POWER_RESET_GPIO,(1<<POWER_RESET_PINx));

    gpio_set_output(POWER_RESET_PTx,GPIO_OUTPUT_SlewRateFast,0); 
}

void power_reset(void)
{
//    gpio_set_output(POWER_RESET_PTx,GPIO_OUTPUT_SlewRateFast,0); 
    gpio_set_bit(POWER_RESET_PTx,1);
    sys_delay_ms(16000);
    gpio_set_bit(POWER_RESET_PTx,0);
}

//void power_reset_input(void)
//{
//    gpio_set_input(POWER_RESET_PTx,PORT_Mode_IN_PDIS);
//}
//
//void power_reset_get(void)
//{
//    int ret;
//    ret = hw_gpio_get_bit(POWER_RESET_GPIO, 1<<POWER_RESET_PINx); 
//    TRACE("\r\nget the reset gpio:%d\r\n", ret);
//}

static void power_down_set_pinx(GPIO_Type GPIOx,PORTPin_TypeDef pinx,uint8_t level)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = GPIOx;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF;
    gpio_init.PORT_Pin =  pinx;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(pinx);
    hw_gpio_init(&gpio_init);
    if (level)
        hw_gpio_set_bits(GPIOx,(1<<pinx));
    else
        hw_gpio_reset_bits(GPIOx,(1<<pinx));
}

extern void hw_led_on(uint32_t ledbit);
void power_keep_down(void)
{ 
    DisableInterrupts //关机前关闭所有中断
    /*下电毛刺特殊处理:BEGIN*/

    power_down_set_pinx(MAG_GPIO,MAG_DATA,1);
    power_down_set_pinx(IC_IO_GPIO,IC_IO_PINx,1);
    power_down_set_pinx(SAM1_IO_GPIO,SAM1_IO_PINx,1);
    power_down_set_pinx(LED_YELLOW_GPIO,LED_YELLOW_PINx,1);
    s_DelayUs(10);

//    GPIO_InitTypeDef gpio_init;
//    gpio_init.GPIOx = KB_GPIO_INDEPENDENT0;
//    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL;
//    gpio_init.PORT_Pin = KB_PINx_INDEPENDENT0;
//    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(KB_PINx_INDEPENDENT0);
//    hw_gpio_init(&gpio_init);
//    hw_gpio_reset_bits(KB_GPIO_INDEPENDENT0,(1<<KB_PINx_INDEPENDENT0));
    gpio_set_output(KB_PTx_INDEPENDENT0,GPIO_OUTPUT_SlewRateFast,0); //关机时要拉低关机键pb11 否则无法下电03012016 
    s_DelayUs(10);

    /*下电毛刺特殊处理:END*/
    hw_gpio_reset_bits(POWER_KEEP_GPIO,(1<<POWER_KEEP_PINx));

    s_DelayMs(1000);
    NVIC_SystemReset();
}


void power_keep_on(void)
{
    hw_gpio_set_bits(POWER_KEEP_GPIO,(1<<POWER_KEEP_PINx));
}

void power_keep_off(void)
{
    hw_gpio_reset_bits(POWER_KEEP_GPIO,(1<<POWER_KEEP_PINx));
    s_DelayMs(100);
}

/*  初始化检测关机口线
 *
 *
 *
 */
void power_down_check_init(void)
{
    gpio_set_input(POWER_DOWN_CHECK_PTx,PORT_Mode_IN_PDIS);
//    
//    GPIO_InitTypeDef gpio_init;
//
//    gpio_init.GPIOx = POWER_DOWN_CHECK_GPIO;
//    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PDIS;
//    gpio_init.PORT_Pin =  POWER_DOWN_CHECK_PINx;
//    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(POWER_DOWN_CHECK_PINx);
//    hw_gpio_init(&gpio_init);

}
void power_down_switch_set(uchar value)
{
    if(value == ON || value == OFF)
    {
        power_down_check_switch = value;
    }
}
/* 检测关机口线
 *
 *
 *
 */
extern uint sys_GetCounter(void);
void power_down_check(void)
{
   if(power_down_check_switch == ON)
   {
       if(hw_gpio_get_bit(POWER_DOWN_CHECK_GPIO, 1<<POWER_DOWN_CHECK_PINx) == 1)
       {
           power_down_check_count++;
           if(power_down_check_count >= POWER_DOWN_CHECK_TIME)
           {
               power_keep_down();
           }
       }
       else
       {
           if(power_down_check_count >= POWER_RESET_CHECK_TIME && sys_GetCounter() > POWER_START_CHECK_TIME)
           {
               NVIC_SystemReset();
           } 
           power_down_check_count = 0;
       }
   }
}
/*  初始化休眠唤醒口线
 *
 *
 *
 */
void power_wake_up_init(void)
{
    gpio_set_input(POWER_WAKE_UP_PTx,PORT_Mode_IN_PDIS);
    
//    GPIO_InitTypeDef gpio_init;
//
//    
//    gpio_init.GPIOx = POWER_WAKE_UP_GPIO;
//    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PDIS;
//    gpio_init.PORT_Pin =  POWER_WAKE_UP_PINx;
//    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(POWER_WAKE_UP_PINx);
//    hw_gpio_init(&gpio_init);

}
/* 检测休眠唤醒口线口线
 *
 *
 *
 */
//0被按下  1-未被按下
uint8_t get_wakeup_io_state(void)
{
    power_wake_up_init();
    return hw_gpio_get_bit(POWER_WAKE_UP_GPIO,1<<POWER_WAKE_UP_PINx);
}

void power_wake_up_check(void)
{
   if(hw_gpio_get_bit(POWER_WAKE_UP_GPIO,1<<POWER_WAKE_UP_PINx) == 0)
   {
       power_wake_up_count++;
       if(power_wake_up_count >= POWER_WAKE_UP_CHECK_TIME)
       {
            power_keep_down();
       }
   }
   else
   {
       power_wake_up_count = 0;
   }

}

