/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : display.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 11/10/2014 11:16:27 AM
 * Description        : 
 *******************************************************************************/
#include "app_common.h"
#include "display.h"
#include <stdarg.h>

const FONTLABDotInfo gcFontLabDotInfo[] =
{
   //width,height,dotbyte,incodebyte,
	{ 6,    8,     8,     1}, /*0 ASCII 6*8 */
    { 7,    8,     8,     1}, /*1 ASCII 7*8 */ 
    { 6,   12,    12,     1}, /*2 ASCII 6*12 */
    { 8,   16,    16,     1}, /*3 ASCII 8*16 */
    {12,   24,    48,     2}, /*4 Chinese 12*24 */
	{12,   12,    24,     2}, /*5 Chinese 12*12 */
    {16,   16,    32,     2}, /*6 Chinese 16*16 */
    {24,   24,    72,     2}  /*7 Chinese 24*24 */
};

const uchar grmbdotbuf[] =
{
    //符号'￥'点阵
    0x00, 0x00, 0x72, 0x7A, 0x0F, 0x0F, 0x7A, 0x72, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x40, 0xC0, 0xC0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 
};
/**********************************************************************
* 函数名称： 
*     font_get_dot
* 功能描述： 
*     根据内码获取点阵,
* 输入参数： 
*     ptFont:字体
*     pucInputBuf:输入字符串
*     charlen:字符串解析前剩余长度
*     pucDotLen:输出点阵长度
*     out:输出点阵
* 输出参数：
*     optlen:此次字符解析个数
* 返回值： 
*     字符点阵类型
* 历史纪录：
*     修改人		日期		    	版本号      修改记录
***********************************************************************/
uint font_get_dot(const _CHARFONT_ *ptFontIndex, uchar *in, uint charlen, uint *optlen, _DOTINFO_ *ptDotInfo, uchar *out)
{
    uint32_t addr; 
	uint8_t i;
	uint8_t charset;
    uint8_t dotbyte;
    uint8_t dot_encode[72];

	ptDotInfo->DotWidth = 0;
	if ((in[0]  >= 0x20) && (in[0]  <= 0x7E))                    // ASCII码字符
	{
font_get_dot1:
		dotbyte = gcFontLabDotInfo[ptFontIndex->AscChaset].dotbyte;
//        addr = SA_ZK +0x1DBF2 + (in[0] - 0x20) * 12;
        addr = FONT_GET_ADDR_ASCII_6X12(in[0]);
        memcpy(dot_encode,(uint8_t *)addr,dotbyte);
//        TRACE_BUF("ASSII encode dot:",dot_encode,12);
        for(i=0; i<6; i++)
        {
            out[i] = (dot_encode[i]<<1)|(dot_encode[i+6]>>7);
            out[i+6] = (dot_encode[i+6]<<1);
        }
//        TRACE_BUF("ASSII decode dot:",out,12);
		*optlen = 1;
        ptDotInfo->DotWidth = gcFontLabDotInfo[ptFontIndex->AscChaset].width;
        ptDotInfo->DotHeight = gcFontLabDotInfo[ptFontIndex->AscChaset].height;
	}
    else if(charlen >= 2)
	{
        if (((in[0] >= 0xB0) && (in[0] <= 0xF7)) && ((in[1] >= 0xA1) && (in[1] <= 0xFE)))
        {
//            addr = ((in[0]- 0xB0) * 94 + in[1] - 0xA1) * 18;
//            addr += SA_ZK +0x12;
            addr = FONT_GET_ADDR_GB2312_12X12(in[0],in[1]);
            memcpy(dot_encode,(uint8_t *)addr,18);
//            TRACE_BUF("HZ encode dot:",dot_encode,18);
            memcpy(out,dot_encode,12);
            for(i=12; i<18; i++)
            {
                out[i]   = (dot_encode[i]<<4)&0xF0;
                out[i+6] = (dot_encode[i]&0xF0);
            }
//            TRACE_BUF("HZ decode dot:",out,24);
            *optlen = 2;
            ptDotInfo->DotWidth = gcFontLabDotInfo[ptFontIndex->HZChaset].width;
            ptDotInfo->DotHeight = gcFontLabDotInfo[ptFontIndex->HZChaset].height;
        }else if((in[0] == 0xA3) && (in[1] == 0xA4))
        {
            memcpy(out, grmbdotbuf, sizeof(grmbdotbuf));
            *optlen = 2;
            ptDotInfo->DotWidth = gcFontLabDotInfo[ptFontIndex->HZChaset].width;
            ptDotInfo->DotHeight = gcFontLabDotInfo[ptFontIndex->HZChaset].height;
        }
        else
        {
            TRACE("\n-|HZ interior code error:0x%02X%02x",in[0],in[1]);
        }
    } else
    {
        TRACE("\n-|Font bytes error");
    }
	if(ptDotInfo->DotWidth == 0)
	{
		//没有正确解析用空格代替
		in[0] = 0x20;
		goto font_get_dot1;
	}
	return charset;
}

