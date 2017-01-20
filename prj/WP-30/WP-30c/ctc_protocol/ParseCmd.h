//ParseCmd.h
//  Command parse module, which is used to communicate with MCU
#ifndef __PARSECMD_H__
#define __PARSECMD_H__ 

#include "common.h"
#define PCK_CHK_CRC16 //是否采用CRC16双字节校验，否则采用LRC单字节校验

#ifdef PCK_CHK_CRC16
#define PCK_CHK_LEN 2//crc16占用2字节
#else
#define PCK_CHK_LEN 1//lrc占用2字节
#endif

#define CTC_STX 0x55
#define CTC_ETX 0xAA
#define ESC 0xA5 //转义字符

#define CTC_STX_T 0xE7 //CTC_STX转义字符
#define CTC_ETX_T 0xE8 //CTC_ETX转义字符
#define ESC_T 0xE9 //ESC转义字符

//通用错误码
#define RET_OK                          0x00  //成功
#define RET_UNKNOWN               0x01//指令码出错
#define RET_PARAM                    0x02//参数错误
#define RET_TIMEOUT                 0x03//操作超时
#define RET_CANCEL                   0x04//操作被取消
#define RET_MEM                       0x05//内存不足
#define RET_OPENED                  0x06//已经被其他进程打开
#define RET_PERMISSION            0x07//权限不足
#define RET_NOT_OPEN              0x08//设备未打开
#define RET_OCCUPIED               0x09//设备被占用，正在使用中
#define RET_ILLEAGLE_SERVICE  0x00A//非法服务ID
#define RET_PARAM_LEN             0x0B//参数长度错误
#define RET_LRC                         0x0C//LRC错误
#define RET_TRANSLATE              0x0D//转义字符串出错
#define RET_NOCTC_STX		           0x0E//没有CTC_STX
#define RET_NOCTC_ETX			     0x0F//没有CTC_ETX
#define RET_UNPACK_LEN		     0x10//解包长度有误
#define RET_CHECK_LEN		     0x11//长度校验有误
#define RET_EMPTY     		     0x12//空数据错误
#define RET_NOT_FOUND     	     0x13//未找到对应数据
#define RET_PACKET_INTEGRITY    0x14//包完整性错误
#define RET_WRITE_DEVICE  	     0x15//写设备失败
#define RET_READ_DEVICE  	     0x16//读设备失败
#define RET_ICCARD_SELECT         0x17
#define RET_RFCARD_SELECT        0x18
#define RET_ICCARD_RESET          0x19
#define RET_ICCARD_APDU           0x1A
#define RET_RFCARD_POLL           0x1B
#define RET_RFCARD_APDU          0x1C

//状态码
#define CMDST_OK					0x0000//处理成功
#define CMDST_CMD_ER				0x0001//指令码不支持
#define CMDST_PARAM_ER				0x0002//参数错误
#define CMDST_DATA_LEN_ER			0x0003//可变数据域长度错误
#define CMDST_FRAME_FORMAT_ER	    0x0004//帧格式错误
#define CMDST_LRC_ER				0x0005//LRC校验失败
#define CMDST_OTHER_ER				0x0006//其他
#define CMDST_TIMEOUT				0x0007//超时
#define CMDST_PUBKEY_FORMAT_ER      0x0008//公钥格式错误
#define CMDST_PUBKEY_ZERO           0x0009//板内公钥数量为0，无法验签
#define CMDST_SIGN_FORMAT_ER		0x000A//签名数据格式错误
#define CMDST_HASE_ERR  			0x000B//Hash值不匹配
#define CMDST_SIGN_ER               0x000C//验签失败
#define CMDST_STORE_FULL			0x000D//存储区已满
#define CMDST_IC_SELECT_ER		    0x000E//IC卡类型选择出错
#define CMDST_RF_SELECT_ER			0x000F//RF卡类型选择出错
#define CMDST_IC_RESET_ER			0x0010//IC卡复位出错
#define CMDST_IC_APDU_ER			0x0011//IC卡APDU出错
#define CMDST_RF_POLL_ER			0x0012//RF卡寻卡失败
#define CMDST_RF_APDU_ER			0x0013//RF卡APDU出错
#define CMDST_OPEN_DEV_ER			0x0014//打开设备出错
#define CMDST_CLOSE_DEV_ER		0x0015//关闭设备出错
#define CMDST_IC_EXIST_ER		0x0016// ICCARD不在位
#define CMDST_UNPACK_LEN_ER		0x0017//解析包长度出错
#define CMDST_CANCEL_ER		    0x0018//取消退出
#define CMDST_RF_TRANS_ER       0x0019//传输错误
#define CMDST_RF_PROT_ER        0x001A//协议错误
#define CMDST_RF_MULTI_ER       0x001B//多卡错误
#define CMDST_RF_NOACT_ER       0x001C//卡未上电错误
#define CMDST_RF_NOAUTH_ER      0x001D//卡未认证错误
#define CMDST_RF_AUTH_ER        0x001E//卡认证失败错误
#define CMDST_RF_UNCHANGE_ER    0x001F//不可修改错误
#define CMDST_RF_KEYNOAUTH_ER   0x0020//密钥无权限
#define CMDST_IC_INVALID_ER     0X0021//卡片过期，无效卡
#define CMDST_IC_CODE_ER        0X0022//卡密码出错
#define CMDST_IC_CODECOUNT_ER   0X0023//卡有效次数用尽
#define CMDST_IC_LOCK_ER        0X0024//卡片已锁定
#define CMDST_RF_MULCARD_ER			0x0025//RF卡多卡出错
/* 命令码*/
/* 磁卡*/
#define CMD_MAGCARD			      0xA3
#define CMD_MAGCARD_OPEN			0x01
#define CMD_MAGCARD_CLOSE			0x02
#define CMD_MAGCARD_READ			0x03
#define CMD_MAGCARD_REPORT	       0x04
#define CMD_MAGCARD_CANCEL	       0x05

