/*
 * static char *rcsid_sock_info_c =
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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/**
 * \file
 * Basic client output functions.
 *
 * \date 2003-12-02
 *
 * This file implements some of the simpler output functions to the
 * client.  Basically, things like sending text strings along
 */

#include <global.h>
#include <sproto.h>
#include <stdarg.h>
#include <spells.h>
#include <skills.h>

/**
 * Draws a normal message on the client.  It is pretty 
 * much the same thing as the draw_info above, but takes a color
 * parameter.  the esrv_drawinfo functions should probably be
 * replaced with this, just using black as the color.
 */
static void esrv_print_msg(NewSocket *ns,int color, const char *str)
{
    char buf[HUGE_BUF];

    if (ns->status == Ns_Old) {
	sprintf(buf,"%s\n", str);
    } else {
	sprintf(buf,"drawinfo %d %s", color, str);
    }
/*    LOG(llevDebug,"sending %s to socket, len=%d", buf, strlen(buf));*/
    Write_String_To_Socket(ns, buf, strlen(buf));
}


/**
 * Frontend for esrv_print_msg
 * \param colr message color
 * \param pl player to send to. Can be NULL
 * \param tmp message to send. Can be NULL
 *
 * If pl is NULL or without contr set, writes message to log.
 *
 * Else sends message to player via esrv_print_msg
 */

static void print_message(int colr, object *pl,const char *tmp) {

  if(tmp == (char *) NULL) {
    tmp="[NULL]";
  }

  if(!pl || (pl->type==PLAYER && pl->contr==NULL)) {
      fprintf(logfile,"%s\n",tmp);
    return;
  }
  if (pl->type == PLAYER) {
    esrv_print_msg(&pl->contr->socket,colr,(char*) tmp);
    return;
  }
}


/**
 * Prints out the contents of specified buffer structures,
 * and clears the string.
 */

void flush_output_element(object *pl, Output_Buf *outputs)
{
    char tbuf[MAX_BUF];

    if (outputs->buf==NULL) return;
    sprintf(tbuf,"%d times %s", outputs->count, outputs->buf);
    print_message(NDI_BLACK, pl, tbuf);
    free_string(outputs->buf);
    outputs->buf=NULL;
    outputs->first_update=0;	/* This way, it will be reused */
}

/**
 * Sends message to player through output buffers.
 * \param pl player to send message
 * \param buf message to send
 *
 * If player's output_count is 1, sends message directly.
 *
 * Else checks output buffers for specified message.
 *
 * If found, checks if message should be sent now.
 *
 * If message not already in buffers, flushes olders buffer,
 * and adds message to queue.
 */

void check_output_buffers(object *pl, char *buf)
{
    int i, oldest=0;

    if (pl->contr->outputs_count<2) {
	print_message(NDI_BLACK, pl, buf);
	return;
    }
    else {
	for (i=0; i<NUM_OUTPUT_BUFS; i++) {
	    if (pl->contr->outputs[i].buf && 
		!strcmp(buf, pl->contr->outputs[i].buf)) break;
	    else if (pl->contr->outputs[i].first_update <
		pl->contr->outputs[oldest].first_update)
			oldest=i;
	}
	/* We found a match */
	if (i<NUM_OUTPUT_BUFS) {
	    pl->contr->outputs[i].count++;
	    if (pl->contr->outputs[i].count>=pl->contr->outputs_count) {
		flush_output_element(pl, &pl->contr->outputs[i]);
	    }
	}
	/* No match - flush the oldest, and put the new one in */
	else {
	    flush_output_element(pl, &pl->contr->outputs[oldest]);

	    pl->contr->outputs[oldest].first_update = pticks;
	    pl->contr->outputs[oldest].count = 1;
	    if (pl->contr->outputs[oldest].buf!=NULL)
		free_string(pl->contr->outputs[oldest].buf);
	    pl->contr->outputs[oldest].buf = add_string(buf);
	}
    }
}
	    


/**
 * Sends message to player(s).
 *
 * flags is various flags - mostly color, plus a few specials.
 *
 * pri is priority.  It is a little odd - the lower the value, the more
 * important it is.  Thus, 0 gets sent no matter what.  Otherwise, the
 * value must be less than the listening level that the player has set.
 * Unfortunately, there is no clear guideline on what each level does what.
 *
 * pl can be passed as NULL - in fact, this will be done if NDI_ALL is set
 * in the flags.
 *
 * If message is black, and not NDI_UNIQUE, gets sent through output buffers.
 *
 */

