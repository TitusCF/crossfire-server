/*
 * static char *rcsid_move_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel
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
*/

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/* object op is trying to move in direction dir.
 * originator is typically the same as op, but
 * can be different if originator is causing op to
 * move (originator is pushing op)
 * returns 0 if the object is not able to move to the
 * desired space, 1 otherwise (in which case we also 
 * move the object accordingly
 */
int move_ob (object *op, int dir, object *originator)
{
    object *tmp_ob=NULL;

    if(op==NULL) {
	LOG(llevError,"Trying to move NULL.\n");
	return 0;
    }
    /* this function should now only be used on the head - it won't call itself
     * recursively, and people calling us should pass the right part.
     */
    if (op->head) {
	LOG(llevDebug,"move_ob called with non head object: %s %s (%d,%d)\n",
	    op->head->name, op->map->name, op->x, op->y);
	op = op->head;
    }

    /* If the space the player is trying to is out of the map,
     * bail now - we know it can't work.
     */
    if (out_of_map(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir])) return 0;


    /* Modified these to check for appropriate object anyplace on map
     * not just the top (stacking is less deterministic now).
     * also now have it return 0 on both of these cases.  The
     * monster may be able to move there, but hacking the door or
     * earthwall was their action this tick.
     * will_apply should only be set for monsters, so players should
     * never use this code.  These could probably get collapsed into
     * one while loop, but that won't really save much code.
     * MSW 2001-07-19
     */

    if (op->will_apply&4 ) {
	tmp_ob=get_map_ob(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]);
	while(tmp_ob->above != NULL) {
	    if (tmp_ob->type == EARTHWALL) {
		hit_player(tmp_ob,5,op,AT_PHYSICAL); /* Tear down the earthwall */
		return 0;
	    }
            tmp_ob = tmp_ob->above; /* To prevent endless loop */
	}
    }
    if (op->will_apply&8 ) {
	tmp_ob=get_map_ob(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]);
	while(tmp_ob->above != NULL) {
	    if (tmp_ob->type == DOOR) {
		hit_player(tmp_ob,9998,op,AT_PHYSICAL); /* Tear down the earthwall */
		return 0;
	    }
            tmp_ob = tmp_ob->above; /* To prevent endless loop */
	}
    }


    /* Modified logic here.  Split code into one for single part objects
     * and one for multi part objects.
     */
    if (op->more == NULL) {
	/* if player does not have wizpass, and the space is blocked and
	 * blocked two, return 0.  We make calls to both blocked and blocked_two
	 * because we only want to call blocked_two when necessary (its a costly
	 * operation), so if the space is not blocked (which is a very cheap 
	 * operation), we then don't need to call blocked_two
	 */
	if (!QUERY_FLAG(op,FLAG_WIZPASS) && 
	    blocked(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]) && 
	    blocked_two(op,op->x+freearr_x[dir],op->y+freearr_y[dir])) return 0;

	/* Else, move this object */
	remove_ob(op);
	op->x+=freearr_x[dir];
	op->y+=freearr_y[dir];
	insert_ob_in_map(op,op->map,originator,0);
	/* Currently, assume that players will only be single space objects */
	if (op->type==PLAYER) {
	    esrv_map_scroll(&op->contr->socket, freearr_x[dir],freearr_y[dir]);
	    op->contr->socket.update_look=1;
	    op->contr->socket.look_position=0;
	}
	return 1;
    } else {
	/* Multipart object handling.  Its my belief that is more efficient
	 * to call blocked_link on each space of the monster than needing
	 * to remove the entire thing and re-insert the entire thing in a case
	 * where it may not be able to move.  MSW 2001-07-19.
	 * If any space is blocked by something other than the object
	 * (which blocked_linked returns), bail out immediately.  Doesn't do
	 * any good if 3 out of 4 spaces are available.  Note also that
	 * some logic for single part monsters is not included here - I presume
	 * that players (and hence wizpass) can only play single space
	 * objects (poor assumption I know)
	 */
	for (tmp_ob = op; tmp_ob != NULL; tmp_ob = tmp_ob->more) {
	    if (blocked_link(op, tmp_ob->x+freearr_x[dir],tmp_ob->y+freearr_y[dir])) return 0;

	}
	/* If we got here, it is possible to move this object.  Remove it,
	 * update the coordinates, and re-insert it.  Note that while
	 * remove_ob takes care of removing all parts, but we need to
	 * insert each one by hand.  Also, insert_ob_in_map does special
	 * processing for the head object - for this to work right,
	 * we want to insert that last.
	 */
	for (tmp_ob = op->more; tmp_ob != NULL; tmp_ob = tmp_ob->more) {
	    tmp_ob->x+=freearr_x[dir];
	    tmp_ob->y+=freearr_y[dir];
	    /* WALK_ON should be handled when head is inserted for all parts.
	     * lets just make sure of that.
	     */
	    insert_ob_in_map(tmp_ob, op->map, originator, INS_NO_WALK_ON);
	}
	op->x+=freearr_x[dir];
	op->y+=freearr_y[dir];
	insert_ob_in_map(op, op->map, originator, INS_NO_WALK_ON);
	return 1;
    }
    return 1;	/* this shouldn't be reached */
}


