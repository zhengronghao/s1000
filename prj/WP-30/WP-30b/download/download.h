/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : download.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/14/2014 10:28:23 AM
 * Description        : 
 *******************************************************************************/

#ifndef __DOWNLOAD_H__
#define __DOWNLOAD_H__


//frame answer return value
#define DL_FRAME_SUCCESS      0x05      //frame success
#define DL_FRAME_SUCCESS_END  0x04      //last frame
#define DL_FRAME_ERR          0x15      //frame error

//write flash result
#define DL_PROGRAM_SUCCESS  0x00        //write flash success
#define DL_PROGRAM_ERR      0x01        //write flash error

//frame format
#define DL_CHAR_START       0x02        //frame START
#define DL_CHAR_END_ONE     0x17        //current frame ending
#define DL_CHAR_END_ALL     0x03        //all frames ending
#define DL_CHAR_HAND_REQ    0x50        //handshake request
#define DL_CHAR_HAND_ACK    0x06        //handshake ack


#define DL_FRAME_SIZE       (2*1024)

typedef struct _DL_TYPE_DEF
{
    uint8_t stx;
    uint8_t length_h;
    uint8_t length_l;
    uint8_t bin[DL_FRAME_SIZE];
    uint8_t lrc;
    uint8_t etx;
    uint8_t rfu;
    uint16_t bin_length;
}DLTYPE_DEF;


int dl_clear_buffer(void);
int dl_check_readbuf(void);
int dl_read(uint8_t *output, uint32_t length, int t_out_ms);
int dl_write(uint8_t const *input, uint32_t length);
int dl_open(uint32_t com, uint8_t *buf_dl, uint32_t buf_size);
int dl_ctrl(uint32_t *code_length,int t_out_ms);
int dl_close(void);
int dl_boot(uint32_t *code_length,int t_out_ms);
int dl_write_flash(uint32_t addr,uint32_t *code_length,int t_out_ms);

#endif

