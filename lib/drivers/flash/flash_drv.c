/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : flash_drv.c
 * bfief              : The drivers level of flash
 * Author             : luocs  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/8/2014 4:36:22 PM
 * Description        : 
 *******************************************************************************/

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "flash_drv.h"

/**  
 * \brief   Initialize the UART for 8N1 operation, interrupts disabled, and 
 *          no hardware flow-control
 * \author   
 * \param 
 *         uartch      UART channel to initialize:UART0~UART4
 *         baud        UART baud rate
 * \return none
 * \todo    
 * \warning . 
 */
uint8_t drv_flash_SectorErase(uint32_t addr)
{
    uint8_t ret;

    addr = FLASH_SECTOR_SIZE*(addr/FLASH_SECTOR_SIZE);
    DisableInterrupts;
    hw_systick_close();
    ret = hw_flash_SectorErase((uint32_t *)addr);
    hw_systick_open();
    EnableInterrupts;
    return ret;
}

uint8_t drv_flash_write(uint32_t StartAddress,uint32_t *DataSrcPtr,uint32_t NumberOfBytes)
{
    uint8_t ret;
    DisableInterrupts;
    hw_systick_close();
    ret = hw_flash_ByteProgram((uint32_t *)StartAddress,DataSrcPtr,NumberOfBytes);
    hw_systick_open();
    EnableInterrupts;

    return ret;
}

//void flash_trace(const char *title,uint32_t addr,const uint8_t *buffer,uint32_t length)
//{
//    uint32_t i;
//
//    if (title != NULL)
//        TRACE("\n-|%s %d",title,length);
//    else 
//        TRACE("\n-|%d ",length);
//	for(i=0; i<length; i++)
//	{
//		if(i%32==0) {
//			TRACE("\r\n%08X:",addr+i);
//		} else if(i%8==0) {
//			TRACE(" ");
//		} 
//        TRACE(" %02X",buffer[i]);
//	}
//
//}

uint8_t flash_write(uint32_t FlashStartAddress, uint32_t NumberOfBytes,const uint8_t *data)
{
    uint8_t buffer[FLASH_SECTOR_SIZE];
    uint16_t remain_bytes;
    uint16_t temp_length;
    uint32_t start_add = FlashStartAddress;
    uint32_t current_sector = ( start_add/FLASH_SECTOR_SIZE );
    uint16_t sector_offset;
    uint16_t i;
    uint8_t delete_flag = 0;
    uint8_t unaligned=0;
    uint8_t word_unaligned = 0;
    uint8_t error = Flash_OK;
    uint8_t *p=NULL;

//    flash_trace("data",FlashStartAddress,data,NumberOfBytes);
    do {
        remain_bytes = (uint16_t)(((current_sector+1)*FLASH_SECTOR_SIZE)-start_add);
        temp_length = (uint16_t)((NumberOfBytes<remain_bytes) ? NumberOfBytes:remain_bytes);/*FSL: special case*/

        sector_offset = (uint32_t)( start_add%FLASH_SECTOR_SIZE );
        unaligned = start_add%CODE_LEN_ALIGN_BYTES;
        word_unaligned = CODE_LEN_ALIGN_BYTES-(temp_length%CODE_LEN_ALIGN_BYTES)-unaligned;
        start_add -= unaligned;
        temp_length +=  unaligned;
        sector_offset -= unaligned;
        p = (uint8_t *)start_add;
//        TRACE("\n-|addr:%08X len:%d addralign:%d flashwtalign\n",start_add,temp_length,unaligned,word_unaligned);
        for(i=0 ;i<temp_length+word_unaligned;i++)
        {
//            TRACE(" %02X",*p);
            if( *p != 0xFF ) {
                delete_flag = 1;
                break;
            }
            p++;
        }
        if(delete_flag) {
//            TRACE("\nErase:%08X",current_sector*FLASH_SECTOR_SIZE);
            start_add += unaligned;
            temp_length -=  unaligned;
            sector_offset += unaligned;
            memcpy(buffer,(uint8_t *)(current_sector*FLASH_SECTOR_SIZE),sizeof(buffer));
            memcpy(buffer+sector_offset,data,temp_length);
            if (word_unaligned < CODE_LEN_ALIGN_BYTES) {
                memcpy(buffer+sector_offset+temp_length,"\xFF\xFF\xFF\xFF\xFF\xFF\xFF",word_unaligned);
            }
//            flash_trace("ERaseVal",current_sector*FLASH_SECTOR_SIZE,
//                        (uint8_t *)(current_sector*FLASH_SECTOR_SIZE),FLASH_SECTOR_SIZE);
//            flash_trace("val",current_sector*FLASH_SECTOR_SIZE,buffer,FLASH_SECTOR_SIZE);
            if( (error=drv_flash_SectorErase(start_add)) != Flash_OK ) {
//                TRACE("\nerase:%d,addr:%08X",error,start_add);
                break;
            }
            if ((error=drv_flash_write(current_sector*FLASH_SECTOR_SIZE,(uint32_t *)buffer,FLASH_SECTOR_SIZE)) != Flash_OK ) {
                break;
            }
            delete_flag = 0;/*done*/
        } else {
//            TRACE("\nNOTneedErase");
            memcpy(buffer,(uint8_t *)(start_add),unaligned);
            memcpy(buffer+unaligned,data,temp_length-unaligned);
            if (word_unaligned < CODE_LEN_ALIGN_BYTES) {
                memcpy(buffer+temp_length,"\xFF\xFF\xFF\xFF\xFF\xFF\xFF",word_unaligned);
            }
//            flash_trace("val",start_add,buffer,temp_length);
            if ((error=drv_flash_write(start_add,(uint32_t *)buffer,temp_length)) != Flash_OK ) {
                break;
            }
            start_add += unaligned;
            temp_length -=  unaligned;
            sector_offset += unaligned;
        }
        NumberOfBytes -= temp_length;
        start_add += temp_length;
        current_sector++;
        data += temp_length;
    } while(NumberOfBytes);
//    TRACE("\nerr:%d",error);
    return error;
}


/********************************************************************/

