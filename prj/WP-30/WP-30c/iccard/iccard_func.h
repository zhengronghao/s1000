#ifndef ICCARD_FUNC_H_
#define ICCARD_FUNC_H_

#define   CARD_SLOTNUM   3

typedef struct
{
	uchar   VolMode;           // 当前卡座电压
	uchar   PortMode;          // 工作模式
	uchar   BaudRate;          // 上电波特率
	uchar   ExChangeBaudRate;  //运行波特率
	uchar   BaudRateClassFlag; //波特率统计开关
	uchar   OnlyAtrFlag;       //只取ATR的开关

	uchar   CardPowerOn;    // 卡是否已上电， 0 - 未上电， 1 - 已上电
	uchar   ResetStatus;    // 当前复位阶段， 0 - 未复位， 1 - 冷复位， 2 - 热复位
	uchar   CardInsert;		// 卡是否在位
	uchar   T;              // 当前卡片协议， 0 - T=0协议     1 - T=1协议

	uchar   D;              // 频率分频因子
	uint  	ETU;            // 位持续时间

	uchar   TS;             // ATR起始字符
	uchar   T0;             // T0字符

	uchar   TAFlag;         // TAi字节存在标志
	uchar   TA1;
	uchar   TA2;
	uchar   TA3;
	uchar   TA4;

	uchar   TBFlag;         // TBi字节存在标志
	uchar   TB1;
	uchar   TB2;
	uchar   TB3;
	uchar   TB4;

	uchar   TCFlag;         // TCi字节存在标志
	uchar   TC1;
	uchar   TC2;
	uchar   TC3;
	uchar   TC4;

	uchar   TDFlag;         // TDi字节存在标志
	uchar   TD1;
	uchar   TD2;
	uchar   TD3;
	uchar   TD4;

	uchar   HistNum;        // ATR携带的历史字节个数

	uchar   PPSENABLE;      // PPS功能允许,  0 - 不允许， 1 - 允许
	uint  	IFSC;           // ICC能接受的最大帧长度
	uint  	IFSD;           // Device能接受的最大帧长度

	uint   	WWT;            // T=0协议的字符最大间隔时间
	uint   	CWT;            // T=1协议的字符最大间隔时间
	uint  	BWT;            // T=1协议的块最大间隔时间
	uint  	BWT_TEMP;       // T=1协议的临时块最大间隔时间
	uint  	BGT;            // T=1块的最小反向保护时间

	uchar   sw1;
	uchar   sw2;

	uchar   TERM_PCB;       // T=1协议下终端发送下个字段格式与序列号
	uchar   CARD_PCB;       // T=1协议下卡发送下个字段格式与序列号

	uchar   XI;				//时钟停止指示符
	uchar   UI;				//类别指示符
}ICCARD_WORK_STRUCT;

// ATR过程中错误返回定义
#define   ATR_TSERR          0x11  /*  正反向约定错误  */
#define   ATR_TCKERR         0x12  /*  复位校验错误    */
#define   ATR_TIMEOUT        0x13  /*  复位等待超时    */
#define   ATR_TA1ERR         0x14  /*  TA1错误         */
#define   ATR_TA2ERR         0X15  /*  TA2错误         */
#define   ATR_TA3ERR         0x16  /*  TA3错误         */
#define   ATR_TB1ERR         0x17  /*  TB1错误         */
#define   ATR_TB2ERR         0x18  /*  TB2错误         */
#define   ATR_TB3ERR         0x19  /*  TB3错误         */
#define   ATR_TC1ERR         0x1A  /*  TC1错误         */
#define   ATR_TC2ERR         0x1B  /*  TC2错误         */
#define   ATR_TC3ERR         0x1C  /*  TC3错误         */
#define   ATR_TD1ERR         0x1D  /*  TD1错误         */
#define   ATR_TD2ERR         0x1E  /*  TD2错误         */
#define   ATR_LENGTHERR      0x1F  /*  ATR数据长度错误  */
#define   ATR_PARERR         0x20  /*  ATR字符奇偶校验错*/

// T=0协议通信过程中错误返回定义
#define   T0_TIMEOUT         0x21  /*  T=0等待时间溢出  */
#define   T0_MORESENDERR     0x22  /*  T=0字符重发次数超出*/
#define   T0_MORERECEERR     0x23  /*  T=0错误字符接收次数超出*/
#define   T0_PARERR          0x24  /*  T=0字符包含奇偶错误*/
#define   T0_INVALIDSW       0x25  /*  T=0返回错误状态码*/

// T=1协议通信过程中错误返回定义
#define   T1_BWTERR          0x31  /*   T=1字组等待时间错误  */
#define   T1_CWTERR          0x32  /*   T=1字符等待时间错误  */
#define   T1_ABORTERR        0x33  /*   放弃通信            */
#define   T1_EDCERR          0x34  /*   校验码错误          */
#define   T1_SYNCHERR        0x35  /*   同步错误            */
#define   T1_EGTERR          0x36  /*   字符保护时间错误    */
#define   T1_BGTERR          0x37  /*   字组保护时间错误    */
#define   T1_NADERR          0x38  /*   字组中NAD错误       */
#define   T1_PCBERR          0x39  /*   字组PCB错误         */
#define   T1_LENGTHERR       0x3A  /*   字组LEN错误         */
#define   T1_IFSCERR         0x3B  /*   IFSC错误            */
#define   T1_IFSDERR         0x3C  /*   IFSD错误            */
#define   T1_MOREERR         0x3D  /*   多次传送错误并放弃  */
#define   T1_PARITYERR       0x3E  /*   T=1块包含奇偶错误字节 */
#define   T1_INVALIDBLOCK    0x3F  /*   为非法T=1块*/

// 社保卡ATR过程中错误返回定义
#define   ATR_TSERR_SHB          0x41  /*  正反向约定错误  */
#define   ATR_TIMEOUT_SHB		 0x42  /*ATR 字节超时*/

extern ICCARD_WORK_STRUCT gl_CardSlotInfo[CARD_SLOTNUM];
extern ushort const F_Table[16];
extern uchar const D_Table[16];

void s_InitCardInfo(void);
void icc_InitICCTiming();

void icc_adjustTiming(void);
int  icc_ColdReset(uchar *ATR);
int  icc_WarmReset(uchar *ATR);
int inter_icc_exchange(int sendlen, uchar *psenddata, uint *precvlen, uchar *precvdata);
int  icc_ifsdCmd(void);
int icc_PPSRequest(void);
int icc_PPSSelect(int mode);

#endif

