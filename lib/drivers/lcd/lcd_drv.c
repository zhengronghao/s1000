/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : lcd_drv.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 7/31/2014 2:20:11 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "lcd_hw.h"
#include "lcd_drv.h"

struct LCD_SYS_DEF gLcdSys UPAREA;

void drv_lcd_update_switch(uint8_t status)
{
    if (status)
        gLcdSys.SysOperate.bit.update = ON;
    else
        gLcdSys.SysOperate.bit.update = OFF;
}

void drv_lcd_update(void)
{
    uint8_t i;

    if (gLcdSys.SysOperate.bit.update == OFF)
    {
        return;
    }
    for (i=0; i<LCD_MAX_PAGE; i++)
    {
        hw_lcd_set_page(i);
        hw_lcd_set_column(0);
        hw_lcd_write_data(gLcdSys.dram[i],LCD_LENGTH);
    }
//    gLcdSys.LcdBackLightTimerCnt10ms = LCD_BACKLIGHT_TIMERCNT/10;
//    hw_lcd_backlight(LCD_BACKLIGHT_ON);
}

/**
 * \brief   Clear Screen         
 *			
 * \param:
 */
void drv_lcd_cls(void)
{
    memset(gLcdSys.dram,0,sizeof(gLcdSys.dram));
    gLcdSys.glcd_x = 0;
    gLcdSys.glcd_y = 0;
    drv_lcd_update();
}

/**
 * \brief 	Set the coordinate(x,y) point in LCD ram.  
 *
(x,y)       x:0--->127
     --------------------------------------------------
(0,0)|................................................|(127,0)
  y  |................................................|
  0  |................................................|
  |  |................................................|
  V  |................................................|
  63 |................................................|
     |................................................|
(0,63)------------------------------------------------|(127,63)
 * \param: x   coordinate x:[0,127]
 * \param: y   coordinate y:[0,63]
 * \param: status  0-the ram fill with 0,1-the ram fill with 1
 */
void drv_lcd_drawdot(uint8_t x, uint8_t y,uint8_t status)
{
    uint8_t page;
    uint8_t bitd;

    y &= (LCD_WIDE-1);
    x &= (LCD_LENGTH-1);
    page = (y>>3);  // x/8	
    bitd = (y&(LCD_MAX_PAGE-1));// x%8
    if (status)
        gLcdSys.dram[page][x] |= (1<<bitd); 
    else
        gLcdSys.dram[page][x] &= (~(1<<bitd)); 
}

void drv_lcd_xordot(uint8_t x, uint8_t y)
{
    uint8_t page;
    uint8_t bitd;

    y &= (LCD_WIDE-1);
    x &= (LCD_LENGTH-1);
    page = (y>>3);  // x/8	
    bitd = (y&(LCD_MAX_PAGE-1));// x%8
    gLcdSys.dram[page][x] ^= (1<<bitd); 
}

/**
 * \brief   clear       
 *
 *(x,y)       x:0--->127
       --------------------------------------------------
 (0,0) |................................................|(127,0)
    y  |................/length \.......................|
    0  |............../|---------|......................|
    |  |..........width|rectangle|......................|
    V  |..............\|_________|......................|
    63 |................................................|
       |................................................|
(0,63) --------------------------------------------------(127,63)
 * \param: x   		coordinate x:[0,63]
 * \param: y   		coordinate y:[0,127]
 * \param: length   
 * \param: width	'width' is 8 multiples
 * \param status     clear with 0 or 1
 */
int drv_lcd_clear_matrix(uint8_t startx,uint8_t starty,uint8_t length,uint8_t width,uint8_t dot)
{
    uint8_t endx,endy,x,y;
	if (width > LCD_WIDE 
		|| length > LCD_LENGTH)
	{
		return -1;
	}
//    TRACE("\n-|4 x,y(%d,%d) len:%d width:%d",startx,starty,length,width);
	starty &= (LCD_WIDE-1);
	startx &= (LCD_LENGTH-1);
    endx = startx+length;
    endy = starty+width;
    if (endx >= LCD_LENGTH) {
        length -= (LCD_LENGTH-startx);
        endx = LCD_LENGTH;
    } else {
        length = 0;
    }
    if (endy >= LCD_WIDE) {
        width -= (LCD_WIDE-starty);
        endy = LCD_WIDE;
    } else {
        width = 0;
    }
//    TRACE("\n-|4 x,y(%d,%d) len:%d width:%d ex,ey(%d,%d)",startx,starty,length,width,endx,endy);
    for(y=starty; y<endy; y++)
    {
        for(x=startx; x<endx; x++)
        {
            drv_lcd_drawdot(x,y,dot);
        }
	}
    if (length>0 && width>0)
//    if (width>0)
    {
        startx = 0;
        starty = 0;
        endx = length;
//        endx = 128;
        endy = width;
//        TRACE("\n-|4 x,y(%d,%d) len:%d width:%d ex,ey(%d,%d)",startx,starty,length,width,endx,endy);
        for(y=starty; y<endy; y++)
        {
            for(x=startx; x<endx; x++)
            {
                drv_lcd_drawdot(x,y,dot);
            }
        }
    }
    return 0;
}

