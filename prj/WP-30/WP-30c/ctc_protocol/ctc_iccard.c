/*
 * =====================================================================================
 *
 *       Filename:  ctc_iccard.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/7/2016 3:34:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */

#include "ctc_iccard.h"

#include "wp30_ctrl.h"

typedef struct _iccard_type{
    int32_t slot;
    uint32_t mode;
    uint32_t vol;
    uint32_t open_slot;  //该卡座是否打开
}iccard_type;
  
typedef struct _iccard_number{

    iccard_type type[10];
    uint32_t number;
}ic_type_nu;

ic_type_nu gIccard;

uint8_t CardSlot;
uint8_t CardVol = VCC_5;
uint8_t CardMode = ISOMODE;

/*
 *功能：打开IC卡座
 *
 *参数：data[0]:卡座类型 data[1]:卡类型
 *
 *返回值：
 *
 *注:
 *
 */
uint16_t iccard_slot_open(uint8_t slot)
{
    uint8_t i=0;
    uint8_t mode,vol;
//    int ret = 0;
TRACE("\r\nIccard_slot_open");    
    for(i = 0; i < gIccard.number; i++)
    {
        if(gIccard.type[i].slot == slot)  //该卡座已经存在
        {
            mode = gIccard.type[i].mode;  //获取卡座信息
            vol = gIccard.type[i].vol;
            break;
        }
    }
    if(i == gIccard.number) //该卡座不存在 使用默认值
    {
            CardSlot = slot;
            vol = CardVol ; //更新卡座信息
            mode = CardMode ;
            if(iccard_type_select(slot,vol,mode) != CMDST_OK)
            {
               return CMDST_PARAM_ER;
            }
    }
    if(gIccard.type[i].open_slot == OFF )
    {
//        if(ret != ICC_SUCCESS)
//        {
//            return CMDST_PARAM_ER; 
//        }
//        else
//        {
            gIccard.type[i].open_slot = ON; //该卡座打开
//        }
    }
//    else
//    {
//        return CMDST_OPEN_DEV_ER ;
//    }
    return CMDST_OK;
}
/*
 *功能：设置IC卡类型
 *
 *参数：
 *
 *返回值：
 *
 *注：
 // 卡座号
#define     USERCARD         0x00        // 半埋卡座
#define     SAM1SLOT         0x01        // SAM卡座1
#define     SAM2SLOT         0x02        // SAM卡座2
#define     SAM3SLOT		 0x03		 // SAM卡座3
#define     SAM4SLOT		 0x04		 // SAM卡座4
#define     SAM5SLOT		 0x05		 // SAM卡座5   对应S-970外置读卡器SAM1
#define     SAM6SLOT		 0x06		 // SAM卡座6   对应S-970外置读卡器SAM2
#define     SAM7SLOT		 0x07		 // SAM卡座7   对应S-970外置读卡器SAM3

// ICC卡通信参数
#define     VCC_5            0x05    	//  5V卡
#define     VCC_3            0x03    	//  3V卡
#define     VCC_18           0x01    	//  1.8V卡

#define     ISOMODE          0x00    	// 符合ISO7816规范
#define     EMVMODE          0x01    	// 符合EMV v4.1规范
#define     SHBMODE          0x02    	// 社保规范
#define     CITYUNIONMODE    0x03

协议中的数值定义：
电压选择	1	
0x05：5.0V
0x03：3.3V
0x01：1.8V
卡规范协议	1	
0x00：ISO7816规范
0x01：EMV v4.1规范
0x02：社保规范

卡座：
0x00	半埋卡座
0x01~0x04	SAM1~SAM4卡
0x10	SLExx42卡
0x11	SLExx28卡
0x12	AT24Cxx卡
 
 *
 */
extern int icc_if_cardslot(int *CardSlot);