/*
 * transfer_ob(): Move an object (even linked objects) to another spot
 * on the same map.
 *
 * Does nothing if there is no free spot.
 *
 * randomly: If true, use find_free_spot() to find the destination, otherwise
 * use find_first_free_spot().
 *
 * Return value: 1 if object was destroyed, 0 otherwise.
 */

int transfer_ob (object *op, int x, int y, int randomly, object *originator)
{
  int i;
  object *tmp;

  if (randomly)
    i = find_free_spot (op->arch,op->map,x,y,0,SIZEOFFREE);
  else
    i = find_first_free_spot(op->arch,op->map,x,y);
  if (i==-1)
    return 0;	/* No free spot */

  if(op->head!=NULL)
    op=op->head;
  remove_ob(op);
  for(tmp=op;tmp!=NULL;tmp=tmp->more)
    tmp->x=x+freearr_x[i]+(tmp->arch==NULL?0:tmp->arch->clone.x),
    tmp->y=y+freearr_y[i]+(tmp->arch==NULL?0:tmp->arch->clone.y);
  return insert_ob_in_map(op,op->map,originator,0) == NULL;
}

/*
 * Return value: 1 if object was destroyed, 0 otherwise.
 * Modified so that instead of passing the 'originator' that had no
 * real use, instead we pass the 'user' of the teleporter.  All the
 * callers know what they wanted to teleporter (move_teleporter or
 * shop map code)
 * tele_type is the type of teleporter we want to match against -
 * currently, this is either set to SHOP_MAT or TELEPORTER.
 * It is basically used so that shop_mats and normal teleporters can
 * be used close to each other and not have the player put to the
 * one of another type.
 */
int teleport (object *teleporter, uint8 tele_type, object *user)
{
    object *altern[120]; /* Better use c/malloc here in the future */
    int i,j,k,nrofalt=0;
    object *other_teleporter, *tmp;

    if(user==NULL) return 0;
    if(user->head!=NULL)
	user=user->head;

    /* Find all other teleporters within range.  This range
     * should really be setable by some object attribute instead of
     * using hard coded values.
     */
    for(i= -5;i<6;i++)
	for(j= -5;j<6;j++) {
	    if(i==0&&j==0)
		continue;
	    if(out_of_map(teleporter->map,teleporter->x+i,teleporter->y+j))
		continue;
	    other_teleporter=get_map_ob(teleporter->map,
                                  teleporter->x+i,teleporter->y+j);

	    while (other_teleporter) {
		if (other_teleporter->type == tele_type) break;
		other_teleporter = other_teleporter->above;
	    }
	    if (other_teleporter)
		altern[nrofalt++]=other_teleporter;
	}

    if(!nrofalt) {
	LOG(llevError,"No alternative teleporters around!\n");
	return 0;
    }

    other_teleporter=altern[RANDOM()%nrofalt];
    k=find_free_spot(user->arch,other_teleporter->map,
                        other_teleporter->x,other_teleporter->y,1,9);
    if (k==-1)
	return 0;

    remove_ob(user);

    /* Update location for the object */
    for(tmp=user;tmp!=NULL;tmp=tmp->more) {
	tmp->x=other_teleporter->x+freearr_x[k]+
           (tmp->arch==NULL?0:tmp->arch->clone.x);
	tmp->y=other_teleporter->y+freearr_y[k]+
           (tmp->arch==NULL?0:tmp->arch->clone.y);
    }
    tmp = insert_ob_in_map(user,other_teleporter->map,NULL,0);
    if (tmp && tmp->type == PLAYER) MapNewmapCmd(tmp->contr);
    return (tmp == NULL);
}

void recursive_roll(object *op,int dir,object *pusher) {
  if(!roll_ob(op,dir,pusher)) {
    new_draw_info_format(NDI_UNIQUE, 0, pusher,
	"You fail to push the %s.",query_name(op));
    return;
  }
  (void) move_ob(pusher,dir,pusher);
  new_draw_info_format(NDI_BLACK, 0, pusher,
	"You roll the %s.",query_name(op));
  return;
}

/*
 * This is a new version of blocked, this one handles objects
 * that can be passed through by monsters with the CAN_PASS_THRU defined.
 *
 * very new version handles also multipart objects
 */

int try_fit (object *op, int x, int y) 
{
    object *tmp, *more;
    int tx, ty;
    if (op->head) 
	op = op->head;

    for (more = op; more ; more = more->more) {
	tx = x + more->x - op->x;
	ty = y + more->y - op->y;
	if (out_of_map(op->map,tx,ty))
	    return 1;

	for (tmp = get_map_ob (more->map, tx, ty); tmp; tmp=tmp->above) {
	    if (tmp->head == op || tmp == op)
		continue;

	    if ((QUERY_FLAG(tmp,FLAG_ALIVE) && tmp->type!=DOOR))
		return 1;

	    if (QUERY_FLAG(tmp,FLAG_NO_PASS) && 
		(!QUERY_FLAG(tmp,FLAG_PASS_THRU) ||
		 !QUERY_FLAG(more,FLAG_CAN_PASS_THRU)))
		return 1;
	}
    }
    return 0;
}

