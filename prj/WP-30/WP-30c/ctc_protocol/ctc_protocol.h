#ifndef __CTC_PROTOCOL_H
#define __CTC_PROTOCOL_H

#include "common.h"
#include "ParseCmd.h"

#define FRAME_FIX_LEN                                             7
#define PUBKEY_LEN                                                   536
#define ITEP_HASH_LEN                                              72

uint32_t ctc_send_frame(MCUPCK res, uint8_t *data) ;

void ctc_magcard_operate(MCUPCK res, uint8_t *outbuf);

void ctc_nprinter_operate(MCUPCK res, uint8_t *data,  uint32_t len);

uint8_t ctc_recev_frame(uint8_t mode, uint8_t *cmd, uint32_t cmd_len);

int ctc_frame_check(uint8_t *data, uint32_t size, int *s_pos, int *e_pos);

int ctc_uart_open(void); 

int ctc_uart_dma_check(void);

int ctc_uart_close(void); 

int ctc_uart_restart(void); 

int ctc_uart_open_nodma(void);

int ctc_uart_close_nodma(void);
#endif
