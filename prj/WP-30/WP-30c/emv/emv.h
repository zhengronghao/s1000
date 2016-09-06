#ifndef EMV_H_
#define EMV_H_

//#include <time.h>
//#include "../inc/libapi.h"

#define  BYTE        unsigned char    // 单字节(Byte)
#define  WORD        unsigned short   // 字(Word)
#define  DWORD       unsigned long    // 双字(Double Word)
#define  UINT        unsigned int
//#ifndef DevHandle
//typedef unsigned long DevHandle;
//#endif

// 国电EMV返回值定义
#define EMV_RET_OK  					0x00		// 成功
#define EMV_RET_ERROR  					0x01		// 错误
#define EMV_RET_PARA_ERROR				0x8B		// 输入参数错误
#define EMV_RET_OTHER_ERROR             0x8D        // 其他错误
#define EMV_RET_TAG_ERROR               0x32        // 标签错误
#define EMV_RET_TAGLEN_ERROR            0x35        // 无对应的标签(标签或标签长度错误)
#define EMV_RET_TAG_NOEXIST             0x34        // 该数据元尚未赋值
#define EMV_RET_NO_LIST                 0x40        // EMV L2内核中无此数据列表或无此应用
#define EMV_RET_OVER_NUM                0x33        // 超出允许的最大个数
#define EMV_RET_FORMAT_ERROR            0x30        // 格式错误
#define EMV_RET_CARD_BLOCKED            0x43        // 卡片锁定或不支持选择命令
#define EMV_RET_SELFLAG_ERROR           0x42        // 获取部份选择标志出错
#define EMV_RET_STATUS_ERROR            0xE7        // 获取格式数据时，卡片返回的状态码错误
#define EMV_RET_CARDOUT                 0x8F        // 卡片不在位等系统错误
#define EMV_RET_CANDLIST_ERROR          0x52        // 建立候选列表时出现了错误,且含有需特殊处理的应用
#define EMV_RET_APP_BLOCKED             0x44        // 匹配的应用均被锁定
#define EMV_RET_PSE_ERROR               0x74        // PSE方式失败, 需要使用AID列表进行选择
#define EMV_RET_TLV_ERROR               0x48        // 卡片数据的TLV编码错误
#define EMV_RET_GPO_ERROR               0x49        // 卡片在取处理选项应答中，返回的状态码!=9000,并且!= 6985
#define EMV_RET_SW_6985                 0x46        // 卡片在取处理选项应答或GAC命令应答中，返回‘6985’状态码，由应用程序决定终止还是回退
#define EMV_RET_DOL_ERROR               0x47        // 填充PDOL时出错(如卡片返回的PDOL列表格式错误)
#define EMV_RET_READ_ERROR              0x45        // 读数据时错误
#define EMV_RET_DATA_MISSING            0x4A        // 必备的脱机认证数据缺失
#define EMV_RET_GETPK_FAIL              0x4B        // 认证中心公钥获取失败
#define EMV_RET_ISSUER_FAIL             0x4C        // 发卡行公钥恢复错误
#define EMV_RET_SDA_FAIL                0x4D        // 签名静态数据验证失败
#define EMV_RET_INTER_AUTH_ERROR        0x4E        // 内部认证命令出错
#define EMV_RET_DDA_DATA_ERROR          0x4F        // 获得动态认证数据出错
#define EMV_RET_DDA_FAIL                0x50        // 签名动态数据验证失败
#define EMV_RET_ONLINE_PIN              0x01        // 需要联机PIN输入
#define EMV_RET_OFFLINE_PIN             0x02        // 需要脱机PIN输入
#define EMV_RET_CERTIFICATE             0x05        // 需要持卡人证件验证
#define EMV_RET_CID_ERROR               0x51        // 卡片应答的CID数据错误，终止交易
#define EMV_RET_TRANS_CANCEL            0x53        // 持卡人取消交易
#define EMV_RET_TRANS_DECLINE           0x54        // 交易拒绝
#define EMV_RET_TRANS_TERMINATE         0x55        // 交易终止
#define EMV_RET_CHANGE_INTERFACE        0x56        // 交易失败，切换其他界面


