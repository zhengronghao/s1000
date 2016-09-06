/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30_cfg.h
 * Author             : luocs
 * Version            : V0.00
 * Date               : 2014.01.02
 * Description        : Hardware pin assignments
 *******************************************************************************/

#ifndef __WP30_CFG_H__
#define __WP30_CFG_H__ 

/* 
 * Include the cpu specific header file 
 */
#include "MK21F12.h"
/* 
 * Include the generic CPU header file 
 */
#include "arm_cm4.h"

/* Global defines to use for TWR-K21D50M */
#define CLK0_TYPE               CRYSTAL

//#define PRO_S1000_V100 
#define PRO_S1000_V200

//========================================================
#define CRYSTAL_MHz             (12)    //Crystal Oscillator
//#define MCU_MK21DZ50
#define MCU_MK21FN120
//========================================================
#if defined(MCU_MK21DZ50)
#define MCK_CLK_MHz             (50)    //K21D:50   K21F:120
#define PLL_CLK_MHz             (2)     //K21D:2    K21F:4
#else
#define MCK_CLK_MHz             (120)   //K21D:50   K21F:120
#define PLL_CLK_MHz             (4)     //K21D:2    K21F:4
#endif
/*
 * The expected PLL output frequency is:
 * PLL out = (((CLKIN/PRDIV) x VDIV))
 * where the CLKIN can be either CLK0_FREQ_HZ or CLK1_FREQ_HZ.
 * For more info on PLL initialization refer to the mcg driver files.
 */
/**********************************************************-|K21D_USB|-K21D-|K21F_USB|-K21F-|-*/
#define CLK0_FREQ_HZ        (1000*1000*CRYSTAL_MHz)//晶振  -|-------------8MHz--------------|   
#define MCK_CLK	            (1000*1000*MCK_CLK_MHz)//主时钟-|--48MHz-|50MHz-|-120MHz-|120MHz|
#define PLLClkPrdivHz       (1000*1000*PLL_CLK_MHz)//PLLClk-|---2MHz-|-2MHz-|---4MHz-|--4MHz|
#define PLL0_VDIV           (MCK_CLK/PLLClkPrdivHz)      //-|---24---|--25--|---30---|--30--|
#define PLL0_PRDIV          (CLK0_FREQ_HZ/PLLClkPrdivHz) //-|----2---|---2--|----2---|---2--|

#define CLK_VLPR_DIV1       (CLK0_FREQ_HZ/2000000-1)
#define CLK_VLPR_DIV2       (CLK0_FREQ_HZ/2000000-1)
#define CLK_VLPR_DIV4       (CLK0_FREQ_HZ/1000000-1)
/*
 * System Bus Clock Info
 */
#define REF_CLK             XTAL8
#define CORE_CLK_MHZ        PLL50      /* system/core frequency in MHz */


/*-------------------------------UART----------------------------------*/
#define TXRTSE  1
#define RXRTSE  0
//uart0---name-------pin----//alt2  | alt3  | alt3  | alt3
#define UART0_RXD   PTA1    //PTA1  | PTA15 | PTB16 | PTD6  
#define UART0_TXD   PTA2    //PTA2  | PTA14 | PTB17 | PTD7
#define UART0_CTS   PTxInvid//PTA0  | PTA16 | PTB3  | PTD5
#define UART0_RTS   PTxInvid//PTA3  | PTA17 | PTB2  | PTD4
#define UART0_TXRTS_ACTIVE_LEVEL  0        //0-low 1-hight 
#define UART0_TXRTS_RXRTS_ENABLE  RXRTSE   //1-TXRTSE 0-RXRTSE

//uart1---name-------pin----//alt3  | alt3  | altx  | altx
#define UART1_RXD   PTC3    //PTE1  | PTC3  | ----- | ----  
#define UART1_TXD   PTC4    //PTE0  | PTC4  | ----- | ----
#define UART1_CTS   PTxInvid//PTE2  | PTC2  | ----- | ---- 
#define UART1_RTS   PTxInvid//PTE3  | PTC1  | ----- | ---- 
#define UART1_TXRTS_ACTIVE_LEVEL  0         //0-low 1-hight
#define UART1_TXRTS_RXRTS_ENABLE  RXRTSE   //1-TXRTSE 0-RXRTSE

