/*
 * static char *rcsid_weather_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
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

    The authors can be reached via e-mail to crossfire-devel@real-time.com
*/

#include <global.h>
#include <tod.h>
#include <map.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

extern unsigned long todtick;
extern weathermap_t **weathermap;

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

int gulf_stream_speed[GULF_STREAM_WIDTH][WEATHERMAPTILESY];
int gulf_stream_dir[GULF_STREAM_WIDTH][WEATHERMAPTILESY];
int gulf_stream_start;
int gulf_stream_direction;

const int season_timechange[5][HOURS_PER_DAY] = {
/* 0  1   2  3  4  5  6  7  8  9  10 11 12 13 14 1  2  3  4  5   6   7
	8    9  10  11  12  13 */
  {0, 0,  0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 1}, 
  {0, 0,  0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 0},
  {0, 0,  0, 0, 0, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 0},
  {0, 0,  0, 0, 0, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 0},
  {0, 0,  0, 0, 0, 0, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 0}
};

const int season_tempchange[HOURS_PER_DAY] = {
/* 0  1   2  3  4  5  6  7  8  9  10 11 12 13 14 1  2  3  4  5   6   7
	8    9  10  11  12  13 */
   0, 0,  0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 1};

void set_darkness_map(mapstruct *m)
{
    int i;
    timeofday_t tod;

    if (!m->outdoor)
	return;

    get_tod(&tod);
    m->darkness = 0;
    for (i = HOURS_PER_DAY/2; i < HOURS_PER_DAY; i++)
	change_map_light(m, season_timechange[tod.season][i]);
    for (i = 0; i <= tod.hour; i++)
	change_map_light(m, season_timechange[tod.season][i]);
}

void dawn_to_dusk(timeofday_t *tod)
{
    mapstruct *m;

    /* If the light level isn't changing, no reason to do all
     * the work below.
     */
    if (season_timechange[tod->season][tod->hour] == 0) return;

    for(m=first_map;m!=NULL;m=m->next) {
#ifndef MAP_RESET
	if (m->in_memory == MAP_SWAPPED)
	    continue;
#endif
	if (!m->outdoor)
	    continue;
	change_map_light(m, season_timechange[tod->season][tod->hour]);
    }
}

/*
 * This performs the basic function of advancing the clock one tick
 * forward.  Every 20 ticks, the clock is saved to disk.  It is also
 * saved on shutdown.  Any time dependant functions should be called
 * from this function, and probably be passed tod as an argument.
 * Please don't modify tod in the dependant function.
 */

void tick_the_clock()
{
    timeofday_t tod;

    todtick++;
    if (todtick%20 == 0)
	write_todclock();
    if (settings.dynamiclevel > 0) {
        if (todtick%21 == 0)
	    write_pressuremap();
	if (todtick%22 == 0)
	    write_winddirmap();
	if (todtick%23 == 0)
	    write_windspeedmap();
	if (todtick%24 == 0)
	    write_humidmap();
	if (todtick%25 == 0)
	    write_elevmap();
	if (todtick%26 == 0)
	    write_temperaturemap();
	if (todtick%27 == 0)
	    write_gulfstreammap();
	if (todtick%28 == 0 && settings.fastclock > 0)
	    write_skymap();
    }
    get_tod(&tod);
    dawn_to_dusk(&tod);
    /* call the weather calculators, here, in order */
    if (settings.dynamiclevel > 0) {
        perform_pressure();	/* pressure is the random factor */
        smooth_wind();	/* calculate the wind. depends on pressure */
	plot_gulfstream();
        update_humid();
        init_temperature();
	compute_sky();
    }
    /* perform_weather must follow calculators */
    perform_weather();
}

/*
 * This batch of routines reads and writes the various
 * weathermap structures.  Each type of data is stored
 * in a separate file to allow the size of these structures to be
 * changed more or less on the fly.  If weather goes haywire, the admin
 * can simply delete and boot the server, and it will regen.
 *
 * The write functions should be called occasionally to keep the data
 * in the maps current.  Whereas the read functions should only be called
 * at boot.  If the read function cannot find the appropriate map, it
 * calls the init function, to initialize that map.
 */

/* sky. We never read this map, only write it for debugging purposes */

void write_skymap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/skymap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++)
	    fprintf(fp, "%d ", weathermap[x][y].sky);
	fprintf(fp, "\n");
    }
    fclose(fp);
}

/* pressure */

void write_pressuremap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/pressuremap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++)
	    fprintf(fp, "%d ", weathermap[x][y].pressure);
	fprintf(fp, "\n");
    }
    fclose(fp);
}

void read_pressuremap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/pressuremap", settings.localdir);
    LOG(llevDebug, "Reading pressure data from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
	LOG(llevError, "Cannot open %s for reading\n", filename);
	LOG(llevDebug, "Initializing pressure maps...");
	init_pressure();
	write_pressuremap();
	LOG(llevDebug, "Done\n");
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    fscanf(fp, "%d ", &weathermap[x][y].pressure);
	    if (weathermap[x][y].pressure < 960 ||
		weathermap[x][y].pressure > 1040)
		weathermap[x][y].pressure = rndm(960, 1040);
	}
	fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

void init_pressure()
{
    int x, y;
    int l, n, k, r;

    for (x=0; x < WEATHERMAPTILESX; x++)
	for (y=0; y < WEATHERMAPTILESY; y++)
	    weathermap[x][y].pressure = 1000;

    for (l=0; l < PRESSURE_ITERATIONS; l++) {
	x = rndm(0, WEATHERMAPTILESX-1);
	y = rndm(0, WEATHERMAPTILESY-1);
	n = rndm(PRESSURE_MIN, PRESSURE_MAX);
	for (k=1; k < PRESSURE_AREA; k++) {
	    r = rndm(0,3);
	    switch (r) {
		case 0: if (x < WEATHERMAPTILESX-1) x++; break;
		case 1: if (y < WEATHERMAPTILESY-1) y++; break;
		case 2: if (x) x--; break;
		case 3: if (y) y--; break;
	    }
	    weathermap[x][y].pressure = (weathermap[x][y].pressure+n)/2;
	}
    }
    /* create random spikes in the pressure */
    for (l=0; l < PRESSURE_SPIKES; l++) {
	x = rndm(0, WEATHERMAPTILESX-1);
	y = rndm(0, WEATHERMAPTILESY-1);
	n = rndm(500, 2000);
	weathermap[x][y].pressure = n;
    }
    smooth_pressure();
}