/* 针式打印机*/
#define CMD_NEEDLE                           0xE3
#define CMD_NEEDLE_OPEN			0x01
#define CMD_NEEDLE_CLOSE			0x02
#define CMD_NEEDLE_STATUS			0x03
#define CMD_NEEDLE_WRITE			0x04

/* ICCARD */
#define CMD_ICCARD                           0xB3
#define CMD_ICCARD_SLOT_OPEN			0x01
#define CMD_ICCARD_SLOT_CLOSE			0x02
#define CMD_ICCARD_TYPE     			0x03
#define CMD_ICCARD_CARD_OPEN			0x04
#define CMD_ICCARD_CARD_CLOSE			0x05
#define CMD_ICCARD_APDU			0x06
#define CMD_ICCARD_CHECK        0x07
/* RFCARD */
#define CMD_RFCARD                           0xC3
#define CMD_RFCARD_OPEN			 0x01
#define CMD_RFCARD_CLOSE		 0x02
#define CMD_RFCARD_TYPE			 0x03
#define CMD_RFCARD_MODULE_OPEN	 0x04
#define CMD_RFCARD_MODULE_CLOSE	 0x05
#define CMD_RFCARD_POLL_POWER_UP 0x06
#define CMD_RFCARD_APDU			 0x07
#define CMD_RFCARD_MIFARE        0x08
#define CMD_RFCARD_POLL			 0x09
#define CMD_RFCARD_CARD_OPEN	 0x0A
#define CMD_RFCARD_CARD_CLOSE	 0x0B

/* TPRINTER */
#define CMD_TPRINTER                         0xC4
#define CMD_TPRINTER_OPEN			0x01
#define CMD_TPRINTER_CLOSE			0x02
#define CMD_TPRINTER_LINESET		0x03
#define CMD_TPRINTER_DESITY		0x04
#define CMD_TPRINTER_FEEDPAPER	0x05
#define CMD_TPRINTER_BITMAP       	0x06
#define CMD_TPRINTER_STATUS      	0x07
#define CMD_TPRINTER_BLACKMARK      0x08

/* SIGN */
#define CMD_SIGN                       0xB1
#define CMD_SIGN_DOWN_PUBKEY           0x01//下装公钥
#define CMD_SIGN_GET_PUBKEY_NUM        0x02//获取公钥个数
#define CMD_SIGN_CLEAR_PUBKEY          0x03//清除所有公钥
#define CMD_SIGN_VERIFY                0x04//验签
#define CMD_SIGN_UPDATE_HASH           0x05//镜像HASH更新
#define CMD_SIGN_VERIFY_IMG            0x06//镜像鉴权
#define CMD_SIGN_PUBKEY_DRENCRY        0x07//公钥解密
#define CMD_SIGN_SET_PROJECT_ID        0x08// 设置项目固件ID
#define CMD_SIGN_GET_PROJECT_ID        0x09// 获取项目固件ID
#define CMD_SIGN_GET_GAPKHASH_REQ      0x0A// 获取GAPK HASH请求
#define CMD_SIGN_GET_GAPKHASH_NUM      0x0B// 获取GAPK 包请求

