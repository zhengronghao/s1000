/*********************************************************************
* 版权所有 (C)2007, 福建实达设备有限公司
* 文件名称：
*
* 当前版本：
*     01-01-01
* 内容摘要：
*     本文件为系统主入口
* 包含的函数：
*
* 历史纪录：
*     修改人		日期			    	版本号
******************************************************************/

#include "wp30_ctrl.h"
#ifdef CFG_DBG_ICCARD 

//*************************************************************************************
//
//    全局变量或函数申明
//
//*************************************************************************************
unsigned int guiDebugi=0,guiDebugj=0,guiDebugk=0;
unsigned int guiDebugFlg=1;
unsigned char gcDebugBuf[1024];
unsigned char gucDebugm=0,gucDebugn=0;

//*************************************************************************************
//
//    外部变量或函数申明
//
//*************************************************************************************

//*************************************************************************************
//
//    宏定义
//
//*************************************************************************************
#define debug_beep()      //sys_beep()
#define EM_ICCard_PROTOCOL_T0               0
#define EM_ICCard_PROTOCOL_T1               1

#define EM_ic_DirectConvention              0x3B
#define EM_ic_InverseConvention             0x3F

#define EM_ic_ATR_TA   0x010
#define EM_ic_ATR_TB   0x020
#define EM_ic_ATR_TC   0x040
#define EM_ic_ATR_TD   0x080



//*************************************************************************************
//
//    全局变量定义
//
//*************************************************************************************
// 卡支持的协议
uchar EG_ucIcCardProtocol[2];
uint EG_uiIcCardFi;
uint EG_uiIcCardDi;
const ushort EG_uiFiTable[] =
{
    372,    372, 558, 744,  1116, 1488, 1860,   0xFFFF,
	0xFFFF, 512, 768, 1024, 1536, 2048, 0xFFFF, 0xFFFF
};

const ushort EG_uiDiTable[] =
{
	0xFFFF, 1, 2, 4, 8, 16, 32, 0xFFFF, 12, 20, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};

//*************************************************************************************
//
//    函数定义
//
//*************************************************************************************

void DispICCardRet(int iRet)
{
	switch (iRet)
	{
	case ICC_SUCCESS:
		TRACE("\r\n iRet=%d 成功! ",iRet);
		break;
	case -ICC_TIMEOUT:
		TRACE("\r\n iRet=%d 超时错误", iRet);
		break;
	case -ICC_COMMERR:
		TRACE("\r\n iRet=%d 校验错误", iRet);
		break;
	case -ICC_RESPERR:
		TRACE("\r\n iRet=%d 卡片返回信息错误", iRet);
		break;
	case -ICC_CARDOUT:
		TRACE("\r\n iRet=%d 卡不在位", iRet);
		break;
	case -ICC_NOPOWER:
		TRACE("\r\n iRet=%d 卡未上电", iRet);
		break;
	case -ICC_ERRORSLOT:
		TRACE("\r\n iRet=%d 卡座错误", iRet);
		break;
	case -ICC_ERRPARA:
		TRACE("\r\n iRet=%d 参数错误", iRet);
		break;
    case -ICC_CARDNOPOWER:
		TRACE("\r\n iRet=%d 卡座未上电", iRet);
		break;
	default:
		TRACE("\r\n iRet=%d 未知错误! ", iRet);
		break;
	}
}
/*
// 将指定位old移动新的位newbit 如:0010,0000 MOVEBIT(0x20,5,3)=0000,1000
#ifndef MOVEBIT
#define MOVEBIT(Data,oldbit,newbit)     (((Data&(1<<oldbit))>>oldbit)<<newbit)
#endif
// 将指定位old右移动Offset位 如:0010,0000 MOVERBIT(0x20,5,2)=0000,1000
#ifndef MOVERBIT
#define MOVERBIT(Data,oldbit,OffsetBit)    ((Data&(1<<oldbit))>>OffsetBit)
#endif
// 将指定位old左移动Offset位 如:0010,0000 MOVELBIT(0x20,5,2)=1000,0000
#ifndef MOVELBIT
#define MOVELBIT(Data,oldbit,OffsetBit)    ((Data&(1<<oldbit))<<OffsetBit)
#endif

//正向反向数据互换
#define FRDATA(x)   CONVERTBYTE((~x))
*/
//半字节1的个数统计表
uchar gucOneTable[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

// 判断字符是否校验位是否正确
// ucData正向数据  ucMode=0正向  ucMode=1反向
uint CheckData(uchar ucData, uchar ucChkBit, uchar ucMode)
{
	uchar ucCheck;
	ucCheck = (gucOneTable[HHALFB(ucData)] + gucOneTable[LHALFB(ucData)]) % 2;
	if (ucMode == 0 && ucCheck == ucChkBit)
	{
		//正向,为偶数
		return 0;
	}
	else if (ucMode == 1 && ucCheck != ucChkBit)
	{
		//反向,为奇数
		return 0;
	}
	else
	{
		return 1;
	}
}

/**********************************************************************
* 函数名称：
*     CreatCApdu
* 功能描述：
*     建立CApdu
* 输入参数：
*     cHead:CApdu命令头  Lc:发送字节数(0~256)  cData:发送内容  Le:接收字节数(0~256)
* 输出参数：
*     gtCApduInfo.tCApdu.cBuf:CApdu内容
*     gtCApduInfo.iCApduLen:命令长度
* 返回值：
*     0-成功  1-失败
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
/*
// CApdu数据格式
typedef union _t_CApdu_
{
	uchar ucBuf[256 + 6];
	struct
	{
		uchar cla;
		uchar ins;
		uchar p1;
		uchar p2;
		uchar Lc;
		uchar ucData[256];
		uchar Le;
	}
} _t_CApdu;

typedef struct
{
	_t_CApdu tCApdu;
	uint uiCApduLen;
} _t_CApduInfo;

//CApdu发送缓冲区
_t_CApduInfo gtCApduInfo;

// RApdu数据格式
typedef union _t_RApdu_
{
	uchar SW1;
	uchar SW2;
} _t_RApdu;
_t_RApdu gtRApdu;
uint CreatCApdu(uchar * cHead, uint Lc, uchar * ucData, uint Le)
{
	uint i;
	if (Lc > 256 || Le > 256)
	{
		return 1;
	}
	else
	{
		memcpy(&gtCApduInfo.tCApdu.cBuf, cHead, 4);
		i = 4;
		if (Lc)
		{
			gtCApduInfo.ucBuf[i++] = (uchar) Lc;
			memcpy(&gtCApduInfo.cBuf[i], ucData, Lc);
		}
		if (Le)
		{
			gtCApduInfo.ucBuf[i++] = (uchar) Le;
		}
		gtCApduInfo.iCApduLen = i;
		return 0;
	}
}
*/
/**********************************************************************
* 函数名称：
*     icc_ExchangeAPDU
* 功能描述：
*     卡Apdu通讯
* 输入参数：
*    iSendLen:发送长度 cSendBuf
*
* 输出参数：
*
* 返回值：
*     0-成功  1-失败
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
uint icc_ExchangeAPDU(uint iSendLen, uchar * ucSendBuf, uint * iRecLen, uchar * ucRecBuf)
{
	return 0;
}

void DispSW(uchar sw1, uchar sw2)
{
	TRACE("sw1:0x%2X sw2:0x%2X - ", sw1, sw2);
	switch (sw1)
	{
		/*
		 * case 0x:
		 * switch(sw2)
		 * {

		 * case 0x:
		 * TRACE("");
		 * break;
		 * case 0x:
		 * TRACE("");
		 * break;
		 * default:
		 * TRACE("***未知状态SW2:%2X!***",sw2);
		 * break;
		 * }
		 * break;
		 */
	case 0x61:
		switch (sw2)
		{

		default:
			TRACE("正确,额外数据长度0x%2X", sw2);
			break;
		}
		break;
	case 62:
		switch (sw2)
		{
		case 0x00:
			TRACE("无信息提供");
			break;
		case 0x81:
			TRACE("回送数据可能出错");
			break;
		case 0x83:
			TRACE("选择文件无效");
			break;
		case 0x84:
			TRACE("FCI格式与P2指定的不符");
			break;
		default:
			TRACE("***未知状态SW2:%2X!***", sw2);
			break;
		}
		break;
	case 0x63:
		switch (sw2)
		{

		case 0x00:
			TRACE("认证失败");
			break;
		case 0xC0:
			TRACE("使用内部重试程序更新成功,不提供计数器");
			break;
		default:
			if ((sw2 & 0xC0))
			{
				TRACE("使用内部重试程序更新成功,重试次数%d", sw2 & 0xC0);
			}
			else
			{
				TRACE("***未知状态SW2:%2X!***", sw2);
			}
			break;
		}
		break;
	case 0x64:
		switch (sw2)
		{
		case 0x00:
			TRACE("状态标志位未变 ");
			break;

		default:
			TRACE("***未知状态SW2:%2X!***", sw2);
			break;
		}
		break;
	case 0x65:
		switch (sw2)
		{
		case 0x81:
			TRACE("内存失败 ");
			break;

		default:
			TRACE("***未知状态SW2:%2X!***", sw2);
			break;
		}
		break;
	case 0x67:
		switch (sw2)
		{
		case 0x00:
			TRACE("Lc或Lc不正确");
			break;

		default:
			TRACE("***未知状态SW2:%2X!***", sw2);
			break;
		}
		break;
	case 0x68:
		switch (sw2)
		{
		case 0x82:
			TRACE("不支持安全报文");
			break;

		default:
			TRACE("***未知状态SW2:%2X!***", sw2);
			break;
		}
		break;
	case 0x69:
		switch (sw2)
		{
		case 0x81:
			TRACE("命令与文件结构不相容");
			break;
		case 0x82:
			TRACE("不满足安全状态");
			break;
		case 0x83:
			TRACE("认证方法锁定");
			break;
		case 0x84:
			TRACE("引用数据无效 ");
		case 0x85:
			TRACE("不满足使用条件");
			break;
		case 0x86:
			TRACE("不满足命令执行条件(非当前EF)");
			break;
		case 0x87:
			TRACE("安全报文数据项丢失 ");
			break;
		case 0x88:
			TRACE("安全报文数据项不正确");

			break;
		default:
			TRACE("***未知状态SW2:%2X!***", sw2);
			break;
		}
		break;
	case 0x6A:
		switch (sw2)
		{
		case 0x80:
			TRACE("数据域参数不正确 ");
			break;
		case 0x81:
			TRACE("不支持此功能");
			break;
		case 0x82:
			TRACE("未找到文件");
			break;
		case 0x83:
			TRACE("未找到记录");
			break;
		case 0x84:
			TRACE("文件中存储空间不够");
			break;
		case 0x86:
			TRACE("P1 P2参数错误");
			break;
		case 0x88:
			TRACE("未找到引用数据 ");
			break;

		default:
			TRACE("***未知状态SW2:%2X!***", sw2);
			break;
		}
		break;
	case 0x6B:
		switch (sw2)
		{
		case 0x00:
			TRACE("参数错误(偏移地址超出了EF)");
			break;

		default:
			TRACE("***未知状态SW2:%2X!***", sw2);
			break;
		}
		break;
	case 0x6C:
		switch (sw2)
		{
		default:
			TRACE("Le错误，实际长度:0x%2X", sw2);
			break;
		}
		break;
	case 0x6F:
		switch (sw2)
		{
		case 0x00:
			TRACE("数据无效");
			break;

		default:
			TRACE("***未知状态SW2:%2X!***", sw2);
			break;
		}
		break;
	case 0x90:
		if (sw2 == 0x00)
		{
			//TRACE("\r\n");
		}
		else
			TRACE("***未知状态SW2:%2X!***", sw2);
		break;
	case 0x93:
		switch (sw2)
		{
		case 0x03:
			TRACE("应用已被永久锁定");
			break;

		default:
			TRACE("***未知状态SW2:%2X!***", sw2);
			break;
		}
		break;
		/*
		 * case 0x:
		 * switch(sw2)
		 * {

		 * case 0x:
		 * TRACE("");
		 * break;
		 * case 0x:
		 * TRACE("");
		 * break;
		 * default:
		 * TRACE("***未知状态SW2:%2X!***",sw2);
		 * break;
		 * }
		 * break;
		 */
	default:
		TRACE("***未知状态SW1:%2X!***", sw1);
		break;
	}
	TRACE("\r\n");
}

