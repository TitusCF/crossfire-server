/*
 * static char *rcsid_skills_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2003-2006 Mark Wedel & Crossfire Development Team
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
 * This file contains core skill handling.
 */

#include <global.h>
#include <object.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <living.h>
#include <skills.h>
#include <spells.h>
#include <book.h>

/**
 * Computes stealing chance.
 * Increased values indicate better attempts.
 *
 * @param op
 * who is stealing.
 * @param victim
 * who to steal from.
 * @param roll
 * dice roll.
 * @return
 * -1 if op can't steal, else adjusted roll value.
 * @todo rename roll to something more meaningful (check attempt_steal()).
 */
static int adj_stealchance (object *op, object *victim, int roll) {
    object *equip;

    if(!op||!victim||!roll) return -1;

    /* Only prohibit stealing if the player does not have a free
     * hand available and in fact does have hands.
     */
    if(op->type==PLAYER && op->body_used[BODY_ARMS] <=0 &&
       op->body_info[BODY_ARMS]) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		      "But you have no free hands to steal with!", NULL);
	return -1;
    }

    /* ADJUSTMENTS */

    /* Its harder to steal from hostile beings! */
    if(!QUERY_FLAG(victim, FLAG_UNAGGRESSIVE)) roll = roll/2;

    /* Easier to steal from sleeping beings, or if the thief is
     * unseen */
    if(QUERY_FLAG(victim, FLAG_SLEEP))
	roll = roll*3;
    else if(op->invisible)
	roll = roll*2;

    /* check stealing 'encumberance'. Having this equipment applied makes
     * it quite a bit harder to steal.
     */
    for(equip=op->inv;equip;equip=equip->below) {
	if(equip->type==WEAPON&&QUERY_FLAG(equip,FLAG_APPLIED)) {
	    roll -= equip->weight/10000;
	}
	if(equip->type==BOW&&QUERY_FLAG(equip,FLAG_APPLIED))
	    roll -= equip->weight/5000;
	if(equip->type==SHIELD&&QUERY_FLAG(equip,FLAG_APPLIED)) {
	    roll -= equip->weight/2000;
	}
	if(equip->type==ARMOUR&&QUERY_FLAG(equip,FLAG_APPLIED))
	    roll -= equip->weight/5000;
	if(equip->type==GLOVES&&QUERY_FLAG(equip,FLAG_APPLIED))
	    roll -= equip->weight/100;
    }
    if(roll<0) roll=0;
    return roll;
}

/**
 * Steal objects.
 * When stealing: dependent on the intelligence/wisdom of whom you're
 * stealing from (op in attempt_steal), offset by your dexterity and
 * skill at stealing. They may notice your attempt, whether successful
 * or not.
 *
 * @param op
 * target (person being pilfered).
 * @param who
 * person doing the stealing.
 * @param skill
 * stealing skill object.
 * @retval 0
 * nothing was stolen.
 * @retval 1
 * something was stolen.
 */
static int attempt_steal(object* op, object* who, object *skill)
{
    object *success=NULL, *tmp=NULL, *next;
    int roll=0, chance=0, stats_value;
    rv_vector	rv;
    char name[MAX_BUF];

    stats_value = ((who->stats.Dex + who->stats.Int) * 3) / 2;

    /* if the victim is aware of a thief in the area (FLAG_NO_STEAL set on them)
     * they will try to prevent stealing if they can. Only unseen theives will
     * have much chance of success.
     */
    if(op->type!=PLAYER && QUERY_FLAG(op,FLAG_NO_STEAL)) {
	if(can_detect_enemy(op,who,&rv)) {
	    npc_call_help(op);
	    CLEAR_FLAG(op, FLAG_UNAGGRESSIVE);
	    draw_ext_info(NDI_UNIQUE, 0,who, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			  "Your attempt is prevented!", NULL);
	    return 0;
	} else /* help npc to detect thief next time by raising its wisdom */
	    op->stats.Wis += (op->stats.Int/5)+1;
	    if (op->stats.Wis > MAX_STAT) op->stats.Wis = MAX_STAT;
    }
    if (op->type == PLAYER && QUERY_FLAG(op, FLAG_WIZ)) {
	draw_ext_info(NDI_UNIQUE, 0, who, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		      "You can't steal from the dungeon master!", NULL);
	return 0;
    }
    if(op->type == PLAYER && who->type == PLAYER && settings.no_player_stealing) {
      draw_ext_info(NDI_UNIQUE, 0, who, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		    "You can't steal from other players!", NULL);
      return 0;
    }


    /* Ok then, go thru their inventory, stealing */
    for(tmp = op->inv; tmp != NULL; tmp = next) {
	next = tmp->below;

	/* you can't steal worn items, starting items, wiz stuff,
	 * innate abilities, or items w/o a type. Generally
	 * speaking, the invisibility flag prevents experience or
	 * abilities from being stolen since these types are currently
	 * always invisible objects. I was implicit here so as to prevent
	 * future possible problems. -b.t.
	 * Flesh items generated w/ fix_flesh_item should have FLAG_NO_STEAL
	 * already  -b.t.
	 */

	if (QUERY_FLAG(tmp,FLAG_WAS_WIZ) || QUERY_FLAG(tmp, FLAG_APPLIED)
	    || !(tmp->type)
	    || tmp->type == EXPERIENCE || tmp->type == SPELL
	    || QUERY_FLAG(tmp,FLAG_STARTEQUIP)
	    || QUERY_FLAG(tmp,FLAG_NO_STEAL)
	    || tmp->invisible ) continue;

	/* Okay, try stealing this item. Dependent on dexterity of thief,
	 * skill level, see the adj_stealroll fctn for more detail.
	 */

	roll=die_roll(2, 100, who, PREFER_LOW)/2; /* weighted 1-100 */

	if((chance=adj_stealchance(who,op,(stats_value+skill->level * 10 - op->level * 3)))==-1)
	    return 0;
	else if (roll < chance ) {
	    tag_t tmp_count = tmp->count;

	    pick_up(who, tmp);
	    /* need to see if the player actually stole this item -
	     * if it is in the players inv, assume it is.  This prevents
	     * abuses where the player can not carry the item, so just
	     * keeps stealing it over and over.
	     */
	    if (was_destroyed(tmp, tmp_count) || tmp->env != op) {
		/* for players, play_sound: steals item */
		success = tmp;
		CLEAR_FLAG(tmp, FLAG_INV_LOCKED);
	    }
	    break;
	}
    } /* for loop looking for an item */

    if (!tmp) {
        query_name(op, name, MAX_BUF);
	draw_ext_info_format(NDI_UNIQUE, 0, who, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			     "%s%s has nothing you can steal!",
			     "%s%s has nothing you can steal!",
			     op->type == PLAYER ? "" : "The ", name);
	return 0;
    }

    /* If you arent high enough level, you might get something BUT
     * the victim will notice your stealing attempt. Ditto if you
     * attempt to steal something heavy off them, they're bound to notice
     */

    if((roll>=skill->level) || !chance
      ||(tmp && tmp->weight>(250*(random_roll(0, stats_value+skill->level * 10-1, who, PREFER_LOW))))) {

	/* victim figures out where the thief is! */
	if(who->hide) make_visible(who);

	if(op->type != PLAYER) {
	    /* The unaggressives look after themselves 8) */
	    if(who->type==PLAYER) {
		npc_call_help(op);
        query_name(op, name, MAX_BUF);
		draw_ext_info_format(NDI_UNIQUE, 0,who,
			     MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			     "%s notices your attempted pilfering!",
			     "%s notices your attempted pilfering!",
			     name);
	    }
	    CLEAR_FLAG(op, FLAG_UNAGGRESSIVE);
	    /* all remaining npc items are guarded now. Set flag NO_STEAL
	     * on the victim.
	     */
	    SET_FLAG(op,FLAG_NO_STEAL);
	} else { /* stealing from another player */
	    char buf[MAX_BUF];
	    /* Notify the other player */
	    if (success && who->stats.Int > random_roll(0, 19, op, PREFER_LOW)) {
            query_name(success, name, MAX_BUF);
		sprintf(buf, "Your %s is missing!", name);
	    } else {
		sprintf(buf, "Your pack feels strangely lighter.");
	    }
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_ITEM, MSG_TYPE_ITEM_REMOVE,
			  buf, buf);
	    if (!success) {
		if (who->invisible) {
		    sprintf(buf, "you feel itchy fingers getting at your pack.");
		} else {
            query_name(who, name, MAX_BUF);
		    sprintf(buf, "%s looks very shifty.", name);
		}
		draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_VICTIM, MSG_TYPE_VICTIM_STEAL,
			      buf, buf);
	    }
	} /* else stealing from another player */
	/* play_sound("stop! thief!"); kindofthing */
    } /* if you weren't 100% successful */
    return success? 1:0;
}


/**
 * Main stealing function.
 *
 * @param op
 * thief.
 * @param dir
 * direction to steal from.
 * @param skill
 * stealing skill.
 * @return
 * experience gained for stealing, 0 if nothing was stolen.
 */
