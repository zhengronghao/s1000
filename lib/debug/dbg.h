/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : dbg.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/22/2014 11:17:28 AM
 * Description        : 
 *******************************************************************************/

#ifndef __DBG_H__
#define __DBG_H__ 


#ifdef  DBG_DES
int des_self_test( int verbose );
#endif

#ifdef CFG_DBG_MCG
void dbg_mcg(uint8_t *buf_cnl,uint32_t buf_size);
#endif
void dbg_out_srs(void);
#ifdef CFG_DBG_UART
void dbg_uart(void);
#endif
#ifdef CFG_DBG_FLASH
void dbg_flash(void);
#endif
#ifdef CFG_DBG_RTC
void dbg_rtc(void);
#endif
#ifdef CFG_DBG_RNG
void dbg_rnga(void);
#endif
#ifdef CFG_DBG_SECURYTY
void dbg_security(void);
#endif
#ifdef CFG_DBG_TIME
void dbg_time(void);
void dbg_time_read(void);
#endif
#ifdef CFG_DBG_DELAY
void dbg_delay(void);
#endif
#ifdef CFG_DBG_GPIO 
void dbg_gpio_reg(void);
#endif
#ifdef CFG_DBG_RSA
void dbg_rsa(void);
#endif
#ifdef CFG_DBG_USB
void dbg_usb(void);
#endif
#ifdef CFG_DBG_LCD
void dbg_lcd(void);
#endif
#ifdef CFG_DBG_LPWR
void dbg_lowerpower(void);
#endif
#ifdef CFG_DBG_CTRL_WORK_COMPORT 
void dbg_work_uart(void)
#endif
#ifdef CFG_DBG_KEYBOARD
void dbg_key(void);
#endif
#ifdef CFG_DBG_ADC
void dbg_adc(void);
#endif
#ifdef CFG_DBG_DRYICE
void dbg_dryice(void);
#endif

#endif