//uart2---name-------pin----//alt3  | alt3  | altx  | altx
#define UART2_RXD   PTE17   //PTE17 | PTD2  | ----- | ----  
#define UART2_TXD   PTE16   //PTE16 | PTD3  | ----- | ----
#define UART2_CTS   PTxInvid//PTE18 | PTD1  | ----- | ---- 
#define UART2_RTS   PTxInvid//PTE19 | PTD0  | ----- | ---- 
#define UART2_TXRTS_ACTIVE_LEVEL  0        //0-low 1-hight
#define UART2_TXRTS_RXRTS_ENABLE  RXRTSE   //1-TXRTSE 0-RXRTSE

//uart3---name-------pin----//alt3  | alt3  | alt3  | altx
#define UART3_RXD   PTC16   //PTE5  | PTB10 | PTC16 | ---- 
#define UART3_TXD   PTC17   //PTE4  | PTB11 | PTC17 | ---- 
#define UART3_CTS   PTxInvid//PTB13 | ----- | ----- | ---- 
#define UART3_RTS   PTxInvid//PTB12 | ----- | ----- | ---- 
#define UART3_TXRTS_ACTIVE_LEVEL  0        //0-low 1-hight
#define UART3_TXRTS_RXRTS_ENABLE  RXRTSE   //1-TXRTSE 0-RXRTSE

//uart4---name-------pin----//alt3  | alt3  | alt3  | altx
#define UART4_RXD   PTC14   //PTC14 | ---- 
#define UART4_TXD   PTC15   //PTC15 | ---- 
#define UART4_CTS   PTxInvid   //PTC13 | ----- | ----- | ---- 
#define UART4_RTS   PTxInvid   //PTC12 | ----- | ----- | ---- 
#define UART4_DMA_CHANNEL  HW_DMA_CH1
#define UART4_TXRTS_ACTIVE_LEVEL  0        //0-low 1-hight
#define UART4_TXRTS_RXRTS_ENABLE  RXRTSE   //1-TXRTSE 0-RXRTSE

#define UART_TOTOL_NUM      5

/* Serial Port Info */
//console  TOWER:UART2 PTE17 PTE16
#define CNL_COMPORT     UART2        
//#define CNL_COMPORT     USBD_CDC        
#define CNL_BAUD        115200
//download port TOWER:UART2 PTE17 PTE16
#define DL_COMUSBD      USBD_CDC   
#define DL_COMPORT      UART2  
#define DL_BAUD         115200 
//work port
#define WORK_COMPORT    UART2
#define WORK_BAUD       115200 
//Bluetooth port
#define BT_COMPORT      UART2
#define BT_BAUD         115200

#define BT_COMPORT      UART2
#define BT_BAUD         115200

#define  CTC_COMPORT    UART4
#define  CTC_BAUD       1500000

//scan port
#define SCAN_COMPORT    UART1
#define SCAN_BAUD       9600 


/*-------------------------------led----------------------------------*/
#define LED_BLUE_GPIO       GPIOC 
#define LED_BLUE_PINx       PORT_Pin_16 
#define LED_PTxy_BLUE       PTC16

#define LED_YELLOW_GPIO     GPIOC  
#define LED_YELLOW_PINx     PORT_Pin_17 
#define LED_PTxy_YELLOW     PTC17

#define LED_GREEN_GPIO      GPIOC  
#define LED_GREEN_PINx      PORT_Pin_18 
#define LED_PTxy_GREEN      PTC18

#define LED_RED_GPIO        GPIOC  
#define LED_RED_PINx        PORT_Pin_19
#define LED_PTxy_RED        PTC19

