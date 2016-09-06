/*********************************************************************
* 版权所有 (C)2008, 福建实达电脑设备有限公司
* 文件名称：
*
* 当前版本：
*     1.0
* 内容摘要：
*
* 历史纪录：
*     修改人		 日期		    	 版本号       修改记录
*     yehf                                           F06-2,SAM2和大卡相连，SAM1独立。CLK由VCC引脚控制
******************************************************************/

#include "wp30_ctrl.h"

#ifdef CFG_ICCARD
/*-----------------------------------------------------------------------------}
 *  定时器分配  基准时钟50M/16=3.125M
 *  0       1         2
 *  PWM    延时时钟  超时时钟
 *  FTM2   PIT3      PIT2
 *-----------------------------------------------------------------------------{*/
#define ICC_PWM_TIMER_ID			FTM0// 用于产生PWM信号的IC卡时钟
#define ICC_PWM_TIMER_ID1			FTM3// 用于产生PWM信号的IC卡时钟
#define ICC_INTER_TIMER_ID			PIT3// 用于IC卡1模块内部定时，延时操作的定时器ID
#define ICC_ETU_TIMER_ID			PIT2// 用于产生ETU周期性中断定时器

#define ICC_BASE_TIME               20  //基准时钟16分频
//#define ICC_CLK_FREQ_HZ             (3125*1000)
#define ICC_CLK_FREQ_HZ             ((PLL0_VDIV*2*1000*1000)/ICC_BASE_TIME) //48M为3M 50M为3.125M 03192015chenf

#define OFFSET0				30			// 定时纠正


void icc_DisableIOInt(void);
int icc_chip8035_init (ICC_GpioInfo_t *p,int mode);
int icc_chip8035_write (ICC_GpioInfo_t *p,int mode,int value);
int icc_chip8035_read (ICC_GpioInfo_t *p,int mode, int *param);
/*-----------------------------------------------------------------------------}
 *  全局变量
 *-----------------------------------------------------------------------------{*/
static const ICC_GpioInfo_t g_IccGpioInfo = {
    .icc_gpio_io  =(uchar) GPIO_ID_IC_IO, 
    .icc_gpio_rst =(uchar) GPIO_ID_IC_RST,
    .icc_gpio_int =(uchar) GPIO_ID_IC_INT,
    .icc_gpio_clk =(uchar) GPIO_ID_IC_CLK, //
    .icc_gpio_sel1=(uchar) GPIO_ID_IC_SEL1, //1.8V
    .icc_gpio_sel2=(uchar) GPIO_ID_IC_SEL2, //3/5V 
    .icc_gpio_sel3=(uchar) GPIO_ID_IC_SEL3, // cmd
    .sam1_gpio_io =(uchar) GPIO_ID_SAM1_IO,
    .sam2_gpio_io =(uchar) GPIO_ID_SAM2_IO,
    .sam1_gpio_rst=(uchar) GPIO_ID_SAM1_RST,
    .sam2_gpio_rst=(uchar) GPIO_ID_SAM2_RST,
    .sam_gpio_clk =(uchar) GPIO_ID_SAM_CLK,
    .sam_gpio_sel1=(uchar) GPIO_ID_SAM_CS1,
    .sam_gpio_sel2=(uchar) GPIO_ID_SAM_CS2,
    .sam1_gpio_vcc=(uchar) GPIO_ID_SAM1_PWR,
    .sam2_gpio_vcc=(uchar) GPIO_ID_SAM2_PWR
};

static const ICC_CHIP_FOPS_t g_Chip_fops[] = {
    {
        .chiptype = ICC_CHIP_8035,
        .init = icc_chip8035_init,
        .write = icc_chip8035_write,
        .read = icc_chip8035_read,
    },
    {
        .chiptype = ICC_CHIP_GPIO,
        .init = icc_gpioctrl_init,
        .write = icc_gpioctrl_write,
        .read = icc_gpioctrl_read,
    },
};
ICC_HalInfo_t gIccHalInfo UPAREA;
volatile ICCWorkInfo gl_CardInfo UPAREA;
volatile UINT64 gl_char_wait_etu;
volatile UINT64 gl_total_atr_etu;
volatile uchar k_ICC_CardInSert = 0;//1-按插卡事件产生


/*-----------------------------------------------------------------------------}
 *  函数定义
 *-----------------------------------------------------------------------------{*/
int get_iccard_Ver(void)
{
    int i;
    gIccHalInfo.version = 1;
    gIccHalInfo.samslots_inside = 2;
    gIccHalInfo.samslots_outside = 0;
    gIccHalInfo.chiptype = ICC_CHIP_8035;
    gIccHalInfo.pGpioInfo = (ICC_GpioInfo_t *)&g_IccGpioInfo;
    for ( i=0 ; i<DIM(g_Chip_fops) ; i++  ) {
        if ( (uchar)g_Chip_fops[i].chiptype ==  gIccHalInfo.chiptype) {
            gIccHalInfo.pChipFops = (ICC_CHIP_FOPS_t *)&g_Chip_fops[i];
            break;
        }
    }
    return gIccHalInfo.version;
}

/*-----------------------------------------------------------------------------}
 *  sam卡座硬件操作
 *-----------------------------------------------------------------------------{*/
void sam1_write_vcc(int data)
{
    hw_iccard_gpio_write(g_IccGpioInfo.sam1_gpio_vcc,data);        
}
void sam2_write_vcc(int data)
{
    hw_iccard_gpio_write(g_IccGpioInfo.sam2_gpio_vcc,data);        
}
void sam_write_vcc(int slot, int data)
{
    switch ( slot )
    {
    case SAM1SLOT :
        sam1_write_vcc(data);
        break;
    case SAM2SLOT :
        sam2_write_vcc(data);
        break;
    default :
        break;
    }
}

void sam1_write_rst(int data)
{
    hw_iccard_gpio_write(g_IccGpioInfo.sam1_gpio_rst,data);        
}
void sam2_write_rst(int data)
{
    hw_iccard_gpio_write(g_IccGpioInfo.sam2_gpio_rst,data);        
}
void sam_write_rst(int slot, int data)
{
    switch ( slot )
    {
    case SAM1SLOT :
        sam1_write_rst(data);
        break;
    case SAM2SLOT :
        sam2_write_rst(data);
        break;
    default :
        break;
    }
}

