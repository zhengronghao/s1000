

#include "common.h"
#include "pit_hw.h"
//用户自定义中断服务函数数组
//static PIT_ISR_CALLBACK PIT_ISR[4];
static uchar gPitUsetimes=0;
/*
 * LPLD_PIT_Init
 * PIT通用初始化函数，选择PITx、配置中断周期、中断函数
 * 
 * 参数:
 *    pit_init_structure--PIT初始化结构体，
 *                        具体定义见PIT_InitTypeDef
 *
 * 输出:
 *    0--配置错误
 *    1--配置成功
 */
uint8 LPLD_PIT_Init(PIT_InitTypeDef *pit_init_structure)
{ 
    //计算定时加载值
    PITx pitx = pit_init_structure->PIT_Pitx;
    //参数检查
    ASSERT( pitx <= PIT3);        //判断模块号
    if ( gPitUsetimes == 0 ) {
//        TRACE("\nPit init");
        SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
        PIT_MCR = 0;
    }
    gPitUsetimes |= (1<<pitx);
    PIT_TFLG(pitx) |= PIT_TFLG_TIF_MASK;//Clear Timer Interrupt Flag
    if(pit_init_structure->PIT_Isr != NULL){
//        PIT_ISR[pitx] = pit_init_structure->PIT_Isr;
        //使能中断
        PIT_TCTRL(pitx) = PIT_TCTRL_TIE_MASK;
        //在NVIC中使能PIT中断
        enable_irq((IRQn_Type)(PIT0_IRQn + (IRQn_Type)pitx)); 
    }
    //period = (period_ns/bus_period_ns)-1m
    if (pit_init_structure->PIT_Period > 1) {
        PIT_LDVAL(pitx) = pit_init_structure->PIT_Period-1;
    }
    return 1;
}

/*
 * LPLD_PIT_Deinit
 * PIT反初始化函数，关闭选择的PITx
 * 
 * 参数:
 *    pit_init_structure--PIT初始化结构体，
 *                        具体定义见PIT_InitTypeDef
 *
 * 输出:
 *    无
 */
void LPLD_PIT_Deinit(PIT_InitTypeDef *pit_init_structure)
{ 
  PITx pitx = pit_init_structure->PIT_Pitx;
  //参数检查
  ASSERT( pitx <= PIT3);        //判断模块号              
  if ((SIM_SCGC6 & SIM_SCGC6_PIT_MASK) == 0 ) {
      SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
  }
  PIT_TCTRL(pitx) &= PIT_TCTRL_TIE_MASK;//keep TIE
  if(pit_init_structure->PIT_Isr != NULL){
      PIT_TCTRL(pitx) &= (~PIT_TCTRL_TIE_MASK);//Disable TIE
      disable_irq((IRQn_Type)(PIT0_IRQn + (IRQn_Type)pitx)); 
  }
  gPitUsetimes &= (uchar)(~(1<<pitx));
  if ( !gPitUsetimes ) {
      SIM_SCGC6 &= ~SIM_SCGC6_PIT_MASK;
  }
}

/*
 * LPLD_PIT_Enable
 *   启动定时器
 * 
 * 参数:
 *    pit_init_structure--PIT初始化结构体，
 *                        具体定义见PIT_InitTypeDef
 *
 * 输出:
 *    无
 *
 */
void LPLD_PIT_Enable(PIT_InitTypeDef *pit_init_structure)
{
    PITx pitx = pit_init_structure->PIT_Pitx;
    //参数检查
    ASSERT( pitx <= PIT3);                //判断PITx
    if ((SIM_SCGC6 & SIM_SCGC6_PIT_MASK) == 0 ) {
        SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
    }
    //开始定时
    PIT_TFLG(pitx) |= PIT_TFLG_TIF_MASK;//Clear Timer Interrupt Flag
    PIT_TCTRL(pitx) |= PIT_TCTRL_TEN_MASK;
}

/*
 * LPLD_PIT_Stop
 * 禁用PITx
 * 
 * 参数:
 *    pit_init_structure--PIT初始化结构体，
 *                        具体定义见PIT_InitTypeDef
 *
 * 输出:
 *    无
 *
 */
void LPLD_PIT_Disable(PIT_InitTypeDef *pit_init_structure)
{
    PITx pitx = pit_init_structure->PIT_Pitx;

    //参数检查
    ASSERT( pitx <= PIT3);                //判断PITx
    if ((SIM_SCGC6 & SIM_SCGC6_PIT_MASK) == 0 ) {
        SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
    }
    PIT_TCTRL(pitx) &= (~PIT_TCTRL_TEN_MASK);
}

/* 
 * LPLD_PIT_Delay - [GENERIC] 延时时钟数，基准时钟50MHz
 * @ 
 */
int LPLD_PIT_Delay (uint32 pitx, uint32 cnt)
{
    //参数检查
    ASSERT( pitx <= PIT3);        //判断模块号
    if ((SIM_SCGC6 & SIM_SCGC6_PIT_MASK) == 0 ) {
        SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
    }
    // 关闭
    PIT_TCTRL(pitx) = 0;
    PIT_LDVAL(pitx) = cnt;
    // 清标志
    PIT_TFLG(pitx) |= PIT_TFLG_TIF_MASK;//Clear Timer Interrupt Flag
    // 启动
    PIT_TCTRL(pitx) |= PIT_TCTRL_TEN_MASK;
    while ( !(PIT_TFLG(pitx) & PIT_TFLG_TIF_MASK) ) ;
    PIT_TCTRL(pitx) = 0;
    if ( gPitUsetimes == 0 ) {
        SIM_SCGC6 &= ~SIM_SCGC6_PIT_MASK;
    }
    return 0;
}		/* -----  end of function LPLD_PIT_Delay  ----- */

uint32 LPLD_PIT_Restart(PIT_InitTypeDef *pit_init_structure)
{
    PITx pitx = pit_init_structure->PIT_Pitx;

    if ((SIM_SCGC6 & SIM_SCGC6_PIT_MASK) == 0 ) {
//        TRACE("\npit clk");
        SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
    }
    gPitUsetimes |= (1<<pitx);
    PIT_TFLG(pitx) |= PIT_TFLG_TIF_MASK; //Clear Timer Interrupt Flag
    if(pit_init_structure->PIT_Isr != NULL){
        //使能中断
        PIT_TCTRL(pitx) |= PIT_TCTRL_TIE_MASK;
        //在NVIC中使能PIT中断
        enable_irq((IRQn_Type)(PIT0_IRQn + (IRQn_Type)pitx)); 
    }
    PIT_TCTRL(pitx) &= ~PIT_TCTRL_TEN_MASK;
    PIT_LDVAL(pitx) = pit_init_structure->PIT_Period;
    PIT_TCTRL(pitx) |= PIT_TCTRL_TEN_MASK;
    return 0;
}

