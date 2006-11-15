/*
 * static char *rcsid_tod_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2003-2006 Mark Wedel & Crossfire Development Team
    Copyright (C) 2000 Tim Rightnour 
    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

#ifndef TOD_H
#define TOD_H

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
#define EQUATOR_BASE_TEMP	30	/* C */
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

/* This is a multiplier for the wind caused by pressure differences.
 * The type of overal climate you get depends on this.
 * Too little wind, and the rain hugs the coast.
 * Too much wind, and there are hurricanes and blizzards everywhere.
 * 1 is too little.
 */
#define WIND_FACTOR  4.0 

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

typedef struct _weather_avoids {
	const char *name;
	int snow;
	archetype *what; /*inited from name, faste to compare arch pointers than strings*/
} weather_avoids_t;

typedef struct _weather_replace {
	const char *tile;
	const char *special_snow;
	const char *doublestack_arch;
	int arch_or_name;
} weather_replace_t;

typedef struct _weather_grow {
	const char *herb; /* arch name of item to grow */
	const char *tile; /* arch tile to grow on, NULL if anything */
	int random; /* random factor.  min 1, higher = lower chance of
		     * appearance */
	float rfmin;
	float rfmax; /* rainfall min/max (inches/day) */
	int humin;
	int humax; /* humidity min/max */
	int tempmin;
	int tempmax; /* temp min/max */
	int elevmin;
	int elevmax; /* elevation min/max */
	int season; /* 0=any or 1-5 */
} weather_grow_t;

#endif /* TOD_H */