/* winddir */

void write_winddirmap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/winddirmap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++)
	    fprintf(fp, "%d ", weathermap[x][y].winddir);
	fprintf(fp, "\n");
    }
    fclose(fp);
}

void read_winddirmap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/winddirmap", settings.localdir);
    LOG(llevDebug, "Reading wind direction data from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
	LOG(llevError, "Cannot open %s for reading\n", filename);
	LOG(llevDebug, "Initializing wind maps...");
	init_wind();
	write_winddirmap();
	LOG(llevDebug, "Done\n");
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    fscanf(fp, "%d ", &weathermap[x][y].winddir);
	    if (weathermap[x][y].winddir < 1 ||
		weathermap[x][y].winddir > 8)
		weathermap[x][y].winddir = rndm(1, 8);
	}
	fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/* windspeed */

void write_windspeedmap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/windspeedmap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++)
	    fprintf(fp, "%d ", weathermap[x][y].windspeed);
	fprintf(fp, "\n");
    }
    fclose(fp);
}

void read_windspeedmap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/windspeedmap", settings.localdir);
    LOG(llevDebug, "Reading wind speed data from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
	LOG(llevError, "Cannot open %s for reading\n", filename);
	LOG(llevDebug, "Initializing wind maps...");
	init_wind();
	write_windspeedmap();
	LOG(llevDebug, "Done\n");
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    fscanf(fp, "%d ", &weathermap[x][y].windspeed);
	    if (weathermap[x][y].windspeed < 0 ||
		weathermap[x][y].windspeed > 120)
		weathermap[x][y].windspeed = rndm(1, 30);
	}
	fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/* initialize the wind randomly. Does both direction and speed in one pass */

void init_wind()
{
    int x, y;

    for (x=0; x < WEATHERMAPTILESX; x++)
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    weathermap[x][y].winddir = rndm(1, 8);
	    weathermap[x][y].windspeed = rndm(1, 10);
	}
}

/* gulf stream */

void write_gulfstreammap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/gulfstreammap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }
    for (x=0; x < GULF_STREAM_WIDTH; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++)
	    fprintf(fp, "%d ", gulf_stream_speed[x][y]);
	fprintf(fp, "\n");
    }
    for (x=0; x < GULF_STREAM_WIDTH; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++)
	    fprintf(fp, "%d ", gulf_stream_dir[x][y]);
	fprintf(fp, "\n");
    }
    fclose(fp);
}

void read_gulfstreammap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/gulfstreammap", settings.localdir);
    LOG(llevDebug, "Reading gulf stream data from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
	LOG(llevError, "Cannot open %s for reading\n", filename);
	LOG(llevDebug, "Initializing gulf stream maps...");
	init_gulfstreammap();
	write_gulfstreammap();
	LOG(llevDebug, "Done\n");
	return;
    }
    for (x=0; x < GULF_STREAM_WIDTH; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    fscanf(fp, "%d ", &gulf_stream_speed[x][y]);
	    if (gulf_stream_speed[x][y] < 0 ||
		gulf_stream_speed[x][y] > 120)
		gulf_stream_speed[x][y] =
		    rndm(GULF_STREAM_BASE_SPEED, GULF_STREAM_BASE_SPEED+10);
	}
	fscanf(fp, "\n");
    }
    for (x=0; x < GULF_STREAM_WIDTH; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    fscanf(fp, "%d ", &gulf_stream_dir[x][y]);
	    if (gulf_stream_dir[x][y] < 0 ||
		gulf_stream_dir[x][y] > 120)
		gulf_stream_dir[x][y] = rndm(1, 8);
	}
	fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

void init_gulfstreammap()
{
    int x, y, tx;

    /* build a gulf stream */
    x = rndm(GULF_STREAM_WIDTH, WEATHERMAPTILESX-GULF_STREAM_WIDTH);
    /* doth the great bob inhale or exhale? */
    gulf_stream_direction = rndm(0, 1);
    gulf_stream_start = x;

    if (gulf_stream_direction) {
	for (y=WEATHERMAPTILESY-1; y >= 0; y--) {
	    switch(rndm(0, 6)) {
	    case 0:
	    case 1:
	    case 2:
		for (tx=0; tx < GULF_STREAM_WIDTH; tx++) {
		    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED,
			GULF_STREAM_BASE_SPEED+10);
		    if (x==0)
			gulf_stream_dir[tx][y] = 7;
		    else {
			gulf_stream_dir[tx][y] = 8;
			if (tx == 0)
			    x--;
		    }
		}
		break;
	    case 3:
		for (tx=0; tx < GULF_STREAM_WIDTH; tx++) {
		    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED,
			GULF_STREAM_BASE_SPEED+10);
		    gulf_stream_dir[tx][y] = 7;
		}
		break;
	    case 4:
	    case 5:
	    case 6:		
		for (tx=0; tx < GULF_STREAM_WIDTH; tx++) {
		    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED,
			GULF_STREAM_BASE_SPEED+10);
		    if (x==WEATHERMAPTILESX-1)
			gulf_stream_dir[tx][y] = 7;
		    else {
			gulf_stream_dir[tx][y] = 6;
			if (tx == 0)
			    x++;
		    }
		}
		break;
	    }
	}
    } else { /* go right to left */
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    switch(rndm(0, 6)) {
	    case 0:
	    case 1:
	    case 2:
		for (tx=0; tx < GULF_STREAM_WIDTH; tx++) {
		    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED,
			GULF_STREAM_BASE_SPEED+10);
		    if (x==0)
			gulf_stream_dir[tx][y] = 3;
		    else {
			gulf_stream_dir[tx][y] = 2;
			if (tx == 0)
			    x--;
		    }
		}
		break;
	    case 3:
		for (tx=0; tx < GULF_STREAM_WIDTH; tx++) {
		    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED,
			GULF_STREAM_BASE_SPEED+10);
		    gulf_stream_dir[tx][y] = 3;
		}
		break;
	    case 4:
	    case 5:
	    case 6:		
		for (tx=0; tx < GULF_STREAM_WIDTH; tx++) {
		    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED,
			GULF_STREAM_BASE_SPEED+10);
		    if (x==WEATHERMAPTILESX-1)
			gulf_stream_dir[tx][y] = 3;
		    else {
			gulf_stream_dir[tx][y] = 4;
			if (tx == 0)
			    x++;
		    }
		}
		break;
	    }
	}
    } /* done */
}