uint16_t iccard_type_select(uint8_t slot,uint8_t vol,uint8_t mode)
{
   int8_t i=0;
   int type;
   
   type = slot;
   
   if(icc_if_cardslot(&type))
	{
		return CMDST_PARAM_ER;
	}
	if(slot == USERCARD)
	{
        if((vol != VCC_5) && (vol != VCC_3) && (vol != VCC_18))
        {
            return CMDST_PARAM_ER;
        }
	}
	else
	{
		if(vol != VCC_5)
		{
			return CMDST_PARAM_ER;
		}
	}
	
    if (mode  > CITYUNIONMODE )
    {
		return CMDST_PARAM_ER;
    }
   
   for(i= 0; i < gIccard.number; i++)
   {
      if(gIccard.type[i].slot == slot) //已存在该卡座
      {
          gIccard.type[i].mode = mode; //更新卡座信息
          gIccard.type[i].vol = vol;
          break;
      }
   }
   if(i == gIccard.number)  //不存在该卡座
   {
       gIccard.type[i].slot = slot; 
       gIccard.type[i].mode = mode; //更新卡座信息
       gIccard.type[i].vol = vol;
       gIccard.number ++;
       if(gIccard.number > 9)
       {
           gIccard.number = 9;
       }
        
   }

   return CMDST_OK;
}
/*
 *功能：关闭IC卡座
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t iccard_slot_close(uint8_t slot)
{
    uint32_t i;
   
    for(i = 0; i < gIccard.number; i++)
    {
        if(gIccard.type[i].slot == slot)  //该卡座已经存在
        {
            break;
        }
    }
    if(i == gIccard.number) //该卡座不存在
    {
        return CMDST_PARAM_ER; 
    }
    if(gIccard.type[i].open_slot == ON)
    {
        gIccard.type[i].open_slot = OFF;
    }
    else
    {
       return CMDST_CLOSE_DEV_ER;
    }
    return CMDST_OK;
}
/*
 *功能：对指定的IC卡上电
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t iccard_card_open(uint8_t slot,uint32_t* len,uint8_t *buffer)
{
    int ret;
    uint32_t i;
    uint32_t vol,mode;  
    for(i = 0; i < gIccard.number; i++)
    {
        TRACE("\r\nIccard_open");    
        if(gIccard.type[i].slot == slot)  //该卡座已经存在
        {
            vol = gIccard.type[i].vol;
            mode = gIccard.type[i].mode;
            break;
        }
    }
    if(i == gIccard.number) //该卡座不存在
    {
        return CMDST_PARAM_ER; 
    }
    if( gIccard.type[i].open_slot == ON )
    {
        ret = icc_InitModule(slot,vol,mode);
        if(ret != ICC_SUCCESS)
        {
            if(ret == -ICC_ERRPARA)
            {
                return CMDST_PARAM_ER; 
            }
            if(ret == -ICC_TIMEOUT)
            {
                return CMDST_TIMEOUT;
            }
            return CMDST_IC_RESET_ER;
        }

        ret = icc_Reset((int)slot,len,buffer);
        if(ret != ICC_SUCCESS)
        {
            if(ret == -ICC_ERRPARA)
            {
                return CMDST_PARAM_ER; 
            }
            if(ret == -ICC_TIMEOUT)
            {
                return CMDST_TIMEOUT;
            }
            return CMDST_IC_RESET_ER;
        }
        else
        {
            return CMDST_OK;
        }
    }
    return CMDST_OPEN_DEV_ER;

}
/*
 *功能：异步卡交互
 *
 *参数：
 *
 *返回值：
 *
 *注：icc_ExchangeData() 有对卡座、卡上电检测
 *
 */
uint16_t iccard_exchange_apdu(uint8_t slot,uint32_t sed_len,uint8_t* data,uint32_t *rev_len, uint8_t* rev_buf)
{
    int ret;
    
    TRACE("\r\nIccard_apdu");    
    ret = icc_ExchangeData((int)slot,(int)sed_len,data,(int *)rev_len,rev_buf);
    if(ret != ICC_SUCCESS)
    {
        if(ret == -ICC_ERRPARA)
        {
            return CMDST_PARAM_ER; 
        }
        if(ret == -ICC_TIMEOUT)
        {
            return CMDST_TIMEOUT;
        }
        return CMDST_IC_APDU_ER;  
    }
    return CMDST_OK;
}
/*
 *功能：关闭IC卡
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t iccard_card_close(uint8_t slot)
{
    int ret;
     uint32_t i;
   
    for(i = 0; i < gIccard.number; i++)
    {
        if(gIccard.type[i].slot == slot)  //该卡座已经存在
        {
            break;
        }
    }
    if(i == gIccard.number) //该卡座不存在
    {
        return CMDST_PARAM_ER; 
    }
    if( gIccard.type[i].open_slot == ON)
    {
        ret = icc_Close(slot);
        if(ret != ICC_SUCCESS)
        {
            if(ret == -ICC_ERRPARA)
            {
                return CMDST_PARAM_ER; 
            }
            if(ret == -ICC_TIMEOUT)
            {
                return CMDST_TIMEOUT;
            }
            return CMDST_CLOSE_DEV_ER;  
        }
        return CMDST_OK;
    }
//    return CMDST_CLOSE_DEV_ER;  
    return CMDST_OK;
}

/*
 *功能：IC卡在位检测
 *
 *参数：卡座号  在位信息长度 在位信息
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t iccard_card_check(uint8_t slot,uint32_t *len,uint8_t *buffer)
{
    int32_t ret;

    TRACE("\r\nIccard_check");    
    ret = icc_CheckInSlot(slot);
    if(ret == ICC_SUCCESS)
    {
        *len = 1;
        *buffer = 1;
        return CMDST_OK;
    }
    else
    {
        *len = 1;
        *buffer = 0;
        if(ret == -ICC_CARDOUT)
        {
            return CMDST_IC_EXIST_ER;
        }
    }
    return CMDST_PARAM_ER;
}