//----------------------------------------------
//b0-b7 -> b7->b0
void axisymmetric_x(uint8_t *input,uint16_t length)
{
	uint16_t i,j;
	uint8_t tmp;

	for (i=0; i<length; i++)
	{
		tmp = input[i];		
		input[i] = 0x00;
		for (j=0x80; j>0x00; j>>=1)
		{	
			input[i] >>= 1;	
			if (tmp & j)
			{
				input[i] |= 0x80; 
			}			
		}
	}
}

uint32_t font_get_str_dotwidth(const _CHARFONT_ *ptFont, uint32_t maxwidth, uint32_t inlen, uchar *in, _DOTINFO_ *ptDotInfo)
{
    uint32_t i=0,width=0,k,asclen,hzlen;
    asclen = gcFontLabDotInfo[ptFont->AscChaset].width;
    hzlen = gcFontLabDotInfo[ptFont->HZChaset].width;
//    TRACE("\n-|byte:%d %d",asclen,hzlen);

    if(ptFont->HZChaset == ASC5X7)
        hzlen <<= 1;  //8点阵中文宽度两倍
    while (i<inlen)
    {
        k = 1;
        if(i+1 < inlen)
        {
            if ( ((in[0] >= 0xA1) && (in[0] <= 0xA9)) 
               &&((in[1] >= 0xA1) && (in[1] <= 0xFE))) {         	 
                k = 2;// 双字节1区
            } else if (((in[0] >= 0xB0) && (in[0] <= 0xF7)) 
                     &&((in[1] >= 0xA1) && (in[1] <= 0xFE))) {
                k = 2; // 双字节2区
            } else if (((in[0] >= 0x81) && (in[0] <= 0xA0))
                     &&((in[1] >= 0x40) && (in[1] <= 0xFE))) {
                k = 2;// 双字节3区
            } else if (((in[0] >= 0xAA) && (in[0] <= 0xFE))
                     &&((in[1] >= 0x40) && (in[1] <= 0xA0))) {
                k = 2;// 双字节4区
            } else if (((in[0] >= 0xA8) && (in[0] <= 0xA9)) 
                     &&(((in[1] >= 0x40) && (in[1] <= 0x7E)) 
                     ||((in[1] >= 0x80) && (in[1] <= 0xA0)))){
                k = 2;// 双字节5区
            } else if ((in[0] == 0xAA) && ((in[1] >= 0xA1) && (in[1] <= 0xD2))){
                k = 2;// 自定义字:用空格填充
            } else if(i+3 < inlen) { // 4字节区
                if ( (in[0] >= 0x81 && in[0] <= 0xFE) && (in[1] >= 0x30 && in[1] <= 0x39) 
                   &&(in[2] >= 0x81 && in[2] <= 0xFE) && (in[3] >= 0x30 && in[3] <= 0x39)) {
                    k = 4;
                }
            }
        }
        if(k == 1) 
        {
            if(width + asclen > maxwidth) {
                break;
            }
            width += asclen;
        } else if(k == 2 || k == 4) 
        {
            if(width + hzlen > maxwidth) {
                break;
            }
            width += hzlen;
        }
        i += k;
    }
    ptDotInfo->DotWidth = width;	
    ptDotInfo->DotHeight = MAX(asclen,hzlen);
//    TRACE(" width:%d",width);
    return i;
}

