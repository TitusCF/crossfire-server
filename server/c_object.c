/*
 * static char *rcsid_c_object_c =
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

    The author can be reached via e-mail to crossfire-devel@real-time.com

   Object (handling) commands
*/

#include <global.h>
#include <loader.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <living.h>

/*
 * Object id parsing functions
 */

#define OBLINKMALLOC(p) if(!((p)=(objectlink *)malloc(sizeof(objectlink))))\
                          fatal(OUT_OF_MEMORY);

#define ADD_ITEM(NEW,COUNT)\
	  if(!first) {\
	    OBLINKMALLOC(first);\
	    last=first;\
	  } else {\
	    OBLINKMALLOC(last->next);\
	    last=last->next;\
	  }\
	  last->next=NULL;\
	  last->ob=(NEW);\
          last->id=(COUNT);

/* Search the inventory of 'pl' for what matches best with params.
 * we use item_matched_string above - this gives us consistent behaviour
 * between many commands.  Return the best match, or NULL if no match.
 */
object *find_best_object_match(object *pl, char *params)
{
    object *tmp, *best=NULL;
    int match_val=0,tmpmatch;

    for (tmp=pl->inv; tmp; tmp=tmp->below) {
	if (tmp->invisible) continue;
	if ((tmpmatch=item_matched_string(pl, tmp, params))>match_val) {
	    match_val=tmpmatch;
	    best=tmp;
	}
    }
    return best;
}


int command_uskill ( object *pl, char *params) {
   if (!params) {
        new_draw_info(NDI_UNIQUE, 0,pl, "Usage: use_skill <skill name>");
        return 0;
   }
   return use_skill(pl,params);
}

int command_rskill ( object *pl, char *params) {
   int skillno;

   if (!params) {
        new_draw_info(NDI_UNIQUE, 0,pl, "Usage: ready_skill <skill name>");
        return 0;
   }
   skillno=lookup_skill_by_name(params);
   if (skillno==-1) {
	new_draw_info_format(NDI_UNIQUE,0,pl,"Couldn't find the skill %s", params);
	return 0;
   }
   return change_skill(pl,skillno);
}

int command_apply (object *op, char *params)
{
  if (!params) {
    player_apply_below(op);
    return 0;
  }
  else {
    enum apply_flag aflag = 0;
    object *inv;

    while (*params==' ') params++;
    if (!strncmp(params,"-a ",3)) {
	aflag=AP_APPLY;
	params+=3;
    }
    if (!strncmp(params,"-u ",3)) {
	aflag=AP_UNAPPLY;
	params+=3;
    }
    while (*params==' ') params++;

    inv=find_best_object_match(op, params);
    if (inv) {
	player_apply(op,inv,aflag,0);
    } else
	  new_draw_info_format(NDI_UNIQUE, 0, op,
	    "Could not find any match to the %s.",params);
  }
  return 0;
}

/*
 * Check if an item op can be put into a sack. If pl exists then tell
 * a player the reason of failure.
 * returns 1 if it will fit, 0 if it will not.  nrof is the number of
 * objects (op) we want to put in.  We specify it separately instead of
 * using op->nrof because often times, a player may have specified a
 * certain number of objects to drop, so we can pass that number, and
 * not need to use split_ob and stuff.
 */
int sack_can_hold (object *pl, object *sack, object *op, int nrof) {
    char buf[MAX_BUF];
    buf[0] = 0;

    if (! QUERY_FLAG (sack, FLAG_APPLIED))
	sprintf (buf, "The %s is not active.", query_name(sack));
    if (sack == op)
	sprintf (buf, "You can't put the %s into itself.", query_name(sack));
    if (sack->race && (sack->race != op->race || op->type == CONTAINER
		       || (sack->stats.food && sack->stats.food != op->type)))
	sprintf (buf, "You can put only %s into the %s.", sack->race,
		 query_name(sack));
    if (op->type == SPECIAL_KEY && sack->slaying && op->slaying)
	sprintf (buf, "You don't want put the key into %s.", query_name(sack));
    if (sack->weight_limit && sack->carrying + (nrof ? nrof : 1) * 
	(op->weight + (op->type==CONTAINER?(op->carrying*op->stats.Str):0))
	* (100 - sack->stats.Str) / 100  > sack->weight_limit)
	sprintf (buf, "That won't fit in the %s!", query_name(sack));
    if (buf[0]) {
	if (pl)
	    new_draw_info(NDI_UNIQUE, 0,pl, buf);
	return 0;
    }
    return 1;
}

/* Pick up commands follow */
/* pl = player (not always - monsters can use this now)
 * op is the object to put tmp into, 
 * tmp is the object to pick up, nrof is the number to
 * pick up (0 means all of them)
 */
