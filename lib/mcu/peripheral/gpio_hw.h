/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
* File Name          : systick_hw.h
* Author             : luocs
* Version            : V0.00
* Date               : 2014.01.02
* Description        : 
*******************************************************************************/
#ifndef __GPIO_HW_H__
#define __GPIO_HW_H__ 


/** 
  * @brief PORT Configuration Mode enumeration:*PORT_MemMapPtr 
  * Pin Control Register n (PORTx_PCRn):Register accessor enumeration
  */
typedef enum
{ 
    PORT_Mode_IN_PDIS = (0 << PORT_PCR_PE_SHIFT) | (0 << PORT_PCR_PS_SHIFT), //Input internal Pull disable
    PORT_Mode_IN_PD   = (1 << PORT_PCR_PE_SHIFT) | (0 << PORT_PCR_PS_SHIFT), //Input internal PullDown resistor
    PORT_Mode_IN_PU   = (1 << PORT_PCR_PE_SHIFT) | (1 << PORT_PCR_PS_SHIFT), //Input internal PullUp resistor

    PORT_Mode_Out_SRF  = (0 << PORT_PCR_SRE_SHIFT), //Output Slew Rate Fast
    PORT_Mode_Out_SRS  = (1 << PORT_PCR_SRE_SHIFT), //Output Slew Rate Sast

    PORT_Mode_IN_PFD  = (0 << PORT_PCR_PFE_SHIFT), //Passive Input Filter Disable
    PORT_Mode_IN_PFE  = (1 << PORT_PCR_PFE_SHIFT), //Passive Input Filter Enable

    PORT_Mode_Out_ODD  = (0 << PORT_PCR_ODE_SHIFT), //Open Drain Oupt Disable
    PORT_Mode_Out_ODE  = (1 << PORT_PCR_ODE_SHIFT), //Open Drain Oupt Enable

    PORT_Mode_Out_DSL  = (0 << PORT_PCR_DSE_SHIFT), //Oupt Low Drive Strength
    PORT_Mode_Out_DSH  = (1 << PORT_PCR_DSE_SHIFT), //Oupt High Drive Strength

    PORT_Mode_MUX_DIS  =  PORT_PCR_MUX(0),  //Pin Mux Control:000 pin disabled
    PORT_Mode_MUX_gpio =  PORT_PCR_MUX(1),  //Pin Mux Control:001 alt1:GPIO
    PORT_Mode_MUX_alt2 =  PORT_PCR_MUX(2),  //Pin Mux Control:010 alt2
    PORT_Mode_MUX_alt3 =  PORT_PCR_MUX(3),  //Pin Mux Control:011 alt3
    PORT_Mode_MUX_alt4 =  PORT_PCR_MUX(4),  //Pin Mux Control:100 alt4
    PORT_Mode_MUX_alt5 =  PORT_PCR_MUX(5),  //Pin Mux Control:101 alt5
    PORT_Mode_MUX_alt6 =  PORT_PCR_MUX(6),  //Pin Mux Control:110 alt6
    PORT_Mode_MUX_alt7 =  PORT_PCR_MUX(7),  //Pin Mux Control:111 alt7

    PORT_Mode_LK_NOT  = (0 << PORT_PCR_LK_SHIFT), //Pin Control Register fields [15:0] are not locked
    PORT_Mode_LK_YES  = (1 << PORT_PCR_LK_SHIFT), //Pin Control Register fields [15:0] are locked and
                                                  // cannot be updated until the next system reset
    PORT_Mode_IRQ_DIS        = PORT_PCR_IRQC(0),  //0000 Interrupt/DMA request disabled.
    PORT_Mode_IRQ_DMA_REdge  = PORT_PCR_IRQC(1),  //0001 DMA request on rising edge.    
    PORT_Mode_IRQ_DMA_FEdge  = PORT_PCR_IRQC(2),  //0010 DMA request on falling edge.   
    PORT_Mode_IRQ_DMA_EEdge  = PORT_PCR_IRQC(3),  //0011 DMA request on either edge.    
    PORT_Mode_IRQ_EXTI_Zero  = PORT_PCR_IRQC(8),  //1000 Interrupt when logic zero.     
    PORT_Mode_IRQ_EXTI_REdge = PORT_PCR_IRQC(9),  //1001 Interrupt on rising edge.      
    PORT_Mode_IRQ_EXTI_FEdge = PORT_PCR_IRQC(10), //1010 Interrupt on falling edge.     
    PORT_Mode_IRQ_EXTI_EEdge = PORT_PCR_IRQC(11), //1011 Interrupt on either edge.      
    PORT_Mode_IRQ_EXTI_One   = PORT_PCR_IRQC(12)  //1100 Interrupt when logic one.      
//    PORT_Mode_ISF  = (1 << PORT_PCR_ISF_SHIFT), //Interrupt Status Flag: w1c
}PORTMode_TypeDef;

