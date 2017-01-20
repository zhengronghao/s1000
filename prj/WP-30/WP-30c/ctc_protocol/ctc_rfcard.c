/*
 * =====================================================================================
 *
 *       Filename:  ctc_rfcard.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/7/2016 7:50:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#include "ctc_rfcard.h"

#include "wp30_ctrl.h"

uint8_t grfcard_type;
uint8_t grfcard_open_flag;   //射频卡模块是否打开标志
uint gpoll_type;  //寻卡获取到的卡类型
/*
 *功能：打开射频卡模块
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
extern void power_5v_open(void);
uint16_t rfcard_open(uint32_t mode)
{
    TRACE("\r\nrfcard_open");    
    if(grfcard_open_flag == OFF)
    {
        grfcard_open_flag = ON;
        return CMDST_OK;
    }
   // return CMDST_OPEN_DEV_ER;
    return CMDST_OK;
}

/*
 *功能：关闭射频卡模块
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
extern void power_5v_close(void);
uint16_t rfcard_close(void)
{
    TRACE("\r\nrfcard_close");    
    if(grfcard_open_flag == ON)
    {
        grfcard_open_flag = OFF;
        return CMDST_OK;
    }
    //return CMDST_CLOSE_DEV_ER;
    return CMDST_OK;
}

/*
 *功能：射频卡模块上电
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t rfcard_module_open(void)
{
    int32_t ret;
    
    TRACE("\r\n rfcard module open");    
    if(grfcard_open_flag == ON )
    {
        power_5v_open();
        ret = rfid_open(grfcard_type);
        
        if(ret != RFID_SUCCESS)
        {
            if(ret == -RFID_ERRPARAM)
            {
                return CMDST_PARAM_ER;
            }
            if(ret == -RFID_TIMEOUT)
            {
                return CMDST_TIMEOUT;
            }
            return CMDST_OTHER_ER;
        }
        gSystem.lpwr.bm.rfc = 1;
        return CMDST_OK;
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：射频模块下电
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t rfcard_module_close(void)
{
//    int32_t ret;

    TRACE("\r\nrfcard module close");    
    if(grfcard_open_flag == ON)
    {
//        ret = rfid_powerdown();
//        if(ret != RFID_SUCCESS)
//        {
//            if(ret == -RFID_ERRPARAM)
//            {
//                return CMDST_PARAM_ER;
//            }
//            if(ret == -RFID_TIMEOUT)
//            {
//                return CMDST_TIMEOUT;
//            }
//            return CMDST_OTHER_ER;
//        }
        rfid_close();
        power_5v_close();
        gSystem.lpwr.bm.rfc = 0;

        return CMDST_OK;
    }
    return CMDST_CLOSE_DEV_ER;
}

/*
 *功能：射频卡类型配置
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t rfcard_type_select(uint8_t type)
{
    switch(type)
    {
    case 0x20:
        grfcard_type = EM_mifs_TYPEA;
        break;
    case 0x23:
        grfcard_type = EM_mifs_TYPEB;
        break;
    case 0x00:
        grfcard_type = EM_mifs_NULL;
        break;
    default:
        return CMDST_IC_SELECT_ER;
    }
    return CMDST_OK;
}

/*
 *功能：射频卡寻卡+卡上电
 *
 *参数：
 *
 *返回值：
 *
 *注：寻卡方式默认为0，返回数据为一个字节
 *
 */
