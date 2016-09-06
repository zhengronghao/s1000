#include "2D.h"
#include "drv_inc.h"
#ifdef CFG_SCANER

#ifdef SCANER_EM1395
uint8_t scaner_2d_open(void)
{
    return com_em1395_open();
}

uint8_t scaner_2d_close()
{
    return com_em1395_close();
}

void scaner_2d_scan_start(uint32_t time, uint32_t mode) 
{
    com_em1395_scan_start(time);
}

uint32_t scaner_2d_scan(void) 
{
    return com_em1395_scan();
}

void scaner_2d_scan_cancel(void)
{
    com_em1395_scan_cancel();
}
#else
uint8_t scaner_2d_open(void)
{
    gSystem.lpwr.bm.scan = 1; //µÕπ¶∫ƒ≈‰÷√
    return com_em3096_open();
}

uint8_t scaner_2d_close()
{
    gSystem.lpwr.bm.scan = 0; //µÕπ¶∫ƒ≈‰÷√
    return com_em3096_close();
}

void scaner_2d_scan_start(uint32_t time, uint32_t mode) 
{
    com_em3096_scan_start(time);
}

uint32_t scaner_2d_scan(void) 
{
    return com_em3096_scan();
}

void scaner_2d_scan_cancel(void)
{
    com_em3096_scan_cancel();
}
#endif
#endif