typedef enum 
{
    PORT_Pin_0 = 0,
    PORT_Pin_1,
    PORT_Pin_2,
    PORT_Pin_3,
    PORT_Pin_4,
    PORT_Pin_5,
    PORT_Pin_6,
    PORT_Pin_7,
    PORT_Pin_8,
    PORT_Pin_9,
    PORT_Pin_10,
    PORT_Pin_11,
    PORT_Pin_12,
    PORT_Pin_13,
    PORT_Pin_14,
    PORT_Pin_15,
    PORT_Pin_16,
    PORT_Pin_17,
    PORT_Pin_18,
    PORT_Pin_19,
    PORT_Pin_20,
    PORT_Pin_21,
    PORT_Pin_22,
    PORT_Pin_23,
    PORT_Pin_24,
    PORT_Pin_25,
    PORT_Pin_26,
    PORT_Pin_27,
    PORT_Pin_28,
    PORT_Pin_29,
    PORT_Pin_30,
    PORT_Pin_31,
    PORT_Pin_Invalid,
}PORTPin_TypeDef;

/** 
  * @brief  PORT Init structure definition  
  */

typedef struct
{
  PORT_MemMapPtr PORTx;       /*!< This parameter can be one of the vale: PORTA_BASE_PTR,
                                |PORTB_BASE_PTR|PORTC_BASE_PTR|PORTD_BASE_PTR|PORTE_BASE_PTR|*/
  PORTPin_TypeDef PORT_Pin;   /*!< Specifies the GPIO pins to be configured.
                                 This parameter can be any value of @ref PORTPin_TypeDef */

  uint32_t PORT_Mode;         /*!< Specifies the operating mode for the selected pins.
                                   This parameter can be a value of @ref PORTMode_TypeDef */
}PORT_InitTypeDef;