//	常用结构定义
//终端固有数据元
typedef struct TM_FIX_DATA
{ 
	BYTE   tTmCntrCode[2];      // 终端国家代码
	BYTE   tAquirerID[6];       // 收单行标识’9F01’ (6字节)
	BYTE   tMerchCatCode[2];    // 商户分类码’9F15’(2字节) 
	BYTE   tTmTransCur[2];      // 终端交易货币代码’5F2A’(2字节) 
	BYTE   cTmTransCurExp;      // 终端交易货币指数’5F36’(1字节)
	BYTE   tTmRefCurCode[2];    // 终端交易参考货币代码’9F3C’(2字节)
	BYTE   tTmRefCurExp;	    // 终端交易参考货币指数’9F3D’(1字节)
	BYTE   tTmRefCurConv[5];    // 终端交易参考货币兑换(5字节) ，字符串形式赋值，如”1000”，以\x00结束
	BYTE   tTmID[8];            // 终端号
	BYTE   tMerchantID[15];     // 商户标识’9F16’(15字节)
	BYTE   MCHLOCLen;           // 商户名称及位置数据域的长度
	BYTE   tMCHNAMELOC[256];    // 商户名称及位置’9F4E’(1-256 字节) 
	BYTE   tTmTransCateCode;    // 终端交易类别代码 0x22
}TmFixData;

//应用相关数据元
typedef struct INIT_EMV_DATA_STRUCT
{ 
	BYTE   tAppVerNo[2];         // 应用版本号’9F09’(2字节)
	BYTE   cTmRmCheckFlg;        // 由收单行确定是否进行终端风险管理检查
	// = 0: 不执行终端风险管理
	// = 1: 强制执行终端风险管理
	// = 2: 由卡片决定是否执行终端风险管理(默认)
	BYTE   tTAC_Denial[5];       // 终端行为代码TAC－拒绝(5字节) 
	BYTE   tTAC_Online[5];       // 终端行为代码TAC－联机(5字节)
	BYTE   tTAC_Default[5];      // 终端行为代码TAC－默认(5字节)  
	BYTE   cLenTDOL;             // TDOL的长度
	BYTE   tTDOL[252];           // TDOL’97’
	BYTE   cLenDDOL;             // DDOL的长度
	BYTE   tDDOL[252];           // DDOL ’9F49’
	BYTE   tFloorLmt[11];        // 最低限额(11字节), 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
	BYTE   tLmt[11];		     // 随机选择阈值, 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
	BYTE   tPercent[3];          // 随机选择目标百分数, 直接以字符串显示赋值, 如”50”，以\x00结束
	BYTE   tMaxPercent[3];       // 随机选择最大目标百分数, 直接以字符串显示赋值, 如”99”，以\x00结束
	BYTE   tECTransLimit[13];    // 终端电子现金交易限额, 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
	BYTE   tQPBOCTransLimit[13]; // 非接触交易限额, 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
	BYTE   tQPBOCFloorLimit[13]; // 非接触脱机最低限额, 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
	BYTE   tQPBOCCVMLimit[13];   // 执行CVM限额, 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
}AppInitData;

//候选列表数据结构
typedef struct TERM_CAND_LIST_STRUCT
{    
	BYTE   tAID[16];             // AID名称
	BYTE   cLenAID;  		     // AID名称的长度
	BYTE   tAppLabel[16];        // 应用标签
	BYTE   cLenLabel;		     // 应用标签的长度
	BYTE   tAPN[16];             // 应用首选名
	BYTE   cLenAPN;              // 应用首选名的长度
	BYTE   cAPID;                // 应用优先权标识符
	BYTE   cFlgAPID;             // 应用优先权标识符存在标志(0-不存在, 1-存在)
	BYTE   sLangPref[8];         // 首选语言
	BYTE   cLenLangPref;         // 首选语言的长度
	BYTE   cIssCTIndex;          // 发卡行代码表索引
	BYTE   cFlgIssCTIndex;       // 发卡行代码表索引存在标志(0-不存在, 1-存在) 
}DCandList;

