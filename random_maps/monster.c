/*
 * static char *rcsid_monster_c =
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
 * Monster placement for random maps.
 */

#include <global.h>
#include <random_map.h>
#include <rproto.h>

/**
 * Inserts a monster in the map.
 * Some monsters are multisquare, and these guys require special
 * handling.
 * @param new_obj
 * monster to insert. Its x and y fields must be the desired location.
 * @param map
 * where to insert to.
 * @todo
 * there probably is a function in the common library for that, so remove this one.
 */
void	 insert_multisquare_ob_in_map(object *new_obj,mapstruct *map) {
    int x,y;
    archetype *at;
    object *old_seg;
    object *head;
    /* first insert the head */
    insert_ob_in_map(new_obj,map,new_obj,INS_NO_MERGE | INS_NO_WALK_ON);

    x = new_obj->x;
    y = new_obj->y;
    old_seg=new_obj;
    head = new_obj;
    for(at=new_obj->arch->more;at!=NULL;at=at->more) {
        object *new_seg;
        new_seg = arch_to_object(at);
        new_seg->x = x + at->clone.x;
        new_seg->y = y + at->clone.y;
        new_seg->map = old_seg->map;
        insert_ob_in_map(new_seg,new_seg->map, new_seg,INS_NO_MERGE | INS_NO_WALK_ON);
        new_seg->head = head;
        old_seg->more = new_seg;
        old_seg = new_seg;
    }
    old_seg->more = NULL;
}

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
void place_monsters(mapstruct *map, char *monsterstyle, int difficulty,RMParms *RP) {
    char styledirname[256];
    mapstruct *style_map=0;
    int failed_placements;
    sint64 exp_per_sq, total_experience;
    int number_monsters=0;
    archetype *at;

    snprintf(styledirname, sizeof(styledirname), "%s","/styles/monsterstyles");
    style_map = find_style(styledirname,monsterstyle,difficulty);
    if(style_map == 0) return;

    /* fill up the map with random monsters from the monster style*/

    total_experience = 0;
    failed_placements = 0;
    exp_per_sq = 0;
    while(exp_per_sq <= level_exp(difficulty,1.0) && failed_placements < 100
      && number_monsters < (RP->Xsize * RP->Ysize)/8) {
        object *this_monster=pick_random_object(style_map);
        int x,y,freeindex;
        if(this_monster == NULL) return;  /* no monster?? */
        x = RANDOM() % RP->Xsize;
        y = RANDOM() % RP->Ysize;
        freeindex = find_first_free_spot(this_monster,map,x,y);
        if(freeindex!=-1) {
            object *new_monster = arch_to_object(this_monster->arch);
            x += freearr_x[freeindex];
            y += freearr_y[freeindex];
            copy_object_with_inv(this_monster,new_monster);
            new_monster->x = x;
            new_monster->y = y;
            insert_multisquare_ob_in_map(new_monster,map);
            total_experience+= this_monster->stats.exp;
            for(at = new_monster->arch; at != NULL; at = at->more)
                number_monsters++;
            RP->total_map_hp+=new_monster->stats.hp;  /*  a global count */
        }
        else {
            failed_placements++;
        }
        exp_per_sq=((double)1000*total_experience)/(MAP_WIDTH(map)*MAP_HEIGHT(map)+1);
    }
}
