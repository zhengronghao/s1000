/***************** (C) COPYRIGHT 2015 START Computer equipment *****************
 * File Name          : local_dl.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/13/2015 9:54:42 AM
 * Description        : 
 *******************************************************************************/
#include "app_common.h"
#include "local_dl.h"

#if 0
#define LTRACE         TRACE
#define LTRACE_BUF     TRACE_BUF
#else 
#define LTRACE(...)
#define LTRACE_BUF(x,y,z)
#endif



int localdl_hand_shake(struct LocalDlOpt *localdl,int32_t t_out_ms)
{
    uint8_t i;
    uint8_t ch=0;
    uint32_t time_s;

    time_s = sys_get_counter();
    localdl->title = "正在握手...";
    if (localdl->callback->display != NULL) 
    {
        localdl->callback->display(1,localdl);
    }
    localdl->contrl.bit.finish = 0;
    localdl->contrl.bit.finish_timer = 0;
    while (1)
    {
        for (i=0; i<lOCALDL_COMPORT_NUM; i++)
        {
            if (t_out_ms > 0 && (sys_get_counter() - time_s > t_out_ms) ) {
                return LOCALDL_TIMEOUT;//timeout
            }
            if (localdl->callback->check_buffer(localdl->commbuf[i]) >= 1) 
            {
                localdl->callback->read_data(localdl->commbuf[i],&ch,1,100);
                if(ch == DL_HANDSHAKE_CTRL) {
                    localdl->callback->clear_buffer(localdl->commbuf[i]);
                    ch = DL_HANDSHAKE_CTRL_REPLY;
                    localdl->callback->write_data(localdl->commbuf[i], &ch, 1);
                    localdl->commindex = i;
                    return(localdl->commbuf[i]);
                } else if(ch == DL_HANDSHAKE_BOOT) {
                    localdl->callback->clear_buffer(localdl->commbuf[i]);
                    ch = DL_HANDSHAKE_BOOT_REPLY;
                    localdl->callback->write_data(localdl->commbuf[i], &ch, 1);
                    localdl->commindex = i;
                    return(localdl->commbuf[i]);
                }
            }
            if (localdl->callback->cancel != NULL) 
            {
                if (localdl->callback->cancel()) {
                    localdl->title = "取消退出";
                    if (localdl->callback->display != NULL) {
                        localdl->callback->display(1,localdl);
                    }
                    return LOCALDL_CANCEL;
                }
            }
        }
    }
}

static int localdl_send_frame(struct LocalDlOpt *localdl)
{
    uint32_t crc16;
    struct LocalDlFrame *frame = localdl->frame;
    //return-code:Each command processing in the process 
    //length
    msb_uint16_to_byte2(frame->bin_len,frame->length);
    crc16 = calc_crc16(frame->cmd,frame->bin_len+DL_LOCAL_FIXEDLEN,0);
    msb_uint16_to_byte2(crc16,frame->bin+frame->bin_len);
    frame->bin_len += (DL_LOCAL_STXLEN+DL_LOCAL_FIXEDLEN+DL_LOCAL_CRCLEN);
    LTRACE_BUF("->Send:",&frame->stx,frame->bin_len);
    if (localdl->callback->write_data(localdl->commbuf[localdl->commindex],
                                      &frame->stx, frame->bin_len) != frame->bin_len){
        return LOCALDL_SEND;
    } else {
        return 0;
    }
}