//交易使用的数据元
typedef struct  _APP_DATA_STRUCT
{
	BYTE   AmntAuth[13];         // 授权金额，字符串形式赋值,以分为单位, 如”50000”，以\x00结束，不足部分前补‘0’
	BYTE   AmntOther[13];        // 其他金额，字符串形式赋值,以分为单位, 如”50000”，以\x00结束，不足部分前补‘0’
	BYTE   cTransType;           // 交易类型’9C’	
	BYTE   tDate[3];             // 交易日期指针(3 BYTE)
	BYTE   tTime[3];             // 交易时间指针(3 BYTE)
	BYTE   tTrace[4];            // 交易序列计数器(4 BYTE)  
}AppDt;

//交易相关数据, 用于单据打印或流水记录
#define  ISS_SCRPT_RSLT_MAX       12*5    // 发卡行脚本结果最大字节数(即12个脚本,每个占用5字节)

typedef struct  TRANS_DATA_STRUCT
{    	// 是否存在标志: 0-不存在, 1-存在
	BYTE   tExpDate[4];          // 失效日期 
	BYTE   cFlgCID;              // 密文信息数据存在标志
	BYTE   cCID;                 // 密文信息数据
	BYTE   cFlgAC;               // 应用密文存在标志
	BYTE   tAC[8];               // 应用密文
	BYTE   cFlgTVR;              // 终端验证结果存在标志
	BYTE   tTVR[5];              // 终端验证结果
	BYTE   cFlgTSI;              // 交易状态信息存在标志
	BYTE   tTSI[2];              // 交易状态信息
	BYTE   cTrack2Len;           // 二磁道等价数据长度
	BYTE   tTrack2[19];          // 二磁道等价数据
	BYTE   cFlgATC;              // 应用交易计数器存在标志
	BYTE   tATC[2];              // 应用交易计数器
	BYTE   cFlgAIP;              // 应用交互特征存在标志
	BYTE   tAIP[2];              // 应用交互特征
	BYTE   cFlgAUC;              // 应用用途控制存在标志
	BYTE   tAUC[2];              // 应用用途控制
	BYTE   cFlgCVMR;             // 持卡人验证方法结果存在标志
	BYTE   tCVMR[3];             // 持卡人验证方法结果
	BYTE   cFlgIAC_Denial;       // 发卡行行为代码－拒绝存在标志
	BYTE   tIAC_Denial[5];       // 发卡行行为代码－拒绝
	BYTE   cFlgIAC_Online;       // 发卡行行为代码－联机存在标志
	BYTE   tIAC_Online[5];       // 发卡行行为代码－联机
	BYTE   cFlgIAC_Default;      // 发卡行行为代码－默认存在标志
	BYTE   tIAC_Default[5];      // 发卡行行为代码－默认
	BYTE   tIssSrptRslt[ISS_SCRPT_RSLT_MAX];    // 发卡行脚本结果 
	BYTE   cNumIssSrptRslt;      // 发卡行脚本结果长度
	BYTE   tUnpreNumber[4];      // 不可预知数
	BYTE   tARC[2];              // ARC
	BYTE   cFlgSign;             // 签名标志
	BYTE   cLenIssAppDt;         // 发卡行应用数据长度 
	BYTE   tIssAppDt[32];        // 发卡行应用数据 
}TransDt;

//CA公钥数据结构
#define  MAX_MODULUS_LEN  248
typedef  struct PUBLIC_KEY_FILE
{ 
	BYTE   cRid[5];  		     // RID
	BYTE   cIndex;   		     // CA公钥索引	
	BYTE   cModLen;       	     // CA公钥模的长度
	BYTE   cMod[MAX_MODULUS_LEN];// CA公钥模	
	BYTE   cExpLen;              // CA公钥指数的长度(只能为1或3)
	BYTE   cExp[3];              // CA公钥指数(只能为”0x03”或”\x01\x00\x01”)
	BYTE   cExpDate[4];          // CA公钥失效期, 格式为:YYYYMMDD. 
	BYTE   cHashFlg;             // CA公钥校验和存在标志(0-不存在, 1-存在)
	BYTE   cHash[32];            // CA公钥校验和
}PKFILESTRU;

//发卡行公钥证书回收列表结构
typedef  struct CERTIFICATE_FILE
{
	BYTE   cIndex;   		     // CA公钥索引
	BYTE   cRid[5];  		     // RID
	BYTE   cCertSn[3];           // 回收的发卡行公钥证书序列号
	BYTE   tRFU[3];              // 保留
}CertFile;

