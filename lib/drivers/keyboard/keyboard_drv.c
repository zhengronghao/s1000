/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : keyboard_drv.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 3/21/2014 4:36:04 PM
 * Description        : 
 *******************************************************************************/
#include "drv_inc.h"
#include "common.h"
#include "keyboard_drv.h"

#define KB_SIZE 16 
#define KB_VALID_KEY_TIME  2000  //ms
#define KB_FILTER_10MS  1 
typedef struct _MaxtrixKB_DEF
{
    SS_QUEUE queue;
    uint8_t val[KB_SIZE];
    uint8_t col_prev_sft[4];
    uint8_t col_previo[4];
    uint8_t t_ms[4];   //unit: 10ms*KB_FILTER_10MS 

    uint8_t col_trg; //trigger
    uint8_t col_release;
    uint8_t row_line;
    uint8_t t_10ms; //unit: 10ms

    uint8_t flag; //every key has timer flag
    uint8_t longkey; //every key has timer flag
    volatile uint8_t irq;
    volatile uint16_t mult_val;
    volatile uint8_t mult_row[2];
    volatile uint8_t mult_num;
}MaxtrixKB_DEF;
MaxtrixKB_DEF gkeyboard;

#if defined(KB_NOTMAXTRIX)
//independent key
typedef struct _NotMaxtrixKB_DEF
{
    uint8_t prev_io:4;
    uint8_t prev_sft:4;
    uint8_t trig:4;
    uint8_t release:4;
    uint8_t sirq:3;
    uint8_t longkey:1;
    uint8_t t_10ms:4;
    uint8_t t_ms;   //unit: 10ms*KB_FILTER_10MS 
}NotMaxtrixKB_DEF;

static NotMaxtrixKB_DEF gNotMaxtrxKB;
#endif

#define gkb_col_trg    gkeyboard.col_trg
#define gkb_col_release    gkeyboard.col_release
#define gkb_prev_sft    gkeyboard.col_prev_sft[gkeyboard.row_line]
#define gkb_col_previo   gkeyboard.col_previo[gkeyboard.row_line]
#define gkb_tms    gkeyboard.t_ms[gkeyboard.row_line]
#define gkb_flag_get    gkeyboard.flag&(1<<gkeyboard.row_line)
#define gkb_flag_set    (gkeyboard.flag |= (1<<gkeyboard.row_line))
#define gkb_flag_clr    (gkeyboard.flag &= ~(1<<gkeyboard.row_line))
#define gkb_longkey_get    (gkeyboard.longkey&(1<<gkeyboard.row_line))
#define gkb_longkey_set    (gkeyboard.longkey |= (1<<gkeyboard.row_line))
#define gkb_longkey_clr    (gkeyboard.longkey &= ~(1<<gkeyboard.row_line))


#if defined(MK210_V3)
//-----------wp70------------
const uint8_t gkb_table[4][4] = {
    KEY1,     KEY4,     KEY7,     KEY_CANCEL,
    KEY2,     KEY5,     KEY8,     KEY0,
    KEY3,     KEY6,     KEY9,     KEY_F1,
    KEY_ALPHA,KEY_CLEAR,KEY_ENTER,KEY_SYMBOL,
};
#endif
#if defined(WP30_V1)
/*-----------wp30_prototype------------*/
//const uint8_t gkb_table[4][4] = {
//    KEY_SYMBOL,KEY4,     KEY_ENTER,KEY_CANCEL,
//    KEY5,     KEY9,     KEY0,     KEY3,
//    KEY2,     KEY6,     KEY8,     KEY_CLEAR,
//    KEY1,     KEY4,     KEY7,     KEY_F1,
//};
/*-----------wp30 hardware modify 1 times------------*/
//const uint8_t gkb_table[4][4] = {
//    KEY_SYMBOL,KEY4,     KEY_ENTER,KEY_CANCEL,
//    KEY3,     KEY6,     KEY9,     KEY_CLEAR,
//    KEY2,     KEY5,     KEY8,     KEY0,
//    KEY1,     KEY4,     KEY7,     KEY_F1,
//};
/*-----------wp30 hardware modify 2 times------------*/
//const uint8_t gkb_table[4][4] = {
//    KEY1,     KEY4,     KEY7,     KEY_F1,
//    KEY2,     KEY5,     KEY8,     KEY0,
//    KEY3,     KEY6,     KEY9,     KEY_CLEAR,
//    KEY_ALPHA,KEY_CLEAR,KEY_ENTER,KEY_SYMBOL,
//};
/*-----------wp30 and mk210 keyboard version 1 modify 3 times------------*/
const uint8_t gkb_table[4][4] = {
    KEY1,     KEY4,     KEY7,     KEY_F1,
    KEY2,     KEY5,     KEY8,     KEY0,
    KEY3,     KEY6,     KEY9,     KEY_CLEAR,
    KEY_DOWN, KEY_UP,   KEY_ENTER,KEY_F2,
};
#endif



