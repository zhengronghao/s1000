/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : flash_drv.h
 * bfief              : The drivers level of UART
 * Author             : luocs   
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/8/2014 4:28:30 PM
 * Description        : 
 *******************************************************************************/

#ifndef __FLASH_DRV_H__
#define __FLASH_DRV_H__

#define FLASH_SECTOR_SIZE       (4*1024)
#define FLASH_SECTOR_NbrOf1024   (FLASH_SECTOR_SIZE/1024)
#define FLASH_SECTOR_127    (64*FLASH_SECTOR_SIZE)  

uint8_t drv_flash_SectorErase(uint32_t addr);
uint8_t drv_flash_write(uint32_t StartAddress,uint32_t *DataSrcPtr,uint32_t NumberOfBytes);
void flash_identify (void);
uint8_t flash_write(uint32_t FlashStartAddress, uint32_t NumberOfBytes,const uint8_t *data);

#endif

