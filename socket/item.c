
/*
 * static char *rcsid_item_c =
 *    "$Id$";
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
*/

/* This containes item logic for client/server.  IT doesn't contain
 * the actual commands that send the data, but does contain
 * the logic for what items should be sent.
 */


#include <global.h>
#include <object.h>	/* LOOK_OBJ */
#include <newclient.h>
#include <newserver.h>
#include <sproto.h>

/* This is the maximum number of bytes we expect any one item to take up */
#define MAXITEMLEN  300

/*******************************************************************************
 *
 * Functions related to sending object data to the client.
 *
 ******************************************************************************/

/* This is more or less stolen from the query_weight function. */
#define WEIGHT(op) (op->nrof?op->weight:op->weight+op->carrying)

/* This is a simple function that we use a lot here.  It basically
 * adds the specified buffer into the socklist, but prepends a
 * single byte in length.  If the data is longer than that byte, it is
 * truncated approprately.
 */
inline void add_stringlen_to_sockbuf(char *buf, SockList *sl)
{
    int len;

    len=strlen(buf);
    if (len>255) len=255;
    SockList_AddChar(sl, len);
    strncpy((char*)sl->buf+sl->len, buf,len);
    sl->len += len;
}

/* 
 *  This is a similar to query_name, but returns flags
 *  to be sended to client. 
 */
unsigned int query_flags (object *op)
{
    unsigned int flags = 0;

    if(QUERY_FLAG(op,FLAG_APPLIED)) {
	switch(op->type) {
	  case BOW:
	  case WAND:
	  case ROD:
	  case HORN:
	    flags = a_readied;
	    break;
	  case WEAPON:
	    flags = a_wielded;
	    break;
	  case SKILL:
	  case ARMOUR:
	  case HELMET:
	  case SHIELD:
	  case RING:
	  case BOOTS:
	  case GLOVES:
	  case AMULET:
	  case GIRDLE:
	  case BRACERS:
	  case CLOAK:
	    flags = a_worn;
	    break;
	  case CONTAINER:
	    flags = a_active;
	    break;
	  default:
	    flags = a_applied;
	    break;
	}
    }
    if (op->type == CONTAINER && ((op->env && op->env->container == op) || 
	(!op->env && QUERY_FLAG(op,FLAG_APPLIED))))
	flags |= F_OPEN;
    
    if (QUERY_FLAG(op,FLAG_KNOWN_CURSED)) {
	if(QUERY_FLAG(op,FLAG_DAMNED))
	    flags |= F_DAMNED;
	else if(QUERY_FLAG(op,FLAG_CURSED))
	    flags |= F_CURSED;
    }
    if (QUERY_FLAG(op,FLAG_KNOWN_MAGICAL) && !QUERY_FLAG(op,FLAG_IDENTIFIED))
	flags |= F_MAGIC;
    if (QUERY_FLAG(op,FLAG_UNPAID))
	flags |= F_UNPAID;
    if (QUERY_FLAG(op,FLAG_INV_LOCKED))
	flags |= F_LOCKED;

    return flags;
}

/* draw the look window.  Don't need to do animations here 
 * This sends all the faces to the client, not just updates.  This is
 * because object ordering would otherwise be inconsistent
 */