void sam1_write_io(int data)
{
    hw_iccard_gpio_write(g_IccGpioInfo.sam1_gpio_io,data);        
}
void sam2_write_io(int data)
{
    hw_iccard_gpio_write(g_IccGpioInfo.sam2_gpio_io,data);        
}
void sam_write_io(int slot, int data)
{
    switch ( slot )
    {
    case SAM1SLOT :
        sam1_write_io(data);
        break;
    case SAM2SLOT :
        sam2_write_io(data);
        break;
    default :
        break;
    }
}
// mode=0 输入态 data:电阻上下拉 0-下拉   1-上拉   2-不配置 
// mode=1 输出态 data:输出状态   0-低电平 1-高电平 2-不配置
void sam1_set_io(int mode, int data)
{
    hw_iccard_gpio_ioctl(g_IccGpioInfo.sam1_gpio_io,mode,data);        
}
void sam2_set_io(int mode, int data)
{
    hw_iccard_gpio_ioctl(g_IccGpioInfo.sam2_gpio_io,mode,data);        
}
void sam_set_io(int slot, int mode, int data)
{
    switch ( slot )
    {
    case SAM1SLOT :
        sam1_set_io( mode,  data);
        break;
    case SAM2SLOT :
        sam2_set_io( mode,  data);
        break;
    default :
        break;
    }
}

uint sam1_read_io(void)
{
    return (uint)hw_iccard_gpio_read(g_IccGpioInfo.sam1_gpio_io);        
}
uint sam2_read_io(void)
{
    return (uint)hw_iccard_gpio_read(g_IccGpioInfo.sam2_gpio_io);        
}
uint sam_read_io(int slot)
{
    switch ( slot )
    {
    case SAM1SLOT :
        return sam1_read_io();
    case SAM2SLOT :
        return sam2_read_io();
    default :
        break;
    }
    return 0;
}

void sam_write_cs(int samslot)
{
    if ( samslot == 0 ) {
        hw_iccard_gpio_write(g_IccGpioInfo.sam_gpio_sel1,0);        
        hw_iccard_gpio_write(g_IccGpioInfo.sam_gpio_sel2,0);        
    } else if(samslot == 1){
        hw_iccard_gpio_write(g_IccGpioInfo.sam_gpio_sel1,1);        
        hw_iccard_gpio_write(g_IccGpioInfo.sam_gpio_sel2,0);        
    } else if(samslot == 2){
        hw_iccard_gpio_write(g_IccGpioInfo.sam_gpio_sel1,0);        
        hw_iccard_gpio_write(g_IccGpioInfo.sam_gpio_sel2,1);        
    } else {
        hw_iccard_gpio_write(g_IccGpioInfo.sam_gpio_sel1,1);        
        hw_iccard_gpio_write(g_IccGpioInfo.sam_gpio_sel2,1);        
    }
}
/*-----------------------------------------------------------------------------}
 *  卡座操作硬件操作
 *-----------------------------------------------------------------------------{*/
// 拉高VCC
void icc_SetVCC(int VolMode)
{
	// 配置VCC口线，输出高电平
	switch(gl_CardInfo.current_slot)
	{
	case USERCARD:
        gIccHalInfo.pChipFops->write(gIccHalInfo.pGpioInfo,VCC_MODE,VolMode);
		break;
	default:
        sam_write_vcc(gl_CardInfo.current_slot, 1);
		break;
	}
    //add
    s_DelayUs(10);
}

// 拉低VCC
void icc_ClrVCC(void)
{
	// 配置VCC口线，输出低电平
	switch(gl_CardInfo.current_slot)
	{
	case USERCARD:
        gIccHalInfo.pChipFops->write(gIccHalInfo.pGpioInfo,VCC_MODE,0);
		break;
	default:
        sam_write_vcc(gl_CardInfo.current_slot, 0);
		break;
    }
}

// 置高Reset引脚
void icc_SetRST(void)
{
    int value;
    value = 1;
	switch(gl_CardInfo.current_slot)
	{
	case USERCARD:
        gIccHalInfo.pChipFops->write(gIccHalInfo.pGpioInfo,WRITE_RST,value);
		break;
	default:
        sam_write_rst(gl_CardInfo.current_slot, value);
		break;
    }
}

// 拉低RST引脚
void icc_ClrRST(void)
{
    int value;

    value = 0;
    switch(gl_CardInfo.current_slot)
	{
	case USERCARD:
        gIccHalInfo.pChipFops->write(gIccHalInfo.pGpioInfo,WRITE_RST,value);
		break;
	default:
        sam_write_rst(gl_CardInfo.current_slot, value);
		break;
    }
}

// 设置IO口为输出态
void icc_SetIOOutput(void)
{
	switch(gl_CardInfo.current_slot)
	{
	case USERCARD:
        gIccHalInfo.pChipFops->write(gIccHalInfo.pGpioInfo,SET_IO_OUTPUT,0);
		break;
	default:
        sam_set_io(gl_CardInfo.current_slot,1,2);
		break;
    }
}
// 设置IO口为输入态
void icc_SetIOInput(void)
{
	switch(gl_CardInfo.current_slot)
	{
	case USERCARD:
        gIccHalInfo.pChipFops->write(gIccHalInfo.pGpioInfo,SET_IO_INPUT,0);
		break;
	default:
        sam_set_io(gl_CardInfo.current_slot,0,1);
		break;
    }
}

// 获取IO口线电平
static uint icc_GetIO(void)
{
    int ret;
    if (gl_CardInfo.current_slot) {
        ret = sam_read_io(gl_CardInfo.current_slot);
        return ret;
    } else {
        gIccHalInfo.pChipFops->read(gIccHalInfo.pGpioInfo,ASYNCLK_RD_IO,&ret);
        return ret;
    }
}

// IO口线置高
void icc_SetIO(void)
{
    if (gl_CardInfo.current_slot) {
        sam_write_io(gl_CardInfo.current_slot, 1);
    } else {
        gIccHalInfo.pChipFops->write(gIccHalInfo.pGpioInfo,WRITE_IO,1);
    }
}

// IO口线置低
void icc_ClrIO(void)
{
    if (gl_CardInfo.current_slot) {
        sam_write_io(gl_CardInfo.current_slot, 0);
    } else {
        gIccHalInfo.pChipFops->write(gIccHalInfo.pGpioInfo,WRITE_IO,0);
    }
}

