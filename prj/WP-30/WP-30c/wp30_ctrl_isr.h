/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
* File Name          : wp30_ctrl_isr.h
* Author             : luocs
* Version            : V0.00
* Date               : 2014.01.02
* Description        : 
*******************************************************************************/

#ifndef __WP30_CTRL_ISR_H__
#define __WP30_CTRL_ISR_H__ 

    /*core interrupt*/
//#undef VECTOR_002
//#undef VECTOR_003
//#undef VECTOR_004
//#undef VECTOR_005
//#undef VECTOR_006
//#undef VECTOR_007
//#undef VECTOR_008
//#undef VECTOR_009
//#undef VECTOR_010
//#undef VECTOR_011
//#undef VECTOR_012
//#undef VECTOR_013
//#undef VECTOR_014
#undef VECTOR_015
    /*Peripheral interrupt*/
//#undef VECTOR_016
//#undef VECTOR_017
//#undef VECTOR_018
//#undef VECTOR_019
//#undef VECTOR_020
//#undef VECTOR_021
//#undef VECTOR_022
//#undef VECTOR_023
//#undef VECTOR_024
//#undef VECTOR_025
//#undef VECTOR_026
//#undef VECTOR_027
//#undef VECTOR_028
//#undef VECTOR_029
//#undef VECTOR_030
//#undef VECTOR_031
//#undef VECTOR_032
//#undef VECTOR_033
//#undef VECTOR_034
//#undef VECTOR_035
//#undef VECTOR_036
#undef VECTOR_037
//#undef VECTOR_038
//#undef VECTOR_039
//#undef VECTOR_040
//#undef VECTOR_041
//#undef VECTOR_042
//#undef VECTOR_043
//#undef VECTOR_044
//#undef VECTOR_045
//#undef VECTOR_046
#undef VECTOR_047
//#undef VECTOR_048
#undef VECTOR_049
//#undef VECTOR_050
#undef VECTOR_051
//#undef VECTOR_052
#undef VECTOR_053
//#undef VECTOR_054
//#undef VECTOR_055
//#undef VECTOR_056
//#undef VECTOR_057
//#undef VECTOR_058
//#undef VECTOR_059
//#undef VECTOR_060
//#undef VECTOR_061
#undef VECTOR_062
#undef VECTOR_063
#undef VECTOR_064
#undef VECTOR_065
#undef VECTOR_066
#undef VECTOR_067
//#undef VECTOR_068
#undef VECTOR_069
//#undef VECTOR_070
#undef VECTOR_071
//#undef VECTOR_072
//#undef VECTOR_073
//#undef VECTOR_074
#undef VECTOR_075
#undef VECTOR_076
#undef VECTOR_077
#undef VECTOR_078
#undef VECTOR_079
//#undef VECTOR_080
//#undef VECTOR_081
#undef VECTOR_082
#undef VECTOR_083
//#undef VECTOR_084
//#undef VECTOR_085
//#undef VECTOR_086
//#undef VECTOR_087
//#undef VECTOR_088
//#undef VECTOR_089
//#undef VECTOR_090
//#undef VECTOR_091
//#undef VECTOR_092
//#undef VECTOR_093
//#undef VECTOR_094
//#undef VECTOR_095
//#undef VECTOR_096
//#undef VECTOR_097
//#undef VECTOR_098
//#undef VECTOR_099
//#undef VECTOR_100
//#undef VECTOR_101
//#undef VECTOR_102
//#undef VECTOR_103
//#undef VECTOR_104
//#undef VECTOR_105
//#undef VECTOR_106
//#undef VECTOR_107


    /*core interrupt*/
