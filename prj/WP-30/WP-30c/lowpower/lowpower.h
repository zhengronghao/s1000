/***************** (C) COPYRIGHT 2015 START Computer equipment *****************
 * File Name          :  lowpower.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 4/28/2015 16:49:45 PM
 * Description        : 
 *******************************************************************************/
#ifndef __LOWERPOER_H__
#define __LOWERPOER_H__ 

struct LpwrGpioMode {
    GPIOPin_Def ptx;
    uint8_t     direct; //0-in 1-out
    uint8_t     level;
    uint8_t     rfu;
    uint32_t    mode;
};
#define LPWR_GPIO_IN        0
#define LPWR_GPIO_OUT       1

#define LPWR_LEVEL_HIGHT    1
#define LPWR_LEVEL_LOW      0

#define LPWR_MODE_OUTPUT      GPIO_OUTPUT_OpenDrainDisable
#define LPWR_MODE_INPUT       GPIO_INPUT_PULLDISABLE 
#define LPWR_MODE_INPUT_UP    GPIO_INPUT_PULLUP 

struct LpwrRFID {
    struct LpwrGpioMode notice;  //INT
    struct LpwrGpioMode mosi;
    struct LpwrGpioMode clk;
    struct LpwrGpioMode miso;
    struct LpwrGpioMode nss;
    struct LpwrGpioMode rst;
};

struct LpwrEXFLASH {
    struct LpwrGpioMode cs;
};

struct LpwrUART2 {
    struct LpwrGpioMode txd;
    struct LpwrGpioMode rxd;
};


struct LpwrICC {
    struct LpwrGpioMode clk;
    struct LpwrGpioMode offn;//INT
    struct LpwrGpioMode cmdvccn;
    struct LpwrGpioMode io;
    struct LpwrGpioMode pwr_5v;
    struct LpwrGpioMode pwr_1v;
    struct LpwrGpioMode rst;
    struct LpwrGpioMode aux1;
    struct LpwrGpioMode cs;
    struct LpwrGpioMode aux2;
};

struct LpwrAdc {
    struct LpwrGpioMode enable;
};

struct LpwrScan {
    struct LpwrGpioMode enable;
    struct LpwrGpioMode rxd;
    struct LpwrGpioMode txd;
    struct LpwrGpioMode trig;
};

struct LpwrSYS {
    struct LpwrGpioMode enable_5v;
    struct LpwrGpioMode enable_3v;
    //struct LpwrGpioMode wake_up;
    struct LpwrGpioMode com_en;
};

struct LpwrPRN {
    struct LpwrGpioMode pwr;
    struct LpwrGpioMode stb;
    struct LpwrGpioMode m_na;
    struct LpwrGpioMode m_a;
    struct LpwrGpioMode m_nb;
    struct LpwrGpioMode m_b;
    struct LpwrGpioMode work;
    struct LpwrGpioMode lat;
    struct LpwrGpioMode clk;
    struct LpwrGpioMode mosi;
};

struct LpwrBEEP {
    struct LpwrGpioMode pwm;
};

struct LpwrESAM {
    struct LpwrGpioMode rst;
    struct LpwrGpioMode shak;
    struct LpwrGpioMode cs;
    struct LpwrGpioMode clk;
    struct LpwrGpioMode mosi;
    struct LpwrGpioMode miso;
};

struct LpwrMAG {
    struct LpwrGpioMode h0;
    struct LpwrGpioMode h1;
    struct LpwrGpioMode h2;
};

struct LpwrCTP {
    struct LpwrGpioMode in;
    struct LpwrGpioMode rst;
    struct LpwrGpioMode ctl;
    struct LpwrGpioMode clk;
    struct LpwrGpioMode sda;
};

struct LpwrLED {
    struct LpwrGpioMode blue;
    struct LpwrGpioMode yellow;
    struct LpwrGpioMode green;
    struct LpwrGpioMode red;
};

struct LpwrSAM {
    struct LpwrGpioMode clk;
    struct LpwrGpioMode rst1;
    struct LpwrGpioMode vcc_en1;
    struct LpwrGpioMode io1;
    struct LpwrGpioMode ret2;
    struct LpwrGpioMode vcc_en2;
    struct LpwrGpioMode io2;
};

struct LpwrDevice {
#if (defined CFG_RFID)
    struct LpwrRFID rfid;
    struct LpwrLED led;
#endif

#if (defined CFG_W25XFLASH)
    struct LpwrEXFLASH exflash;
#endif

#if (defined CFG_ICCARD_USERCARD)
    struct LpwrICC icc;
#endif

#if (defined CFG_SCANER)
    struct LpwrScan scan;
#endif

#if (defined CFG_TPRINTER)
    struct LpwrPRN prn;
#endif

#if (defined CFG_SM2 || defined CFG_SM3 || defined CFG_SM4)
    struct LpwrESAM esam;
#endif

#if (defined CFG_MAGCARD)
    struct LpwrMAG mag;
#endif

#if (defined CFG_ICCARD)
    struct LpwrSAM sam1;
#endif

    struct LpwrUART2 uart2;
    struct LpwrUART2 uart4;

    struct LpwrAdc adc;
    struct LpwrSYS sys;
    struct LpwrBEEP beep;
    struct LpwrCTP ctp; 
};

/*************************************
 *  µÍ¹¦ºÄÄ£¿é
 *************************************/
#define SYS_STATE_NORMAL          0x0000
#define SYS_STATE_LOWPOWER          0x0001

#define     EVENT_TIMEOUT           0x01
#define     EVENT_KEYPRESS          0x02
#define     EVENT_MAGSWIPED         0x04
#define     EVENT_ICCIN             0x08
#define     EVENT_UARTRECV          0x10
#define     EVENT_USBRECV           0x20
#define     EVENT_ALL  (EVENT_TIMEOUT|EVENT_KEYPRESS|EVENT_MAGSWIPED|EVENT_ICCIN|EVENT_UARTRECV|EVENT_USBRECV)
void ctrl_pherip_reinit(void);
void lowerpower_init(void);
void enter_lowerpower_llwu(void);
void enter_lowerpower_wait(void);
void enter_lowerpower_freq(void);
void exit_lowerpower_freq(void);
uint32_t sys_power_save(uint32_t Event, uint32_t TimeOutMs);
int sys_SleepWaitForEvent(uint waitevent, uint mode,uint timeout_ms);
void LowPower_Wakeup_IRQHandler(void);
#endif

