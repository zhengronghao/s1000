/*********************************************************************
* 版权所有 (C)2003, 福建实达电脑设备有限公司。
* 文件名称：
*     Mifs_PayPass.c
* 当前版本：
*     01-01-01
* 内容摘要：
*     本文件负责实现符合ISO14443和PAYPASS规范的非接触卡命令层的程序
* 包含的函数：
*     EI_paypass_vInit;         // 相关变量初始化
*     EI_paypass_vSetTimer;     // 设置超时时限
*     EI_paypass_vDelay;        // 延时
*     EI_paypass_ucWUPA;        // 唤醒TypeA卡
*     EI_paypass_ucAnticoll;    // TypeA卡防冲突
*     EI_paypass_ucSelect;      // 选择TypeA进入ACTIVATE状态
*     EI_paypass_ucRATS;        // 发送RATS命令，使TypeA卡进入Protocol状态
*     EI_paypass_ucHALTA;       // 使TypeA卡进入HALT状态
*     EI_paypass_vResetPICC;    // 关闭RC531载波，复位所有的PICCs
*     EI_paypass_ucWUPB;        // 唤醒TypeB卡
*     EI_paypass_ucAttrib;      // 选中TypeB卡，使之进入ACTIVATE状态
*     EI_paypass_ucHALTB;       // 使TypeB卡进入HALT状态
*     EI_paypass_ucGetUID;      // 获取TypeA卡的完整UID信息
*     EI_paypass_ucActPro;      // 激活TypeA卡
*     EI_paypass_ucActTypeB;    // 激活TypeB卡
*     EI_paypass_ucSelectType;  // 选择载波调制方法
*     EI_paypass_ucPOLL;        // 判断感应区内是否有卡
*     EI_paypass_ucIfMifExit;   // 判断Mif卡是否还在感应区
*     EI_paypass_ucExchange;    // APDU交换
*     EI_paypass_ucDeSelect;    // DESELECT命令
*     EI_paypass_ucProcess;     // 命令处理
*     EI_paypass_ucMifRBlock;   // 处理R_Block
*     EI_paypass_ucMifSBlock;   // 处理S_Block
* 历史纪录：
*     修改人           日期              版本号
*                   04-04-2006          01-01-01
******************************************************************/
#include "wp30_ctrl.h"

#ifdef CFG_RFID
/*****************************************************************

                        外部变量说明

******************************************************************/

/***************************************************************
                        全局变量说明

****************************************************************/
#define EM_paypass_TRYON_TIMES     3  //操作尝试最多次数

#define EM_paypass_protocol_TIMES  3  //协议最多重发次数

#define EM_paypass_polltime        5000 //type a/b卡 轮询间隔时间
#define EM_paypass_minFDTadtime    250  //pcd发送帧保护时间FDTmin

_t_RfidDebugInfo_ gtRfidDebugInfo;
#ifdef RFID_ENABLE_REG
_t_RfidReg_Info_ gtRfidRegInfo = {
	0xFF,0x73,0x06,0x3F,0x04,
    0x64,0x73,0x03,0x3F,0x03
};
#endif

#ifdef EM_PN512_Module
MIFS_REG gcMifReg = {
//#if Product_Type == Product_3
//	#if PCD_TYPE == PCD_F11
//	0xF4,0x0F,0x04
//	#elif PCD_TYPE == PCD_F10_V20
//	0xF4,0x0F,0x0b
//	#else
//	0xF4,0x0F,0x04
//	#endif
//#elif Product_Type == Product_F05_2
//	0xF4,0x0F,0x04
//#elif Product_Type == Product_F14
//	0xF4,0x0F,0x04
//#else
//	0xF4,0x0F,0x04
//#endif
    0x7B,0x6B,0x5D,0x38,0x78,0x82,0x38,0x19
};
#endif
/*****************************************************************

                          函数定义

******************************************************************/
extern int s_rfid_getPara(int module, int para, int index, s_rfidpara_info *rfdata);
/*****************************************************************
* 函数名称：
*        s_rfid_init
* 功能描述：
*
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
****************************************************************/
void s_rfid_init(void)
{
    extern const s_rfidpara_info rfidpara_FM17550_V1EX[4];
    int  module_type;
    int  index,ret;
    s_rfidpara_info rfdata;

	memset(&gtRfidProInfo,0,sizeof(gtRfidProInfo));
	memset(&EG_mifs_tWorkInfo, 0, sizeof(EG_mifs_tWorkInfo));
//	memset(&gtPN512Info, 0, sizeof(gtPN512Info));

    module_type = sys_get_module_type(MODULE_RF_TYPE);
//    module_type = MODULE_RF_FM17550;

#ifdef EM_RC531_Module
    if ( module_type == MODULE_RF_RC531 ) {
        EG_mifs_tWorkInfo.RFIDModule = RFID_Module_RC531;  //0-RC531   1-PN512
        EG_mifs_tWorkInfo.FIFOSIZE = 64;
    }
#endif

#ifdef EM_PN512_Module
    if ( (module_type == MODULE_RF_PN512_BASE) || (module_type == MODULE_RF_FM17550) ) {
        EG_mifs_tWorkInfo.RFIDModule = RFID_Module_PN512;  //0-RC531   1-PN512
        EG_mifs_tWorkInfo.FIFOSIZE = 64; //max 96
    }
#endif

#ifdef EM_RC663_Module
    if ( module_type == MODULE_RF_RC663 ) {
        EG_mifs_tWorkInfo.RFIDModule = RFID_Module_RC663;  //0-RC531   1-PN512
    }
#endif

#ifdef EM_AS3911_Module
    if ( module_type == MODULE_RF_AS3911 ) {
        EG_mifs_tWorkInfo.RFIDModule = RFID_Module_AS3911;  //0-RC531   1-PN512
        EG_mifs_tWorkInfo.FIFOSIZE = 64; //max 96
    }
#endif

	s_Rfid_vHalInit();

	// 初始化全局变量
	EG_mifs_tWorkInfo.ulMaxLen = 32;
	EG_mifs_tWorkInfo.ucCIDSupport = 1;
	EG_mifs_tWorkInfo.ucFWI = EM_mifs_FWI_MAXVALUE;

	EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEA;
	EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_NONE;
	EG_mifs_tWorkInfo.FSC = EM_mifs_FSC_Default;
	EG_mifs_tWorkInfo.FSD = EM_mifs_FSD_Default;

    //先获取默认参数
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
        s_rfid_getPara(EG_mifs_tWorkInfo.RFIDModule, RFID_PARA_PWR, RFID_DEFAULT_PWR, &rfdata);
    }
#ifdef EM_PN512_Module
    else if (  EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512  ) {
        s_rfid_getPara(EG_mifs_tWorkInfo.RFIDModule, RFID_PARA_PWR, RFID_DEFAULT_PWR, &rfdata);
        gcMifReg.GsNOn = (uchar)rfdata.pwrfield;
        gcMifReg.CWGsP = (uchar)rfidpara_FM17550_V1EX[RFID_DEFAULT_PWR].pwrfield;
    }
#endif
    else{
        //531初始用最强场强
        s_rfid_getPara(EG_mifs_tWorkInfo.RFIDModule, RFID_PARA_PWR, RFID_PWR_LEVEL3, &rfdata);
    }
    gtRfidDebugInfo.CW_A = rfdata.pwrfield;
    gtRfidDebugInfo.CW_B = gtRfidDebugInfo.CW_A;

//    Dprintk("\r\n befor pwr:%x ",gtRfidDebugInfo.CW_A);

    //获取flash参数
	ret = ReadRfid_Para_Get(RFID_PARA_PWR, &index);
    if ( ret || (index > RFID_MAX_PWR)) {
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
            index = RFID_DEFAULT_PWR;
        }else if (  EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512 ) {
            index = RFID_DEFAULT_PWR;
        }else
            index = RFID_PWR_LEVEL3; //531初始用最强场强
		WriteRfid_Para_Set(RFID_PARA_PWR, index);
    }else{
        ret = s_rfid_getPara(EG_mifs_tWorkInfo.RFIDModule, RFID_PARA_PWR, index, &rfdata);
        if ( !ret ) {
            gtRfidDebugInfo.CW_A = rfdata.pwrfield;
            gtRfidDebugInfo.CW_B = gtRfidDebugInfo.CW_A;
#ifdef EM_PN512_Module
            if (  EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512  ){
                gcMifReg.GsNOn = (uchar)rfdata.pwrfield;
                gcMifReg.CWGsP = (uchar)rfidpara_FM17550_V1EX[index].pwrfield;
            }
#endif
        }
    }

//    Dprintk("\r\n after pwr:%x ",gtRfidDebugInfo.CW_A);

	gtRfidDebugInfo.ModWidth_A = 0x13;

	gtRfidDebugInfo.ModConduct_B = 0x06;

	ReadRfid_Para_Get(RFID_PARA_PWRSETTIME, &EG_mifs_tWorkInfo.PwrAdTime);
	if((EG_mifs_tWorkInfo.PwrAdTime<RFID_DEFAULT_PWRSETTIME)
		|| (EG_mifs_tWorkInfo.PwrAdTime>RFID_MAX_PWRSETTIME))
	{
		EG_mifs_tWorkInfo.PwrAdTime = RFID_DEFAULT_PWRSETTIME;  //cf02282012
		WriteRfid_Para_Set(RFID_PARA_PWRSETTIME, EG_mifs_tWorkInfo.PwrAdTime);
	}
}
/*****************************************************************
* 函数名称：
*        EI_rfid_GetFramesize
* 功能描述：
*        根据FSDI或FSCI获取帧大小
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
****************************************************************/
uint  s_rfid_FWI2FWT(uint FWI)
{
	uint uiFWITemp,FWT;
	uiFWITemp = (uint) 0x01 << FWI;
	if(EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEA)
	{
		FWT = (uint) (35) * uiFWITemp;
		if(FWI != 14 && FWI != 13 && FWI != 12)
			FWT += 1;
	}
	else
	{
		FWT = (uint) (35) * uiFWITemp;
		if(FWI <= 2)
		{
			FWT += 2;
		}
		else
		{
			FWT += 1;
		}
	}
	return FWT;
}
/*****************************************************************
* 函数名称：
*        EI_rfid_GetFramesize
* 功能描述：
*        根据FSDI或FSCI获取帧大小
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
****************************************************************/
uint EI_rfid_GetFramesize(uchar fsdi)
{
	switch(fsdi)
	{
		case 0:
			return 16;
		case 1:
			return 24;
		case 2:
			return 32;
		case 3:
			return 40;
		case 4:
			return 48;
		case 5:
			return 64;
		case 6:
			return 96;
		case 7:
			return 128;
		case 8:
		default:
			return 256;
	}
}

/*****************************************************************
* 函数名称：
*
* 功能描述：
*        当前块号自增
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
****************************************************************/
#define INC_PCB_BLOCKNO(PCB)    do{PCB ^= 0x01;}while(0)

#define GET_PCB_BLOCKNO(PCB)    (PCB&0x01)  //获得块号
/*****************************************************************
* 函数名称：
*
* 功能描述：
*        判断块类型
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
****************************************************************/
#if 1
#define IF_IBLOCK(PCB)      ((PCB & 0xC0) == 0x00)   //判断是否是I块b8b7=00
uint IF_IBLOCK_ERR(uchar PCB)
{
	if(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV)
	{
		//b6=0 b4=0(CID)b3=0(NAD)b2=1
		return ((PCB & 0x2E) != 0x02);
	}
	else
	{
		// 2011-12-30 I块不要求判断CID和NAD b4~b3位
		return ((PCB & 0x22) != 0x02);
	}
}


#define IF_RBLOCK(PCB)      ((PCB & 0xC0) == 0x80)   //判断R块b8b7=10
uint IF_RBLOCK_ERR(uchar PCB)
{
	if(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV)
	{
		//b6=0 b4=0(CID)b3=0(NAD)b2=1
		return ((PCB & 0x3E) != 0x22);
	}
	else
	{
		// 2011-12-30 R块不要求判断CID  b4位
		return ((PCB & 0x36) != 0x22);
	}
}
#define IF_R_ACK(PCB)   ((PCB & 0xF6) == 0xA2)       //判断ACK块
#define IF_R_NAK(PCB)   ((PCB & 0xF6) == 0xB2)       //判断NAK块

#define IF_SBLOCK(PCB)      ((PCB & 0xC0) == 0xC0)   //判断是否是S块 b8b7=11
uint IF_SBLOCK_ERR(uchar PCB)
{
	if(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV)
	{
		//判断 b6b5=11(WTX) b4=0 b3=0 b2=1 b1=0
		return ((PCB & 0x3F) != 0x32);
	}
	else
	{
		// 2011-12-30 S块不要求判断CID  b4位
		return ((PCB & 0x37) != 0x32);
	}
}
#define IF_S_WTX(PCB)   ((PCB & 0xF7) == 0xF2)       //判断WTX块
#else
#define IF_IBLOCK(PCB)      ((PCB & 0xC0) == 0x00)   //判断是否是I块b8b7=00
#define IF_IBLOCK_ERR(PCB)  ((PCB & 0x2E) != 0x02)   //b6=0 b4=0(CID)b3=0(NAD)b2=1

#define IF_RBLOCK(PCB)      ((PCB & 0xC0) == 0x80)   //判断R块b8b7=10
#define IF_RBLOCK_ERR(PCB)  ((PCB & 0x3E) != 0x22)   //b6=1 b5=0(必须是ACK) b4=0(CID=0) b3=0 b2=1
	#define IF_R_ACK(PCB)   ((PCB & 0xF6) == 0xA2)       //判断ACK块
	#define IF_R_NAK(PCB)   ((PCB & 0xF6) == 0xB2)       //判断NAK块

#define IF_SBLOCK(PCB)      ((PCB & 0xC0) == 0xC0)   //判断是否是S块 b8b7=11
#define IF_SBLOCK_ERR(PCB)  ((PCB & 0x3F) != 0x32)   //判断 b6b5=11(WTX) b4=0 b3=0 b2=1 b1=0
	#define IF_S_WTX(PCB)   ((PCB & 0xF7) == 0xF2)       //判断WTX块
#endif
/*****************************************************************
* 函数名称：
*        EI_paypass_vInit(void)
* 功能描述：
*        初始化MIF结构
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
****************************************************************/
void EI_paypass_vInit(void)
{
	EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEA;
	EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_NONE;
	EG_mifs_tWorkInfo.FSC = EM_mifs_FSC_Default;
	EG_mifs_tWorkInfo.FSD = EM_mifs_FSD_Default;
	EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_S50;
}

/*****************************************************************
* 函数名称：
*         EI_paypass_vSetTimer(ulong ulETU)
* 功能描述：
*         设置操作时限
* 被以下函数调用：
*         无
* 调用以下函数：
*         无
* 输入参数：
*        实际需要定时的ETU数
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
****************************************************************/
void EI_paypass_vSetTimer(ulong ulETU)
{
	uchar ucTempData = 0;

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;
        //注意:ulFWT参数3911设计为1/fc  其他都是表示etu
        //输入参数为ETU个数(128/fc)  换算成1/fc
        EG_mifs_tWorkInfo.ulTimeout = ulETU*128;
#endif

    }
    else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
    {

#ifdef EM_RC663_Module
        if ( ulETU <= 65535 ) {
            ucTempData = 0x80;  //1/13.56MHZ*128 == 9.43us(1/106kbit)
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_T0RELOADLO, &ucTempData);
            ucTempData = 0;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_T0RELOADHI, &ucTempData);

            ulETU--;//微调1个etu
            ucTempData = (uchar)(ulETU&0xFF);
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_T1RELOADLO, &ucTempData);
            ucTempData = (uchar)((ulETU>>8)&0xFF);
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_T1RELOADHI, &ucTempData);
        }else{
            //大于65535
        }
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x40;      //64/6.78 = 9.43us
		EI_mifs_vWriteReg(1, TPrescalerReg, &ucTempData);
		ucTempData = 0x0F;
		EI_mifs_vClrBitMask(TModeReg, ucTempData);

		ucTempData = (uchar)(ulETU&0xFF);
		EI_mifs_vWriteReg(1, TReloadVal_Lo, &ucTempData);
		ucTempData = (uchar)((ulETU>>8)&0xFF);
		EI_mifs_vWriteReg(1, TReloadVal_Hi, &ucTempData);

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		if (ulETU == EM_mifs_FWTMIN)
		{
			// 主要用于定时9.2ETU
			// TAutoRestart = 0, TPrescale = 8
			ucTempData = 0x03;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			// 9.2ETU = 1172CLK / 8 = 147 = 0x93
			// ucTempData = 0x99; // TReloadValue,定时器单位定时8Clk
			ucTempData = 0xFF;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 256)
		{
			// TAutoRestart = 0, TPrescale = 128
			ucTempData = 0x07;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) ulETU;		// TReloadValue,定时器单位定时1ETU
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 511)
		{
			// TAutoRestart = 0, TPrescale = 256
			ucTempData = 0x08;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 2);	// TReloadValue,定时器单位定时2ETU
			if ((ulETU % 2) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 1021)
		{
			// TAutoRestart = 0, TPrescale = 512
			ucTempData = 0x09;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 4);	// TReloadValue,定时器单位定时4ETU
			if ((ulETU % 4) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 2041)
		{
			// TAutoRestart = 0, TPrescale = 1024
			ucTempData = 0x0A;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 8);	// TReloadValue,定时器单位定时8ETU
			if ((ulETU % 8) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 4081)
		{
			// TAutoRestart = 0, TPrescale = 2048
			ucTempData = 0x0B;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 16);	// TReloadValue,定时器单位定时16ETU
			if ((ulETU % 16) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 8161)
		{
			// TAutoRestart = 0, TPrescale = 4096
			ucTempData = 0x0C;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 32);	// TReloadValue,定时器单位定时32ETU
			if ((ulETU % 32) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 16321)
		{
			// TAutoRestart = 0, TPrescale = 8192
			ucTempData = 0x0D;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 64);	// TReloadValue,定时器单位定时64ETU
			if ((ulETU % 64) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 32641)
		{
			// TAutoRestart = 0, TPrescale = 16384
			ucTempData = 0x0E;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 128);	// TReloadValue,定时器单位定时128ETU
			if ((ulETU % 128) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 65281)
		{
			// TAutoRestart = 0, TPrescale = 32768
			ucTempData = 0x0F;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 256);	// TReloadValue,定时器单位定时256ETU
			if ((ulETU % 256) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 130561)
		{
			// TAutoRestart = 0, TPrescale = 65536
			ucTempData = 0x10;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 512);	// TReloadValue,定时器单位定时512ETU
			if ((ulETU % 512) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 261121)
		{
			// TAutoRestart = 0, TPrescale = 131072
			ucTempData = 0x11;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 1024);	// TReloadValue,定时器单位定时1024ETU
			if ((ulETU % 1024) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else if (ulETU < 522241)
		{
			// TAutoRestart = 0, TPrescale = 262144
			ucTempData = 0x12;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 2048);	// TReloadValue,定时器单位定时2048ETU
			if ((ulETU % 2048) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}
		else
		{
			// TAutoRestart = 0, TPrescale = 524288
			ucTempData = 0x13;
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERCLOCK, &ucTempData);

			ucTempData = (uchar) (ulETU / 4096);	// TReloadValue,定时器单位定时4096ETU
			if ((ulETU % 4096) != 0)
			{
				ucTempData = ucTempData + 1;
			}
			EI_mifs_vWriteReg(1, EM_mifs_REG_TIMERRELOAD, &ucTempData);
		}

#endif

	}

}

/*****************************************************************
* 函数名称：
*         EI_paypass_vDelay(ulong ulETU)
* 功能描述：
*         延时操作
* 被以下函数调用：
*         无
* 调用以下函数：
*         无
* 输入参数：
*        ulETU  延时的ETU个数
* 输出参数：
*        无
* 返 回 值：
*        无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
****************************************************************/
void EI_paypass_vDelay(ulong ulETU)
{
	ulong DelayTime;
	volatile uint j;
	DelayTime = ulETU * 10/100;  //1etu=9.43us

	for (j = 0; j < DelayTime; j++)
	{
		s_DelayUs(85);		// 100us
	}
}

/*****************************************************************
* 函数名称：
*        EI_paypass_ucWUPA(uchar* pucResp)
* 功能描述：
*        对Pro卡发送WUPA命令，并接收响应
* 调用以下函数：
*        无
* 被以下函数调用：
*        无
* 输入参数：
*        无
* 输出参数：
*        pucResp   卡片返回的ATQA参数
* 返 回 值：
*        EM_mifs_SUCCESS   成功
*        EM_mifs_PROTERR   返回的参数不符合规范
*        EM_mifs_TRANSERR  通信错误
*        EM_mifs_TIMEOUT   卡无响应
*        EM_mifs_MULTIERR  多卡冲突
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
****************************************************************/
uchar EI_paypass_ucWUPA(uchar * pucResp)
{
	uchar ucRet = EM_mifs_SUCCESS;
	uchar ucTempData = 0x00;
	uchar ucCMD = 0;

	// 先将载波调制为TypeA型
	EI_paypass_vSelectType(EM_mifs_TYPEA);
	EI_paypass_vSetTimer(EM_mifs_DEFAULT);

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WUPA;
        EG_mifs_tWorkInfo.expectMaxRec = 2;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = ucTempData;
		// 禁止 RxCRC 和 TxCRC , 允许奇偶校验
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_STATUS, 0x20);
        //冲突需要设置?
