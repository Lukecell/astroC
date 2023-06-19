#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "vec.h"
#include "dates.h"

#define DEG_TO_RAD_FACTOR 0.0174532925
#define PI 3.14159265358979323846264338327950288419716939937510
// lower = more precise, but also slower.
#define PRECISION_CONSTANT 0.000119543099451666409378334984143055665304306293736

// global coordinate system
// in degrees
typedef struct  {
	double lat;
	double lon;
} GCS;

// equatorial coordinate system
// in degrees
typedef struct  {
	double ra;
	double dec;
} EqCS ;

struct celestial_object {
	char* name;
	EqCS coord;
};

struct observatory {
	char* name;
	GCS coord;
};

double deg_to_rad(double degrees) {
	return degrees * DEG_TO_RAD_FACTOR;
}

double find_altitude_rad(double sin_const, double cos_const, double hour_angle_rad) {
	return asin(sin_const + cos_const * cos(hour_angle_rad));
}

bool ever_visible_altitude(struct observatory obs, struct celestial_object obj, 
		double min_alt_deg) {
	double min_alt_rad = deg_to_rad(min_alt_deg);

	double lat_rad = deg_to_rad(obs.coord.lat);
	double lon_rad = deg_to_rad(obs.coord.lon);

	double ra_rad = deg_to_rad(obj.coord.ra);
	double dec_rad = deg_to_rad(obj.coord.dec);

	double const1 = sin(lat_rad) * sin(dec_rad);
	double const2 = cos(lat_rad) * cos(dec_rad);
	
	// here we use the altitude formula to see if it's EVER at/above the minimum
	// altitude
	if (asin( const1 ) >= min_alt_rad ||
		asin( const1 + const2 ) >= min_alt_rad ||
		asin( const1 - const2 ) >= min_alt_rad ) {

		return true;
	}
	
	return false;
}

vec_double find_valid_altitude_intervals(struct observatory obs, 
		struct celestial_object obj, double min_alt_deg) {
	double min_alt_rad = deg_to_rad(min_alt_deg);

	double lat_rad = deg_to_rad(obs.coord.lat);
	double lon_rad = deg_to_rad(obs.coord.lon);

	double ra_rad = deg_to_rad(obj.coord.ra);
	double dec_rad = deg_to_rad(obj.coord.dec);

	// to understand this, read the formula on finding the altitude from DEC/Latittude
	double sin_const = sin(lat_rad) * sin(dec_rad);
	double cos_const = cos(lat_rad) * cos(dec_rad);

	// return default if the object is never visible.
	if (! ever_visible_altitude(obs, obj, min_alt_deg)) {
		return NULL;
	}
	
	vec_double visible_time_intervals = vector_create();
	bool interval_started = false;
	// iterate from -pi to +pi, with 2pi/PRECISION_CONSTANT iterations total
	// to find the intervals where we can see the object
	for (double hour_angle_rad = PI * -1; hour_angle_rad <= PI; 
			hour_angle_rad += PRECISION_CONSTANT) {
		// here we use local apparent sidereal time for our calculations (hour_angle_rad + lon_rad)
		// but we save greenwich mean sidereal time in the intervals. this is
		// to help make later calculations simpler.
		// see here for more info on converting GMST to LAST: https://aa.usno.navy.mil/faq/GAST
		double alt_at_curr_time = 
			find_altitude_rad(sin_const, cos_const, hour_angle_rad + lon_rad);
		
		// if interval started but invalid altitude at time, end current interval
		if (interval_started && alt_at_curr_time < min_alt_rad) {
			vector_add(&visible_time_intervals, hour_angle_rad);
			interval_started = false;
		} // continue interval
		else if (interval_started && alt_at_curr_time >= min_alt_rad) {
			continue;
		}
		else if (!interval_started && alt_at_curr_time < min_alt_rad) {
			continue;
		} // start interval
		else if (!interval_started && alt_at_curr_time >= min_alt_rad) {
			vector_add(&visible_time_intervals, hour_angle_rad);
			interval_started = true;
		}
	}
	
	// if we have an open interval, we close it.
	if (interval_started) {
		vector_add(&visible_time_intervals, PI);
	}
	
	return visible_time_intervals;
}



/**
 * returns NULL if endTime < startTime
*/
vec_tm find_valid_sun_intervals(struct observatory obs, 
		struct celestial_object obj, time_t _startTime, time_t _endTime){
	
	// if difftime outputs a negative number, endTime < startTime, which is
	// an invalid input, meaning we return NULL
	if (difftime(_startTime, _endTime) <= 0.0) {
		return NULL;
	}



}

int main()
{
	GCS south_pole_coord = { .lat = 80.5, .lon = 2.53 };
	char* south_pole_name = "South Pole";
	struct observatory south_pole = {.coord = south_pole_coord, .name=south_pole_name};
	
	EqCS polaris_coord = {.dec = 80.5, .ra = 10};
	char *ptr;
	struct celestial_object polaris = {.coord = polaris_coord, .name=0};
	
	vec_double intervals = 
		find_valid_altitude_intervals(south_pole, polaris, (double) 89.5);
	
	if (intervals == NULL) {
		return 0;
	}
	
	for (long unsigned int i = 0; i < vector_size(intervals); i++) {
		printf("%f ", intervals[i]);
	}
	printf("%ld\n", vector_size(intervals));
	printf("%f", -1 * PI);
	
	return 0;
}