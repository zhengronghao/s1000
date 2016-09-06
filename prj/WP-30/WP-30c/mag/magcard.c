#include "wp30_ctrl.h"
#include "bitbuf.h"

#if (defined CFG_MAGCARD)


/**********************************************************************
*
*
*   extern function declare
*
*                         
***********************************************************************/
/**********************************************************************
*
*
*   globel variable
*
*                         
***********************************************************************/
uchar const gBitValue[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
const MAG_FOPS_t  gMagFops[] = {
#if MAG_PRODUCT == EM_IDT178 
    {
        .chiptype = EM_IDT178,
        .init  = mag_IDT178_init,
        .open  = mag_IDT178_open,
        .close = mag_IDT178_close,
        .read  = mag_IDT178_read,
        .ioctl = mag_IDT178_ioctl,
        .main  = mag_IDT178_main
    },
#endif
#if 11//MAG_PRODUCT == EM_MAGTEK 
    {
        .chiptype = EM_MAGTEK,
        .init  = mag_magtek_init,
        .open  = mag_magtek_open,
        .close = mag_magtek_close,
        .read  = mag_magtek_read,
        .ioctl = mag_magtek_ioctl,
        .main  = mag_magtek_main
    },
#endif
#if 11//MAG_PRODUCT == EM_HCM4003
    {
        .chiptype = EM_HCM4003,
        .init  = mag_hcm4003_init,
        .open  = mag_hcm4003_open,
        .close = mag_hcm4003_close,
        .read  = mag_hcm4003_read,
        .ioctl = mag_hcm4003_ioctl,
        .main  = mag_hcm4003_main
    },
#endif
#if MAG_PRODUCT == EM_MTK211 
    {
        .chiptype = EM_MTK211,
        .init  = mag_MTK211_init,
        .open  = mag_MTK211_open,
        .close = mag_MTK211_close,
        .read  = mag_MTK211_read,
        .ioctl = mag_MTK211_ioctl,
        .main  = mag_MTK211_main 
    },
#endif
};
ET_MAG_INFO gMagInfo UPAREA;

//#define DEBUG_MAGCARD  1
/**********************************************************************
 *
***********************************************************************/

/**********************************************************************
* name:
*   void disptrackdata_new(uchar *ucSrc,uchar *ucDest)
* describe:
*  Hex turn to Asc
***********************************************************************/
/*
void disptrackdata_new(uchar * ucSrc, uchar * ucDest)
{
	int nLen = 0, i = 0, nOffset = 0;
	Hex2Str(120, ucSrc, &i, ucDest);
	for (i = 0; i < 240; i++)
	{
		if (ucDest[i] == 'b' || ucDest[i] == 'B')
			break;
	}
	nOffset = i + 1;
	for (i = nOffset; i < 240; i++)
	{
		if (ucDest[i] == 'd' || ucDest[i] == 'D')
			ucDest[i] = '=';
		if (ucDest[i] == 'F' || ucDest[i] == 'f')
			break;
	}
	nLen = i - nOffset;

	memcpy(ucSrc, &ucDest[nOffset], nLen);

	memcpy(ucDest, ucSrc, nLen);
	ucDest[nLen] = '\0';

}
*/
/*
uchar gettrk23MagData(uchar data)
{
	if(data <= 9)
	{
		return data+'0';
	}
	else if(data == 0x0d)
	{
		return '=';
	}
	return 0;
}
*/
#define  gettrk23MagData(data) (data+0x30)
#define  gettrk1MagData(data)  (data+0x20)

/**********************************************************************
* name:
*   SI_mag_ucDecodeTrk1
* describe :
*   scan  track1 
* input:
*   dest:             
*   f=0,Forward     f=0,Backward                   
* output                
* return:
*   
***********************************************************************/
uchar SI_mag_ucDecodeTrk1(BitBuf * pucRecordSrcBuf, uint usRecordLength, uchar *outlen, uchar * dest, uchar f)
{
	uint i, k;
	uchar j, count = 0;
	uchar ack = EM_mag_INPROC;
	uchar ucTempValue = 0;
	uchar ucRecordValue = 0, ucLRCValue = 0;

	// 判磁道的头信息    
	for (i = 0; i < usRecordLength; i++)
	{
		ucRecordValue = (ucRecordValue << 1);
		if (f > 0)
		{
			k = i;
		}
		else
		{
			k = usRecordLength - 1 - i;
		}

		ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, k);
		if ((ucRecordValue & EM_mag_TRK1MASK) == EM_mag_TRK1HEAD)
		{
			break;
		}
	}
	if (i == usRecordLength)
	{
		ack = EM_mag_HEADERR;
	}
	else
	{
		i++;
		ucLRCValue = EM_mag_TRK1HEAD;
		while (i < usRecordLength)
		{
			ucRecordValue = 0x00;

			for (j = 0; j < 7; j++)
			{
				if (i == usRecordLength)
				{
					ack = EM_mag_TAILERR;
					break;
				}
				ucRecordValue = (ucRecordValue << 1);
				if (f > 0)
				{
					k = i++;
				}
				else
				{
					k = usRecordLength - 1 - i++;
				}
				ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, k);
			}

			if (ack == EM_mag_TAILERR)
			{
				break;
			}

			if (ucRecordValue == EM_mag_TRK1TAIL)
			{
				ucLRCValue ^= ucRecordValue;
				ucRecordValue = 0x00;

				for (j = 0; j < 7; j++)
				{
					ucRecordValue = (ucRecordValue << 1);
					if (i == usRecordLength)
					{
						ucRecordValue += 0x00;
					}
					else
					{
						if (f > 0)
						{
							k = i;
						}
						else
						{
							k = usRecordLength - 1 - i;
						}
						ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, k);
						i++;
					}
				}

				//整个track的LRC校验
				if ((ucRecordValue & 0x7E) != (ucLRCValue & 0x7E))
				{
					ack = EM_mag_LRCERR;
					break;
				}
				else
				{
					// 判断后续有无后导零
					ucRecordValue = 0x00;

					for (j = 0; j < 7; j++)
					{
						ucRecordValue = (ucRecordValue << 1);
						if (i == usRecordLength)
						{
							ucRecordValue += 0x00;
						}
						else
						{
							if (f > 0)
							{
								k = i;
							}
							else
							{
								k = usRecordLength - 1 - i;
							}
							ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, k);
							i++;
						}
					}

					if (ucRecordValue != 0x00)
					{
						ack = EM_mag_ENDERR;
						break;
					}
					else
					{
						ack = EM_mag_SUCCESS;
						break;
					}
				}

			}							//END IF 如果是ISO7811结束符或IBM结束符 判断HJZ   

			ucLRCValue ^= ucRecordValue;
			//odd check
			ucTempValue = 0x00;
			for (j = 0; j < 7; j++)
			{
				ucTempValue += (ucRecordValue >> j);
				ucTempValue &= 0x01;
			}
			if (ucTempValue != 0x01)
			{
				ack = EM_mag_PARERR;
				break;
			}
			else
			{
				ucTempValue = 0x00;
				for (j = 1; j < 7; j++)
				{
					ucTempValue <<= 1;
					if (((ucRecordValue >> j) & 0x01) == 0x01)
					{
						ucTempValue++;
					}
				}
				dest[count++] = gettrk1MagData(ucTempValue);
			}
		}
		// 如果未找到结束符,则返回结束符错        
		if ((i >= usRecordLength) && (ack == EM_mag_INPROC))
		{
			ack = EM_mag_TAILERR;
		}
	}
	*outlen = count;
	return ack;
}

