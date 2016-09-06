#include "wp30_ctrl.h"

#ifdef CFG_ICCARD


/*-----------------------------------------------------------------------------}
 *  外部变量
 *-----------------------------------------------------------------------------{*/
extern volatile UINT64 gl_char_wait_etu;
extern volatile UINT64 gl_total_atr_etu;
/*-----------------------------------------------------------------------------}
 *  全局变量
 *-----------------------------------------------------------------------------{*/
//----Fi/Di表-----
//   0      1      2      3      4      5      6      7      8      9      10     11     12     13     14     15
//Fi:372    372    558    744    1116   1488   1860   0      0      512    768    1024   1536   2048   0      0
//Di:0      1      2      4      8      16     32     64     12     20     0      0      0      0      0      0
ushort const 	F_Table[16] = {372,372,558,744,1116,1488,1860,0,0,512,768,1024,1536,2048,0,0};
uchar const 	D_Table[16] = {0,1,2,4,8,16,32,64,12,20,0,0,0,0,0,0};

//=====ETU表======
//   0      1      2      3      4      5      6      7      8      9      10     11     12     13     14     15
//1 :372    372    558    744    1116   1488   1860   0      0      512    768    1024   1536   2048   0      0
//2 :186    186    279    372    558    744    930    0      0      256    384    512    768    1024   0      0
//3 :93     93     139    186    279    372    465    0      0      128    192    256    384    512    0      0
//4 :46     46     69     93     139    186    232    0      0      64     96     128    192    256    0      0
//5 :23     23     34     46     69     93     116    0      0      32     48     64     96     128    0      0
//6 :11     11     17     23     34     46     58     0      0      16     24     32     48     64     0      0
//7 :5      5      8      11     17     23     29     0      0      8      12     16     24     32     0      0
//8 :31     31     46     62     93     124    155    0      0      42     64     85     128    170    0      0
//9 :18     18     27     37     55     74     93     0      0      25     38     51     76     102    0      0

//=====EPT - PPS表======
//Di:0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
//1 :93   93   93   93   93   93   93   93   93   93   93   93   93   93   372  93
//2 :372  372  372  372  372  372  372  372  372  372  372  372  372  372  372  372
//3 :186  186  279  372  372  372  372  372  372  372  372  372  372  372  372  372
//4 :93   93   372  186  279  372  372  372  372  372  372  372  372  372  372  372
//5 :372  372  372  93   372  186  372  372  372  372  372  372  372  372  372  372
//6 :372  372  372  372  372  93   372  372  93   372  372  372  372  372  372  372
//7 :372  372  372  372  372  372  372  372  372  372  372  372  372  372  372  372
//8 :372  372  372  372  372  372  372  372  372  372  372  372  372  372  372  372
//9 :93   93   372  93   93   124  155  372  372  372  372  372  372  372  372  372

/* 0表示按照默认9600通信,1表示可以根据TA1设置,2表示最大可以协商到38400*/
const uchar gucPPSEnbleBps[9][16]={
  //0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,
  	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //1	行对应F值0-F，列对应D的值1-9
  	{1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0}, //2
  	{1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0}, //3
  	{0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0}, //4
  	{0,0,0,0,0,1,0,0,2,0,0,0,0,0,0,0}, //5
  	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //6
  	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //7
  	{2,2,0,2,1,1,1,0,0,0,0,0,0,0,0,0}, //8
  	{0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0}  //9
};

ICCARD_WORK_STRUCT gl_CardSlotInfo[CARD_SLOTNUM] UPAREA;
/*-----------------------------------------------------------------------------}
 *  函数 
 *-----------------------------------------------------------------------------{*/
void s_InitCardInfo(void)
{
//	uchar Init_temp;
    gl_CardSlotInfo[gl_CardInfo.current_slot].CardPowerOn = 0;
	gl_CardSlotInfo[gl_CardInfo.current_slot].ResetStatus = 0x00;
	gl_CardSlotInfo[gl_CardInfo.current_slot].T = 0;
	
	switch(gl_CardSlotInfo[gl_CardInfo.current_slot].BaudRate)
	{
		case B9600:
			gl_CardSlotInfo[gl_CardInfo.current_slot].D = 1;
			gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = 372;
			break;
		case B19200:
			gl_CardSlotInfo[gl_CardInfo.current_slot].D = 2;
			gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = 186;
			break;
		case B38400:
			gl_CardSlotInfo[gl_CardInfo.current_slot].D = 4;
			gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = 93;
			break;
		case B55800:
			gl_CardSlotInfo[gl_CardInfo.current_slot].D = 8;
			gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = 64;
			break;
		case B115200:
			gl_CardSlotInfo[gl_CardInfo.current_slot].D = 8;
			gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = 46;
			break;
		default:
			gl_CardSlotInfo[gl_CardInfo.current_slot].D = 1;
			gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = 372;
			break;
	}
//	Init_temp = gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode;
//	gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode = Init_temp & 0xff;
	gl_CardSlotInfo[gl_CardInfo.current_slot].PPSENABLE = 0x00;
	gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC = 0x20;
	gl_CardSlotInfo[gl_CardInfo.current_slot].IFSD = 0xFE;

	gl_CardSlotInfo[gl_CardInfo.current_slot].WWT = 10081;
	gl_CardSlotInfo[gl_CardInfo.current_slot].CWT = 48;
	gl_CardSlotInfo[gl_CardInfo.current_slot].BWT = 16331;
	gl_CardSlotInfo[gl_CardInfo.current_slot].BWT_TEMP = 16331;
	gl_CardSlotInfo[gl_CardInfo.current_slot].BGT = 16;

	gl_CardSlotInfo[gl_CardInfo.current_slot].sw1 = 0x90;
	gl_CardSlotInfo[gl_CardInfo.current_slot].sw2 = 0x00;

	gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB = 0x00;
	gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB = 0x00;

	// 复位所有ATR接口字符
	memset((uchar*)&gl_CardSlotInfo[gl_CardInfo.current_slot].TS, 0x00, 23);
}

