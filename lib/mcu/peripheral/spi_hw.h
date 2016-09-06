/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : spi_hw.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 7/18/2014 4:05:42 PM
 * Description        : 
 *******************************************************************************/
#ifndef __SPI_HW_H__
#define __SPI_HW_H__

typedef enum  _SPIn
{
    SPI0,
    SPI1,
    SPI2,
    SPIx_MAX
}SPIn;

typedef enum _SPI_ATTRIBUTE 
{
    //Baud Rate Scaler
    SCK_BR_DIV_2     = SPI_CTAR_BR(0),
    SCK_BR_DIV_4     = SPI_CTAR_BR(1),
    SCK_BR_DIV_6     = SPI_CTAR_BR(2),
    SCK_BR_DIV_8     = SPI_CTAR_BR(3),
    SCK_BR_DIV_16    = SPI_CTAR_BR(4),
    SCK_BR_DIV_32    = SPI_CTAR_BR(5),
    SCK_BR_DIV_64    = SPI_CTAR_BR(6),
    SCK_BR_DIV_128   = SPI_CTAR_BR(7),
    SCK_BR_DIV_256   = SPI_CTAR_BR(8),
    SCK_BR_DIV_512   = SPI_CTAR_BR(9),
    SCK_BR_DIV_1024  = SPI_CTAR_BR(10),
    SCK_BR_DIV_2048  = SPI_CTAR_BR(11),
    SCK_BR_DIV_4096  = SPI_CTAR_BR(12),
    SCK_BR_DIV_8192  = SPI_CTAR_BR(13),
    SCK_BR_DIV_16384 = SPI_CTAR_BR(14),
    SCK_BR_DIV_32768 = SPI_CTAR_BR(15),
    //Baud Rate Prescaler
    SCK_PBR_DIV_2 = SPI_CTAR_PBR(0),
    SCK_PBR_DIV_3 = SPI_CTAR_PBR(1),
    SCK_PBR_DIV_5 = SPI_CTAR_PBR(2),
    SCK_PBR_DIV_7 = SPI_CTAR_PBR(3),

    MODE_MSBit = (0<<SPI_CTAR_LSBFE_SHIFT),
    MODE_LSBit = (1<<SPI_CTAR_LSBFE_SHIFT),
    //Clock Phase
    CPHA_DATA_CAPTURED_LeadingEdge   = (0<<SPI_CTAR_CPHA_SHIFT),//DATA is CAPTURED on the leading edge of SCK and changed  on the following edge
    CPHA_DATA_CAPTURED_FollowingEdge = (1<<SPI_CTAR_CPHA_SHIFT),//Data is CHANGED  on the leading edge of SCK and captured on the following edge
    //Clock Polarity
    CPOL_CLK_LOW  = (0<<SPI_CTAR_CPOL_SHIFT),//The inactive state value of SCK is low
    CPOL_CLK_HIGH = (1<<SPI_CTAR_CPOL_SHIFT),//The inactive state value of SCK is high

    FRAME_SIZE_4  = SPI_CTAR_FMSZ(3),
    FRAME_SIZE_5  = SPI_CTAR_FMSZ(4),
    FRAME_SIZE_6  = SPI_CTAR_FMSZ(5),
    FRAME_SIZE_7  = SPI_CTAR_FMSZ(6),
    FRAME_SIZE_8  = SPI_CTAR_FMSZ(7),
    FRAME_SIZE_9  = SPI_CTAR_FMSZ(8),
    FRAME_SIZE_10 = SPI_CTAR_FMSZ(9),
    FRAME_SIZE_11 = SPI_CTAR_FMSZ(10),
    FRAME_SIZE_12 = SPI_CTAR_FMSZ(11),
    FRAME_SIZE_13 = SPI_CTAR_FMSZ(12),
    FRAME_SIZE_14 = SPI_CTAR_FMSZ(13),
    FRAME_SIZE_15 = SPI_CTAR_FMSZ(14),
    FRAME_SIZE_16 = SPI_CTAR_FMSZ(15),
    //Double Baud Rate
    SCK_DBR_MUL_1 = (0<<SPI_CTAR_DBR_SHIFT),
    SCK_DBR_MUL_2 = (1<<SPI_CTAR_DBR_SHIFT)
}SPI_ATTRIBUTE;

#define SPI_MODE_SLAVE        (uint8_t)(0<<0)
#define SPI_MODE_MASTER       (uint8_t)(1<<0)
#define SPI_PCS_0             (uint8_t)0
#define SPI_PCS_1             (uint8_t)1
#define SPI_PCS_2             (uint8_t)2
#define SPI_PCS_3             (uint8_t)3
#define SPI_PCS_4             (uint8_t)4
#define SPI_PCS_MAX           (uint8_t)5

typedef struct _SPI_InitDef
{
    SPIn spix;
    GPIOPin_Def pcs[SPI_PCS_MAX];
    GPIOPin_Def sck;
    GPIOPin_Def mosi;
    GPIOPin_Def miso;
    uint32_t attr; //SPI_ATTRIBUTE 
    uint8_t mode; //SPI_MODE_MASTER or SPI_MODE_SLAVE   
    uint8_t TxFIFO_switch;
    uint8_t RxFIFO_switch;
    uint8_t TxCompleteIrq;
    uint8_t TxQueueEndIrq;
    uint8_t TxFIFO_UnderflowIrq;
    uint8_t RxFIFO_OverflowIrq;
    uint8_t TxFIFO_FillIrq;
    uint8_t RxFIFO_DrainIrq;
    uint8_t TxFIFO_IrqMode; //interrupt or DMA 
    uint8_t RxFIFO_IrqMode; //interrupt or DMA 
    uint8_t t_csc;
    uint8_t p_csc;
    uint8_t t_dt;
    uint8_t p_dt;
    uint8_t t_asc;
    uint8_t p_asc;
}SPI_InitDef;

//定义SPI FIFO 事件方式
#define SPI_FIFO_DMAREQUEST  (1)
#define SPI_FIFO_INTREQUEST  (0)
//SPI PCS
#define SPI_PCS0             (1<<SPI_PCS_0) //0x000001
#define SPI_PCS1             (1<<SPI_PCS_1) //0x000010
#define SPI_PCS2             (1<<SPI_PCS_2) //0x000100
#define SPI_PCS3             (1<<SPI_PCS_3) //0x001000
#define SPI_PCS4             (1<<SPI_PCS_4) //0x010000
//发送完毕后SPI PCS的状态
#define SPI_PCS_ASSERTED     (0x01)
#define SPI_PCS_INACTIVE     (0x00)


int hw_spi_init(const SPI_InitDef *spi_init);
int hw_spi_stop(SPIn spix);
int hw_spi_irq_open(SPIn spix);
int hw_spi_irq_close(SPIn spix);
uint8_t hw_spi_master_WriteRead(SPIn spix,uint8_t data,uint8_t pcsx,uint8_t pcs_state);
uint8_t hw_spi_master_read(SPIn spix);
int hw_spi_master_write(SPIn spix,uint8_t pcsx,const uint8_t *input,uint32_t length);
void dbg_spi_reg(SPIn spix);
#endif