/* humidity */

void write_humidmap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/humidmap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++)
	    fprintf(fp, "%d ", weathermap[x][y].humid);
	fprintf(fp, "\n");
    }
    fclose(fp);
}

void read_humidmap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/humidmap", settings.localdir);
    LOG(llevDebug, "Reading humidity data from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
	LOG(llevError, "Cannot open %s for reading\n", filename);
	LOG(llevDebug, "Initializing humidity and elevation maps...");
	init_humid_elev();
	write_elevmap();
	write_humidmap();
	write_watermap();
	LOG(llevDebug, "Done\n");
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    fscanf(fp, "%d ", &weathermap[x][y].humid);
	    if (weathermap[x][y].humid < 0 ||
		weathermap[x][y].humid > 100)
		weathermap[x][y].humid = rndm(0, 100);
	}
	fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/* average elevation */

void write_elevmap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/elevmap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++)
	    fprintf(fp, "%d ", weathermap[x][y].avgelev);
	fprintf(fp, "\n");
    }
    fclose(fp);
}

void read_elevmap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/elevmap", settings.localdir);
    LOG(llevDebug, "Reading elevation data from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
	LOG(llevError, "Cannot open %s for reading\n", filename);
	/* initializing these is expensive, and should have been done
	   by the humidity.  It's not worth the wait to do it twice. */
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    fscanf(fp, "%d ", &weathermap[x][y].avgelev);
	    if (weathermap[x][y].avgelev < -10000 ||
		weathermap[x][y].avgelev > 15000)
		weathermap[x][y].avgelev = rndm(-1000, 10000);
	}
	fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/* water % */

void write_watermap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/watermap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++)
	    fprintf(fp, "%d ", weathermap[x][y].water);
	fprintf(fp, "\n");
    }
    fclose(fp);
}

void read_watermap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/watermap", settings.localdir);
    LOG(llevDebug, "Reading water data from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
	LOG(llevError, "Cannot open %s for reading\n", filename);
	/* initializing these is expensive, and should have been done
	   by the humidity.  It's not worth the wait to do it twice. */
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    fscanf(fp, "%d ", &weathermap[x][y].water);
	    if (weathermap[x][y].water > 100)
		weathermap[x][y].water = rndm(0, 100);
	}
	fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/* initialize both humidity and elevation */

void init_humid_elev()
{
    int x, y, tx, ty, nx, ny, ax, ay, j, k;
    int spwtx, spwty, dir;
    char *mapname;
    long int elev;
    int water, space;
    mapstruct *m;

    /* handling of this is kinda nasty.  For that reason,
     * we do the elevation here too.  Not because it makes the
     * code cleaner, or makes handling easier, but because I do *not*
     * want to maintain two of these nightmares.
     */

    spwtx = (settings.worldmaptilesx * settings.worldmaptilesizex) / WEATHERMAPTILESX;
    spwty = (settings.worldmaptilesy * settings.worldmaptilesizey) / WEATHERMAPTILESY;
    for (x=0; x < WEATHERMAPTILESX; x++) {
        for (y=0; y < WEATHERMAPTILESY; y++) {
	    water = 0;
	    elev = 0;
	    nx = 0;
	    ny = 0;
	    space = 0;

	    /* top left */
	    mapname=weathermap_to_worldmap_corner(x, y, &tx, &ty, 8);
	    m = load_original_map(mapname, 0);
	    if (m == NULL)
		continue;
	    m = load_overlay_map(mapname, m);
	    if (m == NULL)
		continue;
	    for (nx=0,ax=tx; (nx < spwtx && ax < settings.worldmaptilesizex &&
		     space < spwtx*spwty); ax++,nx++) {
		for (ny=0,ay=ty; (ny < spwty && ay < settings.worldmaptilesizey &&
			 space < spwtx*spwty);
		     ay++,ny++,space++) {
		    if (QUERY_FLAG(m->spaces[ax+ay].bottom,
			    FLAG_IS_WATER))
			water++;
		    elev += m->spaces[ax+ay].bottom->elevation;
		}
	    }
	    delete_map(m);

	    /* bottom left */
	    mapname=weathermap_to_worldmap_corner(x, y, &tx, &ty, 6);
	    m = load_original_map(mapname, 0);
	    if (m == NULL)
		continue;
	    m = load_overlay_map(mapname, m);
	    if (m == NULL)
		continue;
	    j = ny;
	    for (nx=0,ax=tx; (nx < spwtx && ax < settings.worldmaptilesizex &&
		     space < spwtx*spwty); ax++,nx++) {
		for (ny=j,ay=MAX(0, ty - (spwty-1)); (ny < spwty && ay <= ty &&
			 space < spwtx*spwty);
		     space++,ay++,ny++) {
		    if (QUERY_FLAG(m->spaces[ax+ay].bottom,
			    FLAG_IS_WATER))
			water++;
		    elev += m->spaces[ax+ay].bottom->elevation;
		}
	    }
	    delete_map(m);

	    /* top right */
	    mapname=weathermap_to_worldmap_corner(x, y, &tx, &ty, 2);
	    m = load_original_map(mapname, 0);
	    if (m == NULL)
		continue;
	    m = load_overlay_map(mapname, m);
	    if (m == NULL)
		continue;
	    for (ax=MAX(0, tx - (spwtx-1)); (nx < spwtx && ax < tx &&
		     space < spwtx*spwty); ax++,nx++) {
		for (ny=0,ay=ty; (ny < spwty && ay < settings.worldmaptilesizey &&
			 space < spwtx*spwty);
		     ay++,ny++,space++) {
		    if (QUERY_FLAG(m->spaces[ax+ay].bottom,
			    FLAG_IS_WATER))
			water++;
		    elev += m->spaces[ax+ay].bottom->elevation;
		}
	    }
	    delete_map(m);

	    /* bottom left */
	    mapname=weathermap_to_worldmap_corner(x, y, &tx, &ty, 4);
	    m = load_original_map(mapname, 0);
	    if (m == NULL)
		continue;
	    m = load_overlay_map(mapname, m);
	    if (m == NULL)
		continue;
	    for (nx=0,ax=MAX(0, tx - (spwtx-1)); (nx < spwtx && ax < tx &&
		     space < spwtx*spwty); ax++,nx++) {
		for (ny=0,ay=MAX(0, ty - (spwty-1)); (ny < spwty && ay <= ty &&
			 space < spwtx*spwty);
		     space++,ay++,ny++) {
		    if (QUERY_FLAG(m->spaces[ax+ay].bottom,
			    FLAG_IS_WATER))
			water++;
		    elev += m->spaces[ax+ay].bottom->elevation;
		}
	    }
	    delete_map(m);
	    /* jesus thats confusing as all hell */
	    printf("water %d humid %d\n", water, water*100/(spwtx*spwty));
	    weathermap[x][y].humid = water*100/(spwtx*spwty);
	    weathermap[x][y].avgelev = elev/(spwtx*spwty);
	    weathermap[x][y].water = weathermap[x][y].humid;
	}
    }

    /* and this does all the real work */
    for (x=0; x < WEATHERMAPTILESX; x++)
	for (y=0; y < WEATHERMAPTILESY; y++)
	    weathermap[x][y].humid = humid_tile(x, y);
}