int steal(object* op, int dir, object *skill)
{
    object *tmp, *next;
    sint16 x, y;
    mapstruct *m;
    int mflags;

    x = op->x + freearr_x[dir];
    y = op->y + freearr_y[dir];

    if(dir == 0) {
	/* Can't steal from ourself! */
	return 0;
    }

    m = op->map;
    mflags = get_map_flags(m, &m ,x,y, &x, &y);
    /* Out of map - can't do it.  If nothing alive on this space,
     * don't need to look any further.
     */
    if ((mflags & P_OUT_OF_MAP) || !(mflags & P_IS_ALIVE))
	return 0;

    /* If player can't move onto the space, can't steal from it. */
    if (OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, x, y)))
	return 0;

    /* Find the topmost object at this spot */
    for(tmp = get_map_ob(m,x,y);
	tmp != NULL && tmp->above != NULL;
        tmp = tmp->above);

    /* For all the stacked objects at this point, attempt a steal */
    for(; tmp != NULL; tmp = next) {
	next = tmp->below;
	/* Minor hack--for multi square beings - make sure we get
	 * the 'head' coz 'tail' objects have no inventory! - b.t.
	 */
	if (tmp->head) tmp=tmp->head;

	if(tmp->type!=PLAYER && !QUERY_FLAG(tmp, FLAG_MONSTER)) continue;

	/* do not reveal hidden DMs */
	if (tmp->type == PLAYER && QUERY_FLAG(tmp, FLAG_WIZ) && tmp->contr->hidden) continue;
	if (attempt_steal(tmp, op, skill)) {
	    if (tmp->type==PLAYER) /* no xp for stealing from another player */
		return 0;

	    /* no xp for stealing from pets (of players) */
	    if (QUERY_FLAG(tmp, FLAG_FRIENDLY) && tmp->attack_movement == PETMOVE) {
		object *owner = get_owner(tmp);
		if (owner != NULL && owner->type == PLAYER)
		    return 0;
	    }

	    return (calc_skill_exp(op,tmp, skill));
	}
    }
    return 0;
}

/**
 * Attempt to pick a lock. Handles traps.
 *
 * @param door
 * lock to pick.
 * @param pl
 * player picking.
 * @param skill
 * locking skill.
 * @retval 0
 * no lock was picked.
 * @retval 1
 * door was locked.
 */
static int attempt_pick_lock (object *door, object *pl, object *skill)
{
    int difficulty= pl->map->difficulty ? pl->map->difficulty : 0;
    int success = 0, number;        /* did we get anything? */


    /* Try to pick the lock on this item (doors only for now).
     * Dependent on dexterity/skill SK_level of the player and
     * the map level difficulty.
     */
    number = (die_roll(2, 40, pl, PREFER_LOW)-2)/2;
    if (number < (pl->stats.Dex + skill->level - difficulty)) {
	remove_door(door);
	success = 1;
    } else if (door->inv && (door->inv->type==RUNE || door->inv->type==TRAP)) {  /* set off any traps? */
	spring_trap(door->inv,pl);
    }
    return success;
}


/**
 * Lock pick handling.
 *
 * Implementation by bt. (thomas@astro.psu.edu)
 * monster implementation 7-7-95 by bt.
 *
 * @param pl
 * player picking the lock.
 * @param dir
 * direction to pick.
 * @param skill
 * lock picking skill.
 * @return
 * experience for picking a lock, 0 if nothing was picked.
 */
int pick_lock(object *pl, int dir, object *skill)
{
    object *tmp;
    int x = pl->x + freearr_x[dir];
    int y = pl->y + freearr_y[dir];

    if(!dir) dir=pl->facing;

    /* For all the stacked objects at this point find a door*/
    if (out_of_map(pl->map,x,y)) {
	draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "There is no lock there.", NULL);
	return 0;
    }

    for(tmp=get_map_ob(pl->map,x,y); tmp; tmp=tmp->above)
	if (tmp->type == DOOR || tmp->type == LOCKED_DOOR) break;

    if (!tmp) {
	draw_ext_info(NDI_UNIQUE, 0,pl,MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "There is no lock there.", NULL);
	return 0;
    }
    if (tmp->type == LOCKED_DOOR) {
	draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "You can't pick that lock!", NULL);
	return 0;
    }

    if (!tmp->move_block) {
	draw_ext_info(NDI_UNIQUE, 0,pl,MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "The door has no lock!", NULL);
	return 0;
    }

    if (attempt_pick_lock(tmp, pl, skill)) {
	draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
		      "You pick the lock.", NULL);
	return calc_skill_exp(pl,NULL, skill);
    } else {
	draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		      "You fail to pick the lock.", NULL);
	return 0;
    }
}


/**
 * Someone is trying to hide.
 * The user becomes undetectable (not just 'invisible') for
 * a short while (success and duration dependant on player SK_level,
 * dexterity, charisma, and map difficulty).
 * Players have a good chance of becoming 'unhidden' if they move
 * and like invisiblity will be come visible if they attack
 * Implemented by b.t. (thomas@astro.psu.edu)
 * July 7, 1995 - made hiding possible for monsters. -b.t.
 *
 * @param op
 * living trying to hide.
 * @param skill
 * hiding skill.
 * @retval 0
 * op couldn't hide.
 * @retval 1
 * op successfully hide.
 */
static int attempt_hide(object *op, object *skill) {
    int number,difficulty=op->map->difficulty;
    int terrain = hideability(op);

    if(terrain<-10) /* not enough cover here */
	return 0;

    /*  Hiding success and duration dependant on skill level,
     *  op->stats.Dex, map difficulty and terrain.
     */

    number = (die_roll(2, 25, op, PREFER_LOW)-2)/2;
    if(!stand_near_hostile(op) && (number < (op->stats.Dex + skill->level + terrain - difficulty))) {
	op->invisible += 100;  /* set the level of 'hiddeness' */
	if(op->type==PLAYER)
	    op->contr->tmp_invis=1;
	op->hide=1;
	return 1;
    }
    return 0;
}

/**
 * Main hide handling.
 * @param op
 * living trying to hide.
 * @param skill
 * hiding skill.
 * @return
 * experience gained for the skill use (can be 0).
 */
int hide(object *op, object *skill) {

    /* the preliminaries -- Can we really hide now? */
    /* this keeps monsters from using invisibilty spells and hiding */

    if (QUERY_FLAG(op, FLAG_MAKE_INVIS)) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "You don't need to hide while invisible!", NULL);
	return 0;
    } else if (!op->hide && op->invisible>0 && op->type == PLAYER) {
	draw_ext_info(NDI_UNIQUE, 0,op,
		      MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_END,
		      "Your attempt to hide breaks the invisibility spell!", NULL);
	make_visible(op);
    }

    if(op->invisible>(50*skill->level)) {
	draw_ext_info(NDI_UNIQUE,0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "You are as hidden as you can get.", NULL);
	return 0;
    }

    if(attempt_hide(op, skill)) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
		      "You hide in the shadows.", NULL);
	update_object(op,UP_OBJ_FACE);
	return calc_skill_exp(op, NULL, skill);
    }
    draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		  "You fail to conceal yourself.", NULL);
    return 0;
}


/**
 * End of jump. Restore the map.
 * @param pl
 * player.
 * @todo Is fix_object() required?
 */
static void stop_jump(object *pl) {
    fix_object(pl);
    insert_ob_in_map(pl,pl->map,pl,0);
}

/**
 * Someone is trying to jump.
 * @param pl
 * living trying to jump.
 * @param dir
 * direction to jump in.
 * @param spaces
 * distance to jump.
 * @param skill
 * jumping skill.
 * @return
 * experience gained when jumping into another living thing.
 */