void drv_close_irq(void)
{
    hw_kb_col_irq_close(KB_COL1);
    hw_kb_col_irq_close(KB_COL2);
    hw_kb_col_irq_close(KB_COL3);
    hw_kb_col_irq_close(KB_COL4);
    hw_kb_col_irq_close(KB_INDEPENDENT0);
}



inline uint16_t drv_kb_keyval_switch(uint8_t col, uint8_t row_line)
{
    uint16_t val = 0xFFFF;

    switch (col)
    {
    case 0x1:
        val = gkb_table[0][row_line];
        break;
    case 0x2:
        val = gkb_table[1][row_line];
        break;
    case 0x4:
        val = gkb_table[2][row_line];
        break;
    case 0x8:
        val = gkb_table[3][row_line];
        break;
    case 0x03:
        val = gkb_table[0][row_line]<<8;
        val |= gkb_table[1][row_line];
        break;
    case 0x05:
        val = gkb_table[0][row_line]<<8;
        val |= gkb_table[2][row_line];
        break;
    case 0x06:
        val = gkb_table[1][row_line]<<8;
        val |= gkb_table[2][row_line];
        break;
    case 0x09:
        val = gkb_table[0][row_line]<<8;
        val |= gkb_table[3][row_line];
        break;
    case 0x0A:
        val = gkb_table[1][row_line]<<8;
        val |= gkb_table[3][row_line];
        break;
    case 0x0C:
        val = gkb_table[2][row_line]<<8;
        val |= gkb_table[3][row_line];
        break;
    default:
//        TRACE("\nkey not valid:%04b",col);
        break;
    
    }
    return val;
}



void kb_report_key(uint8_t status)
{
    uint16_t val;
    uint8_t ch;
//    static uint32_t time=0;

//    TRACE(" status:%04b", status);
    if (gkeyboard.mult_num == 2)
    {
//       TRACE("mult: row0%d-row1%d",gkeyboard.mult_row[0],gkeyboard.mult_row[1]);
       val = drv_kb_keyval_switch(gkb_col_release,gkeyboard.mult_row[0]);
       gkeyboard.mult_val = val;
       gkeyboard.mult_num = 0;
    } else 
    {
        val = drv_kb_keyval_switch(gkb_col_release,gkeyboard.row_line);
        if (val > 0xFF) {
            gkeyboard.mult_val = val;
        } else {
            ch = (val&0xFF);
            if (ch != KEY_INVALID)
                sQueueWrite(&gkeyboard.queue,&ch,1);
        }
        gkeyboard.mult_num = 0;
    }
//    TRACE(" key:%04X ",val);
//    if (val == KEY_ENTER)
//        time = 0;
//    else 
//        time ++;
//    TRACE(" times:%02d\n",time);
}