void icc_set_RecTime(void)
{
#if 2
	gl_CardInfo.Rec_P15 =  (uint)(15 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P25 =  (uint)(25 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P35 =  (uint)(35 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P45 =  (uint)(45 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P55 =  (uint)(55 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P65 =  (uint)(65 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P75 =  (uint)(75 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P85 =  (uint)(85 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P95 =  (uint)(95 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P100 = (uint)(100 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P105 = (uint)(105 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P110 = (uint)(110 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
	gl_CardInfo.Rec_P120 = (uint)(120 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU / 10);
#else
	gl_CardInfo.Rec_P15 =  (uint)(1.5 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P25 =  (uint)(2.5 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P35 =  (uint)(3.5 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P45 =  (uint)(4.5 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P55 =  (uint)(5.5 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P65 =  (uint)(6.5 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P75 =  (uint)(7.5 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P85 =  (uint)(8.5 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P95 =  (uint)(9.5 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P100 = (uint)(10  * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P105 = (uint)(10.5* gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P110 = (uint)(11 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
	gl_CardInfo.Rec_P120 = (uint)(12 *  gl_CardSlotInfo[gl_CardInfo.current_slot].ETU);
#endif
}


void icc_InitICCTiming(void)
{
	//gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = 372;
	gl_CardSlotInfo[gl_CardInfo.current_slot].TS = 0xFF;
//D2(TRACE("\r\n[ETU0:%d]",gl_CardSlotInfo[gl_CardInfo.current_slot].ETU););
    icc_set_RecTime();

	gl_CardInfo.Parity_Check_Enable = 0x00;
	gl_CardInfo.k_Ic_CharWait_TimeCount = 10081;
	gl_CardInfo.k_Ic_LastSendByte = 0;
}

void icc_adjustTiming(void)
{
//D2(TRACE("\r\n[ETU1:%d]",gl_CardSlotInfo[gl_CardInfo.current_slot].ETU););
	// 设置字节接收时间点
    icc_set_RecTime();
	// 设置字节发送时间点
	gl_CardInfo.Send_P1 = 1 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P2 = 2 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P3 = 3 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P4 = 4 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P5 = 5 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P6 = 6 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P7 = 7 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P8 = 8 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P9 = 9 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P10 = 10 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P11 = 11 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
	gl_CardInfo.Send_P13 = 13 * gl_CardSlotInfo[gl_CardInfo.current_slot].ETU;
}

// 返回值说明:
// 0   --- 成功
// -1 --- 超时失败
// -2 --- TS错
// -3 --- 奇偶校验错
// -4 --- ATR数据错
static int icc_GetAtr(uchar *Buff)
{
	uchar	i = 0,Flag = 0,x = 0,y,z,atr_x;
	uchar	*Lenth_Point;
	//uchar	*ptr_atr;
	uchar	T = 0,edc = 0,TCK = 0,CWI = 0,BWI = 0;
	ushort	N = 0,j = 0;
	uchar	Lenth = 2;
	uint  	Fi = 0;
	uint  	Di = 0;
	uint  	ETU = 0;
	uint k;
	edc = 0;
	T = 0;

	gl_CardInfo.k_total_timeover = 0x00;
	gl_CardInfo.Parity_Check_Enable = 0x00;

	gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag = 0;
	gl_CardSlotInfo[gl_CardInfo.current_slot].TBFlag = 0;
	gl_CardSlotInfo[gl_CardInfo.current_slot].TCFlag = 0;
	gl_CardSlotInfo[gl_CardInfo.current_slot].TDFlag = 0;
	gl_CardSlotInfo[gl_CardInfo.current_slot].TS = 0xFF;

	Lenth_Point = Buff;

	// 获取TS字节
	*Buff = 0x00;
	*++Buff = icc_RecvByte();
	if(gl_CardInfo.k_IccComErr)
	{
		if(gl_CardInfo.k_IccErrTimeOut)
		{
            return ATR_TIMEOUT;
		}
		else if(gl_CardInfo.k_IccErrPar)
		{
            return ATR_PARERR;
		}
		else
		{
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode != SHBMODE)
			{
            	return ATR_TSERR;
			}
		}
	}
	gl_CardSlotInfo[gl_CardInfo.current_slot].TS = *Buff;

	//社保规范不支持TS!=3F的卡
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
	{
		if(gl_CardSlotInfo[gl_CardInfo.current_slot].TS!= 0x3b)
		{
			return ATR_TSERR_SHB;
		}
	}

	edc = 0;
	//ptr_atr = Buff;

	// 获取T0字节
	*++Buff = icc_RecvByte();
	if(gl_CardInfo.k_IccComErr)
	{
		if(gl_CardInfo.k_IccErrTimeOut)
		{
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
			{
				return ATR_TIMEOUT_SHB;
			}
			else
			{
				return ATR_TIMEOUT;
			}
		}
		else
		{
			return ATR_PARERR;
		}
	}

	edc = edc^*Buff;
	Flag = *Buff;
	// 计算历史字节个数
	x = (Flag & 0x0f);
	i = 1;
	for(;;)
	{
		if((Flag & 0x10) == 0x10)
		{   /* TA */
			*++Buff = icc_RecvByte();
			if(gl_CardInfo.k_IccComErr)
			{
				if(gl_CardInfo.k_IccErrTimeOut)
				{
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
					{
						return ATR_TIMEOUT_SHB;
					}
					else
					{
						return ATR_TIMEOUT;
					}
				}
				else
				{
					return ATR_PARERR;
				}
			}

			if(i == 1)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag |= 0x01;
			}
			else if(i == 2)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TA2 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag |= 0x02;
			}
			else if(i == 3)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TA3 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag |= 0x04;
			}
			else
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TA4 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag |= 0x08;
			}
			edc = edc^*Buff;
			Lenth++;
		}

		if((Flag&0x20) == 0x20)
		{	/* TB  */

			*++Buff = icc_RecvByte();
			if(gl_CardInfo.k_IccComErr)
			{
				if(gl_CardInfo.k_IccErrTimeOut)
				{
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
					{
						return ATR_TIMEOUT_SHB;
					}
					else
					{
						return ATR_TIMEOUT;
					}
				}
				else
				{
					return ATR_PARERR;
				}
			}

			if(i == 1)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TB1 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TBFlag |= 0x01;
			}
			else if(i == 2)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TB2 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TBFlag |= 0x02;
			}
			else if(i == 3)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TB3 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TBFlag |= 0x04;
			}
			else
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TB4 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TBFlag |= 0x08;
			}
			edc = edc^*Buff;
			Lenth++;
		}

		if((Flag&0x40) == 0x40)
		{   /* TC */
			*++Buff = icc_RecvByte();
			if(gl_CardInfo.k_IccComErr)
			{
				if(gl_CardInfo.k_IccErrTimeOut)
				{
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
					{
						return ATR_TIMEOUT_SHB;
					}
					else
					{
						return ATR_TIMEOUT;
					}
				}
				else
				{
					return ATR_PARERR;
				}
			}

			if(i == 1)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TC1 = *Buff;
                gl_CardSlotInfo[gl_CardInfo.current_slot].TCFlag |= 0x01;
			}
			else if(i == 2)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TC2 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TCFlag |= 0x02;
			}
			else if(i == 3)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TC3 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TCFlag |= 0x04;
			}
			else
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TC4 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TCFlag |= 0x08;
			}
			edc = edc^*Buff;
			Lenth++;
		}

		if((Flag & 0x80) == 0x00)
		{	/*	no TD  */
			for(i = 0; i < x; i++)
			{
				*++Buff = icc_RecvByte();
				if(gl_CardInfo.k_IccComErr)
				{
					if(gl_CardInfo.k_IccErrTimeOut)
					{
						if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
						{
							return ATR_TIMEOUT_SHB;
						}
						else
						{
							return ATR_TIMEOUT;
						}
					}
					else
					{
						return ATR_PARERR;
					}
				}
				edc = edc ^ (*Buff);
			}

			Lenth = Lenth + x;

			if(T)
			{  /* 如果为T<>0协议卡片一定有TCK  */

				*++Buff = icc_RecvByte();
				if(gl_CardInfo.k_IccComErr)
				{
					if(gl_CardInfo.k_IccErrTimeOut)
					{
						if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
						{
							return ATR_TIMEOUT_SHB;
						}
						else
						{
							return ATR_TIMEOUT;
						}
					}
					else
					{
						return ATR_PARERR;
					}
				}

				TCK = *Buff;
				if(edc != TCK)
				{
					return ATR_TCKERR;
				}
				Lenth++;
			}

			*Lenth_Point = Lenth;
			// 关闭定时器检测
			gl_char_wait_etu = 0;
			gl_total_atr_etu = 0;
			icc_close_etu_timer();
			icc_disable_inter_timer();

			gl_CardSlotInfo[gl_CardInfo.current_slot].PPSENABLE = 0;   // 默认禁止PPS协商

			/* 支持TA1 =  '11’'12'	'13'值的ATR  */
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag & 0x02)
			{
				//EMV模式，存在TA1,TA2存在且b5=0且TA1必须在0x11~0x13范围
				//         存在TA1,TA2不存在,采用F=372 D=1，不协商
				//         不存在TA1,采用F=372 D=1
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
				{
					y = gl_CardSlotInfo[gl_CardInfo.current_slot].TA2;
					if((y & 0x10) || ((y & 0x0F) != (gl_CardSlotInfo[gl_CardInfo.current_slot].TD1 & 0x0F)))
					{
						//  TA2的Bit5不为0，或者低四位不为TD1的低四位
						return ATR_TA2ERR;
					}
				}

				if(gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag & 0x01)
				{
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
					{
						if((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 < 0x11) || (gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 > 0x13))
						{
							return ATR_TA1ERR;
						}
					}
				}
			}
			if(!(gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag & 0x02))
			{
				// TA2不存在，默认按照TA1 = 11处理。
				if((gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == ISOMODE) && (gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag & 0x01))
				{
					// ISO模式下允许PPS协商
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].D == 1 && gl_CardSlotInfo[gl_CardInfo.current_slot].ETU == 372)
					{
						gl_CardSlotInfo[gl_CardInfo.current_slot].PPSENABLE = 1;
					}
				}
				//社保卡TA2，TA1应采用协商模式，且TA1存在
				if((gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
					&& (gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag & 0x01))
				{
					// 社保模式下允许PPS协商
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].D == 1 && gl_CardSlotInfo[gl_CardInfo.current_slot].ETU == 372)
					{
						gl_CardSlotInfo[gl_CardInfo.current_slot].PPSENABLE = 1;
					}
				}
			}
			else if(gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag & 0x01)
			{
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
				{
					 //TA2存在情况下社保要求
	                Fi = (gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) >> 4;
	                Di = gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0x0F;
	                ETU = F_Table[Fi]/D_Table[Di];
					gl_CardSlotInfo[gl_CardInfo.current_slot].D = D_Table[Di];
					gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = ETU;
				}
				else
				{
				    //TA2存在情况下
	                Fi = (gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) >> 4;
					if(Fi == 0x07 || Fi == 0x08 || Fi > 0x0D)
					{
						// ISO7816 F范围0~6 9~D
						return ATR_TA1ERR;
					}
	                Di = gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0x0F;
					if(Di == 0x00 || Di == 0x07 || Di > 0x09)
					{
						// ISO7816 D范围1~6  8~9
						return ATR_TA1ERR;
					}
	                ETU = F_Table[Fi]/D_Table[Di];
					gl_CardSlotInfo[gl_CardInfo.current_slot].D = D_Table[Di];
					gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = ETU;
				}
			}

			/*	冷复位下无TB1值或TB1!=0时则进行热复位	*/
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].ResetStatus == 0x00)
			{
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
				{
					if(((gl_CardSlotInfo[gl_CardInfo.current_slot].TBFlag & 0x01) == 0x00) || (gl_CardSlotInfo[gl_CardInfo.current_slot].TB1 != 0x00))
					{
						return ATR_TB1ERR;
					}
				}
			}


			if((gl_CardSlotInfo[gl_CardInfo.current_slot].TDFlag & 0x01) == 0x01)
			{
				//社保卡返回TD1则应该返回低位为0
				if(((gl_CardSlotInfo[gl_CardInfo.current_slot].TD1 & 0x0f) != 0x00)
					&& (gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE))
				{
					return ATR_TD1ERR;
				}
				/* TD1存在 */
				if(((gl_CardSlotInfo[gl_CardInfo.current_slot].TD1 & 0x0f) > 1) && (gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE))
				{
					return ATR_TD1ERR;
				}

				if(gl_CardSlotInfo[gl_CardInfo.current_slot].TD1 & 0x0f)
				{
					// TD1的T=1  TD2的T!=1，则错误
					if((gl_CardSlotInfo[gl_CardInfo.current_slot].TDFlag&0x02) && ((gl_CardSlotInfo[gl_CardInfo.current_slot].TD2&0x0f) != 0x01))
					{
						if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
						{
							return ATR_TD2ERR;
						}
					}
				}
				else
				{
					if(((gl_CardSlotInfo[gl_CardInfo.current_slot].TDFlag & 0x02)
						&&(gl_CardSlotInfo[gl_CardInfo.current_slot].TD2 & 0x0f) != 0x0f)
						&& (gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE))
					{
						return ATR_TD2ERR;
					}
					//TD1的T=0
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].TDFlag & 0x02)
					{
						// TD2的 T!=1且T!=E则错误,因为送TD2的高半字节为任意值且低半字节的值为‘1’或‘E’
						if(((gl_CardSlotInfo[gl_CardInfo.current_slot].TD2 & 0x0F) != 0x01) && ((gl_CardSlotInfo[gl_CardInfo.current_slot].TD2 & 0x0F) != 0x0E))
						{
							if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
							{
								return ATR_TD2ERR;
							}
						}
						else
						{
							if(((gl_CardSlotInfo[gl_CardInfo.current_slot].TD2 & 0x0F) == 0x0E)
								&& ((gl_CardSlotInfo[gl_CardInfo.current_slot].TD1 & 0x0F) != 0x00))
							{
								if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
								{
									return ATR_TD2ERR;
								}
							}
						}
					}
				}
			}

			/* 无效的TB2字节 */
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
			{
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].TBFlag & 0x02)
				{
					return ATR_TB2ERR;
				}

				/* 无效的TC2字节 */
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].TCFlag & 0x02)
				{
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].TC2 != 0x0A)
					{
						return ATR_TC2ERR;
					}
				}
			}
			//社保卡工作等待时间
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
			{
				if((gl_CardSlotInfo[gl_CardInfo.current_slot].TCFlag & 0x02) == 0)
				{
					gl_CardSlotInfo[gl_CardInfo.current_slot].TC2 = 0x0A;
				}
				y = gl_CardSlotInfo[gl_CardInfo.current_slot].D;
				atr_x = gl_CardSlotInfo[gl_CardInfo.current_slot].TC2;
				gl_CardSlotInfo[gl_CardInfo.current_slot].WWT = (960 * y * atr_x);
				atr_x = gl_CardSlotInfo[gl_CardInfo.current_slot].WWT;
				gl_CardSlotInfo[gl_CardInfo.current_slot].WWT = atr_x + 1 + 113;
			}
			else
			{
                gl_CardSlotInfo[gl_CardInfo.current_slot].TC2 = 0x0A;
                z = gl_CardSlotInfo[gl_CardInfo.current_slot].TC2;
				y = gl_CardSlotInfo[gl_CardInfo.current_slot].D;
				gl_CardSlotInfo[gl_CardInfo.current_slot].WWT = (960 * y * z) + (480 * y);
				gl_CardSlotInfo[gl_CardInfo.current_slot].WWT += 1;
			}


			//社保卡应回TA3
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
			{
				if((gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag & 0x04) != 0x00)
				{
					if((gl_CardSlotInfo[gl_CardInfo.current_slot].TA3 & 0x3c) != 0x00)
					{
						return ATR_TA3ERR;
					}
				}
				else
				{
					if(T == 15)
					{
						gl_CardSlotInfo[gl_CardInfo.current_slot].TA3 = 0x01;
					}
				}
				atr_x = gl_CardSlotInfo[gl_CardInfo.current_slot].TA3;
				gl_CardSlotInfo[gl_CardInfo.current_slot].XI = (atr_x & 0xc0)>>6;
				gl_CardSlotInfo[gl_CardInfo.current_slot].UI = atr_x & 0x03;
			}
			else
			{
				/*	无效的TA3字节 */
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag & 0x04)
				{
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA3 < 0x10)
					{
						return ATR_TA3ERR;
					}

					if(gl_CardSlotInfo[gl_CardInfo.current_slot].TA3 == 0xFF)
					{
						return ATR_TA3ERR;
					}
				}
				else
				{
					gl_CardSlotInfo[gl_CardInfo.current_slot].TA3 = 0x20;
				}
				y = gl_CardSlotInfo[gl_CardInfo.current_slot].TA3;
				gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC = y;
			}

			/*	无效的TB3字节 */
			if(T == 1)
			{
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
				{
					if((gl_CardSlotInfo[gl_CardInfo.current_slot].TBFlag & 0x04) == 0x00)
					{
						return ATR_TB3ERR;
					}
				}

				CWI = gl_CardSlotInfo[gl_CardInfo.current_slot].TB3 & 0x0F;
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
				{
					if(CWI > 5)
					{
						return ATR_TB3ERR;
					}
				}
				// 计算CWT时间CWT=2^CWI+11+4+1
				gl_CardSlotInfo[gl_CardInfo.current_slot].CWT = ((uint)0x01 << CWI) + 11 + 5;

				BWI = (gl_CardSlotInfo[gl_CardInfo.current_slot].TB3 >> 4)&0x0f;
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
				{
					if(BWI > 4)
					{
						return ATR_TB3ERR;
					}
				}
				// 计算BWT时间
				// BWT=[2^BWI ×960 ×372D/F]+11+D*960      当Fi!=372时 
				// BWT=[2^BWI ×960 + 11] + D*960           当Fi=372时
				k = (((uint)1 << BWI) * 960 *gl_CardSlotInfo[gl_CardInfo.current_slot].D) + 11;
				k += (gl_CardSlotInfo[gl_CardInfo.current_slot].D * 960);
                k += 1;
				gl_CardSlotInfo[gl_CardInfo.current_slot].BWT = k;
				gl_CardSlotInfo[gl_CardInfo.current_slot].BWT_TEMP = k;

				// 适当延长发送字符间隔
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].TC1 == 0xFF)
				{
					N = 0;
				}
				else
				{
					N = gl_CardSlotInfo[gl_CardInfo.current_slot].TC1 + 1;
				}
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
				{
					j = (ushort)0x01 << CWI;
					if(j <= N)
					{
						return ATR_TB3ERR;
					}
				}
			}

			/* 无效的TC3值	*/
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].TCFlag & 0x04)
			{
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == EMVMODE)
				{
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].TC3)
					{
						return ATR_TC3ERR;
					}
				}
			}

			//除TS外最大长度为32个字节
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
			{
				if(TCK != 0)
				{
					if(T != 15)
					{
						return ATR_TCKERR;
					}
				}
				if((Lenth - 1) > 32)
				{
					return ATR_LENGTHERR;
				}
			}
			return ICC_SUCCESS;
		}

		if(Flag & 0x80)
		{   //  TD1  TD2
			*++Buff = icc_RecvByte();
			if(gl_CardInfo.k_IccComErr)
			{
				if(gl_CardInfo.k_IccErrTimeOut)
				{
					if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
					{
						return ATR_TIMEOUT_SHB;
					}
					else
					{
						return ATR_TIMEOUT;
					}
				}
				else
				{
					return ATR_PARERR;
				}
			}

			if(i == 1)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TD1 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TDFlag |= 0x01;
				T = *Buff&0x0f;
			}
			else if(i == 2)
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TD2 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TDFlag |= 0x02;
				//如果TD2低字节等于E,则也要接收TCK
				T |= *Buff&0x0f;
			}
			else
			{
				gl_CardSlotInfo[gl_CardInfo.current_slot].TD3 = *Buff;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TDFlag |= 0x04;
				T |= *Buff&0x0f;
			}

			edc = edc^*Buff;
			Lenth++;
			Flag = *Buff;
		}
		i++;
	}
}