/* PINPAD */

#define CMD_PED                         0xD3 // 密码键盘类
#define CMD_PED_OPEN                    0x01 // 打开密码键盘
#define CMD_PED_CLOSE                   0x02 // 关闭密码键盘 
#define CMD_PED_DL_MK                   0x03 // 下载主密钥，认证密钥，传输密钥和过程密钥（分散随机数）
#define CMD_PED_DL_WK                   0x04 // 下载工作密钥
#define CMD_PED_GET_RANDOM              0x05 // 获取随机数
#define CMD_PED_ONLINE_PIN              0x06 // 获取联机PIN
#define CMD_PED_OFFLINE_PIN             0x07 // 获取脱机PIN
#define CMD_PED_ENCRYPT                 0x08 // 数据加解密
#define CMD_PED_GET_MAC                 0x09 // 获取MAC
#define CMD_PED_SELE_KEY                0x0A // 选择当前密钥
#define CMD_PED_AUTHENT                 0x0B // 用认证随机数进行认证
#define CMD_PED_HAND                    0x0D // 与密码键盘握手
#define CMD_PED_GET_PINSTR              0x0E // 通知准备获取PIN密钥，上位机显示密码键盘窗口
#define CMD_PED_EXTERN                  0x0F // 扩展 
#define CMD_PED_NOTIFY_KEY              0x10 // pin键值通知指令
#define CMD_PED_DIS_KEYNUM              0x11 // 下位机提示APP显示密码*的个数
#define CMD_PED_PINSTR_END              0x12 // PIN密码输入结束
#define CMD_PED_ONLINEPINSTR_RESULT     0x13 // 连接PIN结果上送
#define CMD_PED_OFFLINEPINSTR_RESULT    0x14 // 脱机PIN结果上送

                                             
/* EMV */                                    
#define CMD_EMV                       0xC5   
#define CMD_EMV_OPEN               0x01     
#define CMD_EMV_CLOSE              0x02
#define CMD_EMV_SET_DATA           0x03  //设置固定数据源  
#define CMD_EMV_BEGIN_TRANSACTION  0x04  //开始交易
#define CMD_EMV_ONLINE_DATA_PROCESS  0x05  //联机处理  
#define CMD_EMV_APP_SET_DATA       0x06  //设置应用相关固定数据源 
#define CMD_EMV_APP_SELECT         0x07  //应用选择  
#define CMD_EMV_APP_FINAL_SELECT   0x08  //应用选择确认
#define CMD_EMV_APP_INIT           0x09  //应用初始化
#define CMD_EMV_APP_READ           0x0A  //应用数据读取 
#define CMD_EMV_OFFLINE_DATA_AUTH      0x0B  //脱机数据认证 
#define CMD_EMV_TREM_RIS_MANAGE        0x0C  //终端风险管理
#define CMD_EMV_PROCESS_RESTRICT       0x0D  //处理限制功能
#define CMD_EMV_CARD_HOLDER_VALIDATE   0x0E  //持卡人验证 
#define CMD_EMA_ACTION_ANALYSIS        0x0F  //终端行为分析和卡片行为分析
#define CMD_EMV_UPDATE_DATA            0x10  //更新EMV L2 内核模块中的数据元 
#define CMD_EMV_GET_KERNEL_DATA        0x11  //获取EMV L2 内核模块数据元 
#define CMD_EMV_GET_APP_DATA           0x12  //获取应用数据  
#define CMD_EMV_SET_KERN_OPT           0x13  //设置内核选项参数 
#define CMD_EMV_AID_INIT_LIST          0x14  //初始化AID列表    
#define CMD_EMV_AID_DEL_LIST_AID       0x15  //删除EMV L2 内核中与 AID项 
#define CMD_EMV_AID_SET_LIST_AID       0x16  //设置AID列表数据  
#define CMD_EMV_CAPKEY_SET            0x17  //设置内核模块中的认证中心公钥数
#define CMD_EMV_CAPKEY_DEL            0x18  //删除中心公钥
#define CMD_EMV_CAPKEY_GET_INFO       0x19  //获取公钥信息
#define CMD_EMV_ICCARD_GET_LOG        0x1A    //读取IC卡消费日志
#define CMD_EMV_ICCARD_GET_LOAD_LOG   0x1B  //获取IC卡圈存日志
#define CMD_EMV_GET_KERN_VERSION      0x1C  //获取EMV内核版本信息 
#define CMD_EMV_GET_DATA              0x1D  //取数据指令
#define CMD_EMV_READ_LAST_RECORD      0x1E  //读取最后一条记录   
#define CMD_EMV_GET_CARD_NO_SFI       0x1F  //获取卡号或二磁道数据所在的文件名和记录号   
#define CMD_EMV_READ_CARD_NO_DATA     0x20  //读取卡号或二磁道数据所在的记录数据
#define CMD_EMV_EXPAND                0x21  //其他扩展指令   
#define CMD_EMV_DIS                   0x22  //EMV显示框指令  
#define CMD_EMV_DIS_RESPONSE          0x23  // EMV显示框结果通知指令   