/******************************************************************************
* 函数名称:
*      IfTDi
* 功能描述:
*     判断TDi或T0,判断是否有TAi+1~TDi+1
*
* 输出参数:
*     str                               输出字符串指针
* 返回值: 无
*     1-存在TDi+1  0-不存在TDi+1
* 历史纪录：
*     修改人     日期           版本         日志
******************************************************************************/
void IfTDi(uchar ucData, uchar * ucTADFlag)
{
	uint i;
	uchar ucTmp = 0x01 << 4;
	for (i = 0; i < 4; i++)
	{
		if (ucData & ucTmp)
		{
			ucTADFlag[i] = 1;
		}
		else
		{
			ucTADFlag[i] = 0;
		}
		ucTmp <<= 1;
	}
}

/******************************************************************************
* 函数名称:
*      AnalyseTA
* 功能描述:
*     分析TA
*
* 输出参数:
*     str                               输出字符串指针
* 返回值: 无
*     1-存在TDi+1  0-不存在TDi+1
* 历史纪录：
*     修改人     日期           版本         日志
******************************************************************************/
void AnalyseTA(uchar ucTa, uint uiCount)
{
	uchar ucData1, ucData2;
	TRACE("\r\n***TA%-2d分析:", uiCount);
	switch (uiCount)
	{
	case 1:
		ucData1 = ucTa & 0x0F;
		ucData2 = (ucTa & 0xF0) >> 4;
		TRACE("\r\n	时钟因子:0x%02X   Fi:", ucData2);
		if (EG_uiFiTable[ucData2] != 0xFFFF)
		{
			TRACE("%4d", EG_uiFiTable[ucData2]);
			EG_uiIcCardFi = EG_uiFiTable[ucData2];
		}
		else
			TRACE("RFU");
		TRACE("\r\n	波特率因子:0x%02X Di:", ucData1);
		if (EG_uiDiTable[ucData1] != 0xFFFF)
		{
			TRACE("%4d", EG_uiDiTable[ucData1]);
			EG_uiIcCardDi = EG_uiDiTable[ucData1];
		}
		else
			TRACE("RFU");
		break;

	case 2:
		if (ucTa & 0x80)
		{
			TRACE("\r\n	可以切换操作模式:专用模式和协商模式");
		}
		else
		{
			TRACE("\r\n	不可以切换操作模式");
		}
		if (ucTa & 0x10)
		{
			TRACE("\r\n	Fi使用缺省值372   Di使用缺省值1");
		}
		else
		{
			TRACE("\r\n	Fi和Di使用接口字节的设置值");
		}
		TRACE("\r\n	专用模式下支持的协议:%d", ucTa & 0x0F);
		break;
	case 3:
		TRACE("\r\n	信息域大小(IFSC):%d", ucTa);
		if (ucTa < 0x0F || ucTa == 0xFF)
		{
			TRACE("\r\n不支持此IFSC");
		}
		break;
	case 4:

		break;
	case 5:

		break;

	case 6:

		break;
	case 7:

		break;
	}
}