static int attempt_jump (object *pl, int dir, int spaces, object *skill) {
    object *tmp;
    int i,exp=0,dx=freearr_x[dir],dy=freearr_y[dir], mflags;
    sint16 x, y;
    mapstruct *m;

    /* Jump loop. Go through spaces opject wants to jump. Halt the
     * jump if a wall or creature is in the way. We set FLAG_FLYING
     * temporarily to allow player to aviod exits/archs that are not
     * fly_on, fly_off. This will also prevent pickup of objects
     * while jumping over them.
     */

    remove_ob(pl);

    /*
     * I don't think this is actually needed - all the movement
     * code is handled in this function, and I don't see anyplace
     * that cares about the move_type being flying.
     */
    pl->move_type |= MOVE_FLY_LOW;

    for(i=0;i<=spaces;i++) {
	x = pl->x + dx;
	y = pl->y + dy;
	m = pl->map;

	mflags = get_map_flags(m, &m, x, y, &x, &y);

	if (mflags & P_OUT_OF_MAP) {
	    (void) stop_jump(pl);
	    return 0;
	}
	if (OB_TYPE_MOVE_BLOCK(pl, GET_MAP_MOVE_BLOCK(m, x, y))) {
	    draw_ext_info(NDI_UNIQUE, 0,pl,MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			  "Your jump is blocked.", NULL);
	    stop_jump(pl);
	    return 0;
	}

	for(tmp=get_map_ob(m, x, y); tmp;tmp=tmp->above) {
	    /* Jump into creature */
	    if(QUERY_FLAG(tmp, FLAG_MONSTER)
	    || (tmp->type==PLAYER && (!QUERY_FLAG(tmp, FLAG_WIZ) || !tmp->contr->hidden))) {
		draw_ext_info_format(NDI_UNIQUE, 0,pl,
			     MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
			     "You jump into %s%s.",
			     "You jump into %s%s.",
			     tmp->type == PLAYER ? "" : "the ", tmp->name);

		if(tmp->type!=PLAYER ||
		   (pl->type==PLAYER && pl->contr->party==NULL) ||
		   (pl->type==PLAYER && tmp->type==PLAYER &&
		    pl->contr->party!=tmp->contr->party))
			exp = skill_attack(tmp,pl,pl->facing,"kicked", skill); /* pl makes an attack */

		stop_jump(pl);
		return exp;  /* note that calc_skill_exp() is already called by skill_attack() */
	    }
	    /* If the space has fly on set (no matter what the space is),
	     * we should get the effects - after all, the player is
	     * effectively flying.
	     */
	    if (tmp->move_on & MOVE_FLY_LOW) {
		pl->x = x;
		pl->y = y;
		pl->map = m;
		if (pl->contr)
		    esrv_map_scroll(&pl->contr->socket, dx, dy);
		stop_jump(pl);
		return calc_skill_exp(pl,NULL, skill);
	    }
	}
	pl->x = x;
	pl->y = y;
	pl->map = m;
	if (pl->contr)
	    esrv_map_scroll(&pl->contr->socket, dx, dy);
    }
    stop_jump(pl);
    return calc_skill_exp(pl,NULL, skill);
}

/**
 * Jump skill handling.
 * This is both a new type of movement for player/monsters and
 * an attack as well.
 * Perhaps we should allow more spaces based on level, eg, level 50
 * jumper can jump several spaces?
 *
 * @param pl
 * object jumping.
 * @param dir
 * direction to jump to.
 * @param skill
 * jumping skill.
 * @return
 * experience gained for jumping.
 */
int jump(object *pl, int dir, object *skill)
{
    int spaces=0,stats;
    int str = pl->stats.Str;
    int dex = pl->stats.Dex;

    dex = dex ? dex : 15;
    str = str ? str : 10;

    stats=str*str*str*dex * skill->level;

    if(pl->carrying!=0)		/* don't want div by zero !! */
	spaces=(int) (stats/pl->carrying);
    else
	spaces=2;	/* pl has no objects - gets the far jump */

    if(spaces>2)
	 spaces = 2;
    else if(spaces==0) {
	draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		      "You are carrying too much weight to jump.", NULL);
	return 0;
    }
    return attempt_jump(pl,dir,spaces, skill);
}


/**
 * Check for cursed object with the 'detect curse' skill.
 *
 * @param pl
 * player detecting.
 * @param skill
 * detect skill object.
 * @return
 * amount of experience gained (on successful detecting).
 */
static int do_skill_detect_curse(object *pl, object *skill) {
    object *tmp;
    int success=0;

    for(tmp=pl->inv;tmp;tmp=tmp->below)
	if (!tmp->invisible
            && !QUERY_FLAG(tmp,FLAG_IDENTIFIED) && !QUERY_FLAG(tmp,FLAG_KNOWN_CURSED)
            && (QUERY_FLAG(tmp,FLAG_CURSED) || QUERY_FLAG(tmp,FLAG_DAMNED)) &&
	    tmp->item_power < skill->level) {
		SET_FLAG(tmp,FLAG_KNOWN_CURSED);
		esrv_update_item(UPD_FLAGS, pl, tmp);
		success+= calc_skill_exp(pl,tmp, skill);
	}

    /* Check ground, too, but only objects the player could pick up */
    for(tmp=get_map_ob(pl->map,pl->x,pl->y);tmp;tmp=tmp->above)
	if (can_pick(pl, tmp) &&
	    !QUERY_FLAG(tmp,FLAG_IDENTIFIED) &&
	    !QUERY_FLAG(tmp,FLAG_KNOWN_CURSED)
            && (QUERY_FLAG(tmp,FLAG_CURSED) || QUERY_FLAG(tmp,FLAG_DAMNED)) &&
	    tmp->item_power < skill->level) {
		SET_FLAG(tmp,FLAG_KNOWN_CURSED);
		esrv_update_item(UPD_FLAGS, pl, tmp);
		success+= calc_skill_exp(pl,tmp, skill);
	}

    return success;
}

/**
 * Check for magic object with the 'detect magic' skill.
 *
 * @param pl
 * player detecting.
 * @param skill
 * detect skill object.
 * @return
 * amount of experience gained (on successful detecting).
 */
static int do_skill_detect_magic(object *pl, object *skill) {
    object *tmp;
    int success=0;

    for(tmp=pl->inv;tmp;tmp=tmp->below)
        if(!tmp->invisible
	    && !QUERY_FLAG(tmp,FLAG_IDENTIFIED) && !QUERY_FLAG(tmp,FLAG_KNOWN_MAGICAL)
	    && (is_magical(tmp)) && tmp->item_power < skill->level) {
            	SET_FLAG(tmp,FLAG_KNOWN_MAGICAL);
		esrv_update_item(UPD_FLAGS, pl, tmp);
		success+=calc_skill_exp(pl,tmp, skill);
	}

    /* Check ground, too, but like above, only if the object can be picked up*/
    for(tmp=get_map_ob(pl->map,pl->x,pl->y);tmp;tmp=tmp->above)
        if (can_pick(pl, tmp) &&
	    !QUERY_FLAG(tmp,FLAG_IDENTIFIED) &&
	    !QUERY_FLAG(tmp,FLAG_KNOWN_MAGICAL)
	    && (is_magical(tmp)) && tmp->item_power < skill->level) {
            	SET_FLAG(tmp,FLAG_KNOWN_MAGICAL);
		esrv_update_item(UPD_FLAGS, pl, tmp);
		success+=calc_skill_exp(pl,tmp, skill);
	}

    return success;
}

/**
 * Helper function for do_skill_ident, so that we can loop
 * over inventory AND objects on the ground conveniently.
 *
 * @param tmp
 * object to try to identify.
 * @param pl
 * object identifying.
 * @param obj_class
 * object type to identify.
 * @param skill
 * identification skill.
 * @return
 * experience for successful identification.
 */
static int do_skill_ident2(object *tmp,object *pl, int obj_class, object *skill)
{
    int success=0,chance, ip;
    int skill_value = skill->level * pl->stats.Int?pl->stats.Int:10;

    if(!QUERY_FLAG(tmp,FLAG_IDENTIFIED) && !QUERY_FLAG(tmp,FLAG_NO_SKILL_IDENT)
       && need_identify(tmp) && !tmp->invisible && tmp->type==obj_class) {
	ip = tmp->magic;
	if (tmp->item_power > ip) ip=tmp->item_power;

	    chance = die_roll(3, 10, pl, PREFER_LOW)-3 +
			rndm(0, (tmp->magic ? tmp->magic*5 : 1)-1);
		if(skill_value >= chance) {
		  identify(tmp);
     		  if (pl->type==PLAYER) {
                  char desc[MAX_BUF];
        	    draw_ext_info_format(NDI_UNIQUE, 0, pl,
				 MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
				 "You identify %s.",
				 "You identify %s.",
				 ob_describe(tmp, pl, desc, sizeof(desc)));
        	    if (tmp->msg) {
          		draw_ext_info_format(NDI_UNIQUE, 0,pl,
				     MSG_TYPE_ITEM, MSG_TYPE_ITEM_INFO,
				     "The item has a story:\n%s",
				     "The item has a story:\n%s",
				     tmp->msg);

        	    }
		  }
	          success += calc_skill_exp(pl,tmp, skill);
        	} else
		  SET_FLAG(tmp, FLAG_NO_SKILL_IDENT);
        }
	return success;
}

/**
 * Workhorse for skill_ident() -b.t.
 *
 * @param pl
 * player identifying.
 * @param obj_class
 * type of objects to identify.
 * @param skill
 * skill to give experience to.
 * @return
 * experience gained by identifying items.
 */
static int do_skill_ident(object *pl, int obj_class, object *skill) {
    object *tmp;
    int success=0, area, i;

        /* check the player */
    for(tmp=pl->inv;tmp;tmp=tmp->below)
        success+=do_skill_ident2(tmp,pl,obj_class, skill);

        /*  check the ground */
        /* Altered to allow ident skills to increase in area with
         * experience. -- Aaron Baugher
         */

    if( skill->level > 64 ){   /* Adjust these levels? */
        area = 49;
    }
    else if( skill->level > 16 ){
        area = 25;
    }
    else if( skill->level > 4 ){
        area = 9;
    }
    else {
        area = 1;
    }

    for(i=0; i<area; i++) {
        sint16 x = pl->x+freearr_x[i];
        sint16 y = pl->y+freearr_y[i];
        mapstruct *m = pl->map;
        int mflags;

        mflags =get_map_flags(m, &m, x,y, &x, &y);
        if (mflags & P_OUT_OF_MAP) continue;

        if( can_see_monsterP(m,pl->x,pl->y,i) ){
            for(tmp=get_map_ob(m,x,y);tmp;tmp=tmp->above){
                success+=do_skill_ident2(tmp,pl,obj_class, skill);
            }
        }
    }
    return success;
}

