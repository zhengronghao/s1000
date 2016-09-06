/*
 * =====================================================================================
 *
 *       Filename:  ctc_emv.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/11/2016 10:06:02 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */

#include "ctc_emv.h"
#include "../emv/emv.h"

#include "wp30_ctrl.h"

uint8_t gemv_open_flag;

/*
 *功能：返回值转换
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t ctc_emv_ret(uint8_t res)
{
    uint16_t ret = 0;
    
    ret = CMDST_OK;
//    switch(res)
//    {
//    case EMV_RET_OK:
//        ret = CMDST_OK;
//        break;
//    case EMV_RET_PARA_ERROR:
//        ret = CMDST_PARAM_ER;
//        break;
//    default:
//        ret = CMDST_OTHER_ER;
//        break;
//    }
    return ret;
}

/*
 *功能：打开EMV
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
extern unsigned char ST_EMV_OpenEMVKernel(void);
uint16_t emv_open(uint32_t mode)
{
   TRACE("\r\nemv_open");
    uint8_t ret;
    if(gemv_open_flag == OFF)
    {
        ret = ST_EMV_OpenEMVKernel();
        if(ret != EMV_RET_OK)
        {
            if(ret == EMV_RET_PARA_ERROR)
            {
                return CMDST_PARAM_ER;
            }
            else if(ret == EMV_RET_OTHER_ERROR)
            {
                return CMDST_OTHER_ER;
            }
        }
        else
        {
            gemv_open_flag = ON;
            return CMDST_OK; 
        }
    }
//    return CMDST_OPEN_DEV_ER;
    return CMDST_OK;
}    

/*
 *功能：关闭EMV
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
extern unsigned char ST_EMV_CloseEMVKernel(void);
uint16_t emv_close(void)
{
   TRACE("\r\nemv_close");
    uint8_t ret;
    if(gemv_open_flag == ON)
    {
        ret = ST_EMV_CloseEMVKernel();
        if(ret != EMV_RET_OK)
        {
            if(ret == EMV_RET_PARA_ERROR)
            {
                return CMDST_PARAM_ER;
            }
           else if(ret == EMV_RET_OTHER_ERROR)
            {
                return CMDST_OTHER_ER;
            }
        }
        else
        {
            gemv_open_flag = OFF;
            return CMDST_OK; 
        }
    }
    //return CMDST_CLOSE_DEV_ER;
    return CMDST_OK; 
}    

/*
 *功能：设置固定数据元
 *
 *参数：数据元长度   数据元结构   返回值
 *
 *返回值：
 *
 *注：更新EMV内核模块中的数据元
 *
 */