static int localdl_recv_frame(struct LocalDlOpt *localdl,uint32_t *flength, int t_out_ms)
{
    uint32_t rvlength = 0;
    uint32_t time_s = 0;
    uint16_t index;
    uint8_t *data = (uint8_t *)localdl->frame;
    uint8_t ch;

    index = 0;
    time_s = sys_get_counter();
    rvlength = 1; 
    while (1)
    {
        if (localdl->callback->check_buffer(localdl->commbuf[localdl->commindex]) >= rvlength)
        {
            if(rvlength == localdl->callback->read_data(localdl->commbuf[localdl->commindex],data,rvlength,100))
            {
                if (data[index] == DL_LOCAL_STX) {
                    LTRACE("\n\n-|->Recv: STX");
                    index++;
                    break;
                }
                if (data[index] == DL_HANDSHAKE_CTRL) {
                    localdl->callback->clear_buffer(localdl->commbuf[localdl->commindex]);
                    ch = DL_HANDSHAKE_CTRL_REPLY;
                    localdl->callback->write_data(localdl->commbuf[localdl->commindex],&ch, 1);
                }
                if (data[index] == DL_HANDSHAKE_BOOT) {
                    localdl->callback->clear_buffer(localdl->commbuf[localdl->commindex]);
                    ch = DL_HANDSHAKE_BOOT_REPLY;
                    localdl->callback->write_data(localdl->commbuf[localdl->commindex],&ch, 1);
                }
                LTRACE("\n-|->RecvSTX:%02X",data[index]);
            }
        }
        if (t_out_ms>0 && sys_get_counter() - time_s > 1000) {
			LTRACE("\n->RecvSTX tout");
            return LOCALDL_TIMEOUT;
        }
        s_DelayUs(8);
    }
    localdl->contrl.bit.finish = 0;
    localdl->contrl.bit.finish_timer = 0;
    *flength = rvlength;
    time_s = sys_get_counter();  	
    //cmdcode[2B]+return code[2B]+length[2B]
    rvlength = DL_LOCAL_FIXEDLEN; 
    while (1)
    {
        if (localdl->callback->check_buffer(localdl->commbuf[localdl->commindex]) >= rvlength)
        {
            if(rvlength == localdl->callback->read_data(localdl->commbuf[localdl->commindex],data+index,rvlength,100)) {
                index += rvlength ;
                break;
            }
        }
        if (t_out_ms>0 && sys_get_counter() - time_s > t_out_ms) {
			LTRACE("->len-tout"); 
            return LOCALDL_TIMEOUT;
        }
    }
    *flength += rvlength;
//    LTRACE_BUF("Recv:",data,*flength);
    time_s = sys_get_counter();
    //data[size bytes]+CRC16[2B]
    rvlength = msb_byte2_to_uint16(data+FPOS(struct LocalDlFrame,length))+DL_LOCAL_CRCLEN; 
    LTRACE("->len:%d",rvlength-2);
    while (1)
    {
        if (rvlength <= localdl->callback->check_buffer(localdl->commbuf[localdl->commindex])) 
        {
            if(rvlength == localdl->callback->read_data(localdl->commbuf[localdl->commindex],data+index,rvlength,t_out_ms)) {
                break;
            }
        }
        if (t_out_ms>0 && sys_get_counter() - time_s >t_out_ms+rvlength) {
			LTRACE("->data-tout:%d",rvlength);
            return LOCALDL_TIMEOUT;
        }
        s_DelayUs(100+rvlength);
    }
    *flength += rvlength;
    LTRACE_BUF("Recv:",data,*flength);

    return OK;
}

//scope:no data to return
void localdl_form_reurncode(struct LocalDlFrame *frame,uint16_t recode)
{
    msb_uint16_to_byte2(recode,frame->recode);
    frame->bin_len = 0;
}

static int localdl_check_frame(struct LocalDlFrame *frame,uint32_t flength)
{
    uint16_t crc16=0;

    if (frame->stx != DL_LOCAL_STX) {
        crc16 = EAPP_ERROR;
        goto ERR_CHK;
    }
    frame->bin_len = msb_byte2_to_uint16(frame->length);
    if ((flength - 1 - DL_LOCAL_FIXEDLEN - DL_LOCAL_CRCLEN) != frame->bin_len) {
        crc16 = EAPP_ERROR;
        goto ERR_CHK;
    }
    crc16 = calc_crc16(frame->cmd,(frame->bin_len+DL_LOCAL_FIXEDLEN),0);
    if (msb_byte2_to_uint16(&frame->bin[frame->bin_len]) != crc16) {
        crc16 = EAPP_CHECKSUM;
        goto ERR_CHK;
    }
//    LTRACE("->chkOK");
    return OK;
ERR_CHK:
//    LTRACE("->chk:%d",crc16);
    localdl_form_reurncode(frame,crc16);
    return LOCALDL_CHECK;
}

