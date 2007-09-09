/*
 * static char *rcsid_c_object_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2007 Mark Wedel & Crossfire Development Team
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

    The author can be reached via e-mail to crossfire-devel@real-time.com
*/

/**
 * @file
 * Object commands, including picking/dropping, locking, etc.
 * @todo clean multiple variations of same stuff (pickup and such), or rename for less confusion.
 */

#include <global.h>
#include <loader.h>
#include <skills.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <living.h>
#include <math.h>

static void set_pickup_mode(const object *op, int i);

/*
 * Object id parsing functions
 */

/** Simple ::objectlink allocation, fail-safe. */
#define OBLINKMALLOC(p) if(!((p)=(objectlink *)malloc(sizeof(objectlink))))\
                          fatal(OUT_OF_MEMORY);

/**
 * Search from start and through below for what matches best with params.
 * we use item_matched_string above - this gives us consistent behaviour
 * between many commands.  Return the best match, or NULL if no match.
 *
 * @param start
 * first object to start searching at.
 * @param pl
 * what object we're searching for.
 * @param params
 * what to search for.
 * @param aflag
 * Combination of AP_APPLY and/or AP_UNAPPLY. Used with apply -u , and apply -a to
 * only unapply applied, or apply unapplied objects.
 * @return
 * matching object, or NULL if no suitable.
 * @todo move the ap_xxx tests before the item_matched_string for performance reasons?
 **/
static object *find_best_apply_object_match(object *start, object* pl, const char *params, int aflag)
{
    object *tmp, *best=NULL;
    int match_val=0,tmpmatch;

    for (tmp=start; tmp; tmp=tmp->below) {
        if (tmp->invisible) continue;
        if ((tmpmatch=item_matched_string(pl, tmp, params))>match_val) {
            if ((aflag==AP_APPLY) && (QUERY_FLAG(tmp,FLAG_APPLIED))) continue;
            if ((aflag==AP_UNAPPLY) && (!QUERY_FLAG(tmp,FLAG_APPLIED))) continue;
            match_val=tmpmatch;
            best=tmp;
        }
    }
    return best;
}

/**
 * Shortcut to find_best_apply_object_match(pl->inv, pl, params, AF_NULL);
 *
 * @param pl
 * who to search an item for.
 * @param params
 * what to search for.
 * @return
 * matching object, or NULL if no suitable.
 **/
static object *find_best_object_match(object *pl, const char *params)
{
    return find_best_apply_object_match(pl->inv, pl, params, AP_NULL);
}

/**
 * 'use_skill' command.
 *
 * @param pl
 * player.
 * @param params
 * skill to use, and optional parameters.
 * @return
 * whether skill was used or not.
 */
int command_uskill ( object *pl, char *params) {
   if (!params) {
        draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Usage: use_skill <skill name>", NULL);
        return 0;
   }
   return use_skill(pl,params);
}

/**
 * 'ready_skill' command.
 *
 * @param pl
 * player.
 * @param params
 * skill name.
 * @return
 * whether skill was readied or not.
 */
int command_rskill ( object *pl, char *params) {
    object *skill;

    if (!params) {
	draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Usage: ready_skill <skill name>", NULL);
	return 0;
    }
    skill = find_skill_by_name(pl, params);

    if (!skill) {
	draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_MISSING,
			     "You have no knowledge of the skill %s",
			     "You have no knowledge of the skill %s",
			     params);
	return 0;
    }
    return change_skill(pl,skill, 0);
}


/* These functions (command_search, command_disarm) are really juse wrappers for
 * things like 'use_skill ...').  In fact, they should really be obsoleted
 * and replaced with those.
 */
/**
 * 'search' command.
 *
 * @param op
 * player.
 * @param params
 * unused.
 * @return
 * whether skill was used or not.
 */
int command_search (object *op, char *params) {
    return use_skill(op, skill_names[SK_FIND_TRAPS]);
}

/**
 * 'disarm' command.
 *
 * @param op
 * player.
 * @param params
 * unused.
 * @return
 * whether skill was used or not.
 */
int command_disarm (object *op, char *params) {
    return use_skill(op, skill_names[SK_DISARM_TRAPS]);
}


/**
 * 'throw' command.
 *
 * A little special because we do want to pass the full params along
 * as it includes the object to throw.
 *
 * @param op
 * player.
 * @param params
 * what to throw.
 * @return
 * whether skill was used or not.
 */
int command_throw (object *op, char *params)
{
    object *skop;

    skop = find_skill_by_name(op, skill_names[SK_THROWING]);
    if (skop) return do_skill(op, op, skop, op->facing,params);
    else {
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_MISSING,
		      "You have no knowledge of the skill throwing.", NULL);
    }
    return 0;
}


/**
 * 'apply' command.
 *
 * @param op
 * player.
 * @param params
 * what to apply.
 * @return
 * whether skill was used or not.
 */
int command_apply (object *op, char *params)
{
    if (!params) {
	player_apply_below(op);
	return 0;
    }
    else {
	int aflag = 0;
	object *inv = op->inv;

	while (*params==' ') params++;
	if (!strncmp(params,"-a ",3)) {
	    aflag=AP_APPLY;
	    params+=3;
	}
	if (!strncmp(params,"-u ",3)) {
	    aflag=AP_UNAPPLY;
	    params+=3;
	}
	if (!strncmp(params,"-b ",3)) {
	    params+=3;
        if (op->container)
            inv = op->container->inv;
        else {
            inv = op;
            while (inv->above)
                inv = inv->above;
        }
	}
	while (*params==' ') params++;

	inv=find_best_apply_object_match(inv, op, params, aflag);
	if (inv) {
	    player_apply(op,inv,aflag,0);
	} else
	  draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			       "Could not find any match to the %s.",
			       "Could not find any match to the %s.",
			       params);
    }
    return 0;
}

/**
 * Check if an item op can be put into a sack. If pl exists then tell
 * a player the reason of failure.
 *
 * @param pl
 * player.
 * @param sack
 * container to try to put into.
 * @param op
 * what to put in the sack.
 * @param nrof
 * number of objects (op) we want to put in. We specify it separately instead of
 * using op->nrof because often times, a player may have specified a
 * certain number of objects to drop, so we can pass that number, and
 * not need to use split_ob() and stuff.
 * @return
 * 1 if it will fit, 0 if it will not.
 */
int sack_can_hold(const object *pl, const object *sack, const object *op, uint32 nrof) {

    char name[MAX_BUF];
    query_name(sack, name, MAX_BUF);

    if (! QUERY_FLAG (sack, FLAG_APPLIED)) {
	draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "The %s is not active.",
			     "The %s is not active.",
			     name);
	return 0;
    }
    if (sack == op) {
	draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "You can't put the %s into itself.",
			     "You can't put the %s into itself.",
			     name);
	return 0;
    }
    if (sack->race && (sack->race != op->race || op->type == CONTAINER
		       || (sack->stats.food && sack->stats.food != op->type))) {
	draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "You can put only %s into the %s.",
			     "You can put only %s into the %s.",
			     sack->race,  name);
	return 0;
    }
    if (op->type == SPECIAL_KEY && sack->slaying && op->slaying) {
	draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "You can't want put the key into %s.",
			     "You can't want put the key into %s.",
			     name);
	return 0;
    }
    if (sack->weight_limit && sack->carrying + (nrof ? nrof : 1) *
	(op->weight + (op->type==CONTAINER?(op->carrying*op->stats.Str):0))
	* (100 - sack->stats.Str) / 100  > sack->weight_limit) {

	draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "That won't fit in the %s!",
			     "That won't fit in the %s!",
			     name);
    return 0;
    }
    /* All other checks pass, must be OK */
    return 1;
}

