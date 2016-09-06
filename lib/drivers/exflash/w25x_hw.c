/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : w25x_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 8/4/2014 4:09:06 PM
 * Description        : 
 *******************************************************************************/

#include "drv_inc.h"
#include "w25x_hw.h"

#ifdef CFG_W25XFLASH

#if defined(ExFLASH_SIMULATE_PCS)
#define HW_W25_SPI_CS_RESET  hw_gpio_reset_bits(EXFLASH_GPIO_CS,(1<<EXFLASH_PINx_CS))
#define HW_W25_SPI_CS_SET    hw_gpio_set_bits(EXFLASH_GPIO_CS,(1<<EXFLASH_PINx_CS))
#else
#define HW_W25_SPI_CS_RESET  
#define HW_W25_SPI_CS_SET   
#endif



int hw_w25x_spi_init(void)
{
    SPI_InitDef spi_init;

    memset(&spi_init,0,sizeof(SPI_InitDef));
    spi_init.spix = ExFLASH_SPIn;
    memset(spi_init.pcs,PTxInvid,SPI_PCS_MAX);
    spi_init.pcs[EXFLASH_SPI_PCSn] = EXFLASH_PTxy_CS;
    spi_init.sck  = EXFLASH_PTxy_SCLK;
    spi_init.mosi = EXFLASH_PTxy_MOSI;
    spi_init.miso = EXFLASH_PTxy_MISO;
    spi_init.mode = SPI_MODE_MASTER;
    spi_init.attr = SCK_BR_DIV_2|SCK_PBR_DIV_2|SCK_DBR_MUL_2|MODE_MSBit
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

    return hw_spi_init(&spi_init);
}

void hw_w25x_gpio_init(void)
{
#if defined(ExFLASH_SIMULATE_PCS) || defined(ExFLASH_SOFT_RESET)
    GPIO_InitTypeDef gpio_init;
#endif

#if defined(ExFLASH_SOFT_RESET) 
    //DATA->Reset
    gpio_init.GPIOx = EXFLASH_GPIO_RESET;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL;
    gpio_init.PORT_Pin = EXFLASH_PINx_RESET;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(EXFLASH_PINx_RESET);
    hw_gpio_init(&gpio_init);
    hw_gpio_set_bits(EXFLASH_GPIO_RESET,(1<<EXFLASH_PINx_RESET));
#endif

#if defined(ExFLASH_SIMULATE_PCS)
    //INT->CS
    gpio_init.GPIOx = EXFLASH_GPIO_CS;
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|PORT_Mode_Out_SRF|PORT_Mode_Out_DSL;
    gpio_init.PORT_Pin = EXFLASH_PINx_CS;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(EXFLASH_PINx_CS);
    hw_gpio_init(&gpio_init);
    hw_gpio_set_bits(EXFLASH_GPIO_CS,(1<<EXFLASH_PINx_CS));
#endif
}


inline uint8_t hw_w25x_spi_write_byte_cs_0(uint8_t byte)
{
    return hw_spi_master_WriteRead(ExFLASH_SPIn,byte,ExFLASH_CSPIN,SPI_PCS_ASSERTED);
}
inline uint8_t hw_w25x_spi_write_byte_cs_1(uint8_t byte)
{
    return hw_spi_master_WriteRead(ExFLASH_SPIn,byte,ExFLASH_CSPIN,SPI_PCS_INACTIVE);
}

inline void hw_w25x_spi_write_multibyte(const uint8_t *input,uint16_t length)
{
//    hw_spi_master_write(ExFLASH_SPIn,ExFLASH_CSPIN,input,length);
    uint16_t i;
    for (i=0; i<length-1; i++)
    {
        hw_spi_master_WriteRead(ExFLASH_SPIn,input[i],ExFLASH_CSPIN,SPI_PCS_ASSERTED);
    }
    hw_spi_master_WriteRead(ExFLASH_SPIn,input[i],ExFLASH_CSPIN,SPI_PCS_INACTIVE);
}