static void localdl_set_bps(struct LocalDlOpt *localdl)
{
//    struct LocalDlFrame *frame = localdl->frame;

//    if ( msb_byte2_to_uint16(frame->cmd) != DLCMD_SETBUAD
//         || msb_byte2_to_uint16(frame->recode) != OK)
    if (localdl->contrl.bit.chagebps)
    {
        localdl->contrl.bit.chagebps = 0;
        LTRACE("\n-|Setbps:%d",localdl->bps);
        if ((localdl->callback->open_comport != NULL)
           && (localdl->callback->close_comport != NULL)
           && (localdl->commbuf[localdl->commindex] < USBD_CDC))
        {
            s_DelayMs(50);//To make sure the uart had send over the last byte.
            localdl->callback->close_comport(localdl->commbuf[localdl->commindex]);
            localdl->callback->open_comport(localdl->commbuf[localdl->commindex],
                                            localdl->bps,
                                            localdl->serialbuf[localdl->commindex],
                                            localdl->seriallen[localdl->commindex]);
        }
    }
}
static void localdl_parse_bps(struct LocalDlOpt *localdl)
{
    struct LocalDlFrame *frame = localdl->frame;

    localdl->bps = msb_byte4_to_uint32(frame->bin);
    LTRACE("\nParsebps:%d",localdl->bps);
    if (localdl->bps > DL_CFG_MAX_BAUD) {
        localdl->contrl.bit.chagebps = 0;
        msb_uint16_to_byte2(EAPP_BAUDERR,frame->recode);
    } else {
        localdl->contrl.bit.chagebps = 1;
        msb_uint16_to_byte2(OK,frame->recode);
    }
    frame->bin_len = 0;
}

static void localdl_fac_ctrl(struct LocalDlOpt *localdl)
{
    if (localdl->callback->fac_ctrl != NULL)
    {
        localdl->callback->fac_ctrl(localdl->frame);
    } else
    {
        localdl_form_reurncode(localdl->frame,EAPP_ERROR);   
    }
}

static void localdl_get_terminal_info(struct LocalDlOpt *localdl)
{
    if (localdl->callback->get_terminal_info != NULL)
    {
        localdl->callback->get_terminal_info(localdl->frame);
    } else
    {
        localdl_form_reurncode(localdl->frame,EAPP_ERROR);   
    }
}

static void localdl_finsh(struct LocalDlOpt *localdl)
{
    struct LocalDlFrame *frame = localdl->frame;
    if (msb_byte2_to_uint16(frame->length) != 4)
    {
        msb_uint16_to_byte2(EAPP_ERROR,frame->recode);
    } else 
    {
        localdl->contrl.bit.chagebps = 0;
        localdl->contrl.bit.finish = 1;
        localdl->contrl.bit.finish_timer = 1;
        if (0 == msb_byte4_to_uint32(frame->bin)) {
            localdl->contrl.bit.chagebps = 1;
            localdl->bps = 9600;
        }
        msb_uint16_to_byte2(OK,frame->recode);
    }
    
    frame->bin_len = 0;
}

static void localdl_set_time(struct LocalDlFrame *frame)
{
    unsigned long second=0;
    struct rtc_time tm;

    if (msb_byte2_to_uint16(frame->length) != 12)
    {
        localdl_form_reurncode(frame,EAPP_ERROR);
    } else 
    {
        tm.tm_year = ascii2_to_dec1(frame->bin+0)+2000-1900;
        tm.tm_mon  = ascii2_to_dec1(frame->bin+2)-1;
        tm.tm_mday = ascii2_to_dec1(frame->bin+4);
        tm.tm_hour = ascii2_to_dec1(frame->bin+6);
        tm.tm_min  = ascii2_to_dec1(frame->bin+8);
        tm.tm_sec  = ascii2_to_dec1(frame->bin+10)+1;
        if (tm.tm_sec >= 60) {
            tm.tm_min++;
            tm.tm_min %= 60;
            tm.tm_sec = 0;
        }
        if (rtc_valid_tm(&tm)) {
            // Something went wrong encoding the date/time
            localdl_form_reurncode(frame,EAPP_CLOCK);
        } else {
            rtc_tm_to_time(&tm,&second);
            hw_rtc_init(second,0);
            localdl_form_reurncode(frame,OK);
        }
    }
}

