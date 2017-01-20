#ifndef _TP_H
#define _TP_H

#include "wp30_ctrl.h"
#ifdef CFG_TPRINTER

//#define TP_STROBE_STEP_TWO
#define TP_STROBE_STEP_FOUR

#define BUFFER_LINE		                        (64)
#define LineDot		                               (384)
#define TP_MAX_HEAT_STROBE                  (1)        
#define TP_MAX_HEAT_DOT                       (96)    
#define TP_MAX_HEAT_DOT_64                    (64)    
#define TpMinWaitTime                              (50)
//#define HEAT_TIMER                                  BD_TIMER3

#if defined(TP_STROBE_STEP_TWO)
#define TP_MAX_STROBE_STEP                   (2)        
#endif

#if defined(TP_STROBE_STEP_FOUR)
#define TP_MAX_STROBE_STEP                   (4)        
#endif

#define TP_OK                   0x00
#define TP_PAPER_READY_MASK                 0x01
#define TP_PAPER_BLOCK_MASK                 0x02
#define TP_STATUS_MASK                           0x04
#define TP_TEMP_MASK                              0x08
#define TP_CUT_MASK                                0x10
#define TP_MARK_MASK                              0x20
#define TP_HEAD_MASK                              0x40
#define TP_POWER_OUT_MASK                     0x80

#define  POWRER_LOW_ALARM_LV1                     335
#define  POWRER_LOW_ALARM_LV2                     360

#define  TEMP_HIGH_ALARM                         63


struct ignore_mode {
    uchar paper_out : 1;
    uchar temperature : 1;
    uchar power : 1;
    uchar FULL : 5;
};

typedef struct _TP_STATES{
    struct ignore_mode ignore; 
    uint16_t temperature;      //温度
    uint16_t voltage;          //电压
    uint16_t count;
    uint8_t paper;            //是否有纸 TRUE 有纸  FALSE 缺纸
    uint8_t paper_out_count;            
}states;


extern states tp_states; 


void tp_init(void);

void tp_close(void);

uint8_t tp_feed_line(uint16_t line);

uint8_t tp_print_line(uint8_t  *dot);

uint8_t tp_printbuf_empty(void);

void tp_feed_to_mark(uint16_t line);

void tp_set_desity(uint8_t speed);

uint16_t tp_printbuf_freeline(void);

uint8_t tp_get_status(void);

#endif
#endif