/**
 * \brief       
 *			  
 *
 * \param:    
 * \param     
 */
void lcd_Display(int x,int y,int mode,const char *format,...)
{
#if CFG_LCD
	va_list varg;
	char sbuffer[176];
	uint8_t buffer[72];
	int i,j,len;
	uint32_t k=1;
	_DOTINFO_ tDotInfo;
	_CHARFONT_ fontlabindex;
	uint8_t vert_byte;//Vertical Matrix:The number of bytes each column
	uint8_t width;
	uint8_t reverse=0,reverse_first=0;
    uint8_t line_flag,tmp;
	int (* lcd_fillmatrix)(uint8_t x, uint8_t y,
						uint8_t length,uint8_t width,
						const uint8_t *input);
	
	memset(buffer,0x00,sizeof(buffer));
	memset(sbuffer,0x00,sizeof(sbuffer));	
	va_start (varg, format);
    i = vsnprintf (sbuffer, sizeof(sbuffer)-1, format, varg);
    va_end (varg);
	if (i<0 || i >= sizeof(sbuffer)-1)
		i = sizeof(sbuffer)-1;
	sbuffer[i] = '\0';
	len = strlen(sbuffer);
	i = mode & 0xFF;
//    TRACE("\n-|MOde:%04X",mode);
    mode &= (uint32_t)(~0xFF);
	
	if (!(i == FONT_SIZE8 
          || i == FONT_SIZE12 
//		  || i == FONT_SIZE16 
//          || i == FONT_SIZE24
          ))
    {
        i = FONT_SIZE12; 		
    }
	switch (i)
	{
		default:
			i = FONT_SIZE12;
		case FONT_SIZE12:	//Vertical Matrix7	
            fontlabindex.AscChaset = ASC6X12;//get index
            fontlabindex.HZChaset = HZ12X12;
            if (mode & DISP_XORCHAR) {
                lcd_fillmatrix = drv_lcd_FillVertMatrix1_xor;
            } else {
                lcd_fillmatrix = drv_lcd_FillVertMatrix1;
            }
			break;
		case FONT_SIZE8:
            fontlabindex.AscChaset = ASC5X7;//get index
            fontlabindex.HZChaset = ASC5X7;
			lcd_fillmatrix = drv_lcd_FillVertMatrix1;
			break;
		case FONT_SIZE16: //Vertical Matrix	
            fontlabindex.AscChaset = ASC8X16;//get index
            fontlabindex.HZChaset = HZ16X16;
			lcd_fillmatrix = drv_lcd_FillVertMatrix;
			break;
		case FONT_SIZE24://Level Matrix
            fontlabindex.AscChaset = ASC12X24;//get index
            fontlabindex.HZChaset = HZ24X24;
			lcd_fillmatrix = drv_lcd_FillLevelMatrix;
			break;
	}
	mode |= i;
//    TRACE("\n-|MOde:%04X",mode);
	if ((mode & DISP_INVCHAR)
        || (mode & DISP_INVLINE))
	{
		reverse = 1;
	}
	if(mode & DISP_MEDIACY)
	{
        len = font_get_str_dotwidth(&fontlabindex,LCD_LENGTH-x,len,(uint8_t *)sbuffer,&tDotInfo);		
		i = LCD_LENGTH-x;
        if(tDotInfo.DotWidth < i)
        {       	
            i = (i - tDotInfo.DotWidth)>>1;
            x = i+x;
        }
		sbuffer[len] = 0;
	}
    
    line_flag = 1;
	for (i=0; i<len; i+=k)
	{
		k = 1;
		if (sbuffer[i] == '\r')
		{	
			x = 0;
			continue;
		} else if (sbuffer[i] == '\n')
		{
			y += tDotInfo.DotHeight;			
            x = 0;
            line_flag = 1;
			continue;			
		}
		memset(buffer,0x00,sizeof(buffer));	
		font_get_dot(&fontlabindex,(uint8_t *)&sbuffer[i], (len-i), &k, &tDotInfo, buffer);
		switch (mode&0xFF)
		{
			case FONT_SIZE8://ACSII:5*7 7*7
				vert_byte = 1;
				tDotInfo.DotHeight++;//SAVELEN_ASC5X7
				width = 8;
				if (reverse)			
				{
					for (j=0; j<tDotInfo.DotWidth; j++)
					{
						buffer[j] ^= 0x7F;
					}
				}
				break;
			case FONT_SIZE12://ASCII:6*12   Chinese characters:12*12 ASC6X12
				vert_byte = 2;
				width = 16;
				tmp = vert_byte*tDotInfo.DotWidth;
				axisymmetric_x(buffer,tmp);
				if (reverse)			
				{
					for (j=0; j<tmp/2; j++)
					{
						buffer[j] ^= 0xFF;
					}
					for (; j<tmp; j++)
					{
						buffer[j] ^= 0x0F;
                    }
				}
				break;
			case FONT_SIZE16://ASCII:8*16   Chinese characters:16*16  ASC8X16
				vert_byte = 2;
				width = 16;
				if (reverse)			
				{
					tmp = tDotInfo.DotWidth<<1;
					for (j=0; j<tmp; j++)
					{
						buffer[j] ^= 0xFF;
					}
				}
				break;
			case FONT_SIZE24://ASCII:12*24   Chinese characters:24*24
				width = 24;
				vert_byte = 3;
				tmp = vert_byte*((tDotInfo.DotWidth ==12)?(16):(24));
				axisymmetric_x(buffer,tmp);
				if (reverse)			
				{
					for (j=0; j<tmp; j++)
					{
						buffer[j] ^= 0xFF;
					}
				}
				break;
			default:
				break;
		}		
		//x: DotWidth <--> length   y: DotHeight <--> width
//		if (x+tDotInfo.DotWidth > (LCD_LENGTH-1) )
		if (x+tDotInfo.DotWidth > LCD_LENGTH)
		{
            if ((mode & DISP_INVLINE) && (x < LCD_LENGTH-1)) {
                drv_lcd_xor_matrix(x,y,LCD_LENGTH-x,gcFontLabDotInfo[fontlabindex.HZChaset].height);
            }
            if ((mode&0xFF) == FONT_SIZE24)
            {
                //横向点阵
            } else 
            {//竖向点阵
                y += tDotInfo.DotHeight;			
                y &= (LCD_WIDE-1);
                x = 0;
                line_flag = 1;
            }
        }
        if ((mode & DISP_CLRLINE) && line_flag != 0)
        {
            line_flag = 0;
            drv_lcd_clear_matrix(0,y,LCD_LENGTH,tDotInfo.DotHeight,0);
        }
        if (mode & DISP_CLRCHAR)
        {
            drv_lcd_clear_matrix(x,y,tDotInfo.DotWidth,tDotInfo.DotHeight,0);
        }
        if (reverse_first == 0)
        {
            reverse_first = 1;
            if (mode & DISP_INVLINE && x != 0) {
                drv_lcd_xor_matrix(0,y,x,gcFontLabDotInfo[fontlabindex.HZChaset].height);
            }
        }
//        TRACE("\n 2  x:%d y:%d dotwidth:%d length:%d width:%d",x,y,
//              tDotInfo.DotHeight,tDotInfo.DotWidth,width);
        lcd_fillmatrix(x, y,tDotInfo.DotWidth,width,buffer);			
        x += tDotInfo.DotWidth;
        if (x>(LCD_LENGTH-1))
        {
            y += tDotInfo.DotHeight;
            x &= (LCD_LENGTH-1);			
            y &= (LCD_WIDE-1);
        }		
    }	
//    TRACE("\n 2  x:%d y:%d dotwidth:%d length:%d width:%d",x,y,
//          tDotInfo.DotHeight,tDotInfo.DotWidth,width);
    if ((mode & DISP_INVLINE) && (x < LCD_LENGTH-1)) {
        drv_lcd_xor_matrix(x,y,LCD_LENGTH-x,gcFontLabDotInfo[fontlabindex.HZChaset].height);
    }
    drv_lcd_update();
#endif
}

