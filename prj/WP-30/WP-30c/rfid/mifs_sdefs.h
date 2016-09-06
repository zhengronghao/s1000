/*********************************************************************
* 版权所有 (C)2003, 福建实达电脑设备有限公司。
* 文件名称： 
*   Mifs_SDEFS.h
* 当前版本： 
*   01-01-01
* 内容摘要： 
*   本文件实现非接触卡模块一些数据结构的定义
* 历史纪录：
*   修改人          日期                    版本号      修改说明
*                06-27-2003              01-01-01
******************************************************************/
 // 只包含一次
#ifndef _MIFS_SDEFS_H_
#define _MIFS_SDEFS_H_

#include "wp30_ctrl.h"

/**********************************************************************

                         常量定义
                          
***********************************************************************/
#define EM_mifs_BUFFER_LEN                  (uint)(272)
#define EM_mifs_KEYNUM                      16	// EEPROM中最多可以存放的每套密钥数量

#define EM_mifs_MIFARE_SECTORNUM            16	// Mifare卡扇区数
#define EM_mifs_MIFARE_BLOCKNUMPERSECTOR    4	// Mifare卡每个扇区块数
#define EM_mifs_FWI_MAXVALUE                14	// 最大FWI值

/**********************************************************************

                         PCD命令集合
                          
***********************************************************************/
#define EM_mifs_PCD_IDLE           0x00	// No action: cancel current command
								// or home state, respectively
#define EM_mifs_PCD_WRITEE2        0x01	// Get data from FIFO and write it to the E2PROM
#define EM_mifs_PCD_READE2         0x03	// Read data from E2PROM and put it into the
								// FIFO
#define EM_mifs_PCD_LOADCONFIG     0x07	// Read data from E2PROM and initialise the
								// registers
#define EM_mifs_PCD_LOADKEYE2      0x0B	// Read a master key from the E2PROM and put
								// it into the master key buffer
#define EM_mifs_PCD_AUTHENT1       0x0C	// Perform the first part of the card
								// authentication using the Crypto1 algorithm.
								// Remark: The master key is automatically taken 
								// from 
								// the master key buffer. this implies, that the
								// command LoadKeyE2 has to be executed before 
								// to use a certain key for card authentication
#define EM_mifs_PCD_CALCCRC        0x12	// Activate the CRC-Coprocessor
								// Remark: The result of the CRC calculation can
								// be read from the register CRCResultXXX
#define EM_mifs_PCD_AUTHENT2       0x14	// Perform the second part of the card
								// authentication using the Crypto1 algorithm.
#define EM_mifs_PCD_RECEIVE        0x16	// Activate Receiver Circuitry. Before the
								// receiver actually starts, the state machine 
								// waits until the time 
								// configured in the register RcvWait has 
								// passed.
								// Remark: It is possible to read data from the 
								// FIFO although Receive is active. Thus it is 
								// possible to receive any number of bytes by 
								// reading them from the FIFO in time.
#define EM_mifs_PCD_LOADKEY        0x19	// Read a master key from the FIFO and put it
								// into the master key buffer
								// Remark: The master key has to be prepared in
								// a certain format. Thus, 12 byte have to be 
								// passed to load a 6 byte master key
#define EM_mifs_PCD_TRANSMIT       0x1A	// Transmit data from FIFO to the card
								// Remark: If data is already in the FIFO when 
								// the command is activated, this data is 
								// transmitted immediately. It is possible to 
								// write data to the FIFO although Transmit 
								// is active. Thus it is possible to transmitt 
								// any number of bytes by writting them to the 
								// FIFO in time. 
#define EM_mifs_PCD_TRANSCEIVE     0x1E	// Transmit data from FIFO to the card and after
								// that automatically activates the receiver. 
								// Before the receiver actually starts, the 
								// STATE MACHINE waits until the time configured
								//  in the register RcvWait has passed.
								// Remark: This command is the combination of 
								// Transmit and Receive
#define EM_mifs_PCD_RESETPHASE     0x3F	// Runs the Reset- and Initialisation Phase
								// Remark: This command can not be activated by 
								// software, but only by a Power-On or 
								// Hard Reset 

