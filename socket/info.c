/*
 * static char *rcsid_socki0_c =
 *   "$Id$";
 */

/* This file implements some of the simpler output functions to the
 * client.  Basically, things like sending text strings along
 */

#include <global.h>
#include <sproto.h>
#include <stdarg.h>
#include <spells.h>
#include <skills.h>

/*
 * esrv_print_msg draws a normal message on the client.  It is pretty 
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

/****************************************************************************
print_message : This routine prints out the character string in tmp on the
                info window. If in color mode, then the text will show up in
                a color specified by the variable colr. If Black and white
                mode, then it prints a line of "="s before and after the
                messsage.

		This has been changed around - this is now a front end to
		draw_info.  draw_info should never be called directly, except
		by this functin.  Likewise, this function should only be
		called by a few functions above (new_draw_info, 
		check_output_buffers)
****************************************************************************/

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


/* Following prints out the contents of one of the buffer structures,
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

/* Following checks the various buffers in the player structure and
 * other things, and stores/prints/whatever's the data, as appropriate.
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
	    


/*
 * new_draw_info:
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
 */


void new_draw_info(int flags,int pri, object *pl, const char *buf)
{

    if (flags & NDI_ALL) {
	player	*tmppl;
	int i;

	for (tmppl=first_player; tmppl!=NULL; tmppl=tmppl->next)
		new_draw_info((flags & ~NDI_ALL), pri, tmppl->ob, buf);

	for (i=1; i<socket_info.allocated_sockets; i++) {
	    if (init_sockets[i].status == Ns_Old && init_sockets[i].old_mode != Old_Listen) {
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

/* This is a pretty trivial function, but it allows us to use printf style
 * formatting, so instead of the calling function having to do it, we do
 * it here.  IT may also have advantages in the future for reduction of
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

void new_info_map(int color, mapstruct *map, char *str) {
    player *pl;

    for(pl = first_player; pl != NULL; pl = pl->next)
	if(pl->ob != NULL && pl->ob->map == map) {
	    new_draw_info(color, 0, pl->ob, str);
	}
}

void draw(object *pl) {

    if(pl->map == NULL || pl->map->in_memory != MAP_IN_MEMORY)
	return;

    if(pl->contr->do_los) {
	update_los(pl);
#ifdef USE_LIGHTING
	pl->map->do_los = 0;
#endif
	pl->contr->do_los = 0;
    }

    draw_client_map(pl);
}


/* This does nothing now.  However, in theory, we should probably send
 * something to the client and let the client figure out how it might want
 * to handle this
 */
void clear_win_info(object *op)
{
}

void rangetostring(object *pl,char *obuf)
{
  int chosen_spell;

  chosen_spell = (pl->contr->shoottype==range_magic)? pl->contr->chosen_spell :
      pl->contr->chosen_item_spell;
  switch(pl->contr->shoottype) {
   case range_none:
    strcpy(obuf,"Range: nothing");
    break;
   case range_bow: {
     char *s;
     object *op;
     for (op = pl->inv; op; op=op->below)
       if (op->type == BOW && QUERY_FLAG (op, FLAG_APPLIED))
       break;
     if(op==NULL) break;
     s = query_name(op);
#if 1                         /* Hack to remove (readied) from a bow description */
     if (strcmp (s + strlen (s) - 10, " (readied)") == 0)
       s[strlen (s) - 10] = 0;
#endif
     sprintf (obuf, "Range: %s (%s)", s, 
            op && op->race ? op->race : "nothing");
   }
    break;
   case range_magic:
#ifdef CASTING_TIME
    if (pl->casting > -1) {
      if (pl->casting == 0)
      sprintf(obuf,"Range: Holding spell (%s)",
              pl->spell->name);
      else
      sprintf(obuf,"Range: Casting spell (%s)",
              pl->spell->name);
    }
    else
#endif
      sprintf(obuf,"Range: spell (%s)",
              spells[pl->contr->chosen_spell].name);
    break;
   case range_wand:
    sprintf(obuf,"Range: wand (%s)",
          pl->contr->known_spell ?
          spells[pl->contr->chosen_item_spell].name : "unknown");
    break;
   case range_rod:
    sprintf(obuf,"Range: rod (%s)",
          pl->contr->known_spell ?
          spells[pl->contr->chosen_item_spell].name : "unknown");
    break;
   case range_horn:
    sprintf(obuf,"Range: horn (%s)",
          pl->contr->known_spell ?
          spells[pl->contr->chosen_item_spell].name : "unknown");
    break;
    /* range_scroll is only used for controlling golems.  If the
     * the player does not have a golem, reset some things.
     */
   case range_scroll:
    if (pl->contr->golem!=NULL)
      sprintf(obuf,"Range: golem (%s)",pl->contr->golem->name);
    else {
      pl->contr->shoottype = range_none;
      strcpy(obuf,"Range: nothing");
    }
    break;
   case range_skill:
       sprintf(obuf,"Skill: %s", pl->chosen_skill!=NULL ?
		skills[pl->chosen_skill->stats.sp].name : "none");
    break;
   default:
    strcpy(obuf,"Range: illegal");
  }
  pl->contr->last_known_spell = pl->contr->known_spell;
  pl->contr->last_shoot=pl->contr->shoottype;
  pl->contr->last_spell=chosen_spell;
}

void set_title(object *pl,char *buf)
{
  if(pl->contr->last_value==-1) {
    /* Eneq(@csd.uu.se): Let players define their own titles. */

    if (pl->contr->own_title[0]=='\0')
      sprintf(buf,"Player: %s the %s",pl->name,pl->contr->title);
    else
      sprintf(buf,"Player: %s the %s",pl->name,pl->contr->own_title);
  }
}



/* Note:  For improved magic mapping display, the space that blocks
 * the view is now marked with value 2.  Any dependencies of map_mark
 * being nonzero have been changed to check for 1.  Also, since
 * map_mark is a char value, putting 2 in should cause no problems.
 * Mark Wedel (master@rahul.net)
 * This function examines the map the player is on, and determines what
 * is visible.  2 is set for walls or objects that blocks view.  1
 * is for open spaces.  map_mark should already have been initialized
 * to zero before this is called.
 * strength is an initial strength*2 rectangular area that we automatically
 * see in/penetrate through.
 */

void magic_mapping_mark(object *pl, char *map_mark, int strength)
{
  int x, y;
  int xmin = pl->x - strength + 1 < 0 ? 0 : pl->x - strength + 1;
  int xmax = pl->x + strength - 1 > pl->map->map_object->x - 1 ? 
    pl->map->map_object->x - 1 : pl->x + strength - 1;
  int ymin = pl->y - strength + 1 < 0 ? 0 : pl->y - strength + 1;
  int ymax = pl->y + strength - 1 > pl->map->map_object->y - 1 ? 
    pl->map->map_object->y - 1 : pl->y + strength - 1;

  for (x = xmin; x <= xmax; x++) {
    for (y = ymin; y <= ymax; y++) {
      if (wall(pl->map, x, y) || blocks_view(pl->map, x, y))
	map_mark[x + pl->map->map_object->x * y] = 2;
      else {
	map_mark[x + pl->map->map_object->x * y] = 1;
	magic_mapping_mark_recursive(pl, map_mark, x, y);
      }
    }
  }
}

/* Takes a player, the map_mark array and an x and y starting position.
 * pl could be replaced by the map, since that is all taht pl is
 * used for.
 * This function examines all the adjacant spaces next to px, py.
 * If there is a wall or it otherwise blocks view on a space, we set
 * map_mark to 2.
 * If the space is otherwise open, we set it to 1, and this function
 * is called again to examine those spaces.
 */
void magic_mapping_mark_recursive(object *pl, char *map_mark, int px, int py)
{
  int x, y, dx, dy;

  for (dx = -1; dx <= 1; dx++) {
    for (dy = -1; dy <= 1; dy++) {
      x = px + dx;
      y = py + dy;
      if (x >= 0 && x < pl->map->map_object->x && y >= 0 && y < pl->map->map_object->y
	&& (map_mark[x + pl->map->map_object->x * y] ==0) ) {
            if (blocks_view(pl->map, x, y))
		map_mark[x + pl->map->map_object->x * y] = 2;
	    else {
		if (wall(pl->map, x, y))
		    map_mark[x + pl->map->map_object->x * y] = 2;
		else
		    map_mark[x + pl->map->map_object->x * y] = 1;
		magic_mapping_mark_recursive(pl, map_mark, x, y);
	    }
	}
    }
  }
}



/* The following function is a lot messier than it really should be,
 * but there is no real easy solution.
 *
 * One of the main causes is uses the crossfire font to draw the stipple
 * pattern.  This then means that the excess needs to be erased.  As things
 * stand now, the excess is erased, and things look ok.
 *
 * Also, display on black and white system is still not as good (useful)
 * as on a color system.  However, things are not too bad.  At present, there
 * are 4 possible outputs:  White, meaning a wall, black, meaning
 * nothing (or only floor), grey (stippled pattern), for any other objects
 * that do not have a black foreground, and another stippled patern for
 * objects that do have a black foreground.
 *
 * Display of the stipples is not perfect.  One of the stipples is just
 * a checkerboard pattern.  IF the resolution is odd, and two of these
 * are placed together, little imperfections in the matching shows up.
 * However, it doesn't affect the usefulness of the display much, and
 * I don't want to add more code to deal with making the stipple perfect.
 * This is because the second stipple pattern used has a different
 * repeat rate
 *
 * Mark Wedel (master@rahul.net)
 */

void draw_map(object *pl) 
{
    int x,y;
    char *map_mark = (char *) malloc(pl->map->map_object->x * pl->map->map_object->y);
    int xmin = pl->map->map_object->x, xmax = 0, ymin = pl->map->map_object->y, ymax = 0;
    SockList sl;

    if (pl->type!=PLAYER) {
	LOG(llevError,"Non player objectg called draw_map.\n");
	return;
    }
    /* First, we figure out what spaces are 'reachable' by the player */
    memset(map_mark, 0, pl->map->map_object->x * pl->map->map_object->y);
    magic_mapping_mark(pl, map_mark, 3);
    for(x = 0; x < pl->map->map_object->x; x++) {
      for(y = 0; y < pl->map->map_object->y; y++) {
        if (map_mark[x + pl->map->map_object->x * y]==1) {
	  xmin = x < xmin ? x : xmin;
	  xmax = x > xmax ? x : xmax;
	  ymin = y < ymin ? y : ymin;
	  ymax = y > ymax ? y : ymax;
        }
      }
    }
    xmin--;
    xmin = xmin < 0 ? 0 : xmin;
    xmax++;
    xmax = xmax > pl->map->map_object->x - 1 ? pl->map->map_object->x - 1: xmax;
    ymin--;
    ymin = ymin < 0 ? 0 : ymin;
    ymax++;
    ymax = ymax > pl->map->map_object->y - 1? pl->map->map_object->y - 1: ymax;


    sl.buf=malloc(MAXSOCKBUF);
    sprintf((char*)sl.buf,"magicmap %d %d %d %d ", (xmax-xmin+1), (ymax-ymin+1),
	    pl->x - xmin, pl->y - ymin);
    sl.len=strlen((char*)sl.buf);
 
    /* Reversed ordering of X and Y in 0.93.2.  This way the order should
     * match up the way we said it would
     */
    for (y = ymin; y <= ymax; y++) {
      for (x = xmin; x <= xmax; x++) {
	    int mark;

	    if ((mark=map_mark[x+pl->map->map_object->x*y])==0)
		sl.buf[sl.len++]=0;
	    else {
		New_Face *f = get_map(pl->map, x, y)->face;
		if (mark==2)
		    sl.buf[sl.len++]=f->magicmap | FACE_WALL;
		else
		    sl.buf[sl.len++]=f->magicmap;
	    }

      } /* x loop */
    } /* y loop */
    Send_With_Handling(&pl->contr->socket, &sl);
    free(sl.buf);
    free(map_mark);
}


/*
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
