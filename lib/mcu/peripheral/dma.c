#include "dma.h"
#include "common.h"
#include "../cpu/MK21F12.h"

#define DMAMUX DMAMUX_BASE_PTR 
#if !defined(DMAMUX_BASES)
#define DMAMUX_BASES {DMAMUX};

#endif

//#define DMAMUX_Type int 

/* gloabl const table defination */
static DMAMUX_Type * const DMAMUX_InstanceTable[] = DMAMUX_BASES;
static DMA_CallBackType DMA_CallBackTable[16] = {NULL};
static uint32_t DMAChlMAP;

uint32_t DMA_ChlAlloc(void);
void DMA_ChlFree(uint32_t chl);

static const IRQn_Type DMA_IRQnTable[] = 
{
    (IRQn_Type)(0 + 0),
    (IRQn_Type)(0 + 1),
    (IRQn_Type)(0 + 2),
    (IRQn_Type)(0 + 3),
    (IRQn_Type)(0 + 4),
    (IRQn_Type)(0 + 5),
    (IRQn_Type)(0 + 6),
    (IRQn_Type)(0 + 7),
    (IRQn_Type)(0 + 8),
    (IRQn_Type)(0 + 9),
    (IRQn_Type)(0 + 10),
    (IRQn_Type)(0 + 11),
    (IRQn_Type)(0 + 12),
    (IRQn_Type)(0 + 13),
    (IRQn_Type)(0 + 14),
    (IRQn_Type)(0 + 15),
};

uint32_t DMA_Init(DMA_InitTypeDef *DMA_InitStruct)
{
    uint8_t chl;
    
	/* enable DMA and DMAMUX clock */
#if defined(DMAMUX0)  
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX0_MASK;
#endif
#if  defined(DMAMUX1)
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX1_MASK;
#endif
#if  defined(DMAMUX)
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
#endif
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
    
    chl = DMA_InitStruct->chl;

    /* disable chl first */
    DMA_BASE_PTR->ERQ &= ~(1<<(chl));
    /* dma chl source config */
    DMAMUX_InstanceTable[0]->CHCFG[chl] = DMAMUX_CHCFG_SOURCE(DMA_InitStruct->chlTriggerSource);
    /* trigger mode */
    switch(DMA_InitStruct->triggerSourceMode)
    {
        case kDMA_TriggerSource_Normal:
            DMAMUX_InstanceTable[0]->CHCFG[chl] &= ~DMAMUX_CHCFG_TRIG_MASK;
            break;
        case kDMA_TriggerSource_Periodic:
            DMAMUX_InstanceTable[0]->CHCFG[chl] |= DMAMUX_CHCFG_TRIG_MASK;
            break;
        default:
            break;
    }
    /* clear some register */
    DMA_BASE_PTR->TCD[chl].ATTR  = 0;
    DMA_BASE_PTR->TCD[chl].CSR   = 0;
    /* minor loop cnt */
    DMA_BASE_PTR->TCD[chl].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(DMA_InitStruct->minorLoopByteCnt);
    /* major loop cnt */
    DMA_BASE_PTR->TCD[chl].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(DMA_InitStruct->majorLoopCnt);
    DMA_BASE_PTR->TCD[chl].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(DMA_InitStruct->majorLoopCnt);
    /* source config */
    DMA_BASE_PTR->TCD[chl].SADDR = DMA_InitStruct->sAddr;
    DMA_BASE_PTR->TCD[chl].SOFF = DMA_InitStruct->sAddrOffset;
    DMA_BASE_PTR->TCD[chl].ATTR |= DMA_ATTR_SSIZE(DMA_InitStruct->sDataWidth);
    DMA_BASE_PTR->TCD[chl].ATTR |= DMA_ATTR_SMOD(DMA_InitStruct->sMod);
    DMA_BASE_PTR->TCD[chl].SLAST = DMA_SLAST_SLAST(DMA_InitStruct->sLastAddrAdj);
    /* destation config */
    DMA_BASE_PTR->TCD[chl].DADDR = DMA_InitStruct->dAddr;
    DMA_BASE_PTR->TCD[chl].DOFF = DMA_InitStruct->dAddrOffset;
    DMA_BASE_PTR->TCD[chl].ATTR |= DMA_ATTR_DSIZE(DMA_InitStruct->dDataWidth);
    DMA_BASE_PTR->TCD[chl].ATTR |= DMA_ATTR_DMOD(DMA_InitStruct->dMod);
    DMA_BASE_PTR->TCD[chl].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(DMA_InitStruct->dLastAddrAdj);
    /* auto close enable(disable req on major loop complete)*/
//    DMA_BASE_PTR->TCD[chl].CSR |= DMA_CSR_DREQ_MASK;
	/* enable DMAMUX */
    DMAMUX_InstanceTable[0]->CHCFG[chl] |= DMAMUX_CHCFG_ENBL_MASK;
    
    return chl;
}

