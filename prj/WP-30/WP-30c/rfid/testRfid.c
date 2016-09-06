/*********************************************************************
* 版权所有 (C)2009,
* 文件名称：
*     射频卡生产测试
* 当前版本：
*     1.0
* 内容摘要：
*
* 历史纪录：
*     修改人		 日期		    	 版本号       修改记录
*******************************************************************************/
#include "wp30_ctrl.h"

#ifdef CFG_RFID
/**********************************************************************
*
*
*  外部函数和外部变量声明
*
*
***********************************************************************/
extern void DispData(uchar *title, uint32_t len, uchar *pucData);
extern int if_rfid_module(void);
/**********************************************************************
*
*
*   调试宏定义
*
*
***********************************************************************/


/**********************************************************************
*
*
*   宏定义
*
*
***********************************************************************/

/**********************************************************************
*
*
*全局变量声明:
*
*
***********************************************************************/
/**********************************************************************
*    功能描述:
*
*
***********************************************************************/



#define LCD_TYPE_LINE4  1
#define LCD_TYPE_LINE2  0
int lcd_get_type(void)
{
    return LCD_TYPE_LINE4;
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void fac_show_ret(int ret)
{
	switch(ret)
	{
		case -RFID_ERROR:
			TRACE("失败:[打开模块失败]");
			break;
		case -RFID_ERRPARAM:
			TRACE("失败:[参数错误]");
			break;
		case -RFID_NOCARD:
			TRACE("失败:[无卡]");
			break;
		case -RFID_TRANSERR:
			TRACE("失败:[通讯错误]");
			break;
		case -RFID_PROTERR:
			TRACE("失败:[无效卡]");
			break;
		case -RFID_CARDEXIST:
			TRACE("失败:[卡未离开]");
			break;
		case -RFID_TIMEOUT:
			TRACE("失败:[超时]");
			break;
		case -RFID_MULTIERR:
			TRACE("失败:[多张卡]");
			break;
		case -RFID_NOACT:
			TRACE("失败:[卡未上电]");
			break;
		default:
			TRACE("失败:[未知错误0x%2X]",ret);
			break;
	}
}



/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
const char *rfid_card_string [] = {
	"S50卡",
	"S70卡",
	"Pro卡",
	"ProS50卡",
	"ProS70卡",
	"TypeB卡",
	"未知卡"
};
void fac_show_cardtype(uint line, uchar type)
{
#ifdef DEBUG_Dx
	char *card_type [] = {
	"TYPE A",
	" ",
	" ",
	" ",
	" ",
	"TYPE B",
	};

	if(type <= EM_mifs_TYPEBCARD)
		TRACE("成功:[%s]",card_type[type]);
	else
		TRACE("失败:[%s]",card_type[type]);
#endif
}

void fac_MIFtest(void)
{
	int ret;
	uchar buf[16];
	ret = rfid_MIFAuth(3, 0,(uchar *)"\xFF\xFF\xFF\xFF\xFF\xFF");
	if(ret)
	{
		TRACE("认证:失败[%d]",ret);
		return;
	}
	ret = rfid_MIFRead(3,buf);
    if(ret)
    {

        TRACE("读卡:失败[%d]",ret);
        return;
    }
    TRACE("读卡:成功 %02X%02X%02X%02X",buf[0],buf[1],buf[2],buf[3]);
    TRACE("%02X%02X%02X%02X%02X%02X%02X%02X",buf[4],buf[5],buf[6]
          ,buf[7],buf[8],buf[9],buf[10],buf[11]);
    TRACE("读卡:成功 %02X%02X%02X%02X",ret,buf[0],buf[1],buf[2]
          ,buf[3]);
    sys_beep_pro(BEEP_PWM_HZ,30,YES);
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void fac_circle_rfid_test(void)
{
	uchar buf[64];
	int ret;
	uint j,type;
#ifdef DEBUG_Dx
    uint i=0;
	char *card_type [] = {
	"TYPE A",
	" ",
	" ",
	" ",
	" ",
	"TYPE B",
	};
#endif
    TRACE("取消-退出");
    while(1)
    {
        TRACE("循环测试%d",++i);
        TRACE("寻卡:");
        ret = rfid_poll(RFID_MODE_ISO,&type);
        if(!ret)
        {
            TRACE("寻卡:[%s]",card_type[type]);
            TRACE("上电:");
            ret = rfid_powerup(type,&j,buf);
            if(!ret)
            {
                TRACE("上电:成功 %s",rfid_card_string[buf[j-1]]);
                TRACE("读卡...");
                if(buf[j-1] >= EM_mifs_PROCARD)
                {
                    ret = rfid_exchangedata(sizeof(gApduGet4rand), (uchar *)gApduGet4rand,&j,buf);
                    if(ret == 0 || ret == EM_mifs_SWDIFF)
                    {
                        TRACE("读卡:成功[%02X-%02X]",buf[j-2],buf[j-1]);
                        sys_beep_pro(BEEP_PWM_HZ,30,YES);
                    }
                    else
                    {
                        TRACE("读卡:失败[%d]",ret);
                    }
                    rfid_powerdown();
                }
                else
                {
                    fac_MIFtest();
                }
            }
            else
            {
                TRACE("上电:失败[%d]",ret);
            }
        }
        else
        {
            TRACE("寻卡:失败[%d]",ret);
        }

        if(kb_getkey(500) == KEY_CANCEL)
        {
            return;
        }
    }
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void fac_exchange_rfid_test(uint *type)
{
	uchar ret;
	uchar buf[36],str[16];
	uint i=0;

	TRACE("读卡");

	if(*type > EM_mifs_TYPEBCARD)
	{
		TRACE("失败:[卡未上电]");
		kb_getkey(-1);
	}
	else if(*type <= EM_mifs_S70)
	{
		TRACE("成功");
		kb_getkey(-1);
	}
	else
	{
		ret = rfid_exchangedata(sizeof(gApduGet4rand), (uchar *)gApduGet4rand,&i,buf);
		if(ret != 0 && ret != EM_mifs_SWDIFF)
		{
			*type = 0xFF;
			fac_show_ret(ret);
			kb_getkey(-1);
		}
		else
		{
			memset(str,0,sizeof(str));
			sprintf((char *)str,"读卡数据:%d",i);
			DispData(str,i, buf);
		}
	}

}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
void fac_powerup_rfid_test(uint *type)
{
	uchar buf[36],str[16];
	int ret;
	uint i;
	lcd_cls();
	lcd_display(0, 0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "卡上电");
	ret = rfid_poll(RFID_MODE_ISO,type);
	if(ret)
	{
		fac_show_ret(ret);
		kb_getkey(-1);
	}
	else
	{
		i = 0;
		ret = rfid_powerup(*type,&i,buf);
		if(ret)
		{
			*type = 0xFF;
			fac_show_ret(ret);
			kb_getkey(-1);
		}
		else
		{
			memset(str,0,sizeof(str));
			sprintf((char *)str,"上电数据:%d",i);
			DispData(str,i, buf);
		}
	}
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数： mode: 0-单项测试 1-生产测试
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/

int fac_poll_rfid_test(int mode ,int total,int sus)
{
	uint type;
	int i=1,ret,sustimes=0,errtimes=0;
	int first_sus = 0;//是否检测到一次成功
    lcd_Cls();
    lcd_Display(0, 0, DISP_FONT|DISP_INVLINE|DISP_CLRLINE|DISP_MEDIACY, "寻卡");
	lcd_Display(0,DISP_FONT_LINE4, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"取消-退出");
    kb_Flush();
	while(1)
	{
		lcd_Display(0, DISP_FONT_LINE1, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "次数:%d",i);
        if ( kb_Hit() ) {
            if(kb_GetKey(10) == KEY_CANCEL)
            {
                goto FAIL;
            }
        }
		ret = rfid_poll(RFID_MODE_ISO,&type);
		if(!ret)
		{
			first_sus = 1;
			sustimes++;
			if(type == 0)
				lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"卡:[TYPE A]");
			else
				lcd_Display(0, DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"卡:[TYPE B]");
            sys_BeepPro(BEEP_PWM_HZ,300,YES);
		}
		else
		{
			if (first_sus != 0)
			{
				errtimes++;
				sustimes = 0;
				lcd_Display(0,DISP_FONT_LINE3, DISP_FONT|DISP_CLRLINE|DISP_MEDIACY, "卡:失败");
			}
		}
		lcd_Display(0, DISP_FONT_LINE2, DISP_FONT|DISP_MEDIACY|DISP_CLRLINE, "成功:%-2d 失败:%-2d",sustimes,errtimes);
        rfid_powerdown();
        if ( !mode ) 
        {
            if(kb_getkey(500) == KEY_CANCEL)
            {
                return 0;
            }
        }
        else
        {
            if(sustimes >= sus)
            {
                //			kb_GetKey(2000);
                goto SUCCESS;
            }

            if(i >= total)
            {
                goto FAIL;
            }
            if (first_sus != 0)
            {
                i++;
            }
        }
	}
SUCCESS:
    return OK;
FAIL:
    return ERROR;
}
/**********************************************************************
* 函数名称：
*
* 功能描述：
*
* 输入参数：
*     无
* 输出参数：
*     无
* 返回值：
*     无
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
int Test_RFID_Fac(int mode)
{
	uint type = 0;
	int key = 0;
	int ret;
	ret = if_rfid_module();
	if(ret)
	{
        lcd_cls();
        lcd_display(0,DISP_FONT_LINE2,DISP_FONT|DISP_CLRLINE|DISP_MEDIACY,"没有该模块");
        kb_getkey(3000);
		return 0;
	}
    type = type;
	ret = rfid_open(0);
	if(ret)
	{
		rfid_close();
		lcd_cls();
		lcd_display(0,DISP_FONT, DISP_FONT|DISP_MEDIACY, "射频模块打开失败");
		kb_getkey(5000);
		return 0;
	}
	while(1)
	{
        if (lcd_get_type() == LCD_TYPE_LINE4)
        {
            lcd_cls();
            lcd_display(0,DISP_FONT_LINE0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "射频卡测试");
            lcd_display(0,DISP_FONT_LINE2,  DISP_FONT|DISP_CLRLINE,            "1-寻卡   ");
            lcd_display(0,DISP_FONT_LINE4,  DISP_FONT|DISP_CLRLINE,"2-循环测试 ");
        }
        else
        {
            lcd_cls();
            lcd_display(0,DISP_FONT_LINE2,  DISP_FONT|DISP_CLRLINE, "1-寻卡   ");
            lcd_display(0,DISP_FONT_LINE4,  DISP_FONT|DISP_CLRLINE,"2-循环测试 ");
        }

//        sys_SleepWaitForEvent(EVENT_KEYPRESS,0,0);
		key = kb_getkey(-1);
		switch(key)
		{
			case KEY1:
				fac_poll_rfid_test(0,0,0);
				break;
			/*
			case KEY2:
				type = 0xFF;
				fac_powerup_rfid_test(&type);
				break;
			case KEY3:
				fac_exchange_rfid_test(&type);
				break;
			case KEY4:
				type = 0xFF;
				lcd_cls();
				lcd_display(0, 0, DISP_FONT|DISP_INVLINE|DISP_MEDIACY, "卡下电");
				lcd_display(0, 2*DISP_FONT, DISP_FONT|DISP_CLRLINE , "成功");
				rfid_powerdown();
				kb_getkey(-1);
				break;
			case KEY5:
			*/
			case KEY2:
				type = 0xFF;
				fac_circle_rfid_test();
				break;
			case KEY_CANCEL:
				rfid_close();
				return 0;
			default:
				break;
		}
	}
}
/*
 * test_rfid_para - [GENERIC] 测定射频卡参数
 * @
 */
