/*
 * File:		arm_cm4.h
 * Purpose:		Definitions common to all ARM Cortex M4 processors
 *
 * Notes:
 */

#ifndef _CPU_ARM_CM4_H
#define _CPU_ARM_CM4_H

/**
 * irq number(the irq number NOT the vector number: IRQInterruptIndex)
 * loucs add
 */
typedef enum {
    /* Core interrupts */
  NMI_IRQn                    = -14,         /**< Non-maskable Interrupt (NMI) */
  HardFaultIRQn               = -13,         /**< Hard Fault */
  MemManageFault_IRQn         = -12,         /**<0 MemManage Fault */
  BusFault_IRQn               = -11,         /**<1 Bus Fault */
  UsageFault_IRQn             = -10,         /**<2 Usage Fault */
  Reserved7_IRQn              = -9,          /**<3 Reserved interrupt 7 */
  Reserved8_IRQn              = -8,          /**<4 Reserved interrupt 8 */
  Reserved9_IRQn              = -7,          /**<5 Reserved interrupt 9 */
  Reserved10_IRQn             = -6,          /**<6 Reserved interrupt 10 */
  SVCall_IRQn                 = -5,          /**<7 Supervisor call (SVCall) */
  DebugMonitor_IRQn           = -4,          /**<8 Debug Monitor */
  Reserved13_IRQn             = -3,          /**<9 Reserved interrupt 13 */
  PendSV_IRQn                 = -2,          /**<10 Pendable request for system service (PendableSrvReq) */
  SysTick_IRQn                = -1,          /**<11 SysTick Interrupt */
  /* Device specific interrupts */
  DMA0_IRQn                   = 0,           /**< DMA Channel 0 Transfer Complete */
  DMA1_IRQn                   = 1,           /**< DMA Channel 1 Transfer Complete */
  DMA2_IRQn                   = 2,           /**< DMA Channel 2 Transfer Complete */
  DMA3_IRQn                   = 3,           /**< DMA Channel 3 Transfer Complete */
  DMA4_IRQn                   = 4,           /**< DMA Channel 4 Transfer Complete */
  DMA5_IRQn                   = 5,           /**< DMA Channel 5 Transfer Complete */
  DMA6_IRQn                   = 6,           /**< DMA Channel 6 Transfer Complete */
  DMA7_IRQn                   = 7,           /**< DMA Channel 7 Transfer Complete */
  DMA8_IRQn                   = 8,           /**< DMA Channel 8 Transfer Complete */
  DMA9_IRQn                   = 9,           /**< DMA Channel 9 Transfer Complete */
  DMA10_IRQn                  = 10,          /**< DMA Channel 10 Transfer Complete */
  DMA11_IRQn                  = 11,          /**< DMA Channel 11 Transfer Complete */
  DMA12_IRQn                  = 12,          /**< DMA Channel 12 Transfer Complete */
  DMA13_IRQn                  = 13,          /**< DMA Channel 13 Transfer Complete */
  DMA14_IRQn                  = 14,          /**< DMA Channel 14 Transfer Complete */
  DMA15_IRQn                  = 15,          /**< DMA Channel 15 Transfer Complete */
  DMA_Error_IRQn              = 16,          /**< DMA Error Interrupt */
  MCM_IRQn                    = 17,          /**< Normal Interrupt */
  FTFL_IRQn                   = 18,          /**< FTFL Interrupt */
  Read_Collision_IRQn         = 19,          /**< Read Collision Interrupt */
  LVD_LVW_IRQn                = 20,          /**< Low Voltage Detect, Low Voltage Warning */
  LLW_IRQn                    = 21,          /**< Low Leakage Wakeup */
  Watchdog_IRQn               = 22,          /**< WDOG Interrupt */
  RNG_IRQn                    = 23,          /**< RNGB Interrupt */
  I2C0_IRQn                   = 24,          /**< I2C0 interrupt */
  I2C1_IRQn                   = 25,          /**< I2C1 interrupt */
  SPI0_IRQn                   = 26,          /**< SPI0 Interrupt */
  SPI1_IRQn                   = 27,          /**< SPI1 Interrupt */
  I2S0_Tx_IRQn                = 28,          /**< I2S0 transmit interrupt */
  I2S0_Rx_IRQn                = 29,          /**< I2S0 receive interrupt */
  UART0_LON_IRQn              = 30,          /**< UART0 LON interrupt */
  UART0_RX_TX_IRQn            = 31,          /**< UART0 Receive/Transmit interrupt */
  UART0_ERR_IRQn              = 32,          /**< UART0 Error interrupt */
  UART1_RX_TX_IRQn            = 33,          /**< UART1 Receive/Transmit interrupt */
  UART1_ERR_IRQn              = 34,          /**< UART1 Error interrupt */
  UART2_RX_TX_IRQn            = 35,          /**< UART2 Receive/Transmit interrupt */
  UART2_ERR_IRQn              = 36,          /**< UART2 Error interrupt */
  UART3_RX_TX_IRQn            = 37,          /**< UART3 Receive/Transmit interrupt */
  UART3_ERR_IRQn              = 38,          /**< UART3 Error interrupt */
  ADC0_IRQn                   = 39,          /**< ADC0 interrupt */
  CMP0_IRQn                   = 40,          /**< CMP0 interrupt */
  CMP1_IRQn                   = 41,          /**< CMP1 interrupt */
  FTM0_IRQn                   = 42,          /**< FTM0 fault, overflow and channels interrupt */
  FTM1_IRQn                   = 43,          /**< FTM1 fault, overflow and channels interrupt */
  FTM2_IRQn                   = 44,          /**< FTM2 fault, overflow and channels interrupt */
  CMT_IRQn                    = 45,          /**< CMT interrupt */
  RTC_IRQn                    = 46,          /**< RTC interrupt */
  RTC_Seconds_IRQn            = 47,          /**< RTC seconds interrupt */
  PIT0_IRQn                   = 48,          /**< PIT timer channel 0 interrupt */
  PIT1_IRQn                   = 49,          /**< PIT timer channel 1 interrupt */
  PIT2_IRQn                   = 50,          /**< PIT timer channel 2 interrupt */
  PIT3_IRQn                   = 51,          /**< PIT timer channel 3 interrupt */
  PDB0_IRQn                   = 52,          /**< PDB0 Interrupt */
  USB0_IRQn                   = 53,          /**< USB0 interrupt */
  USBDCD_IRQn                 = 54,          /**< USBDCD Interrupt */
  Tamper_IRQn                 = 55,          /**< Tamper detect interrupt */
  DAC0_IRQn                   = 56,          /**< DAC0 interrupt */
  MCG_IRQn                    = 57,          /**< MCG Interrupt */
  LPTimer_IRQn                = 58,          /**< LPTimer interrupt */
  PORTA_IRQn                  = 59,          /**< Port A interrupt */
  PORTB_IRQn                  = 60,          /**< Port B interrupt */
  PORTC_IRQn                  = 61,          /**< Port C interrupt */
  PORTD_IRQn                  = 62,          /**< Port D interrupt */
  PORTE_IRQn                  = 63,          /**< Port E interrupt */
  SWI_IRQn                    = 64,          /**< Software interrupt */
  SPI2_IRQn                   = 65,          /**< SPI2 Interrupt */
  UART4_RX_TX_IRQn            = 66,          /**< UART4 Receive/Transmit interrupt */
  UART4_ERR_IRQn              = 67,          /**< UART4 Error interrupt */
  UART5_RX_TX_IRQn            = 68,          /**< UART5 Receive/Transmit interrupt */
  UART5_ERR_IRQn              = 69,          /**< UART5 Error interrupt */
  CMP2_IRQn                   = 70,          /**< CMP2 interrupt */
  FTM3_IRQn                   = 71,          /**< FTM3 fault, overflow and channels interrupt */
  DAC1_IRQn                   = 72,          /**< DAC1 interrupt */
  ADC1_IRQn                   = 73,          /**< ADC1 interrupt */
  I2C2_IRQn                   = 74,          /**< I2C2 interrupt */
  CAN0_ORM_IRQn               = 75,          /**< CAN0 interrupt:OR'ed Message buffer (0-15) */
  CAN0_BusOff_IRQn            = 76,          /**< CAN0 interrupt:Bus Off */
  CAN0_ERR_IRQn               = 77,          /**< CAN0 interrupt:Error*/
  CAN0_Tran_IRQn              = 78,          /**< CAN0 interrupt:Transmit Warning*/
  CAN0_Recv_IRQn              = 79,          /**< CAN0 interrupt:Receive Warning*/
  CAN0_WakeUp_IRQn            = 80,          /**< CAN0 interrupt:Wake Up */
  SDHC_IRQn                   = 81,          /**< SDHC interrupt */
}IRQn_Type;


