/* Copyright 2000 Tim Rightnour */

#ifndef _TOD_H_
#define _TOD_H_

#define PTICKS_PER_CLOCK	1500

/* game time */
#define HOURS_PER_DAY		28
#define DAYS_PER_WEEK		7
#define WEEKS_PER_MONTH		5
#define MONTHS_PER_YEAR		17

/* convenience */
#define WEEKS_PER_YEAR		(WEEKS_PER_MONTH*MONTHS_PER_YEAR)
#define DAYS_PER_MONTH		(DAYS_PER_WEEK*WEEKS_PER_MONTH)
#define DAYS_PER_YEAR		(DAYS_PER_MONTH*MONTHS_PER_YEAR)
#define HOURS_PER_WEEK		(HOURS_PER_DAY*DAYS_PER_WEEK)
#define HOURS_PER_MONTH		(HOURS_PER_WEEK*WEEKS_PER_MONTH)
#define HOURS_PER_YEAR		(HOURS_PER_MONTH*MONTHS_PER_YEAR)

#define LUNAR_DAYS		DAYS_PER_MONTH

typedef struct _timeofday {
	int year;
	int month;
	int day;
	int dayofweek;
	int hour;
	int minute;
	int weekofmonth;
	int season;
} timeofday_t;

/* from common/time.c */
extern void get_tod(timeofday_t *tod);

/* weather stuff */

#define POLAR_BASE_TEMP		0	/* C */
#define EQUATOR_BASE_TEMP	25	/* C */
#define SEASONAL_ADJUST		10	/* polar distance */
#define GULF_STREAM_WIDTH       3       /* width of gulf stream */
#define GULF_STREAM_BASE_SPEED  40      /* base speed of gulf stream */

/* don't muck with these unless you are sure you know what they do */
#define PRESSURE_ITERATIONS		30
#define PRESSURE_AREA			180
#define PRESSURE_ROUNDING_FACTOR	2
#define PRESSURE_ROUNDING_ITER		1
#define PRESSURE_SPIKES			3
#define PRESSURE_MAX			1040
#define PRESSURE_MIN			960

/* editing the below might require actual changes to code */
#define WEATHERMAPTILESX		100
#define WEATHERMAPTILESY		100

/* sky conditions */
#define SKY_CLEAR         0
#define SKY_LIGHTCLOUD    1
#define SKY_OVERCAST      2
#define SKY_LIGHT_RAIN    3
#define SKY_RAIN          4 /* rain -> storm has lightning */
#define SKY_HEAVY_RAIN    5
#define SKY_HURRICANE     6
/* wierd weather 7-12 */
#define SKY_FOG           7
#define SKY_HAIL          8
/* snow */
#define SKY_LIGHT_SNOW    13 /* add 10 to rain to get snow */
#define SKY_SNOW          14
#define SKY_HEAVY_SNOW    15
#define SKY_BLIZZARD      16

#endif /* _TOD_H_ */
