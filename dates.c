
#include "dates.h"

// finds # of days since J2000
int_least32_t DATE_days_since_J2000(time_t inputTime) {
	int_least32_t  daysSince = 0;
	struct tm* converted = gmtime(&inputTime);
	int_least32_t yearsSince = converted->tm_year - 2000;
	int leapdays = 0;

	// one leapday every 4 years as per Julian calendar
	daysSince += yearsSince/4;

	// 365 days/year on average
	daysSince += yearsSince * 365;

	// if the date is before 2000, we 
	if (yearsSince >= 0) {
		daysSince += converted->tm_yday;
	} else {
		daysSince -= converted->tm_yday;
	}

	return daysSince;
}


// not sure if this is really necessary, not familiar enough with C. Used for
// adding stuff to vectors.
void DATE_copy_tm(struct tm* orig, struct tm* target) {
	target->tm_sec = orig->tm_sec;
	target->tm_min = orig->tm_min;
	target->tm_hour = orig->tm_hour;
	target->tm_mday = orig->tm_mday;
	target->tm_mon = orig->tm_mon;
	target->tm_year = orig->tm_year;
	target->tm_wday = orig->tm_wday;
	target->tm_yday = orig->tm_yday;
	target->tm_isdst = orig->tm_isdst;
}

void DATE_add_time(int seconds, struct tm* date) {
	if (date == NULL) {
		return;
	}
	date->tm_sec += seconds;
	mktime(date);
}