/******************************************************************************
* 函数名称:
*      AnalyseTB
* 功能描述:
*     分析TA
*
* 输出参数:
*     str                               输出字符串指针
* 返回值: 无
*     1-存在TDi+1  0-不存在TDi+1
* 历史纪录：
*     修改人     日期           版本         日志
******************************************************************************/
void AnalyseTB(uchar ucTb, uint uiCount)
{
	uchar ucData1;
	uint i;
	TRACE("\r\n***TB%-2d分析:", uiCount);
	switch (uiCount)
	{
	case 1:
		ucData1 = ucTb & 0x1F;
		if (ucData1)
		{
			TRACE("\r\n	Vpp编程电压:%d", ucData1);
		}
		else
		{
			TRACE("\r\n	不使用Vpp编程");
		}
		ucData1 = (ucTb & 0x60) >> 5;
		if (ucData1)
			TRACE("\r\n	最大编程电流:%d", ucData1);
		else
		{
			TRACE("\r\n	不使用最大编程电流");
		}
		break;

	case 2:
		TRACE("\r\n	Vpp编程电压:%d 取代TB1的编程电压", ucTb);
		break;
	case 3:
		ucData1 = ucTb & 0x0F;
		i = 1;
		while (ucData1--)
		{
			i *= 2;
		}
		i = i + 11;
		TRACE("\r\n	CWI(字符等待时间整数值):%d  CWT(字符等待时间)=2^CWI+11+4+1=%d etu", ucTb, i);

		ucData1 = (ucTb >> 4) & 0x0F;
		i = 1;
		while (ucData1--)
		{
			i *= 2;
		}
		i = i * 960 * 372 * EG_uiIcCardDi / EG_uiIcCardFi + 11 + EG_uiIcCardDi * 960 + 1;
		TRACE("\r\n	BWI(块等待时间整数值):%d BWT(块等待时间)=[2^BWI ×960 ×372D/F]+11+D*960＋1=%d etu",
			ucTb,i);
		break;
	case 4:

		break;
	case 5:

		break;

	case 6:

		break;
	case 7:

		break;
	}
}

/******************************************************************************
* 函数名称:
*      AnalyseTC
* 功能描述:
*     分析TA
*
* 输出参数:
*     str                               输出字符串指针
* 返回值: 无
*     1-存在TDi+1  0-不存在TDi+1
* 历史纪录：
*     修改人     日期           版本         日志
******************************************************************************/
void AnalyseTC(uchar ucTc, uint uiCount)
{
	//uchar ucData1,ucData2;
	uint i;
	TRACE("\r\n***TC%-2d分析:", uiCount);
	switch (uiCount)
	{
	case 1:
		if (uiCount == 0xFF)
		{
			if (EG_ucIcCardProtocol[0] == EM_ICCard_PROTOCOL_T0)
			{
				TRACE("\r\n	T0卡字符间隔最小时间12etu");
			}
			else if (EG_ucIcCardProtocol[0] == EM_ICCard_PROTOCOL_T1)
			{
				TRACE("\r\n	T1卡字符间隔最小时间11etu");
			}
		}
		else
		{
			TRACE("\r\n	额外保护时间:%3d, 字符间隔最小时间:%3d", ucTc, 12 + ucTc);
		}
		break;

	case 2:
		i = 960 * ucTc * EG_uiIcCardDi;
		TRACE("\r\n	T0卡字符间最大间隔时间(Wi=%d)-%d", ucTc, i);
		break;
	case 3:
		if (ucTc & 0x01)
			TRACE("\r\n	块错误检测方式CRC");
		else
			TRACE("\r\n	块错误检测方式LRC");
		break;
	case 4:

		break;
	case 5:

		break;

	case 6:

		break;
	case 7:

		break;
	}
}

/******************************************************************************
* 函数名称:
*      AnalyseTD
* 功能描述:
*     分析TA
*
* 输出参数:
*     str                               输出字符串指针
* 返回值: 无
*     1-存在TDi+1  0-不存在TDi+1
* 历史纪录：
*     修改人     日期           版本         日志
******************************************************************************/
void AnalyseTD(uchar ucTd, uint uiCount)
{
	//uchar ucData1;
	TRACE("\r\n***TD%-2d分析:", uiCount);
	TRACE("\r\n++++ ICCard协议:T=%d ++++", ucTd & 0x0F);
	EG_ucIcCardProtocol[0] = ucTd & 0x0F;
	switch (uiCount)
	{
	case 1:

		break;

	case 2:

		break;
	case 3:

		break;
	case 4:

		break;
	case 5:

		break;

	case 6:

		break;
	case 7:

		break;
	}
}

/******************************************************************************
* 函数名称: ICCard_ATRAnalyse
*
* 功能描述:
*     从键盘输入字符串，可以说数字或者西文和字符
*
* 输出参数:
*     str                               输出字符串指针
* 返回值: 无
*
* 历史纪录：
*     修改人     日期           版本         日志
******************************************************************************/
uint ICCard_ATRAnalyse(uint uiCount, uchar * ucBuff)
{
	uint i = 0, j;
	uchar ucData, ucData1, ucTmp, ucTAD[4], ucTADData[4], ucLrc;
	TRACE("\r\n------------IC卡ATR数据分析------------");
	// TS
	DISPBUF("atr",uiCount,0,ucBuff);
	ucData = ucBuff[i++];
	TRACE("\r\nTS:%2X ", ucData);
	if (ucData == EM_ic_DirectConvention)
	{
		TRACE("正向协议");
	}
	else if (ucData == EM_ic_InverseConvention)
	{
		TRACE("反向协议");
	}
	else
	{
		TRACE("协议错误");
	}
	// T0
	ucData = ucBuff[i++];
	ucLrc = ucData;
	TRACE("\r\nT0:%2X ", ucData);
	ucTmp = ucData & 0x0F;

	j = 1;
	while (1)
	{
		TRACE("\r\n---|TA%-2d | TB%-2d | TC%-2d | TD%-2d|---", j, j, j, j);
		IfTDi(ucData & 0xF0, ucTAD);
		CLRBUF(ucTADData);
		TRACE("\r\n   |");
		if (ucTAD[0])
		{
			// TA
			ucTADData[0] = ucBuff[i++];
			ucLrc ^= ucTADData[0];
			TRACE("%-02X   | ", ucTADData[0]);
		}
		else
		{
			TRACE("     | ");
		}
		if (ucTAD[1])
		{
			// TB
			ucTADData[1] = ucBuff[i++];
			ucLrc ^= ucTADData[1];
			TRACE("%-02X   | ", ucTADData[1]);
		}
		else
		{
			TRACE("     | ");
		}
		if (ucTAD[2])
		{
			// TC
			ucTADData[2] = ucBuff[i++];
			ucLrc ^= ucTADData[2];
			TRACE("%-02X   | ", ucTADData[2]);
		}
		else
		{
			TRACE("     | ");
		}
		if (ucTAD[3])
		{
			// TD
			ucTADData[3] = ucBuff[i++];
			ucLrc ^= ucTADData[3];
			TRACE("%-02X   | ", ucTADData[3]);
		}
		else if (j == 1)
		{
			TRACE("无  |");
		}

		// 分析
		if (ucTAD[0])
		{
			// TA
			AnalyseTA(ucTADData[0], j);
		}
		else if (j == 1)
		{
			//无 TA1采用默认Fd和Dd
			ucTADData[0] = 0x11;
			AnalyseTA(ucTADData[0], j);
		}
		if (ucTAD[1])
		{
			// TB
			AnalyseTB(ucTADData[1], j);
		}
		if (ucTAD[2])
		{
			// TC
			AnalyseTC(ucTADData[2], j);
		}
		if (ucTAD[3])
		{
			// TD
			AnalyseTD(ucTADData[3], j);
			// 解析下一个TDi
			ucData = ucTADData[3];
		}
		else
		{
			if (j == 1)
			{
				EG_ucIcCardProtocol[0] = EM_ICCard_PROTOCOL_T0;
				TRACE("\r\n+++++ ICCard协议:T=0 +++++");
			}
			//TD无,退出
			break;
		}
		++j;
	}
	if (j == 1)
	{
		// 无TC2
		TRACE("\r\n	T0卡字符间最大间隔默认时间(Wi)10");
	}
	if (EG_ucIcCardProtocol[0] == EM_ICCard_PROTOCOL_T1)
	{
		ucLrc ^= ucBuff[i];
		if (ucLrc)
		{
			TRACE("\r\nT1卡TCK错误 0x%2X - 0x%2X", ucBuff[i], ucLrc);
		}
	}
	if (ucTmp)
	{
		TRACE("\r\n历史字节数%2d:  ", ucTmp);
		for (j = 0; j < ucTmp; j++)
		{
			ucData1 = ucBuff[i++];
			ucLrc ^= ucData1;
			TRACE("0x%02X ", ucData1);
		}
	}
	TRACE("\r\n------------ATR数据分析完毕------------\r\n\r\n");
	return 0;
}


