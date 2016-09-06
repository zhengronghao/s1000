/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : sys_api.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 12/4/2014 11:47:25 AM
 * Description        : 
 *******************************************************************************/
#ifndef __SYS_API_H__
#define __SYS_API_H__

/*************************************
 *  按键模块
 *************************************/
int kb_hit(void);
int kb_getkey(int timeout_ms);
void kb_flush(void);
int kb_getstr(uint input_mode, int minlen, int maxlen, int timeout_ms, char *str_buf);

#define TDEA_ENCRYPT            0x01
#define TDEA_DECRYPT            0x00
//*********************************************************************
//                          算法API定义
//*********************************************************************
#define TDEA_ENCRYPT            0x01
#define TDEA_DECRYPT            0x00
/* RSA public and private key. */
#define MIN_RSA_MODULUS_BITS    508
#define MAX_RSA_MODULUS_BITS    2048
#define MAX_RSA_MODULUS_LEN     ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS      ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN       ((MAX_RSA_PRIME_BITS + 7) / 8)

typedef struct
{
	uint bits;							/* length in bits of modulus */
	uchar modulus[MAX_RSA_MODULUS_LEN];	/* modulus */
	uchar exponent[4];					/* public exponent */
} R_RSA_PUBLIC_KEY;

typedef struct
{
	uint bits;							/* length in bits of modulus */
	uchar modulus[MAX_RSA_MODULUS_LEN];	/* modulus */
	uchar publicExponent[4];			/* public exponent */
	uchar exponent[MAX_RSA_MODULUS_LEN];	/* private exponent */
	uchar prime[2][MAX_RSA_PRIME_LEN];	/* prime factors */
	uchar primeExponent[2][MAX_RSA_PRIME_LEN];	/*exponents for CRT */
	uchar coefficient[MAX_RSA_PRIME_LEN];	/* CRT coefficient */
} R_RSA_PRIVATE_KEY;



/*************************************
 *  液晶模块
 *************************************/
void lcd_cls(void);
void lcd_goto(int x, int y);

/*************************************
 *  蜂鸣模块
 *************************************/
#define DEFAULT_SOUNDFREQUENCY BEEP_PWM_HZ 
#define DEFAULT_WARNING_BEEPTIME 300
void  sys_beep(void);
void sys_beep_pro(uint freq, uint duration_ms, int choke);

/*************************************
 *  flash读写模块
 *************************************/
#define FILESYSTEM_STARTADDRESS  (uint32_t)0x0004F000
#define FILESYSTEM_ENDADDRESS    (uint32_t)0x00056FFF
#define FILESYSTEM_LEN           (uint32_t)(32*1024)
int flash_app_erasedata(uint addr);
int flash_app_writedata(uint FlashStartAddress, uint NumberOfBytes,uchar *data);
int flash_app_readdata(uint FlashStartAddress, uint NumberOfBytes,uchar *data);



int rsa_pub_dec(void *outbuf, uint * outlen, const void *inbuf, uint inlen,
	const R_RSA_PUBLIC_KEY * pubkey);

#endif