int  icc_ColdReset(uchar *ATR)
{
	int Ret = 0;

	gl_CardInfo.k_total_timeover = 0;
	gl_CardInfo.k_timeover = 0;
	gl_CardInfo.k_IccComErr = 0;
	gl_CardInfo.k_IccErrTimeOut = 0;

	gl_char_wait_etu = 0;
	gl_total_atr_etu = 0;

	icc_init_etu_timer();
	icc_init_inter_timer();
	// 事先释放所有触点
	icc_ClrVCC();
	icc_SetIOOutput();
	icc_ClrRST();
	icc_ClrIO();
	icc_close_clk();
	icc_SetVCC(gl_CardSlotInfo[gl_CardInfo.current_slot].VolMode);
	icc_SetIO();
	icc_SetIOInput();
	icc_open_clk();        // 打开时钟
	icc_delay_clock(42500); //时钟=3.104M 延时42500CLK*(1/3.104M)= 13.69ms

	// 等待IC在42000CLK内回送TS字节
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
	{
		gl_CardInfo.k_Ic_CharWait_TimeCount = 9600;
	}
	else
	{
		gl_CardInfo.k_Ic_CharWait_TimeCount = 10081;
	}
    gl_CardInfo.k_RecvFirst = gl_CardInfo.k_Ic_CharWait_TimeCount - 11;

	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE) {
		//社保40000*0.322=12.88ms
		gl_CardInfo.k_Ic_ATR_TotalTimeCount = 40000;
    } else if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == CITYUNIONMODE) {
        //目前看1张卡复位后IO应答时间需要22ms,另外一张只要5ms
		gl_CardInfo.k_Ic_ATR_TotalTimeCount = 90000;        
    } else {
		gl_CardInfo.k_Ic_ATR_TotalTimeCount = 42000 + 25;
	}
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
	{
    	gl_CardInfo.k_IC_TotalATRTime = 19200;
	}
	else
	{
		gl_CardInfo.k_IC_TotalATRTime = 20161;
	}
	icc_SetRST();                   // 将RST信号置高
    if ( gl_CardInfo.k_Ic_ATR_TotalTimeCount > 65564) {
        gl_total_atr_etu = 2;
        icc_start_etu_timer(gl_CardInfo.k_Ic_ATR_TotalTimeCount/2);      // 等待TS字节时间
    } else {
        gl_total_atr_etu = 1;
        icc_start_etu_timer(gl_CardInfo.k_Ic_ATR_TotalTimeCount);      // 等待TS字节时间
    }

	icc_DisableOtherInt();
	Ret = icc_GetAtr(ATR);
    icc_EnableOtherInt();
	if(Ret == ICC_SUCCESS)
	{
		icc_adjustTiming();
	}
	icc_close_etu_timer();
	icc_disable_inter_timer();
	gl_total_atr_etu = 0;
	gl_char_wait_etu = 0;

	return Ret;
}

int  icc_WarmReset(uchar *ATR)
{
	int Ret;
	memset((uchar*)&gl_CardSlotInfo[gl_CardInfo.current_slot].TS, 0x00, 23);
	gl_CardInfo.k_total_timeover = 0;
	gl_CardInfo.k_timeover = 0;
	gl_CardInfo.k_IccComErr = 0;
	gl_CardInfo.k_IccErrTimeOut = 0;

	gl_char_wait_etu = 0;
	gl_total_atr_etu = 0;

	icc_init_etu_timer();
	icc_init_inter_timer();
	icc_ClrRST();         // 将RST置低，发起热复位操作
	icc_SetIOInput();
	gl_CardInfo.k_Ic_ATR_TotalTimeCount = 42500;  // 延时42500CLK
	icc_delay_clock(gl_CardInfo.k_Ic_ATR_TotalTimeCount);

	// 等待IC在42000CLK内回送TS字节
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
	{
		gl_CardInfo.k_Ic_CharWait_TimeCount = 9600;
	}
	else
	{
		gl_CardInfo.k_Ic_CharWait_TimeCount = 10081;
	}
    gl_CardInfo.k_RecvFirst = gl_CardInfo.k_Ic_CharWait_TimeCount - 11;
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE) {
		//社保40000*0.322=12.88ms
		gl_CardInfo.k_Ic_ATR_TotalTimeCount = 40000;
    } else if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == CITYUNIONMODE) {
        //目前看1张卡复位后IO应答时间需要22ms,另外一张只要5ms
		gl_CardInfo.k_Ic_ATR_TotalTimeCount = 90000;        
    } else {
		gl_CardInfo.k_Ic_ATR_TotalTimeCount = 42000 + 25;
	}
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
	{
    	gl_CardInfo.k_IC_TotalATRTime = 19200;
	}
	else
	{
		gl_CardInfo.k_IC_TotalATRTime = 20161;
	}
	icc_SetRST();                                          // 将RST信号置高
    if ( gl_CardInfo.k_Ic_ATR_TotalTimeCount > 65564) {
        gl_total_atr_etu = 2;
        icc_start_etu_timer(gl_CardInfo.k_Ic_ATR_TotalTimeCount/2);      // 等待TS字节时间
    } else {
        gl_total_atr_etu = 1;
        icc_start_etu_timer(gl_CardInfo.k_Ic_ATR_TotalTimeCount);      // 等待TS字节时间
    }

	icc_DisableOtherInt();
	Ret = icc_GetAtr(ATR);
    icc_EnableOtherInt();
	if(Ret == ICC_SUCCESS)
	{
		icc_adjustTiming();
	}
	icc_close_etu_timer();
	icc_disable_inter_timer();
	gl_total_atr_etu = 0;
	gl_char_wait_etu = 0;
	return Ret;
}