//		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, 0x80);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXCRCCON, 0x01);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, 0x01);

		// set TxLastBits to 7, 短帧格式
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXDATANUM, 0x07);

		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		// 禁止 RxCRC 和 TxCRC , 允许奇偶校验
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(Status2Reg, 0x08);
		//cf_pn512 冲突位
		ucTempData = 0x80;
		EI_mifs_vSetBitMask(CollReg, ucTempData);
		EI_mifs_vClrBitMask(TxModeReg, 0xF0);
		EI_mifs_vClrBitMask(RxModeReg, 0xF0);

		// set TxLastBits to 7, 短帧格式
		ucTempData = 0x07;
		EI_mifs_vWriteReg(1, BitFramingReg, &ucTempData);

		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

        // 禁止 RxCRC 和 TxCRC , 允许奇偶校验
        ucTempData = 0x03;
        EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

        // 禁止crypto1单元
        EI_mifs_vClrBitMask(EM_mifs_REG_CONTROL, 0x08);

        // set TxLastBits to 7, 短帧格式
        ucTempData = 0x07;
        EI_mifs_vWriteReg(1, EM_mifs_REG_BITFRAMING, &ucTempData);

        ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif

	}

	// 准备发送数据: 命令码
	EG_mifs_tWorkInfo.ulSendBytes = 1;	// how many bytes to send
	if (EG_mifs_tWorkInfo.ucAnticollFlag == 1)
	{
		EG_mifs_tWorkInfo.ucDisableDF = 1;
	}
	else
	{
		EG_mifs_tWorkInfo.ucDisableDF = 0;
	}

	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_REQALL;	// WUPA命令
	ucRet = EI_mifs_ucHandleCmd(ucCMD);

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
	EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXDATANUM, 0x07); //发送完毕后发送完整字节
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module
	EI_mifs_vClrBitMask(BitFramingReg, 0x07); //发送完毕后发送完整字节
#endif

	}

	if (ucRet == EM_mifs_NOTAGERR)
	{
		return EM_mifs_TIMEOUT;
	}

	// 处理接收数据
	// 期望接收到两个完整字节: 卡类型识别号(2字节)
	if ((ucRet == EM_mifs_SUCCESS) && (EG_mifs_tWorkInfo.lBitsReceived != 16))
	{
		ucRet = EM_mifs_TRANSERR;
	}

	// 1. ATQA第一个字节的第B8和B7位必须为00 01 10之一，否则按协议错误处理
	// 2. ATQA第一个字节的第B6为必须为0，否则按协议错误处理
	// 3. ATQA第一个字节的第B5到B1位必须为10000 01000 00100 00010 00001之一，否则按协议错误处理。
	// 4. ATQA第二个字节的第B8到B5位必须为0000，否则按协议错误处理。
	if (ucRet != EM_mifs_SUCCESS)
	{
		//如果多张A卡 ATQA不同将出现卡片冲突
		if (ucRet == EM_mifs_COLLERR)
		{
			ucRet = EM_mifs_MULTIERR;
		}
		else if(ucRet == EM_mifs_FDTERR)
		{
			//FDT错误认为协议错误
			ucRet = EM_mifs_PROTERR;
		}
		else
		{
			// 通信错误
			ucRet = EM_mifs_TRANSERR;
		}
	}
	else
	{
		//判断D4~D0位只能1位有1
		if (((EG_mifs_tWorkInfo.aucBuffer[0] & 0x1f) != 0x10)
			&& ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x1f) != 0x08)
			&& ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x1f) != 0x04)
			&& ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x1f) != 0x02)
			&& ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x1f) != 0x01))
		{
			ucRet = EM_mifs_PROTERR;	// 协议错误
		}
		//D0必须为0
		//QPBOC 测试案例要求
		/*
		else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x20) != 0x00)
		{
			ucRet = EM_mifs_PROTERR;	// 协议错误
		}
		*/
		//D8D7=11
		else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xC0) == 0xC0)
		{
			ucRet = EM_mifs_PROTERR;	// 协议错误
		}

		//QPBOC 测试案例要求第2字节任意数值
		/*
		//判断第2字节高4位必须等于0000
		else if ((EG_mifs_tWorkInfo.aucBuffer[1] & 0xf0) != 0x00)
		{
			ucRet = EM_mifs_PROTERR;	// 协议错误
		}
		*/
		else
		{
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xC0) == 0x00)
			{
				EG_mifs_tWorkInfo.ucUIDLen = 4;
			}
			else if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0xC0) == 0x40)
			{
				EG_mifs_tWorkInfo.ucUIDLen = 7;
			}
			else
			{
				EG_mifs_tWorkInfo.ucUIDLen = 10;
			}
			memcpy(pucResp, EG_mifs_tWorkInfo.aucBuffer, 2);
		}
	}
    if ( EG_mifs_tWorkInfo.RFIDModule != RFID_Module_AS3911 ) {
//        EI_paypass_vDelay(EM_mifs_FDTMIN);	// 延时500us再发送下一条命令
        s_DelayUs(EM_paypass_minFDTadtime);	// emv要求(针对RC531) 调整时间刚好延时500us
    }
	return ucRet;
}

/**********************************************************************
* 函数名称：
*       EI_paypass_ucAnticoll(uchar ucSEL, uchar* pucUID)
* 功能描述：
*       应用防冲突机制获取UID信息
* 被以下函数调用：
*       无
* 调用以下函数：
*       无
* 输入参数：
*       ucSEL   冲突等级 取值如下：
*              0x93   第一级冲突
*              0x95   第二级冲突
*              0x97   第三级冲突
* 输出参数：
*       pucUID  获取的UID信息，包括BCC字节
* 返 回 值：
*       EM_mifs_SUCCESS    获取成功
*       EM_mifs_TRANSERR   通信错误
*       EM_mifs_PROTERR    返回的参数不符合规范
*       EM_mifs_TIMEOUT    卡无响应
*       EM_mifs_MULTIERR   多卡冲突
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
****************************************************************/
uchar EI_paypass_ucAnticoll(uchar ucSEL, uchar * pucUID)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData = 0x00;
	uchar ucTempVal = 0x00;
	uchar i = 0,ucCMD = 0;

	if (EG_mifs_tWorkInfo.ucAnticollFlag == 1)
	{
		// 采用防冲突机制获取UID
		result = EI_ucAnticoll(ucSEL, pucUID);
		return result;
	}
	// 设置定时器
	EI_paypass_vSetTimer(EM_mifs_DEFAULT);

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WITHOUT_CRC;
        EG_mifs_tWorkInfo.expectMaxRec = 5;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
		ucTempData = 0x80;
		//cf_RC663   冲突标志
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, ucTempData);
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_STATUS, 0x20);

		// 禁止 RxCRC 和 TxCRC , 允许奇偶校验
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXCRCCON, 0x01);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, 0x01);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x80;
		//cf_pn512   冲突标志
		EI_mifs_vSetBitMask(CollReg, ucTempData);   //cf_pn512 是否要清零
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(Status2Reg, 0x08);
		// 禁止 RxCRC 和 TxCRC , 允许奇偶校验
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

        // 冲突后数据都置0, Rx帧格式为ISO14443A
        ucTempData = 0x28;
        EI_mifs_vWriteReg(1, EM_mifs_REG_DECODERCONTROL, &ucTempData);	// Zero After Coll activate
        // 禁止crypto1单元
        EI_mifs_vClrBitMask(EM_mifs_REG_CONTROL, 0x08);
        // 每个字节带奇校验
        ucTempData = 0x03;
        EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif

	}

	EG_mifs_tWorkInfo.aucBuffer[0] = ucSEL;	// SEL
	EG_mifs_tWorkInfo.aucBuffer[1] = 0x20;
	EG_mifs_tWorkInfo.ulSendBytes = 2;	// how many bytes to send
	// 执行命令
	result = EI_mifs_ucHandleCmd(ucCMD);

/*
	if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{
#ifdef EM_PN512_Module
	EI_mifs_vSetBitMask(CollReg, 0x80);  //cf_pn512 冲突完毕设回原值
#endif
	}
*/

	if (result == EM_mifs_NOTAGERR)		// 超时无响应则直接返回
	{
		return EM_mifs_TIMEOUT;
	}

	if (result == EM_mifs_SUCCESS)
	{
		if (EG_mifs_tWorkInfo.lBitsReceived != 40)	// not 5 bytes answered
		{
			result = EM_mifs_TRANSERR;
		}
		else
		{
			if (EG_mifs_tWorkInfo.ucUIDLen == 4)
			{
				if (ucSEL == EM_mifs_PICC_ANTICOLL1)
				{
					if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x88)
					{
						result = EM_mifs_PROTERR;	// 协议错误
					}
				}
			}
			else if (EG_mifs_tWorkInfo.ucUIDLen == 7)
			{
				if (ucSEL == EM_mifs_PICC_ANTICOLL1)
				{
					if (EG_mifs_tWorkInfo.aucBuffer[0] != 0x88)
					{
						result = EM_mifs_PROTERR;	// 协议错误
					}
				}

				if (ucSEL == EM_mifs_PICC_ANTICOLL2)
				{
					if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x88)
					{
						result = EM_mifs_PROTERR;	// 协议错误
					}
				}
			}
			else
			{
				if (ucSEL == EM_mifs_PICC_ANTICOLL1)
				{
					if (EG_mifs_tWorkInfo.aucBuffer[0] != 0x88)
					{
						result = EM_mifs_PROTERR;	// 协议错误
					}
				}

				if (ucSEL == EM_mifs_PICC_ANTICOLL2)
				{
					if (EG_mifs_tWorkInfo.aucBuffer[0] != 0x88)
					{
						result = EM_mifs_PROTERR;	// 协议错误
					}
				}
			}

			// 校验BCC
			ucTempVal = 0;
			for (i = 0; i < 4; i++)
			{
				ucTempVal ^= EG_mifs_tWorkInfo.aucBuffer[i];
			}
			if (ucTempVal != EG_mifs_tWorkInfo.aucBuffer[4])
			{
				result = EM_mifs_TRANSERR;	// 通信错误
			}
		}

		memcpy(pucUID, EG_mifs_tWorkInfo.aucBuffer, 5);
	}
	else if (result == EM_mifs_COLLERR)
	{
		result = EM_mifs_MULTIERR;
	}
	else
	{
		result = EM_mifs_TRANSERR;
	}

    if ( EG_mifs_tWorkInfo.RFIDModule != RFID_Module_AS3911 ) {
//        EI_paypass_vDelay(EM_mifs_FDTMIN);	// 延时500us再发送下一条命令
        s_DelayUs(EM_paypass_minFDTadtime);	// emv要求(针对RC531) 调整时间刚好延时500us
    }
	return result;
}

/*******************************************************************
* 函数名称：
*        EI_paypass_ucSelect(uchar ucSEL, uchar* pucUID, uchar* pucSAK)
* 功能描述：
*        利用UID信息选中Pro卡
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*       ucSEL     UID等级，取值如下：
*                 0x93        第一级
*                 0x95        第二级
*                 0x97        第三级
*       pucUID   UID参数，5个字节
* 输出参数：
*		pucSAK     Pro卡回送的SAK信息
* 返 回 值：
*       EM_mifs_SUCCESS   获取成功
*       EM_mifs_PROTERR   返回的参数不符合规范
*       EM_mifs_TRANSERR  通信错误
*       EM_mifs_TIMEOUT   卡无响应
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
*******************************************************************/
uchar EI_paypass_ucSelect(uchar ucSEL, uchar * pucUID, uchar * pucSAK)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData = 0x00;
	uchar ucCMD = 0;

	// 设置定时器
	EI_paypass_vSetTimer(EM_mifs_DEFAULT);
	// RxCRC,TxCRC, Odd Parity enable

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
        EG_mifs_tWorkInfo.expectMaxRec = 1+2;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = ucTempData;
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_STATUS, 0x20);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, 0x01);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, 0x01);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);
		EI_mifs_vSetBitMask(RxModeReg, ucTempData);

		// 禁止crypto1单元
		EI_mifs_vClrBitMask(Status2Reg, 0x08);
		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

        ucTempData = 0x0F;
        EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

        // 禁止crypto1单元
        EI_mifs_vClrBitMask(EM_mifs_REG_CONTROL, 0x08);	// disable crypto 1 unit
		ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif

	}

	EG_mifs_tWorkInfo.ulSendBytes = 7;
	EG_mifs_tWorkInfo.ucDisableDF = 0;

	// 导入UID参数
	memcpy(EG_mifs_tWorkInfo.aucBuffer + 2, pucUID, 5);

	EG_mifs_tWorkInfo.aucBuffer[0] = ucSEL;
	EG_mifs_tWorkInfo.aucBuffer[1] = 0x70;	// number of bytes send

	// 发送命令并接收响应
	result = EI_mifs_ucHandleCmd(ucCMD);
	if (result == EM_mifs_NOTAGERR)		// 超时无响应则直接返回
	{
		return EM_mifs_TIMEOUT;
	}
	else if(result == EM_mifs_FDTERR)
	{
		//FDT错误认为协议错误
		result = EM_mifs_PROTERR;
	}
	if (result != EM_mifs_SUCCESS)
	{
		result = EM_mifs_TRANSERR;		// 返回通信错误
	}
	else
	{
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module
            ;
#endif
        }
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {
#ifdef EM_RC663_Module
#endif
        }
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
		{
#ifdef EM_PN512_Module
#endif
		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
		{
#ifdef EM_RC531_Module
			EI_mifs_vReadReg(1, EM_mifs_REG_TIMERVALUE, &ucTempData);
#endif
		}
		if (EG_mifs_tWorkInfo.lBitsReceived != 8)
		{
			// 一个字节没有收满
			result = EM_mifs_TRANSERR;
		}
		else
		{
			// B3位判断UID是否完整
			if (EG_mifs_tWorkInfo.ucUIDLen == 4)
			{
				if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x04) == 0x04)
				{
					result = EM_mifs_PROTERR;
				}
				EG_mifs_tWorkInfo.ucSAK1 = EG_mifs_tWorkInfo.aucBuffer[0];
			}
			else if (EG_mifs_tWorkInfo.ucUIDLen == 7)
			{
				if (ucSEL == EM_mifs_PICC_ANTICOLL1)
				{
					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x04) != 0x04)
					{
						result = EM_mifs_PROTERR;
					}
					EG_mifs_tWorkInfo.ucSAK1 = EG_mifs_tWorkInfo.aucBuffer[0];
				}

				if (ucSEL == EM_mifs_PICC_ANTICOLL2)
				{
					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x04) == 0x04)
					{
						result = EM_mifs_PROTERR;
					}
					EG_mifs_tWorkInfo.ucSAK2 = EG_mifs_tWorkInfo.aucBuffer[0];
				}
			}
			else						// EG_mifs_tWorkInfo.ucUIDLen == 10
			{
				if (ucSEL == EM_mifs_PICC_ANTICOLL1)
				{
					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x04) != 0x04)
					{
						result = EM_mifs_PROTERR;
					}
					EG_mifs_tWorkInfo.ucSAK1 = EG_mifs_tWorkInfo.aucBuffer[0];
				}

				if (ucSEL == EM_mifs_PICC_ANTICOLL2)
				{
					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x04) != 0x04)
					{
						result = EM_mifs_PROTERR;
					}
					EG_mifs_tWorkInfo.ucSAK2 = EG_mifs_tWorkInfo.aucBuffer[0];
				}

				if (ucSEL == EM_mifs_PICC_ANTICOLL3)
				{
					if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x04) == 0x04)
					{
						result = EM_mifs_PROTERR;
					}
					EG_mifs_tWorkInfo.ucSAK3 = EG_mifs_tWorkInfo.aucBuffer[0];
				}
			}
		}
	}
	*pucSAK = EG_mifs_tWorkInfo.aucBuffer[0];
    if ( EG_mifs_tWorkInfo.RFIDModule != RFID_Module_AS3911 ) {
//        EI_paypass_vDelay(EM_mifs_FDTMIN);	// 延时500us再发送下一条命令
        s_DelayUs(EM_paypass_minFDTadtime);	// emv要求(针对RC531) 调整时间刚好延时500us
    }
	return result;
}

