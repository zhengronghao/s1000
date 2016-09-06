#include "wp30_ctrl.h"
//#include "hal_uart.h"
#include "ctc_magcard.h"
//#include "nprinter.h"
#include "ctc_iccard.h"
#include "ctc_rfcard.h"
#include "ctc_tprinter.h"
#include "ctc_emv.h"
#include "ctc_sys.h"
#include "ctc_pinpad.h"
#include "ctc_authent.h"

uint32_t ctc_send_frame(MCUPCK res, uint8_t *data) 
{
    uint8_t outbuf[2048];
    uint32_t outlen;
    
    if (data == NULL ) {
        return RET_PARAM;    
    }
    
    packCmd(res, data, outbuf, &outlen);
//    hal_uart_write(BD_COMM_INSTANCE, outbuf, outlen);
    TRACE_BUF("ctc_send_frame\r\n", outbuf, outlen);
    drv_uart_write(CTC_COMPORT, outbuf, outlen);

    return RET_OK;
}

/*
 *功能： 磁卡数据主动上送
 *参数： data:磁道数据  len:磁道数据长度
 *
 *注:
 *
 */
#ifdef CFG_MAGCARD
void ctc_mag_report(uint8_t *data) 
{
    MCUPCK magpck;
    ET_MAGCARD *s_magcard;
    uchar buff[512];
    int i,pos=0;
    memset(buff, 0, sizeof(buff));
    // copy valid value
    for ( i=0 ; i<3 ; i++ ) {
        s_magcard = (ET_MAGCARD *)(data+sizeof(ET_MAGCARD)*i);
        buff[pos] = s_magcard->ucTrkFlag;  
        pos++;
        buff[pos] = s_magcard->ucTrkLength;
        pos++;
        memcpy( buff+pos, s_magcard->aucTrkBuf, s_magcard->ucTrkLength);
        pos += s_magcard->ucTrkLength;
    }

    magpck.len = pos+4;
	magpck.cmdcls = CMD_MAGCARD;
	magpck.cmdop = CMD_MAGCARD_REPORT;
	magpck.sno1 = 0;
	magpck.sno2 = 1;
    ctc_send_frame(magpck, buff);
}

void ctc_magcard_operate(MCUPCK res, uint8_t *outbuf)
{
    //uint8_t ret = 0;
    uint16_t respose;
    uint8_t data_temp[2];
    uint32_t mode;
    switch (res.cmdop) {
        case CMD_MAGCARD_OPEN :
                mode = outbuf[0] | outbuf[1] << 8 | outbuf[2] << 16 | outbuf[3] << 24; 
                respose = magcard_open(mode);
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
                   
        case CMD_MAGCARD_CLOSE :
                respose = magcard_close();
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                  
                break;

        case CMD_MAGCARD_READ :
                mode = outbuf[0] | outbuf[1]<<8;
                respose = magcard_credit_card(mode);
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                   
                break;
              
        case CMD_MAGCARD_CANCEL :
                respose = magcard_cancle_card();
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
        case CMD_MAGCARD_REPORT:

                break;
        default :
                respose = CMDST_CMD_ER;
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
    }
}
#endif

void ctc_nprinter_operate(MCUPCK res, uint8_t *data,  uint32_t len)
{
    uint8_t ret = 0;
    uint16_t respose;
    uint8_t data_temp[2];
    uint8_t status = 0;
    uint8_t buffer[3];   
    
    switch (res.cmdop) {
        case CMD_NEEDLE_OPEN :
//                ret = nprinter_open(data, len);
                if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_OPEN_DEV_ER;
                }
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
                    
        case CMD_NEEDLE_CLOSE :
//                ret = nprinter_close(data, len);
                 if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_CLOSE_DEV_ER;
                }
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);             
                break;

        case CMD_NEEDLE_STATUS :
//                ret = nprinter_get_status(data, len, &status);
                if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_TIMEOUT;
                }
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                buffer[2] = status;
                res.len = MIN_CMD_RESPONSE +1;
                ctc_send_frame(res, buffer);                    
                break;
                    
        case CMD_NEEDLE_WRITE :
//                ret = nprinter_write(data, len);
                if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_PARAM_ER;
                }
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                  
                break;
                    
        default :
                respose = CMDST_CMD_ER;
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
    }  
}

void ctc_tprinter_operate(MCUPCK res, uint8_t *data,  uint32_t len)
{
   // uint8_t ret = 0;
    uint16_t respose;
    uint16_t witdth;
    uint16_t high;
   // uint16_t freeline;
    uint16_t x_offset;
    uint16_t y_offset;
    uint8_t data_temp[2];
    uint8_t status;
    uint8_t buffer[3];   
    unsigned int dot_len;

    switch (res.cmdop) {
        case CMD_TPRINTER_OPEN :
                respose = ctc_tprinter_open(data[0]);
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
                    
        case CMD_TPRINTER_CLOSE :
                respose = ctc_tprinter_close();
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);             
                break;

        case CMD_TPRINTER_LINESET :
                dot_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
                respose = ctc_tprinter_line_set(dot_len);
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                    
                break;
                    
        case CMD_TPRINTER_DESITY :
                respose = ctc_tprinter_desity_set(data[0]);
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                  
                break;

        case CMD_TPRINTER_FEEDPAPER :
                dot_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
                respose = ctc_tprinter_feed_paper(dot_len);
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                  
                break;

        case CMD_TPRINTER_BITMAP :
                witdth = (data[0]) |(data[1] << 8);
                high =   (data[2]) |(data[3] << 8); 
                x_offset = (data[4]) |(data[5] << 8);
                y_offset =   (data[6]) |(data[7] << 8); 
                respose = ctc_tprinter_bitmap(witdth, high, x_offset, y_offset, data+8);
//                TRACE("tp respose:%d", respose);
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                  
                break;

        case CMD_TPRINTER_STATUS :
                status = ctc_tprinter_get_status();
                respose = CMDST_OK;
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                buffer[2] = status;
                res.len = MIN_CMD_RESPONSE +1;
                ctc_send_frame(res, buffer);                      
                break;

        case CMD_TPRINTER_BLACKMARK :
                respose = ctc_tprinter_bkmark_locate();
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                  
                break;  
                    
        default :
                respose = CMDST_CMD_ER;
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
    }
}

