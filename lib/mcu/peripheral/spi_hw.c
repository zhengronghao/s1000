/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : spi_hw.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 7/18/2014 4:05:21 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "spi_hw.h"

static volatile struct SPI_MemMap *const SPIBA[] = SPI_BASE_PTRS;

void dbg_spi_reg(SPIn spix)
{
    TRACE("\n-|--SPI%d--|",spix);
    SIM->SCGC6 |=  SIM_SCGC6_SPI1_MASK;  
    SIM->SCGC6 |=  SIM_SCGC6_SPI0_MASK;  
    TRACE("\n-|MCR  : %08X",SPI_MCR_REG(SPIBA[spix]));
    TRACE("  TCR  : %08X",SPI_TCR_REG(SPIBA[spix]));
    TRACE("\n-|CTAR0: %08X",SPI_CTAR_REG(SPIBA[spix],0));
    TRACE("  CTAR1: %08X",SPI_CTAR_REG(SPIBA[spix],1));
    TRACE("\n-|SR   : %08X",SPI_SR_REG(SPIBA[spix]));
    TRACE("  RSER : %08X",SPI_RSER_REG(SPIBA[spix]));
    TRACE("\n-|PUSHR: %08X",SPI_PUSHR_REG(SPIBA[spix]));
    TRACE("  POPR : %08X",SPI_POPR_REG(SPIBA[spix]));
}

