/********************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
**************************************************************************//*!
 *
 * @file flash_FTFL.h
 *
 * @author
 *
 * @version
 *
 * @date
 *
 * @brief flash driver header file
 *
 *****************************************************************************/

/*********************************** Includes ***********************************/

/*********************************** Macros ************************************/

/*********************************** Defines ***********************************/
#ifndef _FLASH_FTFL_H_
#define _FLASH_FTFL_H_


#define __MK_xxx_H__ 

#define FTFE_SUPPORT

#if defined(FTFE_SUPPORT)
#define FTFL_FSTAT                  FTFE_FSTAT
#define FTFL_FSTAT_ACCERR_MASK      FTFE_FSTAT_ACCERR_MASK
#define FTFL_FSTAT_FPVIOL_MASK      FTFE_FSTAT_FPVIOL_MASK 
#define FTFL_FSTAT_RDCOLERR_MASK    FTFE_FSTAT_RDCOLERR_MASK 
#define FTFL_FSTAT_CCIF_MASK        FTFE_FSTAT_CCIF_MASK 
#define FTFL_FSTAT_MGSTAT0_MASK     FTFE_FSTAT_MGSTAT0_MASK 

#define FTFL_FCCOB0  FTFE_FCCOB0 
#define FTFL_FCCOB1  FTFE_FCCOB1 
#define FTFL_FCCOB2  FTFE_FCCOB2 
#define FTFL_FCCOB3  FTFE_FCCOB3 
#define FTFL_FCCOB4  FTFE_FCCOB4 
#define FTFL_FCCOB5  FTFE_FCCOB5 
#define FTFL_FCCOB6  FTFE_FCCOB6 
#define FTFL_FCCOB7  FTFE_FCCOB7 
#define FTFL_FCCOB8  FTFE_FCCOB8 
#define FTFL_FCCOB9  FTFE_FCCOB9 
#define FTFL_FCCOBA  FTFE_FCCOBA 
#define FTFL_FCCOBB  FTFE_FCCOBB 

#define CODE_LEN_ALIGN_BYTES    (uint32_t)8
#else
#define CODE_LEN_ALIGN_BYTES    (uint32_t)4
#endif
#define CODE_LEN_ALIGN_MASK_AND     (~((uint32_t)(CODE_LEN_ALIGN_BYTES-1)))
#define CODE_LEN_ALIGN_4BYTES       (CODE_LEN_ALIGN_BYTES/4)



/* error code */
#define Flash_OK          0x00
#define Flash_FACCERR     0x01
#define Flash_FPVIOL      0x02
#define Flash_MGSTAT0	  0x04
#define Flash_RDCOLERR	  0x08
#define Flash_NOT_ERASED   0x10
#define Flash_CONTENTERR   0x11

/* flash commands */
#if defined(FTFE_SUPPORT)
#define FlashCmd_ProgramLongWord  	0x07/*Phrase write*/
#else /*Including FTFA_SUPPORT and FTFL_SUPPORT (default) */
#define FlashCmd_ProgramLongWord  	0x06/*longword write*/
#endif
#define FlashCmd_SectorErase    	0x09
#define FlashCmd_SecurityVFY  	    0x45


#define FTFx_SSD_FSTAT_CCIF                 0x80U
#define FTFx_SSD_FSTAT_RDCOLERR             0x40U
#define FTFx_SSD_FSTAT_ACCERR               0x20U
#define FTFx_SSD_FSTAT_FPVIOL               0x10U
#define FTFx_SSD_FSTAT_MGSTAT0              0x01U
#define FTFx_SSD_FSTAT_ERROR_BITS           (FTFx_SSD_FSTAT_ACCERR \
                                            |FTFx_SSD_FSTAT_FPVIOL \
                                            |FTFx_SSD_FSTAT_MGSTAT0)



/********************************** Constant ***********************************/

/*********************************** Variables *********************************/

/*********************************** Prototype *********************************/
void hw_flash_init(void);
unsigned char hw_flash_SectorErase(uint32_t *FlashPtr);
unsigned char hw_flash_ByteProgram(uint32_t *FlashStartAddress,uint32_t *DataSrcPtr,uint32_t NumberOfBytes);
uint8_t hw_flash_SecurityVerify(const uint8_t backdoor_key[8]);

#if defined(__IAR_SYSTEMS_ICC__)
__ramfunc
#elif defined(__CWCC__)
__relocate_code__
#endif
void SpSub(void);
void SpSubEnd(void);
#endif /*_FLASH_FTFL_H_*/

