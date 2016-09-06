/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : libapi_dbg.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 11/26/2014 5:42:34 PM
 * Description        : 
 *******************************************************************************/
#include "wp30_ctrl.h"
#include "libapi_dbg.h"
#include "../libdll/libdll.h"


#ifdef DEBUG_Dx 

#ifdef CFG_FS
const char buf_data[] = {
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
};
char *cyc_name = "fstest";
char *count_name = "count";
char *err_name = "err";
static int reset_count;
#define WRITE_SIZE  256
#define WRITE_COUNT 600
#define EXCHANGE_COUNT 20
int fs_write_err(const char *format,...)
{
//    TRACE("fs_write_err\n");

    int fd, ret;
    int       count=0;
    va_list     marker;
    char        buff[1024];	/* 1K 就够了，8K太浪费 */

    if (format == NULL){
        TRACE("no err data\n");
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"no err data\n");
        return -1;
    }
    memset(buff, 0, sizeof(buff));
    va_start( marker, format);
    count = vsnprintf(buff, sizeof(buff)-4, format, marker);
    va_end( marker );
    if(count == -1)	{
        count = sizeof(buff)-4;
    }
    fd = fs_open(err_name, O_RDWR|O_CREAT);
    if(fd < 0){
        TRACE("open %s err:%d\n", err_name, fd);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"open %s err:%d\n", err_name, fd);
        return fd;
    }
    ret = fs_write(fd, buff, strlen(buff));
    if(ret != strlen(buff)){
        TRACE("Write %s err:%d(%d)\n", err_name, ret, strlen(buff));
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"Write %s err:%d(%d)\n", err_name, ret, strlen(buff));
        goto write_err;
    }
    ret = fs_close(fd);
    if(ret != 0){
        TRACE("close %s err:%d\n", err_name, ret);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"close %s err:%d\n", err_name, ret);
        return -1;
    }
    return 0;
write_err:
    fs_close(fd);
    return -1;
}

int fs_reset_count()
{
//    TRACE("fs_reset_count\n");
    int fd, ret;
    ret = fs_exist(count_name);
    if(ret < 0){
        reset_count = 1;
    }else{
        ret = fs_size(count_name);
        if(ret < 0){
            TRACE("get %s size err:%d\n", count_name, ret);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"get %s size err:%d\n", count_name, ret);
            fs_write_err("get %s size err:%d\n", count_name, ret);
            return -1;
        }
        reset_count = ret + 1;
        ret = fs_remove(count_name);
        if(ret != 0){
            TRACE("rm %s err:%d\n", count_name, ret);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"rm %s err:%d\n", count_name, ret);
            fs_write_err("remove %s err:%d\n", count_name, ret);
            return -1;
        }
    }
    return 0;
}

int fs_cycle_write(int num)
{
//    TRACE("fs_cycle_write\n");
    int fd, ret, count, offset;
    char tmp[WRITE_SIZE];
    fd = fs_open(cyc_name, O_RDWR|O_CREAT);
    if(fd < 0){
        TRACE("open %s err:%d\n", cyc_name, fd);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"open %s err:%d\n", cyc_name, fd);
        fs_write_err("open %s err:%d\n", cyc_name, fd);
        return fd;
    }
    count = num;
    while(count--){
        ret = fs_write(fd, buf_data, WRITE_SIZE);
        if(ret != WRITE_SIZE){
            TRACE("Write %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"Write %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            fs_write_err("Write %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            goto write_err;
        }
        offset = fs_seek(fd, 0, SEEK_CUR);
        ret = fs_seek(fd, -WRITE_SIZE, SEEK_CUR);
        if(ret != offset-WRITE_SIZE){
            TRACE("seek err:%d(%d)\n", ret, offset-WRITE_SIZE);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"seek err:%d(%d)\n", ret, offset-WRITE_SIZE);
            fs_write_err("seek err:%d(%d)\n", ret, offset-WRITE_SIZE);
            goto write_err;
        }
        memset(tmp, 0x00, WRITE_SIZE);
        ret = fs_read(fd, tmp, WRITE_SIZE);
        if(ret != WRITE_SIZE){
            TRACE("read %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"read %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            fs_write_err("read %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            goto write_err;
        }
        if(memcmp(tmp, buf_data, WRITE_SIZE) != 0){
            TRACE("memcmp err:%d\n", count);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"memcmp err:%d\n", count);
            fs_write_err("memcmp err:%d\n", count);
            goto write_err;
        }
    }
    ret = fs_close(fd);
    if(ret != 0){
        TRACE("close %s err:%d\n", cyc_name, ret);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"close %s err:%d\n", cyc_name, ret);
        fs_write_err("close %s err:%d\n", cyc_name, ret);
        return -1;
    }
    return 0;
write_err:
    fs_close(fd);
    return -1;
}

int fs_exchange_data(int num)
{
//    TRACE("fs_exchange_data\n");
    int fd, ret, count, cyc_c, offset;
    cyc_c = 20;
    char tmp[WRITE_SIZE];
    fd = fs_open(cyc_name, O_RDWR);
    if(fd < 0){
        TRACE("open %s err:%d\n", cyc_name, fd);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"open %s err:%d\n", cyc_name, fd);
        fs_write_err("open %s err:%d\n", cyc_name, fd);
        return fd;
    }
    count = num;
    while(count--){
        offset = count*WRITE_SIZE*cyc_c;
        ret = fs_seek(fd, offset, SEEK_SET);
        if(ret != offset){
            TRACE("seek %s err:%d(%d)\n", cyc_name, ret, offset);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"seek %s err:%d(%d)\n", cyc_name, ret, offset);
            fs_write_err("seek %s err:%d(%d)\n", cyc_name, ret, offset);
            goto write_err;
        }
        ret = fs_write(fd, buf_data, WRITE_SIZE);
        if(ret != WRITE_SIZE){
            TRACE("exchange %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"exchange %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            fs_write_err("exchange %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            goto write_err;
        }
        offset = fs_seek(fd, 0, SEEK_CUR);
        ret = fs_seek(fd, -WRITE_SIZE, SEEK_CUR);
        if(ret != offset-WRITE_SIZE){
            TRACE("seek %s err:%d(%d)\n", cyc_name, ret, offset-WRITE_SIZE);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"seek %s err:%d(%d)\n", cyc_name, ret, offset-WRITE_SIZE);
            fs_write_err("seek %s err:%d(%d)\n", cyc_name, ret, offset-WRITE_SIZE);
            goto write_err;
        }
        memset(tmp, 0x00, WRITE_SIZE);
        ret = fs_read(fd, tmp, WRITE_SIZE);
        if(ret != WRITE_SIZE){
            TRACE("exchange read %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"exchange read %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            fs_write_err("exchange read %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            goto write_err;
        }
        if(memcmp(tmp, buf_data, WRITE_SIZE) != 0){
            TRACE("exchange memcmp err:%d\n", count);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"exchange memcmp err:%d\n", count);
            fs_write_err("exchange memcmp err:%d\n", count);
            goto write_err;
        }
    }
    ret = fs_close(fd);
    if(ret != 0){
        TRACE("close %s err:%d\n", cyc_name, ret);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"close %s err:%d\n", cyc_name, ret);
        fs_write_err("close %s err:%d\n", cyc_name, ret);
        return -1;
    }
    return 0;
write_err:
    fs_close(fd);
    return -1;
}

int memcmp_truncate(int num, int times)
{
//    TRACE("memcmp_truncate\n");
    int fd, ret, count, trun_len, offset;
    char tmp[WRITE_SIZE];
    trun_len = WRITE_SIZE*times;
    count = fs_size(cyc_name);
    if(count != WRITE_SIZE*num){
        TRACE("size %s err:%d(%d)\n", cyc_name, count, WRITE_SIZE*600);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"size %s err:%d(%d)\n", cyc_name, count, WRITE_SIZE*600);
        fs_write_err("size %s err:%d(%d)\n", cyc_name, count, WRITE_SIZE*600);
        return -1;
    }
    fd = fs_open(cyc_name, O_RDWR);
    if(fd < 0){
        TRACE("open %s err:%d\n", cyc_name, fd);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"open %s err:%d\n", cyc_name, fd);
        fs_write_err("open %s err:%d\n", cyc_name, fd);
        return fd;
    }
    count = num;
    while(count--){
        memset(tmp, 0x00, WRITE_SIZE);
        ret = fs_read(fd, tmp, WRITE_SIZE);
        if(ret != WRITE_SIZE){
            TRACE("read %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"read %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            fs_write_err("read %s err:%d(%d)\n", cyc_name, ret, WRITE_SIZE);
            goto write_err;
        }
        if(memcmp(tmp, buf_data, WRITE_SIZE) != 0){
            TRACE("memcmp err:%d\n", count);
            lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"memcmp err:%d\n", count);
            fs_write_err("memcmp err:%d\n", count);
            goto write_err;
        }
    }
    offset = WRITE_SIZE*(num-times);
    ret = fs_seek(fd, offset, SEEK_SET);
    if(ret != offset){
        TRACE("seek %s err:%d(%d)\n", cyc_name, ret, offset);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"seek %s err:%d(%d)\n", cyc_name, ret, offset);
        fs_write_err("seek %s err:%d(%d)\n", cyc_name, ret, offset);
        goto write_err;
    }
    ret = fs_truncate(fd);
    if(ret != offset){
        TRACE("trun %s err:%d(%d)\n", cyc_name, ret, offset);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"trun %s err:%d(%d)\n", cyc_name, ret, offset);
        fs_write_err("trun %s err:%d(%d)\n", cyc_name, ret, offset);
        goto write_err;
    }
    ret = fs_close(fd);
    if(ret != 0){
        TRACE("close %s err:%d\n", cyc_name, ret);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"close %s err:%d\n", cyc_name, ret);
        fs_write_err("close %s err:%d\n", cyc_name, ret);
        return -1;
    }
    ret = fs_remove(cyc_name);
    if(ret != 0){
        TRACE("remove %s err:%d\n", cyc_name, ret);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"remove %s err:%d\n", cyc_name, ret);
        fs_write_err("remove %s err:%d\n", cyc_name, ret);
        return -1;
    }
    return 0;