/**********************************************************************
* name:
*   EI_mag_vProcTrk23
* describe :
*   scan 2nd or 3rd track 
* input:
*   dest:             
*   f > 0, Forward  =1,  Backward                   
* output                
* return:

  A: 未找到头
  B: 奇偶校验错误
  C: 未找到结束符
  D: LCR校验错误
  E: 
*   
***********************************************************************/
uchar EI_mag_vProcTrk23(BitBuf * pucRecordSrcBuf, uint usRecordLength, uchar *outlen, uchar * dest, uchar f)
{
	uchar ucRecordValue, ucValuePoint, ucLRCValue;
	uchar ucIfIBMEndFlag = EM_NOIBMEND, ucTempValue;
	unsigned int i, j;

	unsigned char ack;

	ucTempValue = 0x00;

	if (usRecordLength <= EM_mag_TRK23MINLEN)
	{
		return EM_mag_HEADERR;
	}

	ack = EM_mag_INPROC;
	ucRecordValue = 0x00;
	//vDispBufTitle("==trk data==",usRecordLength/8, 0, pucRecordSrcBuf->dataptr);
	// Forward  decord
	if (f > 0)
	{
		// check head
		for (i = 0; i < usRecordLength; i++)
		{
			ucRecordValue = (ucRecordValue << 1);
			ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, i);
			if ((ucRecordValue & EM_mag_TRK23MASK) == EM_mag_TRK23HEAD)
			{
				//TRACE("\r\n find head:%03d:%02X", i, ucRecordValue);
				break;
			}

		}
		if (i == usRecordLength)
		{
			// not find start bit
			ack = EM_mag_HEADERR;
		}
		else
		{
			i++;
			ucValuePoint = 0x00;
			ucLRCValue = EM_mag_TRK23HEAD;
			while (i < usRecordLength)
			{
				ucRecordValue = 0x00;
				for (j = 0; j < 5; j++)
				{
					if (i == usRecordLength)
					{
						// not find stop bit
						ack = EM_mag_TAILERR;
						break;
					}
					ucRecordValue = (ucRecordValue << 1);
					ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, i++);
				}
				//TRACE("[%03d:%02X]", i, ucRecordValue);
				if (ack == EM_mag_TAILERR)
				{
					break;
				}
				// if IBM Mag data, if 2rd byte is zero
				if (ucRecordValue == EM_mag_TRK23IBMTAIL)
				{
					//TRACE("\r\n IBM Tail");
					ucTempValue = 0x00;
					for (j = 5; j < 10; j++)
					{
						ucTempValue = (ucTempValue << 1);
						if ((i + j) == usRecordLength)
						{
							ucTempValue += 0x00;
						}
						else
						{
							
							ucTempValue += bitbufGetAtIndex(pucRecordSrcBuf, i+j);
						}
					}
					//TRACE("[%03d:%02X]", i, ucTempValue);
					if (ucTempValue != 0x00)
					{
						ucIfIBMEndFlag = EM_NOIBMEND;
					}
					else
					{
						ucIfIBMEndFlag = EM_ISIBMEND;
					}
				}
				if ((ucRecordValue == EM_mag_TRK23ISOTAIL)
					|| ((ucRecordValue == EM_mag_TRK23IBMTAIL) && (ucIfIBMEndFlag == EM_ISIBMEND)))
				{
					// decode is over
					//TRACE("\r\n decode is over");
					ucLRCValue ^= ucRecordValue;
					ucRecordValue = 0x00;
					for (j = 0; j < 5; j++)
					{
						ucRecordValue = (ucRecordValue << 1);
						if (i == usRecordLength)
						{
							ucRecordValue += 0x00;
						}
						else
						{

							ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, i++);
						}
					}
					//TRACE("[%03d:%02X]", i, ucRecordValue);
					if (0)
					{
						if (ucRecordValue != 0x00)
						{
							ack = EM_mag_ENDERR;
							break;
						}
						else
						{
							ack = EM_mag_SUCCESS;
							break;
						}
					}
					else
					{
						if ((ucRecordValue & 0x1E) != (ucLRCValue & 0x1E))
						{
							ack = EM_mag_LRCERR;
							break;
						}
						else
						{
							ucRecordValue = 0x00;
							for (j = 0; j < 5; j++)
							{
								ucRecordValue = (ucRecordValue << 1);
								if (i == usRecordLength)
								{
									ucRecordValue += 0x00;
								}
								else
								{
									ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, i++);
								}
							}
							//TRACE("[%03d:%02X]", i, ucRecordValue);
							if (ucRecordValue != 0x00)
							{
								ack = EM_mag_ENDERR;
								break;
							}
							else
							{
								ack = EM_mag_SUCCESS;
								break;
							}
						}
					}
				}						//END IF if stop char of ISO7811 or IBM 

				ucLRCValue ^= ucRecordValue;
				//TRACE("<LRC:%2X>", ucLRCValue);
				// odd check 
				ucTempValue = 0x00;
				for (j = 0; j < 5; j++)
				{
					ucTempValue += (ucRecordValue >> j);
					ucTempValue &= 0x01;
				}
				if (ucTempValue != 0x01)
				{
					ack = EM_mag_PARERR;
					break;
				}
				else
				{
					ucTempValue = 0x00;
					for (j = 1; j < 5; j++)
					{
						if (((ucRecordValue >> j) & 0x01) == 0x01)
						{
							ucTempValue = (ucTempValue << 1) + 1;
						}
						else
						{
							ucTempValue = ucTempValue << 1;
						}
					}
					//TRACE("(%02X)\r\n", ucTempValue);
					dest[ucValuePoint++] = gettrk23MagData(ucTempValue);      
				}
			}
			if ((i >= usRecordLength) && (ack == EM_mag_INPROC))
			{
				ack = EM_mag_TAILERR;
			}
		}
	}
	else
	{
		// check head
		for (i = 0; i < usRecordLength; i++)
		{
			ucRecordValue = (ucRecordValue << 1);
			ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, usRecordLength-1-i);
			if ((ucRecordValue & EM_mag_TRK23MASK) == EM_mag_TRK23HEAD)
			{
				//TRACE("\r\n find head:%03d:%02X", i, ucRecordValue);
				break;
			}
		}
		if (i == usRecordLength)
		{
			// not find start bit
			ack = EM_mag_HEADERR;
		}
		else
		{
			i++;
			ucValuePoint = 0x00;
			ucLRCValue = EM_mag_TRK23HEAD;
			while (i < usRecordLength)
			{
				ucRecordValue = 0x00;
				for (j = 0; j < 5; j++)
				{
					if (i == usRecordLength)
					{
						// not find stop bit
						ack = EM_mag_TAILERR;
						break;
					}
					ucRecordValue = (ucRecordValue << 1);
					ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, usRecordLength-1-(i++));
				}
				//TRACE("[%03:%02X]", i, ucRecordValue);
				if (ack == EM_mag_TAILERR)
				{
					break;
				}
				// if IBM Mag data, if 2rd byte is zero
				if (ucRecordValue == EM_mag_TRK23IBMTAIL)
				{
					//TRACE("\r\n IBM Tail");
					ucTempValue = 0x00;
					for (j = 5; j < 10; j++)
					{
						ucTempValue = (ucTempValue << 1);
						if ((i + j) == usRecordLength)
						{
							ucTempValue += 0x00;
						}
						else
						{
							ucTempValue += bitbufGetAtIndex(pucRecordSrcBuf, usRecordLength-1-i-j);
						}
					}
					//TRACE("[%03:%02X]", i, ucRecordValue);
					if (ucTempValue != 0x00)
					{
						ucIfIBMEndFlag = EM_NOIBMEND;
					}
					else
					{
						ucIfIBMEndFlag = EM_ISIBMEND;
					}
				}

				if ((ucRecordValue == EM_mag_TRK23ISOTAIL)
					|| ((ucRecordValue == EM_mag_TRK23IBMTAIL) && (ucIfIBMEndFlag == EM_ISIBMEND)))
				{
					// decode is over
					//TRACE("\r\n decode is over");
					ucLRCValue ^= ucRecordValue;
					ucRecordValue = 0x00;
					for (j = 0; j < 5; j++)
					{
						ucRecordValue = (ucRecordValue << 1);
						if (i == usRecordLength)
						{
							ucRecordValue += 0x00;
						}
						else
						{
							ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf, usRecordLength-1-(i++));
						}
					}
					//TRACE("[%03:%02X]", i, ucRecordValue);
					if (0)
					{
						if (ucRecordValue != 0x00)
						{
							ack = EM_mag_ENDERR;
							break;
						}
						else
						{
							ack = EM_mag_SUCCESS;
							break;
						}
					}
					else
					{
						if ((ucRecordValue & 0x1E) != (ucLRCValue & 0x1E))
						{
							ack = EM_mag_LRCERR;
							break;
						}
						else
						{
							ucRecordValue = 0x00;
							for (j = 0; j < 5; j++)
							{
								ucRecordValue = (ucRecordValue << 1);
								if (i == usRecordLength)
								{
									ucRecordValue += 0x00;
								}
								else
								{
									ucRecordValue += bitbufGetAtIndex(pucRecordSrcBuf,usRecordLength-1-(i++));
								}
							}
							//TRACE("[%03:%02X]", i, ucRecordValue);
							if (ucRecordValue != 0x00)
							{
								ack = EM_mag_ENDERR;
								break;
							}
							else
							{
								ack = EM_mag_SUCCESS;
								break;
							}
						}
					}
				}						//END IF if stop char of ISO7811 or IBM 
				ucLRCValue ^= ucRecordValue;
				//TRACE("<LRC:%2X>", ucLRCValue);
				// odd check 

				ucTempValue = 0x00;
				for (j = 0; j < 5; j++)
				{
					ucTempValue += (ucRecordValue >> j);
					ucTempValue &= 0x01;
				}
				if (ucTempValue != 0x01)
				{
					ack = EM_mag_PARERR;
					break;
				}
				else
				{
					ucTempValue = 0x00;
					for (j = 1; j < 5; j++)
					{
						if (((ucRecordValue >> j) & 0x01) == 0x01)
						{
							ucTempValue = (ucTempValue << 1) + 1;
						}
						else
						{
							ucTempValue = ucTempValue << 1;
						}
					}
					//TRACE("(%2X)\r\n", ucTempValue);
					dest[ucValuePoint++] = gettrk23MagData(ucTempValue);     
				}
			}
			if ((i >= usRecordLength) && (ack == EM_mag_INPROC))
			{
				ack = EM_mag_TAILERR;
			}
		}
	}									//END  IF F> 0

	if (ack == EM_mag_SUCCESS)
	{
		*outlen = ucValuePoint;
	}
	return (ack);
}

