#include <time.h>

#define MINUTE 	60
#define HOUR 	3600
#define DAY 	86400
#define YEAR 	31536000	// 365 * DAY

// Assum leap year
/*
static int kernel_month_old[12] = {
	0,
	DAY * (31),
	DAY * (31 + 29),
	DAY * (31 + 29 + 31),
	DAY * (31 + 29 + 31 + 30),
	DAY * (31 + 29 + 31 + 30 +31),
	DAY * (31 + 29 + 31 + 30 + 31 + 30),
	DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31),
	DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31),
	DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
	DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
	DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)
};
*/
// To make it faster
static int kernel_month[12] = {
	0, 2678400, 5184000, 7862400, 10454400, 13132800, 15724800, 
	18403200, 21081600, 23673600, 26352000, 28944000 };

// Make time
long kernel_mktime(struct tm *tm)
{
	long _res;
	int _year;

	_year = tm->tm_year - 70;
	_res = YEAR * _year + DAY * ((_year + 1) >> 2);

	_res += kernel_month[tm->tm_mon];
	// Common year
	(tm->tm_mon > 1 && ((_year + 2) % 4)) && (_res -= DAY);

	_res += DAY * (tm->tm_mday - 1);
	_res += HOUR * tm->tm_hour;
	_res += MINUTE * tm->tm_min;
	_res += tm->tm_sec;

	return _res;
}

long startup_time;
void init_time(void)
{
	struct tm time;

    // CMOS load speed is slow
	do {
		time.tm_sec = CMOS_READ(0);
		time.tm_min = CMOS_READ(2);
		time.tm_hour = CMOS_READ(4);
		time.tm_mday = CMOS_READ(7);
		time.tm_mon = CMOS_READ(8);
		time.tm_year = CMOS_READ(9);
	} while (time.tm_sec != CMOS_READ(0));
	BCD_TO_BIN(time.tm_sec);
	BCD_TO_BIN(time.tm_min);
	BCD_TO_BIN(time.tm_hour);
	BCD_TO_BIN(time.tm_mday);
	BCD_TO_BIN(time.tm_mon);
	BCD_TO_BIN(time.tm_year);
	time.tm_mon--;                              // tm_mon 中月份的范围是 0-11
	startup_time = kernel_mktime(&time);
}
