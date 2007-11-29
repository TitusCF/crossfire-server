/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2007 Mark Wedel & Crossfire Development Team
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

/** @file peacemaker/peacemaker.c
 * The implementation of the Peacemaker class of objects.
 * @todo Consider merging Peacemaker with Spell Effects
 */
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>
#include <sproto.h>

static method_ret peacemaker_type_process(ob_methods *context, object *op);

/**
 * Initializer for the peacemaker object type.
 */
void init_type_peacemaker(void)
{
    register_process(PEACEMAKER, peacemaker_type_process);
}

/**
 * Handle ob_process for all peacemaker objects. Makes monster it passes
 * peaceful.
 * @param context The method context
 * @param op The peacemaker that's being processed.
 * @return METHOD_OK
 */
static method_ret peacemaker_type_process(ob_methods *context, object *op) {
    object *tmp;

	for(tmp=get_map_ob(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above) {
    	int atk_lev, def_lev;
    	object *victim=tmp;

    	if (tmp->head) victim=tmp->head;
    	if (!QUERY_FLAG(victim,FLAG_MONSTER)) continue;
    	if (QUERY_FLAG(victim,FLAG_UNAGGRESSIVE)) continue;
    	if (victim->stats.exp == 0) continue;

    	def_lev = MAX(1,victim->level);
    	atk_lev = MAX(1,op->level);

    	if (rndm(0, atk_lev-1) > def_lev) {
    	    /* make this sucker peaceful. */

    	    change_exp(get_owner(op),victim->stats.exp, op->skill, 0);
    	    victim->stats.exp=0;
    	    victim->attack_movement = RANDO2;
    	    SET_FLAG(victim,FLAG_UNAGGRESSIVE);
    	    SET_FLAG(victim,FLAG_RUN_AWAY);
    	    SET_FLAG(victim,FLAG_RANDOM_MOVE);
    	    CLEAR_FLAG(victim,FLAG_MONSTER);
    	    if(victim->name) {
    		draw_ext_info_format(NDI_UNIQUE,0,op->owner,
    				     MSG_TYPE_SPELL, MSG_TYPE_SPELL_SUCCESS,
    				     "%s no longer feels like fighting.",
    				     "%s no longer feels like fighting.",
    				     victim->name);
    	    }
    	}
	}
    return METHOD_OK;
}