/***************************************************************
* 函数名称：
*        EI_paypass_ucRATS(uchar* pucOutLen, uchar* pucATSData)
* 功能描述：
*        给Pro卡发送RATS命令，接收ATS响应
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*       无
* 输出参数：
*       pucOutLen   Pro卡返回的ATS长度
*       pucATSData  Pro卡返回的ATS数据
* 返 回 值：
*       EM_mifs_SUCCESS   获取成功
*       EM_mifs_PROTERR   返回的参数不符合规范
*       EM_mifs_COMMERR   通信错误
*       EM_mifs_TIMEOUT   卡无响应
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
*******************************************************************/
uchar EI_paypass_ucRATS(uchar * pucOutLen, uchar * pucATSData)
{
	uchar ucTempData = 0;
	uchar result;
	uchar tucTemp[2];
	uchar i = 0;
	uchar ucCMD = 0;
RATSSTART:
	result = EM_SUCCESS;
	*pucOutLen = 0;
	EG_mifs_tWorkInfo.FSC = EM_mifs_FSC_Default;
	EG_mifs_tWorkInfo.FSD = EM_mifs_FSD_Default;
	EG_mifs_tWorkInfo.ucFWI = 4;
	EG_mifs_tWorkInfo.ulFWT = s_rfid_FWI2FWT(EG_mifs_tWorkInfo.ucFWI);
	EG_mifs_tWorkInfo.ucSFGI = 0;
	// PayPass要求，当SFGI=0或没有时，要至少延迟(6780/128)etu=500us
	EG_mifs_tWorkInfo.ulSFGT = 60;
	// 默认不采用CID和NAD
	EG_mifs_tWorkInfo.ucCIDFlag = 0;
	EG_mifs_tWorkInfo.ucNADFlag = 0;

	tucTemp[0] = 0xE0;
	//yehf 2010-09-03 Level-1要求修改,FSD = 256Bytes 不支持CID
	tucTemp[1] = 0x80;					//0x70;

	// 设置定时器
	EI_paypass_vSetTimer(EM_mifs_FWTRATS);	// 定时560ETU
	// RxCRC,TxCRC, Parity enable

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
        //emv规范20+2(crc)字节
        EG_mifs_tWorkInfo.expectMaxRec = sizeof(EG_mifs_tWorkInfo.aucBuffer);//20+2;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{

#ifdef EM_RC663_Module
        ucTempData = 0x01;
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);
		EI_mifs_vSetBitMask(RxModeReg, ucTempData);
		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		ucTempData = 0x0F;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif

	}

	// 准备发送数据: 命令码
	EG_mifs_tWorkInfo.ulSendBytes = 2;	// how many bytes to send
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	memcpy(EG_mifs_tWorkInfo.aucBuffer, tucTemp, EG_mifs_tWorkInfo.ulSendBytes);

	// 发送命令并接收响应
	result = EI_mifs_ucHandleCmd(ucCMD);

	if (result == EM_mifs_NOTAGERR)		// 超时无响应则直接返回
	{
		return EM_mifs_TIMEOUT;
	}
	else if(result == EM_mifs_FDTERR)
	{
		//FDT错误认为协议错误
		result = EM_mifs_PROTERR;
	}
	if (result == EM_mifs_SUCCESS)
	{
		i = 0;
		if (EG_mifs_tWorkInfo.ulBytesReceived != EG_mifs_tWorkInfo.aucBuffer[0])	// TL字节错误
		{
			result = EM_mifs_TRANSERR;	// 通信错误
			goto RATSEND;
		}

		// 保存ATS参数
		*pucOutLen = EG_mifs_tWorkInfo.aucBuffer[0];
		memcpy(pucATSData, EG_mifs_tWorkInfo.aucBuffer, EG_mifs_tWorkInfo.ulBytesReceived);
//		if(EG_mifs_tWorkInfo.aucBuffer[0] > 20 || EG_mifs_tWorkInfo.aucBuffer[0] == 0)
		if(EG_mifs_tWorkInfo.aucBuffer[0] == 0)//暂时不判断TL个数12162015 chenf
		{
			//PBOC-Level1协议规定TL不大于20,且大于0
			result = EM_mifs_PROTERR;	// 协议错误
			goto RATSEND;
		}
		else if (EG_mifs_tWorkInfo.aucBuffer[0] == 1)
		{
			//只包含TL字节,不包含T0，TA1、TB1或者TC1，则要求重发
			goto RATSEND;
		}

		i = 1;							// 取T0字节
		// T0字节定义FSC和TA1、TB1、TC1的存在与否
		if ((EG_mifs_tWorkInfo.aucBuffer[1] & 0x80) != 0x00)	// Bit8必须为0
		{
			result = EM_mifs_PROTERR;	// 协议错误
			goto RATSEND;
		}
		else
		{
			//获取FSC
			EG_mifs_tWorkInfo.FSC = EI_rfid_GetFramesize(EG_mifs_tWorkInfo.aucBuffer[1] & 0x0F);
			EG_mifs_tWorkInfo.ulMaxLen = EG_mifs_tWorkInfo.FSC;
		}
		// TA1字节定义设备与卡之间的通信速率，默认为106KBits/s
		// 不管TA1回送任何值，读卡芯片与卡片之间仍按106KBits/s进行通信
		if ((EG_mifs_tWorkInfo.aucBuffer[1] & 0x10) == 0x10)	// TA1字节
		{
			i++;
			//QPBOC 测试案例要求88和08必须通过
			/*
			if ((EG_mifs_tWorkInfo.aucBuffer[i] & 0x08) != 0x00)
			{
				result = EM_mifs_PROTERR;	// 协议错误
				goto RATSEND;
			}
			*/
		}
		// TB1字节定义FWT和SFGT
		if ((EG_mifs_tWorkInfo.aucBuffer[1] & 0x20) == 0x20)	// TB1字节
		{
			i++;
			EG_mifs_tWorkInfo.ucFWI = (EG_mifs_tWorkInfo.aucBuffer[i] & 0x0F0) >> 4;
			EG_mifs_tWorkInfo.ucSFGI = EG_mifs_tWorkInfo.aucBuffer[i] & 0x0F;
			if(EG_mifs_tWorkInfo.ucFWI == 0x0F || EG_mifs_tWorkInfo.ucSFGI == 0x0F)
			{
				//FWI和SFGI范围0~14,15预留
				result = EM_mifs_PROTERR;	// 协议错误
				goto RATSEND;
			}
			EG_mifs_tWorkInfo.ulFWT = s_rfid_FWI2FWT(EG_mifs_tWorkInfo.ucFWI);
 			if(EG_mifs_tWorkInfo.ucSFGI != 0)
			{
				//SFGT=(32 + 3)*2^SFGI  etu
				EG_mifs_tWorkInfo.ulSFGT = s_rfid_FWI2FWT(EG_mifs_tWorkInfo.ucSFGI);
			}
		}
		else
		{
			// TB1没有 采用缺省FWI=4  SFGI=0
			EG_mifs_tWorkInfo.ucFWI = 4;
			EG_mifs_tWorkInfo.ulFWT = s_rfid_FWI2FWT(EG_mifs_tWorkInfo.ucFWI);
			EG_mifs_tWorkInfo.ucSFGI = 60;
		}
		if ((EG_mifs_tWorkInfo.aucBuffer[1] & 0x40) == 0x40)	// TC1
		{
			i++;
			// QPBOC 测试案例TA103.0和TA103.2不要求测试
			/*
			// TC1的高6位必须为000000
			if ((EG_mifs_tWorkInfo.aucBuffer[i] & 0xFC) != 0x00)
			{
				result = EM_mifs_PROTERR;	// 协议错误
				goto RATSEND;
			}
			*/
			// 判断CID
			if(EG_mifs_tWorkInfo.OptMode&RFID_MODE_CID)
			{
				if (EG_mifs_tWorkInfo.aucBuffer[i] & 0x02)
				{
					EG_mifs_tWorkInfo.ucCIDFlag = 1;
				}
			}
			if(EG_mifs_tWorkInfo.OptMode&RFID_MODE_NAD)
            {
				// 判断NAD
				if (EG_mifs_tWorkInfo.aucBuffer[i] & 0x01)
				{
					EG_mifs_tWorkInfo.ucNADFlag = 1;
				}
            }
			result = EM_mifs_SUCCESS;
		}
	}
	else if(result != EM_mifs_NOTAGERR)
	{
		if(EG_mifs_tWorkInfo.lBitsReceived/8<4 || EG_mifs_tWorkInfo.lBitsReceived%8 != 0)
		{
			EI_paypass_vDelay(EG_mifs_tWorkInfo.ulSFGT*5);
			goto RATSSTART;
		}
	}
  RATSEND:
//	EI_paypass_vDelay(EG_mifs_tWorkInfo.ulSFGT+100);	// 延时SFGT再发送下一条命令
	EI_paypass_vDelay(EG_mifs_tWorkInfo.ulSFGT+10);	// emv要求(针对RC531) 调整时间
	return result;
}

/**********************************************************************
* 函数名称：
*       EI_paypass_vHALTA(void)
* 功能描述：
*       将Pro卡置为HALT
* 被以下函数调用：
*       无
* 调用以下函数：
*       无
* 输入参数：
*       无
* 输出参数：
*       无
* 返 回 值：
*       无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
*******************************************************************/
void EI_paypass_vHALTA(void)
{
	uchar ucTempData;
	uchar ucCMD = 0;
	uchar ucRegAddr = 0;;

	// 定时器时间设置
	EI_paypass_vSetTimer(60);			// 定时560ETU

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
        EG_mifs_tWorkInfo.expectMaxRec = 0;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, 0x01);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, 0x01);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
		ucRegAddr = PHHAL_HW_RC663_REG_COMMAND;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, 0x80);
		EI_mifs_vSetBitMask(RxModeReg, 0x80);
		ucCMD = PN512CMD_TRANSCEIVE;
		ucRegAddr = CommandReg;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		// TxCRC, Parity enable
		ucTempData = 0x07;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;
		ucRegAddr = EM_mifs_REG_COMMAND;

#endif

	}


	// 准备发送数据: 停止命令码, 0
	EG_mifs_tWorkInfo.ulSendBytes = 2;
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_HALT;	// Halt command code
	EG_mifs_tWorkInfo.aucBuffer[1] = 0x00;
	// dummy address
	// 执行命令
    gtRfidProInfo.Cmd = EM_mifs_PICC_HALT;
	EI_mifs_ucHandleCmd(ucCMD);
    gtRfidProInfo.Cmd = 0;

	// 当PCD发送完HLTA后，始终认为PICC已成功接收并正确执行了HLTA命令，而不会去理会
	// PICC会回送任何响应。

	// 复位命令寄存器
	if(EG_mifs_tWorkInfo.RFIDModule != RFID_Module_AS3911)
    {
        ucTempData = 0x00;
        EI_mifs_vWriteReg(1, ucRegAddr, &ucTempData);
    }
}

/**********************************************************************
* 函数名称：
*       EI_paypass_vResetPICC(void)
* 功能描述：
*       关闭RC531载波，复位感应区内所有的PICC
* 被以下函数调用：
*       无
* 调用以下函数：
*       无
* 输入参数：
*       无
* 输出参数：
*       无
* 返 回 值：
*       无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
*******************************************************************/
void EI_paypass_vResetPICC(void)
{
	uchar data = 0;

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        data = data;
        as3911WriteRegister(AS3911_REG_OP_CONTROL, 0xD0);
//        emvHalActivateField(OFF);
		s_DelayMs(10);
        emvHalActivateField(ON);
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        data = (uchar)PHHAL_HW_RC663_BIT_TXEN;
        EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_DRVMODE, data);
		s_DelayMs(10);
        EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_DRVMODE, data);
		s_DelayMs(10);
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		data = 0x03;
		// 使RC531输出固定电平
		EI_mifs_vClrBitMask(TxControlReg, data);
		s_DelayMs(10);
		EI_mifs_vSetBitMask(TxControlReg, data);
		s_DelayMs(10);  //PN512 开载波需要延时 稳定系统

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

	//S980支持低功耗模式
#if (defined(PRODUCT_F16_2))
	data = 0x00;
	EI_mifs_vWriteReg(1, EM_mifs_REG_CONTROL, &data);	//进入正常模式
#endif
	// 使RC531输出固定电平
	EI_mifs_vClrBitMask(EM_mifs_REG_TXCONTROL, 0x03);
	s_DelayMs(10);	// TIME_WAIT_TReset 关闭载波1000ETU Treset=5.1~10ms
	// 需要增加原因不明
	EI_mifs_vSetBitMask(EM_mifs_REG_TXCONTROL, 0x03);

#endif

    }
	if((EG_mifs_tWorkInfo.ucAnticollFlag == 0)
        || (EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV))  //EMV模式
		s_DelayMs(6);
	else
		s_DelayMs(EG_mifs_tWorkInfo.PwrAdTime); //ISO载波开启延时 默认15ms (qpboc 5-10ms) cf02282012
}

/**********************************************************************
* 函数名称：
*       EI_paypass_vOptField(uchar mode)
* 功能描述：
*       开关模块载波，复位感应区内所有的PICC
* 被以下函数调用：
*       无
* 调用以下函数：
*       无
* 输入参数：
*       无
* 输出参数：
*       无
* 返 回 值：
*       无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
*******************************************************************/
void EI_paypass_vOptField(uchar mode)
{
    uchar data = 0;

    EI_paypass_vSelectType(EM_mifs_TYPEA);
    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        emvHalActivateField(mode);
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        data = (uchar)PHHAL_HW_RC663_BIT_TXEN;
        if ( mode == ON ) {
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_DRVMODE, data);
        }else
            EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_DRVMODE, data);
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		data = 0x03;
		// 使RC531输出固定电平
        if ( mode == ON ) {
            EI_mifs_vSetBitMask(TxControlReg, data);
        }else
            EI_mifs_vClrBitMask(TxControlReg, data);
		s_DelayMs(10);  //PN512 开载波需要延时 稳定系统

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

	//S980支持低功耗模式
#if (defined(PRODUCT_F16_2))
	data = 0x00;
	EI_mifs_vWriteReg(1, EM_mifs_REG_CONTROL, &data);	//进入正常模式
#endif
	// 使RC531输出固定电平
    if ( mode == ON ) {
        EI_mifs_vSetBitMask(EM_mifs_REG_TXCONTROL, 0x03);
    }else
        EI_mifs_vClrBitMask(EM_mifs_REG_TXCONTROL, 0x03);

#endif

    }
    s_DelayUs(EM_paypass_polltime);
}
/*****************************************************************
* 函数名称：
*     EI_paypass_ucWUPB(uchar* pucOutLen, uchar* pcuOut)
* 功能描述：
*     激活TypeB卡
* 被以下函数调用：
*     无
* 调用以下函数：
*     无
* 输入参数：
*     无
* 输出参数：
*     无
* 返 回 值：
*        EM_mifs_SUCCESS   成功
*        EM_mifs_PROTERR   返回的参数不符合规范
*        EM_mifs_TRANSERR  通信错误
*        EM_mifs_TIMEOUT   卡无响应
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
*******************************************************************/
uchar EI_paypass_ucWUPB(uchar * pucOutLen, uchar * pucOut)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData = 0;
	uchar ucCMD = 0;
	EI_mifs_vFlushFIFO();				//empty FIFO
	// 将载波调制为TypeB型
	EI_paypass_vSelectType(EM_mifs_TYPEB);

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
        EG_mifs_tWorkInfo.expectMaxRec = 13+2;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = 0x01;
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_STATUS, 0x20);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vSetBitMask(ManualRCVReg, 0x10);  //禁止奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);  //CRC
		EI_mifs_vSetBitMask(RxModeReg, ucTempData);  //CRC
		EI_mifs_vClrBitMask(Status2Reg, 0x08);

		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		// RxCRC and TxCRC enable, parity
		ucTempData = 0x2C;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);

		EI_mifs_vClrBitMask(EM_mifs_REG_CONTROL, 0x08);	// disable crypto 1 unit

		ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif

	}

//	EI_paypass_vSetTimer(3 * EM_mifs_FWTWUPB);	// 超时等待180ETU
    EI_paypass_vSetTimer(65);	// 超时等待65*128/fc //emv chenf 12132013

	// 准备发送数据: 命令码
	EG_mifs_tWorkInfo.ulSendBytes = 3;	// how many bytes to send
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_REQB;	// 激活命令
	EG_mifs_tWorkInfo.aucBuffer[1] = 0x00;	// AFI = 0x00
	// EMV要求PCD不支持扩展ATQB b5=0  0x08
	EG_mifs_tWorkInfo.aucBuffer[2] = 0x08;// TimeSlot = 0, WUPB

	// 执行命令
	result = EI_mifs_ucHandleCmd(ucCMD);

	if (result == EM_mifs_NOTAGERR)		// 超时无响应
	{
		return EM_mifs_TIMEOUT;
	}
	if (result != EM_mifs_SUCCESS)
	{
		result = EM_mifs_TRANSERR;
	}
	else
	{
		// 返回12或13个字节
		if (EG_mifs_tWorkInfo.ulBytesReceived != 12 && EG_mifs_tWorkInfo.ulBytesReceived != 13)
		{
			result = EM_mifs_TRANSERR;
			goto WUPBEND;
		}
		if (EG_mifs_tWorkInfo.aucBuffer[0] != 0x50)
		{
			result = EM_mifs_PROTERR;
			goto WUPBEND;
		}

		// 第6~9字节为Application Data,一般为0000，终端可以不予理会

		// 第10字节为BitRate，PCD与PICC一般只支持106KBit
		// 不管该字节回送任何值，均按106KBits/s进行通信
		// QPBOC案例要求不要判断
		/*
		if ((EG_mifs_tWorkInfo.aucBuffer[9] & 0x08) != 0x00)
		{
			result = EM_mifs_PROTERR;
			goto WUPBEND;
		}
		*/
		// 第11字节(MaxFrameSize Protocol Type)返回帧大小和协议 高4位为帧大小，低4位为协议值
		EG_mifs_tWorkInfo.FSC = EI_rfid_GetFramesize(EG_mifs_tWorkInfo.aucBuffer[10]>>4);
		EG_mifs_tWorkInfo.ulMaxLen = EG_mifs_tWorkInfo.FSC;
		// TR2忽略 低4位表示是否支持ISO14443协议，默认支持
		//if ((EG_mifs_tWorkInfo.aucBuffer[10] & 0x09) != 0x01)
		// QPBOC 案例要求Protocol_type的D3位不要判断
		if ((EG_mifs_tWorkInfo.aucBuffer[10] & 0x01) != 0x01)
		{
			result = EM_mifs_PROTERR;
			goto WUPBEND;
		}

		// 第12字节包含FWI，CID和NAD等信息
		// 高4位返回FWI
		EG_mifs_tWorkInfo.ucFWI = (EG_mifs_tWorkInfo.aucBuffer[11] & 0x0F0) >> 4;
		if(EG_mifs_tWorkInfo.ucFWI == 0x0F)
		{
			//不能等于0F
			result = EM_mifs_PROTERR;
			goto WUPBEND;
		}
		EG_mifs_tWorkInfo.ulFWT = s_rfid_FWI2FWT(EG_mifs_tWorkInfo.ucFWI);


		// 第3~4位表示ADC，不予考虑
		// 第1~2位表示CID和NAD机制，不管PICC返回什么，均按不支持处理
		// 判断NAD
		/*
		if (EG_mifs_tWorkInfo.aucBuffer[11] & 0x01)
		{
			EG_mifs_tWorkInfo.ucCIDFlag = 1;
		}
		else
		{
			EG_mifs_tWorkInfo.ucCIDFlag = 0;
		}
		// 判断CID
		if (EG_mifs_tWorkInfo.aucBuffer[11] & 0x02)
		{
			EG_mifs_tWorkInfo.ucNADFlag = 1;
		}
		else
		{
			EG_mifs_tWorkInfo.ucNADFlag = 0;
		}
		*/
		*pucOutLen = (uchar) EG_mifs_tWorkInfo.ulBytesReceived;
		memcpy(pucOut, EG_mifs_tWorkInfo.aucBuffer, EG_mifs_tWorkInfo.ulBytesReceived);

		EG_mifs_tWorkInfo.ucUIDLen = 4;
		// 保存TypeB的UID序列号 第2~5字节
		memcpy(EG_mifs_tWorkInfo.ucUIDB, EG_mifs_tWorkInfo.aucBuffer + 1, 4);
	}

  WUPBEND:

    if ( EG_mifs_tWorkInfo.RFIDModule != RFID_Module_AS3911 ) {
//        EI_paypass_vDelay(EM_mifs_FDTMIN);	// 延时500us再发送下一条命令
        s_DelayUs(EM_paypass_minFDTadtime);	// emv要求(针对RC531) 调整时间刚好延时500us
    }

	return result;
}

/***********************************************************
* 函数名称：
*        EI_paypass_ucAttrib(uchar* pucResp)
* 功能描述：
*        选择TypeB卡
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        pucResp   TypeB卡返回的数据
* 返 回 值：
*        EM_mifs_SUCCESS   成功
*        EM_mifs_PROTERR   返回的参数不符合规范
*        EM_mifs_TRANSERR  通信错误
*        EM_mifs_TIMEOUT   卡无响应
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
*******************************************************************/
uchar EI_paypass_ucAttrib(uchar * pucResp)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData = 0x00;
	uchar ucCMD = 0;
EI_paypass_ucAttrib_start:
	EI_mifs_vFlushFIFO();				//empty FIFO

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
        EG_mifs_tWorkInfo.expectMaxRec = 1+2;
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = ucTempData;
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);//disable parity
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, 0x01);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, 0x01);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{
#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vSetBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);
		EI_mifs_vSetBitMask(RxModeReg, ucTempData);
		ucCMD = PN512CMD_TRANSCEIVE;

#endif
	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{
#ifdef EM_RC531_Module

		// RxCRC and TxCRC enable, parity
		ucTempData = 0x2C;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif
	}

	EI_paypass_vSetTimer(EG_mifs_tWorkInfo.ulFWT);	// 设置超时时限

	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_ATTRIB;	// Attrib命令码
	// 第2~5字节为PUPI
	EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.ucUIDB[0];
	EG_mifs_tWorkInfo.aucBuffer[2] = EG_mifs_tWorkInfo.ucUIDB[1];
	EG_mifs_tWorkInfo.aucBuffer[3] = EG_mifs_tWorkInfo.ucUIDB[2];
	EG_mifs_tWorkInfo.aucBuffer[4] = EG_mifs_tWorkInfo.ucUIDB[3];
	// 第6字节为param1，定义TR0和TR1，及SOF、EOF
	// PAYPASS要求：采用默认TR0和TR1，支持SOF和EOF
	EG_mifs_tWorkInfo.aucBuffer[5] = 0x00;
	// 第7字节为param2，定义BitRate和Frame Size
	// PAYPASS要求BitRate为106KBits，Frame Size = 256Bytes
	EG_mifs_tWorkInfo.aucBuffer[6] = 0x08;
	// 第8字节为param3，定义PCD是否支持ISO14443-4
	// PAYPASS要求能够支持ISO14443-4
	EG_mifs_tWorkInfo.aucBuffer[7] = 0x01;
	// 第9字节为param4，定义了CID
	// PAYPASS要求不支持CID
	EG_mifs_tWorkInfo.aucBuffer[8] = 0x00;

	EG_mifs_tWorkInfo.ulSendBytes = 9;	// how many bytes to send
	EG_mifs_tWorkInfo.ucDisableDF = 0;

	result = EI_mifs_ucHandleCmd(ucCMD);

	if (result == EM_mifs_NOTAGERR)
	{
		// 超时无响应，直接返回
		return EM_mifs_TIMEOUT;
	}
	// 2010-12-19  QPBOC TB305如果CRC错误、帧错误、校验错误则重发最多3次
	// QPBOC TB306_12 00无CRC_B
	else if(result == EM_mifs_PARITYERR || result == EM_mifs_CRCERR || result == EM_mifs_FRAMINGERR)
	{
		if(EG_mifs_tWorkInfo.ulBytesReceived == 1 && result == EM_mifs_CRCERR)
		{
			result = EM_mifs_TRANSERR;
			EI_paypass_vDelay(EM_mifs_FDTMIN);
			return result;
		}
		EI_paypass_vDelay(EM_mifs_FDTMIN);
		goto EI_paypass_ucAttrib_start;
	}
	else if (result != EM_mifs_SUCCESS)
	{
		// 其他情况按通信错误处理
		result = EM_mifs_TRANSERR;
	}
	else
	{
		//响应最小1B数据
		if (EG_mifs_tWorkInfo.ulBytesReceived != 1)
		{
			result = EM_mifs_TRANSERR;
		}
		else
		{
			if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x0F) != 0x00)
			{
				// CID必须为0
				result = EM_mifs_PROTERR;
			}
		}

		*pucResp = EG_mifs_tWorkInfo.aucBuffer[0];
	}

	EI_paypass_vDelay(EM_mifs_FDTMIN);	// 延时500us再发送下一条命令
	return result;
}

