/*******************************************************************************
*******************************************************************************/
#ifndef __KEY_STORE_H__
#define __KEY_STORE_H__
#if 0

//*************************************
//   key num
//*************************************

#define NUM_APP       (uchar)APP_TOTAL_NUM //APP_TOTAL_NUM  // 4

// key num in flash
#define NUM_AUTHMK    (uchar)MASTERKEY_NUM  //2048/32=64
#define NUM_MK        (uchar)MASTERKEY_NUM
#define NUM_AUTHPINK  (uchar)WORKKEY_NUM
#define NUM_AUTHMACK  (uchar)WORKKEY_NUM
#define NUM_AUTHDESK  (uchar)WORKKEY_NUM
#define NUM_AUTHWK    (uchar)(NUM_AUTHPINK+NUM_AUTHMACK+NUM_AUTHDESK)

// key num in ram
#define NUM_PINK      (uchar)WORKKEY_NUM
#define NUM_MACK      (uchar)WORKKEY_NUM
#define NUM_DESK      (uchar)WORKKEY_NUM
#define NUM_WK        (uchar)(NUM_PINK+NUM_DESK+NUM_MACK)

//*******************************************************
//
//   save ram
//
//*******************************************************
// pubkey 32B
typedef struct ST_MMAP_KEY_
{
	uchar aucMMK[MK_UNIT_LEN];   //16
	uchar aucELRCK[ELRCK_LEN];   //8  ELRCK is MagKey
	uchar check[CHECK_LEN];      //4 0-aucMMK+aucELRCK check 1-fsk check 2-AppFsk 3-CtrlFSK
    uchar facchk[FUN_LEN];       //4
} ST_MMAP_KEY;
#define MMAP_KEY_LEN  (MK_UNIT_LEN+ELRCK_LEN)
	


typedef struct ST_KEY_CURRENT_
{
	uchar appid;
	uchar MKIndex;     //1~64:AUTH_MK成功  0-未认证
	uchar AuthType;    //KEY_TYPE_AUTHPINK~KEY_TYPE_AUTHDESK:认证工作密钥类型  0-非法
	uchar AuthIndex;   //From:0~	
	uchar AuthFlag;    //认证密钥认证标志
	uchar WKType;
	uchar HsFlag;           // 握手标志
	uchar DlAuthPinkPerHs;  //
	uchar DlAuthDeskPerHs;
	uchar DlAuthMackPerHs;
	uchar DlAuthMKPerHs;
	uchar DlMKPerHs;
	uchar apptype;         //固定为1
	uchar scatter;         //分散标志 0-不分散 1-分散
	uchar encflg;          //加解密标志 0-加密 1-解密
	uchar pinlen;
	int BeginTime;          // auth OK begintime
	int PinLimitTime;            // Pin
	int ShakehandBeginTime; // PTK or shakehand  
	uchar aucRand[16];       // 必须2B对齐
} ST_KEY_CURRENT;



#define EM_ERR			0x01
#define EM_FLASH_ERR	0x02
#define EM_PARA_ERR	    0x03


/*-----------------------------------------------------------------------------}
 *  应用信息
 *-----------------------------------------------------------------------------{*/
#define   LEN_APP_APPNAME  8 
#define   APPINFO_LEN      48   //appname(8B) + use A(8B) + use B(8B) + appk(24B)

//*****************************************
extern ST_MMAP_KEY gMKey;
//*****************************************
#define PCI_MMKEncrypt(len,in,out)  des_encrypt_ecb(in, out, len, gMKey.aucMMK, sizeof(gMKey.aucMMK)/8)
#define PCI_MMKDecrypt(len,in,out)  des_decrypt_ecb(in, out, len, gMKey.aucMMK, sizeof(gMKey.aucMMK)/8)

#define CURRENT_AUTHMK_NUM      (FPOS(struct APP_ZONE,app[appid-1].auth_mk[index])>>5)
#define CURRENT_AUTHPINK_NUM    (FPOS(struct APP_ZONE,app[appid-1].auth_wk.pink[index])>>5)
#define CURRENT_AUTHMACK_NUM    (FPOS(struct APP_ZONE,app[appid-1].auth_wk.mack[index])>>5)
#define CURRENT_AUTHDESK_NUM    (FPOS(struct APP_ZONE,app[appid-1].auth_wk.desk[index])>>5)
#define CURRENT_MSK_NUM         (FPOS(struct APP_ZONE,app[appid-1].msk[index])>>5)
#define CURRENT_PINK_NUM        (FPOS(struct APP_ZONE,app[appid-1].wsk.pink[index])>>5)
#define CURRENT_MACK_NUM        (FPOS(struct APP_ZONE,app[appid-1].wsk.mack[index])>>5)
#define CURRENT_DESK_NUM        (FPOS(struct APP_ZONE,app[appid-1].wsk.desk[index])>>5)

#define CURRENT_APPFLAG     (FPOS(struct APP_ZONE,app[appid-1].pub.appflag)>>5)
#define CURRENT_APPNAME     (FPOS(struct APP_ZONE,app[appid-1].pub.appname)>>5)
#define CURRENT_APPPSWA     (FPOS(struct APP_ZONE,app[appid-1].pub.apppswa)>>5)
#define CURRENT_APPPSWB     (FPOS(struct APP_ZONE,app[appid-1].pub.apppswb)>>5)
#define CURRENT_APPSN       (FPOS(struct APP_ZONE,app[appid-1].pub.appsn)>>5)
#define CURRENT_APPAK       (FPOS(struct APP_ZONE,app[appid-1].pub.appak)>>5)
#define CURRENT_APPEK       (FPOS(struct APP_ZONE,app[appid-1].pub.appek)>>5)
#define CURRENT_TRSK        (FPOS(struct APP_ZONE,app[appid-1].pub.trsk)>>5)
#define CURRENT_APPFSK      (FPOS(struct APP_ZONE,app[appid-1].pub.appfsk)>>5)
#define CURRENT_MAGK        (FPOS(struct APP_ZONE,app[appid-1].pub.magkey)>>5)
#define CURRENT_MCK         (FPOS(struct APP_ZONE,app[appid-1].pub.mckey)>>5)

void PCI_InitPublicKey(uint mode);
uint PCI_WriteKeyToFlash(uchar appid,uchar type,uchar index, uchar len, uchar *key);
int PCI_ReadKeyFromFlash(uchar appid,uchar type,uchar index, uchar *len, uchar *outkey);
uint PCI_ReadCurrentKey(uchar type,uchar index, uchar *len, uchar *outkey);
uint PCI_WriteCurrentKey(uchar type,uchar index, uchar len, uchar *outkey);
void dispkeyinfo(uint mode, void *pci);
uint PCI_WriteAuthErrTimes(uint appno,uint times);
uint PCI_ReadAuthErrTimes(uint appno);
uint PKS_CheckPbulicKey(void);
uint PKS_CheckWorkKey(void);
uint PCI_SaveFSK(uchar type,uchar *fsk);
uint PCI_ReadFSK(uchar type,uchar *fsk);
void PCI_MakeKeyCheck(uchar *in,uint len,uchar *check);   
uint PCI_ReadMCKey(uchar type,uchar index, uchar *len, uchar *outkey);

int PCI_ClearAppKey(uint8_t appno);
int PCI_ClearPubKey(uint8_t appno);
int app_get_current_key(uint8_t appno);
void keyextend(uint keylen, uchar *inbuf, uchar *outbuf);
#endif

#endif






