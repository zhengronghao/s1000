/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
* File Name          : gpio_hw.c
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
#include "gpio_hw.h"


/**  
 * \brief   Initialize PORTx
 * \author   
 * \param 
 * \return none
 * \todo    
 * \warning 
 */
void hw_port_init(PORT_InitTypeDef *port_initstruct)
{
    PORT_PCR_REG(port_initstruct->PORTx,port_initstruct->PORT_Pin) = port_initstruct->PORT_Mode;
}
/* 
 * hw_gpio2port - [GENERIC] gpio->port
 * @ 
 */
PORT_MemMapPtr hw_gpio2port (GPIO_Type GPIOx)
{
    if (GPIOx == GPIOA)
    {
        return PORTA_BASE_PTR;
    } else if (GPIOx == GPIOB)
    {
        return PORTB_BASE_PTR;
    } else if (GPIOx == GPIOC)
    {
        return PORTC_BASE_PTR;
    } else if (GPIOx == GPIOD)
    {
        return PORTD_BASE_PTR;
    } else if (GPIOx == GPIOE)
    {
        return PORTE_BASE_PTR;
    } else {
        ASSERT(0);
        return 0;
    }
}		/* -----  end of function hw_gpio2port  ----- */
/**  
 * \brief   Initialize PORTx
 * \author   
 * \param 
 *         uartch     
 *         baud      
 * \return none
 * \todo    
 * \warning 
 */
void hw_gpio_init(GPIO_InitTypeDef *gpio_initstruct)
{
    PORT_InitTypeDef port_init;

    ASSERT(IS_GPIO_ALL_PERIPH(gpio_initstruct->GPIOx));
    if (gpio_initstruct->GPIOx == GPIOA)
    {
        port_init.PORTx = PORTA_BASE_PTR;
    } else if (gpio_initstruct->GPIOx == GPIOB)
    {
        port_init.PORTx = PORTB_BASE_PTR;
    } else if (gpio_initstruct->GPIOx == GPIOC)
    {
        port_init.PORTx = PORTC_BASE_PTR;
    } else if (gpio_initstruct->GPIOx == GPIOD)
    {
        port_init.PORTx = PORTD_BASE_PTR;
    } else if (gpio_initstruct->GPIOx == GPIOE)
    {
        port_init.PORTx = PORTE_BASE_PTR;
    }
    port_init.PORT_Mode = gpio_initstruct->PORT_Mode;
    port_init.PORT_Pin = gpio_initstruct->PORT_Pin;
    hw_port_init(&port_init);
    if (gpio_initstruct->GPIO_Pinx_Mode & GPIO_OUTPUT(gpio_initstruct->PORT_Pin)) {
        GPIO_PDDR_REG(gpio_initstruct->GPIOx) |= GPIO_OUTPUT(gpio_initstruct->PORT_Pin);
    } else {
        GPIO_PDDR_REG(gpio_initstruct->GPIOx) &= ~GPIO_OUTPUT(gpio_initstruct->PORT_Pin);
    }
}

/**
  * @brief  Configure the selected pins port data for an input.
  * @param  GPIOx: where x can be (A..E) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bits to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..31).
  * @retval None
  */
void hw_set_gpio_input(GPIO_Type GPIOx, uint32_t GPIO_Pin)
{
  /* Check the parameters */
    ASSERT(IS_GPIO_ALL_PERIPH(GPIOx));
    ASSERT(GPIO_Pin);
  
    GPIOx->PDDR &= (~GPIO_Pin);
}

/**
  * @brief  Configure the selected pins port data for an output.
  * @param  GPIOx: where x can be (A..E) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bits to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..31).
  * @retval None
  */
void hw_set_gpio_output(GPIO_Type GPIOx, uint32_t GPIO_Pin)
{
  /* Check the parameters */
    ASSERT(IS_GPIO_ALL_PERIPH(GPIOx));
    ASSERT(GPIO_Pin);
  
    GPIOx->PDDR |= GPIO_Pin;
}

/**
  * @brief  Sets the selected data port bits.
  *         Assuming the pin is muxed as a gpio output,set its value.
  * @param  GPIOx: where x can be (A..E) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bits to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..31).
  * @retval None
  */
void hw_gpio_set_bits(GPIO_Type GPIOx, uint32_t GPIO_Pin)
{
  /* Check the parameters */
    ASSERT(IS_GPIO_ALL_PERIPH(GPIOx));
    ASSERT(GPIO_Pin);
  
    GPIOx->PSOR = GPIO_Pin;
}

/**
  * @brief  Clears the selected data port bits.
  * @param  GPIOx: where x can be (A..E) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bits to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..31).
  * @retval None
  */
void hw_gpio_reset_bits(GPIO_Type GPIOx, uint32_t GPIO_Pin)
{
  /* Check the parameters */
    ASSERT(IS_GPIO_ALL_PERIPH(GPIOx));
    ASSERT(GPIO_Pin);
  
    GPIOx->PCOR = GPIO_Pin;
}