//应用标识列表结构
typedef struct TERM_AID_LIST_STRUCT
{
	BYTE  cLen;                  // AID名称的长度
	BYTE  tAID[16];              // AID名称数据
	BYTE  SelFlag;	             // 应用选择指示符(1 部分匹配  0 全匹配)
	BYTE  tRFU[2];               // 保留
}DAidList;

//卡片日志信息结构
typedef struct TERM_CL_LIST_STRUCT
{
	BYTE   tAid[16];             // 应用标签
	BYTE   cLenAID;		         // 应用标签长度
	BYTE   cLogFSI;		         // 卡片日志SFI
	BYTE   cLogNum;		         // 卡片日志记录个数
	BYTE   cAppIsLock;	         // 该应用是否被锁定(0-未锁定; 1-锁定)
}DClList;

//IC卡卡座属性结构
typedef struct ICCARD_STATUS
{
	char cTP;	         	     // 传输协议标识(0: T=0字符, 1: T=1块帧)
	char cCardType;	             // IC卡卡座类型
	DevHandle hCardHandle;       // IC卡卡座句柄 
}DCardSt;

//RSA私钥数据结构
typedef struct RSA_PRI_KEY_STRUCT
{
	BYTE 	uModLen;		     //模长度(字节数)
	BYTE 	*pMod;			     //模数据
	BYTE 	uPkExpLen;		     //公钥指数长度(字节数)
	BYTE 	*pPkExp;			 //公钥指数
	BYTE 	uSkExpLen; 		     //私钥指数长度(字节数)
	BYTE 	*pSkExp;			 //私钥指数
	BYTE 	uPrimeLen1;		     //素数1长度(字节数)
	BYTE 	*pPrime1;		     //素数1数据
	BYTE 	uPrimeLen2;		     //素数2长度(字节数)
	BYTE 	*pPrime2;            //素数2数据
	BYTE 	uPrimeExpLen1;  	 //素数1指数长度(字节数)
	BYTE 	*pPrimeExp1;         //素数1指数数据
	BYTE 	uPrimeExpLen2;       //素数2指数长度(字节数)
	BYTE 	*pPrimeExp2;	 	 //素数2指数数据
	BYTE 	uCoefLen;		     //系数长度(字节数)
	BYTE 	*pCoef;			     //系数数据
} PriKeyDt;

//交易应用流程数据结构
typedef struct TRANS_PROCESS_STRUCT
{
	BYTE    ucProcessFlag; //流程标志
	// = 0 简化流程（包括接触式和非接触），只到读应用数据为止
	// = 1 EMV完整流程
	// = 2 接触式电子现金流程
	// = 3 qPBOC小额脱机流程
	// = 4 非接联机流程（仅包括消费，预授权，余额查询的非接q联机）
	// = 5 电子现金余额查询或读日志流程（不发GPO命令）
	BYTE    ucCmd;          //命令标识 0 － 正常进行 1 － 强制联机
	BYTE    ucCardType;     //卡片类型 0x00 - 接触式IC卡  0xFF - 非接触IC卡
}TransProDt;

typedef struct EMV_IC_PARA
{
	BYTE cFlag;		   //应用选择指示符 DF01
	BYTE AID[16];      //AID 9F06
	BYTE cLenAID;      //AID长度
	BYTE tTAC_Denial[5];	// 终端行为代码TAC－拒绝(5字节) DF13
	BYTE tTAC_Online[5];	// 终端行为代码TAC－联机(5字节) DF12
	BYTE tTAC_Default[5];	// 终端行为代码TAC－默认(5字节)  DF11
	BYTE tFloorLmt[11];	    // 终端最低限额 9F1B 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
	BYTE tLmt[11];          // 偏置随机选择的阈值 DF15 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
	BYTE tPercent[3];      // 随机选择的目标百分数 DF17 直接以字符串显示赋值, 如”50”，以\x00结束
	BYTE tMaxPercent[3];   // 偏置随机选择的最大目标百分数 DF16 直接以字符串显示赋值, 如”99”，以\x00结束
	BYTE tOnline;          // 终端联机PIN支持能力 DF18
	BYTE tDDOL[252];       // 缺省DDOL DF14
	BYTE cLenDDOL;
	BYTE tAppVerNo[2];     //应用版本号 9F08
	//小额支付
	BYTE tECTransLimit[13];// 终端电子现金交易限额  9F7B 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
	BYTE tQPBOCLimit[13];  // 非接触交易限额 DF20 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
	BYTE tQPBOCFloorLimit[13];  // 非接触脱机最低限额 DF19 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
	BYTE tCVMLimit[13];    // 执行CVM限额 DF21 字符串形式赋值,以分为单位, 如”50000”，以\x00结束
}EMVPARA;

