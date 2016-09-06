/********************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
**************************************************************************//*!
 *
 * @file flash_hw.c
 *
 * @author
 *
 * @version
 *
 * @date
 *
 * @brief The file includes flash routines
 *
 *****************************************************************************/

/*********************************** Includes ***********************************/
#include "common.h"
#include "flash_hw.h" /* include flash driver header file */
/*********************************** Macros ************************************/
#define UNUSED(x)   (void)x;

/*********************************** Defines ***********************************/

/********************************** Constant ***********************************/

/*********************************** Variables *********************************/

/*********************************** Prototype *********************************/

/*********************************** Function **********************************/

/*******************************************************************************
 * Function:        Flash_Init
 *
 * Description:     Set the flash clock
 *
 * Returns:         none
 *
 * Notes:
 *
 *******************************************************************************/
void hw_flash_init(void)
{
//    UNUSED(FlashClockDiv)
			
    /* checking access error */
    if (FTFL_FSTAT & (FTFL_FSTAT_ACCERR_MASK|FTFL_FSTAT_FPVIOL_MASK|FTFL_FSTAT_RDCOLERR_MASK))
    {
        /* clear error flag */
    	FTFL_FSTAT |= (FTFL_FSTAT_ACCERR_MASK|FTFL_FSTAT_FPVIOL_MASK|FTFL_FSTAT_RDCOLERR_MASK);
    }
    FMC_BASE_PTR->PFB0CR &= ~FMC_PFB0CR_B0DCE_MASK;
    FMC_BASE_PTR->PFB1CR &= ~FMC_PFB1CR_B1DCE_MASK;

}

/*******************************************************************************
 * Function:        Flash_SectorErase
 *
 * Description:     erase a sector of the flash
 *
 * Returns:         Error Code
 *
 * Notes:
 *
 *******************************************************************************/
unsigned char hw_flash_SectorErase(uint32_t *FlashPointer)
{
    unsigned char Return = Flash_OK;
    uint32_t FlashPtr = (uint32_t)FlashPointer;

    /* Allocate space on stack to run flash command out of SRAM */
    /* wait till CCIF is set*/
    while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK))
    {;}
    
    /* Write command to FCCOB registers */
    FTFL_FCCOB0 = FlashCmd_SectorErase;
    FTFL_FCCOB1 = (uint8_t)(FlashPtr >> 16);
    FTFL_FCCOB2 = (uint8_t)((FlashPtr >> 8) & 0xFF);
    FTFL_FCCOB3 = (uint8_t)(FlashPtr & 0xFF);
    
    /* Launch command */
    SpSub();
    /* checking access error */
    if (FTFL_FSTAT & FTFL_FSTAT_ACCERR_MASK)
    {
        /* clear error flag */
    	FTFL_FSTAT |= FTFL_FSTAT_ACCERR_MASK;

        /* update return value*/
        Return |= Flash_FACCERR;
    }
    /* checking protection error */
    else if (FTFL_FSTAT & FTFL_FSTAT_FPVIOL_MASK)
    {
    	/* clear error flag */
    	FTFL_FSTAT |= FTFL_FSTAT_FPVIOL_MASK;

        /* update return value*/
        Return |= Flash_FPVIOL;
    }
    else if (FTFL_FSTAT & FTFL_FSTAT_RDCOLERR_MASK)
    {
       	/* clear error flag */
       	FTFL_FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;

       	/* update return value*/
       	Return |= Flash_RDCOLERR;
    }
    /* checking MGSTAT0 non-correctable error */
    else if (FTFL_FSTAT & FTFL_FSTAT_MGSTAT0_MASK)
    {
    	Return |= Flash_MGSTAT0;
    }
    /* function return */
    return  Return;
}

/*******************************************************************************
 * Function:        Flash_ByteProgram
 *
 * Description:     byte program the flash
 *
 * Returns:         Error Code
 *
 * Notes:
 *
 *******************************************************************************/
