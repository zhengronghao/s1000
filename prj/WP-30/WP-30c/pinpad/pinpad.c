
/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : pinpad.c
 * bfief              : 
 * Author             : yehf()  
 * Email              : yehf@itep.com.cn
 * Version            : V0.00
 * Date               : 10/7/2014 4:21:49 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"

/*
 *功能：提示上位机开启按键窗口 
 *参数： 
 *
 *注:
 */
/*
 *功能：标准返回判断 
 *参数： 
 *
 *注:
 */
int unpackCmd(const uint8_t *inBuf,uint32_t inLen, MCUPCK *res, uint8_t *outBuf, uint32_t *outLen);
uint8_t ctc_standard_response(uint8_t *cmd, uint32_t cmd_len, uint8_t compare_cmd, 
                              uint8_t compare_subcmd, uchar* output, int* outputlen, 
                              MCUPCK *packhead)
{
    MCUPCK res;
    uint32_t outlen;
    uint32_t ret = 0;
    uint16_t respose;
    uint8_t data_temp[2];
    uint8_t outbuf[1024];

    ret = unpackCmd(cmd, cmd_len, &res, outbuf, &outlen);
    memcpy((uchar *)packhead, &res, sizeof(MCUPCK));
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

    if ((res.cmdcls == compare_cmd)&&(res.cmdop == compare_subcmd)) {
        // 要求有返回值 
        if ((outbuf != NULL) && (outputlen != NULL)) {
            memcpy(output, outbuf, outlen); 
            *outputlen = outlen;
        }
        return  RET_OK;
    }
    
    TRACE("***err.cmdcls:%x,err.cmdop:%x\r\n", res.cmdcls, res.cmdop) ;
    TRACE("***exp.cmdcls:%x,exp.cmdop:%x\r\n", compare_cmd, compare_subcmd) ;
    ret = RET_UNKNOWN;
RET:
    return ret;
}


uint8_t ctc_pin_strstart_response(uint8_t *cmd, uint32_t cmd_len)
{
    MCUPCK res;
    return  ctc_standard_response(cmd, cmd_len, CMD_PED , CMD_PED_GET_PINSTR, NULL, NULL, &res);
}

/*
 *功能：pin提示输入密钥，提示上位机开启按键窗口,上位机接收到命令后，回送信息 
 *参数： 
 *注:
 */
int ctc_pin_getstrstart_report(s_pin_popcontent *content_info) 
{
    MCUPCK pinpck;
    int pos, e_pos, s_pos;
    int ret;
    int resendnum = 0;
    int starttime = 0;
    int data_vail_len = 0;
    int len;
    uint8_t buffer[1024];    
START:
    // 发送帧
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, (uchar *)content_info, sizeof(s_pin_popcontent));
    pinpck.len = sizeof(s_pin_popcontent)+4;
	pinpck.cmdcls = CMD_PED;
    pinpck.cmdop = CMD_PED_GET_PINSTR;
    pinpck.sno1 = 0;
    pinpck.sno2 = 1;
    ctc_send_frame(pinpck, buffer);

    // 同步等待回送
    starttime = sys_get_counter(); 
    while(1)
    {
        if ( sys_get_counter() - starttime > 2000 ) {
            if ( resendnum >= 3 ) {
                return RET_TIMEOUT;
            }     
            // 重新计时
            starttime = sys_get_counter();
            resendnum ++;
            goto START;
        }
        if (ctc_uart_dma_check()) {
            pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
            data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
            ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
            if (ret == RET_OK) {
                drv_dma_stop();
                len = e_pos - s_pos + 1;
                ret = ctc_pin_strstart_response(gwp30SysBuf_c.work, len);
                data_vail_len = 0;
                memset(gwp30SysBuf_c.work, sizeof(gucBuff), 0);
                ctc_uart_restart();
                // 接收到返回指令
                if ( RET_OK == ret ) {
                    return RET_OK; 
                }else{
                    // 收到异常指令
                    TRACE("rece ctc_pin_getstrstart_report cmd err\r\n");
                    return RET_UNKNOWN;
                //                    buffer[0] = (uint8_t)CMDST_OTHER_ER & 0xFF;
//                    buffer[1] = (uint8_t)(CMDST_OTHER_ER >> 8) & 0xFF;
//                    res.len = MIN_CMD_RESPONSE;
//                    ctc_send_frame(res, buffer);
                }
            }
        }
    }
}

uint8_t ctc_pin_strend_response(uint8_t *cmd, uint32_t cmd_len)
{
    MCUPCK res;
    return  ctc_standard_response(cmd, cmd_len, CMD_PED, CMD_PED_PINSTR_END, NULL, NULL, &res);
}

int ctc_pin_getstrend_report(uchar status) 
{
    MCUPCK pinpck;
    int pos, e_pos, s_pos;
    int ret;
    int resendnum = 0;
    int starttime = 0;
    int data_vail_len = 0;
    int len;
    uint8_t buffer[10];

START:
    memset(buffer, 0, sizeof(buffer));
    pinpck.len = 1+4;
	pinpck.cmdcls = CMD_PED;
	pinpck.cmdop = CMD_PED_PINSTR_END;
	pinpck.sno1 = 0;
	pinpck.sno2 = 1;
    ctc_send_frame(pinpck, &status);

// 同步等待回送
    starttime = sys_get_counter(); 
    while(1)
    {
        if ( sys_get_counter() - starttime > 2000 ) {
            if ( resendnum >= 3 ) {
                return RET_TIMEOUT;
            }     
            // 重新计时
            starttime = sys_get_counter();
            resendnum ++;
            goto START;
        }
        if (ctc_uart_dma_check()) {
            pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
            data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
            ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
            if (ret == RET_OK) {
                drv_dma_stop();
                len = e_pos - s_pos + 1;
                ret = ctc_pin_strend_response(gwp30SysBuf_c.work, len);
                data_vail_len = 0;
                memset(gwp30SysBuf_c.work, sizeof(gucBuff), 0);
                ctc_uart_restart();
                // 接收到返回指令
                if ( RET_OK == ret ) {
                    return RET_OK; 
                }else{
//                    // 解析正常，但所预期接收指令有误
//                    buffer[0] = (uint8_t)CMDST_OTHER_ER & 0xFF;
//                    buffer[1] = (uint8_t)(CMDST_OTHER_ER >> 8) & 0xFF;
//                    pinpck.len = MIN_CMD_RESPONSE;
//                    ctc_send_frame(pinpck, buffer);
                }
            }
        }
    }
}

uint8_t ctc_pin_showkeynum_response(uint8_t *cmd, uint32_t cmd_len)
{
    MCUPCK res;
    return  ctc_standard_response(cmd, cmd_len, CMD_PED, CMD_PED_DIS_KEYNUM, NULL, NULL, &res);
}
int ctc_pin_showkeynum_report(uchar keynum) 
{
    MCUPCK pinpck;
    int pos, e_pos, s_pos;
    int ret;
    int resendnum = 0;
    int starttime = 0;
    int data_vail_len = 0;
    int len;
    uint8_t buffer[10];

START:
    memset(buffer, 0, sizeof(buffer));
    pinpck.len = 1+4;
	pinpck.cmdcls = CMD_PED;
	pinpck.cmdop = CMD_PED_DIS_KEYNUM;
	pinpck.sno1 = 0;
	pinpck.sno2 = 1;
    ctc_send_frame(pinpck, &keynum);

// 同步等待回送
    starttime = sys_get_counter(); 
    while(1)
    {
        if ( sys_get_counter() - starttime > 2000 ) {
            if ( resendnum >= 3 ) {
                return RET_TIMEOUT;
            }     
            // 重新计时
            starttime = sys_get_counter();
            resendnum ++;
            goto START;
        }
        if (ctc_uart_dma_check()) {
            pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
            data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
            ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
            if (ret == RET_OK) {
                drv_dma_stop();
                len = e_pos - s_pos + 1;
                ret = ctc_pin_showkeynum_response(gwp30SysBuf_c.work, len);
                data_vail_len = 0;
                memset(gwp30SysBuf_c.work, sizeof(gucBuff), 0);
                ctc_uart_restart();
                // 接收到返回指令
                if ( RET_OK == ret ) {
                    return RET_OK; 
                }else{
//                    // 解析正常，但所预期接收指令有误
//                    buffer[0] = (uint8_t)CMDST_OTHER_ER & 0xFF;
//                    buffer[1] = (uint8_t)(CMDST_OTHER_ER >> 8) & 0xFF;
//                    pinpck.len = MIN_CMD_RESPONSE;
//                    ctc_send_frame(pinpck, buffer);
                }
            }
        }
    }
}


