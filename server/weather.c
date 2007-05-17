/*
 * static char *rcsid_weather_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006,2007 Mark Wedel & Crossfire Development Team
    Copyright (C) 2002 Tim Rightnour
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

/* This weather system was written for crossfire by Tim Rightnour */

#include <global.h>
#include <tod.h>
#include <map.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

extern unsigned long todtick;
extern weathermap_t **weathermap;

static void dawn_to_dusk(const timeofday_t *tod);
static void write_skymap(void);
static void write_pressuremap(void);
static void read_pressuremap(void);
static void init_pressure(void);
static void write_winddirmap(void);
static void read_winddirmap(void);
static void write_windspeedmap(void);
static void read_windspeedmap(void);
static void init_wind(void);
static void write_gulfstreammap(void);
static void read_gulfstreammap(void);
static void init_gulfstreammap(void);
static void write_humidmap(void);
static void read_humidmap(void);
static void write_elevmap(void);
static void read_elevmap(void);
static void write_watermap(void);
static void read_watermap(void);
static void init_humid_elev(void);
static void write_temperaturemap(void);
static void read_temperaturemap(void);
static void init_temperature(void);
static void write_rainfallmap(void);
static void read_rainfallmap(void);
static void init_rainfall(void);
static void init_weatheravoid (weather_avoids_t wa[]);
static void perform_weather(void);
static object *avoid_weather(int *av, mapstruct *m, int x, int y, int *gs, int grow);
static void calculate_temperature(mapstruct *m, int wx, int wy);
static void let_it_snow(mapstruct *m, int wx, int wy);
static void singing_in_the_rain(mapstruct *m, int wx, int wy);
static void plant_a_garden(mapstruct *m, int wx, int wy);
static void change_the_world(mapstruct *m, int wx, int wy);
static const char *weathermap_to_worldmap_corner(int wx, int wy, int *x, int *y, int dir);
static int polar_distance(int x, int y, int equator);
static void update_humid(void);
static int humid_tile(int x, int y);
static void temperature_calc(int x, int y, const timeofday_t *tod);
static int real_temperature(int x, int y);
static void smooth_pressure(void);
static void perform_pressure(void);
static void smooth_wind(void);
static void plot_gulfstream(void);
static void compute_sky(void);
static void process_rain(void);
static void spin_globe(void);
static void write_weather_images(void);

static int gulf_stream_speed[GULF_STREAM_WIDTH][WEATHERMAPTILESY];
static int gulf_stream_dir[GULF_STREAM_WIDTH][WEATHERMAPTILESY];
static int gulf_stream_start;
static int gulf_stream_direction;

