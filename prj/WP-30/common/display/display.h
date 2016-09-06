/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : display.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 11/10/2014 11:17:44 AM
 * Description        : 
 *******************************************************************************/
#ifndef __DISPLAY_H__
#define __DISPLAY_H__


typedef struct _FONTLAB_ROM
{
    uint8_t version[18];
    uint8_t GB2312_12X12[18*6768];
    uint8_t ASCII_6X12[12*96];
}FONTLABSTRUCT;

#define SA_GB2312_12X12           (SA_KEYZONE+FPOS(FONTLABSTRUCT,GB2312_12X12))
#define SA_ASCII_6X12             (SA_KEYZONE+FPOS(FONTLABSTRUCT,ASCII_6X12))

//interior code addr high:ica_h;
//interior code addr lower:ica_l;
#define FONT_GET_ADDR_GB2312_12X12(ica_l,ica_h)  ( SA_GB2312_12X12\
                                                  + (((ica_l)-0xB0)*94 + (ica_h) - 0xA1)*18 )
#define FONT_GET_ADDR_ASCII_6X12(ica)     (SA_ASCII_6X12+((ica)-0x20)*12)

typedef struct FontLabDotInfo
{
    uint8_t width;
    uint8_t height;
    uint8_t dotbyte;     //the bytes of dot matrix
    uint8_t incodebyte; //the Bytes of Internal Code 
}FONTLABDotInfo;

typedef struct
{
    uchar charset;        //  字符集
    uchar height;         //  字体高度
    uchar width;          //  字体宽度
    uchar rfu;
}_exfontinfo;

enum _CHAR_FONT_
{
	ASC5X7=0, ASC7X7,  ASC6X12, ASC8X16, ASC12X24, 
	HZ12X12,  HZ16X16, HZ24X24
};

typedef struct 
{
    uchar AscChaset;  
    uchar HZChaset;   
}_CHARFONT_;

#define CHARSET_ASCII       0X10
#define CHARSET_GB2312_ZF   0X21
#define CHARSET_GB2312_HZ   0X22
#define CHARSET_GB18030_ZF  0X31
#define CHARSET_GB18030_HZ  0X32
#define CHARSET_GB18030_TW  0X33

#define CHARSET_GB18030_IME 0X3A
#define CHARSET_GB18030_IMEBIHUA 0X3B
#define CHARSET_GB2312_IMEBIHUA 0X3C
#define CHARSET_GBKSIMPLE_HWI  0X3D   //手写输入法GBK简装版字库
#define CHARSET_GBKBASE_HWI  0X3E     //手写输入法GBK基本版字库

//点阵信息
typedef struct 
{
    uchar DotWidth;    //点阵宽度
    uchar DotHeight;   //点阵高度
}_DOTINFO_;  


#define     FONT_SIZE8          8       //  小字体显示(6*8) 无汉字
#define     FONT_SIZE12         12      //  大字体显示(8*16) 汉字(12*12)
#define     FONT_SIZE16         16      //  大字体显示(8*16) 汉字(16*16)
#define     FONT_SIZE24         24      //  大字体显示(12*24) 汉字(24*24)
#define     DISP_CLRLINE        0x100   //  清除显示占用的行
#define     DISP_MEDIACY        0x200   //  居中显示
#define     DISP_INVCHAR        0x400   //  反白显示（字符）
#define     DISP_INVLINE        0x800   //  反白显示（整行）
#define     DISP_CLRCHAR        0x1000  //  清除显示占用的行
#define     DISP_XORCHAR        0x2000  //  当前显示字符与液晶缓存异或
//*********************************************************

//各种字体点阵存储长度
#define     SAVELEN_DOT           1            // 插入间隔所需字节数
#define     SAVELEN_ASC5X7        5            // 5X7的ASCII码所需字节数
#define     SAVELEN_ASC7X7        7            // 7X7的ASCII码所需字节数
#define     SAVELEN_ASC6X12       12           // 6X12的ASCII码所需字节数
#define     SAVELEN_ASC8X16       16           // 8X16的ASCII码所需字节数
#define     SAVELEN_ASC12X24      48           // 24X12的ASCII码所需字节数48
#define     SAVELEN_HZ12X12       24           // 
#define     SAVELEN_HZ16X16       32           // 汉字处理所需的字节数
#define     SAVELEN_HZ24X24       72           // 24X24 汉字处理所需的字节数72
//各种字体点阵点阵宽度
#define     WIDTH_DOT           1            // 插入间隔所需字节数
#define     WIDTH_ASC5X7        8            // 5X7的ASCII码所需字节数
#define     WIDTH_ASC7X7        8            // 7X7的ASCII码所需字节数
#define     WIDTH_ASC6X12       6            // 6X12的ASCII码所需字节数
#define     WIDTH_ASC8X16       8            // 8X16的ASCII码所需字节数
#define     WIDTH_ASC12X24      12           // 24X12的ASCII码所需字节数48
#define     WIDTH_HZ12X12       12           // 
#define     WIDTH_HZ16X16       16           // 汉字处理所需的字节数
#define     WIDTH_HZ24X24       24           // 24X24 汉字处理所需的字节数72
//*********************************************************

#define DISP_FONT  							FONT_SIZE12

#define   DISP_FONT_LINE0     				DISP_FONT*0
#define   DISP_FONT_LINE1     				DISP_FONT*1
#define   DISP_FONT_LINE2     				DISP_FONT*2
#define   DISP_FONT_LINE3     				DISP_FONT*3
#define   DISP_FONT_LINE4     				DISP_FONT*4
#define   DISP_FONT_LINE5     				DISP_FONT*5
#define   DISP_FONT_LINE6     				DISP_FONT*6
#define   DISP_FONT_LINE7     				DISP_FONT*7
#define   DISP_FONT_LINE8     				DISP_FONT*8



void lcd_Display(int x,int y,int mode,const char *format,...);
void lcd_Printf(const char *format, ...);
int kb_InCount(uint8_t x,uint8_t y,char *s);
void axisymmetric_x(uint8_t *input,uint16_t length);
void lcd_bmp_disp(int x, int y, const void *bmpdata);
void lcd_bmpmap_disp(uint8_t x, uint8_t  y, uint8_t  width, uint8_t  hight, void *scrbmp);
int s_sysinfo_font_info(uint *outlen, void *outbuf);
void s_touchscreen_gpio(void);

#ifdef CFG_LCD

#define lcd_display(x,y,mode,format,...) lcd_Display(x,y,mode,format,##__VA_ARGS__)
#define lcd_printf                       lcd_Printf

#define lcdDispMultiLang(col, line, mode, ch, en, arg...)    \
    do {    \
        if(s_lcdgetMenuLang())\
            lcd_display(col, line, mode, ch, ## arg);\
        else    \
            lcd_display(col, line, mode, en, ## arg);\
    } while(0)
//#define lcdDispMultiLang(col, line, mode, cn, en, arg...)    \
//    do {   \
//          lcd_display(col, line, mode, cn, ## arg);\
//    } while(0)

#else 

#define lcd_display(x,y,mode,format,...)    do{}while(0) 
#define lcd_printf(format,...)              do{}while(0) 
#define lcdDispMultiLang(col, line, mode, ch, en, arg...)  do{}while(0)    

#endif

#endif