uint8_t ctc_getkey_notify(uint8_t *cmd, uint32_t cmd_len, uchar* keyvalue, MCUPCK *packhead)
{
    int keylen;
    return ctc_standard_response(cmd, cmd_len, CMD_PED , CMD_PED_NOTIFY_KEY, keyvalue, &keylen, packhead);
}

uint8_t ctc_endgetkey_notify(uint8_t *cmd, uint32_t cmd_len, MCUPCK *packhead)
{
    return ctc_standard_response(cmd, cmd_len, CMD_PED , CMD_PED_PINSTR_END, NULL, NULL, packhead);
}

uint8_t ctc_pinpadclose_notify(uint8_t *cmd, uint32_t cmd_len, MCUPCK *packhead)
{
    return ctc_standard_response(cmd, cmd_len, CMD_PED , CMD_PED_CLOSE, NULL, NULL, packhead);
}

/*
 *功能：pin提示输入密钥，提示上位机开启按键窗口,上位机接收到命令后，回送信息 
 *参数： 
 *注:
 */
int ctc_getkey(int timeout) 
{
    MCUPCK pinpck;
    int pos, e_pos, s_pos;
    int ret;
//    int resendnum = 0;
    int starttime = 0;
    int data_vail_len = 0;
    int len;
    int notify_endflag = 0;
    uchar keyvalue;
    uchar buffer[10];

    memset(buffer, 0, sizeof(buffer));

    starttime = sys_get_counter(); 
    while ( 1 ) {
        if ( sys_get_counter() - starttime > timeout ) {
            TRACE("%s timeout\r\n", __FUNCTION__);
            return KEY_TIMEOUT;
        }

        if (ctc_uart_dma_check()) {
            pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
            data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
            ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
            if (ret == RET_OK) {
                drv_dma_stop();
                len = e_pos - s_pos + 1;
                ret = ctc_getkey_notify(gwp30SysBuf_c.work, len, &keyvalue, &pinpck);
                // 如果接收到指令是通知结束按键操作必须作响应
                if ( ret == RET_UNKNOWN ) {
                    // 如果接收到指令是通知结束按键操作必须作响应
                    ret = ctc_pinpadclose_notify(gwp30SysBuf_c.work, len, &pinpck);
                    if ( ret == RET_UNKNOWN ) {
                        ret = ctc_endgetkey_notify(gwp30SysBuf_c.work, len, &pinpck);
                        if ( ret == RET_UNKNOWN ) {
                                // 处理其他指令
                                ret = ctc_recev_frame(1, gwp30SysBuf_c.work, len);
                                data_vail_len = 0;
                                memset(gwp30SysBuf_c.work, sizeof(gucBuff), 0);
                                ctc_uart_restart();
                                if ( RET_OK == ret  ) {
                                //                        notify_endflag = 1;
                                return KEY_DEALOTHER;
                            }else{
                                return KEY_INVALID; // 暂时认为无效键值操作
                            }
                        }else{
                            notify_endflag = 1;
                        }
                    }else{
                        notify_endflag = 1;
                    }
                }
                // 接收到返回指令
                data_vail_len = 0;
                memset(gwp30SysBuf_c.work, sizeof(gucBuff), 0);
                ctc_uart_restart();
                if ( RET_OK == ret ) {
//                    if ( notify_endflag != 0 ) {
//                        TRACE("rece ctc_endgetkey_notify\r\n");
//                        // 解析正常，但所预期接收指令有误,返回接收正常指令
//                        buffer[0] = (uint8_t)CMDST_OK & 0xFF;
//                        buffer[1] = (uint8_t)(CMDST_OK >> 8) & 0xFF;
//                        pinpck.len = MIN_CMD_RESPONSE;
//                        ctc_send_frame(pinpck, buffer);
//                        return KEY_TIMEOUT; // 暂时认为超时退出
//                    }
                    buffer[0] = (uint8_t)CMDST_OK & 0xFF;
                    buffer[1] = (uint8_t)(CMDST_OK >> 8) & 0xFF;
                    pinpck.len = MIN_CMD_RESPONSE;
                    ctc_send_frame(pinpck, buffer);
                    if ( notify_endflag != 0 ) {
                        return KEY_TIMEOUT; // 暂时认为超时退出
                    }
                    // 正确返回键值
                    return keyvalue;
                }
            }
        }
    }
}

extern int ctc_getkey(int timeout); 
int pinpad_getkey (int mode,int timeout_ms)
{
//    int ret;

    return ctc_getkey(timeout_ms); 
}		/* -----  end of function pinpad_getkey  ----- */

#define PINEND_STATUS_NORMAL    0   // 0：正常结束  
#define PINEND_STATUS_TIMEOUT   1   // 1：超时      
#define PINEND_STATUS_CANCEL    2   // 2：取消键退出
#define PINEND_STATUS_OTHER     3   // 3：其他错误  

extern int ctc_pin_getstrstart_report(s_pin_popcontent *content_info); 
extern int ctc_pin_getstrend_report(uchar status); 
extern int ctc_pin_showkeynum_report(uchar keynum); 
/******************************************************************
 * param: 
 *      mode: bit 0      bit 1     bit 2  
 *            按键伴音  按键震动   用于同时是否在密码键盘显示长度 
 *****************************************************************/
uint8_t KB_GetPINStr(uint8_t mode, uint8_t ucMinLen, uint8_t ucMaxLen, uint32_t uiTimeOut, uint32_t *puiRetLen, uint8_t *pucGetBuf)
{
    int ret;
    uint8_t ucKey,ucKeyBuf[16], len = 0;
    //    uint8_t gets_buf[16];
    int curTime = 0;
    int beginTime = 0;
//    ushort randlen;
    s_pin_popcontent dispinfo;
    if (uiTimeOut == 0)
    {
//        TRACE("%s param err timeout\r\n", __FUNCTION__);
        uiTimeOut = 0xffffffff;     // admit infinite waiting
        dispinfo.timeout = 0;       // 0,通知为无限等待
//        return (2);
    }else{
        dispinfo.timeout = uiTimeOut/1000;
    }

    TRACE("------------------timeout:%d\r\n", uiTimeOut);
    // 确保打开dma准备接收数据
    ctc_uart_restart();
    memset(gwp30SysBuf_c.work, 0, sizeof(gucBuff));
    
    dispinfo.tone = mode & 0x01;
    dispinfo.vibrative = (mode & 0x02)>>1;
    dispinfo.minlen = ucMinLen;
    dispinfo.maxlen = ucMaxLen;
    dispinfo.type = 0;
    dispinfo.mode = 0;
    dispinfo.showlenflag = (mode & 0x04)>>2;
    // 随机数获取
    memset(&dispinfo.random[0], 0, 8);
    RNG_FillRandom(&dispinfo.random[0],8);

    ret = ctc_pin_getstrstart_report(&dispinfo); 
    if ( ret != RET_OK) {
        TRACE("%s ctc_pin_getstrstart_report timeout\r\n", __FUNCTION__);
        return 3;
    }
    // 等待按键
    beginTime = sys_get_counter();	
    //    KB_Flush();
    //    gets_buf[0] = '_';
    //    gets_buf[1] = 0;
    //    pinpad_Display(ucLine, (const char *)gets_buf);
    //TRACE("\r\n timeout:%d-%d",beginTime,uiTimeOut);
    while (1)
    {
        curTime = sys_get_counter();
        if ((uiTimeOut > 0) && ((curTime - beginTime) >= uiTimeOut))
        {
            //TRACE("\r\n curTime:%d",curTime);
            ctc_pin_getstrend_report(PINEND_STATUS_TIMEOUT);
            return (2);
        }

    //        if (uiTimeOut == 0)
    //        {
    //            return (2);
    //        }

    //        if (KB_Hit())
    //        {
        ucKey = pinpad_getkey(1,uiTimeOut);
        TRACE("get the key value:%d\r\n", ucKey);
        if (ucKey >= '0' && ucKey <= '9')
        {
            if (len >= ucMaxLen)
            {
                //                    BuzzerOn(800);
                continue;
            }

            if (len < ucMaxLen)
            {
                ucKeyBuf[len++] = ucKey;
                ret = ctc_pin_showkeynum_report(len);
                if ( ret == RET_TIMEOUT ) {
                    TRACE("\r\n %s, %d timeout\r\n", __FUNCTION__, __LINE__);
                    ctc_pin_getstrend_report(PINEND_STATUS_TIMEOUT);
                    return 2;
                }
            }
        }
        else if (ucKey == KEY_BACKSPACE )
        {
            if ( len > 0 ) {
                len--;
                ucKeyBuf[len] = '\0';
                ret = ctc_pin_showkeynum_report(len);
                if ( ret == RET_TIMEOUT ) {
                    TRACE("\r\n %s, %d timeout\r\n", __FUNCTION__, __LINE__);
                    ctc_pin_getstrend_report(PINEND_STATUS_TIMEOUT);
                    return 2;
                }
            }
            //                len = 0;
            //                memset(ucKeyBuf, 0, sizeof(ucKeyBuf));
            //                ucKeyBuf[0] = 0;
            //                beginTime = sys_get_counter();
        }
        else if (ucKey == KEY_ENTER)
        {
//            if((len == 0) && (ucMinLen == 0))
            if(len == 0)
            {
                *puiRetLen = len;
                TRACE("\r\n enter ok but len 0\r\n");
                ctc_pin_getstrend_report(PINEND_STATUS_NORMAL);
                return 0;
            }
            // 输入密码参数必须大于4， 否则确定不允许退出
            else if (len < ucMinLen || len < 4)
            {
                //                    BuzzerOn(800);
                continue;
            }
            memcpy(pucGetBuf, ucKeyBuf, len);
            *puiRetLen = len;
            TRACE("\r\n %s, %d enter ok\r\n", __FUNCTION__, __LINE__);
            ctc_pin_getstrend_report(PINEND_STATUS_NORMAL);
            return 0;
        }
        else if (ucKey == KEY_CANCEL)
        {
            *puiRetLen = 0;
            TRACE("\r\n cancel exit\r\n");
            ctc_pin_getstrend_report(PINEND_STATUS_CANCEL);

            return 1;
        }
        else if ((ucKey == KEY_TIMEOUT)|| (ucKey == KEY_INVALID))
        {
            *puiRetLen = 0;
            ctc_pin_showkeynum_report(0);
            TRACE("\r\n %s, %d timeout\r\n", __FUNCTION__, __LINE__);
            ctc_pin_getstrend_report(PINEND_STATUS_TIMEOUT);
            return 2;
        }
        //            memset(gets_buf, '*', len);
        //            gets_buf[len] = '_';
        //            gets_buf[len + 1] = 0;
        //            pinpad_Display(ucLine, (const char *)gets_buf);
        //        }
    }
}

