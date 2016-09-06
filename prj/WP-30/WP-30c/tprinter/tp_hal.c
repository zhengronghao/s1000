
#include "wp30_ctrl.h"
#ifdef CFG_TPRINTER

static void __tp_gpio_output(GPIO_Type GPIOx,PORTPin_TypeDef pin)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = GPIOx;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_DSH;
    gpio_init.PORT_Pin = pin;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(pin);
    hw_gpio_init(&gpio_init);
}

void tp_gpio_init(void )
{
    /* MOT-PWR   */
    __tp_gpio_output(TP_MOTPWR_GPIO, TP_MOTPWR_PINx);
    MOTOR_PWR_OFF();
    /* PRN-LATCH */
    __tp_gpio_output(TP_LATCH_GPIO, TP_LATCH_PINx);
    LATCH_LOW();
   /* PRN-POWER */
    __tp_gpio_output(TP_WORK_GPIO, TP_WORK_PINx);
    PRN_POWER_DISCHARGE();
    /* MOT-PHASE-1A */
    __tp_gpio_output(TP_MnA_GPIO, TP_MnA_PINx);
    MOTOR_PHASE_1A_LOW();
    /* MOT-PHASE-1B  */
    __tp_gpio_output(TP_MA_GPIO, TP_MA_PINx);
    MOTOR_PHASE_1B_LOW();
    /*  MOT-PHASE-2A  */
    __tp_gpio_output(TP_MnB_GPIO, TP_MnB_PINx);
    MOTOR_PHASE_2A_LOW();
    /* MOT-PHASE-2B  */
    __tp_gpio_output(TP_MB_GPIO, TP_MB_PINx);
    MOTOR_PHASE_2B_LOW();
    /*  PRN-STROBE1 */
    __tp_gpio_output(TP_STB_GPIO, TP_STB_PINx);
    STROBE_1_OFF();
}

void tp_spi_int(void)
 {
    SPI_InitDef spi_init;

    memset(&spi_init,0,sizeof(SPI_InitDef));
    spi_init.spix = TPRINT_SPIn;
    memset(spi_init.pcs,PTxInvid,SPI_PCS_MAX);
//    spi_init.pcs[EXFLASH_SPI_PCSn] = PTxInvid;
    spi_init.sck  = TPRINT_PTxy_SCLK;
    spi_init.mosi = TPRINT_PTxy_MOSI;
//    spi_init.miso = PTxInvid;
    spi_init.mode = SPI_MODE_MASTER;
    //60M外设频率 4分频 2分频 共8分频7.5M
    spi_init.attr = SCK_BR_DIV_4|SCK_PBR_DIV_2|MODE_MSBit
        |CPOL_CLK_LOW | CPHA_DATA_CAPTURED_LeadingEdge 
        |FRAME_SIZE_8;
    spi_init.TxFIFO_switch = FALSE;
    spi_init.RxFIFO_switch = FALSE;
    spi_init.TxCompleteIrq = FALSE;
    spi_init.TxQueueEndIrq = FALSE;
    spi_init.TxFIFO_UnderflowIrq = FALSE;
    spi_init.RxFIFO_OverflowIrq  = FALSE;
    spi_init.TxFIFO_FillIrq  = FALSE;
    spi_init.RxFIFO_DrainIrq = FALSE;
    spi_init.TxFIFO_IrqMode  = FALSE;
    spi_init.RxFIFO_IrqMode  = FALSE;

    hw_spi_init(&spi_init);
}

void tp_data_send(uint8_t *in_buf, uint32_t in_len)
{
    //printf("2%s\n",__FUNCTION__);
    //片选无效
    hw_spi_master_write(TPRINT_SPIn,TPRINT_PCS_PIN,in_buf,in_len); 
}

 /* ADC0_DP1 */ 
static uint16_t __tp_temp_ad_value(void)
{
    uint16_t value;
    
    value = (uint16_t)(drv_adcx_caculate(TP_PAP_TEM_ADC, TP_TEM_ADC_CHANEL));

     return value;
}
 
 /* ADC0_DP1 */ 
static void __tp_temp_ad_init(void)
{
    /* adc 初始化 */
    drv_adcx_open(TP_PAP_TEM_ADC, TP_FUNC_ADC_CTRLR,ADC_SC2_TRIGER_SOFTWARE,ADC_SC1n_SingleEnded,
                 ADC_SC3_HARDWARE_AVERAGE_ENABLE|ADC_SC3_HARDWARE_AVERAGE_32);
//    drv_adc_close(TP_PAP_TEM_ADC);
}

/* ADC1_SE16 */
uint16_t tp_paper_ad_value(void)
{
    uint16_t value;
    
    value = (uint16_t)(drv_adcx_caculate(TP_PAP_TEM_ADC, TP_PAP_ADC_CHANEL));

     return value;
}

/* ADC1_SE16 */
static void __tp_paper_ad_init(void)
{
    /* adc 初始化 */
    //和温度一致
}

