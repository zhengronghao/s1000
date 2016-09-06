/*
 * =====================================================================================
 *
 *       Filename:  Magtek.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  5/9/2014 11:41:54 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yehf (), yehf@itep.com.cn
 *        Company:  START
 *
 * =====================================================================================
 */
#ifndef _MAGTEK_H_
#define _MAGTEK_H_ 

/*-----------------------------------------------------------------------------}
 *  º¯Êý
 *-----------------------------------------------------------------------------{*/
void magtek_enable_VCC(void);
void magtek_disable_VCC(void);
void magtek_data(int mode);
uchar magtek_readdata(void);
void magtek_strobe(int mode);
int magtek_check(void);

int mag_magtek_init(int mode);
int mag_magtek_open(int mode);
int mag_magtek_close(int mode);
int mag_magtek_read(void *Track1, void *Track2, void *Track3);
int mag_magtek_ioctl(int mode, int para);
int mag_magtek_main (int mode);

#endif