typedef enum
{ 
    GPIO_Pin_0  = ((uint32_t)0x0001), /*!< Pin 0 selected */
    GPIO_Pin_1  = ((uint32_t)0x0002), /*!< Pin 1 selected */
    GPIO_Pin_2  = ((uint32_t)0x0004), /*!< Pin 2 selected */
    GPIO_Pin_3  = ((uint32_t)0x0008), /*!< Pin 3 selected */
    GPIO_Pin_4  = ((uint32_t)0x0010), /*!< Pin 4 selected */
    GPIO_Pin_5  = ((uint32_t)0x0020), /*!< Pin 5 selected */
    GPIO_Pin_6  = ((uint32_t)0x0040), /*!< Pin 6 selected */
    GPIO_Pin_7  = ((uint32_t)0x0080), /*!< Pin 7 selected */
    GPIO_Pin_8  = ((uint32_t)0x0100), /*!< Pin 8 selected */
    GPIO_Pin_9  = ((uint32_t)0x0200), /*!< Pin 9 selected */
    GPIO_Pin_10 = ((uint32_t)0x0400), /*!< Pin 10 selected */
    GPIO_Pin_11 = ((uint32_t)0x0800), /*!< Pin 11 selected */
    GPIO_Pin_12 = ((uint32_t)0x1000), /*!< Pin 12 selected */
    GPIO_Pin_13 = ((uint32_t)0x2000), /*!< Pin 13 selected */
    GPIO_Pin_14 = ((uint32_t)0x4000), /*!< Pin 14 selected */
    GPIO_Pin_15 = ((uint32_t)0x8000), /*!< Pin 15 selected */
    GPIO_Pin_16 = ((uint32_t)0x00010000), /*!< Pin 16 selected */
    GPIO_Pin_17 = ((uint32_t)0x00020000), /*!< Pin 17 selected */
    GPIO_Pin_18 = ((uint32_t)0x00040000), /*!< Pin 18 selected */
    GPIO_Pin_19 = ((uint32_t)0x00080000), /*!< Pin 19 selected */
    GPIO_Pin_20 = ((uint32_t)0x00100000), /*!< Pin 20 selected */
    GPIO_Pin_21 = ((uint32_t)0x00200000), /*!< Pin 21 selected */
    GPIO_Pin_22 = ((uint32_t)0x00400000), /*!< Pin 22 selected */
    GPIO_Pin_23 = ((uint32_t)0x00800000), /*!< Pin 23 selected */
    GPIO_Pin_24 = ((uint32_t)0x01000000), /*!< Pin 24 selected */
    GPIO_Pin_25 = ((uint32_t)0x02000000), /*!< Pin 25 selected */
    GPIO_Pin_26 = ((uint32_t)0x04000000), /*!< Pin 26 selected */
    GPIO_Pin_27 = ((uint32_t)0x08000000), /*!< Pin 27 selected */
    GPIO_Pin_28 = ((uint32_t)0x10000000), /*!< Pin 28 selected */
    GPIO_Pin_29 = ((uint32_t)0x20000000), /*!< Pin 29 selected */
    GPIO_Pin_30 = ((uint32_t)0x40000000), /*!< Pin 30 selected */
    GPIO_Pin_31 = ((uint32_t)0x80000000), /*!< Pin 31 selected */
}GPIOPin_TypeDef;

#define GPIO_Type  GPIO_MemMapPtr

//#define GPIOA          ((GPIO_Type volatile *)PTA_BASE_PTR)  
//#define GPIOB          ((GPIO_Type volatile *)PTB_BASE_PTR) 
//#define GPIOC          ((GPIO_Type volatile *)PTC_BASE_PTR)  
//#define GPIOD          ((GPIO_Type volatile *)PTD_BASE_PTR)  
//#define GPIOE          ((GPIO_Type volatile *)PTE_BASE_PTR)  

#define GPIOA          (PTA_BASE_PTR)  
#define GPIOB          (PTB_BASE_PTR) 
#define GPIOC          (PTC_BASE_PTR)  
#define GPIOD          (PTD_BASE_PTR)  
#define GPIOE          (PTE_BASE_PTR)  

/** @defgroup GPIO_Exported_Types
  * @{
  */

#define IS_GPIO_ALL_PERIPH(PERIPH) (((PERIPH) == GPIOA) || \
                                    ((PERIPH) == GPIOB) || \
                                    ((PERIPH) == GPIOC) || \
                                    ((PERIPH) == GPIOD) || \
                                    ((PERIPH) == GPIOE))

//  GPIO_Type GPIO_PTx;       /*!< This parameter can be one of the vale:
//                                      GPIOA,GPIOB,GPIOB,GPIOD,GPIOE */

/** 
  * @brief  GPIO Init structure definition  
  */

