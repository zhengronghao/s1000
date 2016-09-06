#include "hal_dma.h"

uint32_t drv_dma_init(dma_init_t *dma_init)
{
    return DMA_Init((DMA_InitTypeDef*)dma_init);
}

void drv_dma_set_daddr(uint8_t ch, uint32_t address)
{
    DMA_SetDestAddress(ch, address);
}

void drv_dma_set_saddr(uint8_t ch, uint32_t address)
{
    DMA_SetSourceAddress(ch, address);
}

void drv_dma_set_major_counter(uint8_t chl, uint32_t val)
{
    DMA_SetMajorLoopCounter(chl,  val);
}

uint32_t drv_dma_get_daddr(uint8_t ch)
{
    return DMA_GetDestAddress(ch);
}
   
void drv_dma_enable(uint8_t chl)
{
    DMA_EnableRequest(chl);
}

void drv_dma_disable(uint8_t chl)
{
    DMA_DisableRequest(chl);
}

void drv_dma_stop(void)
{
    DMA_CancelTransfer();
}