/**********************************************************************

                         PICC命令集合
                          
***********************************************************************/
#define EM_mifs_PICC_REQSTD         0x26	// request idle
#define EM_mifs_PICC_REQALL         0x52	// request all
#define EM_mifs_PICC_ANTICOLL1      0x93	// anticollision level 1
#define EM_mifs_PICC_ANTICOLL2      0x95	// anticollision level 2
#define EM_mifs_PICC_ANTICOLL3      0x97	// anticollision level 3
#define EM_mifs_PICC_AUTHENT1A      0x60	// authentication step 1
#define EM_mifs_PICC_AUTHENT1B      0x61	// authentication step 2
#define EM_mifs_PICC_READ           0x30	// read block
#define EM_mifs_PICC_WRITE          0xA0	// write block
#define EM_mifs_PICC_DECREMENT      0xC0	// decrement value
#define EM_mifs_PICC_INCREMENT      0xC1	// increment value
#define EM_mifs_PICC_RESTORE        0xC2	// restore command code
#define EM_mifs_PICC_TRANSFER       0xB0	// transfer command code
#define EM_mifs_PICC_HALT           0x50	// halt
#define EM_mifs_PICC_DESELECT       0xCA	// DESELECT command code
#define EM_mifs_PICC_RATS           0xE0
// T Y P E  B  -  C O M M A N D S 
// commands which are handled by the tag
// Each tag command is written to the reader IC and transfered via RF
#define EM_mifs_PICC_REQB           0x05	// request
#define EM_mifs_PICC_REQBSTD        0x00	// request normal
#define EM_mifs_PICC_REQBWUP        0x08	// request wakeup
#define EM_mifs_PICC_ATTRIB         0x1D	// Attrib命令码
#define EM_mifs_PICC_SLOTMARKER     0x05	// Slot-Marker命令码低4位



#define RFID_ACTIVE_NONE    0   //未poll
#define RFID_ACTIVE_POLLOK  1   //未激活
#define RFID_ACTIVE_OK      2   //激活

#define RFID_PARA_PWRSETTIME      0        //参数场强建立时间
	#define RFID_DEFAULT_PWRSETTIME   15   //默认场强建立时间
	#define RFID_MAX_PWRSETTIME		  30   //最大时间

#define RFID_PARA_PWR      1        //参数场强大小等级
	#define RFID_PWR_LEVEL0   0   //场强等级0 最弱
	#define RFID_PWR_LEVEL1   1   //场强等级1 
	#define RFID_PWR_LEVEL2   2   //场强等级2 
	#define RFID_PWR_LEVEL3   3   //场强等级3 最强
	#define RFID_DEFAULT_PWR  RFID_PWR_LEVEL1   //默认场强大小
	#define RFID_MAX_PWR	  RFID_PWR_LEVEL3   //最大等级

#define RFID_PARA_CHIP         2        //芯片类型
/**********************************************************************

                         数据结构定义
                          
***********************************************************************/
typedef struct
{
	char acDeviceName[20];				// 打开设备的名称
	uint uiOpenMode;					// 打开设备的方式
} ET_IO_DEVICE;

typedef struct
{
    uint  expectMaxRec;                 // 预计接收数据最大长度
	ulong ulSendBytes;					// 预计发送数据长度
	ulong ulBytesSent;					// 当前已发送字节数
	ulong ulBytesReceived;				// 当前已经接收字节数
	long lBitsReceived;					// 当前已经接收位数
	ulong ulCardID;						// 当前选择的卡号
	ulong ulMaxLen;						// MifarePro卡和type B卡通信最大包长度

	uchar ucCollPos;					// 产生冲突的位置
	uchar ucAnticol;					// 防冲突操作标志
	uchar ucnBits;						// 在防冲突时, 用于表示所要发送的合法数据位数模8的值, 取值范围：0～7
	uchar ucDisableDF;					// 是否忽略接收响应过程产生的错误, 1-忽略 0-不忽略 在选卡命令是才置1

	uchar ucCurResult;					// 在命令处理过程中的结果
	uchar ucINTSource;					// 在命令处理过程中所有发生的中断
	uchar ucErrFlags;					// 在命令执行中产生错误标志
	uchar ucSaveErrState;				// 保存在命令执行中产生错误标志

	uchar ucEnableTransfer;				// 表示允许或者禁止使用Transfer命令, 1- 允许 0－禁止
	uchar ucFirstOpenFlag;				// 第一次调用EA_ucMIFOpen的标志, 0-第一次调用
	uchar ucCurType;					// 当前使用的卡类型号: EM_mifs_TYPEA-TYPE A, EM_mifs_TYPEB-TYPE B
	uchar ucCurPCB;						// 帧号

	vuchar TimeSource;					// CLRC663定时器中断记录
	uchar ucPwrLevel;					// 电源级别
	uchar ucCIDSupport;					// 是否支持有CID: 1-支持, 0-不支持
	uchar ucFWI;						// 超时等待时间设置 
	uchar aucCmdBuf[10];				// 在数值块操作时，存放要发送的命令         

	uchar ucSAK1;						// 第一级SELECT返回SAK值
	uchar ucSAK2;						// 第二级SELECT返回SAK值
	uchar ucSAK3;						// 第三级SELECT返回SAK值

	uchar ucMifCardType;				// 当前卡片类型，0 - S50, 1 - S70, 2 - PRO, 3 - TypeB
	uchar ucMifActivateFlag;			// 射频卡激活标志 0-未poll 1-未激活  2-已激活
	uchar ucUIDLen;						// 射频卡的UID长度，TypeA型卡可能有4、7、10字节，TypeB卡只有4字节
	uchar ucATQA[2];					// 射频卡在WUPA命令中返回的ATQA参数

	uchar ucATQB[12];					// 射频卡在WUPB命令中返回的参数      
	uchar ucUIDCL1[5];					// TypeA第一级序列号，包含BCC字节
	uchar ucUIDCL2[5];					// TypeA第二级序列号，包含BCC字节
	uchar ucUIDCL3[5];					// TypeA第三级序列号，包含BCC字节
	uchar ucUIDB[4];					// TypeB序列号
	uchar ucSFGI;						// TypeA型射频卡，RATS后的正向帧等待时间整数
	uchar ATS[20];                      // TypeA卡ATS

	ushort FIFOSIZE;                    //64-RC531/PN512   512-CLRC663

	uchar RFIDModule;				    //0-RC531  1-PN512  2-CLRC663

	// 不管PICC是否支持CID和NAD机制，
	// 按照PAYPASS要求，在PCD与PICC之间均不使用CID和NAD机制
	uchar ucCIDFlag;					// PICC是否支持CID机制，0 - 不支持， 1 - 支持
	uchar ucNADFlag;					// PICC是否支持NAD机制，0 - 不支持， 1 - 支持
	uchar ucWTX;						// 帧等待时间扩展整数，适用于TypeA和TypeB卡
	uchar ucAnticollFlag;				// 是否允许从多张TypeA卡中获取一张卡片 0 - 不允许 1 - 允许
//	  uchar Frameflg;						// 是否判读卡端帧号0-有判断 1-不判断
//    uchar ucCIDPollFlg;                 // 探测CID是否支持标志 0-CID准备探测 1-CID探测完毕	

	ulong ulSFGT;						// TypeA型射频卡，RATS后的正向帧等待时间， 单位：ETU（128/13.56MHz）
	ulong ulFWT;						// 帧等待时间，适用于TypeA和TypeB卡，单位：ETU（128/13.56MHz）
	ulong ulFWTTemp;					// 帧等待时间扩展，适用于TypeA和TypeB卡，单位：ETU（128/13.56MHz）
	ulong ulTimeout;					// AS3911超时时间单位：ETU（1/13.56MHz）

	uint FSC;							// FSC的值,卡能接收的帧最大长度
	uint FSD;							// FSD的值，终端能接收的帧最大长度  

	int PwrAdTime;					// 寻卡载波建立延迟时间
	uint OptMode;                    // 操作模式 00-ISO 01-EMV模式(应用QPBOC认证使用)
	uchar aucBuffer[EM_mifs_BUFFER_LEN];	// 收发缓冲区    
} ET_mifs_WORKSTRUCT;

