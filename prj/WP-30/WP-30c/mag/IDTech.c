/*
********************************************************************************
********************************************************************************
*/

#include "wp30_ctrl.h"
#if (defined CFG_MAGCARD)
#if MAG_PRODUCT == EM_IDT178 
#include "bitbuf.h"
//#define EM_IDT_DEUBG

#define EM_SET_SPI_MOSI  
#define EM_CLR_SPI_MOSI 

#define EM_SET_SPI_CLK() 
#define EM_CLR_SPI_CLK()


void idt_enable_device(void)
{
}

void idt_disable_device(void)
{
}

void idt_enable_VCC(void)
{
    hw_mag_power_write(0);
	delay_ms(10);
}
void idt_disable_VCC(void)
{	
    hw_mag_power_write(1);
}

void spi_vWriteByte(unsigned char ucByte)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		// drop write
		EM_CLR_SPI_CLK();
		s_DelayUs(1);
		if(ucByte & gBitValue[i])
		{
			EM_SET_SPI_MOSI();
		}
		else
		{
			EM_CLR_SPI_MOSI();
		}
		EM_SET_SPI_CLK();
		s_DelayUs(1);
	}	
}

unsigned char spi_ucReadByte(void)
{
	int i;
	uchar ucData = 0;
	for(i = 0; i < 8; i++)
	{
		EM_CLR_SPI_CLK();
		s_DelayUs(2);
		EM_SET_SPI_CLK();
		s_DelayUs(1);
//		if ((PI1 & MTK_SPI_MISO) == MTK_SPI_MISO)
//		{
//			ucData |= gBitValue[i];
//		}	
	}	
	return ucData;
}

unsigned char spi_ucReadByte3(void)
{
	int i;
	uchar ucData = 0;
	for(i = 0; i < 8; i++)
	{
		EM_CLR_SPI_CLK();
		s_DelayUs(2);
		EM_SET_SPI_CLK();
		s_DelayUs(1);
//		if ((PI1 & MTK_SPI_MISO) == MTK_SPI_MISO)
//		{
//			ucData |= gBitValue[7-i];
//		}	
	}	
	return ucData;
}


