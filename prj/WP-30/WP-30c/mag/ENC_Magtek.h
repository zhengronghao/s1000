



#ifndef __ENC_MAGTECK_H__
#define __ENC_MAGTECK_H__



//-----------------------
//SPI_DAV:  P1.2
//SPI_VCT:  P1.3
//SPI_MISO: P1.4
//SPI_MOSI: P1.5
//SPI_CLK:  P1.6 
//SPI_CS:   P1.7
//-----------------------

#define MTK_SPI_DAV  0x04
#define MTK_SPI_VCT  0x08
#define MTK_SPI_MISO 0x10
#define MTK_SPI_MOSI 0x20
#define MTK_SPI_CLK  0x40
#define MTK_SPI_CS   0x80



//#define MTK_SET_SPI_MOSI()  do{PO1 |= (uchar)MTK_SPI_MOSI;}while(0)
//#define MTK_CLR_SPI_MOSI()  do{PO1 &= (uchar)(~MTK_SPI_MOSI);}while(0)
//
//
//#define MTK_SET_SPI_CLK()   do{PO1 |= (uchar)MTK_SPI_CLK;}while(0) 
//#define MTK_CLR_SPI_CLK()   do{PO1 &= (uchar)(~MTK_SPI_CLK);}while(0) 

#define MTK_SET_SPI_MOSI()  
#define MTK_CLR_SPI_MOSI()  


#define MTK_SET_SPI_CLK()  
#define MTK_CLR_SPI_CLK() 



#define REQUEST_MESSAGE         0
#define RESPONSE_MESSAGE        1
#define NOTIFICATION_MESSAGE    2

#define GET_PROPERTY       0
#define SET_PROPERTY       1
#define SAVE_PROPERTY      2
#define RST_DEVICE         3
#define LOAD_DEVICE_KEY    4
#define EXTERNAL_AUTHENTICATE_COMMAND 5


#define MTK_SOF  0x01
 
//#define MTK_CTRLDEBUG  1



typedef	struct  _ET_MTK_INPKT_
{
	unsigned char	sof;              
	unsigned char	len_h;	
	unsigned char	len_l;    
	unsigned char	mtyp;
	unsigned char	cmd;
	unsigned char	pid;	
} ET_MTK_INPKT;
// 4B 
#define EM_MTK_INPKT_LEN  6 //sizeof(ET_MTK_INPKT)



#define RC_SUCCESS        0
#define RC_FAILURE        1
#define RC_BAD_PARAMETER  2

#define MTK_SUCCESS     EM_SUCCESS
#define MTK_ERROR       EM_ERROR
#define MTK_ERRPARAM    EM_ERRPARAM

#define MTK_ERR_SOF     0x0A  //
#define MTK_ERR_LEN     0x0B  //
#define MTK_ERR_ACK     0x0C  //


void mtk_enable_VCC(void);
void mtk_disable_VCC(void);
void mtk_enable_device(void);
void mtk_disable_device(void);
void mtk_Init(void);
uchar mtk_ucIfDataRdy(void);
uchar mtk_ScanMagCard(uint *b1Len,uint *b2Len,uint *b3Len,uchar *b1, uchar * b2, uchar * b3);
int mtk_iGetSoftwareID(int *pRetlen,unsigned char *pDest);
int mtk_iGetDevSerial(int *pRetlen,unsigned char *pDest);
int mtk_iSetDevSerial(unsigned char inlen,unsigned char *pinBuff);
int mtk_iGetEncRandomData(int *pRetlen,unsigned char *pDest);
int mtk_iGetEncryptCaraDataSW(int *pRetlen,unsigned char *pDest);
int mtk_iSetEncryptCaraDataSW(unsigned char inlen,unsigned char *pinBuff);
int mtk_iGetLockDeviceSW(int *pRetlen,unsigned char *pDest);
int mtk_iSetLockDeviceSW(unsigned char inlen,unsigned char *pinBuff);
int mtk_iLoadDeviceKey(unsigned char part,unsigned char inlen,unsigned char *pinBuff);
int mtk_iExternalAuthCmd(unsigned char inlen,unsigned char *pinDevkey);
int mtk_SaveDataCmd(void);
int mtk_RstDeviceCmd(void);


int mag_MTK211_init(int mode);
int mag_MTK211_open(int mode);
int mag_MTK211_close(int mode);
int mag_MTK211_read(void *Track1, void *Track2, void *Track3);
int mag_MTK211_ioctl(int mode, int para);
int mag_MTK211_main (int mode);

#endif