uint16_t rfcard_poll_powerup(uint32_t* len, uint8_t* buffer)
{
  int32_t ret;
  int8_t mode = 0;//寻卡方式
  uint32_t cardtype;
  if(grfcard_open_flag == ON)
  {

    TRACE("\r\nrfcard_poll + power up");    

      *len = 0;
      mode = RFID_MODE_EMV;  
//      mode = RFID_MODE_ISO;  
      
      rfid_powerdown();
    
      ret = rfid_poll(mode,&cardtype); 
    
      if(ret != RFID_SUCCESS)
      {
          if(ret == -RFID_ERRPARAM)
          {
              return CMDST_PARAM_ER;
          }else if (ret == -RFID_MULTIERR)
          {
              return CMDST_RF_MULCARD_ER;
          }
          return CMDST_RF_POLL_ER;
      }
     
      ret = rfid_powerup(cardtype,len,buffer);

      if(ret != RFID_SUCCESS)
      {
          if(ret == -RFID_ERRPARAM)
          {
              return CMDST_PARAM_ER;
          }else if (ret == -RFID_MULTIERR)
          {
              return CMDST_RF_MULCARD_ER;
          }
          return CMDST_RF_POLL_ER;
      }
      buffer[*len] = (uint8_t)cardtype;
      *len++;
      return CMDST_OK;
  }
  return CMDST_OPEN_DEV_ER;
}
/*
 *功能：射频卡apdu交互
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t rfcard_exchange_apdu(uint8_t sed_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer)
{
    int32_t ret;
    if(grfcard_open_flag == ON)
    {
    TRACE("\r\nrfcard_apdu");    
        ret = rfid_exchangedata(sed_len,data,rev_len,buffer);
        
        if(ret != RFID_SUCCESS)
        {
            TRACE("get the rifd ret:%x\r\n", ret);
            if(ret == -RFID_ERRPARAM)
            {
                return CMDST_PARAM_ER;
            }
            return CMDST_RF_APDU_ER; 
        }
        return RET_OK;
    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：射频卡寻卡
 *
 *参数：
 *
 *返回值：
 *
 *注：寻卡方式默认为0，返回数据为一个字节
 *
 */
uint16_t rfcard_poll(void)
{
  int32_t ret;
  int8_t mode = 0;//寻卡方式
  if(grfcard_open_flag == ON)
  {
      TRACE("\r\nrfcard_poll");    
     
      mode = RFID_MODE_EMV;  
//      mode = RFID_MODE_ISO;  
    
      ret = rfid_poll(mode,&gpoll_type); 
    
      if(ret != RFID_SUCCESS)
      {
          if(ret == -RFID_ERRPARAM)
          {
              return CMDST_PARAM_ER;
          }else if (ret == -RFID_MULTIERR)
          {
              return CMDST_RF_MULCARD_ER;
          }
          return CMDST_RF_POLL_ER;
      }
     
      return CMDST_OK;
  }
  return CMDST_OPEN_DEV_ER;
}
/*
 *功能：射频卡上电
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t rfcard_card_open(uint32_t* len, uint8_t* buffer)
{
    int32_t ret;
    
    TRACE("\r\nrfcard_open");    
    if(grfcard_open_flag == ON )
    {
        ret = rfid_powerup(gpoll_type,len,buffer);

      if(ret != RFID_SUCCESS)
      {
          if(ret == -RFID_ERRPARAM)
          {
              return CMDST_PARAM_ER;
          }else if (ret == -RFID_MULTIERR)
          {
              return CMDST_RF_MULCARD_ER;
          }
          return CMDST_RF_POLL_ER;
      }
      buffer[*len] = (uint8_t)gpoll_type;
      *len++;
      return CMDST_OK;
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：射频卡下电
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t rfcard_card_close(void)
{
    int32_t ret;

    TRACE("\r\nrfcard_close");    
    if(grfcard_open_flag == ON)
    {
        ret = rfid_powerdown();
        if(ret != RFID_SUCCESS)
        {
            if(ret == -RFID_ERRPARAM)
            {
                return CMDST_PARAM_ER;
            }
            if(ret == -RFID_TIMEOUT)
            {
                return CMDST_TIMEOUT;
            }
            return CMDST_OTHER_ER;
        }
        
        return CMDST_OK;
    }
    return CMDST_CLOSE_DEV_ER;
}


/* 
 * rfcard_mifare - [GENERIC] mifare卡操作
 * @ 
 */