//#define VECTOR_002      NMI_Handler            // 0x0000_0008 2 -          ARM core        Non-maskable Interrupt (NMI)                           
//#define VECTOR_003      HardFault_Handler      // 0x0000_000C 3 -          ARM core        Hard Fault                                             
//#define VECTOR_004      MemManage_Handler      // 0x0000_0010 4 -          ARM core        MemManage Fault                                        
//#define VECTOR_005      BusFault_Handler       // 0x0000_0014 5 -          ARM core         Bus Fault                                             
//#define VECTOR_006      UsageFault_Handler     // 0x0000_0018 6 -          ARM core         Usage Fault                                           
//#define VECTOR_007      0                      // 0x0000_001C 7 -                                                                                 
//#define VECTOR_008      0                      // 0x0000_0020 8 -                                                                                 
//#define VECTOR_009      0                      // 0x0000_0024 9 -                                                                                 
//#define VECTOR_010      0                      // 0x0000_0028 10 -                                                                                
//#define VECTOR_011      SVC_Handler            // 0x0000_002C 11 -         ARM core         Supervisor call (SVCall)                              
//#define VECTOR_012      DebugMon_Handler       // 0x0000_0030 12 -         ARM core         Debug Monitor                                         
//#define VECTOR_013      0                      // 0x0000_0034 13 -                                                                                
//#define VECTOR_014      PendSV_Handler         // 0x0000_0038 14 -         ARM core         Pendable request for system service (PendableSrvReq)  
#define VECTOR_015      SysTick_Handler        // 0x0000_003C 15 -         ARM core         System tick timer (SysTick)                           
    /*Peripheral interrupt*/