typedef struct
{
  GPIO_Type GPIOx;       /*!< This parameter can be one of the vale:
                                      GPIOA,GPIOB,GPIOC,GPIOD,GPIOE */

  PORTPin_TypeDef PORT_Pin;      /*!< Specifies the GPIO pins to be configured.
                                      This parameter can be any value of @ref PORTPin_TypeDef */

  uint32_t PORT_Mode;    /*!< Specifies the operating mode for the selected pins.
                                      This parameter can be a value of @ref PORTMode_TypeDef */
  uint32_t GPIO_Pinx_Mode;
}GPIO_InitTypeDef;

#define GPIO_INPUT(pinx)     (0<<(pinx))
#define GPIO_OUTPUT(pinx)    (1<<(pinx))


#define PORTx_IRQPinx(PORTX, PINX)  (PORTX->ISFR&(0x01<<PINX))
#define PORTx_IRQPinx_Clear(PORTX,PINX)     (PORTX->ISFR=(0x01<<PINX))
#define PORTx_IRQPin_Clear(PORTX,VAL,PINX)     (PORTX->ISFR=(VAL<<PINX))
#define PORTx_IRQ_Clear(PORTX)     (PORTX->ISFR=0xFFFFFFFFU)

/** @defgroup GPIO_Exported_Functions
  * @{
  */

void hw_port_init(PORT_InitTypeDef *port_initstruct);
void hw_gpio_init(GPIO_InitTypeDef *gpio_initstruct);
void hw_set_gpio_input(GPIO_Type GPIOx, uint32_t GPIO_Pin);
void hw_set_gpio_output(GPIO_Type GPIOx, uint32_t GPIO_Pin);
void hw_gpio_set_bits(GPIO_Type GPIOx, uint32_t GPIO_Pin);
void hw_gpio_reset_bits(GPIO_Type GPIOx, uint32_t GPIO_Pin);
void hw_gpio_inverse_bits(GPIO_Type GPIOx, uint32_t GPIO_Pin);
void hw_gpio_set_value(GPIO_Type GPIOx,uint32_t PortVal);
uint32_t hw_gpio_get_value(GPIO_Type GPIOx);
uint8_t hw_gpio_get_bit(GPIO_Type GPIOx, uint32_t GPIO_Pin);
PORT_MemMapPtr hw_gpio2port (GPIO_Type GPIOx);

#define GPIO_INPUT_PULLDISABLE   (PORT_Mode_IN_PDIS) //default
#define GPIO_INPUT_PULLDOWN      (PORT_Mode_IN_PD) 
#define GPIO_INPUT_PULLUP        (PORT_Mode_IN_PU) 
#define GPIO_INPUT_FILTERDISABLE (PORT_Mode_IN_PFD)  //default 
#define GPIO_INPUT_FILTERENABLE  (PORT_Mode_IN_PFE) 
#define GPIO_INPUT_MASK          (PORT_Mode_IN_PU|GPIO_INPUT_FILTERENABLE)
int gpio_set_input(GPIOPin_Def pin,uint32_t mode);
int gpio_get_bit(GPIOPin_Def pin);

#define GPIO_OUTPUT_OpenDrainDisable     (PORT_Mode_Out_ODD) //default 
#define GPIO_OUTPUT_OpenDrainEnable      (PORT_Mode_Out_ODE) 
#define GPIO_OUTPUT_LowDriveStrength     (PORT_Mode_Out_DSL) //default
#define GPIO_OUTPUT_HighDriveStrength    (PORT_Mode_Out_DSH) 
#define GPIO_OUTPUT_SlewRateFast         (PORT_Mode_Out_SRF) //default 
#define GPIO_OUTPUT_SlewRateSast         (PORT_Mode_Out_SRS) 
#define GPIO_OUTPUT_MASK                 (GPIO_OUTPUT_OpenDrainEnable|GPIO_OUTPUT_HighDriveStrength|GPIO_OUTPUT_SlewRateSast)
int gpio_set_output(GPIOPin_Def pin,uint32_t mode, uint32_t level);
int gpio_set_bit(GPIOPin_Def pin,uint32_t level);


#endif /* __HW_GPIO_H__ */