void lcd_Printf(const char *format, ...)
{
#if CFG_LCD
	va_list varg;
	char sbuffer[176];
	uint8_t buffer[72];
	int i,j,len,x,y;
	uint32_t k=1;
	_DOTINFO_ tDotInfo;
	_CHARFONT_ fontlabindex;
	uint8_t vert_byte;//Vertical Matrix:The number of bytes each column
	uint8_t width;
    uint8_t tmp;
//    uint8_t line_flag = 0,tmp;
	int (* lcd_fillmatrix)(uint8_t x, uint8_t y,
						uint8_t length,uint8_t width,
						const uint8_t *input);
	
	memset(buffer,0x00,sizeof(buffer));
	memset(sbuffer,0x00,sizeof(sbuffer));	
	va_start (varg, format);
    i = vsnprintf (sbuffer, sizeof(sbuffer)-1, format, varg);
    va_end (varg);
	if (i<0 || i >= sizeof(sbuffer)-1)
		i = sizeof(sbuffer)-1;
	sbuffer[i] = '\0';
	len = strlen(sbuffer);

    fontlabindex.AscChaset = ASC6X12;//get index
    fontlabindex.HZChaset = HZ12X12;
    lcd_fillmatrix = drv_lcd_FillVertMatrix1;

//    line_flag = 1;
    x = gLcdSys.glcd_x;
    y = gLcdSys.glcd_y;
	for (i=0; i<len; i+=k)
	{
		k = 1;
		if (sbuffer[i] == '\r')
		{	
			x = 0;
			continue;
		} else if (sbuffer[i] == '\n')
		{
			y += tDotInfo.DotHeight;			
            x = 0;
//            line_flag = 1;
			continue;			
		}
		memset(buffer,0x00,sizeof(buffer));	
		font_get_dot(&fontlabindex,(uint8_t *)&sbuffer[i], (len-i), &k, &tDotInfo, buffer);

        vert_byte = 2;
        width = 16;
        tmp = vert_byte*tDotInfo.DotWidth;
        axisymmetric_x(buffer,tmp);
        if (gLcdSys.SysOperate.bit.reverse == ON)
        {
            for (j=0; j<tmp/2; j++)
            {
                buffer[j] ^= 0xFF;
            }
            for (; j<tmp; j++)
            {
                buffer[j] ^= 0x0F;
            }
        }

		//x: DotWidth <--> length   y: DotHeight <--> width
//		if (x+tDotInfo.DotWidth > (LCD_LENGTH-1))
		if (x+tDotInfo.DotWidth > LCD_LENGTH)
		{
            //12dot
            y += tDotInfo.DotHeight;			
            y &= (LCD_WIDE-1);
            x = 0;
            //            line_flag = 1;
        }
        if ( y+tDotInfo.DotHeight > LCD_WIDE) {
            y = 0;
        }
//        TRACE("\n 2  x:%d y:%d dotwidth:%d length:%d width:%d",x,y,
//              tDotInfo.DotHeight,tDotInfo.DotWidth,width);
//        if ( line_flag != 0 ) {
//            line_flag = 0;
//            drv_lcd_clear_matrix(0,y,LCD_LENGTH,tDotInfo.DotHeight,0);
//        }
        drv_lcd_clear_matrix(x,y,tDotInfo.DotWidth,tDotInfo.DotHeight,0);
        lcd_fillmatrix(x, y,tDotInfo.DotWidth,width,buffer);			
//        TRACE("\r\n x:%d y:%d tDotInfo.DotWidth:%d tDotInfo.DotHeight:%d  width:%d \r\n",x,y,tDotInfo.DotWidth,tDotInfo.DotHeight,width);
        x += tDotInfo.DotWidth;
        if (x>(LCD_LENGTH-1))
        {
            y += tDotInfo.DotHeight;
            x &= (LCD_LENGTH-1);			
            y &= (LCD_WIDE-1);
        }		
    }	

    gLcdSys.glcd_x = x;
    gLcdSys.glcd_y = y;
    drv_lcd_update();
#endif
}