//SCK baud rate = (fSYS/PBR) x [(1+DBR)/BR] = [fSYS*(1+DBR)]/(PBR*BR)
int hw_spi_init(const SPI_InitDef *spi_init)
{
    uint8_t spix = spi_init->spix;

    ASSERT(spi_init->mode <= SPI_MODE_MASTER);

    if(spix == SPI0)
    {
//        TRACE("\n-|SPI0 ");
        /* Enable the clock to the SPIx */    
        SIM->SCGC6 |=  SIM_SCGC6_SPI0_MASK;  
        if (spi_init->pcs[SPI_PCS_0] == PTA14) {
            PORTA->PCR[14] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->pcs[SPI_PCS_0] == PTC4) {
            PORTC->PCR[4] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->pcs[SPI_PCS_0] == PTD0) {
            PORTD->PCR[0] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->pcs[SPI_PCS_0] == PTE16) {
            PORTE->PCR[16] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" pcs0 NULL");
        }
        if (spi_init->pcs[SPI_PCS_1] == PTC3) {
            PORTC->PCR[3]  = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->pcs[SPI_PCS_1] == PTD4) {
            PORTD->PCR[4] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" pcs1 NULL");
        }
        if (spi_init->pcs[SPI_PCS_2] == PTC2) {
            PORTC->PCR[2]  = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->pcs[SPI_PCS_2] == PTD5) {
            PORTD->PCR[5]  = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" pcs2 NULL");
        }
        if (spi_init->pcs[SPI_PCS_3] == PTC1) {
            PORTC->PCR[1]  = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->pcs[SPI_PCS_3] == PTD6) {
            PORTD->PCR[6]  = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" pcs3 NULL");
        }
        if(spi_init->pcs[SPI_PCS_4] == PTC0) {
            PORTC->PCR[0]  = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" pcs4 NULL");
        }

        if (spi_init->sck == PTA15) {
            PORTA->PCR[15] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->sck == PTC5) {
            PORTC->PCR[5] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->sck == PTD1) {
            PORTD->PCR[1] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" SCK NULL");
        }
        if (spi_init->mosi == PTA16) {
            PORTA->PCR[16] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;//SOUT
        } else if (spi_init->mosi == PTC6) {
            PORTC->PCR[6] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;//SOUT
        } else if (spi_init->mosi == PTD2) {
            PORTD->PCR[2] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;//SOUT
        } else if (spi_init->mosi == PTE18) {
            PORTE->PCR[18] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;//SOUT
        } else {
//            TRACE(" MOSI NULL");
        }
        if (spi_init->miso == PTA17) {
            PORTA->PCR[17] = 0 | PORT_PCR_MUX(2); //SIN
        } else if (spi_init->miso == PTC7) {
            PORTC->PCR[7] = 0 | PORT_PCR_MUX(2); //SIN
        } else if (spi_init->miso == PTD3) {
            PORTD->PCR[3] = 0 | PORT_PCR_MUX(2); //SIN
        } else if (spi_init->miso == PTE19) {
            PORTE->PCR[19] = 0 | PORT_PCR_MUX(2); //SIN
        } else {
//            TRACE(" MISO NULL");
        } 
    } else if(spix == SPI1)
    {
//        TRACE("\n-|SPI1 ");
        SIM->SCGC6 |= SIM_SCGC6_SPI1_MASK;
        if (spi_init->pcs[SPI_PCS_0] == PTB10) {
            PORTB->PCR[10] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->pcs[SPI_PCS_0] == PTE4) {
            PORTE->PCR[4] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" pcs0 NULL");
        }
        if (spi_init->pcs[SPI_PCS_1] == PTE0) {
            PORTE->PCR[0] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" pcs1 NULL");
        }
        if (spi_init->pcs[SPI_PCS_2] == PTE5) {
            PORTE->PCR[5]  = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" pcs2 NULL");
        }

        if (spi_init->sck == PTB11) {
            PORTB->PCR[11] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->sck == PTE2) {
            PORTE->PCR[2] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;//SCK
        } else {
//            TRACE(" SCK NULL");
        }
        if (spi_init->mosi == PTB16) {
            PORTB->PCR[16] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->mosi == PTE1) {
            PORTE->PCR[1] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->mosi == PTE3) {
            PORTE->PCR[3] = 0 | PORT_PCR_MUX(7) | PORT_PCR_DSE_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
        } else {
//            TRACE(" MOSI NULL");
        }
        if (spi_init->miso == PTB17) {
            PORTB->PCR[17] = 0 | PORT_PCR_MUX(2); //SIN
        } else if (spi_init->miso == PTE3) {
            PORTE->PCR[3] = 0 | PORT_PCR_MUX(2); //SIN
        } else if (spi_init->miso == PTE1) {
            PORTE->PCR[1] = 0 | PORT_PCR_MUX(7); //SIN
        } else {
//            TRACE(" MISO NULL");
        } 
    } else if(spix == SPI2)
    {
        SIM->SCGC3 |= SIM_SCGC3_SPI2_MASK;
        if (spi_init->pcs[SPI_PCS_0] == PTB20) {
            PORTB->PCR[20] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->pcs[SPI_PCS_0] == PTD11) {
            PORTD->PCR[11] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" pcs0 NULL");
        }
        if (spi_init->pcs[SPI_PCS_1] == PTD15) {
            PORTD->PCR[15] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" pcs1 NULL");
        }

        if (spi_init->sck == PTB21) {
            PORTB->PCR[21] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->sck == PTD12) {
            PORTD->PCR[12] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;//SCK
        } else {
//            TRACE(" SCK NULL");
        }
        if (spi_init->mosi == PTB22) {
            PORTB->PCR[22] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else if (spi_init->mosi == PTD13) {
            PORTD->PCR[13] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
        } else {
//            TRACE(" MOSI NULL");
        }
        if (spi_init->miso == PTB23) {
            PORTB->PCR[23] = 0 | PORT_PCR_MUX(2); //SIN
        } else if (spi_init->miso == PTD14) {
            PORTD->PCR[14] = 0 | PORT_PCR_MUX(2); //SIN
        } else {
//            TRACE(" MISO NULL");
        } 
    } else 
    {
        ASSERT(0);
        return -1; 
    }

    SPI_MCR_REG(SPIBA[spix]) = 0 & (~SPI_MCR_MDIS_MASK)        
                             | SPI_MCR_HALT_MASK        
                             | SPI_MCR_PCSIS_MASK       
                             | SPI_MCR_CLR_TXF_MASK    
                             | SPI_MCR_CLR_RXF_MASK;  
    if(spi_init->mode == SPI_MODE_SLAVE)
    {
        SPI_MCR_REG(SPIBA[spix]) &= ~SPI_MCR_MSTR_MASK;//Slave mode
    } else
    {
        SPI_MCR_REG(SPIBA[spix]) |= SPI_MCR_MSTR_MASK; //Master mode
    }
    if(spi_init->TxFIFO_switch == TRUE)
    {
        SPI_MCR_REG(SPIBA[spix]) &= ~SPI_MCR_DIS_TXF_MASK; //TX FIFO is enabled.
    } else
    {
        SPI_MCR_REG(SPIBA[spix]) |= SPI_MCR_DIS_TXF_MASK; //TX FIFO is disabled
    }
    if(spi_init->RxFIFO_switch == TRUE)
    {
        SPI_MCR_REG(SPIBA[spix]) &= ~SPI_MCR_DIS_RXF_MASK;//RX FIFO is enabled.
    } else
    {
        SPI_MCR_REG(SPIBA[spix]) |= SPI_MCR_DIS_RXF_MASK; //RX FIFO is disabled
    }
    //选择使能发送完成中断
    if(spi_init->TxCompleteIrq == TRUE)
    {
        SPI_RSER_REG(SPIBA[spix]) |= SPI_RSER_TCF_RE_MASK; 
    } else
    { 
        SPI_RSER_REG(SPIBA[spix]) &= ~SPI_RSER_TCF_RE_MASK;
    }
    //选择使能发送队列结束中断
    if(spi_init->TxQueueEndIrq == TRUE)
    {
        SPI_RSER_REG(SPIBA[spix]) |=SPI_RSER_EOQF_RE_MASK;
    } else 
    {
        SPI_RSER_REG(SPIBA[spix]) &= ~SPI_RSER_EOQF_RE_MASK;
    }
    if(spi_init->TxFIFO_UnderflowIrq== TRUE)
    {
        SPI_RSER_REG(SPIBA[spix]) |=SPI_RSER_TFUF_RE_MASK;
    } else
    {
        SPI_RSER_REG(SPIBA[spix]) &= ~SPI_RSER_TFUF_RE_MASK;
    }
    if(spi_init->RxFIFO_OverflowIrq== TRUE)
    {
        SPI_RSER_REG(SPIBA[spix]) |=SPI_RSER_RFOF_RE_MASK;
    } else
    {
        SPI_RSER_REG(SPIBA[spix]) &= ~SPI_RSER_RFOF_RE_MASK;
    }
    if(spi_init->TxFIFO_FillIrq == TRUE)
    {
        SPI_RSER_REG(SPIBA[spix]) |=SPI_RSER_TFFF_RE_MASK;
    } else
    {
        SPI_RSER_REG(SPIBA[spix]) &= ~SPI_RSER_TFFF_RE_MASK;
    }
    if(spi_init->RxFIFO_DrainIrq == TRUE)
    {
        SPI_RSER_REG(SPIBA[spix]) |=SPI_RSER_RFDF_RE_MASK;
    } else
    {
        SPI_RSER_REG(SPIBA[spix]) &= ~SPI_RSER_RFDF_RE_MASK;
    }
    if(spi_init->TxFIFO_IrqMode == SPI_FIFO_DMAREQUEST)
    {
        SPI_RSER_REG(SPIBA[spix]) |=SPI_RSER_TFFF_DIRS_MASK;
    } else
    {
        SPI_RSER_REG(SPIBA[spix]) &= ~SPI_RSER_TFFF_DIRS_MASK;
    }
    if(spi_init->RxFIFO_IrqMode == SPI_FIFO_DMAREQUEST)
    {
        SPI_RSER_REG(SPIBA[spix]) |= SPI_RSER_RFDF_DIRS_MASK;
    } else 
    {
        SPI_RSER_REG(SPIBA[spix]) &= ~SPI_RSER_RFDF_DIRS_MASK;
    }

    //配置SPI CTAR寄存器，设置SPI的总线时序
    SPI_CTAR_REG(SPIBA[spix],0) = spi_init->attr;        

    //tCSC = (1/g_bus_clock) x PCSSCK x CSSCK
    //tCSC = 1/50,000,000 x PCSSCK x CSSCK
    SPI_CTAR_REG(SPIBA[spix],0) |= SPI_CTAR_PCSSCK(spi_init->p_csc); 
    SPI_CTAR_REG(SPIBA[spix],0) |= SPI_CTAR_CSSCK(spi_init->t_csc);  

    //tDT = (1/g_bus_clock) x PDT x DT
    SPI_CTAR_REG(SPIBA[spix],0) |= SPI_CTAR_DT(spi_init->t_dt);
    SPI_CTAR_REG(SPIBA[spix],0) |= SPI_CTAR_PDT(spi_init->p_dt);

    //Config the Delay of the last edge of SCK and the negation of PCS
    //tASC = (1/g_bus_clock) x PASC x ASC :g_bus_clock == periph_clk_khz
    SPI_CTAR_REG(SPIBA[spix],0) |= SPI_CTAR_PASC(spi_init->p_asc);//PASC = 2*n+1
    SPI_CTAR_REG(SPIBA[spix],0) |= SPI_CTAR_ASC(spi_init->t_asc);//
    //清除标志位
    SPI_SR_REG(SPIBA[spix]) = SPI_SR_RFDF_MASK   
                            | SPI_SR_RFOF_MASK
                            | SPI_SR_TFFF_MASK
                            | SPI_SR_TFUF_MASK
                            | SPI_SR_TCF_MASK
                            | SPI_SR_EOQF_MASK;
