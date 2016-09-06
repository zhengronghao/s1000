/*
 * =====================================================================================
 *
 *       Filename:  Magtek.c
 *
 *    Description:  非加密磁头 magteck（量产版本） 
 *
 *        Version:  1.0
 *        Created:  5/9/2014 11:40:28 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yehf (), yehf@itep.com.cn
 *        Company:  START
 *
 * =====================================================================================
 */
#include "wp30_ctrl.h"
#if (defined CFG_MAGCARD)

#if 11//MAG_PRODUCT == EM_MAGTEK 
#include "bitbuf.h"
/*-----------------------------------------------------------------------------}
 *  全局变量
 *-----------------------------------------------------------------------------{*/
uint gMagTeckTimeout;
uchar gMagTeckStat=0;  
uchar gMagAsynStat=0;  

#define  DELAY_250ns_TIMES  150   // 100: 255ns
/*-----------------------------------------------------------------------------}
 *  函数定义
 *-----------------------------------------------------------------------------{*/
void magtek_enable_VCC(void)
{
    hw_mag_power_write(0);
}

void magtek_disable_VCC(void)
{	
    hw_mag_power_write(1);
}

void magtek_data(int mode)
{
	if(mode == ON)
	{
        hw_mag_data_write(1);
	}
	else
	{
        hw_mag_data_write(0);
	}
}

uchar magtek_readdata(void)
{
    return hw_mag_data_read();
}


void magtek_strobe(int mode)
{
	if(mode == ON)
	{
        hw_mag_strobe_write(1);
	}
	else
	{
        hw_mag_strobe_write(0);
	}
}


void magtek_set_datamode(int mode)
{
	if(mode == IO_IN)
	{
        hw_mag_gpio_input(MAG_DATA);
	}
	else
	{
        hw_mag_gpio_output(MAG_DATA);
	}
}

void magtek_set_strobemode(int mode)
{
	if(mode == IO_IN)
	{
        hw_mag_gpio_input(MAG_STROBE);
	}
	else
	{
        hw_mag_gpio_output(MAG_STROBE);
	}
}

int magtek_ready(int step)
{
	int     bResult = 0;
	int     i;
	volatile uint j;
//GPIO_SetBits(GPIOC, GPIO_Pin_1);
	if(step == 0)
	{
        magtek_strobe(ON);
        // 至少1.1us
        s_DelayUs(3);
		if(magtek_readdata() == ON)
		{
			magtek_strobe(OFF);                 // CLR FLAG
			bResult = 1;
		}
	}
	else
	{
        magtek_strobe(ON);
        for(j = 0; j < DELAY_250ns_TIMES; j++); 
		if(magtek_readdata() == ON)
		{
			for(i=0; i<16; i++)               // Initialize
			{
				magtek_strobe(OFF);
                s_DelayUs(2);
				magtek_strobe(ON);
                s_DelayUs(2);
			}
			bResult = 1;
		}
	}
	return bResult;
}

