/*
 * =====================================================================================
 *
 *       Filename:  exflash_drv.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  5/17/2016 10:29:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#include "w25x_hw.h"
/*
 *功能：写外部FLASH
 *
 *参数：
 *
 *返回值：
 *
 *注：读取写入地址所在的扇区，根据写入数据长度判断第一个扇区需要写入的数据量（数据长度，还是扇区偏移后剩余的总长度）
 *
 */
unsigned int exflash_write(unsigned int addr,unsigned int data_len,unsigned char* data)
{
    unsigned char sectors_buf[4*1024];
    unsigned char *data_offset;
    unsigned char  i = 0;
    unsigned char  sectors_start_no = 0;       //地址所在扇区号
    unsigned char  sectors_end_no = 0;       //地址所在扇区号
    unsigned char  sectors_count = 0;       //数据所占的扇区个数
    unsigned int  sectors_start_offset = 0;   //扇区内地址偏移  
    unsigned int sectors_data_len = 0;   //应该向第一个扇区内写入的数据量 
    unsigned int sectors_end_offset = 0;   //扇区内地址偏移  
    unsigned int sectors_size = 4*1024;
     int temp;
    data_offset = data;
    //计算开始扇区号
    sectors_start_no = addr / sectors_size;
    //计算开始地址偏移
    sectors_start_offset = addr % sectors_size;
    temp = data_len - sectors_size + sectors_start_offset;  
    TRACE("\r\ntemp:%d",temp); 
    if(temp > 0)//一个扇区存不下
    {
        TRACE("\r\n111");
        sectors_data_len =  sectors_size-sectors_start_offset;
        sectors_count = (data_len - sectors_data_len)/ sectors_size ;
        sectors_end_offset = (data_len - sectors_data_len)% sectors_size;
        //计算结束扇区号
        sectors_end_no = sectors_count + sectors_start_no;
        if(sectors_end_offset != 0)
        {
           sectors_end_no += 1;

        }
    }
    else
    {
        TRACE("\r\n222");
        sectors_data_len =  data_len;
        sectors_end_no = sectors_start_no;
        sectors_count = 0;
        sectors_end_offset = 0;
    }
    
    TRACE("\r\nsectors_data_len:%d",sectors_data_len); 
    TRACE("\r\nstart no:%d  end_no:%d",sectors_start_no,sectors_end_no);
    TRACE("\r\nstart offset:%d  end offset:%d",sectors_start_offset,sectors_end_offset);
    TRACE("\r\nsectors count:%d",sectors_count);

    //写开始扇区  
   // if(sectors_start_offset != 0)
    {
        hw_w25x_flash_read(sectors_buf,sectors_start_no*sectors_size,sectors_size);//读出一个扇区的数据 
        memcpy(sectors_buf+sectors_start_offset,data_offset,sectors_data_len);
        data_offset = data_offset+sectors_data_len;
        hw_w25_flash_erase_sector(sectors_start_no*sectors_size); 
        hw_w25x_flash_write(sectors_buf,sectors_start_no*sectors_size,sectors_size);
        sectors_start_no ++;
        TRACE("\r\nbuf offset%d",data_offset-data);
    }
    //中间整块扇区
    for(i = 0; i < sectors_count; i++)
    {
        hw_w25_flash_erase_sector(sectors_start_no*sectors_size); 
        hw_w25x_flash_write(data_offset,sectors_start_no*sectors_size,sectors_size);
        data_offset += sectors_size;
        sectors_start_no ++;
    }
    //结束扇区
    if(sectors_end_offset != 0)
    {
        if(sectors_start_no != sectors_end_no)
        {
            TRACE("\r\nexflash:计算错误");
        }
        TRACE("\r\nbuf offset%d",data_offset-data);
        hw_w25x_flash_read(sectors_buf,sectors_start_no*sectors_size,sectors_size);//读出一个扇区的数据 
        memcpy(sectors_buf,data_offset,sectors_end_offset);
        hw_w25_flash_erase_sector(sectors_start_no*sectors_size); 
        hw_w25x_flash_write(sectors_buf,sectors_start_no*sectors_size,sectors_size); 
    }   
       
    return 0;
}
/*
 *功能：读外部FLASH
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
unsigned int exflash_read(unsigned int addr,unsigned int data_len,unsigned char* data)
{
    hw_w25x_flash_read(data,addr,data_len); 

    return 0;
}







