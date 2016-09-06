#ifndef _S1000_BOARDTEST_H_
#define _S1000_BOARDTEST_H_
#include "wp30_ctrl.h"

#define MODULE_AMOUNT 14  //Ä£¿é×ÜÊý


//PRN
#define PRN_nLAT        PTB17        //TP46
#define PRN_STB         PTB7     //TP37
#define PRN_MTA         PTB9    //TP52
#define PRN_MTAN        PTB8    //TP51
#define PRN_MTB         PTB11    //TP54
#define PRN_MTBN        PTB10    //TP53
#define PRN_MOSI        PTD13    //TP48
#define PRN_CLK         PTD12    //TP47
#define PRN_PE         PTD12    //TP47
#define PRN_TM         PTD12    //TP47

//android GPIO
#define Android_WAKE_UP   PTB10    //PTB10
#define Android_HEARTBEAT PTE18   //PTE18



int fac_init(int mode);
int fac_dl_boot(void *param);
int fac_dl_ctrl(void *param);
int fac_dl_font(void *param);
int test_err(int mode);

#endif