int drv_lcd_xor_matrix(uint8_t startx,uint8_t starty,uint8_t length,uint8_t width)
{
    uint8_t endx,endy,x,y;
	if (width > LCD_WIDE 
		|| length > LCD_LENGTH)
	{
		return -1;
	}
//    TRACE("\n-|4 x,y(%d,%d) len:%d width:%d",startx,starty,length,width);
	starty &= (LCD_WIDE-1);
	startx &= (LCD_LENGTH-1);
    endx = startx+length;
    endy = starty+width;
    if (endx >= LCD_LENGTH) {
        length -= (LCD_LENGTH-startx);
        endx = LCD_LENGTH;
    } else {
        length = 0;
    }
    if (endy >= LCD_WIDE) {
        width -= (LCD_WIDE-starty);
        endy = LCD_WIDE;
    } else {
        width = 0;
    }
//    TRACE("\n-|4 x,y(%d,%d) len:%d width:%d ex,ey(%d,%d)",startx,starty,length,width,endx,endy);
    for(y=starty; y<endy; y++)
    {
        for(x=startx; x<endx; x++)
        {
            drv_lcd_xordot(x,y);
        }
	}
    if (length>0 && width>0)
    {
        startx = 0;
        starty = 0;
        endx = length;
        endy = width;
//        TRACE("\n-|4 x,y(%d,%d) len:%d width:%d ex,ey(%d,%d)",startx,starty,length,width,endx,endy);
        for(y=starty; y<endy; y++)
        {
            for(x=startx; x<endx; x++)
            {
                drv_lcd_xordot(x,y);
            }
        }
    }
    return 0;
}

/**
 * \brief    Read-Modify-Write
 *
(x,y)                  x:0--->127
     --------------------------------------------------
(0,0)|................................................|(127,0)
  y  |................................................|
  0  |................................................|
  |  |................................................|
  V  |................................................|
  63 |................................................|
     |................................................|
(0,63)------------------------------------------------|(127,63)
 * \param: x   coordinate x:[0,127]
 * \param: y   coordinate y:[0,63]
 * \param data     data
 */
void drv_lcd_writebyte(uint8_t x, uint8_t y,uint8_t data)
{
    uint8_t page;
    uint8_t bitd;

    y &= (LCD_WIDE-1);
    x &= (LCD_LENGTH-1);
    page = (y>>3);  
    bitd = (y&(LCD_MAX_PAGE-1));
//    TRACE("\r\nx:%d y:%d page:%d bitd:%d data:%x",x,y,page,bitd,data);
    if (bitd)
    {
        gLcdSys.dram[page][x] |= (data<<bitd); 
        gLcdSys.dram[(page+1)&(LCD_MAX_PAGE-1)][x] |= (data>>(8-bitd)); 
    } else
    {
        gLcdSys.dram[page][x] |= data;
    }
}

void drv_lcd_writebyte_xor(uint8_t x, uint8_t y,uint8_t data)
{
    uint8_t page;
    uint8_t bitd;

    y &= (LCD_WIDE-1);
    x &= (LCD_LENGTH-1);
    page = (y>>3);  
    bitd = (y&(LCD_MAX_PAGE-1));
    if (bitd)
    {
        gLcdSys.dram[page][x] ^= data<<bitd;
        gLcdSys.dram[page+1][x] ^= data>>(8-bitd);
    } else
    {
        gLcdSys.dram[page][x] ^= data;
    }
}


/**
 * \brief        Draw a straight line.
 *			Initial coordinate(sx,sy),End coordinate(ex,ey).  
 * \param:sx,ex		[0,127]
 * \param:sy,ex		[0,63] 
 * \param:val		
 */