//    if (spi_init->attr & CPOL_CLK_HIGH)
//        SPI_PUSHR_REG(SPIBA[spix]) = SPI_PUSHR_CONT_MASK;
    //使能SPIx
    SPI_MCR_REG(SPIBA[spix]) &=~ SPI_MCR_HALT_MASK; 

    return 0;
}


int hw_spi_stop(SPIn spix)
{
    SPI_MCR_REG(SPIBA[spix]) |= SPI_MCR_HALT_MASK; //set stop mode
    if(spix == SPI0)
    {
        disable_irq(SPI0_IRQn);
        SIM->SCGC6 &= ~SIM_SCGC6_SPI0_MASK;
    } else if(spix == SPI1)
    {
        disable_irq(SPI1_IRQn);
        SIM->SCGC6 &= ~SIM_SCGC6_SPI1_MASK;
    } else 
    {
        return -1;
    }
    return 0;
}

int hw_spi_irq_open(SPIn spix)
{
    if(spix == SPI0)
    {
        enable_irq(SPI0_IRQn);
    } else if(spix == SPI1)
    {
        enable_irq(SPI1_IRQn);
    } else
    {
        return -1;
    }

    return 0;
}

int hw_spi_irq_close(SPIn spix)
{
    if(spix == SPI0)
    {
        disable_irq(SPI0_IRQn);
    } else if(spix == SPI1)
    {
        disable_irq(SPI1_IRQn);
    } else
    {
        return -1;
    }
    return 0;
}