int mag_check(int mode)
{
    if ( gMagInfo.magfops ) {
        if ( gMagInfo.magstat ) {
            return (uchar)gMagInfo.magfops->ioctl(mode,0);
        }
    }
    //无磁卡数据
    return EM_mag_NULL;
}

void mag_clear(void)
{
    if ( gMagInfo.magfops ) {
        if(gMagInfo.magstat) {
            gMagInfo.magfops->ioctl(1,0);
        }
    }
}

int mag_init(int mode)
{
    uint i,type;
    MODULE_MAGCARD_INFO tmagcardinfo;
    memset((uchar*)&tmagcardinfo, 0xFF, sizeof(tmagcardinfo));
    s_sysinfo_magcardinfo (0, &tmagcardinfo);
    // 默认magteck
//    type = EM_MAGTEK;
    type = tmagcardinfo.type;
    if ( type == 0xFF ) {
        //旧版
        type = EM_MAGTEK;
    }
    memset(&gMagInfo,0,sizeof(gMagInfo));
    for ( i=0 ; i<DIM(gMagFops) ; i++ ) {
        if ( gMagFops[i].chiptype == type ) {
            gMagInfo.magfops = (MAG_FOPS_t *)&gMagFops[i];
            gMagInfo.magtype = gMagFops[i].chiptype;
//            DISPPOS(type);
            break;
        }
    }
    gMagInfo.magfops->init(0);
	return 0;
}

