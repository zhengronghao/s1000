/*
 * =====================================================================================
 *
 *       Filename:  ctc_tprinter.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/8/2016 9:43:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#include "ctc_tprinter.h"

#include "wp30_ctrl.h"

//
//#define PAPEROUT         0
//#define PARER_JAMS       1
//#define NORMAL           2
//#define OVERHEAT         3
//#define CUTTER           4
//#define BLACK_LABEL      5
//#define THERMAL_HEAD     6
//#define PAPER            7
//

uint8_t gtp_open_flag;

/*
 *功能：打开热敏打印机
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t ctc_tprinter_open(uchar mode)
{
    uint8_t ret;
    if(gtp_open_flag == OFF)
    {
        ret = tprinter_open(mode);
        
        if(ret == RET_OK)
        {
            gtp_open_flag = ON;
            return CMDST_OK;
        }
    }
   // return CMDST_OPEN_DEV_ER;
    return CMDST_OK;
}

/*
 *功能：关闭热敏打印机
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t ctc_tprinter_close(void)
{
    uint8_t ret;
    if(gtp_open_flag == ON)
    {
        ret = tprinter_close();
        if(ret == RET_OK)
        {
            gtp_open_flag = OFF;
            return CMDST_OK;
        }
    }
    return CMDST_CLOSE_DEV_ER;
}
/*
 *功能：热敏打印机行间距设置
 *
 *参数：
 *
 *返回值：
 *
 *注：点行基本单位0.125mm
 *
 */
uint16_t ctc_tprinter_line_set(uint32_t dot_len)
{
    uint8_t ret;
  


    if(gtp_open_flag == ON)
   {
    ret = tprinter_line_set(dot_len);
    if(ret == RET_OK)
    {
        return CMDST_OK;
    }
    else
    {
        return CMDST_OTHER_ER;
    }
   }
   return CMDST_OPEN_DEV_ER;
}

/*
 *功能：热敏打印机浓度设置
 *
 *参数：
 *
 *返回值：
 *
 *注：0-14共15个等级
 *
 */
uint16_t ctc_tprinter_desity_set(uint8_t data)
{
    uint8_t ret;
   if(gtp_open_flag == ON)
   {
       ret = tprinter_desity_set(data);
       if(ret == RET_OK)
       {
           return CMDST_OK;
       }
       else
       {
           return CMDST_OTHER_ER;
       }
   }
   return CMDST_OPEN_DEV_ER;
}

/*
 *功能：热敏打印机走纸
 *
 *参数：
 *
 *返回值：
 *
 *注：单位0.125mm
 *
 */
uint16_t ctc_tprinter_feed_paper(uint32_t dot_len)
{
    uint8_t ret;
    uint16_t res;
    if(gtp_open_flag == ON)
    {
        ret = tprinter_feed_paper(dot_len);
        if(ret == RET_OK)
        {
            return CMDST_OK;
        }
        else
        {
            res = CMD_TPRINTER;
            res = ((res<<8) | ret);
            return res;
        }
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：热敏打印机位图打印
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t ctc_tprinter_bitmap(uint16_t witdth,uint16_t high,uint16_t x_offset,uint16_t y_offset,uint8_t* data)
{
    uint8_t ret;
    uint16_t res = 0;
    
    if(gtp_open_flag == ON)
    {
        ret = tprinter_bitmap(witdth,high,x_offset,y_offset,data);
        if(ret == RET_OK)
        {
            return CMDST_OK;
        }
        else
        {
            res = (CMD_TPRINTER << 8) | ret;
            return res;
        }
    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：获取热敏打印机状态
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint8_t ctc_tprinter_get_status(void)
{
    uint8_t ret;
    if(gtp_open_flag == ON)
    {
        ret = tprinter_get_status();
        return ret;
    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：寻找黑标
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t ctc_tprinter_bkmark_locate(void)
{
    uint8_t ret;
    if(gtp_open_flag == ON)
    {
        ret = tprinter_bkmark_locate();
        if(ret == RET_OK)
        {
            return CMDST_OK;
        }
        else
        {
            return CMDST_OTHER_ER;
        }
    }
    return CMDST_OPEN_DEV_ER;
}



