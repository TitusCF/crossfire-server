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
    }
    get_tod(&tod);
    dawn_to_dusk(&tod);
    /* call the weather calculators, here, in order */
    if (settings.dynamiclevel > 0) {
        perform_pressure();	/* pressure is the random factor */
        smooth_wind();	/* calculate the wind. depends on pressure */
        update_humid();
        init_temperature();
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

    /* build a gulf stream */
    x = rndm(0, WEATHERMAPTILESX);
    y = rndm(0, 1);

    if (y) {
	for (y=WEATHERMAPTILESY-1; y >= 0; y--) {
	    switch(rndm(0, 6)) {
	    case 0:
	    case 1:
	    case 2:
		weathermap[x][y].windspeed = rndm(40, 50);
		weathermap[x][y].winddir = 8;
		if (x==0) {
		    weathermap[x+1][y].winddir = 7;
		    weathermap[x+1][y].windspeed = rndm(40, 50);
		} else {
		    weathermap[x-1][y].winddir = 8;
		    weathermap[x-1][y].windspeed = rndm(40, 50);
		    x--;
		}
		break;
	    case 3:
		weathermap[x][y].windspeed = rndm(40, 50);
		weathermap[x][y].winddir = 7;
		if (x==0) {
		    weathermap[x+1][y].winddir = 7;
		    weathermap[x+1][y].windspeed = rndm(40, 50);
		} else {
		    weathermap[x-1][y].winddir = 7;
		    weathermap[x-1][y].windspeed = rndm(40, 50);
		}
		break;
	    case 4:
	    case 5:
	    case 6:		
		weathermap[x][y].windspeed = rndm(40, 50);
		weathermap[x][y].winddir = 6;
		if (x==WEATHERMAPTILESX-1) {
		    weathermap[x-1][y].winddir = 7;
		    weathermap[x-1][y].windspeed = rndm(40, 50);
		} else {
		    weathermap[x+1][y].winddir = 6;
		    weathermap[x+1][y].windspeed = rndm(40, 50);
		    x++;
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
		weathermap[x][y].windspeed = rndm(40, 50);
		weathermap[x][y].winddir = 2;
		if (x==0) {
		    weathermap[x+1][y].winddir = 3;
		    weathermap[x+1][y].windspeed = rndm(40, 50);
		} else {
		    weathermap[x-1][y].winddir = 2;
		    weathermap[x-1][y].windspeed = rndm(40, 50);
		    x--;
		}
		break;
	    case 3:
		weathermap[x][y].windspeed = rndm(40, 50);
		weathermap[x][y].winddir = 2;
		if (x==0) {
		    weathermap[x+1][y].winddir = 2;
		    weathermap[x+1][y].windspeed = rndm(40, 50);
		} else {
		    weathermap[x-1][y].winddir = 2;
		    weathermap[x-1][y].windspeed = rndm(40, 50);
		}
		break;
	    case 4:
	    case 5:
	    case 6:		
		weathermap[x][y].windspeed = rndm(40, 50);
		weathermap[x][y].winddir = 4;
		if (x==WEATHERMAPTILESX-1) {
		    weathermap[x-1][y].winddir = 3;
		    weathermap[x-1][y].windspeed = rndm(40, 50);
		} else {
		    weathermap[x+1][y].winddir = 4;
		    weathermap[x+1][y].windspeed = rndm(40, 50);
		    x++;
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
    int x, y;
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
    read_watermap();
    read_humidmap();
    read_elevmap(); /* elevation must allways follow humidity */
    read_temperaturemap();

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

    for (x=0; x < WEATHERMAPTILESX; x++)
	for (y=0; y < WEATHERMAPTILESY; y++)
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

    humid = MIN(100, ((weathermap[x][y].humid * 2 +
	weathermap[ox][oy].humid * weathermap[ox][oy].windspeed +
	weathermap[x][y].water + rndm(-2, 2)) / 
	(weathermap[ox][oy].windspeed+3)));
    return humid;
}

/* calculate temperature */

void temperature_calc(int x, int y, timeofday_t *tod)
{
    int dist, equator, elev, eq2;
    float diff, tdiff;

    equator = (WEATHERMAPTILESX + WEATHERMAPTILESY) / 4;
    eq2 = (WEATHERMAPTILESX + WEATHERMAPTILESY) / 2;
    diff = (float)(EQUATOR_BASE_TEMP - POLAR_BASE_TEMP) / (float)equator;
    tdiff = (float)SEASONAL_ADJUST / ((float)MONTHS_PER_YEAR / 2.0);

    /* we essentially move the equator during the season */
    if (tod->month > (MONTHS_PER_YEAR / 2)) { /* EOY */
        eq2 -= tod->month * tdiff;
    } else {
        eq2 += (MONTHS_PER_YEAR - tod->month) * tdiff;
    }
    dist = polar_distance(x, y, eq2);

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
	    for (k=y-2; k<y+2; k++)
	      weathermap[j][k].pressure = n;
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