uint8_t ctc_emv_lcd_response(uint8_t *cmd, uint32_t cmd_len, uchar* timeout)
{
    int timeoutlen;
    MCUPCK res;
    int ret;
    ret = ctc_standard_response(cmd, cmd_len, CMD_EMV , CMD_EMV_DIS, timeout, &timeoutlen, &res);
//    if ( ret == 0 ) {
//        TRACE_BUF("get ctc_standard_response:", timeout, timeoutlen);
//    }
    return ret;
}

int ctc_lcd_emv_report(uchar * input, int inputlen, int* resp_timeout) 
{
    MCUPCK pinpck;
    int pos, e_pos, s_pos;
    int ret;
    int resendnum = 0;
    int starttime = 0;
    int data_vail_len = 0;
    int len;
    uint8_t buffer[20];    

// 确保打开dma准备接收数据
    memset(gwp30SysBuf_c.work, sizeof(gucBuff), 0);
    ctc_uart_restart();
START:
    // 发送帧
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, (uchar *)input, inputlen);
    pinpck.len = inputlen + 4;
	pinpck.cmdcls = CMD_EMV;
    pinpck.cmdop = CMD_EMV_DIS;
    pinpck.sno1 = 0;
    pinpck.sno2 = 1;
    ctc_send_frame(pinpck, buffer);

    // 同步等待回送
    starttime = sys_get_counter(); 
    while(1)
    {
        if ( sys_get_counter() - starttime > 2000 ) {
            if ( resendnum >= 3 ) {
                return RET_TIMEOUT;
            }     
            // 重新计时
            starttime = sys_get_counter();
            resendnum ++;
            goto START;
        }
        if (ctc_uart_dma_check()) {
            pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
            data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
            ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
            if (ret == RET_OK) {
                drv_dma_stop();
                len = e_pos - s_pos + 1;
                ret = ctc_emv_lcd_response(gwp30SysBuf_c.work, len, buffer);
                ctc_uart_restart();
                data_vail_len = 0;
                memset(gwp30SysBuf_c.work, 0, sizeof(gucBuff));
                // 接收到返回指令
                if ( RET_OK == ret ) {
                    *resp_timeout = (buffer[2])|(buffer[3]<<8)|(buffer[4]<<16)|(buffer[5]<<24);
                    return RET_OK; 
                }else{
                    // 收到异常指令
                    TRACE("rece ctc_emv_lcd_response cmd err\r\n");
//                    return RET_UNKNOWN;
                    //                    buffer[0] = (uint8_t)CMDST_OTHER_ER & 0xFF;
                    //                    buffer[1] = (uint8_t)(CMDST_OTHER_ER >> 8) & 0xFF;
                    //                    res.len = MIN_CMD_RESPONSE;
                    //                    ctc_send_frame(res, buffer);
                }
            }
        }
    }
}

uint8_t ctc_emv_key_response(uint8_t *cmd, uint32_t cmd_len, uchar* output, int* outputlen)
{
    MCUPCK res;
    uint8_t ret;
    uint8_t respbuf[2];
    ret = ctc_standard_response(cmd, cmd_len, CMD_EMV , CMD_EMV_DIS_RESPONSE, output, outputlen, &res);
    if ( RET_OK == ret ) {
        res.len = 6;
        respbuf[0]=0;
        respbuf[1]=0;
        ctc_send_frame(res, respbuf);
    }
    return ret;
}
/*
 *********************************************************************
 *       function:  lcd_EmvDisp.c
 *    Description:  
 *        Version:  1.0
 *          param:  s_lcd_emv结构体
 *           （1）   frametype：0-文本框 1-选择框 2-列表框 3-列表选择框
 *           （2）   button: 1有效 D0-选择键 D1-取消键 D2-方向键 D3-功能键
 *           （3）   titleAlige: 0-左对齐 1-中间对齐 2-右对齐
 *           （4）   contentAlige：0-左对齐 1-中间对齐 2-右对齐
 *           （5）   title：框标题
 *           （6）   content：框内显示内容，不同行用\n分割。
 *           （7）   rfu：预留。
 *           ret :   0xff   超时
 *                   0x01   确认
 *                   0x00   取消
 *                   ASCII  对应ASCII键值
 *********************************************************************/
