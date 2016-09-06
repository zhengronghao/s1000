  /*********************************************************************
* 版权所有 (C)2003, 福建实达电脑设备有限公司。
* 文件名称： 
*   Mifs.h
* 当前版本： 
*   01-01-01
* 内容摘要： 
*   本文件实现Mifs.c的头文件,向上提供API接口
* 历史纪录：
*   修改人      日期		    	版本号      修改记录
******************************************************************/
 // 只包含一次
#ifndef _MIFS_H_
#define _MIFS_H_

#include "wp30_ctrl.h"
/**********************************************************************

                         参数定义
                          
***********************************************************************/
// ISO14443 
#define EM_mifs_TYPEA				0x00
#define EM_mifs_TYPEB				0x08
#define EM_mifs_NFCIP				0x01
#define EM_mifs_NULL				0xFF

// Mifare卡Request时的参数
#define EM_mifs_IDLE    0				// 只给一张卡复位
#define EM_mifs_ALL     1				// 给射频天线的有效范围内的所有卡复位

#define EM_mifs_HALT    1				// 给处在HALT状态的卡复位

// Mifare卡认证时参数
#define EM_mifs_KEYA	0				// A套密码
#define EM_mifs_KEYB	1				// B套密码

// TYPE B卡的参数
#define EM_mifs_TYPEB_REQALL  0			// 区域内的卡片都响应

#define EM_mifs_TIMESLOT_MAXINT  4		// 最大时隙数对应的系数
#define EM_mifs_TIMESLOT_MAXNUM  16		// 最大时隙数

#define EM_mifs_IDLE 0					// 激活卡片时卡片处于IDLE状态
#define EM_mifs_HALT 1					// 激活卡片时卡片处于HALT状态

#define EM_mifs_SUPPORTB    1			// 支持ISO14443-4
#define EM_mifs_NOTSUPPORTB 0			// 不支持ISO14443-4

#define EM_mifs_MAXPID   14				// TYPE B卡的最大PID

#define EM_mifs_S50          0
#define EM_mifs_S70          1
#define EM_mifs_PROCARD      2
#define EM_mifs_PRO_S50      3
#define EM_mifs_PRO_S70      4
#define EM_mifs_TYPEBCARD    5

/**********************************************************************

                         返回值
                          
***********************************************************************/
// ICODE1 Error   Codes                        
#define EM_mifs_NOTAGERR		     2	// 操作范围内无卡
#define EM_mifs_CRCERR      	     3	// 卡CRC校验错误
#define EM_mifs_AUTHERR              4	// 不可能认证
#define EM_mifs_PARITYERR	         5	// 从卡中收到错误的PARITY
#define EM_mifs_CODEERR     	     6	// 卡回送数据内容错误
#define EM_mifs_SERNRERR    	     7	// 在Anticoll中通讯错误
#define EM_mifs_NOTAUTHERR           8	// 未认证
#define EM_mifs_BITCOUNTERR          9	// 卡上送来的数据位位数错误
#define EM_mifs_BYTECOUNTERR         10	// 卡上送来的数据字节数错误
#define EM_mifs_OVFLERR              17	// 数据溢出
#define EM_mifs_FRAMINGERR           18	// 帧错误
#define EM_mifs_UNKNOWN_COMMAND      19	// 无效命令
#define EM_mifs_COLLERR              20	// 卡片冲突
#define EM_mifs_RESETERR             21	// 复位失败
#define EM_mifs_INTERFACEERR         22	// 接口错误
#define EM_mifs_RECBUF_OVERFLOW      23	// 接收缓冲区溢出
#define EM_mifs_VALERR               24	// 数值错误
#define EM_mifs_ERRTYPE              25	// 卡类型错误
#define EM_mifs_FDTERR               26 // FDT错误
#define EM_mifs_SWDIFF               0xE7	// SW1!=0X90或者SW2!=0X00

// 新增返回值 
#define EM_mifs_SUCCESS              EM_SUCCESS
#define EM_mifs_TRANSERR             0xA2	// 卡片与读卡器之间通信错误
#define EM_mifs_PROTERR              0xA3	// 卡片返回数据不符合ISO14443或者PAYPASS规范
#define EM_mifs_MULTIERR             0xA4	// 感应区多卡存在
#define EM_mifs_NOCARD               0xA5	// 在规定时间内没有判断到卡进入感应区
#define EM_mifs_CARDEXIST            0xA6	// 在规定时间内没有将卡移离感应区
#define EM_mifs_TIMEOUT              0xA7	// 数据交换过程中，卡片无响应
#define EM_mifs_NOACT                0xB3	// 卡片未激活
#define EM_mifs_NOISE                0xA8   // 噪声