void magtek_update(MSR_ByteInfo *track1, MSR_ByteInfo *track2, MSR_ByteInfo *track3)
{
	int b = 0;
	track1->BitOneNum = 0;
	track1->byte = 3;
	for(b=0; b<704; b++)
	{
		magtek_strobe(OFF);
		delay_us(10);
		if(magtek_readdata() == OFF)
		{
			track1->BitOneNum++;
			if(b % 8 == 0 && b != 0)
			{
				track1->byte++;
				track1->buf[track1->byte]=0x01;
			}
			else
			{
				track1->buf[track1->byte] |= 1<< (b%8);
			}
		}
		else
		{
			if(b % 8 == 0 && b != 0)
			{
				track1->byte++;
				track1->buf[track1->byte]=0x00;
			}
		}
		magtek_strobe(ON);
		delay_us(10);
	}
	DISPBUF("track1", track1->byte, 0, track1->buf);

	track2->BitOneNum = 0;
	track2->byte = 3;
	for(b=0; b<704; b++)
	{
		magtek_strobe(OFF);
		delay_us(20);
		if(magtek_readdata() == OFF)
		{
			track2->BitOneNum++;
			if(b % 8 == 0 && b != 0)
			{
				track2->byte++;
				track2->buf[track2->byte]=0x01;
			}
			else
			{
				track2->buf[track2->byte] |= 1<< (b%8);
			}
		}
		else
		{
			if(b % 8 == 0 && b != 0)
			{
				track2->byte++;
				track2->buf[track2->byte]=0x00;
			}
		}
		magtek_strobe(ON);
		delay_us(20);
	}
//	DISPBUF("track2", track2->byte, 0, track2->buf);
//    for(b=0; b<16; b++)
//    {
//        magtek_strobe(OFF);
//        delay_us(20);
//        magtek_strobe(ON);
//        delay_us(20);
//	}

	track3->BitOneNum = 0;
	track3->byte = 3;
	for(b=0; b<704; b++)
	{
		magtek_strobe(OFF);
		delay_us(20);
		if(magtek_readdata() == OFF)
		{
			track3->BitOneNum++;
			if(b % 8 == 0 && b != 0)
			{
				track3->byte++;
				track3->buf[track3->byte]=0x01;
			}
			else
			{
				track3->buf[track3->byte] |= 1<< (b%8);
			}
		}
		else
		{
			if(b % 8 == 0 && b != 0)
			{
				track3->byte++;
				track3->buf[track3->byte]=0x00;
			}
		}
		magtek_strobe(ON);
		delay_us(20);
	}
   
//	DISPBUF("track1", track1->byte, 0, track1->buf);
	DISPBUF("track2", track2->byte, 0, track2->buf);
	DISPBUF("track3", track3->byte, 0, track3->buf);
}


void magtek_reset(void)
{
	volatile uint i;  //100次大概20us  for(x = 0; x < 100; x++);
	magtek_set_datamode(IO_OUT);
	magtek_set_datamode(IO_OUT);
	magtek_set_strobemode(IO_OUT);

	magtek_data(ON);  //约292ns
	magtek_strobe(ON);
	magtek_data(OFF);
	//s_DelayUs(1);  //banyan == 5
	for(i = 0; i < DELAY_250ns_TIMES; i++);
	magtek_strobe(OFF);
	magtek_strobe(ON);
	magtek_strobe(OFF);

	magtek_data(ON);
	magtek_strobe(ON);
	magtek_strobe(OFF);
	magtek_strobe(ON);
	magtek_strobe(OFF);
    gMagTeckTimeout = 0;
    gMagTeckStat = 0;
    CLRBIT(gMagInfo.mode,1);
	magtek_set_datamode(IO_IN);
}


int magtek_check(void)
{
	if(magtek_readdata()==0x00)
	{
		return 0;
	}
	else
	{
		return 1;
	}	
}

int mag_magtek_init(int mode)
{
    hw_mag_gpio_output(MAG_POWER);
    hw_mag_gpio_output(MAG_STROBE);
    hw_mag_gpio_output(MAG_DATA);
	magtek_disable_VCC();
	magtek_data(OFF);
	magtek_strobe(OFF);
    return 0;
}
int mag_magtek_close(int mode)
{
    mag_magtek_init(0);
    gMagAsynStat = 0;
    return 0;
}

int mag_magtek_open(int mode)
{
	mag_magtek_close(0);
	magtek_enable_VCC();
//	delay_ms(10);
    s_DelayUs(5000);
	magtek_reset();
	magtek_set_strobemode(IO_OUT);
	magtek_set_datamode(IO_IN);
    gMagAsynStat = 1;
	return 0;
}
int mag_magtek_ioctl(int mode,int value)
{
    if ( mode == 0 ) {
        //check
        return (int)magtek_check();
    }else if ( mode == 1 ) {
        //clear
        magtek_reset();
    }
    return 0;
}