write_err:
    fs_close(fd);
    return -1;
}

int fs_set_count()
{
//    TRACE("fs_set_count\n");
    int fd, ret;
    fd = fs_open(count_name, O_CREAT|O_RDWR);
    if(fd < 0){
        TRACE("open %s err:%d\n", count_name, fd);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"open %s err:%d\n", count_name, fd);
        fs_write_err("open %s err:%d\n", count_name, fd);
        return fd;
    }
    ret = fs_write(fd, buf_data, reset_count);
    if(ret != reset_count){
        TRACE("write %s err:%d(%d)\n", count_name, ret, reset_count);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"write %s err:%d(%d)\n", count_name, ret, reset_count);
        fs_write_err("write %s err:%d(%d)\n", count_name, ret, reset_count);
        goto write_err;
    }
    ret = fs_close(fd);
    if(ret != 0){
        TRACE("close %s err:%d\n", count_name, ret);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"close %s err:%d\n", count_name, ret);
        fs_write_err("close %s err:%d\n", count_name, ret);
        return -1;
    }
    ret = fs_size(count_name);
    if(ret != reset_count){
        TRACE("size %s err:%d(%d)\n", count_name, ret, reset_count);
        lcd_Display(0,8,FONT_SIZE12|DISP_MEDIACY|DISP_INVCHAR,"size %s err:%d(%d)\n", count_name, ret, reset_count);
        fs_write_err("size %s err:%d(%d)\n", count_name, ret, reset_count);
        return -1;
    }
    return 0;
write_err:
    fs_close(fd);
    return -1;
}

int reset_file_test()
{
    int ret, count, times;
    reset_count = 0;
    ret = fs_reset_count();
    if(ret != 0){
        return ret; 
    }
    count = WRITE_COUNT;
    times = EXCHANGE_COUNT;
    ret = fs_cycle_write(count);
    if(ret != 0){
        return ret;
    }
    ret = fs_exchange_data(times);
    if(ret != 0){
        return ret;
    }
    ret = memcmp_truncate(count, times);
    if(ret != 0){
        return ret;
    }
    ret = fs_set_count();
    if(ret != 0){
        return ret;
    }
    TRACE("reset_file_test ok!\n");
    TRACE("reset_count:%d\n", reset_count);
    lcd_Display(0,FONT_SIZE12,FONT_SIZE12|DISP_CLRLINE,"reset_file_test ok!\n");
    lcd_Display(0,FONT_SIZE12*2,FONT_SIZE12|DISP_CLRLINE,"reset_count:%d\n", reset_count);
    s_DelayMs(1000);
    if(reset_count < 365*5){
//        do_Reset();
    }
    return 0;
}
#endif
void flash_debug(void)
{
    uint8_t ch;
//    uint8_t ret;
//    uint8_t buffer[2048];
    uint8_t buffer[2048];
    uint32_t flash_addr;
    uint32_t i;
    uint32_t number=0;


    hw_flash_init();
    while (1) {
        TRACE("\n1-Erase sector");
        TRACE("\n2-Write sector");
        TRACE("\n3-Read sector");
        flash_addr = FLASH_SECTOR_SIZE * FLASH_SECTOR_127;

        ch = InkeyCount(0);
        TRACE("\nFlash addr:%d %02X",flash_addr,flash_addr);
        switch (ch)
        {
        case 1:
            TRACE("\nerase--inter the sector:%d",flash_addr);
            flash_addr = InkeyCount(0);
            TRACE("\nret:%d",flash_app_erasedata(flash_addr));
            break;
        case 2:
            TRACE("\n write--inter the addr:%d",flash_addr);
            flash_addr = InkeyCount(0);
            TRACE("\n write--inter the num");
            number = InkeyCount(0);
            for ( i = 0 ; i<number ; i++ ) {
                buffer[i] = (uchar)i;
            }
            TRACE("\n write ret:%d",flash_app_writedata(flash_addr,number,buffer));
            break;
        case 3:
            TRACE("\nread--inter the addr:%d",flash_addr);
            flash_addr = InkeyCount(0);
            TRACE("\n read--inter the num");
            number = InkeyCount(0);
            memset(buffer,1,number);
            TRACE("\nret:%d",flash_app_readdata(flash_addr,number,buffer));
            DISPBUF("flash data",number,0,buffer);
            break;
        case 4:
            TRACE("\n write--inter the addr:%d",flash_addr);
            flash_addr = InkeyCount(0);
            TRACE("\n write--inter the num");
            number = InkeyCount(0);
            for ( i = 0 ; i<number ; i++ ) {
                buffer[i] = (uchar)i;
            }
            flash_write(flash_addr,number,buffer);
            break;
        case 99:
            return;
        default :
            break;
        }
    }

}