//采用新api的返回值
// 寻卡模式
// D0: 寻卡方式 0-ISO 1-EMV   
// D1: 是否判断卡端帧号 1-不判断
// D2: 是否支持CID 
// D3: 是否支持NAD 
// D8: 支持TYPEA卡
// D9: 支持TYPEB卡
#define RFID_MODE_ISO                (0<<0)   //ISO方式寻卡，可以从多张卡中寻到1张卡
#define RFID_MODE_EMV                (1<<0)   //EMV方式寻卡，只能有1张卡
#define RFID_MASK_OPT                (1<<0)
#define RFID_MODE_NOFRAME            (1<<1)   //ISO方式寻卡，不判断卡端发送帧号
#define RFID_MASK_FRAME              (1<<1)
#define RFID_MODE_CID                (1<<2)   //支持CID
#define RFID_MASK_CID                (1<<2)
#define RFID_MODE_NAD                (1<<3)   //支持NAD
#define RFID_MASK_NAD                (1<<3)   

#define RFID_MODE_TYPEA              (1<<8)   //寻找TypeA卡
#define RFID_MODE_TYPEB              (1<<9)   //寻找TypeB卡
//#define RFID_MODE_ALL                (RFID_MODE_TYPEA | RFID_MODE_TYPEB)  //寻找2种卡
//#define RFID_MODE_MASK               (RFID_MODE_ALL | RFID_MODE_EMV)

// poll返回卡类型模式
#define RFID_TYPEA                   0   //A卡
#define RFID_TYPEB                   5   //B卡

// powerup最后1个字节返回卡卡类型
#define RFID_CARD_S50                0
#define RFID_CARD_S70                1
#define RFID_CARD_PRO                2
#define RFID_CARD_PROS50             3
#define RFID_CARD_PROS70             4
#define RFID_CARD_B                  5


// 返回值
enum RFID_RET{
	RFID_SUCCESS = 0,
	RFID_ERROR = 4601,   //1打开模块失败
	RFID_ERRPARAM,	     //2参数错误 
	RFID_TIMEOUT,        //3超时
	RFID_NOCARD,         //4无卡
	RFID_TRANSERR,       //5卡片与读卡器之间通信错误
	RFID_PROTERR,        //6卡片返回数据不符合ISO14443或者PAYPASS规范
	RFID_MULTIERR,	     //7感应区多卡存在	
	RFID_NOACT,          //8卡未上电
	RFID_CARDEXIST,      //9卡未离开
	RFID_NOAUTH,         //10未认证
	RFID_AUTHERR,        //11认证失败
	RFID_UNCHANGE,		 //12不可修改
	RFID_KEYNOTPOWER,	 //13密钥无权限

	RFID_DEVICEUSED	= 4630,  	   //设备被占用
	RFID_NODEVICE	= 4631,      // 设备未打开
};

#ifdef __ICCARM__						// IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif	
typedef union 
{
	uchar byte;
	struct
	{
		uchar bit0:1;
		uchar bit1:1;
		uchar bit2:1;
		uchar bit3:1;
		uchar bit4:1;
		uchar bit5:1;
		uchar bit6:1;
		uchar bit7:1;		
	};
}__attribute__ ((packed)) BYTE2BIT;
typedef struct
{
	uchar sector;
	uchar blk;
}__attribute__ ((packed)) MIFARE_MEM;

typedef struct 
{
	uchar pwred;    //上电标志 0-未上电 1-上电 2-认证
	uchar sector;
	uchar type; 	//0-S50 1-S70
	uchar key;		//0-无密钥 1-KeyA 2-KeyB
	uchar keybuf[6];
}__attribute__ ((packed)) MIFARECARD;

