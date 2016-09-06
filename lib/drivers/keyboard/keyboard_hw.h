/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : keyboard_hw.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/21/2014 8:48:35 PM
 * Description        : 
 *******************************************************************************/

#ifndef __KEYBOARD_HW_H__
#define __KEYBOARD_HW_H__ 
#include "common.h"

typedef enum 
{
   KB_COL0,  //not used
   KB_COL1,
   KB_COL2,
   KB_COL3,
   KB_COL4,
   KB_ROW0,
   KB_ROW1,
   KB_ROW2,
   KB_ROW3,
   KB_INDEPENDENT0,
}KB_IndexDef;



//bitmap: |bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|
//        |col7|col6|col5|col4|col3|col2|col1|col0|
//col0 not used:bit4 is indepent0
//        |col6|col5|col4|indp|col4|col3|col2|col1|
inline uint8_t hw_kb_get_col(void)
{
   return (uint8_t)((~(hw_gpio_get_value(KB_GPIO_COL)>>KB_PINx_COL1)) & KB_COL_MAXTRIX_MASK);
}

inline void hw_kb_set_row(uint8_t row)
{
    row &= KB_COL_MAXTRIX_MASK;
    hw_gpio_set_bits(KB_GPIO_ROW,(row<<KB_PINx_ROW0));
}

inline void hw_kb_reset_row(uint8_t row)
{
    row &= KB_COL_MAXTRIX_MASK;
    hw_gpio_reset_bits(KB_GPIO_ROW,(row<<KB_PINx_ROW0));
}

inline void hw_kb_scan_row_line(uint8_t line)
{
    hw_gpio_set_bits(KB_GPIO_ROW,(KB_COL_MAXTRIX_MASK<<KB_PINx_ROW0));
    hw_kb_reset_row(1<<line);
}
#if defined(KB_NOTMAXTRIX)
inline uint8_t hw_kb_get_independent0(void)
{
   return (uint8_t)(hw_gpio_get_bit(KB_GPIO_INDEPENDENT0,1<<KB_PINx_INDEPENDENT0));
}
#endif

void hw_kb_gpio_init(void);
void hw_kb_col_irq_open(KB_IndexDef pinx);
void hw_kb_col_irq_close(KB_IndexDef pinx);

#endif



