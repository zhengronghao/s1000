#ifndef __drv_dma_H
#define __drv_dma_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "dma.h"

#define BD_DMA_CH0    HW_DMA_CH0    
#define BD_DMA_CH1    HW_DMA_CH1  
#define BD_DMA_CH2    HW_DMA_CH2    
#define BD_DMA_CH3    HW_DMA_CH3 

typedef enum
{
    DMA_TriggerSource_Normal,    //正常模式
    DMA_TriggerSource_Periodic,  //周期触发模式
}dma_tri_source_t;

typedef enum
{
    DMA_DataWidthBit_8,   /* 8 bit data width */
    DMA_DataWidthBit_16,  /* 16 bit data width */
    DMA_DataWidthBit_32,  /* 32 bit data width*/
}dma_data_len_t; 

typedef enum 
{
    DMA_ModuloDisable = 0x0U,
    DMA_Modulo2bytes = 0x1U,
    DMA_Modulo4bytes = 0x2U,
    DMA_Modulo8bytes = 0x3U,
    DMA_Modulo16bytes = 0x4U,
    DMA_Modulo32bytes = 0x5U,
    DMA_Modulo64bytes = 0x6U,
    DMA_Modulo128bytes = 0x7U,
    DMA_Modulo256bytes = 0x8U,
    DMA_Modulo512bytes = 0x9U,
    DMA_Modulo1Kbytes = 0xaU,
    DMA_Modulo2Kbytes = 0xbU,
    DMA_Modulo4Kbytes = 0xcU,
    DMA_Modulo8Kbytes = 0xdU,
    DMA_Modulo16Kbytes = 0xeU,
    DMA_Modulo32Kbytes = 0xfU,
    DMA_Modulo64Kbytes = 0x10U,
    DMA_Modulo128Kbytes = 0x11U,
    DMA_Modulo256Kbytes = 0x12U,
    DMA_Modulo512Kbytes = 0x13U,
    DMA_Modulo1Mbytes = 0x14U,
    DMA_Modulo2Mbytes = 0x15U,
    DMA_Modulo4Mbytes = 0x16U,
    DMA_Modulo8Mbytes = 0x17U,
    DMA_Modulo16Mbytes = 0x18U,
    DMA_Modulo32Mbytes = 0x19U,
    DMA_Modulo64Mbytes = 0x1aU,
    DMA_Modulo128Mbytes = 0x1bU,
    DMA_Modulo256Mbytes = 0x1cU,
    DMA_Modulo512Mbytes = 0x1dU,
    DMA_Modulo1Gbytes = 0x1eU,
    DMA_Modulo2Gbytes = 0x1fU
} dma_modulo_t;

typedef struct 
{
    uint8_t                     chl;                                /* DMA通道号0~15 */       
    uint8_t                     chlTriggerSource;                   /* DMA触发源选择 */
    uint16_t                    minorLoopByteCnt;                   /* MINOR LOOP 中一次传输的字节数 */
    uint16_t                    majorLoopCnt;                       /* MAJOR LOOP 循环次数 */
    dma_tri_source_t        triggerSourceMode;                  /* 触发模式选择 */
    /* 源地址配置 */
    int32_t                     sAddrOffset;                /* DMA每次读取sAddr之后 sAddr的偏移量 可正可负 */
    uint32_t                    sAddr;                      /* 数据源地址 */
    dma_data_len_t       sDataWidth;                 /* 数据源地址数据宽度 8 16 32 */
    int32_t                     sLastAddrAdj;               /* 所有MAJOR LOOP循环完成后 源地址偏移量 */
    dma_modulo_t             sMod;                       /* Modulo 设置 参见 AN2898 */
    /* 目标地址属性配置 */
    int32_t                     dAddrOffset;                
    uint32_t                    dAddr;                      
    dma_data_len_t       dDataWidth;                 
    int32_t                     dLastAddrAdj;               
    dma_modulo_t             dMod;                       
}dma_init_t;

uint32_t drv_dma_init(dma_init_t *dma_init);

void drv_dma_set_daddr(uint8_t ch, uint32_t address);

void drv_dma_set_saddr(uint8_t ch, uint32_t address);

void drv_dma_set_major_counter(uint8_t chl, uint32_t val);

uint32_t drv_dma_get_daddr(uint8_t ch);

void drv_dma_enable(uint8_t chl);

void drv_dma_disable(uint8_t chl);

void drv_dma_stop(void);

#endif