int lcd_emvdisp(s_lcd_emv * lcd_emvdis, int distimeout) 
{
    int pos, e_pos, s_pos;
    int ret;
    int starttime = 0;
    int data_vail_len = 0;
    int len=0;
    uint8_t buffer[1024];    
    int resp_timeout; // ms
    uint8_t outbuf[20];
    int outlen;

    memset(buffer, 0, sizeof(buffer));
    memset(outbuf, 0, sizeof(buffer));

    TRACE("enter lcd_emvdisp*************\r\n");
    if ( lcd_emvdis->frametype > 4 ) {
        return  -RET_PARAM;
    }
    if ((lcd_emvdis->button & 0xf0) != 0) {
        return  -RET_PARAM;
    }
    if (lcd_emvdis->titlealign > 2) {
        return  -RET_PARAM;
    }
    if (lcd_emvdis->contextalign > 2) {
        return  -RET_PARAM;
    }
    if ((lcd_emvdis->titlelen > 0)&&(lcd_emvdis->title == NULL)) {
        return  -RET_PARAM;
    }
    if ((lcd_emvdis->contextlen > 0)&&(lcd_emvdis->context == NULL)) {
        return  -RET_PARAM;
    }
    buffer[len] = lcd_emvdis->frametype; 
    len++;
    buffer[len] = lcd_emvdis->button; 
    len++;
    buffer[len] = lcd_emvdis->button; 
    len++;
    buffer[len] = lcd_emvdis->contextalign; 
    len++;
    // rfu 2bytes
    buffer[len] = 0; 
    buffer[len+1] = 0; 
    len+=2;
    // title len 
    buffer[len] = lcd_emvdis->titlelen & 0xff ; 
    buffer[len + 1] = (lcd_emvdis->titlelen>>8) & 0xff ; 
    len+=2;
    memcpy(&buffer[len], lcd_emvdis->title, lcd_emvdis->titlelen);
    len+=lcd_emvdis->titlelen;
    memcpy(&buffer[len], lcd_emvdis->context, lcd_emvdis->contextlen);
    len+=lcd_emvdis->contextlen;
    // 
    ret = ctc_lcd_emv_report(buffer, len, &resp_timeout); 
    if ( ret != RET_OK ) {
        return -ret; 
    }

//    TRACE("enter lcd_emvdisp*************step1\r\n");
    // 如果需要按键返回，等待按键返回
    if ( resp_timeout != 0 ) {
        TRACE("enter resp_timeout:%d\r\n",resp_timeout);
        starttime = sys_get_counter(); 
        while(1)
        {
            if ( sys_get_counter() - starttime > resp_timeout ) {
                return -RET_TIMEOUT;
            }

            if (ctc_uart_dma_check()) {
                pos = drv_dma_get_daddr(UART4_DMA_CHANNEL);
                data_vail_len =  pos -(uint32_t)(&gwp30SysBuf_c.work[0]);
                ret = ctc_frame_check(gwp30SysBuf_c.work, data_vail_len, &s_pos, &e_pos);
                if (ret == RET_OK) {
                    drv_dma_stop();
                    len = e_pos - s_pos + 1;
                    ret = ctc_emv_key_response(gwp30SysBuf_c.work, len, outbuf, &outlen);
                    data_vail_len = 0;
                    memset(gwp30SysBuf_c.work, sizeof(gucBuff), 0);
                    ctc_uart_restart();
                    // 接收到返回指令
                    if ( RET_OK == ret ) {
//                        TRACE("enter lcd_emvdisp*************step2\r\n");
                        TRACE("get the buf[0]:%d, len:%d\r\n",outbuf[0], outlen);
                        return outbuf[0]; 
                    }else{
                        // 收到异常指令
//                        TRACE("enter lcd_emvdisp*************step3\r\n");
                        TRACE("rece ctc_emv_key_response cmd err\r\n");
//                        TRACE_BUF("key resp", outbuf, outlen);
                        //                    buffer[0] = (uint8_t)CMDST_OTHER_ER & 0xFF;
                        //                    buffer[1] = (uint8_t)(CMDST_OTHER_ER >> 8) & 0xFF;
                        //                    res.len = MIN_CMD_RESPONSE;
                        //                    ctc_send_frame(res, buffer);
                    }
                }
            }
        }
    }
    return -RET_UNKNOWN;
}

uchar PinEncrypt(uint uiPINFormat,
	uint uiKeyLen,uchar * pucKey,
	uint uiLenIn, uchar * pvCardNo, uchar * pvDataIn, uint * puiOutLen, uchar * pvDataOut)
{
	uchar ucBlock[20], ucTemp[26];
	uchar *pucDataIn, *pucDataOut;
	uint i, j, uiLen;

	pucDataIn = pvDataIn;
	pucDataOut = pvDataOut;

	uiLen = uiLenIn;

	for (i = 0; i < uiLen; i++)
	{
		ucBlock[i] = pucDataIn[i];
	}
	for (i = uiLen; i < 16; i++)
	{
		ucBlock[i] = 0xff;				// not enough 16 filled with 0XFF;
	}
	
	//To Form BLOCK1
	ucTemp[0] = '0';
	ucTemp[1] = '0' + uiLen;
	memcpy(ucTemp + 2, ucBlock, 14);
	for (i = 0, j = 0; i < 8; i++, j += 2)
	{
		ucBlock[i] = (ucTemp[j] << 4) + (ucTemp[j + 1] & 0x0f);
	}
#ifdef EM_DEBUG	
	vDispBufTitle("BLOCK1", 8, 0, ucBlock);
#endif
	//To Form BLOCK2    
	ucTemp[0] = ucTemp[1] = 0;
	for (i = 2, j = 0; i < 8; i++, j += 2)
	{
		ucTemp[i] = (pvCardNo[j] << 4) + (pvCardNo[j + 1] & 0x0f);
	}
#ifdef EM_DEBUG	
	vDispBufTitle("BLOCK2", 8, 0, ucTemp);
#endif
	//To get PIN-BLOCK
	for (i = 0; i < 8; i++)
	{
		ucTemp[i] ^= ucBlock[i];
	}
#ifdef EM_DEBUG	
	vDispBufTitle("BLOCK3", 8, 0, ucTemp);
#endif	
	des_encrypt_ecb(ucTemp, ucBlock, 8, pucKey, uiKeyLen / 8);
#ifdef EM_DEBUG	
	vDispBufTitle("BLOCK4", 8, 0, ucBlock);
#endif
	if(!uiPINFormat)
	{
//		SI_ucParseStr(0,8,ucBlock,pucDataOut);
        memcpy(pucDataOut,ucBlock,8);
		*puiOutLen = 8;
	}
	else
	{
		memcpy(pucDataOut,ucBlock,8);
		*puiOutLen = 8;
	}
#ifdef EM_DEBUG	
	vDispBufTitle("BLOCK5", *puiOutLen, 0, pucDataOut);
#endif	
	return EM_SUCCESS;
}

uint Rsa_calt_pub(int inlen, const uchar *in, R_RSA_PUBLIC_KEY *keyinfo,int *outlen, uchar *out)
{
    rsa_context rsa;
    uint32_t ret=0;
    uint32_t i;
    uint8_t *p=NULL;
    //TRACE_BUF("N",(uint8_t *)keyinfo->modulus,sizeof(keyinfo->modulus));
    //TRACE_BUF("E",(uint8_t *)keyinfo->exponent,sizeof(keyinfo->exponent));
    rsa_init( &rsa, RSA_PKCS_V15, 0 );
//    big_endian2little_endian(keyinfo->exponent,keyinfo->exponent);
    ret = mpi_read_big_endian(&rsa.N,keyinfo->modulus,keyinfo->bits>>3);
    ret += mpi_read_big_endian(&rsa.E,keyinfo->exponent,sizeof(keyinfo->exponent));
    if (ret) {
        TRACE("\nRead rsa error");
        goto PUB_CLEAN;
    }
    rsa.len = ( mpi_msb( &rsa.N ) + 7 ) >> 3;
    //TRACE_BUF("N1",(uint8_t *)rsa.N.p,rsa.N.n<<2);
    //TRACE_BUF("E1",(uint8_t *)rsa.E.p,rsa.E.n<<2);
    //TRACE("\n-|rsa len:%d",rsa.len);
    // 输入e=65537采用大端格式(00 01 00 01)
    // RSA库采用小端格式(01 00 01 00)
    ret = rsa_check_pubkey(&rsa);
    if( ret != 0)
    {
        TRACE("\nrsa key error:%d",ret);
        ret = 1;
        goto PUB_CLEAN;
    }
    *outlen = 0;
    if (inlen >= rsa.len)
    {
        for (i=0; i<inlen; i += rsa.len)
        {
            if (rsa_public(&rsa,in+i,out+i)) {
                TRACE("\n-|Erro:rsa pub error");
                ret = -1;
                goto PUB_CLEAN;
            }
            *outlen += rsa.len;
        }
    }
    if (*outlen < inlen) 
    {
        if( ( p = (uint8_t *) malloc(rsa.len+4) ) == NULL ) {
            ret = -1;
            goto PUB_CLEAN;
        }
        memset(p,0,rsa.len);
        i = inlen-*outlen;
        memcpy(p+rsa.len-i,in+*outlen,i);
        if (rsa_public(&rsa,p,out+*outlen)) {
            TRACE("\n-|Erro:rsa pub error");
            free(p);
            p = NULL;
            ret = -1;
            goto PUB_CLEAN;
        }
        *outlen += rsa.len;
        free(p);
        p = NULL;
    }
//    TRACE_BUF("Chiper cau",out,rsa.len);
PUB_CLEAN:
    rsa_free( &rsa );
    return ret;
}
//in: cipher_text  len = keyinfo->bits/8
//keyinfo->modulus:  pubkey+00
//out:cipher_text  len = keyinfo->bits/8
uint Rsa_calt_pri(int inlen, uchar *in, R_RSA_PRIVATE_KEY *keyinfo,int *outlen, uchar *out)
{
#if 0
    rsa_context rsa;
    uint32_t ret=0;
    uint32_t i;
    uint8_t *p=NULL;

    rsa_init( &rsa, RSA_PKCS_V15, 0 );
    ret = mpi_read_little_endian(&rsa.N,keyinfo->tPubinfo.modulus,keyinfo->tPubinfo.bits>>3);
    ret = mpi_read_little_endian(&rsa.D,keyinfo->prikey,keyinfo->tPubinfo.bits>>3);
    ret += mpi_read_little_endian(&rsa.E,keyinfo->tPubinfo.exponent,sizeof(keyinfo->tPubinfo.exponent));
    if (ret) {
        TRACE("\nRead rsa error");
        goto PUB_CLEAN;
    }
    rsa.len = ( mpi_msb( &rsa.N ) + 7 ) >> 3;
//    TRACE_BUF("N",(uint8_t *)rsa.N.p,rsa.N.n<<2);
//    TRACE_BUF("E",(uint8_t *)rsa.E.p,rsa.E.n<<2);
//    TRACE("\n-|rsa len:%d",rsa.len);
    if( rsa_check_pubkey(&rsa) != 0)
    {
        TRACE("\nrsa key error");
        ret = 1;
        goto PUB_CLEAN;
    }
    *outlen = 0;
    if (inlen >= rsa.len)
    {
        for (i=0; i<inlen; i += rsa.len)
        {
            if (rsa_private(&rsa,in+i,out+i)) {
                TRACE("\n-|Erro:rsa pub error");
                ret = -1;
                goto PUB_CLEAN;
            }
            *outlen += rsa.len;
        }
    }
    if (*outlen < inlen) 
    {
        if( ( p = (uint8_t *) malloc(rsa.len+4) ) == NULL ) {
            ret = -1;
            goto PUB_CLEAN;
        }
        memset(p,0,rsa.len);
        i = inlen-*outlen;
        memcpy(p+rsa.len-i,in+*outlen,i);
        if (rsa_private(&rsa,p,out+*outlen)) {
            TRACE("\n-|Erro:rsa pub error");
            free(p);
            p = NULL;
            ret = -1;
            goto PUB_CLEAN;
        }
        *outlen += rsa.len;
        free(p);
        p = NULL;
    }
//    TRACE_BUF("Chiper cau",out,rsa.len);
PUB_CLEAN:
    rsa_free( &rsa );
    return ret;
#else
    return 0;
#endif
}