void ctc_iccard_operate(MCUPCK res, uint8_t *data,  uint32_t len)
{
   // uint8_t ret = 0;
    uint16_t respose;
    uint8_t data_temp[2];
    unsigned int rev_len = 0;
    unsigned int sed_len = 0;
    uint8_t buffer[1024];
    
    switch (res.cmdop) {
    case CMD_ICCARD_SLOT_OPEN :
        respose = iccard_slot_open(data[0]);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;

    case CMD_ICCARD_SLOT_CLOSE :
        respose = iccard_slot_close(data[0]);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);             
        break;

    case CMD_ICCARD_TYPE :
        respose = iccard_type_select(data[0], data[1], data[2]);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);                 
        break;

    case CMD_ICCARD_APDU :
        sed_len = len; 
        respose = iccard_exchange_apdu(data[0], sed_len-1, data+1, &rev_len, buffer+2);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);                  
        break;

    case CMD_ICCARD_CARD_OPEN :
        respose = iccard_card_open(data[0], &rev_len, buffer+3);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = buffer[rev_len+2];
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);                     
        break;

    case CMD_ICCARD_CARD_CLOSE :
        respose = iccard_card_close(data[0]);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);             
        break;

    case CMD_ICCARD_CHECK :
        respose = iccard_card_check(data[0],&rev_len,&buffer[2]);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;                  

    default :
        respose = CMDST_CMD_ER;
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;
    }
}

void ctc_rfcard_operate(MCUPCK res, uint8_t *data,  uint32_t len)
{
   // uint8_t ret = 0;
    uint16_t respose;
    uint8_t data_temp[2] ={0};   
    unsigned int rev_len;
    uint8_t buffer[1024]={0};  
    unsigned int sed_len;
    uint32_t mode; 
    switch (res.cmdop) {
        case CMD_RFCARD_OPEN :
            mode = data[0]|data[1]<<8|data[2]<<16|data[3]<<24;
            respose = rfcard_open(mode);
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
                    
        case CMD_RFCARD_CLOSE :
                respose = rfcard_close();
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);             
                break;

        case CMD_RFCARD_TYPE :
                respose = rfcard_type_select(data[0]);
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                    
                break;
        case CMD_RFCARD_MODULE_OPEN :
                respose = rfcard_module_open();
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
                    
        case CMD_RFCARD_MODULE_CLOSE :
                respose = rfcard_module_close();
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);             
                break;

        case CMD_RFCARD_POLL_POWER_UP :
                respose = rfcard_poll_powerup(&rev_len, buffer+2);
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE  + rev_len;
                ctc_send_frame(res, buffer);
                break;

        case CMD_RFCARD_APDU :
                sed_len = len; 
                respose = rfcard_exchange_apdu(sed_len, data+0, &rev_len, buffer+2);
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE + rev_len;
                ctc_send_frame(res, buffer);                  
                break;

        case CMD_RFCARD_MIFARE :
                // C3 08 00 00 
                sed_len = len; 
                respose = rfcard_mifare(sed_len, data+0, &rev_len, buffer+2);
                DISPPOS(respose);
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE + rev_len;
                ctc_send_frame(res, buffer);                  
                break;

        case CMD_RFCARD_POLL:
                respose = rfcard_poll();
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE ;
                ctc_send_frame(res, buffer);
                break;                   
 
        case CMD_RFCARD_CARD_OPEN :
                respose = rfcard_card_open(&rev_len, buffer+2);
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE + rev_len;
                ctc_send_frame(res, buffer);
                break;
                    
        case CMD_RFCARD_CARD_CLOSE :
                respose = rfcard_card_close();
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);             
                break;

       default :
                respose = CMDST_CMD_ER;
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
    }
}

extern int sys_write_project_id(uchar *id, int len);
void ctc_sign_operate(MCUPCK res, uint8_t *data,  uint32_t len)
{
    uint8_t ret = 0;
    uint16_t respose;
    uint8_t data_temp[2];
 //   uint8_t status;
    uint8_t buffer[520];   
    uint32_t outlen;
    
    memset(buffer, 0, sizeof(buffer)); 
    switch (res.cmdop) {
        case CMD_SIGN_DOWN_PUBKEY :
//                ret = sign_downpubkey(data, data+PUBKEY_LEN); 
                if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_STORE_FULL;
                }
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
                    
        case CMD_SIGN_GET_PUBKEY_NUM :
//                ret = sign_getpubkeynum(&status);
                 if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_PARAM_ER;
                }
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
     //           buffer[2] = status;
                res.len = MIN_CMD_RESPONSE +1;
                ctc_send_frame(res, buffer);             
                break;

        case CMD_SIGN_CLEAR_PUBKEY :
//                ret = sign_clearpubkey();
                if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_TIMEOUT;
                }
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                    
                break;
                    
        case CMD_SIGN_VERIFY :
//                ret = sign_verify(data, data+ITEP_HASH_LEN, len-ITEP_HASH_LEN);
                if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_PARAM_ER;
                }
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                  
                break;

        case CMD_SIGN_UPDATE_HASH :
//                ret = sign_updatehash(data[0], data+1);
                if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_PARAM_ER;
                }
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                  
                break;

        case CMD_SIGN_PUBKEY_DRENCRY :
                respose = authent_pub_decrpt(data, len, &buffer[2]);
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE + 256;
                ctc_send_frame(res, buffer);                  
                break;

         case CMD_SIGN_VERIFY_IMG :