/*F*********************************************************************
* NAME: ReadAllTracksSequentially
*----------------------------------------------------------------------------
* PARAMS:  none
* return: none
*----------------------------------------------------------------------------
* PURPOSE:
* Send out ATR before communication
*----------------------------------------------------------------------------
* NOTE:
*SPI Operational Command Descriptions:
Command 			Action				Serial Output
0x01 	Read track one data sequentially 		Header, Track 1 data, LRC
0x02 	Read track two data sequentially 		Header, Track 2 data, LRC
0x03 	Read track three data sequentially 		Header, Track 3 data, LRC
0x04 	Read all tracks sequentially 			Header, Track one data,
Track two data, Track
three data, LRC
0x05 	Arm to Read / Flush RAM buffer 		0x05
0x06 	Report Status 					Header
0x11 	Byte read track one data 			Header
0x12 	Byte read track two data 			Header
0x13 	Byte read track three data 			Header
0x14 	Byte read all track data 			Header
0x15 	Continue reading data 			Data or LRC
0x16 	End of Cycle / Reset 				None
0x00 or 0xFF 	NoOp					Empty, unless in
(preferably 0xFF)					sequential read, in which
							case the read continues
unaffected.

***********************************************************************/
uchar idt_ScanMagCard(uint *puiStack1BitLen, uint *puiStack2BitLen, uint *puiStack3BitLen,uchar * b1, uchar * b2, uchar * b3)
{
    uchar i;
    volatile uchar head;
	uchar data,lrc;
	idt_enable_device();
	//MTK_SPI_DAV ÉèÖÃÎªÊä³ö
    hw_mag_gpio_output(MAG_STROBE);
    hw_mag_strobe_write(1);
	// read trk1 
	spi_vWriteByte(0x01);            
	head = spi_ucReadByte(); 
	lrc = 0;
	for(i=0;i<96;i++)
	{
		 data = spi_ucReadByte3();
		 //if(data)
		 {
		 	*b1++ = data;
		 	lrc ^= data; 
		 }		 
	}
	data = spi_ucReadByte3();
	if(lrc != data)
	{
		*puiStack1BitLen = 0;
	}
	else
	{
		*puiStack1BitLen = 96*8;
	}
#ifdef EM_IDT_DEUBG
	TRACE("[%x-%x-%x-%d]",head,lrc,data,*puiStack1BitLen);
	vDispBufTitle("Trk1", 96, 0, b1-96);
#endif
	// read trk3 
	spi_vWriteByte(0x02);            
	head = spi_ucReadByte(); 
	lrc = 0;
	for(i=0;i<96;i++)
	{
		 data = spi_ucReadByte3();
		 //if(data)
		 {
		 	*b2++ = data;
		 	lrc ^= data; 
		 }
	}
	data = spi_ucReadByte3();
	if(lrc != data)
	{
		*puiStack2BitLen = 0;
	}
	else
	{
		*puiStack2BitLen = 96*8;
	}
#ifdef EM_IDT_DEUBG
	TRACE("[%x-%x-%x-%d]",head,lrc,data,*puiStack2BitLen);
	vDispBufTitle("Trk2", 96, 0, b2-96);
#endif	
	// read trk3 
	spi_vWriteByte(0x03);            
	head = spi_ucReadByte();  	
	lrc = 0;
	for(i=0;i<96;i++)
	{
		 data = spi_ucReadByte3();
		 //if(data)
		 {
		 	*b3++ = data;
		 	lrc ^= data; 
		 } 
	}
	data = spi_ucReadByte3();
	if(lrc != data)
	{
		*puiStack3BitLen = 0;
	}
	else
	{
		*puiStack3BitLen = 96*8;
	} 
#ifdef EM_IDT_DEUBG
	TRACE("[%x-%x-%x-%d]",head,lrc,data,*puiStack3BitLen);
	vDispBufTitle("Trk3", 96, 0, b3-96);
#endif	
	idt_disable_device();
    hw_mag_strobe_write(0);
	return 0;
}


uint idt_poll(void)
{
    uchar read;
	idt_enable_device();
	spi_vWriteByte(0x05);     // Sending out Arm to Read / Flush RAM Buffer: 0x05
	read = spi_ucReadByte();  // Sending out 0xff to get ATR response.
#ifdef EM_IDT_DEUBG	
	TRACE("%x",read);
#endif
	if(read == 0x05)
	{
		delay_ms(2);
		spi_vWriteByte(0x05);
		read = spi_ucReadByte();  // Sending out 0xff to get ATR response.  
	#ifdef EM_IDT_DEUBG	
		TRACE("%x",read);		
	#endif
		if(read == 0x05)
		{
			mtk_disable_device();
			return 0;
		}
	}
	mtk_disable_device();
	return 1;
}

void idt_RouteInit(void)
{
	uchar i;
    uchar read;
	idt_enable_device();
    for (i=0; i<3; i++)
    { 
        spi_vWriteByte(0x05);     // Sending out Arm to Read / Flush RAM Buffer: 0x05
		read = spi_ucReadByte();  // Sending out 0xff to get ATR response.   
        if (read == 0x05)            // Verify chip receives ATR command.
        {
			break;
        }
        s_DelayMs(10);          // Pull every 96 ms.        
#ifdef EM_IDT_DEUBG
        TRACE("Send0:%x",read);
#endif           
    } 
	mtk_disable_device();
}

/*
********************************************************************************
********************************************************************************
*/
uchar idt_ucIfDataRdy(void)
{
	uchar read;
	idt_enable_device();
	spi_vWriteByte(0x06);           // Report Status.
	read = spi_ucReadByte();        // Get status. 
	mtk_disable_device();
    if ((read & 0x03) == 0)
    {
    	return 0;	                  //Data not ready
    }
	return 1;	                    //Data ready
}

