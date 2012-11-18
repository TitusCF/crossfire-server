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

/**
 * @file
 * Time-related functions.
 */

#include <global.h>
#include <tod.h>
#include <map.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <assert.h>

extern unsigned long todtick;

static void dawn_to_dusk(const timeofday_t *tod);
/** How to alter darkness, based on time of day and season. */
static const int season_timechange[5][HOURS_PER_DAY] = {
/*    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14  1  2  3  4  5  6  7  8  9 10 11 12 13 */
    { 0, 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 0, 0, 0, 0,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0 },
    { 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0 }
};

/**
 * Set the darkness level for a map, based on the time of the day.
 *
 * @param m
 * map to alter.
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
 * Compute the darkness level for all loaded maps in the game.
 *
 * @param tod
 * time of day to compute darkness for.
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
    get_tod(&tod);
    dawn_to_dusk(&tod);
}