#define CMD_EMV_DELETE_ALL_CAPKEY   0x24          //删除EMV内的所有公钥
#define CMD_EMV_SET_IC_PARAMETER    0x25          //设置EMV的IC卡参数
#define CMD_EMV_CONTINUE_TRANSACTION  0x26         //继续交易
#define CMD_EMV_CONTINUE_TRANSACTION_RESULT  0x27  //继续交易结果
#define CMD_EMV_BEGIN_TRANSACTION_RESULT     0x28  //开始交易结果
#define CMD_EMV_EXTEND_SET_ATTRIBUTE         0x29  // 扩展指令，设置emv相关属性
#define CMD_EMV_EXTEND_GET_ATTRIBUTE         0x2A  // 扩展指令，获取emv相关属性

/* currency detect */
#define CMD_CURRENCYDETE                      0xE5   
#define CMD_CURRENCYDETE_OPEN                 0x01   
#define CMD_CURRENCYDETE_CLOSE                0x02   

/*系统管理*/
#define CMD_SYS_MANAGE         0xA1     //系统管理
#define CMD_SYS_MANAGE_LED     0x01     //LED 控制
#define CMD_SYS_MANAGE_BEEP    0x02     //蜂鸣器 控制
#define CMD_SYS_MANAGE_GET_RTC 0x03     // RTC时钟获取
#define CMD_SYS_MANAGE_SET_RTC 0x04     // RTC时钟设置
#define CMD_SYS_MANAGE_MDF_STATUS       0x05  // 系统篡改状态查询
#define CMD_SYS_MANAGE_MDF_UNLOCK       0x06  // 系统篡改解锁
#define CMD_SYS_MANAGE_READ_EXFLASH     0x07  // 系统外部flash读操作
#define CMD_SYS_MANAGE_WRITE_EXFLASH    0x08  // 系统外部flash写操作
#define CMD_SYS_MANAGE_DL_INFORM        0x09  // 通知下载文件
#define CMD_SYS_MANAGE_DL_DATA          0x0A  // 数据下载
#define CMD_SYS_MANAGE_DL_FINISH        0x0B  // 下载完成
#define CMD_SYS_MANAGE_INFO_MANAGE      0x0C  // 通知安全CPU进入管理模式
#define CMD_SYS_MANAGE_GET_CUPSTATUS    0x0D  // 获取安全CPU状态 
#define CMD_SYS_MANAGE_SET_CUPSTATUS    0x0E  // 设置安全CPU状态
#define CMD_SYS_MANAGE_LOWPOWER    0x0F  // 系统进入低功耗
#define CMD_SYS_MANAGE_STATE_CHANGE    0x10  // 系统状态改变

/*生产管理*/
#define CMD_FAC             0xA2
#define CMD_FAC_ATTACK      0x01   // 

#define CMD_FAC_TEST_UART      0x02 
#define CMD_FAC_TEST_GPIO      0x03 
#define CMD_FAC_TEST_CHARGING  0x04 
#define CMD_FAC_TEST_TRUMPET   0x05 