void mag_open(void)
{
//    DISPPOS(gMagInfo.magfops);
    if ( gMagInfo.magfops ) {
        gMagInfo.magfops->open(0);
        gMagInfo.magstat = 1;
    }
}
void mag_close(void)
{
//    DISPPOS(gMagInfo.magfops);
    if ( gMagInfo.magfops ) {
        gMagInfo.magfops->close(0);
        gMagInfo.magstat = 0;
    }
}

uchar mag_get(void)
{
    if(gMagInfo.magtype == EM_MAGTEK)
    {
        return 0;
    }
    else if(gMagInfo.magtype == EM_IDT178)
    {
        return 1;
    }
    else if(gMagInfo.magtype == EM_MTK211)
    {
        return 2;
    }
    else {
        return 3;         
    }
}

void mag_set(uchar type)
{
//    uint i;
//    type = EM_MAGTEK;
//    memset(&gMagInfo,0,sizeof(gMagInfo));
//    for ( i=0 ; i<DIM(gMagFops) ; i++ ) {
//        if ( gMagFops[i].chiptype == type ) {
//            gMagInfo.magfops = (MAG_FOPS_t *)&gMagFops[i];
//            break;
//        }
//    }
}


int mag_read(void *Track1, void *Track2, void *Track3)
{
    if(gMagInfo.magstat) {
        return(gMagInfo.magfops->read(Track1, Track2, Track3));
    }
    return 0x70;//无刷卡数据
}		/* -----  end of function magcard_main  ----- */

int magcard_main(int mode)
{
    if(gMagInfo.magstat) {
        gMagInfo.magfops->main(0);
    }
    return 0;
}		/* -----  end of function magcard_main  ----- */


#endif