unsigned char hw_flash_ByteProgram(uint32_t *FlashStartAddress,uint32_t *DataSrcPtr,uint32_t NumberOfBytes)
{
    unsigned char Return = Flash_OK;
    uint32_t FlashStartAdd = (uint32_t)FlashStartAddress;/*FSL: glue logic*/
    
    /* Allocate space on stack to run flash command out of SRAM */
    uint32_t size_buffer;
    if (NumberOfBytes == 0)
    {
    	return Flash_CONTENTERR;
    }
    else
    {
//        TRACE("\n-|num:%d",NumberOfBytes);
        NumberOfBytes = ((NumberOfBytes +CODE_LEN_ALIGN_BYTES-1)&CODE_LEN_ALIGN_MASK_AND);
    	size_buffer = (NumberOfBytes - 1)/4 + 1;	
//        TRACE("\n-|size_buffer:%d num:%d",size_buffer,NumberOfBytes);
    }
    /* wait till CCIF is set*/
    while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK))
    {;}
        
    while ((size_buffer) && (Return == Flash_OK))
    {
      /*FSL: for FTFL, 4-byte aligned, for FTFE, 8 byte aligned!*/
//        TRACE("\n-|add:%04X  size_buffer:%d",FlashStartAdd,size_buffer);
      
       /* Write command to FCCOB registers */
       FTFL_FCCOB0 = FlashCmd_ProgramLongWord;
       FTFL_FCCOB1 = (uint8_t)(FlashStartAdd >> 16);
       FTFL_FCCOB2 = (uint8_t)((FlashStartAdd >> 8) & 0xFF);
       FTFL_FCCOB3 = (uint8_t)(FlashStartAdd & 0xFF);
#ifdef __MK_xxx_H__ /*little endian*/
       FTFL_FCCOB4 = (uint8_t)(*((uint8_t*)DataSrcPtr+3));
       FTFL_FCCOB5 = (uint8_t)(*((uint8_t*)DataSrcPtr+2));
       FTFL_FCCOB6 = (uint8_t)(*((uint8_t*)DataSrcPtr+1));
       FTFL_FCCOB7 = (uint8_t)(*((uint8_t*)DataSrcPtr+0));
#else /* Big endian */
#error "Big endian"
       FTFL_FCCOB4 = (uint8_t)(*((uint8_t*)DataSrcPtr+0));
       FTFL_FCCOB5 = (uint8_t)(*((uint8_t*)DataSrcPtr+1));
       FTFL_FCCOB6 = (uint8_t)(*((uint8_t*)DataSrcPtr+2));
       FTFL_FCCOB7 = (uint8_t)(*((uint8_t*)DataSrcPtr+3));
#endif
#if defined(FTFE_SUPPORT)
#ifdef __MK_xxx_H__ /*little endian*/
       FTFE_FCCOB8 = (uint8_t)(*((uint8_t*)DataSrcPtr+7));
       FTFE_FCCOB9 = (uint8_t)(*((uint8_t*)DataSrcPtr+6));
       FTFE_FCCOBA = (uint8_t)(*((uint8_t*)DataSrcPtr+5));
       FTFE_FCCOBB = (uint8_t)(*((uint8_t*)DataSrcPtr+4));
#else /* Big endian */
       FTFE_FCCOB8 = (uint8_t)(*((uint8_t*)DataSrcPtr+4));
       FTFE_FCCOB9 = (uint8_t)(*((uint8_t*)DataSrcPtr+5));
       FTFE_FCCOBA = (uint8_t)(*((uint8_t*)DataSrcPtr+6));
       FTFE_FCCOBB = (uint8_t)(*((uint8_t*)DataSrcPtr+7));
#endif
#endif
       
       /* Launch command */
       SpSub();
	
       /* checking access error */
       if (FTFL_FSTAT & FTFL_FSTAT_ACCERR_MASK)
       {
          /* clear error flag */
          FTFL_FSTAT |= FTFL_FSTAT_ACCERR_MASK;

          /* update return value*/
          Return |= Flash_FACCERR;
       }
       /* checking protection error */
       else if (FTFL_FSTAT & FTFL_FSTAT_FPVIOL_MASK)
       {
          /* clear error flag */
          FTFL_FSTAT |= FTFL_FSTAT_FPVIOL_MASK;

          /* update return value*/
          Return |= Flash_FPVIOL;
       }
       else if (FTFL_FSTAT & FTFL_FSTAT_RDCOLERR_MASK)
       {
          /* clear error flag */
          FTFL_FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;

          /* update return value*/
          Return |= Flash_RDCOLERR;
       }
       /* checking MGSTAT0 non-correctable error */
       else if (FTFL_FSTAT & FTFL_FSTAT_MGSTAT0_MASK)
       {
          Return |= Flash_MGSTAT0;
       }
       /* decrement byte count */
#ifndef FTFE_SUPPORT
       size_buffer --;
       (uint32_t*)DataSrcPtr++;
       FlashStartAdd +=4;
#else
       size_buffer-=2;
       (uint32_t*)DataSrcPtr++;
       (uint32_t*)DataSrcPtr++;
       FlashStartAdd +=8;
#endif
    }
    /* function return */
    return  Return;
}

