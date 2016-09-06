#ifndef ICCARD_HAL_H_
#define ICCARD_HAL_H_

// 寄存器位定义
#define BIT31   (1 << 31)
#define BIT30   (1 << 30)
#define BIT29   (1 << 29)
#define BIT28   (1 << 28)
#define BIT27   (1 << 27)
#define BIT26   (1 << 26)
#define BIT25   (1 << 25)
#define BIT24   (1 << 24)
#define BIT23   (1 << 23)
#define BIT22   (1 << 22)
#define BIT21   (1 << 21)
#define BIT20   (1 << 20)
#define BIT19   (1 << 19)
#define BIT18   (1 << 18)
#define BIT17   (1 << 17)
#define BIT16   (1 << 16)
#define BIT15   (1 << 15)
#define BIT14   (1 << 14)
#define BIT13   (1 << 13)
#define BIT12   (1 << 12)
#define BIT11   (1 << 11)
#define BIT10   (1 << 10)
#define BIT9    (1 << 9)
#define BIT8    (1 << 8)
#define BIT7    (1 << 7)
#define BIT6    (1 << 6)
#define BIT5    (1 << 5)
#define BIT4    (1 << 4)
#define BIT3    (1 << 3)
#define BIT2    (1 << 2)
#define BIT1    (1 << 1)
#define BIT0    (1 << 0)


/*-----------------------------------------------------------------------------}
 *  芯片类型
 *-----------------------------------------------------------------------------{*/
#define ICC_CHIP_SAM    0
#define ICC_CHIP_8035   1
#define ICC_CHIP_GPIO   2


/*-----------------------------------------------------------------------------}
 *  芯片操作类型
 *-----------------------------------------------------------------------------{*/
#define   INITIAL          0
#define   WRITE_CS         1
#define   VCC_MODE         2
#define   ASYNCLK_MODE     3
#define   CRDDET_TYPE      4
#define   SPIMODE          5
#define   WRITE_RST        6
#define   CARD_DETECT      7
#define   WRITE_IO     	   8
#define   READ_IO      	   9
#define   CARDCLK_SLOPE    10
#define   SYNC_C4          11  //同步卡控制C4 FUS
#define   SYNC_C8          12  //同步卡控制C8 PGM
#define   SYNC_WR_IO       13  //同步卡写IO
#define   SYNC_CLK         14  //同步卡控制CLK
#define   SYNC_RST         15  //同步卡控制RST
#define   SYNC_RD_IO       16  //同步卡读IO
#define   ASYNCLK_RD_IO    17  //同步卡读IO
#define   SET_IO_OUTPUT    18
#define   SET_IO_INPUT     19

#define   ASYNCLK_L        0
#define   ASYNCLK_1_1      1
#define   ASYNCLK_1_2      2
#define   ASYNCLK_1_4      3

#define   SPIMODE_NORMAL   0
#define   SPIMODE_SPECIAL  1

#define   CARDCLK_SLOW     0
#define   CARDCLK_FAST     1

#define   CRD_NC           0
#define   CRD_NO           1


typedef struct
{
    uint  Rec_P15;
    uint  Rec_P25;
    uint  Rec_P35;
    uint  Rec_P45;
    uint  Rec_P55;
    uint  Rec_P65;
    uint  Rec_P75;
    uint  Rec_P85;
    uint  Rec_P95;
	uint  Rec_P100;
    uint  Rec_P105;
	uint  Rec_P110;
    uint  Rec_P120;

    uint  Send_P1;
    uint  Send_P2;
    uint  Send_P3;
    uint  Send_P4;
    uint  Send_P5;
    uint  Send_P6;
    uint  Send_P7;
    uint  Send_P8;
    uint  Send_P9;
    uint  Send_P10;
    uint  Send_P11;
    uint  Send_P13;

    uint  k_RecvFirst;           //接收下1个字符超时时间

    uchar   k_Ic_LastSendByte;

    uchar   k_timeover;        	 //gl_char_wait_etu超时标识
    uchar   k_total_timeover;    //gl_total_atr_etu复位接收时间超时标识
    uchar   Parity_Check_Enable;
    uchar   k_IccComErr;
    uchar   k_IccErrPar;
    uchar   k_IccErrTimeOut;

    uchar   k_Iccrecetimes;
    uchar   k_Iccsendtimes;
    uchar   k_IccReceByte;
    uchar   k_IccSendByte;
    uchar   k_IccReceByteStatus;   // FF-接收完毕 FE-奇偶错误 FD-奇偶错误超过3次
    uchar   k_IccP_bit;

    // 时序点检测
    uint    k_Ic_CharWait_TimeCount;    //接收下1个字符超时时间
    uint    k_Ic_ATR_TotalTimeCount;    //接收TS超时时间

    uint  	k_IC_TotalATRTime;         //接收ATR超时时间20161Etu内
    uint    current_slot;
}ICCWorkInfo;
/*-----------------------------------------------------------------------------}
 *  芯片驱动层
 *-----------------------------------------------------------------------------{*/