void esrv_draw_look(object *pl)
{
    object *tmp, *last;
    int flags, got_one=0,anim_speed, start_look=0, end_look=0;
    SockList sl;
    char buf[MAX_BUF];

    if (!pl->contr->socket.update_look) {
	LOG(llevDebug,"esrv_draw_look called when update_look was not set\n");
	return;
    } else {
	pl->contr->socket.update_look=0;
    }

    if(QUERY_FLAG(pl, FLAG_REMOVED) || pl->map == NULL ||
       pl->map->in_memory != MAP_IN_MEMORY || out_of_map(pl->map,pl->x,pl->y))
	    return;

    for (tmp=get_map_ob(pl->map,pl->x,pl->y); tmp && tmp->above;tmp=tmp->above)
	; 

    sl.buf=malloc(MAXSOCKBUF);

    Write_String_To_Socket(&pl->contr->socket, "delinv 0", strlen("delinv 0"));
    strcpy((char*)sl.buf,"item1 ");
    sl.len=strlen((char*)sl.buf);

    SockList_AddInt(&sl, 0);

    if (!pl->contr->socket.faces_sent[empty_face->number])
	esrv_send_face(&pl->contr->socket, empty_face->number,0);

    if (pl->contr->socket.look_position) {
	SockList_AddInt(&sl, 0x80000000 | (pl->contr->socket.look_position- NUM_LOOK_OBJECTS));
	SockList_AddInt(&sl, 0);
	SockList_AddInt(&sl, -1);
	SockList_AddInt(&sl, empty_face->number);
	sprintf(buf,"Click here to see %d previous items", NUM_LOOK_OBJECTS);
	add_stringlen_to_sockbuf(buf, &sl);
	SockList_AddShort(&sl,0);
	SockList_AddChar(&sl, 0);
	SockList_AddInt(&sl, 0);
    }

    for (last=NULL; tmp!=last; tmp=tmp->below) {
	if (QUERY_FLAG(tmp, FLAG_IS_FLOOR) && !last) {
	    last = tmp->below;  /* assumes double floor mode */
	    if (last && QUERY_FLAG(last, FLAG_IS_FLOOR))
		last = last->below;
	}
	if (LOOK_OBJ(tmp)) {
	    if (++start_look < pl->contr->socket.look_position) continue;
	    end_look++;
	    if (end_look > NUM_LOOK_OBJECTS) {
		SockList_AddInt(&sl, 0x80000000 | (pl->contr->socket.look_position+ NUM_LOOK_OBJECTS));
		SockList_AddInt(&sl, 0);
		SockList_AddInt(&sl, -1);
		SockList_AddInt(&sl, empty_face->number);
		sprintf(buf,"Click here to see next group of items");
		add_stringlen_to_sockbuf(buf, &sl);
		SockList_AddShort(&sl,0);
		SockList_AddChar(&sl, 0);
		SockList_AddInt(&sl, 0);
		break;
	    }

	    flags = query_flags (tmp);
	    if (QUERY_FLAG(tmp, FLAG_NO_PICK))
		flags |=  F_NOPICK;
	    if (!pl->contr->socket.faces_sent[tmp->face->number])
		esrv_send_face(&pl->contr->socket, tmp->face->number,0);

	    if (QUERY_FLAG(tmp,FLAG_ANIMATE) && 
			   !pl->contr->socket.anims_sent[tmp->animation_id])
		esrv_send_animation(&pl->contr->socket, tmp->animation_id);

	    SockList_AddInt(&sl, tmp->count);
	    SockList_AddInt(&sl, flags);
	    SockList_AddInt(&sl, QUERY_FLAG(tmp, FLAG_NO_PICK) ? -1 : WEIGHT(tmp));
	    SockList_AddInt(&sl, tmp->face->number);
	    if (pl->contr->socket.sc_version>=1024) {
		int len;
		char *item_p,item_n[MAX_BUF];

		strncpy(item_n,query_base_name(tmp, 0),127);
		item_n[127]=0;
		len=strlen(item_n);
		item_p=query_base_name(tmp, 1);
		strncpy(item_n+len+1, item_p, 127);
		item_n[254]=0;
		len += strlen(item_n+1+len) + 1;
		SockList_AddChar(&sl, len);
		memcpy(sl.buf+sl.len, item_n, len);
		sl.len += len;
	    } else
		add_stringlen_to_sockbuf(query_base_name(tmp,0), &sl);

	    SockList_AddShort(&sl,tmp->animation_id);
	    anim_speed=0;
	    if (QUERY_FLAG(tmp,FLAG_ANIMATE)) {
		if (tmp->anim_speed) anim_speed=tmp->anim_speed;
		else {
		    if (FABS(tmp->speed)<0.001) anim_speed=255;
		    else if (FABS(tmp->speed)>=1.0) anim_speed=1;
		    else anim_speed = (1.0/FABS(tmp->speed));
		}
		if (anim_speed>255) anim_speed=255;
	    }
	    SockList_AddChar(&sl, anim_speed);
	    SockList_AddInt(&sl, tmp->nrof);
	    SET_FLAG(tmp, FLAG_CLIENT_SENT);
	    got_one++;

	    if (sl.len > (MAXSOCKBUF-MAXITEMLEN)) {
		Send_With_Handling(&pl->contr->socket, &sl);
		strcpy((char*)sl.buf,"item1 ");
		sl.len=strlen((char*)sl.buf);
		SockList_AddInt(&sl, 0);
		got_one=0;
	    }
	}
    }
    if (got_one)
	Send_With_Handling(&pl->contr->socket, &sl);

    free(sl.buf);
}