void new_draw_info(int flags,int pri, object *pl, const char *buf)
{

    if (flags & NDI_ALL) {
	player	*tmppl;
	int i;

	for (tmppl=first_player; tmppl!=NULL; tmppl=tmppl->next)
		new_draw_info((flags & ~NDI_ALL), pri, tmppl->ob, buf);

	for (i=1; i<socket_info.allocated_sockets; i++) {
	    if (init_sockets[i].status == Ns_Old && init_sockets[i].old_mode != Old_Listen && pri< 10) {
		cs_write_string(&init_sockets[i], buf, strlen(buf));
		/* Most messages don't have a newline, so add one */
		cs_write_string(&init_sockets[i], "\n", 1);
	    }
	}

	return;
    }
    if(!pl || (pl->type==PLAYER && pl->contr==NULL)) {
	/* Write to the socket? */
	print_message(0, NULL, buf);
	return;
    }
    if (pl->type!=PLAYER) return;
    if (pri>=pl->contr->listening) return;

    if ((flags&NDI_COLOR_MASK)==NDI_BLACK && !(flags &NDI_UNIQUE)) {
	/* following prints stuff out, as appropriate */
	check_output_buffers(pl, (char*)buf);
    }
    else {
	print_message(flags&NDI_COLOR_MASK, pl, buf);
    }
}

/**
 * Wrapper for new_draw_info printf-like.
 *
 * This is a pretty trivial function, but it allows us to use printf style
 * formatting, so instead of the calling function having to do it, we do
 * it here.  It may also have advantages in the future for reduction of
 * client/server bandwidth (client could keep track of various strings
 */

void new_draw_info_format(int flags, int pri,object *pl, char *format, ...)
{
    char buf[HUGE_BUF];

    va_list ap;
    va_start(ap, format);

    vsprintf(buf, format, ap);

    va_end(ap);

    new_draw_info(flags, pri, pl, buf);
}

/**
 * Writes to everyone on the map *except* op.  This is useful for emotions.
 */

void new_info_map_except(int color, mapstruct *map, object *op, char *str) {
    player *pl;

    for(pl = first_player; pl != NULL; pl = pl->next)
	if(pl->ob != NULL && pl->ob->map == map && pl->ob != op) {
	    new_draw_info(color, 0, pl->ob, str);
	}
}

/**
 * Writes to everyone on the map except op1 and op2
 */

void new_info_map_except2(int color, mapstruct *map, object *op1, object *op2,
			  char *str) {
    player *pl;

    for(pl = first_player; pl != NULL; pl = pl->next)
	if(pl->ob != NULL && pl->ob->map == map
	   && pl->ob != op1 && pl->ob != op2) {
	    new_draw_info(color, 0, pl->ob, str);
	}
}

/**
 * Writes to everyone on the specified map
 */

void new_info_map(int color, mapstruct *map, char *str) {
    player *pl;

    for(pl = first_player; pl != NULL; pl = pl->next)
	if(pl->ob != NULL && pl->ob->map == map) {
	    new_draw_info(color, 0, pl->ob, str);
	}
}


/**
 * This does nothing now.  However, in theory, we should probably send
 * something to the client and let the client figure out how it might want
 * to handle this
 */
void clear_win_info(object *op)
{
}

/**
 * Get player's current range attack in obuf.
 */