// 硬件引脚定义
typedef struct __ICC_GpioInfo_t {
    uchar icc_gpio_io;   //IO口
    uchar icc_gpio_rst;  //
    uchar icc_gpio_int;  //卡在位检测
    uchar icc_gpio_clk;  //clk
    uchar icc_gpio_sel1; //1.8v
    uchar icc_gpio_sel2; //3/5V
    uchar icc_gpio_sel3; //cmd
    uchar icc_gpio_c4;   //
    uchar icc_gpio_c8;   //
    uchar icc_gpio_ioplus;

    uchar sam1_gpio_io;   //IO口
    uchar sam2_gpio_io;   //IO口
    uchar sam1_gpio_rst;  //
    uchar sam2_gpio_rst;  //
    uchar sam_gpio_clk;  //clk
    uchar sam_gpio_sel1; //CS1
    uchar sam_gpio_sel2; //CS2
    uchar sam1_gpio_vcc;  //
    uchar sam2_gpio_vcc;  //
}ICC_GpioInfo_t;
// 卡芯片操作函数
typedef struct __ICC_CHIP_FOPS_t {
    int chiptype;
    int (*init)(ICC_GpioInfo_t *picc_halinfo,int mode);
    int (*write)(ICC_GpioInfo_t *picc_halinfo,int mode,int value);
    int (*read)(ICC_GpioInfo_t *picc_halinfo,int mode, int *para);
    int (*ioctl)(ICC_GpioInfo_t *picc_halinfo,int mode, int para);
}ICC_CHIP_FOPS_t;

typedef struct __ICC_HalInfo_t {
    uchar curvcc;        //当前大卡电压
    uchar usercard_stat;   //大卡工作状态 0-异步卡 1-同步卡
    uchar version;         //版本
    uchar chiptype;        //芯片类型
    uchar samslots_inside; //内置sam卡座数
    uchar samslots_outside;//外置sam卡座数
    uchar reserved[2];
    ICC_GpioInfo_t *pGpioInfo;
    ICC_CHIP_FOPS_t *pChipFops;
}ICC_HalInfo_t;

// 中断控制
#define s_CloseIsr() DisableInterrupts 
#define s_OpenIsr()  EnableInterrupts 
/*-----------------------------------------------------------------------------}
 *  全局变量
 *-----------------------------------------------------------------------------{*/
extern ICC_HalInfo_t gIccHalInfo;
extern volatile ICCWorkInfo gl_CardInfo;
/*-----------------------------------------------------------------------------}
 *  全局函数
 *-----------------------------------------------------------------------------{*/
void icc_SetVCC(int VolMode);
void icc_ClrVCC(void);
void icc_SetRST(void);
void icc_ClrRST(void);
void icc_SetIOOutput(void);
void icc_SetIOInput(void);
void icc_SetIO(void);
void icc_ClrIO(void);
void icc_init_clk(void);
void icc_open_clk(void);
void icc_close_clk(void);

void icc_init_etu_timer(void);
void icc_start_etu_timer(int clk);
void icc_close_etu_timer(void);
void icc_init_inter_timer(void);
void icc_delay_clock(int clock);
void icc_delay_etu(int etu, int num);
void icc_disable_inter_timer(void);

void icc_SamOff(int CardSlot);
void icc_SelectSlot(int CardSlot);
void icc_CpuCardInit(void );
void icc_SamCardInit( void );
uchar icc_RecvByte(void);
int icc_SendByte(uchar dat);

void icc_DisableOtherInt(void);
void icc_EnableOtherInt(void);
int icc_checkCardInsert(int slot);
int get_iccard_Ver(void);
void icc_ControlLowpower(int mode);
void icc_ControlClkPin(int value);
int s_Lowpower_icc (int mode);
int icc_get_usercard_workmode(void);
void usercard_insert_int(void);
void icc_CardIOIsr(void);
void icc_etu_timer_isr(void);

#endif