//                ret = authent_sign_verifyimg(data[0], data+1);
                if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_PARAM_ER;
                }
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);                  
                break;

         case CMD_SIGN_SET_PROJECT_ID :
                if ( len > 36 || len < 4) {
                    TRACE("len err:%dr\r\n", len);
                    respose = CMDST_PARAM_ER;
                }else{
                    ret = (uint8_t)sys_write_project_id((uchar *)(&data[4]), len-4);
                    TRACE_BUF("ID:", &data[4], 32);
                    if (ret == RET_OK) {
                        respose = CMDST_OK;
                    } else {
                        TRACE("get the ret:%d\r\n", ret);
                        respose = CMDST_PARAM_ER;
                    }
                }
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, buffer);
                break;
         case CMD_SIGN_GET_PROJECT_ID :
                ret = (uint8_t)s_read_syszone(SYS_PROJIECT_ID_ADDR, SYS_PROJIECT_ID_SIZE, (uchar *)(&buffer[2]));
                TRACE_BUF("ID", &buffer[2], 32);
                if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_PARAM_ER;
                }
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE+32;
                ctc_send_frame(res, buffer);
                break;
         case CMD_SIGN_GET_GAPKHASH_REQ:
                buffer[2] = authent_get_gapkhash_req();
                respose = 0;
//                // test 
//                buffer[2]=0;
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE+1;
                ctc_send_frame(res, buffer);
                break;
         case CMD_SIGN_GET_GAPKHASH_NUM:
                if ( data[0] < 1 || data[0]> authent_get_gapkhash_req() ) {
                    respose = CMDST_PARAM_ER;
                    res.len = MIN_CMD_RESPONSE;
                }else{
                    respose = authent_get_gapkhash_num(data[0], (uchar *)(&buffer[2]), &outlen);
                }
                buffer[0] = (uint8_t)respose & 0xFF;
                buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE+outlen;
                ctc_send_frame(res, buffer);
                break;
         default :
                respose = CMDST_CMD_ER;
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
    }
}


void ctc_pinpad_operate(MCUPCK res, uint8_t *data, uint32_t len)
{
   // uint8_t ret = 0;
    uint16_t respose;
    uint8_t data_temp[2];
    unsigned short rev_len = 0;
    //unsigned int sed_len = 0;
    uint8_t buffer[1024];
    
    switch (res.cmdop) {
    case CMD_PED_OPEN :
        respose = ctc_pinpad_open(0);
        TRACE("\r\nenter ctc_pinpad_open:%x\r\n", respose);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;

    case CMD_PED_CLOSE :
        respose = ctc_pinpad_close(0);
        TRACE("\r\nenter ctc_pinpad_close:%x\r\n", respose);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);             
        break;

    case CMD_PED_DL_MK :
        respose = ctc_pinpad_load_mk(len, data);
        TRACE("\r\nenter ctc_pinpad_load_mk:%x\r\n", respose);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);                 
        break;

    case CMD_PED_DL_WK :
        respose = ctc_pinpad_load_wk(len, data);
        TRACE("\r\nenter ctc_pinpad_load_wk:%x\r\n", respose);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, data_temp);                  
        break;

    case CMD_PED_GET_RANDOM :
        respose = ctc_pinpad_get_rand(len, data, &rev_len, buffer+2);
        TRACE("\r\nenter ctc_pinpad_get_rand:%x\r\n", respose);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);                     
        break;

    case CMD_PED_ONLINE_PIN :
        // 同步回送数据
        buffer[0] = 0;
        buffer[1] = 0;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, buffer);

        respose = ctc_pinpad_get_pin_online(len, data, &rev_len, buffer+2);
        TRACE("\r\nenter ctc_pinpad_get_pin_online:%x\r\n", respose);

        res.cmdop = CMD_PED_ONLINEPINSTR_RESULT;
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_PED_OFFLINE_PIN :
        // 同步回送数据
        buffer[0] = 0;
        buffer[1] = 0;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, buffer);

        respose = ctc_pinpad_get_pin_offline(len, data, &rev_len, buffer+2);
        TRACE("\r\nenter offline:%x\r\n", respose);

        res.cmdop = CMD_PED_OFFLINEPINSTR_RESULT;
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;                  

    case CMD_PED_ENCRYPT :
        respose = ctc_pinpad_get_encrpt_data(len, data, &rev_len, buffer+2);
        TRACE("\r\n ctc_pinpad_get_encrpt_data:%x\r\n", respose);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_PED_GET_MAC :
        respose = ctc_pinpad_get_mac(len, data,&rev_len,&buffer[2]);
        TRACE("\r\n ctc_pinpad_get_mac:%x\r\n", respose);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_PED_SELE_KEY :
        respose = ctc_pinpad_select_key(len, data);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);             
        break;

    case CMD_PED_AUTHENT :
        respose = 0; 
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_PED_HAND :
//        respose = iccard_card_check(len, data[0],&rev_len,&buffer[2]);
        respose = 0;
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_PED_GET_PINSTR :
//        respose = iccard_card_check(data[0],&rev_len,&buffer[2]);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_PED_NOTIFY_KEY :
//        respose = iccard_card_check(data[0],&rev_len,&buffer[2]);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_PED_DIS_KEYNUM :
//        respose = iccard_card_check(data[0],&rev_len,&buffer[2]);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;
        
    case CMD_PED_PINSTR_END :
//        respose = iccard_card_check(data[0],&rev_len,&buffer[2]);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_PED_EXTERN :
        respose = ctc_pinpad_extern_main(len, data,&rev_len,&buffer[2]);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);             
        break;
//    case CMD_PED_NOTIFY_KEY:
//    case CMD_PED_DIS_KEYNUM:
//    case CMD_PED_PINSTR_END:
    case CMD_PED_ONLINEPINSTR_RESULT:
    case CMD_PED_OFFLINEPINSTR_RESULT:
        break;

    default :
        respose = CMDST_CMD_ER;
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;
    }
}