#if 0 

//#pragma message("|------------PINPAD----------|")
//#if defined(CFG_EXTERN_PINPAD)
//#pragma message("*** Product:MK-210 ***")
//#endif
//#if defined(CFG_INSIDE_PINPAD)
//#pragma message("*** Product:PPKB ***")
//#endif
#ifdef CFG_TAMPER 
#pragma message("*** Tamper:Enable ***")
#else
#pragma message("*** Tamper:Disable ***")
#endif
#ifdef CFG_CMD_V1
#pragma message("*** CMD:V1 ***")
#endif
#ifdef CFG_KEYCHECK_LRC 
#pragma message("*** CHECK:LRC ***")
#endif
#ifdef CFG_KEYCHECK_MAC 
#pragma message("*** CHECK:MAC ***")
#endif

typedef struct _VERSION_INFO_
{
    char product[20];				
    char version[8];			
    char time[12];					
} VERSION_INFO;
// 0-hard version 1-boot version 2-app version 3-厂家信息 4-磁头信息
uint sys_get_ver(uint mode, uint *verlen, uchar *buf)
{
    extern int sys_read_ver(int module, char *ver);
    VERSION_INFO_NEW t;
    if(mode==0)
    {
        sys_read_ver(READ_MAINB_VER,(char *)buf);
        *verlen = strlen((char *)buf);
        return 0;
    }
    else if(mode == 1)
    {
        sys_read_ver(READ_BOOT_VER,(char *)buf);
        *verlen = strlen((char *)buf);
        return 0;
    }
    else if(mode == 2)
    {
        memset((uchar *)&t,0,sizeof(t));
        strcpy((char *)t.product,(char *)k_SonTermName);
        memcpy((uchar *)t.version,(uchar *)gCtrlVerInfo.version,sizeof(gCtrlVerInfo.version));
        memcpy((uchar *)t.time,(uchar *)gCtrlVerInfo.time,sizeof(gCtrlVerInfo.time));
        memcpy((uchar *)buf,(uchar *)&t,sizeof(VERSION_INFO_NEW));
        *verlen = sizeof(VERSION_INFO_NEW);
        return 0;
    }
    else if(mode == 3)
    {
        memset(buf,0xFF,16);
        //04:厂家编码  01:键盘型号(A命令集) 09:键盘版本
        memcpy(buf,"\x04\x01\x01",3);
        *verlen = 16;
        return 0;
    }
#if (defined CFG_MAGCARD)
    else if(mode == 4)
    {
        buf[0] = mag_get();
        *verlen = 1;
        return 0;
    }
#endif
    else
    {
        return 1;
    }
}



// mode=0 bin recover  
// mode=1 command recover 
// mode=2 del all 
uchar sys_recover(uint mode)
{
    uint appno,Crc;
	memset(gucBuff,0xFF,OFFSET_SYSZONE_CHECK);
	if(mode == 0)
	{
		s_read_syszone(OFFSET_FIRST_FLAG,OFFSET_SN-OFFSET_FIRST_FLAG,&gucBuff[OFFSET_FIRST_FLAG]);
	}
	else if(mode == 1)
	{
		s_read_syszone(OFFSET_APP_CHECK,OFFSET_SN-OFFSET_APP_CHECK,&gucBuff[OFFSET_APP_CHECK]);
	}
	else if(mode == 2)
	{
		goto sys_recover_0;
	}
	s_read_syszone(OFFSET_PSW,OFFSET_FSK_CTRL-OFFSET_PSW,&gucBuff[OFFSET_PSW]);
	//del appinfo
	Crc = (uint)crc_calculate32by16((ushort *)gucBuff,OFFSET_SYSZONE_CHECK/2);
    memcpy(&gucBuff[OFFSET_SYSZONE_CHECK],(uchar *)&Crc,LEN_SYSZONE_CHECK);
sys_recover_0:	
	if(eraseFlashPage(SA_SYSZONE/2))
	{
		return 1;
	}
	if(writeFlashPage(SA_SYSZONE/2,gucBuff,SYSZONE_LEN))
	{
		return 1;
	}
	if(save_syszone_back(gucBuff))
	{
		return 1;
	}
	for(appno=1;appno<=NUM_APP;appno++)
	{
        PCI_ClearAppKey(appno);
	}
	PCI_InitPublicKey(3);
	Fac_SRAM(2);
#ifdef CFG_CMD_V1
	if(create_default_V1APP())
	{
		return 3;
	}
#endif
    return 0;
}

uchar app_recover(void)
{
    uint appno,ret;
    uint32_t flag;
    uint8_t length;

    for(appno=1;appno<=NUM_APP;appno++)
    {
        // !0=1
        // ~0=ffffffff
        //TRACE("%d-%x-%x-%x",appno,flag,!0,~0);
        PCI_ReadKeyFromFlash(appno,KEY_TYPE_APPFLG,0,&length,(uint8_t *)&flag);
        if(flag == ~0)
        {
            continue;
        }
        else
        {
            gtCurKey.appid = appno;
            app_set_APPEK(appno,1);
            if(PKS_CheckWorkKey())
            {
                ret = app_create_again(appno);
                if(ret)
                {
                    return COMBINE8(appno, ret);
                }
            }		
        }
    }
    gtCurKey.appid = 0;
    app_set_APPEK(appno,0);
    return 0;
}
//***************************************
//  sys psw
//***************************************
uint IfValidPsw(uint len, uchar *psw)
{
    uint i;
    for(i=0;i<len;i++)
    {
        if(psw[i] < '0' || psw[i] > '9')
            return 1;
    }
    return 0;
}

// 0-not new app 1-NUM_APP:new app no  
uint app_find_newapp(void)
{
    uint i,len;
    uint flag;
    for(i=1;i<=NUM_APP;i++)
    {
        if(app_read_info(i,INFO_FLAG,&len,(uchar *)&flag) == 2)
        {
            TRACE("\r\n[find idle space:%d]",i);
            return i;            
        }
    }
    TRACE("\r\n[no idle space]");
    return 0;
}


uint app_init_APPEK(uint appno)
{
    uint ret;
    uchar key[MK_UNIT_LEN];
    if(appno == 0 || appno > NUM_APP)
        return ERROR;
#if defined(__DEBUG_AUTOINPUT__) || defined(__AUTOINPUT_PTK__)
    memset(key,appno,MK_UNIT_LEN);
#else
    RNG_FillRandom(key, MK_UNIT_LEN);
#endif
    ret = PCI_WriteKeyToFlash(appno,KEY_TYPE_APPEK,0,MK_UNIT_LEN,key);
    vDispBufTitle("initAPPEK",MK_UNIT_LEN,0,key);
    return ret;
}

