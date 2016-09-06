/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : debug.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/22/2014 4:41:46 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "drv_inc.h"

const char ASSERT_FAILED_STR[] = "Assertion failed in %s at line %d\n";

/********************************************************************/
void assert_failed(char *file, int line)
{
    int i;
    
    TRACE(ASSERT_FAILED_STR, file, line);

    while (1)
    {
        for (i = 100000; i; i--) ;
    }
}


void vDispBuf(uint32_t length, uint32_t mode, const uchar *const buffer)
{
#ifdef DEBUG_Dx 
	uint i;
	TRACE("\r\n--------%d---------",length);
	for(i=0; i<length; i++)
	{
		if(i%48==0) {
			TRACE("\r\n%04X:",i);
		} else if(i%8==0) {
			TRACE(" ");
		} 

        if(mode == 1) {
			TRACE(" %02d",buffer[i]);
		} else if(mode == 2) {
			TRACE(" %c",buffer[i]);
		} else {
			TRACE(" %02X",buffer[i]);
		}
	}
//	TRACE("\r\n");
#endif
}

void TRACE_BUF(const char *title,const uint8_t *const buffer,uint32_t length)
{
#ifdef DEBUG_Dx 
    uint32_t i;

    if (title != NULL)
        TRACE("\n-|%s %d",title,length);
    else 
        TRACE("\n-|%d ",length);
	for(i=0; i<length; i++)
	{
		if(i%32==0) {
			TRACE("\r\n%04d:",i/32);
		} else if(i%8==0) {
			TRACE(" ");
		} 
        TRACE(" %02X",buffer[i]);
	}
#endif
}


uint32_t IfInkey(uint32_t mode)
{
    return console_check_buffer();
}

int InkeyCount(int mode)
{
    uint8_t ucBuff[32],ucData;
    int j=0;

    memset(ucBuff,0x00,sizeof(ucBuff));
    if(!mode)
    {
        console_clear();
        TRACE("[IN]:");
        while(1)
        {
            if(console_read_buffer(&ucData,1,-1) > 0)
            {
                if(ucData == 0x0D) {
                    ucData = 0x0A;
                    out_char(ucData);
                    break;
                } else if(ucData <= '9' && ucData >= '0') {
                    ucBuff[j++] = ucData;
                    out_char(ucData);
                } else if(ucData == 0x1b) {
                    return 0;
                } else if(ucData == 0x08) {
                    if(j) {
                        ucBuff[j--] = 0;
                    }
                    ucData = '\\';
                    out_char(ucData);
                }
            }
        }
    }
    return (int)atoi((char const *)ucBuff);
}

// '1'~1 'a'~a 'A'~A
char Ascii_To_NumAlp(char ch)
{
    if (ch <= '9' && ch >= '0') {
        return (ch - '0') & 0x0f;
    } else if (ch <= 'F' && ch >= 'A') {
        return (ch - 'A' + 0x0A) & 0x0f;
    } else if (ch <= 'f' && ch >= 'a') {
        return (ch - 'a' + 0x0A) & 0x0f;
    }
    return 0;
}

int InkeyStrToHex(char * outbuf)
{
    char ucBuff[128],ucData;
    int i=0, j=0;

    if ( outbuf == NULL ) {
        return 0;
    }
    memset(ucBuff,0x00,sizeof(ucBuff));

    console_clear();
    TRACE("\r\n[IN]:");
    while(1)
    {
        if(console_read_buffer((uchar *)&ucData,1,-1) > 0)
        {
            if(ucData == 0x0D) {
                ucData = 0x0A;
                out_char(ucData);
                break;
            } else if((ucData <= '9' && ucData >= '0')||(ucData <= 'f' && ucData >= 'a') 
                      ||(ucData <= 'F' && ucData >= 'A')) {
                ucBuff[j++] = ucData;
                out_char(ucData);
            } else if(ucData == 0x1b) {
                return 0;
            } else if(ucData == 0x08) {
                if(j) {
                    ucBuff[j--] = 0;
                }
                ucData = '\\';
                out_char(ucData);
            }
        }
    }
    if ( j ) {
        for ( i=0 ; i<(j>>1) ; i++) {
            outbuf[i]= (Ascii_To_NumAlp(ucBuff[i*2])<<4) | (Ascii_To_NumAlp(ucBuff[i*2+1])) ; 
        }
    }

    return i; 
}

