#ifndef __PADLIB_H
#define __PADLIB_H

#if 0
#include <stdint.h>


#define	RESULT_SUCCESS		    (uint8_t)0
#define	RESULT_ERROR			(uint8_t)1

// FLASH OPERATION is error
#define RESULT_ERROR_FLASH	    (uint8_t)2

// cmd is not permit 
#define	RESULT_NOT_PERMIT		(uint8_t)3

// para is error
#define	RESULT_ERROR_PARA		(uint8_t)4

// mac is error
#define	RESULT_ERROR_MACK		(uint8_t)5

// cmd is invalid
#define	RESULT_INVALIDCMD	    (uint8_t)6

// the key is error
#define	RESULT_KEY_ERROR		(uint8_t)7

// pad is locked
#define	RESULT_PAD_LOCKED		(uint8_t)8

// two psw is not same
#define	RESULT_PSW_DIFF		(uint8_t)9  //not used

// the index is error
#define	RESULT_INDEX_ERROR	(uint8_t)10 //not used

// cancel the inputing
#define	RESULT_INPUT_CANCEL	(uint8_t)11

// auth-ing is error
#define	RESULT_AUTH_ERR		(uint8_t)12

// the auth key is not exist
#define	RESULT_AUTHKEY_NOTEXIST		(uint8_t)13 //not used

// the type of the authkey is error
#define	RESULT_AUTHKEY_TYPEERR		(uint8_t)14

// is not auth-ed
#define	RESULT_NOT_AUTH		(uint8_t)15

//it is different from the auth key
#define	RESULT_AUTHKEY_DIFF	(uint8_t)16

// inputing is timeout
#define	RESULT_INPUT_TIMEOUT	(uint8_t)17

// auth timeout
#define	RESULT_TIMEOUT_ERR	 (uint8_t)18

#define	RESULT_FRAME_ERRLRC	 (uint8_t)20

// the index is err
#define	RESULT_INDEX_ERR		(uint8_t)21  //not used

// main key is not exist
#define	RESULT_MK_NOTEXIST	(uint8_t)22     //not used

// main key is error
#define	RESULT_MK_ERROR		(uint8_t)23    //not used

// not handshake
#define	RESULT_NOT_HANDSHAKE	(uint8_t)24

// exceed the number of the downloading
#define	RESULT_DL_EXCEED			(uint8_t)25

#define	RESULT_INPUT_NOTALLOW	(uint8_t)26

//not key enter
#define	RESULT_INPUT_NONE	(uint8_t)40
//27	应用未建立	RESULT_APP_NOTEXIST
//28	应用未启动	RESULT_APP_NOTRUN
//29	应用锁定	RESULT_APP_LOCKED
//30	应用认证密钥未修改	RESULT_APP_NOTKEY
//31    应用已经满  RESULT_APP_OVERFLOW 
//32    RESULT_APP_NOAUTH     
#define RESULT_APP_NOTEXIST    (uint8_t)27
#define RESULT_APP_NOTRUN      (uint8_t)28
#define RESULT_APP_LOCKED      (uint8_t)29
#define RESULT_APP_OVERFLOW    (uint8_t)31
#define RESULT_APP_NOAUTH      (uint8_t)32
#define RESULT_APP_EXIST       (uint8_t)33  
#define RESULT_APP_TYPEERR     (uint8_t)34 
#endif

#endif