#define S_LED_BLUE    (1<<3)
#define S_LED_YELLOW  (1<<2)
#define S_LED_GREEN   (1<<1)
#define S_LED_RED     (1<<0)
#define LED_ALL     0x0F
/*----------------------------keyboard----------------------------------*/
//keyboard col
#define KB_GPIO_COL    GPIOC 
#define KB_PORT_COL    PORTC_BASE_PTR 
#define KB_GPIO_COL0    GPIOC 
#define KB_PORT_COL0    PORTC_BASE_PTR 
#define KB_PINx_COL0    PORT_Pin_7 //not used
#define KB_GPIO_COL1    GPIOC 
#define KB_PORT_COL1    PORTC_BASE_PTR 
#define KB_PINx_COL1    PORT_Pin_8
#define KB_GPIO_COL2    GPIOC 
#define KB_PORT_COL2    PORTC_BASE_PTR 
#define KB_PINx_COL2    PORT_Pin_9
#define KB_GPIO_COL3    GPIOC 
#define KB_PORT_COL3    PORTC_BASE_PTR 
#define KB_PINx_COL3    PORT_Pin_10
#define KB_GPIO_COL4    GPIOC 
#define KB_PORT_COL4    PORTC_BASE_PTR 
#define KB_PINx_COL4    PORT_Pin_11
//keyboard row
#define KB_GPIO_ROW    GPIOB 
#define KB_PORT_ROW    PORTB_BASE_PTR 
#define KB_GPIO_ROW0    GPIOB 
#define KB_PINx_ROW0    PORT_Pin_0
#define KB_GPIO_ROW1    GPIOB 
#define KB_PINx_ROW1    PORT_Pin_1
#define KB_GPIO_ROW2    GPIOB 
#define KB_PINx_ROW2    PORT_Pin_2
#define KB_GPIO_ROW3    GPIOB 
#define KB_PINx_ROW3    PORT_Pin_3
// independent key
#define KB_GPIO_INDEPENDENT0    GPIOB 
#define KB_PORT_independent0    PORTB_BASE_PTR 
#define KB_PINx_INDEPENDENT0    PORT_Pin_11
#define KB_PTx_INDEPENDENT0     PTB11

#define KB_NOTMAXTRIX
//4 col + 1 independent0
#define KB_COL_MAXTRIX_MASK  (uint8_t)0x0F
#define KB_COL_MAXTRIX_MAX   4

/*----------------------------Lcd----------------------------------*/
//#define LCD_SOFT_RESET
#define LCD_SPIn              SPI0  
//PCS:无效
#define LCD_GPIO_PCS    
#define LCD_PINx_PCS    
#define LCD_PTxy_PCSn     PTA14 
#define LCD_SPI_PCSn      SPI_PCS_0  
#define LCD_PCS_PIN       (1<<LCD_SPI_PCSn) 
//MOSI
#define LCD_GPIO_MOSI    GPIOA
//#define LCD_PINx_MOSI    PORT_Pin_17
//#define LCD_PTxy_MOSI    PTA17
#define LCD_PINx_MOSI    PORT_Pin_16
#define LCD_PTxy_MOSI    PTA16
//MISO
#define LCD_GPIO_SCLK    GPIOA
#define LCD_PINx_SCLK    PORT_Pin_15
#define LCD_PTxy_SCLK    PTA15
//RS
#define LCD_GPIO_RS      GPIOE
#define LCD_PINx_RS      PORT_Pin_5
//RESET
#if defined(LCD_SOFT_RESET)
#define LCD_GPIO_RESET   GPIOC  
#define LCD_PINx_RESET   PORT_Pin_6
#endif
//BL
#define LCD_GPIO_BACKLIGHT   GPIOA
#define LCD_PINx_BACKLIGHT   PORT_Pin_13

#define LCD_BACKLIGHT_ON     1
#define LCD_BACKLIGHT_OFF    0
#define LCD_BACKLIGHT_TIMERCNT    (uint16_t)(4*1000)//6s 

/*-------------------------------SPI Flash--------------------------------*/
//#define ExFLASH_SOFT_RESET
#define ExFLASH_SIMULATE_PCS

#define ExFLASH_SPIn     SPI1 

#define EXFLASH_GPIO_MOSI    GPIOE
#define EXFLASH_PINx_MOSI    PORT_Pin_1
#define EXFLASH_PTxy_MOSI    PTE1

#define EXFLASH_GPIO_MISO    GPIOE
#define EXFLASH_PINx_MISO    PORT_Pin_3
#define EXFLASH_PTxy_MISO    PTE3

#define EXFLASH_GPIO_SCLK    GPIOE
#define EXFLASH_PINx_SCLK    PORT_Pin_2
#define EXFLASH_PTxy_SCLK    PTE2

#define EXFLASH_GPIO_CS      GPIOE
#define EXFLASH_PINx_CS      PORT_Pin_4
#define EXFLASH_PTxy_CS      PTE4 
#define EXFLASH_SPI_PCSn     SPI_PCS_0 
#define ExFLASH_CSPIN        (1<<EXFLASH_SPI_PCSn) 