uint32_t DMA_ChlAlloc(void)
{
    uint32_t i;
    uint32_t MaxDMAChl;
    
    /* get max DMA chl on this device */
    MaxDMAChl = (ARRAY_SIZE(DMAMUX_InstanceTable[0]->CHCFG)>32)?(ARRAY_SIZE(DMAMUX_InstanceTable[0]->CHCFG)):(32);
    
    /* alloc a chl */
    for(i=0;i<MaxDMAChl;i++)
    {
        if(!(DMAChlMAP & (1<< i)))
        {
            DMAChlMAP |= (1<<i);
            return i;
        }
    }
    return 0;
} 

void DMA_ChlFree(uint32_t chl)
{
    DMAChlMAP &= ~(1<<chl);
}

uint32_t DMA_GetMajorLoopCount(uint8_t chl)
{
    return (DMA_BASE_PTR->TCD[chl].CITER_ELINKNO & DMA_CITER_ELINKNO_CITER_MASK) >> DMA_CITER_ELINKNO_CITER_SHIFT;
}

void DMA_SetMajorLoopCounter(uint8_t chl, uint32_t val)
{
    DMA_BASE_PTR->TCD[chl].CITER_ELINKNO &= ~DMA_CITER_ELINKNO_CITER_MASK;
    DMA_BASE_PTR->TCD[chl].CITER_ELINKNO |= DMA_CITER_ELINKNO_CITER(val);
}

void DMA_EnableRequest(uint8_t chl)
{

    DMA_BASE_PTR->SERQ = DMA_SERQ_SERQ(chl);
}

void DMA_DisableRequest(uint8_t chl)
{
    DMA_BASE_PTR->CERQ = DMA_CERQ_CERQ(chl);
}

void DMA_DisableClock(void)
{
#if defined(DMAMUX0)  
    SIM->SCGC6 &= (~SIM_SCGC6_DMAMUX0_MASK);
#endif
#if  defined(DMAMUX1)
    SIM->SCGC6 &= (~SIM_SCGC6_DMAMUX1_MASK);
#endif
#if  defined(DMAMUX)
    SIM->SCGC6 &= (~SIM_SCGC6_DMAMUX_MASK);
#endif
    SIM->SCGC7 &= (~SIM_SCGC7_DMA_MASK);
}

void DMA_EnableAutoDisableRequest(uint8_t chl , bool flag)
{
    if(flag)
    {
        DMA_BASE_PTR->TCD[chl].CSR |= DMA_CSR_DREQ_MASK;
    }
    else
    {
        DMA_BASE_PTR->TCD[chl].CSR &= ~DMA_CSR_DREQ_MASK;  
    }
}

void DMA_EnableMajorLink(uint8_t chl , uint8_t linkChl, bool flag)
{
    if(flag)
    {
        /* enable major loop link */
        DMA_BASE_PTR->TCD[chl].CSR |= DMA_CSR_MAJORELINK_MASK;
        /* set chl */
        DMA_BASE_PTR->TCD[chl].CSR &= ~DMA_CSR_MAJORLINKCH_MASK;
        DMA_BASE_PTR->TCD[chl].CSR |= DMA_CSR_MAJORLINKCH(linkChl);
    }
    else
    {
        DMA_BASE_PTR->TCD[chl].CSR &= ~DMA_CSR_MAJORELINK_MASK;
    }
}

