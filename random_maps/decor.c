/*
 * static char *rcsid_decor_ =
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
 * Those functions handle the decor in the random maps.
 */

#include <global.h>
#include <random_map.h>
#include <rproto.h>

/** Number of decor styles that can be chosen if none specified. */
#define NR_DECOR_OPTIONS 1

/**
 * Count objects at a spot.
 * @param map
 * map we want to check
 * @param x
 * @param y
 * coordinates
 * @return
 * count of objects in the map at x,y.
 */
int obj_count_in_map(mapstruct *map, int x, int y) {
    int count = 0;

    FOR_MAP_PREPARE(map, x, y, tmp)
        count++;
    FOR_MAP_FINISH();
    return count;
}

/**
 * Put the decor into the map.  Right now, it's very primitive.
 * @param map
 * map to add decor to.
 * @param maze
 * layout of the map, as was generated.
 * @param decorstyle
 * style to use. Can be NULL.
 * @param decor_option
 * how to place decor:
 * - 0 means no decor.
 * - 1 means to place randomly decor.
 * - other means to fill the map with decor.
 * @param RP
 * parameters of the random map.
 */
void put_decor(mapstruct *map, char **maze, char *decorstyle, int decor_option, RMParms *RP) {
    mapstruct *decor_map;
    char style_name[256];

    snprintf(style_name, sizeof(style_name), "/styles/decorstyles");

    decor_map = find_style(style_name, decorstyle, -1);
    if (decor_map == NULL)
        return;

    /* pick a random option, only 1 option right now. */
    if (decor_option == 0)
        decor_option = RANDOM()%NR_DECOR_OPTIONS+1;

    switch (decor_option) {
    case 0:
        break;

    case 1: { /* random placement of decor objects. */
            int number_to_place = RANDOM()%((RP->Xsize*RP->Ysize)/5);
            int failures = 0;
            object *new_decor_object;

            while (failures < 100 && number_to_place > 0) {
                int x, y;

                x = RANDOM()%(RP->Xsize-2)+1;
                y = RANDOM()%(RP->Ysize-2)+1;
                if (maze[x][y] == 0 && obj_count_in_map(map, x, y) < 2) { /* empty */
                    object *this_object;

                    new_decor_object = pick_random_object(decor_map);
                    this_object = arch_to_object(new_decor_object->arch);
                    object_copy(new_decor_object, this_object);
                    /*
                     * Don't change move_block, this prevents item merging.
                     * Instead, fix the item on the style map if blocking
                     * is bad.
                     */
                    /*this_object->move_block = MOVE_BLOCK_DEFAULT;*/
                    object_insert_in_map_at(this_object, map, NULL, 0, x, y);
                    number_to_place--;
                } else
                    failures++;
            }
            break;
        }

        default: { /* place decor objects everywhere: tile the map. */
            int i, j;

            for (i = 1; i < RP->Xsize-1; i++)
                for (j = 1; j < RP->Ysize-1; j++) {
                    if (maze[i][j] == 0) {
                        object *new_decor_object, *this_object;

                        new_decor_object = pick_random_object(decor_map);
                        this_object = arch_to_object(new_decor_object->arch);
                        object_copy(new_decor_object, this_object);
                        /*
                         * Don't change move_block, this prevents item merging.
                         * Instead, fix the item on the style map if blocking
                         * is bad.
                         */
                        /*this_object->move_block = MOVE_BLOCK_DEFAULT;*/
                        object_insert_in_map_at(this_object, map, NULL, 0, i, j);
                    }
                }
            break;
        }
    }
}
