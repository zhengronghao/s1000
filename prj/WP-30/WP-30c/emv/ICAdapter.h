#ifndef ICADAPTER_H
#define ICADAPTER_H
/************************************************************************/
/*
常量定义,
全局宏,
全局数据类型,
*/
/************************************************************************/
//#include "../inc/libapi.h"

typedef struct ICAPARAStru
{
	long  lICComPortNo;
	long  lICComPortBPS;
	long  lICCardVCC;
	long  lICCardProtocol;
	long  lICCardIndex;
}ICAPARASTRU;

#define     EM_ICA_USERCARD       (long)USERCARD         // 本机半埋卡座
#define     EM_ICA_SAM1SLOT       (long)SAM1SLOT         // 本机SAM卡座1
#define     EM_ICA_SAM2SLOT       (long)SAM2SLOT         // 本机SAM卡座2
#define     EM_ICA_SAM3SLOT		  (long)SAM3SLOT         // 本机SAM卡座3
#define     EM_ICA_SAM4SLOT		  (long)SAM4SLOT         // 本机SAM卡座3
#define     EM_ICA_LOCAL_RFCARD	  (long)0x00FF           // 内置射频
#define     EM_ICA_RSD_USERCARD	  (long)0x8000           // SD读卡器大卡
#define     EM_ICA_RSD_SAM1SLOT	  (long)0x8010           // SD读卡器SAM卡座
#define     EM_ICA_RSD_RFCARD	  (long)0x80FF           // SD读卡器射频

//卡片状态
#define     EM_ICA_CARDPOWERUP      (long)0x0001  //卡片上电
#define     EM_ICA_CARDPOWEROFF     (long)0x0000  //卡片未上电
#define     EM_ICA_CARDPOWERNULL    (long)-1  //卡片未上电
/************************************************************************/
/*  返回值宏                                                         */
/************************************************************************/
#define  EM_ICA_SUCC               (long)0x0000    // 功能正确执行
#define  EM_ICA_CARDRESULTERR      (long)0x8000    // 功能正确执行
#define  EM_ICA_INPUTERR           (long)0x8081    // 参数错误
#define  EM_ICA_DATAERR            (long)0x8082    // 通讯数据错误
#define  EM_ICA_COMMERR            (long)0x8083    // 设备故障
#define  EM_ICA_ERRORSLOT          (long)0x8001    // 不支持该卡座类型
#define  EM_ICA_CARDOUT            (long)0x8002    // 卡未到位
#define  EM_ICA_POWERUPREADY       (long)0x8003    // 卡片已经上电
#define  EM_ICA_NOPOWER            (long)0x8004    // 卡片未上电或激活
#define  EM_ICA_TIMEOUT            (long)0x8006    // 卡片通讯超时
#define  EM_ICA_RESPERR            (long)0x8007    // 卡片应答数据错误
#define  EM_ICA_RFHALTERR          (long)0x8008    // 射频卡片HALT失败
#define  EM_ICA_RFCARDNUMERR       (long)0x8009    // 射频感应区存在多张卡
#define  EM_ICA_OTHERERR           (long)0x80FF    // 其他错误
/************************************************************************/
/*  局部函数原型                                                         */
/************************************************************************/
long ICPowerUp(long lICCardIndex,long *puiRetLen,void *pvOutBuf);
long ICPowerDown(long lICCardIndex);
long ICCardIn(long lICCardIndex,long *plCardPowerStatus);
long ICExchangeAPDU(long lICCardIndex,long  lSendLen, char  *pcSendData, long *pulRecLen, char *pcRecData);

long RFID_Local_PowerUp(long lICCardIndex,long *puiRetLen,void *pvOutBuf);
long RFID_Local_PowerDown(long lICCardIndex);
long RFID_Local_CardIn(long lICCardIndex,long *plCardPowerStatus);
long RFID_Local_ExchangeAPDU(long lICCardIndex,long  lSendLen, char  *pcSendData, long *pulRecLen, char *pcRecData);
/************************************************************************/
/*  全局函数原型                                                         */
/************************************************************************/
long ICAdapterSelectCard(long lIndex);
long ICAdapterSetCardPara(ICAPARASTRU strPara);
long ICAdapterGetCardPara(ICAPARASTRU *pstrPara);
long ICAdapterCardIn(void);
long ICAdapterPowerDown(void);
long ICAdapterPowerUp(long *puiRetLen,char *pvOutBuf);
long ICAdapterExchangeAPDU(long lSendLen, char *pvSendData, long* pulRecLen, char *pvRecData);
void EMV_CardModule_init(void);


/************************************************************************/
/*  全局变量                                                         */
/************************************************************************/
extern long  glCardType;                 //卡片类型


#endif