/******************************************************
* 函数名称：
*      EI_paypass_ucHALTB(uchar* pucResp)
* 功能描述：
*      将TypeB卡置为HALT态
* 被以下函数调用：
*      无
* 调用以下函数：
*      无
* 输入参数：
*      无
* 输出参数：
*      pucResp   TypeB返回的数据
* 返 回 值
*        EM_mifs_SUCCESS   成功
*        EM_mifs_PROTERR   返回的参数不符合规范
*        EM_mifs_TRANSERR  通信错误
*        EM_mifs_TIMEOUT   卡无响应
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
*******************************************************************/
uchar EI_paypass_ucHALTB(uchar * pucResp)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData = 0x00;
	uchar ucCMD = 0;

	EI_mifs_vFlushFIFO();				// empty FIFO

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
        ucTempData = ucTempData;
        ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
        EG_mifs_tWorkInfo.expectMaxRec = 1+2; //00 + CRC
#endif

    }
    else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
        ucTempData = ucTempData;
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);//disable parity
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, 0x01);
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, 0x01);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{
#ifdef EM_PN512_Module

		ucTempData = 0x80;
		EI_mifs_vSetBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(TxModeReg, ucTempData);
		EI_mifs_vSetBitMask(RxModeReg, ucTempData);
		ucCMD = PN512CMD_TRANSCEIVE;

#endif
	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{
#ifdef EM_RC531_Module

		// RxCRC and TxCRC enable, parity
		ucTempData = 0x2C;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif
	}

	EI_paypass_vSetTimer(EG_mifs_tWorkInfo.ulFWT);	// 设置超时时限

	// 准备发送数据: 命令码＋PUPI
	EG_mifs_tWorkInfo.ulSendBytes = 5;	// how many bytes to send
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	EG_mifs_tWorkInfo.aucBuffer[0] = EM_mifs_PICC_HALT;	// HALT命令码
	// 第2~5字节为PUPI
	EG_mifs_tWorkInfo.aucBuffer[1] = EG_mifs_tWorkInfo.ucUIDB[0];
	EG_mifs_tWorkInfo.aucBuffer[2] = EG_mifs_tWorkInfo.ucUIDB[1];
	EG_mifs_tWorkInfo.aucBuffer[3] = EG_mifs_tWorkInfo.ucUIDB[2];
	EG_mifs_tWorkInfo.aucBuffer[4] = EG_mifs_tWorkInfo.ucUIDB[3];

	result = EI_mifs_ucHandleCmd(ucCMD);

	if (result == EM_mifs_SUCCESS)
	{
		if (EG_mifs_tWorkInfo.ulBytesReceived != 1)
		{
			result = EM_mifs_TRANSERR;
		}
		else if (EG_mifs_tWorkInfo.aucBuffer[0] == 0x0)
		{
			*pucResp = EG_mifs_tWorkInfo.aucBuffer[0];
		}
		else
		{
			result = EM_mifs_PROTERR;
		}
	}
	else if (result == EM_mifs_NOTAGERR)
	{
		result = EM_mifs_TIMEOUT;
	}
	else
	{
		result = EM_mifs_TRANSERR;
	}

	EI_paypass_vDelay(EM_mifs_FDTMIN);	// 延时500us再发送下一条命令
	return result;
}

/***********************************************************
* 函数名称：
*       EI_paypass_ucGetUID(void)
* 功能描述：
*       TypeA卡采用防冲突机制，获取UID
* 被以下函数调用：
*       无
* 调用以下函数：
*       EI_paypass_ucAnticoll
*       EI_paypass_ucSelect
* 输入参数：
*       无
* 输出参数：
*       无
* 返 回 值：
*       EM_mifs_SUCCESS   获取成功
*       EM_mifs_PROTERR   返回的参数不符合规范
*       EM_mifs_TRANSERR  通信错误
*       EM_mifs_TIMEOUT   卡无响应
*       EM_mifs_MULTIERR  多卡冲突
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
* 说    明：
*
*******************************************************************/
uchar EI_paypass_ucGetUID(void)
{
#if 0
	uint i,j,ucReSendNo;
	uchar cmd;
	uchar result = EM_mifs_SUCCESS;
	uchar ucSAK = 0x00;
	uchar ucUIDTemp[6];

	if(EG_mifs_tWorkInfo.ucUIDLen == 4)
	{
		j = 1;
	}
	else if(EG_mifs_tWorkInfo.ucUIDLen == 7)
	{
		j = 2;
	}
	else
	{
		j = 3;
	}
    for(i=0;i<j;i++)
    {
    	if(i == 0)
    	{
    		cmd = EM_mifs_PICC_ANTICOLL1;
    	}
		else if(i == 1)
		{
			cmd = EM_mifs_PICC_ANTICOLL2;
		}
		else
		{
			cmd = EM_mifs_PICC_ANTICOLL3;
		}
    	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
		{
			result = EI_paypass_ucAnticoll(cmd, ucUIDTemp);
			if (result != EM_mifs_TIMEOUT)
			{
				break;
			}
		}
		if (result != EM_mifs_SUCCESS)
		{
			//Anticoll错误退出
			return result;
		}
		if(i == 0)
    	{
    		// 保存第1级UID参数
			memcpy(EG_mifs_tWorkInfo.ucUIDCL1, ucUIDTemp, 5);
    	}
		else if(i == 1)
		{
			// 保存第2级UID参数
			memcpy(EG_mifs_tWorkInfo.ucUIDCL2, ucUIDTemp, 5);
		}
		else
		{
			// 保存第3级UID参数
			memcpy(EG_mifs_tWorkInfo.ucUIDCL3, ucUIDTemp, 5);
		}

		for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
		{
			result = EI_paypass_ucSelect(cmd, ucUIDTemp, &ucSAK);
			if (result != EM_mifs_TIMEOUT)
			{
				break;
			}
		}

		if (result != EM_mifs_SUCCESS)
		{
			//Select错误退出
			return result;
		}
    }
	//成功获取SAK
	//b5=1时符合ISO14443-3协议
	if ((ucSAK & 0x20) != 0x20)
	{
		if ((ucSAK & 0x18) == 0x08)
		{
			EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_S50;
		}
		else if ((ucSAK & 0x18) == 0x18)
		{
			EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_S70;
		}
		else
		{
			return EM_mifs_TRANSERR;
		}
	}
	else
	{
		if ((ucSAK & 0x18) == 0x08)
		{
			EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PRO_S50;
		}
		else if ((ucSAK & 0x18) == 0x18)
		{
			EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PRO_S70;
		}
		else
		{
			EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PROCARD;
		}
	}
	return EM_mifs_SUCCESS;
#else

	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData = 0x00;
	uchar ucUIDTemp[5];
	uchar ucSAK = 0x00;
	uchar ucReSendNo = 0x00;

	memset(ucUIDTemp, 0x00, sizeof(ucUIDTemp));

	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
	{
		ucTempData = EM_mifs_PICC_ANTICOLL1;	// 第一级冲突
		result = EI_paypass_ucAnticoll(ucTempData, ucUIDTemp);
		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
	{
		result = EI_paypass_ucSelect(ucTempData, ucUIDTemp, &ucSAK);
		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	// 保存第一级UID参数
	memcpy(EG_mifs_tWorkInfo.ucUIDCL1, ucUIDTemp, 5);
	if (EG_mifs_tWorkInfo.ucUIDLen == 4)
	{
		memcpy((uchar *)&EG_mifs_tWorkInfo.ulCardID,ucUIDTemp,4);  //MIFARE卡/CPU+M1卡认证使用
		if ((ucSAK & 0x20) != 0x20)
		{
			//b5=0 不符合ISO14443-3协议 M1卡还需要判断是否为Pro卡
			// 2011-12-31  yehf 修改
			if ((ucSAK & 0x18) == 0x18)
			{
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_S70;
				//memcpy((uchar *)&EG_mifs_tWorkInfo.ulCardID,ucUIDTemp,4);
			}
			else
			{
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_S50;
				//memcpy((uchar *)&EG_mifs_tWorkInfo.ulCardID,ucUIDTemp,4);
			}
		}

		else
		{
			// b5=1 符合ISO14443-3规范为Pro卡
			if ((ucSAK & 0x18) == 0x08)
			{
				// S50+Pro
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PRO_S50;

			}
			else if ((ucSAK & 0x18) == 0x18)
			{
				// S70+Pro
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PRO_S70;

			}
			else
			{
				// Pro
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PROCARD;
			}
		}
		return EM_mifs_SUCCESS;
	}

	// 获取第二级UID参数
	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
	{
		ucTempData = EM_mifs_PICC_ANTICOLL2;	// 第二级冲突
		result = EI_paypass_ucAnticoll(ucTempData, ucUIDTemp);
		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
	{
		result = EI_paypass_ucSelect(ucTempData, ucUIDTemp, &ucSAK);
		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}
	// 保存第二级UID参数
	memcpy(EG_mifs_tWorkInfo.ucUIDCL2, ucUIDTemp, 5);

	if (EG_mifs_tWorkInfo.ucUIDLen == 7)
	{
		if ((ucSAK & 0x20) != 0x20)
		{
			//b5=0 不符合ISO14443-3协议 M1卡还需要判断是否为Pro卡
			// 2011-12-31  yehf 修改
			if ((ucSAK & 0x18) == 0x18)
			{
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_S70;
			}
			else
			{
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_S50;
			}
		}
		else
		{
			if ((ucSAK & 0x18) == 0x08)
			{
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PRO_S50;
			}
			else if ((ucSAK & 0x18) == 0x18)
			{
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PRO_S70;
			}
			else
			{
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PROCARD;
			}
		}
		return EM_mifs_SUCCESS;
	}

	// 获取第三级UID参数
	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
	{
		ucTempData = EM_mifs_PICC_ANTICOLL3;	// 第三级冲突
		result = EI_paypass_ucAnticoll(ucTempData, ucUIDTemp);
		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
	{
		result = EI_paypass_ucSelect(ucTempData, ucUIDTemp, &ucSAK);
		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	// 保存第三级UID参数
	memcpy(EG_mifs_tWorkInfo.ucUIDCL3, ucUIDTemp, 5);

	if ((ucSAK & 0x20) != 0x20)
	{
		//b5=0 不符合ISO14443-3协议 M1卡还需要判断是否为Pro卡
		// 2011-12-31  yehf 修改
		if ((ucSAK & 0x18) == 0x18)
		{
			EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_S70;
		}
		else
		{
			EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_S50;
		}
	}
	else
	{
		if ((ucSAK & 0x18) == 0x08)
		{
			EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PRO_S50;
		}
		else if ((ucSAK & 0x18) == 0x18)
		{
			EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PRO_S70;
		}
		else
		{
			EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_PROCARD;
		}
	}
	return EM_mifs_SUCCESS;
#endif
}

/****************************************************************
* 函数名称：
*       EI_paypass_ucActPro(uchar* pucOutLen, uchar* pucATSData)
* 功能描述：
*       激活Pro卡
* 被以下函数调用：
*       无
* 调用以下函数：
*       EI_paypass_ucWUPA
*       EI_paypass_ucSelect
*       EI_paypass_ucRATS
* 输入参数：
*       无
* 输出参数：
*       pucOutLen   Pro卡返回的ATS长度
*       pucATSData  Pro卡返回的ATS数据
* 返 回 值：
*       EM_mifs_SUCCESS   获取成功
*       EM_mifs_PROTERR   返回的参数不符合规范
*       EM_mifs_TRANSERR  通信错误
*       EM_mifs_TIMEOUT   卡无响应
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
* 说    明：
*        1、在激活过程中，如果出现超时无应答错误，则终端应重发该命令，总发送次数可达3次
*        2、如果出现通信错误，则直接返回
*        3、如果出现协议错误，则直接返回
*******************************************************************/
uchar EI_paypass_ucActPro(uchar * pucOutLen, uchar * pucATSData)
{
	uchar result = EM_SUCCESS;
	uchar ucTempData = 0x00;
	uchar ucSAK = 0x00;
	uchar ucResp[2];
	uchar ucReSendNo = 0;

	// 超时情况下，最多发送三次同一条命令
	EI_paypass_vSelectType(EM_mifs_TYPEA);
//	s_DelayMs(6);
	s_DelayUs(EM_paypass_polltime);
	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
	{
		// 先进行WUPA操作
		result = EI_paypass_ucWUPA(ucResp);
		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	// 其他错误情况则不重发，直接返回给终端
	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}
	else
	{
		// 先判断获取的ATQA参数是否已保存的ATQA参数一致
		if (memcmp(ucResp, EG_mifs_tWorkInfo.ucATQA, 2))
		{
			return EM_mifs_PROTERR;
		}
	}

	if(!(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV))
	{
		//ISO 通常模式
		// 进行第一级SELECT
		for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
		{
			ucTempData = EM_mifs_PICC_ANTICOLL1;
			result = EI_paypass_ucSelect(ucTempData, EG_mifs_tWorkInfo.ucUIDCL1, &ucSAK);
			if (result != EM_mifs_TIMEOUT)
			{
				break;
			}
		}

		if (result != EM_mifs_SUCCESS)
		{
			return result;
		}
		// UID为4个字节，一次Select操作即可使PICC进入ACTIVATE状态
		if (EG_mifs_tWorkInfo.ucUIDLen == 4)
		{
			goto ACTEND;
		}

		// 进行第二级SELECT
		for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
		{
			ucTempData = EM_mifs_PICC_ANTICOLL2;
			result = EI_paypass_ucSelect(ucTempData, EG_mifs_tWorkInfo.ucUIDCL2, &ucSAK);
			if (result != EM_mifs_TIMEOUT)
			{
				break;
			}
		}

		if (result != EM_mifs_SUCCESS)
		{
			return result;
		}
		// UID为7个字节，两次Select操作即可使PICC进入ACTIVATE状态
		if (EG_mifs_tWorkInfo.ucUIDLen == 7)
		{
			goto ACTEND;
		}

		// 进行第三级SELECT

		// UID为10个字节，三次Select操作即可使PICC进入ACTIVATE状态
		for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
		{
			ucTempData = EM_mifs_PICC_ANTICOLL3;
			result = EI_paypass_ucSelect(ucTempData, EG_mifs_tWorkInfo.ucUIDCL3, &ucSAK);
			if (result != EM_mifs_TIMEOUT)
			{
				break;
			}
		}

		if (result != EM_mifs_SUCCESS)
		{
			return result;
		}
	}
	else if((EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV))
	{
		//EMV 认证模式
		//EMV模式poll无法获取ID 补做EI_paypass_ucGetUID重新获取卡类型
		result = EI_paypass_ucGetUID();

		if(result != EM_mifs_SUCCESS)
		{
			return result;
		}
		else
		{
			goto ACTEND;
		}
	}

  ACTEND:
	if ((EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S50)
		|| (EG_mifs_tWorkInfo.ucMifCardType == EM_mifs_S70))
	{
		*pucOutLen = 0;
		return EM_mifs_SUCCESS;
	}
	// SELECT操作使PICC进入ACTIVATE状态，应再发送RATS获取ATS内容
	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
	{
		result = EI_paypass_ucRATS(pucOutLen, pucATSData);

		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_OK;
	EG_mifs_tWorkInfo.ucCurPCB = 0;

	return EM_mifs_SUCCESS;
}

/****************************************************************
* 函数名称：
*       EI_paypass_ucActTypeB(uchar* pucOutLen, uchar* pucOut)
* 功能描述：
*       激活TypeB卡
* 被以下函数调用：
*       无
* 调用以下函数：
*       EI_paypass_ucWUPB
*       EI_paypass_ucAttrib
* 输入参数：
*       无
* 输出参数：
*       pucOutLen   TypeB卡返回长度
*       pucOut      TypeB卡返回数据
* 返 回 值：
*       EM_mifs_SUCCESS   获取成功
*       EM_mifs_PROTERR   返回的参数不符合规范
*       EM_mifs_TRANSRR   通信错误
*       EM_mifs_TIMEOUT   卡无响应
*       EM_mifs_NOSUP4    卡片不支持ISO14443-4协议
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
* 说    明：
*        1、在激活过程中，如果出现超时无应答错误，则终端应重发该命令，总发送次数可达3次
*        2、如果出现通信错误，则直接返回
*        3、如果出现协议错误，则直接返回
*******************************************************************/
uchar EI_paypass_ucActTypeB(uchar * pucOutLen, uchar * pucOut)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData = 0x00;
	uchar ucReSendNo = 0;

	// 先执行WUPB命令
	EI_paypass_vSelectType(EM_mifs_TYPEB);
	s_DelayMs(6);
	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
	{
		result = EI_paypass_ucWUPB(pucOutLen, pucOut);
		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	// 比较ATQB参数与原先保存的ATQB参数
	if (memcmp(pucOut, EG_mifs_tWorkInfo.ucATQB, 12))
	{
		return EM_mifs_PROTERR;
	}

	// 接下来执行ATTRIB命令使TypeB卡进入ACTIVATE状态
	for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
	{
		result = EI_paypass_ucAttrib(&ucTempData);
		if (result != EM_mifs_TIMEOUT)
		{
			break;
		}
	}

	if (result != EM_mifs_SUCCESS)
	{
		return result;
	}

	EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_OK;
	EG_mifs_tWorkInfo.ucCurPCB = 0;

	return EM_mifs_SUCCESS;
}

/***********************************************************
* 函数名称：
*        EI_paypass_vSelectType(uchar ucMifType)
* 功能描述：
*        选择载波调制类型
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*       ucMifType     载波调制类型，取值如下：
*　　　　　　　　　　 EM_mifs_TYPEA  TypeA型调制
*                     EM_mifs_TYPEB  TypeB型调制
* 输出参数：
*       无
* 返 回 值：
*       无
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
************************************************************/
//extern uchar gCWGSNON;
//extern uint EG_Param[20];
void EI_paypass_vSelectType(uchar ucMifType)
{
	uchar ucTempData = 0;

	if (ucMifType == EM_mifs_TYPEA)
	{
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
            ucTempData = ucTempData;
            emvHalSetStandard(EMV_HAL_TYPE_A);
            emvHalSetErrorHandling(EMV_HAL_PREACTIVATION_ERROR_HANDLING);
#endif

        }
        else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
            ucTempData = 0x8E;//EG_Param[0];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_DRVMODE, &ucTempData);
            ucTempData = 0x08;//EG_Param[1];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXAMP, &ucTempData);
            ucTempData = 0x21;//EG_Param[2];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_DRVCON, &ucTempData);
            ucTempData = 0x1A;//EG_Param[3];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXI, &ucTempData);
            ucTempData = 0x18;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXCRCCON, &ucTempData);
            ucTempData = 0x18;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXCRCCON, &ucTempData);
            ucTempData = 0x27;//EG_Param[4];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXMODWIDTH, &ucTempData);
            ucTempData = 0xC0;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXWAITCTRL, &ucTempData);
            ucTempData = 0x12;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXWAITLO, &ucTempData);
            ucTempData = 0xCF;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_FRAMECON, &ucTempData);
            ucTempData = 0x04;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXCTRL, &ucTempData);
            ucTempData = 0x32;//EG_Param[5];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXTHRESHOLD, &ucTempData);
            ucTempData = 0x12;//EG_Param[6];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RCV, &ucTempData);
            ucTempData = 0x0A;//EG_Param[7];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXANA, &ucTempData);
//            ucTempData = 0x12;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RCV, &ucTempData);
//            ucTempData = 0x0A;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXANA, &ucTempData);

            ucTempData = 0x20;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXBITMOD, &ucTempData);
//            ucTempData = 0x04;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXDATACON, &ucTempData);
            ucTempData = 0x50;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXDATAMOD, &ucTempData);
            ucTempData = 0x40;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYMFREG, &ucTempData);
            ucTempData = 0x00;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM0L, &ucTempData);

            ucTempData = 0x00;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM1L, &ucTempData);
//            ucTempData = 0x00;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM10LEN, &ucTempData);
//            ucTempData = 0x00;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM10MOD, &ucTempData);
            ucTempData = 0x50;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM32MOD, &ucTempData);
            ucTempData = 0x02;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXBITMOD, &ucTempData);
            ucTempData = 0x01;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXSYNCVALL, &ucTempData);
            ucTempData = 0x00;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXSYNCMOD, &ucTempData);
            ucTempData = 0x08;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXMOD, &ucTempData);
//            ucTempData = 0x80;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXCORR, &ucTempData);
//            ucTempData = 0xF0;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_FABCALI, &ucTempData);
            s_DelayUs(5000); //RC663 开载波需要延时 稳定系统
