/*
 * =====================================================================================
 *
 *       Filename:  ctc_magcard.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/27/2016 3:01:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  none (), none@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */


#include "ctc_pinpad.h"
#include "wp30_ctrl.h"

static uint8_t gpinpad_open_flag = OFF;   // 按键板是否打开标志

uint16_t ctc_pinpad_switch_respond(int ret)
{
    uint16_t iRet = 0;
    switch (ret)
    {
    case RTV_PED_SUCCE:
        iRet = CMDST_OK ;
        break;
    case RTV_PED_ERR :
        iRet = (CMD_PED<<8) | RTV_PED_ERR ;
        break;
    case RTV_PED_ERR_CMD :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_CMD ;
        break;
    case RTV_PED_ERR_PARA :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_PARA ;
        break;
        //        return CMDST_PARAM_ER;
    case RTV_PED_ERR_MACK :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_MACK ;
        break;
    case RTV_PED_ERR_ALGR :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_ALGR ;
        break;
    case RTV_PED_ERR_USAGE_FAULT :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_USAGE_FAULT ;
        break;
    case RTV_PED_ERR_KEYLEN :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_KEYLEN ;
        break;
    case RTV_PED_ERR_KEYTYPE :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_KEYTYPE ;
        break;
    case RTV_PED_ERR_KEYINDEX :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_KEYINDEX ;
        break;
    case RTV_PED_ERR_KEYNULL :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_KEYNULL ;
        break;
    case RTV_PED_ERR_KEYFLASH :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_KEYFLASH ;
        break;
    case RTV_PED_ERR_KEYCHECK :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_KEYCHECK ;
        break;
    case RTV_PED_ERR_INPUT_NONE :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_INPUT_NONE ;
        break;
    case RTV_PED_ERR_INPUT_CANCEL :
        iRet = CMDST_CANCEL_ER ;
        break;
    case RTV_PED_ERR_INPUT_TIMEOUT :
        iRet = (CMD_PED<<8) | RTV_PED_ERR_INPUT_TIMEOUT ;
        break;
    default :
        iRet = CMDST_OTHER_ER; 
        break;
    }
    return iRet;
}

uint16_t ctc_pinpad_open(uint32_t mode)
{
    int iRet;
    if ( gpinpad_open_flag == OFF ) {
         gpinpad_open_flag = ON;
         iRet = ped_open(mode);
         if ( 0 == iRet ) {
            return CMDST_OK;
         }
         return CMDST_OPEN_DEV_ER;
    }
    return CMDST_OK; 
}

uint16_t ctc_pinpad_close(uint32_t mode)
{
    int iRet;
    if ( gpinpad_open_flag == ON ) {
         gpinpad_open_flag = OFF;
         iRet = ped_close(mode);
         if ( 0 == iRet ) {
            return CMDST_OK;
         }
         return CMDST_CLOSE_DEV_ER;
    }
    return CMDST_OK; 

}

uint16_t ctc_pinpad_load_mk(uint32_t len, uint8_t *input)
{
    int iRet;
    if ( gpinpad_open_flag == ON ) {
        iRet = ped_load_mk(len,input);
//        if ( iRet ) {
//           TRACE("\r\nped_load_mk err:%d", iRet);
//        }
        return ctc_pinpad_switch_respond(iRet);
    }
    return CMDST_OPEN_DEV_ER;
}

uint16_t ctc_pinpad_load_wk(uint32_t len, uint8_t *input)
{
    int iRet;
    if ( gpinpad_open_flag == ON ) {
        iRet = ped_load_wk(len,input);
//        if ( iRet ) {
//            TRACE("\r\nped_load_wk err:%d", iRet);
//        }
        return ctc_pinpad_switch_respond(iRet);
    }
    return CMDST_OPEN_DEV_ER;
}

uint16_t ctc_pinpad_get_rand(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output)
{
    int iRet;
    if ( gpinpad_open_flag == ON ) {
        iRet = ped_get_rand(inlen, input, outlen, output);
        return ctc_pinpad_switch_respond(iRet);
    }
    return CMDST_OPEN_DEV_ER;
}

uint16_t ctc_pinpad_get_pin_online(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output)
{
    int iRet;
    if ( gpinpad_open_flag == ON ) {
        iRet = ped_get_pin_online(inlen, input, outlen, output);
        return ctc_pinpad_switch_respond(iRet);
    }
    return CMDST_OPEN_DEV_ER;
}

uint16_t ctc_pinpad_get_pin_offline(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output)
{
    int iRet;
    if ( gpinpad_open_flag == ON ) {
        iRet = ped_get_pin_offline(inlen, input, outlen, output);
        return ctc_pinpad_switch_respond(iRet);
    }
    return CMDST_OPEN_DEV_ER;
}

uint16_t ctc_pinpad_get_encrpt_data(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output)
{
    int iRet;
    if ( gpinpad_open_flag == ON ) {
        iRet = ped_get_encrpt_data(inlen, input, outlen, output);
        return ctc_pinpad_switch_respond(iRet);
    }
    return CMDST_OPEN_DEV_ER;
}

uint16_t ctc_pinpad_get_mac(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output)
{
    int iRet;
    if ( gpinpad_open_flag == ON ) {
        iRet = ped_get_mac(inlen, input, outlen, output);
        return ctc_pinpad_switch_respond(iRet);
    }
    return CMDST_OPEN_DEV_ER;
}

uint16_t ctc_pinpad_select_key(uint32_t len, uint8_t *input)
{
    int iRet;
    if ( gpinpad_open_flag == ON ) {
        iRet = ped_select_key(len, input);
        return ctc_pinpad_switch_respond(iRet);
    }
    return CMDST_OPEN_DEV_ER;
}

uint16_t ctc_pinpad_extern_main(uint32_t inlen, uint8_t *input, uint16_t* outlen, uint8_t *output)
{
    int iRet;
    if ( gpinpad_open_flag == ON ) {
        iRet = ped_extern_main(inlen, input, outlen, output);
        return ctc_pinpad_switch_respond(iRet);
    }
    return CMDST_OPEN_DEV_ER;
}

