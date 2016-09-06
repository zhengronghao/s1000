/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : rtc-lib.h
 * bfief              : 
 * Author             : luocs() 
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/19/2014 8:12:01 PM
 * Description        : 
 *******************************************************************************/

#ifndef __RTC_LIB_H__
#define __RTC_LIB_H__
/*
 * The struct used to pass data via the following ioctl. Similar to the
 * struct tm in <time.h>, but it needs to be here so that the kernel
 * source is self contained, allowing cross-compiles, etc. etc.
 */

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday; //1~
	int tm_mon;  //0~11
	int tm_year; //年数(从现年到TM_YEAR_REFERENCE的年数)
	int tm_wday; //星期几
	int tm_yday; //一年的第几天
//	int tm_isdst;//夏令时
};

static inline uint32_t is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}


unsigned long
mktime(const unsigned int year0, const unsigned int mon0,
       const unsigned int day, const unsigned int hour,
       const unsigned int min, const unsigned int sec);

int rtc_month_days(unsigned int month, unsigned int year);
int rtc_year_days(unsigned int day, unsigned int month, unsigned int year);
void rtc_time_to_tm(unsigned long time, struct rtc_time *tm);
int rtc_valid_tm(struct rtc_time *tm);
int rtc_tm_to_time(struct rtc_time *tm, unsigned long *time);

#endif