int kb_InCount(uint8_t x,uint8_t y,char *s)
{
    uint8_t ucBuff[32];
    int j=0;
    int key;

    memset(ucBuff,0x00,sizeof(ucBuff));
    drv_kb_clear();
    lcd_display(x,y,FONT_SIZE12|DISP_MEDIACY,"%s:",s);
    x += (strlen(s)+1)*6;
    while(1)
    {
        key = drv_kb_getkey(-1);
        if(key == 0x0D) {
            break;
        } else if(key <= '9' && key >= '0') {
            ucBuff[j++] = key;
            lcd_display(x,y,FONT_SIZE12|DISP_MEDIACY,"%c",ucBuff[j-1]);
            x += 6;
        } else if(key == KEY_CANCEL) {
            return 0;
        } else if(key == KEY_BACKSPACE) {
            if(j) {
                ucBuff[j--] = ' ';
                x -= 6;
                lcd_display(x,y,FONT_SIZE12|DISP_MEDIACY,"%d",ucBuff[j]);
            }
        }
    }
    return (int)atoi((char const *)ucBuff);
}

int s_lcdStipple(int x, int y, int color)
{
#if CFG_LCD
    drv_lcd_drawdot(x,y,(color?1:0));
#endif
    return 0;
}

void lcd_bmp_disp(int x, int y, const void *bmpdata)
{
#if CFG_LCD
    if((x < 0) || (x >= LCD_LENGTH))
    {
        return;
    }

    if((y < 0) || (y >= LCD_WIDE))
    {
        return;
    }
    s_DrawBitmap(x, y, EM_BMP_ROTATE0, EM_BMP_NOT_TRANSPARENCE, bmpdata, s_lcdStipple);
    drv_lcd_update();
#endif
}