uint app_set_APPEK(uint appno,uint mode)
{
    return OK;
}




#define CURRENT_APPPSWA_NUM       (FPOS(struct APP_ZONE,app[gtCurKey.appid-1].pub.psw_a)>>5)
#define CURRENT_APPPSWB_NUM       (FPOS(struct APP_ZONE,app[gtCurKey.appid-1].pub.psw_b)>>5)

// appname and psw 使用MMK加密
// appak 使用appek加密
// mode  D0~D3
//     =INFO_APPNAME  out appname(8B)
//     =INFO_PSW  out psw(16B)
//     =INFO_ALL  out appname(8B) + use A(8B) + use B(8B) + appk(24B)
//       D4~D7:  1~4
// ret=0 OK  1-Err 2-app is blank
uint app_read_info(uint appno, uint mode, uint *outlen, uchar *info)
{
    uint8_t length;
    uint len=0;
    int ret;
#ifdef DEBUG_Dx
    TRACE("\r\n[读应用%d:",appno,mode);
    if ( mode & INFO_APPNAME ) {
        TRACE("\t应用名");
    } 
    if(mode & INFO_PSW){
        TRACE("\t密码");
    } 
    if(mode & INFO_APPK){
        TRACE("\tAPPAK");
    } 
    if(mode & INFO_FLAG){
        TRACE("\t标志");
    }
    TRACE("]");
#endif
    if(appno == 0 || appno > NUM_APP)
    {
        return ERROR;
    }
    if(mode & INFO_APPNAME)
    {
        if(PCI_ReadKeyFromFlash(appno,KEY_TYPE_APPNAME,0,&length,info)) {
            return ERROR;
        }
        if (length != LEN_APP_APPNAME) {
            return ERROR;
        }
        len += LEN_APP_APPNAME;
    }
    if(mode & INFO_PSW)
    {
        if(PCI_ReadKeyFromFlash(appno,KEY_TYPE_APPPSWA,0,&length,info+len)) {
            return ERROR;
        }
        if (length != PSW_LEN) {
            return ERROR;
        }
        len += PSW_LEN;
        if(PCI_ReadKeyFromFlash(appno,KEY_TYPE_APPPSWB,0,&length,info+len)) {
            return ERROR;
        }
        if (length != PSW_LEN) {
            return ERROR;
        }
        len += PSW_LEN;
    }
    if(mode & INFO_APPK)
    {
        if (PCI_ReadKeyFromFlash(appno,KEY_TYPE_APPAK,0,&length,info+len)) {
            return ERROR;
        }
//        if (length != MK_UNIT_LEN) {
//            return ERROR;
//        }
//        len += MK_UNIT_LEN;
        len += length;
    }
    if(mode & INFO_FLAG)
    {
        ret = PCI_ReadKeyFromFlash(appno,KEY_TYPE_APPFLG,0,&length,info+len);
        if (ret > 0) {
            return 2;
        } else if(ret < 0) {
            return ERROR;
        }
        len += 4;
    }
    vDispBufTitle("Info", len, 0, info);
    *outlen = len;
    return OK;	
}

#define APP_PSWA         (FPOS(struct APP_ZONE,app[appno-1].pub.psw_a)>>5)
#define APP_PSWB         (FPOS(struct APP_ZONE,app[appno-1].pub.psw_b)>>5)
// mode  D0~D3
//     =INFO_APPNAME  out appname(8B)
//     =INFO_PSW  out psw(16B)
//     =INFO_ALL  out appname(8B) + use A(8B) + use B(8B) + appk(24B)
//       D4~D7:  1~4
//       D8:     0-V1 1-PCI
uint app_write_info(uint appno, uint mode, uchar *info)
{
    uint i,k;
    uint32_t flag;
    TRACE("\r\n[写应用%d信息%x]",appno,mode);
    vDispBufTitle(NULL, APPINFO_LEN, 0, info);
    if(appno == 0 || appno > NUM_APP)
    {
        return ERROR;
    }	
    i = 0;
    if(mode & INFO_APPNAME)
    {
        TRACE_BUF("appname",info,LEN_APP_APPNAME);
        if(PCI_WriteKeyToFlash(appno,KEY_TYPE_APPNAME,0,LEN_APP_APPNAME,info)) {
            return ERROR;
        }
        i += LEN_APP_APPNAME;
        flag = mode & APP_TYPE_PCI;
        k = app_find_newapp();
        if(!k)
        {
            return ERROR;
        }
        if(PCI_WriteKeyToFlash(appno,KEY_TYPE_APPFLG,0,4,(uint8_t*)&flag)) {
            return ERROR;
        }
    }
    if(mode & INFO_PSW)
    {
        TRACE_BUF("apppswa",info+i,PSW_LEN);
        if(PCI_WriteKeyToFlash(appno,KEY_TYPE_APPPSWA,0,PSW_LEN,info+i)) {
            return ERROR;
        }
        i += PSW_LEN;
        TRACE_BUF("apppswb",info+i,PSW_LEN);
        if(PCI_WriteKeyToFlash(appno,KEY_TYPE_APPPSWB,0,PSW_LEN,info+i)) {
            return ERROR;
        }
        i += PSW_LEN;
    }
    if(mode & INFO_APPK)
    {
        TRACE_BUF("appappak",info+i,MK_UNIT_LEN);
        if( PCI_WriteKeyToFlash(appno,KEY_TYPE_APPAK,0,MK_UNIT_LEN,info+i)) {
            return ERROR;
        }
    }

    return 0;
}
// check app type
// 0-V1 1-PCI 2-Fail
uint app_get_type(uint appno)
{
    uint8_t length;
    uint32_t flag;
    if (PCI_ReadKeyFromFlash(appno,KEY_TYPE_APPFLG,0,&length,(uint8_t *)&flag)) {
        TRACE(" app flag read err");
        return 2;
    }
    if (length != 4) {
        TRACE(" app flag len err");
        return 2;
    }
    if(flag & APP_TYPE_PCI)
    {
        TRACE(" appno%d is PCI",appno);
        return APP_PROTOCOL_PCI;
    }
    else
    {
        TRACE(" appno%d is V1",appno);
        return APP_PROTOCOL_V1;
    }
}

// return appno: 0-not find 1~4:find 
uint app_find(uchar *appname)
{
    uchar buf[LEN_APP_APPNAME];
    uint len,i;
    TRACE("\r\n finding %s",appname);
    for(i=0;i<NUM_APP;i++)
    {	
//        TRACE("[i=%d]", i);
        if(!app_read_info(i+1,INFO_APPNAME,&len,buf))
        {
            if(!memcmp(buf, appname, LEN_APP_APPNAME))
            {
                CLRBUF(buf);
                TRACE("==match==");
                return (i+1);
            }
        }
    }
    TRACE("==Not find==");
    CLRBUF(buf);
    return 0;
}
// 产生APPEK,写入应用信息和传输密钥
uint app_create(uint appno, uint mode, uchar *appinfo)
{
    uint ret;
    uchar tk[24];

    app_init_APPEK(appno);
    CLRBUF(tk);	
    ret = write_transferkey(appno,tk);
    if(ret)
    {
        TRACE("\n-wr trst error");
        return ret;
    }
    ret = write_default_sn(appno);
    if(ret)
    {
        TRACE("\n-wr default sn error");
        return ret;
    }
    ret = app_write_info(appno,mode,appinfo);
    app_set_APPEK(appno,0);
    return ret;
}


uint app_create_again(uint appno)
{
    extern uchar SI_ucRenewDefault(void);
    uchar tk[24];
    app_init_APPEK(appno);
    CLRBUF(tk);	
    if(write_transferkey(appno,tk))
    {
        return 1;
    }
    if(write_default_sn(appno))
    {
        return 2;
    }
    if(app_get_type(appno)==APP_PROTOCOL_V1)
    {
        app_open(appno);
        if(SI_ucRenewDefault())
        {
            app_close(appno);
            return 3;
        }
        app_close(appno);
    }
    else
    {
        app_set_APPEK(appno,0);
    }	
    return 0;
}

uint app_close(uint appno)
{
    if(appno)
    {
        gReadyAppid = 0;
        gAppProcolType = 0;
        gAppAuthFlag = 0;
        app_set_APPEK(appno,0);
        memset((uchar *)&gtCurKey,0,sizeof(gtCurKey));
    }
    return 0;
}

