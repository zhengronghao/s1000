#ifndef LIB_EMV_H_
#define LIB_EMV_H_

//#include <time.h>
#include "wp30_ctrl.h"
#include "emv.h"
//#include "emv_public_func.h"
// #include "debug.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

extern int lcd_EmvDisp(s_lcd_emv * lcd_emvdis, int timeout);


// 联机返回值
#define ONLINE_APPROVE    		0x00       	// 后台批准交易
#define ONLINE_REFER      		0x02       	// 后台要求参考
#define ONLINE_DENIAL     		0x03       	// 后台拒绝交易
#define ONLINE_TIMEOUT	  		0x04		// 连接后台超时
#define ONLINE_OTHER_ERROR  	0x05		// 从后台接收到错误数据，校验错等		

//	返回值定义	
#define EMV_RET_ERROR_OK  					0		// 批准交易或者操作成功
#define EMV_RET_ERROR_PARA				-1		// 参数错误
#define EMV_RET_ERROR_CHECKSUM			-2		// 校验和错
#define EMV_RET_ERROR_DATA				-3		// 错误的数据
#define EMV_RET_ERROR_ICCARD				-4		//与IC 通信失败
#define EMV_RET_ERROR_SW					-5		// IC卡返回错误状态值
#define EMV_RET_ERROR_CARD_BLOCKED		-6		// IC卡被锁定
#define EMV_RET_ERROR_APP_BLOCKED		-7		// 卡内应用被锁定
#define EMV_RET_ERROR_OVERFLOW			-8		// 内存溢出
#define EMV_RET_ERROR_NO_APP				-9		// 卡内无该文件
#define EMV_RET_ERROR_NOT_ACCEPTED		-10		// 应用不被接受
#define EMV_RET_ERROR_USER_CANCEL		-11		// 操作员取消交易
#define EMV_RET_ERROR_NO_LOG				-12		// 卡内无记录
#define EMV_RET_ERROR_DATA_MISSING		-13		// 必要数据缺失
#define EMV_RET_ERROR_EXPIRED			-14		// 公钥过期或者应用过期
#define EMV_RET_ERROR_BYPASSPIN       		-15     	// 取消PIN输入
#define EMV_RET_ERROR_FORCE_ACCEPT		-16     	// 操作者强制批准交易
#define EMV_RET_ERROR_PIN_BLOCKED		-17		// pin验证被锁定
#define EMV_RET_ERROR_DECLINE				-18		// IC卡拒绝交易
#define EMV_RET_ERROR_TERMINATE				-19		// 终止交易

//	常用结构定义
typedef struct				// 终端固有数据元结构
{
	uchar	TermID[8];          		// Terminal identifier
	uchar	MerchantID[15];     	// Merchant identifier
	uchar	MerchCatCode[2];    	// Merchant category code
	uchar	RFU;				// RFU
	ushort	MCHLOCLen;          	// the length of Merchant Name and Location
	uchar  	MCHNameLoc[256];    	// Merchant Name and Location    
	uchar  	TermCntrCode[2];    	// Terminal country code
	uchar  	TransCurCode[2];    	// Transaction currency code
	uchar  	ReferCurCode[2];    	// Transaction Reference Currency Code
	uchar  	TransCurExp;        	// Transation Currency Exponent
	uchar  	ReferCurExp;        	// Transation Reference Currency Exponent
	ushort 	ReferCurrCon;       	// Transaction Reference Currency Conversion
	uchar	tRFU[2];
}EMV_TERM_FIX_DATA;

int emv_set_terminal_data(EMV_TERM_FIX_DATA *pData);
int emv_get_terminal_data(EMV_TERM_FIX_DATA *pData);

typedef struct
{
	uint		TargetPer;           	// 目标百分比数
	uint		MaxTargetPer;        	// 最大目标百分比数
	uint		FloorLimit;          		// 最低限额
	uint		Threshold;           	// 阀值
	uint        ECTransLimit;           // 终端电子现金交易限额
	uint        QPBOCTransLimit;        // 非接触交易限额
	uint        QPBOCFloorLimit;        // 非接触脱机最低限额
	uint        QPBOCCVMLimit;          // 执行CVM限额
	uchar	TACDenial[5];        	// 终端行为代码(拒绝)
	uchar	TACOnline[5];        	// 终端行为代码(联机)
	uchar	TACDefault[5];       	// 终端行为代码(缺省)
	uchar	AcquierId[6];        	// 收单行标志
	uchar	LenOfdDOL;           	// 终端缺省DDOL长度
	uchar	dDOL[252];           	// 终端缺省DDOL
	uchar	LenOftDOL;           	// 终端缺省TDOL长度
	uchar	tDOL[252];           	// 终端缺省TDOL
	uchar	Version[2];          		// 应用版本
	uchar	RiskManDataLen;      	// 风险管理数据长度
	uchar	RiskManData[8];      	// 风险管理数据
	uchar	tRFU[2];
}EMV_APP_INIT_DATA;

