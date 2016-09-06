/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
* File Name          : flash_hw.c
* bfief              : The drivers level of UART
* Author             : luocs
* Version            : V0.00
* Date               : 2014.01.02
* Description        : 
*******************************************************************************/
/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "uart_hw.h"
#include "cpu.h"

volatile struct UART_MemMap *const UARTx[5] = {
    UART0_BASE_PTR,UART1_BASE_PTR,UART2_BASE_PTR,UART3_BASE_PTR, UART4_BASE_PTR
};

#define UART0_RTSE  ((UART0_TXRTS_RXRTS_ENABLE==RXRTSE)?(UART_MODEM_RXRTSE_MASK):(UART_MODEM_TXRTSE_MASK|((UART0_TXRTS_ACTIVE_LEVEL!=0)<<UART_MODEM_TXRTSPOL_SHIFT)))
#define UART1_RTSE  ((UART1_TXRTS_RXRTS_ENABLE==RXRTSE)?(UART_MODEM_RXRTSE_MASK):(UART_MODEM_TXRTSE_MASK|((UART1_TXRTS_ACTIVE_LEVEL!=0)<<UART_MODEM_TXRTSPOL_SHIFT)))
#define UART2_RTSE  ((UART2_TXRTS_RXRTS_ENABLE==RXRTSE)?(UART_MODEM_RXRTSE_MASK):(UART_MODEM_TXRTSE_MASK|((UART2_TXRTS_ACTIVE_LEVEL!=0)<<UART_MODEM_TXRTSPOL_SHIFT)))
#define UART3_RTSE  ((UART3_TXRTS_RXRTS_ENABLE==RXRTSE)?(UART_MODEM_RXRTSE_MASK):(UART_MODEM_TXRTSE_MASK|((UART3_TXRTS_ACTIVE_LEVEL!=0)<<UART_MODEM_TXRTSPOL_SHIFT)))
#define UART4_RTSE  ((UART4_TXRTS_RXRTS_ENABLE==RXRTSE)?(UART_MODEM_RXRTSE_MASK):(UART_MODEM_TXRTSE_MASK|((UART4_TXRTS_ACTIVE_LEVEL!=0)<<UART_MODEM_TXRTSPOL_SHIFT)))


/**  
 * \brief   Initialize the UART for 8N1 operation, interrupts disabled, and 
 *          no hardware flow-control
 * \author   
 * \param 
 *         uartch      UART channel to initialize:UART0~UART4
 *         baud        UART baud rate
 * \return none
 * \todo    
 * \warning  Since the UARTs are pinned out in multiple locations on most
 *       Kinetis devices, this driver does not enable UART pin functions.
 *       The desired pins should be enabled before calling this init function. 
 */
