/*
 * static char *rcsid_move_c =
 *    "$Id$";
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
*/

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/*
 * move_object() tries to move object op in the direction "dir".
 * If it fails (something blocks the passage), it returns 0,
 * otherwise 1.
 * This is an improvement from the previous move_ob(), which
 * removed and inserted objects even if they were unable to move.
 */

int move_object(object *op, int dir) {
    return move_ob(op, dir, op);
}


/* object op is trying to move in direction dir.
 * originator is typically the same as op, but
 * can be different if originator is causing op to
 * move (originator is pushing op)
 * returns 0 if the object is not able to move to the
 * desired space, 1 otherwise (in which case we also 
 * move the object accordingly.  This function is
 * very similiar to move_object.
  */
int move_ob (object *op, int dir, object *originator)
{
    int newx = op->x+freearr_x[dir];
    int newy = op->y+freearr_y[dir];
    object *tmp;

    if(op==NULL) {
	LOG(llevError,"Trying to move NULL.\n");
	return 0;
    }

    /* Is this space blocked?  Players with wizpass are immune to
     * this condition.
     */
    if(blocked_link(op, newx, newy) &&
       !QUERY_FLAG(op, FLAG_WIZPASS))
	return 0;

    /* If the space the player is trying to is out of the map,
     * bail now - we know it can't work.
     */
    if (out_of_map(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir])) return 0;

    /* 0.94.2 - we need to set the direction for the new animation code.
     * it uses it to figure out face to use - I can't see it
     * breaking anything, but it might.
     */
    if(op->more != NULL && !move_ob(op->more, dir, op->more->head))
	return 0;

    op->direction = dir;

    if(op->will_apply&4)
	check_earthwalls(op,newx,newy);
    if(op->will_apply&8)
	check_doors(op,newx,newy);

    /* 0.94.1 - I got a stack trace that showed it crash with remove_ob trying
     * to remove a removed object, and this function was the culprit.  A possible
     * guess I have is that check_doors above ran into a trap, killing the
     * monster.
     *
     * Unfortunately, it doesn't appear that the calling functions of move_object
     * deal very well with op being killed, so all this might do is just
     * migrate the problem someplace else.
     */

    if (QUERY_FLAG(op, FLAG_REMOVED)) {
	LOG(llevDebug,"move_object: monster has been removed - will not process further\n");
	/* Was not successful, but don't want to try and move again */
	return 1;
    }

    /* If this is a tail portion, just want to tell caller that move is
     * ok - the caller will deal with actual object removal/insertion
     */
    if(op->head)
	return 1;

    remove_ob(op);

    for(tmp = op; tmp != NULL; tmp = tmp->more)
	tmp->x+=freearr_x[dir], tmp->y+=freearr_y[dir];

    insert_ob_in_map(op, op->map, originator,0);

    /* Hmmm.  Should be possible for multispace players now */
    if (op->type==PLAYER) {
	esrv_map_scroll(&op->contr->socket, freearr_x[dir],freearr_y[dir]);
	op->contr->socket.update_look=1;
	op->contr->socket.look_position=0;
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
 * This is currently only used for the boulder roll code.
 */

int try_fit (object *op, mapstruct *m, int x, int y) 
{
    object *tmp, *more;
    sint16 tx, ty;
    int mflags;

    if (op->head) 
	op = op->head;

    for (more = op; more ; more = more->more) {
	tx = x + more->x - op->x;
	ty = y + more->y - op->y;

	mflags = get_map_flags(m, &m, tx, ty, &tx, &ty);

	if (mflags & P_OUT_OF_MAP)
	    return 1;

	for (tmp = get_map_ob (m, tx, ty); tmp; tmp=tmp->above) {
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
 * Support for rolling multipart objects is questionable.
 */

int roll_ob(object *op,int dir, object *pusher) {
    object *tmp;
    sint16 x, y;
    int flags;
    mapstruct *m;

    if (op->head) 
	op = op->head;

    x=op->x+freearr_x[dir];
    y=op->y+freearr_y[dir];

    if(!QUERY_FLAG(op,FLAG_CAN_ROLL) || 
       (op->weight &&
       random_roll(0, op->weight/50000-1, pusher, PREFER_LOW) > pusher->stats.Str))
	return 0;

    m = op->map;
    flags = get_map_flags(m, &m, x, y, &x, &y);

    if (flags & (P_OUT_OF_MAP | P_IS_ALIVE))
	return 0;

    

    /* If the target space is not blocked, no need to look at the objects on it */
    if (flags & P_BLOCKED) {
	for (tmp=get_map_ob(m, x, y); tmp!=NULL; tmp=tmp->above) {
	if (tmp->head == op)
	    continue;
	if (QUERY_FLAG(tmp,FLAG_NO_PASS) && !roll_ob(tmp,dir,pusher))
	    return 0;
	}
    }
    if (try_fit (op, m, x, y))
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

    /* Wake up sleeping monsters that may be pushed */
    CLEAR_FLAG(who,FLAG_SLEEP);
  
    /* player change place with his pets or summoned creature */
    /* TODO: allow multi arch pushing. Can't be very difficult */
    if (who->more == NULL && owner == pusher) {
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

	/* we presume that if the player is pushing his put, he moved in
	 * direction 'dir'.  I can' think of any case where this would not be
	 * the case.  Putting the map_scroll should also improve performance some.
	 */
	if (pusher->type == PLAYER ) {
	    esrv_map_scroll(&pusher->contr->socket, freearr_x[dir],freearr_y[dir]);
	    pusher->contr->socket.update_look=1;
	    pusher->contr->socket.look_position=0;
	}
	return 0;
    }


    /* We want ONLY become enemy of evil, unaggressive monster. We must RUN in them */
    /* In original we have here a unaggressive check only - that was the reason why */
    /* we so often become an enemy of friendly monsters... */
    /* funny: was they set to unaggressive 0 (= not so nice) they don't attack */

    if(owner != pusher &&  pusher->type == PLAYER && who->type != PLAYER &&
      !QUERY_FLAG(who,FLAG_FRIENDLY)&& !QUERY_FLAG(who,FLAG_NEUTRAL)) {
	if(pusher->contr->run_on) /* only when we run */ {
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

    /* now, lets test stand still. we NEVER can push stand_still monsters. */
    if(QUERY_FLAG(who,FLAG_STAND_STILL))
    {
	new_draw_info_format(NDI_UNIQUE, 0, pusher,
          "You can't push %s.",who->name);
	return 0;
    }
  
    /* This block is basically if you are pushing friendly but
     * non pet creaturs.
     * It basically does a random strength comparision to
     * determine if you can push someone around.  Note that
     * this pushes the other person away - its not a swap.
     */

    str1 = (who->stats.Str>0?who->stats.Str:who->level);
    str2 = (pusher->stats.Str>0?pusher->stats.Str:pusher->level);
    if(QUERY_FLAG(who,FLAG_WIZ) ||
       random_roll(str1, str1/2+str1*2, who, PREFER_HIGH) >= 
       random_roll(str2, str2/2+str2*2, pusher, PREFER_HIGH) ||
       !move_object(who,dir))
    {
	if (who ->type == PLAYER) {
	    new_draw_info_format(NDI_UNIQUE, 0, who,
		 "%s tried to push you.",pusher->name);
	}
	return 0;
    }

    /* If we get here, the push succeeded.  Let each now the
     * status.  I'm not sure if the second statement really needs
     * to be in an else block - the message is going to a different
     * player
     */
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
