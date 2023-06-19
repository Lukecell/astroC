#include <time.h>
#include <stdint.h>

#ifndef dates_h
#define dates_h

int_least32_t DATE_days_since_J2000(time_t inputTime);

// not sure if this is really necessary, not familiar enough with C. Used for
// adding stuff to vectors.
void DATE_copy_tm(struct tm* orig, struct tm* target);

void DATE_add_time(int seconds, struct tm* date);

#endif