/**
 * Main identification skill handling.
 * @param pl
 * player identifying.
 * @param skill
 * identification skill.
 * @return
 * experience gained for identification.
 */
int skill_ident(object *pl, object *skill) {
    int success=0;

    if(pl->type != PLAYER) return 0;  /* only players will skill-identify */

    draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
		  "You look at the objects nearby...", NULL);

    switch (skill->subtype) {
	case SK_SMITHERY:
	    success += do_skill_ident(pl,WEAPON, skill) + do_skill_ident(pl,ARMOUR, skill)
		    + do_skill_ident(pl,BRACERS,skill) + do_skill_ident(pl,CLOAK,skill)
		    + do_skill_ident(pl,BOOTS,skill) + do_skill_ident(pl,SHIELD,skill)
		    + do_skill_ident(pl,GIRDLE,skill) + do_skill_ident(pl,HELMET,skill)
		    + do_skill_ident(pl,GLOVES,skill);
	    break;

	case SK_BOWYER:
	    success += do_skill_ident(pl,BOW,skill) + do_skill_ident(pl,ARROW,skill);
	    break;

	case SK_ALCHEMY:
	    success += do_skill_ident(pl,POTION,skill) + do_skill_ident(pl,POISON,skill)
		    + do_skill_ident(pl,CONTAINER,skill)
		    + do_skill_ident(pl,DRINK,skill) + do_skill_ident(pl,INORGANIC,skill);
	    break;

	case SK_WOODSMAN:
	    success += do_skill_ident(pl,FOOD,skill) + do_skill_ident(pl,DRINK,skill)
		    + do_skill_ident(pl,FLESH,skill);
	    break;

	case SK_JEWELER:
	    success += do_skill_ident(pl,GEM,skill) + do_skill_ident(pl,RING,skill) +
		do_skill_ident(pl,AMULET,skill);
	    break;

	case SK_LITERACY:
	    success += do_skill_ident(pl,SPELLBOOK,skill)
		    + do_skill_ident(pl,SCROLL,skill) + do_skill_ident(pl,BOOK,skill);
	    break;

	case SK_THAUMATURGY:
	    success += do_skill_ident(pl,WAND,skill) + do_skill_ident(pl,ROD,skill)
		    + do_skill_ident(pl,HORN,skill);
	    break;

	case SK_DET_CURSE:
	    success = do_skill_detect_curse(pl,skill);
	    if(success)
		draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
			      "...and discover cursed items!", NULL);
	    break;

	case SK_DET_MAGIC:
	    success = do_skill_detect_magic(pl,skill);
	    if(success)
		draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
		      "...and discover items imbued with mystic forces!", NULL);
	    break;

	default:
	    LOG(llevError,"Error: bad call to skill_ident()\n");
	    return 0;
	    break;
    }
    if(!success) {
	draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
		      "...and learn nothing more.", NULL);
    }
    return success;
}


/**
 * Oratory skill handling.
 * Players using this skill can 'charm' a monster --
 * into working for them. It can only be used on
 * non-special (see below) 'neutral' creatures.
 * -b.t. (thomas@astro.psu.edu)
 *
 * @param pl
 * player trying to convince a monster.
 * @param dir
 * direction to orate in.
 * @param skill
 * oratory skill object.
 * @return
 * experience gained for oratoring.
 * @todo
 * check if can't be simplified, code looks duplicated.
 */
int use_oratory(object *pl, int dir, object *skill) {
    sint16 x=pl->x+freearr_x[dir],y=pl->y+freearr_y[dir];
    int mflags,chance;
    object *tmp;
    mapstruct *m;
    char name[MAX_BUF];

    if(pl->type!=PLAYER) return 0;	/* only players use this skill */
    m = pl->map;
    mflags =get_map_flags(m, &m, x,y, &x, &y);
    if (mflags & P_OUT_OF_MAP) return 0;

    /* Save some processing - we have the flag already anyways
     */
    if (!(mflags & P_IS_ALIVE)) {
	draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		      "There is nothing to orate to.", NULL);
	return 0;
    }

    for(tmp=get_map_ob(m,x,y);tmp;tmp=tmp->above) {
        /* can't persuade players - return because there is nothing else
	 * on that space to charm.  Same for multi space monsters and
	 * special monsters - we don't allow them to be charmed, and there
	 * is no reason to do further processing since they should be the
	 * only monster on the space.
	 */
       	if(tmp->type==PLAYER) return 0;
       	if(tmp->more || tmp->head) return 0;
	if(tmp->msg) return 0;

	if(QUERY_FLAG(tmp,FLAG_MONSTER)) break;
    }

    if (!tmp) {
	draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		      "There is nothing to orate to.", NULL);
	return 0;
    }

    query_name(tmp, name, MAX_BUF);
    draw_ext_info_format(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
			 "You orate to the %s.",
			 "You orate to the %s.",
			 name);

    /* the following conditions limit who may be 'charmed' */

    /* it's hostile! */
    if(!QUERY_FLAG(tmp,FLAG_UNAGGRESSIVE) && !QUERY_FLAG(tmp, FLAG_FRIENDLY)) {
        query_name(tmp, name, MAX_BUF);
	draw_ext_info_format(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			     "Too bad the %s isn't listening!",
			     "Too bad the %s isn't listening!",
			     name);
	return 0;
    }

    /* it's already allied! */
    if(QUERY_FLAG(tmp,FLAG_FRIENDLY)&&(tmp->attack_movement==PETMOVE)){
	if(get_owner(tmp)==pl) {
	    draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
		      "Your follower loves your speech.", NULL);
	    return 0;
	} else if (skill->level > tmp->level) {
	    /* you steal the follower.  Perhaps we should really look at the
	     * level of the owner above?
	     */
	    set_owner(tmp,pl);
        query_name(tmp, name, MAX_BUF);
	    draw_ext_info_format(NDI_UNIQUE, 0,pl,
				 MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
				 "You convince the %s to follow you instead!",
				 "You convince the %s to follow you instead!",
				 name);

	    /* Abuse fix - don't give exp since this can otherwise
	     * be used by a couple players to gets lots of exp.
	     */
	    return 0;
	} else {
	    /* In this case, you can't steal it from the other player */
	    return 0;
	}
    } /* Creature was already a pet of someone */

    chance=skill->level*2+(pl->stats.Cha-2*tmp->stats.Int)/2;

    /* Ok, got a 'sucker' lets try to make them a follower */
    if(chance>0 && tmp->level<(random_roll(0, chance-1, pl, PREFER_HIGH)-1)) {
        query_name(tmp, name, MAX_BUF);
	draw_ext_info_format(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
	     "You convince the %s to become your follower.",
	     "You convince the %s to become your follower.",
	     name);

	set_owner(tmp,pl);
	tmp->stats.exp = 0;
	add_friendly_object(tmp);
	SET_FLAG(tmp,FLAG_FRIENDLY);
	tmp->attack_movement = PETMOVE;
	return calc_skill_exp(pl,tmp, skill);
    }
    /* Charm failed.  Creature may be angry now */
    else if((skill->level+((pl->stats.Cha-10)/2)) < random_roll(1, 2*tmp->level, pl, PREFER_LOW)) {
        query_name(tmp, name, MAX_BUF);
	draw_ext_info_format(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		     "Your speech angers the %s!",
		     "Your speech angers the %s!",
		     name);

	if(QUERY_FLAG(tmp,FLAG_FRIENDLY)) {
	    CLEAR_FLAG(tmp,FLAG_FRIENDLY);
	    remove_friendly_object(tmp);
	    tmp->attack_movement = 0; 	/* needed? */
	}
	CLEAR_FLAG(tmp,FLAG_UNAGGRESSIVE);
    }
    return 0;	/* Fall through - if we get here, we didn't charm anything */
}

/**
 * Singing skill handling.
 * This skill allows the player to pacify nearby creatures.
 * There are few limitations on who/what kind of
 * non-player creatures that may be pacified. Right now, a player
 * may pacify creatures which have Int == 0. In this routine, once
 * successfully pacified the creature gets Int=1. Thus, a player
 * may only pacify a creature once.
 * BTW, I appologize for the naming of the skill, I couldnt think
 * of anything better! -b.t.
 *
 * @param pl
 * player singing.
 * @param dir
 * direction to sing in.
 * @param skill
 * singing skill object.
 * @return
 * experience gained for singing.
 */
