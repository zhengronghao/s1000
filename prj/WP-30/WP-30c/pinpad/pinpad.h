

#ifndef __PINPAD_H__
#define __PINPAD_H__

#include "pinpad_parser.h"

typedef struct
{
    uchar timeout;
    uchar tone;
    uchar vibrative;
    uchar minlen;
    uchar maxlen;
    uchar type;
    uchar mode;
    uchar showlenflag;
    uchar random[8];
}s_pin_popcontent;

uint8_t KB_GetPINStr(uint8_t mode, uint8_t ucMinLen, uint8_t ucMaxLen, uint32_t uiTimeOut, uint32_t *puiRetLen, uint8_t *pucGetBuf);
uchar PinEncrypt(uint uiPINFormat,
	uint uiKeyLen,uchar * pucKey,
	uint uiLenIn, uchar * pvCardNo, uchar * pvDataIn, uint * puiOutLen, uchar * pvDataOut);

int lcd_emvdisp(s_lcd_emv * lcd_emvdis, int distimeout); 

#ifdef CFG_TAMPER
typedef struct ST_MMAP_KEY_
{
	uchar aucMMK[MK_UNIT_LEN];   //16
	uchar aucELRCK[ELRCK_LEN];   //8  ELRCK is MagKey
	uchar check[CHECK_LEN];      //4 0-aucMMK+aucELRCK check 1-fsk check 2-AppFsk 3-CtrlFSK
    uchar facchk[FUN_LEN];       //4
} ST_MMAP_KEY;
#define MMAP_KEY_LEN  (MK_UNIT_LEN+ELRCK_LEN)

//#define TAMPER_MASK             ((hal_dry_tamper_t)(DRY_TER_TPE(0x3F)|kDryTamper_Voltage|kDryTamper_Clock)
#define TAMPER_MASK             0x3F0030 
#define TAMPER_PIN_MASK         (0x3FU)
//#define TAMPER_PIN_SHIFT        (1U)

#define SECURITY_LEVEL_NOTENABLE    (~((uint32_t)0))
#define SECURITY_LEVEL_FIXKEY       ((uint32_t)0)
#define SECURITY_LEVEL_HIGHST       TAMPER_MASK 
#endif 

uint Rsa_calt_pri(int inlen, uchar *in, R_RSA_PRIVATE_KEY *keyinfo,int *outlen, uchar *out);
uint Rsa_calt_pub(int inlen, const uchar *in, R_RSA_PUBLIC_KEY *keyinfo,int *outlen, uchar *out);

#if 0 
#include "wp30_ctrl.h"
/*-----------------------------------------------------------------------------}
 *   密码键盘
 *-----------------------------------------------------------------------------{*/
#if defined(CFG_EXTERN_PINPAD) 
#define PRODUCT_TYPE PRODUCT_TYPE_MK210
#endif
#if defined(CFG_INSIDE_PINPAD) 
#define PRODUCT_TYPE PRODUCT_TYPE_F12 
#endif
#define  PINPAD_WORK_COM  UART_COM1
#define CFG_CMD_V1   //支持V1协议
//#define CFG_KEYCHECK_MAC
#define CFG_KEYCHECK_LRC

// lcd mode
#define DISP_CLR       ((1<<0))  //清屏
//*******************************************************
// 
//   APP TYPE
// 
//*******************************************************
#define APP_PROTOCOL_V1   0
#define APP_PROTOCOL_PCI  1

//*****************************************
#define INFO_APPNAME    (1<<0)
#define INFO_PSW        (1<<1)
#define INFO_APPK       (1<<2)
#define INFO_FLAG       (1<<3)
#define INFO_ALL        (INFO_APPNAME|INFO_PSW|INFO_APPK|INFO_FLAG)
#define APP_TYPE_V1     (uint)(APP_PROTOCOL_V1<<31)
#define APP_TYPE_PCI    (uint)(APP_PROTOCOL_PCI<<31)
//*****************************************
extern uchar *gucTmpBuf;
extern ST_KEY_CURRENT gtCurKey;
extern uint8_t gucCMDBuf[2048+48];