/* ____________________________________
**BIT| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0  |
**   |SPR| RV| TB|BP2|BP1|BP0|WEL|BUSY|
**|SPR : 默认0,状态寄存器保护位,配合WP使用
**|TB,BP2,BP1,BP0:FLASH区域写保护设置
**|WEL : 写使能锁定
**|BUSY: 忙标记位(1,忙;0,空闲)
**|默认: 0x00
*/
uint8_t hw_w25x_flash_read_SR(void)
{  
    uint8_t tmp;
    HW_W25_SPI_CS_RESET;
    hw_w25x_spi_write_byte_cs_0(W25X_ReadStatusReg); 
    tmp =  hw_w25x_spi_write_byte_cs_1(ExFLASH_DummyByte);
    HW_W25_SPI_CS_SET;
    return tmp;
} 
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写
void hw_w25x_flash_write_SR(uint8_t status)
{   
    HW_W25_SPI_CS_RESET;
    hw_w25x_spi_write_byte_cs_0(W25X_WriteStatusReg);
    hw_w25x_spi_write_byte_cs_1(status);
    HW_W25_SPI_CS_SET;
}   
//置位WEL  
static void hw_w25x_flash_write_enable(void)   
{
    HW_W25_SPI_CS_RESET;
    hw_w25x_spi_write_byte_cs_1(W25X_WriteEnable);
    HW_W25_SPI_CS_SET;
} 
//void hw_w25x_flash_write_disable(void)   
//{  
//    HW_W25_SPI_CS_RESET;
//    hw_w25x_spi_write_lastbyte(W25X_WriteDisable);
//    HW_W25_SPI_CS_SET;
//} 			    
uint16_t hw_w25x_flash_read_ID(void)
{
    uint16_t Temp = 0;	  
    HW_W25_SPI_CS_RESET;
    hw_w25x_spi_write_byte_cs_0(W25X_ManufactDeviceID);
    hw_w25x_spi_write_byte_cs_0(0x00); 	    
    hw_w25x_spi_write_byte_cs_0(0x00); 	    
    hw_w25x_spi_write_byte_cs_0(0x00); 	    
    Temp|=hw_w25x_spi_write_byte_cs_0(ExFLASH_DummyByte)<<8;  
    Temp|=hw_w25x_spi_write_byte_cs_1(ExFLASH_DummyByte);	 
    HW_W25_SPI_CS_SET;
    return Temp;
}   		    

void hw_w25x_flash_read(uint8_t *pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
    uint16_t i;    												    
    if (NumByteToRead == 0)
        return;
    HW_W25_SPI_CS_RESET;
    hw_w25x_spi_write_byte_cs_0(W25X_ReadData);
    hw_w25x_spi_write_byte_cs_0((uint8_t)((ReadAddr)>>16));
    hw_w25x_spi_write_byte_cs_0((uint8_t)((ReadAddr)>>8));   
    hw_w25x_spi_write_byte_cs_0((uint8_t)ReadAddr);   
    for (i=0; i<NumByteToRead-1; i++)
    { 
        pBuffer[i] = hw_w25x_spi_write_byte_cs_0(ExFLASH_DummyByte);
    }
    pBuffer[i] = hw_w25x_spi_write_byte_cs_1(ExFLASH_DummyByte);
    HW_W25_SPI_CS_SET;
}  

//– 256-bytes per programmable page 
void hw_w25x_flash_write_page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    hw_w25x_flash_write_enable();
    HW_W25_SPI_CS_RESET;
    hw_w25x_spi_write_byte_cs_0(W25X_PageProgram);
    hw_w25x_spi_write_byte_cs_0((uint8_t)((WriteAddr)>>16));
    hw_w25x_spi_write_byte_cs_0((uint8_t)((WriteAddr)>>8));   
    hw_w25x_spi_write_byte_cs_0((uint8_t)WriteAddr);
    hw_w25x_spi_write_multibyte(pBuffer,NumByteToWrite);
    HW_W25_SPI_CS_SET;
    hw_w25x_flash_wait_busy(1000);
} 