/**
 * Try to pick up some item.
 *
 * @param pl
 * object (player or monster) picking up.
 * @param op
 * object to put tmp into.
 * @param tmp
 * object to pick up.
 * @param nrof
 * number of tmp to pick up (0 means all of them).
 */
static void pick_up_object (object *pl, object *op, object *tmp, int nrof)
{
    /* buf needs to be big (more than 256 chars) because you can get
     * very long item names.
     */
    char buf[HUGE_BUF], name[MAX_BUF];
    object *env=tmp->env;
    uint32 weight, effective_weight_limit;
    int tmp_nrof = tmp->nrof ? tmp->nrof : 1;

    /* IF the player is flying & trying to take the item out of a container
     * that is in his inventory, let him.  tmp->env points to the container
     * (sack, luggage, etc), tmp->env->env then points to the player (nested
     * containers not allowed as of now)
     */
    if((pl->move_type & MOVE_FLYING) && !QUERY_FLAG(pl, FLAG_WIZ) &&
	get_player_container(tmp)!=pl) {
	draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "You are levitating, you can't reach the ground!", NULL);
	return;
    }
    if (QUERY_FLAG (tmp, FLAG_NO_DROP))
	return;
    if(QUERY_FLAG(tmp,FLAG_WAS_WIZ) && !QUERY_FLAG(pl, FLAG_WAS_WIZ)) {
	draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
		      "The object disappears in a puff of smoke! It must have been an illusion.",
		      NULL);
	if ( ! QUERY_FLAG (tmp, FLAG_REMOVED))
            remove_ob (tmp);
	free_object(tmp);
	return;
    }

    if (nrof > tmp_nrof || nrof == 0)
	nrof = tmp_nrof;
    /* Figure out how much weight this object will add to the player */
    weight = tmp->weight * nrof;
    if (tmp->inv) weight += tmp->carrying * (100 - tmp->stats.Str) / 100;
    if (pl->stats.Str <= MAX_STAT)
        effective_weight_limit = weight_limit[pl->stats.Str];
    else
        effective_weight_limit = weight_limit[MAX_STAT];
    if ((pl->weight + pl->carrying + weight) > effective_weight_limit) {
	draw_ext_info(0, 0,pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
		      "That item is too heavy for you to pick up.", NULL);
	return;
    }
    if (settings.real_wiz == FALSE && QUERY_FLAG(pl, FLAG_WAS_WIZ))
	SET_FLAG(tmp, FLAG_WAS_WIZ);
    if (nrof != tmp_nrof) {
        char failure[MAX_BUF];

	tmp = get_split_ob (tmp, nrof, failure, sizeof(failure));
	if(!tmp) {
	    draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			  failure, NULL);
	    return;
	}
    } else {
	/* If the object is in a container, send a delete to the client.
	 * - we are moving all the items from the container to elsewhere,
	 * so it needs to be deleted.
	 */
        if ( ! QUERY_FLAG (tmp, FLAG_REMOVED)) {
	    remove_ob(tmp); /* Unlink it */
	}
    }
    query_name(tmp, name, MAX_BUF);
    if(QUERY_FLAG(tmp, FLAG_UNPAID))
	snprintf(buf, sizeof(buf), "%s will cost you %s.", name,
		query_cost_string(tmp,pl,F_BUY | F_SHOP));
    else
	snprintf(buf, sizeof(buf), "You pick up the %s.", name);

    /* Now item is about to be picked. */
    if (execute_event(tmp, EVENT_PICKUP, pl, op, NULL, SCRIPT_FIX_ALL) != 0)
        return;

    draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		  buf, NULL);

    tmp = insert_ob_in_ob(tmp, op);


    /* All the stuff below deals with client/server code, and is only
     * usable by players
     */
    if(pl->type!=PLAYER) return;

    /* These are needed to update the weight for the container we
     * are putting the object in.
     */
    if (op!=pl) {
	esrv_update_item (UPD_WEIGHT, pl, op);
	esrv_update_item (UPD_WEIGHT, pl, pl);
    }

    /* Update the container the object was in */
    if (env && env!=pl && env!=op) esrv_update_item (UPD_WEIGHT, pl, env);
}

/**
 * Try to pick up an item.
 *
 * @param op
 * object trying to pick up.
 * @param alt
 * optional object op is trying to pick. If NULL, try to pick first item under op.
 * @todo remove goto that doesn't have any effect.
 */
void pick_up(object *op,object *alt)
/* modified slightly to allow monsters use this -b.t. 5-31-95 */
{
    int need_fix_tmp = 0;
    object *tmp=NULL;
    mapstruct *tmp_map=NULL;
    int count;
    tag_t tag;

    /* Decide which object to pick. */
    if (alt)
    {
        if ( ! can_pick (op, alt)) {
            draw_ext_info_format (NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				  "You can't pick up the %s.",
				  "You can't pick up the %s.",
                                  alt->name);
	    goto leave;
        }
        tmp = alt;
    }
    else
    {
        if (op->below == NULL || ! can_pick (op, op->below)) {
             draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                            "There is nothing to pick up here.", NULL);
             goto leave;
        }
        tmp = op->below;
    }

    /* Try to catch it. */
    tmp_map = tmp->map;
    tmp = stop_item (tmp);
    if (tmp == NULL)
        goto leave;
    need_fix_tmp = 1;
    if ( ! can_pick (op, tmp))
        goto leave;

    if (op->type==PLAYER) {
	count=op->contr->count;
	if (count==0) count = tmp->nrof;
    }
    else
	count=tmp->nrof;

    /* container is open, so use it */
    if (op->container) {
	alt = op->container;
	if (alt != tmp->env && !sack_can_hold (op, alt, tmp,count))
	    goto leave;
    } else { /* non container pickup */
	for (alt=op->inv; alt; alt=alt->below)
	    if (alt->type==CONTAINER && QUERY_FLAG(alt, FLAG_APPLIED) &&
		alt->race && alt->race==tmp->race &&
		sack_can_hold (NULL, alt, tmp,count))
		break;  /* perfect match */

	if (!alt)
	    for (alt=op->inv; alt; alt=alt->below)
		if (alt->type==CONTAINER && QUERY_FLAG(alt, FLAG_APPLIED) &&
		    sack_can_hold (NULL, alt, tmp,count))
		    break;  /* General container comes next */
	if (!alt)
	    alt = op; /* No free containers */
    }
    if(tmp->env == alt) {
	/* here it could be possible to check rent,
	 * if someone wants to implement it
	 */
	alt = op;
    }
#ifdef PICKUP_DEBUG
    LOG(llevDebug, "Pick_up(): %s picks %s (%d) and inserts it %s.\n", op->name, tmp->name,  op->contr->count, alt->name);
#endif

    /* startequip items are not allowed to be put into containers: */
    if (op->type == PLAYER && alt->type == CONTAINER
	&& QUERY_FLAG (tmp, FLAG_STARTEQUIP))
    {
        draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                       "This object cannot be put into containers!", NULL);
        goto leave;
    }

    tag = tmp->count;
    pick_up_object (op, alt, tmp, count);
    if (was_destroyed (tmp, tag) || tmp->env)
        need_fix_tmp = 0;
    if (op->type == PLAYER)
       op->contr->count=0;
    goto leave;

  leave:
    if (need_fix_tmp)
        fix_stopped_item (tmp, tmp_map, op);
}


/**
 * This takes (picks up) and item.
 *
 * @param op
 * player who issued the command.
 * @param params
 * string to match against the item name.
 * @return
 * 0.
 */