uint8_t hw_flash_SecurityVerify(const uint8_t backdoor_key[8]) 
{
    /* clear RDCOLERR & ACCERR & FPVIOL flag in flash status register. Write 1 to clear*/
    //    FTFL_FSTAT  =  FTFx_SSD_FSTAT_ERROR_BITS;
    hw_flash_init();

    /* Write command to FCCOB registers */
    FTFL_FCCOB0 = FlashCmd_SecurityVFY;
#ifdef __MK_xxx_H__ /*little endian*/
    FTFL_FCCOB4 = (uint8_t)(*((uint8_t*)backdoor_key+3));
    FTFL_FCCOB5 = (uint8_t)(*((uint8_t*)backdoor_key+2));
    FTFL_FCCOB6 = (uint8_t)(*((uint8_t*)backdoor_key+1));
    FTFL_FCCOB7 = (uint8_t)(*((uint8_t*)backdoor_key+0));
    FTFL_FCCOB8 = (uint8_t)(*((uint8_t*)backdoor_key+7));
    FTFL_FCCOB9 = (uint8_t)(*((uint8_t*)backdoor_key+6));
    FTFL_FCCOBA = (uint8_t)(*((uint8_t*)backdoor_key+5));
    FTFL_FCCOBB = (uint8_t)(*((uint8_t*)backdoor_key+4));
#else /* Big endian */
#error "Big endian"
    FTFL_FCCOB4 = (uint8_t)(*((uint8_t*)backdoor_key+0));
    FTFL_FCCOB5 = (uint8_t)(*((uint8_t*)backdoor_key+1));
    FTFL_FCCOB6 = (uint8_t)(*((uint8_t*)backdoor_key+2));
    FTFL_FCCOB7 = (uint8_t)(*((uint8_t*)backdoor_key+3));
    FTFL_FCCOB8 = (uint8_t)(*((uint8_t*)backdoor_key+4));
    FTFL_FCCOB9 = (uint8_t)(*((uint8_t*)backdoor_key+5));
    FTFL_FCCOBA = (uint8_t)(*((uint8_t*)backdoor_key+6));
    FTFL_FCCOBB = (uint8_t)(*((uint8_t*)backdoor_key+7));
#endif
    SpSub();
    return (FTFL_FSTAT&FTFx_SSD_FSTAT_ERROR_BITS);
}


/*******************************************************************************
 * Function:        SpSub
 *
 * Description:     Execute the Flash write while running out of SRAM
 *
 * Returns:         none
 *
 * Notes:
 *
 *******************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
__ramfunc
#elif defined(__CWCC__)
__relocate_code__
#endif
void SpSub(void)
{
    /* Launch command */
    FTFL_FSTAT |= FTFL_FSTAT_CCIF_MASK;    
    /* wait for command completion */
    while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK)) {};
}
/* Leave this immediately after SpSub */
void SpSubEnd(void) {}

