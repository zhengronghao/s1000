/*
 * =====================================================================================
 *
 *       Filename:  ctc_emv.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/11/2016 10:06:14 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */
#ifndef __CTC_EMV_H__
#define __CTC_EMV_H__ 

#include "ParseCmd.h"
#include "../emv/emv.h"

/*应用选择数据结构
 *SelType:
 *[0] - 先按支付系统环境(PSE) 建立候选列表，如果失败但可以转向AID选择，再按照AID列表方式建立候选列表。建议使用该值进行设置..
 *[1] - 通过PSE方式建立候选列表；
 *[2] - 通过AID列表方式建立候选列表。
 *
 *长度：8
 */
typedef struct APP_SELECT_DATA_STRUCT
{
    unsigned char SelType;   //应用选择方式
    DCardSt CardSt;          //卡座属性
}ctc_emv_asd;

//最后确认应用数据结构
typedef struct APP_FINAL_DATA_STRUCT
{
    unsigned char AidNo;       //最终选择的AID编号
}ctc_emv_afd;

//应用初始化数据结构
typedef struct APP_INIT_DATA_STRUCT
{
    unsigned char AidNo;       //最终选择的AID编号
    AppDt    AppData;          //交易开始所需要的数据
}ctc_emv_aid;

//终端风险管理数据结构
typedef struct TERM_RISK_MANAGE_STRUCT
{
    unsigned char CardType;       //是否为黑卡
    unsigned short PanAmnt;          //当前卡流水记录中最近一次交易金额
}ctc_emv_trm;
//持卡人验证数据结构
typedef struct CARD_HOLDER_VALIDATE
{
    unsigned char Flg;
    unsigned char Cmd;
}ctc_emv_chv;

//开始交易数据结构
typedef struct BEGIN_TRANSACTION
{
    TransProDt TransInfo;
//    AppInitData InitDt;
    AppDt Dt;     
}ctc_emv_bt;

//获取数据结构
typedef struct GET_DTAT
{
    int P1;
    int P2;
}ctc_emv_gd;

//卡号所在文件名 记录号
typedef struct CARD_ON_SFI
{
    int SFI;    //卡号所在的文件名
    int Record; //记录号
}ctc_emv_cos;


uint16_t emv_open(uint32_t mode);

uint16_t emv_close(void);

uint16_t emv_set_data(uint32_t len,uint8_t *data,uint8_t* ret);

uint16_t emv_app_set_data(uint8_t *data,uint8_t* ret);

uint16_t emv_app_select(uint32 data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_app_fianl_select(uint32 data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_app_init(uint32 data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_app_read(uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_app_offline_data_auth(uint32_t mode,uint8_t* ret);

uint16_t emv_trem_ris_manage(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_process_restrict(uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_card_holder_validate(uint32_t data_len,uint8_t *data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);
//uint16_t emv_card_holder_validate(uint32_t mode,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_action_analysis(uint32_t mode,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_online_data_process(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_update_data(uint32_t data_len,uint8_t* data,uint8_t* ret);

uint16_t emv_get_kernel_data(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_get_app_data(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_set_kernel_opt(uint32_t item,uint32_t flg,uint8_t *ret);

uint16_t emv_aid_init_list(uint8_t *ret);

uint16_t emv_aid_delete_list_aid(uint32_t data_len,uint8_t* data,uint8_t* ret);

uint16_t emv_aid_set_list_aid(uint32_t data_len,uint8_t* data,uint8_t mode,uint8_t* ret);

uint16_t emv_capkey_set(uint32_t data_len,uint8_t* data,uint8_t *ret);
//uint16_t emv_capkey_set(uint32_t index,uint32_t data_len,uint8_t* data,uint8_t* ret);

uint16_t emv_capkey_delete(uint32_t index,uint32_t data_len,uint8_t* data,uint8_t* ret);

uint16_t emv_capkey_get_info(uint8_t index,uint8_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t *ret);

uint16_t emv_iccard_get_log(uint32_t mode,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_iccard_get_load_log(uint32_t mode,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_get_kernel_version(uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_expend(uint32_t data_len,uint8_t *data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_get_data(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_begin_transaction(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_read_record_last(uint32_t *rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_get_card_no_sfi(uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_read_card_no_data(uint32_t data_len,uint8_t* data,uint32_t* rev_len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_set_ic_parameter(uint32_t data_len,uint8_t* data,uint8_t* ret);

uint16_t emv_delete_all_capkey(uint8_t* ret);

uint16_t emv_continue_transaction(uint32_t data_len,uint8_t* data,uint32_t* len,uint8_t* buffer,uint8_t* ret);

uint16_t emv_extend_attribute_set(uint32_t data_len,uint8_t* data);

uint16_t emv_extend_attribute_get(uint32_t data_len,uint8_t* data, uint32_t* len,uint8_t* buffer);
#endif /*end __CTC_EMV_H__*/