//***************************************EMV*******************************************************
int ManualInputCApdu(uchar *ucCmd)
{
	uchar ucData[256+6];
	int Lc,Le,i,j=0;
	//while(1)
	{
		TRACE("\r\n 输入CLA:");
		ucData[j++] = InkeyHex(0);
		TRACE("\r\n 输入Ins:");
		ucData[j++] = InkeyHex(0);
		TRACE("\r\n 输入P1:");
		ucData[j++] = InkeyHex(0);
		TRACE("\r\n 输入P2:");
		ucData[j++] = InkeyHex(0);
		TRACE("\r\n 输入Lc(0~256):");
		Lc = InkeyCount(0);
		if(Lc)
		{
			if(Lc == 256)
			{
				ucData[j++] = 0;
			}
			else
			{
				ucData[j++] = (uchar)Lc;
			}
			TRACE("\r\n 输入数据:");
			for(i=0;i<Lc;i++)
			{
				ucData[j++] = InkeyHex(0);
			}
		}
		TRACE("\r\n 输入Le(0~256):");
		Le = InkeyCount(0);
		if(Lc)
		{
			if(Lc == 256)
			{
				ucData[j++] = 0;
			}
			else
			{
				ucData[j++] = (uchar)Le;
			}
		}
		memcpy(ucCmd,ucData,j);
		return j;
	}
}
/******************************************************************************
* 函数名称: EI_vICTest
*
* 功能描述:
*
* 输出参数:
*     str                               输出字符串指针
* 返回值: 无
*
* 历史纪录：
*     修改人     日期           版本         日志
******************************************************************************/
extern int test_iccard(int mode);
extern void ShowCardSlotInfo(uint ucSlotNo);
extern void s_lcd_backlight(int status);
//extern void icc_DelayByCLK(int value);
extern void vTestColdRest(void);
extern void vTestWarmRest(void);
extern void Show_CardInfo(void);
extern void set_ncn6001_mosi(void);
extern void clr_ncn6001_mosi(void);
extern void set_ncn6001_sck(void);
extern void clr_ncn6001_sck(void);
extern uchar recv_byte_from_ncn6001(void);
extern void send_byte_to_ncn6001(uchar ucData);
extern void icc_EnableIOInt(void);
extern void icc_DisableIOInt(void);
extern uchar  k_ICC_CardInSert;



void ICCardHardTest(void)
{
    extern int giICSamIOPin,giICCardIOPin;
    extern void icc_delay_clock(int clock);
    extern void icc_delay_etu(int etu, int num);
    extern void icc_delay_inter_timer_cnt(uint cnt);
	int i,j,ret;
	uchar buf[64];
	while(1)
	{
		TRACE("\r\n---------当前卡座:%d----------",gl_CardInfo.current_slot);
		TRACE("\r\n 1-clk  2-rst 3-vcc 4-io 5-设置卡座 6-测试热复位 7-低功耗 8-读取IO中断使能状态");
        TRACE("\r\n 9-时钟源控制 10-etu超时测试 11-icc_delay_clock 12-icc_delay_etu");
        TRACE("\r\n 13-delay_inter_timer_cnt 14-init_inter_timer 15-disable_inter_timer");
        TRACE("\r\n 16-etu_timer_init 17-etu_timer_start 18-etu_timer_close 19-card detecttion");
		i = InkeyCount(0);
		switch(i)
		{
			case 1:
				TRACE("\r\n 1-开 2-关   ");
				i = InkeyCount(0);
				if(i == 1)
				{
//                    icc_control_clk(ON);
//                    icc_ControlClkPin(ON);
					icc_open_clk();
				}
				else if(i == 2)
				{
//                    icc_control_clk(OFF);
//                    icc_ControlClkPin(OFF);
					icc_close_clk();
				}

				break;
			case 2:
				TRACE("\r\n 1-高 2-低   ");
				i = InkeyCount(0);
				if(i == 1)
				{
					icc_SetRST();
				}
				else if(i == 2)
				{
					icc_ClrRST();
				}
				break;
			case 3:
				TRACE("\r\n1-高 2-低 ");
				i = InkeyCount(0);
				if(i == 1)
				{
					TRACE("\r\n 电压:");
					icc_SetVCC(InkeyCount(0));

				}
				else if(i == 2)
				{
					icc_ClrVCC();
				}
				break;
			case 4:
				TRACE("\r\n 1-输入 2-输出  ");
				i = InkeyCount(0);
				if(i == 1)
				{
					icc_SetIOInput();
				}
				else if(i == 2)
				{
					icc_SetIOOutput();
					icc_SetIO();
					TRACE("\r\nIO:1-高 2-低  3-循环   ");
					i = InkeyCount(0);
					if(i == 1)
					{
						icc_SetIO();
					}
					else if(i == 2)
					{
						icc_ClrIO();
					}
					else if(i == 3)
					{
//						for(i=0;i<10;i++)
                        while(1)
						{
							icc_SetIO();
							s_DelayUs(100);
							icc_ClrIO();
							s_DelayUs(100);
                            if ( IfInkey(0) ) {
                                break;
                            }
						}
					}
				}
				break;
			case 5:
				TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
				i = InkeyCount(0) - 1;
				icc_SelectSlot(i);
				break;
			case 6:
				TRACE("\r\n 热复位测试...");
				TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
				ret = icc_Reset(InkeyCount(0) - 1, (uint *)&i, buf);
				DispICCardRet(ret);
				break;
			case 7:
				TRACE("\r\n 1-低功耗 2-正常 ");
				if(InkeyCount(0) == 1)
				{
					icc_ControlLowpower(ON);
				}
				else
				{
					icc_ControlLowpower(OFF);
				}
				break;
            case 8:
                break;
            case 9:
                TRACE("\r\n 1-时钟源开启 2-关闭 ");
                ret = InkeyCount(0);
                if ( ret == 1 ) {
//                    icc_control_clk(ON);
//                    icc_ControlClkPin(ON);
                } else {
//                    icc_control_clk(OFF);
//                    icc_ControlClkPin(OFF);
                }
                break;
            case 10:
                i = InkeyCount(0);
                TRACE("\r\n ---clock delay");
                icc_start_etu_timer(i);      // 等待TS字节时间
                s_DelayMs(100);
                icc_close_etu_timer();
                break;
            case 11:
                TRACE("\r\n clock:");
                i = InkeyCount(0);
                SETSIGNAL1_H();
                icc_delay_clock(i);
                SETSIGNAL1_L();
                break;
            case 12:
                TRACE("\r\n etu:");
                i = InkeyCount(0);
                TRACE("\r\n num:");
                j = InkeyCount(0);
                SETSIGNAL1_H();
                icc_delay_etu(i,j);
                SETSIGNAL1_L();
                break;
            case 13:
                TRACE("\r\n cnt:");
                i = InkeyCount(0);
                icc_delay_inter_timer_cnt(i);
                break;
            case 14:
                icc_init_inter_timer();
                break;
            case 15:
                icc_disable_inter_timer();
                break;
            case 16:
                icc_init_etu_timer();
                break;
            case 17:
                TRACE("\r\n cnt:");
                i = InkeyCount(0);
//                SETSIGNAL2_H();
                icc_start_etu_timer(i);
                break;
            case 18:
                icc_close_etu_timer();
                break;
            case 19:
                j = 0;
                while (1)
                {
                    i = hw_iccard_gpio_read(GPIO_ID_IC_INT);
                    if (i==0)
                    {
                        if (j == 0)
                        {
                            TRACE("\nCardIN");
                            j = 1;
                        }
                    } else
                    {
                        if (j == 1)
                        {
                            TRACE("\nCardOUT");
                            j = 0;
                        }
                    }
                    if (IfInkey(0))
                        break;
                }
                break;
            case 99:
                return;
		}
	}

}