void lcd_bmpmap_disp(uint8_t x, uint8_t  y, uint8_t  width, uint8_t  hight, void *scrbmp)
{
#if CFG_LCD
    if (scrbmp == NULL)
    {
        return;
    }
    drv_lcd_FillVertMatrixBigEndBits(x,y,width,hight,scrbmp);
    drv_lcd_update();
#endif
}
int s_sysinfo_font_info(uint *outlen, void *outbuf)
{
    _exfontinfo t[4];
    memset((char *)t,0,sizeof(t));
    uint i=0;
    // ASC 6X12
    t[i].charset=CHARSET_ASCII;t[i].height=gcFontLabDotInfo[2].height;t[i].width=gcFontLabDotInfo[2].width;
    ++i;
    // HZ 12X12
    t[i].charset=CHARSET_GB2312_HZ;t[i].height=gcFontLabDotInfo[5].height;t[i].width=gcFontLabDotInfo[5].width;
    ++i;
    *outlen = i*sizeof(_exfontinfo);
    memcpy((char *)outbuf,(char *)t,*outlen);
    return 0;
}

void s_touchscreen_gpio(void)
{
    gpio_set_output(PTC9,GPIO_OUTPUT_SlewRateFast,0); 
#ifdef PRO_S1000_V100
    gpio_set_bit(PTC9,0);
#else  
    gpio_set_bit(PTC9,1);
#endif
}