#ifdef CFG_DBG_RFID
#define Dprintk(...)  TRACE(__VA_ARGS__)
int fac_poll_rfid_test(void);
void fac_circle_rfid_test(void);
int rfid_capability_test(void);
typedef struct
{
	int cmdlen;
	char *cmd;
} ET_IC_CMD;
uchar gTypeBmodulation = AS3911_TYPEB_MODULATION;
int reg27hflg = 0x00;
//unsigned int guiDebugi=0,guiDebugj=0,guiDebugk=0;
//unsigned int guiDebugFlg=1;
//unsigned char gcDebugBuf[1024];
//unsigned char gucDebugm=0,gucDebugn=0;
uchar EI_mifs_ucBeginProtect(DevHandle * phDevHandle, uchar ucMode);
void EI_mifs_vEndProtect(void);
int rfid_pwrtest(int mode)
{
	int i,j,k,ret,flg = 0;
	uint cardtype,len,type;
    int key;
    uchar buf[256];
    char Track[3][256];
    uchar result,ucVal;
    uchar ucATQA[4];
	char *card_type [] = {
	"TYPE A",
	" ",
	" ",
	" ",
	" ",
	"TYPE B",
	};
    const char *rfid_card_string [] = {
        "S50卡",
        "S70卡",
        "Pro卡",
        "ProS50卡",
        "ProS70卡",
        "TypeB卡",
        "未知卡"
    };
    const ET_IC_CMD rfidcheckvaule[] = {
        {20,"\x00\xa4\x04\x00\x0e\x32\x50\x41\x59\x2e\x53\x59\x53\x2e\x44\x44\x46\x30\x31\x00"},
        {14,"\x00\xa4\x04\x00\x08\xa0\x00\x00\x03\x33\x01\x01\x06\x00"},
        {5,"\x80\xca\x9f\x79\x00"},
    };

	lcd_cls();
//    MODULE_SYS_INFO tsysinfo;
//    s_sysinfo_sysinfo(0,&tsysinfo);
//    Dprintk("\r\n ----menu: %d  %d  %x  %x",gwp30SysMemory.SysCtrl.bit.language,gwp30SysMemory.SysCtrl.bit.keytone,tsysinfo.language,tsysinfo.keytone);
    Dprintk("\r\ntest rfid module:%x----open:%d ",sys_get_module_type(MODULE_RF_TYPE),rfid_open(0));
    flg = flg;
	while(1)
	{
        Dprintk("\r\n ----------rfid pwr test---------:%d  pwr:%x",EG_mifs_tWorkInfo.RFIDModule,gtRfidDebugInfo.CW_A);
        Dprintk("\r\n 1-poll  2-exchange  3-rc531 set  4-as3911 set  5-wupa  6-wupb");
        Dprintk("\r\n 7-set fix TypbValue  8-check 20h  9-set fix Moudle");
        Dprintk("\r\n 10-3 cards check  11-PN512  12-test bankcard");
        Dprintk("\r\n 13-api test  14-rfid step test 15-   16-set rfid para");
//        lcd_display(0,0*FONT_SIZE8,DISP_CLRLINE | FONT_SIZE8 ,"ooOO00:%d  %x",EG_mifs_tWorkInfo.RFIDModule, gtRfidDebugInfo.CW_A);
//        lcd_display(0,1*FONT_SIZE8,DISP_CLRLINE | FONT_SIZE12,"oooOOO000");
//        lcd_display(0,20,DISP_CLRLINE | FONT_SIZE16,"oooOOO000");
//        lcd_display(0,32,DISP_CLRLINE | FONT_SIZE24,"oooOOO000");

		key = InkeyCount(0);
        EI_mifs_ucBeginProtect(0, 3);
		switch(key)
        {
        case 0:
            break;
        case 1:
//            fac_poll_rfid_test();
            rfid_ResetField(0);
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                s_DelayMs(500);
                ret = rfid_poll(RFID_MODE_EMV,(uint *)&type);
                if ( !ret ) {
                    Dprintk("\r\n type:%x",type);
                    if ( type != EM_mifs_TYPEBCARD ) {
                        Dprintk("\r\n ATQA:%x  %x",EG_mifs_tWorkInfo.ucATQA[0],EG_mifs_tWorkInfo.ucATQA[1]);
                    }
                    sys_beep();
                }
            }
            break;
        case 2:
            rfid_open(0);
//            fac_circle_rfid_test();
            lcd_cls();
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                s_DelayMs(800);
                ret = rfid_poll(RFID_MODE_EMV,&type);
                if(!ret)
                {
                    TRACE("寻卡:[%d]",card_type[type]);
                    TRACE("上电:");
                    ret = rfid_powerup(type,(uint *)&j,buf);
                    if(!ret)
                    {
                        TRACE("上电:成功 %s",rfid_card_string[buf[j-1]]);
                        TRACE("读卡...");
                        if(buf[j-1] >= EM_mifs_PROCARD)
                        {
                            for ( i = 0 ; i<5 ; i++ ) {
                                ret = rfid_exchangedata(sizeof(gApduGet4rand), (uchar *)gApduGet4rand,(uint *)&j,buf);
                                if(ret == 0 || ret == EM_mifs_SWDIFF)
                                {
                                    TRACE("读卡:成功[%02X-%02X]",buf[j-2],buf[j-1]);
                                    sys_beep_pro(BEEP_PWM_HZ,30,YES);
                                }
                                else
                                {
                                    TRACE("读卡:失败[%d]",ret);
                                }
                            }
                        }
                    }
                    else
                    {
                        TRACE("上电:失败[%d]",ret);
                    }
                    rfid_powerdown();
                }
                else
                {
                    TRACE("寻卡:失败[%d]",ret);
                }
            }
//            rfid_close();
            break;
        case 3:
            if (  EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531  ) {
                Dprintk("\r\n set the rc531 power type a:%d",gtRfidDebugInfo.CW_A);
                gtRfidDebugInfo.CW_A = InkeyCount(0);
                Dprintk("\r\n set the rc531 power type b");
                gtRfidDebugInfo.CW_B = InkeyCount(0);
            }
            break;
        case 4:
            if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module  //AS3911
                as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
                Dprintk("\r\n\r\n set the 3911 power  bit0-bit7  27h:%x",ucVal);
                ret = InkeyHex(0);
reg27hflg = 1;
gtRfidDebugInfo.CW_A = ret;

                as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, (uchar)ret); //根据硬件调节0x9F
                as3911ModifyRegister(AS3911_REG_AM_MOD_DEPTH_CONF, 0x7E, 0x1E);
                as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
                as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
                as3911ExecuteCommand(AS3911_CMD_CALIBRATE_MODULATION);
                ret = 0;
                as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 200, (ulong *)&ret);
                if ( ret != AS3911_IRQ_MASK_DCT) {
                    Dprintk("\r\n\r\n set the 3911 power fail");
                }
#endif
            }
            break;
        case 5:
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                EI_mifs_vFlushFIFO();				// empty FIFO

                // 初始化相关变量
                EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEA;
                EG_mifs_tWorkInfo.ucMifActivateFlag = 0;

                CLRBUF(ucATQA);
                result = EI_paypass_ucWUPA(ucATQA);
                Dprintk("\r\n\r\n type a :%x",result);
                if ( !result ) {
                    sys_beep();
                    DISPBUF("atqa",2,0,ucATQA);
                    EI_paypass_vHALTA();
                }
                s_DelayMs(500);
            }
            break;
        case 6:
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                result = EI_paypass_ucWUPB(&buf[0], buf+1);
                Dprintk("\r\n\r\n type b :%x",result);
                if ( !result ) {
                    sys_beep();
                    DISPBUF("atqa",2,0,ucATQA);
                }
                s_DelayMs(500);
            }
            break;
        case 7:
            Dprintk("\r\n\r\n input the count");
            gTypeBmodulation = InkeyCount(0);
            break;
        case 8:
            as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
            as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
            as3911ExecuteCommand(AS3911_CMD_MEASURE_AMPLITUDE);
            as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 20, (ulong *)&j);
            Dprintk("\r\n\r\n interrupt flag  :%x",j);
            if (j != AS3911_IRQ_MASK_DCT)
            {
                /* ToDo: enter debug code here. */
                Dprintk("\r\n\r\n ---------get the interrupt status error-------  :%x",j);
            }
            as3911DisableInterrupts(AS3911_IRQ_MASK_DCT);
            as3911ReadRegister(AS3911_REG_AD_RESULT, &ucVal);
            Dprintk("\r\n\r\n reg 20h  :%x   %d",ucVal, ucVal);
            break;
        case 9:
            as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONF, 0x80);
            emvHalSetAs3911TypeBModulationMode(AS3911_MODULATION_LEVEL_FROM_AMPLITUDE, buf);
            break;
        case 10:
            icc_InitIccModule();
            icc_InitModule(USERCARD, VCC_5,ISOMODE);
            mag_open();
            Dprintk("\r\n rfid open:%d",rfid_open(0));
            Dprintk("\r\n enter the count");
            len = InkeyCount(0);
            i = 0;
            while ( 1 ) {

                if ( IfInkey(0) ) {
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }
                if ( i ) {
                    i++;
                }
                Dprintk("\r\n count:%d",i);
                if(mag_check(0) == 0){
                    Dprintk("\r\n mag check ok");
                    ret = mag_read(Track[0], Track[1], Track[2]);
                    Dprintk("\r\n mag ret:%x",ret);
                    DISPBUF("track1",(ret>>8&0xFF),2,Track[0]);
                    DISPBUF("track2",(ret>>16&0xFF),2,Track[1]);
                    DISPBUF("track3",(ret>>24&0xFF),2,Track[2]);
                    sys_beep();
                    i = 0;
                    rfid_powerdown();
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }

                if ( icc_CheckInSlot(USERCARD) == ICC_SUCCESS) {
                    j = 0;
                    ret = icc_Reset(USERCARD,(uint *)&j,buf);
                    Dprintk("\r\n icc :%d",ret);
                    if ( ret == 0 ) {
                        DISPBUF("ic reset",j,0,buf);
                        sys_beep();
                    }
                    if ( InkeyCount(0) == 1 ) {
                        break;
                    }
                }

                if ( (i == 0) || (i > len) ) {
                    if ( i == 0 ) {
                        k = rfid_poll(RFID_MODE_ISO, (uint *)&j);
                    }
                    Dprintk("\r\n rfid :%d",k);
                    if ( (k == 0) && (i == 0)) {
                        i = 1;
                    }
                    if ( i > len ) {
                        rfid_powerup(j,(uint *)&k,buf);
                        DISPBUF("powerup",k,0,buf);
                        ret = rfid_exchangedata(5,(uchar *)"\x00\x84\x00\x00\x04",(uint *)&k,buf);
                        if ( ret == 0 ) {
                            DISPBUF("apdu",k,0,buf);
                            sys_beep();
                        }
                        i = 0;
                        rfid_powerdown();
                        if ( InkeyCount(0) == 1 ) {
                            break;
                        }
                    }
                }
            }
            break;
        case 11:
//            TI_PN512ModuleTest();
            break;
        case 12:
            Dprintk("\r\n 查询余额");

            Dprintk("\r\n0-ISO  1-EMV");
