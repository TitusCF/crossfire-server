/*
 * static char *rcsid_swamp_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2005 Mark Wedel & Crossfire Development Team
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

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/* Note this code is very specialized for swamps, in terms of the messages
 * as well as handling of move types.
 */

void walk_on_deep_swamp (object *op, object *victim)
{
    if (victim->type == PLAYER && victim->stats.hp >= 0 && !(victim->move_type & MOVE_FLYING)) {
	new_draw_info_format(NDI_UNIQUE, 0, victim, "You are down to your knees in the %s.", op->name);
	op->stats.food = 1;
	victim->speed_left -= op->move_slow_penalty;
    }
}

void move_deep_swamp (object *op)
{
    object *above = op->above;
    object *nabove;
    int got_player = 0;

    while(above) {
        nabove = above->above;
        if (above->type == PLAYER && !(above->move_type & MOVE_FLYING) && above->stats.hp >= 0 && !QUERY_FLAG(above,FLAG_WIZ)) {
            object *woodsman = find_obj_by_type_subtype(above, SKILL, SK_WOODSMAN);
            got_player = 1;
            if (op->stats.food < 1) {
                LOG (llevDebug, "move_deep_swamp(): player is here, but state is %d\n", op->stats.food);
                op->stats.food = 1;
            }

            if ( op->stats.food < 10 ) {
                if (rndm(0, 2) == 0) {
                    new_draw_info_format(NDI_UNIQUE, 0,above, "You are down to your waist in the wet %s.", op->name);
                    op->stats.food = woodsman ? op->stats.food + 1 : 10;
                    above->speed_left -= op->move_slow_penalty;
                }
            }
            else if ( op->stats.food < 20 ) {
                if (rndm(0, 2) == 0) {
                    new_draw_info_format(NDI_UNIQUE | NDI_RED, 0,above, "You are down to your NECK in the dangerous %s.", op->name);
                    op->stats.food = woodsman ? op->stats.food + 1 : 20;
                    sprintf(above->contr->killer,"drowning in a %s", op->name);
                    above->stats.hp--;
                    above->speed_left -= op->move_slow_penalty;
                }
            }
            else
                if (rndm(0, 4) == 0) {
                    op->stats.food = 0;
                    new_draw_info_format(NDI_UNIQUE | NDI_ALL, 1, NULL, "%s disappeared into a %s.",above->name, op->name);
                    sprintf(above->contr->killer,"drowning in a %s", op->name);
                    above->stats.hp = -1;
                    kill_player(above); /* player dies in the swamp */
                }
        } else if (!QUERY_FLAG(above, FLAG_ALIVE) && !(above->move_type & MOVE_FLYING) && 
            !(QUERY_FLAG(above,FLAG_IS_FLOOR)) && !(QUERY_FLAG(above,FLAG_OVERLAY_FLOOR)) &&
            !(QUERY_FLAG(above, FLAG_NO_PICK))) {
            if (rndm(0, 2) == 0) decrease_ob(above);
        }
        above = nabove;
    }
    if ( !got_player )
        op->stats.food = 1;
}
