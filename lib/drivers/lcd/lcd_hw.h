/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : lcd_hw.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 7/31/2014 1:51:20 PM
 * Description        : 
 *******************************************************************************/
#ifndef __LCD_HW_H__
#define __LCD_HW_H__

//Òº¾§¿ØÖÆÆ÷ÃüÁîÊý¾Ý
//don't have para
/*ADC = 0: normal direction (SEG0 ¡úSEG131) */
#define LCD_CMD_ADC_NORMAL         (uint8_t)0xA0
/*ADC = 0: reverse direction(SEG131 ¡úSEG0) */
#define LCD_CMD_ADC_REVERSE        (uint8_t)0xA1
/*Selects LCD bias ratio of the voltage required for driving the LCD.*/
#define LCD_CMD_BIAS_A2            (uint8_t)0xA2
#define LCD_CMD_BIAS_A3            (uint8_t)0xA3
/*Forces the whole LCD points to be turned on regardless of the contents of the 
*display data RAM. At this time,the contents of the display data RAM are held.
This instruction has priority over the reverse display ON / OFF instruction. */
#define LCD_CMD_ENTIRE_DIS_NORMAL  (uint8_t)0xA4
#define LCD_CMD_ENTIRE_DIS_ALLON   (uint8_t)0xA5
/*Reverses the display status on LCD panel without rewriting the contents of the display data RAM. */
#define LCD_CMD_DISP_NORMAL        (uint8_t)0xA6
#define LCD_CMD_DISP_REVERSE       (uint8_t)0xA7
/* Turns the Display ON or OFF*/
#define LCD_CMD_DISPLAY_OFF        (uint8_t)0xAE
#define LCD_CMD_DISPLAY_ON         (uint8_t)0xAF

#define LCD_CMD_ReadModifyWrite	   (uint8_t)0xE0
/*RESET:
*This instruction resets initial display line, column address, page address,and common output
*status select to their initial status, but dose not affect the contents of display data RAM.
*This instruction cannot initialize the LCD power supply,which is initialized by the RESETB pin.
*/
#define LCD_CMD_RESET              (uint8_t)0xE2
#define LCD_CMD_NOP				   (uint8_t)0xE3
#define LCD_CMD_HZ314			   (uint8_t)0xE4
#define LCD_CMD_HZ263			   (uint8_t)0xE5
#define LCD_CMD_END				   (uint8_t)0xEE

/*SHL = 0: normal direction (COM0 ¡úCOM63)*/
#define LCD_CMD_COM_NORMAL         (uint8_t)0xC0
/*SHL = 1: reverse direction (COM63 ¡úCOM0)*/
#define LCD_CMD_COM_REVERSE        (uint8_t)0xC8

/*Selects resistance ratio of the internal resistor used in the internal voltage regulator. 
 * See voltage regulator section in power supply circuit. Refer to the table 15.*/
#define LCD_CMD_V0_SET(x)             (uint8_t)(0x20+(x)&0x07)//20~27
/*Reference Voltage Select:CONTRAST
 * Selects one of eight power circuit functions by using 3-bit register. An external power 
 * supply and part of internal power supply functions can be used simultaneously.
 |-|RS|RW|DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|
 |-|0 |0 | 0 | 0 | 1 | 0 | 1 |VC |VR |VF |
 |VC=0:Internal voltage converter circuit is OFF
 |VC=1:Internal voltage converter circuit is ON
 |VR=0:Internal voltage regulator circuit is OFF
 |VR=1:Internal voltage regulator circuit is ON
 |VF=0:Internal voltage follower circuit is OFF
 |VF=1:Internal voltage follower circuit is ON
 * */
#define LCD_CMD_POWER_CTRL                  (uint8_t)0x28
#define LCD_CMD_POWER_ConverterCircuitMASK  (uint8_t)0x04
#define LCD_CMD_POWER_RegulatorCircuitMASK  (uint8_t)0x02
#define LCD_CMD_POWER_FollowerCircuitMASK   (uint8_t)0x01

/*
Consists of 2-byte instruction. The 1st instruction sets reference voltage mode, the 2nd
one updates the contents of reference voltage register. After second instruction, reference
voltage mode is released.*/
#define LCD_CMD_REFVOL_1ST            (uint8_t)0x81      //Double Byte Command:frist
#define LCD_CMD_REFVOL_2ND(x)         (uint8_t)((x)&0x3F)//Double Byte Command:second

//have one para
#define LCD_CMD_START_LINE(x)         (uint8_t)(0x40+((x)&0x3F))//40~7F.x=[0,63]
#define LCD_CMD_PAGE_ADDR(x)          (uint8_t)(0xB0+(x))//B0~B8
/*Set Column Address:SEG0~SEG131
Sets the Column Address of display RAM from the microprocessor into the Column Address register. 
Along with the Column Address, the Column Address defines the address of the display RAM to 
write or read display data. When the microprocessor reads or writes display data to or from 
display RAM, Column Addresses are automatically increased. */
#define LCD_CMD_COL_ADDRH             (uint8_t)0x10//10~18
#define LCD_CMD_COL_ADDRL             (uint8_t)0x00//00~0F

//===============================================================================================
#define LCD_NORMAL		(uint8_t)0
#define LCD_REVERSE 	(uint8_t)1
//===============================================================================================



int hw_lcd_spi_init(void);
void hw_lcd_gpio_init(void);
void hw_lcd_write_cmd(uint8_t cmd);
void hw_lcd_write_bytedata(uint8_t data);
void hw_lcd_write_data(const uint8_t *data,uint32_t length);

void hw_lcd_init(void);
void hw_lcd_set_column(uint8_t column);
void hw_lcd_set_line(uint8_t line);
void hw_lcd_set_page(uint8_t page);
void hw_lcd_dismode(uint8_t mode);
void hw_lcd_contrast(uint8_t volume);
void hw_lcd_backlight(uint8_t level);
#endif