int singing(object *pl, int dir, object *skill) {
    int i,exp = 0,chance, mflags;
    object *tmp;
    mapstruct *m;
    sint16  x, y;
    char name[MAX_BUF];

    if(pl->type!=PLAYER) return 0;    /* only players use this skill */

    draw_ext_info_format(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
			 "You sing", NULL);
    for(i=dir;i<(dir+MIN(skill->level,SIZEOFFREE));i++) {
	x = pl->x+freearr_x[i];
	y = pl->y+freearr_y[i];
	m = pl->map;

	mflags =get_map_flags(m, &m, x,y, &x, &y);
	if (mflags & P_OUT_OF_MAP) continue;
	if (!(mflags & P_IS_ALIVE)) continue;

	for(tmp=get_map_ob(m, x, y); tmp;tmp=tmp->above) {
	    if(QUERY_FLAG(tmp,FLAG_MONSTER)) break;
	    /* can't affect players */
            if(tmp->type==PLAYER) break;
	}

	/* Whole bunch of checks to see if this is a type of monster that would
	 * listen to singing.
	 */
	if (tmp && QUERY_FLAG(tmp, FLAG_MONSTER) &&
	    !QUERY_FLAG(tmp, FLAG_NO_STEAL) &&	    /* Been charmed or abused before */
	    !QUERY_FLAG(tmp, FLAG_SPLITTING) &&	    /* no ears */
	    !QUERY_FLAG(tmp, FLAG_HITBACK) &&	    /* was here before */
	    (tmp->level <= skill->level) &&
	    (!tmp->head) &&
	    !QUERY_FLAG(tmp, FLAG_UNDEAD) &&
	    !QUERY_FLAG(tmp,FLAG_UNAGGRESSIVE) &&   /* already calm */
	    !QUERY_FLAG(tmp,FLAG_FRIENDLY)) {	    /* already calm */

	    /* stealing isn't really related (although, maybe it should
	     * be).  This is mainly to prevent singing to the same monster
	     * over and over again and getting exp for it.
	     */
	    chance=skill->level*2+(pl->stats.Cha-5-tmp->stats.Int)/2;
	    if(chance && tmp->level*2<random_roll(0, chance-1, pl, PREFER_HIGH)) {
		SET_FLAG(tmp,FLAG_UNAGGRESSIVE);
        query_name(tmp, name, MAX_BUF);
		draw_ext_info_format(NDI_UNIQUE, 0,pl,
				     MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
				     "You calm down the %s",
				     "You calm down the %s",
				     name);

		/* Give exp only if they are not aware */
		if(!QUERY_FLAG(tmp,FLAG_NO_STEAL))
		    exp += calc_skill_exp(pl,tmp, skill);
		SET_FLAG(tmp,FLAG_NO_STEAL);
	    } else {
            query_name(tmp, name, MAX_BUF);
                 draw_ext_info_format(NDI_UNIQUE, 0,pl,
				      MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
				      "Too bad the %s isn't listening!",
				      "Too bad the %s isn't listening!",
				      name);
		SET_FLAG(tmp,FLAG_NO_STEAL);
	    }
	}
    }
    return exp;
}

/**
 * Checks for traps on the spaces around the player or in certain objects.
 *
 * @param pl
 * player searching.
 * @param skill
 * find trap skill object.
 * @return
 * experience gained for finding traps.
 */
int find_traps (object *pl, object *skill) {
    object *tmp,*tmp2;
    int i,expsum=0, mflags;
    sint16 x,y;
    mapstruct *m;

    /* First we search all around us for runes and traps, which are
     * all type RUNE
     */

    for(i=0;i<9;i++) {
	x = pl->x+freearr_x[i];
	y = pl->y+freearr_y[i];
	m = pl->map;

	mflags =get_map_flags(m, &m, x,y, &x, &y);
	if (mflags & P_OUT_OF_MAP) continue;

        /*  Check everything in the square for trapness */
        for(tmp = get_map_ob(m, x, y); tmp!=NULL;tmp=tmp->above) {

            /* And now we'd better do an inventory traversal of each
             * of these objects' inventory
	     * We can narrow this down a bit - no reason to search through
	     * the players inventory or monsters for that matter.
	     */
	    if (tmp->type != PLAYER && !QUERY_FLAG(tmp, FLAG_MONSTER)) {
		for(tmp2=tmp->inv;tmp2!=NULL;tmp2=tmp2->below)
		    if(tmp2->type==RUNE || tmp2->type == TRAP)
			if(trap_see(pl,tmp2)) {
			    trap_show(tmp2,tmp);
			    if(tmp2->stats.Cha>1) {
				if (!tmp2->owner || tmp2->owner->type!=PLAYER)
				    expsum += calc_skill_exp(pl,tmp2, skill);

				tmp2->stats.Cha = 1; /* unhide the trap */
			    }
			}
	    }
            if((tmp->type==RUNE || tmp->type == TRAP) && trap_see(pl,tmp)) {
		trap_show(tmp,tmp);
		if(tmp->stats.Cha>1) {
		    if (!tmp->owner || tmp->owner->type!=PLAYER)
			expsum += calc_skill_exp(pl,tmp, skill);
			tmp->stats.Cha = 1; /* unhide the trap */
		}
	    }
	}
    }
    draw_ext_info(NDI_BLACK, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
		  "You search the area.", NULL);
    return expsum;
}

/**
 * This skill will disarm any previously discovered trap.
 * the algorithm is based (almost totally) on the old command_disarm() - b.t.
 *
 * @param op
 * player disarming. Must be on a map.
 * @param skill
 * disarming skill.
 * @return
 * experience gained to disarm.
 */
int remove_trap (object *op, object *skill) {
    object *tmp,*tmp2;
    int i,success=0,mflags;
    mapstruct *m;
    sint16 x,y;

    for(i=0;i<9;i++) {
	x = op->x + freearr_x[i];
	y = op->y + freearr_y[i];
	m = op->map;

	mflags =get_map_flags(m, &m, x,y, &x, &y);
	if (mflags & P_OUT_OF_MAP) continue;

	/* Check everything in the square for trapness */
	for(tmp = get_map_ob(m,x,y);tmp!=NULL;tmp=tmp->above) {
	    /* And now we'd better do an inventory traversal of each
	     * of these objects inventory.  Like above, only
	     * do this for interesting objects.
	     */

	    if (tmp->type != PLAYER && !QUERY_FLAG(tmp, FLAG_MONSTER)) {
		for(tmp2=tmp->inv;tmp2!=NULL;tmp2=tmp2->below)
		    if((tmp2->type==RUNE || tmp2->type == TRAP) && tmp2->stats.Cha<=1) {
			trap_show(tmp2,tmp);
			if(trap_disarm(op,tmp2,1, skill) && (!tmp2->owner || tmp2->owner->type!=PLAYER)) {
			    tmp->stats.exp = tmp->stats.Cha * tmp->level;
			    success += calc_skill_exp(op,tmp2, skill);
			} else {
			    /* Can't continue to disarm after failure */
			    return success;
			}
		    }
	    }
	    if((tmp->type==RUNE || tmp->type==TRAP) && tmp->stats.Cha<=1) {
		trap_show(tmp,tmp);
		if (trap_disarm(op,tmp,1,skill) && (!tmp->owner || tmp->owner->type!=PLAYER)) {
		    tmp->stats.exp = tmp->stats.Cha * tmp->level;
		    success += calc_skill_exp(op,tmp,skill);
		} else {
		    /* Can't continue to disarm after failure */
		    return success;
		}
	    }
	}
    }
    return success;
}


/**
 * Praying skill handling.
 *
 * When this skill is called from do_skill(), it allows
 * the player to regain lost grace points at a faster rate. -b.t.
 *
 * This always returns 0 - return value is used by calling function
 * such that if it returns true, player gets exp in that skill.  This
 * the effect here can be done on demand, we probably don't want to
 * give infinite exp by returning true in any cases.
 *
 * @param pl
 * object praying, should be a player.
 * @param skill
 * praying skill.
 * @return
 * 0.
 */
int pray (object *pl, object *skill) {
    char buf[MAX_BUF];
    object *tmp;

    if(pl->type!=PLAYER) return 0;

    snprintf(buf, sizeof(buf), "You pray.");

    /* Check all objects - we could stop at floor objects,
     * but if someone buries an altar, I don't see a problem with
     * going through all the objects, and it shouldn't be much slower
     * than extra checks on object attributes.
     */
    for (tmp=pl->below; tmp!=NULL; tmp=tmp->below) {
	/* Only if the altar actually belongs to someone do you get special benefits */
	if(tmp && tmp->type==HOLY_ALTAR && tmp->other_arch) {
	    snprintf(buf, sizeof(buf), "You pray over the %s.",tmp->name);
	    pray_at_altar(pl,tmp, skill);
	    break;  /* Only pray at one altar */
	}
    }

    draw_ext_info(NDI_BLACK,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
		  buf, buf);

    if(pl->stats.grace < pl->stats.maxgrace) {
	pl->stats.grace++;
	pl->last_grace = -1;
    }
    return 0;
}

/**
 * Meditation skill handling.
 *
 * This skill allows the player to regain a few sp or hp for a
 * brief period of concentration.
 * The amount of time needed to concentrate and the # of points regained is dependant on
 * the level of the user.
 *
 * Depending on the level, the player can wear armour or not.
 * @author b.t. thomas@astro.psu.edu
 *
 * @param pl
 * livng meditating, should be a player.
 * @param skill
 * meditation skill.
 */