void drv_key_proc(void)
{
    uint8_t col_io_val; //current IO hardware value
    uint8_t col_curr_sft; //current soft value

    col_io_val = hw_kb_get_col();
	col_curr_sft = (col_io_val&gkb_col_previo)|gkb_prev_sft&(col_io_val^gkb_col_previo);
    gkb_col_trg = (~gkb_prev_sft)&col_curr_sft;
    gkb_col_release = gkb_prev_sft&(~col_curr_sft);
    if (gkb_col_trg) {
        gkeyboard.mult_num++;
        gkeyboard.mult_num %= 3;
        gkeyboard.mult_row[gkeyboard.mult_num-1] = gkeyboard.row_line;
//        TRACE("\n-|-io-|soft|trig|release-|");
//        TRACE("\n-V|%04b|%04b|%04b|%04b   -| %d-%05d",gkb_col_previo,gkb_prev_sft,gkb_col_trg,gkb_col_release,gkeyboard.row_line,sys_get_counter());
        gkb_flag_set;
        gkb_longkey_clr; 
    }
    if (gkb_flag_get) {
        gkb_tms++;
    }
//    if ((gkeyboard.flag==0x01 && gkeyboard.row_line ==0) || (gkeyboard.flag == 0x02 && gkeyboard.row_line == 1))
//    TRACE("\n-|%04b|%04b|%04b|%04b   -| %d %04b",gkb_col_previo,gkb_prev_sft,gkb_col_trg,gkb_col_release,gkeyboard.row_line,gkeyboard.flag);
    if ((gkb_col_release && gkb_flag_get)
        || (gkb_tms >= KB_VALID_KEY_TIME/(10*4*KB_FILTER_10MS))) //按住不放
    {
//        TRACE("\n-^|%04b|%04b|%04b|%04b   -| %d-%05d",gkb_col_previo,gkb_prev_sft,gkb_col_trg,gkb_col_release,gkeyboard.row_line,sys_get_counter());
        if (gkb_longkey_get == 0) {
            if (gkb_col_release == 0x00) {//长按键
                gkb_col_release = gkb_prev_sft;
                kb_report_key(0);
                gkb_col_release = 0;
            } else {
                kb_report_key(0);
            }
            if (gcKbCallBack.normalkey != NULL)
                gcKbCallBack.normalkey();
        }
        if (gkb_prev_sft == gkb_col_release //释放时没有按键未释放
            && gkb_col_trg==0x00)           //释放时没有按键触发
        {
            gkeyboard.irq = 2;
            gkb_prev_sft = 0;
            gkb_tms = 0;
            gkb_flag_clr;
            gkb_longkey_clr; 
        }
        if (gkb_col_release == 0x00) //长按键
        {
            gkb_longkey_set;
            gkb_tms = 0;
        }
    }
    gkb_col_previo = col_io_val;
    gkb_prev_sft = col_curr_sft;
}

void drv_kb_scankey(void)
{
    if (gkeyboard.irq == 0)
        return;
    if (gkeyboard.irq == 1)
    {
        if (++gkeyboard.t_10ms == KB_FILTER_10MS)
        {
            gkeyboard.t_10ms = 0;
            for (gkeyboard.row_line=0; gkeyboard.row_line<4; gkeyboard.row_line++)
            {
                hw_kb_scan_row_line(gkeyboard.row_line);
                drv_key_proc();
            }
        }
    }
    else if (gkeyboard.irq == 2) 
    {
        gkeyboard.irq = 0;
        hw_kb_reset_row(KB_COL_MAXTRIX_MASK);
    }
}



void drv_kb_init(void)
{
    memset(&gkeyboard,0x00,sizeof(gkeyboard));
    sQueueInit(&gkeyboard.queue,gkeyboard.val,KB_SIZE);
    gkeyboard.row_line = 0;
    gkeyboard.mult_val = 0xFFFF;

}


void drv_kb_open(void)
{
    drv_kb_init();
    hw_kb_gpio_init();
    hw_kb_reset_row(KB_COL_MAXTRIX_MASK);
    PORTx_IRQPin_Clear(KB_PORT_COL,KB_COL_MAXTRIX_MASK,KB_PINx_COL1);
    enable_irq(PORTC_IRQn);		
#if defined(KB_NOTMAXTRIX)
    PORTx_IRQPinx_Clear(KB_PORT_independent0,KB_PINx_INDEPENDENT0);
    enable_irq(PORTB_IRQn);		
//    if (hw_kb_get_independent0()) {
//        gNotMaxtrxKB.sirq = 1;
//    }
#endif
    if (hw_kb_get_col()) {
        gkeyboard.irq = 1;
    }
}

void drv_kb_close(void)
{
    drv_close_irq();		
}

int drv_kb_read(uint8_t *buffer,uint8_t length,int32_t timout_ms)
{
    uint32_t begin_time;
    uint8_t rev_length,tmp;

    if (NULL == buffer) {
        return -1;
    }
    if (length == 0) {
        return length;
    }
    rev_length = 0;
    begin_time = sys_get_counter();
    while (1)
    {
        tmp = sQueueRead(&gkeyboard.queue,
                         buffer+rev_length,
                         length);
        rev_length += tmp;
        if (rev_length == length) {
            break;
        }
        if ( (timout_ms >= 0) && (sys_get_counter() - begin_time > timout_ms)) {
            break;
        }
    }
    
    return rev_length;
}