void Test_TDA8035(void)
{
	uint i,j;
        //uint oldtime;
	//uchar ret;
	while(1)
	{
		TRACE("\r\n ----------------------TDA8035----------------------");
		TRACE("\r\n 1-初始化 2-nCMDVCC 3-VCC 4-1.5/3/5V 5-RSTIN  6-卡检测 7-C4 8-C8 9-CLK开启 ");
		TRACE("\r\n 10-低功耗 11-定时器 12-PWM输出 13-icc_control_clk 14-上电  15-下电 16-置IOUC电平");
		i = InkeyCount(0);
		switch(i)
		{
			case 1:
				break;
			case 2:
				TRACE("\r\n CMDVCC:");
				i = InkeyCount(0);
//				icc_hal_TDA8035_CMDVCC(i);
				break;
			case 3:
				TRACE("\r\n VCC(1,3,5,0,8):");
				i = InkeyCount(0);
                icc_SetVCC(i);
//				icc_hal_TDA8035_VCC(i);
				break;
			case 4:
                while ( 1 ) {
                    TRACE("\r\n 1-1.8V高 2-1.8V低 3-3/5V高 4-3/5V低 ");
                    i = InkeyCount(0);
//                    if ( i == 1 ) {
//                        set_pio_output_high(TDA8035_VCC_18);
//                    } else if (i == 2){
//                        clear_pio_output_low(TDA8035_VCC_18);
//                    } else if (i == 3){
//                        set_pio_output_high(TDA8035_VCC_3_5);
//                    } else if (i == 4){
//                        clear_pio_output_low(TDA8035_VCC_3_5);
//                    } else if( i == 99) {
//                        break;
//                    }
                }
				break;
			case 5:
				TRACE("\r\n RST:");
				i = InkeyCount(0);
//                gIccHalInfo.pChipFops->write(&gIccHalInfo,WRITE_RST,i);
//				icc_hal_TDA8035_RSTIN(i);
//                if ( i ) {
//                    set_pio_output_high(PIOC_PIN_(26));
//                }else
//                    clear_pio_output_low(PIOC_PIN_(26));
				break;
			case 6:
				while(1)
				{
					if(IfInkey(0))
						break;
//					TRACE("[%d]",check_pio_input(USERCARD_INT));
                    sys_delay_ms(1000);
				}
				break;
			case 7:
				TRACE("\r\n C4:");
				i = InkeyCount(0);
//				icc_hal_TDA8035_C4(i);
				break;
			case 8:
				TRACE("\r\n C8:");
				i = InkeyCount(0);
//				icc_hal_TDA8035_C8(i);
				break;
			case 9:
				break;
			case 10:
                TRACE("\r\n deep shutdown...");
//                clear_pio_output_low(TDA8035_VCC_18);
//                clear_pio_output_low(TDA8035_VCC_3_5);
//                set_pio_output_high(TDA8035_CMD);
                InkeyCount(0);
                TRACE("\r\n 退出");
//                set_pio_output_high(TDA8035_VCC_18);
				break;
			case 11:
                TRACE("\r\n 1-开启T0外设时钟 2-关闭T0外设时钟 3-Init 4-启动T0 5-关闭T0 ");
                i = InkeyCount(0);
                if ( i == 1 ) {
                    //Enable_TC_Clk(0);
                } else if(i == 2) {
                    //Disable_TC_Clk(0);
                } else if(i == 3) {
//                    icc_init_clk();
                } else if(i == 4) {
                    //TC_CCR_WRITE(0,TC_CLKEN|TC_SWTRG);
                } else if(i == 5) {
                    //TC_CCR_WRITE(0,TC_CLKDIS);
                }
				break;
			case 12:
                TRACE("\r\n 1-时钟源开启 2-关闭 ");
                j = InkeyCount(0);
                if ( j == 1) {
                    icc_open_clk();
                } else if( j == 2){
                    icc_close_clk();
                }
				break;
			case 13:
                TRACE("\r\n 1-时钟源开启 2-关闭 ");
                j = InkeyCount(0);
                if ( j == 1 ) {
//                    icc_control_clk(ON);
                } else {
//                    icc_control_clk(OFF);
                }
				break;
			case 14:
                TRACE("\r\n 上电时序...");
                icc_open_clk();
                icc_SetVCC(5);
                icc_SetIOInput();
                icc_SetRST();
				break;
			case 15:
                TRACE("\r\n 下电时序...");
//                set_pio_output_high(TDA8035_CMD);
                s_DelayUs(200);
//                icc_ControlClkPin(0);
                icc_ClrVCC();
                icc_close_clk();
                icc_ClrRST();
				break;
			case 16:
//                TRACE("\r\n 电平:");
//                if ( InkeyCount(0) ) {
//                    set_pio_output_high(giICCardIOPin);
//                } else {
//                    clear_pio_output_low(giICCardIOPin);
//                }
				break;
			case 99:
				return;
        }
        TRACE("\r\n调试数据:");
        vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
        guiDebugi=0;
	}
}