uint16_t emv_set_data(uint32_t len,uint8_t *data,uint8_t* ret)
{
   TRACE("\r\nemv_set_data 设置固定数据源");

   if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_InitTmFixData((TmFixData *)data);
        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：设置固定应用相关数据元
 *
 *参数：
 *
 *返回值：
 *
 *注：初始化EMV L2内核模块中与应用相关的数据元，以备交易使用。
 *
 */
uint16_t emv_app_set_data(uint8_t *data,uint8_t* ret)
{
    TRACE("\r\nemv_app_set_data 设置固定应用相关数据元");
   
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_InitiateEMV((AppInitData *)data);
        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：开始交易
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_begin_transaction(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_begin_transaction");
  ctc_emv_bt *bt;
  if(data_len != sizeof(ctc_emv_bt))
  {
      return CMDST_PARAM_ER;
  }
  bt = (ctc_emv_bt*)data;
  TRACE_BUF("begin data:",data,sizeof(ctc_emv_bt));
  *ret = ST_EMV_BeginTransaction(&(bt->TransInfo),&(bt->Dt));
  *rev_len = sizeof(TransProDt); 
  
  memcpy(buffer,(char*)&(bt->TransInfo),*rev_len);
  return ctc_emv_ret(*ret);
}
/*
 *功能：应用选择
 *
 *参数：
 *
 *返回值：
 *
 *注：应用选择
 *
 */
uint16_t emv_app_select(uint32 data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_app_select");
    ctc_emv_asd *app_s;

    if(data_len != sizeof(ctc_emv_asd))
    {
        return CMDST_PARAM_ER;
    }

    app_s = (ctc_emv_asd*)data;

    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_AppSelection(&(app_s->CardSt),app_s->SelType,buffer,(unsigned short* )rev_len);
        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;


}
/*
 *功能：选择最后确认的应用
 *
 *参数：
 *
 *返回值：
 *
 *注：数据返回 数据长度1+数据内容1+数据长度2+数据内容2
 *
 */
uint16_t emv_app_fianl_select(uint32 data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_app_fianl_select");
    ctc_emv_afd *app_f;
    uint16_t len1,len2;
    uint8_t data1[256],data2[256];

    if(data_len != sizeof(ctc_emv_afd))
    {
        return CMDST_PARAM_ER;
    }

    app_f =(ctc_emv_afd *)data;
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_FinalSelect(app_f->AidNo,data1,&len1,data2,&len2);
        data[0] = (uint8_t)(len1 & 0xff);
        data[1] = (uint8_t)((len1>>8) & 0xff);
        memcpy(data+2,data1,len1);
        data[len1] = (uint8_t)(len2 & 0xff);
        data[len1 + 1] = (uint8_t)((len2>>8) & 0xff);
        memcpy(data+2+len1+2,data2,len2);
        *rev_len = 2 + len1 + 2 + len2;

        return ctc_emv_ret(*ret);

    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：初始化应用
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_app_init(uint32 data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_app_init 初始化应用");
    ctc_emv_aid *app_i;

    if(data_len != sizeof(ctc_emv_aid))
    {
        return CMDST_PARAM_ER;
    }

    app_i = (ctc_emv_aid *)data;

    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_GetProcOption(app_i->AidNo,&(app_i->AppData),buffer,(unsigned short*)rev_len);
        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：读取应用数据
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_app_read(uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_app_read 读取应用数据");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_ReadAppData(buffer,(unsigned short*)rev_len);
        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：拖机数据认证
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_app_offline_data_auth(uint32_t mode,uint8_t* ret)
{
   TRACE("\r\nemv_app_offline_data_auth 脱机数据认证");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_OfflineDataAuth();
        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：终端风险管理
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_trem_ris_manage(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_trem_ris_mange 终端风险管理");
    ctc_emv_trm *trm;
    if(data_len != sizeof(ctc_emv_trm))
    {
        return CMDST_PARAM_ER;
    }

    trm = (ctc_emv_trm*)data;

    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_TermRiskManage(trm->CardType,trm->PanAmnt,buffer);
        *rev_len = 1;
        return ctc_emv_ret(*ret);

    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：处理限制功能
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_process_restrict(uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_process_restrict 处理限制功能");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_ProcessRestrict(buffer);
        *rev_len = 1;

        return ctc_emv_ret(*ret);

    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：持卡人验证
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_card_holder_validate(uint32_t data_len,uint8_t *data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_card_holder_validate 持卡人验证");
    ctc_emv_chv *chv;
    chv = (ctc_emv_chv *)data;
//    if(data_len != sizeof(ctc_emv_chv))
//    {
//        return CMDST_PARAM_ER; 
//    }
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_NewCardHolderValidate(chv->Flg,chv->Cmd,buffer,(unsigned short *)rev_len);

        return ctc_emv_ret(*ret);

    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：终端行为分析和卡片行为分析
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_action_analysis(uint32_t mode,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_action_analysis 终端行为分析");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_ActionAnalysis((uint8_t) mode,(TransDt *)buffer);
        *rev_len = sizeof(TransDt);
        return ctc_emv_ret(*ret);

    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：联机处理
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_online_data_process(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_online_data_process 联机处理");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_OnlineDataProcess(data,(unsigned short)data_len,(TransDt *)buffer);
        *rev_len = sizeof(TransDt);

        return ctc_emv_ret(*ret);

    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：更新内核数据元
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_update_data(uint32_t data_len,uint8_t* data,uint8_t* ret)
{
   TRACE("\r\nemv_update_data 更新内核数据元");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_SetData(data,(unsigned short)data_len);

        return ctc_emv_ret(*ret);

    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：获取模块内核数据
 *
 *参数：
 *
 *返回值：
 *
 *注：获取EMV内核的数据元列表
 *
 */
uint16_t emv_get_kernel_data(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_get_kernel_data");
    if(gemv_open_flag == ON)
    {
      *ret = ST_EMV_GetTLVList(data,(unsigned short)data_len,buffer,(int *)rev_len);

        return ctc_emv_ret(*ret);

    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：获取内核模块中的应用数据
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_get_app_data(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_get_app_data");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_GetAppData(data,(unsigned char)data_len,buffer,(unsigned short *)rev_len);

        return ctc_emv_ret(*ret);

    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：设置内核类型
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_set_kernel_opt(uint32_t item,uint32_t flg,uint8_t *ret)
{
   TRACE("\r\nemv_set_kernel_opt");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_SetKernOpt((unsigned char)item,(unsigned char)flg);

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：初始化AID列表
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
extern void ST_EMV_DelAllICPara(void);
uint16_t emv_aid_init_list(uint8_t *ret)
{
   TRACE("\r\nemv_aid_inti_list 初始化AID列表");
    if(gemv_open_flag == ON)
    {
        *ret = 0; 
//        ST_EMV_DelallTermAIDList();
        ST_EMV_DelAllICPara();
        return CMDST_OK;
    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：删除指定AID数据
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_aid_delete_list_aid(uint32_t data_len,uint8_t* data,uint8_t* ret)
{
   TRACE("\r\nemv_aid_delete_list_aid 删除指定AID数据");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_DelAidListAid(data,(unsigned char)data_len);

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：设置AID列表数据
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_aid_set_list_aid(uint32_t data_len,uint8_t* data,uint8_t mode,uint8_t* ret)
{
   TRACE("\r\nemv_aid_set_list_aid 设置AID列表数据");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_SetAidListItem(data,(unsigned char)data_len,mode);

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}


/*
 *功能：导入中心公钥
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_capkey_set(uint32_t data_len,uint8_t* data,uint8_t* ret)
{
    TRACE("\r\nemv_capkey_set");
//   if(data_len != sizeof(PKFILESTRU))
//   {
//       TRACE("\r\nPKFILESTRU:%d",sizeof(PKFILESTRU));
//       TRACE("\r\ncapkey len:%d",data_len);
//       return CMDST_PARAM_ER;
//   }
    TRACE_BUF("\r\ncap_key:",data,data_len);

    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_SetCAPKey((PKFILESTRU *)data);

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：删除认证中心公钥
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_capkey_delete(uint32_t index,uint32_t data_len,uint8_t* data,uint8_t* ret)
{
   TRACE("\r\nemv_capkey_delete");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_DelCAPKeyByRidIndex(data,(unsigned char)index);

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：获取公钥信息
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_capkey_get_info(uint8_t index,uint8_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t *ret)
{
   TRACE("\r\nemv_capkey_get_info");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_GetCAPKeyByRidIndex(data,index,(PKFILESTRU *)buffer);
        *rev_len = sizeof(PKFILESTRU); 

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}
/*
 *功能：获取IC卡消费日志
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_iccard_get_log(uint32_t mode,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_iccard_get_log");
    // uint8_t num;
    if(gemv_open_flag == ON)
    {
        // *ret = (*,buffer);
        *rev_len = sizeof(PKFILESTRU); 

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：获取IC卡圈存日志
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_iccard_get_load_log(uint32_t mode,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_iccard_get_load_log");
    // uint8_t num;
    if(gemv_open_flag == ON)
    {
        // *ret = (*data,buffer);
        *rev_len = sizeof(PKFILESTRU); 

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：获取内核版本信息
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_get_kernel_version(uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{

   TRACE("\r\nemv_get_kernel_version");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_GetVersionInfo((char *)buffer,(int *)rev_len);

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：获取数据
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_get_data(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_get_data 获取数据");
    ctc_emv_gd *gd;
    
//    if(data_len != sizeof(ctc_emv_bt))
//    {
//        return CMDST_PARAM_ER;
//    }
    gd = (ctc_emv_gd*)data;

    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_ICCommand_GetData(gd->P1,gd->P2,buffer,(int*)rev_len);

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;

}

/*
 *功能：读最后一条记录
 *
 *参数：
 *
 *返回值：
 *
 *注：读取最后一条记录，用于闪卡处理
 *
 */
uint16_t emv_read_record_last(uint32_t *rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_read_record_last");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_ReadLastRecord();
        *rev_len = 0;
        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：获取卡号或卡号所在记录文件名和记录号
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_get_card_no_sfi(uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_get_card_no_sfi 获取卡号或者记录号");
    ctc_emv_cos cos;

    if(gemv_open_flag == ON)
    {
       *ret = ST_EMV_GetCardnoSfi(&cos.SFI,&cos.Record);
        memcpy(buffer,(uint8_t*)&cos,sizeof(ctc_emv_cos));
        *rev_len = sizeof(ctc_emv_cos);
        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;

}

/*
 *功能：读取卡号或二磁道数据所在的记录数据
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_read_card_no_data(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_read_card_no_data");
    ctc_emv_cos *cos;

    cos = (ctc_emv_cos*)data;

    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_ReadCardnoData(cos->SFI, cos->Record,(char *)buffer,(int*)rev_len);

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;

}
/*
 *功能：EMV其他扩展指令
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_expend(uint32_t data_len,uint8_t *data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_expend");
    
   if(gemv_open_flag == ON)
    {
        *ret = 0;

        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：设置EMV的IC卡参数
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
extern unsigned char ST_EMV_SetICPara(EMVPARA *pParaStru);
uint16_t emv_set_ic_parameter(uint32_t data_len,uint8_t* data,uint8_t* ret)
{
   TRACE("\r\nemv_set_ic_parameter");
   
   if(gemv_open_flag == ON)
    {
        if(data_len != sizeof(EMVPARA))
        {
            return CMDST_PARAM_ER;
        }

        *ret = ST_EMV_SetICPara((EMVPARA*)data);
        
        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：删除EMV内核中所有的公钥
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_delete_all_capkey(uint8_t* ret)
{
   TRACE("\r\nemv_delete_all_capkey");
    if(gemv_open_flag == ON)
    {
        *ret = ST_EMV_DelAllCAPKey();
        
        return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：继续交易
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_continue_transaction(uint32_t data_len,uint8_t* data,uint32_t* len,uint8_t* buffer,uint8_t* ret)
{
   TRACE("\r\nemv_continue_transaction");
    if(gemv_open_flag == ON)
    {
        if(data_len != sizeof(PINSTATUS))
        {
            return CMDST_PARAM_ER;
        }

        
        *ret = ST_EMV_ContinueTransaction((PINSTATUS*) data);
      
      *len = sizeof(PINSTATUS);
      memcpy(buffer,data,sizeof(PINSTATUS));
      
      return ctc_emv_ret(*ret);
    }
    return CMDST_OPEN_DEV_ER;
}

/*
 *功能：设置emv 扩展属性，如设置emv密码弹窗时是否需要按键伴音，是否需要显示密码窗格
 *:
 *参数：
 *
 *返回值：
 *
 *注：目前只支持type=0即密码窗体属性设置
 *
 */
static char s_emv_pinpad_attribute=0;
uint16_t emv_extend_attribute_set(uint32_t data_len,uint8_t* data)
{
    char type;
    type = data[0];
    // 表示为密码弹窗设置
    switch ( type )
    {
    case 0 :
        if ( data_len < 2 ) {
            return CMDST_PARAM_ER;
        }
        // 低两位有效
        s_emv_pinpad_attribute = data[1] & 0x03;
        break;
    default :
        return CMDST_PARAM_ER;
    }
    return CMDST_OK;
} 

/*
 *功能：和emv_extend_attribute_set对应
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
uint16_t emv_extend_attribute_get(uint32_t data_len,uint8_t* data, uint32_t* len,uint8_t* buffer)
{
    char type;
    type = data[0];
    // 表示为密码弹窗设置
    switch ( type )
    {
    case 0 :
        // 低两位有效
        buffer[0] = s_emv_pinpad_attribute & 0x03;
        *len = 1;
        break;
    default :
        return CMDST_PARAM_ER;
    }
    return CMDST_OK;
}

// 提供给emv内核调用函数
// Bit0密码键盘密码显示窗  0-不显示 1-显示 
// Bit1按键伴音  0-无伴音 1-按键伴音
char emv_extend_pinpadwindow_get(void)
{
    return s_emv_pinpad_attribute & 0x03; 
}
/*
 *功能：
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */
/*
 *功能：
 *
 *参数：
 *
 *返回值：
 *
 *注：
 *
 */