int command_take (object *op, char *params)
{
    object *tmp, *next;
    int ival;
    int missed = 0;

    if (op->container)
        tmp=op->container->inv;
    else {
        tmp=op->above;
        if (tmp) while (tmp->above) {
            tmp=tmp->above;
        }
        if (!tmp)
            tmp=op->below;
    }

    if (tmp==NULL) {
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Nothing to take!", NULL);
        return 0;
    }

    /* Makes processing easier */
    if (params && *params=='\0') params=NULL;

    while (tmp) {
        next=tmp->below;

        if (tmp->invisible) {
            tmp=next;
            continue;
        }
        /* This following two if and else if could be merged into line
         * but that probably will make it more difficult to read, and
         * not make it any more efficient
         */
        if (params && (ival=item_matched_string(op, tmp, params))>0) {
            if ((ival<=2)&&(!can_pick(op,tmp)))
            {
                if(!QUERY_FLAG(tmp, FLAG_IS_FLOOR))/* don't count floor tiles */
                    missed++;
            }
            else
                pick_up(op, tmp);
        }
        else if (can_pick(op, tmp) && !params) {
            pick_up(op,tmp);
            break;
        }
        tmp=next;
        /* Might as well just skip over the player immediately -
         * we know it can't be picked up
         */
        if (tmp == op) tmp=tmp->below;
    }
    if (!params && !tmp) {
        for (tmp=op->below; tmp!=NULL; tmp=tmp->next)
            if (!tmp->invisible) {
                draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				     "You can't pick up a %s.",
				     "You can't pick up a %s.",
				     tmp->name? tmp->name:"null");

                break;
            }
        if (!tmp) draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				"There is nothing to pick up.", NULL);
    }
    if (missed==1)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "You were unable to take one of the items.", NULL);
    else if (missed>1)
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
            "You were unable to take %d of the items.",
            "You were unable to take %d of the items.",
	     missed);
    return 0;
}


/**
 * Something tries to put an object into another.
 *
 * This function was part of drop(), now is own function.
 *
 * @note
 * the 'sack' in question can now be a transport,
 * so this function isn't named very good anymore.
 *
 * @param op
 * who is moving the item.
 * @param sack
 * where to put the object.
 * @param tmp
 * what to put into sack.
 * @param nrof
 * if non zero, then nrof objects is tried to put into sack, else everything is put.
 */
void put_object_in_sack (object *op, object *sack, object *tmp, uint32 nrof)
{
    tag_t tmp_tag, tmp2_tag;
    object *tmp2, *sack2;
    char name_sack[MAX_BUF], name_tmp[MAX_BUF];

    if (sack==tmp) return;	/* Can't put an object in itself */
    query_name(sack, name_sack, MAX_BUF);
    if (sack->type != CONTAINER && sack->type != TRANSPORT) {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		     "The %s is not a container.",
		     "The %s is not a container.",
		     name_sack);
	return;
    }
    if (QUERY_FLAG(tmp,FLAG_STARTEQUIP)) {
        query_name(tmp, name_tmp, MAX_BUF);
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
	     "You cannot put the %s in the %s.",
	     "You cannot put the %s in the %s.",
	     name_tmp, name_sack);
	return;
    }
    if (tmp->type == CONTAINER && tmp->inv) {

	/* Eneq(@csd.uu.se): If the object to be dropped is a container
	 * we instead move the contents of that container into the active
	 * container, this is only done if the object has something in it.
	 */
	sack2 = tmp;
    query_name(tmp, name_tmp, MAX_BUF);
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		     "You move the items from %s into %s.",
		     "You move the items from %s into %s.",
		     name_tmp, name_sack);

	for (tmp2 = tmp->inv; tmp2; tmp2 = tmp) {
	    tmp = tmp2->below;
	    if ((sack->type == CONTAINER && sack_can_hold(op, op->container, tmp2,tmp2->nrof)) ||
		(sack->type == TRANSPORT && transport_can_hold(sack, tmp2, tmp2->nrof))) {
		    put_object_in_sack (op, sack, tmp2, 0);
	    } else {
		draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
				      "Your %s fills up.",
				      "Your %s fills up.",
				      name_sack);
		break;
	    }
	}
	esrv_update_item (UPD_WEIGHT, op, sack2);
	return;
    }

    /* Don't worry about this for containers - our caller should have
     * already checked this.
     */
    if ((sack->type == CONTAINER) && !sack_can_hold (op, sack, tmp,(nrof?nrof:tmp->nrof)))
	return;

    if(QUERY_FLAG(tmp, FLAG_APPLIED)) {
	if (apply_special (op, tmp, AP_UNAPPLY | AP_NO_MERGE))
	    return;
    }

    /* we want to put some portion of the item into the container */
    if (nrof && tmp->nrof != nrof) {
        char failure[MAX_BUF];
	object *tmp2 = tmp;
        tmp2_tag = tmp2->count;
	tmp = get_split_ob (tmp, nrof, failure, sizeof(failure));

	if(!tmp) {
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			  failure, NULL);
	    return;
	}
    } else
	remove_ob(tmp);

    query_name(tmp, name_tmp, MAX_BUF);
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			 "You put the %s in %s.",
			 "You put the %s in %s.",
			 name_tmp, name_sack);
    tmp_tag = tmp->count;
    tmp2 = insert_ob_in_ob(tmp, sack);
    fix_object(op); /* This is overkill, fix_player() is called somewhere */
		  /* in object.c */

    /* If a transport, need to update all the players in the transport
     * the view of what is in it.
     */
    if (sack->type == TRANSPORT) {
	for (tmp=sack->inv; tmp; tmp=tmp->below) {
	    if (tmp->type == PLAYER) tmp->contr->socket.update_look=1;
	}
    } else {
	/* update the sacks weight */
	esrv_update_item (UPD_WEIGHT, op, sack);
    }
}

/**
 * Try to drop an object on the floor.
 *
 * This function was part of drop, now is own function.
 *
 * @param op
 * who is dropping the item.
 * @param tmp
 * item to drop.
 * @param nrof
 * if is non zero, then nrof objects is tried to be dropped.
 * @return
 * object dropped, NULL if it was destroyed.
 * @todo shouldn't tmp be NULL if was_destroyed returns true?
 */
object *drop_object (object *op, object *tmp, uint32 nrof)
{
    tag_t tmp_tag;

    if (QUERY_FLAG(tmp, FLAG_NO_DROP)) {
      return NULL;
    }

    if(QUERY_FLAG(tmp, FLAG_APPLIED)) {
      if (apply_special (op, tmp, AP_UNAPPLY | AP_NO_MERGE))
          return NULL;		/* can't unapply it */
    }

    /* We are only dropping some of the items.  We split the current objec
     * off
     */
    if(nrof && tmp->nrof != nrof) {
        char failure[MAX_BUF];

	tmp = get_split_ob (tmp, nrof, failure, sizeof(failure));
	if(!tmp) {
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			  failure, NULL);
	    return NULL;
	}
    } else
      remove_ob (tmp);

    /* Lauwenmark: Handle for plugin drop event */
    if (execute_event(tmp, EVENT_DROP,op,NULL,NULL,SCRIPT_FIX_ALL)!= 0)
        return NULL;

    if (QUERY_FLAG (tmp, FLAG_STARTEQUIP)) {
        char name[MAX_BUF];
        query_name(tmp, name, MAX_BUF);
      draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			   "You drop the %s. The gods who lent it to you retrieves it.",
			   "You drop the %s. The gods who lent it to you retrieves it.",
			   name);
      free_object(tmp);
      fix_object(op);
      return NULL;
    }

/*  If SAVE_INTERVAL is commented out, we never want to save
 *  the player here.
 */