static void localdl_set_serial_number(struct LocalDlOpt *localdl)
{
    uint16_t recode=OK;
    struct LocalDlFrame *frame = localdl->frame;
    if (msb_byte2_to_uint16(frame->length) > 32)
    {
        recode= EAPP_ERROR;
    } else 
    {
        LTRACE_BUF("SN",frame->bin,frame->bin_len);
        if (localdl->callback->save_sn != NULL)
        {
            if (localdl->callback->save_sn(frame->bin,frame->bin_len)) {
                recode= EAPP_ERROR;
            }
        } else 
        {
            recode= EAPP_ERROR;
        }
    }
    localdl_form_reurncode(frame,recode);
}

static void localdl_download_request(struct LocalDlOpt *localdl)
{
    uint32_t recode;
    struct LocalDlFrame *frame = localdl->frame;

    if (msb_byte2_to_uint16(frame->length) > 64)
    {
        recode = EAPP_ERROR;
        goto REQ_ERR;
    } else 
    {
//        LTRACE_BUF("Req:",frame->bin,frame->bin_len);
        localdl->request = msb_byte4_to_uint32(frame->bin+0);
        localdl->filelen = msb_byte4_to_uint32(frame->bin+4);
//        LTRACE("\n-|spacesize:%d",localdl->filelen);
        switch (localdl->request)
        {
        case FILE_TYPE_BOOT:
            if (localdl->dllevel == DL_HANDSHAKE_BOOT) {
                recode = EAPP_UNKNOWNAPP;
                goto REQ_ERR;
            }
            break;
        case FILE_TYPE_CTRL:
            if (localdl->dllevel == DL_HANDSHAKE_CTRL) {
                recode = EAPP_UNKNOWNAPP;
                goto REQ_ERR;
            }
            break;
        case FILE_TYPE_FONT:
            break;
//        case FILE_TYPE_APP://应用程序
//            break;
//        case FILE_TYPE_DATA://数据文件
//            break;
        default:
            recode = EAPP_ERROR;
            goto REQ_ERR;
        }
    }
    localdl_form_reurncode(frame,OK);
    return;
REQ_ERR:
    localdl_form_reurncode(frame,recode);
}

static void localdl_transfer_data(struct LocalDlOpt *localdl)
{
    uint16_t recode = EAPP_ERROR;

    if (localdl->callback->save_data != NULL)
    {
        recode = localdl->callback->save_data(localdl);
    }
    localdl_form_reurncode(localdl->frame,recode);
}

static void localdl_save_data(struct LocalDlOpt *localdl)
{
    localdl_form_reurncode(localdl->frame,OK);
}

