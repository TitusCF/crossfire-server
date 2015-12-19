/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2013 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * Monster placement for random maps.
 */

#include "global.h"

#include <stdlib.h>

#include "random_map.h"
#include "rproto.h"

/**
 * Place some monsters into the map. Their experience is linked to difficulty to insert a
 * suitable number of critters.
 * @param map
 * where to put monsters on.
 * @param monsterstyle
 * style. Can be NULL, in which case a random one is used.
 * @param difficulty
 * how difficult the monsters should be, and how many there should be.
 * @param RP
 * random map parameters.
 */
void place_monsters(mapstruct *map, char *monsterstyle, int difficulty, RMParms *RP)
{
    char styledirname[256];
    mapstruct *style_map = NULL;
    int failed_placements;
    int64_t exp_per_sq, total_experience;
    int number_monsters = 0;
    archetype *at;

    snprintf(styledirname, sizeof(styledirname), "%s", "/styles/monsterstyles");
    style_map = find_style(styledirname, monsterstyle, difficulty);
    if (style_map == NULL) {
        return;
    }

    /* fill up the map with random monsters from the monster style*/

    total_experience = 0;
    failed_placements = 0;
    exp_per_sq = 0;
    while (exp_per_sq <= level_exp(difficulty, 1.0)
            && failed_placements < 100
            && number_monsters < (RP->Xsize*RP->Ysize)/8) {
        object *this_monster = pick_random_object(style_map);
        int x, y, freeindex;

        if (this_monster == NULL) {
            return;    /* no monster?? */
        }
        x = RANDOM()%RP->Xsize;
        y = RANDOM()%RP->Ysize;
        freeindex = object_find_first_free_spot(this_monster, map, x, y);
        if (freeindex != -1) {
            object *new_monster = object_create_arch(this_monster->arch);
            object_copy_with_inv(this_monster, new_monster);
            x += freearr_x[freeindex];
            y += freearr_y[freeindex];
            object_insert_in_map_at(new_monster, map, NULL, INS_NO_MERGE|INS_NO_WALK_ON, x, y);

            total_experience += this_monster->stats.exp;
            for (at = new_monster->arch; at != NULL; at = at->more) {
                number_monsters++;
            }
            RP->total_map_hp += new_monster->stats.hp;  /*  a global count */
        } else {
            failed_placements++;
        }
        exp_per_sq = ((double)1000*total_experience)/(MAP_WIDTH(map)*MAP_HEIGHT(map)+1);
    }
}