void hw_uart_init(UARTn uartch, uint32_t baud)
{
    register uint16 sbr, brfa;
    uint8 temp,uart_modem;
    uint32_t sysclk;

    uart_modem = UART_MODEM_TXCTSE_MASK;
    /* Enable the pins for the selected UART */
    switch (uartch)
    {
    case UART0:
        /* Enable the UART0_RXD function on PTA15 */
        if (UART0_RXD == PTA1){
            PORTA_PCR1 = PORT_PCR_MUX(0x2); // UART is alt2 function for this pin
        } else if (UART0_RXD == PTA15){
            PORTA_PCR15 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART0_RXD == PTB16){
            PORTB_PCR16 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART0_RXD == PTD6){
            PORTD_PCR6 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else
            ASSERT(0);

        /* Enable the UART0_TXD function on PTA1 */
        if (UART0_TXD == PTA2){
            PORTA_PCR2 = PORT_PCR_MUX(0x2); // UART is alt2 function for this pin
        } else if (UART0_TXD == PTA14){
            PORTA_PCR14 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART0_TXD == PTB17){
            PORTB_PCR17 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART0_TXD == PTD7){
            PORTD_PCR7 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else
            ASSERT(0);

        /* Enable the UART0_CTS function*/
        if (UART0_CTS == PTA0){
            PORTA_PCR0 = PORT_PCR_MUX(0x2); // UART is alt2 function for this pin
        } else if (UART0_CTS == PTA16){
            PORTA_PCR16 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART0_CTS == PTB3){
            PORTB_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART0_CTS == PTD5){
            PORTD_PCR5 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else {
            uart_modem = 0;
            if (uartch != CNL_COMPORT)
            {
//            TRACE("\nUART0 CTS %s",(UART0_CTS == PTxInvid)?"NOTUSED":"pin ERROR!");
            }
        }

        uart_modem |= UART0_RTSE;
        /* Enable the UART0_RTS function*/
        if (UART0_RTS == PTA3){
            PORTA_PCR3 = PORT_PCR_MUX(0x2); // UART is alt2 function for this pin
        } else if (UART0_RTS == PTA17){
            PORTA_PCR17 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART0_RTS == PTB2){
            PORTB_PCR2 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART0_RTS == PTD4){
            PORTD_PCR4 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else {
            if (uartch != CNL_COMPORT)
            {
//            TRACE("\nUART0 RTS %s",(UART0_RTS== PTxInvid)?"NOTUSED":"pin ERROR!");
            }
            uart_modem ^= UART0_RTSE;
        }
        break;
    case UART1:
        /* Enable the UART1_RXD function on PTA15 */
        if (UART1_RXD == PTE1){
            PORTE_PCR1 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART1_RXD == PTC3){
            PORTC_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else
            ASSERT(0);

        /* Enable the UART1_TXD function on PTA1 */
        if (UART1_TXD == PTE0){
            PORTE_PCR0 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART1_TXD == PTC4){
            PORTC_PCR4 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else
            ASSERT(0);

        /* Enable the UART1_CTS function*/
        if (UART1_CTS == PTE2){
            PORTE_PCR2 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART1_CTS == PTC2){
            PORTC_PCR2 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else {
            uart_modem = 0;
            if (uartch != CNL_COMPORT)
            {
//            TRACE("\nUART1 CTS %s",(UART1_CTS == PTxInvid)?"NOTUSED":"pin ERROR!");
            }
        }

        uart_modem |= UART1_RTSE;
        /* Enable the UART1_RTS function*/
        if (UART1_RTS == PTE3){
            PORTE_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART1_RTS == PTC1){
            PORTC_PCR1 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else {
            uart_modem ^= UART1_RTSE;
            if (uartch != CNL_COMPORT)
            {
//            TRACE("\nUART1 RTS %s",(UART1_RTS  == PTxInvid)?"NOTUSED":"pin ERROR!");
            }
        }
        break;
    case UART2:
        /* Enable the UART2_RXD function on PTA15 */
        if (UART2_RXD == PTE17){
            PORTE_PCR17 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART2_RXD == PTD2){
            PORTD_PCR2 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else
            ASSERT(0);

        /* Enable the UART2_TXD function on PTA1 */
        if (UART2_TXD == PTE16){
            PORTE_PCR16 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART2_TXD == PTD3){
            PORTD_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else
            ASSERT(0);

        /* Enable the UART2_CTS function*/
        if (UART2_CTS == PTE18){
            PORTE_PCR18 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART2_CTS == PTD1){
            PORTD_PCR1 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else {
            uart_modem = 0;
            if (uartch != CNL_COMPORT)
            {
//            TRACE("\nUART2 CTS %s",(UART2_CTS == PTxInvid)?"NOTUSED":"pin ERROR!");
            }
        }

        uart_modem |= UART2_RTSE;
        /* Enable the UART2_RTS function*/
        if (UART2_RTS == PTE19){
            PORTE_PCR19 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART2_RTS == PTD0){
            PORTD_PCR0 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else {
            uart_modem ^= UART2_RTSE;
            if (uartch != CNL_COMPORT)
            {
//            TRACE("\nUART2 RTS %s",(UART2_RTS == PTxInvid)?"NOTUSED":"pin ERROR!");
            }
        }
        break;
    case UART3:
        /* Enable the UART3_RXD function on PTA15 */
        if (UART3_RXD == PTE5){
            PORTE_PCR5 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART3_RXD == PTB10){
            PORTB_PCR10 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART3_RXD == PTC16){
            PORTC_PCR16 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else
            ASSERT(0);

        /* Enable the UART3_TXD function on PTA1 */
        if (UART3_TXD == PTE4){
            PORTE_PCR4 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART3_TXD == PTB11){
            PORTB_PCR11 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else if (UART3_TXD == PTC17){
            PORTC_PCR17 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else
            ASSERT(0);

        /* Enable the UART3_CTS function*/
        if (UART3_CTS == PTB13){
            PORTB_PCR13 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else {
            uart_modem = 0;
            if (uartch != CNL_COMPORT)
            {
//            TRACE("\nUART3 CTS %s",(UART3_CTS  == PTxInvid)?"NOTUSED":"pin ERROR!");
            }
        }
        uart_modem |= UART3_RTSE;
        /* Enable the UART3_RTS function*/
        if (UART3_RTS == PTB12){
            PORTB_PCR12 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else {
            uart_modem ^= UART3_RTSE;
            if (uartch != CNL_COMPORT)
            {
//            TRACE("\nUART3 RTS %s",(UART3_RTS  == PTxInvid)?"NOTUSED":"pin ERROR!");
            }
        }

//        /* Enable the clock to the UART3 */    
//        SIM_SCGC4 |= SIM_SCGC4_UART3_MASK;
        break;
    case UART4:
        /* Enable the UART4_RXD function on PTA15 */
        if (UART4_RXD == PTC14){
            PORTC_PCR14 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else
            ASSERT(0);

        /* Enable the UART4_TXD function on PTA1 */
        if (UART4_TXD == PTC15){
            PORTC_PCR15 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else
            ASSERT(0);

        /* Enable the UART3_CTS function*/
        if (UART4_CTS == PTC13){
            PORTC_PCR13 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else {
            uart_modem = 0;
            if (uartch != CNL_COMPORT)
            {
//            TRACE("\nUART3 CTS %s",(UART3_CTS  == PTxInvid)?"NOTUSED":"pin ERROR!");
            }
        }
        uart_modem |= UART4_RTSE;
        /* Enable the UART4_RTS function*/
        if (UART4_RTS == PTC12){
            PORTC_PCR12 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
        } else {
            uart_modem ^= UART4_RTSE;
            if (uartch != CNL_COMPORT)
            {
//            TRACE("\nUART3 RTS %s",(UART3_RTS  == PTxInvid)?"NOTUSED":"pin ERROR!");
            }
        }
        break;
    default:
        break;
    }
    /* Enable the clock to the UARTx */    
    if ((uartch == UART0)||(uartch == UART1)||(uartch == UART2)||(uartch == UART3)) {
        SIM_SCGC4 |= SIM_SCGC4_UART0_MASK<<uartch;
    }else if ( uartch == UART4 ) {
        SIM_SCGC1 |= SIM_SCGC1_UART4_MASK;
    }
    /* Make sure that the transmitter and receiver are disabled while we 
     * change settings.
     */
    UART_C2_REG(UARTx[uartch]) &= ~(UART_C2_TE_MASK
                                    | UART_C2_RE_MASK );

    /* Configure the UART for 8-bit mode, no parity */
    UART_C1_REG(UARTx[uartch]) = 0;	/* We need all default settings, so entire register is cleared */


    if ( uartch == UART0 || uartch == UART1)
        sysclk = core_clk_khz;     //core clock
    else
        sysclk = periph_clk_khz;  //peripheral clock

    /* Calculate baud settings */
    sbr = (uint16)((sysclk*1000)/(baud * 16));

    /* Save off the current value of the UARTx_BDH except for the SBR field */
    temp = UART_BDH_REG(UARTx[uartch]) & ~(UART_BDH_SBR(0x1F));

    UART_BDH_REG(UARTx[uartch]) = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
    UART_BDL_REG(UARTx[uartch]) = (uint8)(sbr & UART_BDL_SBR_MASK);

    /* Determine if a fractional divider is needed to get closer to the baud rate */
    brfa = (((sysclk * 32000)/(baud<<4)) - (sbr<<5));

    /* Save off the current value of the UARTx_C4 register except for the BRFA field */
    temp = UART_C4_REG(UARTx[uartch]) & ~(UART_C4_BRFA(0x1F));

    UART_C4_REG(UARTx[uartch]) = temp |  UART_C4_BRFA(brfa);    

//    UART_MODEM_REG(UARTx[uartch]) = uart_modem;

    /* Enable receiver and transmitter */
    UART_C2_REG(UARTx[uartch]) |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
    UART_MODEM_REG(UARTx[uartch]) = uart_modem;
//    TRACE("\n-|modem:%08b-%08b",uart_modem,UART_MODEM_REG(UARTx[uartch]));
}

/**  
 * \brief    enable the received irq on the specified UART
 * \author   
 * \param    uartch      UART channel 
 * \return   none
 * \warning
 */
void hw_uart_irq_open(UARTn uartch)
{
    /* Enable uart revcive interrupts*/
    UART_C2_REG(UARTx[uartch]) |= UART_C2_RIE_MASK;
    /* Enable revcive pin irq interrupts*/
    if ( uartch <  UART4) {
        enable_irq((uartch << 1) + UART0_RX_TX_IRQn);		
    }else{
        enable_irq(((uartch-UART4) << 1) + UART4_RX_TX_IRQn); 
    }
}

/**  
 * \brief    disable the received irq on the specified UART
 * \author   
 * \param    uartch      UART channel 
 * \return   none
 * \warning
 */
void hw_uart_irq_close(UARTn uartch)
{
    /* Disable uart revcive interrupts*/
    UART_C2_REG(UARTx[uartch]) &= UART_C2_RIE_MASK;
    /* Disable revcive pin irq interrupts*/
    if ( uartch <  UART4) {
        disable_irq((uartch << 1) + UART0_RX_TX_IRQn);		
    }else{
        disable_irq(((uartch-UART4) << 1) + UART4_RX_TX_IRQn); 
    }
    /* Make sure that the transmitter and receiver are disabled while we 
     * change settings.
     */
    UART_C2_REG(UARTx[uartch]) &= ~(UART_C2_TE_MASK
                                    | UART_C2_RE_MASK );
    /* disable the clock to the UARTx */    
    if ( uartch <  UART4) {
        SIM_SCGC4 &= ~(SIM_SCGC4_UART0_MASK<<uartch);
    }else{
        SIM_SCGC1 &= ~(SIM_SCGC1_UART4_MASK<<uartch);
    }
}



/**  
 * \brief   Wait for a character to be received on the specified UART
 * \author   
 * \param    uartch      UART channel to read from
 * \return   the received character
 * \todo    
 * \warning
 */
char hw_uart_getchar (UARTn uartch)
{
    /* Wait until character has been received */
    while (!(UART_S1_REG(UARTx[uartch]) & UART_S1_RDRF_MASK));
    
    /* Return the 8-bit data from the receiver */
    return UART_D_REG(UARTx[uartch]);
}


/**  
 * \brief   Wait for space in the UART Tx FIFO and then send a character
 * \author   
 * \param    channel  UART channel to send to
*  \param    ch	     character to send
 * \return   none
 * \todo    
 * \warning
 */
int hw_uart_putchar (UARTn  uartch, char ch)
{
    uint32_t i=0;
    /* Wait until space is available in the FIFO */
    while(!(UART_S1_REG(UARTx[uartch]) & UART_S1_TDRE_MASK))
    {
        if (i++ > 20000)
        {
            UART_D_REG(UARTx[uartch]) = (uint8)ch;
            return -1;
        }
    }
    /* Send the character */
    UART_D_REG(UARTx[uartch]) = (uint8)ch;
    return 0;
 }

void hw_uart_tx_check(UARTn uartch)
{
    uint32_t i=0;
    /* Wait until space is available in the FIFO */
    while(!(UART_S1_REG(UARTx[uartch]) & UART_S1_TC_MASK))
    {
        if (i++ > 5000)
            break;
    }
}


/**  
 * \brief   Check to see if a character has been received
 * \author   
 * \param    uartch      UART channel to check for a character
*  \param    ch	     character to send
 * \return    0       No character received
 * \return    1       Character has been received
 * \todo    
 * \warning
 */
int hw_uart_getchar_present (UARTn  uartch)
{
    return (UART_S1_REG(UARTx[uartch]) & UART_S1_RDRF_MASK);
}
/********************************************************************/
    
void UART_ITDMAConfig(uint32_t instance, UART_ITDMAConfig_Type config, bool status)
{
//    IP_CLK_ENABLE(instance);
    switch(config)
    {
        case kUART_IT_Tx:
            (status)?
            (UART_C2_REG(UARTx[instance]) |= UART_C2_TIE_MASK):
            (UART_C2_REG(UARTx[instance]) &= ~UART_C2_TIE_MASK);
            NVIC_EnableIRQ(UART_IRQnTable[instance]);
            break; 
        case kUART_IT_Rx:
            (status)?
            (UART_C2_REG(UARTx[instance]) |= UART_C2_RIE_MASK):
            (UART_C2_REG(UARTx[instance]) &= ~UART_C2_RIE_MASK);
            NVIC_EnableIRQ(UART_IRQnTable[instance]);
            break;
        case kUART_DMA_Tx:
            (status)?
            (UART_C2_REG(UARTx[instance]) |= UART_C2_TIE_MASK):
            (UART_C2_REG(UARTx[instance]) &= ~UART_C2_TIE_MASK);
             UART_C2_REG(UARTx[instance]) &= ~UART_C2_TCIE_MASK; /* 禁止发送中断,只使能DMA请求*/
            (status)?
            (UART_C5_REG(UARTx[instance]) |= UART_C5_TDMAS_MASK):
            (UART_C5_REG(UARTx[instance]) &= ~UART_C5_TDMAS_MASK);
            break;
        case kUART_DMA_Rx:
            (status)?
            (UART_C2_REG(UARTx[instance]) |= UART_C2_RIE_MASK):
            (UART_C2_REG(UARTx[instance]) &= ~UART_C2_RIE_MASK);
            (status)?
            (UART_C5_REG(UARTx[instance]) |= UART_C5_RDMAS_MASK):
            (UART_C5_REG(UARTx[instance]) &= ~UART_C5_RDMAS_MASK);
            break;
        default:
            break;
    }
}

void UART_Disable_Tx_Rx(uint32_t instance)
{
    UART_C2_REG(UARTx[instance]) &= ~((UART_C2_TE_MASK)|(UART_C2_RE_MASK));   
}

void UART_Enable_Tx_Rx(uint32_t instance)
{
    UART_C2_REG(UARTx[instance]) |= ((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
}

