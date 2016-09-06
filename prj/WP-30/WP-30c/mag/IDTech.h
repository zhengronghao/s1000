/*
 * =====================================================================================
 *
 *       Filename:  IDTech.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  5/9/2014 11:53:49 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yehf (), yehf@itep.com.cn
 *        Company:  START
 *
 * =====================================================================================
 */

#ifndef __IDTECK_H__
#define __IDTECK_H__ 
void idt_RouteInit(void);
uint idt_poll(void);
uchar idt_ucIfDataRdy(void);
uchar idt_ScanMagCard(uint *puiStack1BitLen, uint *puiStack2BitLen, uint *puiStack3BitLen,uchar * b1, uchar * b2, uchar * b3);

int mag_IDT178_init(int mode);
int mag_IDT178_open(int mode);
int mag_IDT178_close(int mode);
int mag_IDT178_read(void *Track1, void *Track2, void *Track3);
int mag_IDT178_ioctl(int mode, int para);
int mag_IDT178_main (int mode);
#endif

