/***************** (C) COPYRIGHT 2015 START Computer equipment *****************
 * File Name          : local_dl_app.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 1/13/2015 9:54:45 AM
 * Description        : 
 *******************************************************************************/
#ifndef __LOCAL_DL_APP_H__
#define __LOCAL_DL_APP_H__ 

int GetCodeSignature(uint8_t *sha, uint8_t *mac, uint32_t ctrl_len);

int s_localdl_get_sn(void *sn);
int localdl_get_boot_version(const VERSION_INFO_NEW ** const ver);
int localdl_get_ctrl_version(const VERSION_INFO_NEW ** const ver);
int localdl_get_font_version(char *ver);
int s_localdl_save_data(void *data);
int s_localdl_get_terminal_info(struct LocalDlFrame *frame);
int localdl_boot_make_check(void *data);
int localdl_ctrl_make_check(void *data);
int s_localdl_save_sn(uint8_t *sn,uint8_t length);
int s_localdl_cancle(void);

int dlcom_open(uint8_t commport,int bps, uint8_t *buf_dl, uint32_t buf_size);
int dlcom_close(uint8_t commport);
int dlcom_check_readbuf(uint8_t commport);
int dlcom_clear(uint8_t commport);
int dlcom_read(uint8_t commport, uint8_t *output, uint32_t length, int t_out_ms);
int dlcom_write(uint8_t commport, uint8_t const *input, uint32_t length);
int dlcom_clear(uint8_t commport);
int s_localdl_display(int mode,void *data);
#endif