/* ADC0_SE16 */
static int16_t __tp_power_ad_value(void)
{
    int16_t value;
    
    value = (int16_t)(drv_adcx_caculate(TP_BATTERY_ADC, TP_BAT_ADC_CHANEL));

     return value;
}

/* ADC0_SE16 */
static void __tp_power_ad_init(void)
{
    /* adc 初始化 */
    drv_adcx_open(TP_BATTERY_ADC, TP_FUNC_ADC_CTRLR,ADC_SC2_TRIGER_SOFTWARE,ADC_SC1n_SingleEnded,
                 ADC_SC3_HARDWARE_AVERAGE_ENABLE|ADC_SC3_HARDWARE_AVERAGE_32);
//    drv_adc_close(TP_PAP_TEM_ADC);
}

void tp_ad_init(void)
{
    __tp_temp_ad_init();
    __tp_paper_ad_init();
    __tp_power_ad_init();
}

void tp_ad_close(void)
{
    drv_adcx_close(TP_PAP_TEM_ADC, TP_FUNC_ADC_CTRLR);
//    drv_adcx_close(TP_BATTERY_ADC, TP_FUNC_ADC_CTRLR);
}
/*
函数    把读取到的AD值，根据上拉或下拉电阻计算对应内部的电阻值
输入    ad:     读取到的ad值,单位:100欧姆
u_res:   外部上拉电阻值
d_res:   外部下拉电阻值
*/
static uint32_t __tp_v_to_r(uint32_t ad, uint32_t ad_max, uint32_t u_res, uint32_t d_res)
{
    if (u_res) {   
        /* 上拉电阻*/
        if (ad >= ad_max) {
            return (ad*u_res);
        } else {
            //根据恒等式 VPR * R热敏/(R热敏+u_res) == AD/ADmax * 3.3 可得出R热敏值
            return ((ad*u_res)/(ad_max-ad));
        }
    } else {
        /* 下拉电阻*/
        if (ad == 0) {
            ad = 1;
        }
        return ((ad_max-ad)*d_res/ad);
    }
}

static uint16_t __tp_r_to_degree(uint32_t res)
{
    uint16_t i;
    uint16_t degree;
    uint16_t restbl[25] = {8430,6230,4660,3520,2690,2080,
                        1610,1240, 968, 757, 595, 471,
                         375, 300, 242, 196, 159, 131,
                         108,  89,  74,  62,  52,  44,37};
    
    i = 0;
    while (i < 24) {
        if (res >= restbl[i+1]) break;
        i++;
    }
    
    if (i >= 24) {
        /* 超过80度*/
        degree = ((restbl[24]-res)*5)/(restbl[23]-restbl[24]) + 80;
    } else {
        degree = 5*(i+1) - ((res-restbl[i+1])*5)/(restbl[i]-restbl[i+1]) - 40;
    }

    return (degree);
}

uint16_t __tp_temp_value(void)
{
    uint16_t ad;
    uint16_t temp;
    ad   = __tp_temp_ad_value();
    temp = __tp_r_to_degree(__tp_v_to_r(ad, TPRINT_ADCMAX_VALUE, 300, 0));

    return temp;
}

uint8_t tp_printer_ready(void)
{
    if (tp_paper_ad_value() > PAPER_AD_LTHRESHOLD){
        //缺纸
        return FALSE;
    } else {
        return TRUE;
    }
}

uint16_t  __tp_power_value(void)
{
    unsigned int new_val;
    uint16_t battery_vol;

    new_val = __tp_power_ad_value();
    //根据恒等式 VPR * R分压1/(R分压1+R分压2) == AD/ADmax * 3.3 可得出VPR
    battery_vol = ((new_val*130*330)/(TPRINT_ADCMAX_VALUE*30));
    
    return battery_vol;
}

uint16_t tp_temp_value(void)
{
    return __tp_temp_value();
}

uint16_t  tp_power_value(void)
{
    return __tp_power_value();
}

void tp_timer_init(PITx time, uint32_t us, IRQ_CALLBACK funtion)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = time;
    pit.PIT_Period = TPRINT_BASE_TIME*us; 
    if (funtion != NULL) {
        pit.PIT_Isr = funtion;
    }
    LPLD_PIT_Init(&pit);
}

void tp_timer_start(PITx time, uint32_t us, IRQ_CALLBACK funtion)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = time;
    pit.PIT_Period = TPRINT_BASE_TIME*us; 
    if (funtion != NULL) {
        pit.PIT_Isr = funtion;
    }
    LPLD_PIT_Restart(&pit);
}

void tp_timer_close(PITx time, uint32_t us, IRQ_CALLBACK funtion)
{
    PIT_InitTypeDef pit;
    pit.PIT_Pitx = time;
//    pit.PIT_Period = TPRINT_BASE_TIME*us; 
    if (funtion != NULL) {
        pit.PIT_Isr = funtion;
    }
    LPLD_PIT_Deinit(&pit);
}

#endif