#endif

        }
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
		{

#ifdef EM_PN512_Module

			// TYPE A卡
			ucTempData = 0x10;
			EI_mifs_vWriteReg(1, ControlReg, &ucTempData);

			ucTempData = 0x03;
			EI_mifs_vSetBitMask(TxControlReg, ucTempData);	// TX1 & TX2 enable

			ucTempData = 0x40;
			EI_mifs_vWriteReg(1, TxAutoReg, &ucTempData);	  //Force100ASK,
			ucTempData = 0x00;
			EI_mifs_vWriteReg(1, TxModeReg, &ucTempData);
			ucTempData = 0x08;
			EI_mifs_vWriteReg(1, RxModeReg, &ucTempData);
//			ucTempData = 0x4D;
			ucTempData = gcMifReg.Demod;
			EI_mifs_vWriteReg(1, DemodReg, &ucTempData);
			//ucTempData = 0x0F;//0x3F (2012-4-20)
			ucTempData = gcMifReg.CWGsP;
			EI_mifs_vWriteReg(1, CWGsPReg, &ucTempData);

			//ucTempData = 0xF4;//gCWGSNON:0x64(2012-4-20)
			ucTempData = gcMifReg.GsNOn;
			EI_mifs_vWriteReg(1, GsNOnReg, &ucTempData);

//			ucTempData = 0x84;
			ucTempData = gcMifReg.RxThresholdTypeA;
			EI_mifs_vWriteReg(1, RxThresholdReg, &ucTempData);
			ucTempData = gcMifReg.RFCfg;//0x48;
			EI_mifs_vWriteReg(1, RFCfgReg, &ucTempData);

			ucTempData = 0x40;
			EI_mifs_vClrBitMask(ModeReg, ucTempData);

			ucTempData = 0x10;
			EI_mifs_vClrBitMask(ModeReg,ucTempData);

#endif

		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
		{

#ifdef EM_RC531_Module

            // TYPE A卡
            ucTempData = 0x5b;
            EI_mifs_vWriteReg(1, EM_mifs_REG_TXCONTROL, &ucTempData);	// Force100ASK, TX1 & TX2 enable
            ucTempData = 0x19;
            EI_mifs_vWriteReg(1, EM_mifs_REG_CODERCONTROL, &ucTempData);	// Miller coding, 106kbps
            ucTempData = 0x08;
            EI_mifs_vWriteReg(1, EM_mifs_REG_DECODERCONTROL, &ucTempData);	// Manchester Coding
            ucTempData = 0x63;
            EI_mifs_vWriteReg(1, EM_mifs_REG_CRCPRESETLSB, &ucTempData);	// set CRC preset to 0x6363
            ucTempData = 0x63;
            EI_mifs_vWriteReg(1, EM_mifs_REG_CRCPRESETMSB, &ucTempData);
		#ifndef RFID_ENABLE_REG
            ucTempData = 0xFF;
            EI_mifs_vWriteReg(1, EM_mifs_REG_RXTHRESHOLD, &ucTempData);	// set max MinLevel & ColLevel.
            ucTempData = 0x73;
            EI_mifs_vWriteReg(1, EM_mifs_REG_RXCONTROL1, &ucTempData);	// Manchester Coding
            ucTempData = 0x06;
            EI_mifs_vWriteReg(1, EM_mifs_REG_RXWAIT, &ucTempData);	// set Guard Time.
            ucTempData = 0x3F;
            EI_mifs_vWriteReg(1, EM_mifs_REG_CWCONDUCTANCE, &gtRfidDebugInfo.CW_A);
        #else
            EI_mifs_vWriteReg(1, EM_mifs_REG_RXTHRESHOLD, &gtRfidRegInfo.ARxThreadhold);	// set max MinLevel & ColLevel.
			EI_mifs_vWriteReg(1, EM_mifs_REG_RXCONTROL1, &gtRfidRegInfo.ARxControl_1);	// Manchester Coding
			EI_mifs_vWriteReg(1, EM_mifs_REG_RXWAIT, &gtRfidRegInfo.ARxWait);	// set Guard Time.
			EI_mifs_vWriteReg(1, EM_mifs_REG_CWCONDUCTANCE, &gtRfidRegInfo.ATxCW);
			EI_mifs_vWriteReg(1, EM_mifs_REG_MFOUTSELECT, &gtRfidRegInfo.AMFout);
        #endif

#endif

		}

		EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEA;
	}
	else if(ucMifType == EM_mifs_TYPEB)
	{
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
            ucTempData = ucTempData;
            emvHalSetStandard(EMV_HAL_TYPE_B);
            emvHalSetErrorHandling(EMV_HAL_PREACTIVATION_ERROR_HANDLING);
#endif

        }
        else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
            ucTempData = 0x8F;//EG_Param[8];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_DRVMODE, &ucTempData);
            ucTempData = 0x4F;//EG_Param[9];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXAMP, &ucTempData);
            ucTempData = 0x01;//EG_Param[10];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_DRVCON, &ucTempData);
            ucTempData = 0x06;//EG_Param[11];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXI, &ucTempData);
            ucTempData = 0x7B;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXCRCCON, &ucTempData);
//            ucTempData = 0x79;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXCRCCON, &ucTempData);
            ucTempData = 0x08;//0x0F
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXDATANUM, &ucTempData);
            ucTempData = 0x00;//EG_Param[12];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXMODWIDTH, &ucTempData);
            ucTempData = 0x00;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM10BURSTLEN, &ucTempData);
            ucTempData = 0x01;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXWAITCTRL, &ucTempData);
            ucTempData = 0x00;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXWAITLO, &ucTempData);
            ucTempData = 0x05;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_FRAMECON, &ucTempData);
            ucTempData = 0x34;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXCTRL, &ucTempData);
//            ucTempData = 0x90;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXWAIT, &ucTempData);
            ucTempData = 0x3F;//EG_Param[13];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXTHRESHOLD, &ucTempData);
            ucTempData = 0x12;//EG_Param[14];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RCV, &ucTempData);
            ucTempData = 0x0A;//EG_Param[15];
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXANA, &ucTempData);
//            ucTempData = 0x12;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RCV, &ucTempData);
//            ucTempData = 0x0A;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXANA, &ucTempData);

            ucTempData = 0x09;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXBITMOD, &ucTempData);
//            ucTempData = 0x04;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXDATACON, &ucTempData);
            ucTempData = 0x08;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXDATAMOD, &ucTempData);
            ucTempData = 0x04;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYMFREG, &ucTempData);
            ucTempData = 0x03;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM0L, &ucTempData);

            ucTempData = 0x01;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM1L, &ucTempData);
            ucTempData = 0xAB;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM10LEN, &ucTempData);
            ucTempData = 0x08;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM10MOD, &ucTempData);
//            ucTempData = 0x04;
//            EI_mifs_vWriteReg(1, 0x57, &ucTempData);
            ucTempData = 0x04;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXBITMOD, &ucTempData);
//            ucTempData = 0x04;
//            EI_mifs_vWriteReg(1, 0x5B, &ucTempData);
            ucTempData = 0x02;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXSYNCMOD, &ucTempData);
            ucTempData = 0x0D;
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXMOD, &ucTempData);
//            ucTempData = 0x80;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXCORR, &ucTempData);
//            ucTempData = 0xF0;
//            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_FABCALI, &ucTempData);

            ucTempData = 0;
//            EI_mifs_vWriteReg(1, 0x4D, &ucTempData);
//            EI_mifs_vWriteReg(1, 0x4F, &ucTempData);
//            EI_mifs_vWriteReg(1, 0x51, &ucTempData);
//            EI_mifs_vWriteReg(1, 0x52, &ucTempData);
//            EI_mifs_vWriteReg(1, 0x54, &ucTempData);
//            EI_mifs_vWriteReg(1, 0x55, &ucTempData);
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_TXSYM32MOD, &ucTempData);
//            EI_mifs_vWriteReg(1, 0x59, &ucTempData);
//            EI_mifs_vWriteReg(1, 0x5A, &ucTempData);
            EI_mifs_vWriteReg(1, PHHAL_HW_RC663_REG_RXSYNCVALL, &ucTempData);  //一定要清零
            s_DelayUs(5000); //RC663 开载波需要延时 稳定系统
#endif
        }
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
		{

#ifdef EM_PN512_Module
			// TYPE-B卡
			ucTempData = 0x03;
			EI_mifs_vSetBitMask(TxControlReg, ucTempData);	// TX1 & TX2 enable

			ucTempData = 0x00;
			EI_mifs_vWriteReg(1, TxAutoReg, &ucTempData);
			ucTempData = 0x10;
			EI_mifs_vWriteReg(1, ControlReg, &ucTempData);
			ucTempData = 0x03;
			EI_mifs_vWriteReg(1, TxModeReg, &ucTempData);
			ucTempData = 0x0B;
			EI_mifs_vWriteReg(1, RxModeReg, &ucTempData);
			ucTempData = 0x00; //0x03
			EI_mifs_vWriteReg(1, TypeBReg, &ucTempData);

//			ucTempData = 0x4D;
			ucTempData = gcMifReg.Demod;
			EI_mifs_vWriteReg(1, DemodReg, &ucTempData);
			//ucTempData = 0xF4;//gCWGSNON:0x64(2012-4-20)
			ucTempData = gcMifReg.GsNOn;
			EI_mifs_vWriteReg(1, GsNOnReg, &ucTempData);
			//ucTempData = 0x0F;//0x3F(2012-4-20)
			ucTempData = gcMifReg.CWGsP;
			EI_mifs_vWriteReg(1, CWGsPReg, &ucTempData);
			//ucTempData = 0x04;  //0x0B(2012-4-20)
			ucTempData = gcMifReg.ModGsP;
			EI_mifs_vWriteReg(1, ModGsPReg, &ucTempData);
			ucTempData = gcMifReg.GsNOff;
			EI_mifs_vWriteReg(1, GsNOffReg, &ucTempData);
//			ucTempData = 0x55;
			ucTempData = gcMifReg.RxThresholdTypeB;
			EI_mifs_vWriteReg(1, RxThresholdReg, &ucTempData);
			ucTempData = gcMifReg.RFCfg;//0x68;
			EI_mifs_vWriteReg(1, RFCfgReg, &ucTempData);

			ucTempData = 0x40;
			EI_mifs_vClrBitMask(ModeReg, ucTempData);

			ucTempData = 0x10;
			EI_mifs_vClrBitMask(ModeReg,ucTempData);

#endif

		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
		{

#ifdef EM_RC531_Module

            // TYPE-B卡
            ucTempData = 0x4b;
            EI_mifs_vWriteReg(1, EM_mifs_REG_TXCONTROL, &ucTempData);	// disable Force100ASk
            EI_mifs_vWriteReg(1, EM_mifs_REG_MODCONDUCTANCE, &gtRfidDebugInfo.ModConduct_B);	// set modulation index at 12% 根据实际情况进行调整的结果
            ucTempData = 0x20;
            EI_mifs_vWriteReg(1, EM_mifs_REG_CODERCONTROL, &ucTempData);	// NRZ-L, TypeB baud 106kbps
            ucTempData = 0x23;
            EI_mifs_vWriteReg(1, EM_mifs_REG_TYPEBFRAMING, &ucTempData);	// EGT=0
            ucTempData = 0x19;
            EI_mifs_vWriteReg(1, EM_mifs_REG_DECODERCONTROL, &ucTempData);	// Manchester Coding
            ucTempData = 0xff;
            EI_mifs_vWriteReg(1, EM_mifs_REG_CRCPRESETLSB, &ucTempData);	// set CRC preset to 0xFFFF
            ucTempData = 0xff;
            EI_mifs_vWriteReg(1, EM_mifs_REG_CRCPRESETMSB, &ucTempData);
            ucTempData = 0x5E;
            EI_mifs_vWriteReg(1, EM_mifs_REG_BPSKDEMCONTROL, &ucTempData);	//
		#ifndef RFID_ENABLE_REG
            ucTempData = 0x64;
            EI_mifs_vWriteReg(1, EM_mifs_REG_RXTHRESHOLD, &ucTempData);	// set max MinLevel & ColLevel.
            ucTempData = 0x73;
            EI_mifs_vWriteReg(1, EM_mifs_REG_RXCONTROL1, &ucTempData);
            ucTempData = 0x03;
            EI_mifs_vWriteReg(1, EM_mifs_REG_RXWAIT, &ucTempData);	// set Guard Time.
			ucTempData = 0x3F;							// TR0min + TR1min
//            EI_mifs_vWriteReg(1, EM_mifs_REG_CWCONDUCTANCE, &gtRfidDebugInfo.CW_B);
            EI_mifs_vWriteReg(1, EM_mifs_REG_CWCONDUCTANCE, &ucTempData);
		#else
			EI_mifs_vWriteReg(1, EM_mifs_REG_RXTHRESHOLD, &gtRfidRegInfo.BRxThreadhold);	// set max MinLevel & ColLevel.
			EI_mifs_vWriteReg(1, EM_mifs_REG_RXCONTROL1, &gtRfidRegInfo.BRxControl_1);	// Manchester Coding
			EI_mifs_vWriteReg(1, EM_mifs_REG_RXWAIT, &gtRfidRegInfo.BRxWait);	// set Guard Time.
			EI_mifs_vWriteReg(1, EM_mifs_REG_CWCONDUCTANCE, &gtRfidRegInfo.BTxCW);
			EI_mifs_vWriteReg(1, EM_mifs_REG_MFOUTSELECT, &gtRfidRegInfo.BMFout);
		#endif

#endif

		}

		EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEB;
	}
	else if(ucMifType == EM_mifs_NFCIP)
	{
		if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
		{

#ifdef EM_PN512_Module

#endif
		}
	}
#ifdef PRODUCT_F12_USB
	else
	{
        if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911)
        {

#ifdef EM_AS3911_Module

            //power down mode
//            emvHalActivateField(OFF);
            as3911WriteRegister(AS3911_REG_IO_CONF1, 0x0F);
            as3911WriteRegister(AS3911_REG_IO_CONF2, 0x80);
            /* Enable Oscillator, Transmitter and receiver. */
            as3911WriteRegister(AS3911_REG_OP_CONTROL, 0x00);
            as3911WriteRegister(AS3911_REG_MODE, 0x08);

#endif

        }
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
		{

#ifdef EM_RC531_Module
            EI_mifs_vClrBitMask(EM_mifs_REG_TXCONTROL, 0x03);
            EI_mifs_vSetBitMask(EM_mifs_REG_CONTROL, 1<<4);  //进入powerdown模式
#endif
        }
	}
#endif
	//s_DelayMs(20);
}


/***********************************************************
* 函数名称：
*       EI_paypass_ucPOLL(uchar* pucMifType)
* 功能描述：
*       轮询感应区，判断是否有卡进入感应区,返回卡类型
* 被以下函数调用：
*       无
* 调用以下函数：
*       无
* 输入参数：
*       无
* 输出参数：
*       pucMifType   感应区内的射频卡类型
* 返 回 值：
*       EM_mifs_SUCCESS   获取成功
*       EM_mifs_PROTERR   返回的参数不符合规范
*       EM_mifs_MULTIERR  多卡冲突
*       EM_mifs_TIMEOUT   卡无响应
*       EM_mifs_TRANSERR  通信错误
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
* 说    明：
*        1、如果在寻卡过程中出现通信错误，则按多卡冲突处理
*        2、如果在寻卡过程中出现协议错误，则直接返回
*******************************************************************/
uchar EI_paypass_ucPOLL(uchar * pucMifType)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData = 0x00;
	uchar ucATQA[2];					// 用于保存ATQA参数
	uchar ucATQB[12];					// 用于保存ATQB参数
	uchar i,j;
	memset(ucATQA, 0x00, sizeof(ucATQA));
	memset(ucATQB, 0x00, sizeof(ucATQB));

	// 发送WUPA命令，判断感应区是否有TypeA卡
	result = EI_paypass_ucWUPA(ucATQA);
	if (result == EM_mifs_TRANSERR)
	{
		for (i = 0; i < 10; i++)
		{
			result = EI_paypass_ucWUPA(ucATQA);
			if (result != EM_mifs_TRANSERR)
			{
				break;
			}
		}
	}

    if(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV){
		//EMV 认证模式
        if(result == EM_mifs_MULTIERR){
            //如果A卡寻卡冲突后 直接报多卡冲突 先在EMV模式下修改
            EI_paypass_vHALTA();
			result = EI_paypass_ucWUPA(ucATQA);
//            EI_paypass_vHALTA();
            if(result == EM_mifs_MULTIERR){
//                lcd_cls();
//                lcd_display(0, DISP_FONT_LINE0,DISP_CLRLINE, "ret:%x",result);
//                while ( 1 ) {
//                    sys_beep_pro(4000,300,YES);
//                    if ( kb_getkey(500) == KEY1 ) {
//                        break;
//                    }
//                }
                return result;
            }
        }
    }

	if(result == EM_mifs_TRANSERR || result == EM_mifs_PROTERR || result == EM_mifs_MULTIERR)
	{
		//yehf 增加 WUPA响应错误，将卡置为Idle状态
		EI_paypass_vHALTA();
		goto EI_paypass_ucPOLLB;
	}
	else if (result == EM_mifs_SUCCESS)
	{
		// 保存获取的ATQA参数
		EG_mifs_tWorkInfo.ucATQA[0] = ucATQA[0];
		EG_mifs_tWorkInfo.ucATQA[1] = ucATQA[1];

		if(!(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV))
		{
			//ISO 通常模式
			// 获取UID信息，并判断是否有多卡存在
			result = EI_paypass_ucGetUID();
			if(result == EM_mifs_SUCCESS)
			{
				if(EG_mifs_tWorkInfo.ucMifCardType <= EM_mifs_S70)
				{
					// 2011-09-05  哈尔滨电信调试 逻辑卡和CPU卡双功能寻卡返回逻辑卡，但是仍然可以按CPU功能继续激活
					j = 1;
				}
				else
				{
					j = EM_paypass_TRYON_TIMES;
				}
				EG_mifs_tWorkInfo.ATS[0] = 0;
				for (i = 0; i < j; i++)
				{
					result = EI_paypass_ucRATS(&ucTempData, EG_mifs_tWorkInfo.ATS);
					if (result != EM_mifs_TIMEOUT)
					{
						break;
					}
				}
				if(EG_mifs_tWorkInfo.ucMifCardType <= EM_mifs_S70)
				{
					if(result == EM_mifs_SUCCESS)
						EG_mifs_tWorkInfo.ucMifCardType += 3; //变为CPU+M1
					else
					{
						result = EM_mifs_SUCCESS; //纯M1逻辑卡直接返回成功
					}
				}
				
                //ISO模式RAT命令失败
				if (result != EM_mifs_SUCCESS)
				{
					return result;
				}
				
				EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_OK;
				EG_mifs_tWorkInfo.ucCurPCB = 0;
			}
			else
			{
				return result;
			}

			//如果是ISO模式 TYPE A到此结束
			if (EG_mifs_tWorkInfo.ucAnticollFlag == 1)
			{
				//ucAnticollFlag == 1 已经是ISO通常模式
                if(!(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV))
				{
					//ISO 通常模式
					// 如果允许多卡存在，则不再判断是否还有B卡
					*pucMifType = EG_mifs_tWorkInfo.ucMifCardType;
				}
				EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEA;
				return EM_mifs_SUCCESS;
			}

		}

		//if (result == EM_mifs_SUCCESS)
		//EMV 认证模式
		if((EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV))
		{
			//EMV 认证模式
			//if(EG_mifs_tWorkInfo.ucOptMode == RFID_MODE_EMV)
			{
				//EMV 认证模式
				// 将该卡置为HALT态
				EI_paypass_vHALTA();
			}

			EI_paypass_vSelectType(EM_mifs_TYPEB);
//			EI_paypass_vDelay(EM_mifs_TPDEALY);	// 延时500ETU
            s_DelayUs(EM_paypass_polltime); //emv要求(针对RC531) 调整时间
			// 接下来判断是否有TypeB卡存在
			result = EI_paypass_ucWUPB(&ucTempData, ucATQB);
			if (result != EM_mifs_TIMEOUT)
			{
				/*
				if(EG_mifs_tWorkInfo.ucOptMode == RFID_MODE_EMV)
				{
					//EMV 认证模式
					s_DelayMs(8);
				}
				*/
				// 卡有响应，说明感应区内除了TypeA外，还有TypeB卡
				// 应该返回多卡冲突
                // 当type b返回成功时才返回多卡冲突
                // 修改原因:s980无线模块启动时 wupb命令发送后会产生接收中段
                // 但FIFO内没有数据  2013-04-26 chenf
                if (result == EM_mifs_SUCCESS)
                    return EM_mifs_MULTIERR;
			}
			// 否则，说明感应区内只有一张TypeA卡
			//EI_paypass_vDelay(EM_mifs_TPDEALY);	// 成功延时500ETU 在ActPro/ActTypeB时延时

			/*
			if(EG_mifs_tWorkInfo.ucOptMode == RFID_MODE_EMV)
			{
				//EMV 认证模式
				//EMV模式则强行变为EM_mifs_PROCARD卡 后面在EI_paypass_ucGetUID中重新处理
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_TYPEA;
			}
			*/

			*pucMifType = EG_mifs_tWorkInfo.ucMifCardType;
			EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEA;
			return EM_mifs_SUCCESS;
		}
		else
		{
			/*
			if(EG_mifs_tWorkInfo.ucOptMode == RFID_MODE_EMV)
			{
				//EMV 认证模式
				s_DelayMs(8);
			}
			return result;
			*/
		}
	}
	else if (result == EM_mifs_TIMEOUT)
	{
	EI_paypass_ucPOLLB:
		// 感应区没有TypeA卡响应，说明感应区内没有TypeA卡存在
		// 接下来应该判断是否TypeB卡存在
		EI_paypass_vSelectType(EM_mifs_TYPEB);
		EI_paypass_vDelay(EM_mifs_TPDEALY);	// 延时500ETU
		//s_DelayMs(50);
		// 接下来判断是否有TypeB卡存在
		result = EI_paypass_ucWUPB(&ucTempData, ucATQB);
		if (result == EM_mifs_TRANSERR)
		{
			for (i = 0; i < 10; i++)
			{
				result = EI_paypass_ucWUPB(&ucTempData, ucATQB);
				if (result != EM_mifs_TRANSERR)
				{
					break;
				}
			}
		}
		if (result == EM_mifs_SUCCESS)
		{
			// 保存ATQB参数
			memcpy(EG_mifs_tWorkInfo.ucATQB, ucATQB, ucTempData);

            if(!(EG_mifs_tWorkInfo.OptMode&RFID_MODE_EMV))
			{
				//ISO 通常模式
				// 接下来将TypeB卡置为HALT状态
				for (i = 0; i < EM_paypass_TRYON_TIMES; i++)
				{
					result = EI_paypass_ucHALTB(&ucTempData);
					if (result != EM_mifs_TIMEOUT)
					{
						break;
					}
				}
			}

			if (result == EM_mifs_SUCCESS)
			{
				if (EG_mifs_tWorkInfo.ucAnticollFlag == 1)
				{
					//ucAnticollFlag == 1 已经是ISO通常模式
					// 如果允许多卡存在，则不再判断是否还有A卡
					EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_TYPEBCARD;
					*pucMifType = EG_mifs_tWorkInfo.ucMifCardType;
					EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEB;
					return EM_mifs_SUCCESS;
				}

				EI_paypass_vSelectType(EM_mifs_TYPEA);
//				EI_paypass_vDelay(EM_mifs_TPDEALY);	// 延时500ETU
                s_DelayUs(EM_paypass_polltime); //emv要求(针对RC531) 调整时间

				result = EI_paypass_ucWUPA(ucATQA);
				if (result != EM_mifs_TIMEOUT)
				{
					/*
					if(EG_mifs_tWorkInfo.ucOptMode == RFID_MODE_EMV)
					{
						//EMV 认证模式
						s_DelayMs(8);
					}
					*/
					// 有TypeA卡响应，则说明存在TypeA，多卡冲突
                    // 当TYPE A返回成功时才返回多卡冲突(同上)
                    // 修改原因:wp70 wupa命令发送后也会产生接收中断 原因不明
                    // 但FIFO内没有数据  2014-09-11 chenf
                    if (result == EM_mifs_SUCCESS)
                        return EM_mifs_MULTIERR;
				}

				//EI_paypass_vDelay(EM_mifs_TPDEALY);	// 成功延时500ETU 在ActPro/ActTypeB时延时
				// 感应区内只有一张TypeB卡
				EG_mifs_tWorkInfo.ucMifCardType = EM_mifs_TYPEBCARD;
				*pucMifType = EG_mifs_tWorkInfo.ucMifCardType;
				EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEB;
				return EM_mifs_SUCCESS;
			}
			else
			{
				/*
				if(EG_mifs_tWorkInfo.ucOptMode == RFID_MODE_EMV)
				{
					//EMV 认证模式
					s_DelayMs(8);
				}
				*/
				return result;
			}
		}
		else if (result == EM_mifs_TIMEOUT)
		{
			//EI_paypass_vDelay(EM_mifs_TPDEALY);	// 延时500ETU
		}
		else
		{
			/*
			if(EG_mifs_tWorkInfo.ucOptMode == RFID_MODE_EMV)
			{
				//EMV 认证模式
				s_DelayMs(8);
			}
			*/
		}
		return result;
	}
	/*
	if(EG_mifs_tWorkInfo.ucOptMode == RFID_MODE_EMV)
	{
		//EMV 认证模式
		s_DelayMs(8);
	}
	*/
	// 其他错误情况，直接返回
	return result;

}