#ifdef SAVE_INTERVAL
    /* I'm not sure why there is a value check - since the save
     * is done every SAVE_INTERVAL seconds, why care the value
     * of what he is dropping?
     */
    if (op->type == PLAYER && !QUERY_FLAG(tmp, FLAG_UNPAID) &&
        (tmp->nrof ? tmp->value * tmp->nrof : tmp->value > 2000) &&
        (op->contr->last_save_time + SAVE_INTERVAL) <= time(NULL)) {
        save_player(op, 1);
        op->contr->last_save_time = time(NULL);
    }
#endif /* SAVE_INTERVAL */


    tmp->x = op->x;
    tmp->y = op->y;

    tmp_tag = tmp->count;
    insert_ob_in_map(tmp, op->map, op,0);
    if (!was_destroyed(tmp, tmp_tag) && !QUERY_FLAG(tmp, FLAG_UNPAID) && tmp->type != MONEY && is_in_shop(op)) {
        sell_item(tmp, op);
    }

/*
    SET_FLAG (op, FLAG_NO_APPLY);
    remove_ob(op);
    insert_ob_in_map(op, op->map, op, INS_NO_MERGE | INS_NO_WALK_ON);
    CLEAR_FLAG (op, FLAG_NO_APPLY);
*/
    /* Call this before we update the various windows/players.  At least
     * that we, we know the weight is correct.
     */
    fix_object(op); /* This is overkill, fix_player() is called somewhere */
		    /* in object.c */

    if (op->type == PLAYER)
    {
        /* insert_ob_in_map handles the update_look for the player. */
        /* Need to update the weight for the player */
        esrv_update_item(UPD_WEIGHT, op, op);
    }
    return tmp;
}

/**
 * Drop an item, either on the floor or in a container.
 *
 * @param op
 * who is dropping an item.
 * @param tmp
 * what object to drop.
 */
void drop(object *op, object *tmp)
{
    /* Hopeful fix for disappearing objects when dropping from a container -
     * somehow, players get an invisible object in the container, and the
     * old logic would skip over invisible objects - works fine for the
     * playes inventory, but drop inventory wants to use the next value.
     */
    if (tmp->invisible) {
	/* if the following is the case, it must be in an container. */
	if (tmp->env && tmp->env->type != PLAYER) {
	    /* Just toss the object - probably shouldn't be hanging
	     * around anyways
	     */
	    remove_ob(tmp);
	    free_object(tmp);
	    return;
	} else {
	    while(tmp!=NULL && tmp->invisible)
		tmp=tmp->below;
	}
    }

    if (tmp==NULL) {
      draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "You don't have anything to drop.", NULL);
      return;
    }
    if (QUERY_FLAG(tmp, FLAG_INV_LOCKED)) {
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "This item is locked", NULL);
      return;
    }
    if (QUERY_FLAG(tmp, FLAG_NO_DROP)) {
        return;
    }

    if (op->type == PLAYER)
    {
    if (op->contr->last_used==tmp && op->contr->last_used_id == tmp->count) {
      object *n=NULL;
      if(tmp->below != NULL)
	  n = tmp->below;
      else if(tmp->above != NULL)
	  n = tmp->above;
      op->contr->last_used = n;
      if (n != NULL)
	  op->contr->last_used_id = n->count;
      else
	  op->contr->last_used_id = 0;
    }
    };

    if (op->container) {
        if (op->type == PLAYER)
        {
                put_object_in_sack (op, op->container, tmp, op->contr->count);
        } else {
                put_object_in_sack(op, op->container, tmp, 0);
        };
    } else {
        if (op->type == PLAYER)
        {
                drop_object (op, tmp, op->contr->count);
        } else {
                drop_object(op,tmp,0);
        };
    }
    if (op->type == PLAYER)
        op->contr->count = 0;
}

/**
 * Command to drop all items that have not been locked.
 *
 * @param op
 * player.
 * @param params
 * optional specifier, like 'armour', 'weapon' and such.
 * @return
 * 0.
 */
int command_dropall (object *op, char *params) {

    object * curinv, *nextinv;

    if(op->inv == NULL)
    {
        draw_ext_info(NDI_UNIQUE, 0,op,
            MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "Nothing to drop!", NULL);
        return 0;
    }

    curinv = op->inv;

    /*
     * This is the default.  Drops everything not locked or considered
     * not something that should be dropped.
     * Care must be taken that the next item pointer is not to money as
     * the drop() routine will do unknown things to it when dropping
     * in a shop. --Tero.Pelander@utu.fi
     */
    if(params==NULL) {
        while(curinv != NULL) {
            nextinv = curinv->below;
            while (nextinv && nextinv->type==MONEY)
                nextinv = nextinv->below;
            if(! QUERY_FLAG(curinv,FLAG_INV_LOCKED) && curinv->type != MONEY
                && curinv->type != FOOD && curinv->type != KEY
                && curinv->type != SPECIAL_KEY && curinv->type != GEM
                && !curinv->invisible
                && (curinv->type!=CONTAINER || op->container!=curinv))
            {
                drop(op,curinv);
            }
            curinv = nextinv;
        }
    }

    else if(strcmp(params, "weapons") == 0) {
        while(curinv != NULL) {
            nextinv = curinv->below;
            while (nextinv && nextinv->type==MONEY)
                nextinv = nextinv->below;
            if(! QUERY_FLAG(curinv,FLAG_INV_LOCKED)
                && ((curinv->type == WEAPON)
                || (curinv->type == BOW) || (curinv->type == ARROW)))
            {
                drop(op,curinv);
            }
            curinv = nextinv;
        }
    }

    else if(strcmp(params, "armor") == 0 || strcmp(params, "armour") == 0)
    {
        while(curinv != NULL)
        {
            nextinv = curinv->below;
            while (nextinv && nextinv->type==MONEY)
                nextinv = nextinv->below;
            if(! QUERY_FLAG(curinv,FLAG_INV_LOCKED)
                && ((curinv->type == ARMOUR)
                || curinv->type == SHIELD || curinv->type==HELMET))
            {
                drop(op,curinv);
            }
            curinv = nextinv;
        }
    }

    else if(strcmp(params, "misc") == 0)
    {
        while(curinv != NULL) {
            nextinv = curinv->below;
            while (nextinv && nextinv->type==MONEY)
                nextinv = nextinv->below;
            if(! QUERY_FLAG(curinv,FLAG_INV_LOCKED)
                && ! QUERY_FLAG(curinv,FLAG_APPLIED))
            {
                switch(curinv->type) {
                    case HORN:
                    case BOOK:
                    case SPELLBOOK:
                    case GIRDLE:
                    case AMULET:
                    case RING:
                    case CLOAK:
                    case BOOTS:
                    case GLOVES:
                    case BRACERS:
                    case SCROLL:
                    case ARMOUR_IMPROVER:
                    case WEAPON_IMPROVER:
                    case WAND:
                    case ROD:
                    case POTION:
                        drop(op,curinv);
                        curinv = nextinv;
                        break;
                    default:
                        curinv = nextinv;
                        break;
                }
            }
            curinv = nextinv;
        }
    }
    op->contr->socket.update_look=1;
/*  draw_look(op);*/
    return 0;
}

/**
 * 'drop' command.
 *
 * @param op
 * player.
 * @param params
 * what to drop.
 * @return
 * 0.
 */