/* temperature */

void write_temperaturemap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/temperaturemap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++)
	    fprintf(fp, "%d ", weathermap[x][y].temp);
	fprintf(fp, "\n");
    }
    fclose(fp);
}

void read_temperaturemap()
{
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    sprintf(filename, "%s/temperaturemap", settings.localdir);
    LOG(llevDebug, "Reading teperature data from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
	LOG(llevError, "Cannot open %s for reading\n", filename);
	init_temperature();
	write_temperaturemap();
	return;
    }
    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    fscanf(fp, "%d ", &weathermap[x][y].temp);
	    if (weathermap[x][y].temp < -30 ||
		weathermap[x][y].temp > 60)
		weathermap[x][y].temp = rndm(-10, 40);
	}
	fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

void init_temperature()
{
    int x, y;
    timeofday_t tod;

    get_tod(&tod);
    for (x=0; x < WEATHERMAPTILESX; x++)
	for (y=0; y < WEATHERMAPTILESY; y++)
	    temperature_calc(x, y, &tod);
}

/* END of read/write/init */


int wmperformstartx;
int wmperformstarty;

void init_weather()
{
    int x, y, tx, ty;
    int i, j;
    int water;
    long int tmp;
    char filename[MAX_BUF];
    FILE *fp;
    mapstruct *m;

    /* all this stuff needs to be set, otherwise this function will cause
     * chaos and destruction.
     */
    if (settings.dynamiclevel < 1)
	return;
    if (settings.worldmapstartx < 1 || settings.worldmapstarty < 1 ||
	settings.worldmaptilesx < 1 || settings.worldmaptilesy < 1 ||
	settings.worldmaptilesizex < 1 || settings.worldmaptilesizex < 1)
	return;

    LOG(llevDebug, "Initializing the weathermap...\n");

    weathermap = (weathermap_t **)malloc(sizeof(weathermap_t *) *
	WEATHERMAPTILESX);
    if (weathermap == NULL)
	fatal(OUT_OF_MEMORY);
    for (y=0; y < WEATHERMAPTILESY; y++) {
	weathermap[y] = (weathermap_t *)malloc(sizeof(weathermap_t) *
	    WEATHERMAPTILESY);
	if (weathermap[y] == NULL)
	    fatal(OUT_OF_MEMORY);
    }
    /* now we load the values in the big worldmap weather array */
    /* do not re-order these */
    read_pressuremap();
    read_winddirmap();
    read_windspeedmap();
    read_gulfstreammap();
    read_watermap();
    read_humidmap();
    read_elevmap(); /* elevation must allways follow humidity */
    read_temperaturemap();
    gulf_stream_direction = rndm(0, 1);
    for (tx=0; tx < GULF_STREAM_WIDTH; tx++)
	for (ty=0; ty < WEATHERMAPTILESY-1; ty++)
	    if (gulf_stream_direction)
		switch (gulf_stream_dir[tx][ty]) {
		case 2: gulf_stream_dir[tx][ty] = 6; break;
		case 3: gulf_stream_dir[tx][ty] = 7; break;
		case 4: gulf_stream_dir[tx][ty] = 8; break;
		}
	    else
		switch (gulf_stream_dir[tx][ty]) {
		case 6: gulf_stream_dir[tx][ty] = 2; break;
		case 7: gulf_stream_dir[tx][ty] = 3; break;
		case 8: gulf_stream_dir[tx][ty] = 4; break;
		}
    gulf_stream_start = rndm(GULF_STREAM_WIDTH, WEATHERMAPTILESY-GULF_STREAM_WIDTH);

    LOG(llevDebug, "Done reading weathermaps\n");
    sprintf(filename, "%s/wmapcurpos", settings.localdir);
    LOG(llevDebug, "Reading current weather position from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
	LOG(llevError, "Can't open %s.\n", filename);
	wmperformstartx = -1;
	return;
    }
    fscanf(fp, "%d %d", &wmperformstartx, &wmperformstarty);
    LOG(llevDebug, "curposx=%d curposy=%d\n", wmperformstartx, wmperformstarty);
    fclose(fp);
    if (wmperformstartx > settings.worldmaptilesx)
	wmperformstartx = -1;
    if (wmperformstarty > settings.worldmaptilesy)
	wmperformstarty = 0;
}