typedef struct EMV_IC_PINFLAG
{
	//输入
	BYTE cType;          //密钥类型
	BYTE cIndex;         //密钥索引
	BYTE cMode;          //加密模式
	BYTE cMinLen;        //最小长度
	BYTE cMaxLen;        //最大长度
	BYTE cTimeout;       //超时时间
	BYTE cShowlenflag;   //密码键盘密码显示窗  0-不显示 1-显示
	BYTE cBeepflag;      //按键伴音  0-无伴音 1-按键伴音
	BYTE acCardNo[12];      //卡号的右12位
	BYTE cInfoLen;          //提示信息长度
	BYTE acInfo[50];        //提示信息
	//输出
	BYTE cPINInputFlag;     //输密标识 1 - 已输密， 2 - bypass，  3 - 未输密
	BYTE cEncryptPinLen;    //PIN密文长度
	BYTE acEncryptPin[16];  //PIN密文
	BYTE cTrack2Len;            //二磁道密文长度
	BYTE acEncryptTrack2[40];  //二磁道密文
}PINSTATUS;


typedef struct
{
	int (*emv_select_candidate_application)(void);   		// 持卡人选择应用，返回应用号
	int (*emv_account_type_select)(void);				// 帐户类型选择
	int (*emv_get_transaction_amount)(void);  			// 操作员输入交易金额，返回交易金额    
	int (*emv_get_cardholder_pin)(PINSTATUS *, uchar *);				//持卡人输入密码，返回密码长度
	int (*emv_show_pin_verify_result)(const uchar);		// 显示pin验证结果
	int (*emv_process_online_pin)(PINSTATUS *);			// 联机pin验证
	int (*emv_process_pboc_certificate)(void);				// 证书验证
	int (*emv_process_advice)(void);						// Advice处理
	int (*emv_process_online)(uchar *,uchar *, int *, uchar *, int *,		// 联机交易处理
		uchar *, int *);
	int (*emv_process_issuer_refer)(void);				// 处理发卡行参考
	int (*emv_set_transaction_data)(void);              // 设置和AID相关的交易应用数据
	int (*emv_ec_account_select)(void);			    	// 电子现金或借贷记帐户类型选择
}EMV_CALLBACK_FUNC;

int emv_set_callback_function(EMV_CALLBACK_FUNC *pFunc);