//#define VECTOR_016      DMA0_IRQHandler              // 0x0000_0040 16     0     DMA              DMA Channel 0 transfer complete
//#define VECTOR_017      DMA1_IRQHandler              // 0x0000_0044 17     1     DMA              DMA Channel 1 transfer complete
//#define VECTOR_018      DMA2_IRQHandler              // 0x0000_0048 18     2     DMA              DMA Channel 2 transfer complete
//#define VECTOR_019      DMA3_IRQHandler              // 0x0000_004C 19     3     DMA              DMA Channel 3 transfer complete
//#define VECTOR_020      DMA4_IRQHandler              // 0x0000_0050 20     4     DMA              DMA Channel 4 transfer complete
//#define VECTOR_021      DMA5_IRQHandler              // 0x0000_0054 21     5     DMA              DMA Channel 5 transfer complete
//#define VECTOR_022      DMA6_IRQHandler              // 0x0000_0058 22     6     DMA              DMA Channel 6 transfer complete
//#define VECTOR_023      DMA7_IRQHandler              // 0x0000_005C 23     7     DMA              DMA Channel 7 transfer complete
//#define VECTOR_024      DMA8_IRQHandler              // 0x0000_0060 24     8     DMA              DMA Channel 8 transfer complete
//#define VECTOR_025      DMA9_IRQHandler              // 0x0000_0064 25     9     DMA              DMA Channel 9 transfer complete
//#define VECTOR_026      DMA10_IRQHandler             // 0x0000_0068 26    10     DMA              DMA Channel 10 transfer complete
//#define VECTOR_027      DMA11_IRQHandler             // 0x0000_006C 27    11     DMA              DMA Channel 11 transfer complete
//#define VECTOR_028      DMA12_IRQHandler             // 0x0000_0070 28    12     DMA              DMA Channel 12 transfer complete
//#define VECTOR_029      DMA13_IRQHandler             // 0x0000_0074 29    13     DMA              DMA Channel 13 transfer complete
//#define VECTOR_030      DMA14_IRQHandler             // 0x0000_0078 30    14     DMA              DMA Channel 14 transfer complete
//#define VECTOR_031      DMA15_IRQHandler             // 0x0000_007C 31    15     DMA              DMA Channel 15 transfer complete
//#define VECTOR_032      DMA_ERR_IRQHandler           // 0x0000_0080 32    16     DMA              DMA Error Interrupt Channels 0-15
//#define VECTOR_033      MCM_IRQHandler               // 0x0000_0084 33    17     MCM              
//#define VECTOR_034      FLASH_CC_IRQHandler          // 0x0000_0088 34    18     Flash memory     Command Complete
//#define VECTOR_035      FLASH_RC_IRQHandler          // 0x0000_008C 35    19     Flash memory     Read Collision
//#define VECTOR_036      VLD_IRQHandler               // 0x0000_0090 36    20     Mode Controller  Low Voltage Detect,Low Voltage Warning, Low Leakage Wakeup
#define VECTOR_037      LLWU_IRQHandler              // 0x0000_0094 37    21     LLWU             Low Leakage Wakeup
//#define VECTOR_038      WDOG_IRQHandler              // 0x0000_0098 38    22     WDOG
//#define VECTOR_039      RNGB_IRQHandler              // 0x0000_009C 39    23		RNG              Random Number Generator
//#define VECTOR_040      I2C0_IRQHandler              // 0x0000_00A0 40    24     I2C0
//#define VECTOR_041      I2C1_IRQHandler              // 0x0000_00A4 41    25     I2C1
//#define VECTOR_042      SPI0_IRQHandler              // 0x0000_00A8 42    26     SPI0             Single interrupt vector for all sources
//#define VECTOR_043      SPI1_IRQHandler              // 0x0000_00AC 43    27     SPI1             Single interrupt vector for all sources
//#define VECTOR_044      I2S0_IRQHandler              // 0x0000_00B0 44    28     I2S0             I2S0 Transmit
//#define VECTOR_045      I2S1_IRQHandler              // 0x0000_00B4 45    29     I2S0             I2S0 Receive
//#define VECTOR_046      UART0_STAT_IRQHandler        // 0x0000_00B8 46    30     UART0            Single interrupt vector for UART LON sources
#define VECTOR_047      UART0_IRQHandler               // 0x0000_00BC 47    31     UART0            Single interrupt vector for UART status sources
//#define VECTOR_048      UART0_ERR_IRQHandler         // 0x0000_00C0 48    32     UART0            Single interrupt vector for UART error sources
#define VECTOR_049      UART1_IRQHandler             // 0x0000_00C4 49    33     UART1            Single interrupt vector for UART status sources
//#define VECTOR_050      UART1_ERR_IRQHandler         // 0x0000_00C8 50    34     UART1            Single interrupt vector for UART error sources
#define VECTOR_051      UART2_IRQHandler             // 0x0000_00CC 51    35     UART2            Single interrupt vector for UART status sources
//#define VECTOR_052      UART2_ERR_IRQHandler         // 0x0000_00D0 52    36     UART2            Single interrupt vector for UART error sources
#define VECTOR_053      UART3_IRQHandler             // 0x0000_00D4 53    37     UART3            Single interrupt vector for UART status sources
//#define VECTOR_054      UART3_ERR_IRQHandler         // 0x0000_00D8 54    38     UART3            Single interrupt vector for UART error sources
//#define VECTOR_055      ADC0_IRQHandler              // 0x0000_00DC 55    39     ADC0
//#define VECTOR_056      CMP0_IRQHandler              // 0x0000_00E0 56    40     CMP0
//#define VECTOR_057      CMP1_IRQHandler              // 0x0000_00E4 57    41     CMP1
//#define VECTOR_058      FTM0_IRQHandler              // 0x0000_00E8 58    42     FTM0             Single interrupt vector for all sources
//#define VECTOR_059      FTM1_IRQHandler              // 0x0000_00EC 59    43     FTM1             Single interrupt vector for all sources
//#define VECTOR_060      FTM2_IRQHandler              // 0x0000_00F0 60    44     FTM2             Single interrupt vector for all sources
//#define VECTOR_061      CMT_IRQHandler               // 0x0000_00F4 61    45     CMT
#define VECTOR_062      RTC_IRQHandler               // 0x0000_00F8 62    46     RTC              Alarm interrupt
#define VECTOR_063      RTC_Seconds_IRQHandler       // 0x0000_00FC 63    47     RTC              Seconds interrupt
#define VECTOR_064      PIT0_IRQHandler              // 0x0000_0100 64    48     PIT              Channel 0
#define VECTOR_065      PIT1_IRQHandler              // 0x0000_0104 65    49     PIT              Channel 1
#define VECTOR_066      PIT2_IRQHandler              // 0x0000_0108 66    50     PIT              Channel 2
#define VECTOR_067      PIT3_IRQHandler              // 0x0000_010C 67    51     PIT              Channel 3
//#define VECTOR_068      PDB_IRQHandler               // 0x0000_0110 68    52     PDB
#define VECTOR_069      USB_OTG_IRQHandler           // 0x0000_0114 69    53     USB OTG
//#define VECTOR_070      USB_CD_IRQHandler            // 0x0000_0118 70    54     USB Chg Detect   
#define VECTOR_071      DryIce_Tamper_IRQHandler     // 0x0000_011C 71    55     DryIce           Tamper
//#define VECTOR_072      DAC0_IRQHandler              // 0x0000_0120 72    56     DAC0
//#define VECTOR_073      MCG_IRQHandler               // 0x0000_0124 73    57     MCG
//#define VECTOR_074      LPT_IRQHandler               // 0x0000_0128 74    58     Low Power Timer
#define VECTOR_075      PORTA_IRQHandler             // 0x0000_012C 75    59     Port ctrl module Pin detect (Port A)
#define VECTOR_076      PORTB_IRQHandler             // 0x0000_0130 76    60     Port ctrl module Pin detect (Port B)
#define VECTOR_077      PORTC_IRQHandler             // 0x0000_0134 77    61     Port ctrl module Pin detect (Port C)
#define VECTOR_078      PORTD_IRQHandler             // 0x0000_0138 78    62     Port ctrl module Pin detect (Port D)
#define VECTOR_079      PORTE_IRQHandler             // 0x0000_013C 79    63     Port ctrl module Pin detect (Port E)
//#define VECTOR_080      SOFTWARE_IRQHandler          // 0x0000_0140 80    64     Software 	     Software interrupt
//#define VECTOR_081      SPI2_IRQHandler              // 0x0000_00B0 81    65     SPI2             Single interrupt vector for all sources                   
#define VECTOR_082      UART4_IRQHandler             // 0x0000_0114 82    66     UART4            Single interrupt vector for UART status sources           
#define VECTOR_083      UART4_ERR_IRQHandler         // 0x0000_0118 83    67     UART4            Single interrupt vector for UART error sources            
//#define VECTOR_084      UART5_IRQHandler             // 0x0000_011C 84    68     UART5            Single interrupt vector for UART status sources           
//#define VECTOR_085      UART5_ERR_IRQHandler         // 0x0000_0120 85    69     UART5            Single interrupt vector for UART error sources            
//#define VECTOR_086      CMP2_IRQHandler              // 0x0000_0130 86    70     CMP2                                                                       
//#define VECTOR_087      FTM3_IRQHandler              // 0x0000_00E8 87    71     FTM0             Single interrupt vector for all sources
//#define VECTOR_088      DAC1_IRQHandler              // 0x0000_0188 88    72     DAC1                                                                       
//#define VECTOR_089      ADC1_IRQHandler              // 0x0000_0128 89    73     ADC1                                                                       
//#define VECTOR_090      I2C2_IRQHandler              // 0x0000_00A4 90    74     I2C2
//#define VECTOR_091      CAN0_ORM_IRQHandler          // 0x0000_00B4 91    75     CAN0             OR'ed Message buffer (0-15)                               
//#define VECTOR_092      CAN0_BusOff_IRQHandler       // 0x0000_00B8 92    76     CAN0             Bus Off                                                   
//#define VECTOR_093      CAN0_ERR_IRQHandler          // 0x0000_00BC 93    77     CAN0             Error                                                     
//#define VECTOR_094      CAN0_Transmit_IRQHandler     // 0x0000_00C0 94    78     CAN0             Transmit Warning                                          
//#define VECTOR_095      CAN0_Recv_IRQHandler         // 0x0000_00C4 95    79     CAN0             Receive Warning                                           
//#define VECTOR_096      CAN0_WakeUp_IRQHandler       // 0x0000_00C8 96    80     CAN0             Wake Up                                                   
//#define VECTOR_097      SDHC_IRQHandler              // 0x0000_0180 97    81     SDHC                                                                       




//systick
void SysTick_Handler(void);
//Uart
void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void UART2_IRQHandler(void);
void UART3_IRQHandler(void);
void UART4_IRQHandler(void);

void UART4_ERR_IRQHandler(void);
//gpio
void PORTA_IRQHandler(void);
void PORTB_IRQHandler(void);
void PORTC_IRQHandler(void);
void PORTD_IRQHandler(void);
void PORTE_IRQHandler(void);
//pit
void PIT0_IRQHandler(void);
void PIT1_IRQHandler(void);
void PIT2_IRQHandler(void);
void PIT3_IRQHandler(void);

//SRTC DryIce
void RTC_IRQHandler(void);
void RTC_Seconds_IRQHandler(void);
void DryIce_Tamper_IRQHandler(void);

void LLWU_IRQHandler(void);

void USB_OTG_IRQHandler(void);



#endif
  