extern ET_mifs_WORKSTRUCT EG_mifs_tWorkInfo;	// 工作变量结构


typedef struct{
	uchar  SBlock;      //连续S块数
	uchar  Cmd;         //当前发送的指令 
	uchar  IntEnable;   //备份中断使能寄存器	
	uchar  TimerReload; //备份时钟超时寄存器	
	uchar  FDT;         //FDT 目前只支持WUPA/Select/RATS的FDT判断
	uchar  FDTFlg;      //FDT 提前到来标志 0-正常  1-TypeA提前  2-TypeB 除WUPB命令接收提前
	uchar  ReRecvFlg;   //重新接收标志  RATS/ATTRIB/协议过程如果接收数据少于4B而要重新开始接收
	uchar  AttribeTime;  //

    //CLRC663 flag
	uchar  TimerIntEnable;   //RC663中断使能定时器部分	
//	uchar  Timerflg;         //定时器中断使用标志 0x01-time0,0x02-time1,0x04-time2,0x08-time3
	uchar gPN512NFCIP;
	uchar gMifareKey[6];     //Mifare卡认证密钥key a/b
    uchar gTypeArec;    //as3911 type a接收灵敏度
    uchar gTypeBrec;    //as3911 type b接收灵敏度
}_t_RfidPro_Info_;

extern _t_RfidPro_Info_ gtRfidProInfo;


typedef struct{
	uchar CW_A;           //
	uchar ModWidth_A;     //
	uchar CW_B;           //
	uchar ModConduct_B;  
}_t_RfidDebugInfo_;
extern _t_RfidDebugInfo_ gtRfidDebugInfo;

typedef struct{
	uchar  ARxThreadhold;
	uchar  ARxControl_1;
	uchar  ARxWait;
	uchar  ATxCW;
	uchar  AMFout;
	
	uchar  BRxThreadhold;
	uchar  BRxControl_1;
	uchar  BRxWait;
	uchar  BTxCW;	
	uchar  BMFout;
}_t_RfidReg_Info_;

extern _t_RfidReg_Info_ gtRfidRegInfo;

//typedef struct{
//	uchar gPN512NFCIP;
//	uchar gPN512Buf[7];
//}_t_RfidPn512_Info_;
//
//extern _t_RfidPn512_Info_ gtPN512Info;

#endif									//_MIFS_SDEFS_H_