//#define EXFLASH_GPIO_CS      GPIOE
//#define EXFLASH_PINx_CS      PORT_Pin_0
//#define EXFLASH_PTxy_CS      PTE0 
//#define EXFLASH_SPI_PCSn     SPI_PCS_1 
//#define ExFLASH_CSPIN        (1<<EXFLASH_SPI_PCSn) 

#if defined(ExFLASH_SOFT_RESET) 
#define EXFLASH_GPIO_RESET   GPIOE
#define EXFLASH_PINx_RESET   PORT_Pin_16
#endif
/*-------------------------------Power module--------------------------------*/
#define POWER_KEEP_GPIO           GPIOD
#define POWER_KEEP_PINx           PORT_Pin_15 

#define POWER_RESET_GPIO           GPIOD
#define POWER_RESET_PINx           PORT_Pin_11 
#define POWER_RESET_PTx            PTD11 


#define POWER_DOWN_CHECK_GPIO           GPIOE
#define POWER_DOWN_CHECK_PINx           PORT_Pin_18 
#define POWER_DOWN_CHECK_PTx            PTE18  //S1000 开机后该引脚为低 关机后该引脚拉高

#define POWER_WAKE_UP_GPIO           GPIOD
#define POWER_WAKE_UP_PINx           PORT_Pin_0 
#define POWER_WAKE_UP_PTx           PTD0   //S1000 开机后该引脚为高，开机键被按后 改引脚拉低 
#define POWER_WAKE_UP_PORT          PORTD_BASE_PTR 
#define POWER_WAKE_UP_IRQn          PORTD_IRQn

#define POWER_CHARGE_FULL_GPIO    GPIOC
#define POWER_CHARGE_FULL_PORT    PORTC_BASE_PTR 
#define POWER_CHARGE_FULL_PINx    PORT_Pin_4 

#define POWER_CHARGE_ADC_GPIO     GPIOD
#define POWER_CHARGE_ADC_PINx     PORT_Pin_3 
#define POWER_CHARGE_ADC_PTx      PTD3
#define POWER_CHARGE_ADC_CTRLR    ADC_CFG2_Mux_ADxxa 
#define POWER_CHARGE_ADC_CHANEL   ADC_SC1n_ChnSelt_SEDP3_DiffDAD3 
#define SYS_VERSION_ADC_CTRLR     ADC_CFG2_Mux_ADxxa 
#define SYS_VERSION_ADC_CHANEL    ADC_SC1n_ChnSelt_SEDP0_DiffDAD0 

#ifdef PRO_S1000_V100
#define POWER_SUPPLY_5V_PTx     PTA1
#define POWER_SUPPLY_3V3_PTx     PTA2
#else 
#define POWER_SUPPLY_5V_PTx     PTB6
#define POWER_SUPPLY_3V3_PTx    PTB16 
#endif

#define POWER_CHARGE_GPIO      GPIOB
#define POWER_CHARGE_PORT      PORTB_BASE_PTR 
#define POWER_CHARGE_PINx      PORT_Pin_17 

#define POWER_BATTER_RUN_TIME  (uint32_t)((3*60*1000)) //3 minutes
/*-------------------------------Beep module--------------------------------*/
//FTM1_CH0
#ifdef  PRO_S1000_V100 
#define BEEP_GPIO         GPIOD
#define BEEP_PINx         PORT_Pin_7 
#define BEEP_PTxy         PTD7 
#define BEEP_PWM_TIMER    FTM0
#define BEEP_PWM_CHANNEL  FTM_Ch7
#else
#define BEEP_GPIO         GPIOB
#define BEEP_PINx         PORT_Pin_13 
#define BEEP_PTxy         PTB13 
#define BEEP_PWM_TIMER    FTM0
#define BEEP_PWM_CHANNEL  FTM_Ch5
#endif
#define BEEP_PWM_HZ         2700 
#define BEEP_PWM_4KHZ       4000  // MK-210V3.01主板采用4K蜂鸣器
#define BEEP_PWM_TIMERCNT_KB       (uint16_t)(40/10) //40ms 
#define BEEP_PWM_TIMERCNT_POWER    (uint16_t)(200/10)//200ms 