static const int season_timechange[5][HOURS_PER_DAY] = {
/*    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14  1  2  3  4  5  6  7  8  9 10 11 12 13 */
    { 0, 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    { 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
    { 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
    { 0, 0, 0, 0, 0,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0},
    { 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0}
};

static const int season_tempchange[HOURS_PER_DAY] = {
/*  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14  1  2  3  4  5  6  7  8  9 10 11 12 13 */
    0, 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};

/**
 * The table below is used to set which tiles the weather will avoid
 * processing.  This keeps it from putting snow on snow, and putting snow
 * on the ocean, and other things like that.
 */
static weather_avoids_t weather_avoids[] = {
    {"snow", 1, NULL},
    {"snow2", 1, NULL},
    {"snow3", 1, NULL},
    {"snow4", 1, NULL},
    {"snow5", 1, NULL},
    {"mountain1_snow", 1, NULL},
    {"mountain2_snow", 1, NULL},
    {"rain1", 1, NULL},
    {"rain2", 1, NULL},
    {"rain3", 1, NULL},
    {"rain4", 1, NULL},
    {"rain5", 1, NULL},
    {"mountain1_rivlets", 1, NULL},
    {"mountain2_rivlets", 1, NULL},
    {"drifts", 0, NULL},
    {"glacier", 0, NULL},
    {"cforest1", 0, NULL},
    {"sea", 0, NULL},
    {"sea1", 0, NULL},
    {"deep_sea", 0, NULL},
    {"shallow_sea", 0, NULL},
    {"lava", 0, NULL},
    {"permanent_lava", 0, NULL},
    /* Mountain cave are weird archetypes: floor, but exit. So we shouldn't cover them. */
    {"mountain_cave", 0, NULL},
    {"mountain_cave2", 0, NULL},
    {NULL, 0, NULL}
};

/**
 * this table is identical to the one above, except these are tiles to avoid
 * when processing growth.  IE, don't grow herbs in the ocean.  The second
 * field is unused.
 */
static weather_avoids_t growth_avoids[] = {
    {"cobblestones", 0, NULL},
    {"cobblestones2", 0, NULL},
    {"flagstone", 0, NULL},
    {"stonefloor2", 0, NULL},
    {"lava", 0, NULL},
    {"permanent_lava", 0, NULL},
    {"sea", 0, NULL},
    {"sea1", 0, NULL},
    {"deep_sea", 0, NULL},
    {"shallow_sea", 0, NULL},
    {"farmland", 0, NULL},
    {"dungeon_magic", 0, NULL},
    {"dungeon_floor", 0, NULL},
    {"lake", 0, NULL},
    {"grasspond", 0, NULL},
    /* Mountain cave are weird archetypes: floor, but exit. So we shouldn't cover them. */
    {"mountain_cave", 0, NULL},
    {"mountain_cave2", 0, NULL},
    {NULL, 0, NULL}
};

/**
 * The table below is used in let_it_snow() and singing_in_the_rain() to
 * decide what type of snow/rain/etc arch to put down.  The first field is the
 * name of the arch we want to match.  The second field is the special snow
 * type we use to cover that arch.  The third field is the doublestack arch,
 * NULL if none, used to stack over the snow after covering the tile.
 * The fourth field is 1 if you want to match arch->name, 0 to match ob->name.
 */
static weather_replace_t weather_replace[] = {
    {"impossible_match", "snow5", NULL, 0},
    {"impossible_match2", "snow4", NULL, 0}, /* placeholders */
    {"impossible_match3", "snow3", NULL, 0},
    {"hills", "drifts", NULL, 0},
    {"treed_hills", "drifts", "woods5", 1},
    {"grass", "snow", NULL, 0},
    {"sand", "snow", NULL, 0},
    {"stones", "snow2", NULL, 0},
    {"steppe", "snow2", NULL, 0},
    {"brush", "snow2", NULL, 0},
    {"farmland", "snow3", NULL, 0},
    {"wasteland", "glacier", NULL, 0},
    {"mountain", "mountain1_snow", NULL, 1},
    {"mountain2", "mountain2_snow", NULL, 1},
    {"mountain4", "mountain2_snow", NULL, 1},
    {"evergreens", "snow", "evergreens2", 1},
    {"evergreen","snow", "tree5", 1},
    {"tree", "snow", "tree3", 0},
    {"woods", "snow3", "woods4", 1},
    {"woods_3", "snow", "woods5", 1},
    {NULL, NULL, NULL, 0},
};

/**
 * The table below is used to grow things on the map. See include/tod.h for
 * the meanings of all of the fields.
 */
static const weather_grow_t weather_grow[] = {
    /* herb, tile, random, rfmin, rfmax, humin, humax, tempmin, tempmax, elevmin, elevmax, season */
    {"mint", "grass", 10, 1.0, 2.0, 30, 100, 10, 25, -100, 9999, 2},
    {"rose_red", "grass", 15, 1.0, 2.0, 30, 100, 10, 25, -100, 9999, 2},
    {"rose_red", "hills", 15, 1.0, 2.0, 30, 100, 10, 25, -100, 9999, 2},
    {"mint", "brush", 8, 1.0, 2.0, 30, 100, 10, 25, -100, 9999, 2},
    {"blackroot", "swamp", 15, 1.6, 2.0, 60, 100, 20, 30, -100, 1500, 0},
    {"mushroom_1", "grass", 15, 1.6, 2.0, 60, 100, 3, 30, -100, 1500, 0},
    {"mushroom_2", "grass", 15, 1.6, 2.0, 60, 100, 3, 30, -100, 1500, 0},
    {"mushroom_1", "swamp", 15, 1.6, 2.0, 60, 100, 3, 30, -100, 1500, 0},
    {"mushroom_2", "swamp", 15, 1.6, 2.0, 60, 100, 3, 30, -100, 1500, 0},
    {"mushroom_1", "hills", 15, 1.6, 2.0, 60, 100, 3, 30, -100, 1500, 0},
    {"mushroom_2", "hills", 15, 1.6, 2.0, 60, 100, 3, 30, -100, 1500, 0},
    {"pipeweed", "farmland", 20, 1.0, 2.0, 30, 100, 10, 25, 100, 5000, 0},
    {"cabbage", "farmland", 10, 1.0, 2.0, 30, 100, 10, 25, -100, 9999, 0},
    {"onion", "farmland", 10, 1.0, 2.0, 30, 100, 10, 25, 100, 9999, 0},
    {"carrot", "farmland", 10, 1.0, 2.0, 30, 100, 10, 25, 100, 9999, 0},
    {"thorns", "brush", 15, 0.5, 1.3, 30, 100, 10, 25, -100, 9999, 0},
    {"mountain_foilage", "mountain", 6, 1.0, 2.0, 25, 100, 5, 30, 0, 15999, 2},
    {NULL, NULL, 1, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0}
};

/**
 * The table below uses the same format as the one above.  However this
 * table is used to change the layout of the worldmap itself.  The tile
 * parameter is a base tile to lay down underneath the herb tile.
 */
static const weather_grow_t weather_tile[] = {
    /* herb, tile, random, rfmin, rfmax, humin, humax, tempmin, tempmax, elevmin, elevmax */
    {"dunes", NULL, 2, 0.0, 0.03, 0, 20, 10, 99, 0, 4000, 0},
    {"desert", NULL, 1, 0.0, 0.05, 0, 20, 10, 99, 0, 4000, 0},
    {"pstone_2", NULL, 1, 0.0, 0.05, 0, 20, -30, 10, 0, 4000, 0},
    {"pstone_3", NULL, 1, 0.0, 0.05, 0, 20, -30, 10, 0, 4000, 0},
    {"grassbrown", NULL, 1, 0.05, 1.0, 20, 80, -20, -3, 0, 5000, 0},
    {"grass_br_gr", NULL, 1, 0.05, 1.0, 20, 80, -3, 5, 0, 5000, 0},
    {"grass", NULL, 1, 0.05, 1.0, 20, 80, 5, 15, 0, 5000, 0},
    {"grassmedium", NULL, 1, 0.05, 1.0, 20, 80, 15, 25, 0, 5000, 0},
    {"grassdark", NULL, 1, 0.05, 1.0, 20, 80, 25, 35, 0, 5000, 0},
    {"brush", NULL, 1, 0.2, 1.0, 25, 70, 0, 30, 500, 6000, 0},
    /* small */
    {"evergreens2", "brush", 1, 0.5, 1.8, 30, 90, -30, 24, 3000, 8000, 0},
    {"fernsdense", "brush", 1, 0.9, 2.5, 50, 100, 10, 35, 1000, 6000, 0},
    {"fernssparse", "brush", 1, 0.7, 2.0, 30, 90, -15, 35, 0, 4000, 0},
    {"woods4", "brush", 1, 0.1, 0.8, 30, 60, -5, 25, 1000, 4500, 0},
    {"woods5", "brush", 1, 0.6, 1.5, 20, 70, -15, 20, 2000, 5500, 0},
    {"forestsparse", "brush", 1, 0.3, 1.5, 15, 60, -20, 25, 0, 4500, 0},
    /* big */
    /*
    {"ytree_2", "brush", 2, 0.1, 0.6, 30, 60, 10, 25, 1000, 3500, 0},
    {"tree3", "grass", 2, 0.9, 2.5, 50, 100, 10, 35, 1000, 4000, 0},
    {"tree5", "grass", 2, 0.5, 1.5, 40, 90, -10, 24, 3000, 8000, 0},
    {"tree3", "grassmeduim", 2, 0.9, 2.5, 50, 100, 10, 35, 1000, 4000, 0},
    {"tree5", "grassmedium", 2, 0.5, 1.5, 40, 90, -10, 24, 3000, 8000, 0},
    {"tree3", "grassdark", 2, 0.9, 2.5, 50, 100, 10, 35, 1000, 4000, 0},
    {"tree5", "grassdark", 2, 0.5, 1.5, 40, 90, -10, 24, 3000, 8000, 0},*/
    /* mountians */
    {"steppe", NULL, 1, 0.5, 1.3, 0, 30, -20, 35, 1000, 6000, 0},
    {"steppelight", NULL, 1, 0.0, 0.6, 0, 20, -50, 35, 0, 5000, 0},
    {"hills", NULL, 1, 0.1, 0.9, 20, 80, -10, 30, 5000, 8500, 0},
    {"hills_rocky", NULL, 1, 0.0, 0.9, 0, 100, -50, 50, 5000, 8500, 0},
    {"swamp", NULL, 1, 1.0, 9.9, 55, 80, 10, 50, 0, 1000, 0},
    {"deep_swamp", NULL, 1, 1.0, 9.9, 80, 100, 10, 50, 0, 1000, 0},
    {"mountain", NULL, 1, 0.0, 9.9, 0, 100, -50, 50, 8000, 10000, 0},
    {"mountain2", NULL, 1, 0.0, 9.9, 0, 100, -50, 50, 9500, 11000, 0},
    {"mountain4", NULL, 1, 0.0, 9.9, 0, 100, -50, 50, 10500, 12000, 0},
    {"mountain5", NULL, 1, 0.0, 9.9, 0, 100, -50, 50, 11500, 13500, 0},
    {"wasteland", NULL, 1, 0.0, 9.9, 0, 100, -50, 50, 13000, 99999, 0},
    /* catchalls */
    {"palms", "pstone_1", 1, 0.01, 0.1, 0, 30, 5, 99, 0, 4000, 0},
    {"large_stones", NULL, 1, 0.0, 9.9, 0, 100, -50, 50, 6000, 8000, 0},
    {"earth", NULL, 1, 0.0, 1.0, 0, 70, -30, 15, 0, 6000, 0},
    {"medium_stones", NULL, 1, 1.0, 3.0, 70, 100, -30, 10,  0, 4000, 0}, /*unsure*/
    {"earth", NULL, 1, 0.1, 0.9, 20, 80, -30, 30, 0, 4999, 0}, /* tundra */
    {"swamp", NULL, 1, 1.0, 9.9, 50, 100, -30, 10, 0, 4000, 0},/* cold marsh */
    {"earth", NULL, 1, 0.0, 99.9, 0, 100, -99, 99, 0, 99999, 0}, /* debug */
    {NULL, NULL, 1, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0}
};

/* This stuff is for creating the images. */

/* Colour offsets into pixel array. */
#define RED   0
#define GREEN 1
#define BLUE  2

/**
 * Colours used for wind directions.
 * winddir is the directoin wind is coming from.
 *   812  456
 *   7 3  3 7
 *   654  218
 */
static const uint32 directions[] = {
    0x0000FFFF, /* south */
    0x000000FF, /* south west */
    0x00FF00FF, /* west */
    0x00FFFFFF, /* north west */
    0x00000000, /* north */
    0x00FF0000, /* north east */
    0x00FFFF00, /* east */
    0x0000FF00  /* south east */
};

/**
 * Colours used for weather types.
 */
static const uint32 skies[] = {
    0x000000FF, /* SKY_CLEAR         0 */
    0x000000BD, /* SKY_LIGHTCLOUD    1 */
    0x0000007E, /* SKY_OVERCAST      2 */
    0x0000FF00, /* SKY_LIGHT_RAIN    3 */
    0x0000BD00, /* SKY_RAIN          4 */
    0x00007E00, /* SKY_HEAVY_RAIN    5 */
    0x00FFFF00, /* SKY_HURRICANE     6 */
/* wierd weather 7-12 */
    0x00FF0000, /* SKY_FOG           7 */
    0x00FF00FF, /* SKY_HAIL          8 */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
/* snow */
    0x003F3F3F, /* SKY_LIGHT_SNOW    13 */
    0x007E7E7E, /* SKY_SNOW          14 */
    0x00BDBDBD, /* SKY_HEAVY_SNOW    15 */
    0x00FFFFFF  /* SKY_BLIZZARD      16 */
};


/**
 * Set the darkness level for a map.  Requires the map pointer.
 */
void set_darkness_map(mapstruct *m) {
    int i;
    timeofday_t tod;

    if (!m->outdoor) {
        return;
    }

    get_tod(&tod);
    m->darkness = 0;
    for (i = HOURS_PER_DAY/2; i < HOURS_PER_DAY; i++) {
        change_map_light(m, season_timechange[tod.season][i]);
    }
    for (i = 0; i <= tod.hour; i++) {
        change_map_light(m, season_timechange[tod.season][i]);
    }
}

/**
 * Compute the darkness level for all maps in the game.  Requires the
 * time of day as an argument.
 */
static void dawn_to_dusk(const timeofday_t *tod) {
    mapstruct *m;

    /* If the light level isn't changing, no reason to do all
     * the work below.
     */
    if (season_timechange[tod->season][tod->hour] == 0) {
        return;
    }

    for (m = first_map; m != NULL; m = m->next) {
        if (!m->outdoor) {
            continue;
        }

        change_map_light(m, season_timechange[tod->season][tod->hour]);
    }
}

/**
 * This performs the basic function of advancing the clock one tick
 * forward.  Every 20 ticks, the clock is saved to disk.  It is also
 * saved on shutdown.  Any time dependant functions should be called
 * from this function, and probably be passed tod as an argument.
 * Please don't modify tod in the dependant function.
 */
void tick_the_clock(void) {
    timeofday_t tod;

    todtick++;
    if (todtick%20 == 0) {
        write_todclock();
    }
    if (settings.dynamiclevel > 0) {
        if (todtick%21 == 0) {
            write_pressuremap();
        }
        if (todtick%22 == 0) {
            write_winddirmap();
        }
        if (todtick%23 == 0) {
            write_windspeedmap();
        }
        if (todtick%24 == 0) {
            write_humidmap();
        }
/*      if (todtick%25 == 0) {
            write_elevmap();
        }*/
        if (todtick%26 == 0) {
            write_temperaturemap();
        }
        if (todtick%27 == 0) {
            write_gulfstreammap();
        }
        if (todtick%28 == 0 && settings.fastclock > 0) {
            write_skymap();
        }
        if (todtick%29 == 0) {
            write_rainfallmap();
        }
    }
    get_tod(&tod);
    dawn_to_dusk(&tod);
    /* call the weather calculators, here, in order */
    if (settings.dynamiclevel > 0) {
        perform_pressure();     /* pressure is the random factor */
        smooth_wind();          /* calculate the wind. depends on pressure */
        plot_gulfstream();
        update_humid();
        init_temperature();
        compute_sky();
        if (tod.hour == 0) {
            process_rain();
        }
    }
    /* perform_weather must follow calculators */
    perform_weather();
    if (settings.dynamiclevel > 0) {
        write_weather_images();
        spin_globe();
    }
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
static void write_skymap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/skymap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%d ", weathermap[x][y].sky);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

/* pressure */
static void write_pressuremap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/pressuremap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%d ", weathermap[x][y].pressure);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

static void read_pressuremap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/pressuremap", settings.localdir);
    LOG(llevDebug, "Reading pressure data from %s...\n", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open %s for reading\n", filename);
        LOG(llevDebug, "Initializing pressure maps...\n");
        init_pressure();
        write_pressuremap();
        LOG(llevDebug, "Done\n");
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fscanf(fp, "%hd ", &weathermap[x][y].pressure);
            if (weathermap[x][y].pressure < 960 ||
                weathermap[x][y].pressure > 1040) {
                weathermap[x][y].pressure = rndm(960, 1040);
            }
        }
        fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

static void init_pressure(void) {
    int x, y;
    int l, n, k, r;

    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            weathermap[x][y].pressure = 1000;
        }
    }

    for (l = 0; l < PRESSURE_ITERATIONS; l++) {
        x = rndm(0, WEATHERMAPTILESX-1);
        y = rndm(0, WEATHERMAPTILESY-1);
        n = rndm(PRESSURE_MIN, PRESSURE_MAX);
        for (k = 1; k < PRESSURE_AREA; k++) {
            r = rndm(0, 3);
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
    for (l = 0; l < PRESSURE_SPIKES; l++) {
        x = rndm(0, WEATHERMAPTILESX-1);
        y = rndm(0, WEATHERMAPTILESY-1);
        n = rndm(500, 2000);
        weathermap[x][y].pressure = n;
    }
    smooth_pressure();
}

/* winddir */
static void write_winddirmap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/winddirmap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%d ", weathermap[x][y].winddir);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

static void read_winddirmap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y, d;

    snprintf(filename, sizeof(filename), "%s/winddirmap", settings.localdir);
    LOG(llevDebug, "Reading wind direction data from %s...\n", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open %s for reading\n", filename);
        LOG(llevDebug, "Initializing wind maps...\n");
        init_wind();
        write_winddirmap();
        LOG(llevDebug, "Done\n");
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fscanf(fp, "%d ", &d);
            weathermap[x][y].winddir = d;
            if (weathermap[x][y].winddir < 1 ||
                weathermap[x][y].winddir > 8) {
                weathermap[x][y].winddir = rndm(1, 8);
            }
        }
        fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/* windspeed */
static void write_windspeedmap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/windspeedmap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%d ", weathermap[x][y].windspeed);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

static void read_windspeedmap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y, d;

    snprintf(filename, sizeof(filename), "%s/windspeedmap", settings.localdir);
    LOG(llevDebug, "Reading wind speed data from %s...\n", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open %s for reading\n", filename);
        LOG(llevDebug, "Initializing wind maps...\n");
        init_wind();
        write_windspeedmap();
        LOG(llevDebug, "Done\n");
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fscanf(fp, "%d ", &d);
            weathermap[x][y].windspeed = d;
            if (weathermap[x][y].windspeed < 0 ||
                weathermap[x][y].windspeed > 120) {
                weathermap[x][y].windspeed = rndm(1, 30);
            }
        }
        fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/* initialize the wind randomly. Does both direction and speed in one pass */
static void init_wind(void) {
    int x, y;

    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            weathermap[x][y].winddir = rndm(1, 8);
            weathermap[x][y].windspeed = rndm(1, 10);
        }
    }
}

/* gulf stream */
static void write_gulfstreammap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/gulfstreammap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    for (x = 0; x < GULF_STREAM_WIDTH; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%d ", gulf_stream_speed[x][y]);
        }
        fprintf(fp, "\n");
    }
    for (x = 0; x < GULF_STREAM_WIDTH; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%d ", gulf_stream_dir[x][y]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

static void read_gulfstreammap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/gulfstreammap", settings.localdir);
    LOG(llevDebug, "Reading gulf stream data from %s...\n", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open %s for reading\n", filename);
        LOG(llevDebug, "Initializing gulf stream maps...\n");
        init_gulfstreammap();
        write_gulfstreammap();
        LOG(llevDebug, "Done\n");
        return;
    }
    for (x = 0; x < GULF_STREAM_WIDTH; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fscanf(fp, "%d ", &gulf_stream_speed[x][y]);
            if (gulf_stream_speed[x][y] < 0 ||
                gulf_stream_speed[x][y] > 120) {
                gulf_stream_speed[x][y] = rndm(GULF_STREAM_BASE_SPEED, GULF_STREAM_BASE_SPEED+10);
            }
        }
        fscanf(fp, "\n");
    }
    for (x = 0; x < GULF_STREAM_WIDTH; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fscanf(fp, "%d ", &gulf_stream_dir[x][y]);
            if (gulf_stream_dir[x][y] < 0 ||
                gulf_stream_dir[x][y] > 120) {
                gulf_stream_dir[x][y] = rndm(1, 8);
            }
        }
        fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

static void init_gulfstreammap(void) {
    int x, y, tx;

    /* build a gulf stream */
    x = rndm(GULF_STREAM_WIDTH, WEATHERMAPTILESX-GULF_STREAM_WIDTH);
    /* doth the great bob inhale or exhale? */
    gulf_stream_direction = rndm(0, 1);
    gulf_stream_start = x;

    if (gulf_stream_direction) {
        for (y = WEATHERMAPTILESY-1; y >= 0; y--) {
            switch (rndm(0, 6)) {
            case 0:
            case 1:
            case 2:
                for (tx = 0; tx < GULF_STREAM_WIDTH; tx++) {
                    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED, GULF_STREAM_BASE_SPEED+10);
                    if (x == 0) {
                        gulf_stream_dir[tx][y] = 7;
                    } else {
                        gulf_stream_dir[tx][y] = 8;
                        if (tx == 0) {
                            x--;
                        }
                    }
                }
                break;

            case 3:
                for (tx = 0; tx < GULF_STREAM_WIDTH; tx++) {
                    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED, GULF_STREAM_BASE_SPEED+10);
                    gulf_stream_dir[tx][y] = 7;
                }
                break;

            case 4:
            case 5:
            case 6:
                for (tx = 0; tx < GULF_STREAM_WIDTH; tx++) {
                    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED, GULF_STREAM_BASE_SPEED+10);
                    if (x == WEATHERMAPTILESX-1) {
                        gulf_stream_dir[tx][y] = 7;
                    } else {
                        gulf_stream_dir[tx][y] = 6;
                        if (tx == 0) {
                            x++;
                        }
                    }
                }
                break;
            }
        }
    } else { /* go right to left */
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            switch(rndm(0, 6)) {
            case 0:
            case 1:
            case 2:
                for (tx = 0; tx < GULF_STREAM_WIDTH; tx++) {
                    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED, GULF_STREAM_BASE_SPEED+10);
                    if (x == 0) {
                        gulf_stream_dir[tx][y] = 3;
                    } else {
                        gulf_stream_dir[tx][y] = 2;
                        if (tx == 0) {
                            x--;
                        }
                    }
                }
                break;

            case 3:
                for (tx = 0; tx < GULF_STREAM_WIDTH; tx++) {
                    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED, GULF_STREAM_BASE_SPEED+10);
                    gulf_stream_dir[tx][y] = 3;
                }
                break;

            case 4:
            case 5:
            case 6:
                for (tx = 0; tx < GULF_STREAM_WIDTH; tx++) {
                    gulf_stream_speed[tx][y] = rndm(GULF_STREAM_BASE_SPEED, GULF_STREAM_BASE_SPEED+10);
                    if (x == WEATHERMAPTILESX-1) {
                        gulf_stream_dir[tx][y] = 3;
                    } else {
                        gulf_stream_dir[tx][y] = 4;
                        if (tx == 0) {
                            x++;
                        }
                    }
                }
                break;
            }
        }
    } /* done */
}

