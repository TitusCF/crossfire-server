#include <global.h>
#include <tod.h>

extern unsigned long todtick;

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
}