/*-------------------------------BlueTooth module--------------------------------*/
//Battery Input enable control
#define BT_BATIN_ENABLE_GPIO      GPIOC            //BT_PWREN
#define BT_BATIN_ENABLE_PORT      PORTC_BASE_PTR 
#define BT_BATIN_ENABLE_PINx      PORT_Pin_12 
#define BT_BATIN_ENABLE_PTxy      PTC12 
//WAKEUP output enable control
#define BT_WAKEUP_ENABLE_GPIO      GPIOB            //
#define BT_WAKEUP_ENABLE_PORT      PORTB_BASE_PTR 
#define BT_WAKEUP_ENABLE_PINx      PORT_Pin_10 
#define BT_WAKEUP_ENABLE_PTxy      PTB10 
//Status Indicator 2 (STATUS_IND_2). See Section 1.5
#define BT_STATUS_IND_2_GPIO      GPIOA            //P04  BT_SLEEP
#define BT_STATUS_IND_2_PORT      PORTA_BASE_PTR 
#define BT_STATUS_IND_2_PINx      PORT_Pin_17 
#define BT_STATUS_IND_2_PTxy      PTA17 
//Status Indicator 1 (STATUS_IND_1). See Section 1.5
#define BT_STATUS_IND_1_GPIO      GPIOC            //P15  BT_STATUS
#define BT_STATUS_IND_1_PORT      PORTC_BASE_PTR 
#define BT_STATUS_IND_1_PINx      PORT_Pin_6 
#define BT_STATUS_IND_1_PTxy      PTC6 
//System configuration (internal pull‐down) (See Section 1.10) 
#define BT_SYSCONFIG_P20_GPIO      GPIOB            //BT_CONFIG
#define BT_SYSCONFIG_P20_PORT      PORTB_BASE_PTR 
#define BT_SYSCONFIG_P20_PINx      PORT_Pin_16 
#define BT_SYSCONFIG_P20_PTxy      PTB16
//System configuration (internal pull‐down) (See Section 1.10) 
#define BT_SYSCONFIG_P24_GPIO      GPIOC            //BT_BOOT/RTS
#define BT_SYSCONFIG_P24_PORT      PORTC_BASE_PTR 
#define BT_SYSCONFIG_P24_PINx      PORT_Pin_7 
#define BT_SYSCONFIG_P24_PTxy      PTC7
//System configuration (internal pull‐down) (See Section 1.10) 
#define BT_SYSCONFIG_EAN_GPIO      GPIOC            //BT_EAN
#define BT_SYSCONFIG_EAN_PORT      PORTC_BASE_PTR 
#define BT_SYSCONFIG_EAN_PINx      PORT_Pin_13 
#define BT_SYSCONFIG_EAN_PTxy      PTC13
//Module reset (active low) (internal pull‐up) 
//Apply a pulse of at least 63 ns. 
#define BT_MODULE_RST_N_GPIO      GPIOC            //BT_RST
#define BT_MODULE_RST_N_PORT      PORTC_BASE_PTR 
#define BT_MODULE_RST_N_PINx      PORT_Pin_1 
#define BT_MODULE_RST_N_PTxy      PTC1
//UART data input:PIN20
#define BT_HCI_RXD_UART_TX_GPIO      GPIOE            //UART2_TX
#define BT_HCI_RXD_UART_TX_PORT      PORTE_BASE_PTR 
#define BT_HCI_RXD_UART_TX_PINx      PORT_Pin_17 
#define BT_HCI_RXD_UART_TX_PTxy      PTE17
//UART data output:PIN21
#define BT_HCI_TXD_UART_RX_GPIO      GPIOE            //UART2_RX
#define BT_HCI_TXD_UART_RX_PORT      PORTE_BASE_PTR 
#define BT_HCI_TXD_UART_RX_PINx      PORT_Pin_17 
#define BT_HCI_TXD_UART_RX_PTxy      PTE17
//Configurable Control or Indication pin  
//(when configured as input: internal pull‐up)
#define BT_CONFIG_CONT_IND_P32_GPIO      GPIOB            //BT_INT
#define BT_CONFIG_CONT_IND_P32_PORT      PORTB_BASE_PTR 
#define BT_CONFIG_CONT_IND_P32_PINx      PORT_Pin_10 
#define BT_CONFIG_CONT_IND_P32_PTxy      PTB10

#define BT_CONFIG_CONT_IND_P33_GPIO      GPIOC            //BT_SLEEPON
#define BT_CONFIG_CONT_IND_P33_PORT      PORTC_BASE_PTR 
#define BT_CONFIG_CONT_IND_P33_PINx      PORT_Pin_7 
#define BT_CONFIG_CONT_IND_P33_PTxy      PTC7