void icc_open_clk(void)
{
//    TRACE("\r\n ==icc_open_clk==");
    // PWM0采用64分频,占空比50%,频率=3.125M
    FTM_InitTypeDef ftm_init_struct;
    memset((char *)&ftm_init_struct,0,sizeof(ftm_init_struct));
    ftm_init_struct.FTM_Ftmx = ICC_PWM_TIMER_ID;	//使能FTM0通道
    ftm_init_struct.FTM_Mode = FTM_MODE_PWM;	    //使能PWM模式
//    ftm_init_struct.FTM_PwmFreq = core_clk_mhz/ICC_BASE_TIME;	            //PWM频率3.125MHz
    ftm_init_struct.FTM_PwmFreq = ICC_CLK_FREQ_HZ;	            //PWM频率3.125MHz

    LPLD_FTM_Init(ftm_init_struct);

    ftm_init_struct.FTM_Ftmx = ICC_PWM_TIMER_ID1;	//使能FTM3通道
    ftm_init_struct.FTM_Mode = FTM_MODE_PWM;	    //使能PWM模式
//    ftm_init_struct.FTM_PwmFreq = core_clk_mhz/ICC_BASE_TIME;	            //PWM频率3.125MHz
    ftm_init_struct.FTM_PwmFreq = ICC_CLK_FREQ_HZ;	            //PWM频率3.125MHz

    LPLD_FTM_Init(ftm_init_struct);

    if ( gl_CardInfo.current_slot == USERCARD ) {
        LPLD_FTM_PWM_Enable(ICC_PWM_TIMER_ID, //使用FTM0
                            FTM_Ch2, //使能Ch2通道
                            FTM_PWM_DUTY_MAX/2, //占空比
                            IC_CLK_PTx, //使用Ch2通道的PTA5引脚
                            ALIGN_LEFT        //脉宽左对齐
                           );    
    } else {
        LPLD_FTM_PWM_Enable(ICC_PWM_TIMER_ID1, //使用FTM3
                            FTM_Ch1, //使能Ch1通道
                            FTM_PWM_DUTY_MAX/2, //占空比
                            SAM_CLK_PTx, //使用Ch1通道的PTD1引脚
                            ALIGN_LEFT);        //脉宽左对齐
    }
}

void icc_close_clk(void)
{
//    TRACE("\r\n ==icc_close_clk==");
    int value;
    
    value = 0;
    if ( gl_CardInfo.current_slot == USERCARD ) {
        hw_iccard_gpio_ioctl(g_IccGpioInfo.icc_gpio_clk,1,value);
        LPLD_FTM_DisableChn(ICC_PWM_TIMER_ID,FTM_Ch2); 
//        hw_iccard_gpio_ioctl(g_IccGpioInfo.icc_gpio_clk,1,1);
    } else {
        hw_iccard_gpio_ioctl(g_IccGpioInfo.sam_gpio_clk,1,value);
        LPLD_FTM_DisableChn(ICC_PWM_TIMER_ID1,FTM_Ch1); 
//        hw_iccard_gpio_ioctl(g_IccGpioInfo.sam_gpio_clk,1,1);
    }
}

/*-----------------------------------------------------------------------------}
 *  定时器操作相关
 *-----------------------------------------------------------------------------{*/
//计数定时器
void icc_etu_timer_isr(void)
{
//SETSIGNAL1_H();
//    TRACE("\r\n etu_timeout");
	if(gl_char_wait_etu)
	{
		if(--gl_char_wait_etu == 0)
		{
			gl_CardInfo.k_IccComErr = 1;
			gl_CardInfo.k_timeover = 1;
			gl_CardInfo.k_IccErrTimeOut = 1;
			icc_DisableIOInt();
//LABLE(D_OUT);
		}
	}

	if(gl_total_atr_etu)
	{
		if(--gl_total_atr_etu == 0)
		{
			gl_CardInfo.k_IccComErr = 1;
			gl_CardInfo.k_total_timeover = 1;
	    	gl_CardInfo.k_IccErrTimeOut = 1;
			icc_DisableIOInt();
//LABLE(D_ATR_OUT);
		}
	}
//    SETSIGNAL2_L();
//SETSIGNAL1_L();
}

void icc_init_etu_timer(void)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = ICC_ETU_TIMER_ID;
    pit.PIT_Period = UINT_MAX; 
    pit.PIT_Isr = icc_etu_timer_isr;
    LPLD_PIT_Init(&pit);
}

void icc_start_etu_timer(int clk)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = ICC_ETU_TIMER_ID;
    pit.PIT_Isr = icc_etu_timer_isr;
    pit.PIT_Period = ICC_BASE_TIME*(uint)clk; 
    LPLD_PIT_Restart(&pit);
}

void icc_close_etu_timer(void)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = ICC_ETU_TIMER_ID;
    pit.PIT_Isr = icc_etu_timer_isr;
    LPLD_PIT_Deinit(&pit);
}

/////////////////////////////////////////////////////////////////////////////////////
// 接收或发送时延迟定时器
// 基准时钟50M 16分频 3.125M = 320ns
void icc_init_inter_timer(void)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = ICC_INTER_TIMER_ID;
    pit.PIT_Period = UINT_MAX;
    pit.PIT_Isr = NULL;
    LPLD_PIT_Init(&pit);
}

void icc_delay_clock(int clock)
{
    uint count;
    PIT_InitTypeDef pit;

    count = ICC_BASE_TIME*(uint)clock;
    pit.PIT_Pitx = ICC_INTER_TIMER_ID;
    pit.PIT_Period = UINT_MAX;
    pit.PIT_Isr = NULL;
//    LPLD_PIT_Delay(count);
    LPLD_PIT_Restart(&pit);
    while(1)
    {
        if ( UINT_MAX - LPLD_PIT_GetValue(ICC_INTER_TIMER_ID) >= count ) {
            break;
        }
    }
}

void icc_delay_etu(int etu, int num)
{
    uint count;
    PIT_InitTypeDef pit;
    count = ICC_BASE_TIME*(uint)etu*(uint)num;
    pit.PIT_Pitx = ICC_INTER_TIMER_ID;
    pit.PIT_Period = UINT_MAX;
    pit.PIT_Isr = NULL;
    LPLD_PIT_Restart(&pit);
    while(1)
    {
        if ( UINT_MAX - LPLD_PIT_GetValue(ICC_INTER_TIMER_ID) >= count ) {
            break;
        }
    }
}

void icc_delay_inter_timer_cnt(uint cnt)
{
    uint count;
    count = ICC_BASE_TIME*cnt;
    while(1)
    {
        if ( UINT_MAX - LPLD_PIT_GetValue(ICC_INTER_TIMER_ID) >= count ) {
            break;
        }
    }
}
void icc_disable_inter_timer(void)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = ICC_INTER_TIMER_ID;
    LPLD_PIT_Deinit(&pit);
}

static void icc_StartCharTimer(void)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = ICC_INTER_TIMER_ID;
    pit.PIT_Period = UINT_MAX;
    pit.PIT_Isr = NULL;
    LPLD_PIT_Restart(&pit);
}

//////////////////////////////////////////////////////////////////////////////////
void icc_EnableIOInt(void)
{
	switch(gl_CardInfo.current_slot)
	{
	case USERCARD:
        hw_iccard_gpio_ioctl(gIccHalInfo.pGpioInfo->icc_gpio_io,0,1);
		break;
	case SAM1SLOT:
        hw_iccard_gpio_ioctl(gIccHalInfo.pGpioInfo->sam1_gpio_io,0,1);
        break;
    case SAM2SLOT:
        hw_iccard_gpio_ioctl(gIccHalInfo.pGpioInfo->sam2_gpio_io,0,1);
        break;
	default:
		break;
	}
}