/*终端信息*/
#define CMD_TERMINAL_INFO       0xD1   //终端信息
#define CMD_TERMINAL_INFO_SOFT  0x01   //CPU软件版本    
#define CMD_TERMINAL_INFO_HARD  0x02   //CPU硬件版本
#define CMD_TERMINAL_INFO_GETANDROID_VER  0x03   //获取android中间键的版本
#define CMD_TERMINAL_INFO_PM    0x04   //ANDROID通知进入正常工作模式，通过这条指令，开启心跳包检测机制，开启心跳包低功耗模式

///*一维码*/
//#define CMD_BARCODE             0xF3
//#define CMD_BARCODE_OPEN        0x01   //打开一维扫描头
//#define CMD_BARCODE_CLOSE       0x02   //关闭一维扫描头
//#define CMD_BARCODE_OPERATE     0x03   //扫描头操作
//
//
///*二维码*/
//#define CMD_QRCODE              0xA4
//#define CMD_QRCODE_OPEN               0x01  //打开二维扫描头
//#define CMD_QRCODE_CLOSE              0x02  //关闭二维码扫描头
//#define CMD_QRCODE_OPERATE            0x03  //扫描头操作

/*一维码*/
#define CMD_SCAN             0xF3
#define CMD_SCAN_OPEN        0x01   //打开一维扫描头
#define CMD_SCAN_CLOSE       0x02   //关闭一维扫描头
#define CMD_SCAN_OPERATE     0x03   //扫描头操作
#define CMD_SCAN_RESULT_REPORT  0x04   //扫描结果上送
#define CMD_SCAN_CANCEL         0x05   //扫描取消

/*串口扩展*/
#define CMD_UART_EXP       0xB4
#define CMD_UART_EXP_OPEN        0x01 
#define CMD_UART_EXP_CLOSE       0x02
#define CMD_UART_EXP_READ        0x03
#define CMD_UART_EXP_WRITE       0x04

/*IDCARD_CARD*/
#define CMD_IDCARD               0xD5 //身份证
#define CMD_IDCARD_OPEN          0x01 //打开
#define CMD_IDCARD_CLOSE         0X02 //关闭
#define CMD_IDCARD_SEARCH        0x03 //寻卡
#define CMD_IDCARD_READ_WORD_PHOTO 0x04 //读卡
#define CMD_IDCARD_ACTIVE        0x05 //激活卡
#define CMD_IDCARD_APDU_GETTHROUTH  0x06 // 透传
#define CMD_IDCARD_FINGER_PRINT  0x07 //读指纹


typedef struct packet_mcu
{
	uint32_t len;
	uint8_t  cmdcls;
	uint8_t  cmdop;
	uint8_t  sno1;
	uint8_t  sno2;
}MCUPCK,*PMCUPCK;

#define PACK_MINSIZE			(2+sizeof(MCUPCK)+PCK_CHK_LEN)//请求包的最小长度

#define MAX_CMD_TEMP_SIZE		2048

#define MIN_CMD_RESPONSE		6
#define FIX_LENGTH               		4



//组一个包头
MCUPCK CmdPackReq(uint32_t len,uint8_t  cmdcls,uint8_t  cmdop,uint8_t  sno1,uint8_t  sno2);

//计算字符串转义后的长度
int CmdTranslateLen(const uint8_t *inBuf,int inLen);

//计算字符串转义前的长度
int CmdRestoreLen(const uint8_t *inBuf,int inLen);

//转义字符串并在末尾加上lrc值
int CmdTranslate(const uint8_t *inBuf,uint32_t inLen,uint8_t *outBuf,uint32_t *outLen);

//转义过的字符串还原,有带LRC校验
int CmdRestore(const uint8_t *inBuf,uint32_t inLen,uint8_t *outBuf,uint32_t *outLen);

//组包
int packCmd(MCUPCK req,const uint8_t *inBuf,uint8_t *outBuf,uint32_t *outLen);

//解包
int unpackCmd(const uint8_t *inBuf,uint32_t inLen,MCUPCK *res,uint8_t *outBuf,uint32_t *outLen);

//指令完整性校验
int frame_integrity_check(uint8_t *data, uint32_t size, int *s_pos, int *e_pos);

//会自动整理缓冲区的解包函数
int unpackCmdEx(uint8_t *data, uint32_t size, MCUPCK *res,uint8_t *outBuf,uint32_t *outLen, uint32_t *restLen);

void dumpByteArray(uint8_t *data, uint32_t len);

#endif