#define BT_CONFIG_CONT_IND_P34_GPIO      GPIOE            //BT_STANDBY/CTS
#define BT_CONFIG_CONT_IND_P34_PORT      PORTE_BASE_PTR 
#define BT_CONFIG_CONT_IND_P34_PINx      PORT_Pin_18
#define BT_CONFIG_CONT_IND_P34_PTxy      PTE18
/*-------------------------------magcard----------------------------------*/
#define MAG_GPIO        GPIOC 
#define MAG_DATA        PORT_Pin_1  
#define MAG_STROBE      PORT_Pin_0  
#define MAG_POWER       PORT_Pin_Invalid 
/*-------------------------------iccard----------------------------------*/
#define IC_C8_PORT          NULL
#define IC_C8_GPIO          NULL
#define IC_C8_PINx          PORT_Pin_3
#define IC_C4_PORT          NULL 
#define IC_C4_GPIO          NULL 
#define IC_C4_PINx          PORT_Pin_4
#define IC_CLK_PORT         PORTA_BASE_PTR 
#define IC_CLK_GPIO         GPIOA
#define IC_CLK_PINx         PORT_Pin_5
#define IC_CLK_PTx          PTA5
#define IC_SEL1_PORT        NULL 
#define IC_SEL1_GPIO        GPIOA//NULL 
#define IC_SEL1_PINx        PORT_Pin_16
#define IC_SEL1_PTx        PTA16
#define IC_SEL2_PORT        NULL 
#define IC_SEL2_GPIO        GPIOA//NULL 
#define IC_SEL2_PINx        PORT_Pin_15//PORT_Pin_1
#define IC_SEL2_PTx        PTA15//PORT_Pin_1
#define IC_SEL3_PORT        NULL 
#define IC_SEL3_GPIO        GPIOA//NULL 
#define IC_SEL3_PINx        PORT_Pin_13//PORT_Pin_2
#define IC_SEL3_PTx         PTA13//PORT_Pin_2
#define IC_IO_PORT          PORTA_BASE_PTR 
#define IC_IO_GPIO          GPIOA
#define IC_IO_PINx          PORT_Pin_14
#define IC_IO_IRQn          PORTA_IRQn 
#define IC_IO_PTx           PTA14
#define IC_RST_PORT         PORTA_BASE_PTR 
#define IC_RST_GPIO         GPIOA 
#define IC_RST_PINx         PORT_Pin_17
#define IC_RST_PTx          PTA17
#define IC_INT_PORT         PORTA_BASE_PTR 
#define IC_INT_GPIO         GPIOA 
#define IC_INT_PINx         PORT_Pin_12
#define IC_INT_IRQn         PORTA_IRQn 
#define IC_INT_PTx          PTA12
#define IC_PWR_PORT         PORTC_BASE_PTR  //新增
#define IC_PWR_GPIO         GPIOC
#define IC_PWR_PINx         PORT_Pin_0
#define IC_PWR_PTx          PTC0
#define IC_CS_PORT          NULL
#define IC_CS_GPIO          GPIOC 
#define IC_CS_PINx          PORT_Pin_7
#define IC_CS_PTx           PTC7

#define SAM_INIT_PORT       NULL 
#define SAM_INIT_GPIO       NULL 
#define SAM_INIT_PINx       PORT_Pin_0
#define SAM_CLK_PORT        PORTD_BASE_PTR 
#define SAM_CLK_GPIO        GPIOD
#define SAM_CLK_PINx        PORT_Pin_1
#define SAM_CLK_PTx         PTD1
#define SAM_CS1_PORT        NULL 
#define SAM_CS1_GPIO        NULL 
#define SAM_CS1_PINx        PORT_Pin_12
#define SAM_CS2_PORT        NULL 
#define SAM_CS2_GPIO        NULL 
#define SAM_CS2_PINx        PORT_Pin_13
#define SAM1_PWR_PORT       PORTD_BASE_PTR 
#define SAM1_PWR_GPIO       GPIOD
#define SAM1_PWR_PINx       PORT_Pin_3
#define SAM1_PWR_PTx        PTD3
#define SAM2_PWR_PORT       PORTD_BASE_PTR 
#define SAM2_PWR_GPIO       GPIOD
#define SAM2_PWR_PINx       PORT_Pin_6
#define SAM2_PWR_PTx        PTD6
#define SAM1_RST_PORT       PORTD_BASE_PTR 
#define SAM1_RST_GPIO       GPIOD
#define SAM1_RST_PINx       PORT_Pin_2 
#define SAM1_RST_PTx        PTD2
#define SAM2_RST_PORT       PORTD_BASE_PTR 
#define SAM2_RST_GPIO       GPIOD
#define SAM2_RST_PINx       PORT_Pin_5
#define SAM2_RST_PTx        PTD5
#define SAM1_IO_PORT        PORTD_BASE_PTR 
#define SAM1_IO_GPIO        GPIOD
#define SAM1_IO_PINx        PORT_Pin_4 
#define SAM1_IO_PTx         PTD4
#define SAM2_IO_PORT        PORTD_BASE_PTR 
#define SAM2_IO_GPIO        GPIOD
#define SAM2_IO_PINx        PORT_Pin_8 
#define SAM2_IO_PTx         PTD8