void meditate (object *pl, object *skill) {
    object *tmp;

    if(pl->type!=PLAYER) return;	/* players only */

    /* check if pl has removed encumbering armour and weapons */
    if(QUERY_FLAG(pl,FLAG_READY_WEAPON) && (skill->level<6)) {
        draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "You can't concentrate while wielding a weapon!", NULL);
	return;
    } else {
	for(tmp=pl->inv;tmp;tmp=tmp->below)
          if (( (tmp->type==ARMOUR && skill->level<12)
		|| (tmp->type==HELMET && skill->level<10)
		|| (tmp->type==SHIELD && skill->level<6)
		|| (tmp->type==BOOTS && skill->level<4)
		|| (tmp->type==GLOVES && skill->level<2) )
  	     && QUERY_FLAG(tmp,FLAG_APPLIED)) {
         	draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "You can't concentrate while wearing so much armour!", NULL);
	  	return;
	  }
    }

    /* ok let's meditate!  Spell points are regained first, then once
     * they are maxed we get back hp. Actual incrementing of values
     * is handled by the do_some_living() (in player.c). This way magical
     * bonuses for healing/sp regeneration are included properly
     * No matter what, we will eat up some playing time trying to
     * meditate. (see 'factor' variable for what sets the amount of time)
     */

    draw_ext_info(NDI_BLACK,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
		  "You meditate.", NULL);

    if(pl->stats.sp < pl->stats.maxsp) {
	pl->stats.sp++;
	pl->last_sp = -1;
    } else if (pl->stats.hp < pl->stats.maxhp)  {
	pl->stats.hp++;
	pl->last_heal = -1;
    }
}

/**
 * This routine allows players to inscribe messages in
 * ordinary 'books' (anything that is type BOOK). b.t.
 *
 * @param pl
 * player writing.
 * @param item
 * book to write into.
 * @param msg
 * message to write.
 * @param skill
 * writing skill object.
 * @return
 * experience gained for writing.
 * @todo assert() instead of simple check.
 */
static int write_note(object *pl, object *item, const char *msg, object *skill) {
    char buf[BOOK_BUF];
    object *newBook = NULL;

    /* a pair of sanity checks */
    if(!item||item->type!=BOOK) return 0;

    if(!msg) {
	draw_ext_info_format(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "No message to write!\nUsage: use_skill %s <message>",
		      "No message to write!\nUsage: use_skill %s <message>",
		     skill->skill);
	return 0;
    }
    if (strcasestr_local(msg, "endmsg")) {
	draw_ext_info(NDI_UNIQUE,0,pl,MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "Trying to cheat now are we?", NULL);
	return 0;
    }

    /* Lauwenmark: Handle for plugin book writing (trigger) event */
    if (execute_event(item, EVENT_TRIGGER,pl,NULL,msg,SCRIPT_FIX_ALL)!=0)
	    return strlen(msg);

    buf[0] = 0;
    if(!book_overflow(item->msg,msg,BOOK_BUF)) { /* add msg string to book */
        if(item->msg)
            snprintf(buf, sizeof(buf), "%s%s\n", item->msg, msg);
        else
            snprintf(buf, sizeof(buf), "%s\n", msg);

	if(item->nrof > 1) {
	    newBook = get_object();
	    copy_object(item, newBook);
	    decrease_ob(item);
	    newBook->nrof = 1;
	    if (newBook->msg) free_string(newBook->msg);
	    newBook->msg = add_string(buf);
	    newBook = insert_ob_in_ob(newBook, pl);
	} else {
	    if (item->msg) free_string(item->msg);
	    item->msg=add_string(buf);
            /* This shouldn't be necessary - the object hasn't changed in any
             * visible way
             */
            /*	    esrv_send_item(pl, item);*/
	}
    query_short_name(item, buf, BOOK_BUF);
        draw_ext_info_format(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
			     "You write in the %s.",
			     "You write in the %s.",
                 buf );
	return strlen(msg);
    } else
    {
        query_short_name(item, buf, BOOK_BUF);
        draw_ext_info_format(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
                             "Your message won't fit in the %s!",
                             "Your message won't fit in the %s!",
                             buf);
    }
    return 0;
}

/**
 * This routine allows players to inscribe spell scrolls
 * of spells which they know. Backfire effects are possible with the
 * severity of the backlash correlated with the difficulty of the scroll
 * that is attempted. -b.t. thomas@astro.psu.edu
 *
 * @param pl
 * player writing a scroll.
 * @param scroll
 * object to write into.
 * @param skill
 * writing skill.
 * @return
 * experience gained.
 */
static int write_scroll (object *pl, object *scroll, object *skill) {
    int success=0,confused=0, grace_cost = 0;
    object *newscroll, *chosen_spell, *tmp;

    /* this is a sanity check */
    if (scroll->type!=SCROLL) {
        draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
            "A spell can only be inscribed into a scroll!", NULL);
        return 0;
    }

    /* Check if we are ready to attempt inscription */
    chosen_spell=pl->contr->ranges[range_magic];
    if(!chosen_spell) {
        draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
            "You need a spell readied in order to inscribe!", NULL);
        return 0;
    }
    /* grace can become negative, we don't want a sp spell to block writing. */
    grace_cost = SP_level_spellpoint_cost(pl,chosen_spell,SPELL_GRACE);
    if(grace_cost > 0 && grace_cost > pl->stats.grace) {
        draw_ext_info_format(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
            "You don't have enough grace to write a scroll of %s.",
            "You don't have enough grace to write a scroll of %s.",
            chosen_spell->name);
        return 0;
    }
    if(SP_level_spellpoint_cost(pl,chosen_spell,SPELL_MANA) > pl->stats.sp) {
        draw_ext_info_format(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
            "You don't have enough mana to write a scroll of %s.",
            "You don't have enough mana to write a scroll of %s.",
            chosen_spell->name);
        return 0;
    }
    /* Prevent an abuse: write a spell you're denied with, then cast it from the
     * written scroll - gros, 28th July 2006
     */
    if (chosen_spell->path_attuned & pl->path_denied && settings.allow_denied_spells_writing == 0)
    {
        char name[MAX_BUF];
        query_name(chosen_spell, name, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
            "The simple idea of writing a scroll of %s makes you sick !",
            "The simple idea of writing a scroll of %s makes you sick !",
            name);
        return 0;
    }

    /* if there is a spell already on the scroll then player could easily
     * accidently read it while trying to write the new one.  give player
     * a 50% chance to overwrite spell at their own level
     */
    if((scroll->stats.sp || scroll->inv) &&
       random_roll(0, scroll->level*2, pl, PREFER_LOW)>skill->level) {
	    draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		"Oops! You accidently read it while trying to write on it.", NULL);
	    manual_apply(pl,scroll,0);
	    return 0;
    }

    if (execute_event(scroll, EVENT_TRIGGER, pl, chosen_spell, NULL, 0) != 0)
        return 0;

    /* ok, we are ready to try inscription */
    if(QUERY_FLAG(pl,FLAG_CONFUSED)) confused = 1;

    /* Lost mana/grace no matter what */
    pl->stats.grace-=SP_level_spellpoint_cost(pl,chosen_spell,SPELL_GRACE);
    pl->stats.sp-=SP_level_spellpoint_cost(pl,chosen_spell,SPELL_MANA);

    if (random_roll(0, chosen_spell->level*4-1, pl, PREFER_LOW) < skill->level) {
	if (scroll->nrof > 1) {
	    newscroll = get_object();
	    copy_object(scroll, newscroll);
	    decrease_ob(scroll);
	    newscroll->nrof = 1;
	} else {
	    newscroll = scroll;
	}

	if(!confused) {
	    newscroll->level= MAX(skill->level, chosen_spell->level);
	    draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
		"You succeed in writing a new scroll.", NULL);
	} else {
	    chosen_spell = find_random_spell_in_ob(pl, NULL);
	    if (!chosen_spell) return 0;

	    newscroll->level= MAX(skill->level, chosen_spell->level);
	    draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		"In your confused state, you write down some odd spell.", NULL);
	}

	if (newscroll->inv) {
	    object *ninv;

	    ninv = newscroll->inv;
	    remove_ob(ninv);
	    free_object(ninv);
	}
	tmp = get_object();
	copy_object(chosen_spell, tmp);
	insert_ob_in_ob(tmp, newscroll);
    /* This is needed so casting from the scroll correctly works with moving_ball types, which
       check attunements. */
    newscroll->path_attuned = tmp->path_repelled;

	/* Same code as from treasure.c - so they can better merge.
	 * if players want to sell them, so be it.
	 */
	newscroll->value =  newscroll->arch->clone.value * newscroll->inv->value *
	    (newscroll->level +50) / (newscroll->inv->level + 50);
	newscroll->stats.exp = newscroll->value/5;

	/* wait until finished manipulating the scroll before inserting it */
	if (newscroll == scroll)
    {
        /* Remove to correctly merge with other items which may exist in inventory */
        remove_ob(newscroll);
    }
	newscroll=insert_ob_in_ob(newscroll,pl);
	success = calc_skill_exp(pl,newscroll, skill);
	if(!confused) success *= 2;
    success = success * skill->level;
	return success;

    } else { /* Inscription has failed */

	if(chosen_spell->level>skill->level || confused) { /*backfire!*/
	    draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		  "Ouch! Your attempt to write a new scroll strains your mind!", NULL);
	    if(random_roll(0, 1, pl, PREFER_LOW)==1)
		drain_specific_stat(pl,4);
	    else {
	        confuse_player(pl,pl,99);
		return (-30*chosen_spell->level);
	    }
	} else if(random_roll(0, pl->stats.Int-1, pl, PREFER_HIGH) < 15) {
	    draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		"Your attempt to write a new scroll rattles your mind!", NULL);
	    confuse_player(pl,pl,99);
	} else
	    draw_ext_info(NDI_UNIQUE,0,pl,MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			  "You fail to write a new scroll.", NULL);
    }
    return 0;
}