void icc_DisableIOInt(void)
{
	switch(gl_CardInfo.current_slot)
	{
	case 0:
//        disable_pio_interrupt(gIccHalInfo.pGpioInfo->icc_gpio_io);
//		disable_pio_output(gIccHalInfo.pGpioInfo->icc_gpio_io);
        hw_iccard_gpio_ioctl(gIccHalInfo.pGpioInfo->icc_gpio_io,0,0);
		break;
    case SAM1SLOT:
        hw_iccard_gpio_ioctl(gIccHalInfo.pGpioInfo->sam1_gpio_io,0,0);
        break;
    case SAM2SLOT:
        hw_iccard_gpio_ioctl(gIccHalInfo.pGpioInfo->sam2_gpio_io,0,0);
        break;
	default:
		break;
	}
}
void usercard_insert_int(void)
{
	int stat;
    gIccHalInfo.pChipFops->read(gIccHalInfo.pGpioInfo,CARD_DETECT,&stat);
    LABLE(0x99);DATAIN(gIccHalInfo.curvcc);DATAIN(stat);
    if(stat)
    {   //卡插入
        gl_CardSlotInfo[0].CardInsert = 1;
        k_ICC_CardInSert = 1;
        if ( gIccHalInfo.curvcc == 0xFF ) {
            // 退出低功耗产生中断
            //usb调试信息会死机
//            TRACE("\r\n 低功耗CardIn");
        } else {
//            TRACE("\r\n CardIn");
        }
    }
    else
    {
        /* 拔卡后，清卡图标，并重新设置卡状态变量      */
        gl_CardSlotInfo[0].CardPowerOn = 0;
        gl_CardSlotInfo[0].CardInsert = 0;
        k_ICC_CardInSert = 0;
        if ( gIccHalInfo.curvcc == 0xFF ) {
            // 退出低功耗产生中断
//            TRACE("\r\n 低功耗CardOut");
        } else {
#ifdef CFG_ASYNCARD
            if (icc_get_usercard_workmode()) {
                //同步卡在使用,拔卡
                gtSynCardStatus.ucPowerON = 0;
                gtSynCardStatus.ucPswPassed = 0;
            }
#endif
            icc_ClrVCC();
//            TRACE("\r\n CardOut");
        }
        gIccHalInfo.curvcc = 0;
    }
}

void icc_EnableCardInsertInt(void)
{
//    hw_iccard_gpio_ioctl(gIccHalInfo.pGpioInfo->icc_gpio_int,0,2);
//    hw_iccard_gpio_ioctl(gIccHalInfo.pGpioInfo->icc_gpio_int,0,1);

    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = IC_INT_GPIO;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_IN_PU
        |PORT_Mode_IN_PFE|PORT_Mode_IRQ_EXTI_EEdge;
    gpio_init.PORT_Pin = IC_INT_PINx;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(gpio_init.PORT_Pin);
    hw_gpio_init(&gpio_init);

    set_irq_priority (IC_INT_IRQn, INT_PRI_ICCARD_INSERT);
    enable_irq(IC_INT_IRQn);

//	set_pio_input(gIccHalInfo.pGpioInfo->icc_gpio_int, 1);
//	requst_pio_interrupt(gIccHalInfo.pGpioInfo->icc_gpio_int,INT_EDGE, 3,usercard_insert_int); // 插拔卡中断
//	enable_pio_interrupt(gIccHalInfo.pGpioInfo->icc_gpio_int);
}

void icc_DisableCardInsertInt(void)
{
//	disable_pio_interrupt(gIccHalInfo.pGpioInfo->icc_gpio_int);
}


void icc_CardIOIsr(void)
{
	uchar temp_p;
//DATAIN(0x88);	
	if(icc_GetIO())
	{
		return;	
	}
	gl_char_wait_etu = gl_CardInfo.k_Ic_CharWait_TimeCount;   // 重新定时字符间等待间隔
	// 启动接收定时
	icc_StartCharTimer();
	icc_DisableIOInt();	
	
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0xFF)
	{
		gl_total_atr_etu = gl_CardInfo.k_IC_TotalATRTime;	// 定义总的ATR定时
		icc_close_etu_timer();
		icc_start_etu_timer(gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);		
	}

	if(gl_CardInfo.k_IccComErr)
	{
		return;
	}
	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P15 - OFFSET0));		
	
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 == 0x14 
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode== SHBMODE
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].CardPowerOn == 1)
	{	
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P15);
	}
	else
	{
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P15 - OFFSET0);
	}
	// 接收BIT0
	if(icc_GetIO())
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS != 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT0;
		}
		gl_CardInfo.k_IccP_bit++;
	}
	else
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT7;
		}
	}
	
	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P25 - OFFSET0));	
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 == 0x14 
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode== SHBMODE
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].CardPowerOn == 1)
	{	
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P25);
	}
	else
	{
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P25 - OFFSET0);
	}
	// 接收BIT1
	if(icc_GetIO())
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS != 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT1;
		}
		gl_CardInfo.k_IccP_bit++;
	}
	else
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT6;
		}
	}

	// 接收BIT2
	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P35 - OFFSET0));	
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 == 0x14 
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode== SHBMODE
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].CardPowerOn == 1)
	{	
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P35);
	}
	else
	{
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P35 - OFFSET0);
	}
	if(icc_GetIO())
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS != 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT2;
		}
		gl_CardInfo.k_IccP_bit++;
	}
	else
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT5;
		}
	}
	
	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P45 - OFFSET0));
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 == 0x14 
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode== SHBMODE
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].CardPowerOn == 1)
	{	
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P45);
	}
	else
	{
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P45 - OFFSET0);
	}
	// 接收BIT3
	if(icc_GetIO())
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS != 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT3;
		}
		gl_CardInfo.k_IccP_bit++;
	}
	else
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT4;
		}
	}	
	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P55 - OFFSET0));
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 == 0x14 
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode== SHBMODE
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].CardPowerOn == 1)
	{	
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P55);
	}
	else
	{
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P55 - OFFSET0);
	}
	// 接收BIT4
	if(icc_GetIO())
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS != 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT4;
		}
		gl_CardInfo.k_IccP_bit++;
	}
	else
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT3;
		}
	}
	
	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P65 - OFFSET0));
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 == 0x14 
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode== SHBMODE
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].CardPowerOn == 1)
	{	
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P65);
	}
	else
	{
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P65 - OFFSET0);
	}
	// 接收BIT5
	if(icc_GetIO())
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS != 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT5;
		}
		gl_CardInfo.k_IccP_bit++;
	}
	else
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT2;
		}
	}
	
	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P75 - OFFSET0));
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 == 0x14 
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode== SHBMODE
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].CardPowerOn == 1)
	{	
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P75);
	}
	else
	{
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P75 - OFFSET0);
	}
	// 接收BIT6
	if(icc_GetIO())
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS != 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT6;
		}
		gl_CardInfo.k_IccP_bit++;
	}
	else
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT1;
		}
	}

	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P85 - OFFSET0));	
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 == 0x14 
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode== SHBMODE
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].CardPowerOn == 1)
	{	
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P85);
	}
	else
	{
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P85 - OFFSET0);
	}
	// 接收BIT7
	if(icc_GetIO())
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS != 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT7;
		}
		gl_CardInfo.k_IccP_bit++;
	}
	else
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3F)
		{
			gl_CardInfo.k_IccReceByte |= BIT0;
		}
	}

	// 接收奇偶位	
	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P95 - OFFSET0));	
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 == 0x14 
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode== SHBMODE
		&& gl_CardSlotInfo[gl_CardInfo.current_slot].CardPowerOn == 1)
	{	
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P95);
	}
	else
	{
		icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P95 - OFFSET0);
	}

	temp_p = icc_GetIO();	

	gl_CardInfo.k_IccP_bit &= 1;