//            if ( InkeyCount(0) == 1 ) {
//                key = RFID_MODE_EMV|RFID_MODE_CID;
//            }else
                key = RFID_MODE_EMV;

            INIT_DEBUGBUF();
            guiDebugFlg = 0;
            while ( 1 ) {
//                Dprintk("\r\n\r\n apdu ");
                if ( IfInkey(0) ) {
                    if ( InkeyCount(0) == 1 ) {
                        Dprintk("\r\n ----卡下电:%d",rfid_powerdown());
                        break;
                    }
                }
                ret = rfid_poll(key,&cardtype);  //RFIiD_MODE_EMV
                if(ret)
                {
                    Dprintk("\r\n\r\n 寻卡失败 %d",ret);
                    TRACE("失败");
//                    guiDebugFlg = 0;
//                    DISPBUF(gcDebugBuf, guiDebugi, 0);
//                    vDispBufKey(guiDebugi,gcDebugBuf);
                }
                else
                {
                    Dprintk("\r\n\r\n 寻卡成功 %d",ret);
                    TRACE("成功");
//                    sys_beep();
                    CLRBUF(buf);
                    len = 0;
                    guiDebugFlg = 1;
                    INIT_DEBUGBUF();
                    ret = rfid_powerup(cardtype,&len,buf);
                    guiDebugFlg = 0;
                    if(ret){
                        Dprintk("\r\n\r\n 上电失败 %d",ret);
                        DISPBUF("powerup",guiDebugi,0,gcDebugBuf);
                    }else{
                        Dprintk("\r\n\r\n 上电成功 %d",ret);
//                        sys_beep();
                        DISPBUF("powerup",len,0,buf);
                        guiDebugFlg = 1;
                        guiDebugi = 0;
                        for ( i = 0 ; i < 3 ; i++ ) {
                            CLRBUF(buf);
                            len = 0;
                            ret = rfid_exchangedata(rfidcheckvaule[i].cmdlen,(uchar *)rfidcheckvaule[i].cmd,&len,buf);
                            if(ret){
                                Dprintk("\r\n\r\n apdu失败 :%d  %d",ret,i);
                                break;
                            }else{
                                Dprintk("\r\n\r\n apdu成功 %d",i);
                                DISPBUF("apdu",len,0,buf);
                            }
                        }
                        if ( i == 3 ) {
                            sys_beep();
                            Dprintk("\r\n\r\n 查询余额成功 %d",i);
                            InkeyCount(0);
                        }
                        guiDebugFlg = 0;
                        DISP_DEBUGBUF();
                    }

                    ret = rfid_powerdown();
                    if(ret){
                        Dprintk("\r\n\r\n 卡下电失败 %d",ret);
                    }else{
                        Dprintk("\r\n\r\n 卡下电成功 %d",ret);
//                        sys_beep();
                    }
//                    DISPBUF(gcDebugBuf,guiDebugi,0);
                }
//                s_DelayMs(1500);
            }
            guiDebugi = 0;
            guiDebugFlg = 0;

            break;
        case 13:
//            ET_Rfid_Test(0);
            break;
        case 14:
//            rfid_apitimetest(0);
            break;
        case 15:
//            while ( 1 ) {
//                if ( IfInkey(0) ) {
//                    if ( InkeyCount(0) == 1 ) {
//                        break;
//                    }
//                }
////            disable_dev_irq(SYSC_ID);
//                i = prn_get_temperature();
////            enable_dev_irq(SYSC_ID);
//                Dprintk("\r\n\r\n tmp: %d",i);
//                if ( i > 40 ) {
//                    Dprintk("\r\n\r\n 温度异常: %d",i);
//                    if ( InkeyCount(0) == 1 ) {
//                        break;
//                    }
//                }
//            }
            break;
        case 16:
            rfid_capability_test();
            break;
        case 98:
//            vTest_Print();
            break;
        case 99:
            EI_mifs_vEndProtect();
            return 0;
        default:
            break;
        }
	}
}

int rfid_capability_test(void)
{
	int key,i,j,ret,flg = 0;
    uint type;
    uchar buf[128];
    uchar result,ucVal;
    uchar ucATQA[4];
    MODULE_RFID_INFO rfidinfo;
	char *card_type [] = {
	"TYPE A",
	" ",
	" ",
	" ",
	" ",
	"TYPE B",
	};
    const char *rfid_card_string [] = {
        "S50卡",
        "S70卡",
        "Pro卡",
        "ProS50卡",
        "ProS70卡",
        "TypeB卡",
        "未知卡"
    };

//	lcd_cls();
    flg = flg;
	while(1)
	{
        Dprintk("\r\n rfid module:%d  27H:%x",EG_mifs_tWorkInfo.RFIDModule,gtRfidDebugInfo.CW_A);
        Dprintk("\r\n 1-poll        5-rc531 field set  ");
        Dprintk("\r\n 2-exchange    6-as3911 field set    ");
        Dprintk("\r\n 3-wupa        7-set fix Typbmodulation     ");
        Dprintk("\r\n 4-wupb        8-check 20h   ");
        Dprintk("\r\n               9-set typeA/B receive");
        Dprintk("\r\n               10-display all reg");
        Dprintk("\r\n               11-carrier on/off");
        Dprintk("\r\n               12-load the para");
        Dprintk("\r\n               13-save the para");
        Dprintk("\r\n               14-set all register");
//        lcd_display(0,0*FONT_SIZE8,DISP_CLRLINE | FONT_SIZE8 ,"ooOO00:%d  %x",EG_mifs_tWorkInfo.RFIDModule, gtRfidDebugInfo.CW_A);
//        lcd_display(0,1*FONT_SIZE8,DISP_CLRLINE | FONT_SIZE12,"oooOOO000");
//        lcd_display(0,20,DISP_CLRLINE | FONT_SIZE16,"oooOOO000");
//        lcd_display(0,32,DISP_CLRLINE | FONT_SIZE24,"oooOOO000");

		key = InkeyCount(0);
        EI_mifs_ucBeginProtect(0, 3);
		switch(key)
        {
        case 0:
            break;
        case 1:
            rfid_ResetField(0);
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                ret = rfid_poll(RFID_MODE_EMV,(uint *)&type);
                s_DelayUs(100);
                if ( !ret ) {
                    TRACE("\r\n type:%x",type);
                    if ( type != EM_mifs_TYPEBCARD ) {
                        TRACE("\r\n ATQA:%x  %x",EG_mifs_tWorkInfo.ucATQA[0],EG_mifs_tWorkInfo.ucATQA[1]);
                    }
                }
            }
            break;
        case 2:
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                ret = rfid_poll(RFID_MODE_EMV,&type);
                if(!ret)
                {
                    TRACE("\r\n寻卡:[%s]",card_type[type]);
                    TRACE("\r\n上电:");
                    ret = rfid_powerup(type,(uint *)&j,buf);
                    if(!ret)
                    {
                        TRACE("\r\n上电:成功 %s",rfid_card_string[buf[j-1]]);
                        TRACE("\r\n读卡...");
                        if(buf[j-1] >= EM_mifs_PROCARD)
                        {
                            ret = rfid_exchangedata(sizeof(gApduGet4rand), (uchar *)gApduGet4rand,(uint *)&j,buf);
                            if(ret == 0 || ret == EM_mifs_SWDIFF)
                            {
                                TRACE("\r\n读卡:成功[%02X-%02X]",buf[j-2],buf[j-1]);
                                sys_beep_pro(2000,30,YES);
                            }
                            else
                            {
                                TRACE("\r\n读卡:失败[%d]",ret);
                            }
                        }
                    }
                    else
                    {
                        TRACE("\r\n上电:失败[%d]",ret);
                    }
                    rfid_powerdown();
                    s_DelayMs(300);
                }
                else
                {
                    TRACE("\r\n寻卡:失败[%d]",ret);
                }
            }
            break;
        case 3:
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                EI_mifs_vFlushFIFO();				// empty FIFO

                // 初始化相关变量
                EG_mifs_tWorkInfo.ucCurType = EM_mifs_TYPEA;
                EG_mifs_tWorkInfo.ucMifActivateFlag = 0;

                CLRBUF(ucATQA);
                result = EI_paypass_ucWUPA(ucATQA);
                Dprintk("\r\n\r\n type a :%x",result);
                if ( !result ) {
                    sys_beep();
                    DISPBUF("atqa",2,0,ucATQA);
//                    DISPBUF(ucATQA,2,0);
                    EI_paypass_vHALTA();
                }
                s_DelayMs(500);
            }
            break;
        case 4:
            while ( 1 ) {
                if ( IfInkey(0) ) {
                    break;
                }
                result = EI_paypass_ucWUPB(&buf[0], buf+1);
                Dprintk("\r\n\r\n type b :%x",result);
                if ( !result ) {
                    sys_beep();
                    DISPBUF("atqb",buf[0],0,buf+1);
//                    DISPBUF(buf+1,buf[0],0);
                }
                s_DelayMs(500);
            }
            break;
        case 5:
            if (  EG_mifs_tWorkInfo.RFIDModule == RFID_Module_RC531  ) {
                Dprintk("\r\n set the rc531 power type a:%d",gtRfidDebugInfo.CW_A);
                gtRfidDebugInfo.CW_A = InkeyCount(0);
                Dprintk("\r\n set the rc531 power type b");
                gtRfidDebugInfo.CW_B = InkeyCount(0);
            }
            break;
        case 6:
            if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911 ) {
#ifdef EM_AS3911_Module  //AS3911
                as3911ReadRegister(AS3911_REG_RFO_AM_OFF_LEVEL, &ucVal);
                Dprintk("\r\n\r\n set the 3911 power  bit0-bit7  27h:%x",ucVal);
                ret = InkeyHex(0);
reg27hflg = 1;
gtRfidDebugInfo.CW_A = ret;

                as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, (uchar)ret); //根据硬件调节0x9F