/*
 * this is not perfect yet. 
 * it does not roll objects behind multipart objects properly.
 */

int roll_ob(object *op,int dir, object *pusher) {
    object *tmp;
    int x, y;
    if (op->head) 
	op = op->head;
    x=op->x+freearr_x[dir];
    y=op->y+freearr_y[dir];
    if(!QUERY_FLAG(op,FLAG_CAN_ROLL) || 
       (op->weight &&
       random_roll(0, op->weight/50000-1, pusher, PREFER_LOW) > pusher->stats.Str))
	return 0;

    if(out_of_map(op->map,x,y))
	return 0;

    for(tmp=get_map_ob(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]);
	tmp!=NULL;tmp=tmp->above) {
	if (tmp->head == op)
	    continue;
	if(QUERY_FLAG(tmp,FLAG_ALIVE))
	    return 0;
	if (QUERY_FLAG(tmp,FLAG_NO_PASS) && !roll_ob(tmp,dir,pusher))
	    return 0;
    }
    if (try_fit (op, x, y))
	return 0;

    remove_ob(op);
    for(tmp=op; tmp!=NULL; tmp=tmp->more)
	tmp->x+=freearr_x[dir],tmp->y+=freearr_y[dir];
    insert_ob_in_map(op,op->map,pusher,0);
    return 1;
}

/* returns 1 if pushing invokes a attack, 0 when not */
int push_ob(object *who, int dir, object *pusher) {
  int str1, str2;
  object *owner;

  if (who->head != NULL)
    who = who->head;
  owner = get_owner(who);

  CLEAR_FLAG(who,FLAG_SLEEP); /* what ever: this action lets wakeup the sucker */
  
  /* player change place with his pets or summoned creature */
  /* TODO: allow multi arch pushing. Can't be very difficult */
  if (who->more == NULL && owner == pusher)
  {
    int temp;
    remove_ob(who);
    remove_ob(pusher);
    temp = pusher->x;
    pusher->x = who->x;
    who->x = temp;
    temp = pusher->y;
    pusher->y = who->y;
    who->y = temp;
    insert_ob_in_map (who,who->map,pusher,0);
    insert_ob_in_map (pusher,pusher->map,pusher,0);
    return 0;
  }


  /* We want ONLY become enemy of evil, unaggressive monster. We must RUN in them */
  /* In original we have here a unaggressive check only - that was the reason why */
  /* we so often become an enemy of friendly monsters... */
  /* funny: was they set to unaggressive 0 (= not so nice) they don't attack */
  if(owner != pusher &&  pusher->type == PLAYER && who->type != PLAYER &&
      !QUERY_FLAG(who,FLAG_FRIENDLY)&& !QUERY_FLAG(who,FLAG_NEUTRAL))
  {
      if(pusher->contr->run_on) /* only when we run */
      {
          new_draw_info_format(NDI_UNIQUE, 0, pusher,
              "You start to attack %s !!",who->name);
          CLEAR_FLAG(who,FLAG_UNAGGRESSIVE); /* the sucker don't like you anymore */
          who->enemy = pusher;
          return 1;
      }
      else 
      {
        new_draw_info_format(NDI_UNIQUE, 0, pusher,
             "You avoid to attack %s .",who->name);
      }
  }

  /* now, lets test stand still we NEVER can psuh stand_still monsters. */
  if(QUERY_FLAG(who,FLAG_STAND_STILL))
  {
      new_draw_info_format(NDI_UNIQUE, 0, pusher,
          "You can't push %s.",who->name);
      return 0;
  }
  
  /* ok, now we are here. I only allow player pushing */
  /* for monsters, we have the arch commands how and why they can move/pushed */
  /* and we have melee power like knockback */
  str1 = (who->stats.Str>0?who->stats.Str:who->level);
  str2 = (pusher->stats.Str>0?pusher->stats.Str:pusher->level);
  if(QUERY_FLAG(who,FLAG_WIZ) ||
     random_roll(str1, str1/2+str1*2, who, PREFER_HIGH) >= 
     random_roll(str2, str2/2+str2*2, pusher, PREFER_HIGH) ||
     !move_ob(who,dir,pusher))
  {
    if (who ->type == PLAYER) {
      new_draw_info_format(NDI_UNIQUE, 0, who,
	"%s tried to push you.",pusher->name);
    }

    return 0;
  }

  if (who->type == PLAYER) {
    new_draw_info_format(NDI_UNIQUE, 0, who,
	"%s pushed you.",pusher->name);
  }
  else if (QUERY_FLAG(who, FLAG_MONSTER)) {
      new_draw_info_format(NDI_UNIQUE, 0, pusher,
          "You pushed %s back.", who->name);
  }
  
  return 1;
}
