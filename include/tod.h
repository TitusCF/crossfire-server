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

/**
 * @file
 * Defines for the ingame clock, ticks management and weather system.
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

/**
 * Represents the ingame time.
 */
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

/**
 * This is a multiplier for the wind caused by pressure differences.
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

/**
 * Defines a tile the weather system should avoid.
 */
typedef struct _weather_avoids {
	const char *name;   /**< Tile archetype name, used during initialisation only. */
	int snow;           /**< Is this snow? Used for various tests. */
	archetype *what;    /**< Inited from name, faster to compare arch pointers than strings. */
} weather_avoids_t;

/**
 * Defines a tile the weather system can change to another tile.
 */
typedef struct _weather_replace {
	const char *tile;               /**< Tile archetype name. */
	const char *special_snow;       /**< If set, this is some special snow. */
	const char *doublestack_arch;   /**< If set, this other archetype will be added. */
	int arch_or_name;               /**< If set, tile matches the archetype name, else the object's name. */
} weather_replace_t;

/**
 * Defines a tile where something can grow.
 */
typedef struct _weather_grow {
	const char *herb;   /**< Arch name of item to grow. */
	const char *tile;   /**< Arch tile to grow on, NULL if anything. */
	int random;         /**< Random apparition factor. Min 1, higher = lower chance of appearance. */
	float rfmin;        /**< Minimum rainfall for herb to grow (inches/day). */
	float rfmax;        /**< Maximum rainfall for herb to grow (inches/day). */
	int humin;          /**< Minimum humidity for herb to grow. */
	int humax;          /**< Maximum humidity for herb to grow. */
	int tempmin;        /**< Minimum temperature for herb to grow. */
	int tempmax;        /**< Maximum temperature for herb to grow. */
	int elevmin;        /**< Minimum elevation for herb to grow. */
	int elevmax;        /**< Maximum elevation for herb to grow. */
	int season;         /**< Season the herb can grow. 0=any or 1-5. */
} weather_grow_t;

#endif /* TOD_H */