//                as3911ModifyRegister(AS3911_REG_AM_MOD_DEPTH_CONF, 0x7E, 0x1E);
//                as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
//                as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
//                as3911ExecuteCommand(AS3911_CMD_CALIBRATE_MODULATION);
//                ret = 0;
//                as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 200, (ulong *)&ret);
//                if ( ret != AS3911_IRQ_MASK_DCT) {
//                    Dprintk("\r\n\r\n set the 3911 power fail");
//                }
#endif
            }
            break;
        case 7:
            Dprintk("\r\n\r\n input the count");
            gTypeBmodulation = InkeyCount(0);
            as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONF, 0x80);//固定type b调制深度
            as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, gTypeBmodulation);
            break;
        case 8:
            as3911ClearInterrupts(AS3911_IRQ_MASK_DCT);
            as3911EnableInterrupts(AS3911_IRQ_MASK_DCT);
            as3911ExecuteCommand(AS3911_CMD_MEASURE_AMPLITUDE);
            as3911WaitForInterruptTimed(AS3911_IRQ_MASK_DCT, 20, (ulong *)&j);
            Dprintk("\r\n\r\n interrupt flag  :%x",j);
            if (j != AS3911_IRQ_MASK_DCT)
            {
                /* ToDo: enter debug code here. */
                Dprintk("\r\n\r\n ---------get the interrupt status error-------  :%x",j);
            }
            as3911DisableInterrupts(AS3911_IRQ_MASK_DCT);
            as3911ReadRegister(AS3911_REG_AD_RESULT, &ucVal);
            Dprintk("\r\n\r\n reg 20h  :%x   %d",ucVal, ucVal);
            break;
        case 9:
            Dprintk("\r\n\r\n set typeA/B receive type A:%x   type B:%x",gtRfidProInfo.gTypeArec, gtRfidProInfo.gTypeBrec);
            Dprintk("\r\n 7=fcH 0=00H 1=24H 2=48H 3=6CH 4=90H 5=B4H 6=d8");
            Dprintk("\r\n set type A");
            gtRfidProInfo.gTypeArec = InkeyHex(0);
            Dprintk("\r\n set type B");
            gtRfidProInfo.gTypeBrec = InkeyHex(0);
            Dprintk("\r\n\r\n  type A:%x   type B:%x",gtRfidProInfo.gTypeArec, gtRfidProInfo.gTypeBrec);
            break;
        case 10:
            if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911) {
                for (i = 0; i < 0x3D; i++){
                    as3911ReadRegister((uchar)i, &ucVal);
                    Dprintk("REG: 0x%02x: 0x%02x  %02d\r\n", i, ucVal,ucVal);
                }
                //            displayRegisterValue(i);
            }
            else if (  EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512 ) {
                for (i = 0; i < 0x3F; i++){
                    EI_mifs_vReadReg(1, (uchar)i, &ucVal);
                    Dprintk("REG:%02X: 0x%02X  %02d\r\n ",(uchar)i,ucVal,ucVal);
                }
            }
//            displayRegisterValue(i);
            break;
        case 11:
            TRACE(0, DISP_FONT_LINE0, DISP_FONT|DISP_CLRLINE,"Carrier On/off  1-off  2-on");
            Dprintk("\r\n\r\n  Carrier on/off  1-off  2-on");
            i = InkeyCount(0);
            if ( i == 1 ) {
                emvHalActivateField(OFF);
            }else{
                emvHalActivateField(ON);
            }
            break;
        case 12:
            Dprintk("\r\n\r\n load the para");
            memset((uchar*)&rfidinfo, 0xFF, sizeof(MODULE_RFID_INFO));
            s_sysinfo_rfidinfo(0, &rfidinfo);
            gTypeBmodulation = rfidinfo.rfu[0];
            gtRfidProInfo.gTypeArec        = rfidinfo.rfu[1];
            gtRfidProInfo.gTypeBrec        = rfidinfo.rfu[2];
            gtRfidDebugInfo.CW_A      = rfidinfo.irfu[0];
            as3911WriteRegister(AS3911_REG_RFO_AM_OFF_LEVEL, gtRfidDebugInfo.CW_A);
            as3911WriteRegister(AS3911_REG_AM_MOD_DEPTH_CONF, 0x80);//固定type b调制深度
            as3911WriteRegister(AS3911_REG_RFO_AM_ON_LEVEL, gTypeBmodulation);
            Dprintk("\r\n\r\n gTypeBmodulation:%d",gTypeBmodulation);
            Dprintk("\r\n\r\n gTypeArec:%x",gtRfidProInfo.gTypeArec);
            Dprintk("\r\n\r\n gTypeBrec:%x",gtRfidProInfo.gTypeBrec);
            Dprintk("\r\n\r\n gtRfidDebugInfo.CW_A:%x",gtRfidDebugInfo.CW_A);
            break;
        case 13:
            Dprintk("\r\n\r\n save the para");
            Dprintk("\r\n\r\n gTypeBmodulation:%d",gTypeBmodulation);
            Dprintk("\r\n\r\n gTypeArec:%x",gtRfidProInfo.gTypeArec);
            Dprintk("\r\n\r\n gTypeBrec:%x",gtRfidProInfo.gTypeBrec);
            Dprintk("\r\n\r\n gtRfidDebugInfo.CW_A:%x",gtRfidDebugInfo.CW_A);
            memset((uchar*)&rfidinfo, 0xFF, sizeof(MODULE_RFID_INFO));
            s_sysinfo_rfidinfo(0, &rfidinfo);
            rfidinfo.rfu[0] = gTypeBmodulation;
            rfidinfo.rfu[1] = gtRfidProInfo.gTypeArec;
            rfidinfo.rfu[2] = gtRfidProInfo.gTypeBrec;
            rfidinfo.irfu[0] = gtRfidDebugInfo.CW_A;
            s_sysinfo_rfidinfo(1, &rfidinfo);
            break;
        case 14:
            Dprintk("\r\n\r\n -----set the registers -----");
            Dprintk("\r\n\r\n -----input the count -----");
            if ( EG_mifs_tWorkInfo.RFIDModule == RFID_Module_AS3911) {
                j = InkeyCount(0);
                for ( i=0 ; i<j ; i++  ) {
                    Dprintk("\r\n add:");
                    buf[i] = InkeyHex(0);
                    Dprintk("\r\n value:");
                    ucVal = InkeyHex(0);
                    as3911WriteRegister(buf[i], ucVal);
                }

                for ( i=0 ; i<j ; i++  ) {
                    as3911ReadRegister(buf[i], &ucVal);
                    Dprintk("\r\n add:%02Xh:%02X",buf[i],ucVal);
                }
            }
            else if (  EG_mifs_tWorkInfo.RFIDModule == RFID_Module_PN512 ) {
                j = InkeyCount(0);
                for ( i=0 ; i<j ; i++  ) {
                    Dprintk("\r\n add:");
                    buf[i] = InkeyHex(0);
                    Dprintk("\r\n value:");
                    ucVal = InkeyHex(0);
                    EI_mifs_vWriteReg(1, buf[i], &ucVal);
                }
                for ( i=0 ; i<j ; i++  ) {
                    EI_mifs_vReadReg(1, buf[i], &ucVal);
                    Dprintk("\r\n add:%02Xh:%02X",buf[i],ucVal);
                }
            }
            break;
        case 15:
            Dprintk("\r\n add:%02Xh",GsNOffReg);
            gcMifReg.GsNOff = InkeyHex(0);
            Dprintk("\r\n add:%02Xh",RFCfgReg);
            gcMifReg.RFCfg = InkeyHex(0);
            Dprintk("\r\n add:%02Xh",GsNOnReg);
            gcMifReg.GsNOn = InkeyHex(0);
            Dprintk("\r\n add:%02Xh",CWGsPReg);
            gcMifReg.CWGsP = InkeyHex(0);
            Dprintk("\r\n add:%02Xh",ModGsPReg);
            gcMifReg.ModGsP = InkeyHex(0);
            break;
        case 98:
//            vTest_Print();
            break;
        case 99:
            return 0;
        default:
            break;
        }
	}
}
#endif
void malloc_debug(void)
{
    int key,i,j,k;
//    uchar buf[32];
    uchar *ptr1;
    uchar *ptr2;
    uchar *ptr3;
    uchar *ptr;

    lcd_cls();
    while (1) {
        TRACE("\r\n-|*****************wp30 ctrl debug**********************|-");
        TRACE("\r\n-|-------core %02dMHz periph %05dKHz mcg %05dKHz--------|-",core_clk_mhz,periph_clk_khz,mcg_clk_khz);
        TRACE("\r\n-|1-malloc 2-free 3-disp 4-clean    |-");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            ptr1 = NULL;
            ptr2 = NULL;
            ptr3 = NULL;

            TRACE("\n\n----ptr1 size");
            i = InkeyCount(0);
            TRACE("\n\n----ptr2 size");
            j = InkeyCount(0);
            TRACE("\n\n----ptr3 size");
            k = InkeyCount(0);
            ptr1 = malloc(i);
            ptr2 = malloc(j);
            ptr3 = malloc(k);
            memset(ptr1,1,i);
            memset(ptr2,2,j);
            memset(ptr3,3,k);
            TRACE("\n\nptr1:%p  \n\nptr2:%p  \n\nptr3:%p\n\n",ptr1,ptr2,ptr3);
            break;
        case 2:
            free(ptr1);
            free(ptr2);
            free(ptr3);
            break;
        case 3:
            ptr = (uchar *)0x1FFFA000;
            DISPBUF("heap",1024,0,ptr);
            break;
        case 4:
            ptr = (uchar *)0x1FFFA000;
            memset(ptr,0,8192);
            break;
        case 99:
            return;
        default :
            break;
        }
    }
}
void sysinfo_debug(void)
{
    int key,i;
    uchar buf[FLASH_SECTOR_SIZE];

    TRACE("\nSA_SYSZONE:%d",SA_SYSZONE);
    TRACE("\nSA_SYSZONE_BACK:%d",SA_SYSZONE_BACK);
    TRACE("\nAPP_CHECK:%d",OFFSET_APP_CHECK);
    TRACE("\nFIRST_FLAG:%d",OFFSET_FIRST_FLAG);
    TRACE("\nappenable :%d",OFFSET_APPENABLE_FLAG);
    TRACE("\nbps :%d",OFFSET_BPS);
    TRACE("\nbeep :%d",OFFSET_BEEP);
    TRACE("\nlcd :%d",OFFSET_LCD);
    TRACE("\nattack :%d",OFFSET_ATTACKFLG);
    TRACE("\npsw :%d",SYS_SUPER_PSW_ADDR);
    TRACE("\npswA :%d",SYS_ADMINA_PSW_ADDR);
    TRACE("\npswB:%d",SYS_ADMINB_PSW_ADDR);
    TRACE("\nsn :%d",SYS_SN_DEV_ADDR);
    TRACE("\nfskhost :%d",SYS_FSK_host_ADDR);
    TRACE("\nrfid :%d",SYS_RFIDINFO_ADDR);
    TRACE("\nrful :%d",OFFSET_Reserved1);
    TRACE("\ncheck :%d",OFFSET_SYSZONE_CHECK);
    TRACE("\nend :%d",OFFSET_SYSZONE_END);
    lcd_cls();
    while (1) {
        TRACE("\r\n-|*****************wp30 ctrl debug**********************|-");
        TRACE("\r\n-|-------core %02dMHz periph %05dKHz mcg %05dKHz--------|-",core_clk_mhz,periph_clk_khz,mcg_clk_khz);
        TRACE("\r\n-|1-first run 2-check 3-clean 4-set    |-");
        TRACE("\r\n-|5-app first run 6-sn   7-getVersion |-");
        key = InkeyCount(0);
        switch (key)
        {
        case 1:
            TRACE("\n\nsys run:%d\n\n",sys_start());
            break;
        case 2:
            memset(buf,0,sizeof(buf));
            memcpy(buf,(void *)(SA_SYSZONE),sizeof(buf));
            DISPBUF("sys",sizeof(buf),0,buf);
            memcpy(buf,(void *)(SA_SYSZONE_BACK),sizeof(buf));
            DISPBUF("sysback",sizeof(buf),0,buf);
            break;
        case 3:
            TRACE("\n\n0-sys 1-sysback\n\n");
            if ( InkeyCount(0) == 0 ) {
                i = SA_SYSZONE;
            }else
                i = SA_SYSZONE_BACK;
            TRACE("\n\n---erase:%d\n\n",drv_flash_SectorErase(i));
            break;
        case 4:
            TRACE("\n\nset 0-sys 1-sysback\n\n");
            memset(buf,0x01,sizeof(buf));
            if ( InkeyCount(0) == 0 ) {
                i = SA_SYSZONE;
                TRACE("\n\n---erase:%d\n\n",drv_flash_SectorErase(i));
                drv_flash_write(SA_SYSZONE,(uint32_t *)buf,FLASH_SECTOR_SIZE);
            }else{
                i = SA_SYSZONE_BACK;
                TRACE("\n\n---erase:%d\n\n",drv_flash_SectorErase(i));
                drv_flash_write(SA_SYSZONE_BACK,(uint32_t *)buf,FLASH_SECTOR_SIZE);
            }
            break;
        case 5:
            TRACE("\n\nset 0-read 1-read and set\n\n");
            if ( InkeyCount(0) == 0 ) {
                TRACE("\n\n---first:%d",sys_FirstRunExp());
            }else
                TRACE("\n\n---first and set:%d",sys_FirstRun());

            break;
        case 6:
            TRACE("\n\nset 0-read 1-write\n\n");
            if ( InkeyCount(0) == 0 ) {
                memset(buf,0x01,sizeof(buf));
                sys_ReadSn_new(buf);
                DISPBUF("sn",32,0,buf);
            }else{
                for ( i = 0 ; i < SYS_SN_DEV_SIZE ; i++ ) {
                    buf[i] = 0x30+i%10;
                }
                TRACE("\n\nwrite sn:%d\n\n",sys_WriteSn_new(buf));
            }
            break;
        case 7:
            sys_check_version(0);
            break;
        case 8:
//            sys_set_hardware_info(0);
            break;
        case 99:
            return;
        default :
            break;
        }
    }
}
int wp30_serial_test(int mode)
{
    uchar ucData;
    hw_led_on(LED_ALL);
    sys_beep();
    console_clear();
    while ( 1 ) {
        if(console_read_buffer(&ucData,1,-1) > 0)
        {
            if(ucData == 0x0D) {
                ucData = 0x0A;
                out_char(ucData);
                break;
            } else if(ucData <= '9' && ucData >= '0') {
                out_char(ucData);
            } else if(ucData == 0x1b) {
                return 0;
            } else if(ucData == 0x08) {
                ucData = '\\';
                out_char(ucData);
            }else{
            
            }
        }
    }
    return 0;
}

#ifdef CFG_FS
void spiflash_debug(void)
{
    void dbg_exflash(void);
    int key,i,fd,flg;
//    uchar buf[32];

    flg = 1;
    while (1) {
        TRACE("\r\n-|*****************wp30 ctrl debug**********************|-");
        TRACE("\r\n-|-------core %02dMHz periph %05dKHz mcg %05dKHz--------|-",core_clk_mhz,periph_clk_khz,mcg_clk_khz);
        TRACE("\r\n-|1-exflash 2-open 3-seek 4-write 5-read 6-init  7-format 8-fs test  |-");
        if ( flg == 0 ) {
            key = 8;
            flg = 1;
        }else
            key = InkeyCount(0);
        switch (key)
        {
        case 1:
            dbg_exflash();
            break;
        case 2:
            fd = fs_open("debugtest",(O_RDWR | O_CREAT));
            TRACE("\r\n open:%d",fd);
            break;
        case 3:
            fs_seek(fd,0,SEEK_SET);
            break;
        case 4:
            TRACE("\r\n write data");
            i = InkeyCount(0);
            key = fs_write(fd,&i,4);
            TRACE("\r\n write ret:%d",key);
            break;
        case 5:
            TRACE("\r\n read data");
            key = fs_read(fd,&i,4);
            TRACE("\r\n read ret:%d   i:%d",key,i);
            break;
        case 6:
            fs_init();
            break;
        case 7:
            fs_format();
            break;
        case 8:
//            console_close();
            kb_flush();
            fs_init();
            lcd_Cls();
            lcd_Display(0,FONT_SIZE12*0,FONT_SIZE12|DISP_CLRLINE,"fs test");
            while ( 1 ) {
                if ( kb_hit() ) {
                    kb_flush();
                    lcd_Cls();
                    lcd_Display(0,FONT_SIZE12*4,FONT_SIZE12|DISP_CLRLINE,"k1-format");
                    if ( kb_getkey(-1) == KEY1 ) {
                        fs_format();
                        fs_init();
                        break;
                    }
                }
                reset_file_test();
            }
//            console_init(USBD_CDC,gwp30SysBuf_c.console,CNL_BUFSIZE_C);
//            s_DelayMs(3000);
            break;
        case 99:
            return;
        default :
            break;
        }
    }

}
#endif