void drv_lcd_drawline(uint8_t sx,uint8_t sy,uint8_t ex,uint8_t ey,uint8_t val)
{
    uint16_t t; 
    uint16_t row,col;
    uint16_t xerr=0,yerr=0,distance;  
    int16_t  incx,incy,delta_x,delta_y;  

    delta_x = ex-sx;                            //计算坐标增量  
    delta_y = ey-sy;  
    col = sx;  
    row = sy;  

    if (delta_x>0)
    {
        incx=1;	//设置单步方向  
    } else   
    {  
        if( delta_x == 0) {
            incx = 0;                //垂直线  
        } else {
            incx = -1;
            delta_x = -delta_x;
        }  
    }

    if (delta_y > 0)
    {
        incy=1;
    } else  
    {  
        if( delta_y==0 ) {
            incy = 0;                //水平线  
        } else {
            incy = -1;
            delta_y = -delta_y;
        }  
    }

    if( delta_x > delta_y)
    {
        distance = delta_x;    	   //选取基本增量坐标轴 
    } else
    {
        distance = delta_y;
    }
    xerr = delta_x;  
    yerr = delta_y;		
    for (t=0; t<=distance; t++)       //distance值比实际线长度小1。
    {                                            		//画线输出  
        drv_lcd_drawdot(col,row,val); 
        xerr += delta_x ;  
        yerr += delta_y  ;            
        if (xerr > distance)  		//基本增量坐标轴每次循环都会变化，另一个坐标轴则按比例变化。  
        {  
            xerr -= distance;  
            col += incx;  
        }  
        if (yerr > distance)  
        {  
            yerr -= distance;  
            row += incy;  
        }  
    }  

    drv_lcd_update();
}

/**
 * \brief    // 八对称性    
 *		
 *					
 * \param      
 */
static inline void _draw_circle_8(uint8_t xc, uint8_t yc, uint8_t x, uint8_t y, uint8_t c) 
{
	// 参数 c 为颜色值
	drv_lcd_drawdot(xc + x, yc + y, c);
	drv_lcd_drawdot(xc - x, yc + y, c);
	drv_lcd_drawdot(xc + x, yc - y, c);
	drv_lcd_drawdot(xc - x, yc - y, c);
	drv_lcd_drawdot(xc + y, yc + x, c);
	drv_lcd_drawdot(xc - y, yc + x, c);
	drv_lcd_drawdot(xc + y, yc - x, c);
	drv_lcd_drawdot(xc - y, yc - x, c);
}
/**
 * \brief  	Bresenham's circle algorithm	
 *			(xc, yc)为圆心，r 为半径     
 *				
 * \param     
 * \param     
 */
void drv_lcd_circle(uint8_t xc, uint8_t yc, uint8_t r,uint8_t c) 
{
	int16_t x = 0, y = r, d;	
	
	if (xc + r < 0  ||	yc + r < 0 ) 
		return;
	
	d = 3 - 2 * r;

	while (x <= y) 
	{
		_draw_circle_8(xc, yc, x, y, c);

		if (d < 0) 
		{
			d = d + 4 * x + 6;
		} else
		{
			d = d + 4 * (x - y) + 10;
			y --;
		}
		x++;
	}
    drv_lcd_update();
}

/**
 * \brief          rectangle filling algorithm:Vertical Matrix
 * \Rules:Vertical take 8 points, then level shift one.
 *			End of the level of vertical down eight points, repeat.
 *
 *(x,y)       x:0--->127
       --------------------------------------------------
 (0,0) |................................................|(127,0)
    y  |................/length \.......................|
    0  |............../|---------|......................|
    |  |..........width|rectangle|......................|
    V  |..............\|_________|......................|
    63 |................................................|
       |................................................|
(0,63) --------------------------------------------------(127,63)
 * \param: x   		coordinate x:[0,63]
 * \param: y   		coordinate y:[0,127]
 * \param: length   
 * \param: width	'width' is 8 multiples
 * \param input   	buffer holding the input data
 */
int drv_lcd_FillVertMatrix1(uint8_t x, uint8_t y,
						uint8_t length,uint8_t width,
						const uint8_t *input)
{
	uint8_t col_x,page_y;

	if ((width&(LCD_MAX_PAGE-1)) > 0
		|| width > LCD_WIDE 
		|| length > LCD_LENGTH)
	{
		return -1;
	}
	y &= (LCD_WIDE-1);
	x &= (LCD_LENGTH-1);
    for (page_y=0; page_y<width; page_y+=LCD_MAX_PAGE)
    {
        for (col_x=0; col_x<length; col_x++) 
        {
            drv_lcd_writebyte(x+col_x,y+page_y,*input);
            input++;
        }
    }
	return 0;
}