/**
  * @brief  Inverse of the selected data port bits' existing logic state. 
  * @param  GPIOx: where x can be (A..E) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bits to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..31).
  * @retval None
  */
void hw_gpio_inverse_bits(GPIO_Type GPIOx, uint32_t GPIO_Pin)
{
  /* Check the parameters */
    ASSERT(IS_GPIO_ALL_PERIPH(GPIOx));
    ASSERT(GPIO_Pin);
  
    GPIOx->PTOR = GPIO_Pin;
}

/**
  * @brief  Reads the specified GPIO input data port.
  * @param  GPIOx: where x can be (A..E) to select the GPIO peripheral.
  * @param  PortVal: specifies the value to be written to the port output data register.
  * @retval None
  */
void hw_gpio_set_value(GPIO_Type GPIOx,uint32_t PortVal)
{
  /* Check the parameters */
    ASSERT(IS_GPIO_ALL_PERIPH(GPIOx));
  
    GPIOx->PDOR = PortVal;
}

/**
  * @brief  Reads the specified GPIO input data port.
  * @param  GPIOx: where x can be (A..E) to select the GPIO peripheral.
  * @retval None
  */
uint32_t hw_gpio_get_value(GPIO_Type GPIOx)
{
  /* Check the parameters */
    ASSERT(IS_GPIO_ALL_PERIPH(GPIOx));
  
    return ((uint32_t)GPIOx->PDIR);
}

/**
  * @brief  Reads the specified GPIO input data port.
  * @param  GPIOx: where x can be (A..E) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bit to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..31).
  * @retval None
  */
uint8_t hw_gpio_get_bit(GPIO_Type GPIOx, uint32_t GPIO_Pin)
{
  /* Check the parameters */
    ASSERT(IS_GPIO_ALL_PERIPH(GPIOx));
    ASSERT(GPIO_Pin);
  
    return (uint8_t)((GPIOx->PDIR & GPIO_Pin) == GPIO_Pin);
}

GPIO_MemMapPtr PTx_To_GPIOx(GPIOPin_Def pin)
{
    if (pin < PTB0) //PTA0~PTA31
    {
         return GPIOA;
    } else if (pin < PTC0) //PTB0~PTB31
    {
        return GPIOB;
    } else if (pin < PTD0) //PTC0~PTC31
    {
        return GPIOC;
    } else if (pin < PTE0) //PTB0~PTB31
    {
        return GPIOD;
    } else if (pin < PTxInvid) //PTB0~PTB31
    {
        return GPIOE;
    } else
    {
        return NULL;
    }
}


int gpio_set_input(GPIOPin_Def pin,uint32_t mode)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = PTx_To_GPIOx(pin);
    if (NULL == gpio_init.GPIOx || ((mode & GPIO_INPUT_MASK) ^ mode) != 0)
    {
        return -1;
    }
    gpio_init.PORT_Pin = (PORTPin_TypeDef)(pin&(32-1));
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|mode;
    gpio_init.GPIO_Pinx_Mode = GPIO_INPUT(gpio_init.PORT_Pin);
    hw_gpio_init(&gpio_init);
    return hw_gpio_get_bit(gpio_init.GPIOx,(1<<gpio_init.PORT_Pin));
}

int gpio_get_bit(GPIOPin_Def pin)
{
    GPIO_MemMapPtr GPIOx;

    GPIOx = PTx_To_GPIOx(pin);
    if (NULL == GPIOx)
    {
        return -1;
    }
    return hw_gpio_get_bit(GPIOx,(1<<(pin&(32-1))));
}

int gpio_set_output(GPIOPin_Def pin,uint32_t mode, uint32_t level)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIOx = PTx_To_GPIOx(pin);
    if (NULL == gpio_init.GPIOx || ((mode & GPIO_OUTPUT_MASK) ^ mode) != 0)
    {
        return -1;
    }
    gpio_init.PORT_Pin = (PORTPin_TypeDef)(pin&(32-1));
    gpio_init.PORT_Mode = PORT_Mode_MUX_gpio|mode;
    gpio_init.GPIO_Pinx_Mode = GPIO_OUTPUT(gpio_init.PORT_Pin);
    hw_gpio_init(&gpio_init);
    if (level)
        hw_gpio_set_bits(gpio_init.GPIOx,(1<<gpio_init.PORT_Pin));
    else
        hw_gpio_reset_bits(gpio_init.GPIOx,(1<<gpio_init.PORT_Pin));
    return 0;
}

int gpio_set_bit(GPIOPin_Def pin,uint32_t level)
{
    GPIO_MemMapPtr GPIOx;

    GPIOx = PTx_To_GPIOx(pin);
    if (NULL == GPIOx)
    {
        return -1;
    }
    if (level)
        hw_gpio_set_bits(GPIOx,(1<<(pin&(32-1))));
    else
        hw_gpio_reset_bits(GPIOx,(1<<(pin&(32-1))));
    return 0;
}


