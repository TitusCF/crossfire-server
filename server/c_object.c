/*
 * static char *rcsid_c_object_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2000 Mark Wedel
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

    The author can be reached via e-mail to mwedel@scruz.net

   Object (handling) commands
*/

#include <global.h>
#include <loader.h>
#include <sproto.h>
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

/* This is a subset of the parse_id command.  Basically, name can be
 * a string seperated lists of things to match, with certain keywords.
 * pl is the player (only needed to set count properly)
 * op is the item we are trying to match.  Calling function takes care
 * of what action might need to be done and if it is valid
 * (pickup, drop, etc.)  Return NONZERO if we have a match.  A higher
 * value means a better match.  0 means no match.
 *
 * Brief outline of the procedure:
 * We take apart the name variable into the individual components.
 * cases for 'all' and unpaid are pretty obvious.
 * Next, we check for a count (either specified in name, or in the
 * player object.)
 * If count is 1, make a quick check on the name.
 * IF count is >1, we need to make plural name.  Return if match.
 * Last, make a check on the full name.
 */
int item_matched_string(object *pl, object *op, char *name)
{

    char *cp, local_name[MAX_BUF];
    int count,retval=0;

    strcpy(local_name, name);	/* strtok is destructive to name */
    for (cp=strtok(local_name,","); cp; cp=strtok(NULL,",")) {
	while (cp[0]==' ') ++cp;	/* get rid of spaces */
/*	LOG(llevDebug,"Trying to match %s\n", cp);*/
	/* All is a very generic match - low match value */
	if (!strcmp(cp,"all")) return 1;
	/* unpaid is a little more specific */
	if (!strcmp(cp,"unpaid") && QUERY_FLAG(op,FLAG_UNPAID)) return 2;
	if (!strcmp(cp,"cursed") && QUERY_FLAG(op,FLAG_KNOWN_CURSED) &&
	    (QUERY_FLAG(op,FLAG_CURSED) ||QUERY_FLAG(op,FLAG_DAMNED)))
		return 2;
	/* Allow for things like '100 arrows' */
	if ((count=atoi(cp))!=0) {
	    cp=strchr(cp, ' ');
	    while (cp && cp[0]==' ') ++cp;	/* get rid of spaces */
	}
	else count=pl->contr->count;
	if (!cp || cp[0]=='\0' || count<0) return 0;
	/* base name matched - not bad */
	if (strcasecmp(cp,op->name)==0 && !count) return 4;
	else if (count>1) {	/* Need to plurify name for proper match */
	    char newname[MAX_BUF];
	    strcpy(newname, op->name);
 	    if (QUERY_FLAG(op,FLAG_NEED_IE)) {
		char *cp1=strrchr(newname,'y');
                if(cp1!=NULL)
                    *cp1='\0'; /* Strip the 'y' */
                strcat(newname,"ies");
	    }
	    else strcat(newname,"s");
	    if (!strcasecmp(newname,cp)) {
		pl->contr->count=count;	/* May not do anything */
		return 6;
	    }
	}
	else if (count==1) {
	    if (!strcasecmp(op->name,cp)) {
		pl->contr->count=count;	/* May not do anything */
		return 6;
	    }
	}
	if (!strcasecmp(cp,query_name(op))) retval=20;
	else if (!strcasecmp(cp,query_short_name(op))) retval=18;
	else if (!strcasecmp(cp,query_base_name(op,0))) retval=16;
	else if (!strcasecmp(cp,query_base_name(op,1))) retval=16;
	else if (!strncasecmp(cp,query_base_name(op,0),
		MIN(strlen(cp),strlen(query_base_name(op,0))))) retval=14;
	else if (!strncasecmp(cp,query_base_name(op,1),
		MIN(strlen(cp),strlen(query_base_name(op,1))))) retval=14;
	if (retval) {
	    pl->contr->count=count;
	    return retval;
	}
    }
    return 0;
}

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
	new_draw_info_format(NDI_UNIQUE,0,pl,"Couldn't find skill %s", params);
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
	    "Could not find any match to %s.",params);
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
	sprintf (buf, "%s is not active.", query_name(sack));
    if (sack == op)
	sprintf (buf, "You can't put %s into itself.", query_name(sack));
    if (sack->race && (sack->race != op->race || op->type == CONTAINER
		       || (sack->stats.food && sack->stats.food != op->type)))
	sprintf (buf, "You can put only %s into %s.", sack->race,
		 query_name(sack));
    if (op->type == SPECIAL_KEY && sack->slaying && op->slaying)
	sprintf (buf, "You don't want put the key into %s.", query_name(sack));
    if (sack->weight_limit && sack->carrying + (nrof ? nrof : 1) * 
	(op->weight + (op->type==CONTAINER?(op->carrying*op->stats.Str):0))
	* (100 - sack->stats.Str) / 100  > sack->weight_limit)
	sprintf (buf, "That won't fit in %s!", query_name(sack));
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
void pick_up_object (object *pl, object *op, object *tmp, int nrof)
{
    char buf[MAX_BUF];
    object *env=tmp->env;
    uint32 weight;

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
    if(!can_pick(pl,tmp)) {
	if (tmp->name!=NULL) {
	    sprintf(buf,"You can't pick up a %s", tmp->name);
	    new_draw_info(NDI_UNIQUE, 0,pl, buf);
	}
	else
	    new_draw_info(NDI_UNIQUE, 0,pl,"You can't take that!");
	return;
    }
    if (QUERY_FLAG (tmp, FLAG_NO_DROP))
	return;
    if(QUERY_FLAG(tmp,FLAG_WAS_WIZ) && !QUERY_FLAG(pl, FLAG_WAS_WIZ)) {
	new_draw_info(NDI_UNIQUE, 0,pl, "The object disappears in a puff of smoke!");
	new_draw_info(NDI_UNIQUE, 0,pl, "It must have been an illusion.");
	if (pl->type==PLAYER) esrv_del_item (pl->contr, tmp->count);
	remove_ob(tmp);
	free_object(tmp);
	return;
    }
    
    /* startequip items are not allowed to be put into containers: */
    if (pl->type == PLAYER && op->type == CONTAINER &&
	QUERY_FLAG(tmp, FLAG_STARTEQUIP)) {
      new_draw_info(NDI_UNIQUE, 0,pl,"This object cannot be put into containers!");
      return;
    }
    
    if (nrof==0 || nrof>tmp->nrof) nrof=(tmp->nrof?tmp->nrof:1);
    /* Figure out how much weight this object will add to the player */
    weight = tmp->weight * nrof;
    if (tmp->inv) weight += tmp->carrying * (100 - tmp->stats.Str) / 100;
    if ((pl->weight + pl->carrying +weight) > weight_limit[pl->stats.Str]) {
	new_draw_info(NDI_UNIQUE, 0,pl,"That item is too heavy for you to pick up.");
	return;
    }
	

#ifndef REAL_WIZ
    if(QUERY_FLAG(pl, FLAG_WAS_WIZ))
	SET_FLAG(tmp, FLAG_WAS_WIZ);
#endif

    if(nrof != tmp->nrof && !(nrof == 1 && tmp->nrof == 0)) {
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
	if (tmp->env && pl->type==PLAYER) 
	    esrv_del_item (pl->contr, tmp->count);
	remove_ob(tmp); /* Unlink it */
    }
    if(QUERY_FLAG(tmp, FLAG_UNPAID))
	(void) sprintf(buf,"%s will cost you %s.", query_name(tmp),
		query_cost_string(tmp,op,F_BUY));
    else
	(void) sprintf(buf,"You pick up %s.", query_name(tmp));
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
    object *tmp=NULL;
    int count;

    if(alt)
	tmp=alt;
    else if(op->below==NULL || !can_pick(op, tmp)) {
	new_draw_info(NDI_UNIQUE, 0,op,"There is nothing to pick up here.");
	return;
    } else
	tmp=op->below;

    if (op->type==PLAYER && op->contr->count && op->contr->count <tmp->nrof)
	count=op->contr->count;
    else
	count=tmp->nrof;

    /* container is open, so use it */
    if (op->container) {
	alt = op->container;
	if (alt != tmp->env && !sack_can_hold (op, alt, tmp,count))
	    return;
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
    if(op->type==PLAYER) { 
       pick_up_object(op, alt, tmp, op->contr->count);
       op->contr->count=0;
    } else 
       pick_up_object(op, alt, tmp, tmp->nrof);
}


int command_take (object *op, char *params)
{
    object *tmp, *next;

    tmp=(op->container) ? op->container->inv : op->below;
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
		sprintf(buf,"You can't pick up a %s",
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
	"%s is not a container.", query_name(sack));
      return;
    }
    if (QUERY_FLAG(tmp,FLAG_STARTEQUIP)) {
      new_draw_info_format(NDI_UNIQUE, 0,op,
	"You cannot put %s in the container.", query_name(tmp));
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

    sprintf(buf, "You put %s in ", query_name(tmp));
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
	if (was_destroyed (tmp2, tmp2_tag))
	      esrv_del_item (op->contr, tmp2_tag);
	else
	      esrv_send_item (op, tmp2);
    } else
      remove_ob (tmp);

    if (QUERY_FLAG (tmp, FLAG_STARTEQUIP)) {
      sprintf(buf,"You drop %s.", query_name(tmp));
      new_draw_info(NDI_UNIQUE, 0,op,buf);
      new_draw_info(NDI_UNIQUE, 0,op,"The gods who lent it to you retrieves it.");
	esrv_del_item (op->contr, tmp->count);
      free_object(tmp);
      fix_player(op);
      return;
    }

/*  If SAVE_INTERVAL is commented out, we never want to save 
 *  the player here. 
 */
#ifdef SAVE_INTERVAL
    if (!QUERY_FLAG(tmp, FLAG_UNPAID) && 
      (tmp->nrof ? tmp->value * tmp->nrof : tmp->value > 2000))
#if SAVE_INTERVAL
      if((op->contr->last_save_time + SAVE_INTERVAL) <= time(NULL)) {
	  save_player(op, 1);
	  op->contr->last_save_time = time(NULL);
}
#else
	save_player(op,1); /* To avoid cheating */
#endif
#endif /* SAVE_INTERVAL */


    floor = get_map_ob (op->map, op->x, op->y);
    if( floor && floor->type == SHOP_FLOOR && 
       !QUERY_FLAG(tmp, FLAG_UNPAID) && tmp->type != MONEY)
      sell_item(tmp,op);

    tmp->x = op->x;
    tmp->y = op->y;
    insert_ob_in_map(tmp, op->map, op);

    SET_FLAG (op, FLAG_NO_APPLY);
    remove_ob(op);
    insert_ob_in_map_simple(op, op->map);
    CLEAR_FLAG (op, FLAG_NO_APPLY);

    /* Call this before we update the various windows/players.  At least
     * that we, we know the weight is correct.
     */
    fix_player(op); /* This is overkill, fix_player() is called somewhere */ 
		    /* in object.c */


    esrv_send_item (op, tmp);
    /* Need to update the weight for the player */
    esrv_send_item (op, op);


#ifdef USE_LIGHTING
    if(tmp->glow_radius>0) remove_light_from_list(tmp,op);
#endif
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

    if (op->container) {
      put_object_in_sack (op, op->container, tmp, op->contr->count);
    } else {
      drop_object (op, tmp, op->contr->count);
    }
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
    op->contr->count=0;
    op->contr->socket.update_look=1;
/*    draw_look(op);*/
    return 0;
}

int command_examine (object *op, char *params)
{
  if (!params)
    examine(op,op->below);
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