/**
 * Writing skill handling.
 *
 * Wrapper for write_note() and write_scroll().
 *
 * @param pl
 * player writing.
 * @param params
 * message to inscribe.
 * @param skill
 * writing skill.
 * @return
 * experience gained for using the skill.
 */
int write_on_item (object *pl,const char *params, object *skill) {
    object *item;
    const char *string=params;
    int msgtype;
    archetype *skat;

    if(pl->type!=PLAYER) return 0;

    if (!params) {
	params="";
	string=params;
    }
    skat = get_archetype_by_type_subtype(SKILL, SK_LITERACY);

    /* Need to be able to read before we can write! */
    if(!find_skill_by_name(pl,skat->clone.skill)) {
	draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_MISSING,
	   "You must learn to read before you can write!", NULL);
	return 0;
    }

    /* if there is a message then it goes in a book and no message means
     * write active spell into the scroll
     */
    msgtype = (string[0]!='\0') ? BOOK : SCROLL;

    /* find an item of correct type to write on */
    if ( !(item = find_marked_object(pl))){
	draw_ext_info(NDI_UNIQUE,0,pl,MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "You don't have any marked item to write on.", NULL);
	return 0;
    }

    if(QUERY_FLAG(item,FLAG_UNPAID)) {
	draw_ext_info(NDI_UNIQUE,0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
	      "You had better pay for that before you write on it.", NULL);
	return 0;
    }
    if (msgtype != item->type) {
	draw_ext_info_format(NDI_UNIQUE,0,pl,MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
			     "You have no %s to write on",
			     "You have no %s to write on",
                         msgtype==BOOK ? "book" : "scroll");
	return 0;
    }

    if (msgtype == SCROLL) {
	return write_scroll(pl,item, skill);
    } else if (msgtype == BOOK) {
	return write_note(pl,item,string, skill);
    }
    return 0;
}



/**
 * Find an object to throw.
 * If we request an object, then
 * we search for it in the inventory of the owner (you've
 * got to be carrying something in order to throw it!).
 * If we didnt request an object, then the top object in inventory
 * (that is "throwable", ie no throwing your skills away!)
 * is the object of choice. Also check to see if object is
 * 'throwable' (ie not applied cursed obj, worn, etc).
 *
 * @param op
 * object wishing to throw.
 * @param request
 * requested item to throw.
 * @return
 * throwable object, NULL if none suitable found.
 */
static object *find_throw_ob( object *op, const char *request ) {
    object *tmp;
    char name[MAX_BUF];

    if(!op) { /* safety */
	LOG(llevError,"find_throw_ob(): confused! have a NULL thrower!\n");
	return (object *) NULL;
    }

    /* prefer marked item */
    tmp = find_marked_object(op);
    if (tmp != NULL) {
        /* can't toss invisible or inv-locked items */
        if (tmp->invisible || QUERY_FLAG(tmp, FLAG_INV_LOCKED)) {
            tmp = NULL;
        }
    }

    /* look through the inventory */
    if (tmp == NULL) {
        for (tmp = op->inv; tmp != NULL; tmp = tmp->below) {
            /* can't toss invisible or inv-locked items */
            if (tmp->invisible || QUERY_FLAG(tmp, FLAG_INV_LOCKED))
                continue;
            query_name(tmp, name, MAX_BUF);
            if (!request
            || !strcmp(name, request)
            || !strcmp(tmp->name, request))
                break;
        }
    }

    /* this should prevent us from throwing away
     * cursed items, worn armour, etc. Only weapons
     * can be thrown from 'hand'.
     */
    if (!tmp) return NULL;

    if (QUERY_FLAG(tmp,FLAG_APPLIED)) {
	if(tmp->type!=WEAPON) {
        query_name(tmp, name, MAX_BUF);
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
				 "You can't throw %s.",
				 "You can't throw %s.",
				 name);
	    tmp = NULL;
	} else if (QUERY_FLAG(tmp,FLAG_CURSED)||QUERY_FLAG(tmp,FLAG_DAMNED)) {
        query_name(tmp, name, MAX_BUF);
	    draw_ext_info_format(NDI_UNIQUE, 0,op,
				 MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
				 "The %s sticks to your hand!",
				 "The %s sticks to your hand!",
				 name);
	    tmp = NULL;
	} else {
	    if (apply_special (op, tmp, AP_UNAPPLY | AP_NO_MERGE)) {
		LOG (llevError, "BUG: find_throw_ob(): couldn't unapply\n");
		tmp = NULL;
	    }
	}
    } else if (QUERY_FLAG(tmp, FLAG_UNPAID)) {
        query_name(tmp, name, MAX_BUF);
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
			     "You should pay for the %s first.",
			     "You should pay for the %s first.",
			     name);
	tmp = NULL;
    }

    if (tmp && QUERY_FLAG (tmp, FLAG_INV_LOCKED)) {
	LOG (llevError, "BUG: find_throw_ob(): object is locked\n");
	tmp=NULL;
    }
    return tmp;
}

/**
 * We construct the 'carrier' object in
 * which we will insert the object that is being thrown.
 * This combination  becomes the 'thrown object'. -b.t.
 *
 * @param orig
 * object to wrap.
 * @return
 * object to throw.
 */

static object *make_throw_ob (object *orig) {
    object *toss_item;

    if(!orig) return NULL;

    toss_item=get_object();
    if (QUERY_FLAG (orig, FLAG_APPLIED)) {
	LOG (llevError, "BUG: make_throw_ob(): ob is applied\n");
	/* insufficient workaround, but better than nothing */
	CLEAR_FLAG (orig, FLAG_APPLIED);
    }
    copy_object(orig,toss_item);
    toss_item->type = THROWN_OBJ;
    CLEAR_FLAG(toss_item,FLAG_CHANGING);
    toss_item->stats.dam = 0; /* default damage */
    insert_ob_in_ob(orig,toss_item);
    return toss_item;
}


/**
 * Op throws any object toss_item. This code
 * was borrowed from fire_bow.
 *
 * @param op
 * living thing throwing something.
 * @param part
 * part of op throwing.
 * @param toss_item
 * item thrown.
 * @param dir
 * direction to throw.
 * @param skill
 * throwing skill.
 * @retval 0
 * skill use failed.
 * @retval 1
 * skill was successfully used.
 * @todo this messy function should probably be simplified.
 */
static int do_throw(object *op, object *part, object *toss_item, int dir, object *skill) {
    object *throw_ob=toss_item, *left=NULL;
    tag_t left_tag;
    int eff_str = 0,maxc,str=op->stats.Str,dam=0;
    int pause_f,weight_f=0, mflags;
    float str_factor=1.0,load_factor=1.0,item_factor=1.0;
    mapstruct *m;
    sint16  sx, sy;
    tag_t tag;
    char name[MAX_BUF];

    if(throw_ob==NULL) {
	if(op->type==PLAYER) {
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
			  "You have nothing to throw.", NULL);
	}
	return 0;
    }
    if (QUERY_FLAG(throw_ob, FLAG_STARTEQUIP)) {
	if (op->type==PLAYER) {
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
			  "The gods won't let you throw that.", NULL);
	}
	return 0;
    }

    /* Because throwing effectiveness must be reduced by the
     * encumbrance of the thrower and weight of the object. THus,
     * we use the concept of 'effective strength' as defined below.
     */

    /* if str exceeds MAX_STAT (30, eg giants), lets assign a str_factor > 1 */
    if(str>MAX_STAT) {
	str_factor = (float) str /(float) MAX_STAT; str = MAX_STAT;
    }

    /* the more we carry, the less we can throw. Limit only on players */
    maxc=max_carry[str]*1000;
    if(op->carrying>maxc&&op->type==PLAYER)
	load_factor = (float)maxc/(float) op->carrying;

    /* lighter items are thrown harder, farther, faster */
    if(throw_ob->weight>0)
	item_factor = (float) maxc/(float) (3.0 * throw_ob->weight);
    else { /* 0 or negative weight?!? Odd object, can't throw it */
        query_name(throw_ob, name, MAX_BUF);
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
			     "You can't throw %s.",
			     "You can't throw %s.",
			     name);
	return 0;
    }

    eff_str = str * (load_factor<1.0?load_factor:1.0);
    eff_str = (float) eff_str * item_factor * str_factor;

    /* alas, arrays limit us to a value of MAX_STAT (30). Use str_factor to
     * account for super-strong throwers. */
    if(eff_str>MAX_STAT) eff_str=MAX_STAT;

