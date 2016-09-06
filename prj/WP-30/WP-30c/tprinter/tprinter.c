
#include "wp30_ctrl.h"
#ifdef CFG_TPRINTER

uint8_t tprinter_open (uint8_t mode)
{
    memcpy(&tp_states.ignore, &mode,1);
  //  TRACE("\r\ntp_states.ignore.paper:%d",tp_states.ignore.paper_out);    

    gSystem.lpwr.bm.prn = 1; //低功耗标示
    
    tp_init();
    
    return RET_OK;
}

uint8_t tprinter_close(void)
{

    gSystem.lpwr.bm.prn = 0; //低功耗标示
    tp_close();
    
    return RET_OK;
}

uint8_t tprinter_line_set(uint32_t linedot)
{
    return RET_OK;
}

uint8_t tprinter_desity_set(uint8_t degree)
{
    tp_set_desity(degree);
    
    return RET_OK;
}

uint8_t tprinter_feed_paper(uint32_t linedot)
{
    return tp_feed_line(linedot);
}

uint8_t tprinter_bitmap(uint16_t width, uint16_t high, uint16_t x_offset, uint16_t y_offset, uint8_t *data)
{
    uint32_t i,j;
    uint8_t buf[LineDot/8];
    uint8_t ret;

    if (x_offset != 0) {
        if (x_offset%8) {
            x_offset = x_offset/8+1;
        } else {
            x_offset = x_offset/8;
        }
    }
    if (y_offset!=0) {
        tp_feed_line(y_offset);
    }

    if (width != 0) {
        if (width%8) {
            width = width/8+1;
        } else {
            width = width/8;
        }
    }
   
    for (j = 0; j < high; j++) {
        memset(buf, 0x00, sizeof(buf));
        for (i = 0; i < width; i++) {
            buf[x_offset+i] = data[j*width +i];
        }
        ret = tp_print_line(buf);
        if(ret != TP_OK)
        {
            return ret;
       }
    }
    return RET_OK;
}

uint8_t tprinter_get_status()
{
    uint8_t status;
    
    status = tp_get_status();
    
    return status;
}

uint8_t tprinter_bkmark_locate(void)
{
    return RET_OK;
}

uint16_t tprinter_printbuf_freeline(void)
{
    return tp_printbuf_freeline();
}
#endif