void esrv_send_inventory(object *pl, object *op)
{
    object *tmp;
    int flags, got_one=0, anim_speed;
    SockList sl;
    char item_n[MAX_BUF];
    
    sl.buf=malloc(MAXSOCKBUF);

    sprintf((char*)sl.buf,"delinv %d", op->count);
    sl.len=strlen((char*)sl.buf);
    Send_With_Handling(&pl->contr->socket, &sl);

    strcpy((char*)sl.buf,"item1 ");
    sl.len=strlen((char*)sl.buf);

    SockList_AddInt(&sl, op->count);
    
    for (tmp=op->inv; tmp; tmp=tmp->below) {
	if (LOOK_OBJ(tmp)) {
	    flags = query_flags (tmp);
	    if (QUERY_FLAG(tmp, FLAG_NO_PICK))
		flags |=  F_NOPICK;
	    if (!pl->contr->socket.faces_sent[tmp->face->number])
		esrv_send_face(&pl->contr->socket, tmp->face->number,0);
	    if (QUERY_FLAG(tmp,FLAG_ANIMATE) && 
			   !pl->contr->socket.anims_sent[tmp->animation_id])
		esrv_send_animation(&pl->contr->socket, tmp->animation_id);
	    SockList_AddInt(&sl, tmp->count);
	    SockList_AddInt(&sl, flags);
	    SockList_AddInt(&sl, QUERY_FLAG(tmp, FLAG_NO_PICK) ? -1 : WEIGHT(tmp));
	    SockList_AddInt(&sl, tmp->face->number);

	    if (pl->contr->socket.sc_version>=1024) {
		int len;
		char *item_p;

		strncpy(item_n,query_base_name(tmp, 0),127);
		item_n[127]=0;
		len=strlen(item_n);
		item_p=query_base_name(tmp, 1);
		strncpy(item_n+len+1, item_p, 127);
		item_n[254]=0;
		len += strlen(item_n+1+len) + 1;
		SockList_AddChar(&sl, len);
		memcpy(sl.buf+sl.len, item_n, len);
		sl.len += len;
	    } else
		add_stringlen_to_sockbuf(query_base_name(tmp,0), &sl);

	    SockList_AddShort(&sl,tmp->animation_id);
	    anim_speed=0;
	    if (QUERY_FLAG(tmp,FLAG_ANIMATE)) {
		if (tmp->anim_speed) anim_speed=tmp->anim_speed;
		else {
		    if (FABS(tmp->speed)<0.001) anim_speed=255;
		    else if (FABS(tmp->speed)>=1.0) anim_speed=1;
		    else anim_speed = (1.0/FABS(tmp->speed));
		}
		if (anim_speed>255) anim_speed=255;
	    }
	    SockList_AddChar(&sl, anim_speed);
	    SockList_AddInt(&sl, tmp->nrof);
	    SET_FLAG(tmp, FLAG_CLIENT_SENT);
	    got_one++;

	    /* IT is possible for players to accumulate a huge amount of
	     * items (especially with some of the bags out there) to
	     * overflow the buffer.  IF so, send multiple item1 commands.
	     */
	    if (sl.len > (MAXSOCKBUF-MAXITEMLEN)) {
		Send_With_Handling(&pl->contr->socket, &sl);
		strcpy((char*)sl.buf,"item1 ");
		sl.len=strlen((char*)sl.buf);
		SockList_AddInt(&sl, op->count);
		got_one=0;
	    }
	}
    }
    if (got_one)
	Send_With_Handling(&pl->contr->socket, &sl);
    free(sl.buf);
}

/* Updates object *op for player *pl.  flags is a list of values to update
 * to the client (as defined in newclient.h - might as well use the
 * same value both places.
 */