static int icc_t0Exchange(int len, uchar *inbuff, uchar *outbuff, uchar *rslt)
{
	uint    i,len1,len2;
	uint    reclen = 0;
	int		Ret;
	uchar	tempsw1,tempsw2;
	uchar	*ptr1,*ptr2,status[4],cmdbuf[5];
	uchar	cmd_case,is_rece_send;
	uchar	FirstByte_Is_Status;
	uchar	Change_Status_Word;
	gl_CardInfo.Parity_Check_Enable = 0x01;
	// 设置字符最大间隔时间
	gl_CardInfo.k_Ic_CharWait_TimeCount = gl_CardSlotInfo[gl_CardInfo.current_slot].WWT;
    gl_CardInfo.k_RecvFirst = gl_CardInfo.k_Ic_CharWait_TimeCount - 11;

	cmd_case = *rslt;  /* 命令CASE号 1，2，3，4*/

	FirstByte_Is_Status = 1; /* 第一个字节为状态字节 ：1-是，0-非 */
	Change_Status_Word = 0;  /* 更新返回状态字节标志：1-更改 */

	gl_CardInfo.k_timeover = 0x00;
	is_rece_send = 0;        /* 当前处于数据包发送与接收状态：0=接收；1=发送 */
	if(cmd_case == 0x01)
	{
		is_rece_send = 0;
	}
	else if(cmd_case == 0x02)
	{
		is_rece_send = 0;
	}
	else if(cmd_case == 0x03)
	{
		is_rece_send = 1;
	}
	else
	{
		is_rece_send = 1;
	}

	/* 置命令头标  */
	cmdbuf[0] = inbuff[0];
	cmdbuf[1] = inbuff[1];
	cmdbuf[2] = inbuff[2];
	cmdbuf[3] = inbuff[3];
	cmdbuf[4] = inbuff[4];
	if(cmd_case == 0x01)
	{
		cmdbuf[4] = 0x00;
	}
	status[0] = 0xff;
	status[1] = 0xff;
	status[2] = 0xff;//备份INS OR ~INS
	status[3] = 0xff;//标志INS是否接收到2次。是=0；

	reclen = 0;
	ptr1 = inbuff+5;
	ptr2 = outbuff + 2;

	/*	读IC卡0时出现拨卡操作时报错  */
	tempsw1 = 0xFF;
	tempsw2 = 0xFF;


	while(1)
	{
flg1:
		icc_init_etu_timer();
		icc_init_inter_timer();
		gl_char_wait_etu = 0;
		gl_total_atr_etu = 0;

		icc_delay_etu(gl_CardSlotInfo[gl_CardInfo.current_slot].ETU, 8);
		len1 = (uint)cmdbuf[4];  /*  发送数据长度  */
		len2 = (uint)cmdbuf[4];  /*  接收数据长度  */
//		if(len2 == 0)
//		{
//			//如果接收长度00,则要求收256 yehf 2011-10-18 modify
//			len2 = 256;
//		}
		Ret = icc_SendByte(cmdbuf[0]);   /* 发送CLA  */
		if(Ret)
		{
			return T0_MORESENDERR;
		}

		Ret = icc_SendByte(cmdbuf[1]);   /* 发送INS  */
		if(Ret)
		{
			return T0_MORESENDERR;
		}

		Ret = icc_SendByte(cmdbuf[2]);   /* 发送P1   */
		if(Ret)
		{
			return T0_MORESENDERR;
		}

		Ret = icc_SendByte(cmdbuf[3]);   /*  发送P2  */
		if(Ret)
		{
			return T0_MORESENDERR;
		}

		gl_CardInfo.k_Ic_LastSendByte = 1;
		Ret = icc_SendByte(cmdbuf[4]);   /* 发送P3   */
		gl_CardInfo.k_Ic_LastSendByte = 0;
		if(Ret)
		{
			return T0_MORESENDERR;
		}

flg2:
		status[0] = icc_RecvByte(); /* 读取一过程字节 */
		if(gl_CardInfo.k_IccComErr)
		{
			if(gl_CardInfo.k_IccErrPar)
			{
				return T0_PARERR;
			}
			else
			{
				return T0_TIMEOUT;
			}
		}

		if(status[0] == cmdbuf[1])
		{   /*  过程字节等于INS----发送或接收其余的数据包	*/
			if(is_rece_send)
			{
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
				{
					if(status[3] != 0xff)
					{
						status[2] = status[0];
						goto flg2;
					}

				}

				/* 发送数据包  */
				icc_delay_etu(gl_CardSlotInfo[gl_CardInfo.current_slot].ETU, 8);
				for(i = 0; i < len1; i++)
				{
					if(i == (len1-1))
					{
						gl_CardInfo.k_Ic_LastSendByte = 1;
					}
					Ret = icc_SendByte(*ptr1);
					gl_CardInfo.k_Ic_LastSendByte = 0;
					if(Ret)
					{
						return T0_MORESENDERR;
					}
					ptr1++;
				}

				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
				{
					status[3] = 0x00;
				}

				FirstByte_Is_Status = 0;
				goto flg2;	      /* 等待下一个过程字节  */
			}
			else
			{  /*  接收数据包  */
				if(len2 == 0)
				{
					//如果接收长度00,则要求收256 yehf 2011-10-18 modify
					len2 = 256;
				}
				reclen += len2;
				if(reclen > 256)
				{
					return (-ICC_RESPERR);
				}
				for(i = 0; i < len2; len2--)
				{
					*ptr2 = icc_RecvByte();
					if(gl_CardInfo.k_IccComErr)
					{
						if(gl_CardInfo.k_IccErrPar)
						{
							return T0_PARERR;
						}
						else
						{
							return T0_TIMEOUT;
						}
					}
					ptr2++;
				}
				FirstByte_Is_Status = 0;
				goto flg2;
			}
		}
		else if(status[0] == (uchar)(~cmdbuf[1]))
		{	/* 等于INS的补码  按收或发送下一个数据  */
			if(is_rece_send)
			{
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
				{
					if(len1 == 0)
					{
						status[2] = status[0];
						goto flg2;
					}
				}
				/* 发送一字节数据	*/
				icc_delay_etu(gl_CardSlotInfo[gl_CardInfo.current_slot].ETU, 8);
				gl_CardInfo.k_Ic_LastSendByte = 1;
				Ret = icc_SendByte(*(ptr1));
				gl_CardInfo.k_Ic_LastSendByte = 0;
				if(Ret)
				{
					return T0_MORESENDERR;
				}
				ptr1++;
				len1--;
				FirstByte_Is_Status = 0;
				goto flg2;
			}
			else
			{/*  接收下一字节数据  */
				*ptr2 = icc_RecvByte();
				if(gl_CardInfo.k_IccComErr)
				{
					if(gl_CardInfo.k_IccErrPar)
					{
						return T0_PARERR;
					}
					else
					{
						return T0_TIMEOUT;
					}
				}
				ptr2++;
				reclen++;
				if(len2 == 0)
				{
					//如果接收长度00,则要求收256 yehf 2011-10-18 modify
					len2 = 256;
				}
				len2--;
				FirstByte_Is_Status = 0;

				if(reclen > 256)
				{
					return (-ICC_RESPERR);
				}
				goto flg2;
			}
		}
		else if(status[0] == 0x60)
		{
			FirstByte_Is_Status = 0;
			goto flg2;
		}

		if(((status[0]&0xf0)!= 0x60)&&((status[0]&0xf0) != 0x90))
		{  /* 收到错误的过程或状态字节后，释放触点 */
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode != SHBMODE)
			{
				return T0_INVALIDSW;
			}
		}

		status[1] = icc_RecvByte();
		if(gl_CardInfo.k_IccComErr)
		{
			if(gl_CardInfo.k_IccErrPar)
			{
				return T0_PARERR;
			}
			else
			{
				return T0_TIMEOUT;
			}
		}

		/* 自动响应  */
		if((status[0] == 0x61) && ((cmd_case == 0x02) || (cmd_case == 0x04)))
		{
			cmdbuf[0] = 0x00;
			cmdbuf[1] = 0xc0;
			cmdbuf[2] = 0x00;
			cmdbuf[3] = 0x00;
			cmdbuf[4] = status[1];
			is_rece_send = 0;  /* 后面的操作只能从IC卡接收数据包 */
			FirstByte_Is_Status = 0;
			if(cmd_case == 0x04)
			{
				cmd_case = 0x02;
			}
			goto flg1;		 /* 发送下个命令头标  */
		}

		if(status[0] == 0x6C)
		{
			cmdbuf[4] = status[1];
			is_rece_send = 0;
			FirstByte_Is_Status = 0;
			if(cmd_case == 0x04)
			{
				cmd_case = 0x02;
			}
			goto flg1;
		}

		if((cmd_case == 0x04) && (FirstByte_Is_Status == 0))
		{
			if((status[0] == 0x62)
				||(status[0] == 0x63)
				||(((status[0]&0xf0) == 0x90) && ((status[0]!=0x90) || (status[1] != 0x00))))
			{
				cmdbuf[0] = 0x00;
				cmdbuf[1] = 0xc0;
				cmdbuf[2] = 0x00;
				cmdbuf[3] = 0x00;
				cmdbuf[4] = 0x00;
				is_rece_send = 0;
				Change_Status_Word = 1;

				cmd_case = 0x02; /* 修改CASE状态防止重发 */

				tempsw1 = status[0];
				tempsw2 = status[1];
				goto flg1;
			}
		}

		if(Change_Status_Word)
		{
			*rslt = tempsw1;
			*(rslt+1) = tempsw2;
		}
		else
		{
			if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
			{
				if(status[2] != 0xff)
				{
					*rslt = status[2];
					*(rslt + 1) = status[0];
				}
				else
				{
					*rslt = status[0];
					*(rslt + 1) = status[1];
				}
			}
			else
			{
				*rslt = status[0];
				*(rslt + 1) = status[1];
			}
		}
		*outbuff = reclen/256;
		*(outbuff + 1) = reclen%256; /* 保存接收的数据长度  */

		return ICC_SUCCESS;
	}
}

static int  icc_t1send(uchar *Inbuf)
{
	uchar	edc = 0x00;
	//int Ret = 0;
	uint	i;
        //uint len;

	//len = Inbuf[2]+3;   /*	发送总长NAD+PCB+LEN+（LEN个DATA）；*/
	edc = 0;			/*	计算命令与数据元的校码EDC */
LABLE(D_T1_SEND);
	gl_CardInfo.k_Ic_CharWait_TimeCount = gl_CardSlotInfo[gl_CardInfo.current_slot].BWT;
	gl_CardInfo.k_RecvFirst = gl_CardInfo.k_Ic_CharWait_TimeCount - 11;

	icc_init_etu_timer();
	icc_init_inter_timer();
	gl_char_wait_etu = 0;
	gl_total_atr_etu = 0;

	icc_delay_etu(gl_CardSlotInfo[gl_CardInfo.current_slot].ETU, 22);
	icc_SendByte(Inbuf[0]);  // 发送NAD
	edc ^= Inbuf[0];

	icc_SendByte(Inbuf[1]);  // 发送PCB
	edc ^= Inbuf[1];

	icc_SendByte(Inbuf[2]);  // 发送LEN
	edc ^= Inbuf[2];

	for(i = 0; i < Inbuf[2]; i++)
	{
		icc_SendByte(Inbuf[3+i]);  // 发送LEN长度的数据域
		edc ^= Inbuf[3+i];
	}

	gl_CardInfo.k_Ic_LastSendByte = 1;
	icc_SendByte(edc);   // 发送EDC字节
	gl_CardInfo.k_Ic_LastSendByte = 0;
	return ICC_SUCCESS;
}

