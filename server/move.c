/*
 * static char *rcsid_move_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
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

    The author can be reached via e-mail to master@rahul.net
*/

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

int move_ob (object *op, int dir, object *originator)
{
  object *tmp=NULL;
  object *tmp_ob=NULL;

  if(op==NULL) {
    LOG(llevError,"Trying to move NULL.\n");
    return 0;
  }

  /* If this is a multipart object and we are dealing with the
   * head, remove the object.  It needs to be removed, otherwise
   * it may block itself from moving to that space (You can't
   * move onto the tail section of the monster, and this is true
   * even if it is your own tail
   */
  if(op->more!=NULL && op->head==NULL)
    remove_ob(op);

/* Vick's (vick@bern.docs.uu.se) @921108 -> If a monster can apply
 * an earthwall (or a door), then walking into it means tearing it down.
 * Note taht a player should never have will_apply set, so thus
 * the following two sections will not be used for players -
 * the routines in move_player_attack will be used instead.
 */

  if (op->will_apply&4 &&
      !out_of_map(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir])) {

    tmp_ob=get_map_ob(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]);

    if (tmp_ob!=NULL) {
      while(tmp_ob->above != NULL)
        tmp_ob=tmp_ob->above;

      if (tmp_ob->type == EARTHWALL)
	hit_player(tmp_ob,5,op,AT_PHYSICAL); /* Tear down the earthwall */
    }
  }
  if (op->will_apply&8 &&
      !out_of_map(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir])) {

    tmp_ob=get_map_ob(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]);
    if (tmp_ob!=NULL) {
      while(tmp_ob->above != NULL)
        tmp_ob=tmp_ob->above;
      if (tmp_ob->type == DOOR)
        hit_player(tmp_ob,9999,op,AT_PHYSICAL); /* Break open the door. */
    }
  }

  /* Eneq(@csd.uu.se): Use blocked_two when selecting direction.
   * The following if statement works like this:  If WIZPASS is
   * set, then the player is prevented from moving to that
   * space if it is out_of_map.  If non WIZPASS, then
   * blocked_two is called instead.
   * This code section is for cases when an object CAN NOT move into
   * that space.
   */

  if(QUERY_FLAG(op,FLAG_WIZPASS)?out_of_map(op->map,op->x+freearr_x[dir],
    op->y+freearr_y[dir]):
    blocked_two(op,op->x+freearr_x[dir],op->y+freearr_y[dir])) {

    /* Re insert object if we removed it above */
    if(op->more!=NULL && op->head==NULL)
      insert_ob_in_map(op,op->map,originator);
    return 0;
  }

  /* Following deals with situation when the object can move
   * into the desired space.
   */

  /* this is a single parted object that needs to be removed. */
  if(op->more==NULL && op->head==NULL)
    remove_ob(op);

  /* This handles multipart objects.  It uses recursion to
   * make sure all the parts will fit in the new space.
   * First, tmp is set the the next segment (op->more), with
   * that being set to null.  Then call move_ob to see if
   * that segment (and also all segments beyond it) can
   * move to taht space.  IF not (!move_ob), then reset op->more.
   * If it is the head segment, insert back to map.  Clear freeze_look
   * if player, and return failed result.
   */
  if(op->more!=NULL) {
    tmp=op->more,op->more=NULL;
    if(!move_ob(tmp,dir,originator)) {
      op->more=tmp;
      if(op->head==NULL)
        insert_ob_in_map(op,op->map,originator);
      return 0;
    }
    else op->more=tmp;
  }
  op->x+=freearr_x[dir],op->y+=freearr_y[dir];  /* Move this object */

  /* If head of object (or single part object), reinsert into map. */
  if(op->head==NULL)
    insert_ob_in_map(op,op->map,originator);

  if (op->type==PLAYER) {
    esrv_map_scroll(&op->contr->socket, freearr_x[dir],freearr_y[dir]);
    op->contr->socket.update_look=1;
  }
  return 1;
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
  return insert_ob_in_map(op,op->map,originator) == NULL;
}

/*
 * Return value: 1 if object was destroyed, 0 otherwise.
 */
int teleport (object *teleporter, unsigned char tele_type, object *originator)
{
  object *altern[120]; /* Better use c/malloc here in the future */
  int i,j,k,nrofalt=0;
  object *other_teleporter,*teleported=teleporter->above,*tmp;

  if(teleported==NULL) return 0;
  if(teleported->head!=NULL)
    teleported=teleported->head;
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
      if (!other_teleporter) continue;
      altern[nrofalt++]=other_teleporter;
    }
  if(!nrofalt) {
    LOG(llevError,"No alternative teleporters around!\n");
    return 0;
  }
  other_teleporter=altern[RANDOM()%nrofalt];
  k=find_free_spot(teleported->arch,other_teleporter->map,
                        other_teleporter->x,other_teleporter->y,1,9);
  if (k==-1)
    return 0;
  remove_ob(teleported);
  for(tmp=teleported;tmp!=NULL;tmp=tmp->more)
    tmp->x=other_teleporter->x+freearr_x[k]+
           (tmp->arch==NULL?0:tmp->arch->clone.x),
    tmp->y=other_teleporter->y+freearr_y[k]+
           (tmp->arch==NULL?0:tmp->arch->clone.y);
  return insert_ob_in_map(teleported,other_teleporter->map,originator) == NULL;
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
       (op->weight&&RANDOM() % (op->weight/50000) > pusher->stats.Str))
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
    insert_ob_in_map(op,op->map,pusher);
    return 1;
}

int push_ob(object *who, int dir, object *pusher) {
  int str1, str2;
  object *owner;

  if (who->head != NULL)
    who = who->head;
  owner = get_owner(who);
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
    insert_ob_in_map (who,who->map,pusher);
    insert_ob_in_map (pusher,pusher->map,pusher);
    return 0;
  }
  if(QUERY_FLAG(who,FLAG_UNAGGRESSIVE) && owner != pusher)
    who->enemy = pusher;
  str1 = (who->stats.Str>0?who->stats.Str:who->level);
  str2 = (pusher->stats.Str>0?pusher->stats.Str:pusher->level);
  if(QUERY_FLAG(who,FLAG_WIZ) || RANDOM()%(str1/2+1) + str1 >= RANDOM()%(str2/2+1) + str2 ||
     !move_ob(who,dir,pusher))
  {
    if (who ->type == PLAYER) {
      new_draw_info_format(NDI_UNIQUE, 0, who,
	"%s tried to push you.",pusher->name);
    }
    if (QUERY_FLAG(who, FLAG_MONSTER)) {
      new_draw_info_format(NDI_UNIQUE, 0, pusher,
	"Your pushing annoys %s.",who->name);
    }
    return 0;
  }
  if (who->type == PLAYER) {
    new_draw_info_format(NDI_UNIQUE, 0, who,
	"%s pushed you.",pusher->name);
  }
  if (QUERY_FLAG(who, FLAG_MONSTER)) {
    if(owner == pusher)
      new_draw_info_format(NDI_UNIQUE, 0,pusher,
	"%s doesn't seem pleased with your action.",who->name);
    else
      new_draw_info_format(NDI_UNIQUE, 0,pusher,
	"%s is definitely annoyed at your pushing.",who->name);
  }
  return 1;
}