int emv_set_app_init_data(EMV_APP_INIT_DATA *pData);
int emv_get_app_init_data(EMV_APP_INIT_DATA *pData);

// MCK 配置结构 
typedef struct
{ // 按ICS中出现的先后顺序排列
	uchar   cTmType;          	// 终端类型     (默认为0x22)
	uchar   tTmCap[3];       	// 终端性能     (默认为 0xE0,0xF8,0xC8)
	uchar   tTmCapAd[5];     	// 附加终端性能 (默认为 0x6F,0x00,0xF0,0xF0,0x01)	
	uchar   cSupPSESelect;    	// PSE选择方式  (默认为 支持)  

	uchar   cSupAppConfirm;   	// 是否支持持卡人确认及选择应用 (默认为 支持)
	uchar   cSupAppDispOrder; 	// 支持按指定顺序显示候选应用   (默认为 支持)
	uchar   cSupMulLang;      	// 支持多语言    (默认为 不支持)
	uchar   cSupIssCertCheck ;	// IssCert 检查 (默认为 支持) 
	uchar   cSupDDOLFlg ;      	// 支持默认DDOL (默认为 支持)      

	uchar   cSupBypassPIN;     	// Bypass PIN, 可跳过密码 (默认为 支持)    
	uchar   cSupFlrLmtCheck ;  	// 最低限额检查 (默认为 支持)   
	uchar   cSupRndOnlineSlt;  	// 随机联机选择 (默认为 支持)
	uchar   cSupVeloCheck;     	// 频度检查     (默认为 支持) 
	uchar   cSupTransLog;      	// 交易流水     (默认为 支持)

	uchar   cSupExcptCheck ;   	// 异常文件检查 (默认为 支持)
	uchar   cTmRmCheckFlg ;    	// 终端风险管理的执行是否基于卡片AIP  (默认为 是)               
	uchar   cSupTmActionCode;  	// 是否支持终端行为代码 (默认为 支持)
	uchar   cSupForceOnline;    // 交易强制联机能力 (默认为 支持)
	uchar   cSupForceAccept;    // 交易强制接受能力 (默认为 支持)

	uchar   cSupAdvice;         // 终端是否支持通知 (默认为 支持)
	uchar   cSupIssVoiceRef;    // 支持发卡行发起的参考 (默认为 支持)
	uchar   cSupCardVoiceRef;   // 支持卡片发起的参考 (默认为 支持)
	uchar   cSupBatchCollect;   // 支持批数据采集  (默认为 支持)
	uchar   cSupOnlineCollect;  // 支持联机数据采集(默认为 不支持)             

	uchar   cSupTDOLFlg ;       // 支持默认TDOL     (默认为 支持)                
	uchar   cPOSEntryMode;  	// POS　Entry Code  (默认为0x25)
	uchar   tHashVal[20];       // 与各配置对应的hash值	
}TermMCK;  // 1-> 支持

int emv_set_mck(TermMCK *pMCK);	// 配置终端MCK
int emv_get_mck(TermMCK *pMCK);	// 获取终端当前的MCK

// typedef struct
// {
// 	uchar   cLen;            // AID名称的长度
// 	uchar   tAID[16];	     // AID名称
// 	uchar   SelFlag;         // 选择标志(1 部分匹配  0 全匹配)	
// 	uchar   tRFU[2];
// }DAidList;

void  emv_init_aidlist(void);								// 初始化AID列表，删除原有的所有AID信息
int emv_set_aidlist(int Num, DAidList *pAidList); 	// 设置AID列表，一次性输入所有的AID信息
int emv_add_aid_item(uchar AidLen, uchar* pAid, uchar SelFlag); // 增加一个AID信息
int emv_get_aid_num(void);							// 返回当前保存的AID数目
int emv_del_aid_item(uchar AidLen, uchar* pAid);		// 删除一个AID
int emv_get_aid_by_index(int index, DAidList *pAidList); // 获取第index个AID信息
int emv_check_aid(uchar AidLen, uchar* pAid);			// 检测该AID是否存在