void ctc_emv_operate(MCUPCK res, uint8_t *data,  uint32_t len)
{
    uint8_t ret = 0;
    uint16_t respose;
    uint8_t data_temp[2];
    unsigned int rev_len = 0;
    //unsigned int sed_len = 0;
    uint8_t buffer[1024];
    uint32_t data_len = 0;
    uint32_t mode;
    uint32_t mode1;


    switch (res.cmdop) {
    case CMD_EMV_OPEN :
        mode = data[0] | data[1] << 8|data[2]<<16|data[3]<<24;
        respose = emv_open(mode);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;
    case CMD_EMV_CLOSE :
        respose = emv_close();
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;

    case CMD_EMV_SET_DATA:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_set_data(data_len,data+4,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_APP_SET_DATA:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_app_set_data(data+4,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1;
        ctc_send_frame(res, buffer);             
        break;
    case CMD_EMV_BEGIN_TRANSACTION:
        respose = CMDST_OK;
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, buffer);             
       
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_begin_transaction(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        res.cmdop = CMD_EMV_BEGIN_TRANSACTION_RESULT ;
        ctc_send_frame(res, buffer);         
        break;
    case CMD_EMV_APP_SELECT:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_app_select(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_APP_FINAL_SELECT:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_app_fianl_select(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_APP_INIT:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_app_init(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_APP_READ:
        respose = emv_app_read(&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1+ 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_OFFLINE_DATA_AUTH:
        mode = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_app_offline_data_auth(mode,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_TREM_RIS_MANAGE:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_trem_ris_manage(data_len,data+4,&rev_len,buffer+3,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_PROCESS_RESTRICT:
        respose = emv_process_restrict(&rev_len,buffer+3,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1 + rev_len;
        ctc_send_frame(res, buffer);             
        break;    

    case CMD_EMV_CARD_HOLDER_VALIDATE:
        data_len  = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_card_holder_validate(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMA_ACTION_ANALYSIS:
        mode = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_action_analysis(mode,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_ONLINE_DATA_PROCESS:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_online_data_process(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_UPDATE_DATA:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_update_data(data_len,data+4,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1;
        ctc_send_frame(res, buffer);             
        break;   

    case CMD_EMV_GET_KERNEL_DATA:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_get_kernel_data(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;   

    case CMD_EMV_GET_APP_DATA:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_get_app_data(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;    

    case CMD_EMV_SET_KERN_OPT:
        mode = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        mode1 = (data[4]) |(data[5]) << 8 |(data[6] << 16) |(data[7] <<24); 
        respose = emv_set_kernel_opt(mode,mode1,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1;
        ctc_send_frame(res, buffer);             
        break;   

    case CMD_EMV_AID_INIT_LIST:
        respose = emv_aid_init_list(&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1;
        ctc_send_frame(res, buffer);             
        break;   

    case CMD_EMV_AID_DEL_LIST_AID:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_aid_delete_list_aid(data_len,data+4,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1;
        ctc_send_frame(res, buffer);             
        break;  

    case CMD_EMV_AID_SET_LIST_AID:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_aid_set_list_aid(data_len,data+4,data[4+data_len],&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1;
        ctc_send_frame(res, buffer);             
        break;  

    case CMD_EMV_CAPKEY_SET:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_capkey_set(data_len,data+4,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1;
        ctc_send_frame(res, buffer);             
        break;   

    case CMD_EMV_CAPKEY_DEL:
        data_len  = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        mode = data[data_len +4]; 
        respose = emv_capkey_delete(mode,data_len,data+4,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1;
        ctc_send_frame(res, buffer);             
        break;  

    case CMD_EMV_CAPKEY_GET_INFO:
        data_len = 5; 
        mode = data[5];
        respose = emv_capkey_get_info(mode,data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_ICCARD_GET_LOG:
        mode = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_iccard_get_log(mode,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_ICCARD_GET_LOAD_LOG:
        mode = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_iccard_get_load_log(mode,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_GET_KERN_VERSION:
        respose = emv_get_kernel_version(&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;
 
    case CMD_EMV_GET_DATA:
        data_len = COMBINE32(data[3],data[2],data[1],data[0]); 
        respose = emv_get_data(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_READ_LAST_RECORD:
        respose = emv_read_record_last(&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_GET_CARD_NO_SFI:
        respose = emv_get_card_no_sfi(&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_READ_CARD_NO_DATA:
        data_len = COMBINE32(data[3],data[2],data[1],data[0]); 
        respose = emv_read_card_no_data(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;


    case CMD_EMV_EXPAND:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_expend(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_DELETE_ALL_CAPKEY:
        respose = emv_delete_all_capkey(&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1 ;
        ctc_send_frame(res, buffer);             
        break;

    case CMD_EMV_SET_IC_PARAMETER:
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_set_ic_parameter(data_len,data+4,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        res.len = MIN_CMD_RESPONSE + 1 ;
        ctc_send_frame(res, buffer);             
        break;
    
    case CMD_EMV_CONTINUE_TRANSACTION:
        respose = CMDST_OK;
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, buffer);             
      
        data_len = (data[0]) |(data[1]) << 8 |(data[2] << 16) |(data[3] <<24); 
        respose = emv_continue_transaction(data_len,data+4,&rev_len,buffer+7,&ret);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        buffer[2] = ret;
        buffer[3] = (uint8_t)(rev_len & 0xFF);
        buffer[4] = (uint8_t)(rev_len>>8 & 0xFF);
        buffer[5] = (uint8_t)(rev_len>>16 & 0xFF);
        buffer[6] = (uint8_t)(rev_len>>24 & 0xFF);
        res.len = MIN_CMD_RESPONSE + 1 + 4 + rev_len;
        res.cmdop = CMD_EMV_CONTINUE_TRANSACTION_RESULT;
        ctc_send_frame(res, buffer);                
        break;

    case CMD_EMV_CONTINUE_TRANSACTION_RESULT:
    case CMD_EMV_BEGIN_TRANSACTION_RESULT:
        break;
    case CMD_EMV_EXTEND_SET_ATTRIBUTE:
        respose = emv_extend_attribute_set(len, data);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, buffer);                
        break;
    case CMD_EMV_EXTEND_GET_ATTRIBUTE:
        respose = emv_extend_attribute_get(len, data,  &rev_len, &buffer[3]);
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);
        break;

    default :
        respose = CMDST_CMD_ER;
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;
    }
}

extern uint16_t sys_manage_get_rtc(struct rtc_time* tm);
extern uint16_t sys_manage_set_rtc(struct rtc_time* tm);
void ctc_sys_manage_operate(MCUPCK res, uint8_t *data,  uint32_t len)
{
    uint16_t respose;
    uint8_t data_temp[20];
    // unsigned int rev_len = 0;
    //unsigned int sed_len = 0;
    //uint8_t buffer[1024];
   uint32_t send_len;
   uint16_t time;
   uint16_t frequency;
   uint16_t number;
   uint16_t data_len;
   uint8_t buffer[1024];
   int status;
   struct rtc_time tm;

    switch (res.cmdop) {
    case CMD_SYS_MANAGE_LED :
        respose = sys_manage_led(data);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;
    case CMD_SYS_MANAGE_BEEP :
        time = data[0] | data[1] << 8;
        frequency = data[2] | data[3] << 8;
        respose = sys_manage_beep(time,frequency);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;
    case CMD_SYS_MANAGE_GET_RTC :
        respose = sys_manage_get_rtc(&tm);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        if ( respose == CMDST_OK ) {
            tm.tm_year = tm.tm_year+1900;
            data_temp[2] = (uint8_t)(tm.tm_year & 0xFF); 
            data_temp[3] = (uint8_t)(tm.tm_year >> 8) & 0xFF;
            data_temp[4] = (uint8_t)(tm.tm_mon & 0xFF) +1;
            data_temp[5] = (uint8_t)tm.tm_mday & 0xFF;
            data_temp[6] = (uint8_t)tm.tm_hour & 0xFF;
            data_temp[7] = (uint8_t)tm.tm_min & 0xFF;
            data_temp[8] = (uint8_t)tm.tm_sec & 0xFF;
        }
        TRACE("get time:%d-%d-%d %d-%d-%d\r\n", tm.tm_year, tm.tm_mon+1, tm.tm_mday,tm.tm_hour,
              tm.tm_min, tm.tm_sec );
        res.len = MIN_CMD_RESPONSE + 7;
        ctc_send_frame(res, data_temp);
        break;
    case CMD_SYS_MANAGE_SET_RTC :
        tm.tm_year = (ushort)(data[0] | (data[1] << 8)) -1900;
//        tm.tm_year = data[0] | (data[1] << 8) ;
        tm.tm_mon = data[2] -1 ;
        tm.tm_mday = data[3];
        tm.tm_hour = data[4];
        tm.tm_min = data[5];
        tm.tm_sec = data[6];
        TRACE("set time:%d-%d-%d %d-%d-%d\r\n", tm.tm_year, tm.tm_mon, tm.tm_mday,tm.tm_hour,
              tm.tm_min, tm.tm_sec );
        respose = sys_manage_set_rtc(&tm);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, data_temp);
        break;
    case CMD_SYS_MANAGE_MDF_STATUS :
        respose = sys_mdf_status_get(&status, (uchar *)(&data_temp[6]));
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        data_temp[2] = (uint8_t)status & 0xFF;
        data_temp[3] = (uint8_t)(status >> 8) & 0xFF;
        data_temp[4] = (uint8_t)(status >> 16) & 0xFF;
        data_temp[5] = (uint8_t)(status >> 24) & 0xFF;
        res.len = MIN_CMD_RESPONSE + 11;
        // 4字节状态码
        ctc_send_frame(res, data_temp);
        break;
    case CMD_SYS_MANAGE_MDF_UNLOCK :
        respose = sys_mdf_unlock(); 
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, data_temp);
        break;
    case CMD_SYS_MANAGE_READ_EXFLASH :
        number = data[0] | data[1]<<8; 
        data_len = data[2] | data[3]<<8; 
        respose = sys_manage_exflash_read(0, number,data_len,buffer+2);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + data_len;
        ctc_send_frame(res, buffer);
        break;

    case CMD_SYS_MANAGE_WRITE_EXFLASH :
        number = data[0] | data[1]<<8; 
        data_len = data[2] | data[3]<<8;
        respose = sys_manage_exflash_write(0, number,data_len,data+4);
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, data_temp);
        break;

    case CMD_SYS_MANAGE_DL_INFORM:
        respose = sys_manage_download_inform(data,buffer+2,&send_len);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE +send_len;
        ctc_send_frame(res, buffer);
        break;

   case CMD_SYS_MANAGE_DL_DATA:
        respose = sys_manage_download_data(data,buffer+2,&send_len);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + send_len;
        ctc_send_frame(res, buffer);
        break;

   case CMD_SYS_MANAGE_DL_FINISH:
        respose = sys_manage_download_finish(data);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;

        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, buffer);
        break;

   case CMD_SYS_MANAGE_INFO_MANAGE:
        respose = sys_safe_manage(data, len, (uchar *)(&buffer[2]), &send_len);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + send_len;
        ctc_send_frame(res, buffer);
        break;

   case CMD_SYS_MANAGE_GET_CUPSTATUS:
        respose = sys_cpu_get_inform(data,(uchar *)(&buffer[2]), &send_len );
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + send_len;
        ctc_send_frame(res, buffer);
        break;

   case CMD_SYS_MANAGE_SET_CUPSTATUS:
//        TRACE("get cmd:%d, subcmd:%d\r\n", data[0], data[1]);
        respose = sys_cpu_set_inform(data, len);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, buffer);
        break;

   case CMD_SYS_MANAGE_LOWPOWER:
        respose = sys_entry_lowpower(data);
        buffer[0] = 0x00;
        buffer[1] = 0x00;
        buffer[2] = (uint8_t)respose & 0xFF;
        buffer[3] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + 2;
        ctc_send_frame(res, buffer);
        break;

   case CMD_SYS_MANAGE_STATE_CHANGE:
        break; 
   
   default :
        respose = CMDST_CMD_ER;
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;

    }
}

void ctc_terminal_info_operate(MCUPCK res, uint8_t *data,  uint32_t len)
{
   // uint8_t ret = 0;
    uint16_t respose;
    uint8_t data_temp[2];
    unsigned int rev_len = 0;
 //   unsigned int sed_len = 0;
    uint8_t buffer[1024];
  //  uint32_t data_len;

    switch (res.cmdop) {
    case CMD_TERMINAL_INFO_SOFT:
        respose = terminal_info_soft(&rev_len,buffer+2);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);
        break;

    case CMD_TERMINAL_INFO_HARD:
        respose = terminal_info_hard(&rev_len,buffer+2);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, buffer);
        break;
    
    default :
        respose = CMDST_CMD_ER;
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;

    }
}

#ifdef CFG_SCANER
void ctc_scaner_report(int type, uint8_t *data , int len)
{
    uint16_t respose;
    MCUPCK scanerpck;
    int retlen;
    int i;
    uint8_t buf_scaner[520];
    memset(buf_scaner, 0, sizeof(buf_scaner));
    // if len euqal 0 ,that mean timeout
    scanerpck.cmdcls=  CMD_SCAN ;
    scanerpck.cmdop=CMD_SCAN_RESULT_REPORT ;
    scanerpck.sno1=0; 
    scanerpck.sno2=1; 
    if ( 0 == len ) {
        respose = CMDST_TIMEOUT;
        retlen = 0;
        buf_scaner[0] = (uint8_t)respose & 0xFF;
        buf_scaner[1] = (uint8_t)(respose >> 8) & 0xFF;
        buf_scaner[2] = (uint8_t)retlen & 0xFF;
        buf_scaner[3] = (uint8_t)(retlen >> 8) & 0xFF;
        buf_scaner[4] = (uint8_t)(retlen >> 16)  & 0xFF;
        buf_scaner[5] = (uint8_t)(retlen >> 24)  & 0xFF;
        scanerpck.len = MIN_CMD_RESPONSE+4;
        ctc_send_frame(scanerpck, buf_scaner);
    }else{
        respose = CMDST_OK;
        buf_scaner[0] = (uint8_t)respose & 0xFF;
        buf_scaner[1] = (uint8_t)(respose >> 8) & 0xFF;
        
        retlen = len -2;
        buf_scaner[2] = (uint8_t)retlen & 0xFF;
        buf_scaner[3] = (uint8_t)(retlen >> 8) & 0xFF;
        buf_scaner[4] = (uint8_t)(retlen >> 16)  & 0xFF;
        buf_scaner[5] = (uint8_t)(retlen >> 24)  & 0xFF;
        for(i = 0; i < retlen; i++) {
            buf_scaner[i+6] = data[i];
        }

        scanerpck.len = MIN_CMD_RESPONSE +retlen+4;
        ctc_send_frame(scanerpck, buf_scaner);     
    } 
}

extern uint Fac_SRAM(uint mode);
void ctc_fac_operate(MCUPCK res, uint8_t *data, uint32_t len) 
{
//    uint8_t ret = 0;
    uint8_t mode;
    uint32_t attackflag;
    uint16_t respose;
    uint8_t sendbuf[20];
    uint32_t status;

    memset(sendbuf, 0, sizeof(sendbuf));

    switch ( res.cmdop )
    {
    case  CMD_FAC_ATTACK:
        mode = data[0] ;
        if ( mode !=0 && mode !=1 ) {
            TRACE("parm err:%d\r\n", mode);
            respose =CMDST_PARAM_ER; 
        }else{
            if ( len != 5 ) {
                attackflag = 0;               
            }else{
                attackflag = data[1]|(data[2]<<8)|(data[3]<<16)|(data[4]<<24);
            }
            respose = CMDST_OK; 
            TRACE("FAC_SRAM opt:%x\r\n", attackflag);
            status = Fac_SRAM(attackflag);
            TRACE("get the fac status:%d\r\n", status);
            sendbuf[2] = status & 0xFF;
        }

        sendbuf[0] = (uint8_t)respose & 0xFF;
        sendbuf[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE+1;
        ctc_send_frame(res, sendbuf);                   
        break;
    default :
        break;
    }

}

void ctc_scaner_operate(MCUPCK res, uint8_t *data,  uint32_t len)
{
    uint8_t ret = 0;
    uint16_t respose;
    uint32_t timer;
    uint32_t mode; 
//    uint32_t retlen;
    uint8_t data_temp[2];
//    int i = 0;

    switch (res.cmdop) {
        case CMD_SCAN_OPEN :
                ret = scaner_2d_open(); 
                if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_OPEN_DEV_ER;
                }
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
                    
        case CMD_SCAN_CLOSE :
                ret = scaner_2d_close();
                 if (ret == RET_OK) {
                    respose = CMDST_OK;
                } else {
                    respose = CMDST_PARAM_ER;
                }
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);             
                break;

        case CMD_SCAN_OPERATE :
                mode = (data[0]) |(data[1]<< 8) |(data[2] << 16) |(data[3] <<24); 
                timer = (data[4]) |(data[5]<< 8) |(data[6] << 16) |(data[7] <<24); 
                scaner_2d_scan_start(timer, mode);
                data_temp[0] = 0;
                data_temp[1] = 0;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;

        case CMD_SCAN_RESULT_REPORT :
                // 接收上报返回，不处理
                break;
        case CMD_SCAN_CANCEL :
                scaner_2d_scan_cancel();
                data_temp[0] = 0;
                data_temp[1] = 0;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;

        default :
                respose = CMDST_CMD_ER;
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
    }
}
#endif
//void ctc_qrcode_operate(MCUPCK res, uint8_t *data,  uint32_t len)
//{
//  //  uint8_t ret = 0;
//    uint16_t respose = 0;
//    uint8_t data_temp[2];
//    unsigned int rev_len = 0;
////    unsigned int sed_len = 0;
//    uint8_t buffer[1024];
////    uint32_t data_len;
//
//    memset(buffer, 0, sizeof(buffer)); 
//    switch (res.cmdop) {
//    case CMD_QRCODE_OPEN:
//     //   respose = qrcord_open();
//        data_temp[0] = (uint8_t)respose & 0xFF;
//        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
//        res.len = MIN_CMD_RESPONSE;
//        ctc_send_frame(res, data_temp);
//        break;
//    case CMD_QRCODE_CLOSE:
//     //   respose = qrcode_close();
//        data_temp[0] = (uint8_t)respose & 0xFF;
//        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
//        res.len = MIN_CMD_RESPONSE;
//        ctc_send_frame(res, data_temp);
//        break;
//    case CMD_QRCODE_OPERATE:
//       // respose = qrcode_operate(&rev_len,buffer+2);
//        buffer[0] = (uint8_t)respose & 0xFF;
//        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
//        res.len = MIN_CMD_RESPONSE + rev_len;
//        ctc_send_frame(res, data_temp);
//        break;
//    default :
//        respose = CMDST_CMD_ER;
//        data_temp[0] = (uint8_t)respose & 0xFF;
//        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
//        res.len = MIN_CMD_RESPONSE;
//        ctc_send_frame(res, data_temp);
//        break;
//
//    }
//}


void ctc_uart_expand_operate(MCUPCK res, uint8_t *data,  uint32_t len)
{
 //   uint8_t ret = 0;
    uint16_t respose = 0;
    uint8_t data_temp[2];
    unsigned int rev_len = 0;
    unsigned int sed_len = 0;
    uint8_t buffer[1024];
//    uint32_t data_len;

    sed_len = sed_len;
    memset(buffer, 0, sizeof(buffer)); 
    switch (res.cmdop) {
    case CMD_UART_EXP_OPEN:
     //   respose = uart_expand_open();
 //       data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;
    case CMD_UART_EXP_CLOSE:
      //  respose = uart_expand_close();
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;
    case CMD_UART_EXP_READ:
       // respose = uart_expand_read(&rev_len,buffer+2);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE + rev_len;
        ctc_send_frame(res, data_temp);
        break;
    case CMD_UART_EXP_WRITE:
        sed_len = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24) ;
        //respose = uart_expand_write(sed_len,data+4);
        buffer[0] = (uint8_t)respose & 0xFF;
        buffer[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE ;
        ctc_send_frame(res, data_temp);
        break;
    default :
        respose = CMDST_CMD_ER;
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        break;

    }
}

#ifdef DEBUG_Dx
uint8_t ctc_recev_frame_debug(uint8_t* buff, uint32_t cmd_len)
{
    MCUPCK res;
    uint32_t outlen;
    uint8_t outbuf[2048];
    // res 为头8字节
    int len;
    if ( cmd_len < 8 ) {
        return -1; 
    }
    memcpy(outbuf, buff+8, cmd_len-8);
    len = buff[0]|(buff[1]<<8)|(buff[2]<<16)|(buff[3]<<24);
    TRACE("get len:%d\r\n", len);
    outlen = cmd_len-8 ;
    TRACE("get outlen:%d\r\n", outlen);
    memcpy(&res, buff, sizeof(MCUPCK));

    TRACE("\r\nget the cmd:%x and struct len:%d\r\n", buff[4], sizeof(MCUPCK));
//    TRACE("\r\nget the cmd:%x\r\n", res.cmdcls);
    switch (res.cmdcls) {
        case CMD_MAGCARD :
                ctc_magcard_operate(res, outbuf);
                break;

        case CMD_FAC :
                ctc_fac_operate(res, outbuf, outlen);
                break;
                    
        case CMD_NEEDLE :
                ctc_nprinter_operate(res, outbuf, outlen);
                break;

        case CMD_TPRINTER :
                ctc_tprinter_operate(res, outbuf, outlen);
                break;

        case CMD_ICCARD :
                ctc_iccard_operate(res, outbuf, outlen);
                break;
                
        case CMD_RFCARD :
                ctc_rfcard_operate(res, outbuf, outlen);
                break;
                    
        case CMD_SIGN :
                ctc_sign_operate(res, outbuf, outlen);
                break;
 
        case CMD_PED :
                ctc_pinpad_operate(res, outbuf, outlen);
                break;

        case CMD_EMV :
                ctc_emv_operate(res, outbuf, outlen);
                break;

        case CMD_SYS_MANAGE :
                ctc_sys_manage_operate(res, outbuf, outlen);
                break;

        case CMD_TERMINAL_INFO:
                ctc_terminal_info_operate(res, outbuf, outlen);
                break;

        case CMD_SCAN :
#ifdef CFG_SCANER
                ctc_scaner_operate(res, outbuf, outlen);
#endif
                break;

//        case CMD_QRCODE:
//                ctc_qrcode_operate(res, outbuf, outlen);
//                break;

        case CMD_UART_EXP:
                ctc_uart_expand_operate(res, outbuf, outlen);
                break;
        default :
                break;
    }
    return 0;
}
#endif

#define CTC_RECEV_NORMAL    0  // 正常模式
#define CTC_RECEV_ASYNC     1  // 异步模式，代用密码键盘使用到

uint8_t ctc_recev_frame(uint8_t mode, uint8_t *cmd, uint32_t cmd_len)
{
    MCUPCK res;
    uint32_t outlen;
    uint8_t outbuf[2048];
    uint32_t ret = 0;
    uint16_t respose;
    uint8_t data_temp[2];

    ret = unpackCmd(cmd, cmd_len, &res, outbuf, &outlen);
    if (ret == RET_CHECK_LEN) {
        respose = CMDST_DATA_LEN_ER;
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp);
        goto RET;
        
    } else if (ret == RET_LRC) {
        respose = CMDST_LRC_ER;
        data_temp[0] = (uint8_t)respose & 0xFF;
        data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
        res.len = MIN_CMD_RESPONSE;
        ctc_send_frame(res, data_temp); 
        goto RET;
    } 

    switch (res.cmdcls) {
        case CMD_MAGCARD :
                ctc_magcard_operate(res, outbuf);
                break;
        case CMD_FAC :
                ctc_fac_operate(res, outbuf, outlen);
                break;
        case CMD_NEEDLE :
                ctc_nprinter_operate(res, outbuf, outlen);
                break;

        case CMD_TPRINTER :
                ctc_tprinter_operate(res, outbuf, outlen);
                break;

        case CMD_ICCARD :
                ctc_iccard_operate(res, outbuf, outlen);
                break;
                
        case CMD_RFCARD :
                ctc_rfcard_operate(res, outbuf, outlen);
                break;
                    
        case CMD_SIGN :
                ctc_sign_operate(res, outbuf, outlen);
                break;
 
        case CMD_PED :
                if ( mode == CTC_RECEV_NORMAL ) {
                    ctc_pinpad_operate(res, outbuf, outlen);
                }
                break;

        case CMD_EMV :
                if ( mode == CTC_RECEV_NORMAL ) {
                    ctc_emv_operate(res, outbuf, outlen);
                }
                break;

        case CMD_SYS_MANAGE :
                ctc_sys_manage_operate(res, outbuf, outlen);
                break;

        case CMD_TERMINAL_INFO:
                ctc_terminal_info_operate(res, outbuf, outlen);
                break;

        case CMD_SCAN :
#ifdef CFG_SCANER
                ctc_scaner_operate(res, outbuf, outlen);
#endif
                break;

//        case CMD_QRCODE:
//                ctc_qrcode_operate(res, outbuf, outlen);
//                break;

        case CMD_UART_EXP:
                ctc_uart_expand_operate(res, outbuf, outlen);
                break;

        default :
                respose = CMDST_PARAM_ER;
                data_temp[0] = (uint8_t)respose & 0xFF;
                data_temp[1] = (uint8_t)(respose >> 8) & 0xFF;
                res.len = MIN_CMD_RESPONSE;
                ctc_send_frame(res, data_temp);
                break;
    }
    ret = RET_OK;

RET:
    return ret;
}

int ctc_frame_check(uint8_t *data, uint32_t size, int *s_pos, int *e_pos)
{
    return frame_integrity_check(data, size,  s_pos, e_pos);
}

int ctc_uart_open(void) 
{
    return drv_uart_open_dma(CTC_COMPORT , CTC_BAUD,
                          gwp30SysBuf_c.work,WRK_BUFSIZE_C,
                          UART_DMATYPE_RECE, UART4_DMA_CHANNEL);

}

int ctc_uart_open_nodma(void)
{
    int result;

    result = drv_uart_open(CTC_COMPORT,CTC_BAUD,
                           gwp30SysBuf_c.work,WRK_BUFSIZE_C,UartAppCallBack[CTC_COMPORT]);
    drv_uart_clear(CTC_COMPORT);
    return result;
}

int ctc_uart_close_nodma(void)
{
    return drv_uart_close(CTC_COMPORT);
}

int ctc_uart_close(void) 
{
    return  drv_uart_dma_close(CTC_COMPORT , UART4_DMA_CHANNEL);

}

extern int drv_uart_dam_recv_init(int uart,uint32 daddr,int buf_len, int channel);
//int ctc_uart_restart(void) 
//{
//    return drv_uart_dam_recv_init(CTC_COMPORT,(uint32)(&gwp30SysBuf_c.work[0]),sizeof(gwp30SysBuf_c.work), UART4_DMA_CHANNEL);
//}


extern void drv_uart_dma_start(uint32_t instance, dma_config_t *dma_config);
int ctc_uart_restart(void) 
{
    dma_config_t dma_config;
    dma_config.chl = UART4_DMA_CHANNEL;
    dma_config.s_addr = (uint32)(&UART_D_REG(UART4_BASE_PTR));
    dma_config.d_addr = (uint32)(&gwp30SysBuf_c.work[0]);
    dma_config.size = sizeof(gwp30SysBuf_c.work);
    drv_uart_dma_start(CTC_COMPORT, &dma_config);
    return 0;
}

void t_print_buff(char* buff, int len)
{
    int i;
    for ( i=0 ; i<len ; i++) {
       TRACE("%02X ", (unsigned char)buff[i]); 
       if ( buff[i] == 0x55 ) {
           TRACE("\r\n"); 
       }
    }
}

int ctc_uart_dma_check1(void)
{
    uint32_t  pos;
    uint32_t  data_vail_len=0; 

    pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
    TRACE("get the dest start address:%x\r\n", (uint32)(&gwp30SysBuf_c.work[0]));
    TRACE("get the dest current address:%x\r\n", pos);
    data_vail_len = pos -(uint32)(&gwp30SysBuf_c.work[0]);
    if (data_vail_len > 0 ) {
        TRACE("receive data=======\r\n");
        t_print_buff((char *)gwp30SysBuf_c.work, data_vail_len);
        return YES;
    }
    return NO;
}

int ctc_uart_dma_check(void)
{
    uint32_t  pos;
    uint32_t  data_vail_len=0; 

    pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
    data_vail_len =  pos -(uint32)(&gwp30SysBuf_c.work[0]);
    if (data_vail_len > 0 ) {
        return YES;
    }
    return NO;
}