typedef struct _KEY
{
	uchar key0;
	uchar key1;
	uchar key2;
	uchar key3;
	uchar key4;
	uchar key5;
}__attribute__ ((packed)) KEY;
typedef union _ACCESSBIT
{
	u32 w;
	struct
	{
		uchar versac1:4;
		uchar versac2:4;
		uchar versac3:4;
		uchar c1:4;			
		uchar c2:4;
		uchar c3:4;
		uchar rfu:8;
	};
}__attribute__ ((packed)) ACCESSBIT;
typedef struct _TRAILER
{
	KEY keyA;
	ACCESSBIT access;
	KEY keyB;
}__attribute__ ((packed)) TRAILER;

struct RECHARGE{
	uchar mode;
	ulong value;
}__attribute__ ((packed)) ;

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif  
extern MIFARECARD gMifareData;

typedef struct _BLKALTER
{
	uchar mode;
	uchar access;
	uchar keyA[6];
	uchar keyB[6];
	uchar rfu[2];
}BLKALTER;

typedef struct
{
    int     index;
    int     pwrfield;
}s_rfidpara_info;
#define S50BLK_SIZE			4
#define S70BLK_SIZE_PRE32	4
#define S70BLK_SIZE_LAST8	16
//#define S70BLK_SIZE			(S70BLK_SIZE_PRE32+S70BLK_SIZE_LAST8)

#define S50SECTOR_SIZE			16
#define S70SECTOR_SIZE_PRE32	32
#define S70SECTOR_SIZE_LAST8	8
#define S70SECTOR_SIZE			(S70SECTOR_SIZE_PRE32+S70SECTOR_SIZE_LAST8)


/**********************************************************************

                         接口定义
                          
***********************************************************************/

typedef struct
{
	// Mifare卡
	uchar(*EA_ucMIFRequest) (DevHandle hDevHandle, uchar ucMode, ushort * pusTagType);
	uchar(*EA_ucMIFAntiColl) (DevHandle hDevHandle, ulong * pulSnr);
	uchar(*EA_ucMIFSelect) (DevHandle hDevHandle, ulong ulSnr, uint * puiSize);
	uchar(*EA_ucMIFAuthentication) (DevHandle hDevHandle, uchar ucMode, uchar ucSecNr);
	uchar(*EA_ucMIFAuthToRam) (DevHandle hDevHandle, uchar ucSecNr, uchar ucKeyType,
		uchar * pucKey);
	uchar(*EA_ucMIFLoadKey) (DevHandle hDevHandle, uchar ucMode, uchar * pucNKey);
	uchar(*EA_ucMIFHalt) (DevHandle hDevHandle);
	uchar(*EA_ucMIFRead) (DevHandle hDevHandle, uchar ucAddr, uchar * pucData);
	uchar(*EA_ucMIFWrite) (DevHandle hDevHandle, uchar ucAddr, uchar * pucData);
	uchar(*EA_ucMIFIncrement) (DevHandle hDevHandle, uchar ucAddr, ulong ulValue);
	uchar(*EA_ucMIFDecrement) (DevHandle hDevHandle, uchar ucAddr, ulong ulValue);
	uchar(*EA_ucMIFRestore) (DevHandle hDevHandle, uchar ucAddr);
	uchar(*EA_ucMIFTransfer) (DevHandle hDevHandle, uchar ucAddr);

	// MifarePro
	uchar(*EA_ucMIFResetPro) (DevHandle hDevHandle, uchar ucMode, uint * puiRespLen, void *pvResp);
	uchar(*EA_ucMIFExchangePro) (DevHandle hDevHandle, uint uiSendLen, void *pvSendData,
		uint * puiRecLen, void *pvRecData);
	uchar(*EA_ucMIFDeselectPro) (DevHandle hDevHandle);

	// TYPE B
	uchar(*EA_ucMIFRequestB) (DevHandle hDevHandle, uchar ucState, uchar ucType,
		uchar ucCount, uint * puiRespLen, void *pvResp);
	uchar(*EA_ucMIFSlotMarkerB) (DevHandle hDevHandle, uchar ucNumber, uint * puiRespLen,
		void *pvResp);
	uchar(*EA_ucMIFAttriB) (DevHandle hDevHandle, uint uiSendLen, void *pvSendData,
		uint * puiRecLen, void *pvRecData);
	uchar(*EA_ucMIFExchangeB) (DevHandle hDevHandle, uchar ucCID, uint uiSendLen, void *pvSendData,
		uint * puiRecLen, void *pvRecData);
	uchar(*EA_ucMIFDeselectB) (DevHandle hDevHandle, uchar ucCID);
	uchar(*EA_ucMIFHaltB) (DevHandle hDevHandle, ulong ulPUPI);

	uchar(*EA_ucMIFCommandPro) (DevHandle hDevHandle, uint uiSendLen, void *pvSendData,
		uint * puiRecLen, void *pvRecData);
	uchar(*EA_ucMIFCommandB) (DevHandle hDevHandle, uchar ucCID, uint uiSendLen, void *pvSendData,
		uint * puiRecLen, void *pvRecData);

	// 扩展
	uchar(*EA_ucMIFSetTime) (DevHandle hDevHandle, uchar ucTime);
	uchar(*EA_ucMIFAuthExtend) (DevHandle hDevHandle, uchar ucMode, uchar ucAddr);
	uchar(*EA_ucMIFAuthToRamExtend) (DevHandle hDevHandle, uchar ucAddr, uchar ucKeyType,
		uchar * pucKey);
	uchar(*EA_ucMIFSetRegVal) (DevHandle hDevHandle, uchar ucAddr, uchar ucVal);

	// 新增API操作
	uchar(*EA_ucMIFPollMifCard) (DevHandle hDevHandle, uchar ucAntiFlag, uchar * pucMifCardType);
	uchar(*EA_ucMIFActivate) (DevHandle hDevHandle, uchar ucMifType, uint * puiOutLen, void *pvOut);
	uchar(*EA_ucMIFAPDU) (DevHandle hDevHandle, uint uiSendLen, void *pvInData, uint * puiRecLen,
		void *pvOutData);
	uchar(*EA_ucMIFDeactivate) (DevHandle hDevHandle);

	void *pvMifsExpand;
} ET_MIFS_BASE_EXPORT;