/*
 * This routine slowly loads the world, patches it up due to the weather,
 * and saves it back to disk.  In this way, the world constantly feels the
 * effects of weather uniformly, without relying on players wandering.
 *
 * The main point of this is stuff like growing herbs, soil, decaying crap,
 * etc etc etc.  Not actual *weather*, but weather *effects*.
 */

void perform_weather()
{
    mapstruct *m;
    char filename[MAX_BUF];
    FILE *fp;

    if (!settings.dynamiclevel)
	return;
    
    /* move right to left, top to bottom */
    if (wmperformstartx == settings.worldmaptilesx) {
	wmperformstartx = 0;
	wmperformstarty++;
    } else
	wmperformstartx++;
    if (wmperformstarty == settings.worldmaptilesy)
	wmperformstartx = wmperformstarty = 0;
    
    sprintf(filename, "world/world_%d_%d",
	wmperformstartx+settings.worldmapstartx,
	wmperformstarty+settings.worldmapstarty);

    m = ready_map_name(filename, 0);
    if (m == NULL)
	return; /* hrmm */
    
    /* for now, all we do is decay stuff.  more to come */
    decay_objects(m);
    new_save_map(m, 2); /* write the overlay */
    m->in_memory = MAP_IN_MEMORY; /*reset this*/
    sprintf(filename, "%s/wmapcurpos", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }

    if (players_on_map(m, TRUE) == 0)
      delete_map(m);

    fprintf(fp, "%d %d", wmperformstartx, wmperformstarty);
    fclose(fp);
}

/* provide wx and wy. Will fill in with weathermap coordinates.  Requires
   the current mapname (must be a worldmap), and your coordinates on the
   map.  returns -1 if you give it something it can't figure out. 0 normally.
*/

int worldmap_to_weathermap(int x, int y, int *wx, int *wy, char *filename)
{
    int spwtx, spwty;
    int fx, fy;
    int nx, ny;

    spwtx = (settings.worldmaptilesx * settings.worldmaptilesizex) / WEATHERMAPTILESX;
    spwty = (settings.worldmaptilesy * settings.worldmaptilesizey) / WEATHERMAPTILESY;

    fx = -1;
    fy = -1;
    sscanf(filename, "world/world_%d_%d", &fx, &fy);
    if (fx > settings.worldmapstartx + settings.worldmaptilesx ||
	fx < settings.worldmapstartx)
	return -1;
    if (fy > settings.worldmapstarty + settings.worldmaptilesy ||
	fy < settings.worldmapstarty)
	return -1;
    fx -= settings.worldmapstartx;
    fy -= settings.worldmapstarty;
    
    nx = fx * settings.worldmaptilesizex + x;
    ny = fy * settings.worldmaptilesizey + y;

    *wx = nx/spwtx;
    *wy = ny/spwty;

    return 0;
}

/* provide x and y.  Will fill in with the requested corner of the real
 * world map, given the current weathermap section.  dir selects which
 * corner to return.  Valid values are 2 4 6 8 for the corners.  return
 * value is the name of the map that corner resides in.
 */

char *weathermap_to_worldmap_corner(int wx, int wy, int *x, int *y, int dir)
{
    int spwtx, spwty;
    int tx, ty, nx, ny;
    char *mapname;
    
    spwtx = (settings.worldmaptilesx * settings.worldmaptilesizex) / WEATHERMAPTILESX;
    spwty = (settings.worldmaptilesy * settings.worldmaptilesizey) / WEATHERMAPTILESY;
    switch (dir) {
    case 2: wx++; break;
    case 4: wx++; wy++; break;
    case 6: wy++; break;
    case 8: break;
    }
    if (wx > 0)
    	tx = (wx*spwtx)-1;
    else
	tx = wx;
    if (wy > 0)
	ty = (wy*spwty)-1;
    else
	ty = wy;
    
    nx = (tx / settings.worldmaptilesizex) + settings.worldmapstartx;
    ny = (ty / settings.worldmaptilesizey) + settings.worldmapstarty;
    mapname = malloc(sizeof(char) * strlen("world/world_0000_0000"));
    sprintf(mapname, "world/world_%d_%d", nx, ny);
    
    *x = tx%settings.worldmaptilesizex;
    *y = ty%settings.worldmaptilesizey;
    return(mapname);
}

int polar_distance(int x, int y, int equator)
{
    if ((x+y) > equator) { /* south pole */
	x = WEATHERMAPTILESX - x;
	y = WEATHERMAPTILESY - y;
	return ((x+y)/2);
    } else if ((x+y) < equator) { /* north pole */
	return ((x+y)/2);
    } else {
	return equator/2;
    }
}

/* update the humidity */

void update_humid()
{
    int x, y;

    for (y=0; y < WEATHERMAPTILESY; y++)
	for (x=0; x < WEATHERMAPTILESX; x++)
	    weathermap[x][y].humid = humid_tile(x, y);
}

/* calculate the humidity of this tile */