static int  icc_t1recv(uchar *Outbuf)
{
	uchar	i,ch,edc;
	uchar	Parity_Error_Flag;
	uchar	r_nad,r_pcb,r_len,r_inf;
	uchar	r_len_temp;
LABLE(D_T1_RECV);
	edc = 0;
	Parity_Error_Flag = 0;
	// 2010-01-17  yehf调试需要gl_CardInfo.k_Ic_CharWait_TimeCount-8处理
	gl_CardInfo.k_Ic_CharWait_TimeCount = gl_CardSlotInfo[gl_CardInfo.current_slot].CWT;
    gl_CardInfo.k_RecvFirst = gl_CardInfo.k_Ic_CharWait_TimeCount - 11;
	ch = icc_RecvByte();				// NAD
	if(gl_CardInfo.k_IccErrTimeOut)
		return T1_BWTERR;
	if(gl_CardInfo.k_IccErrPar)
		Parity_Error_Flag = 0x01;

	edc = edc ^ ch;
	*Outbuf++ = ch;
	r_nad = ch;

	ch = icc_RecvByte();				// PCB
	if(gl_CardInfo.k_IccErrTimeOut)
		return T1_CWTERR;
	if(gl_CardInfo.k_IccErrPar)
		Parity_Error_Flag = 0x01;

	edc = edc ^ ch;
	*Outbuf++ = ch;
	r_pcb = ch;

	ch = icc_RecvByte();				//LEN
	if(gl_CardInfo.k_IccErrTimeOut)
		return T1_CWTERR;
	if(gl_CardInfo.k_IccErrPar)
		Parity_Error_Flag = 0x01;

	edc = edc ^ ch;
	*Outbuf++ = ch;
	r_len = ch;
	r_len_temp = ch;
	//20140106  yehf修改
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
	{
		if((r_pcb&0xC0) == 0x80)
		{
			if(ch)
				r_len_temp = 0;
		}
		else if((r_pcb&0xC0) == 0xC0)
		{
			if(ch > 1)
				r_len_temp = 1;
		}
	}

	for(i = 0; i < r_len_temp; i++)
	{
		ch = icc_RecvByte();				// Data[n]
		if(gl_CardInfo.k_IccErrTimeOut)
			return T1_CWTERR;
		if(gl_CardInfo.k_IccErrPar)
			Parity_Error_Flag = 0x01;
		edc = edc ^ ch;
		*Outbuf++ = ch;
	}

	r_inf = ch;

	ch = icc_RecvByte();				// EDC
	if(gl_CardInfo.k_IccErrTimeOut)
		return T1_CWTERR;
	if(gl_CardInfo.k_IccErrPar)
		Parity_Error_Flag = 0x01;

	icc_close_etu_timer();
	icc_disable_inter_timer();
	gl_char_wait_etu = 0;
	gl_total_atr_etu = 0;

	*Outbuf = ch;

	if(ch != edc)
		return T1_EDCERR;
	if(Parity_Error_Flag)
		return T1_PARITYERR;

	if(r_nad)
		return T1_INVALIDBLOCK;   //nad!=0
	if((r_pcb&0x80) == 0)
	{ // I_BLOCK
		if(r_len == 0xFF)
			return T1_INVALIDBLOCK;
	}
	else if((r_pcb&0xC0) == 0x80)
	{ // R_BLOCK
		if(r_len)
			return T1_INVALIDBLOCK;

		if((r_pcb&0x20) == 0x20)
			return T1_INVALIDBLOCK; //rblock bit6=1
	}
	else
	{ // S_BLOCK
		if(r_pcb == 0xC1)
		{
			if((r_inf < 0x10) || (r_inf > 0xFE))
				return T1_INVALIDBLOCK;
		}
		else if(r_pcb == 0xC2)  // abort request
		    //不响应
			return T1_ABORTERR;

		else if(r_pcb == 0xE3) return T1_INVALIDBLOCK; //S(WTX response)
		else if(r_pcb == 0xE2) return T1_INVALIDBLOCK; //S(ABORT response)
		else if(r_pcb == 0xE0) return T1_INVALIDBLOCK; //S(RESYNCH response)

		if(r_len != 1)
			return T1_INVALIDBLOCK;

		if((r_pcb&0x1F) > 4)    //10_01_12yehf 添加VCC错误
			return T1_INVALIDBLOCK;
	}
	return ICC_SUCCESS;
}

#if 11
//06052014 chenf T1
static int icc_t1Exchange(uchar *inbuf,uchar *outbuf)
{
	uchar	sour[300],oldsour[300],respbuf[300],oldrblock[5];
	uchar	OLDIFSC;
	uchar	PCB,tmpch = 0,tmpch1;
	uint	len,len1,i;

	uint	total_send_len,sent_len,remain_len;
	uchar	re_send_i_times,re_send_r_times;
	uint	WTX = 0;
	int		Ret = 0;
	OLDIFSC = gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC;
	gl_CardInfo.Parity_Check_Enable = 0x00;
//    uchar   rblockmax = 1;//重发r块最大次数

start_t1_exchange:

	re_send_i_times = 0;
	re_send_r_times = 0;
	total_send_len = inbuf[0]*256 + inbuf[1];
	remain_len = inbuf[0]*256 + inbuf[1];
	sent_len = 0;

	outbuf[0] = 0x00;
	outbuf[1] = 0x00;
	outbuf[2] = 0x00;
	len = 0;
	len1 = 0;

	gl_CardSlotInfo[gl_CardInfo.current_slot].sw1 = 0xFF;
	gl_CardSlotInfo[gl_CardInfo.current_slot].sw2 = 0xFF;

	memset(respbuf, 0x00, sizeof(respbuf));
	memset(sour, 0x00, sizeof(sour));
	memset(oldsour, 0x00, sizeof(oldsour));
	memset(oldrblock, 0x00, sizeof(oldrblock));

send_remain_data:
	if(remain_len == 0)
	{
		return 0xFF;
	}

	if(remain_len > gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC)
	{   /*  如果数据包长大于IFSC时，则将包进行切分后再发送*/
		gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB |= 0x20;   /*给I字组加上链接符 */
		memcpy(&sour[3],&inbuf[2+sent_len],gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC);
		/*设定发送数据I_BLOCK  */
		sour[2] = gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC;  /* 设定I_block包数据长度*/
	}
	else
	{   /*否则只用一包就可完全发出  */
		gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB &= 0xDF;
		memcpy(&sour[3],&inbuf[2+sent_len],remain_len);
		sour[2] = remain_len;
	}
	sent_len += sour[2]; /* 设定已发送数据长度	*/
	remain_len = total_send_len - sent_len;
	sour[1] = gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB; /* 设定发送包头的PCB协议控制字节*/

	sour[0] = 0x00;
	memcpy(oldsour, sour, sour[2]+3);/* 备份发送的I字组 */
	gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB = sour[1]^0x40;
	re_send_i_times = 0;

re_send_i_block:
//    rblockmax = 1;
	if(re_send_i_times > 2)
	{
		s_DelayUs(100);
		return (-ICC_COMMERR);
	}
	re_send_i_times++;
	re_send_r_times = 0;
	sour[0] = 0x00;

	icc_t1send(sour);

receive_one_block:
	Ret = icc_t1recv(respbuf);
	// 将BWT时间调整到原来的时间
    i = gl_CardSlotInfo[gl_CardInfo.current_slot].BWT_TEMP;
	gl_CardSlotInfo[gl_CardInfo.current_slot].BWT = i;

	// 接收一帧数据后，关闭相关时序检测
	if((Ret == T1_CWTERR)||(Ret == T1_BWTERR)||(Ret == T1_ABORTERR) || (Ret == ERROR))
	{
		s_DelayUs(100);
		return Ret;
	}

	if((Ret == T1_PARITYERR) || (Ret == T1_EDCERR))
	{
        if ( re_send_r_times ) {
            //之前已发过R 当再接收时 重发上一次的R
            sour[0] = oldrblock[0];
            sour[1] = oldrblock[1];
            sour[2] = oldrblock[2];
        }else{
            tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB^0x40;
            /* 序号为正在接收卡片的I字组顺序号（即为上次接收的下个序号）*/
            sour[1] = ((tmpch >> 2) & 0x10) | 0x81; /* 置奇偶/EDC错误 */
            sour[2] = 0;
        }
		goto re_send_r_block;
	}

	if(Ret == T1_INVALIDBLOCK){
        if ( re_send_r_times ) {
            //之前已发过R 当再接收时 重发上一次的R
            sour[0] = oldrblock[0];
            sour[1] = oldrblock[1];
            sour[2] = oldrblock[2];
            goto re_send_r_block;
        }else{
            goto set_rblock_with_othererr;
        }
    }

	PCB = respbuf[1]; /* 读取响应数据包头*/

	if((PCB & 0x80) == 0x00)
	{
		if(remain_len > 0)
		{
			//发送链接块收到I块说明错误
			goto set_rblock_with_othererr;
		}

		if((PCB & 0x40) != (gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB & 0x40))
		{/*  收到的I字组序列号不同于先前收到的I字组序列号
		时表确认接收正确，并保存接收的数据到OUTBUF中。*/
			re_send_r_times = 0;

			if(respbuf[2] > 0)
			{ /*如果接收数据长度不为零则保存接收数据*/
				len1 = outbuf[0] * 256 + outbuf[1];
				len=len1 + respbuf[2];
				outbuf[0]= len / 256;
				outbuf[1]= len % 256;
				memcpy(&outbuf[len1+2],&respbuf[3],respbuf[2]);
			}

			gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB = PCB;
			/*设置新的I字组顺序号*/
			if((PCB&0x20) == 0x20)
			{ /* 收到有链的I字组 */
				tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB^0x40;
				/* 下个I_Block顺序号*/
				sour[1] = ((tmpch >>2 ) & 0x10) | 0x80;
				sour[2] = 0;
				goto re_send_r_block;
			}
			/* 因正确接收到无链接的I字组则退出*/
			Ret = ICC_SUCCESS;
			goto end_t1_exchange;
		}
		else
		{
            if(re_send_r_times == 0){
                goto set_rblock_with_othererr;
            }
            else{
                //当再次收到错误编号数据数据时(非期望块) 要重发上一次R
                sour[0] = oldrblock[0];
                sour[1] = oldrblock[1];
                sour[2] = oldrblock[2];
//                rblockmax = 2;
				goto re_send_r_block;
            }
		}
	}

	else if((PCB&0XC0) == 0X80)
	{// ?r_block
		tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB;
		if((tmpch&0x40) == ((PCB&0x10) << 2))
		{
			/*	序号等于终端下个要发送的I字组序号 */
			if((PCB&0xef) == 0x80)
			{// r-block with error-free
				if((oldsour[1]&0x20) == 0x20) /* 终端有链 */
				{
					//发送成功
					goto send_remain_data;
				}
				else
				{   // 说明卡发给终端I块,终端没有收到,卡重发R块要求终端应答
                    //1770_L40-41
					tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB^ 0x40;
					tmpch1 = ((tmpch >> 2 ) & 0x10) | 0x82;
					re_send_r_times = 0;
					sour[1] = tmpch1;
					sour[2] = 0;
					goto re_send_r_block;
				}
			}
			else
			{/* 带有错误标识的R字组 */
				if(re_send_r_times == 0)
				{ /* 上一次无R字组 */
//					tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB ^ 0x40;
//					sour[1] = ((tmpch >>2 ) & 0x10) | 0x80;
//					sour[2] = 0;
//					goto re_send_r_block;

                    //卡端发了一些错误R 并且编号为下一个终端要发送的I
                    //(本来正确应该卡端要用I来回应 现在卡端却发送了错误R)
                    goto set_rblock_with_othererr;
				}
				else
				{ /* 修改错误标识 */
//					if((sour[1]&0x0f)!= 0x00)
//					{
//						sour[1] = sour[1]&0xf0;
//						re_send_r_times = 0;
//					}
                    //第二次 发送R 那应该重发上一次的R case1772
                    sour[0] = oldrblock[0];
                    sour[1] = oldrblock[1];
                    sour[2] = oldrblock[2];
//                    rblockmax = 2;
					re_send_r_times = 0; //收到带有错误信息的R 不算无效块
                    goto re_send_r_block;
				}
			}
			/* 重发R字组 */
//			goto re_send_r_block;
		}


		if((tmpch&0x40) != ((PCB&0X10) << 2))
		{
			/* 如果序号等于终端已发送的I字组序号则重发该I字组 */
			if((gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB&0x20) == 0x20)
			{
				//上次接收到链接块,终端发送的R块应答,卡未收到，要求终端重发R块
				goto re_send_r_block;
			}

			if(OLDIFSC != gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC)
			{
				//发送I块时，收到卡的IFSC请求,响应后，又收到R块要求终端按照新的IFSC大小重发I块
				// 此时因为终端发送I块已经确认，因此终端重发I块，I块块号按照新的块号发送
				OLDIFSC = gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB = oldsour[1];
				goto start_t1_exchange;
			}
			memcpy(sour, oldsour, oldsour[2]+3);
			goto re_send_i_block;
		}
		/* 重发R字组 */
		goto re_send_r_block;

	}
	else if((PCB&0XC0) == 0XC0)
	{// ?s_block
		if((PCB & 0x20) == 0X00)
		{/*  如果收到的是IC卡发送的请求信号则根据传递来的设置
		参数修改相应的设定并回送S响应字组给IC卡  */
			sour[1] = 0x20 | respbuf[1] ;
			sour[2] = respbuf[2];
			memcpy(&sour[3], &respbuf[3], sour[2]);
			if((respbuf[1] & 0x1F) == 0x03)
			{	/* 扩充BWT请求	BWT=BWT * respbuf[3]; */
                i = gl_CardSlotInfo[gl_CardInfo.current_slot].BWT;
				gl_CardSlotInfo[gl_CardInfo.current_slot].BWT_TEMP = i;
				if(respbuf[3] != 0)
				{
                    WTX = (gl_CardSlotInfo[gl_CardInfo.current_slot].TB3 >> 4)&0x0f;
                    WTX = (((uint)1 << WTX) * 960 *gl_CardSlotInfo[gl_CardInfo.current_slot].D) + 11;
                    i = (WTX + 960 *gl_CardSlotInfo[gl_CardInfo.current_slot].D) * respbuf[3] + 100;
					gl_CardSlotInfo[gl_CardInfo.current_slot].BWT = i;
				}
			}
			else if ((respbuf[1] & 0x1f) == 0x01)
			{/* 信息字段容量请求*/
				gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC = respbuf[3];
			}

			else if((respbuf[1]&0x1f) == 0x00)
			{ /*  同步请求	*/

			}
			sour[0] = 0x00;  /* 回送S响应字组给IC卡*/

			icc_t1send(sour);
			re_send_r_times = 0;

			goto receive_one_block;
		}
		else
		{ /* 接收的是IC卡发送的S响应信号*/
			if((sour[1]&0xe0) == 0xc0)
			{/*  如果终端发送是请求信息时则根据响应设置相应的参数*/
				re_send_r_times = 0;

				if ((respbuf[1] & 0x1f) == 0x01)
				{
					gl_CardSlotInfo[gl_CardInfo.current_slot].IFSD = respbuf[3];
				}
				else if((respbuf[1] & 0x1f) == 0x00)
				{	 //  Reset or deactivation
					gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB = 0x00;
					gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB = 0x40;
					goto start_t1_exchange;
				}
				goto end_t1_exchange;
			}
            if(re_send_r_times){
                //当再次收到错误编号数据数据时(非期望块) 要重发上一次R
                sour[0] = oldrblock[0];
                sour[1] = oldrblock[1];
                sour[2] = oldrblock[2];
//                rblockmax = 2;
                goto re_send_r_block;
            }
		}
	}
set_rblock_with_othererr:
	tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB^0x40;
	sour[1] = ((tmpch >>2 ) & 0x10) | 0x82; /* 置其它错误的R字组 */
	sour[2] = 0;

re_send_r_block:
	if(re_send_r_times != 0)
	{
		if((oldrblock[0]!= sour[0]) || (oldrblock[1] != sour[1]) || (oldrblock[2] != sour[2]))
			re_send_r_times = 0;
	}
	if(re_send_r_times == 0)
	{
		oldrblock[0]=sour[0];
		oldrblock[1]=sour[1];
		oldrblock[2]=sour[2];
	}

	if(re_send_r_times > 1)
//	if(re_send_r_times > 2)
//	if(re_send_r_times > rblockmax)
	{
		s_DelayUs(100);
		return T1_MOREERR;
	}

	icc_t1send(sour);
	re_send_r_times++;
	goto receive_one_block;

end_t1_exchange:
	if(Ret != 0)
	{/* 如果通信与处理失败，则置SW1 SW2为0xFF；*/
		gl_CardSlotInfo[gl_CardInfo.current_slot].sw1 = 0xFF;
		gl_CardSlotInfo[gl_CardInfo.current_slot].sw2 = 0xFF;
		return Ret;
	}

	len = outbuf[0]*256 + outbuf[1]+2;
	/* 将接收到的最后两位数据保存到SW1，SW2*/
	gl_CardSlotInfo[gl_CardInfo.current_slot].sw1 = outbuf[len-2];
	gl_CardSlotInfo[gl_CardInfo.current_slot].sw2 = outbuf[len-1];
	return ICC_SUCCESS;
}