/* humidity */
static void write_humidmap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/humidmap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%d ", weathermap[x][y].humid);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

static void read_humidmap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y, d;

    snprintf(filename, sizeof(filename), "%s/humidmap", settings.localdir);
    LOG(llevDebug, "Reading humidity data from %s...\n", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open %s for reading\n", filename);
        LOG(llevDebug, "Initializing humidity and elevation maps...\n");
        init_humid_elev();
        write_elevmap();
        write_humidmap();
        write_watermap();
        LOG(llevDebug, "Done\n");
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fscanf(fp, "%d ", &d);
            weathermap[x][y].humid = d;
            if (weathermap[x][y].humid < 0 ||
                weathermap[x][y].humid > 100) {
                weathermap[x][y].humid = rndm(0, 100);
            }
        }
        fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/* average elevation */
static void write_elevmap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/elevmap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%d ", weathermap[x][y].avgelev);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

static void read_elevmap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/elevmap", settings.localdir);
    LOG(llevDebug, "Reading elevation data from %s...\n", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open %s for reading\n", filename);
        /* initializing these is expensive, and should have been done
           by the humidity.  It's not worth the wait to do it twice. */
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fscanf(fp, "%d ", &weathermap[x][y].avgelev);
            if (weathermap[x][y].avgelev < -10000 ||
                weathermap[x][y].avgelev > 15000) {
                weathermap[x][y].avgelev = rndm(-1000, 10000);
            }
        }
        fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/* water % */
static void write_watermap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/watermap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%d ", weathermap[x][y].water);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

static void read_watermap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y, d;

    snprintf(filename, sizeof(filename), "%s/watermap", settings.localdir);
    LOG(llevDebug, "Reading water data from %s...\n", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open %s for reading\n", filename);
        /* initializing these is expensive, and should have been done
           by the humidity.  It's not worth the wait to do it twice. */
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fscanf(fp, "%d ", &d);
            weathermap[x][y].water = d;
            if (weathermap[x][y].water > 100) {
                weathermap[x][y].water = rndm(0, 100);
            }
        }
        fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/**
 * initialize both humidity and elevation
 */
static void init_humid_elev(void) {
    int x, y, tx, ty, nx, ny, ax, ay, j;
    int spwtx, spwty;
    const char *mapname;
    long int elev;
    int water, space;
    mapstruct *m;

    /* handling of this is kinda nasty.  For that reason,
     * we do the elevation here too.  Not because it makes the
     * code cleaner, or makes handling easier, but because I do *not*
     * want to maintain two of these nightmares.
     */

    spwtx = (settings.worldmaptilesx*settings.worldmaptilesizex)/WEATHERMAPTILESX;
    spwty = (settings.worldmaptilesy*settings.worldmaptilesizey)/WEATHERMAPTILESY;
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            water = 0;
            elev = 0;
            nx = 0;
            ny = 0;
            space = 0;

            /* top left */
            mapname = weathermap_to_worldmap_corner(x, y, &tx, &ty, 8);
            m = load_original_map(mapname, 0);
            if (m == NULL) {
                continue;
            }

            m = load_overlay_map(mapname, m);
            if (m == NULL) {
                continue;
            }

            for (nx = 0, ax = tx; nx < spwtx && ax < settings.worldmaptilesizex && space < spwtx*spwty; ax++, nx++) {
                for (ny = 0, ay = ty; ny < spwty && ay < settings.worldmaptilesizey && space < spwtx*spwty; ay++, ny++, space++) {
                    if(GET_MAP_OB(m, ax, ay)) {
                        if (QUERY_FLAG(GET_MAP_OB(m, ax, ay), FLAG_IS_WATER)) {
                            water++;
                        }
                        elev += GET_MAP_OB(m, ax, ay)->elevation;
                    }
                }
            }
            delete_map(m);

            /* bottom left */
            mapname = weathermap_to_worldmap_corner(x, y, &tx, &ty, 6);
            m = load_original_map(mapname, 0);
            if (m == NULL) {
                continue;
            }

            m = load_overlay_map(mapname, m);
            if (m == NULL) {
                continue;
            }

            j = ny;
            for (nx = 0, ax = tx; nx < spwtx && ax < settings.worldmaptilesizex && space < spwtx*spwty; ax++, nx++) {
                for (ny = j, ay = MAX(0, ty-(spwty-1)); ny < spwty && ay <= ty && space < spwtx*spwty; space++, ay++, ny++) {
                    if(GET_MAP_OB(m, ax, ay)) {
                        if (QUERY_FLAG(GET_MAP_OB(m, ax, ay), FLAG_IS_WATER)) {
                            water++;
                        }
                        elev += GET_MAP_OB(m, ax, ay)->elevation;
                    }
                }
            }
            delete_map(m);

            /* top right */
            mapname = weathermap_to_worldmap_corner(x, y, &tx, &ty, 2);
            m = load_original_map(mapname, 0);
            if (m == NULL) {
                continue;
            }

            m = load_overlay_map(mapname, m);
            if (m == NULL) {
                continue;
            }

            for (ax = MAX(0, tx-(spwtx-1)); nx < spwtx && ax < tx && space < spwtx*spwty; ax++, nx++) {
                for (ny = 0, ay = ty; ny < spwty && ay < settings.worldmaptilesizey && space < spwtx*spwty; ay++, ny++, space++) {
                    if(GET_MAP_OB(m, ax, ay)) {
                        if (QUERY_FLAG(GET_MAP_OB(m, ax, ay), FLAG_IS_WATER)) {
                            water++;
                        }
                        elev += GET_MAP_OB(m, ax, ay)->elevation;
                    }
                }
            }
            delete_map(m);

            /* bottom left */
            mapname = weathermap_to_worldmap_corner(x, y, &tx, &ty, 4);
            m = load_original_map(mapname, 0);
            if (m == NULL) {
                continue;
            }

            m = load_overlay_map(mapname, m);
            if (m == NULL) {
                continue;
            }

            for (nx = 0, ax = MAX(0, tx - (spwtx-1)); nx < spwtx && ax < tx && space < spwtx*spwty; ax++, nx++) {
                for (ny = 0, ay = MAX(0, ty-(spwty-1)); ny < spwty && ay <= ty && space < spwtx*spwty; space++, ay++, ny++) {
                    if(GET_MAP_OB(m, ax, ay)) {
                        if (QUERY_FLAG(GET_MAP_OB(m, ax, ay), FLAG_IS_WATER)) {
                            water++;
                        }
                        elev += GET_MAP_OB(m, ax, ay)->elevation;
                    }
                }
            }
            delete_map(m);

            /* jesus thats confusing as all hell */
            weathermap[x][y].humid = water*100/(spwtx*spwty);
            weathermap[x][y].avgelev = elev/(spwtx*spwty);
            weathermap[x][y].water = weathermap[x][y].humid;
        }
    }

    /* and this does all the real work */
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            weathermap[x][y].humid = humid_tile(x, y);
        }
    }
}

/* temperature */
static void write_temperaturemap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/temperaturemap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%d ", weathermap[x][y].temp);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

static void read_temperaturemap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/temperaturemap", settings.localdir);
    LOG(llevDebug, "Reading temperature data from %s...\n", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open %s for reading\n", filename);
        init_temperature();
        write_temperaturemap();
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fscanf(fp, "%hd ", &weathermap[x][y].temp);
            if (weathermap[x][y].temp < -30 ||
                weathermap[x][y].temp > 60) {
                weathermap[x][y].temp = rndm(-10, 40);
            }
        }
        fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

static void init_temperature(void) {
    int x, y;
    timeofday_t tod;

    get_tod(&tod);
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            temperature_calc(x, y, &tod);
        }
    }
}

/* rainfall */
static void write_rainfallmap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/rainfallmap", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fprintf(fp, "%u ", weathermap[x][y].rainfall);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

static void read_rainfallmap(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;

    snprintf(filename, sizeof(filename), "%s/rainfallmap", settings.localdir);
    LOG(llevDebug, "Reading rainfall data from %s...\n", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open %s for reading\n", filename);
        init_rainfall();
        write_rainfallmap();
        return;
    }
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            fscanf(fp, "%u ", &weathermap[x][y].rainfall);
        }
        fscanf(fp, "\n");
    }
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

static void init_rainfall(void)
{
    int x, y;
    int days;

    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            days = todtick/HOURS_PER_DAY;
            if (weathermap[x][y].humid < 10) {
                weathermap[x][y].rainfall = days/20;
            } else if (weathermap[x][y].humid < 20) {
                weathermap[x][y].rainfall = days/15;
            } else if (weathermap[x][y].humid < 30) {
                weathermap[x][y].rainfall = days/10;
            } else if (weathermap[x][y].humid < 40) {
                weathermap[x][y].rainfall = days/5;
            } else if (weathermap[x][y].humid < 50) {
                weathermap[x][y].rainfall = days/2;
            } else if (weathermap[x][y].humid < 60) {
                weathermap[x][y].rainfall = days;
            } else if (weathermap[x][y].humid < 80) {
                weathermap[x][y].rainfall = days*2;
            } else {
                weathermap[x][y].rainfall = days*3;
            }
        }
    }
}

/* END of read/write/init */

static void init_weatheravoid(weather_avoids_t wa[]) {
    int i;
    for (i = 0; wa[i].name != NULL; i++) {
        wa[i].what = find_archetype(wa[i].name);
    }
}

static int wmperformstartx;
static int wmperformstarty;

/**
 * This function initializes the weather system.  It should be called once,
 * at game startup only.
 */