uint app_open(uint appno)
{
    uint i,flag;
    TRACE("\n-|open appno:%d",appno);
    app_set_APPEK(appno,1);
    memset((uchar *)&gtCurKey,0,sizeof(gtCurKey));	
    app_read_info(appno,INFO_FLAG,&i,(uchar *)&flag);
    gtCurKey.apptype = 1;
    if(flag & APP_TYPE_PCI)
    {
        gAppProcolType = APP_PROTOCOL_PCI;
    }
    else
    {
        gAppProcolType = APP_PROTOCOL_V1;
    }
    gtCurKey.appid = appno;
    if(PKS_CheckWorkKey())
    {
        gtCurKey.appid = 0;
        DISPPOS(0);
        return ERROR;
    }
    gWorkMode = MODE_APP;
    gAppAuthFlag = STAT_APPAUTH_OK;		
    return OK;
}

uint app_del(uint appno)
{
	TRACE("DEL:%d",appno);
	if(appno == 0 || appno > NUM_APP)
		return ERROR;	
    PCI_ClearAppKey(appno);
    return PCI_ClearPubKey(appno);
}

// 1-使能应用  0-未使能应用
uint get_appenable(void)
{
    if(gpSys->app_enable == 0x0)
    {
        TRACE("\nApp is enable");
        return 1;
    }
    else
    {
        TRACE("\nApp isn't enable");
        return 0;
    }
}
// mode 1-enable  0-disable
uint set_appenable(uint mode)
{
    uint flag;
    uchar backbuf[SYSZONE_LEN];
    if(mode == 1)
    {
        flag = 0;
        PCI_InitPublicKey(3);
#ifdef CFG_CMD_V1	
        if(create_default_V1APP()) {
            return 1;
        }

#endif
    } else
    {
        flag = ~0;
    }
    return s_write_syszone(backbuf,OFFSET_APPENABLE_FLAG,LEN_APPENABLE_FLAG,(uchar *)&flag);
}

uint write_sn(uint appno, uchar len,uchar *sn)
{		
    return PCI_WriteKeyToFlash(appno,KEY_TYPE_APPSN,0,len,sn);
}

uint write_default_sn(uint appno)
{
#if 2
    uint8_t sn[8];

    CLRBUF(sn);
	return write_sn(appno,8,sn);
#else
    return 0;
#endif
}

uint read_sn(uint appno, uchar *len,uchar *sn)
{			
    return PCI_ReadKeyFromFlash(appno,KEY_TYPE_APPSN,0,len,sn);
}

uint write_transferkey(uint appno, uchar *key)
{	
    return PCI_WriteKeyToFlash(appno,KEY_TYPE_TRSK,0,24,key);
}

uint read_transferkey(uint appno, uchar *key)
{			
    uint8_t length;

    return PCI_ReadKeyFromFlash(appno,KEY_TYPE_TRSK,0,&length,key);
}

void read_default_tranferkey(uchar *key)
{
    memset(key,0x00,KEY_UNIT_LEN);
}


uint8_t KB_GetPINStr(uint8_t ucLine, uint8_t ucMinLen, uint8_t ucMaxLen, uint32_t uiTimeOut, uint32_t *puiRetLen, uint8_t *pucGetBuf)
{
    uint8_t ucKey,ucKeyBuf[16], len = 0;
    uint8_t gets_buf[16];
    int curTime = 0;
    int beginTime = 0;
    beginTime = sys_get_counter();	
    KB_Flush();
    gets_buf[0] = '_';
    gets_buf[1] = 0;
    pinpad_Display(ucLine, (const char *)gets_buf);
    //TRACE("\r\n timeout:%d-%d",beginTime,uiTimeOut);
    while (1)
    {
        curTime = sys_get_counter();
        if ((uiTimeOut > 0) && ((curTime - beginTime) >= uiTimeOut))
        {
            //TRACE("\r\n curTime:%d",curTime);
            return (2);
        }

        if (uiTimeOut == 0)
        {
            return (2);
        }

        if (KB_Hit())
        {
            ucKey = pinpad_getkey(1,-1);
            if (ucKey >= '0' && ucKey <= '9')
            {
                if (len >= ucMaxLen)
                {
                    BuzzerOn(800);
                    continue;
                }

                if (len < ucMaxLen)
                {
                    ucKeyBuf[len++] = ucKey;
                }
            }
            else if (ucKey == KEY_CLEAR)
            {
                len = 0;
                memset(ucKeyBuf, 0, sizeof(ucKeyBuf));
                ucKeyBuf[0] = 0;
                beginTime = sys_get_counter();
            }
            else if (ucKey == KEY_ENTER)
            {
                if(len == 0)
                {
                    *puiRetLen=len;
                    return 0;
                }
                else if (len < ucMinLen)
                {
                    BuzzerOn(800);
                    continue;
                }
                memcpy(pucGetBuf, ucKeyBuf, len);
                *puiRetLen = len;
                return 0;
            }
            else if (ucKey == KEY_CANCEL)
            {
                *puiRetLen = 0;
                return 1;
            }
            memset(gets_buf, '*', len);
            gets_buf[len] = '_';
            gets_buf[len + 1] = 0;
            pinpad_Display(ucLine, (const char *)gets_buf);
        }
    }
}
void BuzzerOn(uint32_t ms)
{
#if defined(CFG_EXTERN_PINPAD) 
    if ( s_getProduct() == STAR_MK210V301) {
        sys_beep_pro(BEEP_PWM_4KHZ,ms,NO);
    }else
        sys_beep_pro(DEFAULT_SOUNDFREQUENCY,ms,NO);
#else
	uchar buf[4];
	buf[0] = HBYTE(ms);
	buf[1] = LBYTE(ms);
	PPRT_Send(CMD_BEEP,buf,2);	
#endif	
}

//in: plaint_text  len = keyinfo->bits/8
//keyinfo->modulus:  pubkey+00
//out:cipher_text  len = keyinfo->bits/8
uint32_t crc_calculate32by16(uint16_t * ptr, uint32_t count)
{
    return 0;
}