void esrv_update_item(int flags, object *pl, object *op)
{
    SockList sl;

    /* If we have a request to send the player item, skip a few checks. */
    if (op!=pl) {
	if (! LOOK_OBJ(op)) 
	    return;
	/* we remove the check for op->env, because in theory, the object
	 * is hopefully in the same place, so the client should preserve
	 * order.
	 */
    }
    if (!QUERY_FLAG(op, FLAG_CLIENT_SENT)) {
	/* FLAG_CLIENT_SENT is debug only.  We are using it to see where
	 * this is happening - we can set a breakpoint here in the debugger
	 * and track back the call.
	 */
	LOG(llevDebug,"We have not sent item %s (%d)\n", op->name, op->count);
    }
    sl.buf=malloc(MAXSOCKBUF);

    strcpy((char*)sl.buf,"upditem ");
    sl.len=strlen((char*)sl.buf);

    SockList_AddChar(&sl, flags);
    SockList_AddInt(&sl, op->count);

    if (flags & UPD_LOCATION)
	SockList_AddInt(&sl, op->env? op->env->count:0);

    if (flags & UPD_FLAGS)
	SockList_AddInt(&sl, query_flags(op));

    if (flags & UPD_WEIGHT)
	SockList_AddInt(&sl, WEIGHT(op));

    if (flags & UPD_FACE) {
	if (!pl->contr->socket.faces_sent[op->face->number])
	    esrv_send_face(&pl->contr->socket, op->face->number,0);
	SockList_AddInt(&sl, op->face->number);
    }
    if (flags & UPD_NAME) {
	if (pl->contr->socket.sc_version>=1024) {
	    int len;
	    char *item_p, item_n[MAX_BUF];

	    strncpy(item_n,query_base_name(op, 0),127);
	    item_n[127]=0;
	    len=strlen(item_n);
	    item_p=query_base_name(op, 1);
	    strncpy(item_n+len+1, item_p, 127);
	    item_n[254]=0;
	    len += strlen(item_n+1+len) + 1;
	    SockList_AddChar(&sl, len);
	    memcpy(sl.buf+sl.len, item_n, len);
	    sl.len += len;
	} else
	    add_stringlen_to_sockbuf(query_base_name(op,0), &sl);
    }
    if (flags & UPD_ANIM) 
	    SockList_AddShort(&sl,op->animation_id);

    if (flags & UPD_ANIMSPEED) {
	int anim_speed=0;
	if (QUERY_FLAG(op,FLAG_ANIMATE)) {
	    if (op->anim_speed) anim_speed=op->anim_speed;
	    else {
		if (FABS(op->speed)<0.001) anim_speed=255;
		else if (FABS(op->speed)>=1.0) anim_speed=1;
		else anim_speed = (1.0/FABS(op->speed));
	    }
	    if (anim_speed>255) anim_speed=255;
	}
	SockList_AddChar(&sl, anim_speed);
    }
    if (flags & UPD_NROF)
	    SockList_AddInt(&sl, op->nrof);

    Send_With_Handling(&pl->contr->socket, &sl);
    free(sl.buf);
}

void esrv_send_item(object *pl, object*op)
{
    int anim_speed;
    SockList sl;
    char item_n[MAX_BUF];
    
    /* If this is not the player object, do some more checks */
    if (op!=pl) {
	/* We only send 'visibile' objects to the client */
	if (! LOOK_OBJ(op)) 
	    return;
	/* if the item is on the ground, mark that the look needs to
	 * be updated.
	 */
	if (!op->env) {
	    pl->contr->socket.update_look=1;
	    return;
	}
    }

    sl.buf=malloc(MAXSOCKBUF);

    strcpy((char*)sl.buf,"item1 ");
    sl.len=strlen((char*)sl.buf);

    SockList_AddInt(&sl, op->env? op->env->count:0);

    if (!pl->contr->socket.faces_sent[op->face->number])
	esrv_send_face(&pl->contr->socket, op->face->number,0);
    if (op->env && QUERY_FLAG(op,FLAG_ANIMATE) &&
	   !pl->contr->socket.anims_sent[op->animation_id])
	esrv_send_animation(&pl->contr->socket, op->animation_id);

    SockList_AddInt(&sl, op->count);
    SockList_AddInt(&sl, query_flags(op));
    SockList_AddInt(&sl, WEIGHT(op));
    SockList_AddInt(&sl, op->face->number);

    if (pl->contr->socket.sc_version>=1024) {
	int len;
	char *item_p;

	strncpy(item_n,query_base_name(op, 0),127);
	item_n[127]=0;
	len=strlen(item_n);
	item_p=query_base_name(op, 1);
	strncpy(item_n+len+1, item_p, 127);
	item_n[254]=0;
	len += strlen(item_n+1+len) + 1;
	SockList_AddChar(&sl, len);
	memcpy(sl.buf+sl.len, item_n, len);
	sl.len += len;
    } else
	add_stringlen_to_sockbuf(query_base_name(op,0), &sl);

    SockList_AddShort(&sl,op->animation_id);
    anim_speed=0;
    if (QUERY_FLAG(op,FLAG_ANIMATE)) {
	if (op->anim_speed) anim_speed=op->anim_speed;
        else {
	    if (FABS(op->speed)<0.001) anim_speed=255;
            else if (FABS(op->speed)>=1.0) anim_speed=1;
            else anim_speed = (1.0/FABS(op->speed));
	}
        if (anim_speed>255) anim_speed=255;
    }
    SockList_AddChar(&sl, anim_speed);
    SockList_AddInt(&sl, op->nrof);
    Send_With_Handling(&pl->contr->socket, &sl);
    SET_FLAG(op, FLAG_CLIENT_SENT);
    free(sl.buf);
}