/*****************************************************************
* 函数名称：
*        EI_paypass_ucIfMifExit(void)
* 功能描述：
*        判断是否已经将Mif卡离开感应区
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        EM_mifs_SUCCESS   卡片已离开感应区
*        EM_mifs_CARDEXIST 卡片还在感应区
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
**********************************************************/
uchar EI_paypass_ucIfMifExit(void)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData[12];
	uchar ucTemp = 0x00;
	uchar ucReSendNo = 0;

	if (EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEA)
	{
		// 先发送WUPA寻卡，如果有卡响应，则说明卡未离开感应区
		for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
		{
			result = EI_paypass_ucWUPA(ucTempData);
			if (result != EM_mifs_TIMEOUT)
			{
				break;
			}

			if ((result == EM_mifs_TIMEOUT) && (ucReSendNo == 2))
			{
				// 无卡响应，则说明卡已离开感应区
				return EM_mifs_SUCCESS;
			}
		}
		// 卡有响应，说明卡未离开感应区
		// 先将卡片置为HALT态
		EI_paypass_vHALTA();
		return EM_mifs_CARDEXIST;
	}
	else
	{
		// 先发送WUPB寻卡，如果有卡响应，则说明卡未离开感应区
		for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
		{
			result = EI_paypass_ucWUPB(&ucTemp, ucTempData);
			if (result != EM_mifs_TIMEOUT)
			{
				break;
			}
			if ((result == EM_mifs_TIMEOUT) && (ucReSendNo == 2))
			{
				// 无卡响应，则说明卡已离开感应区
				return EM_mifs_SUCCESS;
			}
		}
		// 卡有响应，说明卡未离开感应区
		// 先将卡片置为HALT态
		for (ucReSendNo = 0; ucReSendNo < EM_paypass_TRYON_TIMES; ucReSendNo++)
		{
			result = EI_paypass_ucHALTB(&ucTemp);
			if (result != EM_mifs_TIMEOUT)
			{
				break;
			}
			if ((result == EM_mifs_TIMEOUT) && (ucReSendNo == 2))
			{
				// 无卡响应，则说明卡已离开感应区
				return EM_mifs_SUCCESS;
			}
		}
	}

	return EM_mifs_CARDEXIST;
}

/**********************************************************************
* 函数名称：
*       EI_paypass_ucProcess(uint uiSendLen, uint* puiRecLen)
* 功能描述:
*       在PCD与PICC之间发送和接收数据
* 被以下函数调用:
*       上层函数
* 调用以下函数:
*       无
* 输入参数:
*       uiSendLen   要发送的数据长度
* 输出参数：
*       puiRecLen   接收到的数据长度
* 返 回 值：
*       EM_mifs_SUCCESS    接收成功
*       EM_mifs_TIMEOUT    超时无响应
*       EM_mifs_TRANSERR   通信错误
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
***************************************************************/
uchar EI_paypass_ucProcess(uint uiSendLen, uint *puiRecLen)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData = 0x00;
	uchar ucCMD = 0;
	// 设置定时器
	EI_paypass_vSetTimer(EG_mifs_tWorkInfo.ulFWT);

	if (EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEA)
	{
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
            ucTempData = ucTempData;
            EI_paypass_vSelectType(EM_mifs_TYPEA);
            ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
            EG_mifs_tWorkInfo.expectMaxRec = sizeof(EG_mifs_tWorkInfo.aucBuffer);
#endif

        }
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
		{

#ifdef EM_RC663_Module
            ucTempData = 0x01;
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);
            ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif
		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
		{

#ifdef EM_PN512_Module

			ucTempData = 0x80;
			EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
			EI_mifs_vSetBitMask(TxModeReg, ucTempData);
			EI_mifs_vSetBitMask(RxModeReg, ucTempData);
			ucCMD = PN512CMD_TRANSCEIVE;

#endif
		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
		{

#ifdef EM_RC531_Module

			ucTempData = 0x0F;
			EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
			ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif
		}
	}
	else
	{
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
            ucTempData = ucTempData;
            EI_paypass_vSelectType(EM_mifs_TYPEB);
            ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
            EG_mifs_tWorkInfo.expectMaxRec = sizeof(EG_mifs_tWorkInfo.aucBuffer);
#endif

        }
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
		{

#ifdef EM_RC663_Module

            ucTempData = 0x01;
            EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);
            ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;

#endif
		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
		{

#ifdef EM_PN512_Module

			EI_mifs_vSetBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
			EI_mifs_vSetBitMask(TxModeReg, 0x80);
			EI_mifs_vSetBitMask(RxModeReg, 0x80);
			ucCMD = PN512CMD_TRANSCEIVE;

#endif
		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
		{

#ifdef EM_RC531_Module

			ucTempData = 0x2C;
			EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
			ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif
		}
	}

	// 准备发送数据: 命令码
	EG_mifs_tWorkInfo.ulSendBytes = uiSendLen;	// how many bytes to send
	EG_mifs_tWorkInfo.ucDisableDF = 0;
	result = EI_mifs_ucHandleCmd(ucCMD);

	if (result == EM_mifs_NOTAGERR)
	{
		result = EM_mifs_TIMEOUT;
	}
	else if (result != EM_mifs_SUCCESS)
	{
		result = EM_mifs_TRANSERR;
	}

	if (result == EM_mifs_SUCCESS)
	{
		*puiRecLen = EG_mifs_tWorkInfo.ulBytesReceived;
		if (EG_mifs_tWorkInfo.ulBytesReceived == 0)
		{
			// 当接收正确，但是实际接收长度为0时，按传输错误处理
			// 2006.08.24

			result = EM_mifs_TRANSERR;
		}
		// 对于TypeB卡，当PICC早于TR0Min + TR1Min开始回送数据的话
		// 根据PAYPASS要求，可以认定为传输错误，也可以正常接收，
		// 在本程序里，不判断这种情况。
	}

    if ( EG_mifs_tWorkInfo.RFIDModule != RFID_Module_AS3911 ) {
//        EI_paypass_vDelay(EM_mifs_FDTMIN);	// 命令之间延时500us，以保证最小正向保护时间
        s_DelayUs(EM_paypass_minFDTadtime);	// emv要求(针对RC531) 调整时间刚好延时500us
    }

	return (result);
}
/**********************************************************************
* 函数名称：
*       生成I块
* 功能描述：
*       发送R_Block
* 被以下函数调用：
*       无
* 调用以下函数：
*       无
* 输入参数：
*       无
* 输出参数：
*       puiRecLen  收到的数据长度
* 返 回 值：
*       EM_mifs_SUCCESS    接收成功
*       EM_mifs_TIMEOUT    超时无响应
*       EM_mifs_TRANSERR   通信错误
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
***************************************************************/
uint make_IBlock(uchar PCB, uint inlen, uchar *in)
{
	uint i=0;
	EG_mifs_tWorkInfo.aucBuffer[i++] = PCB;
	if(EG_mifs_tWorkInfo.ucCIDFlag)
	{
		EG_mifs_tWorkInfo.aucBuffer[0] |= 0x08;  //有CID
		EG_mifs_tWorkInfo.aucBuffer[i++] = 0x00;
	}
	if(EG_mifs_tWorkInfo.ucNADFlag)
	{
		EG_mifs_tWorkInfo.aucBuffer[0] |= 0x04;  //有NAD
		EG_mifs_tWorkInfo.aucBuffer[i++] = 0x00;
	}
	memcpy(&EG_mifs_tWorkInfo.aucBuffer[i],in,inlen);
	i += inlen;
	return i;
}

uint make_RBlock(uchar PCB)
{
	uint i=0;
	EG_mifs_tWorkInfo.aucBuffer[i++] = PCB;
	if(EG_mifs_tWorkInfo.ucCIDFlag)
	{
		EG_mifs_tWorkInfo.aucBuffer[0] |= 0x08;  //有CID
		EG_mifs_tWorkInfo.aucBuffer[i++] = 0x00;
	}
	return i;
}
/**********************************************************************
* 函数名称：
*       EI_paypass_ucMifRBlock(uint* puiRecLen)
* 功能描述：
*       发送R_Block
* 被以下函数调用：
*       无
* 调用以下函数：
*       无
* 输入参数：
*       无
* 输出参数：
*       puiRecLen  收到的数据长度
* 返 回 值：
*       EM_mifs_SUCCESS    接收成功
*       EM_mifs_TIMEOUT    超时无响应
*       EM_mifs_TRANSERR   通信错误
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
***************************************************************/
uchar EI_paypass_ucMifRBlock(uint * puiRecLen)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucReSendNo = 0x00;
	uint sendlen;
	// 最多发送NAK两次
	for (ucReSendNo = 0; ucReSendNo < 2; ucReSendNo++)
	{
		sendlen = make_RBlock(EG_mifs_tWorkInfo.ucCurPCB|0xB2);
		result = EI_paypass_ucProcess(sendlen, puiRecLen);
		if ((result != EM_mifs_TIMEOUT) && (result != EM_mifs_TRANSERR))
		{
			// 如果出现超时或者传输错误，则应重发NAK
			break;
		}
	}
	return result;
}
/**********************************************************************
* 函数名称：
*       EI_paypass_ucMifSBlock(uint* puiRecLen)
* 功能描述：
*       发送S_Block
* 被以下函数调用：
*       无
* 调用以下函数：
*       无
* 输入参数：
*       无
* 输出参数：
*       puiRecLen  收到的数据长度
* 返 回 值：
*       EM_mifs_SUCCESS     接收成功
*       EM_mifs_TIMEOUT     超时无响应
*       EM_mifs_TRANSERR    通信错误
*       EM_mifs_PROTERR     协议错误
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
***************************************************************/
uchar EI_paypass_ucMifSBlock(uint * puiRecLen)
{
	uchar ucTempData = 0;
	uchar result = EM_mifs_SUCCESS;
	ulong tmp,i;
	uint sendlen;
	if (IF_SBLOCK_ERR(EG_mifs_tWorkInfo.aucBuffer[0]))
	{
		// PICC不应带CID字节
		return EM_mifs_PROTERR;
	}
	else
	{
		ucTempData = EG_mifs_tWorkInfo.aucBuffer[1];
		if ((ucTempData & 0xC0) != 0x00)
		{
			// 不支持Power Level
			return EM_mifs_PROTERR;
		}
		// 获取WTXM整数
		EG_mifs_tWorkInfo.ucWTX = ucTempData & 0x3F;
		if (EG_mifs_tWorkInfo.ucWTX == 0x00)
		{
			return EM_mifs_PROTERR;
		}
		if (EG_mifs_tWorkInfo.ucWTX > 59)
		{
			EG_mifs_tWorkInfo.ucWTX = 59;
		}

		// 计算临时FWT时间
		// TB412_12~14需要减少1
		if(EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEB
			&& (EG_mifs_tWorkInfo.ucFWI >=12 && EG_mifs_tWorkInfo.ucFWI <=14))
		{
			i = EG_mifs_tWorkInfo.ulFWT - 1;
		}
		else
		{
			i = EG_mifs_tWorkInfo.ulFWT;
		}
		EG_mifs_tWorkInfo.ulFWTTemp = EG_mifs_tWorkInfo.ucWTX * i;
	}
	//发送S应答
	sendlen = 0;
	EG_mifs_tWorkInfo.aucBuffer[sendlen++] = 0xf2;
	if(EG_mifs_tWorkInfo.ucCIDFlag)
	{
		EG_mifs_tWorkInfo.aucBuffer[0] |= 0x08;
		EG_mifs_tWorkInfo.aucBuffer[sendlen++] = 0x00;
	}
	EG_mifs_tWorkInfo.aucBuffer[sendlen++] = ucTempData & 0x3F;
	if(EG_mifs_tWorkInfo.ulFWTTemp > (1146880))  //EM_mifs_MAXFWT 2^15*35
	{
	    // 设置定时器,最大不超过FWT_MAX
	    //return EM_mifs_PROTERR;
	    EG_mifs_tWorkInfo.ulFWTTemp = s_rfid_FWI2FWT(14);
		 if(EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEB
			&& (EG_mifs_tWorkInfo.ucFWI >=12 && EG_mifs_tWorkInfo.ucFWI <=14))
		{
			EG_mifs_tWorkInfo.ulFWTTemp  -= 1;
		}
	}
	//D2(TRACE("\r\n FWT:d",EG_mifs_tWorkInfo.ulFWTTemp););
	tmp = EG_mifs_tWorkInfo.ulFWT;
	EG_mifs_tWorkInfo.ulFWT = EG_mifs_tWorkInfo.ulFWTTemp;
	EG_mifs_tWorkInfo.aucBuffer[sendlen-1] = ucTempData & 0x3F;
	result = EI_paypass_ucProcess(sendlen,puiRecLen);
	EG_mifs_tWorkInfo.ulFWT = tmp;
	/*
	if(++gtRfidProInfo.SBlock>=3)
	{
		//TA421连续S块不能超过3次
		return EM_mifs_PROTERR;
	}
	*/
	return (result);
}

