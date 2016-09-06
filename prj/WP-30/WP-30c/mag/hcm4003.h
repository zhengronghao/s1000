/*
 * =====================================================================================
 *
 *       Filename:  Magtek.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  23/9/2015 
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ty
 *        Company:  START
 *
 * =====================================================================================
 */
#ifndef _HCM4003_H_
#define _HCM4003_H_ 



#define HCM_TIM FTM0
#define HCM_CS_GPIO GPIOD
#define HCM_CS_PIN GPIO_Pin_6
#define HCM_POWER_PTx      PTD6
#define HCM_POWER_PINx     PORT_Pin_6 
#define HCM_CH2_GPIO GPIOC
#define HCM_CH2_PIN GPIO_Pin_2
#define HCM_CH3_GPIO GPIOC
#define HCM_CH3_PIN GPIO_Pin_3

//FTM0_IRQn  定时器0中断

#define MAGMOD	0x10000   //每0x10000个时钟中断一次
#define TIME_DATA_MAX 10  //存储 脉宽时间 数组长度

typedef struct
{
	uchar present;                     //当前位置
	uchar len;                         //数据长度
	uint data[TIME_DATA_MAX];  
}Time_width;

typedef struct 
{
	uchar swipe_flag;          //数据转换完成标志 脉宽数据转换为01数据--第一次解码
	volatile uchar fisrtrun;            //检测到刷卡标志
	uint t_irq;                //定时器溢出中断次数
	uint irq_last;             //最后一IO中断时的定时器中断次数
	uint err;
	
}Mag_TimeInfo;               //解码时定时器相关结构

typedef struct
{  
	uchar byte_bit;           //第一轮解码时 用来标志解码后数据在一个字节中的存放位置 
	uchar io_lever;            // 用于io中断，中断电平与上一次中断的电平比较，相同电平不做处理
    uint sample_width;         //磁道参考脉宽

    uint count_last;		   //一磁道最后一次刷卡时间

	Time_width timer_data;    //二磁道存放脉宽结构
	
    uchar   buf[MAG_UINT_LEN]; //解码后的数据
    int     BitOneNum;         //解码后数据长度
}Mag_DecodeInfo;


/*-----------------------------------------------------------------------------}
 *  函数
 *-----------------------------------------------------------------------------{*/
void hcm4003_enable_VCC(void);
void hcm4003_disable_VCC(void);
void hcm4003_data(int mode);
uchar hcm4003_readdata(void);
void hcm4003_strobe(int mode);
int hcm4003_check(void);

int mag_hcm4003_init(int mode);
int mag_hcm4003_open(int mode);
int mag_hcm4003_close(int mode);
int mag_hcm4003_read(void *Track1, void *Track2, void *Track3);
int mag_hcm4003_ioctl(int mode, int para);
int mag_hcm4003_main (int mode);

#endif