#ifdef CFG_DBG_TPRINTER
int prnsendflg = 0;
void test_tp_forward_step(void)
{
    static int direction = 0;
//    const uchar     prn_motor_phase[8] = {0x0a, 0x0e, 0x06, 0x07, 0x05, 0x0d, 0x09, 0x0b};
    PRN_POWER_DISCHARGE();
    STROBE_1_OFF();
    switch (direction & 0x03)
    {
        case 0:
            LATCH_HIGH();
            LATCH_LOW();
            s_DelayUs(1);
            LATCH_HIGH();
            prnsendflg = 1;
            s_DelayUs(2);

            MOTOR_PHASE_1A_HIGH();
            MOTOR_PHASE_1B_LOW();
            MOTOR_PHASE_2A_HIGH();
            MOTOR_PHASE_2B_LOW();
            break;
        case 1:
            MOTOR_PHASE_1A_HIGH();
            MOTOR_PHASE_1B_LOW();
            MOTOR_PHASE_2A_LOW();
            MOTOR_PHASE_2B_HIGH();
            break;
        case 2:
            LATCH_HIGH();
            LATCH_LOW();
            s_DelayUs(1);
            LATCH_HIGH();
            prnsendflg = 1;
            s_DelayUs(2);

            MOTOR_PHASE_1A_LOW();
            MOTOR_PHASE_1B_HIGH();
            MOTOR_PHASE_2A_LOW();
            MOTOR_PHASE_2B_HIGH();
            break;
        case 3:
            MOTOR_PHASE_1A_LOW();
            MOTOR_PHASE_1B_HIGH();
            MOTOR_PHASE_2A_HIGH();
            MOTOR_PHASE_2B_LOW();
            break;
    }
    STROBE_1_ON();
    direction++;
    PRN_POWER_CHARGE();
}
void test_prn_press(void)
{
    uchar tmp;
    uchar prndata[48];
//    tp_gpio_init();
//    tp_spi_int();
//    tp_ad_init();
    tp_timer_init(TPRINT_TIMER_ID, 1200, test_tp_forward_step);
    set_irq_priority (TPRINT_TIMER_IRQn, INT_PRI_TPRINTER);

    tmp = 0x80;
    memset(prndata,tmp,sizeof(prndata));
    tp_data_send(prndata, sizeof(prndata));
    LATCH_HIGH();
    LATCH_LOW();
    s_DelayUs(1);
    LATCH_HIGH();

    MOTOR_PWR_ON();
    tp_timer_start(TPRINT_TIMER_ID, 1200, test_tp_forward_step);

    while ( 1 ) {
        if ( IfInkey(0) ) {
            break;
        }
        if ( prnsendflg == 1 ) {
            prnsendflg = 0;
            tmp >>= 1;
            if ( tmp == 0 ) {
                tmp = 0x80;
            }
            memset(prndata,tmp,sizeof(prndata));
            tp_data_send(prndata, sizeof(prndata));
        }
        TRACE("\r\ntem:%d",tp_temp_value());
        TRACE("\r\npower:%d",tp_power_value());
        if ( tp_temp_value() > 60 ) {
            TRACE("\r\n---temperature:%d",tp_temp_value());
            break;
        }
    }
    STROBE_1_OFF();
    MOTOR_PWR_OFF();
    tp_timer_close(TPRINT_TIMER_ID, 1200, test_tp_forward_step);
}
void TPSelfTest_4(void);
void dbg_tprint(void)
{
    int i,key,flg = 0;
    uchar buf[2048];

    tp_init();
    while (1)
    {
        TRACE("\r\n-|**********************tprinter debug***********************|-");
        TRACE("\r\n-|1-press  2-adc 3-prnQR 4-打印黑块  5-    |-");
        TRACE("\r\n-|7-adc  8-加热时间  9-电机引脚测试        |-");
        TRACE("\r\n-|******************************************************|-\t");
        if ( flg == 1 ) {
            flg = 0;
            key = 5;
        }else
            key = InkeyCount(0);
        switch (key)
        {
        case 1:
            test_prn_press();
            break;
        case 2:
//            tp_ad_init();
            while ( 1 ) {
                TRACE("\r\n");
                TRACE("\r\n---paper:%d",tp_paper_ad_value());
                TRACE("\r\n---temperature:%d",tp_temp_value());
                TRACE("\r\n---power:%d",tp_power_value());
                if ( InkeyCount(0) != 0 ) {
                    break;
                }
            }
            break;
        case 3:
            TPSelfTest_4();
            break;
        case 4:
            memset(buf,0xFF,sizeof(buf));
            for ( key = 0; key < 24 ; key++ ) {
                tp_print_line(buf);
            }
            break;
        case 5:
            tp_timer_init(TPRINT_TIMER_ID, 1200, test_tp_forward_step);
            set_irq_priority (TPRINT_TIMER_IRQn, INT_PRI_TPRINTER);

            memset(buf,0x80,sizeof(buf));
            tp_data_send(buf, sizeof(buf));
            LATCH_HIGH();
            LATCH_LOW();
            s_DelayUs(1);
            LATCH_HIGH();
            MOTOR_PWR_ON();

            while ( 1 ) {
                tp_timer_start(TPRINT_TIMER_ID, 1200, test_tp_forward_step);
                s_DelayMs(100);
                TRACE("\r\n---power:%d",tp_power_value());
                tp_timer_close(TPRINT_TIMER_ID, 1200, test_tp_forward_step);
                STROBE_1_OFF();
                s_DelayMs(2000);
                if ( IfInkey(0) ) {
                    break;
                }
            }
            MOTOR_PWR_OFF();
            tp_timer_close(TPRINT_TIMER_ID, 1200, test_tp_forward_step);
            break;
        case 6:
            TRACE("\r\n---serial prnter");
            console_clear();
            while ( 1 ) {
                if(console_read_buffer(buf,1536,-1) == 1536)
                {
                    key = 0;
                    for ( i = 0 ; i < 1536 ; i++ ) {
                        key ^= buf[i];
                    }
                    if ( key ) {
                        break;
                    }
                    for(i = 0; i < 1536/48; i++) {
                        tp_print_line(buf+i*48);
                    }
                }
            }
            break;
        case 7:
            for ( i = 0; i < 10; i++) {
                PRN_POWER_CHARGE();
                s_DelayUs(600);
                PRN_POWER_DISCHARGE();
                s_DelayUs(600);
            }
            PRN_POWER_CHARGE();
            TRACE("\r\ntp value:%d",tp_power_value());
            break; 
        case 8:
            
          //  TRACE("\r\n加热时间：%d",tp_test_count);
        //    for (i = 0; i < tp_test_count; i++) {

        //        TRACE("\r\n%d",tp_test_buf[i]);
          //  }
         //   tp_test_count = 0;

            break;
        case 9:
           
//            gpio_set_output(TP_WORK_PTxy,GPIO_OUTPUT_SlewRateFast,0);
//           
//            MOTOR_PWR_ON();
//            while(1)
//            {
//            boardtest_prn_pwr_strobe(0);          //打开激励
//            boardtest_set_pio_output(TP_MnA_PTxy, 1); //51
//            boardtest_set_pio_output(TP_MA_PTxy, 1);  //52
//            boardtest_set_pio_output(TP_MnB_PTxy, 0);//53
//            boardtest_set_pio_output(TP_MB_PTxy, 0);  //54
//            
//                if(InkeyCount(0) == 99)
//                    break;
//            }

            break;
        case 10:
//            TRACE("\r\nVPR: 0-low 1-hight");
//            if (InkeyCount(0) ==0)
//            {
//                //boardtest_set_pio_output(TP_MnA_PTxy, 0); //51
//                boardtest_set_pio_output(TP_MA_PTxy, 0);  //52
//                boardtest_set_pio_output(TP_MnB_PTxy, 0);//53
//                boardtest_set_pio_output(TP_MB_PTxy, 0);  //54
//            }else
//            {
//                //boardtest_set_pio_output(TP_MnA_PTxy, 0); //51
//                boardtest_set_pio_output(TP_MA_PTxy, 0);  //52
//                boardtest_set_pio_output(TP_MnB_PTxy, 0);//53
//                boardtest_set_pio_output(TP_MB_PTxy, 0);  //54
//            } 
            break;
        case 11:
            while(1)
            {
                break;
            }
            break;
        case 97:
            TRACE("\n--tprinter init");
            tp_init();
            break;
        case 98:
            TRACE("\n--tprinter close");
            tp_close();
            break;
        case 99:
            return;
        default:
            break;
        }
    }
}
#endif
void dbg_des(void)
{
    int key,flg = 0;
    uchar ucIBuf[32];
    uchar ucOBuf[32];
    uchar keybuf[32];
    int keylen = 16;

    memset(ucIBuf,0,sizeof(ucIBuf));
    memset(ucOBuf,0,sizeof(ucOBuf));
    memset(keybuf,0,sizeof(keybuf));
    while (1)
    {
        TRACE("\r\n-|**********************tprinter debug***********************|-");
        TRACE("\r\n-|1-encrypt  2-deencrypt 3- 4-  5-    |-");
        TRACE("\r\n-|******************************************************|-\t");
        if ( flg == 1 ) {
            flg = 0;
            key = 5;
        }else
            key = InkeyCount(0);
        switch (key)
        {
        case 1:
            des_encrypt_ecb(ucIBuf, ucOBuf, 8, keybuf, keylen/8);	
            DISPBUF("des encrypt", 32, 0, ucOBuf);
            break;
        case 2:
            des_decrypt_ecb(ucOBuf, ucIBuf, 8, keybuf, keylen/8);	
            DISPBUF("des decrypt", 32, 0, ucIBuf);
            break;
        case 3:
            memset(keybuf,0,sizeof(keybuf));
            RNG_FillRandom(keybuf, keylen);
            DISPBUF("des key", 16, 0, keybuf);
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 99:
            return;
        default:
            break;
        }
    }
}