// typedef  struct
// { 
// 	uchar   cRid[5];  		// RID
// 	uchar   cIndex;   		// 公钥索引	
// 	uchar   cModLen;       	// 公钥模的长度
// 	uchar   cMod[248]; 		// 公钥模	
// 	uchar   cExpLen;        // 公钥指数的长度
// 	uchar   cExp[3];        // 公钥指数
// 	uchar   cExpDate[4];    // 公钥失效期, YYYYMMDD
// 	uchar   cHashFlg;       // 公钥校验和是否存在
// 	uchar   cHash[20];      // 公钥校验和 - 用于定期检查, 可不赋值
// }PKFILESTRU;

void emv_init_capklist(void);								// 初始化CAPK列表，删除原有的所有CAPK
int emv_del_capk(uchar *pRid, uchar cIndex);			// 删除一个CAPK
int emv_add_capk_item(PKFILESTRU  *pCAPKStru);		// 增加一个CAPK
int emv_change_capk_expdate(uchar *pRid, uchar cIndex, uchar *pExpDate); // 更改指定CAPK的过期日期
int emv_get_capk_num(void);							// 获取CAPK总数目
int emv_get_capk_item(uchar *pRid, uchar cIndex, PKFILESTRU *cpkeystru);	// 根据RID和Index获取CAPK
int emv_get_capk_by_index(uchar cNo, PKFILESTRU *pCAPKeyInfo);			// 获取第cNo个CAPK

typedef struct
{
    uchar PANLen;
    uchar Pan[10];
    uchar PANSn;	
}ExceptFile;

void emv_init_except_files(void);	// 清除所有的黑名单
int emv_add_except_file_item(uchar PANLen, uchar *Pan, uchar PANSn);	// 增加一个黑名单
int emv_del_except_file_item(uchar PANLen, uchar *Pan, uchar PANSn);		// 删除一个黑名单
int emv_get_except_file_num(void);	// 获取黑名单个数
int emv_check_except_file(uchar PANLen, uchar *Pan, uchar PANSn);	// 检测该Pan是否处于黑名单中
int emv_set_except_file_list(int Num, ExceptFile *pExceptFile);		// 一次性设置所有的黑名单
int emv_get_except_file_list(int Num, ExceptFile *pExceptFile);		// 一次性获取多个黑名单

typedef  struct 
{
	uchar   cIndex;   		        // 公钥索引
	uchar   cRid[5];  		        // RID
	uchar   cCertSn[3];             // 证书序列号
	uchar   tRFU[3];                // RFU,将结构字节数填充为4的整数倍
}IssCRL;

void emv_init_isscrl(void);		// 清除所有的发卡行回收证书列表
int emv_add_isscrl_item(uchar cIndex, uchar *pRid, uchar *pCertSn);	// 增加一个回收证书
int emv_del_isscrl_item(uchar cIndex, uchar *pRid, uchar *pCertSn);	// 删除一个回收证书
int emv_get_isscrl_num(void);	// 获取回收证书个数
int emv_check_isscrl(uchar cIndex, uchar *pRid, uchar *pCertSn); // 检测该公钥是否已被回收
int emv_set_isscrl_list(int Num, IssCRL *pCert);	// 一次性设置所有的回收证书
int emv_get_isscrl_list(int Num, IssCRL *pCert);	// 一次性获取多次回收证书

// 交易记录操作
typedef  struct
{
	uchar    PanLen;            // Pan号长度
	uchar    Pan[10];   		// 卡Pan号
	uchar    PANSn;             // Pan序列号
	uint     TransAmount;  	    // 交易金额
	uchar    TransTime[6];      // 交易时间YYMMDDHHMMSS
	uchar    tRFU[2];			// 预留
}TransLog;

void emv_init_translogs(void);
int emv_add_translog_item(TransLog *pLog);
int emv_get_translogs(int LogNum, TransLog *pLog);
int emv_get_translog_item(TransLog *pLog);
int emv_get_translogs_num(void);
int emv_get_transerr_log(char *pData, int *pnDataLen);

void emv_init_all_data_elements(void);		// 初始化所有的数据元，包含终端数据元和IC卡数据元
void emv_init_icc_data_elements(void);	// 初始化所有的IC卡数据元
int emv_set_data_element(ushort Tag, int pLen, uchar *pVal);	// 设置一个数据元
int emv_get_data_element(ushort Tag, int *pLen, uchar *pVal);	// 获取一个数据元
int  emv_check_data_element(ushort Tag);	// 检测该数据元是否已设置
int emv_del_data_element(ushort Tag);	// 删除一个数据元