void init_weather(void) {
    int y, tx, ty;
    char filename[MAX_BUF];
    FILE *fp;

    /* all this stuff needs to be set, otherwise this function will cause
     * chaos and destruction.
     */
    if (settings.dynamiclevel < 1) {
        return;
    }

    if (settings.worldmapstartx < 1 || settings.worldmapstarty < 1 ||
        settings.worldmaptilesx < 1 || settings.worldmaptilesy < 1 ||
        settings.worldmaptilesizex < 1 || settings.worldmaptilesizex < 1) {
        return;
    }

    /*prepare structures used for avoidance*/
    init_weatheravoid (weather_avoids);
    init_weatheravoid (growth_avoids);

    LOG(llevDebug, "Initializing the weathermap...\n");

    weathermap = (weathermap_t **)malloc(sizeof(weathermap_t *)*WEATHERMAPTILESX);
    if (weathermap == NULL) {
        fatal(OUT_OF_MEMORY);
    }
    for (y = 0; y < WEATHERMAPTILESY; y++) {
        weathermap[y] = (weathermap_t *)malloc(sizeof(weathermap_t)*WEATHERMAPTILESY);
        if (weathermap[y] == NULL) {
            fatal(OUT_OF_MEMORY);
        }
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
    for (tx = 0; tx < GULF_STREAM_WIDTH; tx++) {
        for (ty = 0; ty < WEATHERMAPTILESY-1; ty++) {
            if (gulf_stream_direction) {
                switch (gulf_stream_dir[tx][ty]) {
                case 2: gulf_stream_dir[tx][ty] = 6; break;
                case 3: gulf_stream_dir[tx][ty] = 7; break;
                case 4: gulf_stream_dir[tx][ty] = 8; break;
                }
            } else {
                switch (gulf_stream_dir[tx][ty]) {
                case 6: gulf_stream_dir[tx][ty] = 2; break;
                case 7: gulf_stream_dir[tx][ty] = 3; break;
                case 8: gulf_stream_dir[tx][ty] = 4; break;
                }
            }
        }
    }

    gulf_stream_start = rndm(GULF_STREAM_WIDTH, WEATHERMAPTILESY-GULF_STREAM_WIDTH);
    read_rainfallmap();

    LOG(llevDebug, "Done reading weathermaps\n");
    snprintf(filename, sizeof(filename), "%s/wmapcurpos", settings.localdir);
    LOG(llevDebug, "Reading current weather position from %s...\n", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Can't open %s.\n", filename);
        wmperformstartx = -1;
        return;
    }

    fscanf(fp, "%d %d", &wmperformstartx, &wmperformstarty);
    LOG(llevDebug, "curposx=%d curposy=%d\n", wmperformstartx, wmperformstarty);
    fclose(fp);
    if (wmperformstartx > settings.worldmaptilesx) {
        wmperformstartx = -1;
    }
    if (wmperformstarty > settings.worldmaptilesy) {
        wmperformstarty = 0;
    }
}

/**
 * Frees all memory allocated by the weather system.
 */
void free_weather(void) {
    int y;

    for (y = 0; y < WEATHERMAPTILESY; y++) {
        FREE_AND_CLEAR(weathermap[y]);
    }
    FREE_AND_CLEAR(weathermap);
}

/**
 * This routine slowly loads the world, patches it up due to the weather,
 * and saves it back to disk.  In this way, the world constantly feels the
 * effects of weather uniformly, without relying on players wandering.
 *
 * The main point of this is stuff like growing herbs, soil, decaying crap,
 * etc etc etc.  Not actual *weather*, but weather *effects*.
 */
static void perform_weather(void) {
    mapstruct *m;
    char filename[MAX_BUF];
    FILE *fp;

    if (!settings.dynamiclevel) {
        return;
    }

    /* move right to left, top to bottom */
    if (wmperformstartx+1 == settings.worldmaptilesx) {
        wmperformstartx = 0;
        wmperformstarty++;
    } else {
        wmperformstartx++;
    }
    if (wmperformstarty == settings.worldmaptilesy) {
        wmperformstartx = wmperformstarty = 0;
    }

    snprintf(filename, sizeof(filename), "world/world_%d_%d", wmperformstartx+settings.worldmapstartx, wmperformstarty+settings.worldmapstarty);

    m = ready_map_name(filename, 0);
    if (m == NULL) {
        return; /* hrmm */
    }

    /* for now, all we do is decay stuff.  more to come */
    decay_objects(m);
    weather_effect(filename);

    /* done */
    save_map(m, 2); /* write the overlay */
    m->in_memory = MAP_IN_MEMORY; /*reset this*/
    snprintf(filename, sizeof(filename), "%s/wmapcurpos", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }

    if (players_on_map(m, TRUE) == 0) {
        delete_map(m);
    }

    fprintf(fp, "%d %d", wmperformstartx, wmperformstarty);
    fclose(fp);
}

/**
 * perform actual effect of weather.  Should be called from perform_weather,
 * or when a map is loaded. (player enter map).  Filename is the name of
 * the map.  The map *must allready be loaded*.
 *
 * This is where things like snow, herbs, earthly rototilling, etc should
 * occur.
 */
void weather_effect(const char *filename) {
    mapstruct *m;
    int wx, wy, x, y;

    /* if the dm shut off weather, go home */
    if (settings.dynamiclevel < 1) {
        return;
    }

    m = ready_map_name(filename, 0);
    if (!m->outdoor) {
        return;
    }

    x = 0;
    y = 0;
    /* for now, just bail if it's not the worldmap */
    if (worldmap_to_weathermap(x, y, &wx, &wy, m) != 0) {
        return;
    }

    /*First, calculate temperature*/
    calculate_temperature(m, wx, wy);
    /* we change the world first, if needed */
    if (settings.dynamiclevel >= 5) {
        change_the_world(m, wx, wy);
    }
    if (settings.dynamiclevel >= 2) {
        let_it_snow(m, wx, wy);
        singing_in_the_rain(m, wx, wy);
    }
    /*if (settings.dynamiclevel >= 4) {
        feather_map(m, wx, wy, filename);
    }*/
    if (settings.dynamiclevel >= 3) {
        plant_a_garden(m, wx, wy);
    }
}

/**
 * Check the current square to see if we should avoid this one for
 * weather processing.  Must pass av and gs, which will be filled in
 * with 1 or 0.  gs will be 1 if we found snow/rain here.  av will be
 * 1 if we should avoid processing this tile. (don't rain on lakes)
 * x and y are the coordinates inside the current map m. If grow is
 * 1, we use the growth table, rather than the avoidance table.
 *
 * Returns the object pointer for any snow item it found, so you can
 * destroy/melt it.
 */
static object *avoid_weather(int *av, mapstruct *m, int x, int y, int *gs, int grow) {
    int avoid, gotsnow, i;
    object *tmp, *snow;

    avoid = 0;
    gotsnow = 0;
    if (grow) {
        for (tmp = GET_MAP_OB(m, x, y); tmp; tmp = tmp->above) {
            /* look for things like walls, holes, etc */
            if (!QUERY_FLAG(tmp, FLAG_IS_FLOOR) && !(tmp->material&M_ICE || tmp->material&M_LIQUID)) {
                gotsnow++;
                snow = tmp;
            }
            for (i = 0; growth_avoids[i].name != NULL; i++) {
                if (tmp->arch == growth_avoids[i].what) {
                    avoid++;
                    break;
                }
            }
            if (!strncmp(tmp->arch->name, "biglake_", 8)) {
                avoid++;
                break;
            }
            if (avoid && gotsnow) {
                break;
            }
        }
    } else {
        for (tmp = GET_MAP_OB(m, x, y); tmp; tmp = tmp->above) {
            for (i = 0; weather_avoids[i].name != NULL; i++) {
                /*if (!strcmp(tmp->arch->name, weather_avoids[i].name)) {*/
                if (tmp->arch == weather_avoids[i].what) {
                    if (weather_avoids[i].snow == 1) {
                        gotsnow++;
                        snow = tmp;
                    } else {
                        avoid++;
                    }
                    break;
                }
            }
            if (avoid && gotsnow) {
                break;
            }
        }
    }
    *gs = gotsnow;
    *av = avoid;

    return snow;
}

/**
 * Temperature is used in a lot of weather function.
 * This need to be precalculated before used.
 */
static void calculate_temperature(mapstruct *m, int wx, int wy) {
    int x,y;
    for (x = 0; x < settings.worldmaptilesizex; x++) {
        for (y = 0; y < settings.worldmaptilesizey; y++) {
            worldmap_to_weathermap(x, y, &wx, &wy, m);
            weathermap[wx][wy].realtemp = real_world_temperature(x, y, m);
        }
    }
}

/**
 * Process snow.  m is the map we are currently processing.  wx and wy are
 * the weathermap coordinates for the weathermap square we want to work on.
 * filename is the pathname for the current map.  This should be called from
 * weather_effect()
 */
static void let_it_snow(mapstruct *m, int wx, int wy) {
    int x, y, i;
    int nx, ny, j, d;
    int avoid, two, temp, sky, gotsnow, found, nodstk;
    const char *doublestack, *doublestack2;
    object *ob, *tmp, *oldsnow, *topfloor;
    archetype *at;

    for (nx = 0; nx < settings.worldmaptilesizex; nx++) {
        for (ny = 0; ny < settings.worldmaptilesizey; ny++) {
            /* jitter factor */
            if (rndm(0, 2) > 0) {
                x = y = d = -1;
                while (OUT_OF_REAL_MAP(m, x, y)) {
                    d++;
                    j = rndm(1, 8);
                    x = nx+freearr_x[j]*(rndm(0, 1)+rndm(0, 1)+rndm(0, 1)+1);
                    y = ny+freearr_y[j]*(rndm(0, 1)+rndm(0, 1)+rndm(0, 1)+1);
                    if (d > 15) {
                        x = nx;
                        y = ny;
                    }
                }
            } else {
                x = nx;
                y = ny;
            }
            /* we use the unjittered coordinates */
            (void)worldmap_to_weathermap(nx, ny, &wx, &wy, m);
            ob = NULL;
            at = NULL;
            /* this will definately need tuning */
            avoid = 0;
            two = 0;
            gotsnow = 0;
            nodstk = 0;
            /*temp = real_world_temperature(x, y, m);*/
            temp = weathermap[wx][wy].realtemp;
            sky = weathermap[wx][wy].sky;
            if (temp <= 0 && sky > SKY_OVERCAST && sky < SKY_FOG) {
                sky += 10; /*let it snow*/
            }
            oldsnow = avoid_weather(&avoid, m, x, y, &gotsnow, 0);
            if (!avoid) {
                if (sky >= SKY_LIGHT_SNOW && sky < SKY_HEAVY_SNOW) {
                    at = find_archetype(weather_replace[0].special_snow);
                }
                if (sky >= SKY_HEAVY_SNOW) {
                    at = find_archetype(weather_replace[1].special_snow);
                }
                if (sky >= SKY_LIGHT_SNOW) {
                    /* the bottom floor of scorn is not IS_FLOOR */
                    topfloor = NULL;
                    for (tmp = GET_MAP_OB(m, x, y); tmp; topfloor = tmp, tmp = tmp->above) {
                        if (strcmp(tmp->arch->name, "dungeon_magic") != 0) {
                            if (!QUERY_FLAG(tmp, FLAG_IS_FLOOR)) {
                                break;
                            }
                        }
                    }
                    /* topfloor should now be the topmost IS_FLOOR=1 */
                    if (topfloor == NULL) {
                        continue;
                    }
                    if (tmp != NULL) {
                        nodstk++;
                    }
                    /* something is wrong with that sector. just skip it */
                    found = 0;
                    for (i = 0; weather_replace[i].tile != NULL; i++) {
                        if (weather_replace[i].arch_or_name == 1) {
                            if (!strcmp(topfloor->arch->name, weather_replace[i].tile)) {
                                found++;
                            }
                        } else {
                            if (!strcmp(topfloor->name, weather_replace[i].tile)) {
                                found++;
                            }
                        }
                        if (found) {
                            if (weather_replace[i].special_snow != NULL) {
                                at = find_archetype(weather_replace[i].special_snow);
                            }
                            if (weather_replace[i].doublestack_arch != NULL && !nodstk) {
                                two++;
                                doublestack = weather_replace[i].doublestack_arch;
                            }
                            break;
                        }
                    }
                }
                if (gotsnow && at) {
                    if (!strcmp(oldsnow->arch->name, at->name)) {
                        at = NULL;
                    } else {
                        remove_ob(oldsnow);
                        free_object(oldsnow);
                        tmp = GET_MAP_OB(m, x, y);
                        /* clean up the trees we put over the snow */
                        found = 0;
                        doublestack2 = NULL;
                        if (tmp) {
                            for (i = 0; weather_replace[i].tile != NULL; i++) {
                                if (weather_replace[i].doublestack_arch == NULL) {
                                    continue;
                                }

                                if (weather_replace[i].arch_or_name == 1) {
                                    if (!strcmp(tmp->arch->name, weather_replace[i].tile)) {
                                        found++;
                                    }
                                } else {
                                    if (!strcmp(tmp->name, weather_replace[i].tile)) {
                                        found++;
                                    }
                                }
                                if (found) {
                                    tmp = tmp->above;
                                    doublestack2 = weather_replace[i].doublestack_arch;
                                    break;
                                }
                            }
                        }
                        if (tmp != NULL && doublestack2 != NULL) {
                            if (strcmp(tmp->arch->name, doublestack2) == 0) {
                                remove_ob(tmp);
                                free_object(tmp);
                            }
                        }
                    }
                }
                if (at != NULL) {
                    ob = get_object();
                    copy_object(&at->clone, ob);
                    ob->x = x;
                    ob->y = y;
                    ob->material = M_ICE;
                    SET_FLAG(ob, FLAG_OVERLAY_FLOOR);
                    CLEAR_FLAG(ob, FLAG_IS_FLOOR);
                    insert_ob_in_map(ob, m, ob, INS_NO_MERGE|INS_NO_WALK_ON|INS_ABOVE_FLOOR_ONLY);
                    if (two) {
                        at = NULL;
                        at = find_archetype(doublestack);
                        if (at != NULL) {
                            ob = get_object();
                            copy_object(&at->clone, ob);
                            ob->x = x;
                            ob->y = y;
                            insert_ob_in_map(ob, m, ob, INS_NO_MERGE|INS_NO_WALK_ON|INS_ON_TOP);
                        }
                    }
                }
            }
            if (temp > 8 && GET_MAP_OB(m, x, y) != NULL) {
                /* melt some snow */
                for (tmp = GET_MAP_OB(m, x, y)->above; tmp; tmp = tmp->above) {
                    avoid = 0;
                    for (i = 0; weather_replace[i].tile != NULL; i++) {
                        if (weather_replace[i].special_snow == NULL) {
                            continue;
                        }

                        if (!strcmp(tmp->arch->name, weather_replace[i].special_snow)) {
                            avoid++;
                        }
                        if (avoid) {
                            break;
                        }
                    }
                    if (avoid) {
                        /* replace snow with a big puddle */
                        remove_ob(tmp);
                        free_object(tmp);
                        tmp = GET_MAP_OB(m, x, y);
                        if (tmp && (!strcmp(tmp->arch->name, "mountain"))) {
                            at = find_archetype("mountain1_rivlets");
                        } else if (tmp && (!strcmp(tmp->arch->name, "mountain2"))) {
                            at = find_archetype("mountain2_rivlets");
                        } else if (tmp && (!strcmp(tmp->arch->name, "mountain4"))) {
                            at = find_archetype("mountain2_rivlets");
                        } else {at = find_archetype("rain5");
                        }
                        if (at != NULL) {
                            ob = get_object();
                            copy_object(&at->clone, ob);
                            ob->x = x;
                            ob->y = y;
                            SET_FLAG(ob, FLAG_OVERLAY_FLOOR);
                            ob->material = M_LIQUID;
                            insert_ob_in_map(ob, m, ob, INS_NO_MERGE|INS_NO_WALK_ON|INS_ABOVE_FLOOR_ONLY);
                        }
                    }
                }
            }
            /* woo it's cold out */
            if (temp < -8) {
                avoid = 0;
                for (tmp = GET_MAP_OB(m, x, y); tmp; tmp = tmp->above) {
                    if (!strcasecmp(tmp->name, "ice")) {
                        avoid--;
                    }
                }
                tmp = GET_MAP_OB(m, x, y);
                if (tmp && (!strcasecmp(tmp->name, "sea"))) {
                    avoid++;
                } else if (tmp && (!strcasecmp(tmp->name, "sea1"))) {
                    avoid++;
                } else if (tmp && (!strcasecmp(tmp->name, "deep_sea"))) {
                    avoid++;
                } else if (tmp && (!strcasecmp(tmp->name, "shallow_sea"))) {
                    avoid++;
                }
                if (avoid > 0) {
                    at = find_archetype("ice");
                    ob = get_object();
                    copy_object(&at->clone, ob);
                    ob->x = x;
                    ob->y = y;
                    insert_ob_in_map(ob, m, ob, INS_NO_MERGE|INS_NO_WALK_ON|INS_ABOVE_FLOOR_ONLY);
                }
            }
        }
    }
}

/**
 * Process rain.  m is the map we are currently processing.  wx and wy are
 * the weathermap coordinates for the weathermap square we want to work on.
 * filename is the pathname for the current map.  This should be called from
 * weather_effect()
 */
static void singing_in_the_rain(mapstruct *m, int wx, int wy) {
    int x, y, i;
    int nx, ny, d, j;
    int avoid, two, temp, sky, gotsnow, found, nodstk;
    object *ob, *tmp, *oldsnow, *topfloor;
    const char *doublestack, *doublestack2;
    archetype *at;

    for (nx = 0; nx < settings.worldmaptilesizex; nx++) {
        for (ny = 0; ny < settings.worldmaptilesizey; ny++) {
            /* jitter factor */
            if (rndm(0, 2) > 0) {
                x = y = d = -1;
                while (OUT_OF_REAL_MAP(m, x, y)) {
                    d++;
                    j = rndm(1, 8);
                    x = nx+freearr_x[j]*(rndm(0, 1)+rndm(0, 1)+rndm(0, 1)+1);
                    y = ny+freearr_y[j]*(rndm(0, 1)+rndm(0, 1)+rndm(0, 1)+1);
                    if (d > 15) {
                        x = nx;
                        y = ny;
                    }
                }
            } else {
                x = nx;
                y = ny;
            }
            /* we use the unjittered coordinates */
            (void)worldmap_to_weathermap(nx, ny, &wx, &wy, m);
            ob = NULL;
            at = NULL;
            avoid = 0;
            two = 0;
            gotsnow = 0;
            nodstk = 0;
            /*temp = real_world_temperature(x, y, m);*/
            temp = weathermap[wx][wy].realtemp;
            sky = weathermap[wx][wy].sky;
            /* it's probably allready snowing */
            if (temp < 0) {
                continue;
            }

            oldsnow = avoid_weather(&avoid, m, x, y, &gotsnow, 0);
            if (!avoid) {
                tmp = GET_MAP_OB(m, x, y);
                if (tmp && (!strcmp(tmp->arch->name, "mountain"))) {
                    at = find_archetype("mountain1_rivlets");
                    break;
                } else if (tmp && (!strcmp(tmp->arch->name, "mountain2"))) {
                    at = find_archetype("mountain2_rivlets");
                    break;
                } else if (tmp && (!strcmp(tmp->arch->name, "mountain4"))) {
                    at = find_archetype("mountain2_rivlets");
                    break;
                }
                if (sky == SKY_LIGHT_RAIN || sky == SKY_RAIN) {
                    switch (rndm(0, SKY_HAIL-sky)) {
                    case 0: at = find_archetype("rain1"); break;
                    case 1: at = find_archetype("rain2"); break;
                    default: at = NULL; break;
                    }
                }
                if (sky >= SKY_HEAVY_RAIN && sky <= SKY_HURRICANE) {
                    switch (rndm(0, SKY_HAIL-sky)) {
                    case 0: at = find_archetype("rain3"); break;
                    case 1: at = find_archetype("rain4"); break;
                    case 2: at = find_archetype("rain5"); break;
                    default: at = NULL; break;
                    }
                }
                /* the bottom floor of scorn is not IS_FLOOR */
                topfloor = NULL;
                for (tmp = GET_MAP_OB(m, x, y); tmp; topfloor = tmp,tmp = tmp->above) {
                    if (strcmp(tmp->arch->name, "dungeon_magic") != 0) {
                        if (!QUERY_FLAG(tmp, FLAG_IS_FLOOR)) {
                            break;
                        }
                    }
                }
                /* topfloor should now be the topmost IS_FLOOR=1 */
                if (topfloor == NULL) {
                    continue;
                }
                if (tmp != NULL) {
                    nodstk++;
                }
                /* something is wrong with that sector. just skip it */
                found = 0;
                for (i = 0; weather_replace[i].tile != NULL; i++) {
                    if (weather_replace[i].arch_or_name == 1) {
                        if (!strcmp(topfloor->arch->name, weather_replace[i].tile)) {
                            found++;
                        }
                    } else {
                        if (!strcmp(topfloor->name, weather_replace[i].tile)) {
                            found++;
                        }
                    }
                    if (found) {
                        if (weather_replace[i].doublestack_arch != NULL && !nodstk) {
                            two++;
                            doublestack = weather_replace[i].doublestack_arch;
                        }
                        break;
                    }
                }
                if (gotsnow && at) {
                    if (!strcmp(oldsnow->arch->name, at->name)) {
                        at = NULL;
                    } else {
                        tmp = GET_MAP_OB(m, x, y);
                        remove_ob(oldsnow);
                        /* clean up the trees we put over the snow */
                        found = 0;
                        doublestack2 = NULL;
                        for (i = 0; weather_replace[i].tile != NULL; i++) {
                            if (weather_replace[i].doublestack_arch == NULL) {
                                continue;
                            }

                            if (weather_replace[i].arch_or_name == 1) {
                                if (!strcmp(tmp->arch->name, weather_replace[i].tile)) {
                                    found++;
                                }
                            } else {
                                if (!strcmp(tmp->name, weather_replace[i].tile)) {
                                    found++;
                                }
                            }
                            if (found) {
                                tmp = tmp->above;
                                doublestack2 = weather_replace[i].doublestack_arch;
                                break;
                            }
                        }
                        free_object(oldsnow);
                        if (tmp != NULL && doublestack2 != NULL) {
                            if (strcmp(tmp->arch->name, doublestack2) == 0) {
                                remove_ob(tmp);
                                free_object(tmp);
                            }
                        }
                    }
                }
                if (at != NULL) {
                    ob = get_object();
                    copy_object(&at->clone, ob);
                    ob->x = x;
                    ob->y = y;
                    SET_FLAG(ob, FLAG_OVERLAY_FLOOR);
                    ob->material = M_LIQUID;
                    insert_ob_in_map(ob, m, ob, INS_NO_MERGE|INS_NO_WALK_ON|INS_ABOVE_FLOOR_ONLY);
                    if (two) {
                        at = find_archetype(doublestack);
                        if (at != NULL) {
                            ob = get_object();
                            copy_object(&at->clone, ob);
                            ob->x = x;
                            ob->y = y;
                            insert_ob_in_map(ob, m, ob, INS_NO_MERGE|INS_NO_WALK_ON|INS_ON_TOP);
                        }
                    }
                }
            }
            /* Things evaporate fast in the heat */
            if (GET_MAP_OB(m, x, y) && temp > 8 && sky < SKY_OVERCAST && rndm(temp, 60) > 50) {
                /* evaporate */
                for (tmp = GET_MAP_OB(m, x, y)->above; tmp; tmp = tmp->above) {
                    avoid = 0;
                    if (!strcmp(tmp->arch->name, "rain1")) {
                        avoid++;
                    } else if (!strcmp(tmp->arch->name, "rain2")) {
                        avoid++;
                    } else if (!strcmp(tmp->arch->name, "rain3")) {
                        avoid++;
                    } else if (!strcmp(tmp->arch->name, "rain4")) {
                        avoid++;
                    } else if (!strcmp(tmp->arch->name, "rain5")) {
                        avoid++;
                    } else if (!strcmp(tmp->arch->name, "mountain1_rivlets")) {
                        avoid++;
                    } else if (!strcmp(tmp->arch->name, "mountain2_rivlets")) {
                        avoid++;
                    }
                    if (avoid) {
                        remove_ob(tmp);
                        free_object(tmp);
                        if (weathermap[wx][wy].humid < 100 && rndm(0, 50) == 0) {
                            weathermap[wx][wy].humid++;
                        }
                        tmp = GET_MAP_OB(m, x, y);
                        /* clean up the trees we put over the rain */
                        found = 0;
                        doublestack2 = NULL;
                        for (i = 0; weather_replace[i].tile != NULL; i++) {
                            if (weather_replace[i].doublestack_arch == NULL) {
                                continue;
                            }
                            if (weather_replace[i].arch_or_name == 1) {
                                if (!strcmp(tmp->arch->name, weather_replace[i].tile)) {
                                    found++;
                                }
                            } else {
                                if (!strcmp(tmp->name, weather_replace[i].tile)) {
                                    found++;
                                }
                            }
                            if (found) {
                                tmp = tmp->above;
                                doublestack2 = weather_replace[i].doublestack_arch;
                                break;
                            }
                        }
                        if (tmp != NULL && doublestack2 != NULL) {
                            if (strcmp(tmp->arch->name, doublestack2) == 0) {
                                remove_ob(tmp);
                                free_object(tmp);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

/**
 * Process growth.  m is the map we are currently processing.  wx and wy are
 * the weathermap coordinates for the weathermap square we want to work on.
 * filename is the pathname for the current map.  This should be called from
 * weather_effect()
 */
static void plant_a_garden(mapstruct *m, int wx, int wy) {
    int x, y, i;
    int avoid, two, temp, sky, gotsnow, found, days;
    object *ob, *tmp;
    archetype *at;

    days = todtick/HOURS_PER_DAY;
    for (x = 0; x < settings.worldmaptilesizex; x++) {
        for (y = 0; y < settings.worldmaptilesizey; y++) {
            (void)worldmap_to_weathermap(x, y, &wx, &wy, m);
            ob = NULL;
            at = NULL;
            avoid = 0;
            two = 0;
            gotsnow = 0;
            /*temp = real_world_temperature(x, y, m);*/
            temp = weathermap[wx][wy].realtemp;
            sky = weathermap[wx][wy].sky;
            (void)avoid_weather(&avoid, m, x, y, &gotsnow, 1);
            if (!avoid) {
                found = 0;
                for (i = 0; weather_grow[i].herb != NULL; i++) {
                    for (tmp = GET_MAP_OB(m, x, y); tmp; tmp = tmp->above) {
                        if (strcmp(tmp->arch->name, weather_grow[i].herb) != 0) {
                            continue;
                        }

                        /* we found there is a herb here allready */
                        found++;
                        if ((float)weathermap[wx][wy].rainfall/days < weather_grow[i].rfmin ||
                            (float)weathermap[wx][wy].rainfall/days > weather_grow[i].rfmax ||
                            weathermap[wx][wy].humid < weather_grow[i].humin ||
                            weathermap[wx][wy].humid > weather_grow[i].humax ||
                            temp < weather_grow[i].tempmin ||
                            temp > weather_grow[i].tempmax ||
                            rndm(0, MIN(weather_grow[i].random/2, 1)) == 0) {
                            /* the herb does not belong, randomly delete
                              herbs to prevent overgrowth. */
                            remove_ob(tmp);
                            free_object(tmp);
                            break;
                        }
                    }
                    /* don't doublestack herbs */
                    if (found) {
                        continue;
                    }
                    /* add a random factor */
                    if (rndm(1, weather_grow[i].random) != 1) {
                        continue;
                    }
                    /* we look up through two tiles for a matching tile */
                    if (weather_grow[i].tile != NULL && GET_MAP_OB(m, x, y) != NULL) {
                        if (strcmp(GET_MAP_OB(m, x, y)->arch->name, weather_grow[i].tile) != 0) {
                            if (GET_MAP_OB(m, x, y)->above != NULL) {
                                if (strcmp(GET_MAP_OB(m, x, y)->above->arch->name, weather_grow[i].tile) != 0) {
                                    continue;
                                }
                            } else {
                                continue;
                            }
                        }
                    }
                    if ((float)weathermap[wx][wy].rainfall/days < weather_grow[i].rfmin ||
                        (float)weathermap[wx][wy].rainfall/days > weather_grow[i].rfmax) {
                        continue;
                    }
                    if (weathermap[wx][wy].humid < weather_grow[i].humin ||
                        weathermap[wx][wy].humid > weather_grow[i].humax) {
                        continue;
                    }
                    if (temp < weather_grow[i].tempmin ||
                        temp > weather_grow[i].tempmax) {
                        continue;
                    }
                    if ((!GET_MAP_OB(m, x, y)) ||
                        GET_MAP_OB(m, x, y)->elevation < weather_grow[i].elevmin ||
                        GET_MAP_OB(m, x, y)->elevation > weather_grow[i].elevmax) {
                        continue;
                    }
                    /* we got this far.. must be a match */
                    at = find_archetype(weather_grow[i].herb);
                    break;
                }
                if (at != NULL) {
                    ob = get_object();
                    copy_object(&at->clone, ob);
                    ob->x = x;
                    ob->y = y;
                    /* XXX is this right?  maybe.. */
                    SET_FLAG(ob, FLAG_OVERLAY_FLOOR);
                    insert_ob_in_map(ob, m, ob, INS_NO_MERGE|INS_NO_WALK_ON|INS_ABOVE_FLOOR_ONLY);
                }
            }
        }
    }
}

/**
 * Process worldmap regrowth.  m is the map we are currently processing.
 * wx and wy are
 * the weathermap coordinates for the weathermap square we want to work on.
 * This should be called from weather_effect()
 */
static void change_the_world(mapstruct *m, int wx, int wy) {
    int x, y, i;
    int nx, ny, j, d;
    int avoid, two, temp, sky, gotsnow, found, days;
    object *ob, *tmp, *doublestack;
    archetype *at, *dat;

    days = todtick/HOURS_PER_DAY;
    for (nx = 0; nx < settings.worldmaptilesizex; nx++) {
        for (ny = 0; ny < settings.worldmaptilesizey; ny++) {
            /* jitter factor */
            if (rndm(0, 2) > 0) {
                x = y = d = -1;
                while (OUT_OF_REAL_MAP(m, x, y)) {
                    d++;
                    j = rndm(1, 8);
                    x = nx+freearr_x[j]*(rndm(0, 1)+rndm(0, 1)+rndm(0, 1)+1);
                    y = ny+freearr_y[j]*(rndm(0, 1)+rndm(0, 1)+rndm(0, 1)+1);
                    if (d > 15) {
                        x = nx;
                        y = ny;
                    }
                }
            } else {
                x = nx;
                y = ny;
            }
            /* we use the unjittered coordinates */
            (void)worldmap_to_weathermap(nx, ny, &wx, &wy, m);
            ob = NULL;
            at = NULL;
            dat = NULL;
            avoid = 0;
            two = 0;
            gotsnow = 0;
            /*temp = real_world_temperature(x, y, m);*/
            temp = weathermap[wx][wy].realtemp;
            sky = weathermap[wx][wy].sky;
            (void)avoid_weather(&avoid, m, x, y, &gotsnow, 1);
            if (!avoid) {
                for (i = 0; weather_tile[i].herb != NULL; i++) {
                    found = 0;
                    doublestack = NULL;
                    if (GET_MAP_OB(m, x, y)) {
                        for (tmp = GET_MAP_OB(m, x, y)->above; tmp; tmp = tmp->above) {
                            if (weather_tile[i].tile != NULL) {
                                if (strcmp(tmp->arch->name, weather_tile[i].tile) == 0) {
                                    doublestack = tmp;
                                    continue;
                                }
                            }
                            if (strcmp(tmp->arch->name, weather_tile[i].herb) != 0) {
                                continue;
                            }

                            if ((float)weathermap[wx][wy].rainfall/days < weather_tile[i].rfmin ||
                                (float)weathermap[wx][wy].rainfall/days > weather_tile[i].rfmax ||
                                weathermap[wx][wy].humid < weather_tile[i].humin ||
                                weathermap[wx][wy].humid > weather_tile[i].humax ||
                                temp < weather_tile[i].tempmin ||
                                temp > weather_tile[i].tempmax) {
                                remove_ob(tmp);
                                free_object(tmp);
                                if (doublestack) {
                                    remove_ob(doublestack);
                                    free_object(doublestack);
                                }
                                break;
                            } else {
                                found++; /* there is one here allready. leave it */
                                break;
                            }
                        }
                    }
                    if (found) {
                        break;
                    }

                    /* add a random factor */
                    if (rndm(1, weather_tile[i].random) != 1) {
                        continue;
                    }
                    if ((float)weathermap[wx][wy].rainfall/days < weather_tile[i].rfmin ||
                        (float)weathermap[wx][wy].rainfall/days > weather_tile[i].rfmax) {
                        continue;
                    }
                    if (weathermap[wx][wy].humid < weather_tile[i].humin ||
                        weathermap[wx][wy].humid > weather_tile[i].humax) {
                        continue;
                    }
                    if (temp < weather_tile[i].tempmin ||
                        temp > weather_tile[i].tempmax) {
                        continue;
                    }
                    if ( (!GET_MAP_OB(m, x, y)) ||
                        GET_MAP_OB(m, x, y)->elevation < weather_tile[i].elevmin ||
                        GET_MAP_OB(m, x, y)->elevation > weather_tile[i].elevmax) {
                        continue;
                    }
                    /* we got this far.. must be a match */
                    if (GET_MAP_OB(m, x, y) && strcmp(GET_MAP_OB(m, x, y)->arch->name, weather_tile[i].herb) == 0) {
                        break; /* no sense in doubling up */
                    }
                    at = find_archetype(weather_tile[i].herb);
                    break;
                }
                if (at != NULL) {
                    if (weather_tile[i].tile != NULL && GET_MAP_OB(m, x, y) && strcmp(weather_tile[i].tile, GET_MAP_OB(m, x, y)->arch->name) != 0) {
                        dat = find_archetype(weather_tile[i].tile);
                    }
                    if (dat != NULL) {
                        ob = get_object();
                        copy_object(&dat->clone, ob);
                        ob->x = x;
                        ob->y = y;
                        insert_ob_in_map(ob, m, ob, INS_NO_MERGE|INS_NO_WALK_ON|INS_ABOVE_FLOOR_ONLY);
                    }
                    if (gotsnow == 0) {
                        ob = get_object();
                        copy_object(&at->clone, ob);
                        ob->x = x;
                        ob->y = y;
                        if (dat != NULL) {
                            insert_ob_in_map(ob, m, ob, INS_NO_MERGE|INS_NO_WALK_ON|INS_ON_TOP);
                        } else {
                            insert_ob_in_map(ob, m, ob, INS_NO_MERGE|INS_NO_WALK_ON|INS_ABOVE_FLOOR_ONLY);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Provide wx and wy. Will fill in with weathermap coordinates.  Requires
 * the current mapname (must be a worldmap), and your coordinates on the
 * map.  returns -1 if you give it something it can't figure out. 0 normally.
*/
int worldmap_to_weathermap(int x, int y, int *wx, int *wy, mapstruct* m) {
    int spwtx, spwty;
    int fx, fy;
    int nx, ny;
    char* filename = m->path;

    spwtx = (settings.worldmaptilesx*settings.worldmaptilesizex)/WEATHERMAPTILESX;
    spwty = (settings.worldmaptilesy*settings.worldmaptilesizey)/WEATHERMAPTILESY;

    while (*filename == '/') {
        filename++;
    }

    fx = MAP_WORLDPARTX(m);
    fy = MAP_WORLDPARTY(m);
    if (fx > settings.worldmapstartx + settings.worldmaptilesx ||
        fx < settings.worldmapstartx ||
        fy > settings.worldmapstarty + settings.worldmaptilesy ||
        fy < settings.worldmapstarty) {
        LOG(llevDebug, "worldmap_to_weathermap(%s)\n", filename);
        sscanf(filename, "world/world_%d_%d", &fx, &fy);
        MAP_WORLDPARTX(m) = fx;
        MAP_WORLDPARTY(m) = fy;
    }
    if (fx > settings.worldmapstartx + settings.worldmaptilesx ||
        fx < settings.worldmapstartx) {
        return -1;
    }
    if (fy > settings.worldmapstarty + settings.worldmaptilesy ||
        fy < settings.worldmapstarty) {
        return -1;
    }
    fx -= settings.worldmapstartx;
    fy -= settings.worldmapstarty;

    nx = fx*settings.worldmaptilesizex+x;
    ny = fy*settings.worldmaptilesizey+y;

    *wx = nx/spwtx;
    *wy = ny/spwty;

    return 0;
}

/**
 * Provide x and y.  Will fill in with the requested corner of the real
 * world map, given the current weathermap section.  dir selects which
 * corner to return.  Valid values are 2 4 6 8 for the corners.  return
 * value is the name of the map that corner resides in.
 */
static const char *weathermap_to_worldmap_corner(int wx, int wy, int *x, int *y, int dir) {
    int spwtx, spwty;
    int tx, ty, nx, ny;
    static char mapname[MAX_BUF];

    spwtx = (settings.worldmaptilesx*settings.worldmaptilesizex)/WEATHERMAPTILESX;
    spwty = (settings.worldmaptilesy*settings.worldmaptilesizey)/WEATHERMAPTILESY;
    switch (dir) {
    case 2: wx++; break;
    case 4: wx++; wy++; break;
    case 6: wy++; break;
    case 8: break;
    }
    if (wx > 0) {
        tx = (wx*spwtx)-1;
    } else {
        tx = wx;
    }
    if (wy > 0) {
        ty = (wy*spwty)-1;
    } else {
        ty = wy;
    }

    nx = (tx/settings.worldmaptilesizex)+settings.worldmapstartx;
    ny = (ty/settings.worldmaptilesizey)+settings.worldmapstarty;
    snprintf(mapname, MAX_BUF, "world/world_%d_%d", nx, ny);

    *x = tx%settings.worldmaptilesizex;
    *y = ty%settings.worldmaptilesizey;
    return mapname;
}

/**
 * Calculates the distance to the nearest pole. x,y are the weathermap
 * coordinates, equator is the current location of the equator.  returns
 * distance as an int.
 */
static int polar_distance(int x, int y, int equator) {
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

/**
 * update the humidity for all weathermap tiles.
 */
static void update_humid(void) {
    int x, y;

    for (y = 0; y < WEATHERMAPTILESY; y++) {
        for (x = 0; x < WEATHERMAPTILESX; x++) {
            weathermap[x][y].humid = humid_tile(x, y);
        }
    }
}

/**
 * calculate the humidity of this tile.  x and y are the weathermap coordinates
 * we wish to calculate humidity for. Returns the humidity of the weathermap
 * square.
 */
static int humid_tile(int x, int y) {
    int ox, oy, humid;

    ox = x;
    oy = y;

    /* find the square the wind is coming from, without going out of bounds */

    if (weathermap[x][y].winddir == 8 || weathermap[x][y].winddir <= 2) {
        if (y != 0) {
            oy = y-1;
        }
    }
    if (weathermap[x][y].winddir >= 6) {
        if (x != 0) {
            ox = x-1;
        }
    }
    if (weathermap[x][y].winddir >= 4 && weathermap[x][y].winddir <= 6) {
        if (y < WEATHERMAPTILESY-1) {
            oy = y+1;
        }
    }
    if (weathermap[x][y].winddir >= 2 && weathermap[x][y].winddir <= 4) {
        if (x < WEATHERMAPTILESX-1) {
            ox = x+1;
        }
    }
    humid = (weathermap[x][y].humid*2+
        weathermap[ox][oy].humid*weathermap[ox][oy].windspeed +
        weathermap[x][y].water+rndm(0, 10))/
        (weathermap[ox][oy].windspeed+3)+rndm(0, 5);
    if (humid < 0) {
        humid = 1;
    }
    if (humid > 100) {
        humid = 100;
    }
    return humid;
}

/**
 * calculate temperature of the weathermap square x,y.  Requires the current
 * time of day in *tod.
 */
static void temperature_calc(int x, int y, const timeofday_t *tod) {
    int dist, equator, elev, n;
    float diff, tdiff;

    equator = (WEATHERMAPTILESX+WEATHERMAPTILESY)/4;
    diff = (float)(EQUATOR_BASE_TEMP-POLAR_BASE_TEMP)/(float)equator;
    tdiff = (float)SEASONAL_ADJUST/((float)MONTHS_PER_YEAR/2.0);
    equator *= 2;
    n = 0;
    /* we essentially move the equator during the season */
    if (tod->month > (MONTHS_PER_YEAR/2)) { /* EOY */
        n -= (tod->month*tdiff);
    } else {
        n = (MONTHS_PER_YEAR - tod->month)*tdiff;
    }
    dist = polar_distance(x-n/2, y-n/2, equator);

    /* now we have the base temp, unadjusted for time.  Time adjustment
       is not recorded on the map, rather, it's done JIT. */
    weathermap[x][y].temp = (int)(dist * diff);
    /* just scrap these for now, its mostly ocean */
    if (weathermap[x][y].avgelev < 0) {
        elev = 0;
    } else {
        elev = MAX(10000, weathermap[x][y].avgelev)/1000;
    }
    weathermap[x][y].temp -= elev;
}

/**
 * Compute the real (adjusted) temperature of a given weathermap tile.
 * This takes into account the wind, base temp, sunlight, and other fun
 * things.  Seasons are automatically handled by moving the equator.
 * Elevation is partially considered in the base temp. x and y are the
 * weathermap coordinates.
*/
static int real_temperature(int x, int y) {
    int i, temp;
    timeofday_t tod;

    /* adjust for time of day */
    temp = weathermap[x][y].temp;
    get_tod(&tod);
    for (i = HOURS_PER_DAY/2; i < HOURS_PER_DAY; i++) {
        temp += season_tempchange[i];
        /* high amounts of water has a buffering effect on the temp */
        if (weathermap[x][y].water > 33) {
            i++;
        }
    }
    for (i = 0; i <= tod.hour; i++) {
        temp += season_tempchange[i];
        if (weathermap[x][y].water > 33) {
            i++;
        }
    }

    /* windchill */
    for (i = 1; i < weathermap[x][y].windspeed; i += i) {
        temp--;
    }

    return temp;
}

/**
 * Given a worldmap name, and x and y on that map, compute the temperature
 * for a specific square.  Used to normalize elevation.
 */
int real_world_temperature(int x, int y, mapstruct *m) {
    int wx, wy, temp, eleva, elevb;
    object *op;

    /*LOG(llevDebug, "real_world_temperature: worldmaptoweathermap : %s\n",m->path);*/
    worldmap_to_weathermap(x, y, &wx, &wy, /*m->path*/m);
    temp = real_temperature(wx, wy);
    if (weathermap[wx][wy].avgelev < 0) {
        eleva = 0;
    } else {
        eleva = weathermap[x][y].avgelev;
    }

    op = GET_MAP_OB(m, x, y);
    if (!op) {
        return eleva;
    }

    elevb = op->elevation;
    if (elevb < 0) {
        elevb = 0;
    }
    if (elevb > eleva) {
        elevb -= eleva;
        temp -= elevb/1000;
    } else {
        elevb = eleva - elevb;
        temp += elevb/1000;
    }
    return temp;
}

/**
 * this code simply smooths the pressure map
 */
static void smooth_pressure(void) {
    int x, y;
    int k;

    for (k = 0; k < PRESSURE_ROUNDING_ITER; k++) {
        for (x = 2; x < WEATHERMAPTILESX-2; x++) {
            for (y = 2; y < WEATHERMAPTILESY-2; y++) {
                weathermap[x][y].pressure = (weathermap[x][y].pressure*
                    PRESSURE_ROUNDING_FACTOR+weathermap[x-1][y].pressure+
                    weathermap[x][y-1].pressure+weathermap[x-1][y-1].pressure+
                    weathermap[x+1][y].pressure+weathermap[x][y+1].pressure+
                    weathermap[x+1][y+1].pressure+weathermap[x+1][y-1].pressure+
                    weathermap[x-1][y+1].pressure)/(PRESSURE_ROUNDING_FACTOR+8);
            }
        }
        for (x = WEATHERMAPTILESX-2; x > 2; x--) {
            for (y = WEATHERMAPTILESY-2; y > 2; y--) {
                weathermap[x][y].pressure = (weathermap[x][y].pressure*
                    PRESSURE_ROUNDING_FACTOR+weathermap[x-1][y].pressure+
                    weathermap[x][y-1].pressure+weathermap[x-1][y-1].pressure+
                    weathermap[x+1][y].pressure+weathermap[x][y+1].pressure+
                    weathermap[x+1][y+1].pressure+weathermap[x+1][y-1].pressure+
                    weathermap[x-1][y+1].pressure)/(PRESSURE_ROUNDING_FACTOR+8);
            }
        }
    }

    for (x = 0; x < WEATHERMAPTILESX; x++)
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            weathermap[x][y].pressure = MIN(weathermap[x][y].pressure, PRESSURE_MAX);
            weathermap[x][y].pressure = MAX(weathermap[x][y].pressure, PRESSURE_MIN);
        }

}

/**
 * perform small randomizations in the pressure map.  Then, apply the
 * smoothing algorithim.. This causes the pressure to change very slowly
 */
static void perform_pressure(void) {
    int x, y, l, n, j, k;

    /* create random spikes in the pressure */
    for (l = 0; l < PRESSURE_SPIKES; l++) {
        x = rndm(0, WEATHERMAPTILESX-1);
        y = rndm(0, WEATHERMAPTILESY-1);
        n = rndm(600, 1300);
        weathermap[x][y].pressure = n;
        if (x > 5 && y > 5 && x < WEATHERMAPTILESX-5 && y < WEATHERMAPTILESY-5) {
            for (j = x-2; j < x+2; j++) {
                for (k = y-2; k < y+2; k++) {
                    weathermap[j][k].pressure = n;
                    /* occasionally add a storm */
                    if (rndm(1, 20) == 1) {
                        weathermap[j][k].humid = rndm(50, 80);
                    }
                }
            }
        }
    }

    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            weathermap[x][y].pressure += rndm(-1, 4);
        }
    }

    smooth_pressure();
}


/**
 * is direction a similar to direction b? Find out in this exciting function
 * below. Returns 1 if true, 0 for false.
 */
int similar_direction(int a, int b) {
    /* shortcut the obvious */
    if (a == b) {
        return 1;
    }

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

/**
 * It doesn't really smooth it as such.  The main function of this is to
 * apply the pressuremap to the wind direction and speed.  Then, we run
 * a quick pass to update the windspeed.
 */
static void smooth_wind(void) {
    int x, y;
    int tx, ty, dx, dy;
    int minp;

    /* skip the outer squares.. it makes handling alot easier */
    dx = 0;
    dy = 0;
    for (x = 1; x < WEATHERMAPTILESX-1; x++)
        for (y = 1; y < WEATHERMAPTILESY-1; y++) {
            minp = PRESSURE_MAX + 1;
            for (tx = -1; tx < 2; tx++) {
                for (ty = -1; ty < 2; ty++) {
                    if (!(tx == 0 && ty == 0)) {
                        if (weathermap[x+tx][y+ty].pressure < minp) {
                            minp = weathermap[x+tx][y+ty].pressure;
                            dx = tx;
                            dy = ty;
                        }
                    }
                }
            }

            /* if the wind is strong, the pressure won't decay it completely */
            if (weathermap[x][y].windspeed > 5 && !similar_direction(weathermap[x][y].winddir, find_dir_2(dx, dy))) {
                weathermap[x][y].windspeed -= 2*2;
            } else {
                weathermap[x][y].winddir = find_dir_2(dx, dy);
                weathermap[x][y].windspeed = (weathermap[x][y].pressure-weathermap[x+dx][y+dy].pressure)*WIND_FACTOR;
            }
                /* Add in sea breezes. */
            weathermap[x][y].windspeed += weathermap[x][y].water/4;
            if (weathermap[x][y].windspeed < 0) {
                weathermap[x][y].windspeed = 0;
            }
        }

    /*  now, lets crank on the speed.  When surrounding tiles all have
        the same speed, inc ours.  If it's chaos. drop it.
     */
    for (x = 1; x < WEATHERMAPTILESX-1; x++) {
        for (y = 1; y < WEATHERMAPTILESY-1; y++) {
            minp = 0;
            for (tx = -1; tx < 2; tx++) {
                for (ty = -1; ty < 2; ty++) {
                    if (ty != 0 && ty != 0) {
                        if (similar_direction(weathermap[x][y].winddir, weathermap[x+tx][y+ty].winddir)) {
                            minp++;
                        }
                    }
                }
            }
            if (minp > 4) {
                weathermap[x][y].windspeed++;
            }
            if (minp > 6) {
                weathermap[x][y].windspeed += 2;
            }
            if (minp < 2) {
                weathermap[x][y].windspeed--;
            }
            if (weathermap[x][y].windspeed < 0) {
                weathermap[x][y].windspeed = 0;
            }
        }
    }
}

/**
 * Plot the gulfstream map over the wind map.  This is done after the wind,
 * to avoid the windsmoothing scrambling the jet stream.
 */
static void plot_gulfstream(void) {
    int x, y, tx;

    x = gulf_stream_start;

    if (gulf_stream_direction) {
        for (y = WEATHERMAPTILESY-1; y > 0; y--) {
            for (tx = 0; tx < GULF_STREAM_WIDTH && x+tx < WEATHERMAPTILESX; tx++) {
                if (similar_direction(weathermap[x+tx][y].winddir, gulf_stream_dir[tx][y]) && weathermap[x+tx][y].windspeed < GULF_STREAM_BASE_SPEED-5) {
                    weathermap[x+tx][y].windspeed += gulf_stream_speed[tx][y];
                } else{
                    weathermap[x+tx][y].windspeed = gulf_stream_speed[tx][y];
                }
                weathermap[x+tx][y].winddir = gulf_stream_dir[tx][y];
                if (tx == GULF_STREAM_WIDTH-1) {
                    switch (gulf_stream_dir[tx][y]) {
                    case 6: x--; break;
                    case 7: break;
                    case 8: x++; ; break;
                    }
                }
                if (x < 0) {
                    x++;
                }
                if (x >= WEATHERMAPTILESX-GULF_STREAM_WIDTH) {
                    x--;
                }
            }
        }
    } else {
        for (y = 0; y < WEATHERMAPTILESY-1; y++) {
            for (tx = 0; tx < GULF_STREAM_WIDTH && x+tx < WEATHERMAPTILESX; tx++) {
                if (similar_direction(weathermap[x+tx][y].winddir, gulf_stream_dir[tx][y]) && weathermap[x+tx][y].windspeed < GULF_STREAM_BASE_SPEED-5) {
                    weathermap[x+tx][y].windspeed += gulf_stream_speed[tx][y];
                } else {
                    weathermap[x+tx][y].windspeed = gulf_stream_speed[tx][y];
                }
                weathermap[x+tx][y].winddir = gulf_stream_dir[tx][y];
                if (tx == GULF_STREAM_WIDTH-1) {
                    switch (gulf_stream_dir[tx][y]) {
                    case 2: x++; break;
                    case 3: break;
                    case 4: x--; break;
                    }
                }
                if (x < 0) {
                    x++;
                }
                if (x >= WEATHERMAPTILESX-GULF_STREAM_WIDTH) {
                    x--;
                }
            }
        }
    }
    /* occasionally move the stream */
    if (rndm(1, 500) == 1) {
        gulf_stream_direction = rndm(0, 1);
        for (tx = 0; tx < GULF_STREAM_WIDTH; tx++) {
            for (y = 0; y < WEATHERMAPTILESY-1; y++) {
                if (gulf_stream_direction) {
                    switch (gulf_stream_dir[tx][y]) {
                    case 2: gulf_stream_dir[tx][y] = 6; break;
                    case 3: gulf_stream_dir[tx][y] = 7; break;
                    case 4: gulf_stream_dir[tx][y] = 8; break;
                    }
                } else {
                    switch (gulf_stream_dir[tx][y]) {
                    case 6: gulf_stream_dir[tx][y] = 2; break;
                    case 7: gulf_stream_dir[tx][y] = 3; break;
                    case 8: gulf_stream_dir[tx][y] = 4; break;
                    }
                }
            }
        }
    }
    if (rndm(1, 25) == 1) {
        gulf_stream_start += rndm(-1, 1);
    }
    if (gulf_stream_start >= WEATHERMAPTILESX-GULF_STREAM_WIDTH) {
        gulf_stream_start--;
    }
    if (gulf_stream_start < 1) {
        gulf_stream_start++;
    }
}

/**
 * let the madness, begin.
 *
 * This function is the one that ties everything together.  Here we loop
 * over all the weathermaps, and compare the various conditions we have
 * calculated up to now, to figure out what the sky conditions are for this
 * square.
 */
static void compute_sky(void) {
    int x, y;
    int temp;

    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            temp = real_temperature(x, y);
            if (weathermap[x][y].pressure < 980) {
                if (weathermap[x][y].humid < 20) {
                    weathermap[x][y].sky = SKY_LIGHTCLOUD;
                } else if (weathermap[x][y].humid < 30) {
                    weathermap[x][y].sky = SKY_OVERCAST;
                } else if (weathermap[x][y].humid < 40) {
                    weathermap[x][y].sky = SKY_LIGHT_RAIN;
                } else if (weathermap[x][y].humid < 55) {
                    weathermap[x][y].sky = SKY_RAIN;
                } else if (weathermap[x][y].humid < 70) {
                    weathermap[x][y].sky = SKY_HEAVY_RAIN;
                } else {
                    weathermap[x][y].sky = SKY_HURRICANE;
                }
                if (weathermap[x][y].sky < SKY_HURRICANE &&
                    weathermap[x][y].windspeed > 30) {
                    weathermap[x][y].sky++;
                }
                if (temp <= 0 && weathermap[x][y].sky > SKY_OVERCAST) {
                    weathermap[x][y].sky += 10; /*let it snow*/
                }
            } else if (weathermap[x][y].pressure < 1000) {
                if (weathermap[x][y].humid < 10) {
                    weathermap[x][y].sky = SKY_CLEAR;
                } else if (weathermap[x][y].humid < 25) {
                    weathermap[x][y].sky = SKY_LIGHTCLOUD;
                } else if (weathermap[x][y].humid < 45) {
                    weathermap[x][y].sky = SKY_OVERCAST;
                } else if (weathermap[x][y].humid < 60) {
                    weathermap[x][y].sky = SKY_LIGHT_RAIN;
                } else if (weathermap[x][y].humid < 75) {
                    weathermap[x][y].sky = SKY_RAIN;
                } else {
                    weathermap[x][y].sky = SKY_HEAVY_RAIN;
                }
                if (weathermap[x][y].sky < SKY_HURRICANE &&
                    weathermap[x][y].windspeed > 30) {
                    weathermap[x][y].sky++;
                }
                if (temp <= 0 && weathermap[x][y].sky > SKY_OVERCAST) {
                    weathermap[x][y].sky += 10; /*let it snow*/
                }
                if (temp > 0 && temp < 5 && weathermap[x][y].humid > 95 &&
                    weathermap[x][y].windspeed < 3) {
                    weathermap[x][y].sky = SKY_FOG; /* rare */
                }
                if (temp > 0 && temp < 5 && weathermap[x][y].humid > 70 &&
                    weathermap[x][y].windspeed > 35) {
                    weathermap[x][y].sky = SKY_HAIL; /* rare */
                }
            } else if (weathermap[x][y].pressure < 1020) {
                if (weathermap[x][y].humid < 20) {
                    weathermap[x][y].sky = SKY_CLEAR;
                } else if (weathermap[x][y].humid < 30) {
                    weathermap[x][y].sky = SKY_LIGHTCLOUD;
                } else if (weathermap[x][y].humid < 40) {
                    weathermap[x][y].sky = SKY_OVERCAST;
                } else if (weathermap[x][y].humid < 55) {
                    weathermap[x][y].sky = SKY_LIGHT_RAIN;
                } else if (weathermap[x][y].humid < 70) {
                    weathermap[x][y].sky = SKY_RAIN;
                } else {
                    weathermap[x][y].sky = SKY_HEAVY_RAIN;
                }
                if (weathermap[x][y].sky < SKY_HURRICANE &&
                    weathermap[x][y].windspeed > 30) {
                    weathermap[x][y].sky++;
                }
                if (temp <= 0 && weathermap[x][y].sky > SKY_OVERCAST) {
                    weathermap[x][y].sky += 10; /*let it snow*/
                }
            } else {
                if (weathermap[x][y].humid < 35) {
                    weathermap[x][y].sky = SKY_CLEAR;
                } else if (weathermap[x][y].humid < 55) {
                    weathermap[x][y].sky = SKY_LIGHTCLOUD;
                } else if (weathermap[x][y].humid < 70) {
                    weathermap[x][y].sky = SKY_OVERCAST;
                } else if (weathermap[x][y].humid < 85) {
                    weathermap[x][y].sky = SKY_LIGHT_RAIN;
                } else if (weathermap[x][y].humid < 95) {
                    weathermap[x][y].sky = SKY_RAIN;
                } else {
                    weathermap[x][y].sky = SKY_HEAVY_RAIN;
                }
                if (weathermap[x][y].sky < SKY_HURRICANE &&
                    weathermap[x][y].windspeed > 30) {
                    weathermap[x][y].sky++;
                }
                if (temp <= 0 && weathermap[x][y].sky > SKY_OVERCAST) {
                    weathermap[x][y].sky += 10; /*let it snow*/
                }
            }
        }
    }
}

/**
 * Keep track of how much rain has fallen in a given weathermap square.
 */
static void process_rain(void) {
    int x, y, rain;

    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
            rain = weathermap[x][y].sky;
            if (rain >= SKY_LIGHT_SNOW) {
                rain -= 10;
            }
            if (rain > SKY_OVERCAST && rain < SKY_FOG) {
                rain -= SKY_OVERCAST;
                weathermap[x][y].rainfall += rain;
            }
        }
    }
}

/**
 * The world spinning drags the weather with it.
 * The equator is diagonal, and the poles are 45 degrees from north /south.
 * What the hell, lets spin the planet backwards.
 */
static void spin_globe(void) {
    int x, y;
    int buffer_humid;
    int buffer_sky;

    for (y = 0; y < WEATHERMAPTILESY; y++) {
        buffer_humid = weathermap[0][y].humid;
        buffer_sky = weathermap[0][y].sky;
        for (x = 0; x < (WEATHERMAPTILESX-1); x++) {
            weathermap[x][y].humid = weathermap[x+1][y].humid;
            weathermap[x][y].sky = weathermap[x+1][y].sky;
        }
        weathermap[WEATHERMAPTILESX-1][y].humid = buffer_humid;
        weathermap[WEATHERMAPTILESX-1][y].sky = buffer_sky;
    }
}

/**
 * Dump all the weather data as an image.
 * Writes two other files that are useful for creating animations and web pages.
 */
static void write_weather_images(void) {
    char filename[MAX_BUF];
    FILE *fp;
    int x, y;
    sint32 min[10], max[10], avgrain, avgwind, realmaxwind;
    double scale[10], realscalewind;
    uint8 pixels[3*3*WEATHERMAPTILESX];
    sint64 total_rainfall = 0;
    sint64 total_wind = 0;

    min[0] = 0;            max[0] = 100;
    min[1] = 0;            max[1] = 0;
    min[2] = 0;            max[2] = 0;
    min[3] = PRESSURE_MIN; max[3] = PRESSURE_MAX;
    min[4] = 0;            max[4] = 0;
    min[5] = 1;            max[5] = 8;
    min[6] = 0;            max[6] = 100;
    min[7] = -45;          max[7] = 45;
    min[8] = 0;            max[8] = 16;
    min[9] = 0;            max[9] = 0;
    for (x = 0; x < WEATHERMAPTILESX; x++) {
        for (y = 0; y < WEATHERMAPTILESY; y++) {
/*          min[0] = MIN(min[0], weathermap[x][y].water); */
            min[1] = MIN(min[1], weathermap[x][y].avgelev);
            min[2] = MIN(min[2], weathermap[x][y].rainfall);
/*          min[3] = MIN(min[3], weathermap[x][y].pressure); */
            min[4] = MIN(min[4], weathermap[x][y].windspeed);
/*          min[5] = MIN(min[5], weathermap[x][y].winddir); */
/*          min[6] = MIN(min[6], weathermap[x][y].humid); */
/*          min[7] = MIN(min[7], real_temp[x][y]); */
/*          min[8] = MIN(min[8], weathermap[x][y].sky); */
/*          min[9] = MIN(min[9], weathermap[x][y].darkness); */

/*          max[0] = MAX(max[0], weathermap[x][y].water); */
            max[1] = MAX(max[1], weathermap[x][y].avgelev);
            max[2] = MAX(max[2], weathermap[x][y].rainfall);
/*          max[3] = MAX(max[3], weathermap[x][y].pressure); */
            max[4] = MAX(max[4], weathermap[x][y].windspeed);
/*          max[5] = MAX(max[5], weathermap[x][y].winddir); */
/*          max[6] = MAX(max[6], weathermap[x][y].humid); */
/*          max[7] = MAX(max[7], real_temp[x][y]); */
/*          max[8] = MAX(max[8], weathermap[x][y].sky); */
/*          max[9] = MAX(max[9], weathermap[x][y].darkness); */
            total_rainfall += weathermap[x][y].rainfall;
            total_wind += weathermap[x][y].windspeed;
        }
    }
    avgrain = total_rainfall/(WEATHERMAPTILESX*WEATHERMAPTILESY);
    avgwind = (total_wind   /((WEATHERMAPTILESX*WEATHERMAPTILESY)*3/2));
    max[2] = avgrain-1;
    realscalewind = 255.0l/(max[4]-min[4]);
    realmaxwind = max[4];
    max[4] = avgwind-1;
    for (x = 0; x < 10; x++) {
        scale[x] = 255.0l/(max[x]-min[x]);
    }

    snprintf(filename, sizeof(filename), "%s/weather.ppm", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    fprintf(fp, "P6\n%d %d\n", 3*WEATHERMAPTILESX, 3*WEATHERMAPTILESY);
    fprintf(fp, "255\n");
    for (y = 0; y < WEATHERMAPTILESY; y++) {
        for (x = 0; x < (3 * 3 * WEATHERMAPTILESX); x++) {
            pixels[x] = 0;
        }
        for (x = 0; x < WEATHERMAPTILESX; x++) {
            pixels[3*x+(0*WEATHERMAPTILESX*3+BLUE)] = (uint8)((weathermap[x][y].water-min[0])*scale[0]);
            if (weathermap[x][y].avgelev >= 0) {
                pixels[3*x+(1*WEATHERMAPTILESX*3+GREEN)] = (uint8)((weathermap[x][y].avgelev-min[1])*scale[1]);
            } else {
                pixels[3*x+(1*WEATHERMAPTILESX*3+BLUE)] = (uint8)((weathermap[x][y].avgelev-min[1])*scale[1]);
            }
            if (weathermap[x][y].rainfall >= avgrain) { /* rainfall is rather spikey, this gives us more detail. */
                pixels[3*x+(2*WEATHERMAPTILESX*3+RED)] = 255;
            } else {
                pixels[3*x+(2*WEATHERMAPTILESX*3+BLUE)] = (uint8)((weathermap[x][y].rainfall-min[2])*scale[2]);
            }
        }
        fwrite(pixels, sizeof(uint8), (3*3*WEATHERMAPTILESX), fp);
    }
    for (y = 0; y < WEATHERMAPTILESY; y++) {
        for (x = 0; x < WEATHERMAPTILESX; x++) {
            uint32 dir = directions[weathermap[x][y].winddir-1];
            uint32 speed = weathermap[x][y].windspeed;
            uint8 pressure = (weathermap[x][y].pressure-min[3])*scale[3];
            pixels[3*x+(0*WEATHERMAPTILESX*3+RED)] = pressure;
            pixels[3*x+(0*WEATHERMAPTILESX*3+GREEN)] = pressure;
            pixels[3*x+(0*WEATHERMAPTILESX*3+BLUE)] = pressure;
            if (speed < avgwind) {
                speed = (speed-min[4])*scale[4];
                pixels[3*x+(1*WEATHERMAPTILESX*3+RED)] = speed;
                pixels[3*x+(1*WEATHERMAPTILESX*3+GREEN)] = speed;
                pixels[3*x+(1*WEATHERMAPTILESX*3+BLUE)] = speed;
            } else {
                speed = (speed-realmaxwind)*realscalewind;
/*              if (speed < 100) {
                    speed = 100;
                }*/
                pixels[3*x+(1*WEATHERMAPTILESX*3+RED)] = speed;
                pixels[3*x+(1*WEATHERMAPTILESX*3+GREEN)] = 0;
                pixels[3*x+(1*WEATHERMAPTILESX*3+BLUE)] = 0;
            }
            pixels[3*x+(2*WEATHERMAPTILESX*3+RED)] = (uint8)((dir&0x00FF0000)>>16);
            pixels[3*x+(2*WEATHERMAPTILESX*3+GREEN)] = (uint8)((dir&0x0000FF00)>>8);
            pixels[3*x+(2*WEATHERMAPTILESX*3+BLUE)] = (uint8)((dir&0x000000FF));
        }
        fwrite(pixels, sizeof(uint8), (3*3*WEATHERMAPTILESX), fp);
    }
    for (y = 0; y < WEATHERMAPTILESY; y++) {
        for (x = 0; x < (3 * 3 * WEATHERMAPTILESX); x++) {
            pixels[x] = 0;
        }
        for (x = 0; x < WEATHERMAPTILESX; x++) {
            uint32 dir = skies[weathermap[x][y].sky];
            pixels[3*x+(0*WEATHERMAPTILESX*3+BLUE)] = (uint8)((weathermap[x][y].humid-min[6])*scale[6]);
            /*pixels[3*x+(1*WEATHERMAPTILESX*3 + RED)] = (uint8) ((real_temp[x][y]-min[7])*scale[7]);*/
            pixels[3*x+(1*WEATHERMAPTILESX*3+RED)] = 1;
            pixels[3*x+(2*WEATHERMAPTILESX*3+RED)] = (uint8)((dir&0x00FF0000)>>16);
            pixels[3*x+(2*WEATHERMAPTILESX*3+GREEN)] = (uint8)((dir&0x0000FF00)>>8);
            pixels[3*x+(2*WEATHERMAPTILESX*3+BLUE)] = (uint8)((dir&0x000000FF));
        }
        fwrite(pixels, sizeof(uint8), (3*3*WEATHERMAPTILESX), fp);
    }
    fclose(fp);

    snprintf(filename, sizeof(filename), "%s/todtick", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    fprintf(fp, "%lu", todtick);
    fclose(fp);
}