/**********************************************************************
* 函数名称：
*       EI_paypass_ucExchange(uint uiSendLen, uchar* pucInData,
*                             uint* puiRecLen, uchar* pucOutData )
* 功能描述：
*      实现读卡器与卡之间的ISO14443-4协议数据交换
* 被以下函数调用：
*      API
* 调用以下函数：
*      无
* 输入参数：
*      uiSendLen    输入数据长度
*      pucInData    输入数据
* 输出参数：
*      puiRecLen    输出数据长度
*      pucOutData   输出数据
* 返 回 值:
*        EM_mifs_SUCCESS  数据交换成功（卡片返回状态字节为0x9000）
*        EM_mifs_SWDIFF   卡片返回状态字节为（不等于 0x9000）
*        EM_mifs_TIMEOUT  卡片无返回
*        EM_mifs_TRANSERR 通信错误
*        EM_mifs_PROTERR  协议错误
*        EM_mifs_SWDIFF   SW1SW2 != 0x9000
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
***************************************************************/
uchar EI_paypass_ucExchange(uint uiSendLen, uchar * pucInData, uint * puiRecLen, uchar * pucOutData)
{
	uint uiSendLeftLen = 0, uiTempLen = 0, uiSendCurrPos = 0, uiCurrRecvLen = 0;
	uchar result = EM_mifs_SUCCESS;
	uchar tempResult = 0x00;
	uchar i = 0;
	uchar ucResendNo = 0x00;
	uint blocklen;
	*puiRecLen = 0;
	// 保存要发送的数据长度
	uiSendLeftLen = uiSendLen;
	// 最大发送帧长度包括两个CRC字节和PCB字节
	// PAYPASS要求不包含CID字节和NAD字节 链接块发送
	while (uiSendLeftLen > (EG_mifs_tWorkInfo.FSC - 3))
	{
		//每次可以传输的最大数据长度
		uiTempLen = EG_mifs_tWorkInfo.FSC - 3;
		// 链接I块
		blocklen = make_IBlock(EG_mifs_tWorkInfo.ucCurPCB|0x12,uiTempLen,pucInData+uiSendCurrPos);
		result = EI_paypass_ucProcess(blocklen, &uiCurrRecvLen);

		// 如果超时没有接收到数据,发送NAK要求PICC回送数据
		// 如果接收过程中出现错误，也发送NAK要求PICC回送数据
		if ((result == EM_mifs_TIMEOUT) || (result == EM_mifs_TRANSERR))
		{
			tempResult = result;
        exchange_noise0:
			result = EI_paypass_ucMifRBlock(&uiCurrRecvLen);
			if (result != EM_mifs_SUCCESS)
			{
				// 仍然没有接收到有效响应数据，则通知设备
				return tempResult;
			}
		}

		if (result != EM_mifs_SUCCESS)
		{
			return result;
		}
		// 去掉2B　CRC校验码
		if (uiCurrRecvLen > (EG_mifs_tWorkInfo.FSD - 2))
		{
			// 接收长度超出了FSD
			return EM_mifs_PROTERR;
		}

		if (uiCurrRecvLen == 0)
		{
			// 数据交换失败
			return EM_mifs_TRANSERR;
		}
		if(IF_IBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
		{
			// 接收到I_Block, 则按协议错误处理
			return EM_mifs_PROTERR;
		}
		else if (IF_RBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
		{
			// 接收到R_Block
			if (IF_RBLOCK_ERR(EG_mifs_tWorkInfo.aucBuffer[0]))
			{
				// PICC不应返回NAK 不应返回CID
				return EM_mifs_PROTERR;
			}

			if(GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.aucBuffer[0]) == GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.ucCurPCB))
			{
				// 收到R_block(ACK)应答块块号正确, 准备发送下一个信息块
				uiSendCurrPos += uiTempLen;
				uiSendLeftLen -= uiTempLen;
				INC_PCB_BLOCKNO(EG_mifs_tWorkInfo.ucCurPCB);
				// 重发计数清零
				ucResendNo = 0;
			}
			else
			{
				// 如果块号不正确, 则重发
				if (ucResendNo++ > EM_paypass_protocol_TIMES)
				{
					// PCD重发一个I_Block超过三次
					return EM_mifs_PROTERR;
				}
			}
		}
		else if (IF_SBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
		{
			// 接收到S_Block
			gtRfidProInfo.SBlock = 0;
			while (1)
			{
				result = EI_paypass_ucMifSBlock(&uiCurrRecvLen);
				if ((result == EM_mifs_TIMEOUT) || (result == EM_mifs_TRANSERR))
				{
					tempResult = result;
				exchange_noise1:
					// 出现无应答或者通信错误，则发送NAK
					result = EI_paypass_ucMifRBlock(&uiCurrRecvLen);
					if (result != EM_mifs_SUCCESS)
					{
						// 仍然没有接收到有效响应数据，则通知设备
						return tempResult;
					}
				}

				if (result != EM_mifs_SUCCESS)
				{
					return result;
				}

				if (uiCurrRecvLen > (EG_mifs_tWorkInfo.FSD - 2))
				{
					// 接收长度超出了FSD
					return EM_mifs_PROTERR;
				}

				if (uiCurrRecvLen == 0)
				{
					// 数据交换失败
					return EM_mifs_TRANSERR;
				}

				if(IF_IBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
				{
					// 接收到I_Block, 则按协议错误处理
					return EM_mifs_PROTERR;
				}
				else if(IF_RBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
				{
					// 接收到R_Block
					if (IF_RBLOCK_ERR(EG_mifs_tWorkInfo.aucBuffer[0]))
					{
						// PICC不应返回CID 不应返回NAK
						return EM_mifs_PROTERR;
					}

					if(GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.aucBuffer[0]) == GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.ucCurPCB))
					{
						// 收到R_block(ACK)应答块块号正确, 准备发送下一个信息块
						uiSendCurrPos += uiTempLen;
						uiSendLeftLen -= uiTempLen;
						INC_PCB_BLOCKNO(EG_mifs_tWorkInfo.ucCurPCB);
						// 重发计数清零
						ucResendNo = 0;
					}
					else
					{
						// 如果块号不正确, 重发上个I_BLOCk
						if (ucResendNo++ > EM_paypass_protocol_TIMES)
						{
							// PCD重发一个I_Block超过三次
							return EM_mifs_PROTERR;
						}
					}
					break;
				}
				else if (!IF_SBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
				{
					// 接收到其他不明Block
					if(uiCurrRecvLen < 4)
					{
						//接收长度小于4字节，认为是噪声，则发NAK
						tempResult = EM_mifs_TIMEOUT;
						goto exchange_noise1;
					}
					else
					{
						return EM_mifs_PROTERR;
					}
				}
			}
		}
		else
		{
			// PCB值不正确
			if(uiCurrRecvLen < 4)
			{
				//接收长度小于4字节，认为是噪声，则发NAK
				tempResult = EM_mifs_TIMEOUT;
				goto exchange_noise0;
			}
			else
			{
				return EM_mifs_PROTERR;
			}
		}
	}

	ucResendNo = 0;						// 重发次数清零
	// 发送最后一帧数据
	while (1)
	{
		blocklen = make_IBlock(EG_mifs_tWorkInfo.ucCurPCB|0x02,uiSendLeftLen,pucInData+uiSendCurrPos);
		result = EI_paypass_ucProcess(blocklen, &uiCurrRecvLen);
		// 如果超时没有接收到数据,发送NAK要求PICC回送数据
		// 如果接收过程中出现错误，也发送NAK要求PICC回送数据
		if ((result == EM_mifs_TIMEOUT) || (result == EM_mifs_TRANSERR))
		{
			tempResult = result;
//            if(EG_mifs_tWorkInfo.ucCIDPollFlg)
//            {
//                return tempResult;
//            }
		exchange_noise2:
			result = EI_paypass_ucMifRBlock(&uiCurrRecvLen);
			if (result != EM_mifs_SUCCESS)
			{
				// 仍然没有接收到有效响应数据，则通知设备
				return tempResult;
			}
		}

		if (result != EM_mifs_SUCCESS)
		{
			return result;
		}
		if (uiCurrRecvLen > (EG_mifs_tWorkInfo.FSD - 2))
		{
			// 接收长度超出了FSD
			return EM_mifs_PROTERR;
		}

		if (uiCurrRecvLen == 0)
		{
			// 数据交换失败
			return EM_mifs_TRANSERR;
		}
		if (IF_IBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
		{
			// 接收到I_Block 准备接受卡应答数据
			break;
		}
		else if (IF_RBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
		{
			// 接收到R_Block
			if (IF_RBLOCK_ERR(EG_mifs_tWorkInfo.aucBuffer[0]))
			{
				// PICC不应返回CID PICC不应返回NAK
				return EM_mifs_PROTERR;
			}

			if(GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.aucBuffer[0]) == GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.ucCurPCB))
			{
				// PICC回送错误块号
				return EM_mifs_PROTERR;
			}
			else
			{
				//重发机制,也可以选择直接退出
				if (++ucResendNo >= EM_paypass_protocol_TIMES)
				{
					// PCD重发一个I_Block超过三次
					return EM_mifs_PROTERR;
				}
			}
		}
		else if (IF_SBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
		{
			// 接收到S_Block
			gtRfidProInfo.SBlock = 0;
			while (1)
			{
				result = EI_paypass_ucMifSBlock(&uiCurrRecvLen);
				if ((result == EM_mifs_TIMEOUT) || (result == EM_mifs_TRANSERR))
				{
					tempResult = result;
				exchange_noise3:
					// 出现无应答或者通信错误，则发送NAK
					result = EI_paypass_ucMifRBlock(&uiCurrRecvLen);
					if (result != EM_mifs_SUCCESS)
					{
						// 仍然没有接收到有效响应数据，则通知设备
						return tempResult;
					}
				}

				if (result != EM_mifs_SUCCESS)
				{
					return result;
				}

				if (uiCurrRecvLen == 0)
				{
					// 数据交换失败
					return EM_mifs_TRANSERR;
				}

				if (uiCurrRecvLen > (EG_mifs_tWorkInfo.FSD - 2))
				{
					// 接收长度超出了FSD
					return EM_mifs_PROTERR;
				}

				if (IF_IBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
				{
					// 接收到I_Block
					break;
				}
				else if (IF_RBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
				{
					// 接收到R_Block
					if (IF_RBLOCK_ERR(EG_mifs_tWorkInfo.aucBuffer[0]))
					{
						// PICC不应返回CID  PICC不应返回NAK
						return EM_mifs_PROTERR;
					}
					if(GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.aucBuffer[0]) == GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.ucCurPCB))
					{
						// PICC回送错误块号
						return EM_mifs_PROTERR;
					}
					else
					{
						// 请求重发
						if (ucResendNo++ > EM_paypass_protocol_TIMES)
						{
							// PCD重发一个I_Block超过三次
							return EM_mifs_PROTERR;
						}
					}
					break;
				}
				else if (!IF_SBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
				{
					// PCB值不正确
					if(uiCurrRecvLen < 4)
					{
						//接收长度小于4字节，认为是噪声，则发NAK
						tempResult = EM_mifs_TIMEOUT;
						goto exchange_noise3;
					}
					else
					{
						return EM_mifs_PROTERR;
					}
				}
			}

			if (IF_IBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
			{
				// 接收到I_Block
				break;
			}
		}
		else
		{
			// 接收到其他不支持的PCB
			// PCB值不正确
			if(uiCurrRecvLen < 4)
			{
				//接收长度小于4字节，认为是噪声，则发NAK
				tempResult = EM_mifs_TIMEOUT;
				goto exchange_noise2;
			}
			else
			{
				return EM_mifs_PROTERR;
			}
		}
	}
	// 接收PICC回送的I_Block
	while (1)
	{
		if(!(EG_mifs_tWorkInfo.OptMode&RFID_MODE_NOFRAME))
		{
			if (GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.aucBuffer[0]) != GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.ucCurPCB))
			{
				// PICC回送错误块号
				return EM_mifs_PROTERR;
			}
		}
		if (IF_IBLOCK_ERR(EG_mifs_tWorkInfo.aucBuffer[0]))
		{
			return EM_mifs_PROTERR;
		}
		// 增加长度越界判断， 2006-10-09
		if (uiCurrRecvLen < 1)
		{
			return EM_mifs_TRANSERR;
		}
		//   保存接收到的I_Block数据
		// 判断有效数据偏移 PCB+CID+NAD+DATA
		blocklen = 1;
		if(EG_mifs_tWorkInfo.ucCIDFlag)
		{
			++blocklen;
		}
		if(EG_mifs_tWorkInfo.ucNADFlag)
		{
			++blocklen;
		}
		memcpy(&pucOutData[*puiRecLen], &EG_mifs_tWorkInfo.aucBuffer[blocklen], uiCurrRecvLen - blocklen);
		*puiRecLen += (uiCurrRecvLen - blocklen);
		INC_PCB_BLOCKNO(EG_mifs_tWorkInfo.ucCurPCB);
		if ((EG_mifs_tWorkInfo.aucBuffer[0] & 0x10) == 0x0)
		{
			// 接收到最后一个I_Block，则返回
			break;
		}

		// PICC发送链接I_Block
		i = 0;
		while (1)
		{
			// 发送ACK获取其他I_Block
			blocklen = make_RBlock(EG_mifs_tWorkInfo.ucCurPCB|0xA2);
			result = EI_paypass_ucProcess(blocklen, &uiCurrRecvLen);

			if ((result == EM_mifs_TIMEOUT) || (result == EM_mifs_TRANSERR))
			{
				i++;
				if (i >= 3)
				{
					// ACK块最多连续发送三次 第1次正常应答 后2次重发
					return result;
				}
				// 没有响应或者通信错误，则重发ACK
				continue;
			}

			if (result != EM_mifs_SUCCESS)
			{
				return result;
			}

			if (uiCurrRecvLen == 0)
			{
				// 数据交换失败
				return EM_mifs_TRANSERR;
			}

			if (uiCurrRecvLen > (EG_mifs_tWorkInfo.FSD - 2))
			{
				// 接收长度超出了FSD
				return EM_mifs_PROTERR;
			}

			if (IF_IBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
			{
				// I_Block
				break;
			}
			else if (IF_RBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
			{
				// 接收到R_Block
				if (IF_RBLOCK_ERR(EG_mifs_tWorkInfo.aucBuffer[0]))
				{
					// PICC不应返回CID PICC不应返回NAK
					return EM_mifs_PROTERR;
				}
				if (GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.aucBuffer[0]) == GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.ucCurPCB))
				{
					// PICC回送错误块号
					return EM_mifs_PROTERR;
				}
				if (++i >= EM_paypass_protocol_TIMES)
				{
					// ACK块最多连续发送三次
					return EM_mifs_PROTERR;
				}
				continue;
			}
			else if (IF_SBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
			{
				// 接收到S_Block
				gtRfidProInfo.SBlock = 0;
				while (1)
				{
					result = EI_paypass_ucMifSBlock(&uiCurrRecvLen);
					if ((result == EM_mifs_TIMEOUT) || (result == EM_mifs_TRANSERR))
					{
						tempResult = result;
						// 出现无应答或者通信错误，则发送NAK
						result = EI_paypass_ucMifRBlock(&uiCurrRecvLen);
						if (result != EM_mifs_SUCCESS)
						{
							// 仍然没有接收到有效响应数据，则通知设备
							return tempResult;
						}
					}

					if (result != EM_mifs_SUCCESS)
					{
						return result;
					}

					if (uiCurrRecvLen == 0)
					{
						// 数据交换失败
						return EM_mifs_TRANSERR;
					}

					if (uiCurrRecvLen > (EG_mifs_tWorkInfo.FSD - 2))
					{
						// 接收长度超出了FSD
						return EM_mifs_PROTERR;
					}

					if (IF_IBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
					{
						// 接收到I_Block
						break;
					}
					else if (IF_RBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
					{
						// 接收到R_Block
						if (IF_RBLOCK_ERR(EG_mifs_tWorkInfo.aucBuffer[0]))
						{
							// PICC不应返回CID PICC不应返回NAK
							return EM_mifs_PROTERR;
						}
						if (GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.aucBuffer[0]) != GET_PCB_BLOCKNO(EG_mifs_tWorkInfo.ucCurPCB))
						{
							// PICC回送错误块号
							return EM_mifs_PROTERR;
						}
						if (++i >= EM_paypass_protocol_TIMES)
						{
							// PCD重发一个I_Block超过三次
							return EM_mifs_PROTERR;
						}
						break;
					}
					else if (!IF_SBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
					{
						// 接收到其他不明Block
						return EM_mifs_PROTERR;
					}
				}

				if (IF_IBLOCK(EG_mifs_tWorkInfo.aucBuffer[0]))
				{
					// 接收到I_Block
					break;
				}
			}
			else
			{
				// 接收到其他不明Block
				return EM_mifs_PROTERR;

			}
		}
		// 接收到I_Block, 则继续进行接收
		i = 0;
	}

	// 最终处理
	if ((*puiRecLen > 1) && (result == EM_mifs_SUCCESS))
	{
		if ((pucOutData[*puiRecLen - 2] != 0x90) || (pucOutData[*puiRecLen - 1] != 0x00))
		{
			result = EM_mifs_SWDIFF;
		}
		else
		{
			result = EM_mifs_SUCCESS;
		}
	}
	else
	{
		*puiRecLen = 0;
	}

	return result;
}

/**************************************************************************
* 函数名称：
*        EI_paypass_ucDeSelect(void)
* 功能描述：
*        发送DESELECT块使Mif卡进入HALT状态
* 被以下函数调用：
*        无
* 调用以下函数：
*        无
* 输入参数：
*        无
* 输出参数：
*        无
* 返 回 值：
*        EM_mifs_SUCCESS   成功
*        EM_mifs_TRANSERR  通信错误
*        EM_mifs_PROTERR   不符合PayPass协议
*        EM_mifs_TIMEOUT   超时无响应
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-23           创建
***************************************************************/
uchar EI_paypass_ucDeSelect(void)
{
	uchar result = EM_mifs_SUCCESS;
	uchar ucTempData = 0x00;
	uchar i = 0;
	uchar ucCMD = 0;

	if (EG_mifs_tWorkInfo.ucCurType == EM_mifs_TYPEA)
	{
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
            ucTempData = ucTempData;
            ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
            EG_mifs_tWorkInfo.expectMaxRec = 1+2; //DESELECT + CRC
#endif

        }
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
		{

#ifdef EM_RC663_Module
            ucTempData = 0x01;
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);
            ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif
		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
		{
#ifdef EM_PN512_Module

			ucTempData = 0x80;
			EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
			EI_mifs_vSetBitMask(TxModeReg, ucTempData);
			EI_mifs_vSetBitMask(RxModeReg, ucTempData);
			ucCMD = PN512CMD_TRANSCEIVE;

#endif
		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
		{
#ifdef EM_RC531_Module

			ucTempData = 0x0F;
			EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
			ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif
		}
	}
	else
	{
        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
            ucTempData = ucTempData;
            ucCMD = EMV_HAL_TRANSCEIVE_WITH_CRC;
            EG_mifs_tWorkInfo.expectMaxRec = 1+2; //DESELECT + CRC
#endif

        }
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
		{

#ifdef EM_RC663_Module

            ucTempData = 0x01;
            EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXCRCCON, ucTempData);
            EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXCRCCON, ucTempData);
            ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;

#endif
		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
		{
#ifdef EM_PN512_Module

			EI_mifs_vSetBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
			EI_mifs_vSetBitMask(TxModeReg, 0x80);
			EI_mifs_vSetBitMask(RxModeReg, 0x80);
			ucCMD = PN512CMD_TRANSCEIVE;

#endif
		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
		{
#ifdef EM_RC531_Module

			ucTempData = 0x2C;
			EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
			ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif
		}
	}

	while (1)
	{
		// 设置定时器
		EI_paypass_vSetTimer(EM_mifs_FWTDESELECT);

		EG_mifs_tWorkInfo.aucBuffer[0] = 0xC2;	// 1100 0010

		// 准备发送数据: 命令码
		EG_mifs_tWorkInfo.ulSendBytes = 1;	// how many bytes to send
		EG_mifs_tWorkInfo.ucDisableDF = 0;
		result = EI_mifs_ucHandleCmd(ucCMD);

		if ((result != EM_mifs_SUCCESS) && (result != EM_mifs_NOTAGERR))
		{
			result = EM_mifs_TRANSERR;
		}

		if (result == EM_mifs_NOTAGERR)
		{
			result = EM_mifs_TIMEOUT;
		}

		if ((result == EM_mifs_TRANSERR) || (result == EM_mifs_TIMEOUT))
		{

			i++;
			if (i > 2)
			{
				break;
			}
			// 重发DESELECT命令
			continue;
		}

		if (result == EM_mifs_SUCCESS)
		{
			if (EG_mifs_tWorkInfo.ulBytesReceived != 1)
			{
				result = EM_mifs_TRANSERR;
			}
			else if (EG_mifs_tWorkInfo.aucBuffer[0] != 0xC2)
			{
				result = EM_mifs_PROTERR;
			}
			break;
		}
	}

	EG_mifs_tWorkInfo.ucMifActivateFlag = RFID_ACTIVE_NONE;

	return result;
}

/**********************************************************************
* 函数名称：
*       EI_ucAnticoll(uchar ucSEL, uchar* pucUID)
* 功能描述：
*       应用防冲突机制获取UID信息
* 被以下函数调用：
*       无
* 调用以下函数：
*       无
* 输入参数：
*       ucSEL   冲突等级 取值如下：
*              0x93   第一级冲突
*              0x95   第二级冲突
*              0x97   第三级冲突
* 输出参数：
*       pucUID  获取的UID信息，包括BCC字节
* 返 回 值：
*       EM_mifs_SUCCESS    获取成功
*       EM_mifs_TRANSERR   通信错误
*       EM_mifs_TIMEOUT    卡无响应
*
*       说明
*       //冲突位置并不是mod 8  而是4个字节具体的位置 如第二字节bit4 就为0x0D(PN512)或者0x0C(CLRC663)
*       //RC531冲突后 冲突位以后清零(不含冲突位)
*       //PN512和CLR663冲突后 冲突位以后有1置1 (如果设置为0 则不会发送5个字节 只发送 到冲突所在的字节数)
*       //PN512 bit0冲突则位置为0(CollPos为0)  CLRC663为1
*       //for example
*       //card 1 uid:  59 4D E6 0C FE
*       //card 2 uid:  59 FD 08 21 8D
*       //RC531 冲突后置0     收到 59 1D 00 00 00 冲突位第二个字节bit4 collpos为0x0D(bit0即1)
*       //PN512 冲突后相或为1 收到 59 FD EE 2D FF 冲突位第二个字节bit4 collpos为0x0D(bit0即1)
*       //RC663 冲突后相或为1 收到 59 FD EE 2D FF 冲突位第二个字节bit4 collpos为0x0C(bit0即0)
* 历史纪录：
*        修改人           修改日期          修改内容
*                        2006-02-22           创建
****************************************************************/
uchar EI_ucAnticoll(uchar ucSEL, uchar * pucUID)
{
#if 11
	uchar ucTempData;
	ulong ulBCnt;						// 当前要发送数据的合法位数
	ulong ulBytes;						// 当前要发送数据的合法字节数
	uchar ucCompleted;					// 表明操作是否完成
	uchar ucTempVal,ucCMD = 0;
	uint i;
	uchar ucResult = 0;
    uchar select = 0,getcol = 0;
	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
//        ucCMD = EMV_HAL_TRANSCEIVE_WITHOUT_CRC;
//        EG_mifs_tWorkInfo.expectMaxRec = 5;
//		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
        ucTempData = 0;
        ucTempData = ucTempData;
        //多卡选卡3911此api处理
        ucResult = as3911AntiCollision(1, ucSEL, pucUID, EM_mifs_DEFAULT*128);
        EG_mifs_tWorkInfo.ucAnticol = 0;
        return ucResult;
#endif

    }
	else if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663 ) {

#ifdef EM_RC663_Module
		//cf_RC663   冲突标志
		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
		ucTempData = 0x80;
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, ucTempData);
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_STATUS, 0x20);

		// 禁止 RxCRC 和 TxCRC , 允许奇偶校验
		EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_FRAMECON, 0xC0);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXCRCCON, 0x01);
		EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXCRCCON, 0x01);
		ucCMD = PHHAL_HW_RC663_CMD_TRANSCEIVE;
