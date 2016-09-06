/*
 * =====================================================================================
 *
 *       Filename:  local_dl_fac.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  1/27/2015 3:51:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhangjp (), zhangjp@itep.com.cn
 *        Company:  start
 *
 * =====================================================================================
 */

#include "app_common.h"
#include "local_dl_app.h"
#include "local_dl_fac.h"
#include "wp30_ctrl.h"

int s_localdl_fac_get_time(struct LocalDlFrame *frame)
{
    char *pbuf = (char *)frame->bin+4;
    unsigned long second=0;
    struct rtc_time tm;

	memset(pbuf, 0x00, DL_LOCAL_FRAME_SIZE);
    if (hw_rtc_read_second((uint32_t *)&second) == 0) 
    {
        rtc_time_to_tm(second,&tm);
        pbuf[0] = tm.tm_year+1900-2000;
        pbuf[1] = tm.tm_mon+1;
        pbuf[2] = tm.tm_mday;
        pbuf[3] = tm.tm_hour;
        pbuf[4] = tm.tm_min;
        pbuf[5] = tm.tm_sec;
        pbuf[6] = 0x00;
    } else
    {
        TRACE("\nRead second reg error!");
        pbuf[0] = 0x00;
        pbuf[1] = 0x00;
        pbuf[2] = 0x00;
        pbuf[3] = 0x00;
        pbuf[4] = 0x00;
        pbuf[5] = 0x00;
        pbuf[6] = 0x00;
    }
    // time 
    // length
    frame->bin_len = 7+sizeof(int);
    // recode
    msb_uint16_to_byte2(OK,frame->recode);
    return 0;
}

int s_localdl_fac_get_testres(struct LocalDlFrame *frame)
{
    FAC_TESTINFO  fac_tinfo;
    uint offset;
    char *pbuf = (char *)frame->bin+sizeof(int);
	memset(pbuf, 0x00, DL_LOCAL_FRAME_SIZE);
    // testres
    fac_ctrl_get_fac_testinfo(&fac_tinfo);
    offset = FPOS(FAC_TESTINFO,auto_res[0]);
    memcpy((char *)pbuf,(char *)&fac_tinfo.mbno[0],offset);
    for(int i=0;i<4;i++)
    {
        msb_uint32_to_byte4(fac_tinfo.auto_res[i],(uint8_t *)pbuf+offset+sizeof(int)*i);
    }
    // length
    frame->bin_len = 4+sizeof(FAC_TESTINFO);
    // recode
    msb_uint16_to_byte2(OK,frame->recode);
    return 0;
}

int s_localdl_fac_get_app_descrpt(struct LocalDlFrame *frame)
{
    return 0;
}

int s_localdl_fac_set_woinfo(struct LocalDlFrame *frame)
{
    char *pbuf = (char *)frame->bin+sizeof(int); 
    uint offset;
//    memset(pbuf, 0x00, DL_LOCAL_FRAME_SIZE);
    // 13B  12+1
    // write voucherno 
    fac_ctrl_set_Voucherno((uchar *)pbuf);
    // write fac_ctrl_flag
    offset = FSIZE(FAC_TESTINFO,voucherno);
    fac_ctrl_set_fac_ctrl_flag((uchar)pbuf[offset]);
    //length
    frame->bin_len = 0;
    //recode
    msb_uint16_to_byte2(OK,frame->recode);
    return 0;
}

int s_localdl_fac_sendinfo(struct LocalDlFrame *frame)
{
    int sub_cmd;
    char *pbuf = (char *)frame->bin+sizeof(int); 
    sub_cmd = msb_byte4_to_uint32((uchar *)pbuf);
    frame->bin_len = 0;
    //recode
    if ( sub_cmd == 0x05 ) 
    {
        //管控成功
        msb_uint16_to_byte2(OK,frame->recode);
        return OK;
    }
    else
    {
        //失败
        msb_uint16_to_byte2(ERROR,frame->recode);
        return ERROR;
    }
}

int s_localdl_fac_ctrl(struct LocalDlFrame *frame)
{

     int recode = 0xFF;
     lcd_display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "生产管控");
    // parse sub cmd
    TRACE("\n-|->cmd:%04X",msb_byte4_to_uint32(frame->bin));
    switch(msb_byte4_to_uint32(frame->bin))
    {
    case SUB_CMD_DEL_FAC:
        break;
    case SUB_CMD_GET_TESTRES:
        s_localdl_fac_get_testres(frame);
        break;
    case SUB_CMD_GET_ATTACK:
        break;
    case SUB_CMD_ALARM:
        break;
    case SUB_CMD_WRITE_WOINFO:
        s_localdl_fac_set_woinfo(frame);
        break;
    case SUB_CMD_SET_ATTACK:
        break;
    case SUB_CMD_MODULE_EXIST:
        break;
    case SUB_CMD_DEL_ALLSAPP:
        break;
    case SUB_CMD_GET_TIME:
        s_localdl_fac_get_time(frame);
        break;
    case SUB_CMD_GET_APP_DESCRPT:
        break;
    case SUB_CMD_SET_MAINBOARDID:
        break;
    case SUB_CMD_SENDINFOR:
        recode = s_localdl_fac_sendinfo(frame);
        if ( recode == OK ) 
        {
            lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE, "管控检成功");
        }
        else if(recode == ERROR) 
        {
            lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE, "管控检失败");
            lcd_display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY|DISP_INVLINE, "请查看管控界面");
            sys_beep();
        }
        break;
    default:
        localdl_form_reurncode(frame,EAPP_UNKNOWNCMD);
        break;
    }
    if ( recode == 0xFF ) 
    {
        lcd_display(0, DISP_FONT_LINE2, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "命令成功");
    }
    return 0;
}

const struct LocalDlCallBack gcLocaldlCallBack = 
{
    .open_comport = dlcom_open,
    .close_comport = dlcom_close,
    .check_buffer = dlcom_check_readbuf,
    .clear_buffer = dlcom_clear,
    .read_data = dlcom_read,
    .write_data = dlcom_write,
    .save_data = s_localdl_save_data,
    .save_sn = s_localdl_save_sn,
    .get_terminal_info = s_localdl_get_terminal_info,
    .after_done = localdl_ctrl_make_check,
    .cancel = s_localdl_cancle,
    .display = s_localdl_display,
    .fac_ctrl = s_localdl_fac_ctrl
};