// 基本接口
unsigned char ST_EMV_SetData(unsigned char *pDt, unsigned short wLenDt);
unsigned char ST_EMV_GetData(unsigned char *pDol, unsigned short wLenDol, unsigned char *pVal, unsigned short *pwLenVal);
unsigned char ST_EMV_GetAppData(unsigned char *pTag, unsigned char cTagLen, unsigned char *pVal, unsigned short *pwLen);
void ST_EMV_SetKernKind(unsigned char cKernKind);
unsigned char ST_EMV_GetTransDt(TransDt	*pTransDt);
unsigned char ST_EMV_SetKernOpt(unsigned char cOptItem, unsigned char cFlg);
void ST_EMV_DelallTermAIDList(void);
unsigned char ST_EMV_DelAidListAid(unsigned char *pAid, unsigned char cLenAid);
unsigned char ST_EMV_SetAidListItem(unsigned char *pAid, unsigned char cLenAid, unsigned char cFlag);
unsigned char ST_EMV_DelAllCAPKey(void);
unsigned char ST_EMV_DelCAPKeyByRidIndex(unsigned char *pRid, unsigned char cIndex);
unsigned char ST_EMV_SetCAPKey(PKFILESTRU *pCAPKStru);
unsigned char ST_EMV_GetCAPKeyNum(void);
unsigned char ST_EMV_GetCAPKeyByRidIndex(unsigned char *pRid, unsigned char cIndex, PKFILESTRU *cpkeystru);
unsigned char ST_EMV_GetCAPKeyInfo(unsigned char cNum, PKFILESTRU *pCAPKeyInfo);
unsigned char ST_EMV_GetCAPKeyInfoByNo(unsigned char cNo, PKFILESTRU *pCAPKeyInfo);
unsigned char ST_EMV_SetCertSn(CertFile *pCertFile, unsigned char cNum);
void ST_EMV_DelAllCAPKeyCertSn(void);
unsigned char ST_EMV_DelCAPKeyCertSn(unsigned char *pCertSn, unsigned char *pRid, unsigned char cIndex);
unsigned char ST_EMV_CardLogAppSlt(DCardSt *ptCardSt, unsigned char cSelType, unsigned char *pList, unsigned short *pwLenList);
unsigned char ST_EMV_GetCardLogFormat(unsigned char *tBuf, unsigned short *cLen);
unsigned char ST_EMV_GetRecordD(unsigned char cRecNo, unsigned char cSFI, unsigned char *pRecDt, unsigned short *pRecDtLen);
unsigned char ST_EMV_GetEMVDeviceSn(unsigned char *pSn, unsigned short *pwLenSn);
unsigned char ST_SHA1(unsigned int uiInlen, void *pvDataIn, void *pvDataOut);
unsigned char ST_EMV_TranKernLog(int comHandle);
void ST_EMV_SetFirstSelApp(unsigned char ucFlag);
unsigned char ST_EMV_GetFirstSelApp(void);
// 交易接口
void ST_EMV_Init(void);
unsigned char ST_EMV_InitTmFixData(TmFixData *ptTmFixDt);
unsigned char ST_EMV_InitiateEMV(AppInitData *pInitDt);
unsigned char ST_EMV_AppSelection(DCardSt *ptCardSt, unsigned char cSelType, unsigned char *pList, unsigned short *pwLenList);
unsigned char ST_EMV_FinalSelect(unsigned char cAidNo, unsigned char *pList, unsigned short *pwLenList, unsigned char *pPDOL, unsigned short *pwLenPDOL);
unsigned char ST_EMV_GetProcOption(unsigned char cAidNo, AppDt *pDt, unsigned char *pList, unsigned short *pwLenList);
unsigned char ST_EMV_ReadAppData(unsigned char *pList, unsigned short *pwLenList);
unsigned char ST_EMV_OfflineDataAuth(void);
unsigned char ST_EMV_TermRiskManage(unsigned char cIsBlackCard, unsigned long lPanAmnt, unsigned char *cResult);
unsigned char ST_EMV_ProcessRestrict(unsigned char *cResult);
unsigned char ST_EMV_NewCardHolderValidate(unsigned char cFlg, unsigned char cCmd, unsigned char *pDt, unsigned short *pwLenDt);
unsigned char ST_EMV_ActionAnalysis(unsigned char cCmd, TransDt *pTransDt);
unsigned char ST_EMV_OnlineDataProcess(unsigned char *pIssDt, unsigned short wLenIssDt, TransDt *pTransDt);
unsigned char ST_EMV_GetVersionInfo(char *pVersion, int *pnLen);
unsigned char ST_EMV_ICCommand_GetData(int p1, int p2, unsigned char *pDataOut, int *pOutLen);
unsigned char ST_EMV_GetTLVList(unsigned char *pTagList, int nTagListLen, unsigned char *pOutList, int *pnOutListLen);
unsigned char ST_EMV_BeginTransaction(TransProDt *pTransInfo, AppDt *pDt);
unsigned char ST_EMV_ContinueTransaction(PINSTATUS *pEncryptInfo);
unsigned char ST_EMV_ReadLastRecord(void);
unsigned char ST_EMV_GetCardnoSfi(int *pnSFI, int *pnRecord);
unsigned char ST_EMV_ReadCardnoData(int nSFI, int nRecord, char *pCard, int *pCardLen);

#endif