//*****************************************
uchar sys_recover(uint mode);
uchar app_recover(void);
uint sys_APPEKDecryptRead(uint uiOffset, uint uiLen, uchar *ucBuf);
uint sys_APPEKEncryptWrite(uint uiOffset, uint uiLen, uchar *ucBuf);
uint sys_MMKEncryptWrite(uint uiOffset, uint uiLen, uchar *ucBuf);
uint sys_MMKDecryptRead(uint uiOffset, uint uiLen, uchar *ucBuf);
uint app_write_info(uint appno, uint mode, uchar *info);
uint app_read_info(uint appno, uint mode, uint *outlen, uchar *info);
uint app_get_type(uint appno);
uint app_find(uchar *appname);
uint app_create(uint appno, uint mode, uchar *appinfo);
uint app_open(uint appno);
uint app_close(uint appno);
uint app_find_newapp(void);
uint app_del(uint appno);
uint app_set_APPEK(uint appno,uint mode);
uint sys_get_ver(uint mode, uint *verlen, uchar *ver);
uint get_appenable(void);
uint set_appenable(uint mode);
uint write_sn(uint appno, uchar len,uchar *sn);
uint write_default_sn(uint appno);
uint read_sn(uint appno, uchar *len,uchar *sn);
uint write_transferkey(uint appno, uchar *key);
uint read_transferkey(uint appno, uchar *key);
void read_default_tranferkey(uchar *key);
uint app_create_again(uint appno);
uchar open_default_V1APPP(void);
uint create_default_V1APP(void);
uint8_t KB_GetPINStr(uint8_t ucLine, uint8_t ucMinLen, uint8_t ucMaxLen, uint32_t uiTimeOut, uint32_t *puiRetLen, uint8_t *pucGetBuf);
void BuzzerOn(uint32_t ms);
void v1protocol_timeout_daemon(uint32_t mode);

//define constant 
#define RSA_MAX_SIZE  0x800            //2048bit
#define RSA_MAX_BSIZE RSA_MAX_SIZE/8   //256
#define RSA_SIZE    0x800              //2048bit
#define RSA_BSIZE   RSA_SIZE/8         //256
#define RSA_DWSZIE  RSA_BSIZE/4   

//define ERROR VALUE
#define RSA_SUCCESS 0
#define RSA_FAIL    1
#define  RSA_UNIT_LEN  256
//typedef struct     // 公钥结构
//{
//	uint bits;                   /* length in bits of modulus */
//	uchar modulus[RSA_UNIT_LEN];           /* modulus */
//	uchar exponent[4];             /* public exponent LSB...MSB */	
//} R_RSA_PUBLIC_KEY;

//typedef struct     // 私钥结构
//{
//	R_RSA_PUBLIC_KEY tPubinfo;
//	uchar prikey[RSA_UNIT_LEN];  /* private key */
//} R_RSA_PRIVATE_KEY;
uint Rsa_calt_pri(int inlen, uchar *in, R_RSA_PRIVATE_KEY *keyinfo,int *outlen, uchar *out);
uint Rsa_calt_pub(int inlen, const uchar *in, R_RSA_PUBLIC_KEY *keyinfo,int *outlen, uchar *out);

uint16_t crc_calculate16by8(unsigned char *ptr, uint32_t count);
uint32_t crc_calculate32by16(uint16_t * ptr, uint32_t count);
void UART_Init(uint bps);
uint uart_get_bps(void);
void dbg_pinpad(void);
/*-----------------------------------------------------------------------------}
 *  宏定义 
 *-----------------------------------------------------------------------------{*/
#define UART_Read(buf,len,timeout) uart_Read(PINPAD_WORK_COM,buf,len,timeout)
#define UART_Write(buf,len) uart_Write(PINPAD_WORK_COM,buf,len)
#define UART_Clear() uart_Flush(PINPAD_WORK_COM)
#define UART_CheckReadbuf() uart_CheckReadBuf(PINPAD_WORK_COM)
void LCD_LightOn(void);
void LCD_LightOff(void);
void LCD_ClearScreen(void);
void LCD_Disp(int x,int y,int mode,const char *format,...);
void pinpad_Display(uint8_t ucLine, const char *pcFormat, ...);
void LCD_DispCenter(uint8_t ucLine,uint mode, const char *pcFormat, ...);
void DispTitle(void *title, uint clearFlag);
void DispStr(uchar displine, uint flag, void *str1, void *str2, void *str3);
int pinpad_getkey (int mode,int timeout_ms);
#define mymemcmp  memcmp

#endif

#endif