void rangetostring(object *pl,char *obuf)
{
    switch(pl->contr->shoottype) {
	case range_none:
	    strcpy(obuf,"Range: nothing");
	    break;

	case range_bow:
	    {
	    object *op;

	    for (op = pl->inv; op; op=op->below)
		if (op->type == BOW && QUERY_FLAG (op, FLAG_APPLIED))
		    break;
	    if(op==NULL) break;

	    sprintf (obuf, "Range: %s (%s)", query_base_name(op, 0), 
		     op->race ? op->race : "nothing");
	    }
	    break;

	case range_magic:
	    if (settings.casting_time == TRUE) {
		if (pl->casting_time > -1) {
		    if (pl->casting_time == 0)
			sprintf(obuf,"Range: Holding spell (%s)",
				pl->spell->name);
		    else
			sprintf(obuf,"Range: Casting spell (%s)",
				pl->spell->name);
		} else
		    sprintf(obuf,"Range: spell (%s)", 
			    pl->contr->ranges[range_magic]->name);
	    } else
		sprintf(obuf,"Range: spell (%s)", 
			pl->contr->ranges[range_magic]->name);
	    break;

	case range_misc:
	    sprintf(obuf,"Range: %s", 
		    pl->contr->ranges[range_misc]?
		    query_base_name(pl->contr->ranges[range_misc],0): "none");
	    break;

	/* range_scroll is only used for controlling golems.  If the
	 * the player does not have a golem, reset some things.
	 */
	case range_golem:
	    if (pl->contr->ranges[range_golem]!=NULL)
		sprintf(obuf,"Range: golem (%s)",pl->contr->ranges[range_golem]->name);
	    else {
		pl->contr->shoottype = range_none;
		strcpy(obuf,"Range: nothing");
	    }
	    break;

	case range_skill:
	    sprintf(obuf,"Skill: %s", pl->chosen_skill!=NULL ?
		    pl->chosen_skill->name : "none");
	    break;

	default:
	    strcpy(obuf,"Range: illegal");
    }
}

/**
 * Sets player title.
 */
void set_title(object *pl,char *buf)
{
    /* Eneq(@csd.uu.se): Let players define their own titles. */
    if (pl->contr->own_title[0]=='\0')
	sprintf(buf,"Player: %s the %s",pl->name,pl->contr->title);
    else
	sprintf(buf,"Player: %s the %s",pl->name,pl->contr->own_title);
}


/**
 * Helper for magic map creation.
 *
 * Takes a player, the map_mark array and an x and y starting position.
 * pl is the player.
 * px, py are offsets from the player.
 *
 * This function examines all the adjacant spaces next to px, py.
 * It updates the map_mark arrow with the color and high bits set
 * for various code values.
 */
static void magic_mapping_mark_recursive(object *pl, char *map_mark, int px, int py)
{
    int x, y, dx, dy,mflags;
    sint16 nx, ny;
    mapstruct *mp;
    New_Face *f;

    for (dx = -1; dx <= 1; dx++) {
	for (dy = -1; dy <= 1; dy++) {
	    x = px + dx;
	    y = py + dy;

	    if (FABS(x) >= MAGIC_MAP_HALF || FABS(y) >= MAGIC_MAP_HALF) continue;

	    mp = pl->map;
	    nx = pl->x + x;
	    ny = pl->y + y;

	    mflags = get_map_flags(pl->map, &mp, nx, ny, &nx, &ny);
	    if (mflags & P_OUT_OF_MAP) continue;

	    if (map_mark[MAGIC_MAP_HALF + x + MAGIC_MAP_SIZE* (MAGIC_MAP_HALF + y)] == 0) {
		f= GET_MAP_FACE(mp, nx, ny, 0);
		if (f == blank_face)
		    f= GET_MAP_FACE(mp, nx, ny, 1);
		if (f == blank_face)
		    f= GET_MAP_FACE(mp, nx, ny, 2);

		/* Should probably have P_NO_MAGIC here also, but then shops don't
		 * work.
		 */
		if (mflags & P_BLOCKSVIEW)
		    map_mark[MAGIC_MAP_HALF + x + MAGIC_MAP_SIZE* (MAGIC_MAP_HALF + y)] = FACE_WALL | (f?f->magicmap:0);
		else {
		    map_mark[MAGIC_MAP_HALF + x + MAGIC_MAP_SIZE* (MAGIC_MAP_HALF + y)] = FACE_FLOOR | (f?f->magicmap:0);
		    magic_mapping_mark_recursive(pl, map_mark, x, y);
		}
	    }
	}
    }
}


/**
 * Creates magic map for player.
 *
 * Note:  For improved magic mapping display, the space that blocks
 * the view is now marked with value 2.  Any dependencies of map_mark
 * being nonzero have been changed to check for 1.  Also, since
 * map_mark is a char value, putting 2 in should cause no problems.
 *
 * This function examines the map the player is on, and determines what
 * is visible.  2 is set for walls or objects that blocks view.  1
 * is for open spaces.  map_mark should already have been initialized
 * to zero before this is called.
 * strength is an initial strength*2 rectangular area that we automatically
 * see in/penetrate through.
 */