int humid_tile(int x, int y)
{
    int ox, oy, humid;

    ox = x;
    oy = y;

    /* find the square the wind is coming from, without going out of bounds */

    if (weathermap[x][y].winddir == 8 || weathermap[x][y].winddir <= 2) {
	if (y != 0)
	    oy = y - 1;
    }
    if (weathermap[x][y].winddir >= 6) {
	if (x != 0)
	    ox = x - 1;
    }
    if (weathermap[x][y].winddir >= 4 && weathermap[x][y].winddir <= 6) {
	if (y != WEATHERMAPTILESY)
	    oy = y + 1;
    }
    if (weathermap[x][y].winddir >= 2 && weathermap[x][y].winddir <= 4) {
	if (x != WEATHERMAPTILESX)
	    ox = x + 1;
    }
    humid = (weathermap[x][y].humid * 2 +
	weathermap[ox][oy].humid * weathermap[ox][oy].windspeed +
	weathermap[x][y].water + rndm(0, 10)) / 
	(weathermap[ox][oy].windspeed+3) + rndm(0, 5);
    if (humid < 0)
	humid = 1;
    if (humid > 100)
	humid = 100;
    return humid;
}

/* calculate temperature */

void temperature_calc(int x, int y, timeofday_t *tod)
{
    int dist, equator, elev, n;
    float diff, tdiff;

    equator = (WEATHERMAPTILESX + WEATHERMAPTILESY) / 4;
    diff = (float)(EQUATOR_BASE_TEMP - POLAR_BASE_TEMP) / (float)equator;
    tdiff = (float)SEASONAL_ADJUST / ((float)MONTHS_PER_YEAR / 2.0);
    equator *= 2;
    n = 0;
    /* we essentially move the equator during the season */
    if (tod->month > (MONTHS_PER_YEAR / 2)) { /* EOY */
	n -= (tod->month * tdiff);
    } else {
	n = (MONTHS_PER_YEAR - tod->month) * tdiff;
    }
    dist = polar_distance(x-n/2, y-n/2, equator);

    /* now we have the base temp, unadjusted for time.  Time adjustment
       is not recorded on the map, rather, it's done JIT. */
    weathermap[x][y].temp = (int)(dist * diff);
    /* just scrap these for now, its mostly ocean */
    if (weathermap[x][y].avgelev < 0)
	elev = 0;
    else
	elev = MAX(10000, weathermap[x][y].avgelev)/1000;
    weathermap[x][y].temp -= elev;
}

/* Compute the real (adjusted) temperature of a given weathermap tile.
   This takes into account the wind, base temp, sunlight, and other fun
   things.  Seasons are automatically handled by moving the equator.
   Elevation is considered in the base temp.
*/

int real_temperature(int x, int y)
{
    int i, temp;
    timeofday_t tod;

    /* adjust for time of day */
    temp = weathermap[x][y].temp;
    get_tod(&tod);
    for (i = HOURS_PER_DAY/2; i < HOURS_PER_DAY; i++) {
	temp += season_tempchange[i];
	/* high amounts of water has a buffering effect on the temp */
	if (weathermap[x][y].water > 33)
	    i++;
    }
    for (i = 0; i <= tod.hour; i++) {
	temp += season_tempchange[i];
	if (weathermap[x][y].water > 33)
	    i++;
    }

    /* windchill */
    for (i=1; i < weathermap[x][y].windspeed; i+=i)
	temp--;

    return temp;
}

/* this code simply smooths the pressure map */

void smooth_pressure()
{
    int x, y;
    int k;

    for (k=0; k < PRESSURE_ROUNDING_ITER; k++) {
	for (x=2; x < WEATHERMAPTILESX-2; x++) {
	    for (y=2; y < WEATHERMAPTILESY-2; y++) {
		weathermap[x][y].pressure = (weathermap[x][y].pressure *
		    PRESSURE_ROUNDING_FACTOR + weathermap[x-1][y].pressure +
		    weathermap[x][y-1].pressure + weathermap[x-1][y-1].pressure +
		    weathermap[x+1][y].pressure + weathermap[x][y+1].pressure +
		    weathermap[x+1][y+1].pressure + weathermap[x+1][y-1].pressure +
		    weathermap[x-1][y+1].pressure) / (PRESSURE_ROUNDING_FACTOR+8);
	    }
	}
	for (x=WEATHERMAPTILESX-2; x > 2; x--) {
	    for (y=WEATHERMAPTILESY-2; y > 2; y--) {
		weathermap[x][y].pressure = (weathermap[x][y].pressure *
		    PRESSURE_ROUNDING_FACTOR + weathermap[x-1][y].pressure +
		    weathermap[x][y-1].pressure + weathermap[x-1][y-1].pressure +
		    weathermap[x+1][y].pressure + weathermap[x][y+1].pressure +
		    weathermap[x+1][y+1].pressure + weathermap[x+1][y-1].pressure +
		    weathermap[x-1][y+1].pressure) / (PRESSURE_ROUNDING_FACTOR+8);
	    }
	}
    }

    for (x=0; x < WEATHERMAPTILESX; x++)
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    weathermap[x][y].pressure = MIN(weathermap[x][y].pressure, PRESSURE_MAX);
	    weathermap[x][y].pressure = MAX(weathermap[x][y].pressure, PRESSURE_MIN);
	}

}

/* perform small randomizations in the pressure map.  Then, apply the
   smoothing algorithim.. This causes the pressure to change very slowly
*/

void perform_pressure()
{
    int x, y, l, n, j, k;

    /* create random spikes in the pressure */
    for (l=0; l < PRESSURE_SPIKES; l++) {
	x = rndm(0, WEATHERMAPTILESX-1);
	y = rndm(0, WEATHERMAPTILESY-1);
	n = rndm(600, 1300);
	weathermap[x][y].pressure = n;
	if (x > 5 && y > 5 && x < WEATHERMAPTILESX-5 && y < WEATHERMAPTILESY-5){
	    for (j=x-2; j<x+2; j++)
		for (k=y-2; k<y+2; k++) {
		    weathermap[j][k].pressure = n;
		    /* occasionally add a storm */
		    if (rndm(1, 20) == 1)
			weathermap[j][k].humid = rndm(50, 80);
		}
	}
    }

    for (x=0; x < WEATHERMAPTILESX; x++)
	for (y=0; y < WEATHERMAPTILESY; y++)
	    weathermap[x][y].pressure += rndm(-1, 4);

    smooth_pressure();
}


