/*
 * static char *rcsid_floor_c =
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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/**
 * @file
 * Floor handling.
 */

#include <global.h>
#include <random_map.h>
#include <rproto.h>

/**
 * Checks if the tile 'propagates' the floor.
 * @param item
 * tile to check.
 * @return
 * 1 if this tile should propagate, 0 else.
 */
static int can_propagate(char item) {
    return (item == '\0' || item == '<' || item == '>') ? 1 : 0;
}

/**
 * Put a floor at specified location, and then to adjacent tiles if applicable.
 * @param map
 * where to put the floor.
 * @param layout
 * layout that was used to generate the map.
 * @param x
 * @param y
 * coordinates.
 * @param floor_arch
 * what floor to use.
 */
static void put_floor(mapstruct* map, char** layout, int x, int y, object* floor_arch) {
    int dx, dy;
    object* floor;

    floor = arch_to_object(floor_arch->arch);
    floor->x = x;
    floor->y = y;
    insert_ob_in_map(floor,map,floor,INS_NO_MERGE | INS_NO_WALK_ON);

    for (dx = -1; dx < 2; dx++) {
        for (dy = -1; dy < 2; dy++) {
            if (GET_MAP_OB(map, x + dx, y + dy) == NULL && can_propagate(layout[x + dx][y + dy]))
                put_floor(map, layout, x + dx, y + dy, floor_arch);
        }
    }
}

/**
 * Creates the Crossfire mapstruct object from the layout, and adds the floor.
 * @param layout
 * generated layout.
 * @param floorstyle
 * floor style. Can be NULL, in which case a random one is chosen.
 * @param RP
 * parameters of the random map.
 * @return
 * Crossfire map.
 * @todo
 * use safe string functions.
 */
mapstruct *make_map_floor(char **layout, char *floorstyle,RMParms *RP) {
    char styledirname[256];
    char stylefilepath[256];
    mapstruct *style_map=0;
    object *the_floor;
    mapstruct *newMap =0;
    int x, y;

    /* allocate the map */
    newMap = get_empty_map(RP->Xsize, RP->Ysize);

    /* get the style map */
    sprintf(styledirname,"%s","/styles/floorstyles");
    sprintf(stylefilepath,"%s/%s",styledirname,floorstyle);
    style_map = find_style(styledirname,floorstyle,-1);
    if(style_map == 0) return newMap;

    if (RP->multiple_floors) {
        for (x = 0; x < RP->Xsize; x++) {
            for (y = 0; y < RP->Ysize; y++) {
                if (GET_MAP_OB(newMap, x, y) == NULL && layout[x][y] == '\0')
                    put_floor(newMap, layout, x, y, pick_random_object(style_map));
            }
        }
    }

    /* fill up the map with the given floor style */
    if ((the_floor=pick_random_object(style_map))!=NULL) {
        for(x=0;x<RP->Xsize;x++)
            for(y=0;y<RP->Ysize;y++) {
                if (GET_MAP_OB(newMap, x, y) != NULL)
                    continue;
                object *thisfloor = arch_to_object(the_floor->arch);
                thisfloor->x = x; thisfloor->y = y;
                insert_ob_in_map(thisfloor,newMap,thisfloor,INS_NO_MERGE | INS_NO_WALK_ON);
            }
    }
    return newMap;
}