/**  
 * \brief   
 * \param 
 *         spix      spi channel:SPI0,SPI1
 *         data      data to write 
 *         pcsx      select spi cs: SPI_PCS0 ~ SPI_PCS4(SPI0)
 *                                  SPI_PCS0 ~ SPI_PCS2(SPI1)
 *         pcs_state This keeps the selected PCS signals to remain asserted between transfers:
 *                   pcs keep valid(SPI_PCS_ASSERTED) or invalid(SPI_PCS_INACTIVE)
 * \return the 8 bits data from spi salve
 * \todo    
 */
uint8_t hw_spi_master_WriteRead(SPIn spix,uint8_t data,uint8_t pcsx,uint8_t pcs_state)
{
    uint8_t temp;

    SPI_PUSHR_REG(SPIBA[spix])  = (((uint32_t)(((uint32_t)(pcs_state))<<SPI_PUSHR_CONT_SHIFT))&SPI_PUSHR_CONT_MASK)
                                | SPI_PUSHR_CTAS(0)
                                | SPI_PUSHR_PCS(pcsx)
                                | data;                 

    while(!(SPI_SR_REG(SPIBA[spix]) & SPI_SR_TCF_MASK));
    SPI_SR_REG(SPIBA[spix]) |= SPI_SR_TCF_MASK ;               

    while(!(SPI_SR_REG(SPIBA[spix]) & SPI_SR_RFDF_MASK)); 
    temp = (uint8_t)(SPI_POPR_REG(SPIBA[spix]) & 0xff);           
    SPI_SR_REG(SPIBA[spix]) |= SPI_SR_RFDF_MASK;                

    return temp;
}