DATAIN(gl_CardInfo.k_IccReceByte);
//DATAIN(temp_p&gl_CardInfo.k_IccP_bit);
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0xFF)
	{
		if(gl_CardInfo.k_IccReceByte == 0x03)
		{
			gl_CardInfo.k_IccReceByte = 0x3F;
			gl_CardSlotInfo[gl_CardInfo.current_slot].TS = 0x3F;
		}
		else if(gl_CardInfo.k_IccReceByte == 0x3B)
		{
			gl_CardSlotInfo[gl_CardInfo.current_slot].TS = 0x3B;
		}
		else		// TS错误，终结操作
		{
			//当第1个ATR字符不是3B/3F时，会关闭超时定时器，导致RecvByte无法退出
			//因为退出条件是gl_CardInfo.k_IccComErr=1且gl_CardInfo.k_IccErrTimeOut=1
			icc_close_etu_timer();
			icc_disable_inter_timer();
			gl_char_wait_etu = 0;
			gl_total_atr_etu = 0;
			gl_CardInfo.k_IccComErr = 1;		
			return;
		}
	}

	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3B)
	{
		if(temp_p != gl_CardInfo.k_IccP_bit)
		{
			//反向数据错误
			gl_CardInfo.k_IccComErr = 1;
			gl_CardInfo.k_IccErrPar = 1;
		}
	}
	else if(temp_p == gl_CardInfo.k_IccP_bit)
	{
		//正向数据错误
		gl_CardInfo.k_IccComErr = 1;
		gl_CardInfo.k_IccErrPar = 1;
	}

	if(gl_CardInfo.k_IccComErr == 0)
	{
		gl_CardInfo.k_IccReceByteStatus = 0xFF;		
		return;
	}
	else if(gl_CardInfo.Parity_Check_Enable == 0x00)
	{
		//T=1和ATR时不校验
		gl_CardInfo.k_IccReceByteStatus = 0xFF;
		return;
	}
	else if(gl_CardInfo.k_IccErrPar == 0)
	{
		gl_CardInfo.k_IccReceByteStatus = 0xFF;
	}	
	else
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
		{
			//奇偶错误纠错
			if(gl_CardInfo.k_Iccrecetimes > 2)
			{
				gl_CardInfo.k_IccReceByteStatus = 0xFD;
				icc_close_etu_timer();
				icc_disable_inter_timer();
				gl_char_wait_etu = 0;
				gl_total_atr_etu = 0;
				return;
			}
		}
		else
		{
			//奇偶错误纠错
			if(gl_CardInfo.k_Iccrecetimes > 3)
			{
				gl_CardInfo.k_IccReceByteStatus = 0xFD;
				icc_close_etu_timer();
				icc_disable_inter_timer();
				gl_char_wait_etu = 0;
				gl_total_atr_etu = 0;
				return;
			}
		}
	}

	// 奇偶位校验错误，要求卡片重发该字符
	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P105 - OFFSET0));	
	icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P105 - OFFSET0);
	icc_SetIOOutput();
	icc_ClrIO();

	// 延时1.5个ETU后，重新置为输入态
	//while(icc_get_inter_timer_cnt() < (gl_CardInfo.Rec_P120 - OFFSET0));	
	icc_delay_inter_timer_cnt(gl_CardInfo.Rec_P120 - OFFSET0);
	gl_CardInfo.k_Iccrecetimes++;
	icc_SetIO();
	icc_SetIOInput();
	gl_CardInfo.k_IccReceByteStatus = 0xFE;
}