int command_drop (object *op, char *params)
{
    object  *tmp, *next;
    int did_one=0;
    int ival=0;
    int missed = 0;

    if (!params) {
        draw_ext_info(NDI_UNIQUE,0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Drop what?", NULL);
        return 0;
    } else {
        for (tmp=op->inv; tmp; tmp=next) {
            next=tmp->below;
            if (QUERY_FLAG(tmp,FLAG_NO_DROP) || tmp->invisible) continue;
            if ((ival = item_matched_string(op,tmp,params))>0) {
                if ((QUERY_FLAG(tmp, FLAG_INV_LOCKED))&&((ival==1)||(ival==2)))
                    missed++;
                else
                    drop(op, tmp);
                did_one=1;
            }
        }
        if (!did_one) draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				    "Nothing to drop.", NULL);
        if (missed==1)
            draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                "One item couldn't be dropped because it was locked.", NULL);
        else if (missed>1)
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                "%d items couldn't be dropped because they were locked.",
                "%d items couldn't be dropped because they were locked.",
		 missed);
    }
    if (op->type==PLAYER)
    {
        op->contr->count=0;
        op->contr->socket.update_look=1;
    }
    return 0;
}

/**
 * Put all contents of the container on the ground below the player or in opened container, except locked items.
 *
 * @param container
 * what to empty.
 * @param pl
 * player to drop for.
 */
static void empty_container(object* container, object* pl) {
    object* inv;
    object* next;
    int left = 0;
    char name[MAX_BUF];

    if (!container->inv)
        return;

    for (inv = container->inv; inv; inv = next) {
        next = inv->below;
        if (QUERY_FLAG(inv, FLAG_INV_LOCKED)) {
            /* you can have locked items in container. */
            left++;
            continue;
        }
        drop(pl, inv);
        if (inv->below == next)
            /* item couldn't be dropped for some reason. */
            left++;
    }
    esrv_update_item(UPD_WEIGHT, pl, container);

    query_name(container, name, sizeof(name));
    if (left)
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS, "You empty the %s except %d items.", NULL, name, left);
    else
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS, "You empty the %s.", NULL, name);
}

/**
 * 'empty' command.
 *
 * @param op
 * player.
 * @param params
 * item specifier.
 * @return
 * 0.
 */
int command_empty(object *op, char *params) {
    object* inv;
    object* container;

    if (!params) {
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "Empty what?", NULL);
        return 0;
    }

    if (strcmp(params, "all") == 0) {
        for (inv = op->inv; inv; inv = inv->below)
            if (inv->type == CONTAINER)
                empty_container(inv, op);
        return 0;
    }

    container = find_best_object_match(op, params);
    if (!container) {
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "No such item.", NULL);
        return 0;
    }
    if (container->type != CONTAINER) {
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "This is not a container!", NULL);
        return 0;
    }
    empty_container(container, op);

    return 0;
}

/**
 * 'examine' command.
 *
 * @param op
 * player.
 * @param params
 * optional item specifier.
 * @return
 * 0.
 */
int command_examine (object *op, char *params)
{
    if (!params) {
	object *tmp=op->below;
	while (tmp && !LOOK_OBJ(tmp)) tmp=tmp->below;
	if (tmp) examine(op,tmp);
    }
    else {
	object *tmp=find_best_object_match(op,params);
	if (tmp)
	    examine(op,tmp);
	else
	    draw_ext_info_format(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				 "Could not find an object that matches %s",
				 "Could not find an object that matches %s",
				 params);
    }
    return 0;
}

/**
 * Return the object the player has marked with the 'mark' command
 * below.  If no match is found (or object has changed), we return
 * NULL.  We leave it up to the calling function to print messages if
 * nothing is found.
 *
 * @param op
 * object. Should be a player.
 * @return
 * marked object if still valid, NULL else.
 */
object *find_marked_object(object *op)
{
    object *tmp;

    if (!op || !op->contr || !op->contr->mark) return NULL;

    /* This may seem like overkill, but we need to make sure that they
     * player hasn't dropped the item.  We use count on the off chance that
     * an item got reincarnated at some point.
     */
    for (tmp=op->inv; tmp; tmp=tmp->below) {
	if (tmp->invisible) continue;
	if (tmp == op->contr->mark) {
	    if (tmp->count == op->contr->mark_count)
		return tmp;
	    else {
		op->contr->mark=NULL;
		op->contr->mark_count=0;
		return NULL;
	    }
	}
    }
    return NULL;
}


/**
 * 'mark' command, to mark an item for some effects (enchant armor, ...).
 *
 * @param op
 * player.
 * @param params
 * If empty, we print out the currently marked object.
 * Otherwise, try to find a matching object - try best match first.
 * @return
 * 1 or 0.
 */
int command_mark(object *op, char *params)
{
    char name[MAX_BUF];

    if (!op->contr) return 1;
    if (!params) {
	object *mark=find_marked_object(op);
	if (!mark) draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				 "You have no marked object.", NULL);
	else {
        query_name(mark, name, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE,0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
				  "%s is marked.",
				  "%s is marked.",
				  name);
    }
    }
    else {
	object *mark1=find_best_object_match(op, params);
	if (!mark1) {
	    draw_ext_info_format(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				 "Could not find an object that matches %s",
				 "Could not find an object that matches %s",
				 params);
	    return 1;
	}
	else {
	    op->contr->mark=mark1;
	    op->contr->mark_count=mark1->count;
        query_name(mark1, name, MAX_BUF);
	    draw_ext_info_format(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
				 "Marked item %s",
				 "Marked item %s",
				 name);
	    return 0;
	}
    }
    return 0;	/*shouldnt get here */
}


/**
 * Player examine a monster.
 *
 * @param op
 * player.
 * @param tmp
 * monster being examined.
 */
void examine_monster(object *op,object *tmp) {
    object *mon=tmp->head?tmp->head:tmp;

    if(QUERY_FLAG(mon,FLAG_UNDEAD))
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      "It is an undead force.", NULL);
    if(mon->level>op->level)
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      "It is likely more powerful than you.", NULL);
    else if(mon->level<op->level)
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      "It is likely less powerful than you.", NULL);
    else
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      "It is probably as powerful as you.", NULL);

    if(mon->attacktype&AT_ACID)
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      "You smell an acrid odor.", NULL);

    /* Anyone know why this used to use the clone value instead of the
     * maxhp field?  This seems that it should give more accurate results.
     */
    switch((mon->stats.hp+1)*4/(mon->stats.maxhp+1)) { /* From 1-4 */
	case 1:
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			  "It is in a bad shape.", NULL);
	    break;
	case 2:
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			  "It is hurt.", NULL);
	    break;
	case 3:
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			  "It is somewhat hurt.", NULL);
	    break;
	case 4:
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			  "It is in excellent shape.", NULL);
	    break;
    }
    if(present_in_ob(POISONING,mon)!=NULL)
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      "It looks very ill.", NULL);
}


/**
 * Player examines some object.
 *
 * @param op
 * player.
 * @param tmp
 * object to examine.
 */
