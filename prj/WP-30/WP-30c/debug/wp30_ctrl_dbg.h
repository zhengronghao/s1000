/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : wp30_ctrl_dbg.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 8/11/2014 4:49:12 PM
 * Description        : 
 *******************************************************************************/
#ifndef __WP30_CTRL_DBG_H__
#define __WP30_CTRL_DBG_H__


#ifdef DEBUG_Dx 
//----------------------------------
//#define CFG_DBG_UART 
//#define CFG_DBG_MCG
//#define CFG_DBG_CRC
//#define CFG_DBG_FLASH
//#define CFG_DBG_RTC 
#define CFG_DBG_TIME 
//#define CFG_DBG_RNG 
//#define CFG_DBG_SECURYTY 
//#define CFG_DBG_DELAY 
//#define CFG_DBG_GPIO 
//#define CFG_DBG_KEYBOARD
//#define CFG_DBG_CTRL_WORK_COMPORT
//#define CFG_DBG_LPWR
#define CFG_DBG_DRYICE
//#define CFG_DBG_ADC
//#define CFG_DBG_RSA
//#define CFG_DBG_USB
//#define CFG_DBG_LCD
#define CFG_DBG_FLASH
#if (defined CFG_W25XFLASH)
#define CFG_DBG_EXFLASH
#endif
//#define CFG_DBG_BT
#define CFG_DBG_MAGTECK
#if defined(CFG_EXTERN_PINPAD) || defined(CFG_INSIDE_PINPAD)
//#define CFG_DBG_PINPAD
#endif
#if (defined CFG_RFID)
#define CFG_DBG_RFID
#endif
//----------------------------------
#ifdef CFG_DBG_SECURYTY
#define DBG_DES
#define DBG_SHA1
#define DBG_SHA256
#endif
#define CFG_DBG_LED 
#if (defined CFG_ICCARD)
#define CFG_DBG_ICCARD
#endif
//#define CFG_LIBAPI_DEBUG

//----------------------------------
void dbg_s1000_ctrl(void);
#endif /* #ifdef DEBUG_Dx */

//#define CFG_DBG_PROTOTYPE 

#ifdef CFG_DBG_PROTOTYPE
void proto_main(void);
#endif

#ifdef CFG_TPRINTER
#define CFG_DBG_TPRINTER
#endif

#if defined(CFG_SECURITY_CHIP)
#define CFG_DBG_SECURITY_CHIP
#endif

#endif /*  __WP30_CTRL_DBG_H__ */