void icc_SamOff(int CardSlot)
{
	int i;
    int samslotmax = (SAM1SLOT+gIccHalInfo.samslots_inside);
	for(i=SAM1SLOT; i<samslotmax; i++)
	{
		if(gl_CardSlotInfo[i].CardPowerOn && i!= CardSlot)
		{
			gl_CardSlotInfo[i].CardPowerOn = 0x00;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void icc_SelectSlot(int CardSlot)
{
    gl_CardInfo.current_slot = CardSlot;
    if ( CardSlot != USERCARD ) {
//        sam_write_cs(CardSlot-1);
		icc_SamOff(CardSlot);
    } else {
    }
}

void icc_CpuCardInit(void)
{
#ifdef CFG_ICCARD_USERCARD
	gl_CardInfo.current_slot = 0;
	gl_CardSlotInfo[0].CardPowerOn = 0;
    gIccHalInfo.pChipFops->init(gIccHalInfo.pGpioInfo,0);
#endif
}


void icc_SamCardInit( void )
{
    hw_sam_gpio_init(0);
    set_irq_priority (PIT2_IRQn, INT_PRI_ICCARD_TIMEOUT);
    enable_irq(PIT2_IRQn);
}
// 0-异步卡 1-同步卡
int icc_get_usercard_workmode(void)
{
    return gIccHalInfo.usercard_stat;
}


// 接收一个字节
uchar icc_RecvByte(void)
{
	ushort l = 0;

	gl_CardInfo.k_Iccrecetimes = 0;

receiv1:

	gl_CardInfo.k_IccComErr = 0;
	gl_CardInfo.k_IccErrTimeOut = 0;
	gl_CardInfo.k_IccErrPar = 0;
	gl_CardInfo.k_IccReceByte = 0;
	gl_CardInfo.k_IccReceByteStatus = 0;
	gl_CardInfo.k_timeover = 0;
	gl_CardInfo.k_IccP_bit = 0;

	icc_StartCharTimer();
	icc_EnableIOInt();

	while(1)
	{
		//接收完毕
		if(gl_CardInfo.k_IccReceByteStatus == 0xFF)
		{
			break;
		}
		//1次奇偶错误
		else if(gl_CardInfo.k_IccReceByteStatus == 0xFE)
		{
			break;
		}
		//3次奇偶错误
		else if(gl_CardInfo.k_IccReceByteStatus == 0xFD)
		{
			break;
		}

		if(gl_CardInfo.k_IccComErr)
		{
			if(gl_CardInfo.k_IccErrTimeOut == 1
				|| gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0xFF)
			{
				return 0xFF;					// 超时错误
			}
		}
	}

	if(gl_CardInfo.k_IccReceByteStatus == 0xFE)
	{
		goto receiv1;
	}

	if(gl_CardInfo.k_IccReceByteStatus == 0xFD)
	{
		l = 0;
		//等待IO变高,延迟58.67us
		do
		{
			if(icc_GetIO())
			break;
		}while(l++ < 30000);
		return (uchar)-3;					// 数据奇偶错
	}

	l = 0;
	//等待IO变高
	do
	{
		if(icc_GetIO())
		break;
	}while(l++ < 30000);
    icc_DisableIOInt();
	return gl_CardInfo.k_IccReceByte;
}

// 发送一个字节
int icc_SendByte(uchar dat)
{	
	uint i,j,ret=0;
	gl_CardInfo.k_IccSendByte = 0;
	gl_CardInfo.k_Iccsendtimes = 0;
	gl_CardInfo.k_IccP_bit = 0;
//DATAIN(dat);
	// 发送BIT0
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3B)
	{
		if(dat & BIT0)
		{
			gl_CardInfo.k_IccSendByte |= BIT0;
			gl_CardInfo.k_IccP_bit++;
		}
	}
	else
	{
		if(dat & BIT7)
		{
			gl_CardInfo.k_IccP_bit++;
		}
		else
		{
			gl_CardInfo.k_IccSendByte |= BIT0;
		}
	}

	// 发送BIT1
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3B)
	{
		if(dat & BIT1)
		{
			gl_CardInfo.k_IccSendByte |= BIT1;
			gl_CardInfo.k_IccP_bit++;
		}
	}
	else
	{
		if(dat & BIT6)
		{
			gl_CardInfo.k_IccP_bit++;
		}
		else
		{
			gl_CardInfo.k_IccSendByte |= BIT1;
		}
	}

	// 发送BIT2
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3B)
	{
		if(dat & BIT2)
		{
			gl_CardInfo.k_IccSendByte |= BIT2;
			gl_CardInfo.k_IccP_bit++;
		}
	}
	else
	{
		if(dat & BIT5)
		{
			gl_CardInfo.k_IccP_bit++;
		}
		else
		{
			gl_CardInfo.k_IccSendByte |= BIT2;
		}
	}

	// 发送BIT3
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3B)
	{
		if(dat & BIT3)
		{
			gl_CardInfo.k_IccSendByte |= BIT3;
			gl_CardInfo.k_IccP_bit++;
		}
	}
	else
	{
		if(dat & BIT4)
		{
			gl_CardInfo.k_IccP_bit++;
		}
		else
		{
			gl_CardInfo.k_IccSendByte |= BIT3;
		}
	}

	// 发送BIT4
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3B)
	{
		if(dat & BIT4)
		{
			gl_CardInfo.k_IccSendByte |= BIT4;
			gl_CardInfo.k_IccP_bit++;
		}
	}
	else
	{
		if(dat & BIT3)
		{
			gl_CardInfo.k_IccP_bit++;
		}
		else
		{
			gl_CardInfo.k_IccSendByte |= BIT4;
		}
	}

	// 发送BIT5
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3B)
	{
		if(dat & BIT5)
		{
			gl_CardInfo.k_IccSendByte |= BIT5;
			gl_CardInfo.k_IccP_bit++;
		}
	}
	else
	{
		if(dat & BIT2)
		{
			gl_CardInfo.k_IccP_bit++;
		}
		else
		{
			gl_CardInfo.k_IccSendByte |= BIT5;
		}
	}

	// 发送BIT6
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3B)
	{
		if(dat & BIT6)
		{
			gl_CardInfo.k_IccSendByte |= BIT6;
			gl_CardInfo.k_IccP_bit++;
		}
	}
	else
	{
		if(dat & BIT1)
		{
			gl_CardInfo.k_IccP_bit++;
		}
		else
		{
			gl_CardInfo.k_IccSendByte |= BIT6;
		}
	}

	// 发送BIT7
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3B)
	{
		if(dat & BIT7)
		{
			gl_CardInfo.k_IccSendByte |= BIT7;
			gl_CardInfo.k_IccP_bit++;
		}
	}
	else
	{
		if(dat & BIT0)
		{
			gl_CardInfo.k_IccP_bit++;
		}
		else
		{
			gl_CardInfo.k_IccSendByte |= BIT7;
		}
	}
	gl_CardInfo.k_IccP_bit %= 2;
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS == 0x3F)
	{
		if(gl_CardInfo.k_IccP_bit)
			gl_CardInfo.k_IccP_bit = 0;
		else
			gl_CardInfo.k_IccP_bit = 1;
	}
DATAIN(gl_CardInfo.k_IccSendByte);	
	while(1)
	{		
		// 处理TC1	
//SETSIGNAL1_H();		
		//icc_init_inter_timer();			
		i = (uint)(1.5*gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
        if(gl_CardSlotInfo[gl_CardInfo.current_slot].T){
            //T1卡需要调整时间
            i = (uint)(1.5*gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
        }
		if((gl_CardSlotInfo[gl_CardInfo.current_slot].TCFlag & 0x01) == 0x00)
		{
			icc_delay_clock(i);
		}
		else
		{
//			icc_delay_clock(i);
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].T)
			{
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].TC1 != 0xFF)
				{
                    icc_delay_clock(i-60);//发送时间太长微调10092015
					j = (uint)gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
					icc_delay_etu(j, (uint)gl_CardSlotInfo[gl_CardInfo.current_slot].TC1);
				}
			}
			else
			{
                icc_delay_clock(i);
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].TC1 != 0xFF)
				{
					j = (uint)gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
					icc_delay_etu(j, (uint)gl_CardSlotInfo[gl_CardInfo.current_slot].TC1);
				}
			}		
		}			
		// 关总中断