int mag_IDT178_init(int mode)
{
    return 0;
}

int mag_IDT178_open(int mode)
{
    idt_enable_VCC();
    idt_RouteInit();
    idt_enable_device(); 
	return 0;
}

int mag_IDT178_close(int mode)
{
    idt_disable_device();
    idt_disable_VCC();	
	return 0;
}
int mag_IDT178_ioctl(int mode,int value)
{
    return (int)idt_ucIfDataRdy();
}
int mag_IDT178_read(void *Track1, void *Track2, void *Track3)
{
	uchar ret;
	ET_MAGCARD *ptTrk1, *ptTrk2, *ptTrk3;	
	uchar *b1,*b2,*b3;
	uint b1bitlen, b2bitlen, b3bitlen;
	BitBuf SrcBitBuf;	
    int magret = 0x00;
    int maglen = 0;
    uchar *ptr;
	ptTrk1 = (ET_MAGCARD *)&gucBuff[512];  
	ptTrk2 = (ET_MAGCARD *)&gucBuff[512+sizeof(ET_MAGCARD)*1]; 
	ptTrk3 = (ET_MAGCARD *)&gucBuff[512+sizeof(ET_MAGCARD)*2];
	memset((char *)ptTrk1, 0, sizeof (ET_MAGCARD));
	memset((char *)ptTrk2, 0, sizeof (ET_MAGCARD));
	memset((char *)ptTrk3, 0, sizeof (ET_MAGCARD));
	b1 = pMagBufTmp1;
	b2 = b1+MAG_UINT_LEN;
	b3 = b2+MAG_UINT_LEN;	
	memset(b1,0,MAG_UINT_LEN);
	memset(b2,0,MAG_UINT_LEN);
	memset(b3,0,MAG_UINT_LEN);
	ret = idt_ScanMagCard(&b1bitlen,&b2bitlen,&b3bitlen,b1,b2,b3);
#ifdef MTK_CTRLDEBUG		
	TRACE("%d %d-%d-%d",ret,b1bitlen,b2bitlen,b3bitlen);
#endif
	if(ret)
	{
        magret = 0x70;
		goto app_read_magcard_end;
	}
	// Trk1 decode
	ret = EM_mag_NULL;
	if(b1bitlen)
	{		
		bitbufInit(&SrcBitBuf, b1, MAG_UINT_LEN * 8);
		ret = SI_mag_ucDecodeTrk1(&SrcBitBuf, b1bitlen, &ptTrk1->ucTrkLength, ptTrk1->aucTrkBuf, 1);	// forward
	#ifdef MTK_CTRLDEBUG	
		TRACE("Trk1 forward:%X", ret);
	#endif
		if (ret != EM_mag_SUCCESS)
		{
			ret = SI_mag_ucDecodeTrk1(&SrcBitBuf, b1bitlen, &ptTrk1->ucTrkLength, ptTrk1->aucTrkBuf, 0);	// backward
		#ifdef MTK_CTRLDEBUG	
			TRACE("Trk1 backward:%X", ret);
		#endif
		}
		if (ret == EM_mag_SUCCESS)
		{
			ptTrk1->ucTrkFlag = EM_mag_SUCCESS;
		}
	}
	if(ret)
	{
		memset((uchar *)ptTrk1,0,sizeof(ET_MAGCARD));
		ptTrk1->ucTrkFlag = ret;
        magret |= 0x10;
    }else{
        magret |= 0x01;
        maglen = ptTrk1->ucTrkLength;
        magret = magret|(maglen<<8);
        ptr = (uchar *)Track1;
        memcpy(ptr, ptTrk1->aucTrkBuf, ptTrk1->ucTrkLength);
    }
	
	// Trk2 decode
	ret = EM_mag_NULL;
	if(b2bitlen)
	{
		bitbufInit(&SrcBitBuf, b2, MAG_UINT_LEN * 8);
		ret = EI_mag_vProcTrk23(&SrcBitBuf, b2bitlen, &ptTrk2->ucTrkLength, ptTrk2->aucTrkBuf, 1);	// forward
	#ifdef MTK_CTRLDEBUG	
		TRACE("Trk2 forward:%X", ret);
	#endif
		if (ret != EM_mag_SUCCESS)
		{
			ret = EI_mag_vProcTrk23(&SrcBitBuf, b2bitlen, &ptTrk2->ucTrkLength, ptTrk2->aucTrkBuf, 0);	// backward
		#ifdef MTK_CTRLDEBUG	
			TRACE("Trk2 backward:%X", ret);
		#endif
		}
		if (ret == EM_mag_SUCCESS)
		{
			ptTrk2->ucTrkFlag = EM_mag_SUCCESS;
		}
	}
	if(ret)
	{
		memset((uchar *)ptTrk2,0,sizeof(ET_MAGCARD));
		ptTrk2->ucTrkFlag = ret;
        magret |= 0x20;
    }else{
        magret |= 0x02;
        maglen = ptTrk2->ucTrkLength;
        magret = magret|(maglen<<16);
        ptr = (uchar *)Track2;
        memcpy(ptr, ptTrk2->aucTrkBuf, ptTrk2->ucTrkLength);
    }
	
	// Trk3 decode
	ret = EM_mag_NULL;
	if(b3bitlen)
	{
		bitbufInit(&SrcBitBuf, b3, MAG_UINT_LEN * 8);
		ret = EI_mag_vProcTrk23(&SrcBitBuf, b3bitlen, &ptTrk3->ucTrkLength, ptTrk3->aucTrkBuf, 1);	// forward
	#ifdef MTK_CTRLDEBUG	
		TRACE("Trk3 forward:%X", ret);
	#endif
		if (ret != EM_mag_SUCCESS)
		{
			ret = EI_mag_vProcTrk23(&SrcBitBuf, b3bitlen, &ptTrk3->ucTrkLength, ptTrk3->aucTrkBuf, 0);	// backward
		#ifdef MTK_CTRLDEBUG	
			TRACE("\r\n Trk3 backward:%X", ret);
		#endif
		}
		if (ret == EM_mag_SUCCESS)
		{
			ptTrk3->ucTrkFlag = EM_mag_SUCCESS;
		}
	}
	if(ret)
	{
		memset((uchar *)ptTrk3,0,sizeof(ET_MAGCARD));
		ptTrk3->ucTrkFlag = ret;
        magret |= 0x40;
    }else{
        magret |= 0x04;
        maglen = ptTrk3->ucTrkLength;
        magret = magret|(maglen<<24);
        ptr = (uchar *)Track3;
        memcpy(ptr, ptTrk3->aucTrkBuf, ptTrk3->ucTrkLength);
    }
#ifdef MTK_CTRLDEBUG	
	vDispBufTitle("trk",sizeof(ET_MAGCARD)*3,0,(uchar *)ptTrk1);
	vDispBufTitle("trk2",sizeof(ET_MAGCARD),0,(uchar *)ptTrk2);
	vDispBufTitle("trk3",sizeof(ET_MAGCARD),0,(uchar *)ptTrk3);
#endif
	mag_close();
	PPRT_Send(CMD_READ_MAGCARD,(uchar *)ptTrk1,sizeof(ET_MAGCARD)*3);
	return magret;
app_read_magcard_end:
	mag_close();
	PPRT_Send(CMD_READ_MAGCARD,&ret,1);
	return magret;
}
int mag_IDT178_main(int mode)
{
    if (idt_ucIfDataRdy() == EM_SUCCESS){
        mag_IDT178_read(0);
        mag_IDT178_open(0);
    }
    return 0;
}

#endif
#endif