uint8_t hw_spi_master_read(SPIn spix)
{
    uint8_t temp;

    while(!(SPI_SR_REG(SPIBA[spix]) & SPI_SR_RFDF_MASK)); //=1 RIFO is not empty
    temp=(uint8_t)SPI_POPR_REG(SPIBA[spix]);
    SPI_SR_REG(SPIBA[spix]) |=SPI_SR_RFDF_MASK;

    return temp;
}

/**  
 * \brief   
 * \param 
 *         spix      spi channel:SPI0,SPI1
 *         data      data to write 
 *         pcsx      select spi cs: SPI_PCS0 ~ SPI_PCS4(SPI0)
 *                                  SPI_PCS0 ~ SPI_PCS2(SPI1)
 * \return the length of the success to send data
 * \todo    
 */
int hw_spi_master_write(SPIn spix,uint8_t pcsx,const uint8_t *input,uint32_t length)
{  
    uint32_t t = 0;
    uint32_t i;

    if (length == 0)
        return 0;
    i = length-1;
    while (i--)
    {
        SPI_PUSHR_REG(SPIBA[spix]) = SPI_PUSHR_CONT_MASK
                                   | SPI_PUSHR_CTAS(0)
                                   | SPI_PUSHR_PCS(pcsx)
                                   | *input; 
        while(!(SPI_SR_REG(SPIBA[spix]) & SPI_SR_TCF_MASK))
        {
            if (t++ > 0x200000) {
                TRACE("\nt_out");
                return length-i-2;
            }
        }
        SPI_SR_REG(SPIBA[spix]) |= SPI_SR_TCF_MASK ;            
        input++;
    }
    SPI_PUSHR_REG(SPIBA[spix]) = SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(pcsx) | *input;
    while(!(SPI_SR_REG(SPIBA[spix]) & SPI_SR_TCF_MASK))
    {
        if (t++ > 0x200000) {
            TRACE("\nt_out1");
            return length - 1;
        }
    }
    SPI_SR_REG(SPIBA[spix]) |= SPI_SR_TCF_MASK ;            
    return length;
}

//#define SPI_TX_WAIT(SPIx)     while((SPI_SR_REG(SPIBA[SPIx]) & SPI_SR_TXRXS_MASK) == 1 )  //等待发送 完成
//#define SPI_RX_WAIT(SPIx)     while((SPI_SR_REG(SPIBA[SPIx]) & SPI_SR_RFDF_MASK ) == 0 )  //等待发送 FIFO为非空
//#define SPI_EOQF_WAIT(SPIx)   while((SPI_SR_REG(SPIBA[SPIx]) & SPI_SR_EOQF_MASK ) == 0 )  //等待传输完成
//void hw_spi_master_write_bytes(SPIn spix,uint8_t *data,uint32_t len)
//{  
//    uint32_t i=0;
//    uint8_t temp;
//
//    SPI_TX_WAIT(spix);
//    do
//    {
//        /*************  清标志位  ***************/
//        SPI_SR_REG(SPIBA[spix]) = (SPI_SR_EOQF_MASK
//                                  | SPI_SR_TFUF_MASK
//                                  | SPI_SR_TFFF_MASK
//                                  | SPI_SR_RFOF_MASK
//                                  | SPI_SR_RFDF_MASK
//                                 );
//
//        /************** 清FIFO计数器 **************/
//        SPI_MCR_REG(SPIBA[spix])    |=  (SPI_MCR_CLR_TXF_MASK     //Clear TX FIFO.写1清 Tx FIFO counter
//                                        |SPI_MCR_CLR_RXF_MASK     //Clear RX FIFO. 写1清 the Rx FIFO counter.
//                                       );
//        //SPI_SR_REG(SPIBA[spix]) |= SPI_SR_RFDF_MASK;
//    }while( (SPI_SR_REG(SPIBA[spix]) & SPI_SR_RFDF_MASK));   //如果 Rx FIFO 非空，则清FIFO.
//
//    /***************** 发送len-1个数据 *******************/                                                ;
//    for(i = 0;i < (len-1);i++)
//    {
//        //DELAY_MS(1); 
//        SPI_PUSHR_REG(SPIBA[spix]) = 0 
//            | SPI_PUSHR_CONT_MASK   //Continuous Peripheral Chip Select Enable，1为 传输期间保持PCSn信号 ，即继续传输数据
//            | SPI_PUSHR_CTAS(0)
//            | SPI_PUSHR_TXDATA(data[i]);    //要传输的数据
//
//        while( !(SPI_SR_REG(SPIBA[spix]) & SPI_SR_RFDF_MASK));    //RFDF为1，Rx FIFO is not empty.
//        temp = (u8)SPI_POPR_REG(SPIBA[spix]);    //读取一次接收的数据    
//        SPI_SR_REG(SPIBA[spix]) |= SPI_SR_RFDF_MASK;
//    }
//
//    /***************** 发送最后一个数据 *******************/    
//    SPI_PUSHR_REG(SPIBA[spix]) = 0 
//        | SPI_PUSHR_CTAS(0)
//        | SPI_PUSHR_EOQ_MASK         //End Of Queue，1为 传输SPI最后的数据
//        | SPI_PUSHR_TXDATA(data[i]); 
//
//    SPI_EOQF_WAIT(spix);    //要及时把RX FIFO的东西清掉，不然这里就无限等待
//
//    while( !(SPI_SR_REG(SPIBA[spix]) & SPI_SR_RFDF_MASK));    //RFDF为1，Rx FIFO is not empty.
//    temp = (u8)SPI_POPR_REG(SPIBA[spix]);    //读取一次接收的数据          
//}