#endif

    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module

		EI_paypass_vSetTimer(EM_mifs_DEFAULT);
		EI_mifs_vClrBitMask(TxModeReg, 0x80);
		EI_mifs_vClrBitMask(RxModeReg, 0x80);
		// 禁止 RxCRC 和 TxCRC , 允许奇偶校验
		EI_mifs_vClrBitMask(ManualRCVReg, 0x10);  //clr 奇偶校验
		EI_mifs_vSetBitMask(CollReg, 0x80);   //cf_pn512 是否要清0 还是置1
		// 禁止crypto1单元
		EI_mifs_vClrBitMask(Status2Reg, 0x08);
		ucCMD = PN512CMD_TRANSCEIVE;

#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		select = select;
		getcol = getcol;
		// 设置定时器
		EI_mifs_vSetTimer(2);
		// 冲突后数据都置0, Rx帧格式为ISO1443A
		ucTempData = 0x28;
		EI_mifs_vWriteReg(1, EM_mifs_REG_DECODERCONTROL, &ucTempData);	// ZeroAfterColl aktivieren

		// 禁止crypto1单元
		EI_mifs_vClrBitMask(EM_mifs_REG_CONTROL, 0x08);
		ucCMD = EM_mifs_PCD_TRANSCEIVE;

#endif

	}


	memset(EG_mifs_tWorkInfo.aucCmdBuf, 0, 5);

	ucCompleted = 0;
	ulBCnt = 0;

	// 持续发送AntiColl命令，直到防冲突成功
	while ((ucCompleted == 0) && (ucResult == EM_SUCCESS))
	{
		EG_mifs_tWorkInfo.ucDisableDF = 1;

        if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
            ;
#endif

        }
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
		{

#ifdef EM_RC663_Module
			;
#endif

		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
		{

#ifdef EM_PN512_Module
			;
#endif

		}
		else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
		{
#ifdef EM_RC531_Module
			// 每个字节带奇校验
			ucTempData = 0x03;
			EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
#endif
		}


		EG_mifs_tWorkInfo.ucnBits = (uchar) (ulBCnt % 8);
//        TRACE("\r\n1111------%x---%d",EG_mifs_tWorkInfo.ucnBits, ulBCnt);
		if (EG_mifs_tWorkInfo.ucnBits != 0)
		{
			// 收到有不满一个字节的数据
			// 根据多出的不满一个字节的位数设置接收数据起始位（使得最后的数据正好收满一个字节）
			// 和最后一个数据位数,
			ucTempData = (EG_mifs_tWorkInfo.ucnBits % 0x10 * 0x10) | EG_mifs_tWorkInfo.ucnBits;

            if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {

#ifdef EM_AS3911_Module
                ;
#endif

            }
			else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
			{

#ifdef EM_RC663_Module
                EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, 0x70);
                EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXDATANUM, 0x07);
                EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_TXDATANUM, (ucTempData&0x07));
//                ucTempData &= 0x70;
                EI_mifs_vSetBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, (ucTempData&0x70));
#endif

			}
			else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
			{

#ifdef EM_PN512_Module
				ucTempData &= 0x7F;
				EI_mifs_vWriteReg(1, BitFramingReg, &ucTempData);
#endif

			}
			else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
			{
#ifdef EM_RC531_Module
				EI_mifs_vWriteReg(1, EM_mifs_REG_BITFRAMING, &ucTempData);
#endif
			}

			ulBytes = ulBCnt / 8 + 1;	//设置需要发送的字节数
            /*
			if (EG_mifs_tWorkInfo.ucnBits == 7) //yehf 这部分可以不要
			{
				// 在中断中进行处理
				EG_mifs_tWorkInfo.lBitsReceived = 7;
				// 第一个字节接收起始位置改为0
				EI_mifs_vWriteReg(1, EM_mifs_REG_BITFRAMING, (uchar *) & EG_mifs_tWorkInfo.ucnBits);
			}
			*/
		}
		else
		{
			ulBytes = ulBCnt / 8;
		}

//        TRACE("\r\n2222---------%d",ulBytes);
		// 准备发送数据: SEL+NVB+上次成功接收到的所有数据
		EG_mifs_tWorkInfo.ucAnticol = ucSEL;
		EG_mifs_tWorkInfo.aucBuffer[0] = ucSEL;	// SEL
		// NVB: 高4位 = 传输的所有合法数据位数/8(包括SEL和NVB,不包括多余位数)
		// 低4位=传输的所有合法数据位数 mod 8
		EG_mifs_tWorkInfo.aucBuffer[1] =
			(uchar) (0x20 + ((ulBCnt / 8) * 0x10) + EG_mifs_tWorkInfo.ucnBits);

		for (i = 0; i < ulBytes; i++)
		{
			EG_mifs_tWorkInfo.aucBuffer[i + 2] = EG_mifs_tWorkInfo.aucCmdBuf[i];
		}
		EG_mifs_tWorkInfo.ulSendBytes = 2 + ulBytes;	// how many bytes to send

		// 执行命令
		ucResult = EI_mifs_ucHandleCmd(ucCMD);
		/*
		if (EG_mifs_tWorkInfo.ucnBits == 7)  //yehf 这部分可以不要
		{
			// 这种情况算作冲突错误, 但仍然存储
			EG_mifs_tWorkInfo.aucBuffer[0] = EG_mifs_tWorkInfo.aucBuffer[0] << 7;
			EG_mifs_tWorkInfo.ucCollPos = 8;
			ucResult = EM_mifs_COLLERR;
			EG_mifs_tWorkInfo.lBitsReceived = 40 - ulBCnt;
		}
		*/
		if ((ucResult == EM_SUCCESS) || (ucResult == EM_mifs_COLLERR))
		{
			// 接收数据长度错误
			if (EG_mifs_tWorkInfo.lBitsReceived != (long)(40 - ulBCnt))	// not 5 bytes answered
			{
				ucResult = EM_mifs_BITCOUNTERR;
			}
			else
			{
                if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module
                    if ( ucResult == EM_mifs_COLLERR ) {
                        //防冲撞暂不实现
                        ucCompleted = 1;
                    }

					// 新接收的数据从本次发送数据产生冲突的位开始存放
					ucTempVal = 0;
					if (EG_mifs_tWorkInfo.ucnBits != 0)	// 最后一个字节不完整
					{
						// 由于设置, 从FIFO中接收的只有第一个字节不完整
						// 所以加入上次接收的最后一个字节中
						EG_mifs_tWorkInfo.aucCmdBuf[ulBytes - 1] |= EG_mifs_tWorkInfo.aucBuffer[0];
						ucTempVal = 1;
					}
					for (i = 0; i < (4 - ulBytes); i++)
					{
						EG_mifs_tWorkInfo.aucCmdBuf[i + ulBytes] =
							EG_mifs_tWorkInfo.aucBuffer[ucTempVal + i];
					}

                    if (ucResult == EM_SUCCESS)
                    {
                        // 成功检查校验和
                        ucTempVal = 0;
                        for (i = 0; i < 4; i++)
                        {
                            ucTempVal ^= EG_mifs_tWorkInfo.aucCmdBuf[i];
                        }
//                        DISPBUF(EG_mifs_tWorkInfo.aucCmdBuf,5,0);
                        if (ucTempVal !=
                            EG_mifs_tWorkInfo.aucBuffer[EG_mifs_tWorkInfo.ulBytesReceived - 1])
                        {
                            // 校验错误
                            ucResult = EM_mifs_SERNRERR;
                        }
                        else
                        {
                            //防冲撞完成
                            ucCompleted = 1;
                            memcpy(pucUID, EG_mifs_tWorkInfo.aucCmdBuf, 4);
                            pucUID[4] = ucTempVal;
                        }
                    }
					if (ucResult == EM_mifs_COLLERR) //如果是冲突 置为成功
						ucResult = EM_mifs_SUCCESS;
#endif
                }
				else if((EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
                   || (EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663))
				{

//#ifdef EM_PN512_Module
#if (defined(EM_PN512_Module) || defined(EM_RC663_Module))

					if (ucResult == EM_mifs_COLLERR) // 冲突
					{
                        if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
                        {

#ifdef EM_RC663_Module
                            ulBCnt = ulBCnt + EG_mifs_tWorkInfo.ucCollPos+1 - EG_mifs_tWorkInfo.ucnBits;
                            select = (EG_mifs_tWorkInfo.ucCollPos)/8;  //collpos为0则保留bit0
                            getcol = (0x01<<((EG_mifs_tWorkInfo.ucCollPos%8)+1)) - 1;
                            EG_mifs_tWorkInfo.aucBuffer[select] = EG_mifs_tWorkInfo.aucBuffer[select]&getcol;

#endif

                        }
                        else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
                        {

#ifdef EM_PN512_Module
                            ulBCnt = ulBCnt + EG_mifs_tWorkInfo.ucCollPos - EG_mifs_tWorkInfo.ucnBits;
                            select = (EG_mifs_tWorkInfo.ucCollPos-1)/8;
                            if(!(EG_mifs_tWorkInfo.ucCollPos%8))   //collpos为1则保留bit0  为8保留bit0~bit7  为9保留bit0
                                getcol = (0x01<<8)-1;
                            else
                                getcol = (0x01<<(EG_mifs_tWorkInfo.ucCollPos%8)) - 1;

                            EG_mifs_tWorkInfo.aucBuffer[select] = EG_mifs_tWorkInfo.aucBuffer[select]&getcol;
#endif

                        }
//                        TRACE("\r\n--%x---%x----%x---%x",(0x01<<8)-1, select, getcol,EG_mifs_tWorkInfo.aucBuffer[select]);
						//ucResult = EM_mifs_SUCCESS;
					}

					// 新接收的数据从本次发送数据产生冲突的位开始存放
					ucTempVal = 0;
					if (EG_mifs_tWorkInfo.ucnBits != 0)	// 最后一个字节不完整
					{
						// 由于设置, 从FIFO中接收的只有第一个字节不完整
						// 所以加入上次接收的最后一个字节中
						EG_mifs_tWorkInfo.aucCmdBuf[ulBytes - 1] |= EG_mifs_tWorkInfo.aucBuffer[0];
						ucTempVal = 1;
					}
					for (i = 0; i < (4 - ulBytes); i++)
					{
						EG_mifs_tWorkInfo.aucCmdBuf[i + ulBytes] =
							EG_mifs_tWorkInfo.aucBuffer[ucTempVal + i];
					}

					if (ucResult == EM_SUCCESS)
					{
						// 成功检查校验和
						ucTempVal = 0;
						for (i = 0; i < 4; i++)
						{
							ucTempVal ^= EG_mifs_tWorkInfo.aucCmdBuf[i];
						}
						if (ucTempVal !=
							EG_mifs_tWorkInfo.aucBuffer[EG_mifs_tWorkInfo.ulBytesReceived - 1])
						{
							// 校验错误
							ucResult = EM_mifs_SERNRERR;
						}
						else
						{
							//防冲撞完成
							ucCompleted = 1;
							memcpy(pucUID, EG_mifs_tWorkInfo.aucCmdBuf, 4);
							pucUID[4] = ucTempVal;
						}
					}

//                    TRACE("\r\n3333--%x---%x----%d",ucResult, EG_mifs_tWorkInfo.ucCollPos, ulBCnt);
//                    DISPBUF(EG_mifs_tWorkInfo.aucCmdBuf, 5, 0);
					if (ucResult == EM_mifs_COLLERR) //如果是冲突 置为成功
						ucResult = EM_mifs_SUCCESS;
#endif

				}
				else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
				{

#ifdef EM_RC531_Module

                    // 新接收的数据从本次发送数据产生冲突的位开始存放
                    ucTempVal = 0;
                    if (EG_mifs_tWorkInfo.ucnBits != 0)	// 最后一个字节不完整
                    {
                        // 由于设置, 从FIFO中接收的只有第一个字节不完整
                        // 所以加入上次接收的最后一个字节中
                        EG_mifs_tWorkInfo.aucCmdBuf[ulBytes - 1] |= EG_mifs_tWorkInfo.aucBuffer[0];
                        ucTempVal = 1;
                    }
                    for (i = 0; i < (4 - ulBytes); i++)
                    {
                        EG_mifs_tWorkInfo.aucCmdBuf[i + ulBytes] =
                            EG_mifs_tWorkInfo.aucBuffer[ucTempVal + i];
                    }
                    if (ucResult == EM_SUCCESS)
                    {
                        // 成功检查校验和
                        ucTempVal = 0;
                        for (i = 0; i < 4; i++)
                        {
                            ucTempVal ^= EG_mifs_tWorkInfo.aucCmdBuf[i];
                        }
                        if (ucTempVal !=
                            EG_mifs_tWorkInfo.aucBuffer[EG_mifs_tWorkInfo.ulBytesReceived - 1])
                        {
                            // 校验错误
                            ucResult = EM_mifs_SERNRERR;
                        }
                        else
                        {
                            //防冲撞完成
                            ucCompleted = 1;
                            memcpy(pucUID, EG_mifs_tWorkInfo.aucCmdBuf, 4);
                            pucUID[4] = ucTempVal;
                        }
                    }
                    else					// 冲突
                    {
                        // 重新设置发送数量: 即第一个冲突位的位置之前的数据发送给卡
                        // 卡会回送从第一个冲突位以后的数据,获得接收到的正确的位数
                        ulBCnt = ulBCnt + EG_mifs_tWorkInfo.ucCollPos - EG_mifs_tWorkInfo.ucnBits;
                        ucResult = EM_mifs_SUCCESS;
                    }

#endif

				}
			}							// if(EG_mifs_tWorkInfo.lBitsReceived != (40 - ulBCnt))
		}								// if(ucResult == EM_SUCCESS || ucResult == EM_mifs_COLLERR)
	}									// while(ucCompleted == 0 && ucResult == EM_SUCCESS)

    if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module
        ;
#endif
    }
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC663)
	{

#ifdef EM_RC663_Module
        EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_RXBITCTRL, 0x70);
        EI_mifs_vClrBitMask(PHHAL_HW_RC663_REG_TXDATANUM, 0x07);
#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512)
	{

#ifdef EM_PN512_Module
		//EI_mifs_vSetBitMask(CollReg, 0x80);	//cf_pn512 是否要置1
        EI_mifs_vClrBitMask(BitFramingReg, 0x07); //发送完毕后发送完整字节 chenf 20121204
        EI_mifs_vClrBitMask(BitFramingReg, 0x70); //发送完毕后接收完整字节 chenf 20121204
#endif

	}
	else if(EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531)
	{

#ifdef EM_RC531_Module

		// 结束处理
		EI_mifs_vClrBitMask(EM_mifs_REG_DECODERCONTROL, 0x20);	// ZeroAfterColl disable
#endif

	}

//    TRACE("\r\n4455-------------%x---",ucResult);
	EG_mifs_tWorkInfo.ucAnticol = 0;

	if (ucResult == EM_mifs_NOTAGERR)
	{
		ucResult = EM_mifs_TIMEOUT;
	}
	else if (ucResult != EM_mifs_SUCCESS)
	{
		ucResult = EM_mifs_TRANSERR;
	}

//    TRACE("\r\n4444-------------%x---",ucResult);
//    DISPBUF(pucUID, 5, 0);
	return ucResult;
#else
	uchar ucTempData;
	uchar ucResult = 0;
	ulong ulBCnt;			// 当前要发送数据的合法位数
	ulong ulBytes,ulByteplus;			// 当前要发送数据的合法字节数
	uint i;

	EG_mifs_tWorkInfo.ucEnableTransfer = 0;

	// 设置定时器
	EI_mifs_vSetTimer(2);

	// 冲突后数据都置0, Rx帧格式为ISO1443A
	ucTempData = 0x28;
	EI_mifs_vWriteReg(1, EM_mifs_REG_DECODERCONTROL, &ucTempData);	// ZeroAfterColl aktivieren

	// 禁止crypto1单元
	EI_mifs_vClrBitMask(EM_mifs_REG_CONTROL, 0x08);

	CLRBUF(EG_mifs_tWorkInfo.aucCmdBuf);
	EG_mifs_tWorkInfo.ucAnticol = ucSEL;
	ulBCnt = 0;
	//防冲撞过程忽略错误
	EG_mifs_tWorkInfo.ucDisableDF = 1;
	while(1)
	{
		// 每个字节带奇校验
		ucTempData = 0x03;
		EI_mifs_vWriteReg(1, EM_mifs_REG_CHANNELREDUNDANCY, &ucTempData);
		EG_mifs_tWorkInfo.ucnBits = (uchar) (ulBCnt % 8);
		//设置需要发送的字节数
		ulBytes = ulBCnt/8;
		if (EG_mifs_tWorkInfo.ucnBits != 0)
		{
			// 收到有不满一个字节的数据
			// 根据多出的不满一个字节的位数设置接收数据起始位和发送起始位
			ucTempData = (EG_mifs_tWorkInfo.ucnBits << 4) | EG_mifs_tWorkInfo.ucnBits;
			EI_mifs_vWriteReg(1, EM_mifs_REG_BITFRAMING, &ucTempData);
			//不满一个字节，发送字节数加1
			ulByteplus = 1;
		}
		else
		{
			ulByteplus = 0;
		}
		EG_mifs_tWorkInfo.aucBuffer[0] = ucSEL;
		//确定要发送数据长度和位数
		EG_mifs_tWorkInfo.aucBuffer[1] = 0x20 + (uchar)(ulBytes<<4) + EG_mifs_tWorkInfo.ucnBits;
		memcpy(&EG_mifs_tWorkInfo.aucBuffer[2],EG_mifs_tWorkInfo.aucCmdBuf,ulBytes+ulByteplus);
		EG_mifs_tWorkInfo.ulSendBytes = 2+ulBytes+ulByteplus;
		// 执行命令
		ucResult = EI_mifs_ucHandleCmd(EM_mifs_PCD_TRANSCEIVE);

		if ((ucResult == EM_SUCCESS) || (ucResult == EM_mifs_COLLERR))
		{
			// 接收数据长度错误
			if (EG_mifs_tWorkInfo.lBitsReceived != (long)(40 - ulBCnt))
			{
				ucResult = EM_mifs_BITCOUNTERR;
				break;
			}
			else
			{
				// 新接收的数据从本次发送数据产生冲突的位开始存放
				// 有冲突，则上次字节接收不完整
				if(EG_mifs_tWorkInfo.ucnBits)
				{
					EG_mifs_tWorkInfo.aucCmdBuf[ulBytes] |= EG_mifs_tWorkInfo.aucBuffer[0];
					//剩余位数
					EG_mifs_tWorkInfo.lBitsReceived -= (8-EG_mifs_tWorkInfo.ucnBits);
					memcpy(&EG_mifs_tWorkInfo.aucCmdBuf[ulBytes+ulByteplus],&EG_mifs_tWorkInfo.aucBuffer[1],
						EG_mifs_tWorkInfo.lBitsReceived/8);
				}
				else
				{
					memcpy(&EG_mifs_tWorkInfo.aucCmdBuf[ulBytes+ulByteplus],&EG_mifs_tWorkInfo.aucBuffer[0],
						EG_mifs_tWorkInfo.lBitsReceived/8);
				}

				//DATAIN(0x11);
				//DATAIN(EG_mifs_tWorkInfo.ucnBits);
				//BUFFIN(7,EG_mifs_tWorkInfo.aucCmdBuf);
				if (ucResult == EM_SUCCESS)
				{
					// 没有冲突差生说明UID接收完毕
					//DATAIN(0x33);
					//BUFFIN(7,EG_mifs_tWorkInfo.aucCmdBuf);
					// 成功检查校验和
					ucTempData = 0;
					for (i=0;i<5;i++)
					{
						ucTempData ^= EG_mifs_tWorkInfo.aucCmdBuf[i];
					}
					if (ucTempData != 0)
					{
						// UID校验错误
						ucResult = EM_mifs_SERNRERR;
					}
					else
					{
						memcpy(pucUID,&EG_mifs_tWorkInfo.aucCmdBuf[0],5);
					}
					break;
				}
				else
				{
					// 冲突
					// 重新设置发送数量: 即第一个冲突位的位置之前的数据发送给卡
					// 卡会回送从第一个冲突位以后的数据,获得接收到的正确的位数
					ulBCnt = ulBCnt + EG_mifs_tWorkInfo.ucCollPos - EG_mifs_tWorkInfo.ucnBits;
					//DATAIN(0x22);
					//DATAIN(LBYTE(ulBCnt));
				}
			}
		}
		else
		{
			break;
		}
	}
	// 结束处理
	EI_mifs_vClrBitMask(EM_mifs_REG_DECODERCONTROL, 0x20);	// ZeroAfterColl disable
	EG_mifs_tWorkInfo.ucAnticol = 0;

	if (ucResult == EM_mifs_NOTAGERR)
	{
		ucResult = EM_mifs_TIMEOUT;
	}
	else if (ucResult != EM_mifs_SUCCESS)
	{
		ucResult = EM_mifs_TRANSERR;
	}
	return ucResult;
#endif
}
#endif