uint8_t drv_lcd_BigAndLittleEndBitsSwitch(uint8_t input)
{
    uint8_t j;
    uint8_t tmp;

    tmp = input;		
    input = 0x00;
    for (j=0x80; j>0x00; j>>=1)
    {	
        input >>= 1;	
        if (tmp & j)
        {
            input |= 0x80; 
        }			
    }
    return input;
}

int drv_lcd_FillVertMatrixBigEndBits(uint8_t x, uint8_t y,
                                     uint8_t length,uint8_t width,
                                     const uint8_t *input)
{
	uint8_t col_x,page_y;
    uint8_t tmp;

	if ((width&(LCD_MAX_PAGE-1)) > 0
		|| width > LCD_WIDE 
		|| length > LCD_LENGTH)
	{
		return -1;
	}
	y &= (LCD_WIDE-1);
	x &= (LCD_LENGTH-1);
    for (page_y=0; page_y<width; page_y+=LCD_MAX_PAGE)
    {
        for (col_x=0; col_x<length; col_x++) 
        {
            tmp = drv_lcd_BigAndLittleEndBitsSwitch(*input);
            drv_lcd_writebyte(x+col_x,y+page_y,tmp);
            input++;
        }
    }
	return 0;
}


int drv_lcd_FillVertMatrix1_xor(uint8_t x, uint8_t y,
                                uint8_t length,uint8_t width,
                                const uint8_t *input)
{
	uint8_t col_x,page_y;

	if ((width&(LCD_MAX_PAGE-1)) > 0
		|| width > LCD_WIDE 
		|| length > LCD_LENGTH)
	{
		return -1;
	}
	y &= (LCD_WIDE-1);
	x &= (LCD_LENGTH-1);
    for (page_y=0; page_y<width; page_y+=LCD_MAX_PAGE)
    {
        for (col_x=0; col_x<length; col_x++) 
        {
            drv_lcd_writebyte_xor(x+col_x,y+page_y,*input);
            input++;
        }
    }
	return 0;
}

/**
 * \brief          rectangle filling algorithm:Vertical Matrix
 * \Rules:Vertical take 'width' points, then level shift one, repeat.
 *(x,y)       x:0--->127
       --------------------------------------------------
 (0,0) |................................................|(127,0)
    y  |................/length \.......................|
    0  |............../|---------|......................|
    |  |..........width|rectangle|......................|
    V  |..............\|_________|......................|
    63 |................................................|
       |................................................|
(0,63) --------------------------------------------------(127,63)
 * \param: x   		coordinate x:[0,63]
 * \param: y   		coordinate y:[0,127]
 * \param: length   
 * \param: width	'width' is 8 multiples
 * \param input   	buffer holding the input data
 */
int drv_lcd_FillVertMatrix(uint8_t x, uint8_t y,
						uint8_t length,uint8_t width,
						const uint8_t *input)
{
	uint8_t col_x,page_y;

	if ((width&(LCD_MAX_PAGE-1)) > 0
		|| width > LCD_WIDE 
		|| length > LCD_LENGTH)
	{
		return -1;
	}
	y &= (LCD_WIDE-1);
	x &= (LCD_LENGTH-1);
    for (col_x=0; col_x<length; col_x++) 
    {
        for (page_y=0; page_y<width; page_y+=LCD_MAX_PAGE)
        {
            drv_lcd_writebyte(x+col_x,y+page_y,*input);
            input++;
        }
    }
	return 0;
}

/**
 * \brief:rectangle filling algorithm:Level Matrix
 * \Rules:Level take 'length' points, then Vertical down shift one bit, repeat.
 *(x,y)       x:0--->127
       --------------------------------------------------
 (0,0) |................................................|(127,0)
    y  |................/length \.......................|
    0  |............../|---------|......................|
    |  |..........width|rectangle|......................|
    V  |..............\|_________|......................|
    63 |................................................|
       |................................................|
(0,63) --------------------------------------------------(127,63)
 * \param: x   		coordinate x:[0,63]
 * \param: y   		coordinate y:[0,127]
 * \param: length   
 * \param: width	'width' is 8 multiples
 * \param input   	buffer holding the input data
 */