extern void ctc_mag_report(uint8_t *data);
int mag_magtek_read(void *Track1, void *Track2, void *Track3)
{
    uint oldtime; 
	uchar ret = 0, swiped = NO;
	int k_MSR_swipe_flag = 0;
	ET_MAGCARD *ptTrk1, *ptTrk2, *ptTrk3;	
	MSR_ByteInfo *k_MSR_buf1, *k_MSR_buf2, *k_MSR_buf3;
	BitBuf SrcBitBuf;	
    int magret = 0x00;
    int maglen = 0;
    uchar *ptr;
    oldtime = sys_get_counter();
    while ( 1 ) {
        if(magtek_readdata()==0x00)
        {
            if(IFBIT(gMagInfo.mode,1) == 0)
            {
                ret = magtek_ready(0);
                if(ret)
                {
                    SETBIT(gMagInfo.mode,1);
                    oldtime = sys_get_counter();
                }
                else
                {
                    magtek_reset();
                    magtek_set_datamode(IO_IN);
                    break;
                }
            }
            else
            {
                swiped = NO;
                ret = magtek_ready(1);
                if(ret)
                {
                    k_MSR_buf1 = (MSR_ByteInfo *)&gucBuff[512]; 
                    k_MSR_buf2 = (MSR_ByteInfo *)&gucBuff[512+sizeof(MSR_ByteInfo)*1]; 
                    k_MSR_buf3 = (MSR_ByteInfo *)&gucBuff[512+sizeof(MSR_ByteInfo)*2];
                    memset((char *)k_MSR_buf1, 0, sizeof (MSR_ByteInfo));
                    memset((char *)k_MSR_buf2, 0, sizeof (MSR_ByteInfo));
                    memset((char *)k_MSR_buf3, 0, sizeof (MSR_ByteInfo));
                    ptTrk1 = (ET_MAGCARD *)pMagBufTmp1;
                    ptTrk2 = (ET_MAGCARD *)(pMagBufTmp1+sizeof(ET_MAGCARD)*1);
                    ptTrk3 = (ET_MAGCARD *)(pMagBufTmp1+sizeof(ET_MAGCARD)*2);	
                    memset(ptTrk1,0,sizeof (ET_MAGCARD));
                    memset(ptTrk2,0,sizeof (ET_MAGCARD));
                    memset(ptTrk3,0,sizeof (ET_MAGCARD));
                    magtek_update(k_MSR_buf1, k_MSR_buf2, k_MSR_buf3);
                    if(k_MSR_buf1->BitOneNum > 8)
                    {
                        swiped = YES;
                    }
                    else if(k_MSR_buf2->BitOneNum > 8)
                    {
                        swiped = YES;
                    }
                    else if(k_MSR_buf3->BitOneNum > 8)
                    {
                        swiped = YES;
                    }
                }
                magtek_reset();
                CLRBIT(gMagInfo.mode,1);
                magtek_set_datamode(IO_IN);
                CLR_IO(DEBUG_PIN0);
                if(swiped == YES)
                {
                    k_MSR_swipe_flag = ON;
                    break;
                }
            }
        }
        // 解决死机问题
        if(IFBIT(gMagInfo.mode,1) == 0){
//            if ( sys_get_counter() - oldtime > 40 ) {
//                ret = 1;
//                break;
//            }
            ret = 1;
            break;
        } else {
            if ( sys_get_counter() - oldtime > 2000 ) {
                ret = 1;
                break;
            }
            // 刷卡时响应按键
            if ( KB_Hit() ) {
//                pinpad_getkey(0,50);
            }
        }
    }
	if(k_MSR_swipe_flag == ON)
	{
		// Trk1 decode
		ret = EM_mag_NULL;
		if(k_MSR_buf1->byte)
		{
			bitbufInit(&SrcBitBuf, k_MSR_buf1->buf, 704);
			ret = SI_mag_ucDecodeTrk1(&SrcBitBuf, k_MSR_buf1->byte*8, &ptTrk1->ucTrkLength, ptTrk1->aucTrkBuf, 1);	// forward
			TRACE("Trk1 forward:%X", ret);
			if (ret != EM_mag_SUCCESS)
			{
				ret = SI_mag_ucDecodeTrk1(&SrcBitBuf, k_MSR_buf1->byte*8, &ptTrk1->ucTrkLength, ptTrk1->aucTrkBuf, 0);	// backward
				TRACE("Trk1 backward:%X", ret);
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
            if ( Track1 != NULL ) {
                ptr = (uchar *)Track1;
                memcpy(ptr, ptTrk1->aucTrkBuf, ptTrk1->ucTrkLength);
            }
        }

		// Trk2 decode
		ret = EM_mag_NULL;
		if(k_MSR_buf2->byte)
		{
			bitbufInit(&SrcBitBuf, k_MSR_buf2->buf, 704);
			ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf2->byte*8, &ptTrk2->ucTrkLength, ptTrk2->aucTrkBuf, 1);	// forward
//			TRACE("Trk2 forward:%X", ret);
			if (ret != EM_mag_SUCCESS)
			{
				ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf2->byte*8, &ptTrk2->ucTrkLength, ptTrk2->aucTrkBuf, 0);	// backward
//				TRACE("Trk2 backward:%X", ret);
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
            if ( Track2 != NULL ) {
                ptr = (uchar *)Track2;
                memcpy(ptr, ptTrk2->aucTrkBuf, ptTrk2->ucTrkLength);
            }
        }

		// Trk3 decode
		ret = EM_mag_NULL;
		if(k_MSR_buf3->byte)
		{
			bitbufInit(&SrcBitBuf, k_MSR_buf3->buf, 704);
			ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf3->byte*8, &ptTrk3->ucTrkLength, ptTrk3->aucTrkBuf, 1);	// forward
//			TRACE("Trk3 forward:%X", ret);
			if (ret != EM_mag_SUCCESS)
			{
				ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf3->byte*8, &ptTrk3->ucTrkLength, ptTrk3->aucTrkBuf, 0);	// backward
//				TRACE("Trk3 backward:%X", ret);
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
            if ( Track3 != NULL ) {
                ptr = (uchar *)Track3;
                memcpy(ptr, ptTrk3->aucTrkBuf, ptTrk3->ucTrkLength);
            }
        }
	}
	else
	{
        magret = 0x70;
		goto app_read_magcard_end;
	}