/*------------------------------RFID--------------------------------*/
#define RFID_GPIO_MOSI    GPIOE
#define RFID_PINx_MOSI    PORT_Pin_1
#define RFID_PTxy_MOSI    PTE1

#define RFID_GPIO_MISO    GPIOE
#define RFID_PINx_MISO    PORT_Pin_3
#define RFID_PTxy_MISO    PTE3

#define RFID_GPIO_SCLK    GPIOE
#define RFID_PINx_SCLK    PORT_Pin_2
#define RFID_PTxy_SCLK    PTE2

//#define RFID_GPIO_CS      GPIOE
//#define RFID_PINx_CS      PORT_Pin_0
//#define RFID_PTxy_CS      PTE0 
#define RFID_SPIn          SPI1
#define RFID_PTxy_PCSn     PTE5 
#define RFID_SPI_PCSn      SPI_PCS_2  
#define RFID_PCS_PIN       (1<<RFID_SPI_PCSn) 

#define RFID_IO_PORT          PORTE_BASE_PTR 
#define RFID_IO_GPIO          GPIOE
#define RFID_IO_PINx          PORT_Pin_0
#define RFID_PTxy_IO          PTE0
#define RFID_IO_IRQn          PORTE_IRQn 

#define RFID_RST_GPIO          GPIOE
#define RFID_RST_PINx          PORT_Pin_6
#define RFID_PTxy_RST          PTE6

/*------------------------------tprinter--------------------------------*/
#define TPRINT_SPIn        SPI2 
//没有片选 无效
#define TPRINT_SPI_PCSn    SPI_PCS_0  
#define TPRINT_PCS_PIN     (1<<TPRINT_SPI_PCSn) 

#define TPRINT_GPIO_MOSI    GPIOD
#define TPRINT_PINx_MOSI    PORT_Pin_13
#define TPRINT_PTxy_MOSI    PTD13

//#define TPRINT_GPIO_MISO    GPIOD
//#define TPRINT_PINx_MISO    PORT_Pin_14
//#define TPRINT_PTxy_MISO    PTD14

#define TPRINT_GPIO_SCLK    GPIOD
#define TPRINT_PINx_SCLK    PORT_Pin_12
#define TPRINT_PTxy_SCLK    PTD12

#define TP_MOTPWR_GPIO      GPIOB 
#define TP_MOTPWR_PINx      PORT_Pin_1 
#define TP_MOTPWR_PTxy      PTB1         /*   MOTO-PWR    */

#define TP_LATCH_GPIO      GPIOB 
#define TP_LATCH_PINx      PORT_Pin_17 
#define TP_LATCH_PTxy      PTB17         /*   LATCH    */

#define TP_WORK_GPIO      GPIOB 
#define TP_WORK_PINx      PORT_Pin_12 
#define TP_WORK_PTxy      PTB12         /*  PRN-POWER */

#define TP_MnA_GPIO        GPIOB 
#define TP_MnA_PINx        PORT_Pin_8 
#define TP_MnA_PTxy        PTB8         /*   MOT-PHASE-1A   */

#define TP_MA_GPIO         GPIOB 
#define TP_MA_PINx         PORT_Pin_9 
#define TP_MA_PTxy         PTB9         /*    MOT-PHASE-1B  */

#define TP_MnB_GPIO        GPIOB 
#define TP_MnB_PINx        PORT_Pin_10 
#define TP_MnB_PTxy        PTB10         /*   MOT-PHASE-2A   */

