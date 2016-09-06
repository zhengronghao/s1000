#ifndef __COM_em3096_H
#define __COM_em3096_H

//#include "board.h"
#include "common.h"


#ifdef em3096_DEBUG
   #define em3096_print(s) printf(s)
#else
   #define em3096_print(s)
#endif

uint8_t com_em3096_open();

uint8_t com_em3096_close();

uint8_t com_em3096_write(uint8_t *data , uint32_t len);

uint32_t com_em3096_read(uint8_t *data);

uint16_t com_em3096_scan_start(uint32_t timeout);

uint32_t com_em3096_open_read(uint8_t *data , uint32_t time);

uint32_t com_em3096_scan(void);

void com_em3096_scan_cancel(void);

void em3096_trig_finish(void);

#endif