void DMA_ITConfig(uint8_t chl, DMA_ITConfig_Type config, bool status)
{
    if(status)
    {
        NVIC_EnableIRQ(DMA_IRQnTable[chl]);
    }
    switch(config)
    {
        case kDMA_IT_Half:
            (status)?
            (DMA_BASE_PTR->TCD[chl].CSR |= DMA_CSR_INTHALF_MASK):
            (DMA_BASE_PTR->TCD[chl].CSR &= ~DMA_CSR_INTHALF_MASK);
            break;
        case kDMA_IT_Major:
            (status)?
            (DMA_BASE_PTR->TCD[chl].CSR |= DMA_CSR_INTMAJOR_MASK):
            (DMA_BASE_PTR->TCD[chl].CSR &= ~DMA_CSR_INTMAJOR_MASK);
            break; 
        default:
            break;
    }
}

void DMA_CallbackInstall(uint8_t chl, DMA_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        DMA_CallBackTable[chl] = AppCBFun;
    }
}

uint8_t DMA_IsMajorLoopComplete(uint8_t chl)
{
    if(DMA_BASE_PTR->ERQ & (1 << chl))
    {
        if(DMA_BASE_PTR->TCD[chl].CSR & DMA_CSR_DONE_MASK)
        {
            /* clear this bit */
            DMA_BASE_PTR->CDNE = DMA_CDNE_CDNE(chl);
            return 0;
        }
        else
        {
            return 1;
        }
    }
    /* this chl is idle, so return 0 and clear DONE bit anyway; */
    DMA_BASE_PTR->CDNE = DMA_CDNE_CDNE(chl);
    return 0;
}

void DMA_SetDestAddress(uint8_t ch, uint32_t address)
{
    DMA_BASE_PTR->TCD[ch].DADDR = address;
}

uint32_t DMA_GetDestAddress(uint8_t ch)
{
    return DMA_BASE_PTR->TCD[ch].DADDR;
}


void DMA_SetSourceAddress(uint8_t ch, uint32_t address)
{
    DMA_BASE_PTR->TCD[ch].SADDR = address;
}

uint32_t DMA_GetSourceAddress(uint8_t ch)
{
    return DMA_BASE_PTR->TCD[ch].SADDR;
}

void DMA_CancelTransfer(void)
{
    DMA_BASE_PTR->CR |= DMA_CR_CX_MASK;
}


static void DMA_IRQHandler(uint32_t instance)
{
    DMA_BASE_PTR->CINT = DMA_CINT_CINT(instance);
    if(DMA_CallBackTable[instance]) DMA_CallBackTable[instance]();
}

void DMA0_IRQHandler(void) {DMA_IRQHandler(0);}
void DMA1_IRQHandler(void) {DMA_IRQHandler(1);}
void DMA2_IRQHandler(void) {DMA_IRQHandler(2);}
void DMA3_IRQHandler(void) {DMA_IRQHandler(3);}
void DMA4_IRQHandler(void) {DMA_IRQHandler(4);}
void DMA5_IRQHandler(void) {DMA_IRQHandler(5);}
void DMA6_IRQHandler(void) {DMA_IRQHandler(6);}
void DMA7_IRQHandler(void) {DMA_IRQHandler(7);}
void DMA8_IRQHandler(void) {DMA_IRQHandler(8);}
void DMA9_IRQHandler(void) {DMA_IRQHandler(9);}
void DMA10_IRQHandler(void) {DMA_IRQHandler(10);}
void DMA11_IRQHandler(void) {DMA_IRQHandler(11);}
void DMA12_IRQHandler(void) {DMA_IRQHandler(12);}
void DMA13_IRQHandler(void) {DMA_IRQHandler(13);}
void DMA14_IRQHandler(void) {DMA_IRQHandler(14);}
void DMA15_IRQHandler(void) {DMA_IRQHandler(15);}