void SPI0_IRQHandler(void)
{
    if( SPI_SR_REG(SPIBA[0]) & SPI_SR_TCF_MASK)
    {
        //todo
        SPI_SR_REG(SPIBA[0]) |= SPI_SR_TCF_MASK;
    } else if( SPI_SR_REG(SPIBA[0]) & SPI_SR_EOQF_MASK)
    {
        //todo
        SPI_SR_REG(SPIBA[0]) |= SPI_SR_EOQF_MASK;
    } else if( SPI_SR_REG(SPIBA[0]) & SPI_SR_TFUF_MASK)
    {
        //todo
        SPI_SR_REG(SPIBA[0]) |= SPI_SR_TFUF_MASK;
    } else if( SPI_SR_REG(SPIBA[0]) & SPI_SR_TFFF_MASK)
    {  
        //todo
        SPI_SR_REG(SPIBA[0]) |= SPI_SR_TFFF_MASK;
    } else if( SPI_SR_REG(SPIBA[0]) & SPI_SR_RFOF_MASK)
    {   
        //todo
        SPI_SR_REG(SPIBA[0]) |= SPI_SR_RFOF_MASK;
    } else if( SPI_SR_REG(SPIBA[0]) & SPI_SR_RFDF_MASK)
    {
        //todo
        SPI_SR_REG(SPIBA[0]) |= SPI_SR_RFDF_MASK;
    }
}


void SPI1_IRQHandler(void)
{
    if( SPI_SR_REG(SPIBA[1]) & SPI_SR_TCF_MASK)
    {
        //todo
        SPI_SR_REG(SPIBA[1]) |= SPI_SR_TCF_MASK;
    } else if( SPI_SR_REG(SPIBA[1]) & SPI_SR_EOQF_MASK)
    {
        //todo
        SPI_SR_REG(SPIBA[1]) |= SPI_SR_EOQF_MASK;
    } else if( SPI_SR_REG(SPIBA[1]) & SPI_SR_TFUF_MASK)
    {
        //todo
        SPI_SR_REG(SPIBA[1]) |= SPI_SR_TFUF_MASK;
    } else if( SPI_SR_REG(SPIBA[1]) & SPI_SR_TFFF_MASK)
    {  
        //todo
        SPI_SR_REG(SPIBA[1]) |= SPI_SR_TFFF_MASK;
    } else if( SPI_SR_REG(SPIBA[1]) & SPI_SR_RFOF_MASK)
    {   
        //todo
        SPI_SR_REG(SPIBA[1]) |= SPI_SR_RFOF_MASK;
    } else if( SPI_SR_REG(SPIBA[1]) & SPI_SR_RFDF_MASK)
    {
        //todo
        SPI_SR_REG(SPIBA[1]) |= SPI_SR_RFDF_MASK;
    }
}