static void pick_up_object (object *pl, object *op, object *tmp, int nrof)
{
    /* buf needs to be big (more than 256 chars) because you can get
     * very long item names.
     */
    char buf[HUGE_BUF];
    object *env=tmp->env;
    uint32 weight, effective_weight_limit;
    int tmp_nrof = tmp->nrof ? tmp->nrof : 1;

    /* IF the player is flying & trying to take the item out of a container 
     * that is in his inventory, let him.  tmp->env points to the container 
     * (sack, luggage, etc), tmp->env->env then points to the player (nested 
     * containers not allowed as of now)
     */
    if(QUERY_FLAG(pl, FLAG_FLYING) && !QUERY_FLAG(pl, FLAG_WIZ) && 
       is_player_inv(tmp)!=pl) {
	new_draw_info(NDI_UNIQUE, 0,pl, "You are levitating, you can't reach the ground!");
	return;
    }
    if (QUERY_FLAG (tmp, FLAG_NO_DROP))
	return;
    if(QUERY_FLAG(tmp,FLAG_WAS_WIZ) && !QUERY_FLAG(pl, FLAG_WAS_WIZ)) {
	new_draw_info(NDI_UNIQUE, 0,pl, "The object disappears in a puff of smoke!");
	new_draw_info(NDI_UNIQUE, 0,pl, "It must have been an illusion.");
	if (pl->type==PLAYER) esrv_del_item (pl->contr, tmp->count);
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
	new_draw_info(NDI_UNIQUE, 0,pl,"That item is too heavy for you to pick up.");
	return;
    }
    if (settings.real_wiz == FALSE && QUERY_FLAG(pl, FLAG_WAS_WIZ))
	SET_FLAG(tmp, FLAG_WAS_WIZ);
    if (nrof != tmp_nrof) {
	object *tmp2 = tmp;
        tag_t tmp2_tag = tmp2->count;
	tmp = get_split_ob (tmp, nrof);
	if(!tmp) {
	    new_draw_info(NDI_UNIQUE, 0,pl, errmsg);
	    return;
	}
	/* Tell a client what happened rest of objects */
	if (pl->type == PLAYER) {
	    if (was_destroyed (tmp2, tmp2_tag))
		esrv_del_item (pl->contr, tmp2_tag);
	    else
		esrv_send_item (pl, tmp2);
	}
    } else {
	/* If the object is in a container, send a delete to the client.
	 * - we are moving all the items from the container to elsewhere,
	 * so it needs to be deleted.
	 */
        if ( ! QUERY_FLAG (tmp, FLAG_REMOVED)) {
	    if (tmp->env && pl->type==PLAYER) 
	        esrv_del_item (pl->contr, tmp->count);
	    remove_ob(tmp); /* Unlink it */
	}
    }
    if(QUERY_FLAG(tmp, FLAG_UNPAID))
	(void) sprintf(buf,"%s will cost you %s.", query_name(tmp),
		query_cost_string(tmp,pl,F_BUY));
    else
	(void) sprintf(buf,"You pick up the %s.", query_name(tmp));
    new_draw_info(NDI_UNIQUE, 0,pl,buf);

    tmp = insert_ob_in_ob(tmp, op);

    /* All the stuff below deals with client/server code, and is only
     * usable by players
     */
    if(pl->type!=PLAYER) return;

    esrv_send_item (pl, tmp);
    /* These are needed to update the weight for the container we
     * are putting the object in, and the players weight, if different.
     */
    esrv_update_item (UPD_WEIGHT, pl, op);
    if (op!=pl) esrv_send_item (pl, pl);

    /* Update the container the object was in */
    if (env && env!=pl && env!=op) esrv_update_item (UPD_WEIGHT, pl, env);
}


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
            new_draw_info_format (NDI_UNIQUE, 0, op, "You can't pick up the %s.",
                                  alt->name);
	    goto leave;
        }
        tmp = alt;
    }
    else
    {
        if (op->below == NULL || ! can_pick (op, op->below)) {
             new_draw_info (NDI_UNIQUE, 0, op,
                            "There is nothing to pick up here.");
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
    printf ("Pick_up(): %s picks %s (%d) and inserts it %s.\n",op->name, tmp->name,  op->contr->count, alt->name);
#endif

    /* startequip items are not allowed to be put into containers: */
    if (op->type == PLAYER && alt->type == CONTAINER
	&& QUERY_FLAG (tmp, FLAG_STARTEQUIP))
    {
        new_draw_info (NDI_UNIQUE, 0, op,
                       "This object cannot be put into containers!");
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


int command_take (object *op, char *params)
{
    object *tmp, *next;

    if (op->container) 
	tmp=op->container;
    else {
	tmp=op->above;
	if (tmp) while (tmp->above) {
	    tmp=tmp->above;
	}
	if (!tmp)
	    tmp=op->below;
    }

    if (tmp==NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"Nothing to take!");
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
	if (params && item_matched_string(op, tmp, params)) {
	    pick_up(op, tmp);
	}
	else if (can_pick(op, tmp) && !params) {
	    pick_up(op,tmp);
	    break;
	}
	tmp=next;
    }
    if (!params && !tmp) {
	for (tmp=op->below; tmp!=NULL; tmp=tmp->next)
	    if (!tmp->invisible) {
		char buf[MAX_BUF];
		sprintf(buf,"You can't pick up a %s.",
		    tmp->name? tmp->name:"null");
		new_draw_info(NDI_UNIQUE, 0,op, buf);
		break;
	    }
	if (!tmp) new_draw_info(NDI_UNIQUE, 0,op, "There is nothing to pick up.");
    }

/* Shouldn't be needed - pick_up should update this */
/*    draw_look(op);*/
    return 0;
}


/*
 *  This function was part of drop, now is own function. 
 *  Player 'op' tries to put object 'tmp' into sack 'sack', 
 *  if nrof is non zero, then nrof objects is tried to put into sack. 
 */
void put_object_in_sack (object *op, object *sack, object *tmp, long nrof) 
{
    tag_t tmp_tag, tmp2_tag;
    object *tmp2, *sack2;
    char buf[MAX_BUF];

    if (sack==tmp) return;	/* Can't put an object in itself */
    if (sack->type != CONTAINER) {
      new_draw_info_format(NDI_UNIQUE, 0,op,
	"The %s is not a container.", query_name(sack));
      return;
    }
    if (QUERY_FLAG(tmp,FLAG_STARTEQUIP)) {
      new_draw_info_format(NDI_UNIQUE, 0,op,
	"You cannot put the %s in the container.", query_name(tmp));
      return;
    }
    if (tmp->type == CONTAINER && tmp->inv) {

      /* Eneq(@csd.uu.se): If the object to be dropped is a container
       * we instead move the contents of that container into the active
       * container, this is only done if the object has something in it.
       */
      sack2 = tmp;
      new_draw_info_format(NDI_UNIQUE, 0,op, "You move the items from %s into %s.",
		    query_name(tmp), query_name(op->container));
      for (tmp2 = tmp->inv; tmp2; tmp2 = tmp) {
	  tmp = tmp2->below;
	if (sack_can_hold(op, op->container, tmp2,tmp2->nrof))
	  put_object_in_sack (op, sack, tmp2, 0);
	else {
	  sprintf(buf,"Your %s fills up.", query_name(op->container));
	  new_draw_info(NDI_UNIQUE, 0,op, buf);
	  break;
	}
      }
      esrv_update_item (UPD_WEIGHT, op, sack2);
      return;
    }

    if (! sack_can_hold (op, sack, tmp,(nrof?nrof:tmp->nrof)))
      return;

    if(QUERY_FLAG(tmp, FLAG_APPLIED)) {
      if (apply_special (op, tmp, AP_UNAPPLY | AP_NO_MERGE))
          return;
    }

    /* we want to put some portion of the item into the container */
    if (nrof && tmp->nrof != nrof) {
	object *tmp2 = tmp;
        tmp2_tag = tmp2->count;
	tmp = get_split_ob (tmp, nrof);

	if(!tmp) {
	    new_draw_info(NDI_UNIQUE, 0,op, errmsg);
	    return;
	}
	/* Tell a client what happened other objects */ 
	if (was_destroyed (tmp2, tmp2_tag))
	      esrv_del_item (op->contr, tmp2_tag);
	else	/* this can proably be replaced with an update */
	      esrv_send_item (op, tmp2);
    } else
	remove_ob(tmp);

    sprintf(buf, "You put the %s in ", query_name(tmp));
    strcat (buf, query_name(sack));
    strcat (buf, ".");
    tmp_tag = tmp->count;
    tmp2 = insert_ob_in_ob(tmp, sack);
    new_draw_info(NDI_UNIQUE, 0,op,buf);
    fix_player(op); /* This is overkill, fix_player() is called somewhere */
		  /* in object.c */

    /* If an object merged (and thus, different object), we need to
     * delete the original.
     */
    if (tmp2 != tmp)
	esrv_del_item (op->contr, tmp_tag);

    esrv_send_item (op, tmp2);
    /* update the sacks and players weight */
    esrv_update_item (UPD_WEIGHT, op, sack);
    esrv_update_item (UPD_WEIGHT, op, op);
}

/*
 *  This function was part of drop, now is own function.
 *  Player 'op' tries to drop object 'tmp', if tmp is non zero, then
 *  nrof objects is tried to dropped.
 * This is used when dropping objects onto the floor.
 */
void drop_object (object *op, object *tmp, long nrof)
{
    char buf[MAX_BUF];
    object *floor;

    if (QUERY_FLAG(tmp, FLAG_NO_DROP)) {
#if 0
      /* Eneq(@csd.uu.se): Objects with NO_DROP defined can't be dropped. */
      new_draw_info(NDI_UNIQUE, 0,op, "This item can't be dropped.");
#endif
      return;
    }

    if(QUERY_FLAG(tmp, FLAG_APPLIED)) {
      if (apply_special (op, tmp, AP_UNAPPLY | AP_NO_MERGE))
          return;		/* can't unapply it */
    }

    /* We are only dropping some of the items.  We split the current objec
     * off
     */
    if(nrof && tmp->nrof != nrof) {
	object *tmp2 = tmp;
        tag_t tmp2_tag = tmp2->count;
	tmp = get_split_ob (tmp, nrof);
	if(!tmp) {
	    new_draw_info(NDI_UNIQUE, 0,op, errmsg);
	    return;
	}
	/* Tell a client what happened rest of objects.  tmp2 is now the
	 * original object
	 */
	 if (op->type == PLAYER)
	 {
                if (was_destroyed (tmp2, tmp2_tag))
                        esrv_del_item (op->contr, tmp2_tag);
                else
                        esrv_send_item (op, tmp2);
	};
    } else
      remove_ob (tmp);
#ifdef PLUGINS
      /* GROS: Handle for plugin drop event */
      if(tmp->event_hook[EVENT_DROP] != NULL)
      {
        CFParm CFP;
        CFParm *CFR;
        int k, l, m, rtn_script;
        m = 0;
        k = EVENT_DROP;
        l = SCRIPT_FIX_ALL;
        CFP.Value[0] = &k;
        CFP.Value[1] = op;
        CFP.Value[2] = tmp;
        CFP.Value[3] = &nrof;
        CFP.Value[4] = NULL;
        CFP.Value[5] = &m;
        CFP.Value[6] = &m;
        CFP.Value[7] = &m;
        CFP.Value[8] = &l;
        CFP.Value[9] = tmp->event_hook[k];
        CFP.Value[10]= tmp->event_options[k];
        if (findPlugin(tmp->event_plugin[k])>=0)
        {
          CFR = ((PlugList[findPlugin(tmp->event_plugin[k])].eventfunc) (&CFP));
          rtn_script = *(int *)(CFR->Value[0]);
          if (rtn_script!=0) return;
        }
      }
#endif
    if (QUERY_FLAG (tmp, FLAG_STARTEQUIP)) {
      sprintf(buf,"You drop the %s.", query_name(tmp));
      new_draw_info(NDI_UNIQUE, 0,op,buf);
      new_draw_info(NDI_UNIQUE, 0,op,"The gods who lent it to you retrieves it.");
      if (op->type==PLAYER)
	esrv_del_item (op->contr, tmp->count);
      free_object(tmp);
      fix_player(op);
      return;
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


    floor = get_map_ob (op->map, op->x, op->y);
    if( floor && floor->type == SHOP_FLOOR &&
       !QUERY_FLAG(tmp, FLAG_UNPAID) && tmp->type != MONEY)
      sell_item(tmp,op);

    tmp->x = op->x;
    tmp->y = op->y;

    if (op->type == PLAYER)
        esrv_del_item (op->contr, tmp->count);
    insert_ob_in_map(tmp, op->map, op,0);


    SET_FLAG (op, FLAG_NO_APPLY);
    remove_ob(op);
    insert_ob_in_map(op, op->map, op, INS_NO_MERGE | INS_NO_WALK_ON);
    CLEAR_FLAG (op, FLAG_NO_APPLY);

    /* Call this before we update the various windows/players.  At least
     * that we, we know the weight is correct.
     */
    fix_player(op); /* This is overkill, fix_player() is called somewhere */
		    /* in object.c */

    if (op->type == PLAYER)
    {
    op->contr->socket.update_look = 1;
/*    esrv_send_item (op, tmp);*/
    /* Need to update the weight for the player */
    esrv_send_item (op, op);
    }
}

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
      new_draw_info(NDI_UNIQUE, 0,op,"You don't have anything to drop.");
      return;
    }
    if (QUERY_FLAG(tmp, FLAG_INV_LOCKED)) {
      new_draw_info(NDI_UNIQUE, 0,op,"This item is locked");
      return;
    }
    if (QUERY_FLAG(tmp, FLAG_NO_DROP)) {
#if 0
      /* Eneq(@csd.uu.se): Objects with NO_DROP defined can't be dropped. */
      new_draw_info(NDI_UNIQUE, 0,op, "This item can't be dropped.");
#endif
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



/* Command will drop all items that have not been locked */
int command_dropall (object *op, char *params) {

  object * curinv, *nextinv;

  if(op->inv == NULL) {
    new_draw_info(NDI_UNIQUE, 0,op,"Nothing to drop!");
    return 0;
  }
  
  curinv = op->inv;

  /* 
    This is the default.  Drops everything not locked or considered
    not something that should be dropped.
  */
  /*
    Care must be taken that the next item pointer is not to money as
    the drop() routine will do unknown things to it when dropping
    in a shop. --Tero.Pelander@utu.fi
  */

  if(params==NULL) {
    while(curinv != NULL) {
      nextinv = curinv->below;
      while (nextinv && nextinv->type==MONEY)
	nextinv = nextinv->below;
      if(! QUERY_FLAG(curinv,FLAG_INV_LOCKED) && curinv->type != MONEY &&
	 curinv->type != FOOD && curinv->type != KEY && 
	 curinv->type != SPECIAL_KEY && curinv->type != GEM &&
	 !curinv->invisible &&
	 (curinv->type!=CONTAINER || op->container!=curinv))
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
      if(! QUERY_FLAG(curinv,FLAG_INV_LOCKED) && ((curinv->type == WEAPON) ||
	 (curinv->type == BOW) || (curinv->type == ARROW)))
	{
	  drop(op,curinv);
	}
      curinv = nextinv;
    }
  }
  
  else if(strcmp(params, "armor") == 0 || strcmp(params, "armour") == 0) {
    while(curinv != NULL) {
      nextinv = curinv->below;
      while (nextinv && nextinv->type==MONEY)
	nextinv = nextinv->below;
      if(! QUERY_FLAG(curinv,FLAG_INV_LOCKED) && ((curinv->type == ARMOUR) ||
	 curinv->type == SHIELD || curinv->type==HELMET))
	{
	  drop(op,curinv);
	}
      curinv = nextinv;
    }
  }

  else if(strcmp(params, "misc") == 0) {
    while(curinv != NULL) {
      nextinv = curinv->below;
      while (nextinv && nextinv->type==MONEY)
	nextinv = nextinv->below;
      if(! QUERY_FLAG(curinv,FLAG_INV_LOCKED) && ! QUERY_FLAG(curinv,FLAG_APPLIED)) {
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

/* Object op wants to drop object(s) params.  params can be a
 * comma seperated list.
 */

int command_drop (object *op, char *params)
{
    object  *tmp, *next;
    int did_one=0;

    if (!params) {
	new_draw_info(NDI_UNIQUE,0, op, "Drop what?");
	return 0;
    } else {
	for (tmp=op->inv; tmp; tmp=next) {
	    next=tmp->below;
	    if (QUERY_FLAG(tmp,FLAG_NO_DROP) ||
		tmp->invisible) continue;
	    if (item_matched_string(op,tmp,params)) {
		drop(op, tmp);
		did_one=1;
	    }
	}
	if (!did_one) new_draw_info(NDI_UNIQUE, 0,op,"Nothing to drop.");
    }
    if (op->type==PLAYER)
    {
        op->contr->count=0;
        op->contr->socket.update_look=1;
    };
/*    draw_look(op);*/
    return 0;
}

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
	    new_draw_info_format(NDI_UNIQUE,0,op,"Could not find an object that matches %s",params);
  }
  return 0;
}

/* op should be a player.
 * we return the object the player has marked with the 'mark' command
 * below.  If no match is found (or object has changed), we return
 * NULL.  We leave it up to the calling function to print messages if
 * nothing is found.
 */
object *find_marked_object(object *op)
{
    object *tmp;

    if (!op || !op->contr) return NULL;
    if (!op->contr->mark) {
/*	new_draw_info(NDI_UNIQUE,0,op,"You have no marked object");*/
	return NULL;
    }
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
/*		new_draw_info(NDI_UNIQUE,0,op,"You have no marked object");*/
		return NULL;
	    }
	}
    }
    return NULL;
}
    

/* op should be a player, params is any params.
 * If no params given, we print out the currently marked object.
 * otherwise, try to find a matching object - try best match first.
 */
int command_mark(object *op, char *params)
{
    if (!op->contr) return 1;
    if (!params) {
	object *mark=find_marked_object(op);
	if (!mark) new_draw_info(NDI_UNIQUE,0,op,"You have no marked object.");
	else new_draw_info_format(NDI_UNIQUE,0,op,"%s is marked.", query_name(mark));
    }
    else {
	object *mark1=find_best_object_match(op, params);
	if (!mark1) {
	    new_draw_info_format(NDI_UNIQUE,0,op,"Could not find an object that matches %s",params);
	    return 1;
	}
	else {
	    op->contr->mark=mark1;
	    op->contr->mark_count=mark1->count;
	    new_draw_info_format(NDI_UNIQUE,0,op,"Marked item %s", query_name(mark1));
	    return 0;
	}
    }
    return 0;	/*shouldnt get here */
}


/* op is the player
 * tmp is the monster being examined.
 */
void examine_monster(object *op,object *tmp) {
    object *mon=tmp->head?tmp->head:tmp;

    if(QUERY_FLAG(mon,FLAG_UNDEAD))
	new_draw_info(NDI_UNIQUE, 0,op,"It is an undead force.");
    if(mon->level>op->level)
	new_draw_info(NDI_UNIQUE, 0,op,"It is likely more powerful than you.");
    else if(mon->level<op->level)
	new_draw_info(NDI_UNIQUE, 0,op,"It is likely less powerful than you.");
    else
	new_draw_info(NDI_UNIQUE, 0,op,"It is probably as powerful as you.");
    if(mon->attacktype&AT_ACID)
	new_draw_info(NDI_UNIQUE, 0,op,"You seem to smell an acrid odor.");

    /* Anyone know why this used to use the clone value instead of the
     * maxhp field?  This seems that it should give more accurate results.
     */
    switch((mon->stats.hp+1)*4/(mon->stats.maxhp+1)) { /* From 1-4 */
	case 1:
	    new_draw_info(NDI_UNIQUE, 0,op,"It is in a bad shape.");
	    break;
	case 2:
	    new_draw_info(NDI_UNIQUE, 0,op,"It is hurt.");
	    break;
	case 3:
	    new_draw_info(NDI_UNIQUE, 0,op,"It is somewhat hurt.");
	    break;
	case 4:
	    new_draw_info(NDI_UNIQUE, 0,op,"It is in excellent shape.");
	    break;
    }
    if(present_in_ob(POISONING,mon)!=NULL)
	new_draw_info(NDI_UNIQUE, 0,op,"It looks very ill.");
}

/* tmp is the object being described, pl is who is examing it. */
char *long_desc(object *tmp, object *pl) {
  static char buf[VERY_BIG_BUF];
  char *cp;

  if(tmp==NULL)
    return "";
  buf[0]='\0';
  switch(tmp->type) {
  case RING:
  case SKILL:
  case WEAPON:
  case ARMOUR:
  case BRACERS:
  case HELMET:
  case SHIELD:
  case BOOTS:
  case GLOVES:
  case AMULET:
  case GIRDLE:
  case BOW:
  case ARROW:
  case CLOAK:
  case FOOD:
  case DRINK:
  case FLESH:
    if(*(cp=describe_item(tmp, pl))!='\0') {
	int len;

	strncpy(buf,query_name(tmp), VERY_BIG_BUF-1);
	buf[VERY_BIG_BUF-1]=0;
	len=strlen(buf);
	if (len<VERY_BIG_BUF-5) {
	    /* Since we know the length, we save a few cpu cycles by using
	     * it instead of calling strcat */
	    strcpy(buf+len," ");
	    len++;
	    strncpy(buf+len, cp, VERY_BIG_BUF-len-1);
	    buf[VERY_BIG_BUF-1]=0;
	}
    }
  }
  if(buf[0]=='\0') {
	strncpy(buf,query_name(tmp), VERY_BIG_BUF-1);
	buf[VERY_BIG_BUF-1]=0;
  }

  return buf;
}

void examine(object *op, object *tmp) {
    char buf[VERY_BIG_BUF];
    int i;

    if (tmp == NULL || tmp->type == CLOSE_CON)
	return;

    strcpy(buf,"That is ");
    strncat(buf, long_desc(tmp, op), VERY_BIG_BUF-strlen(buf)-1);
    buf[VERY_BIG_BUF-1]=0;

    new_draw_info(NDI_UNIQUE, 0,op,buf);
    buf[0]='\0';
    switch(tmp->type) {
	case SPELLBOOK:
	    if(QUERY_FLAG(tmp, FLAG_IDENTIFIED) && tmp->stats.sp >= 0 && tmp->stats.sp <= NROFREALSPELLS ) {
		if(!strcmp(tmp->arch->name,"cleric_book"))
		    sprintf(buf,"%s is a %d level prayer.",
			    spells[tmp->stats.sp].name,spells[tmp->stats.sp].level);
		else
		    sprintf(buf,"%s is a %d level spell.",
			    spells[tmp->stats.sp].name,spells[tmp->stats.sp].level);
	    }
	break;

	case BOOK:
	    if(tmp->msg!=NULL)
		strcpy(buf,"Something is written in it.");
	break;

	case CONTAINER:
	    if(tmp->race!=NULL) {
		if(tmp->weight_limit && tmp->stats.Str<100)
		    sprintf (buf,"It can hold only %s and its weight limit is %.1f kg.", 
			 tmp->race, tmp->weight_limit/(10.0 * (100 - tmp->stats.Str)));
		else
		    sprintf (buf,"It can hold only %s.", tmp->race);
	    } else
		if(tmp->weight_limit && tmp->stats.Str<100)
		    sprintf (buf,"Its weight limit is %.1f kg.", 
			     tmp->weight_limit/(10.0 * (100 - tmp->stats.Str)));
	    break;

	case WAND:
	    if(QUERY_FLAG(tmp, FLAG_IDENTIFIED))
		sprintf(buf,"It has %d charges left.",tmp->stats.food);
	    break;
    }

    if(buf[0]!='\0')
	new_draw_info(NDI_UNIQUE, 0,op,buf);

    if(tmp->material && !tmp->msg) {
	strcpy(buf,"It is made of: ");
	for(i=0; i < NROFMATERIALS; i++) {
	  if(tmp->material & (1<<i)) {
	    strcat(buf, material[i].name);
	    strcat(buf, " ");
	  }
	}
	new_draw_info(NDI_UNIQUE, 0,op,buf);
    }
    /* Where to wear this item */
    for (i=0; i < NUM_BODY_LOCATIONS; i++) {
	if (tmp->body_info[i]<-1) {
	    if (op->body_info[i])
		new_draw_info_format(NDI_UNIQUE, 0,op, 
			"It goes %s (%d)", body_locations[i].use_name, -tmp->body_info[i]);
	    else
		new_draw_info_format(NDI_UNIQUE, 0,op, 
		        "It goes %s", body_locations[i].nonuse_name);
	} else if (tmp->body_info[i]) {
	    if (op->body_info[i])
		new_draw_info_format(NDI_UNIQUE, 0,op, 
			"It goes %s", body_locations[i].use_name);
	    else
		new_draw_info_format(NDI_UNIQUE, 0,op, 
			"It goes %s", body_locations[i].nonuse_name);
	}
    }

    if(tmp->weight) {
	sprintf(buf,tmp->nrof>1?"They weigh %3.3f kg.":"It weighs %3.3f kg.",
            (tmp->nrof?tmp->weight*tmp->nrof:tmp->weight)/1000.0);
	new_draw_info(NDI_UNIQUE, 0,op,buf);
    }

    if(tmp->value&&!QUERY_FLAG(tmp, FLAG_STARTEQUIP)) {
	if(QUERY_FLAG(tmp, FLAG_UNPAID))
	    sprintf(buf,"%s would cost you %s.",
		    tmp->nrof>1?"They":"It",query_cost_string(tmp,op,F_BUY));
	else
	    sprintf(buf,"You would get %s for the %s.",
		    query_cost_string(tmp,op,F_SELL), tmp->nrof>1?"them":"it");
	new_draw_info(NDI_UNIQUE, 0,op,buf);
    }

    if(QUERY_FLAG(tmp, FLAG_MONSTER))
	examine_monster(op,tmp);
   

    /* Does the object have a message?  Don't show message for all object
     * types - especially if the first entry is a match
     */
    if(tmp->msg && tmp->type != EXIT && tmp->type != BOOK && 
       tmp->type != CORPSE && !QUERY_FLAG(tmp, FLAG_WALK_ON) && 
       strncasecmp(tmp->msg, "@match",7)) {

	/* This is just a hack so when identifying hte items, we print
	 * out the extra message
	 */
	if (need_identify(tmp) && QUERY_FLAG(tmp, FLAG_IDENTIFIED))
	    new_draw_info(NDI_UNIQUE, 0,op, "The object has a story:");

	new_draw_info(NDI_UNIQUE, 0,op,tmp->msg);
    }
    new_draw_info(NDI_UNIQUE, 0,op," "); /* Blank line */
}

/*
 * inventory prints object's inventory. If inv==NULL then print player's
 * inventory. 
 * [ Only items which are applied are showed. Tero.Haatanen@lut.fi ]
 */
void inventory(object *op,object *inv) {
  object *tmp;
  char *in;
  int items = 0, length;

  if (inv==NULL && op==NULL) {
    new_draw_info(NDI_UNIQUE, 0,op,"Inventory of what object?");
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
      new_draw_info(NDI_UNIQUE, 0,op,"You carry nothing.");
      return;
    } else {
      length = 28;
      in = "";
      if (op)
        clear_win_info(op);
      new_draw_info(NDI_UNIQUE, 0,op,"Inventory:");
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
    if((!op || QUERY_FLAG(op, FLAG_WIZ)))
      new_draw_info_format(NDI_UNIQUE, 0,op ,"%s- %-*.*s (%5d) %-8s", in, length, length,
		     query_name(tmp), tmp->count,query_weight(tmp));
    else
      new_draw_info_format(NDI_UNIQUE,0, op, "%s- %-*.*s %-8s", in, length+8, 
		     length+8, query_name(tmp),
                     query_weight(tmp));
  }
  if(!inv && op) {
    new_draw_info_format(NDI_UNIQUE,0, op ,"%-*s %-8s",
            41,"Total weight :",query_weight(op));
  }
}


int command_pickup (object *op, char *params)
{
  uint32 i;
  char putstring[128];

  if(!params) {
    /* if the new mode is used, just print the settings */
    /* yes, a GOTO is ugly, but its simpple and should stay until this
     * mode is cleanly integrated and the old one deprecated */
    if(op->contr->mode & PU_NEWMODE) 
    {
      i=op->contr->mode;
      goto NEWPICKUP;
    }
    if(1)fprintf(stderr,"command_pickup: !params\n");
    set_pickup_mode(op, (op->contr->mode > 6)? 0: op->contr->mode+1);
    return 0;
  }
  if(params==NULL || !sscanf(params, "%ud", &i) || i<0 ) {
    if(1)fprintf(stderr,"command_pickup: params==NULL\n");
    new_draw_info(NDI_UNIQUE, 0,op,"Usage: pickup <0-7> or <value_density> .");
    return 1;
  }
  set_pickup_mode(op,i);

/* To me, all this output is meaningless - it certainly is not
 * humanly readable, and if anything will create more questions/bugs
 * as people will ask 'what does this strange message mean'.
 */

#if 0
  sprintf(putstring,"command_pickup: set_pickup_mode\ndec %u, 0x%x", i, i);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"0b");

  for(j=0;j<32;j++) 
  {
    strcat(putstring,((i>>(31-j))&0x01)?"1":"0");
    if(!((j+1)%4))strcat(putstring," ");
  }
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
#endif

NEWPICKUP:
#if 1
  if(!(i & PU_NEWMODE)) return 1;

  sprintf(putstring,"%d NEWMODE",i & PU_NEWMODE?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d DEBUG",i & PU_DEBUG?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d INHIBIT",i & PU_INHIBIT?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d STOP",i & PU_STOP?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);

  sprintf(putstring,"%d <= x pickup weight/value RATIO (0==off)",(i & PU_RATIO)*5);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);

  sprintf(putstring,"%d FOOD",i & PU_FOOD?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d DRINK",i & PU_DRINK?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d VALUABLES",i & PU_VALUABLES?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);

  sprintf(putstring,"%d BOW",i & PU_BOW?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d ARROW",i & PU_ARROW?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);

  sprintf(putstring,"%d HELMET",i & PU_HELMET?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d SHIELD",i & PU_SHIELD?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d ARMOUR",i & PU_ARMOUR?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);

  sprintf(putstring,"%d BOOTS",i & PU_BOOTS?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d GLOVES",i & PU_GLOVES?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d CLOAK",i & PU_CLOAK?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d KEY",i & PU_KEY?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);

  sprintf(putstring,"%d MISSILEWEAPON",i & PU_MISSILEWEAPON?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d ALLWEAPON",i & PU_ALLWEAPON?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d MAGICAL",i & PU_MAGICAL?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d POTION",i & PU_POTION?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);

  sprintf(putstring,"%d SPELLBOOK",i & PU_SPELLBOOK?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d SKILLSCROLL",i & PU_SKILLSCROLL?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);
  sprintf(putstring,"%d READABLES",i & PU_READABLES?1:0);
  new_draw_info(NDI_UNIQUE, 0,op,putstring);

  new_draw_info(NDI_UNIQUE, 0,op,"");
#endif

  return 1;
}

void set_pickup_mode(object *op,int i) {
  switch(op->contr->mode=i) {
    case 0:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Don't pick up.");
      break;
    case 1:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up one item.");
      break;
    case 2:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up one item and stop.");
      break;
    case 3:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Stop before picking up.");
      break;
    case 4:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up all items.");
      break;
    case 5:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up all items and stop.");
      break;
    case 6:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up all magic items.");
      break;
    case 7:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up all coins and gems");
      break;
    }
}

int command_search_items (object *op, char *params)
{
      char buf[MAX_BUF];

      if (settings.search_items == FALSE)
	  return 1;

  if(params == NULL) {
	if(op->contr->search_str[0]=='\0') {
	  new_draw_info(NDI_UNIQUE, 0,op,"Example: search magic+1");
	  new_draw_info(NDI_UNIQUE, 0,op,"Would automatically pick up all");
	  new_draw_info(NDI_UNIQUE, 0,op,"items containing the word 'magic+1'.");
	  return 1;
	}
	op->contr->search_str[0]='\0';
	new_draw_info(NDI_UNIQUE, 0,op,"Search mode turned off.");
	fix_player(op);
	return 1;
  }
  if((int)strlen(params) >= MAX_BUF) {
	new_draw_info(NDI_UNIQUE, 0,op,"Search string too long.");
	return 1;
      }
  strcpy(op->contr->search_str, params);
      sprintf(buf,"Searching for '%s'.",op->contr->search_str);
      new_draw_info(NDI_UNIQUE, 0,op,buf);
  fix_player(op);
      return 1;
}