#else

static int icc_t1Exchange(uchar *inbuf,uchar *outbuf)
{
	uchar	sour[300],oldsour[300],respbuf[300],oldrblock[5];
	uchar	OLDIFSC;
	uchar	PCB,tmpch = 0,tmpch1;
	uint	len,len1,i;

	uint	total_send_len,sent_len,remain_len;
	uchar	re_send_i_times,re_send_r_times;
	uint	WTX = 0;
	int		Ret = 0;
	OLDIFSC = gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC;
	gl_CardInfo.Parity_Check_Enable = 0x00;

start_t1_exchange:

	re_send_i_times = 0;
	re_send_r_times = 0;
	total_send_len = inbuf[0]*256 + inbuf[1];
	remain_len = inbuf[0]*256 + inbuf[1];
	sent_len = 0;

	outbuf[0] = 0x00;
	outbuf[1] = 0x00;
	outbuf[2] = 0x00;
	len = 0;
	len1 = 0;

	gl_CardSlotInfo[gl_CardInfo.current_slot].sw1 = 0xFF;
	gl_CardSlotInfo[gl_CardInfo.current_slot].sw2 = 0xFF;

	memset(respbuf, 0x00, sizeof(respbuf));
	memset(sour, 0x00, sizeof(sour));
	memset(oldsour, 0x00, sizeof(oldsour));
	memset(oldrblock, 0x00, sizeof(oldrblock));

send_remain_data:
	if(remain_len == 0)
	{
		return 0xFF;
	}

	if(remain_len > gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC)
	{   /*  如果数据包长大于IFSC时，则将包进行切分后再发送*/
		gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB |= 0x20;   /*给I字组加上链接符 */
		memcpy(&sour[3],&inbuf[2+sent_len],gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC);
		/*设定发送数据I_BLOCK  */
		sour[2] = gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC;  /* 设定I_block包数据长度*/
	}
	else
	{   /*否则只用一包就可完全发出  */
		gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB &= 0xDF;
		memcpy(&sour[3],&inbuf[2+sent_len],remain_len);
		sour[2] = remain_len;
	}
	sent_len += sour[2]; /* 设定已发送数据长度	*/
	remain_len = total_send_len - sent_len;
	sour[1] = gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB; /* 设定发送包头的PCB协议控制字节*/

	sour[0] = 0x00;
	memcpy(oldsour, sour, sour[2]+3);/* 备份发送的I字组 */
	gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB = sour[1]^0x40;
	re_send_i_times = 0;

re_send_i_block:
	if(re_send_i_times > 2)
	{
		s_DelayUs(100);
		return (-ICC_COMMERR);
	}
	re_send_i_times++;
	re_send_r_times = 0;
	sour[0] = 0x00;

	icc_t1send(sour);

receive_one_block:
	Ret = icc_t1recv(respbuf);
	// 将BWT时间调整到原来的时间
    i = gl_CardSlotInfo[gl_CardInfo.current_slot].BWT_TEMP;
	gl_CardSlotInfo[gl_CardInfo.current_slot].BWT = i;

	// 接收一帧数据后，关闭相关时序检测
	if((Ret == T1_CWTERR)||(Ret == T1_BWTERR)||(Ret == T1_ABORTERR) || (Ret == ERROR))
	{
		s_DelayUs(100);
		return Ret;
	}

	if((Ret == T1_PARITYERR) || (Ret == T1_EDCERR))
	{
		tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB^0x40;
		/* 序号为正在接收卡片的I字组顺序号（即为上次接收的下个序号）*/
		sour[1] = ((tmpch >> 2) & 0x10) | 0x81; /* 置奇偶/EDC错误 */
		sour[2] = 0;
		goto re_send_r_block;
	}

	if(Ret == T1_INVALIDBLOCK)
		goto set_rblock_with_othererr;

	PCB = respbuf[1]; /* 读取响应数据包头*/

	if((PCB & 0x80) == 0x00)
	{
		if(remain_len > 0)
		{
			//发送链接块收到I块说明错误
			goto set_rblock_with_othererr;
		}

		if((PCB & 0x40) != (gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB & 0x40))
		{/*  收到的I字组序列号不同于先前收到的I字组序列号
		时表确认接收正确，并保存接收的数据到OUTBUF中。*/
			re_send_r_times = 0;

			if(respbuf[2] > 0)
			{ /*如果接收数据长度不为零则保存接收数据*/
				len1 = outbuf[0] * 256 + outbuf[1];
				len=len1 + respbuf[2];
				outbuf[0]= len / 256;
				outbuf[1]= len % 256;
				memcpy(&outbuf[len1+2],&respbuf[3],respbuf[2]);
			}

			gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB = PCB;
			/*设置新的I字组顺序号*/
			if((PCB&0x20) == 0x20)
			{ /* 收到有链的I字组 */
				tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB^0x40;
				/* 下个I_Block顺序号*/
				sour[1] = ((tmpch >>2 ) & 0x10) | 0x80;
				sour[2] = 0;
				goto re_send_r_block;
			}
			/* 因正确接收到无链接的I字组则退出*/
			Ret = ICC_SUCCESS;
			goto end_t1_exchange;
		}
		else
		{
			goto set_rblock_with_othererr;
		}
	}

	else if((PCB&0XC0) == 0X80)
	{// ?r_block
		tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB;
		if((tmpch&0x40) == ((PCB&0x10) << 2))
		{
			/*	序号等于终端下个要发送的I字组序号 */
			if((PCB&0xef) == 0x80)
			{// r-block with error-free
				if((oldsour[1]&0x20) == 0x20) /* 终端有链 */
				{
					//发送成功
					goto send_remain_data;
				}
				else
				{   // 说明卡发给终端I块,终端没有收到,卡重发R块要求终端应答
					tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB^ 0x40;
					tmpch1 = ((tmpch >> 2 ) & 0x10) | 0x82;
					re_send_r_times = 0;
					sour[1] = tmpch1;
					sour[2] = 0;
					goto re_send_r_block;
				}
			}
			else
			{/* 带有错误标识的R字组 */
				if(re_send_r_times == 0)
				{ /* 上一次无R字组 */
					tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB ^ 0x40;
					sour[1] = ((tmpch >>2 ) & 0x10) | 0x80;
					sour[2] = 0;
					goto re_send_r_block;
				}
				else
				{ /* 修改错误标识 */
					if((sour[1]&0x0f)!= 0x00)
					{
						sour[1] = sour[1]&0xf0;
						re_send_r_times = 0;
					}
				}
			}
			/* 重发R字组 */
			goto re_send_r_block;
		}


		if((tmpch&0x40) != ((PCB&0X10) << 2))
		{
			/* 如果序号等于终端已发送的I字组序号则重发该I字组 */
			if((gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB&0x20) == 0x20)
			{
				//上次接收到链接块,终端发送的R块应答,卡未收到，要求终端重发R块
				goto re_send_r_block;
			}

			if(OLDIFSC != gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC)
			{
				//发送I块时，收到卡的IFSC请求,响应后，又收到R块要求终端按照新的IFSC大小重发I块
				// 此时因为终端发送I块已经确认，因此终端重发I块，I块块号按照新的块号发送
				OLDIFSC = gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC;
				gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB = oldsour[1];
				goto start_t1_exchange;
			}
			memcpy(sour, oldsour, oldsour[2]+3);
			goto re_send_i_block;
		}
		/* 重发R字组 */
		goto re_send_r_block;

	}
	else if((PCB&0XC0) == 0XC0)
	{// ?s_block
		if((PCB & 0x20) == 0X00)
		{/*  如果收到的是IC卡发送的请求信号则根据传递来的设置
		参数修改相应的设定并回送S响应字组给IC卡  */
			sour[1] = 0x20 | respbuf[1] ;
			sour[2] = respbuf[2];
			memcpy(&sour[3], &respbuf[3], sour[2]);
			if((respbuf[1] & 0x1F) == 0x03)
			{	/* 扩充BWT请求	BWT=BWT * respbuf[3]; */
				i = gl_CardSlotInfo[gl_CardInfo.current_slot].BWT;
				gl_CardSlotInfo[gl_CardInfo.current_slot].BWT_TEMP = i;
				if(respbuf[3] != 0)
				{
                    WTX = (gl_CardSlotInfo[gl_CardInfo.current_slot].TB3 >> 4)&0x0f;
                    WTX = (((uint)1 << WTX) * 960 *gl_CardSlotInfo[gl_CardInfo.current_slot].D) + 11;
					i = (WTX + 960 *gl_CardSlotInfo[gl_CardInfo.current_slot].D) * respbuf[3] + 100;
                    gl_CardSlotInfo[gl_CardInfo.current_slot].BWT = i;
				}
			}
			else if ((respbuf[1] & 0x1f) == 0x01)
			{/* 信息字段容量请求*/
				gl_CardSlotInfo[gl_CardInfo.current_slot].IFSC = respbuf[3];
			}

			else if((respbuf[1]&0x1f) == 0x00)
			{ /*  同步请求	*/

			}
			sour[0] = 0x00;  /* 回送S响应字组给IC卡*/

			icc_t1send(sour);
			re_send_r_times = 0;

			goto receive_one_block;
		}
		else
		{ /* 接收的是IC卡发送的S响应信号*/
			if((sour[1]&0xe0) == 0xc0)
			{/*  如果终端发送是请求信息时则根据响应设置相应的参数*/
				re_send_r_times = 0;

				if ((respbuf[1] & 0x1f) == 0x01)
				{
					gl_CardSlotInfo[gl_CardInfo.current_slot].IFSD = respbuf[3];
				}
				else if((respbuf[1] & 0x1f) == 0x00)
				{	 //  Reset or deactivation
					gl_CardSlotInfo[gl_CardInfo.current_slot].TERM_PCB = 0x00;
					gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB = 0x40;
					goto start_t1_exchange;
				}
				goto end_t1_exchange;
			}
		}
	}
set_rblock_with_othererr:
	tmpch = gl_CardSlotInfo[gl_CardInfo.current_slot].CARD_PCB^0x40;
	sour[1] = ((tmpch >>2 ) & 0x10) | 0x82; /* 置其它错误的R字组 */
	sour[2] = 0;

re_send_r_block:
	if(re_send_r_times != 0)
	{
		if((oldrblock[0]!= sour[0]) || (oldrblock[1] != sour[1]) || (oldrblock[2] != sour[2]))
			re_send_r_times = 0;
	}
	if(re_send_r_times == 0)
	{
		oldrblock[0]=sour[0];
		oldrblock[1]=sour[1];
		oldrblock[2]=sour[2];
	}

	if(re_send_r_times > 1)
	{
		s_DelayUs(100);
		return T1_MOREERR;
	}

	icc_t1send(sour);
	re_send_r_times++;
	goto receive_one_block;

end_t1_exchange:
	if(Ret != 0)
	{/* 如果通信与处理失败，则置SW1 SW2为0xFF；*/
		gl_CardSlotInfo[gl_CardInfo.current_slot].sw1 = 0xFF;
		gl_CardSlotInfo[gl_CardInfo.current_slot].sw2 = 0xFF;
		return Ret;
	}

	len = outbuf[0]*256 + outbuf[1]+2;
	/* 将接收到的最后两位数据保存到SW1，SW2*/
	gl_CardSlotInfo[gl_CardInfo.current_slot].sw1 = outbuf[len-2];
	gl_CardSlotInfo[gl_CardInfo.current_slot].sw2 = outbuf[len-1];
	return ICC_SUCCESS;
}
#endif