void hw_w25x_flash_write_nocheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
    uint16_t pageremain;	   
    pageremain=256-WriteAddr%256;
    if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
    while(1)
    {	   
        hw_w25x_flash_write_page(pBuffer,WriteAddr,pageremain);
        if(NumByteToWrite==pageremain)
        {
            break;
        }else 
        {
            pBuffer+=pageremain;
            WriteAddr+=pageremain;	
            NumByteToWrite-=pageremain;
            if(NumByteToWrite > 256) {
                pageremain = 256; //一次可以写入256个字节
            } else {
                pageremain=NumByteToWrite; 	  //不够256个字节了
            }
        }
    };	    
} 
//NumByteToWrite:要写入的字节数(最大65535)  		   
void hw_w25x_flash_write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{
    uint16_t AlignOfPageNum = 0, NumOfNonAlignBack = 0, Addr = 0, NumOfNonAlignPrev = 0;

    Addr = WriteAddr % SPI_FLASH_PageSize;
    NumOfNonAlignPrev = (SPI_FLASH_PageSize - Addr)%SPI_FLASH_PageSize;
    if (NumByteToWrite > NumOfNonAlignPrev)
    {
        AlignOfPageNum =  (NumByteToWrite-NumOfNonAlignPrev) / SPI_FLASH_PageSize;
        NumOfNonAlignBack = (NumByteToWrite-NumOfNonAlignPrev) % SPI_FLASH_PageSize;
    } else 
    {
        NumOfNonAlignPrev = NumByteToWrite;
    }

    if (NumOfNonAlignPrev > 0) //WriteAddr非SPI_FLASH_PageSize对齐
    {
//        TRACE("\nPrevNonAlignNum:%d addr:%d",NumOfNonAlignPrev ,WriteAddr);
        hw_w25x_flash_write_page(pBuffer, WriteAddr, NumOfNonAlignPrev);
        WriteAddr +=  NumOfNonAlignPrev;
        pBuffer += NumOfNonAlignPrev;
    }
    while (AlignOfPageNum--)//SPI_FLASH_PageSize对齐
    {
//        TRACE("\nAlignPageIndex:%d addr:%d",AlignOfPageNum,WriteAddr);
        hw_w25x_flash_write_page(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
    }
    if (NumOfNonAlignBack > 0)
    {
//        TRACE("\nBackNonAlignNum:%d addr:%d",NumOfNonAlignBack,WriteAddr);
        hw_w25x_flash_write_page(pBuffer, WriteAddr, NumOfNonAlignBack);
    }
}

void hw_w25_flash_erase_chip(void)   
{                                             
    hw_w25x_flash_write_enable();
    hw_w25x_flash_wait_busy(30*1000);  
    HW_W25_SPI_CS_RESET;
    hw_w25x_spi_write_byte_cs_1(W25X_ChipErase); 
    HW_W25_SPI_CS_SET;
    hw_w25x_flash_wait_busy(30*1000);
}   

/*
 * The Sector Erase instruction sets all memory withina specified sector (4K-bytes) to the erased state 
 * of all 1s (FFh).
//– 256-bytes per programmable page 
//– Uniform 4K-byte Sectors / 64K-byte Blocks
 * 擦除一个区的最少时间:150ms
*/
void hw_w25_flash_erase_sector(uint32_t Dst_Addr)   
{   
    hw_w25x_flash_write_enable();
    hw_w25x_flash_wait_busy(1000);
    HW_W25_SPI_CS_RESET;
    hw_w25x_spi_write_byte_cs_0(W25X_SectorErase);
    hw_w25x_spi_write_byte_cs_0((uint8_t)((Dst_Addr)>>16));
    hw_w25x_spi_write_byte_cs_0((uint8_t)((Dst_Addr)>>8));
    hw_w25x_spi_write_byte_cs_1((uint8_t)Dst_Addr);
    HW_W25_SPI_CS_SET;
    hw_w25x_flash_wait_busy(1000);
}  

void hw_w25x_flash_wait_busy(uint32_t timeout_ms)   
{   
    uint32_t time_cn=sys_get_counter();
//    s_DelayMs(3);                               //等待TPD  
    while ((hw_w25x_flash_read_SR()&0x01)==0x01)
    {
        if (sys_get_counter() - time_cn > timeout_ms)
            break;
        s_DelayUs(100);   
    }
}  

//进入掉电模式
void hw_w25x_flash_powerdown(void)   
{ 
    HW_W25_SPI_CS_RESET;
    hw_w25x_spi_write_byte_cs_1(W25X_PowerDown);  
    HW_W25_SPI_CS_SET;
    s_DelayUs(3);                               //等待TPD  
}   
//唤醒
void hw_w25x_flash_wakeup(void)   
{  
    HW_W25_SPI_CS_RESET;
    hw_w25x_spi_write_byte_cs_1(W25X_ReleasePowerDown); 
    HW_W25_SPI_CS_SET;
    s_DelayUs(3);                               //等待TRES1
}   

#endif