int drv_lcd_FillLevelMatrix(uint8_t x, uint8_t y,
						uint8_t length,uint8_t width,
						const uint8_t *input)
{
	uint8_t i,j;
	uint8_t level_byte,tmp;
	uint8_t index;
	uint32_t bitmap;

	if ((width&(LCD_MAX_PAGE-1)) > 0
		|| width >LCD_WIDE 
		|| length > LCD_LENGTH)
	{	
		return -1;
	}
	y &= (LCD_WIDE-1);
	y &= (LCD_LENGTH-1);
	level_byte = (length+7)>>3;
	index = 0;
	for (i=0; i<width; i++)
	{
		bitmap = 0;
		for (j=0; j<level_byte; j++)
		{
			bitmap |= (input[j+index]<<(8*j));
		}
		for (j=0; j<length; j++)
		{
			tmp = (bitmap & 0x01);
			if (x+j>(LCD_LENGTH-1))
				drv_lcd_drawdot(x+j,(y+width)&(LCD_WIDE-1),tmp);
			else
				drv_lcd_drawdot(x+j,y&(LCD_WIDE-1),tmp);
			bitmap >>= 1;
		}	
		index += level_byte;
		y++;
	}

	return 0;
}

int drv_lcd_setbacklight(uint8_t mode)
{
    switch (mode)
    {
    case LCD_MODE_TEMPOFF:
        mode = LCD_MODE_TIMEROUT;
    case LCD_MODE_ALWAYSOFF:
        hw_lcd_backlight(LCD_BACKLIGHT_OFF);
        break;
    case LCD_MODE_TIMEROUT:
    case LCD_MODE_ALWAYSON:
        gLcdSys.LcdBackLightTimerCnt10ms = gLcdSys.LcdBackLightTimerOut10ms;
        hw_lcd_backlight(LCD_BACKLIGHT_ON);
        break;
    default:
        return -1;
    }
    gLcdSys.LcdBackLightMode = mode;
    return 0;
}

//----------------------------------------------
void gui_drawbox(uint8_t Xbegin,uint8_t Ybegin,uint8_t Xend,uint8_t Yend,uint8_t BoxModel,uint8_t FillType)
{
	uint8_t n;

	if(BoxModel==BoxModel_NoBox)
    {
        if(FillType==FillType_NoFill)
		{
			return ; 
        }
    }
	else
	{
		drv_lcd_drawline(Xbegin,Ybegin,Xend,Ybegin,BoxModel);
		drv_lcd_drawline(Xbegin,Ybegin,Xbegin,Yend,BoxModel);
		drv_lcd_drawline(Xbegin,Yend,Xend,Yend,BoxModel);
		drv_lcd_drawline(Xend,Ybegin,Xend,Yend,BoxModel);
	}
	if(FillType!=FillType_NoFill)
	{
		for(n=Xbegin+1;n<=Xend;n++)
		{
			drv_lcd_drawline(n,Ybegin+1,n,Yend-1,FillType);
		}
	}
}


void gui_window(char x,char Y,char Width,char Height)
{
	unsigned char i=0;
	//黑色阴影
	//gui_drawbox(x+4,Y+4,x+Width+4,Y+Height+4,BoxModel_NoBox,FillType_Fill);
	//绘制边框阴影 （黑色阴影优化）
	for(i=0;i<4;i++)
	{
		drv_lcd_drawline(x+4,Y+Height+i,x+Width+i,Y+Height+i,1);
		drv_lcd_drawline(x+Width+i,Y+4,x+Width+i,Y+Height+i,1);
	}
	//清扫空地
	gui_drawbox(x,Y,Width+x,Height+Y,BoxModel_NoBox,FillType_Clear);
	//画框框
	gui_drawbox(x,Y,Width+x,Height+Y,BoxModel_Draw,FillType_NoFill);
}



void drv_lcd_init(int backligth_mode)
{
    hw_lcd_init();
    memset(&gLcdSys,0x00,sizeof(struct LCD_SYS_DEF));
    drv_lcd_update_switch(ON);
    drv_lcd_update();
    gLcdSys.LcdBackLightTimerOut10ms = LCD_BACKLIGHT_TIMERCNT/10;
    gLcdSys.SysOperate.bit.enable = 1;
    drv_lcd_setbacklight(backligth_mode);
}


void drv_lcd_reinit(void)
{
    hw_lcd_init();
    drv_lcd_update_switch(ON);
}


void drv_lcd_close(void)
{
    drv_lcd_update_switch(OFF);
}




