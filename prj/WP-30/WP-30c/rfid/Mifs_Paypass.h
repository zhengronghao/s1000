/*********************************************************************
* 版权所有 (C)2003, 福建实达电脑设备有限公司。
* 文件名称： 
*     Mifs_PAYPASS.h
* 当前版本： 
*     01-01-01
* 内容摘要： 
*     本文件定义了符合PAYPASS要求操作函数及数据结构
* 历史纪录： 
*     修改人		      日期			    	   版本号
*                  02-22-2006 	        01-01-01  
******************************************************************/
 // 只包含一次
#ifndef _MIFS_PAYPASS_
#define _MIFS_PAYPASS_

#include "wp30_ctrl.h"

//=========================================
typedef struct 
{
    uint8_t RxThresholdTypeA;//RxThresholdReg
    uint8_t RxThresholdTypeB;//RxThresholdReg
    uint8_t Demod;       //DemodReg
	uint8_t GsNOff; //GsNOffReg
    uint8_t RFCfg;  //RFCfgReg
	uint8_t GsNOn;	//GsNOnReg
	uint8_t CWGsP;	//CWGsPReg
	uint8_t ModGsP; //ModGsPReg
//    uint8_t rfu[3];
}MIFS_REG;
#ifdef EM_PN512_Module
extern MIFS_REG gcMifReg;
#endif


/***********************************************************************
                            常量定义
************************************************************************/
#define  EM_mifs_NODELAY     (uint)0x0000
#define  EM_mifs_LOOP        (uint)0xFFFF

#define  EM_mifs_FWTMIN       9			// 用于WUPA、ANTICOLLISION和SELECT命令的等待响应时间
									    // 实际上应该是9.2ETU								
#define  EM_mifs_FWTRATS      560		// 用于等待RATS帧
#define  EM_mifs_FWTDESELECT  560		// 用于等待DESELECT响应帧
#define  EM_mifs_FWTWUPB      60		// 用于等待WUPB响应帧
#define  EM_mifs_MAXFWT       8960		// FWT最大时间为32 * 2^8 + 3 * 2^8 = 8960ETU
#define  EM_mifs_DEFAULT      560		// 缺省帧等待时间为560ETU
#define  EM_mifs_TPDEALY     (uint)500*2	// 切换调制方式时延时500个ETU
#define  EM_mifs_TRESET      (uint)1000	// 复位PICC时需延时1000ETU
#define  EM_mifs_FDTMIN      (uint)60	// 用于保证PCD发送下一条命令帧前的500us延时

#define  EM_mifs_FSD_Default  256		//默认FSD
#define  EM_mifs_FSC_Default  32		//默认FSC


// PCD操作时间
#define  TIME_WAIT_TReset          8   //8ms  5.1ms~10ms  Poll复位工作场
#define  TIME_WAIT_TResetDelay     25  //25ms 0~33ms      Col复位工作场
#define  TIME_WAIT_Tp              7   //8ms  5.1ms~10ms  载波开启后延迟
#define  TIME_WAIT_Resend          6   //1ms  0~33ms      重发命令延迟

#define  PCD_TRYON_TIMES           3   //除poll和remove过程外命令超时时最多尝试3次

/**********************************************************************

                         内部返回值定义
                          
***********************************************************************/

//精确到1ms
#define rfid_delayms(ms)   s_DelayMs(ms)
/**********************************************************************

                         全局函数原型
                          
***********************************************************************/
void EI_paypass_vInit(void);
void EI_paypass_vSetTimer(ulong ulETU);	// 设置超时时限
void EI_paypass_vDelay(ulong ulETU);	// 延时
uchar EI_paypass_ucWUPA(uchar * pucResp);	// 唤醒TypeA卡
uchar EI_paypass_ucAnticoll(uchar ucSEL, uchar * pucUID);	// TypeA卡防冲突
uchar EI_paypass_ucSelect(uchar ucSEL, uchar * pucUID, uchar * pucSAK);	// 选择TypeA进入ACTIVATE状态
uchar EI_paypass_ucRATS(uchar * pucOutLen, uchar * pucATSData);	// 发送RATS命令，使TypeA卡进入Protocol状态
void EI_paypass_vHALTA(void);			// 使TypeA卡进入HALT状态
void EI_paypass_vResetPICC(void);		// 关闭RC531载波，复位所有的PICCs
void EI_paypass_vOptField(uchar mode);
uchar EI_paypass_ucWUPB(uchar * pucOutLen, uchar * pucOut);	// 唤醒TypeB卡
uchar EI_paypass_ucAttrib(uchar * pucResp);	// 选中TypeB卡，使之进入ACTIVATE状态
uchar EI_paypass_ucHALTB(uchar * pucResp);	// 使TypeB卡进入HALT状态
uchar EI_paypass_ucGetUID(void);		// 获取TypeA卡的完整UID信息
uchar EI_paypass_ucActPro(uchar * pucOutLen, uchar * pucATSData);	// 激活TypeA卡
uchar EI_paypass_ucActTypeB(uchar * pucOutLen, uchar * pucOut);	// 激活TypeB卡
void EI_paypass_vSelectType(uchar ucMifType);	// 选择载波调制方法
uchar EI_paypass_ucPOLL(uchar * pucMifType);	// 判断感应区内是否有卡
uchar EI_paypass_ucIfMifExit(void);		// 判断Mif卡是否还在感应区
uchar EI_paypass_ucExchange(uint uiSendLen, uchar * pucInData, uint * puiRecLen,
	uchar * pucOutData);
uchar EI_paypass_ucDeSelect(void);		// DESELECT命令
uchar EI_paypass_ucProcess(uint uiSendLen, uint * puiRecLen);	// 命令处理
uchar EI_paypass_ucMifRBlock(uint * puiRecLen);	// 处理R_Block
uchar EI_paypass_ucMifSBlock(uint * puiRecLen);	// 处理S_Block
uchar EI_ucAnticoll(uchar ucSEL, uchar * pucUID);
uint EI_rfid_GetFramesize(uchar fsdi);
void s_rfid_init(void);
uint s_rfid_FWI2FWT(uint FWI); 


#endif
