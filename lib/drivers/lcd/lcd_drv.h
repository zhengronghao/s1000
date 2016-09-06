/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : lcd_drv.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 7/31/2014 2:24:47 PM
 * Description        : 
 *******************************************************************************/
#ifndef __LCD_CMD_DRV_H__
#define __LCD_CMD_DRV_H__

//===============================================================================================
#define LCD_ON			(uint8_t)1
#define LCD_OFF			(uint8_t)0
#define LCD_LENGTH       128
#define LCD_WIDE         64   //COM0~COM63
#define LCD_MAX_PAGE    (LCD_WIDE>>3)
typedef union _LCD_BITMAP
{
    uint8_t byte;
    struct {
        uint8_t enable:1;
        uint8_t update:1;
        uint8_t reverse:1;
    }bit;
}LCD_BITMAP;

struct LCD_SYS_DEF
{
    uint8_t dram[LCD_MAX_PAGE][LCD_LENGTH];
    uint8_t glcd_x;
    uint8_t glcd_y;
    volatile uint16_t LcdBackLightTimerCnt10ms; //unit:10ms
    volatile uint16_t LcdBackLightTimerOut10ms; //unit:10ms
    volatile uint8_t  LcdBackLightMode;
    volatile LCD_BITMAP SysOperate;
};

#define LCD_MODE_TEMPOFF   0 
#define LCD_MODE_TIMEROUT  1
#define LCD_MODE_ALWAYSON  2
#define LCD_MODE_ALWAYSOFF 3

extern struct LCD_SYS_DEF gLcdSys;


void drv_lcd_cls(void);
void drv_lcd_drawdot(uint8_t x, uint8_t y,uint8_t status);
void drv_lcd_drawline(uint8_t sx,uint8_t sy,uint8_t ex,uint8_t ey,uint8_t val);
void drv_lcd_circle(uint8_t xc, uint8_t yc, uint8_t r,uint8_t c);
void drv_lcd_init(int backligth_mode);
void drv_lcd_reinit(void);
void drv_lcd_close(void);

void drv_lcd_update_switch(uint8_t status);
void drv_lcd_update(void);
int  drv_lcd_setbacklight(uint8_t mode);

int drv_lcd_FillVertMatrix1(uint8_t x, uint8_t y,
						uint8_t length,uint8_t width,
						const uint8_t *input);
int drv_lcd_FillVertMatrixBigEndBits(uint8_t x, uint8_t y,
                                     uint8_t length,uint8_t width,
                                     const uint8_t *input);
int drv_lcd_FillVertMatrix(uint8_t x, uint8_t y,
						uint8_t length,uint8_t width,
						const uint8_t *input);
int drv_lcd_FillLevelMatrix(uint8_t x, uint8_t y,
						uint8_t length,uint8_t width,
						const uint8_t *input);
int drv_lcd_FillVertMatrix1_xor(uint8_t x, uint8_t y,
                                uint8_t length,uint8_t width,
                                const uint8_t *input);
int drv_lcd_clear_matrix(uint8_t x,uint8_t y,uint8_t length,uint8_t width,uint8_t dot);
int drv_lcd_xor_matrix(uint8_t startx,uint8_t starty,uint8_t length,uint8_t width);

//BoxModel »­·½¿òµÄÄ£Ê½
#define BoxModel_Draw 0x01 //»­±ß¿ò
#define BoxModel_NoBox 0x03 //ÎÞ±ß¿ò
#define BoxModel_Clear 0x00 //»­°×É«±ß¿ò
#define BoxModel_Not 0x02 //±ß¿ò·´ÏÔ

//FillType ¾ØÐÎÌî³äÄ£Ê½
#define FillType_Fill 0x01 //Ìî³äºÚÉ«
#define FillType_Clear 0x00 //Ìî³ä°×É«
#define FillType_Not 0x02 //Ìî³ä·´É«
#define FillType_NoFill 0x03 //²»Ìî³ä

void gui_window(char x,char Y,char Width,char Height);
void gui_drawbox(uint8_t Xbegin,uint8_t Ybegin,uint8_t Xend,uint8_t Yend,uint8_t BoxModel,uint8_t FillType);

#endif