#ifdef DEBUG_THROW
    LOG(llevDebug,"%s carries %d, eff_str=%d\n",op->name,op->carrying,eff_str);
    LOG(llevDebug," max_c=%d, item_f=%f, load_f=%f, str=%d\n",maxc,
        item_factor,load_factor,op->stats.Str);
    LOG(llevDebug," str_factor=%f\n",str_factor);
    LOG(llevDebug," item %s weight= %d\n",throw_ob->name,throw_ob->weight);
#endif

    /* 3 things here prevent a throw, you aimed at your feet, you
     * have no effective throwing strength, or you threw at something
     * that flying objects can't get through.
     */
    mflags = get_map_flags(part->map,&m, part->x+freearr_x[dir],part->y+freearr_y[dir],&sx,&sy);

    if(!dir || (eff_str <= 1) || (mflags & P_OUT_OF_MAP) ||
       (GET_MAP_MOVE_BLOCK(m, sx, sy) & MOVE_FLY_LOW)) {

	/* bounces off 'wall', and drops to feet */
	remove_ob(throw_ob);
	throw_ob->x = part->x; throw_ob->y = part->y;
	insert_ob_in_map(throw_ob,part->map,op,0);
	if(op->type==PLAYER) {
	    if(eff_str<=1) {
            query_name(throw_ob, name, MAX_BUF);
		draw_ext_info_format(NDI_UNIQUE, 0,op,
			     MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			     "Your load is so heavy you drop %s to the ground.",
			     "Your load is so heavy you drop %s to the ground.",
			     name);
	    }
	    else if(!dir) {
            query_name(throw_ob, name, MAX_BUF);
		draw_ext_info_format(NDI_UNIQUE, 0,op,
				     MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
				     "You throw %s at the ground.",
				     "You throw %s at the ground.",
				     name);
	    }
	    else
		draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			      "Something is in the way.", NULL);
	}
	return 0;
    } /* if object can't be thrown */

    left = throw_ob; /* these are throwing objects left to the player */
    left_tag = left->count;

    /* sometimes get_split_ob can't split an object (because op->nrof==0?)
     * and returns NULL. We must use 'left' then
     */

    if((throw_ob = get_split_ob(throw_ob, 1, NULL, 0))==NULL) {
	throw_ob = left;
	remove_ob(left);
    }

    /* special case: throwing powdery substances like dust, dirt */
    if(throw_ob->type == POTION && throw_ob->subtype == POT_DUST) {
	cast_dust(op,throw_ob,dir);
	return 1;
    }

    /* Make a thrown object -- insert real object in a 'carrier' object.
     * If unsuccessfull at making the "thrown_obj", we just reinsert
     * the original object back into inventory and exit
     */
    if((toss_item = make_throw_ob(throw_ob))) {
	throw_ob = toss_item;
	if (throw_ob->skill) free_string(throw_ob->skill);
	throw_ob->skill = add_string(skill->skill);
    }
    else {
	insert_ob_in_ob(throw_ob,op);
	return 0;
    }

    set_owner(throw_ob,op);
    /* At some point in the attack code, the actual real object (op->inv)
     * becomes the hitter.  As such, we need to make sure that has a proper
     * owner value so exp goes to the right place.
     */
    set_owner(throw_ob->inv,op);
    throw_ob->direction=dir;
    throw_ob->x = part->x;
    throw_ob->y = part->y;

    /* the damage bonus from the force of the throw */
    dam = str_factor * dam_bonus[eff_str];

    /* Now, lets adjust the properties of the thrown_ob. */

    /* how far to fly */
    throw_ob->last_sp = (eff_str*3)/5;

    /* speed */
    throw_ob->speed = (speed_bonus[eff_str]+1.0)/1.5;
    throw_ob->speed = MIN(1.0,throw_ob->speed); /* no faster than an arrow! */

    /* item damage. Eff_str and item weight influence damage done */
    weight_f = (throw_ob->weight/2000)>MAX_STAT?MAX_STAT:(throw_ob->weight/2000);
    throw_ob->stats.dam += (dam/3) + dam_bonus[weight_f]
        + (throw_ob->weight/15000) - 2;

    /* chance of breaking. Proportional to force used and weight of item */
    throw_ob->stats.food = (dam/2) + (throw_ob->weight/60000);

    /* replace 25 with a call to clone.arch wc? messes up w/ NPC */
    throw_ob->stats.wc = 25 - dex_bonus[op->stats.Dex]
	- thaco_bonus[eff_str] - skill->level;


    /* the properties of objects which are meant to be thrown (ie dart,
     * throwing knife, etc) will differ from ordinary items. Lets tailor
     * this stuff in here.
     */

    if(QUERY_FLAG(throw_ob->inv,FLAG_IS_THROWN)) {
	throw_ob->last_sp += eff_str/3; /* fly a little further */
	throw_ob->stats.dam += throw_ob->inv->stats.dam + throw_ob->magic + 2;
	throw_ob->stats.wc -= throw_ob->magic + throw_ob->inv->stats.wc;
	/* only throw objects get directional faces */
	if(GET_ANIM_ID(throw_ob) && NUM_ANIMATIONS(throw_ob))
	    SET_ANIMATION(throw_ob, dir);
    } else {
	/* some materials will adjust properties.. */
	if(throw_ob->material&M_LEATHER) {
	    throw_ob->stats.dam -= 1;
	    throw_ob->stats.food -= 10;
	}
	if(throw_ob->material&M_GLASS) throw_ob->stats.food += 60;

	if(throw_ob->material&M_ORGANIC) {
	    throw_ob->stats.dam -= 3;
	    throw_ob->stats.food += 55;
	}
	if(throw_ob->material&M_PAPER||throw_ob->material&M_CLOTH) {
	    throw_ob->stats.dam -= 5; throw_ob->speed *= 0.8;
	    throw_ob->stats.wc += 3; throw_ob->stats.food -= 30;
	}
	/* light obj have more wind resistance, fly slower*/
	if(throw_ob->weight>500) throw_ob->speed *= 0.8;
	if(throw_ob->weight>50) throw_ob->speed *= 0.5;

    } /* else tailor thrown object */

    /* some limits, and safeties (needed?) */
    if(throw_ob->stats.dam<0) throw_ob->stats.dam=0;
    if(throw_ob->last_sp>eff_str) throw_ob->last_sp=eff_str;
    if(throw_ob->stats.food<0) throw_ob->stats.food=0;
    if(throw_ob->stats.food>100) throw_ob->stats.food=100;
    if(throw_ob->stats.wc>30) throw_ob->stats.wc=30;

    /* how long to pause the thrower. Higher values mean less pause */
    pause_f = ((2*eff_str)/3)+20+skill->level;

    /* Put a lower limit on this */
    if (pause_f < 10) pause_f=10;
    if (pause_f > 100) pause_f=100;

    /* Changed in 0.94.2 - the calculation before was really goofy.
     * In short summary, a throw can take anywhere between speed 5 and
     * speed 0.5
     */
    op->speed_left -=  50 / pause_f;

    update_ob_speed(throw_ob);
    throw_ob->speed_left = 0;
    throw_ob->map = part->map;

    throw_ob->move_type = MOVE_FLY_LOW;
    throw_ob->move_on = MOVE_FLY_LOW | MOVE_WALK;

    /* Lauwenmark - Now we can call the associated script_throw event (if any) */
    execute_event(throw_ob, EVENT_THROW,op,NULL,NULL,SCRIPT_FIX_ACTIVATOR);
#ifdef DEBUG_THROW
    LOG(llevDebug," pause_f=%d \n",pause_f);
    LOG(llevDebug," %s stats: wc=%d dam=%d dist=%d spd=%f break=%d\n",
        throw_ob->name,throw_ob->stats.wc,throw_ob->stats.dam,
        throw_ob->last_sp,throw_ob->speed,throw_ob->stats.food);
    LOG(llevDebug,"inserting tossitem (%d) into map\n",throw_ob->count);
#endif
    tag = throw_ob->count;
    insert_ob_in_map(throw_ob,part->map,op,0);
    if (!was_destroyed (throw_ob, tag))
        ob_process(throw_ob);
    return 1;
}

/**
 * Throwing skill handling.
 * @param op
 * object throwing.
 * @param part
 * actual part of op throwing.
 * @param dir
 * direction to throw into.
 * @param params
 * optional message, used to find object to throw.
 * @param skill
 * throwing skill.
 * @retval 0
 * skill use failed.
 * @retval 1
 * skill was successfully used.
 */
int skill_throw (object *op, object *part, int dir, const char *params, object *skill) {
    object *throw_ob;

    if(op->type==PLAYER)
	throw_ob =  find_throw_ob(op,params);
    else
	throw_ob = find_mon_throw_ob(op);

    return do_throw(op,part, throw_ob,dir, skill);
}