static void localdl_execute_cmd(struct LocalDlOpt *localdl)
{
    LTRACE("\n-|->cmd:%04X",msb_byte2_to_uint16(localdl->frame->cmd));
    switch (msb_byte2_to_uint16(localdl->frame->cmd))
    {
    case DLCMD_SETBUAD:
        localdl_parse_bps(localdl);
        localdl->title = "设置波特率";
        break;
    case DLCMD_GETTERMINFO:
        localdl_get_terminal_info(localdl);
        localdl->title = "获取终端信息";
        break;
    case DLCMD_SETTIME:
        localdl_set_time(localdl->frame);
        localdl->title = "设置时间";
        break;
    case DLCMD_SETSN:
        localdl_set_serial_number(localdl);
        localdl->title = "设置系列号";
        break;
    case DLCMD_LOADFILE:
        localdl_download_request(localdl);
        localdl->title = "下载请求";
        break;
    case DLCMD_TRANSDATA:
        localdl_transfer_data(localdl);
        localdl->title = "数据传输";
        break;
    case DLCMD_SAVEFILE:
        localdl_save_data(localdl);
        localdl->title = "保存文件";
        break;
    case DLCMD_FINISH:
        localdl_finsh(localdl);
        localdl->title = "完成";
        break;
    case DLCMD_GETAPPINFO:
        localdl_form_reurncode(localdl->frame,EAPP_NOAPP);
        localdl->title = "获取应用信息";
        break;
    case DLCMD_FAC_CTRL:
        localdl_fac_ctrl(localdl);
        localdl->title = "生产管控";
        break;
    default:
        localdl_form_reurncode(localdl->frame,EAPP_UNKNOWNCMD);
        localdl->title = "不支持命令";
        break;
    }
    localdl->dlrecode = msb_byte2_to_uint16(localdl->frame->recode);
}

int localdl_check_para(struct LocalDlOpt *localdl)
{
    if (localdl->frame == NULL) {
        return LOCALDL_PARA;
    }
    if (localdl->callback->open_comport == NULL) {
        return LOCALDL_PARA;
    }
    if (localdl->callback->close_comport == NULL) {
        return LOCALDL_PARA;
    }
    if (localdl->callback->check_buffer == NULL) {
        return LOCALDL_PARA;
    }
    if (localdl->callback->clear_buffer == NULL) {
        return LOCALDL_PARA;
    }
    if (localdl->callback->read_data == NULL) {
        return LOCALDL_PARA;
    }
    if (localdl->callback->write_data == NULL) {
        return LOCALDL_PARA;
    }
    if (localdl->callback->save_data == NULL) {
        return LOCALDL_PARA;
    }
    if (localdl->callback->save_sn == NULL) {
        return LOCALDL_PARA;
    }
    if (localdl->callback->get_terminal_info == NULL) {
        return LOCALDL_PARA;
    }
    return 0;
}

int localdl_process(struct LocalDlOpt *localdl)
{
    uint32_t flength;
    uint32_t tout;
    int iRet;

    if (localdl_check_para(localdl)) {
        return LOCALDL_PARA;
    }
    iRet = localdl_hand_shake(localdl,3*100);
    if (iRet < 0) {
        return iRet;
    }
    LTRACE("\n-|Handshake succe! dlcomindex:%d",localdl->commindex);
    while (1)
    {
        if (localdl->callback->cancel != NULL)
        {
            if (localdl->callback->cancel())
            {
                if (localdl->contrl.bit.finish) {
                    localdl->title = "结束退出";
                } else {
                    localdl->title = "取消退出";
                }
                iRet = LOCALDL_CANCEL;
                break;
            }
        }
        if (localdl->contrl.bit.finish)
        {
            if (localdl->contrl.bit.boot_update || localdl->contrl.bit.ctrl_update) {
                if (localdl->contrl.bit.finish_timer) {
                    localdl->contrl.bit.finish_timer = 0;
                    tout = sys_get_counter();
                }
                if (sys_get_counter() - tout >= LOCALDL_FINSH_TIMEOUT) {
                    localdl->title = "结束退出";
                    iRet = OK;
                    break;
                }
            }
        }
        if ((iRet = localdl_recv_frame(localdl,&flength,3500)) < OK) 
        {
            continue;
        }
        if ((iRet = localdl_check_frame(localdl->frame,flength)) == OK) 
        {
            localdl_execute_cmd(localdl);
        }
        if ((iRet = localdl_send_frame(localdl)) < OK)
        {
            break;
        }
        localdl_set_bps(localdl);
        if (localdl->callback->display != NULL)
        {
            localdl->callback->display(0,localdl);
        }
        tout = sys_get_counter(); //有其它数据下载，重新计算超时
    }
    if (localdl->callback->display != NULL)
    {
        localdl->callback->display(0,localdl);
        s_DelayUs(1500);
    }
    return iRet;
}