int drv_kb_hit(void)
{
    return (sQueueDataSize(&gkeyboard.queue)>0)?1:0;
}

int drv_kb_read_buffer(uint8_t *buffer)
{
    int iRet=0;
    uint8_t number;
    unsigned char head;
    unsigned char tail;

    if (drv_kb_hit() > 0)
    {
        if (sQueueClearSemaphore(&gkeyboard.queue) == 0)
        {
            number = sQueueDataSize(&gkeyboard.queue);
            head = gkeyboard.queue.head;
            tail = gkeyboard.queue.tail;
            iRet = drv_kb_read(buffer,number,100);
            gkeyboard.queue.head = head;
            gkeyboard.queue.tail = tail;
            sQueuePost(&gkeyboard.queue);
        }
        if (iRet < 0)
        {
            iRet = 0;
        }
    }
    return iRet;
}

void drv_kb_clear(void)
{
    sQueueReset(&gkeyboard.queue);   
}


int drv_kb_getkey(int timeout_ms)
{
    uint8_t key;
    if( drv_kb_read(&key,1,timeout_ms)== 1)
    {
        return key;
    } else 
    {
        return KEY_TIMEOUT;
    }
}

int drv_kb_ifkey(int keyval)
{
    if (drv_kb_hit())
    {
        return (drv_kb_getkey(100) == keyval)?1:0; 
    }
    return 0;
}

int drv_kb_inkey(int keyval)
{
    uint8 i = 0;
    uint8 j = 0;
    uint8_t val ;

    if (keyval == KEY_CANCEL ) 
    {
        //取消键单独处理
        val = KEY_CANCEL;
        sQueueWrite(&gkeyboard.queue,&val,1);
        return OK;
    }
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            if (keyval == gkb_table[i][j] ) 
            {
                val = gkb_table[i][j];
                sQueueWrite(&gkeyboard.queue,&val,1);
                return OK;
            }
        }
    }
   return -KB_ERROR;
}
uint8 drv_kb_ifprocessing(void)
{
    return (gkeyboard.irq);
}

void PORTC_KeyBoard_IRQHandler(void)
{
//    TRACE("\nkirq");
    if (PORTx_IRQPinx(KB_PORT_COL1,KB_PINx_COL1)) {
//        TRACE(" COL1");
        PORTx_IRQPinx_Clear(KB_PORT_COL1,KB_PINx_COL1);
        gkeyboard.irq = 1;
        gkeyboard.mult_num=0;
    }
    if (PORTx_IRQPinx(KB_PORT_COL2,KB_PINx_COL2)) {
//        TRACE(" COL2");
        PORTx_IRQPinx_Clear(KB_PORT_COL2,KB_PINx_COL2);
        gkeyboard.irq = 1;
        gkeyboard.mult_num=0;
    }
    if (PORTx_IRQPinx(KB_PORT_COL3,KB_PINx_COL3)) {
//        TRACE(" COL3");
        PORTx_IRQPinx_Clear(KB_PORT_COL3,KB_PINx_COL3);
        gkeyboard.irq = 1;
        gkeyboard.mult_num=0;
    }
    if (PORTx_IRQPinx(KB_PORT_COL4,KB_PINx_COL4)) {
//        TRACE(" COL4");
        PORTx_IRQPinx_Clear(KB_PORT_COL4,KB_PINx_COL4);
        gkeyboard.irq = 1;
        gkeyboard.mult_num=0;
    }
//    TRACE("\n-|-io-|soft|trig|release-|");
}


#if defined(KB_NOTMAXTRIX)
void kb_NotMaxtrix_report_key(uint8_t status)
{
    uint8_t val;

//    val = gkb_table[0][3]; //cancel key
    val = KEY_CANCEL;
    sQueueWrite(&gkeyboard.queue,&val,1);
}