void examine(object *op, object *tmp) {
    char buf[VERY_BIG_BUF];
    int in_shop;
    int i;
    buf[0]='\0';

    if (tmp == NULL || tmp->type == CLOSE_CON)
	return;

    /* Put the description in buf. */
    ob_describe(tmp, op, buf, sizeof(buf));

    /* Send the player the description, prepending "That is" if singular
     * and "Those are" if plural.
     */
    if (tmp->nrof <= 1)
        draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			 "That is %s",
			 "That is %s",
			 buf);
    else
        draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			 "Those are %s",
			 "Those are %s",
			 buf);
    buf[0] = '\0';

    if(tmp->custom_name) {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		     "You name it %s",
		     "You name it %s",
		     tmp->custom_name);
    }

    switch(tmp->type) {
	case SPELLBOOK:
	    if(QUERY_FLAG(tmp, FLAG_IDENTIFIED) && tmp->inv ) {
            char level[100];
            get_levelnumber(tmp->inv->level, level, 100);
            snprintf(buf, sizeof(buf), "%s is a %s level %s spell",
                tmp->inv->name, level,
                tmp->inv->skill);
	    }
	    break;

	case BOOK:
	    if(tmp->msg!=NULL)
		snprintf(buf, sizeof(buf), "Something is written in it.");
	    break;

	case CONTAINER:
	    if(tmp->race!=NULL) {
		if(tmp->weight_limit && tmp->stats.Str<100)
		    snprintf(buf, sizeof(buf), "It can hold only %s and its weight limit is %.1f kg.",
			 tmp->race, tmp->weight_limit/(10.0 * (100 - tmp->stats.Str)));
		else
		    snprintf(buf, sizeof(buf), "It can hold only %s.", tmp->race);
	    } else
		if(tmp->weight_limit && tmp->stats.Str<100)
		    snprintf(buf, sizeof(buf), "Its weight limit is %.1f kg.",
			     tmp->weight_limit/(10.0 * (100 - tmp->stats.Str)));
	    break;

	case WAND:
	    if(QUERY_FLAG(tmp, FLAG_IDENTIFIED))
		snprintf(buf, sizeof(buf), "It has %d charges left.", tmp->stats.food);
	    break;
    }

    if(buf[0]!='\0')
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      buf, NULL);

    if(tmp->materialname != NULL && !tmp->msg) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			     "It is made of: %s.",
			     "It is made of: %s.",
			     tmp->materialname);
    }
    /* Where to wear this item */
    for (i=0; i < NUM_BODY_LOCATIONS; i++) {
	if (tmp->body_info[i]<-1) {
	    if (op->body_info[i])
		draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			     "It goes %s (%d)",
			     "It goes %s (%d)",
			     body_locations[i].use_name, -tmp->body_info[i]);
	    else
		draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			     "It goes %s",
			     "It goes %s",
			     body_locations[i].nonuse_name);
	} else if (tmp->body_info[i]) {
	    if (op->body_info[i])
		draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			     "It goes %s",
			     "It goes %s",
			     body_locations[i].use_name);
	    else
		draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			     "It goes %s",
			     "It goes %s",
			     body_locations[i].nonuse_name);
	}
    }

    if(tmp->weight) {
	snprintf(buf, sizeof(buf), tmp->nrof > 1 ? "They weigh %3.3f kg." : "It weighs %3.3f kg.",
            tmp->weight*(tmp->nrof?tmp->nrof:1)/1000.0);
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      buf, NULL);
    }

    in_shop = is_in_shop(op);

    if (tmp->value && !QUERY_FLAG(tmp, FLAG_STARTEQUIP) && !QUERY_FLAG(tmp, FLAG_NO_PICK)) {
    	snprintf(buf, sizeof(buf), "You reckon %s worth %s.",
		    tmp->nrof>1?"they are":"it is",query_cost_string(tmp,op,F_SELL | F_APPROX));
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      buf, NULL);
	if (in_shop) {
	    if(QUERY_FLAG(tmp, FLAG_UNPAID))
	    	snprintf(buf, sizeof(buf), "%s would cost you %s.",
		    tmp->nrof>1?"They":"It",query_cost_string(tmp,op,F_BUY | F_SHOP));
	    else
	    	snprintf(buf, sizeof(buf), "You are offered %s for %s.",
		    query_cost_string(tmp,op,F_SELL+F_SHOP), tmp->nrof>1?"them":"it");
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			  buf, NULL);
	}
    }

    if(QUERY_FLAG(tmp, FLAG_MONSTER))
	examine_monster(op,tmp);

    /* Is this item buildable? */
    if ( QUERY_FLAG( tmp, FLAG_IS_BUILDABLE ) )
        draw_ext_info( NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      "This is a buildable item.", NULL);

    /* Does the object have a message?  Don't show message for all object
     * types - especially if the first entry is a match
     */
    if(tmp->msg && tmp->type != EXIT && tmp->type != BOOK &&
       tmp->type != CORPSE && !tmp->move_on &&
       strncasecmp(tmp->msg, "@match",6)) {

	/* This is just a hack so when identifying hte items, we print
	 * out the extra message
	 */
	if (need_identify(tmp) && QUERY_FLAG(tmp, FLAG_IDENTIFIED))
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
			  "The object has a story:", NULL);

	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		      tmp->msg, NULL);
    }
    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
		  " ", " "); /* Blank line */
}

/**
 * Prints object's inventory.
 *
 * @param op
 * who to print for.
 * @param inv
 * if NULL then print op's inventory, else print the inventory of inv.
 */
void inventory(object *op,object *inv) {
  object *tmp;
  const char *in;
  int items = 0, length;
  char weight[MAX_BUF], name[MAX_BUF];

  if (inv==NULL && op==NULL) {
    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		  "Inventory of what object?", NULL);
    return;
  }
  tmp = inv ? inv->inv : op->inv;

  while (tmp) {
    if ((!tmp->invisible &&
        (inv==NULL || inv->type == CONTAINER || QUERY_FLAG(tmp, FLAG_APPLIED)))
         || (!op || QUERY_FLAG(op, FLAG_WIZ)))
      items++;
    tmp=tmp->below;
  }
  if (inv==NULL) { /* player's inventory */
    if (items==0) {
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "You carry nothing.", NULL);
      return;
    } else {
      length = 28;
      in = "";
      draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INVENTORY,
		    "Inventory:", NULL);
    }
  } else {
    if (items==0)
      return;
    else {
      length = 28;
      in = "  ";
    }
  }
  for (tmp=inv?inv->inv:op->inv; tmp; tmp=tmp->below) {
    if((!op||!QUERY_FLAG(op, FLAG_WIZ)) && (tmp->invisible ||
       (inv && inv->type != CONTAINER && !QUERY_FLAG(tmp, FLAG_APPLIED))))
      continue;
    query_weight(tmp, weight, MAX_BUF);
    query_name(tmp, name, MAX_BUF);
    if((!op || QUERY_FLAG(op, FLAG_WIZ)))
      draw_ext_info_format(NDI_UNIQUE, 0,op , MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INVENTORY,
		   "[fixed]%s- %-*.*s (%5d) %-8s",
		   "%s- %-*.*s (%5d) %-8s",
		   in, length, length, name, tmp->count,weight);
    else
      draw_ext_info_format(NDI_UNIQUE,0, op,  MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INVENTORY,
		   "[fixed]%s- %-*.*s %-8s",
		   "%s- %-*.*s %-8s",
		   in, length+8, length+8, name, weight);
  }
  if(!inv && op) {
      query_weight(op, weight, MAX_BUF);
    draw_ext_info_format(NDI_UNIQUE,0, op , MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INVENTORY,
		 "[fixed]%-*s %-8s",
		 "%-*s %-8s",
            41,"Total weight :",weight);
  }
}

/**
 * Utility function to display the pickup mode for a player.
 *
 * @param op
 * must be a player.
 */