/* is direction a similar to direction b? Find out in this exciting function
   below. 
 */

int similar_direction(int a, int b)
{
    /* shortcut the obvious */
    if (a == b)
	return 1;

    switch(a) {
	case 1: if (b <= 2 || b == 8) return 1; break;
	case 2: if (b > 0 && b < 4) return 1; break;
	case 3: if (b > 1 && b < 5) return 1; break;
	case 4: if (b > 2 && b < 6) return 1; break;
	case 5: if (b > 3 && b < 7) return 1; break;
	case 6: if (b > 4 && b < 8) return 1; break;
	case 7: if (b > 5) return 1; break;
	case 8: if (b > 6 || b == 1) return 1; break;
    }
    return 0;
}

/*
   It doesn't really smooth it as such.  The main function of this is to
   apply the pressuremap to the wind direction and speed.  Then, we run
   a quick pass to update the windspeed.
*/

void smooth_wind()
{
    int x, y;
    int tx, ty, dx, dy;
    int minp;

    /* skip the outer squares.. it makes handling alot easier */
    for (x=1; x < WEATHERMAPTILESX-1; x++)
	for (y=1; y < WEATHERMAPTILESY-1; y++) {
	    minp = PRESSURE_MAX + 1;
	    for (tx=-1; tx < 2; tx++)
		for (ty=-1; ty < 2; ty++)
		    if (!(tx == 0 && ty == 0))
			if (weathermap[x+tx][y+ty].pressure < minp) {
			    minp = weathermap[x+tx][y+ty].pressure;
			    dx = tx;
			    dy = ty;
			}

	    /* if the wind is strong, the pressure won't decay it completely */
	    if (weathermap[x][y].windspeed > 5 &&
		!similar_direction(weathermap[x][y].winddir, find_dir_2(dx, dy))) {
		weathermap[x][y].windspeed -= 2;
	    } else {
		weathermap[x][y].winddir = find_dir_2(dx, dy);
		weathermap[x][y].windspeed = weathermap[x][y].pressure -
		    weathermap[x+dx][y+dy].pressure;
	    }
	    if (weathermap[x][y].windspeed < 0)
		weathermap[x][y].windspeed = 0;
	}

    /*  now, lets crank on the speed.  When surrounding tiles all have
	the same speed, inc ours.  If it's chaos. drop it.
     */
    for (x=1; x < WEATHERMAPTILESX-1; x++)
	for (y=1; y < WEATHERMAPTILESY-1; y++) {
	    minp = 0;
	    for (tx=-1; tx < 2; tx++)
		for (ty=-1; ty < 2; ty++)
		    if (ty != 0 && ty != 0)
			if (similar_direction(weathermap[x][y].winddir,
				weathermap[x+tx][y+ty].winddir))
			    minp++;
	    if (minp > 4)
		weathermap[x][y].windspeed++;
	    if (minp > 6)
		weathermap[x][y].windspeed += 2;
	    if (minp < 2)
		weathermap[x][y].windspeed--;
	    if (weathermap[x][y].windspeed < 0)
		weathermap[x][y].windspeed = 0;
	}
}

void plot_gulfstream()
{
    int x, y, tx;

    x = gulf_stream_start;

    if (gulf_stream_direction) {
	for (y=WEATHERMAPTILESY-1; y > 0; y--) {
	    for (tx=0; tx < GULF_STREAM_WIDTH && x+tx < WEATHERMAPTILESX; tx++) {
		if (similar_direction(weathermap[x+tx][y].winddir,
		    gulf_stream_dir[tx][y]) &&
		    weathermap[x+tx][y].windspeed < GULF_STREAM_BASE_SPEED-5)
		    weathermap[x+tx][y].windspeed += gulf_stream_speed[tx][y];
		else
		    weathermap[x+tx][y].windspeed = gulf_stream_speed[tx][y];
		weathermap[x+tx][y].winddir = gulf_stream_dir[tx][y];
		if (tx == GULF_STREAM_WIDTH-1) {
		    switch (gulf_stream_dir[tx][y]) {
		    case 6: x--; break;
		    case 7: break;
		    case 8: x++; ; break;
		    }
		}
		if (x < 0)
		    x++;
		if (x >= WEATHERMAPTILESX-GULF_STREAM_WIDTH)
		    x--;
	    }
	}
    } else {
	for (y=0; y < WEATHERMAPTILESY-1; y++) {
	    for (tx=0; tx < GULF_STREAM_WIDTH && x+tx < WEATHERMAPTILESX; tx++) {
		if (similar_direction(weathermap[x+tx][y].winddir,
		    gulf_stream_dir[tx][y]) &&
		    weathermap[x+tx][y].windspeed < GULF_STREAM_BASE_SPEED-5)
		    weathermap[x+tx][y].windspeed += gulf_stream_speed[tx][y];
		else
		    weathermap[x+tx][y].windspeed = gulf_stream_speed[tx][y];
		weathermap[x+tx][y].winddir = gulf_stream_dir[tx][y];
		if (tx == GULF_STREAM_WIDTH-1) {
		    switch (gulf_stream_dir[tx][y]) {
		    case 2: x++; break;
		    case 3: break;
		    case 4: x--; break;
		    }
		}
		if (x < 0)
		    x++;
		if (x >= WEATHERMAPTILESX-GULF_STREAM_WIDTH)
		    x--;
	    }
	}
    }
    /* occasionally move the stream */
    if (rndm(1, 500) == 1) {
	gulf_stream_direction = rndm(0, 1);
	for (tx=0; tx < GULF_STREAM_WIDTH; tx++)
	    for (y=0; y < WEATHERMAPTILESY-1; y++)
		if (gulf_stream_direction)
		    switch (gulf_stream_dir[tx][y]) {
		    case 2: gulf_stream_dir[tx][y] = 6; break;
		    case 3: gulf_stream_dir[tx][y] = 7; break;
		    case 4: gulf_stream_dir[tx][y] = 8; break;
		    }
		else
		    switch (gulf_stream_dir[tx][y]) {
		    case 6: gulf_stream_dir[tx][y] = 2; break;
		    case 7: gulf_stream_dir[tx][y] = 3; break;
		    case 8: gulf_stream_dir[tx][y] = 4; break;
		    }
    }
    if (rndm(1, 25) == 1)
	gulf_stream_start += rndm(-1, 1);
    if (gulf_stream_start >= WEATHERMAPTILESX-GULF_STREAM_WIDTH)
	gulf_stream_start--;
    if (gulf_stream_start < 1)
	gulf_stream_start++;

}