//    if (ptTrk2->ucTrkFlag == EM_mag_SUCCESS)
//    {
//        trk2_n++;
//    }
//    if (ptTrk2->ucTrkFlag == EM_mag_SUCCESS)
//    {
//        trk3_n++;
//    }
//    trk_n++;
//    lcd_display(0,16,FONT_SIZE12|DISP_MEDIACY,"trk2 succe:%d err:%d",trk2_n,trk_n-trk2_n);
//    lcd_display(0,32,FONT_SIZE12|DISP_MEDIACY,"trk3 succe:%d err:%d",trk3_n,trk_n-trk3_n);
//    lcd_display(0,48,FONT_SIZE12|DISP_MEDIACY,"trk2:%02d%% trk3:%02d%%",100*trk2_n/trk_n,100*trk3_n/trk_n);
//	DISPBUF("trk1",ptTrk1->ucTrkLength,0,(uchar *)ptTrk1+2);TRACE("\r\n [trk1:%s]",(uchar *)ptTrk1+2);
//	DISPBUF("trk2",ptTrk2->ucTrkLength,0,(uchar *)ptTrk2+2);TRACE("\r\n [trk2:%s]",(uchar *)ptTrk2+2);
//	DISPBUF("trk3",ptTrk3->ucTrkLength,0,(uchar *)ptTrk3+2);TRACE("\r\n [trk3:%s]",(uchar *)ptTrk3+2);
    magtek_reset();
    ctc_mag_report((uchar *)ptTrk1); 
//	PPRT_Send(CMD_READ_MAGCARD,(uchar *)ptTrk1,sizeof(ET_MAGCARD)*3);
//	return magret;
app_read_magcard_end:
    magtek_reset();
//	PPRT_Send(CMD_READ_MAGCARD,&ret,1);
//    trk_n++;
//    lcd_display(0,16,FONT_SIZE12|DISP_MEDIACY,"trk2 succe:%d err:%d",trk2_n,trk_n-trk2_n);
//    lcd_display(0,32,FONT_SIZE12|DISP_MEDIACY,"trk3 succe:%d err:%d",trk3_n,trk_n-trk3_n);
//    lcd_display(0,48,FONT_SIZE12|DISP_MEDIACY,"trk2:%02d%% trk3:%02d%%",100*trk2_n/trk_n,100*trk3_n/trk_n);
	return magret;
}
/* 
 * magteck_main - [GENERIC] 
 * @ 
 */
