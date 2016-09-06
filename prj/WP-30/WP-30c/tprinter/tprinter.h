#ifndef __TPRINTER_H
#define __TPRINTER_H

#include "wp30_ctrl.h"
#ifdef CFG_TPRINTER

uint8_t tprinter_open (uint8_t mode);

uint8_t tprinter_close(void);

uint8_t tprinter_line_set (uint32_t linedot);

uint8_t tprinter_desity_set(uint8_t degree);

uint8_t tprinter_feed_paper (uint32_t linedot);

uint8_t tprinter_get_status();

uint8_t tprinter_bitmap(uint16_t width, uint16_t high, uint16_t x_offset, uint16_t y_offset,uint8_t *data);

uint8_t tprinter_bkmark_locate(void);

uint16_t tprinter_printbuf_freeline(void);
#endif
#endif