#define TP_MB_GPIO         GPIOB 
#define TP_MB_PINx         PORT_Pin_11 
#define TP_MB_PTxy         PTB11         /*    MOT-PHASE-2B  */

#define TP_STB_GPIO         GPIOB 
#define TP_STB_PINx         PORT_Pin_7 
#define TP_STB_PTxy         PTB7         /*    STB1      */


/*----------------------scan--------------------------*/
#define SCAN_GPIO_EN          GPIOB 
#define SCAN_PINx_EN          PORT_Pin_2 
#define SCAN_PTxy_EN          PTB2         /*    MOT-PHASE-2B  */

#define SCAN_GPIO_TRIG        GPIOD 
#define SCAN_PINx_TRIG        PORT_Pin_9 
#define SCAN_PTxy_TRIG        PTD9



/*----------------------securitychip IS8U256A--------------------------*/
#define IS8U256A_GPIO_MOSI    GPIOB
#define IS8U256A_PINx_MOSI    PORT_Pin_22
#define IS8U256A_PTxy_MOSI    PTB22

#define IS8U256A_GPIO_MISO    GPIOB 
#define IS8U256A_PINx_MISO    PORT_Pin_23
#define IS8U256A_PTxy_MISO    PTB23

#define IS8U256A_GPIO_SCLK    GPIOB 
#define IS8U256A_PINx_SCLK    PORT_Pin_21
#define IS8U256A_PTxy_SCLK    PTB21

#define IS8U256A_SPIn          SPI2
#define IS8U256A_PTxy_PCSn     PTB20
#define IS8U256A_SPI_PCSn      SPI_PCS_0  
#define IS8U256A_PCS_PIN       (1<<IS8U256A_SPI_PCSn) 

#define IS8U256A_GPIO_RST      GPIOB 
#define IS8U256A_PINx_RST      PORT_Pin_18
#define IS8U256A_PTxy_RST      PTB18

#define IS8U256A_GPIO_SHAKEHAND      GPIOB 
#define IS8U256A_PINx_SHAKEHAND      PORT_Pin_19
#define IS8U256A_PTxy_SHAKEHAND      PTB19

#define MINI_USB_PTxy_COM_EN         PTD7

/*-------------------------------xx xxx--------------------------------*/


/*-------------------------------xx xxx--------------------------------*/
#define DL_CFG_VERSION          "1.0"
#define DL_CFG_COMPRESS         "0"
#define DL_CFG_ENCRYPT          "0"
#define DL_CFG_CHECKSUM         "0"
#define DL_CFG_AUTH             "0"
#define DL_CFG_PACKLEN          (4*1024)//FLASH_SECTOR_SIZE 
#define DL_CFG_MAX_BAUD         460800
#define DL_CFG_VENDOR           "START"
#define DL_CFG_MODULE           "S1000"
#define S1000_BOOT_VER          "V1.0.1.1"
#define S1000_CTRL_VER          "V1.0.1.5"
#define MK210_APP_VER           "3.0.1.0"
/*-----------------------------------------------------------------------------}
 *  版本信息
 *-----------------------------------------------------------------------------{*/
typedef struct _VERSION_INFO_NEW_
{
	char product[20];				
	char version[16];  // "255.255.255.255"			
	char time[12];					
} VERSION_INFO_NEW;
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define UPAREA   @ ".uparea"    //定义在片内
#define DOWNAREA   @ ".downarea"    //定义在片外
/*
 * Debug prints ON (#define) or OFF (#undef)
 */
//#define DEBUG_Dx

#define TERMNAME       "WP-30" 
/*------------------------------------------------------------------------*/
/*------------------driver header file------------------------------------*/
/*------------------------------------------------------------------------*/
#include "adc_hw.h"
#include "dryice_functions.h"
#include "dryice_hw.h"
#include "ftm_hw.h"
#include "pit_hw.h"
#include "rnga_hw.h"
#include "rtc_hw.h"
#include "spi_hw.h"
#include "clib.h"

#ifdef WP30_V1_BOOT
#include "wp30_boot_cfg.h"
#include "wp30_boot_isr.h"
#else
#include "wp30_ctrl_cfg.h"
#include "wp30_ctrl_isr.h"
#include "./debug/wp30_ctrl_dbg.h"
#endif

#endif