/* Tells the client to delete an item.  Uses the item
 * command with a -1 location.
 */

void esrv_del_item(player *pl, int tag)
{
    SockList sl;

    sl.buf=malloc(MAXSOCKBUF);

    strcpy((char*)sl.buf,"delitem ");
    sl.len=strlen((char*)sl.buf);
    SockList_AddInt(&sl, tag);

    Send_With_Handling(&pl->socket, &sl);
    free(sl.buf);
}


/*******************************************************************************
 *
 * Client has requested us to do something with an object.
 *
 ******************************************************************************/

/* Takes a player and object count (tag) and returns the actual object
 * pointer, or null if it can't be found.
 */

object *esrv_get_ob_from_count(object *pl, long count)
{
    object *op, *tmp;

    if (pl->count == count)
	return pl;

    for(op = pl->inv; op; op = op->below)
	if (op->count == count)
	    return op;
	else if (op->type == CONTAINER && pl->container == op)
	    for(tmp = op->inv; tmp; tmp = tmp->below)
		if (tmp->count == count)
		    return tmp;

    for(op = get_map_ob (pl->map, pl->x, pl->y); op; op = op->above)
	if (op->count == count)
	    return op;
	else if (op->type == CONTAINER && pl->container == op)
	    for(tmp = op->inv; tmp; tmp = tmp->below)
		if (tmp->count == count)
		    return tmp;
    return NULL;
}


/* Client wants to examine some object.  So lets do so. */
void ExamineCmd(char *buf, int len,player *pl)
{
    long tag = atoi(buf);
    object *op = esrv_get_ob_from_count(pl->ob, tag);

    if (!op) {
	LOG(llevDebug, "Player '%s' tried examine the unknown object (%d)\n",
	    pl->ob->name, tag);
	return;
    }
    examine (pl->ob, op);
}

/* Client wants to apply some object.  Lets do so. */
void ApplyCmd(char *buf, int len,player *pl)
{
    uint32 tag = atoi(buf);
    object *op = esrv_get_ob_from_count(pl->ob, tag);

    /* sort of a hack, but if the player saves and the player then manually
     * applies a savebed (or otherwise tries to do stuff), we run into trouble.
     */
    if (QUERY_FLAG(pl->ob, FLAG_REMOVED)) return;

    /* If the high bit is set, player applied a pseudo object. */
    if (tag & 0x80000000) {
	pl->socket.look_position = tag & 0x7fffffff;
	pl->socket.update_look = 1;
	return;
    }

    if (!op) {
	LOG(llevDebug, "Player '%s' tried apply the unknown object (%d)\n",
	  pl->ob->name, tag);
	return;
    }
    player_apply (pl->ob, op, 0, 0);
}

/* Client wants to apply some object.  Lets do so. */
void LockItem(uint8 *data, int len,player *pl)
{
    int flag, tag;
    object *op;

    flag = data[0];
    tag = GetInt_String(data+1);
    op = esrv_get_ob_from_count(pl->ob, tag);

    if (!op) {
	new_draw_info(NDI_UNIQUE, 0, pl->ob,"Could not find object to lock/unlock");
	return;
    }
    if (!flag)
	CLEAR_FLAG(op,FLAG_INV_LOCKED);
    else
	SET_FLAG(op,FLAG_INV_LOCKED);
    esrv_update_item(UPD_FLAGS, pl->ob, op);
}

/* Client wants to apply some object.  Lets do so. */
void MarkItem(uint8 *data, int len,player *pl)
{
    int tag;
    object *op;

    tag = GetInt_String(data);
    op = esrv_get_ob_from_count(pl->ob, tag);
    if (!op) {
	new_draw_info(NDI_UNIQUE, 0, pl->ob,"Could not find object to mark");
	return;
    }
    pl->mark = op;
    pl->mark_count = op->count;
    new_draw_info_format(NDI_UNIQUE, 0, pl->ob, "Marked item %s", query_name(op));
}


