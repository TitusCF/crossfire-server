#include <global.h>
#include <tod.h>
#include <map.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

extern unsigned long todtick;
extern weathermap_t **weathermap;

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
    for (i = 0; i < tod.hour; i++)
	change_map_light(m, season_timechange[tod.season][i]);
}

void dawn_to_dusk(timeofday_t *tod)
{
    mapstruct *m;

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
    get_tod(&tod);
    dawn_to_dusk(&tod);
    perform_weather();
}

int wmperformstartx;
int wmperformstarty;

void init_weather()
{
    int x, y;
    int i, j;
    long long int tmp;
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
	settings.worldmaptilesx);
    if (weathermap == NULL)
	fatal(OUT_OF_MEMORY);
    for (y=0; y < settings.worldmaptilesy; y++) {
	weathermap[y] = (weathermap_t *)malloc(sizeof(weathermap_t) *
	    settings.worldmaptilesy);
	if (weathermap[y] == NULL)
	    fatal(OUT_OF_MEMORY);
    }
    /* now we load the values in the big worldmap weather array */
    for (x=0; x < settings.worldmaptilesx; x++) {
	for (y=0; y < settings.worldmaptilesy; y++) {
	    sprintf(filename, "world/world_%d_%d",
		x+settings.worldmapstartx, y+settings.worldmapstarty);
	    m = load_original_map(filename, 0);
	    if (m == NULL)
		continue;
	    m = load_overlay_map(filename, m);
	    if (m == NULL)
		continue;
	    sprintf(weathermap[x][y].path, "%s", m->path);
	    if (m->tmpname)
		weathermap[x][y].tmpname = strdup(m->tmpname);
	    weathermap[x][y].name = strdup(m->name);
	    weathermap[x][y].temp = m->temp;
	    weathermap[x][y].pressure = m->pressure;
	    weathermap[x][y].humid = m->humid;
	    weathermap[x][y].windspeed = m->windspeed;
	    weathermap[x][y].winddir = m->winddir;
	    weathermap[x][y].sky = m->sky;
	    weathermap[x][y].darkness = m->darkness;
	    tmp = 0;
	    for (i=0; i < settings.worldmaptilesizex; i++)
		for (j=0; j < settings.worldmaptilesizey; j++)
		    tmp += m->spaces[i+j].bottom->elevation;
	    weathermap[x][y].avgelev = tmp / (i*j);
	    delete_map(m);
	}
    }
    LOG(llevDebug, "Done reading worldmaps\n");
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

    m->temp = weathermap[wmperformstartx][wmperformstarty].temp;
    m->pressure = weathermap[wmperformstartx][wmperformstarty].pressure;
    m->humid = weathermap[wmperformstartx][wmperformstarty].humid;
    m->windspeed = weathermap[wmperformstartx][wmperformstarty].windspeed;
    m->winddir = weathermap[wmperformstartx][wmperformstarty].winddir;
    m->sky = weathermap[wmperformstartx][wmperformstarty].sky;
    decay_objects(m);
    new_save_map(m, 2); /* write the overlay */
    sprintf(filename, "%s/wmapcurpos", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
	LOG(llevError, "Cannot open %s for writing\n", filename);
	return;
    }
    fprintf(fp, "%d %d", wmperformstartx, wmperformstarty);
    fclose(fp);
}
