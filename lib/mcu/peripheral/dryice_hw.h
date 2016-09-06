/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : dryice_hw.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 5/23/2014 6:13:08 PM
 * Description        : 
 *******************************************************************************/

#ifndef _DRYICE_HW_H__
#define _DRYICE_HW_H__

#include "dryice_functions.h"


#define KEYERR_REG_LOCK             (0x80)
#define KEYERR_ACCESS_BLOCK_READ    (0x81)
#define KEYERR_ACCESS_BLOCK_WRITE   (0x82)
#define KEYERR_KEY_LOCK_READ        (0x83)
#define KEYERR_KEY_LOCK_WRITE       (0x84)
#define KEYERR_VALID    (0x83)



//================PGFR=================
#define FILTER_CLK_512_HZ           (0 << DRY_PGFR_GFP_SHIFT) //0 篡改引脚的脉冲滤波通过512Hz预分频器提供时钟。
#define FILTER_CLK_32_KHZ           (1 << DRY_PGFR_GFP_SHIFT) //1 篡改引脚的脉冲滤波通过32.768kHz预分频器提供时钟
#define FILTER_GLITCH_DISABLE       (0 << DRY_PGFR_GFE_SHIFT) //0  篡改引脚脉冲滤波旁路
#define FILTER_GLITCH_ENABLE        (1 << DRY_PGFR_GFE_SHIFT) //1  篡改引脚脉冲滤波使能
//#define FILTER_TAMPER_SAMPLE_CLK_8   (0<<10)
//#define FILTER_TAMPER_SAMPLE_CLK_32  (1<<10)
//#define FILTER_TAMPER_SAMPLE_CLK_128  (2<<10)
//#define FILTER_TAMPER_SAMPLE_CLK_512  (3<<10)
#define FILTER_TAMPER_PIN_LOGIC_0           (0<<DRY_PGFR_TPEX_SHIFT)//00 Tamper pin expected value is logic zero.
#define FILTER_TAMPER_PIN_ACTIVE_0          (1<<DRY_PGFR_TPEX_SHIFT)//01 Tamper pin expected value is active tamper 0 output.
#define FILTER_TAMPER_PIN_ACTIVE_1          (2<<DRY_PGFR_TPEX_SHIFT)//10 Tamper pin expected value is active tamper 1 output
#define FILTER_TAMPER_PIN_ACTIVE_0_XOR_1    (3<<DRY_PGFR_TPEX_SHIFT)//11 Tamper pin 0 expected value is active tamper 0 output XORed with active tamper 1 output

#define FILTER_PullResistor_DISABLE          (0 << DRY_PGFR_TPE_SHIFT) //0 Pull resistor is disabled on tamper pin
#define FILTER_PullResistor_ENABLE           (1 << DRY_PGFR_TPE_SHIFT) //0 Pull resistor is enabled on tamper pin
#define FILTER_GLITCH_FILTER_WITH(x)        DRY_PGFR_GFW(x)  
//================PPR=================
//Tamper Pin Polarity
#define TAMPER_PIN_NOT_INVERTED     0
#define TAMPER_PIN_INVERTED         1
//================PPR=================
#define TAMPER_PIN_SET_INPUT             0
#define TAMPER_PIN_SET_OUTPUT_INVERSE    1 //set to output and drives inverse of expected value.


//Tamper Pins
#define TAMPER_PIN0 0
#define TAMPER_PIN1 1
#define TAMPER_PIN2 2
#define TAMPER_PIN3 3
#define TAMPER_PIN4 4
#define TAMPER_PIN5 5
#define TAMPER_MAX_NUM  6

//Tamper Pin Direction
#define INPUT                  0
#define OUTPUT_INVERSE_EXP_VAL 1

//Active Tamper defines
#define ACTIVE_TAMPER_0 0
#define ACTIVE_TAMPER_1 1
#define ACTIVE_TAMPER_0_1 0xFF

//Internal Tamper Sources
#define DRYICE_TAMPER           0
#define TIME_OVERFLOW           2
#define MONOTONIC_OVERFLOW      3
#define VBAT_TAMPER             4
#define RTCOSC_TAMPER           5
#define TEMPERATURE_TAMPER      6
#define SECURITY_TAMPER         7   //Security Tamper Enable
#define FLASH_SECURITY_DISABLED 8   
#define TEST_MODE_ENTRY         9
#define TAMPER_SOURCES_MAX      10

typedef enum {
    DRY_Time       = (1<<DRY_TER_TOE_SHIFT),  //02 Time Overflow Enable
    DRY_Monotonic  = (1<<DRY_TER_MOE_SHIFT),  //03 Monotonic Overflow Enable
    DRY_Voltage    = (1<<DRY_TER_VTE_SHIFT),  //04 Voltage Tamper Enable:When set, this bit enables the analog voltage tamper detect circuit.
    DRY_Clock      = (1<<DRY_TER_CTE_SHIFT),  //05 Clock Tamper Enable:When set, this bit enables the analog clock tamper detect circuit.
    DRY_Temperature= (1<<DRY_TER_TTE_SHIFT),  //06 Temperature Tamper Enable:When set, this bit enables the analog temperature tamper detect circuit.
    DRY_Security   = (1<<DRY_TER_STE_SHIFT),  //07 Security Tamper Enable
    DRY_Flash      = (1<<DRY_TER_FSE_SHIFT),  //08 Flash Security Enable
    DRY_Test       = (1<<DRY_TER_TME_SHIFT),  //09 Test Mode Enable

    DRY_Tamper0    = (1<<(DRY_TER_TPE_SHIFT+0)),  //16 Tamper Pin 0 Enable
    DRY_Tamper1    = (1<<(DRY_TER_TPE_SHIFT+1)),  //17 Tamper Pin 1 Enable
    DRY_Tamper2    = (1<<(DRY_TER_TPE_SHIFT+2)),  //18 Tamper Pin 2 Enable
}TamperSource;




void dbg_dryice_reg(void);

int hw_dryice_tamper_pin_polarity_config(uint8_t pin_number, uint8_t polarity);
int hw_dryice_tamper_pin_direction_config(uint8_t pin_number, uint8_t direction);

int hw_dryice_read_key(uint8_t offset,uint8_t dw_number,uint32_t *output);
int hw_dryice_write_key(uint8_t offset,uint8_t dw_number,uint32_t *input);
void hw_dryice_init(void);
int hw_dryice_pgfr_cfg(uint8_t tamper_pin,uint32_t pgfr);
int hw_dryice_tamper_cfg(uint8_t pin,uint8_t trig_level,uint32_t glitch_filter);
uint8_t hw_dryice_read_pin(void);
uint8_t hw_dryice_read_trigger(void);

#endif