/* let the madness, begin. */

void compute_sky()
{
    int x, y;
    int temp;

    for (x=0; x < WEATHERMAPTILESX; x++) {
	for (y=0; y < WEATHERMAPTILESY; y++) {
	    temp = real_temperature(x, y);
	    if (weathermap[x][y].pressure < 980) {
		if (weathermap[x][y].humid < 20)
		    weathermap[x][y].sky = SKY_LIGHTCLOUD;
		else if (weathermap[x][y].humid < 30)
		    weathermap[x][y].sky = SKY_OVERCAST;
		else if (weathermap[x][y].humid < 40)
		    weathermap[x][y].sky = SKY_LIGHT_RAIN;
		else if (weathermap[x][y].humid < 55)
		    weathermap[x][y].sky = SKY_RAIN;
		else if (weathermap[x][y].humid < 70)
		    weathermap[x][y].sky = SKY_HEAVY_RAIN;
		else
		    weathermap[x][y].sky = SKY_HURRICANE;
		if (weathermap[x][y].sky < SKY_HURRICANE &&
		    weathermap[x][y].windspeed > 30)
		    weathermap[x][y].sky++;
		if (temp <= 0 && weathermap[x][y].sky > SKY_OVERCAST)
		    weathermap[x][y].sky += 10; /*let it snow*/
	    } else if (weathermap[x][y].pressure < 1000) {
		if (weathermap[x][y].humid < 10)
		    weathermap[x][y].sky = SKY_CLEAR;
		else if (weathermap[x][y].humid < 25)
		    weathermap[x][y].sky = SKY_LIGHTCLOUD;
		else if (weathermap[x][y].humid < 45)
		    weathermap[x][y].sky = SKY_OVERCAST;
		else if (weathermap[x][y].humid < 60)
		    weathermap[x][y].sky = SKY_LIGHT_RAIN;
		else if (weathermap[x][y].humid < 75)
		    weathermap[x][y].sky = SKY_RAIN;
		else
		    weathermap[x][y].sky = SKY_HEAVY_RAIN;
		if (weathermap[x][y].sky < SKY_HURRICANE &&
		    weathermap[x][y].windspeed > 30)
		    weathermap[x][y].sky++;
		if (temp <= 0 && weathermap[x][y].sky > SKY_OVERCAST)
		    weathermap[x][y].sky += 10; /*let it snow*/
		if (temp > 0 && temp < 5 && weathermap[x][y].humid > 95 &&
		    weathermap[x][y].windspeed < 3)
		    weathermap[x][y].sky = SKY_FOG; /* rare */
		if (temp > 0 && temp < 5 && weathermap[x][y].humid > 70 &&
		    weathermap[x][y].windspeed > 35)
		    weathermap[x][y].sky = SKY_HAIL; /* rare */
	    } else if (weathermap[x][y].pressure < 1020) {
		if (weathermap[x][y].humid < 20)
		    weathermap[x][y].sky = SKY_CLEAR;
		else if (weathermap[x][y].humid < 30)
		    weathermap[x][y].sky = SKY_LIGHTCLOUD;
		else if (weathermap[x][y].humid < 40)
		    weathermap[x][y].sky = SKY_OVERCAST;
		else if (weathermap[x][y].humid < 55)
		    weathermap[x][y].sky = SKY_LIGHT_RAIN;
		else if (weathermap[x][y].humid < 70)
		    weathermap[x][y].sky = SKY_RAIN;
		else
		    weathermap[x][y].sky = SKY_HEAVY_RAIN;
		if (weathermap[x][y].sky < SKY_HURRICANE &&
		    weathermap[x][y].windspeed > 30)
		    weathermap[x][y].sky++;
		if (temp <= 0 && weathermap[x][y].sky > SKY_OVERCAST)
		    weathermap[x][y].sky += 10; /*let it snow*/
	    } else {
		if (weathermap[x][y].humid < 35)
		    weathermap[x][y].sky = SKY_CLEAR;
		else if (weathermap[x][y].humid < 55)
		    weathermap[x][y].sky = SKY_LIGHTCLOUD;
		else if (weathermap[x][y].humid < 70)
		    weathermap[x][y].sky = SKY_OVERCAST;
		else if (weathermap[x][y].humid < 85)
		    weathermap[x][y].sky = SKY_LIGHT_RAIN;
		else if (weathermap[x][y].humid < 95)
		    weathermap[x][y].sky = SKY_RAIN;
		else
		    weathermap[x][y].sky = SKY_HEAVY_RAIN;
		if (weathermap[x][y].sky < SKY_HURRICANE &&
		    weathermap[x][y].windspeed > 30)
		    weathermap[x][y].sky++;
		if (temp <= 0 && weathermap[x][y].sky > SKY_OVERCAST)
		    weathermap[x][y].sky += 10; /*let it snow*/
	    }
	}
    }
}