int test_rfid_para (uint mode)
{
    int ret;
    uint type,i;
    uint recvlen;
    uchar recv[128];
    ret = rfid_poll(mode,&type);
//    TRACE("\r\n poll ret:%d",ret);
    if ( !ret ) {
        ret = rfid_powerup(type,&recvlen,recv);
//        TRACE("\r\n powerup ret:%d",ret);
        if ( !ret ) {
            type = recv[recvlen-1];
            if (type  < RFID_CARD_PRO ) {
                return 1;
            }else{
                lcd_display(0, DISP_FONT, DISP_FONT|DISP_CLRLINE,"成功:%s",rfid_card_string[type]);
            }
            for ( i=0 ; i<3 ; i++ ) {
                ret = rfid_exchangedata(sizeof(gApduGet4rand), (uchar *)gApduGet4rand,&recvlen,recv);
//                TRACE("\r\n exchange ret:%d",ret);
                if ( ret ) {
                    break;
                }
//                vDispBufTitle("rand",recvlen,0,recv);
            }
            rfid_powerdown();
        }
    }
    return ret;
}		/* -----  end of function test_rfid_para  ----- */

int test_rfid_para_menu(int mode)
{
    int pollmode[]={
        0,
        RFID_MODE_CID|RFID_MODE_NAD,
        RFID_MODE_NOFRAME,
    };
    int i,ret;
    kb_flush();
    lcd_cls();
    lcd_display(0,0,DISP_FONT|DISP_MEDIACY|DISP_INVLINE,"射频卡参数测定");
    lcd_display(0,DISP_FONT,DISP_FONT,"请放好卡,任意键开始");
    kb_getkey(-1);
    while(1)
    {
        lcd_cls();
        lcd_display(0,0,DISP_FONT|DISP_MEDIACY|DISP_INVLINE,"射频卡参数测定");
        for(i=0;i<DIM(pollmode);i++)
        {
            lcd_display(0,DISP_FONT,DISP_FONT|DISP_CLRLINE,"测试%d中...",i+1);
            ret = test_rfid_para(pollmode[i]);
            if(!ret){
                break;
            }
        }
        if ( !ret ) {
//            TRACE("\r\n %d-%x",i,pollmode[i]);
            lcd_display(0,DISP_FONT*2,DISP_FONT,"帧号:%s",
                        (pollmode[i]&RFID_MODE_NOFRAME) ? "不支持" : "支持");
            lcd_display(0,DISP_FONT*3,DISP_FONT,"CID:%s",
                        (pollmode[i]&RFID_MODE_CID) ? "支持" : "不支持");
            lcd_display(0,DISP_FONT*4,DISP_FONT,"NAD:%s",
                        (pollmode[i]&RFID_MODE_CID) ? "支持" : "不支持");
        } else {
            fac_show_ret(ret);
        }
        sys_beep();
        if ( kb_getkey(3000) == KEY_CANCEL) {
            break;
        }
    }
    return ret;
}
#endif