uint16_t rfcard_mifare (uint8_t sed_len,uint8_t* data,uint32_t *rev_len,uint8_t* buffer)
{
    int32_t ret,block=0;
    *rev_len = 0;
    if(grfcard_open_flag != ON)
    {
        return CMDST_OPEN_DEV_ER;
    }
    if ( sed_len <= 1 ) {
        return CMDST_UNPACK_LEN_ER;
    }
    TRACE_BUF("mifare",data,sed_len);
    --sed_len;
    switch ( data[0] )
    {
    case 0 :
        // C3 08 00 00 00 00 00/01 FF FF FF FF FF FF
        if ( sed_len != (1+1+6)) {
            return CMDST_UNPACK_LEN_ER;
        }
        ret = rfid_MIFAuth(data[1],data[2],&data[3]);
        break;
    case 1 :
        // C3 08 00 00 01 00 00 
        if ( sed_len != 2) {
            return CMDST_UNPACK_LEN_ER;
        }
        block = COMBINE16(data[2],data[1]);
        ret = rfid_MIFRead(block,buffer);
        if ( !ret ) {
            *rev_len = 16;
        }
        break;
    case 2 :
        // C3 08 00 00 02 00 00 11 22 33 44 55 66 77 88 99 00 AA BB CC DD EE FF   
        if ( sed_len != (2+16)) {
            return CMDST_UNPACK_LEN_ER;
        }
        block = COMBINE16(data[2],data[1]);
        ret = rfid_MIFWrite(block,&data[3]);
        break;
    case 3 :
        // C3 08 00 00 03 01 02 03 04 05
        if ( sed_len != (4+1)) {
            return CMDST_UNPACK_LEN_ER;
        }
        rfid_MIFMakevalue(COMBINE32(data[4],data[3],data[2],data[1]),data[5],buffer);
        *rev_len = 16;
        ret = 0;
        break;
    case 4 :
        // C3 08 00 00 04 00 00 01 00 00 00
        if ( sed_len != (2+4)) {
            return CMDST_UNPACK_LEN_ER;
        }
        block = COMBINE16(data[2],data[1]);
        ret = rfid_MIFIncrement(block,COMBINE32(data[6],data[5],data[4],data[3]));
        break;
    case 5 :
        // C3 08 00 00 05 00 00 01 00 00 00
        if ( sed_len != (2+4)) {
            return CMDST_UNPACK_LEN_ER;
        }
        block = COMBINE16(data[2],data[1]);
        ret = rfid_MIFDecrement(block,COMBINE32(data[6],data[5],data[4],data[3]));
        break;
    case 6 :
        // C3 08 00 00 06 00 00 
        if ( sed_len != 2) {
            return CMDST_UNPACK_LEN_ER;
        }
        block = COMBINE16(data[2],data[1]);
        ret = rfid_MIFRestore(block);
        break;
    case 7 :
        // C3 08 00 00 07 00 00 
        if ( sed_len != 2) {
            return CMDST_UNPACK_LEN_ER;
        }
        block = COMBINE16(data[2],data[1]);
        ret = rfid_MIFTransfer(block);
        break;
    case 8 :
        // C3 08 00 00 08 00 00 
        if ( sed_len != (2+16)) {
            return CMDST_UNPACK_LEN_ER;
        }
        block = COMBINE16(data[2],data[1]);
        ret = rfid_MIFModify(block,(const BLKALTER *)&data[3]);
        break;
    default :
        return CMDST_CMD_ER;
    }
    DISPPOS(ret);
    switch ( ret )
    {
    case 0 :
        return RET_OK;
    case -RFID_ERRPARAM :
        return CMDST_PARAM_ER;
    case -RFID_TIMEOUT :
        return CMDST_TIMEOUT;
    case -RFID_TRANSERR :
        return CMDST_RF_TRANS_ER;
    case -RFID_PROTERR :
        return CMDST_RF_PROT_ER;
    case -RFID_MULTIERR :
        return CMDST_RF_MULTI_ER;
    case -RFID_NOACT :
        return CMDST_RF_NOACT_ER;
    case -RFID_NOAUTH :
        return CMDST_RF_NOAUTH_ER;
    case -RFID_AUTHERR :
        return CMDST_RF_AUTH_ER;
    case -RFID_UNCHANGE :
        return CMDST_RF_UNCHANGE_ER;
    case -RFID_KEYNOTPOWER :
        return CMDST_RF_KEYNOAUTH_ER;
    default :
        return CMDST_OTHER_ER;
    }
}		/* -----  end of function rfcard_mifare  ----- */