typedef struct
{
	uchar   tCandAppName[33];
	uchar   cFlgAPID;
	uchar   cAPID;
	uchar   tRFU;
}EMV_CANDAPP_LIST;

/* 定义AID列表的结构 */
typedef struct
{
	uchar   tAID[16];               // AID
	uchar   cLenAID;
	uchar   tAppLabel[16];          // 应用标签
	uchar   cLenLabel;
	uchar   tAPN[16];  	            // 应用首选名
	uchar   cLenAPN;
	uchar   cAPID;                  // 应用优先权标识符
	uchar   cFlgAPID;               // 应用优先权标识符标志
	uchar   sLangPref[8];           // 首选语言
	uchar   cLenLangPref;
	uchar   cIssCTIndex;            // 发卡行代码表索引
	uchar   cFlgIssCTIndex;
	uchar   cFlgPreferNameFlg;      // 是否使用应用主选名
	uchar   cLenPDOL;
	uchar   tPDOL[128];             // 原来是256
}DCandList_ST;

void emv_Get_All_CandAppInfo(uint *CandAppNum, EMV_CANDAPP_LIST *pCandApp);
int emv_Get_CandApp(int cIndex, DCandList_ST *pCandList);

// EMV交易接口
int emv_cardslot_init(long CardSlot);
void emv_set_pse_type(int nFlag);
int emv_get_pse_type(void);
int emv_application_select(uint TransNo);
int emv_read_app_data(void);
int emv_offline_data_auth(void);
int emv_cardholder_verify(void);
void emv_process_restriction(void);
int emv_term_risk_manage(void);
int emv_complete_transaction(void);

int emv_application_select_read_log(uint TransNo);
int ReadLogRecord(int RecordNo);
int GetLogItem(uchar *Tag, uchar TagLen, uchar *Value, int *ValueLen);
int ReadLoadLogRecord(int RecordNo);
int GetLoadLogItem(char *Tag, uchar TagLen, uchar *Value, int *ValueLen);
int GetLoadLogHead(uchar *Value);
int ReadLoadLogAll(uchar *pLoadLog, int *pLogLen);

int emv_check_force_accept(void);
int emv_get_script_result(uchar *Result, int *RetLen);
int emv_check_print_signature(void);
int emv_del_script_result(void);
int emv_preonline_transaction(uchar *pucFlag);
int emv_onlineback_transaction(uchar ucProcFlag, int nOnlineResult, uchar *pRspCode,uchar *pAuthCode, int *pAuthCodeLen,
	uchar  *pIAuthData, int *pIAuthDataLen, uchar *pScript1, int *pScriptLen1, uchar *pScript2, int *pScriptLen2);

//为了处理闪卡新增
int emv_read_last_record(void);
int emv_Init_App_Process(void);
int emv_read_app_data_QA(void);
int emv_get_cardno_sfi(int *pnSFI, int *pnRecord);
int emv_read_cardno_data(int nSFI, int nRecord, char *pCard, int *pCardLen);
int emv_check_QA_occur(void);
void emv_clear_QA_flag(void);
void emv_set_QA_flag(void);
int  emv_Get_OdaList_QA(uchar *pDt, int *pLen, int *pNum);
int  emv_Set_OdaList_QA(uchar *pDt, int nLen, int nNum);

//qPBOC交易接口
int qPBOC_offline_data_auth(void);
int qPBOC_term_risk_manage(void);
int qPBOC_complete_transaction(void);
int qPBOC_trans_preprocess(void);
int qPBOC_check_except_file(uchar PANLen, uchar *Pan);  // 检测该Pan是否处于黑名单中
int qPBOC_preonline_transaction(uchar *pucFlag);
int qPBOC_onlineback_transaction(int nOnlineResult, uchar *pRspCode);

//卡片指令
int emv_icc_get_data(int p1, int p2, uchar *DataOut, int *OutLen);
ushort emv_get_last_iccard_sw(void);


typedef struct EMV_ICAdapterStru
{
	long  (*glICAdapterSelectCard)(long lIndex);
	long  (*glICAdapterCardIn)(long);
	long  (*glICAdapterPowerUp)(long,long *, char *);
	long  (*glICAdapterPowerDown)(long);
	long  (*glICAdapterExchangeAPDU)(long, long , char *, long *, char *);
}EMV_ICAdapterStruDef;
long emv_ICAdapterSet(EMV_ICAdapterStruDef tEMVICAdapter);
#endif

