

#ifndef __MAGCARD_H__
#define __MAGCARD_H__

#include "bitbuf.h"

#ifndef     IO_OUT
#define     IO_OUT              0
#endif

#ifndef     IO_IN
#define     IO_IN               1
#endif
#define MEGTEK_VCC 0x08
#define MEGTEK_DATA 0x80
#define MEGTEK_STROBE 0x40


// mag type
#define EM_IDT178        1  //IDTECH
#define EM_MTK211        2  //megtek spi 加密磁头
#define	EM_MAGTEK        3  //量产版
#define	EM_HCM4003       4  //软解码

#define MAG_PRODUCT     EM_MAGTEK 
/*
*****************************************************************************************    
*Note:        macro  of   decode              
*****************************************************************************************
*/
 
#define EM_mag_INPROC               0xBB
#define EM_mag_TRK1MASK             0x7F
#define EM_mag_TRK1HEAD             0x51
#define EM_mag_TRK1TAIL             0x7C
#define EM_mag_TRK23MASK            0x1F
#define EM_mag_TRK23HEAD            0x1A
#define EM_mag_TRK23ISOTAIL         0x1F
#define EM_mag_TRK23IBMTAIL         0x07
#define EM_mag_SUCCVALUE            10
#define EM_mag_LRCVALUE             3
#define EM_mag_ENDVALUE             1
#define EM_mag_TRK1MINLEN           28
#define EM_mag_TRK23MINLEN          20
#define EM_NOIBMEND                 0
#define EM_ISIBMEND                 1



/******************************************************************************
	                            
*******************************************************************************/
#define EM_TEXT_MAXLENGTH            19	// max len of card no 


// input
#define EM_mag_TRK1ON                   (uchar)0x01	// Trk1 open
#define EM_mag_TRK1OFF                  (uchar)0x00	// Trk1 close
#define EM_mag_TRK2ON                   (uchar)0x02	// Trk2 open
#define EM_mag_TRK2OFF                  (uchar)0x00	// Trk2 close
#define EM_mag_TRK3ON                   (uchar)0x04	// Trk3 open
#define EM_mag_TRK3OFF                  (uchar)0x00	// Trk3 close

// set opt mode 
#define EM_RECARD                       (uchar)0x00	//read card by mode of timerout
#define EM_TRACKIN                      (uchar)0x01	//read 

// manual input card no
#define EM_mag_ENMANUAL_SEPARATOR       (uchar)0x02 //
#define EM_mag_ENMANUAL                 (uchar)0x01	// 
#define EM_mag_DISMANUAL                (uchar)0x00	//

// output 
#define EM_mag_NORMAL                   (uchar)0x00
#define EM_mag_MANUAL                   (uchar)0x04
#define EM_mag_MANUALEXP                (uchar)0x74


/*
*****************************************************************************************    
*Note:           err code macro of decode                
*****************************************************************************************
*/ 
#define EM_mag_SUCCESS		            (uchar)0x00	// OK 
#define EM_mag_NULL			            (uchar)0x01	// blank track 

#define EM_mag_HEADERR		            (uchar)0x0A	// not find start bit
#define EM_mag_PARERR		            (uchar)0x0B	// parity check err
#define EM_mag_TAILERR		            (uchar)0x0C	// not find stop bit
#define EM_mag_LRCERR		            (uchar)0x0D	// LCR check err
#define EM_mag_ENDERR		            (uchar)0x0E	// err aften LCR check byte 
#define EM_mag_NULLEXP		            (uchar)0x0F	// 

// LRC
#define EM_mag_ENLRC                    (uchar)0x01	// 
#define EM_mag_DISLRC                   (uchar)0x00	// 

#define BAD_DATA            5
#define FIRST_TRK_CHAR      (79+BAD_DATA)    //trk1 max char 
#define SECOND_TRK_CHAR     (40+BAD_DATA)    //trk2 max char 
#define THREE_TRK_CHAR      (107+BAD_DATA)   //trk3 max char 
#define MAX_CHARACTERS      (107+BAD_DATA)   // max char 

typedef struct MAGCARD
{
	uchar ucTrkFlag;
	uchar ucTrkLength;
	uchar aucTrkBuf[108+2];
} ET_MAGCARD;

typedef struct ByteInfo
{
    uchar   buf[110];
    int     BitOneNum;
	int 	byte;
}MSR_ByteInfo;
//gucBuff[2050]
#define  OFF_MAG_TMP0   1040     //src 2050-1040=1010/2=505
	#define MAG_UINT_LEN  120
#define  OFF_MAG_TMP1   (OFF_MAG_TMP0 + MAG_UINT_LEN*3)  //dest


#define  pMagBufTmp0   &gucBuff[OFF_MAG_TMP0]
#define  pMagBufTmp1   &gucBuff[OFF_MAG_TMP1]


typedef struct __MAG_FOPS_t {
    uchar chiptype;
    int (*init)(int mode);
    int (*open)(int mode);
    int (*close)(int mode);
//    int (*write)(int mode,int value);
    int (*read)(void *Track1, void *Track2, void *Track3);//79 40 107
    int (*ioctl)(int mode, int para);
    int (*main)(int mode);
}MAG_FOPS_t;

typedef	struct  _ET_MAG_INFO_
{
	uchar magtype;    //
	uchar magstat;    //0-关闭 1-开启
	uchar mode;       //D0: 0-不加密 1-加密   
                      //D1: Magteck磁头阶段标志 0-第1阶段 1-第2阶段
                      //D2: 
    uchar rfu;
//	uchar MagKey[16];
    MAG_FOPS_t *magfops;
} ET_MAG_INFO;


//MODE
#define MAG_MODE_DISMANUAL   (0<<0)
#define MAG_MODE_ENMANUAL    (1<<0)  //允许手输入
#define MAG_MODE_DISENCRYPT  (0<<1)  //非加密模式
#define MAG_MODE_ENENCRYPT   (1<<1)  //加密模式

extern ET_MAG_INFO gMagInfo;
extern uchar const gBitValue[8];

#define delay_ms s_DelayMs  
#define delay_us s_DelayUs

uchar SI_mag_ucDecodeTrk1(BitBuf *pucRecordSrcBuf, uint usRecordLength, uchar *outlen, uchar * dest, uchar f);
uchar EI_mag_vProcTrk23(BitBuf *pucRecordSrcBuf, uint usRecordLength, uchar *outlen, uchar * dest, uchar f);
uchar mag_get(void);
void mag_set(uchar type);
void mag_open(void);
void mag_close(void);
int mag_init(int mode);
int mag_check(int mode);
void mag_clear(void);
int mag_read(void *Track1, void *Track2, void *Track3);
int magcard_main(int mode);



/*-----------------------------------------------------------------------------}
 *  调试口
 *-----------------------------------------------------------------------------{*/
#define SET_IO(pin) 
#define CLR_IO(pin)

#endif












