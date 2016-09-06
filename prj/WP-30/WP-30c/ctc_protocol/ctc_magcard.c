/*
 * =====================================================================================
 *
 *       Filename:  ctc_magcard.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/7/2016 3:01:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */


#include "ctc_magcard.h"

#include "wp30_ctrl.h"

uint8_t gmag_open_flag;
uint8_t gtrack1_flag;
uint8_t gtrack2_flag;
uint8_t gtrack3_flag;


/*
 *功能：打开磁条卡模块
 *
 *参数：无
 *
 *返回值：ERT_OK
 *
 * 注：mag_open()仅进行一些寄存器配置，函数只返回成功
 */
uint16_t magcard_open(uint32_t mode)
{
    if(gmag_open_flag == OFF)
    {
        gtrack1_flag = (mode & 1<<0);
        gtrack2_flag = (mode & 1<<1);
        gtrack3_flag = (mode & 1<<2);

        gmag_open_flag = ON;
        return CMDST_OK;
    }
//    return CMDST_OPEN_DEV_ER; 
    return CMDST_OK; 
}

/*
 *功能：关闭磁卡模块
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t magcard_close(void)
{
    if(gmag_open_flag == ON)
    {
        gmag_open_flag = OFF;
        gSystem.lpwr.bm.mtk = 0; //低功耗配置
        return CMDST_OK;
    }
    return CMDST_CLOSE_DEV_ER;
}

/*
 *功能：读磁条卡
 *
 *参数：无
 *
 *返回值：无
 *
 *注：
 *
 */
uint16_t magcard_credit_card(uint16_t mode)
{
    if(gmag_open_flag == ON)
    {
        mag_open();
        gSystem.lpwr.bm.mtk = 1; //低功耗配置
        return CMDST_OK;
    }
    return CMDST_OPEN_DEV_ER;

}
/*
 *功能：取消磁条卡
 *
 *参数：无
 *
 *返回值：无
 *
 *注：
 *
 */
uint16_t magcard_cancle_card(void)
{
     if(gmag_open_flag == ON)
    {
        mag_close();
        return CMDST_OK;
    }
    return CMDST_OPEN_DEV_ER;

}