//#if defined(IC_CONCHIP_8113)
//extern void icc_hal_DS8113_CMDVCC(uint value);
//extern void icc_hal_DS8113_RSTIN(uint value);
//extern void icc_hal_DS8113_C4(uint value);
//extern void icc_hal_DS8113_C8(uint value);
//extern void icc_hal_DS8113_VCC(uint value);
//
//void Test_DS8113(void)
//{
//	uint i,oldtime;
//	uchar ret;
//	while(1)
//	{
//		TRACE("\r\n ----------------------DS8113----------------------");
//		TRACE("\r\n 1-初始化 2-nCMDVCC 3-VCC 4-1.8VN 5-RSTIN  6-卡检测 7-C4 8-C8 9-CLK开启 ");
//		TRACE("\r\n 10-异步卡复位 11-Stop模式 12-低功耗  13-Shim检测 ");
//		i = InkeyCount(0);
//		switch(i)
//		{
//			case 1:
//				cfg_ncn6001(INITIAL,0);
//				break;
//			case 2:
//				TRACE("\r\n CMDVCC:");
//				i = InkeyCount(0);
//				icc_hal_DS8113_CMDVCC(i);
//				break;
//			case 3:
//				TRACE("\r\n VCC:");
//				i = InkeyCount(0);
//				icc_hal_DS8113_VCC(i);
//				break;
//			case 4:
//				TRACE("\r\n 1-1.8V高 2-1.8V低 3-3/5V高 4-3/5V低 ");
//				i = InkeyCount(0);
//                if ( i == 1 ) {
//                    set_pio_output_high(DS8113_VCC_18);
//                } else if (i == 2){
//                    clear_pio_output_low(DS8113_VCC_18);
//                } else if (i == 3){
//                    clear_pio_output_low(DS8113_VCC_3_5);
//                } else if (i == 4){
//                    clear_pio_output_low(DS8113_VCC_3_5);
//                }
//				break;
//			case 5:
//				TRACE("\r\n RST:");
//				i = InkeyCount(0);
//				icc_hal_DS8113_RSTIN(i);
//				break;
//			case 6:
//				while(1)
//				{
//					if(IfInkey(0))
//						break;
//					TRACE("[%d]",cfg_ncn6001(CARD_DETECT,0));
//				}
//				break;
//			case 7:
//				TRACE("\r\n C4:");
//				i = InkeyCount(0);
//				icc_hal_DS8113_C4(i);
//				break;
//			case 8:
//				TRACE("\r\n C8:");
//				i = InkeyCount(0);
//				icc_hal_DS8113_C8(i);
//				break;
//			case 9:
//				cfg_ncn6001(ASYNCLK_MODE,ASYNCLK_1_1);
//				break;
//			case 10:
//				icc_SetIOOutput();
//				icc_ClrIO();
//				icc_hal_DS8113_RSTIN(1);
//				icc_hal_DS8113_CMDVCC(1);
//				s_DelayUs(10);
//				icc_hal_DS8113_CMDVCC(0);
//				icc_SetIOInput();
//				s_DelayUs(150);
//				icc_hal_DS8113_RSTIN(0);
//				s_DelayUs(100);
//				icc_hal_DS8113_RSTIN(1);
//				s_DelayUs(50);
//				icc_hal_DS8113_CMDVCC(1);
//				break;
//			case 11:
//				set_pio_output_high(DS8113_VCC_18);
//				set_pio_output_high(DS8113_VCC_3_5);
//				icc_hal_DS8113_CMDVCC(1);
//				break;
//			case 12:
//
//				break;
//			case 13:
//                icc_hal_DS8113_init(1);
//                clear_pio_output_low(giICCardIOPin);
//                disable_pio_output(giICCardIOPin);
//                i = 0;
//                while ( 1 ) {
//                    cfg_ncn6001(VCC_MODE,5);
//                    oldtime=sys_get_counter();
//                    s_DelayMs(10);
//                    cfg_ncn6001(VCC_MODE,0);
////                    while(1){
////                        ret = cfg_ncn6001(SYNC_RD_IO, 0);
////                        if ( ret ) {
////                            curtime = sys_get_counter();
////                            TRACE("\r\n time:%d-%d",oldtime,curtime);
////                            break;
////                        }
////                        if(kb_hit())
////                        {
////                            break;
////                        }
////                    }
////                    if ( ret ) {
////                        cfg_ncn6001(VCC_MODE,0);
////                    }
//                    ret = cfg_ncn6001(SYNC_RD_IO, 0);
//					TRACE("[%d]",ret);
//                    lcd_display(0,0,FONT_SIZE12|DISP_CLRLINE,"[%d-%d]",ret,i++);
////                    s_DelayMs(500);
////					if(IfInkey(0))
////					{
////						break;
////					}
////					if(kb_hit())
////					{
////						break;
////					}
//                    if(drv_kb_getkey(-1) == KEY_CANCEL)
//                        break;
//                }
//				break;
//			case 14:
//
//				break;
//			case 15:
//
//				break;
//			case 99:
//				return;
//		}
//	}
//}
//#endif


int DrawCardTest(void)
{
	int ret=-ICC_CARDOUT,i,k=0;
	uchar buf[64];
	TRACE("\r\n ----------拔卡测试----------\r\n");
	while(1)
	{
		if(IfInkey(0))
		{
			if(InkeyCount(0) == 99)
				return 99;
		}
		if(ret == -ICC_CARDOUT)
		{
			ret = icc_CheckInSlot(0);
		}
		if(ret != -ICC_CARDOUT && ret != 0)
		{
			ret = icc_Reset(0, (uint *)&i, buf);
			if(!ret)
			{
				TRACE("\r\n");
				k = 0;
			}
			else
			{
				TRACE("[reset:%d]",ret);
			}

		}
		if(!ret)
		{
			ret = icc_ExchangeData(0,sizeof(gApduGet4rand), (uchar *)gApduGet4rand,&i,buf);
			if(ret)
			{
				TRACE("[%d-%d]",k,ret);
			}
			else
			{
				if(k % 200 == 0)
				{
					TRACE("[%d-%d]",k,ret);
				}
			}
			++k;
		}

	}
}


typedef struct
{
	int cmdlen;
	char *cmd;
} ET_IC_CMD;

//查询余额命令
const ET_IC_CMD gYueCmd[] = {
	{20,"\x00\xa4\x04\x00\x0e\x31\x50\x41\x59\x2e\x53\x59\x53\x2e\x44\x44\x46\x30\x31\x00"},
	{5, "\x00\xb2\x01\x0c\x00"},
	{5, "\x00\xb2\x02\x0c\x00"},
	{14,"\x00\xa4\x04\x00\x08\xa0\x00\x00\x03\x33\x01\x01\x06\x00"},
	{17,"\x80\xa8\x00\x00\x0b\x83\x09\x00\x00\x00\x00\x00\x00\x00\x01\x56\x00"},
	{5, "\x00\xb2\x01\x0c\x00"},
	{5, "\x00\xb2\x02\x0c\x00"},
	{5, "\x00\xb2\x01\x14\x00"},
	{5, "\x00\xb2\x02\x14\x00"},
	{5, "\x00\xb2\x03\x14\x00"},
	{5, "\x00\xb2\x04\x14\x00"},
    {5, "\x00\xb2\x01\x1c\x00"},
	{5, "\x00\xb2\x02\x1c\x00"},
	{5, "\x00\xb2\x03\x1c\x00"},
	{5, "\x00\xb2\x01\x24\x00"},
	//{0, ""},
};

int Test_checkyue(uint slot, uint times,uint mode)
{
	uint i,j;
	uchar buf[256];
	int ret;
	TRACE("\r\n ------查询余额测试%d------",times);
	guiDebugi = 0;
	ret = icc_Reset(slot, &i, buf);
	if(ret)
	{
		TRACE("\r\n 复位失败:%d",ret);
		return ret;
	}
	for(i=0;i<DIM(gYueCmd);i++)
	{
		if(mode)
			TRACE("\r\n %d",i);
		else
			TRACE(" %d",i);
		if(mode)
			vDispBuf(gYueCmd[i].cmdlen, 0, (uchar *)gYueCmd[i].cmd);
		guiDebugi = 0;
		ret = icc_ExchangeData(slot,gYueCmd[i].cmdlen,(uchar *)gYueCmd[i].cmd,(int *)&j,buf);
		if(ret)
		{
			TRACE("\r\n 读卡失败:%d",ret);
			break;
		}
		else
		{
			if(mode)
				vDispBuf(j, 0, buf);
		}
	}
	icc_Close(slot);
	return ret;
}




void atr_analyse(void)
{
	int len,i=0;
	uchar atr[32];
	TRACE("\r\n 输入ATR长度");
	len = InkeyCount(0);
	while(len--)
	{
		atr[i++] = InkeyHex(0);
	}
    ICCard_ATRAnalyse(i, atr);
}