int InkeyStr(char * outbuf)
{
    char ucBuff[128],ucData;
    int j=0;

    if ( outbuf == NULL ) {
        return 0;
    }
    memset(ucBuff,0x00,sizeof(ucBuff));

    console_clear();
    TRACE("\r\n[IN]:");
    while(1)
    {
        if(console_read_buffer((uchar *)&ucData,1,-1) > 0)
        {
            if(ucData == 0x0D) {
                ucData = 0x0A;
                out_char(ucData);
                break;
            }else if(ucData == 0x1b) {
                return 0;
            }else if(ucData == 0x08) {
                if(j) {
                    ucBuff[j--] = 0;
                }
                ucData = '\\';
                out_char(ucData);
            }else{
                ucBuff[j++] = ucData;
                out_char(ucData);
            }
        }
    }
    memcpy(outbuf, ucBuff, j);    
    return j; 
}


uint32_t InkeyHex(uint32_t Flag)
{
    uint8_t ucBuff[11],ucData;     
    int i,j=0,k,sum;

    memset(ucBuff,0x00,sizeof(ucBuff));
    TRACE("[IN]:");
    if(!Flag)
    {
        console_clear();
        while(1)
        {		
            if (console_read_buffer(&ucData,1,20) == 1)
            {
                if (ucData == 0x0D) {
                    out_char(ucData);
                    break;
                } else if (ucData <= '9' && ucData >= '0') {
                    ucBuff[j++] = ucData - '0';
                    out_char(ucData);
                } else if (ucData <= 'F' && ucData >= 'A') {
                    ucBuff[j++] = ucData - 'A' + 0x0A;
                    out_char(ucData);
                } else if (ucData <= 'f' && ucData >= 'a') {
                    ucBuff[j++] = ucData - 'a' + 0x0A;
                    out_char(ucData);
                } else if (ucData == 0x1b) {	
                    return 0;
                } else if (ucData == 0x08) { // backspace
                    if(j) {
                        ucBuff[j--] = 0;
                    }
                    ucData = '\\';
                    out_char(ucData);
                } 
                if (j >= sizeof(uint32_t)*2) {
                    break;
                }
            }			
        }
    }
    sum = 0;
    for(i=j-1,k=0; i>=0; i--,k++) {
        sum += (ucBuff[i] *(1<<(k*4)));
    }
    return (uint32_t)sum;
}

// mode:   D0~D7:  0~3 显示类型 0-16进制 1-10进制 2-字符
//         D8~D15：0-char  1-short 2-int
void vDispBufTitle(char *title, uint uiLen, uint Mode, void *p)
{
#ifdef DEBUG_Dx 
	uint i;
    uint disptype,dispmode;
    uchar *ucBuf = (uchar *)p;
    uint *uiBuf = (uint *)p;
    ushort *usBuf = (ushort *)p;
    dispmode = (Mode>>8);
    disptype = (Mode&0xFF);
	if(title)
	{
		TRACE("\r\n-----%s:%d------",title,uiLen/(1<<dispmode));
	}
	else
	{
		TRACE("\r\n-----%d------",uiLen/(1<<dispmode));
	}
    if ( dispmode == 0 ) {
        for(i=0;i<uiLen;i++)
        {
            if(i%32==0)
            {
                //TRACE("\r\n%04X:",i);
                TRACE("\r\n");
            }
            else if(i%8==0)
            {
                TRACE(" ");
            }
            if(disptype == 1)
            {
                TRACE("%02d ",ucBuf[i]);
            }
            else if(disptype == 2)
            {
                TRACE("%c ",ucBuf[i]);
            }
            else
            {
                TRACE("%02X ",ucBuf[i]);
            }
        }
    } else if (dispmode == 1){
        for(i=0;i<uiLen/2;i++)
        {
            if(i%16==0)
            {
                //TRACE("\r\n%04X:",i);
                TRACE("\r\n");
            }
            else if(i%8==0)
            {
                //TRACE(" ");
            }
            if(disptype == 1)
            {
                TRACE("%05d ",usBuf[i]);
            }
            else
            {
                TRACE("%04X ",usBuf[i]);
            }
        }
    } else {
        for(i=0;i<uiLen/4;i++)
        {
            if(i%8==0)
            {
                //TRACE("\r\n%04X:",i);
                TRACE("\r\n");
            }
            if(disptype == 1)
            {
                TRACE("%08d ",uiBuf[i]);
            }
            else
            {
                TRACE("%08X ",uiBuf[i]);
            }
        }
    }
#endif
}
void vDispLable(uint mode, uint num)
{
#ifdef DEBUG_Dx 
    TRACE("\r\n");
    while ( num-- ) {
        TRACE("-");
    }
#endif
}