static void display_new_pickup(const object* op)
    {
    int i = op->contr->mode;

    if(!(i & PU_NEWMODE)) return;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d NEWMODE",
			 "%d NEWMODE",
			 i & PU_NEWMODE?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d DEBUG",
			 "%d DEBUG",
			 i & PU_DEBUG?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d INHIBIT",
			 "%d INHIBIT",
			 i & PU_INHIBIT?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d STOP",
			 "%d STOP",
			 i & PU_STOP?1:0);

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d <= x pickup weight/value RATIO (0==off)",
			 "%d <= x pickup weight/value RATIO (0==off)",
			 (i & PU_RATIO)*5);

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d FOOD",
			 "%d FOOD",
			 i & PU_FOOD?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d DRINK",
			 "%d DRINK",
			 i & PU_DRINK?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d VALUABLES",
			 "%d VALUABLES",
			 i & PU_VALUABLES?1:0);

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d BOW",
			 "%d BOW",
			 i & PU_BOW?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d ARROW",
			 "%d ARROW",
			 i & PU_ARROW?1:0);

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d HELMET",
			 "%d HELMET",
			 i & PU_HELMET?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d SHIELD",
			 "%d SHIELD",
			 i & PU_SHIELD?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d ARMOUR",
			 "%d ARMOUR",
			 i & PU_ARMOUR?1:0);

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d BOOTS",
			 "%d BOOTS",
			 i & PU_BOOTS?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d GLOVES",
			 "%d GLOVES",
			 i & PU_GLOVES?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d CLOAK",
			 "%d CLOAK",
			 i & PU_CLOAK?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d KEY",
			 "%d KEY",
			 i & PU_KEY?1:0);

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d MISSILEWEAPON",
			 "%d MISSILEWEAPON",
			 i & PU_MISSILEWEAPON?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d ALLWEAPON",
			 "%d ALLWEAPON",
			 i & PU_ALLWEAPON?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d MAGICAL",
			 "%d MAGICAL",
			 i & PU_MAGICAL?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d POTION",
			 "%d POTION",
			 i & PU_POTION?1:0);

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d SPELLBOOK",
			 "%d SPELLBOOK",
			 i & PU_SPELLBOOK?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d SKILLSCROLL",
			 "%d SKILLSCROLL",
			 i & PU_SKILLSCROLL?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d READABLES",
			 "%d READABLES",
			 i & PU_READABLES?1:0);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d MAGICDEVICE",
			 "%d MAGICDEVICE",
			 i & PU_MAGIC_DEVICE?1:0);

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d NOT CURSED",
			 "%d NOT CURSED",
			 i & PU_NOT_CURSED?1:0);

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			 "%d JEWELS",
			 "%d JEWELS",
			 i & PU_JEWELS?1:0);

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
             "%d FLESH",
             "%d FLESH",
             i & PU_FLESH?1:0);

    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
		  "", "");
    }

/**
 * 'pickup' command.
 *
 * @param op
 * player.
 * @param params
 * pickup mode. Can be empty to display the current mode.
 * @return
 * 1 if success, 0 else.
 * @todo trash old pickup mode, merge with new pickup.
 */
int command_pickup (object *op, char *params)
{
  uint32 i;
  static const char* names[ ] = {
      "debug", "inhibit", "stop", "food", "drink", "valuables", "bow", "arrow", "helmet",
      "shield", "armour", "boots", "gloves", "cloak", "key", "missile", "allweapon",
      "magical", "potion", "spellbook", "skillscroll", "readables", "magicdevice",
      "notcursed", "jewels", "flesh", NULL };
      static uint32 modes[ ] = {
      PU_DEBUG, PU_INHIBIT, PU_STOP, PU_FOOD, PU_DRINK, PU_VALUABLES, PU_BOW, PU_ARROW, PU_HELMET,
      PU_SHIELD, PU_ARMOUR, PU_BOOTS, PU_GLOVES, PU_CLOAK, PU_KEY, PU_MISSILEWEAPON, PU_ALLWEAPON,
      PU_MAGICAL, PU_POTION, PU_SPELLBOOK, PU_SKILLSCROLL, PU_READABLES, PU_MAGIC_DEVICE,
      PU_NOT_CURSED, PU_JEWELS, PU_FLESH, 0 };

  if(!params) {
    /* if the new mode is used, just print the settings */
    if(op->contr->mode & PU_NEWMODE)
    {
        display_new_pickup( op );
        return 1;
    }
    if(1) LOG(llevDebug, "command_pickup: !params\n");
    set_pickup_mode(op, (op->contr->mode > 6)? 0: op->contr->mode+1);
    return 0;
  }

  while ( *params == ' ' )
      params++;

  if ( *params == '+' || *params == '-' )
      {
      int mode;
      for ( mode = 0; names[ mode ]; mode++ )
          {
          if ( !strcmp( names[ mode ], params + 1 ) )
              {
              i = op->contr->mode;
              if ( !( i & PU_NEWMODE ) )
                  i = PU_NEWMODE;
              if ( *params == '+' )
                  i = i | modes[ mode ];
              else
                  i = i & ~modes[ mode ];
              op->contr->mode = i;
              display_new_pickup( op );
              return 1;
              }
          }
      draw_ext_info_format( NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			   "Pickup: invalid item %s\n"
,			   "Pickup: invalid item %s\n",
			   params );
      return 1;
      }

  if(sscanf(params, "%u", &i) != 1) {
    if(1) LOG(llevDebug, "command_pickup: params==NULL\n");
    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		  "Usage: pickup <0-7> or <value_density> .", NULL);
    return 1;
  }
  set_pickup_mode(op,i);
  display_new_pickup( op );

  return 1;
}

/**
 * Sets the 'old' pickup mode.
 *
 * @param op
 * player.
 * @param i
 * new pickup mode.
 */
static void set_pickup_mode(const object *op, int i) {
  switch(op->contr->mode=i) {
    case 0:
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		    "Mode: Don't pick up.", NULL);
      break;
    case 1:
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		    "Mode: Pick up one item.", NULL);
      break;
    case 2:
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		    "Mode: Pick up one item and stop.", NULL);
      break;
    case 3:
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		    "Mode: Stop before picking up.", NULL);
      break;
    case 4:
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		    "Mode: Pick up all items.", NULL);
      break;
    case 5:
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		    "Mode: Pick up all items and stop.", NULL);
      break;
    case 6:
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		    "Mode: Pick up all magic items.", NULL);
      break;
    case 7:
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		    "Mode: Pick up all coins and gems", NULL);
      break;
    }
}

/**
 * 'search-items' command.
 *
 * @param op
 * player.
 * @param params
 * options.
 * @return
 * 1.
 */
int command_search_items (object *op, char *params)
{

    if (settings.search_items == FALSE)
	return 1;

    if(params == NULL) {
	if(op->contr->search_str[0]=='\0') {
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			"Example: search magic+1 "
			"Would automatically pick up all "
			"items containing the word 'magic+1'.",
			NULL);
	    return 1;
	}
	op->contr->search_str[0]='\0';
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		      "Search mode turned off.", NULL);
    fix_object(op);
	return 1;
    }
    if((int)strlen(params) >= MAX_BUF) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Search string too long.", NULL);
	return 1;
    }
    strcpy(op->contr->search_str, params);
    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			 "Searching for '%s'.",
			 "Searching for '%s'.",
			 op->contr->search_str);
    fix_object(op);
    return 1;
}

/**
 * Changing the custom name of an item
 *
 * Syntax is: rename \<what object\> to \<new name\>
 * - if 'what object' is omitted, marked object is used
 * - if 'to new name' is omitted, custom name is cleared
 *
 * Names are considered for all purpose having a length <=127 (max length sent to client
 * by server).
 *
 * @param op
 * player.
 * @param params
 * how to rename.
 * @return 1
 */
