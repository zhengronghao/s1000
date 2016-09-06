/***************** (C) COPYRIGHT 2014 START Computer equipment *****************
 * File Name          : rtc-lib.c
 * bfief              : 
 * Author             : luocs()  
 * Email              : luocs@itep.com.cn
 * Version            : V0.00
 * Date               : 2/19/2014 8:11:41 PM
 * Description        : 
 *******************************************************************************/
#include "common.h"
#include "rtc-lib.h"

#define TM_YEAR_REFERENCE  1900
#define REFERENCE_YEAR   1970
#define CORRECT_DAYS    (337+(REFERENCE_YEAR-1)*365\
                            +(REFERENCE_YEAR-1)/4\
                            -(REFERENCE_YEAR-1)/100\
                            +(REFERENCE_YEAR-1)/400)
//REFERENCE_YEAR = 1970, CORRECT_DAYS = 719499

static const unsigned char rtc_days_in_month[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const unsigned short rtc_ydays[2][13] = {
	/* Normal years */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	/* Leap years */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

#define LEAPS_THRU_END_OF(y) ((y)/4 - (y)/100 + (y)/400)


/* Converts Gregorian date to seconds since REFERENCE_YEAR-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines where long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
unsigned long
mktime(const unsigned int year0, const unsigned int mon0,
       const unsigned int day, const unsigned int hour,
       const unsigned int min, const unsigned int sec)
{
	unsigned int mon = mon0, year = year0;

    TRACE("\n1900 CORRECT_DAYS is 719499 == %d",CORRECT_DAYS);
	/* 1..12 -> 11,12,1..10 */
	if (0 >= (int) (mon -= 2)) {
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}

	return ((((unsigned long)
		  (year/4 - year/100 + year/400 + 367*mon/12 + day) +
		  year*365 - CORRECT_DAYS
	    )*24 + hour /* now have hours */
	  )*60 + min /* now have minutes */
	)*60 + sec; /* finally seconds */
}


/*
 * The number of days in the month.
 */
int rtc_month_days(unsigned int month, unsigned int year)
{
	return rtc_days_in_month[month] + (is_leap_year(year) && month == 1);
}

/*
 * The number of days since January 1. (0 to 365)
 */
int rtc_year_days(unsigned int day, unsigned int month, unsigned int year)
{
	return rtc_ydays[is_leap_year(year)][month] + day-1;
}


/*
 * Convert seconds since 01-01-REFERENCE_YEAR 00:00:00 to Gregorian date.
 */
void rtc_time_to_tm(unsigned long time, struct rtc_time *tm)
{
	unsigned int month, year;
	int days;

	days = time / 86400;
	time -= (unsigned int) days * 86400;

	/* day of the week, REFERENCE_YEAR-01-01 was a Thursday */
	tm->tm_wday = (days + 4) % 7;

	year = REFERENCE_YEAR + days / 365;
	days -= (year - REFERENCE_YEAR) * 365
		+ LEAPS_THRU_END_OF(year - 1)
		- LEAPS_THRU_END_OF(REFERENCE_YEAR - 1);
	if (days < 0) {
		year -= 1;
		days += 365 + is_leap_year(year);
	}
	tm->tm_year = year - TM_YEAR_REFERENCE;
	tm->tm_yday = days + 1;

	for (month = 0; month < 11; month++) {
		int newdays;

		newdays = days - rtc_month_days(month, year);
		if (newdays < 0)
			break;
		days = newdays;
	}
	tm->tm_mon = month;
	tm->tm_mday = days + 1;

	tm->tm_hour = time / 3600;
	time -= tm->tm_hour * 3600;
	tm->tm_min = time / 60;
	tm->tm_sec = time - tm->tm_min * 60;

//	tm->tm_isdst = 0;
}
/*
 * Does the rtc_time represent a valid date/time?
 */
int rtc_valid_tm(struct rtc_time *tm)
{
	if (tm->tm_year < 70
		|| ((unsigned)tm->tm_mon) >= 12
		|| tm->tm_mday < 1
		|| tm->tm_mday > rtc_month_days(tm->tm_mon, tm->tm_year + TM_YEAR_REFERENCE)
		|| ((unsigned)tm->tm_hour) >= 24
		|| ((unsigned)tm->tm_min) >= 60
		|| ((unsigned)tm->tm_sec) >= 60)
		return -1;

	return 0;
}

/*
 * Convert Gregorian date to seconds since 01-01-REFERENCE_YEAR 00:00:00.
 */
int rtc_tm_to_time(struct rtc_time *tm, unsigned long *time)
{
	*time = mktime(tm->tm_year + TM_YEAR_REFERENCE, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);
	return 0;
}

/*
 * Convert rtc_time to ktime
// */
//ktime_t rtc_tm_to_ktime(struct rtc_time tm)
//{
//	time_t time;
//	rtc_tm_to_time(&tm, &time);
//	return ktime_set(time, 0);
//}

/*
 * Convert ktime to rtc_time
 */
//struct rtc_time rtc_ktime_to_tm(ktime_t kt)
//{
//	struct timespec ts;
//	struct rtc_time ret;
//
//	ts = ktime_to_timespec(kt);
//	/* Round up any ns */
//	if (ts.tv_nsec)
//		ts.tv_sec++;
//	rtc_time_to_tm(ts.tv_sec, &ret);
//	return ret;
//}