//		s_CloseIsr();

		// 发送起始位
		icc_SetIOOutput();
		icc_ClrIO();
		//icc_set_inter_timer_cnt(0);		
		// 初始化发送定时器
		icc_StartCharTimer();
		
		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P1);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P1);
		// 发送BIT0
		if(gl_CardInfo.k_IccSendByte & BIT0)
		{
			icc_SetIO();
		}
		else
		{
			icc_ClrIO();
		}
		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P2);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P2);
		// 发送BIT1
		if(gl_CardInfo.k_IccSendByte & BIT1)
		{
			icc_SetIO();
		}
		else
		{
			icc_ClrIO();
		}
		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P3);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P3);
		// 发送BIT2
		if(gl_CardInfo.k_IccSendByte & BIT2)
		{
			icc_SetIO();
		}
		else
		{
			icc_ClrIO();
		}
		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P4);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P4);
		// 发送BIT3
		if(gl_CardInfo.k_IccSendByte & BIT3)
		{
			icc_SetIO();
		}
		else
		{
			icc_ClrIO();
		}
		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P5);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P5);
		// 发送BIT4
		if(gl_CardInfo.k_IccSendByte & BIT4)
		{
			icc_SetIO();
		}
		else
		{
			icc_ClrIO();
		}
		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P6);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P6);
		// 发送BIT5
		if(gl_CardInfo.k_IccSendByte & BIT5)
		{
			icc_SetIO();
		}
		else
		{
			icc_ClrIO();
		}
		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P7);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P7);
		// 发送BIT6
		if(gl_CardInfo.k_IccSendByte & BIT6)
		{
			icc_SetIO();
		}
		else
		{
			icc_ClrIO();
		}
		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P8);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P8);
		// 发送BIT7
		if(gl_CardInfo.k_IccSendByte & BIT7)
		{
			icc_SetIO();
		}
		else
		{
			icc_ClrIO();
		}
		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P9);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P9);
		// 发送奇偶位
		if(gl_CardInfo.k_IccP_bit)
			icc_SetIO();
		else
			icc_ClrIO();

		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P10);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P10-30);//Stop bit at 0.230etu 停止位太长 缩短10092015 chenf
		// 发送停止位
		icc_SetIOInput();
		//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P11);
		icc_delay_inter_timer_cnt(gl_CardInfo.Send_P11);
		if(gl_CardInfo.Parity_Check_Enable == 0)
		{
			if(gl_CardInfo.k_Ic_LastSendByte)
			{
				// 启动下一个字节接收
				gl_char_wait_etu = gl_CardInfo.k_RecvFirst;
				icc_start_etu_timer(gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);				
			}
			goto icc_SendByteOver;
		}

		// 判断IC卡是否要求重发
		if(icc_GetIO())
		{
			if(gl_CardInfo.k_Ic_LastSendByte)
			{
				// 启动下一个字节接收
				gl_char_wait_etu = gl_CardInfo.k_RecvFirst;
				icc_start_etu_timer(gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);	
			}
			goto icc_SendByteOver;
		}
		else
		{
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
			{
				if(gl_CardInfo.k_Iccsendtimes < 3)
				{
					/* 将IO口置为输出方式，重发次数变量加1，此步可以在主程序中实现  */
					gl_CardInfo.k_Iccsendtimes++;
					//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P13);  // 延时两个ETU准备重发
	//SETSIGNAL1_L();				
					icc_delay_inter_timer_cnt(gl_CardInfo.Send_P13);
//					s_OpenIsr();     // 恢复中断
					continue;
				}
				else
				{
					icc_disable_inter_timer();
					ret = T0_MORESENDERR;
					goto icc_SendByteOver;
				}
			}
			else
			{
				if(gl_CardInfo.k_Iccsendtimes < 4)
				{
					/* 将IO口置为输出方式，重发次数变量加1，此步可以在主程序中实现  */
					gl_CardInfo.k_Iccsendtimes++;
					//while(icc_get_inter_timer_cnt() < gl_CardInfo.Send_P13);  // 延时两个ETU准备重发
	//SETSIGNAL1_L();				
					icc_delay_inter_timer_cnt(gl_CardInfo.Send_P13);
//					s_OpenIsr();     // 恢复中断
					continue;
				}
				else
				{
					icc_disable_inter_timer();
					ret = T0_MORESENDERR;
					goto icc_SendByteOver;
				}
			}
		}
	}
icc_SendByteOver:
//SETSIGNAL1_L();	
//	s_OpenIsr();
	return ret;
}

/**********************************************************************
* 函数名称： 
*     icc_DisableOtherInt
* 功能描述： 
*      屏蔽中断等级高于或等于6的中断
* 输入参数： 
*     无
* 输出参数：
*     无
* 返回值： 
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void icc_DisableOtherInt(void)
{
	//__set_BASEPRI(INT_PRI_MASK);//INT_PRI_MASK
	//s_SysTick_ITConfig(DISABLE);
//	gbSysLCDInitFlg = OFF;
	//s_SysTick_ITConfig(DISABLE);
    hw_systick_close();
}
/**********************************************************************
* 函数名称： 
*     icc_EnableOtherInt
* 功能描述： 
*      
* 输入参数： 
*     无
* 输出参数：
*     无
* 返回值： 
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void icc_EnableOtherInt(void)
{
	//__set_BASEPRI(0);
	//s_SysTick_ITConfig(ENABLE);
//	gbSysLCDInitFlg = ON;
	//s_SysTick_ITConfig(ENABLE);
    hw_systick_open();
}

int icc_checkCardInsert(int slot)
{
    int stat;
	if(slot == USERCARD)
	{
        gIccHalInfo.pChipFops->read(gIccHalInfo.pGpioInfo,CARD_DETECT,&stat);
		if(stat)
		{
			gl_CardSlotInfo[0].CardInsert = 1;
			k_ICC_CardInSert = 1;
			return ICC_SUCCESS;
		}
		else
		{
			gl_CardSlotInfo[0].CardInsert = 0;
			gl_CardSlotInfo[0].CardPowerOn = 0;
			k_ICC_CardInSert = 0;
			return (-ICC_CARDOUT);
		}
	}
	else
	{
		return ICC_SUCCESS;
	}
}


/**********************************************************************
*
*      大卡硬件控制
*
*
***********************************************************************/
// -1-进入低功耗
// 0-前台置0V
// 1,3,5-前台置工作电压
void icc_hal_vcc(ICC_GpioInfo_t *p, int value)
{
#if 2
//    TRACE("\r\n VCC:%d",value);
    ICC_HalInfo_t *haltinfo;
//    haltinfo = container_of(p,ICC_HalInfo_t,pGpioInfo);
//    TRACE("\r\n p:%x haltinfo:%x %x",p,haltinfo,sizeof(ICC_HalInfo_t),FPOS(ICC_HalInfo_t,pGpioInfo));
//    vDispBufTitle("haltinfo",0,sizeof(ICC_HalInfo_t),haltinfo);
    haltinfo = &gIccHalInfo;
    LABLE(0x11);DATAIN(value);
    if ( value > 0 ) {
        // CMDVCC置L前，IOCU必须置为H，否则会进入test模式
        hw_iccard_gpio_ioctl(p->icc_gpio_io,1,1);
        s_DelayUs(100);
        if(value == 1) {
            hw_iccard_gpio_write(p->icc_gpio_sel1,0);
            hw_iccard_gpio_write(p->icc_gpio_sel2,1);
        } else if(value == 3) {
            hw_iccard_gpio_write(p->icc_gpio_sel1,1);
            hw_iccard_gpio_write(p->icc_gpio_sel2,0);
        } else {
            hw_iccard_gpio_write(p->icc_gpio_sel1,1);
            hw_iccard_gpio_write(p->icc_gpio_sel2,1);
        }
        hw_iccard_gpio_write(p->icc_gpio_sel3,0);
        s_DelayUs(2260); //由于8035拉低cmvcc后 卡端vcc不是立即变高 要延时2.56ms后拉高 
        //故clk要多产生2.56ms 根据实验调整为2.260
    } else {
        if ( value == 0 ) {
            hw_iccard_gpio_write(p->icc_gpio_sel3,1);
            s_DelayUs(60);
            hw_iccard_gpio_write(p->icc_gpio_sel1,1);
            hw_iccard_gpio_write(p->icc_gpio_sel2,1);
            if (haltinfo->curvcc == 0xFF ) {
                sys_delay_ms(30);
                TRACE("\r\n 8035退出低功耗");
            }
        } else {
            hw_iccard_gpio_write(p->icc_gpio_sel1,0);
            hw_iccard_gpio_write(p->icc_gpio_sel2,0);
            hw_iccard_gpio_write(p->icc_gpio_sel3,1);
        }
    }
    haltinfo->curvcc = value;
#endif
}