int command_rename_item(object *op, char *params)
{
  char buf[VERY_BIG_BUF], name[MAX_BUF];
  int itemnumber;
  object *item=NULL;
  object *tmp;
  char *closebrace;
  size_t counter;
  tag_t tag;

  if (params) {
    /* Let's skip white spaces */
    while(' '==*params) params++;

    /* Checking the first part */
    if ((itemnumber = atoi(params))!=0) {
	for (item=op->inv; item && ((item->count != itemnumber) || item->invisible); item=item->below);
	if (!item) {
	    draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			  "Tried to rename an invalid item.", NULL);
	    return 1;
	}
	while(isdigit(*params) || ' '==*params) params++;
    }
    else if ('<'==*params) {
      /* Got old name, let's get it & find appropriate matching item */
      closebrace=strchr(params,'>');
      if(!closebrace) {
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Syntax error!", NULL);
        return 1;
      }
      /* Sanity check for buffer overruns */
      if((closebrace-params)>127) {
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Old name too long (up to 127 characters allowed)!", NULL);
        return 1;
      }
      /* Copy the old name */
      snprintf(buf, sizeof(buf), "%.*s", (int)(closebrace-(params+1)), params+1);

      /* Find best matching item */
      item=find_best_object_match(op,buf);
      if(!item) {
        draw_ext_info(NDI_UNIQUE,0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Could not find a matching item to rename.", NULL);
        return 1;
      }

      /* Now need to move pointer to just after > */
      params=closebrace+1;
      while(' '==*params) params++;

    } else {
        /* Use marked item */
        item=find_marked_object(op);
        if(!item) {
          draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			"No marked item to rename.", NULL);
          return 1;
        }
    }

    /* Now let's find the new name */
    if(!strncmp(params,"to ",3)) {
      params+=3;
      while(' '==*params) params++;
      if('<'!=*params) {
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Syntax error, expecting < at start of new name!", NULL);
        return 1;
      }
      closebrace=strchr(params+1,'>');
      if(!closebrace) {
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Syntax error, expecting > at end of new name!", NULL);
        return 1;
      }

      /* Sanity check for buffer overruns */
      if((closebrace-params)>127) {
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "New name too long (up to 127 characters allowed)!", NULL);
        return 1;
      }

      /* Copy the new name */
      snprintf(buf, sizeof(buf), "%.*s", (int)(closebrace-(params+1)), params+1);

      /* Let's check it for weird characters */
      for(counter=0;counter<strlen(buf);counter++) {
        if(isalnum(buf[counter])) continue;
        if(' '==buf[counter]) continue;
        if('\''==buf[counter]) continue;
        if('+'==buf[counter]) continue;
        if('_'==buf[counter]) continue;
        if('-'==buf[counter]) continue;

        /* If we come here, then the name contains an invalid character...
        tell the player & exit */
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Invalid new name!", NULL);
        return 1;
      }

    } else {
      /* If param contains something, then syntax error... */
      if(strlen(params)) {
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Syntax error, expected 'to <' after old name!", NULL);
        return 1;
      }
      /* New name is empty */
      buf[0]='\0';
    }
  } else {
    /* Last case: params==NULL */
    item=find_marked_object(op);
    if(!item) {
      draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "No marked item to rename.", NULL);
      return 1;
    }
    buf[0]='\0';
  }

  /* Coming here, everything is fine... */
  if(!strlen(buf)) {
    /* Clear custom name */
    if(item->custom_name == NULL) {
      draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "This item has no custom name.", NULL);
      return 1;
    }

    FREE_AND_CLEAR_STR(item->custom_name);
    query_base_name(item,item->nrof>1?1:0, name, MAX_BUF);
    draw_ext_info_format(NDI_UNIQUE, 0, op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			 "You stop calling your %s with weird names.",
			 "You stop calling your %s with weird names.",
			 name);
  } else {
    if(item->custom_name != NULL && strcmp(item->custom_name, buf) == 0) {
        query_base_name(item,item->nrof>1?1:0, name, MAX_BUF);
      draw_ext_info_format(NDI_UNIQUE, 0, op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			   "You keep calling your %s %s.",
			   "You keep calling your %s %s.",
			   name,buf);
      return 1;
    }

    /* Set custom name */
    FREE_AND_COPY(item->custom_name,buf);

    query_base_name(item,item->nrof>1?1:0, name, MAX_BUF);
    draw_ext_info_format(NDI_UNIQUE, 0, op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			 "Your %s will now be called %s.",
			 "Your %s will now be called %s.",
			 name,buf);
  }

  tag = item->count;
  tmp = merge_ob(item, NULL);
  if (tmp == NULL) {
    /* object was not merged - if it was, merge_ob handles updating for us. */
    esrv_update_item(UPD_NAME, op, item);
  }

  return 1;
}

/**
 * Alternate way to lock/unlock items (command line).
 *
 * @param op
 * player
 * @param params
 * sent command line.
 */
int command_lock_item(object *op, char *params) {
    object* item;
    object* tmp;
    tag_t tag;
    char name[HUGE_BUF];

    if (!params || strlen(params) == 0) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
            "Lock what item?", "Lock what item?");
        return 1;
    }

    item = find_best_object_match(op, params);
    if (!item) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
            "Can't find any matching item.", "Can't find any matching item.");
        return 1;
    }

    query_short_name(item, name, HUGE_BUF);
    if (QUERY_FLAG(item, FLAG_INV_LOCKED)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
            "Unlocked %s.", "Unlocked %s.", name);
        CLEAR_FLAG(item,FLAG_INV_LOCKED);
    } else {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
            "Locked %s.", "Locked %s.", name);
        SET_FLAG(item,FLAG_INV_LOCKED);
    }

    tag = item->count;
    tmp = merge_ob(item, NULL);
    if (tmp == NULL) {
        /* object was not merged, if it was merge_ob handles updates for us */
        esrv_update_item(UPD_FLAGS, op, item);
    }
    return 1;
}

/**
 * Try to use an item on another. Items are checked for key/values matching.
 * @param op
 * player.
 * @param params
 * sent string, with all parameters.
 * @return
 * 1.
 */
int command_use(object* op, char* params) {
    char* with, copy[MAX_BUF];
    object *first, *second, *add;
    archetype* arch;
    int count;
    sstring data;

    if (!op->type == PLAYER)
        return 1;

    snprintf(copy, sizeof(copy), params);
    with = strstr(copy, " with ");
    if (!with) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "Syntax is use <item> with <item>.", NULL);
        return 1;
    }

    with[0] = '\0';
    with = with + strlen(" with ");

    first = find_best_object_match(op, copy);
    if (!first) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "No match for %s.", NULL, copy);
        return 1;
    }
    second = find_best_object_match(op, with);
    if (!second) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "No match for %s.", NULL, with);
        return 1;
    }

    snprintf(copy, sizeof(copy), "on_use_with_%s", first->arch->name);
    data = get_ob_key_value(second, copy);
    if (!data) {
        snprintf(copy, sizeof(copy), "on_use_with_%d_%d", first->type, first->subtype);
        data = get_ob_key_value(second, copy);
        if (!data) {
            snprintf(copy, sizeof(copy), "on_use_with_%d", first->type);
            data = get_ob_key_value(second, copy);
            if (!data) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "Nothing happens.", NULL);
                return 1;
            }
        }
    }

    while (data != NULL) {
        if (strncmp(data, "add ", 4) == 0) {
            data += 4;
            if (isdigit(*data)) {
                count = atol(data);
                data = strchr(data, ' ') + 1;
            }
            else
                count = 1;
            with = strchr(data, ' ');
            if (!with) {
                strncpy(copy, data, sizeof(copy));
                data = NULL;
            }
            else {
                *with = '\0';
                strncpy(copy, data, sizeof(copy));
                data += strlen(copy) + 1;
            }
            arch = find_archetype(copy);
            if (!arch) {
                LOG(llevError, "Use: invalid archetype %s in %s.\n", copy, second->name);
                return 1;
            }
            add = object_create_arch(arch);
            add->nrof = count;
            insert_ob_in_ob(add, op);
        }
        else if (strncmp(data, "remove $", 8) == 0) {
            data += 8;
            if (*data == '1') {
                if (first)
                    first = decrease_ob(first);
                data += 2;
            }
            else if (*data == '2') {
                if (second)
                    second = decrease_ob(second);
                data += 2;
            }
            else {
                LOG(llevError, "Use: invalid use string %s in %s\n", data, second->name);
                return 1;
            }
        }
        else {
            LOG(llevError, "Use: invalid use string %s in %s\n", data, second->name);
            return 1;
        }
    }

    return 1;
}
