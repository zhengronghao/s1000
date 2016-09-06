/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : keyboard_drv.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/21/2014 8:48:35 PM
 * Description        : 
 *******************************************************************************/


#ifndef __KEYBOARD_DRV_H__
#define __KEYBOARD_DRV_H__ 

#define MAX_KEYTABLE      10

//  定义按键值
#define     KEY0                    0x30	//  '0'
#define     KEY1                    0x31	//  '1'
#define     KEY2                    0x32	//  '2'
#define     KEY3                    0x33	//  '3'
#define     KEY4                    0x34	//  '4'
#define     KEY5                    0x35	//  '5'
#define     KEY6                    0x36	//  '6'
#define     KEY7                    0x37	//  '7'
#define     KEY8                    0x38	//  '8'
#define     KEY9                    0x39	//  '9'
#define     KEY_BACKSPACE           0x08	//  back
#define	    KEY_CLEAR				0x2E
#define     KEY_SYMBOL              0x13
#define     KEY_ALPHA               0x07    //字母
#define     KEY_UP                  0x26
#define     KEY_DOWN                0x28
#define     KEY_F1                  0x15
#define     KEY_F2                	0x14
#define     KEY_F3                  0x16
#define     KEY_F4                  0x17
//#define     KEY_F8                  0x18
#define     KEY_ENTER               0x0D
#define     KEY_FUNC                KEY_F1 
#define     KEY_CANCEL              0x18
#define     KEY_INVALID             0xFF
#define     KEY_TIMEOUT             0x00
#define     KEY_DEALOTHER           0x99

//  串输入控制和返回值
#define 	KB_MAXLEN          		256         //  串输入最大允许输入的长度
#define 	KB_EN_REVDISP     		0x01        //  1（0） 正（反）显示
#define 	KB_EN_FLOAT        		0x02        //  1（0） 有（否）小数点
#define 	KB_EN_SHIFTLEFT   		0x04        //  1（0） 左（右）对齐输入
#define 	KB_EN_CRYPTDISP   		0x08        //  1（0） 是（否）密码方式
#define 	KB_EN_CHAR         		0x10        //  1（0） 能（否）输字符
#define 	KB_EN_NUM           	0x20        //  1（0） 能（否）输数字
#define 	KB_EN_BIGFONT      		0x40        //  1（0） 大（小）字体
#define 	KB_EN_PRESETSTR    		0x80    	//  1（0） 是（否）允许预设的字符串
#define     KB_EN_NEWLINE           0x100       //  1（0） 是（否）允许左对齐输入换行显示//  预设几组常用的输入控制模式
#define     KB_BIG_ALPHA            0x1D5       //  大字体字符串输入
#define     KB_BIG_NUM              0x1E5       //  大字体数字串输入
#define     KB_BIG_PWD              0x4D        //  大字体密码输入
#define     KB_SMALL_ALPHA          0x195       //  小字体字符串输入
#define     KB_SMALL_NUM            0x1A5       //  小字体数字串输入
#define     KB_SMALL_PWD            0xD         //  小字体密码输入

#define     KB_CANCEL               3501     //  串输入被用户按取消键退出
#define     KB_TIMEOUT              3502     //  串输入超时退出
#define     KB_ERROR                3503      //  串输入参数非法
#define     KB_OVERBUF              3504      //  按键缓存区满
#define     KB_CHANGE               3505      //  按切换键退出

//	输入控制模式
#define		KB_HZMODE_NORMAL		(0<<1)			//正常模式
#define		KB_HZMODE_EX			(1<<0)		//扩展模式


//默认输入法，可与 KB_HZMODE_NORMAL,KB_HZMODE_NORMAL使用"或"组合

#define 	KB_HZMODE_CUR_PY 			(0<<1)      //默认拼音输入法
#define 	KB_HZMODE_CUR_BH 			(1<<1)		//默认笔划输入法
#define 	KB_HZMODE_CUR_CHAR 			(2<<1)		//默认字符输入法
#define 	KB_HZMODE_CUR_NUM 			(3<<1)		//默认数字输入法

typedef struct KBCALLBACK
{
    void (*normalkey)(void);
    void (*longkey)(void);
}KB_CALLBACK;

extern const KB_CALLBACK gcKbCallBack;

void drv_kb_open(void);
void drv_kb_close(void);
int drv_kb_read(uint8_t *buffer,uint8_t length,int32_t timout_ms);
int drv_kb_hit(void);
int drv_kb_ifkey(int keyval);
void drv_kb_clear(void);
int drv_kb_read_buffer(uint8_t *buffer);
uint8_t drv_kb_ifprocessing(void);
uint8 drv_kb_NOtMaxtrix_ifprocessing(void);
int drv_kb_inkey(int keyval);

int drv_kb_getkey(int timeout_ms);
#define KB_Hit drv_kb_hit
#define KB_GetKey drv_kb_getkey 
#define KB_Flush drv_kb_clear 

void PORTC_KeyBoard_IRQHandler(void);
void drv_kb_systick_irq(void);
#if defined(KB_NOTMAXTRIX)
void PORTB_KB_NotMaxtrix_IRQHandler(void);
void drv_kb_NotMaxtrix_clear_irq(void);
#endif
#endif