int inter_icc_exchange(int sendlen, uchar *psenddata, uint *precvlen, uchar *precvdata)
{
	uchar test[8];
	uchar cmd_case;
	uchar inbuf[512];
	uchar outbuf[512];
	uchar *ptr, len;
	ushort i;
	int Ret = 0;

	gl_CardSlotInfo[gl_CardInfo.current_slot].sw1 = 0xff;
	gl_CardSlotInfo[gl_CardInfo.current_slot].sw2 = 0xff;
	memset(outbuf, 0x00, sizeof(outbuf));

	switch(gl_CardSlotInfo[gl_CardInfo.current_slot].T)
	{/* 根据T=1或T=0协议进行相应的处理*/
		case 1:
			i = 4;
			inbuf[2] = psenddata[0];
			inbuf[3] = psenddata[1];
			inbuf[4] = psenddata[2];
			inbuf[5] = psenddata[3];
			ptr = &inbuf[6];

			if(sendlen == 4)
			{
				i++;
				*ptr++ = 0;
			}
			else if(sendlen == 5)
			{
				i++;
				*ptr++ = psenddata[4];
			}
			else
			{
				len = psenddata[4];
				i += (len + 1);
				*ptr++ = len;
				if(len > 0)
				{
					memcpy(ptr, psenddata+5, len);
					ptr += len;
				}

				if(sendlen == (6 + psenddata[4]))
				{
					i++;
					*ptr++ = psenddata[sendlen-1];
				}
			}

			inbuf[0] = i/256;
			inbuf[1] = i%256;
LABLE(D_T1_ST);
            icc_DisableOtherInt();
			Ret = icc_t1Exchange(inbuf, outbuf);
            icc_EnableOtherInt();
LABLE(D_T1_END);
			icc_close_etu_timer();
			icc_disable_inter_timer();
			gl_char_wait_etu = 0;
			gl_total_atr_etu = 0;

			if(Ret == ICC_SUCCESS)
			{
				memcpy(precvdata,&outbuf[2],outbuf[0]*256+outbuf[1]);
				*precvlen = outbuf[0]*256+outbuf[1];
			}
			return Ret;

		case 0:
			memcpy(inbuf, psenddata, 4);
			cmd_case = 0x01;
			if(sendlen == 4)
			{
				cmd_case = 0x01;  // case 1 command
				len = 4;
			}
			else if(sendlen == 5)
			{
				cmd_case = 0x02;  // case 2 command
				inbuf[4] = psenddata[4];
				len = 5;
			}
			else if(sendlen == (5 + psenddata[4]))
			{
				cmd_case = 0x03;  // case 3 command
			}
			else
			{
				cmd_case = 0x04;  // case 4 command
			}
            CLRBUF(test);
			test[0] = cmd_case;

			if((cmd_case == 0x03) || (cmd_case == 0x04))
			{   /* 表向IC卡发送P3长度的数据*/
				len = 5 + psenddata[4];
				inbuf[4] = psenddata[4];
				memcpy(inbuf+5, &psenddata[5], psenddata[4]);
				if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
				{
					if(cmd_case == 0x04)
					{
						inbuf[len]= psenddata[len];
						len++;
					}
				}
			}
LABLE(D_T0_ST);
            icc_DisableOtherInt();
			Ret = icc_t0Exchange(len,inbuf,outbuf,test);
            icc_EnableOtherInt();
LABLE(D_T0_END);
			icc_close_etu_timer();
			icc_disable_inter_timer();
			gl_char_wait_etu = 0;
			gl_total_atr_etu = 0;

			if(Ret)
			{
				return Ret;
			}
			memcpy(precvdata,outbuf+2,outbuf[0]*256 + outbuf[1]);
			*precvlen = outbuf[0]*256 + outbuf[1];

			precvdata[*precvlen] = test[0];
			precvdata[(*precvlen) + 1] = test[1];
			*precvlen += 2;
			gl_CardSlotInfo[gl_CardInfo.current_slot].sw1 = test[0];
			gl_CardSlotInfo[gl_CardInfo.current_slot].sw2 = test[1];
			return ICC_SUCCESS;
		default:
			break;
	}
	return (-ICC_RESPERR);
}

int  icc_ifsdCmd(void)
{
	int Ret = 0;
	uchar ifsdbuf[10], respbuf[300];
	uchar resendtimes = 0;

	if(gl_CardSlotInfo[gl_CardInfo.current_slot].T == 0)
		return ICC_SUCCESS;

	gl_CardInfo.Parity_Check_Enable = 0x00;

	memset(ifsdbuf, 0, sizeof(ifsdbuf));
	memset(respbuf, 0, sizeof(respbuf));

	ifsdbuf[0] = 0x00;
	ifsdbuf[1] = 0xC1;
	ifsdbuf[2] = 0x01;
	ifsdbuf[3] = 0xFE;
	ifsdbuf[4] = 0x00;

re_send_ifsd:

	if(resendtimes < 3)
	{
		Ret = icc_t1send(ifsdbuf);
	}
	else
	{
		s_DelayUs(100);
		return T1_IFSDERR;
	}

	resendtimes++;
    icc_DisableOtherInt();
    Ret = icc_t1recv(respbuf);
    icc_EnableOtherInt();

	if((Ret == T1_CWTERR)||(Ret == T1_BWTERR)||(Ret == T1_ABORTERR)||(Ret == ERROR))
	{
		s_DelayUs(100);
		return Ret;
	}
	if((Ret == T1_PARITYERR)||(Ret == T1_EDCERR)||(Ret == T1_INVALIDBLOCK))
		goto re_send_ifsd;
	if(((respbuf[1]&0xE0) != 0xE0)||((respbuf[1]&0x0F) != 0x01))
		goto re_send_ifsd;

	if((respbuf[2] != 0x01)||(respbuf[3] != 0xFE))
		goto re_send_ifsd;

	gl_CardSlotInfo[gl_CardInfo.current_slot].IFSD = 254;
	return ICC_SUCCESS;

}