#if 0
int mag_magtek_main (int mode)
{
	//int len;
    int ret;
	uchar swiped = NO;
	ET_MAGCARD *ptTrk1, *ptTrk2, *ptTrk3;	
	MSR_ByteInfo *k_MSR_buf1, *k_MSR_buf2, *k_MSR_buf3;
	BitBuf SrcBitBuf;	
    if ( IFBIT(gMagInfo.mode,1) == 0) {
        SET_IO(DEBUG_PIN0);
        if ( magtek_check() == EM_SUCCESS && magtek_check() == EM_SUCCESS) {
            SET_IO(DEBUG_PIN1);
            ret = magtek_ready(0);
            if(ret) {
                gMagTeckTimeout = sys_get_counter();
                SETBIT(gMagInfo.mode,1);
            } else {
                magtek_reset();
            }
            CLR_IO(DEBUG_PIN1);
        }
        CLR_IO(DEBUG_PIN0);
    } else if ( IFBIT(gMagInfo.mode,1) == 1){
        if ( magtek_check() == EM_SUCCESS ) {
            SET_IO(DEBUG_PIN2);
            ret = magtek_ready(1);
            if(ret) {
//                exit_lowerpower_freq();
                k_MSR_buf1 = (MSR_ByteInfo *)&gucBuff[512]; 
                k_MSR_buf2 = (MSR_ByteInfo *)&gucBuff[512+sizeof(MSR_ByteInfo)*1]; 
                k_MSR_buf3 = (MSR_ByteInfo *)&gucBuff[512+sizeof(MSR_ByteInfo)*2];
                memset((char *)k_MSR_buf1, 0, sizeof (MSR_ByteInfo));
                memset((char *)k_MSR_buf2, 0, sizeof (MSR_ByteInfo));
                memset((char *)k_MSR_buf3, 0, sizeof (MSR_ByteInfo));
                ptTrk1 = (ET_MAGCARD *)pMagBufTmp1;
                ptTrk2 = (ET_MAGCARD *)(pMagBufTmp1+sizeof(ET_MAGCARD)*1);
                ptTrk3 = (ET_MAGCARD *)(pMagBufTmp1+sizeof(ET_MAGCARD)*2);	
                memset(ptTrk1,0,sizeof (ET_MAGCARD));
                memset(ptTrk2,0,sizeof (ET_MAGCARD));
                memset(ptTrk3,0,sizeof (ET_MAGCARD));
                magtek_update(k_MSR_buf1, k_MSR_buf2, k_MSR_buf3);
                swiped = NO;
                if(k_MSR_buf1->BitOneNum > 8)
                {
                    swiped = YES;
                }
                else if(k_MSR_buf2->BitOneNum > 8)
                {
                    swiped = YES;
                }
                else if(k_MSR_buf3->BitOneNum > 8)
                {
                    swiped = YES;
                }
                if ( swiped == YES ) {
                    // Trk1 decode
                    ret = EM_mag_NULL;
                    if(k_MSR_buf1->byte)
                    {
                        bitbufInit(&SrcBitBuf, k_MSR_buf1->buf, 704);
                        ret = SI_mag_ucDecodeTrk1(&SrcBitBuf, k_MSR_buf1->byte*8, &ptTrk1->ucTrkLength, ptTrk1->aucTrkBuf, 1);	// forward
                        TRACE("Trk1 forward:%X", ret);
                        if (ret != EM_mag_SUCCESS)
                        {
                            ret = SI_mag_ucDecodeTrk1(&SrcBitBuf, k_MSR_buf1->byte*8, &ptTrk1->ucTrkLength, ptTrk1->aucTrkBuf, 0);	// backward
                            TRACE("Trk1 backward:%X", ret);
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
                    }	

                    // Trk2 decode
                    ret = EM_mag_NULL;
                    if(k_MSR_buf2->byte)
                    {
                        bitbufInit(&SrcBitBuf, k_MSR_buf2->buf, 704);
                        ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf2->byte*8, &ptTrk2->ucTrkLength, ptTrk2->aucTrkBuf, 1);	// forward
                        TRACE("Trk2 forward:%X", ret);
                        if (ret != EM_mag_SUCCESS)
                        {
                            ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf2->byte*8, &ptTrk2->ucTrkLength, ptTrk2->aucTrkBuf, 0);	// backward
                            TRACE("Trk2 backward:%X", ret);
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
                    }

                    // Trk3 decode
                    ret = EM_mag_NULL;
                    if(k_MSR_buf3->byte)
                    {
                        bitbufInit(&SrcBitBuf, k_MSR_buf3->buf, 704);
                        ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf3->byte*8, &ptTrk3->ucTrkLength, ptTrk3->aucTrkBuf, 1);	// forward
                        TRACE("Trk3 forward:%X", ret);
                        if (ret != EM_mag_SUCCESS)
                        {
                            ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf3->byte*8, &ptTrk3->ucTrkLength, ptTrk3->aucTrkBuf, 0);	// backward
                            TRACE("Trk3 backward:%X", ret);
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
                    }
//                    enter_lowerpower_freq();
//                    PPRT_Send(CMD_READ_MAGCARD,(uchar *)ptTrk1,sizeof(ET_MAGCARD)*3);
//                    TRACE_BUF("TRK1",(uint8_t *)ptTrk1,sizeof(ET_MAGCARD));
//                    TRACE_BUF("TRK2",(uint8_t *)ptTrk2,sizeof(ET_MAGCARD));
//                    TRACE_BUF("TRK3",(uint8_t *)ptTrk3,sizeof(ET_MAGCARD));
                } else {
//                    enter_lowerpower_freq();
//                    PPRT_Send(CMD_READ_MAGCARD,(uchar *)&ret,1);
                } 
                magtek_reset();
                CLR_IO(DEBUG_PIN2);
            }
        } else if (sys_get_counter() - gMagTeckTimeout > 2000){
            SET_IO(DEBUG_PIN2);
            magtek_reset();
            CLR_IO(DEBUG_PIN2);
        }
    }
    return 0;
}		/* -----  end of function magteck_main  ----- */
#endif
int mag_magtek_main (int mode)
{
	//int len;
    int ret;
	uchar swiped = NO;
	ET_MAGCARD *ptTrk1, *ptTrk2, *ptTrk3;	
	MSR_ByteInfo *k_MSR_buf1, *k_MSR_buf2, *k_MSR_buf3;
	BitBuf SrcBitBuf;	
//    DISPPOS(gMagTeckStat);
    if ( gMagTeckStat == 0 && gMagAsynStat ==1) {
        SET_IO(DEBUG_PIN0);
        if ( magtek_check() == EM_SUCCESS && magtek_check() == EM_SUCCESS) {
            SET_IO(DEBUG_PIN1);
            ret = magtek_ready(0);
            if(ret) {
                gMagTeckTimeout = sys_get_counter();
                gMagTeckStat = 1;
            } else {
                magtek_reset();
            }
            CLR_IO(DEBUG_PIN1);
        }
        CLR_IO(DEBUG_PIN0);
    } else if ( gMagTeckStat == 1){
        if ( magtek_check() == EM_SUCCESS ) {
            SET_IO(DEBUG_PIN2);
            ret = magtek_ready(1);
            if(ret) {
               // exit_lowerpower_freq();
                k_MSR_buf1 = (MSR_ByteInfo *)&gucBuff[512]; 
                k_MSR_buf2 = (MSR_ByteInfo *)&gucBuff[512+sizeof(MSR_ByteInfo)*1]; 
                k_MSR_buf3 = (MSR_ByteInfo *)&gucBuff[512+sizeof(MSR_ByteInfo)*2];
                memset((char *)k_MSR_buf1, 0, sizeof (MSR_ByteInfo));
                memset((char *)k_MSR_buf2, 0, sizeof (MSR_ByteInfo));
                memset((char *)k_MSR_buf3, 0, sizeof (MSR_ByteInfo));
                ptTrk1 = (ET_MAGCARD *)pMagBufTmp1;
                ptTrk2 = (ET_MAGCARD *)(pMagBufTmp1+sizeof(ET_MAGCARD)*1);
                ptTrk3 = (ET_MAGCARD *)(pMagBufTmp1+sizeof(ET_MAGCARD)*2);	
                memset(ptTrk1,0,sizeof (ET_MAGCARD));
                memset(ptTrk2,0,sizeof (ET_MAGCARD));
                memset(ptTrk3,0,sizeof (ET_MAGCARD));
                magtek_update(k_MSR_buf1, k_MSR_buf2, k_MSR_buf3);
                swiped = NO;
                if(k_MSR_buf1->BitOneNum > 8)
                {
                    swiped = YES;
                }
                else if(k_MSR_buf2->BitOneNum > 8)
                {
                    swiped = YES;
                }
                else if(k_MSR_buf3->BitOneNum > 8)
                {
                    swiped = YES;
                }
                if ( swiped == YES ) {
                    // Trk1 decode
                    ret = EM_mag_NULL;
                    if(k_MSR_buf1->byte)
                    {
                        bitbufInit(&SrcBitBuf, k_MSR_buf1->buf, 704);
                        ret = SI_mag_ucDecodeTrk1(&SrcBitBuf, k_MSR_buf1->byte*8, &ptTrk1->ucTrkLength, ptTrk1->aucTrkBuf, 1);	// forward
                        TRACE("Trk1 forward:%X", ret);
                        if (ret != EM_mag_SUCCESS)
                        {
                            ret = SI_mag_ucDecodeTrk1(&SrcBitBuf, k_MSR_buf1->byte*8, &ptTrk1->ucTrkLength, ptTrk1->aucTrkBuf, 0);	// backward
                            TRACE("Trk1 backward:%X", ret);
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
                    }	

                    // Trk2 decode
                    ret = EM_mag_NULL;
                    if(k_MSR_buf2->byte)
                    {
                        bitbufInit(&SrcBitBuf, k_MSR_buf2->buf, 704);
                        ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf2->byte*8, &ptTrk2->ucTrkLength, ptTrk2->aucTrkBuf, 1);	// forward
                        TRACE("Trk2 forward:%X", ret);
                        if (ret != EM_mag_SUCCESS)
                        {
                            ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf2->byte*8, &ptTrk2->ucTrkLength, ptTrk2->aucTrkBuf, 0);	// backward
                            TRACE("Trk2 backward:%X", ret);
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
                    }

                    // Trk3 decode
                    ret = EM_mag_NULL;
                    if(k_MSR_buf3->byte)
                    {
                        bitbufInit(&SrcBitBuf, k_MSR_buf3->buf, 704);
                        ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf3->byte*8, &ptTrk3->ucTrkLength, ptTrk3->aucTrkBuf, 1);	// forward
                        TRACE("Trk3 forward:%X", ret);
                        if (ret != EM_mag_SUCCESS)
                        {
                            ret = EI_mag_vProcTrk23(&SrcBitBuf, k_MSR_buf3->byte*8, &ptTrk3->ucTrkLength, ptTrk3->aucTrkBuf, 0);	// backward
                            TRACE("Trk3 backward:%X", ret);
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
                    }
                //    enter_lowerpower_freq();
//                    PPRT_Send(CMD_READ_MAGCARD,(uchar *)ptTrk1,sizeof(ET_MAGCARD)*3);
                    ctc_mag_report((uchar *)ptTrk1); 
                } else {
//                    enter_lowerpower_freq();
//                    PPRT_Send(CMD_READ_MAGCARD,(uchar *)&ret,1);
                } 
                magtek_reset();
                mag_magtek_close(0);
                // 异步上报上报数据完关闭检测，等待下次open
                gMagAsynStat = 0;
                CLR_IO(DEBUG_PIN2);
            } 
        } else if (sys_get_counter() - gMagTeckTimeout > 2000){
            SET_IO(DEBUG_PIN2);
            magtek_reset();
            CLR_IO(DEBUG_PIN2);
        }
    }
    return 0;
}		/* -----  end of function magteck_main  ----- */
#endif
#endif