/*
 * look_at prints items on the specifc square.
 * [ removed EARTHWALL check and added check for containers inventory.
 *   Tero.Haatanen@lut.fi ]
 */
void look_at(object *op,int dx,int dy) {
    object *tmp;
    int flag=0,x,y;
    mapstruct *m;


    if (out_of_map(op->map, op->x+dx, op->y+dy)) return;

    x = op->x + dx;
    y = op->y + dy;

    m = get_map_from_coord(op->map, &x, &y);
    if (!m) return;

    for(tmp=get_map_ob(m, x ,y);tmp!=NULL&&tmp->above!=NULL;
	    tmp=tmp->above);

    for ( ; tmp != NULL; tmp=tmp->below ) {
	 if (tmp->invisible && !QUERY_FLAG(op, FLAG_WIZ)) continue;

	 if(!flag) {
	    if(dx||dy)
		new_draw_info(NDI_UNIQUE, 0,op,"There you see:");
	    else {
		clear_win_info(op);
		new_draw_info(NDI_UNIQUE, 0,op,"You see:");
	    }
	    flag=1;
	 }

	 if (QUERY_FLAG(op, FLAG_WIZ))
	    new_draw_info_format(NDI_UNIQUE,0, op, "- %s (%d).",query_name(tmp),tmp->count);
	 else
	    new_draw_info_format(NDI_UNIQUE,0, op, "- %s.",query_name(tmp));

	 if((tmp->inv!=NULL || (tmp->head && tmp->head->inv)) && 
	    ( (!dx&&!dy) || tmp->type != CONTAINER || QUERY_FLAG(op, FLAG_WIZ)
	     || !(tmp->type) || tmp->type!=FLESH ))
	    inventory(op,tmp->head==NULL?tmp:tmp->head);

	 if(QUERY_FLAG(tmp, FLAG_IS_FLOOR)&&!QUERY_FLAG(op, FLAG_WIZ))	/* don't continue under the floor */
	    break;
    }

    if(!flag) {
	if(dx||dy)
	    new_draw_info(NDI_UNIQUE, 0,op,"You see nothing there.");
	else
	    new_draw_info(NDI_UNIQUE, 0,op,"You see nothing.");
    }
}



/* Client wants to apply some object.  Lets do so. */
void LookAt(char *buf, int len,player *pl)
{
    int dx, dy;
    char *cp;

    dx=atoi(buf);
    if (!(cp=strchr(buf,' '))) {
	return;
    }
    dy=atoi(cp);

    if (FABS(dx)>MAP_CLIENT_X/2 || FABS(dy)>MAP_CLIENT_Y/2)
          return;

    if(pl->blocked_los[dx+MAP_CLIENT_X/2][dy+MAP_CLIENT_Y/2])
          return;
    look_at(pl->ob, dx, dy);
}

/* Move an object to a new lcoation */

void esrv_move_object (object *pl, long to, long tag, long nrof)
{
    object *op, *env;

    op = esrv_get_ob_from_count(pl, tag);
    if (!op) {
	LOG(llevDebug, "Player '%s' tried to move the unknown object (%ld)\n",
	    pl->name, tag);
	return;
    }

    if (!to) {	/* drop it to the ground */
/*	printf ("Drop it on the ground.\n");*/

	if (op->map && !op->env) {
/*	    LOG(llevDebug,"Dropping object to ground that is already on ground\n");*/
	    return;
	}
	/* If it is an active container, then we should drop all objects
	 * in the container and not the container itself.
	 */
	if (op->inv && QUERY_FLAG(op, FLAG_APPLIED)) {
	    object *current, *next;
	    for (current=op->inv; current!=NULL; current=next) {
		next=current->below;
		drop_object(pl, current, 0);
	    }
	    esrv_update_item(UPD_WEIGHT, pl, op);
	}
	else {
	    drop_object (pl, op, nrof);
	}
	return;
    } else if (to == pl->count) {     /* pick it up to the inventory */
	/* return if player has already picked it up */
	if (op->env == pl) return;

	pl->contr->count = nrof;
	pick_up(pl, op);
	return ;
    }
    /* If not dropped or picked up, we are putting it into a sack */
    env = esrv_get_ob_from_count(pl, to);
    if (!env) {
      LOG(llevDebug, 
	  "Player '%s' tried to move object to the unknown location (%d)\n",
	  pl->name, to);
      return;
    }
#if 0
    printf ("Sacks name was '%s'.\n", env->name);
#endif
    put_object_in_sack (pl, env, op, nrof);
}