// PPS协议
int icc_PPSRequest(void)
{
    uchar ucSendBuf[4];
    uchar ucRecvBuf[4];
    int Ret = 0;
    uint i = 0;
    uint Fi = 0;
    uint Di = 0;
    uint ETU = 0;
	uchar TA1=0x11;
    memset(ucSendBuf, 0x00, sizeof(ucSendBuf));
    memset(ucRecvBuf, 0x00, sizeof(ucRecvBuf));

    gl_CardInfo.Parity_Check_Enable = 0x01;
    gl_CardInfo.k_Ic_CharWait_TimeCount = gl_CardSlotInfo[gl_CardInfo.current_slot].WWT;
    gl_CardInfo.k_RecvFirst = gl_CardInfo.k_Ic_CharWait_TimeCount - 11;

    if(!(gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag & 0x01))  // 无TA1，不协商，仍按照D=1通信
    {
        return ICC_SUCCESS;
    }

    if((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0x0F) == 0x01) // TA1为D=1,则没有必要协商
    {
        return ICC_SUCCESS;
    }

    // 判断是否为合法的Fi值
    if(((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) == 0x70)
        || ((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) == 0x80)
        || ((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) == 0xE0)
        || ((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) == 0xF0))
    {
        // 不支持的Fi值，不协商仍按Initial ETU通信
        return ICC_SUCCESS;
    }
    Fi = (gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) >> 4;

    // 判断是否为合法的Di值
    if(((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0x0F) == 0x00)
        || ((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0x0F) > 0x09))
    {
        // 不支持的Di值，不协商仍按Initial ETU通信
        return ICC_SUCCESS;
    }
    Di = gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0x0F;

    // 最高支持5MHz
	ucSendBuf[0] = 0xFF;
    ucSendBuf[1] = 0x10 | gl_CardSlotInfo[gl_CardInfo.current_slot].T;
	ucSendBuf[2] = gl_CardSlotInfo[gl_CardInfo.current_slot].TA1;

    ETU = (F_Table[Fi]/D_Table[Di]);
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
	{
		 if(ETU < 46)
		 {
		 	if(F_Table[Fi] == 512 && D_Table[Di] == 16)
		 	{
				TA1 = 0x11;
			}
			else
			{
				TA1 = 0x94;
			}
		 }
		 else
		 {
			TA1 = gl_CardSlotInfo[gl_CardInfo.current_slot].TA1;
		 }
	}
	else
	{
	    if(ETU < 64)
	    {
	    	if(F_Table[Fi] == 512 && D_Table[Di] == 16)
			{
				//yehf 2012-03-08 特殊处理需要用11协商
				TA1 = 0x11;
				// 采用95波特率协商
				//TA1 = 0x95;
			}
			else
			{
				//超过55800，采用94进行协商
				TA1 = 0x94;
			}

	    }
		else
		{
			TA1 = gl_CardSlotInfo[gl_CardInfo.current_slot].TA1;
		}
	}
	ucSendBuf[2] = TA1;
	Di = (TA1 & 0x0F);
	Fi = ((TA1 & 0xF0) >> 4);
	ETU = (F_Table[Fi]/D_Table[Di]);

	ucSendBuf[3] = (ucSendBuf[0]^ucSendBuf[1]^ucSendBuf[2]);

	icc_init_inter_timer();
	icc_init_etu_timer();
	gl_char_wait_etu = 0;
	gl_total_atr_etu = 0;
	icc_delay_etu(gl_CardSlotInfo[gl_CardInfo.current_slot].ETU,8);
LABLE(D_PPS_ST);
    // 发送PPSS
    Ret = icc_SendByte(ucSendBuf[0]);
    if(Ret)
    {
        return (-ICC_COMMERR);
    }

    // 发送PPS0
    Ret = icc_SendByte(ucSendBuf[1]);
    if(Ret)
    {
        return (-ICC_COMMERR);
    }

    // 发送PPS1
    Ret = icc_SendByte(ucSendBuf[2]);
    if(Ret)
    {
        return (-ICC_COMMERR);
    }

    // 发送PPCK
    gl_CardInfo.k_Ic_LastSendByte = 1;
    Ret = icc_SendByte(ucSendBuf[3]);
    gl_CardInfo.k_Ic_LastSendByte = 0;
    if(Ret)
    {
        return (-ICC_COMMERR);
    }
    for(i = 0; i < 4; i++)
    {
		ucRecvBuf[i] = icc_RecvByte();
        if(gl_CardInfo.k_IccComErr)
        {
            if(gl_CardInfo.k_IccErrTimeOut)
            {
            	D2(TRACE("\r\n PPS超时"););
                return (-ICC_TIMEOUT);
            }
            else
            {
                return (-ICC_COMMERR);
            }
        }
    }
    // 比较发送接收的字符是否一致
    if(memcmp(ucSendBuf, ucRecvBuf, sizeof(ucRecvBuf)))
    {
        return (-ICC_RESPERR);
    }

    gl_CardSlotInfo[gl_CardInfo.current_slot].D = D_Table[Di];
    gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = ETU;
//T0 测试时间。
	Di = gl_CardSlotInfo[gl_CardInfo.current_slot].D;
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
	{
		i = ((960 * Di * gl_CardSlotInfo[gl_CardInfo.current_slot].TC2) + 1);
	}
	else
	{
		i = ((960 * Di * gl_CardSlotInfo[gl_CardInfo.current_slot].TC2) + (480 * Di) + 1);
	}
	gl_CardSlotInfo[gl_CardInfo.current_slot].WWT = (i);
	//TRACE("\r\n Di:%d ETU:%d TC2:%02X WWT:%d",Di,ETU,gl_CardSlotInfo[gl_CardInfo.current_slot].TC2,gl_CardSlotInfo[gl_CardInfo.current_slot].WWT);
//T1 测试时间。
	Di = gl_CardSlotInfo[gl_CardInfo.current_slot].TB3;
	i = (((uint)0x01<<((Di & 0xF0) >> 4))*960*gl_CardSlotInfo[gl_CardInfo.current_slot].D) + 11;
	i += (gl_CardSlotInfo[gl_CardInfo.current_slot].D * 960);
	i += 1;
    gl_CardSlotInfo[gl_CardInfo.current_slot].BWT = i;
	gl_CardSlotInfo[gl_CardInfo.current_slot].BWT_TEMP = i;
	D2(
		TRACE("\r\n Di:%d BWT:%d-%d",Di,gl_CardSlotInfo[gl_CardInfo.current_slot].BWT,i);
	);
    icc_adjustTiming();
    return ICC_SUCCESS;
}

// PPS协议
int icc_PPSSelect(int mode)
{
    uchar ucSendBuf[4];
    uchar ucRecvBuf[4];
    int Ret = 0;
    uint i = 0;
    uint Fi = 0;
    uint Di = 0;
    uint ETU = 0;
	uchar TA1 = mode;
    memset(ucSendBuf, 0x00, sizeof(ucSendBuf));
    memset(ucRecvBuf, 0x00, sizeof(ucRecvBuf));
//D2(TRACE("\r\n PPS mode=%x", mode););
    gl_CardInfo.Parity_Check_Enable = 0x01;
    gl_CardInfo.k_Ic_CharWait_TimeCount = gl_CardSlotInfo[gl_CardInfo.current_slot].WWT;
    gl_CardInfo.k_RecvFirst = gl_CardInfo.k_Ic_CharWait_TimeCount - 11;

    if(!(gl_CardSlotInfo[gl_CardInfo.current_slot].TAFlag & 0x01))  // 无TA1，不协商，仍按照D=1通信
    {
        return ICC_SUCCESS;
    }

    if((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0x0F) == 0x01) // TA1为D=1,则没有必要协商
    {
        return ICC_SUCCESS;
    }

    // 判断是否为合法的Fi值
    if(((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) == 0x70)
        || ((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) == 0x80)
        || ((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) == 0xE0)
        || ((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0xF0) == 0xF0))
    {
        // 不支持的Fi值，不协商仍按Initial ETU通信
        return ICC_SUCCESS;
    }
    Fi = ((TA1 & 0xF0) >> 4);

    // 判断是否为合法的Di值
    if(((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0x0F) == 0x00)
        || ((gl_CardSlotInfo[gl_CardInfo.current_slot].TA1 & 0x0F) > 0x09))
    {
        // 不支持的Di值，不协商仍按Initial ETU通信
        return ICC_SUCCESS;
    }
    Di = (TA1 & 0x0F);
	ETU = (F_Table[Fi]/D_Table[Di]);
    // 最高支持5MHz
	ucSendBuf[0] = 0xFF;
    ucSendBuf[1] = 0x10 | gl_CardSlotInfo[gl_CardInfo.current_slot].T;
	ucSendBuf[2] = TA1;
	ucSendBuf[3] = (ucSendBuf[0]^ucSendBuf[1]^ucSendBuf[2]);

	icc_init_inter_timer();
	icc_init_etu_timer();
	gl_char_wait_etu = 0;
	gl_total_atr_etu = 0;
	icc_delay_etu(gl_CardSlotInfo[gl_CardInfo.current_slot].ETU,8);
LABLE(D_PPS_ST);
    // 发送PPSS
    Ret = icc_SendByte(ucSendBuf[0]);
    if(Ret)
    {
        return (-ICC_COMMERR);
    }

    // 发送PPS0
    Ret = icc_SendByte(ucSendBuf[1]);
    if(Ret)
    {
        return (-ICC_COMMERR);
    }

    // 发送PPS1
    Ret = icc_SendByte(ucSendBuf[2]);
    if(Ret)
    {
        return (-ICC_COMMERR);
    }

    // 发送PPCK
    gl_CardInfo.k_Ic_LastSendByte = 1;
    Ret = icc_SendByte(ucSendBuf[3]);
    gl_CardInfo.k_Ic_LastSendByte = 0;
    if(Ret)
    {
        return (-ICC_COMMERR);
    }
    for(i = 0; i < 4; i++)
    {
		ucRecvBuf[i] = icc_RecvByte();
        if(gl_CardInfo.k_IccComErr)
        {
            if(gl_CardInfo.k_IccErrTimeOut)
            {
            	D2(TRACE("\r\n PPS超时"););
                return (-ICC_TIMEOUT);
            }
            else
            {
                return (-ICC_COMMERR);
            }
        }
    }
    // 比较发送接收的字符是否一致
    if(memcmp(ucSendBuf, ucRecvBuf, sizeof(ucRecvBuf)))
    {
        return (-ICC_RESPERR);
    }

    gl_CardSlotInfo[gl_CardInfo.current_slot].D = D_Table[Di];
    gl_CardSlotInfo[gl_CardInfo.current_slot].ETU = ETU;
//T0 测试时间。
	Di = gl_CardSlotInfo[gl_CardInfo.current_slot].D;
	if(gl_CardSlotInfo[gl_CardInfo.current_slot].PortMode == SHBMODE)
	{
		i = ((960 * Di * gl_CardSlotInfo[gl_CardInfo.current_slot].TC2) + 1);
	}
	else
	{
		i = ((960 * Di * gl_CardSlotInfo[gl_CardInfo.current_slot].TC2) + (480 * Di) + 1);
	}
	gl_CardSlotInfo[gl_CardInfo.current_slot].WWT = (i);
	//TRACE("\r\n Di:%d ETU:%d TC2:%02X WWT:%d",Di,ETU,gl_CardSlotInfo[gl_CardInfo.current_slot].TC2,gl_CardSlotInfo[gl_CardInfo.current_slot].WWT);

//T1 测试时间。
	Di = gl_CardSlotInfo[gl_CardInfo.current_slot].TB3;
	i = (((uint)0x01<<((Di & 0xF0) >> 4))*960*gl_CardSlotInfo[gl_CardInfo.current_slot].D) + 11;
	i += (gl_CardSlotInfo[gl_CardInfo.current_slot].D * 960);
	i += 1;
    gl_CardSlotInfo[gl_CardInfo.current_slot].BWT = i;
	gl_CardSlotInfo[gl_CardInfo.current_slot].BWT_TEMP = i;
	D2(
		TRACE("\r\n Di:%d BWT:%d-%d",Di,gl_CardSlotInfo[gl_CardInfo.current_slot].BWT,i);
	);
    icc_adjustTiming();
    return ICC_SUCCESS;
}

#endif