typedef struct
{
	uchar(*EA_ucMifsOpen) (uchar ucOpenMode);
	uchar(*EA_ucMifsClose) (void);
	void *Reserved;
} ET_MIFS_IODRV_EXPORT;

typedef struct
{
	ET_MIFS_BASE_EXPORT *ptBase;
	ET_MIFS_IODRV_EXPORT *ptIODrv;
	void *Reserved;
} ET_MIFS_EXPORT_TABLE;

/**********************************************************************

                         API函数原型
                          
***********************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

	uchar EA_ucMifsOpen(uchar ucOpenMode);
	uchar EA_ucMifsClose(void);
	uchar EA_ucMIFSetTime(DevHandle hDevHandle, uchar ucTime);

	// Mifare卡
	uchar EA_ucMIFRequest(DevHandle hDevHandle, uchar ucMode, ushort * pusTagType);
	uchar EA_ucMIFAntiColl(DevHandle hDevHandle, ulong * pulSnr);
	uchar EA_ucMIFSelect(DevHandle hDevHandle, ulong ulSnr, uint * puiSize);
	uchar EA_ucMIFAuthentication(DevHandle hDevHandle, uchar ucMode, uchar ucSecNr);
	uchar EA_ucMIFAuthToRam(DevHandle hDevHandle, uchar ucSecNr, uchar ucKeyType, uchar * pucKey);
	uchar EA_ucMIFLoadKey(DevHandle hDevHandle, uchar ucMode, uchar * pucNKey);
	uchar EA_ucMIFHalt(DevHandle hDevHandle);
	uchar EA_ucMIFRead(DevHandle hDevHandle, uchar ucAddr, uchar * pucData);
	uchar EA_ucMIFWrite(DevHandle hDevHandle, uchar ucAddr, uchar * pucData);
	uchar EA_ucMIFIncrement(DevHandle hDevHandle, uchar ucAddr, ulong ulValue);
	uchar EA_ucMIFDecrement(DevHandle hDevHandle, uchar ucAddr, ulong ulValue);
	uchar EA_ucMIFRestore(DevHandle hDevHandle, uchar ucAddr);;
	uchar EA_ucMIFTransfer(DevHandle hDevHandle, uchar ucAddr);

	// MifarePro
	uchar EA_ucMIFResetPro(DevHandle hDevHandle, uchar ucMode, uint * puiRespLen, void *pvResp);
	uchar EA_ucMIFExchangePro(DevHandle hDevHandle, uint uiSendLen, void *pvSendData,
		uint * puiRecLen, void *pvRecData);
	uchar EA_ucMIFDeselectPro(DevHandle hDevHandle);
	uchar EA_ucMIFCommandPro(DevHandle hDevHandle, uint uiSendLen, void *pvSendData,
		uint * puiRecLen, void *pvRecData);

	// TYPE B
	uchar EA_ucMIFRequestB(DevHandle hDevHandle, uchar ucState, uchar ucType,
		uchar ucCount, uint * puiRespLen, void *pvResp);
	uchar EA_ucMIFSlotMarkerB(DevHandle hDevHandle, uchar ucNumber, uint * puiRespLen,
		void *pvResp);
	uchar EA_ucMIFAttriB(DevHandle hDevHandle, uint uiSendLen, void *pvSendData, uint * puiRecLen,
		void *pvRecData);
	uchar EA_ucMIFExchangeB(DevHandle hDevHandle, uchar ucCID, uint uiSendLen, void *pvSendData,
		uint * puiRecLen, void *pvRecData);
	uchar EA_ucMIFDeselectB(DevHandle hDevHandle, uchar ucCID);
	uchar EA_ucMIFCommandB(DevHandle hDevHandle, uchar ucCID, uint uiSendLen, void *pvSendData,
		uint * puiRecLen, void *pvRecData);
	uchar EA_ucMIFHaltB(DevHandle hDevHandle, ulong ulPUPI);

	uchar EA_ucMIFAuthExtend(DevHandle hDevHandle, uchar ucMode, uchar ucAddr);
	uchar EA_ucMIFAuthToRamExtend(DevHandle hDevHandle, uchar ucAddr, uchar ucKeyType,
		uchar * pucKey);
	uchar EA_ucMIFSetRegVal(DevHandle hDevHandle, uchar ucAddr, uchar ucVal);

	// 新增API函数 
	uchar EA_ucMIFPollMifCard(DevHandle hDevHandle, uchar ucAntiFlag, uchar * pucMifCardType);
	uchar EA_ucMIFActivate(DevHandle hDevHandle, uchar ucMifType, uint * puiOutLen, void *pvOut);
	uchar EA_ucMIFAPDU(DevHandle hDevHandle, uint uiSendLen, void *pvInData, uint * puiRecLen,
		void *pvOutData);
	uchar EA_ucMIFDeactivate(DevHandle hDevHandle);

	// F01新API
	int rfid_open(uint mode);
	int rfid_close(void);
	int rfid_poll(uint mode, uint *cardtype);
	int rfid_powerup(uint cardtype, uint *puiOutLen, uchar *pvOut);
	int rfid_exchangedata(uint sendlen, uchar *psenddata, uint *precvlen, uchar *precvdata);
	int rfid_powerdown(void);
	int rfid_MIFAuth(uint Sec, uint keytype, uchar *pData);
	int rfid_MIFRead(uint block, uchar *out);
	int rfid_MIFWrite(uint block, uchar *in);
	int rfid_MIFIncrement(uint block, ulong value);
	int rfid_MIFDecrement(uint block, ulong value);
	int rfid_MIFRestore(uint block);
	int rfid_MIFTransfer(uint block);
	int rfid_MIFModify(uint block, const BLKALTER *alterVal);
	void rfid_MIFMakevalue(long value,uchar addr, uchar *out);

	uchar rfid_MIFParseStatus(const ACCESSBIT *pAccess, uchar ucBlkNum);
	uchar rfid_MIFAccessIfAlter(uchar access,uchar authKey);
	uchar rfid_MIFKeyIfAlter(uchar access,uchar authKey);
	void rfid_MIFMakeStatus(uchar ucIndata,uchar ucBlkNum, ACCESSBIT *pAccess);
	int rfid_ParaAdjust(int para, int step);
    int rfid_ResetField(int para);
    int adjust_rfid_PwrFieldSetTime(int mode);
    int adjust_rfid_PwrField(int mode);
//    int rfid_type_menu(int mode);
    int ReadRfid_Para_Get(int paramIndex, int *param);
    int WriteRfid_Para_Set(int paramIndex, int param);

	
#ifdef __cplusplus
}
#endif

#endif									//_MIFS_H_