extern volatile UINT64 gl_char_wait_etu;
extern void vTest_Icc(void);
extern int sm_selftest(void);
int test_cpucard(int mode);
void dbg_test(void)
{
    int i,key,flg = 0;
//    uchar buf[32];
    char cvalue;
    int ivalue;
//    uchar *pswbuf[10] = {
//        "00000000",
//        "11111111",
//        "22222222",
//        "33333333",
//        "44444444",
//        "55555555",
//        "66666666",
//        "77777777",
//        "88888888",
//        "99999999"
//    };
//    uchar track[3][192];

    SETSIGNAL1_L();
    SETSIGNAL2_L();
    power_5v_open();
    lcd_SetLight(LIGHT_ON_MODE);
    icc_InitModule(USERCARD, VCC_5, ISOMODE);
    icc_InitModule(SAM1SLOT, VCC_5, ISOMODE);
    while (1) {
        TRACE("\r\n-|*****************cf wp30 ctrl debug**********************|-:%d",kb_getkey(0));
        TRACE("\r\n-|-------core %02dMHz periph %05dKHz mcg %05dKHz--------|-",core_clk_mhz,periph_clk_khz,mcg_clk_khz);
        TRACE("\r\n-|1-rfid 2-flash 3-malloc 4-lcd 5-sysinfo 6-spiflash 7-lowpower |-");
        TRACE("\r\n-|8-ic   9-powerdown 10-tprintr 11-delayUs 12-delayMs 13-IS8U256A |-");
        TRACE("\r\n-|14-des 15-emv                                                      |-");
        TRACE("\r\n-|                                                      |-");
        TRACE("\r\n-|                                                      |-");
        TRACE("\r\n-|                                                      |-");
        TRACE("\r\n-|******************************************************|-\t");
        if ( flg == 1 ) {
            flg = 0;
            key = 15;
        }else
            key = InkeyCount(0);
//            kb_getstr(0x25,1,6,-1,(char *)buf);
//            value = (uint)atoi((char const *)buf);
        switch (key)
        {
        case 1:
#ifdef CFG_DBG_RFID
            rfid_pwrtest(0);
#endif
            break;
        case 2:
            flash_debug();
            break;
        case 3:
            malloc_debug();
            break;
        case 4:
//            memset(&gLcdSys.dram[0][0],0xB5,sizeof(gLcdSys.dram[0])*8);
            i = InkeyHex(0);
            memset(&gLcdSys.dram[0][0],i,sizeof(gLcdSys.dram[0])*8);
            TRACE("\r\n--size:%d",sizeof(gLcdSys.dram[0]));
            for (i=0; i<LCD_MAX_PAGE; i++)
            {
                hw_lcd_set_page(i);
                hw_lcd_set_column(0);
                hw_lcd_write_data(gLcdSys.dram[i],LCD_LENGTH);
            }
            DISPBUF("lcd",sizeof(gLcdSys.dram[0])*8,0,&gLcdSys.dram[0][0]);
            break;
        case 5:
            sysinfo_debug();
            break;
        case 6:
            cvalue = -1;
            ivalue = cvalue;
            TRACE("\n\n--cvalue:%d  ivalue:%d",cvalue,ivalue);
            TRACE("\n\n--cvalue:%x  ivalue:%x",(int)cvalue,ivalue);

#ifdef CFG_FS
            spiflash_debug();
#endif
            break;
        case 7:
#ifdef CFG_USBD_CDC
            usbdserial_Close();
            sys_SleepWaitForEvent(EVENT_KEYPRESS,1,0);
            usbdserial_Open();
            lcd_cls();
            lcd_Display(0,DISP_FONT_LINE0,DISP_FONT|DISP_MEDIACY|DISP_CLRLINE,"退出低功耗");
            i = kb_getkey(-1);

//            key = gSystem.lpwr.periph;
//            gSystem.lpwr.periph = 0;
//            sys_SleepWaitForEvent(0,1,0);
//            gSystem.lpwr.periph = key;
//            lcd_cls();
//            lcd_Display(0,0*FONT_SIZE12, DISP_FONT|DISP_CLRLINE, "lowpower test");
//            kb_getkey(-1);
#endif
            break;
        case 8:
          //  vTest_Icc();
            break;
        case 9:
//            if ( set_appenable(1) == 0 ) {
//                TRACE("\r\n enable app ok");
//            }
//            sys_Shutdown();

//            lcd_cls();
//            while(1)
//            {
//                i = kb_getkey(-1);
//                if ( i == KEY1 ) {
//                    power_keep_down();
//                }else if ( i == KEY2){
//                    NVIC_SystemReset();
//                }else
//                    break;
//                lcd_Display(0,0*FONT_SIZE12, DISP_FONT|DISP_CLRLINE, "status:%d",power_ifcharging());
//            } 


            //ic delay
//			gl_CardInfo.k_IccComErr = 0;
//			gl_CardInfo.k_timeover = 0;
//			gl_CardInfo.k_IccErrTimeOut = 0;
//            gl_char_wait_etu = 10081;
//            icc_start_etu_timer(372);
//            while ( gl_char_wait_etu ) {
//                s_DelayMs(10);
//                TRACE("\r\n :gl_char_wait_etu:%d",gl_char_wait_etu);
//                if ( IfInkey(0) ) {
//                    break;
//                }
//            }
//            TRACE("\r\n :%d  %d  %d gl_char_wait_etu:%d",gl_CardInfo.k_IccComErr,gl_CardInfo.k_timeover,gl_CardInfo.k_IccErrTimeOut,gl_char_wait_etu);
            break;
        case 10:
            dbg_tprint();
            break;
        case 11:
//            usbdserial_Printf("\r\n%d%2d%c%f123456\n789\n123\r1234\t12345",5,5,0x35,0.2);
            while ( 1 ) {
                TRACE("\r\n\r\n-|*****us******");
                i = InkeyCount(0);
                if ( i == 0 ) {
                    break;
                }
                LED_BLUE_GPIO->PSOR = 0x04;
                s_DelayUs(i);
                LED_BLUE_GPIO->PCOR = 0x04;
            }
            break;
        case 12:
//            usbdserial_Printf("\r\n%f",0.21);
            while ( 1 ) {
                TRACE("\r\n\r\n-|*****ms******");
                i = InkeyCount(0);
                if ( i == 0 ) {
                    break;
                }
                LED_BLUE_GPIO->PSOR = 0x04;
                s_DelayMs(i);
                LED_BLUE_GPIO->PCOR = 0x04;
            }
            break;
        case 13:
#if defined(CFG_DBG_SECURITY_CHIP)
            TRACE("\r\n\r\n-|****IS8U256A INIT:%d******\r\n",sc_is8u256a_init());
            TRACE("\r\n\r\n-|****ssl test:%d******\r\n",sm_selftest());
#endif
            break;
        case 14:
            dbg_des();
            break;
        case 15:
//            test_cpucard(0);
            break;
        case 16:
            break;
        case 98:
            break;
        case 99:
            return;
        default :
            break;
        }
    }
}

#endif

