/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : w25x_hw.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 8/4/2014 4:14:50 PM
 * Description        : 
 *******************************************************************************/
#ifndef __W25X_HW_H__
#define __W25X_HW_H__
#include "common.h"
//硌鍔桶
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

#define ExFLASH_DummyByte       0xFF
#define SPI_FLASH_PageSize      256


//每 W25X10: 1M-bit / 128K-byte (131,072)
//每 W25X20: 2M-bit / 256K-byte (262,144)
//每 W25X40: 4M-bit / 512K-byte (524,288)
//每 W25X80: 8M-bit / 1M-byte (1,048,576)
//每 256-bytes per programmable page 
//每 Uniform 4K-byte Sectors / 64K-byte Blocks
#define ManufactDeviceW25X10 (uint16_t)0xEF10
#define ManufactDeviceW25X20 (uint16_t)0xEF11
#define ManufactDeviceW25X40 (uint16_t)0xEF12
#define ManufactDeviceW25X80 (uint16_t)0xEF13
//每 W25X16: 16M-bit / 2M-byte (2,097,152)
//每 W25X32: 32M-bit / 4M-byte (4,194,304)
//每 W25X64: 64M-bit / 8M-byte (8,388,608)
//每 256-bytes per programmable page 
//每 Uniform 4K-byte Sectors / 64K-byte Blocks
#define ManufactDeviceW25X16 (uint16_t)0xEF14
#define ManufactDeviceW25X32 (uint16_t)0xEF15
#define ManufactDeviceW25X64 (uint16_t)0xEF16


int hw_w25x_spi_init(void);
void hw_w25x_gpio_init(void);
uint8_t hw_w25x_flash_read_SR(void);
void hw_w25x_flash_write_SR(uint8_t status);
uint16_t hw_w25x_flash_read_ID(void);
void hw_w25x_flash_read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);
void hw_w25x_flash_write_page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void hw_w25x_flash_write_nocheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite); 
void hw_w25x_flash_write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void hw_w25_flash_erase_chip(void); 
void hw_w25_flash_erase_sector(uint32_t Dst_Addr);
void hw_w25x_flash_wait_busy(uint32_t timeout_ms);
void hw_w25x_flash_powerdown(void);
void hw_w25x_flash_wakeup(void);

#endif