//低功耗模式控制
// mode: 1-进入低功耗  0-退出低功耗
void icc_ControlLowpower(int mode)
{
//	if(gl_CardInfo.current_slot == USERCARD)
//	{
//		if(mode == ON)
//		{
////            D1(TRACE("\r\n 8035进入低功耗"););
////            icc_SetVCC(0xFF);
//            icc_SetVCC(0);
//		}
//		else
//		{
//            icc_SetVCC(0);
//		}
//	}
}


/*
 * icc_chip8035_init - [GENERIC]
 *   mode: 0-异步模式 1-同步模式
 * @
 */
int icc_chip8035_init (ICC_GpioInfo_t *p,int mode)
{
#ifdef CFG_ICCARD_USERCARD
    ICC_HalInfo_t *haltinfo;
//    haltinfo = container_of(p,ICC_HalInfo_t,pGpioInfo);
//    TRACE("\r\n p:%x haltinfo:%x %x",p,haltinfo,sizeof(ICC_HalInfo_t),FPOS(ICC_HalInfo_t,pGpioInfo));
//    vDispBufTitle("haltinfo",0,sizeof(ICC_HalInfo_t),haltinfo);
    haltinfo = &gIccHalInfo;
    hw_ic_gpio_init(mode);
    icc_EnableCardInsertInt(); 	// 打开插拔卡中断
	if(mode)
	{
        //关闭时钟PWN输出,变为普通IO口
        haltinfo->usercard_stat = 1;
	} else {
//        requst_pio_interrupt(p->icc_gpio_io,INT_EDGE, 6,icc_CardIOIsr); // IO中断
//        disable_pio_interrupt(p->icc_gpio_io);
        haltinfo->usercard_stat = 0;
    }
    icc_ControlLowpower(ON);
#endif
    return 0;
}		/* -----  end of function icc_chip8035_init  ----- */

/*
 * icc_chip8035_check - [GENERIC]
 * @
 */
int icc_chip8035_read (ICC_GpioInfo_t *p,int mode, int *param)
{
#ifdef CFG_ICCARD_USERCARD
    int ret=0;
    switch ( mode )
    {
    case CARD_DETECT :
//        ret = check_pio_input(p->icc_gpio_int);
        if (hw_iccard_gpio_read(GPIO_ID_IC_INT))
            ret = 1;
        else
            ret = 0;
        *param = ret;
        break;
    case SYNC_RD_IO :
//        disable_pio_output(p->icc_gpio_io);
//        clear_pio_output_low(p->icc_gpio_io);
//        ret = check_pio_input(p->icc_gpio_io);
//        *param = ret;
        break;
    case ASYNCLK_RD_IO :
//        ret = check_pio_input(p->icc_gpio_io);
//        *param = ret;
        *param = hw_iccard_gpio_read(GPIO_ID_IC_IO);
        break;
    default :
        break;
    }
    return ret;
#else
    return 0;
#endif
}		/* -----  end of function icc_chip8035_check  ----- */

/*
 * icc_chip8035_write - [GENERIC]
 *    mode: 操作类型
 *    value: 操作值
 * @
 */
int icc_chip8035_write (ICC_GpioInfo_t *p,int mode,int value)
{
#ifdef CFG_ICCARD_USERCARD
    switch(mode)
    {
    case VCC_MODE:
        icc_hal_vcc(p,value);
        break;
    case ASYNCLK_MODE:
        break;
    case WRITE_RST:
    case SYNC_RST:
        hw_iccard_gpio_write(p->icc_gpio_rst,value);
        break;
    case SYNC_C8:
        hw_iccard_gpio_write(p->icc_gpio_c8,value);
        break;
    case SYNC_C4:
        hw_iccard_gpio_write(p->icc_gpio_c4,value);
        break;
    case SET_IO_OUTPUT:
        hw_iccard_gpio_ioctl(p->icc_gpio_io,1,0); 
        break;
    case SET_IO_INPUT:
        hw_iccard_gpio_ioctl(p->icc_gpio_io,0,0); 
//        hw_iccard_gpio_write(p->icc_gpio_io,0);
        break;
    case WRITE_IO:
        hw_iccard_gpio_write(p->icc_gpio_io,value);
        break;
    case SYNC_WR_IO:
        hw_iccard_gpio_ioctl(p->icc_gpio_io,1,value); 
        break;
    case SYNC_CLK:
        hw_iccard_gpio_write(p->icc_gpio_clk,value); 
        break;
    default:
        break;
    }
#endif
    return 0;
}		/* -----  end of function icc_chip8035_write  ----- */

/*
 * s_Lowpower_icc - [GENERIC] IC卡低功耗处理
 *    mode:  0-退出低功耗 1-进入低功耗
 * @
 */
int s_Lowpower_icc (int mode)
{
#ifdef CFG_ICCARD_USERCARD
    int slot;
    if ( mode == 1 ) {
        // 由于进入低功耗前所有都下电
        for (slot=0; slot<1 ; slot++ ) {
            if (gl_CardSlotInfo[slot].CardPowerOn) {
                gl_CardSlotInfo[slot].CardPowerOn = 0;
                icc_SelectSlot(slot);
                icc_DisableIOInt();
                icc_ClrVCC();
                icc_ClrRST();
                icc_close_clk();
                icc_SetIOOutput();
                icc_ClrIO();
                icc_ControlLowpower(ON);
            }
        }
    }
#endif
    return 0;
}		/* -----  end of function s_Lowpower_icc  ----- */
#endif