int test_asyncard_combine(int mode)
{
    int i,j,k,h,m,n,iRet,flag;
    uchar ucBuff[128];
    TRACE("\r\n SAM卡数:");
    i = InkeyCount(0);
    TRACE("\r\n 测试次数:");
    h = InkeyCount(0);
    TRACE("\r\n 读卡次数:");
    n = InkeyCount(0);
    TRACE("\r\n 1-开启调试信息");
    flag = InkeyCount(0);
    TRACE("\r\n 大卡和%d张SAM卡混合测试");
    while(h--)
    {
        if(IfInkey(0))
        {
            break;
        }
        icc_InitModule(USERCARD, VCC_5,ISOMODE);
        TRACE("\r\n %d大卡复位...",h);
        iRet = icc_Reset(USERCARD, (uint *)&j, ucBuff);
        if(!iRet)
        {
            if(flag == 1)
                DISPBUF("ATR",j,0,ucBuff);
            for(j=0;j<i;j++)
            {
                TRACE("\r\n 小卡%d复位...",SAM1SLOT+j);
                icc_InitModule(SAM1SLOT+j, VCC_5,ISOMODE);
                iRet = icc_Reset(SAM1SLOT+j, (uint *)&k, ucBuff);
                if(iRet)
                {
                    TRACE("失败!",SAM1SLOT+j);
                    DispICCardRet(iRet);
                    debug_beep();
                    goto cpucard_combine_testend;
                }
                else
                {
                    //TRACE("成功");
                    if(flag == 1)
                        DISPBUF("ATR",k,0,ucBuff);
                    for(k=0;k<n;k++)
                    {
                        iRet = icc_ExchangeData(USERCARD,sizeof(gApduGet4rand), (uchar *)gApduGet4rand,(int *)&m,ucBuff);
                        if(iRet)
                        {
                            TRACE("\r\n [%d]读大卡失败%d!",k,iRet);
                            debug_beep();
                            goto cpucard_combine_testend;
                        }
                        else
                        {
                            TRACE("[读大卡%d]",k);
                            if(flag == 1)
                            {
                                DISPBUF("读大卡",m,0,ucBuff);
                            }
                        }
                        iRet = icc_ExchangeData(SAM1SLOT+j,sizeof(gApduGet4rand), (uchar *)gApduGet4rand,(int *)&m,ucBuff);
                        if(iRet)
                        {
                            TRACE("\r\n [%d]读SAM%d卡失败%d!",k,SAM1SLOT+j,iRet);
                            debug_beep();
                            goto cpucard_combine_testend;
                        }
                        else
                        {
                            TRACE("[读SAM%d卡%d]:",SAM1SLOT+j,k);
                            if(flag == 1)
                            {
                                DISPBUF(NULL,m,0,ucBuff);
                            }

                        }
                    }
                    TRACE("\r\n SAM%d卡下电!",SAM1SLOT+j);
                    icc_Close(SAM1SLOT+j);
                }
            }
            TRACE("\r\n 大卡下电!");
            icc_Close(USERCARD);
        }
        else
        {
            TRACE("失败!");
            debug_beep();
            break;
        }
    }
cpucard_combine_testend:
    return 0;
}
void vTest_Icc(void)
{
	int iRet=0,iSlot=0;
	uint i, j, k,m,n;
	uchar ucBuff[384];
	guiDebugFlg = 1;
    icc_InitIccModule();
	icc_InitModule(USERCARD, VCC_5,ISOMODE);
	//icc_InitModule(USERCARD, VCC_5,CITYUNIONMODE|(B38400<<8)|(B38400<<8));
	icc_InitModule(SAM1SLOT, VCC_5,ISOMODE);
	icc_InitModule(SAM2SLOT, VCC_5,ISOMODE);
//	icc_InitModule(SAM3SLOT, VCC_5,ISOMODE);
//	icc_InitModule(SAM4SLOT, VCC_5,ISOMODE);
	while (1)
	{
		CLRBUF(ucBuff);
        gucDebugm = 0;
        TRACE("\r\n ===core:%d mcg:%d===",core_clk_mhz,mcg_clk_hz);
		TRACE("\r\n--------------IC卡测试 卡座:%d-%d  ",gl_CardInfo.current_slot,k_ICC_CardInSert);
		TRACE("\r\n k:%d  Flag:%d",guiDebugk,guiDebugFlg);
		TRACE("\r\n1-icc_InitModule 2-icc_CheckInSlot  3-icc_CheckVcc 4-icc_Reset 5-icc_Close ");
		TRACE("\r\n6-随机数 7-主目录 8-初始化模块 9-ShowCardSlotInfo 10-Show_CardInfo ");
		TRACE("\r\n11-硬件测试 12-8035 13- 生产测试 14-ATRAnalyse 15-读余额测试");
		TRACE("\r\n16-连续根目录 17-设置参考电平 18-设置Debugm-n 19-调试开关 20-显示调试数据");
		TRACE("\r\n21-连续复位 22-磁卡 23-同步卡测试 24-CPU卡混合测试 25-Rfid ");
		TRACE("\r\n26-选择卡槽 27-获得响应 28-热复位 29- 30- 31- 33-拔卡测试  ");

		j = InkeyCount(0);
		if(j)
		{
			switch (j)
			{
				case 1:
					TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
					iSlot = InkeyCount(0) - 1;
					TRACE("\r\n 选择规范:1-1.8V 3-3V 5-5V ");
					j = InkeyCount(0);
					TRACE("\r\n 选择规范:0-ISO 1-EMV 2-社保");
					k = InkeyCount(0);
					TRACE("\r\n 上电速率:0-9600 1-19200 2-38400 3-55800 4-115200 ");
					m = InkeyCount(0);
					m = k|(m<<8);
					iRet = icc_InitModule(iSlot,j,m);
					DispICCardRet(iRet);
					break;
				case 2:
					TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
					i = InkeyCount(0) - 1;
					iRet = icc_CheckInSlot(i);
					DispICCardRet(iRet);
					DISP_DEBUGBUF();
					break;
				case 3:
					TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
					i = InkeyCount(0) - 1;
					iRet = icc_CheckVcc(i);
					DispICCardRet(iRet);
					break;
				case 4:
					CLRBUF(ucBuff);
					INIT_DEBUGBUF();
					TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
					i = InkeyCount(0) - 1;
					iRet = icc_Reset(i, &j, ucBuff);
					DISP_DEBUGBUF();
					DispICCardRet(iRet);
					if (!iRet)
					{
                        ICCard_ATRAnalyse(j, ucBuff);
					}
					break;
				case 5:
					TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
					i = InkeyCount(0) - 1;
					iRet = icc_Close(i);
					DispICCardRet(iRet);
					break;
				case 6:
					TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
					i = InkeyCount(0) - 1;
					TRACE("\r\n 次数:");
					j = InkeyCount(0);
					TRACE("\r\n 1-读卡失败跳出");
					m = InkeyCount(0);
					while(j--)
					{
						if(IfInkey(0))
						{
							if(InkeyCount(0))
								break;
						}
						TRACE("\r\n 随机数%d:",j);
						guiDebugi = 0;
						iRet = icc_ExchangeData(i,sizeof(gApduGet4rand), (uchar *)gApduGet4rand,(int *)&k,ucBuff);
						if(!iRet)
						{
							DISPBUF("recv",k,0,ucBuff);
						}
						else
						{
							if(m==1)
							{
								break;
							}
						}

					}
					DispICCardRet(iRet);
					TRACE("\r\n1-输出调试数据");
					if(InkeyCount(0) == 1)
						DISP_DEBUGBUF();

					break;
				case 7:
					TRACE("\r\n 1-选择主目录1  2-选择主目录2");
					k = InkeyCount(0);
					TRACE("\r\n 1-先复位 ");
					j = InkeyCount(0);
					if(j == 1)
					{
						TRACE("\r\n 复位...",iRet);
						iRet = icc_Reset(iSlot, &i, ucBuff);
						if(iRet)
						{
							TRACE("\r\n 复位失败:%d",iRet);
							break;
						}
					}
					TRACE("\r\n 发送:");
					if(k == 1)
					{
						vDispBuf(gYueCmd[0].cmdlen, 0, (uchar *)gYueCmd[0].cmd);
						iRet = icc_ExchangeData(iSlot,gYueCmd[0].cmdlen,(uchar *)gYueCmd[0].cmd,(int *)&i,ucBuff);
					}
					else
					{
						iRet = icc_ExchangeData(iSlot,sizeof(gApduSelMaindir), (uchar *)gApduSelMaindir,(int *)&i,ucBuff);
					}
					DispICCardRet(iRet);
					if(!iRet)
					{
                        DISPBUF("recv",i,0,ucBuff);
					}
					TRACE("\r\n调试数据%d  1-显示",guiDebugi);
					if(InkeyCount(0) == 1)
						vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
					guiDebugi = 0;
					break;
				case 8:
					icc_InitIccModule();
					break;
				case 9:
					ShowCardSlotInfo(iSlot);
					break;
				case 10:
					Show_CardInfo();
					break;
				case 11:
					ICCardHardTest();
					break;
				case 12:
                    Test_TDA8035();
					break;
				case 13:
//					test_iccard(0);
					break;
				case 14:
					CLRBUF(ucBuff);
					i = 16;
					memcpy(ucBuff,"\x3B\xB7\x94\x00\x81\x31\xFE\x45\x53\x50\x4B\x32\x34\x90\x00\xF6",16);
					/*
					TRACE("\r\n 输入ATR长度:");
					i = InkeyCount(0);
					for(j=0;j<i;j++)
					{
						ucBuff[j] = InkeyHex(0);
					}
					*/
					ICCard_ATRAnalyse(i,ucBuff);
					break;
				case 15:
					i = 0;
					TRACE("\r\n 卡座:");
					m = InkeyCount(0);
					TRACE("\r\n 是否显示数据:");
					j = InkeyCount(0);
					TRACE("\r\n 1-失败退出:");
					k = InkeyCount(0);

					while(1)
					{
						if(IfInkey(0))
						{
							if(InkeyCount(0) == 99)
								break;
						}
						if(Test_checkyue(m,++i,j))
						{
							if(k == 1)
								break;
						}
					}
					break;
				case 16:
					TRACE("\r\n选目录次数:");
					i = InkeyCount(0);
					for(j=0;j<i;j++)
					{
						iRet = icc_ExchangeData(iSlot,sizeof(gApduSelMaindir), (uchar *)gApduSelMaindir,(int *)&k,ucBuff);
					}
					break;
				case 17:
                    TRACE("\r\n 1-高  2-低 3-循环");
                    j = InkeyCount(0);
                    TRACE("\r\n 参考(1~2)");
                    i = InkeyCount(0);
                    if ( i == 1 ) {
                        if ( j == 1 ) {
                            SETSIGNAL1_H();
                        } else if(j==2){
                            SETSIGNAL1_L();
                        } else if(j==3) {
                            while ( 1 ) {
                                SETSIGNAL1_H();
                                s_DelayUs(10);
                                SETSIGNAL1_L();
                                s_DelayUs(10);
                                if ( IfInkey(0) ) {
                                    break;
                                }
                            }
                        }
                    } else if (i == 2){
                        if ( j == 1 ) {
                            SETSIGNAL2_H();
                        } else if(j==2){
                            SETSIGNAL2_L();
                        } else if(j==3) {
                            while ( 1 ) {
                                SETSIGNAL2_H();
                                s_DelayUs(10);
                                SETSIGNAL2_L();
                                s_DelayUs(10);
                                if ( IfInkey(0) ) {
                                    break;
                                }
                            }
                        }
                    }
					break;
				case 18:
					
					break;
				case 19:
					TRACE("\r\nguiDebugFlg:");
					guiDebugFlg = InkeyCount(0);

					break;
				case 20:
					TRACE("\r\n调试数据:");
					vDispBuf(guiDebugi,0,(uchar *)gcDebugBuf);
					guiDebugi=0;
					break;
				case 21:
					TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
					i = InkeyCount(0)-1;
					TRACE("\r\n 1-下电:");
					m = InkeyCount(0);
					TRACE("\r\n 间隔时间:");
					n = InkeyCount(0);
					TRACE("\r\n 连续复位:");
					k = 0;
					while(1)
					{
						guiDebugi=0;
						TRACE("\r\n ***********复位:[%03d]",++k);
						iRet = icc_Reset(i, &j, ucBuff);
						if(iRet)
						{
							DispICCardRet(iRet);
							break;
						}
						else
						{
							/*
							if(j != 16)
							{
								TRACE("\r\n 长度错误:%d",j);
								vDispBuf(j, 0, ucBuff);
								break;
							}
							//if(memcmp(ucBuff,"\x3B\xD9\x11\x06\x40\xF0\x45\x53\x41\x4D\x12\x2E\xCE\x06\x32\x00",16))

							if(memcmp(ucBuff,"\x3B\xD9\x11\x06\x40\xF0\x45\x53\x41\x4D\x12\x5C\x1B\xD1\xA9\x00",16))
							{
								TRACE("\r\n 内容错误:");
								vDispBuf(j, 0, ucBuff);
								break;
							}
							*/
							vDispBuf(j, 0, ucBuff);
							if(m == 1)
							{
								TRACE("\r\n 下电");
								icc_Close(i);
							}
							s_DelayMs(n);
						}
						if(IfInkey(0))
						{
							if(InkeyCount(0) == 99)
							{
								ICCard_ATRAnalyse(j, ucBuff);
								break;
							}
						}
					}
					DISP_DEBUGBUF();
					break;
				case 22:
#ifdef CFG_MAGCARD
                    mag_open();
                    TRACE("mag test");
                    while ( 1 ) {
                        if ( mag_check(0) == EM_SUCCESS) {
                            TRACE("\nhave data");
                            mag_magtek_read(&ucBuff[0],&ucBuff[128],&ucBuff[256]);
                        }
                        if (IfInkey(0)) {
                            TRACE("key");
                            if (InkeyCount(0) == 99)
                                break;
                        }
                    }
#endif
					break;
				case 23:
				#if (defined(CFG_ASYNCARD))
					SysCardTest();
				#endif
					break;
				case 24:
					break;
				case 25:
//				#ifdef CFG_RFID
//					Test_Rfid();
//				#endif
					break;
				case 26:
					TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
					i = InkeyCount(0) - 1;
					icc_SelectSlot(i);
					break;
				case 27:
					TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
					i = InkeyCount(0) - 1;
					TRACE("\r\n 次数:");
					j = InkeyCount(0);
					if(j==0)
						j = 1;
					while(j--)
					{
						TRACE("\r\n 获得响应%d:",j);
						iRet = icc_ExchangeData(i,5,(uchar *)"\x00\xC0\x00\x00\x04",(int *)&k,ucBuff);
						if(!iRet)
						{
							DISPBUF("recv",k,0,ucBuff);
						}
						DispICCardRet(iRet);
					}

					TRACE("\r\n调试数据:");
					DISP_DEBUGBUF();
					break;
				case 28:
					CLRBUF(ucBuff);
					INIT_DEBUGBUF();
					TRACE("\r\n 选择卡槽:1.大卡 2.小卡1 3.小卡2 4.小卡3 5.小卡4    ");
					i = InkeyCount(0) - 1;
                    gucDebugm = 0x55;
					iRet = icc_Reset(i, &j, ucBuff);
					DISP_DEBUGBUF();
					DispICCardRet(iRet);
					if (!iRet)
					{
						ICCard_ATRAnalyse(j, ucBuff);
					}
					break;
				case 29:
                    gucDebugm = 0x55;
                    while ( 1 ) {
                        if ( IfInkey(0) ) {
                            break;
                        }
                        CLRBUF(ucBuff);
                        INIT_DEBUGBUF();
                        iRet = icc_Reset(i, &j, ucBuff);
                        TRACE("\r\n ----iRet:%d ",iRet);
                        if ( iRet == 0 ) {
                           sys_beep(); 
                        }
                    }
					break;
				case 30:
					break;
				case 31:
					break;
				case 32:
					break;
				case 33:
					DrawCardTest();
					break;
				case 34:
					break;
				case 35:
					break;
				case 36:
					break;
				case 37:
					break;
				case 38:
					break;
                case 39:
                    break;
				case 99:
					return;
					break;
			}
		}


	}

}



#endif