/***********************************************************************/
/*ARM Cortex M4 implementation for interrupt priority shift*/
#define ARM_INTERRUPT_LEVEL_BITS          4


/***********************************************************************/
  /*!< Macro to enable all interrupts. */
#ifndef KEIL
#define EnableInterrupts asm(" CPSIE i");
#else
#define EnableInterrupts  __enable_irq()
#endif

  /*!< Macro to disable all interrupts. */
#ifndef KEIL
#define DisableInterrupts asm(" CPSID i");
#else
#define DisableInterrupts __disable_irq()
#endif
/***********************************************************************/


/*
 * Misc. Defines
 */
#ifdef	FALSE
#undef	FALSE
#endif
#define FALSE	(0)

#ifdef	TRUE
#undef	TRUE
#endif
#define	TRUE	(1)

#ifdef	NULL
#undef	NULL
#endif
#define NULL	(0)

#ifdef  ON
#undef  ON
#endif
#define ON      (1)

#ifdef  OFF
#undef  OFF
#endif
#define OFF     (0)

/***********************************************************************/
/*
 * The basic data types
 */
typedef unsigned char		uint8;  /*  8 bits */
typedef unsigned short int	uint16; /* 16 bits */
typedef unsigned long int	uint32; /* 32 bits */
//typedef unsigned int	uint32_t; /* 32 bits */
//typedef unsigned char		uint8_t;  /*  8 bits */
//typedef unsigned short int	uint16_t; /* 16 bits */

typedef char			int8;   /*  8 bits */
typedef short int	        int16;  /* 16 bits */
typedef int		        int32;  /* 32 bits */
//typedef int		        int32_t;  /* 32 bits */

typedef volatile int8		vint8;  /*  8 bits */
typedef volatile int16		vint16; /* 16 bits */
typedef volatile int32		vint32; /* 32 bits */

typedef volatile uint8		vuint8;  /*  8 bits */
typedef volatile uint16		vuint16; /* 16 bits */
typedef volatile uint32		vuint32; /* 32 bits */

/***********************************************************************/
// function prototypes for arm_cm4.c
void stop (void);
void wait (void);
void write_vtor (int);
void enable_irq (int);
void disable_irq (int);
void set_irq_priority (int, int);
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
void NVIC_SystemReset(void);
void NVIC_EnableIRQ(IRQn_Type IRQn);
void NVIC_DisableIRQ(IRQn_Type IRQn);

/***********************************************************************/

// function prototype for main function
int main(void);

/***********************************************************************/
#endif	/* _CPU_ARM_CM4_H */