void magic_mapping_mark(object *pl, char *map_mark, int strength)
{
    int x, y, mflags;
    sint16 nx, ny;
    mapstruct *mp;
    New_Face *f;

    for (x = -strength; x <strength; x++) {
	for (y = -strength; y <strength; y++) {
	    mp = pl->map;
	    nx = pl->x + x;
	    ny = pl->y + y;
	    mflags = get_map_flags(pl->map, &mp, nx, ny, &nx, &ny);
	    if (mflags & P_OUT_OF_MAP)
		continue;
	    else {
		f= GET_MAP_FACE(mp, nx, ny, 0);
		if (f == blank_face)
		    f= GET_MAP_FACE(mp, nx, ny, 1);
		if (f == blank_face)
		    f= GET_MAP_FACE(mp, nx, ny, 2);
	    }

	    if (mflags & P_BLOCKSVIEW)
		map_mark[MAGIC_MAP_HALF + x + MAGIC_MAP_SIZE* (MAGIC_MAP_HALF + y)] = FACE_WALL | (f?f->magicmap:0);
	    else {
		map_mark[MAGIC_MAP_HALF + x + MAGIC_MAP_SIZE* (MAGIC_MAP_HALF + y)] = FACE_FLOOR | (f?f->magicmap:0);
		magic_mapping_mark_recursive(pl, map_mark, x, y);
	    }
	}
    }
}


/**
 * Creates and sends magic map to player.
 *
 * The following function is a lot messier than it really should be,
 * but there is no real easy solution.
 *
 * Mark Wedel
 */

void draw_magic_map(object *pl) 
{
    int x,y;
    char *map_mark = (char *) calloc(MAGIC_MAP_SIZE*MAGIC_MAP_SIZE, 1);
    int xmin, xmax, ymin, ymax;
    SockList sl;

    if (pl->type!=PLAYER) {
	LOG(llevError,"Non player object called draw_map.\n");
	return;
    }
   
    /* First, we figure out what spaces are 'reachable' by the player */
    magic_mapping_mark(pl, map_mark, 3);

    /* We now go through and figure out what spaces have been
     * marked, and thus figure out rectangular region we send
     * to the client (eg, if only a 10x10 area is visible, we only
     * want to send those 100 spaces.)
     */
    xmin = MAGIC_MAP_SIZE;
    ymin = MAGIC_MAP_SIZE;
    xmax = 0;
    ymax = 0;
    for(x = 0; x < MAGIC_MAP_SIZE ; x++) {
	for(y = 0; y < MAGIC_MAP_SIZE; y++) {
	    if (map_mark[x + MAP_WIDTH(pl->map) * y] | FACE_FLOOR) {
		xmin = x < xmin ? x : xmin;
		xmax = x > xmax ? x : xmax;
		ymin = y < ymin ? y : ymin;
		ymax = y > ymax ? y : ymax;
	    }
	}
    }

    sl.buf=malloc(MAXSOCKBUF);
    sprintf((char*)sl.buf,"magicmap %d %d %d %d ", (xmax-xmin+1), (ymax-ymin+1),
	    MAGIC_MAP_HALF - xmin, MAGIC_MAP_HALF - ymin);
    sl.len=strlen((char*)sl.buf);
    
    for (y = ymin; y <= ymax; y++) {
	for (x = xmin; x <= xmax; x++) {
	    sl.buf[sl.len++]= map_mark[x+MAGIC_MAP_SIZE*y] & ~FACE_FLOOR;
	} /* x loop */
    } /* y loop */
    
    Send_With_Handling(&pl->contr->socket, &sl);
    free(sl.buf);
    free(map_mark);
}


/**
 * Send a kill log record to sockets
 */

void Log_Kill(const char *Who,
            const char *What, int WhatType,
            const char *With, int WithType)
{
    int i;
    size_t len;
    char buf[MAX_BUF];

    if (With!=NULL) {
	sprintf(buf,"%s\t%s\t%d\t%s\t%d\n",Who,What,WhatType,With,WithType);
    }
    else {
	sprintf(buf,"%s\t%s\t%d\n",Who,What,WhatType);
    }
    len=strlen(buf);
    for(i=1; i<socket_info.allocated_sockets; i++) {
	if (init_sockets[i].old_mode == Old_Listen) {
	    cs_write_string(&init_sockets[i], buf, len);
	}
    }
}