uint32_t Factory_IO_Test(uint8_t mode)
{
    uint32_t ret = 0;
#if 0
    switch (mode)
    {
    case 0:
#if (defined CFG_MAGCARD)
        hw_mag_gpio_output(MAG_POWER);
        hw_mag_gpio_output(MAG_STROBE);
        hw_mag_gpio_output(MAG_DATA);
        hw_mag_strobe_write(1);
        hw_mag_power_write(1);
        hw_mag_data_write(1);
#endif
#if (defined CFG_ICCARD)
        hw_sam_gpio_init (0);
        hw_gpio_set_bits(SAM_RST_GPIO,1<<SAM_RST_PINx);
        hw_gpio_set_bits(SAM_CS1_GPIO,1<<SAM_CS1_PINx);
        hw_gpio_set_bits(SAM_CS2_GPIO,1<<SAM_CS2_PINx);
        hw_gpio_set_bits(SAM_CLK_GPIO,1<<SAM_CLK_PINx);
        hw_gpio_set_bits(SAM_IO_GPIO,1<<SAM_IO_PINx);
        hw_gpio_set_bits(SAM_PWR_GPIO,1<<SAM_PWR_PINx);
        hw_gpio_set_bits(SAM_INIT_GPIO,1<<SAM_INIT_PINx);
#endif
        break;
#if (defined CFG_MAGCARD)
    case 9: //strobe
        hw_mag_strobe_write(0);
        hw_mag_data_write(1);
        hw_mag_power_write(0); //电源使能电路，反向
        break;
    case 10://vcc
        hw_mag_strobe_write(1);
        hw_mag_data_write(1);
        hw_mag_power_write(0);
        break;
    case 11://data
        hw_mag_strobe_write(1);
        hw_mag_data_write(0);
        hw_mag_power_write(0);
        break;
#endif
#if (defined CFG_ICCARD)
    case 15:
        hw_gpio_reset_bits(SAM_RST_GPIO,1<<SAM_RST_PINx);
        hw_gpio_set_bits(SAM_CS1_GPIO,1<<SAM_CS1_PINx);
        hw_gpio_set_bits(SAM_CS2_GPIO,1<<SAM_CS2_PINx);
        hw_gpio_set_bits(SAM_CLK_GPIO,1<<SAM_CLK_PINx);
        hw_gpio_set_bits(SAM_IO_GPIO,1<<SAM_IO_PINx);
        hw_gpio_set_bits(SAM_PWR_GPIO,1<<SAM_PWR_PINx);
        hw_gpio_set_bits(SAM_INIT_GPIO,1<<SAM_INIT_PINx);
        break;
    case 16:
        hw_gpio_set_bits(SAM_RST_GPIO,1<<SAM_RST_PINx);
        hw_gpio_reset_bits(SAM_CS1_GPIO,1<<SAM_CS1_PINx);
        hw_gpio_set_bits(SAM_CS2_GPIO,1<<SAM_CS2_PINx);
        hw_gpio_set_bits(SAM_CLK_GPIO,1<<SAM_CLK_PINx);
        hw_gpio_set_bits(SAM_IO_GPIO,1<<SAM_IO_PINx);
        hw_gpio_set_bits(SAM_PWR_GPIO,1<<SAM_PWR_PINx);
        hw_gpio_set_bits(SAM_INIT_GPIO,1<<SAM_INIT_PINx);
        break;
    case 17:
        hw_gpio_set_bits(SAM_RST_GPIO,1<<SAM_RST_PINx);
        hw_gpio_set_bits(SAM_CS1_GPIO,1<<SAM_CS1_PINx);
        hw_gpio_reset_bits(SAM_CS2_GPIO,1<<SAM_CS2_PINx);
        hw_gpio_set_bits(SAM_CLK_GPIO,1<<SAM_CLK_PINx);
        hw_gpio_set_bits(SAM_IO_GPIO,1<<SAM_IO_PINx);
        hw_gpio_set_bits(SAM_PWR_GPIO,1<<SAM_PWR_PINx);
        hw_gpio_set_bits(SAM_INIT_GPIO,1<<SAM_INIT_PINx);
        break;
    case 18:
        hw_gpio_set_bits(SAM_RST_GPIO,1<<SAM_RST_PINx);
        hw_gpio_set_bits(SAM_CS1_GPIO,1<<SAM_CS1_PINx);
        hw_gpio_set_bits(SAM_CS2_GPIO,1<<SAM_CS2_PINx);
        hw_gpio_reset_bits(SAM_CLK_GPIO,1<<SAM_CLK_PINx);
        hw_gpio_set_bits(SAM_IO_GPIO,1<<SAM_IO_PINx);
        hw_gpio_set_bits(SAM_PWR_GPIO,1<<SAM_PWR_PINx);
        hw_gpio_set_bits(SAM_INIT_GPIO,1<<SAM_INIT_PINx);
        break;
    case 19:
        hw_gpio_set_bits(SAM_RST_GPIO,1<<SAM_RST_PINx);
        hw_gpio_set_bits(SAM_CS1_GPIO,1<<SAM_CS1_PINx);
        hw_gpio_set_bits(SAM_CS2_GPIO,1<<SAM_CS2_PINx);
        hw_gpio_set_bits(SAM_CLK_GPIO,1<<SAM_CLK_PINx);
        hw_gpio_reset_bits(SAM_IO_GPIO,1<<SAM_IO_PINx);
        hw_gpio_set_bits(SAM_PWR_GPIO,1<<SAM_PWR_PINx);
        hw_gpio_set_bits(SAM_INIT_GPIO,1<<SAM_INIT_PINx);
        break;
    case 20:
        hw_gpio_set_bits(SAM_RST_GPIO,1<<SAM_RST_PINx);
        hw_gpio_set_bits(SAM_CS1_GPIO,1<<SAM_CS1_PINx);
        hw_gpio_set_bits(SAM_CS2_GPIO,1<<SAM_CS2_PINx);
        hw_gpio_set_bits(SAM_CLK_GPIO,1<<SAM_CLK_PINx);
        hw_gpio_set_bits(SAM_IO_GPIO,1<<SAM_IO_PINx);
        hw_gpio_reset_bits(SAM_PWR_GPIO,1<<SAM_PWR_PINx);
        hw_gpio_set_bits(SAM_INIT_GPIO,1<<SAM_INIT_PINx);
        break;
    case 21:
        hw_gpio_set_bits(SAM_RST_GPIO,1<<SAM_RST_PINx);
        hw_gpio_set_bits(SAM_CS1_GPIO,1<<SAM_CS1_PINx);
        hw_gpio_set_bits(SAM_CS2_GPIO,1<<SAM_CS2_PINx);
        hw_gpio_set_bits(SAM_CLK_GPIO,1<<SAM_CLK_PINx);
        hw_gpio_set_bits(SAM_IO_GPIO,1<<SAM_IO_PINx);
        hw_gpio_set_bits(SAM_PWR_GPIO,1<<SAM_PWR_PINx);
        hw_gpio_reset_bits(SAM_INIT_GPIO,1<<SAM_INIT_PINx);
        break;
#endif
    case 0xFF:
#if (defined CFG_MAGCARD)
    mag_init(0);
#endif
#if (defined CFG_ICCARD)
    icc_InitIccModule();
#endif
        break;
    default:
        ret = 1;
        break;
    
    }
#endif    
    return ret;
}



struct calendar_s
{
    uint16_t seconds;
    uint16_t minutes;
    uint16_t hours;
    uint16_t day;
    uint16_t month;
    uint16_t year;
};


//获取时间
//02 06 0B 01 00 03 9A 07
//               秒    分    时    日    月    年    
//               15    29    01    15    1     2000
//02 12 0B 01 00 0F 00 1D 00 01 00 0F 00 01 00 D0 07 03 7C D1 
void app_get_time(uchar *pucData, ushort *pusLen)
{
    unsigned long second=0;
    struct rtc_time tm;
    struct calendar_s time;

    if (hw_rtc_read_second((uint32_t *)&second)) {
        TRACE("\nRead second reg error!");
    }
    rtc_time_to_tm(second,&tm);
    pucData[0] = RESULT_SUCCESS;
    time.year  = tm.tm_year+1900;
    time.month = tm.tm_mon+1;
    time.day   = tm.tm_mday;
    time.hours = tm.tm_hour;
    time.minutes = tm.tm_min;
    time.seconds = tm.tm_sec;
    memcpy(&pucData[1],(uchar *)&time,sizeof(time));
    *pusLen = 1+sizeof(time);
}
void app_set_time(uchar *pucData, ushort *pusLen)
{
    unsigned long second=0;
    struct rtc_time tm;
    struct calendar_s *time;

    time = (struct calendar_s *)pucData;
#if 0
    TRACE("set time:");
    TRACE("%04d-%02d-%02d ", time->year,time->month,time->day);
    TRACE("%02d:%02d:%02d" , time->hours,time->minutes,time->seconds);
#endif
    tm.tm_year = time->year-1900;
    tm.tm_mon  = time->month-1;
    tm.tm_mday = time->day   ;
    tm.tm_hour = time->hours ;
    tm.tm_min = time->minutes;
    tm.tm_sec = time->seconds;
    if (rtc_valid_tm(&tm))
    {
        // Something went wrong encoding the date/time
        pucData[0] = RESULT_ERROR;
        *pusLen = 1;
    }
    else
    {
        rtc_tm_to_time(&tm,&second);
        hw_rtc_init(second,0);
        pucData[0] = RESULT_SUCCESS;
        *pusLen = 1;
    }	
}

void PPRT_SetConfig(uchar *pucData, ushort *pusLen)
{
#if (defined CFG_MAGCARD)
    if(pucData[0] == 0 && *pusLen == 2)
    {
        mag_set(pucData[1]);
        pucData[0] = RESULT_SUCCESS;		
    }
    else
#endif
    {
        pucData[0] = RESULT_ERROR_PARA;		
    }
    *pusLen = 1;
}


// recv Open:  0x0D + 01
// recv Close: 0x0D + 02
void app_magcard_parse(uchar *pucData, ushort *pusLen)
{
#if (defined CFG_MAGCARD)
    if(pucData[0] == 0x01)
    {
        //open magcard
        mag_open();
        pucData[0] = RESULT_SUCCESS;
    }
    else if(pucData[0] == 0x02)
    {
        mag_close();
        pucData[0] = RESULT_SUCCESS;
    }
    else
    {
        pucData[0] = RESULT_ERROR;
    }
#else
    pucData[0] = RESULT_ERROR;
#endif
    *pusLen = 1;
}


void UART_Init(uint bps)
{
    char para[32];
    uart_Close(PINPAD_WORK_COM);
    CLRBUF(para);
    sprintf(para,"%d,8,n,1",bps);
    uart_Open(PINPAD_WORK_COM, para);
}             

uint uart_get_bps(void)
{
    uint data;

    data = gpSys->bps;
    if(/* data != 1200  && */data != 9600 && data != 19200 && data != 38400 
       && data != 57600 && data != 115200)
    {
        data = DEFAULT_SERIAL_BAUD;
    }	
    return data;
}

#endif