void drv_key_NotMaxtrix_proc(void)
{
    uint8_t col_io_val; //current IO hardware value
    uint8_t col_curr_sft; //current soft value

    col_io_val = hw_kb_get_independent0();
	col_curr_sft = (col_io_val&gNotMaxtrxKB.prev_io)
                   |gNotMaxtrxKB.prev_sft&(col_io_val^gNotMaxtrxKB.prev_io);
    gNotMaxtrxKB.trig = (~gNotMaxtrxKB.prev_sft)&col_curr_sft;
    gNotMaxtrxKB.release = (gNotMaxtrxKB.prev_sft&(~col_curr_sft));
    if (gNotMaxtrxKB.trig) {
//        TRACE("\n-|-prio-|prsft|trig|release-|%d-%d",sizeof(gNotMaxtrxKB),sizeof(gNotMaxtrxKB.t_ms));
//        TRACE("\n-V| %04b| %04b|%04b|%04b   -|%04b-| %dms",gNotMaxtrxKB.prev_io,gNotMaxtrxKB.prev_sft,gNotMaxtrxKB.trig,gNotMaxtrxKB.release,col_curr_sft,sys_get_counter());
        gNotMaxtrxKB.t_ms++; //0->1
        gNotMaxtrxKB.longkey = 0;
    } else {
        if (gNotMaxtrxKB.t_ms) {
            gNotMaxtrxKB.t_ms++;
        }
    }
    //    if ((gNotMaxtrxKB.flag==0x01 && gNotMaxtrxKB.row_line ==0) || (gNotMaxtrxKB.flag == 0x02 && gNotMaxtrxKB.row_line == 1))
//    TRACE("\n-| %04b| %04b|%04b|%04b   -|%04b-|tms:%d",gNotMaxtrxKB.prev_io,gNotMaxtrxKB.prev_sft,gNotMaxtrxKB.trig,gNotMaxtrxKB.release,col_curr_sft,gNotMaxtrxKB.t_ms);
    if ((gNotMaxtrxKB.release && gNotMaxtrxKB.t_ms)
        || (gNotMaxtrxKB.t_ms >= KB_VALID_KEY_TIME/(10*KB_FILTER_10MS))) //按住不放
    {
        if (gNotMaxtrxKB.t_ms >= KB_VALID_KEY_TIME/(10*KB_FILTER_10MS))
        {
            //add powerdown
            drv_kb_clear();
            kb_NotMaxtrix_report_key(0);
            if (gcKbCallBack.longkey != NULL)
                gcKbCallBack.longkey();
            gNotMaxtrxKB.longkey = 1;
        } else
        {
            kb_NotMaxtrix_report_key(0);
            if (gcKbCallBack.normalkey != NULL)
                gcKbCallBack.normalkey();
        }
        gNotMaxtrxKB.prev_sft = 0;
        gNotMaxtrxKB.t_ms = gNotMaxtrxKB.longkey;
        gNotMaxtrxKB.sirq = 0;
    }
    gNotMaxtrxKB.prev_io = col_io_val;
    gNotMaxtrxKB.prev_sft = col_curr_sft;
}

void drv_kb_NotMaxtrix_scankey(void)
{
    if (gNotMaxtrxKB.sirq == 1)
    {
        if (++gNotMaxtrxKB.t_10ms == KB_FILTER_10MS)
        {
            gNotMaxtrxKB.t_10ms = 0;
            drv_key_NotMaxtrix_proc();
        }
    }
}

uint8 drv_kb_NOtMaxtrix_ifprocessing(void)
{
    return (gNotMaxtrxKB.sirq);
}

void drv_kb_NotMaxtrix_clear_irq(void)
{
#if defined(KB_NOTMAXTRIX)
    gNotMaxtrxKB.sirq = 0;
#endif
}

void PORTB_KB_NotMaxtrix_IRQHandler(void)
{
//    TRACE("\nkirq");
    if (PORTx_IRQPinx(KB_PORT_independent0,KB_PINx_INDEPENDENT0)) {
//        TRACE(" NotMaxtrix");
        PORTx_IRQPinx_Clear(KB_PORT_independent0,KB_PINx_INDEPENDENT0);
        gNotMaxtrxKB.sirq = 1;
    }
}
#endif

void drv_kb_systick_irq(void)
{
    if (sQueueGetSemaphore(&gkeyboard.queue))
    {
#if defined(KB_NOTMAXTRIX)
        drv_kb_NotMaxtrix_scankey();
#endif
        drv_kb_scankey();
    }
}
