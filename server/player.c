/*
 * static char *rcsid_player_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002,2006-2007 Mark Wedel & Crossfire Development Team
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
#ifndef WIN32 /* ---win32 remove headers */
#include <pwd.h>
#endif
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <sounds.h>
#include <living.h>
#include <object.h>
#include <spells.h>
#include <skills.h>
#include <newclient.h>

static archetype *get_player_archetype(archetype* at);
static int action_makes_visible (object *op);

player *find_player(const char *plname)
{
    player *pl;
    char name[MAX_BUF];
    for(pl=first_player;pl!=NULL;pl=pl->next)
    {
        if(pl->ob != NULL) {
            query_name(pl->ob, name, MAX_BUF);
            if (!strcmp(name,plname))
                return pl;
        }
    };
    return NULL;
}

player* find_player_partial_name( const char* plname )
    {
    player* pl;
    player* found = NULL;
    size_t namelen = strlen( plname );
    for ( pl = first_player; pl != NULL; pl = pl->next )
        {
        if ( strlen( pl->ob->name ) < namelen )
            continue;

        if ( !strcmp( pl->ob->name, plname) )
            return pl;

        if ( !strncasecmp( pl->ob->name, plname, namelen ) )
            {
            if ( found )
                return NULL;

            found = pl;
            }
        }
    return found;
    }

void display_motd(const object *op) {
    char buf[MAX_BUF];
    char motd[HUGE_BUF];
    FILE *fp;
    int comp;
    int size;

    sprintf(buf, "%s/%s", settings.confdir, settings.motd);
    if ((fp=open_and_uncompress(buf, 0, &comp)) == NULL) {
	return;
    }
    motd[0]='\0';
    size=0;
    while (fgets(buf, MAX_BUF, fp) != NULL) {
	  if( *buf == '#')
	    continue;
      strncat(motd+size,buf,HUGE_BUF-size);
      size+=strlen(buf);
    }
    draw_ext_info(NDI_UNIQUE | NDI_GREEN, 0, op, MSG_TYPE_MOTD, MSG_SUBTYPE_NONE,
		  motd, NULL);
    close_and_delete(fp, comp);
}

void send_rules(const object *op) {
    char buf[MAX_BUF];
    char rules[HUGE_BUF];
    FILE *fp;
    int comp;
    int size;

    sprintf(buf, "%s/%s", settings.confdir, settings.rules);
    if ((fp=open_and_uncompress(buf, 0, &comp)) == NULL) {
    return;
    }
    rules[0]='\0';
    size=0;
    while (fgets(buf, MAX_BUF, fp) != NULL) {
      if( *buf == '#')
        continue;
      if (size + strlen(buf)>=HUGE_BUF)
          {
          LOG(llevDebug, "Warning, rules size is > %d bytes.\n", HUGE_BUF);
          break;
          }
      strncat(rules+size,buf,HUGE_BUF-size);
      size+=strlen(buf);
    }
    draw_ext_info(NDI_UNIQUE | NDI_GREEN, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_RULES,
		  rules, NULL);
    close_and_delete(fp, comp);
}

void send_news(const object *op) {
    char buf[MAX_BUF];
    char news[HUGE_BUF];
    char subject[MAX_BUF];
    FILE *fp;
    int comp;
    int size;

    sprintf(buf, "%s/%s", settings.confdir, settings.news);
    if ((fp=open_and_uncompress(buf, 0, &comp)) == NULL)
        return;
    news[0]='\0';
    subject[0]='\0';
    size=0;
    while (fgets(buf, MAX_BUF, fp) != NULL) {
      if( *buf == '#')
        continue;
      if ( *buf =='%'){ /* send one news */
          if (size>0)
              draw_ext_info_format(NDI_UNIQUE | NDI_GREEN, 0, op,
                  MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_NEWS,
                  "%s:\n%s",
                  "%s:\n%s",
                  subject, news); /*send previously read news*/
          strcpy(subject,buf+1);
          strip_endline(subject);
          size=0;
          news[0]='\0';
      }
      else{
          if (size + strlen(buf)>=HUGE_BUF)
              {
              LOG(llevDebug, "Warning, one news item has size > %d bytes.\n", HUGE_BUF);
              break;
              }
          strncat(news+size,buf,HUGE_BUF-size);
          size+=strlen(buf);
      }
    }

    draw_ext_info_format(NDI_UNIQUE | NDI_GREEN, 0, op,
        MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_NEWS,
        "%s:\n%s",
        "%s:\n%s",
        subject, news);
    close_and_delete(fp, comp);
}

int playername_ok(const char *cp) {
    /* Don't allow - or _ as first character in the name */
    if (*cp == '-' || *cp == '_') return 0;

    for(;*cp!='\0';cp++)
	if(!((*cp>='a'&&*cp<='z')||(*cp>='A'&&*cp<='Z'))&&*cp!='-'&&*cp!='_')
	    return 0;
    return 1;
}

/* This no longer sets the player map.  Also, it now updates
 * all the pointers so the caller doesn't need to do that.
 * Caller is responsible for setting the correct map.
 *
 * Redo this to do both get_player_ob and get_player.
 * Hopefully this will be less bugfree and simpler.
 * Returns the player structure.  If 'p' is null,
 * we create a new one.  Otherwise, we recycle
 * the one that is passed.
 */
static player* get_player(player *p) {
    object *op=arch_to_object(get_player_archetype(NULL));
    int i;

    if (!p) {
	player *tmp;

	p = (player *) malloc(sizeof(player));
	if(p==NULL)
	    fatal(OUT_OF_MEMORY);

	/* This adds the player in the linked list.  There is extra
	 * complexity here because we want to add the new player at the
	 * end of the list - there is in fact no compelling reason that
	 * that needs to be done except for things like output of
	 * 'who'.
	 */
	tmp=first_player;
	while(tmp!=NULL&&tmp->next!=NULL)
	    tmp=tmp->next;
	if(tmp!=NULL)
	    tmp->next=p;
	else
	    first_player=p;

	p->next = NULL;
    }

    /* Clears basically the entire player structure except
     * for next and socket.
     */
    memset((void*)((char*)p + offsetof(player, maplevel)), 0,
	    sizeof(player) - offsetof(player, maplevel));

    /* There are some elements we want initialized to non zero value -
     * we deal with that below this point.
     */
    p->party=NULL;
    p->outputs_sync=16;		/* Every 2 seconds */
    p->outputs_count=1;		/* Keeps present behaviour */
    p->unapply = unapply_nochoice;
    p->Swap_First = -1;

#ifdef AUTOSAVE
    p->last_save_tick = 9999999;
#endif

    strcpy(p->savebed_map, first_map_path);  /* Init. respawn position */

    op->contr=p; /* this aren't yet in archetype */
    p->ob = op;
    op->speed_left=0.5;
    op->speed=1.0;
    op->direction=5;     /* So player faces south */
    op->stats.wc=2;
    op->run_away = 25; /* Then we panick... */
    p->socket.monitor_spells = 0; /* Needed because esrv_update_spells( ) gets called by roll_stats */

    roll_stats(op);
    p->state=ST_ROLL_STAT;
    clear_los(op);

    p->gen_sp_armour=10;
    p->last_speed= -1;
    p->shoottype=range_none;
    p->bowtype=bow_normal;
    p->petmode=pet_normal;
    p->listening=10;
    p->last_weapon_sp= -1;
    p->peaceful=1;			/* default peaceful */
    p->do_los=1;
    p->explore=0;
	p->no_shout=0;			/* default can shout */

    strncpy(p->title, op->arch->clone.name, sizeof(p->title)-1);
    p->title[sizeof(p->title)-1] = '\0';
    op->race = add_string (op->arch->clone.race);

    CLEAR_FLAG(op,FLAG_READY_SKILL);

    /* we need to clear these to -1 and not zero - otherwise,
     * if a player quits and starts a new character, we wont
     * send new values to the client, as things like exp start
     * at zero.
     */
    for (i=0; i < NUM_SKILLS; i++) {
	p->last_skill_exp[i] = -1;
	p->last_skill_ob[i] = NULL;
    }
    for (i=0; i < NROFATTACKS; i++) {
	p->last_resist[i] = -1;
    }
    p->last_stats.exp = -1;
    p->last_weight = (uint32)-1;

    p->socket.update_look=0;
    p->socket.look_position=0;
    return p;
}


/* This loads the first map an puts the player on it. */
static void set_first_map(object *op)
{
    strcpy(op->contr->maplevel, first_map_path);
    op->x = -1;
    op->y = -1;
    enter_exit(op, NULL);
}

/**
 * Tries to add player on the connection passwd in ns.
 * All we can really get in this is some settings like host and display
 * mode.
 */

int add_player(socket_struct *ns) {
    player *p;

    p=get_player(NULL);
    memcpy(&p->socket, ns, sizeof(socket_struct));
    p->socket.faces_sent = malloc(p->socket.faces_sent_len*sizeof(*p->socket.faces_sent));
    if(p->socket.faces_sent == NULL)
	fatal(OUT_OF_MEMORY);
    memcpy(p->socket.faces_sent, ns->faces_sent, p->socket.faces_sent_len*sizeof(*p->socket.faces_sent));
    /* Needed because the socket we just copied over needs to be cleared.
     * Note that this can result in a client reset if there is partial data
     * on the uncoming socket.
     */
    p->socket.inbuf.len = 0;
    set_first_map(p->ob);

    CLEAR_FLAG(p->ob, FLAG_FRIENDLY);
    add_friendly_object(p->ob);
    send_rules(p->ob);
    send_news(p->ob);
    display_motd(p->ob);
    get_name(p->ob);
    return 0;
}

/* get_player_archetype() return next player archetype from archetype
 * list. Not very efficient routine, but used only creating new players.
 * Note: there MUST be at least one player archetype!
 */
static archetype *get_player_archetype(archetype* at)
{
    archetype *start = at;
    for (;;) {
	if (at==NULL || at->next==NULL)
	    at=first_archetype;
	else
	    at=at->next;
	if(at->clone.type==PLAYER)
	    return at;
	if (at == start) {
	    LOG (llevError, "No Player archetypes\n");
	    exit (-1);
	}
    }
}


object *get_nearest_player(object *mon) {
    object *op = NULL;
    player *pl = NULL;
    objectlink *ol;
    unsigned lastdist;
    rv_vector	rv;

    for(ol=first_friendly_object,lastdist=1000;ol!=NULL;ol=ol->next) {
	/* We should not find free objects on this friendly list, but it
	 * does periodically happen.  Given that, lets deal with it.
	 * While unlikely, it is possible the next object on the friendly
	 * list is also free, so encapsulate this in a while loop.
	 */
	while (QUERY_FLAG(ol->ob, FLAG_FREED) || !QUERY_FLAG(ol->ob, FLAG_FRIENDLY)) {
	    object *tmp=ol->ob;

	    /* Can't do much more other than log the fact, because the object
	     * itself will have been cleared.
	     */
	    LOG(llevDebug,"get_nearest_player: Found free/non friendly object on friendly list\n");
	    ol = ol->next;
	    remove_friendly_object(tmp);
	    if (!ol) return op;
	}

	/* Remove special check for player from this.  First, it looks to cause
	 * some crashes (ol->ob->contr not set properly?), but secondly, a more
	 * complicated method of state checking would be needed in any case -
	 * as it was, a clever player could type quit, and the function would
	 * skip them over while waiting for confirmation.  Remove
	 * on_same_map check, as can_detect_enemy also does this
	 */
	if (!can_detect_enemy(mon,ol->ob,&rv))
		continue;

	if(lastdist>rv.distance) {
	    op=ol->ob;
	    lastdist=rv.distance;
	}
    }
    for (pl=first_player; pl != NULL; pl=pl->next) {
	if (on_same_map(mon, pl->ob)&& can_detect_enemy(mon, pl->ob,&rv)) {

	    if(lastdist>rv.distance) {
		op=pl->ob;
		lastdist=rv.distance;
	    }
	}
    }
    return op;
}

/* I believe this can safely go to 2, 3 is questionable, 4 will likely
 * result in a monster paths backtracking.  It basically determines how large a
 * detour a monster will take from the direction path when looking
 * for a path to the player.  The values are in the amount of direction
 * the deviation is
 */
#define DETOUR_AMOUNT	2

/* This is used to prevent infinite loops.  Consider a case where the
 * player is in a chamber (with gate closed), and monsters are outside.
 * with DETOUR_AMOUNT==2, the function will turn each corner, trying to
 * find a path into the chamber.  This is a good thing, but since there
 * is no real path, it will just keep circling the chamber for
 * ever (this could be a nice effect for monsters, but not for the function
 * to get stuck in.  I think for the monsters, if max is reached and
 * we return the first direction the creature could move would result in the
 * circling behaviour.  Unfortunately, this function is also used to determined
 * if the creature should cast a spell, so returning a direction in that case
 * is probably not a good thing.
 */
#define MAX_SPACES	50


/* Returns the direction to the player, if valid.  Returns 0 otherwise.
 * modified to verify there is a path to the player.  Does this by stepping towards
 * player and if path is blocked then see if blockage is close enough to player that
 * direction to player is changed (ie zig or zag).  Continue zig zag until either
 * reach player or path is blocked.  Thus, will only return true if there is a free
 * path to player.  Though path may not be a straight line. Note that it will find
 * player hiding along a corridor at right angles to the corridor with the monster.
 *
 * Modified by MSW 2001-08-06 to handle tiled maps. Various notes:
 * 1) With DETOUR_AMOUNT being 2, it should still go and find players hiding
 * down corriders.
 * 2) I think the old code was broken if the first direction the monster
 * should move was blocked - the code would store the first direction without
 * verifying that the player can actually move in that direction.  The new
 * code does not store anything in firstdir until we have verified that the
 * monster can in fact move one space in that direction.
 * 3) I'm not sure how good this code will be for moving multipart monsters,
 * since only simple checks to blocked are being called, which could mean the monster
 * is blocking itself.
 */
int path_to_player(object *mon, object *pl, unsigned mindiff) {
    rv_vector	rv;
    sint16  x,y;
    int lastx,lasty,dir,i,diff, firstdir=0,lastdir, max=MAX_SPACES, mflags, blocked;
    mapstruct *m ,*lastmap;

    get_rangevector(mon, pl, &rv, 0);

    if (rv.distance<mindiff) return 0;

    x=mon->x;
    y=mon->y;
    m=mon->map;
    dir = rv.direction;
    lastdir = firstdir = rv.direction; /* perhaps we stand next to pl, init firstdir too */
    diff = FABS(rv.distance_x)>FABS(rv.distance_y)?FABS(rv.distance_x):FABS(rv.distance_y);
    /* If we can't solve it within the search distance, return now. */
    if (diff>max) return 0;
    while (diff >1 && max>0) {
	lastx = x;
	lasty = y;
	lastmap = m;
	x = lastx + freearr_x[dir];
	y = lasty + freearr_y[dir];

	mflags = get_map_flags(m, &m, x, y, &x, &y);
	blocked = (mflags & P_OUT_OF_MAP) ? MOVE_ALL : GET_MAP_MOVE_BLOCK(m, x, y);

	/* Space is blocked - try changing direction a little */
	if ((mflags & P_OUT_OF_MAP) || ((OB_TYPE_MOVE_BLOCK(mon, blocked) || (mflags&P_IS_ALIVE))
	   && (m == mon->map && blocked_link(mon, m, x, y)))) {
	    /* recalculate direction from last good location.  Possible
	     * we were not traversing ideal location before.
	     */
	    get_rangevector_from_mapcoord(lastmap, lastx, lasty, pl, &rv, 0);
	    if (rv.direction != dir) {
		/* OK - says direction should be different - lets reset the
		 * the values so it will try again.
		 */
		x = lastx;
		y = lasty;
		m = lastmap;
		dir = firstdir = rv.direction;
	    } else {
		/* direct path is blocked - try taking a side step to
		 * either the left or right.
		 * Note increase the values in the loop below to be
		 * more than -1/1 respectively will mean the monster takes
		 * bigger detour.  Have to be careful about these values getting
		 * too big (3 or maybe 4 or higher) as the monster may just try
		 * stepping back and forth
		 */
		for (i=-DETOUR_AMOUNT; i<=DETOUR_AMOUNT; i++) {
		    if (i==0) continue;	/* already did this, so skip it */
		    /* Use lastdir here - otherwise,
		     * since the direction that the creature should move in
		     * may change, you could get infinite loops.
		     * ie, player is northwest, but monster can only
		     * move west, so it does that.  It goes some distance,
		     * gets blocked, finds that it should move north,
		     * can't do that, but now finds it can move east, and
		     * gets back to its original point.  lastdir contains
		     * the last direction the creature has successfully
		     * moved.
		     */

		    x = lastx + freearr_x[absdir(lastdir+i)];
		    y = lasty + freearr_y[absdir(lastdir+i)];
		    m = lastmap;
		    mflags = get_map_flags(m, &m, x, y, &x, &y);
		    if (mflags & P_OUT_OF_MAP) continue;
		    blocked = GET_MAP_MOVE_BLOCK(m, x, y);
		    if (OB_TYPE_MOVE_BLOCK(mon, blocked)) continue;
		    if (mflags & P_IS_ALIVE) continue;

		    if (m == mon->map && blocked_link(mon, m, x, y)) break;
		}
		/* go through entire loop without finding a valid
		 * sidestep to take - thus, no valid path.
		 */
		if (i==(DETOUR_AMOUNT+1))
		    return 0;
		diff--;
		lastdir=dir;
		max--;
		if (!firstdir) firstdir = dir+i;
	    } /* else check alternate directions */
	} /* if blocked */
	else {
	    /* we moved towards creature, so diff is less */
	    diff--;
	    max--;
	    lastdir=dir;
	    if (!firstdir) firstdir = dir;
	}
	if (diff<=1) {
	    /* Recalculate diff (distance) because we may not have actually
	     * headed toward player for entire distance.
	     */
	    get_rangevector_from_mapcoord(m, x, y, pl, &rv, 0);
	    diff = FABS(rv.distance_x)>FABS(rv.distance_y)?FABS(rv.distance_x):FABS(rv.distance_y);
	}
	if (diff>max) return 0;
    }
    /* If we reached the max, didn't find a direction in time */
    if (!max) return 0;

    return firstdir;
}

void give_initial_items(object *pl,treasurelist *items) {
    object *op,*next=NULL;

    if(pl->randomitems!=NULL)
	create_treasure(items,pl,GT_STARTEQUIP | GT_ONLY_GOOD,1,0);

    for (op=pl->inv; op; op=next) {
	next = op->below;

	/* Forces get applied per default, unless they have the
         * flag "neutral" set. Sorry but I can't think of a better way
	 */
  	if(op->type==FORCE && !QUERY_FLAG(op, FLAG_NEUTRAL))
	    SET_FLAG(op,FLAG_APPLIED);

	/* we never give weapons/armour if these cannot be used
         * by this player due to race restrictions
	 */
	if (pl->type == PLAYER) {
        if ((!QUERY_FLAG(pl, FLAG_USE_ARMOUR) && IS_ARMOR(op)) ||
            (!QUERY_FLAG(pl, FLAG_USE_WEAPON) && IS_WEAPON(op)) ||
            (!QUERY_FLAG(pl, FLAG_USE_SHIELD) && IS_SHIELD(op))) {
	    remove_ob (op);
	    free_object (op);
	    continue;
	  }
	}

	/* This really needs to be better - we should really give
	 * a substitute spellbook.  The problem is that we don't really
	 * have a good idea what to replace it with (need something like
	 * a first level treasurelist for each skill.)
	 * remove duplicate skills also
	 */
  	if(op->type==SPELLBOOK || op->type == SKILL) {
	    object *tmp;

	    for (tmp=op->below; tmp; tmp=tmp->below)
		if (tmp->type == op->type && tmp->name == op->name) break;

	    if (tmp) {
		remove_ob(op);
		free_object(op);
		LOG(llevError,"give_initial_items: Removing duplicate object %s\n",
		    tmp->name);
		continue;
	    }
	    if (op->nrof > 1) op->nrof = 1;
	}

	if (op->type == SPELLBOOK && op->inv) {
	    CLEAR_FLAG(op->inv, FLAG_STARTEQUIP);
	}

	/* Give starting characters identified, uncursed, and undamned
	 * items.  Just don't identify gold or silver, or it won't be
	 * merged properly.
	 */
	if (need_identify(op)) {
	    SET_FLAG(op, FLAG_IDENTIFIED);
	    CLEAR_FLAG(op, FLAG_CURSED);
	    CLEAR_FLAG(op, FLAG_DAMNED);
	}
	if(op->type==SPELL)  {
	    remove_ob(op);
	    free_object(op);
            continue;
	}
	else if(op->type==SKILL)  {
	    SET_FLAG(op, FLAG_CAN_USE_SKILL);
	    op->stats.exp = 0;
	    op->level = 1;
	}
	/* lock all 'normal items by default */
	else SET_FLAG(op, FLAG_INV_LOCKED);
    } /* for loop of objects in player inv */


    /* Need to set up the skill pointers */
    link_player_skills(pl);

    /**
     * Now we do a second loop, to apply weapons/armors/...
     * This is because weapons require the skill, which can be given after the first loop.
     */
    for (op=pl->inv; op; op=next) {
        next = op->below;
        if ((IS_ARMOR(op) || IS_WEAPON(op) || IS_SHIELD(op)) && !QUERY_FLAG(op, FLAG_APPLIED))
            manual_apply(pl, op, AP_NOPRINT);
    }
}

void get_name(object *op) {
    op->contr->write_buf[0]='\0';
    op->contr->state=ST_GET_NAME;
    send_query(&op->contr->socket,0,"What is your name?\n:");
}

void get_password(object *op) {
    op->contr->write_buf[0]='\0';
    op->contr->state=ST_GET_PASSWORD;
    send_query(&op->contr->socket,CS_QUERY_HIDEINPUT, "What is your password?\n:");
}

void play_again(object *op)
{
    op->contr->state=ST_PLAY_AGAIN;
    op->chosen_skill = NULL;
    send_query(&op->contr->socket, CS_QUERY_SINGLECHAR, "Do you want to play again (a/q)?");
    /* a bit of a hack, but there are various places early in th
     * player creation process that a user can quit (eg, roll
     * stats) that isn't removing the player.  Taking a quick
     * look, there are many places that call play_again without
     * removing the player - it probably makes more sense
     * to leave it to play_again to remove the object in all
     * cases.
     */
    if (!QUERY_FLAG(op, FLAG_REMOVED))
	remove_ob(op);
    /* Need to set this to null - otherwise, it could point to garbage,
     * and draw() doesn't check to see if the player is removed, only if
     * the map is null or not swapped out.
     */
    op->map = NULL;
}


int receive_play_again(object *op, char key)
{
    if(key=='q'||key=='Q') {
	remove_friendly_object(op);
	leave(op->contr,0); /* ericserver will draw the message */
	return 2;
    }
    else if(key=='a'||key=='A') {
	player *pl = op->contr;
	const char *name = op->name;

	add_refcount(name);
	remove_friendly_object(op);
	free_object(op);
	pl = get_player(pl);
	op = pl->ob;
	add_friendly_object(op);
	op->contr->password[0]='~';
	FREE_AND_CLEAR_STR(op->name);
	FREE_AND_CLEAR_STR(op->name_pl);

	/* Lets put a space in here */
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
		      "\n", "\n");
	get_name(op);
	op->name = name;		/* Alrady added a refcount above */
	op->name_pl = add_string(name);
	set_first_map(op);
    } else {
	/* user pressed something else so just ask again... */
	play_again(op);
    }
    return 0;
}

void confirm_password(object *op) {

    op->contr->write_buf[0]='\0';
    op->contr->state=ST_CONFIRM_PASSWORD;
    send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, "Please type your password again.\n:");
}

void get_party_password(object *op, partylist *party) {
    if (party == NULL) {
	LOG(llevError, "get_party_password(): tried to make player %s join a NULL party\n", op->name);
	return;
    }
    op->contr->write_buf[0]='\0';
    op->contr->state=ST_GET_PARTY_PASSWORD;
    op->contr->party_to_join = party;
    send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, "What is the password?\n:");
}


/* This rolls four 1-6 rolls and sums the best 3 of the 4. */
int roll_stat(void) {
    int a[4],i,j,k;

    for(i=0;i<4;i++)
	a[i]=(int)RANDOM()%6+1;

    for(i=0,j=0,k=7;i<4;i++)
	if(a[i]<k)
	    k=a[i],j=i;

    for(i=0,k=0;i<4;i++) {
	if(i!=j)
	    k+=a[i];
    }
    return k;
}

void roll_stats(object *op) {
    int sum=0;
    int i = 0, j = 0;
    int statsort[7];

    do {
	op->stats.Str=roll_stat();
	op->stats.Dex=roll_stat();
	op->stats.Int=roll_stat();
	op->stats.Con=roll_stat();
	op->stats.Wis=roll_stat();
	op->stats.Pow=roll_stat();
	op->stats.Cha=roll_stat();
	sum=op->stats.Str+op->stats.Dex+op->stats.Int+
	    op->stats.Con+op->stats.Wis+op->stats.Pow+
	    op->stats.Cha;
    } while(sum<82||sum>116);

    /* Sort the stats so that rerolling is easier... */
    statsort[0] = op->stats.Str;
    statsort[1] = op->stats.Dex;
    statsort[2] = op->stats.Int;
    statsort[3] = op->stats.Con;
    statsort[4] = op->stats.Wis;
    statsort[5] = op->stats.Pow;
    statsort[6] = op->stats.Cha;

    /* a quick and dirty bubblesort? */
    do {
	if (statsort[i] < statsort[i + 1]) {
	    j = statsort[i];
	    statsort[i] = statsort[i + 1];
	    statsort[i + 1] = j;
	    i = 0;
	} else {
	    i++;
	}
    } while (i < 6);

    op->stats.Str = statsort[0];
    op->stats.Dex = statsort[1];
    op->stats.Con = statsort[2];
    op->stats.Int = statsort[3];
    op->stats.Wis = statsort[4];
    op->stats.Pow = statsort[5];
    op->stats.Cha = statsort[6];


    op->contr->orig_stats.Str=op->stats.Str;
    op->contr->orig_stats.Dex=op->stats.Dex;
    op->contr->orig_stats.Int=op->stats.Int;
    op->contr->orig_stats.Con=op->stats.Con;
    op->contr->orig_stats.Wis=op->stats.Wis;
    op->contr->orig_stats.Pow=op->stats.Pow;
    op->contr->orig_stats.Cha=op->stats.Cha;

    op->level=1;
    op->stats.exp=0;
    op->stats.ac=0;

    op->contr->levhp[1] = 9;
    op->contr->levsp[1] = 6;
    op->contr->levgrace[1] = 3;

    fix_object(op);
    op->stats.hp = op->stats.maxhp;
    op->stats.sp = op->stats.maxsp;
    op->stats.grace = op->stats.maxgrace;
    op->contr->orig_stats=op->stats;
}

void roll_again(object *op)
{
    esrv_new_player(op->contr, 0);
    send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"[y] to roll new stats [n] to use stats\n[1-7] [1-7] to swap stats.\nRoll again (y/n/1-7)?  ");
}

static void swap_stat(object *op,int Swap_Second)
{
    signed char tmp;

    if ( op->contr->Swap_First == -1 ) {
	LOG(llevError,"player.c:swap_stat() - Swap_First is -1\n");
	return;
    }

    tmp = get_attr_value(&op->contr->orig_stats, op->contr->Swap_First);

    set_attr_value(&op->contr->orig_stats, op->contr->Swap_First,
	get_attr_value(&op->contr->orig_stats, Swap_Second));

    set_attr_value(&op->contr->orig_stats, Swap_Second, tmp);

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_NEWPLAYER,
		  "%s done\n",
		  "%s done\n",
		  short_stat_name[Swap_Second]);

    op->stats.Str = op->contr->orig_stats.Str;
    op->stats.Dex = op->contr->orig_stats.Dex;
    op->stats.Con = op->contr->orig_stats.Con;
    op->stats.Int = op->contr->orig_stats.Int;
    op->stats.Wis = op->contr->orig_stats.Wis;
    op->stats.Pow = op->contr->orig_stats.Pow;
    op->stats.Cha = op->contr->orig_stats.Cha;
    op->stats.ac=0;

    op->level=1;
    op->stats.exp=0;
    op->stats.ac=0;

    op->contr->levhp[1] = 9;
    op->contr->levsp[1] = 6;
    op->contr->levgrace[1] = 3;

    fix_object(op);
    op->stats.hp = op->stats.maxhp;
    op->stats.sp = op->stats.maxsp;
    op->stats.grace = op->stats.maxgrace;
    op->contr->orig_stats=op->stats;
    op->contr->Swap_First=-1;
}


/* This code has been greatly reduced, because with set_attr_value
 * and get_attr_value, the stats can be accessed just numeric
 * ids.  stat_trans is a table that translate the number entered
 * into the actual stat.  It is needed because the order the stats
 * are displayed in the stat window is not the same as how
 * the number's access that stat.  The table does that translation.
 */
int key_roll_stat(object *op, char key)
{
    int keynum = key -'0';
    static sint8 stat_trans[] = {-1, STR, DEX, CON, INT, WIS, POW, CHA};

    if (keynum>0 && keynum<=7) {
	if (op->contr->Swap_First==-1) {
	    op->contr->Swap_First=stat_trans[keynum];
	    draw_ext_info_format(NDI_UNIQUE, 0,op,
			 MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_NEWPLAYER,
			  "%s ->",
			  "%s ->",
			  short_stat_name[stat_trans[keynum]]);
	}
	else
	    swap_stat(op,stat_trans[keynum]);

	send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"");
	return 1;
    }
    switch (key) {
	case 'n':
        case 'N': {
	    SET_FLAG(op, FLAG_WIZ);
	    if(op->map==NULL) {
		LOG(llevError,"Map == NULL in state 2\n");
		break;
	    }

	    SET_ANIMATION(op, 2);     /* So player faces south */
	    /* Enter exit adds a player otherwise */
	    add_statbonus(op);
	    send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"Now choose a character.\nPress any key to change outlook.\nPress `d' when you're pleased.\n");
	    op->contr->state = ST_CHANGE_CLASS;
	    if (op->msg)
		draw_ext_info(NDI_BLUE, 0, op,
			      MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_NEWPLAYER,
			      op->msg, op->msg);
	    return 0;
	}
     case 'y':
     case 'Y':
	roll_stats(op);
	send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"");
	return 1;

     case 'q':
     case 'Q':
      play_again(op);
      return 1;

     default:
	  send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"Yes, No, Quit or 1-6.  Roll again?");
	return 0;
    }
    return 0;
}

/* This function takes the key that is passed, and does the
 * appropriate action with it (change race, or other things).
 * The function name is for historical reasons - now we have
 * separate race and class; this actually changes the RACE,
 * not the class.
 */

int key_change_class(object *op, char key)
{
    int tmp_loop;

    if(key=='q'||key=='Q') {
      remove_ob(op);
      play_again(op);
      return 0;
    }
    if(key=='d'||key=='D') {
	char buf[MAX_BUF];

	/* this must before then initial items are given */
	esrv_new_player(op->contr, op->weight+op->carrying);
	create_treasure(find_treasurelist("starting_wealth"),op, 0, 0, 0);

        /* Lauwenmark : Here we handle the BORN global event */
        execute_global_event(EVENT_BORN, op);

        /* Lauwenmark : We then generate a LOGIN event */
        execute_global_event(EVENT_LOGIN, op->contr, op->contr->socket.host);
	op->contr->state=ST_PLAYING;

	if (op->msg) {
	    free_string(op->msg);
	    op->msg=NULL;
	}

	/* We create this now because some of the unique maps will need it
	 * to save here.
	 */
	sprintf(buf,"%s/%s/%s",settings.localdir,settings.playerdir,op->name);
	make_path_to_file(buf);

#ifdef AUTOSAVE
	op->contr->last_save_tick = pticks;
#endif
	start_info(op);
	CLEAR_FLAG(op, FLAG_WIZ);
	give_initial_items(op,op->randomitems);
	link_player_skills(op);
	esrv_send_inventory(op, op);
    fix_object(op);

        /* This moves the player to a different start map, if there
         * is one for this race
         */
        if(*first_map_ext_path) {
            object *tmp;
            char mapname[MAX_BUF];

            snprintf(mapname, MAX_BUF-1, "%s/%s",
                     first_map_ext_path, op->arch->name);
            printf("%s\n", mapname);
            tmp=get_object();
            EXIT_PATH(tmp) = add_string(mapname);
            EXIT_X(tmp) = op->x;
            EXIT_Y(tmp) = op->y;
            enter_exit(op,tmp); /* we don't really care if it succeeded;
                                 * if the map isn't there, then stay on the
                                 * default initial map */
            free_object(tmp);
        } else {
            LOG(llevDebug,"first_map_ext_path not set\n");
        }
	return 0;
    }

    /* Following actually changes the race - this is the default command
     * if we don't match with one of the options above.
     */

    tmp_loop = 0;
    while(!tmp_loop) {
	const char *name = add_string (op->name);
	int x = op->x, y = op->y;
	remove_statbonus(op);
	remove_ob (op);
	op->arch = get_player_archetype(op->arch);
	copy_object (&op->arch->clone, op);
	op->stats = op->contr->orig_stats;
	free_string (op->name);
	op->name = name;
	free_string(op->name_pl);
	op->name_pl = add_string(name);
	op->x = x;
	op->y = y;
	SET_ANIMATION(op, 2);    /* So player faces south */
	insert_ob_in_map (op, op->map, op,0);
	strncpy(op->contr->title, op->arch->clone.name, sizeof(op->contr->title)-1);
	op->contr->title[sizeof(op->contr->title)-1] = '\0';
	add_statbonus(op);
	tmp_loop=allowed_class(op);
    }
    update_object(op,UP_OBJ_FACE);
    esrv_update_item(UPD_FACE,op,op);
    fix_object(op);
    op->stats.hp=op->stats.maxhp;
    op->stats.sp=op->stats.maxsp;
    op->stats.grace=0;
    if (op->msg)
	draw_ext_info(NDI_BLUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_NEWPLAYER,
		      op->msg, op->msg);
    send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"Press any key for the next race.\nPress `d' to play this race.\n");
    return 0;
}

int key_confirm_quit(object *op, char key)
{
    char buf[MAX_BUF];

    if(key!='y'&&key!='Y'&&key!='q'&&key!='Q') {
	op->contr->state=ST_PLAYING;
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
		      "OK, continuing to play.", NULL);
	return 1;
    }

    /* Lauwenmark : Here we handle the REMOVE global event */
    execute_global_event(EVENT_REMOVE, op);
    terminate_all_pets(op);
    leave_map(op);
    op->direction=0;
    draw_ext_info_format(NDI_UNIQUE | NDI_ALL, 5, NULL,
			 MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_PLAYER,
			 "%s quits the game.",
			 "%s quits the game.",
			 op->name);

    strcpy(op->contr->killer,"quit");
    check_score(op);
    op->contr->party=NULL;
    if (settings.set_title == TRUE)
	op->contr->own_title[0]='\0';

    if(!QUERY_FLAG(op,FLAG_WAS_WIZ)) {
	mapstruct *mp, *next;

	/* We need to hunt for any per player unique maps in memory and
	 * get rid of them.  The trailing slash in the path is intentional,
	 * so that players named 'Ab' won't match against players 'Abe' pathname
	 */
	sprintf(buf,"%s/%s/%s/", settings.localdir, settings.playerdir, op->name);
	for (mp=first_map; mp!=NULL; mp=next) {
	    next = mp->next;
	    if (!strncmp(mp->path, buf, strlen(buf)))
		delete_map(mp);
	}

	delete_character(op->name);
    }
    play_again(op);
    return 1;
}

static void flee_player(object *op) {
    int dir,diff;
    rv_vector rv;

    if(op->stats.hp < 0) {
	LOG(llevDebug, "Fleeing player is dead.\n");
	CLEAR_FLAG(op, FLAG_SCARED);
	return;
    }

    if(op->enemy==NULL) {
	LOG(llevDebug,"Fleeing player had no enemy.\n");
	CLEAR_FLAG(op, FLAG_SCARED);
	return;
    }

    /* Seen some crashes here.  Since we don't store an
     * op->enemy_count, it is possible that something destroys the
     * actual enemy, and the object is recycled.
     */
    if (op->enemy->map == NULL) {
	CLEAR_FLAG(op, FLAG_SCARED);
	op->enemy=NULL;
	return;
    }

    if(!(random_roll(0, 4, op, PREFER_LOW)) && did_make_save(op, op->level, 0)) {
	op->enemy=NULL;
	CLEAR_FLAG(op, FLAG_SCARED);
	return;
    }
    get_rangevector(op, op->enemy, &rv, 0);

    dir=absdir(4+rv.direction);
    for(diff=0;diff<3;diff++) {
	int m=1-(RANDOM()&2);
	if(move_ob(op,absdir(dir+diff*m),op)||
	   (diff==0 && move_ob(op,absdir(dir-diff*m),op))) {
		return;
	}
    }
    /* Cornered, get rid of scared */
    CLEAR_FLAG(op, FLAG_SCARED);
    op->enemy=NULL;
}


/* check_pick sees if there is stuff to be picked up/picks up stuff.
 * IT returns 1 if the player should keep on moving, 0 if he should
 * stop.
 */
int check_pick(object *op) {
  object *tmp, *next;
  tag_t next_tag=0, op_tag;
  int stop = 0;
  int j, k, wvratio;
  char putstring[128], tmpstr[16];


  /* if you're flying, you can't pick up anything */
  if (op->move_type & MOVE_FLYING)
    return 1;

  op_tag = op->count;

  next = op->below;
  if (next)
    next_tag = next->count;

  /* loop while there are items on the floor that are not marked as
   * destroyed */
  while (next && ! was_destroyed (next, next_tag))
  {
    tmp = next;
    next = tmp->below;
    if (next)
      next_tag = next->count;

    if (was_destroyed (op, op_tag))
        return 0;

    if ( ! can_pick (op, tmp))
      continue;

    if (op->contr->search_str[0]!='\0' && settings.search_items == TRUE)
    {
      if (item_matched_string (op, tmp, op->contr->search_str))
        pick_up (op, tmp);
      continue;
    }

    /* high not bit set?  We're using the old autopickup model */
    if(!(op->contr->mode & PU_NEWMODE)) {
	switch (op->contr->mode) {
	    case 0:	return 1;	/* don't pick up */
	    case 1: pick_up (op, tmp);
		return 1;
	    case 2: pick_up (op, tmp);
		return 0;
	    case 3: return 0;	/* stop before pickup */
	    case 4: pick_up (op, tmp);
		break;
	    case 5: pick_up (op, tmp);
		stop = 1;
		break;
	    case 6:
		if (QUERY_FLAG (tmp, FLAG_KNOWN_MAGICAL) &&
		    ! QUERY_FLAG(tmp, FLAG_KNOWN_CURSED))
		  pick_up(op, tmp);
		break;

	    case 7:
		if (tmp->type == MONEY || tmp->type == GEM)
		  pick_up(op, tmp);
		break;

	    default:
		/* use value density */
		if ( ! QUERY_FLAG (tmp, FLAG_UNPAID)
		    && (query_cost (tmp, op, F_TRUE) * 100
		        / (tmp->weight * MAX (tmp->nrof, 1)))
                       >= op->contr->mode)
		  pick_up(op,tmp);
	}
    }
    else { /* old model */
      /* NEW pickup handling */
      if(op->contr->mode & PU_DEBUG)
      {
	/* some debugging code to figure out item information */
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
		      "item name: %s    item type: %d    weight/value: %d",
		      "item name: %s    item type: %d    weight/value: %d",
		      tmp->name?tmp->name:tmp->arch->name, tmp->type,
		      (int)(query_cost(tmp, op, F_TRUE)*100 / (tmp->weight * MAX(tmp->nrof,1))));


	sprintf(putstring,"...flags: ");
	for(k=0;k<4;k++)
	{
	  for(j=0;j<32;j++)
	  {
	    if((tmp->flags[k]>>j)&0x01)
	    {
	      sprintf(tmpstr,"%d ",k*32+j);
	      strcat(putstring, tmpstr);
	    }
	  }
	}
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		      putstring, putstring);
      }
      /* philosophy:
       * It's easy to grab an item type from a pile, as long as it's
       * generic.  This takes no game-time.  For more detailed pickups
       * and selections, select-items should be used.  This is a
       * grab-as-you-run type mode that's really useful for arrows for
       * example.
       * The drawback: right now it has no frontend, so you need to
       * stick the bits you want into a calculator in hex mode and then
       * convert to decimal and then 'pickup <#>
       */

      /* the first two modes are exclusive: if NOTHING we return, if
       * STOP then we stop.  All the rest are applied sequentially,
       * meaning if any test passes, the item gets picked up. */

      /* if mode is set to pick nothing up, return */

      if(op->contr->mode & PU_NOTHING) return 1;

      /* if mode is set to stop when encountering objects, return.
       * Take STOP before INHIBIT since it doesn't actually pick
       * anything up */

      if(op->contr->mode & PU_STOP) return 0;

      /* useful for going into stores and not losing your settings... */
      /* and for battles wher you don't want to get loaded down while
       * fighting */
      if(op->contr->mode & PU_INHIBIT) return 1;

      /* prevent us from turning into auto-thieves :) */
      if (QUERY_FLAG (tmp, FLAG_UNPAID)) continue;

      /* ignore known cursed objects */
      if (QUERY_FLAG (tmp, FLAG_KNOWN_CURSED) && op->contr->mode & PU_NOT_CURSED) continue;

      /* all food and drink if desired */
      /* question: don't pick up known-poisonous stuff? */
      if(op->contr->mode & PU_FOOD)
	if (tmp->type == FOOD)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"FOOD\n"); continue; }
      if(op->contr->mode & PU_DRINK)
	if (tmp->type == DRINK || (tmp->type == POISON && !QUERY_FLAG(tmp, FLAG_KNOWN_CURSED)))
	{ pick_up(op, tmp); if(0)fprintf(stderr,"DRINK\n"); continue; }
      /* we don't forget dragon food */
      if(op->contr->mode & PU_FLESH)
	if (tmp->type == FLESH)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"FLESH\n"); continue; }

      if(op->contr->mode & PU_POTION)
	if (tmp->type == POTION)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"POTION\n"); continue; }

      /* spellbooks, skillscrolls and normal books/scrolls */
      if(op->contr->mode & PU_SPELLBOOK)
	if (tmp->type == SPELLBOOK)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"SPELLBOOK\n"); continue; }
      if(op->contr->mode & PU_SKILLSCROLL)
	if (tmp->type == SKILLSCROLL)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"SKILLSCROLL\n"); continue; }
      if(op->contr->mode & PU_READABLES)
	if (tmp->type == BOOK || tmp->type == SCROLL)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"READABLES\n"); continue; }

    /* wands/staves/rods/horns */
    if (op->contr->mode & PU_MAGIC_DEVICE)
	if (tmp->type == WAND || tmp->type == ROD || tmp->type == HORN)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"MAGIC_DEVICE\n"); continue; }

      /* pick up all magical items */
      if(op->contr->mode & PU_MAGICAL)
	if (QUERY_FLAG (tmp, FLAG_KNOWN_MAGICAL) && ! QUERY_FLAG(tmp, FLAG_KNOWN_CURSED))
	{ pick_up(op, tmp); if(0)fprintf(stderr,"MAGICAL\n"); continue; }

      if(op->contr->mode & PU_VALUABLES)
      {
	if (tmp->type == MONEY || tmp->type == GEM)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"MONEY/GEM\n"); continue; }
      }

      /* rings & amulets - talismans seems to be typed AMULET */
    if(op->contr->mode & PU_JEWELS)
       if (tmp->type == RING || tmp->type == AMULET)
       { pick_up(op, tmp); if(0)fprintf(stderr,"JEWELS\n"); continue; }

      /* bows and arrows. Bows are good for selling! */
      if(op->contr->mode & PU_BOW)
	if (tmp->type == BOW)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"BOW\n"); continue; }
      if(op->contr->mode & PU_ARROW)
	if (tmp->type == ARROW)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"ARROW\n"); continue; }

      /* all kinds of armor etc. */
      if(op->contr->mode & PU_ARMOUR)
	if (tmp->type == ARMOUR)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"ARMOUR\n"); continue; }
      if(op->contr->mode & PU_HELMET)
	if (tmp->type == HELMET)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"HELMET\n"); continue; }
      if(op->contr->mode & PU_SHIELD)
	if (tmp->type == SHIELD)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"SHIELD\n"); continue; }
      if(op->contr->mode & PU_BOOTS)
	if (tmp->type == BOOTS)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"BOOTS\n"); continue; }
      if(op->contr->mode & PU_GLOVES)
	if (tmp->type == GLOVES)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"GLOVES\n"); continue; }
      if(op->contr->mode & PU_CLOAK)
	if (tmp->type == CLOAK)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"GLOVES\n"); continue; }

      /* hoping to catch throwing daggers here */
      if(op->contr->mode & PU_MISSILEWEAPON)
	if(tmp->type == WEAPON && QUERY_FLAG(tmp, FLAG_IS_THROWN))
	{ pick_up(op, tmp); if(0)fprintf(stderr,"MISSILEWEAPON\n"); continue; }

      /* careful: chairs and tables are weapons! */
      if(op->contr->mode & PU_ALLWEAPON)
      {
	if(tmp->type == WEAPON && tmp->name!=NULL)
	{
	  if(strstr(tmp->name,"table")==NULL && strstr(tmp->arch->name,"table")==NULL &&
	      strstr(tmp->name,"chair") && strstr(tmp->arch->name,"chair")==NULL)
	  { pick_up(op, tmp); if(0)fprintf(stderr,"WEAPON\n"); continue; }
	}
	if(tmp->type == WEAPON && tmp->name==NULL)
	{
	  if(strstr(tmp->arch->name,"table")==NULL &&
	      strstr(tmp->arch->name,"chair")==NULL)
	  { pick_up(op, tmp); if(0)fprintf(stderr,"WEAPON\n"); continue; }
	}
      }

      /* misc stuff that's useful */
      if(op->contr->mode & PU_KEY)
	if (tmp->type == KEY || tmp->type == SPECIAL_KEY)
	{ pick_up(op, tmp); if(0)fprintf(stderr,"KEY\n"); continue; }

      /* any of the last 4 bits set means we use the ratio for value
       * pickups */
      if(op->contr->mode & PU_RATIO)
      {
	/* use value density to decide what else to grab.
	 * >=7 was >= op->contr->mode
	 * >=7 is the old standard setting.  Now we take the last 4 bits
	 * and multiply them by 5, giving 0..15*5== 5..75 */
	wvratio=(op->contr->mode & PU_RATIO) * 5;
	if ((query_cost(tmp, op, F_TRUE)*100 / (tmp->weight * MAX(tmp->nrof, 1))) >= wvratio)
	{
	  pick_up(op, tmp);
	  continue;
	}
      }
    } /* the new pickup model */
  }
  return ! stop;
}

/*  Find an arrow in the inventory and after that
 *  in the right type container (quiver). Pointer to the
 *  found object is returned.
 */
static object *find_arrow(object *op, const char *type)
{
    object *tmp = NULL;

    for(op=op->inv; op; op=op->below)
	if(!tmp && op->type==CONTAINER && op->race==type &&
	   QUERY_FLAG(op,FLAG_APPLIED))
	tmp = find_arrow (op, type);
	else if (op->type==ARROW && op->race==type)
	    return op;
    return tmp;
}

/* Similar to find_arrow, but looks for (roughly) the best arrow to use
 * against the target.  A full test is not performed, simply a basic test
 * of resistances.  The archer is making a quick guess at what he sees down
 * the hall.  Failing that it does it's best to pick the highest plus arrow.
 */

static object *find_better_arrow(object *op, object *target, const char *type, int *better)
{
    object *tmp = NULL, *arrow, *ntmp;
    int attacknum, attacktype, betterby=0, i;

    if (!type)
	return NULL;

    for (arrow=op->inv; arrow; arrow=arrow->below) {
	if (arrow->type==CONTAINER && arrow->race==type &&
	    QUERY_FLAG(arrow, FLAG_APPLIED)) {
	    i = 0;
	    ntmp = find_better_arrow(arrow, target, type, &i);
	    if (i > betterby) {
		tmp = ntmp;
		betterby = i;
	    }
	} else if (arrow->type==ARROW && arrow->race==type) {
	    /* allways prefer assasination/slaying */
	    if (target->race != NULL && arrow->slaying != NULL &&
		strstr(arrow->slaying, target->race)) {
		if (arrow->attacktype & AT_DEATH) {
		    *better = 100;
		    return arrow;
		} else {
		    tmp = arrow;
		    betterby = (arrow->magic + arrow->stats.dam) * 2;
		}
	    } else {
		for (attacknum=0; attacknum < NROFATTACKS; attacknum++) {
		    attacktype = 1<<attacknum;
		    if ((arrow->attacktype & attacktype) && (target->arch->clone.resist[attacknum]) < 0)
			if (((arrow->magic + arrow->stats.dam)*(100-target->arch->clone.resist[attacknum])/100) > betterby) {
			    tmp = arrow;
			    betterby = (arrow->magic + arrow->stats.dam)*(100-target->arch->clone.resist[attacknum])/100;
			}
		}
		if ((2 + arrow->magic + arrow->stats.dam) > betterby) {
		    tmp = arrow;
		    betterby = 2 + arrow->magic + arrow->stats.dam;
		}
		if (arrow->title && (1 + arrow->magic + arrow->stats.dam) > betterby) {
		    tmp = arrow;
		    betterby = 1 + arrow->magic + arrow->stats.dam;
		}
	    }
	}
    }
    if (tmp == NULL && arrow == NULL)
	return find_arrow(op, type);

    *better = betterby;
    return tmp;
}

/* looks in a given direction, finds the first valid target, and calls
 * find_better_arrow to find a decent arrow to use.
 * op = the shooter
 * type = bow->race
 * dir = fire direction
 */

static object *pick_arrow_target(object *op, const char *type, int dir)
{
    object *tmp = NULL;
    mapstruct *m;
    int i, mflags, found, number;
    sint16 x, y;

    if (op->map == NULL)
	return find_arrow(op, type);

    /* do a dex check */
    number = (die_roll(2, 40, op, PREFER_LOW)-2)/2;
    if (number > (op->stats.Dex + (op->chosen_skill?op->chosen_skill->level:op->level)))
	return find_arrow(op, type);

    m = op->map;
    x = op->x;
    y = op->y;

    /* find the first target */
    for (i=0, found=0; i<20; i++) {
	x += freearr_x[dir];
	y += freearr_y[dir];
	mflags = get_map_flags(m, &m, x, y, &x, &y);
	if (mflags & P_OUT_OF_MAP || mflags & P_BLOCKSVIEW) {
	    tmp = NULL;
	    break;
	} else if (GET_MAP_MOVE_BLOCK(m, x, y) == MOVE_FLY_LOW) {
	    /* This block presumes arrows and the like are MOVE_FLY_SLOW -
	     * perhaps a bad assumption.
	     */
	    tmp = NULL;
	    break;
	}
	if (mflags & P_IS_ALIVE) {
	    for (tmp = GET_MAP_OB(m, x, y); tmp; tmp=tmp->above)
		if (QUERY_FLAG(tmp, FLAG_ALIVE)) {
		    found++;
		    break;
		}
	    if (found)
		break;
	}
    }
    if (tmp == NULL)
	return find_arrow(op, type);

    if (tmp->head)
	tmp = tmp->head;

    return find_better_arrow(op, tmp, type, &i);
}

/* Creature fires a bow - op can be monster or player.  Returns
 * 1 if bow was actually fired, 0 otherwise.
 * op is the object firing the bow.
 * part is for multipart creatures - the part firing the bow.
 * dir is the direction of fire.
 * wc_mod is any special modifier to give (used in special player fire modes)
 * sx, sy are coordinates to fire arrow from - also used in some of the special
 * player fire modes.
 */
int fire_bow(object *op, object *part, object *arrow, int dir, int wc_mod,
             sint16 sx, sint16 sy)
{
    object *left, *bow;
    tag_t left_tag, tag;
    int bowspeed, mflags;
    mapstruct	*m;

    if (!dir) {
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "You can't shoot yourself!", NULL);
	return 0;
    }
    if (op->type == PLAYER)
	bow=op->contr->ranges[range_bow];
    else {
	for(bow=op->inv; bow; bow=bow->below)
	    /* Don't check for applied - monsters don't apply bows - in that way, they
	     * don't need to switch back and forth between bows and weapons.
	     */
	    if(bow->type==BOW)
		break;

	if (!bow) {
	    LOG (llevError, "Range: bow without activated bow (%s).\n", op->name);
	    return 0;
	}
    }
    if( !bow->race || !bow->skill) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "Your %s is broken.",
			     "Your %s is broken.",
			     bow->name);
	return 0;
    }

    bowspeed = bow->stats.sp + dex_bonus[op->stats.Dex];

    /* penalize ROF for bestarrow */
    if (op->type == PLAYER && op->contr->bowtype == bow_bestarrow)
	bowspeed -= dex_bonus[op->stats.Dex] + 5;
    if (bowspeed < 1)
	bowspeed = 1;

    if (arrow == NULL) {
	if ((arrow=find_arrow(op, bow->race)) == NULL) {
	    if (op->type == PLAYER)
		draw_ext_info_format(NDI_UNIQUE, 0, op,
				     MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				     "You have no %s left.",
				     "You have no %s left.",
				     bow->race);
	    /* FLAG_READY_BOW will get reset if the monsters picks up some arrows */
	    else
		CLEAR_FLAG(op, FLAG_READY_BOW);
	    return 0;
	}
    }
    mflags = get_map_flags(op->map, &m, sx, sy, &sx, &sy);
    if (mflags & P_OUT_OF_MAP) {
	return 0;
    }
    if (GET_MAP_MOVE_BLOCK(m, sx, sy) & MOVE_FLY_LOW) {
	return 0;
    }

    /* this should not happen, but sometimes does */
    if (arrow->nrof==0) {
	remove_ob(arrow);
	free_object(arrow);
	return 0;
    }

    left = arrow; /* these are arrows left to the player */
    left_tag = left->count;
    arrow = get_split_ob(arrow, 1, NULL, 0);
    if (arrow == NULL) {
	draw_ext_info_format(NDI_UNIQUE, 0, op,
			     MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "You have no %s left.",
			     "You have no %s left.",
			     bow->race);
	return 0;
    }
    set_owner(arrow, op);
    if (arrow->skill) free_string(arrow->skill);
    arrow->skill = add_refcount(bow->skill);

    arrow->direction=dir;
    arrow->x = sx;
    arrow->y = sy;

    if (op->type == PLAYER) {
	op->speed_left = 0.01 - (float)FABS(op->speed) * 100 / bowspeed;
    fix_object(op);
    }

    SET_ANIMATION(arrow, arrow->direction);
    arrow->stats.sp = arrow->stats.wc; /* save original wc and dam */
    arrow->stats.hp = arrow->stats.dam;
    arrow->stats.grace = arrow->attacktype;
    if (arrow->slaying != NULL)
	arrow->spellarg = strdup_local(arrow->slaying);

    /* Note that this was different for monsters - they got their level
     * added to the damage.  I think the strength bonus is more proper.
     */

    arrow->stats.dam += (QUERY_FLAG(bow, FLAG_NO_STRENGTH) ?
	    0 : dam_bonus[op->stats.Str]) +
	    bow->stats.dam + bow->magic + arrow->magic;

    /* update the speed */
    arrow->speed = (float)((QUERY_FLAG(bow, FLAG_NO_STRENGTH) ?
		   0 : dam_bonus[op->stats.Str]) +
		   bow->magic + arrow->magic) / 5.0 +
		(float)bow->stats.dam / 7.0;

    if (arrow->speed < 1.0)
	arrow->speed = 1.0;
    update_ob_speed(arrow);
    arrow->speed_left = 0;

    if (op->type == PLAYER) {
	arrow->stats.wc = 20 - bow->magic - arrow->magic -
	    (op->chosen_skill?op->chosen_skill->level:op->level) -
	    dex_bonus[op->stats.Dex] - thaco_bonus[op->stats.Str] -
	    arrow->stats.wc - bow->stats.wc + wc_mod;

	arrow->level = op->chosen_skill?op->chosen_skill->level:op->level;
    } else {
	arrow->stats.wc= op->stats.wc - bow->magic - arrow->magic -
	    arrow->stats.wc + wc_mod;

	arrow->level = op->level;
    }
    if (arrow->attacktype == AT_PHYSICAL)
	arrow->attacktype |= bow->attacktype;
    if (bow->slaying != NULL)
	arrow->slaying = add_string(bow->slaying);

    arrow->map = m;
    /* If move_type is ever changed, monster.c:monster_use_bow() needs to be changed too. */
    arrow->move_type = MOVE_FLY_LOW;
    arrow->move_on = MOVE_FLY_LOW | MOVE_WALK;

    play_sound_map(op->map, op->x, op->y, SOUND_FIRE_ARROW);
    tag = arrow->count;
    insert_ob_in_map(arrow, m, op, 0);

    if (!was_destroyed(arrow, tag))
	ob_process(arrow);

    if (op->type == PLAYER) {
	if (was_destroyed (left, left_tag))
	    esrv_del_item(op->contr, left_tag);
	else
	    esrv_send_item(op, left);
    }
    return 1;
}

/* Special fire code for players - this takes into
 * account the special fire modes players can have
 * but monsters can't.  Putting that code here
 * makes the fire_bow code much cleaner.
 * this function should only be called if 'op' is a player,
 * hence the function name.
 */
static int player_fire_bow(object *op, int dir)
{
    int ret=0, wcmod=0;

    if (op->contr->bowtype == bow_bestarrow) {
	ret = fire_bow(op, op,
	    pick_arrow_target(op, op->contr->ranges[range_bow]->race, dir),
	    dir, 0, op->x, op->y);
    } else if (op->contr->bowtype >= bow_n && op->contr->bowtype <= bow_nw) {
	if (!similar_direction(dir, op->contr->bowtype - bow_n + 1))
	    wcmod =-1;
	ret = fire_bow(op, op, NULL, op->contr->bowtype - bow_n + 1, wcmod,
	    op->x, op->y);
    } else if (op->contr->bowtype == bow_threewide) {
	ret = fire_bow(op, op, NULL, dir, 0, op->x, op->y);
	ret |= fire_bow(op, op, NULL, dir, -5, op->x + freearr_x[absdir(dir+2)], op->y + freearr_y[absdir(dir+2)]);
	ret |= fire_bow(op, op, NULL, dir, -5, op->x + freearr_x[absdir(dir-2)], op->y + freearr_y[absdir(dir-2)]);
    } else if (op->contr->bowtype == bow_spreadshot) {
	ret |= fire_bow(op, op, NULL, dir, 0, op->x, op->y);
	ret |= fire_bow(op, op, NULL, absdir(dir-1), -5, op->x, op->y);
	ret |= fire_bow(op, op, NULL, absdir(dir+1), -5, op->x, op->y);

    } else {
	/* Simple case */
	ret = fire_bow(op, op, NULL, dir, 0, op->x, op->y);
    }
    return ret;
}


/**
 * Fires a misc (wand/rod/horn) object in 'dir'.
 * Broken apart from 'fire' to keep it more readable.
 *
 * @param op
 * player firing.
 * @param dir
 * firing direction.
 *
 * @warning
 * op must be a player (contr != NULL).
 */
static void fire_misc_object(object *op, int dir)
{
    object  *item;
    char name[MAX_BUF];

    if (!op->contr->ranges[range_misc]) {
        draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "You have no range item readied.", NULL);
        return;
    }

    item = op->contr->ranges[range_misc];
    if (!item->inv) {
        LOG(llevError,"Object %s lacks a spell\n", item->name);
        return;
    }
    if (item->type == WAND) {
        if(item->stats.food<=0) {
            play_sound_player_only(op->contr, SOUND_WAND_POOF,0,0);
            query_base_name(item, 0, name, MAX_BUF);
            draw_ext_info_format(NDI_UNIQUE, 0,op,
                MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
                "The %s goes poof.",
                "The %s goes poof.",
                name);
            return;
        }
    } else if (item->type == ROD || item->type==HORN) {
        if(item->stats.hp<SP_level_spellpoint_cost(item, item->inv, SPELL_HIGHEST)) {
            play_sound_player_only(op->contr, SOUND_WAND_POOF,0,0);
            query_base_name(item,0, name, MAX_BUF);
            if (item->type== ROD)
                draw_ext_info_format(NDI_UNIQUE, 0,op,
                    MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
                    "The %s whines for a while, but nothing happens.",
                    "The %s whines for a while, but nothing happens.",
                    name);
            else
                draw_ext_info_format(NDI_UNIQUE, 0,op,
                    MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
                    "The %s needs more time to charge.",
                    "The %s needs more time to charge.",
                    name);
            return;
        }
    }

    if(cast_spell(op,item,dir,item->inv,NULL)) {
        SET_FLAG(op, FLAG_BEEN_APPLIED); /* You now know something about it */
        if (item->type == WAND) {
            if (!(--item->stats.food)) {
                object *tmp;
                if (item->arch) {
                    CLEAR_FLAG(item, FLAG_ANIMATE);
                    item->face = item->arch->clone.face;
                    item->speed = 0;
                    update_ob_speed(item);
                }
                if ((tmp=get_player_container(item)))
                    esrv_update_item(UPD_ANIM, tmp, item);
            }
        }
        else if (item->type == ROD || item->type==HORN) {
            drain_rod_charge(item);
        }
    }
}

/* Received a fire command for the player - go and do it.
 */
void fire(object *op,int dir) {
    int spellcost=0;

    /* check for loss of invisiblity/hide */
    if (action_makes_visible(op)) make_visible(op);

    switch(op->contr->shoottype) {
	case range_none:
	    return;

	case range_bow:
	    player_fire_bow(op, dir);
	    return;

	case range_magic: /* Casting spells */
        spellcost=(cast_spell(op,op,dir,op->contr->ranges[range_magic],op->contr->spellparam[0]?op->contr->spellparam:NULL));
	    return;

	case range_misc:
	    fire_misc_object(op, dir);
	    return;

	case range_golem: /* Control summoned monsters from scrolls */
	    if(op->contr->ranges[range_golem]==NULL ||
	       op->contr->golem_count != op->contr->ranges[range_golem]->count) {
		op->contr->ranges[range_golem] = NULL;
		op->contr->shoottype=range_none;
		op->contr->golem_count = 0;
	    }
	    else
		control_golem(op->contr->ranges[range_golem], dir);
	    return;

	case range_skill:
	    if(!op->chosen_skill) {
		if(op->type==PLAYER)
		    draw_ext_info(NDI_UNIQUE, 0,op,
				  MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				  "You have no applicable skill to use.", NULL);
		return;
	    }
	    (void) do_skill(op,op,op->chosen_skill,dir,NULL);
	    return;

	case range_builder:
	    apply_map_builder( op, dir );
	    return;

	default:
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			  "Illegal shoot type.", NULL);
	    return;
    }
}



/* We try to find a key for the door as passed.  If we find a key
 * and successfully use it, we return the key, otherwise NULL
 * This function merges both normal and locked door, since the logic
 * for both is the same - just the specific key is different.
 * pl is the player,
 * inv is the objects inventory to searched
 * door is the door we are trying to match against.
 * This function can be called recursively to search containers.
 */

object * find_key(object *pl, object *container, object *door)
{
    object *tmp,*key;

    /* Should not happen, but sanity checking is never bad */
    if (container->inv == NULL) return NULL;

    /* First, lets try to find a key in the top level inventory */
    for (tmp=container->inv; tmp!=NULL; tmp=tmp->below) {
	if (door->type==DOOR && tmp->type==KEY) break;
	/* For sanity, we should really check door type, but other stuff
	 * (like containers) can be locked with special keys
	 */
	if (tmp->slaying && tmp->type==SPECIAL_KEY &&
	    tmp->slaying==door->slaying) break;
    }
    /* No key found - lets search inventories now */
    /* If we find and use a key in an inventory, return at that time.
     * otherwise, if we search all the inventories and still don't find
     * a key, return
     */
    if (!tmp) {
	for (tmp=container->inv; tmp!=NULL; tmp=tmp->below) {
	    /* No reason to search empty containers */
	    if (tmp->type==CONTAINER && tmp->inv) {
		if ((key=find_key(pl, tmp, door))!=NULL) return key;
	    }
	}
	if (!tmp) return NULL;
    }
    /* We get down here if we have found a key.  Now if its in a container,
     * see if we actually want to use it
     */
    if (pl!=container) {
	/* Only let players use keys in containers */
	if (!pl->contr) return NULL;
	/* cases where this fails:
	 * If we only search the player inventory, return now since we
	 * are not in the players inventory.
	 * If the container is not active, return now since only active
	 * containers can be used.
	 * If we only search keyrings and the container does not have
	 * a race/isn't a keyring.
	 * No checking for all containers - to fall through past here,
	 * inv must have been an container and must have been active.
	 *
	 * Change the color so that the message doesn't disappear with
	 * all the others.
	 */
	if (pl->contr->usekeys == key_inventory ||
	    !QUERY_FLAG(container, FLAG_APPLIED) ||
	    (pl->contr->usekeys == keyrings &&
	     (!container->race || strcmp(container->race, "keys")))
	      ) {
        char name_tmp[MAX_BUF], name_cont[MAX_BUF];
        query_name(tmp, name_tmp, MAX_BUF);
        query_name(container, name_cont, MAX_BUF);
	    draw_ext_info_format(NDI_UNIQUE|NDI_BROWN, 0, pl,
			 MSG_TYPE_ITEM, MSG_TYPE_ITEM_INFO,
			 "The %s in your %s vibrates as you approach the door",
			 "The %s in your %s vibrates as you approach the door",
			 name_tmp, name_cont);
	    return NULL;
	}
    }
    return tmp;
}

/* moved door processing out of move_player_attack.
 * returns 1 if player has opened the door with a key
 * such that the caller should not do anything more,
 * 0 otherwise
 */
static int player_attack_door(object *op, object *door)
{

    /* If its a door, try to find a use a key.  If we do destroy the door,
     * might as well return immediately as there is nothing more to do -
     * otherwise, we fall through to the rest of the code.
     */
    object *key=find_key(op, op, door);

    /* IF we found a key, do some extra work */
    if (key) {
	object *container=key->env;

	play_sound_map(op->map, op->x, op->y, SOUND_OPEN_DOOR);
	if(action_makes_visible(op)) make_visible(op);
	if(door->inv &&(door->inv->type ==RUNE || door->inv->type ==TRAP)) spring_trap(door->inv,op);
	if (door->type == DOOR) {
	    hit_player(door,9998,op,AT_PHYSICAL,1); /* Break through the door */
	}
	else if(door->type==LOCKED_DOOR) {
        char name[HUGE_BUF];
        query_short_name(key, name, HUGE_BUF);
	    draw_ext_info_format(NDI_UNIQUE, NDI_BROWN, op,
				 MSG_TYPE_ITEM, MSG_TYPE_ITEM_REMOVE,
				 "You open the door with the %s",
				 "You open the door with the %s",
				 name);
	    remove_door2(door); /* remove door without violence ;-) */
	}
	/* Do this after we print the message */
	decrease_ob(key); /* Use up one of the keys */
	/* Need to update the weight the container the key was in */
	if (container != op)
	    esrv_update_item(UPD_WEIGHT, op, container);
	return 1; /* Nothing more to do below */
    } else if (door->type==LOCKED_DOOR) {
	/* Might as well return now - no other way to open this */
	 draw_ext_info(NDI_UNIQUE | NDI_NAVY, 0, op,
		       MSG_TYPE_ATTACK, MSG_TYPE_ATTACK_NOKEY,
		       door->msg, door->msg);
	return 1;
    }
    return 0;
}

/* This function is just part of a breakup from move_player.
 * It should keep the code cleaner.
 * When this is called, the players direction has been updated
 * (taking into account confusion.)  The player is also actually
 * going to try and move (not fire weapons).
 */

void move_player_attack(object *op, int dir)
{
    object *tmp, *mon, *tpl, *mon_owner;
    sint16 nx, ny;
    int on_battleground;
    mapstruct *m;

    if (op->contr->transport) tpl = op->contr->transport;
    else tpl = op;
    nx=freearr_x[dir]+tpl->x;
    ny=freearr_y[dir]+tpl->y;

    on_battleground = op_on_battleground(tpl, NULL, NULL);

    /* If braced, or can't move to the square, and it is not out of the
     * map, attack it.  Note order of if statement is important - don't
     * want to be calling move_ob if braced, because move_ob will move the
     * player.  This is a pretty nasty hack, because if we could
     * move to some space, it then means that if we are braced, we should
     * do nothing at all.  As it is, if we are braced, we go through
     * quite a bit of processing.  However, it probably is less than what
     * move_ob uses.
     */
    if ((op->contr->braced || !move_ob(tpl,dir,tpl)) && !out_of_map(tpl->map,nx,ny)) {
	if (OUT_OF_REAL_MAP(tpl->map, nx, ny)) {
	    m = get_map_from_coord(tpl->map, &nx, &ny);
	    if (!m) return; /* Don't think this should happen */
	}
	else m =tpl->map;

	if ((tmp=get_map_ob(m,nx,ny))==NULL) {
	    /*	LOG(llevError,"player_move_attack: get_map_ob returns NULL, but player can not more there.\n");*/
	    return;
	}

	mon = NULL;
        /* Go through all the objects, and find ones of interest. Only stop if
         * we find a monster - that is something we know we want to attack.
         * if its a door or barrel (can roll) see if there may be monsters
         * on the space
         */
        while (tmp!=NULL)
        {
            if (tmp == op)
            {
                tmp=tmp->above;
                continue;
            }
            if (QUERY_FLAG(tmp,FLAG_ALIVE))
            {
                mon = tmp;
                /* Gros: Objects like (pass-through) doors are alive, but haven't
                 * their monster flag set - so this is a good way attack real
                 * monsters in priority.
                 */
                if (QUERY_FLAG(tmp, FLAG_MONSTER))
                    break;
            }
            if (tmp->type==LOCKED_DOOR || QUERY_FLAG(tmp,FLAG_CAN_ROLL))
                mon = tmp;
            tmp=tmp->above;
        }

	if (mon==NULL)		/* This happens anytime the player tries to move */
	    return;		/* into a wall */

	if(mon->head != NULL)
	    mon = mon->head;

	if ((mon->type==DOOR && mon->stats.hp>=0) || (mon->type==LOCKED_DOOR))
	    if (player_attack_door(op, mon)) return;

	/* The following deals with possibly attacking peaceful
	 * or frienddly creatures.  Basically, all players are considered
	 * unaggressive.  If the moving player has peaceful set, then the
	 * object should be pushed instead of attacked.  It is assumed that
	 * if you are braced, you will not attack friends accidently,
	 * and thus will not push them.
	 */

	/* If the creature is a pet, push it even if the player is not
	 * peaceful.  Our assumption is the creature is a pet if the
	 * player owns it and it is either friendly or unagressive.
	 */
	mon_owner = get_owner(mon);
	if ((op->type==PLAYER) &&
	    (mon_owner == op || (mon_owner != NULL && mon_owner->type == PLAYER && mon_owner->contr->party != NULL && mon_owner->contr->party == op->contr->party)) &&
	    (QUERY_FLAG(mon,FLAG_UNAGGRESSIVE) ||  QUERY_FLAG(mon, FLAG_FRIENDLY)))
	{
	    /* If we're braced, we don't want to switch places with it */
	    if (op->contr->braced) return;
	    play_sound_map(tpl->map, tpl->x, tpl->y, SOUND_PUSH_PLAYER);
	    (void) push_ob(mon,dir,op);
	    if(op->contr->tmp_invis||op->hide) make_visible(op);
	    return;
	}

	/* in certain circumstances, you shouldn't attack friendly
	 * creatures.  Note that if you are braced, you can't push
	 * someone, but put it inside this loop so that you won't
	 * attack them either.
	 */
	if ((mon->type==PLAYER || mon->enemy != op) &&
	    (mon->type==PLAYER || QUERY_FLAG(mon,FLAG_UNAGGRESSIVE) || QUERY_FLAG(mon, FLAG_FRIENDLY)) &&
	    (op->contr->peaceful && !on_battleground)) {
	    if (!op->contr->braced) {
		play_sound_map(tpl->map, tpl->x, tpl->y, SOUND_PUSH_PLAYER);
		(void) push_ob(mon,dir,op);
	    } else {
		draw_ext_info(0, 0,op,MSG_TYPE_ATTACK, MSG_TYPE_ATTACK_NOATTACK,
			      "You withhold your attack", NULL);
	    }
	    if(op->contr->tmp_invis||op->hide) make_visible(op);
	}

	/* If the object is a boulder or other rollable object, then
	 * roll it if not braced.  You can't roll it if you are braced.
	 */
	else if(QUERY_FLAG(mon,FLAG_CAN_ROLL)&&(!op->contr->braced)) {
	    recursive_roll(mon,dir,op);
	    if(action_makes_visible(op)) make_visible(op);
	}

	/* Any generic living creature.  Including things like doors.
	 * Way it works is like this:  First, it must have some hit points
	 * and be living.  Then, it must be one of the following:
	 * 1) Not a player, 2) A player, but of a different party.  Note
	 * that party_number -1 is no party, so attacks can still happen.
	 */

	else if ((mon->stats.hp>=0) && QUERY_FLAG(mon, FLAG_ALIVE) &&
 		 ((mon->type!=PLAYER || op->contr->party==NULL ||
 		   op->contr->party!=mon->contr->party))) {

	    /* If the player hasn't hit something this tick, and does
	     * so, give them speed boost based on weapon speed.  Doing
	     * it here is better than process_players2, which basically
	     * incurred a 1 tick offset.
	     */
	    if (!op->contr->has_hit) {
		op->speed_left += op->speed / op->contr->weapon_sp;

		op->contr->has_hit = 1; /* The last action was to hit, so use weapon_sp */
	    }

	    skill_attack(mon, op, 0, NULL, NULL);

	    /* If attacking another player, that player gets automatic
	     * hitback, and doesn't loose luck either.
	     * Disable hitback on the battleground or if the target is
	     * the wiz.
	     */
	    if (mon->type == PLAYER && mon->stats.hp >= 0 && !mon->contr->has_hit &&
		!on_battleground && !QUERY_FLAG(mon, FLAG_WIZ)) {
		short luck = mon->stats.luck;
		mon->contr->has_hit = 1;
		skill_attack(op, mon, 0, NULL, NULL);
		mon->stats.luck = luck;
	    }
	    if(action_makes_visible(op)) make_visible(op);
	}
    } /* if player should attack something */
}

int move_player(object *op,int dir) {
    int pick;
    object *transport = op->contr->transport;

    if(!transport && (op->map == NULL || op->map->in_memory != MAP_IN_MEMORY))
	return 0;

    /* Sanity check: make sure dir is valid */
    if ( ( dir < 0 ) || ( dir >= 9 ) ) {
        LOG( llevError, "move_player: invalid direction %d\n", dir);
        return 0;
    }

    /* peterm:  added following line */
    if(QUERY_FLAG(op,FLAG_CONFUSED) && dir)
	dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

    op->facing = dir;

    if(!transport && op->hide) do_hidden_move(op);

    if (transport) {
	/* transport->contr is set up for the person in charge of the boat.
	 * if that isn't this person, he can't steer it, etc
	 */
	if (transport->contr != op->contr) return 0;

	/* Transport can't move.  But update dir so it at least
	 * will point in the same direction if player is running.
	 */
	if (transport->speed_left < 0.0) {
	    transport->direction = dir;
	    op->direction = dir;
	    return 0;
	}
	/* Remove transport speed.  Give player just a little speed -
	 * enough so that they will get an action again quickly.
	 */
	transport->speed_left -= 1.0;
	if (op->speed_left < 0.0) op->speed_left = -0.01;

    }

    if(op->contr->fire_on) {
	fire(op,dir);
    }
    else move_player_attack(op,dir);

    pick = check_pick(op);


    /* Add special check for newcs players and fire on - this way, the
     * server can handle repeat firing.
     */
    if (op->contr->fire_on || (op->contr->run_on && pick!=0)) {
	op->direction = dir;
    } else {
	op->direction=0;
    }
    /* Update how the player looks.  Use the facing, so direction may
     * get reset to zero.  This allows for full animation capabilities
     * for players.
     */
    if (!transport) animate_object(op, op->facing);
    return 0;
}

/* This is similar to handle_player, below, but is only used by the
 * new client/server stuff.
 * This is sort of special, in that the new client/server actually uses
 * the new speed values for commands.
 *
 * Returns true if there are more actions we can do.
 */
int handle_newcs_player(object *op)
{
    if (op->contr->hidden) {
	op->invisible = 1000;
	/* the socket code flasehs the player visible/invisible
	 * depending on the value if invisible, so we need to
	 * alternate it here for it to work correctly.
	 */
	if (pticks & 2) op->invisible--;
    }
    else if(op->invisible&&!(QUERY_FLAG(op,FLAG_MAKE_INVIS))) {
	op->invisible--;
	if(!op->invisible) {
	    make_visible(op);
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_END,
			  "Your invisibility spell runs out.", NULL);
	}
    }

    if (QUERY_FLAG(op, FLAG_SCARED)) {
	flee_player(op);
	/* If player is still scared, that is his action for this tick */
	if (QUERY_FLAG(op, FLAG_SCARED)) {
	    op->speed_left--;
	    return 0;
	}
    }

    /* I've been seeing crashes where the golem has been destroyed, but
     * the player object still points to the defunct golem.  The code that
     * destroys the golem looks correct, and it doesn't always happen, so
     * put this in a a workaround to clean up the golem pointer.
     */
    if (op->contr->ranges[range_golem] &&
	((op->contr->golem_count != op->contr->ranges[range_golem]->count) ||
	 QUERY_FLAG(op->contr->ranges[range_golem], FLAG_REMOVED))) {
	op->contr->ranges[range_golem] = NULL;
	op->contr->golem_count = 0;
    }

    /* call this here - we also will call this in do_ericserver, but
     * the players time has been increased when doericserver has been
     * called, so we recheck it here.
     */
    handle_client(&op->contr->socket, op->contr);
    if (op->speed_left<0) return 0;

    if(op->direction && (op->contr->run_on || op->contr->fire_on)) {
	/* All move commands take 1 tick, at least for now */
	op->speed_left--;

	/* Instead of all the stuff below, let move_player take care
	 * of it.  Also, some of the skill stuff is only put in
	 * there, as well as the confusion stuff.
	 */
	move_player(op, op->direction);
	if (op->speed_left>0) return 1;
	else return 0;
    }
    return 0;
}

/* Returns 1 if player had his life saved by an item.
 * In this case, first item saving life is removed.
 */
static int save_life(object *op) {
    object *tmp;

    if(!QUERY_FLAG(op,FLAG_LIFESAVE))
	return 0;

    for(tmp=op->inv;tmp!=NULL;tmp=tmp->below)
	if(QUERY_FLAG(tmp, FLAG_APPLIED)&&QUERY_FLAG(tmp,FLAG_LIFESAVE)) {
        char name[MAX_BUF];
        query_name(tmp, name, MAX_BUF);
	    play_sound_map(op->map, op->x, op->y, SOUND_OB_EVAPORATE);
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_ITEM, MSG_TYPE_ITEM_REMOVE,
				 "Your %s vibrates violently, then evaporates.",
				 "Your %s vibrates violently, then evaporates.",
				 name);
	    if (op->contr)
			esrv_del_item(op->contr, tmp->count);
	    remove_ob(tmp);
	    free_object(tmp);
	    CLEAR_FLAG(op, FLAG_LIFESAVE);
	    if(op->stats.hp<0)
		op->stats.hp = op->stats.maxhp;
	    if(op->stats.food<0)
		op->stats.food = 999;
        fix_object(op);
	    return 1;
	}
    LOG(llevError,"Error: LIFESAVE set without applied object.\n");
    CLEAR_FLAG(op, FLAG_LIFESAVE);
    enter_player_savebed(op); /* bring him home. */
    return 0;
}

/* This goes throws the inventory and removes unpaid objects, and puts them
 * back in the map (location and map determined by values of env).  This
 * function will descend into containers.  op is the object to start the search
 * from.
 */
static void remove_unpaid_objects(object *op, object *env)
{
    object *next;

    while (op) {
	next=op->below;	/* Make sure we have a good value, in case
			 * we remove object 'op'
			 */
	if (QUERY_FLAG(op, FLAG_UNPAID)) {
	    remove_ob(op);
	    op->x = env->x;
	    op->y = env->y;
	    if (env->type == PLAYER)
		esrv_del_item(env->contr, op->count);
	    insert_ob_in_map(op, env->map, NULL,0);
	}
	else if (op->inv) remove_unpaid_objects(op->inv, env);
	op=next;
    }
}


/* Returns pointer a static string containing gravestone text
 * Moved from apply.c to player.c - player.c is what
 * actually uses this function.  player.c may not be quite the
 * best, a misc file for object actions is probably better,
 * but there isn't one in the server directory.
 */
static const char *gravestone_text (object *op)
{
    static char buf2[MAX_BUF];
    char buf[MAX_BUF];
    time_t now = time (NULL);

    strcpy (buf2, "                 R.I.P.\n\n");
    if (op->type == PLAYER)
        sprintf (buf, "%s the %s\n", op->name, op->contr->title);
    else
        sprintf (buf, "%s\n", op->name);
    strncat (buf2, "                    ",  20 - strlen (buf) / 2);
    strcat (buf2, buf);
    if (op->type == PLAYER)
        sprintf (buf, "who was in level %d when killed\n", op->level);
    else
        sprintf (buf, "who was in level %d when died.\n\n", op->level);
    strncat (buf2, "                    ",  20 - strlen (buf) / 2);
    strcat (buf2, buf);
    if (op->type == PLAYER) {
        sprintf (buf, "by %s.\n\n", op->contr->killer);
        strncat (buf2, "                    ",  21 - strlen (buf) / 2);
        strcat (buf2, buf);
    }
    strftime (buf, MAX_BUF, "%b %d %Y\n", localtime (&now));
    strncat (buf2, "                    ",  20 - strlen (buf) / 2);
    strcat (buf2, buf);
    return buf2;
}

/* Regenerate hp/sp/gr, decreases food. This only works for players.
 * Will grab food if needed, or kill player.
 */
void do_some_living(object *op) {
  int last_food=op->stats.food;
  int gen_hp, gen_sp, gen_grace;
  int over_hp, over_sp, over_grace;
  int i;
  int rate_hp = 1200;
  int rate_sp = 2500;
  int rate_grace = 2000;
  const int max_hp = 1;
  const int max_sp = 1;
  const int max_grace = 1;

  if (op->contr->outputs_sync) {
    for (i=0; i<NUM_OUTPUT_BUFS; i++)
      if (op->contr->outputs[i].buf!=NULL &&
	(op->contr->outputs[i].first_update+op->contr->outputs_sync)<pticks)
	    flush_output_element(op, &op->contr->outputs[i]);
  }

  if(op->contr->state==ST_PLAYING) {

    /* these next three if clauses make it possible to SLOW DOWN
       hp/grace/spellpoint regeneration. */
    if(op->contr->gen_hp >= 0 )
      gen_hp=(op->contr->gen_hp+1)*op->stats.maxhp;
    else {
      gen_hp = op->stats.maxhp;
      rate_hp -= rate_hp/2 * op->contr->gen_hp;
    }
    if(op->contr->gen_sp >= 0 )
      gen_sp=(op->contr->gen_sp+1)*op->stats.maxsp;
    else {
      gen_sp = op->stats.maxsp;
      rate_sp -= rate_sp/2 * op->contr->gen_sp;
    }
    if(op->contr->gen_grace >= 0)
      gen_grace=(op->contr->gen_grace+1)*op->stats.maxgrace;
    else {
      gen_grace = op->stats.maxgrace;
      rate_grace -= rate_grace/2 * op->contr->gen_grace;
    }

    /* Regenerate Spell Points */
    if(op->contr->ranges[range_golem]==NULL && --op->last_sp<0) {
      gen_sp = gen_sp * 10 / (op->contr->gen_sp_armour < 10? 10 : op->contr->gen_sp_armour);
      if(op->stats.sp<op->stats.maxsp) {
	op->stats.sp++;
 	/* dms do not consume food */
 	if (!QUERY_FLAG(op,FLAG_WIZ)) {
 	   op->stats.food--;
 	   if(op->contr->digestion<0)
 	     op->stats.food+=op->contr->digestion;
 	   else if(op->contr->digestion>0 &&
  		random_roll(0, op->contr->digestion, op, PREFER_HIGH))
 	     op->stats.food=last_food;
         }
      }
      if (max_sp>1) {
	over_sp = (gen_sp+10)/rate_sp;
	if (over_sp > 0) {
	  if(op->stats.sp<op->stats.maxsp) {
	    op->stats.sp += over_sp>max_sp ? max_sp : over_sp;
	    if(random_roll(0, rate_sp-1, op, PREFER_LOW) > ((gen_sp+10)%rate_sp))
	      op->stats.sp--;
	    if(op->stats.sp>op->stats.maxsp)
	      op->stats.sp=op->stats.maxsp;
	  }
	  op->last_sp=0;
	} else {
	  op->last_sp=rate_sp/(gen_sp<20 ? 30 : gen_sp+10);
	}
      } else {
	op->last_sp=rate_sp/(gen_sp<20 ? 30 : gen_sp+10);
      }
    }

    /* Regenerate Grace */
    /* I altered this a little - maximum grace is ony achieved through prayer -b.t.*/
    if(--op->last_grace<0) {
	if(op->stats.grace<op->stats.maxgrace/2)
	    op->stats.grace++; /* no penalty in food for regaining grace */
	if(max_grace>1) {
	    over_grace = (gen_grace<20 ? 30 : gen_grace+10)/rate_grace;
	    if (over_grace > 0) {
		op->stats.sp += over_grace
		    + (random_roll(0, rate_grace-1, op, PREFER_HIGH) > ((gen_grace<20 ? 30 : gen_grace+10)%rate_grace))? -1 : 0;
		op->last_grace=0;
	    } else {
		op->last_grace=rate_grace/(gen_grace<20 ? 30 : gen_grace+10);
	    }
	} else {
	    op->last_grace=rate_grace/(gen_grace<20 ? 30 : gen_grace+10);
	}
      /* wearing stuff doesn't detract from grace generation. */
    }

    /* Regenerate Hit Points (unless you are a wraith player) */
    if(--op->last_heal<0 && !is_wraith_pl(op)) {
      if(op->stats.hp<op->stats.maxhp) {
	op->stats.hp++;
 	/* dms do not consume food */
 	if (!QUERY_FLAG(op,FLAG_WIZ)) {
 	   op->stats.food--;
 	   if(op->contr->digestion<0)
 	     op->stats.food+=op->contr->digestion;
 	   else if(op->contr->digestion>0 &&
 		  random_roll(0, op->contr->digestion, op, PREFER_HIGH))
 	     op->stats.food=last_food;
         }
      }
      if(max_hp>1 && !is_wraith_pl(op)) {
	over_hp = (gen_hp<20 ? 30 : gen_hp+10)/rate_hp;
	if (over_hp > 0) {
	  op->stats.sp += over_hp
	    + (RANDOM()%rate_hp > ((gen_hp<20 ? 30 : gen_hp+10)%rate_hp))? -1 : 0;
	  op->last_heal=0;
	} else {
	  op->last_heal=rate_hp/(gen_hp<20 ? 30 : gen_hp+10);
	}
      } else {
	op->last_heal=rate_hp/(gen_hp<20 ? 30 : gen_hp+10);
      }
    }

    /* Digestion */
    if(--op->last_eat<0) {
      int bonus=op->contr->digestion>0?op->contr->digestion:0,
	penalty=op->contr->digestion<0?-op->contr->digestion:0;
      if(op->contr->gen_hp > 0)
	op->last_eat=25*(1+bonus)/(op->contr->gen_hp+penalty+1);
      else
	op->last_eat=25*(1+bonus)/(penalty +1);
    /* dms do not consume food */
    if (!QUERY_FLAG(op,FLAG_WIZ)) op->stats.food--;
    }
  }

    if(op->contr->state==ST_PLAYING&&op->stats.food<0&&op->stats.hp>=0) {
         if (is_wraith_pl(op))
            draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_ITEM, MSG_TYPE_ITEM_REMOVE,"You feel a hunger for living flesh.", NULL);
         else {
	object *tmp, *flesh=NULL;

	for(tmp=op->inv;tmp!=NULL;tmp=tmp->below) {
	    if(!QUERY_FLAG(tmp, FLAG_UNPAID)) {
		if (tmp->type==FOOD || tmp->type==DRINK || tmp->type==POISON) {
		    draw_ext_info(NDI_UNIQUE, 0,op,
				  MSG_TYPE_ITEM, MSG_TYPE_ITEM_REMOVE,
				  "You blindly grab for a bite of food.", NULL);
		    manual_apply(op,tmp,0);
		    if(op->stats.food>=0||op->stats.hp<0)
			break;
		}
		else if (tmp->type==FLESH) flesh=tmp;
	    } /* End if paid for object */
	} /* end of for loop */
	/* If player is still starving, it means they don't have any food, so
	 * eat flesh instead.
	 */
	if (op->stats.food<0 && op->stats.hp>=0 && flesh) {
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_ITEM, MSG_TYPE_ITEM_REMOVE,
			  "You blindly grab for a bite of food.", NULL);
	    manual_apply(op,flesh,0);
	}
         } /* end not wraith */
    } /* end if player is starving */

    while(op->stats.food<0&&op->stats.hp>0)
	op->stats.food++,op->stats.hp--;

    if (!op->contr->state&&!QUERY_FLAG(op,FLAG_WIZ)&&(op->stats.hp<0||op->stats.food<0))
	kill_player(op);
}


static void loot_object(object *op) { /* Grab and destroy some treasure */
    object *tmp,*tmp2,*next;

    if (op->container) { /* close open sack first */
        esrv_apply_container (op, op->container);
    }

    for(tmp=op->inv;tmp!=NULL;tmp=next) {
        next=tmp->below;
        if (tmp->type==EXPERIENCE || tmp->invisible) continue;
        remove_ob(tmp);
        tmp->x=op->x,tmp->y=op->y;
        if (tmp->type == CONTAINER) { /* empty container to ground */
            loot_object(tmp);
        }
        if(!QUERY_FLAG(tmp, FLAG_UNIQUE) && (QUERY_FLAG(tmp, FLAG_STARTEQUIP)
            || QUERY_FLAG(tmp,FLAG_NO_DROP) || !(RANDOM()%3))) {
                if(tmp->nrof>1) {
                    tmp2=get_split_ob(tmp,1+RANDOM()%(tmp->nrof-1), NULL, 0);
                    free_object(tmp2);
                    insert_ob_in_map(tmp,op->map,NULL,0);
                } else
                    free_object(tmp);
            } else
                insert_ob_in_map(tmp,op->map,NULL,0);
    }
}


/* If the player should die (lack of hp, food, etc), we call this.
 * op is the player in jeopardy.  If the player can not be saved (not
 * permadeath, no lifesave), this will take care of removing the player
 * file.
 */
void kill_player(object *op)
{
    char buf[MAX_BUF];
    int x,y,i;
    mapstruct *map;  /*  this is for resurrection */
    int z;
    int num_stats_lose;
    int lost_a_stat;
    int lose_this_stat;
    int this_stat;
    int will_kill_again;
    archetype *at;
    object *tmp;

    if(save_life(op))
	return;


    /* If player dies on BATTLEGROUND, no stat/exp loss! For Combat-Arenas
     * in cities ONLY!!! It is very important that this doesn't get abused.
     * Look at op_on_battleground() for more info       --AndreasV
     */
    if (op_on_battleground(op, &x, &y)) {
	draw_ext_info(NDI_UNIQUE | NDI_NAVY, 0,op,
		      MSG_TYPE_VICTIM,  MSG_TYPE_VICTIM_DIED,
		      "You have been defeated in combat!\n"
		      "Local medics have saved your life...",
		      NULL);

	/* restore player */
	at = find_archetype("poisoning");
        tmp=present_arch_in_ob(at,op);
	if (tmp) {
	    remove_ob(tmp);
	    free_object(tmp);
	    draw_ext_info(NDI_UNIQUE, 0,op,
			  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
			  "Your body feels cleansed", NULL);
	}

	at = find_archetype("confusion");
        tmp=present_arch_in_ob(at,op);
	if (tmp) {
	    remove_ob(tmp);
	    free_object(tmp);
            draw_ext_info(NDI_UNIQUE, 0,tmp,
			  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
			  "Your mind feels clearer", NULL);
	}

	cure_disease(op,0);  /* remove any disease */
	op->stats.hp=op->stats.maxhp;
	if (op->stats.food<=0) op->stats.food=999;

	/* create a bodypart-trophy to make the winner happy */
	tmp=arch_to_object(find_archetype("finger"));
        if (tmp != NULL)
        {
	    sprintf(buf,"%s's finger",op->name);
	    tmp->name = add_string(buf);
	    sprintf(buf,"  This finger has been cut off %s\n"
	            "  the %s, when he was defeated at\n  level %d by %s.\n",
	        op->name, op->contr->title, (int)(op->level),
	        op->contr->killer);
	    tmp->msg=add_string(buf);
	    tmp->value=0, tmp->material=0, tmp->type=0;
	    tmp->materialname = NULL;
	    tmp->x = op->x, tmp->y = op->y;
	    insert_ob_in_map(tmp,op->map,op,0);
	}

	/* teleport defeated player to new destination*/
	transfer_ob(op, x, y, 0, NULL);
	op->contr->braced=0;
	return;
    }

    /* Lauwenmark: Handle for plugin death event */
    if (execute_event(op, EVENT_DEATH,NULL,NULL,NULL,SCRIPT_FIX_ALL) != 0)
            return;

    /* Lauwenmark: Handle for the global death event */
    execute_global_event(EVENT_PLAYER_DEATH, op);
    if(op->stats.food<0) {
	if (op->contr->explore) {
	    draw_ext_info(NDI_UNIQUE, 0,op,
			  MSG_TYPE_VICTIM, MSG_TYPE_VICTIM_DIED,
			  "You would have starved, but you are "
			  "in explore mode, so...", NULL);
	    op->stats.food=999;
	    return;
	}
	sprintf(buf,"%s starved to death.",op->name);
	strcpy(op->contr->killer,"starvation");
    }
    else {
	if (op->contr->explore) {
	    draw_ext_info(NDI_UNIQUE, 0,op,
			  MSG_TYPE_VICTIM, MSG_TYPE_VICTIM_DIED,
			  "You would have died, but you are "
			  "in explore mode, so...", NULL);
	    op->stats.hp=op->stats.maxhp;
	    return;
	}
	sprintf(buf,"%s died.",op->name);
    }
    play_sound_player_only(op->contr, SOUND_PLAYER_DIES,0,0);

    /*  save the map location for corpse, gravestone*/
    x=op->x;y=op->y;map=op->map;


    if (settings.not_permadeth == TRUE) {
	/* NOT_PERMADEATH code.  This basically brings the character back to
	 * life if they are dead - it takes some exp and a random stat.
	 * See the config.h file for a little more in depth detail about this.
	 */

	/* Basically two ways to go - remove a stat permanently, or just
	 * make it depletion.  This bunch of code deals with that aspect
	 * of death.
	 */

	if (settings.balanced_stat_loss) {
	    /* If stat loss is permanent, lose one stat only. */
	    /* Lower level chars don't lose as many stats because they suffer
	       more if they do. */
	    /* Higher level characters can afford things such as potions of
	       restoration, or better, stat potions. So we slug them that
	       little bit harder. */
	    /* GD */
	    if (settings.stat_loss_on_death)
		num_stats_lose = 1;
	    else
		num_stats_lose = 1 + op->level/BALSL_NUMBER_LOSSES_RATIO;
	} else {
	    num_stats_lose = 1;
	}
	lost_a_stat = 0;

	for (z=0; z<num_stats_lose; z++) {
	    if (settings.stat_loss_on_death) {
		/* Pick a random stat and take a point off it.  Tell the player
		 * what he lost.
		 */
		i = RANDOM() % 7;
		change_attr_value(&(op->stats), i,-1);
		check_stat_bounds(&(op->stats));
		change_attr_value(&(op->contr->orig_stats), i,-1);
		check_stat_bounds(&(op->contr->orig_stats));
		draw_ext_info(NDI_UNIQUE, 0,op,
			      MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_STAT_LOSS,
			      lose_msg[i], lose_msg[i]);
		lost_a_stat = 1;
	    } else {
		/* deplete a stat */
		archetype *deparch=find_archetype("depletion");
		object *dep;

		i = RANDOM() % 7;
		dep = present_arch_in_ob(deparch,op);
		if(!dep) {
		    dep = arch_to_object(deparch);
		    insert_ob_in_ob(dep, op);
		}
		lose_this_stat = 1;
		if (settings.balanced_stat_loss) {
		    /* GD */
		    /* Get the stat that we're about to deplete. */
		    this_stat = get_attr_value(&(dep->stats), i);
		    if (this_stat < 0) {
			int loss_chance = 1 + op->level/BALSL_LOSS_CHANCE_RATIO;
			int keep_chance = this_stat * this_stat;
			/* Yes, I am paranoid. Sue me. */
			if (keep_chance < 1)
			    keep_chance = 1;

			/* There is a maximum depletion total per level. */
			if (this_stat < -1 - op->level/BALSL_MAX_LOSS_RATIO) {
			    lose_this_stat = 0;
			    /* Take loss chance vs keep chance to see if we
			       retain the stat. */
			} else {
			    if (random_roll(0, loss_chance + keep_chance-1,
				op, PREFER_LOW) < keep_chance)
				lose_this_stat = 0;
			    /* LOG(llevDebug, "Determining stat loss. Stat: %d Keep: %d Lose: %d Result: %s.\n",
			       this_stat, keep_chance, loss_chance,
			       lose_this_stat?"LOSE":"KEEP"); */
			}
		    }
		}

		if (lose_this_stat) {
		    this_stat = get_attr_value(&(dep->stats), i);
		    /* We could try to do something clever like find another
		     * stat to reduce if this fails.  But chances are, if
		     * stats have been depleted to -50, all are pretty low
		     * and should be roughly the same, so it shouldn't make a
		     * difference.
		     */
		    if (this_stat>=-50) {
			change_attr_value(&(dep->stats), i, -1);
			SET_FLAG(dep, FLAG_APPLIED);
			draw_ext_info(NDI_UNIQUE, 0,op,
				      MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_STAT_LOSS,
				      lose_msg[i], lose_msg[i]);
            fix_object(op);
			lost_a_stat = 1;
		    }
		}
	    }
	}
	/* If no stat lost, tell the player. */
	if (!lost_a_stat)
	    {
		/* determine_god() seems to not work sometimes... why is this?
		   Should I be using something else? GD */
		const char *god = determine_god(op);
		if (god && (strcmp(god, "none")))
		    draw_ext_info_format(NDI_UNIQUE, 0, op,
			 MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
			 "For a brief moment you feel the holy presence of %s protecting you",
			 "For a brief moment you feel the holy presence of %s protecting you",
			 god);
		else
		    draw_ext_info(NDI_UNIQUE, 0, op,
			  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
			  "For a brief moment you feel a holy presence protecting you.",
			  NULL);
	    }

	/* Put a gravestone up where the character 'almost' died.  List the
	 * exp loss on the stone.
	 */
	tmp=arch_to_object(find_archetype("gravestone"));
	sprintf(buf,"%s's gravestone",op->name);
	FREE_AND_COPY(tmp->name, buf);
	sprintf(buf,"%s's gravestones",op->name);
	FREE_AND_COPY(tmp->name_pl, buf);
	sprintf(buf,"RIP\nHere rests the hero %s the %s,\n"
	        "who was killed\n"
	        "by %s.\n",
	        op->name, op->contr->title,
	        op->contr->killer);
	tmp->msg = add_string(buf);
	tmp->x=op->x,tmp->y=op->y;
	insert_ob_in_map (tmp, op->map, NULL,0);



	/* restore player: remove any poisoning, disease and confusion the
         * character may be suffering.*/
	at = find_archetype("poisoning");
        tmp=present_arch_in_ob(at,op);
	if (tmp) {
	    remove_ob(tmp);
	    free_object(tmp);
	    draw_ext_info(NDI_UNIQUE, 0,op,
			  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
			  "Your body feels cleansed", NULL);
	}

	at = find_archetype("confusion");
        tmp=present_arch_in_ob(at,op);
	if (tmp) {
	    remove_ob(tmp);
	    free_object(tmp);
            draw_ext_info(NDI_UNIQUE, 0,tmp,
			  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
			  "Your mind feels clearer", NULL);
	}
	cure_disease(op,0);  /* remove any disease */

	/* Subtract the experience points, if we died cause of food, give
         * us food, and reset HP's...
         */
	apply_death_exp_penalty(op);
	if(op->stats.food < 100) op->stats.food = 900;
	op->stats.hp = op->stats.maxhp;
	op->stats.sp = MAX(op->stats.sp,  op->stats.maxsp);
	op->stats.grace = MAX(op->stats.grace, op->stats.maxgrace);

	/* Check to see if the player is in a shop. IF so, then check to see if
	 * the player has any unpaid items.  If so, remove them and put them back
	 * in the map.
	 */
        if (is_in_shop(op))
            remove_unpaid_objects(op->inv, op);

	/* Move player to his current respawn-position (usually last savebed) */
	enter_player_savebed(op);

	/* Save the player before inserting the force to reduce chance of abuse. */
	op->contr->braced=0;
	save_player(op,1);

	/* it is possible that the player has blown something up
	 * at his savebed location, and that can have long lasting
	 * spell effects.  So first see if there is a spell effect
	 * on the space that might harm the player.
	 */
	will_kill_again=0;
	for (tmp= get_map_ob(op->map, op->x, op->y); tmp; tmp=tmp->above) {
	    if (tmp->type ==SPELL_EFFECT)
		will_kill_again|=tmp->attacktype;
	}
	if (will_kill_again) {
	    object *force;
	    int  at;

	    force=create_archetype(FORCE_NAME);
	    /* 50 ticks should be enough time for the spell to abate */
	    force->speed=0.1;
	    force->speed_left=-5.0;
	    SET_FLAG(force, FLAG_APPLIED);
	    for (at=0; at<NROFATTACKS; at++) {
		if (will_kill_again & (1 << at))
		    force->resist[at] = 100;
	    }
	    insert_ob_in_ob(force, op);
        fix_object(op);

	}

	/* Tell the player they have died */
	draw_ext_info(NDI_UNIQUE, 0,op,
		      MSG_TYPE_VICTIM, MSG_TYPE_VICTIM_DIED,
		      "YOU HAVE DIED.", NULL);
	return;
    } /* NOT_PERMADETH */
    else {
	/* If NOT_PERMADETH is set, then the rest of this is not reachable.  This
	 * should probably be embedded in an else statement.
	 */

	op->contr->party=NULL;
	if (settings.set_title == TRUE)
	    op->contr->own_title[0]='\0';

	/* buf should be the kill message */
	draw_ext_info(NDI_UNIQUE|NDI_ALL, 0,NULL,
		      MSG_TYPE_VICTIM, MSG_TYPE_VICTIM_DIED,
		      buf, buf);
	check_score(op);
	if(op->contr->ranges[range_golem]!=NULL) {
	    remove_friendly_object(op->contr->ranges[range_golem]);
	    remove_ob(op->contr->ranges[range_golem]);
	    free_object(op->contr->ranges[range_golem]);
	    op->contr->ranges[range_golem]=NULL;
	    op->contr->golem_count=0;
	}
	loot_object(op); /* Remove some of the items for good */
	remove_ob(op);
	op->direction=0;

	if(!QUERY_FLAG(op,FLAG_WAS_WIZ)&&op->stats.exp) {
	    if (settings.resurrection == TRUE) {
		/* save playerfile sans equipment when player dies
		 * -then save it as player.pl.dead so that future resurrection
		 * -type spells will work on them nicely
		 */
		op->stats.hp = op->stats.maxhp;
		op->stats.food = 999;

		/* set the location of where the person will reappear when  */
		/* maybe resurrection code should fix map also */
		strcpy(op->contr->maplevel, settings.emergency_mapname);
		if(op->map!=NULL)
		    op->map = NULL;
		op->x = settings.emergency_x;
		op->y = settings.emergency_y;
		save_player(op,0);
		op->map = map;
		/* please see resurrection.c: peterm */
		dead_player(op);
	    } else {
		delete_character(op->name);
	    }
	}
	play_again(op);

	/*  peterm:  added to create a corpse at deathsite.  */
	tmp=arch_to_object(find_archetype("corpse_pl"));
	sprintf(buf,"%s", op->name);
	FREE_AND_COPY(tmp->name, buf);
	FREE_AND_COPY(tmp->name_pl, buf);
	tmp->level=op->level;
	tmp->x=x;tmp->y=y;
	if (tmp->msg)
	    free_string(tmp->msg);
	tmp->msg = add_string (gravestone_text(op));
	SET_FLAG (tmp, FLAG_UNIQUE);
	insert_ob_in_map (tmp, map, NULL,0);
    }
}

/* fix_weight(): Check recursively the weight of all players, and fix
 * what needs to be fixed.  Refresh windows and fix speed if anything
 * was changed.
 */

void fix_weight(void) {
  player *pl;
  for (pl = first_player; pl != NULL; pl = pl->next) {
    int old = pl->ob->carrying, sum = sum_weight(pl->ob);
    if(old == sum)
      continue;
    fix_object(pl->ob);
    LOG(llevDebug,"Fixed inventory in %s (%d -> %d)\n",
	pl->ob->name, old, sum);
  }
}

void fix_luck(void) {
  player *pl;
  for (pl = first_player; pl != NULL; pl = pl->next)
    if (!pl->ob->contr->state)
      change_luck(pl->ob, 0);
}


/* Handles op throwing objects of type 'DUST'.
 * This is much simpler in the new spell code - we basically
 * just treat this as any other spell casting object.
 */

void cast_dust (object *op, object *throw_ob, int dir) {
    object *skop, *spob;

    skop = find_skill_by_name(op, throw_ob->skill);

    /* casting POTION 'dusts' is really a use_magic_item skill */
    if(op->type==PLAYER && throw_ob->type==POTION && !skop) {
	LOG(llevError,"Player %s lacks critical skill use_magic_item!\n",
                op->name);
	return;
    }
    spob = throw_ob->inv;
    if (op->type==PLAYER && spob)
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
			     "You cast %s.",
			     "You cast %s.",
			     spob->name);

    cast_spell(op, throw_ob, dir, spob, NULL);

    if(!QUERY_FLAG(throw_ob,FLAG_REMOVED)) remove_ob(throw_ob);
    free_object(throw_ob);
}

void make_visible (object *op) {
    op->hide = 0;
    op->invisible = 0;
    if(op->type==PLAYER) {
	op->contr->tmp_invis = 0;
	if (op->contr->invis_race) FREE_AND_CLEAR_STR(op->contr->invis_race);
    }
    update_object(op,UP_OBJ_FACE);
}

int is_true_undead(object *op) {
  object *tmp=NULL;

  if(QUERY_FLAG(&op->arch->clone,FLAG_UNDEAD)) return 1;

  if(op->type==PLAYER)
    for(tmp=op->inv;tmp;tmp=tmp->below)
       if(tmp->type==EXPERIENCE && tmp->stats.Wis)
	  if(QUERY_FLAG(tmp,FLAG_UNDEAD)) return 1;
  return 0;
}

/* Look at the surrounding terrain to determine
 * the hideability of this object. Positive levels
 * indicate greater hideability.
 */

int hideability(object *ob) {
    int i,level=0, mflag;
    sint16 x,y;

    if(!ob||!ob->map) return 0;

    /* so, on normal lighted maps, its hard to hide */
    level=ob->map->darkness - 2;

    /* this also picks up whether the object is glowing.
     * If you carry a light on a non-dark map, its not
     * as bad as carrying a light on a pitch dark map
     */
    if(has_carried_lights(ob)) level =- (10 + (2*ob->map->darkness));

    /* scan through all nearby squares for terrain to hide in */
    for(i=0,x=ob->x,y=ob->y;i<9;i++,x=ob->x+freearr_x[i],y=ob->y+freearr_y[i]) {
	mflag = get_map_flags(ob->map, NULL, x, y, NULL, NULL);
	if (mflag & P_OUT_OF_MAP) { continue; }
	if(mflag & P_BLOCKSVIEW) /* something to hide near! */
	    level += 2;
	else /* open terrain! */
	    level -= 1;
    }

    return level;
}

/* For Hidden creatures - a chance of becoming 'unhidden'
 * every time they move - as we subtract off 'invisibility'
 * AND, for players, if they move into a ridiculously unhideable
 * spot (surrounded by clear terrain in broad daylight). -b.t.
 */
void do_hidden_move (object *op) {
    int hide=0, num=random_roll(0, 19, op, PREFER_LOW);
    object *skop;

    if(!op || !op->map) return;

    skop = find_obj_by_type_subtype(op, SKILL, SK_HIDING);

    /* its *extremely* hard to run and sneak/hide at the same time! */
    if(op->type==PLAYER && op->contr->run_on) {
	if(!skop || num >= skop->level) {
	    draw_ext_info(NDI_UNIQUE,0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			  "You ran too much! You are no longer hidden!", NULL);
	    make_visible(op);
	    return;
	} else num += 20;
    }
    num += op->map->difficulty;
    hide = hideability(op); /* modify by terrain hidden level */
    num -= hide;
    if((op->type==PLAYER && hide<-10) || ((op->invisible-=num)<=0)) {
	make_visible(op);
	if(op->type==PLAYER) draw_ext_info(NDI_UNIQUE, 0,op,
		   MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		   "You moved out of hiding! You are visible!", NULL);
    }
    else if (op->type == PLAYER && skop) {
	change_exp(op, calc_skill_exp(op, NULL, skop), skop->skill, 0);
    }
}

/* determine if who is standing near a hostile creature. */
int stand_near_hostile( object *who ) {
    object *tmp=NULL;
    int i,friendly=0,player=0, mflags;
    mapstruct *m;
    sint16  x,y;

    if(!who) return 0;

    if(who->type==PLAYER) player=1;
    else friendly = QUERY_FLAG(who,FLAG_FRIENDLY);

    /* search adjacent squares */
    for(i=1;i<9;i++) {
	x = who->x+freearr_x[i];
	y = who->y+freearr_y[i];
	m = who->map;
	mflags = get_map_flags(m, &m, x, y, &x, &y);
	/* space must be blocked if there is a monster.  If not
	 * blocked, don't need to check this space.
	 */
	if (mflags & P_OUT_OF_MAP) continue;
	if (OB_TYPE_MOVE_BLOCK(who, GET_MAP_MOVE_BLOCK(m, x, y))) continue;

	for(tmp=get_map_ob(m,x,y);tmp;tmp=tmp->above) {
	    if((player||friendly)
	       &&QUERY_FLAG(tmp,FLAG_MONSTER)&&!QUERY_FLAG(tmp,FLAG_UNAGGRESSIVE))
		return 1;
	    else if(tmp->type==PLAYER)
	    {
		/*don't let a hidden DM prevent you from hiding*/
		if(!QUERY_FLAG(tmp, FLAG_WIZ) || tmp->contr->hidden == 0)
		    return 1;
	    }
	}
    }
    return 0;
}

/* Check the player los field for viewability of the
 * object op. This function works fine for monsters,
 * but we dont worry if the object isnt the top one in
 * a pile (say a coin under a table would return "viewable"
 * by this routine). Another question, should we be
 * concerned with the direction the player is looking
 * in? Realistically, most of use cant see stuff behind
 * our backs...on the other hand, does the "facing" direction
 * imply the way your head, or body is facing? Its possible
 * for them to differ. Sigh, this fctn could get a bit more complex.
 * -b.t.
 * This function is now map tiling safe.
 */
int player_can_view (object *pl,object *op) {
    rv_vector rv;
    int dx,dy;

    if(pl->type!=PLAYER) {
	LOG(llevError,"player_can_view() called for non-player object\n");
	return -1;
    }
    if (!pl || !op) return 0;

    if(op->head) { op = op->head; }
    get_rangevector(pl, op, &rv, 0x1);

    /* starting with the 'head' part, lets loop
     * through the object and find if it has any
     * part that is in the los array but isnt on
     * a blocked los square.
     * we use the archetype to figure out offsets.
     */
    while(op) {
	dx = rv.distance_x + op->arch->clone.x;
	dy = rv.distance_y + op->arch->clone.y;

	/* only the viewable area the player sees is updated by LOS
	 * code, so we need to restrict ourselves to that range of values
	 * for any meaningful values.
	 */
	if (FABS(dx) <= (pl->contr->socket.mapx/2) &&
	    FABS(dy) <= (pl->contr->socket.mapy/2) &&
	    !pl->contr->blocked_los[dx + (pl->contr->socket.mapx/2)][dy+(pl->contr->socket.mapy/2)] )
	    return 1;
	op = op->more;
    }
    return 0;
}

/* routine for both players and monsters. We call this when
 * there is a possibility for our action distrubing our hiding
 * place or invisiblity spell. Artefact invisiblity is not
 * effected by this. If we arent invisible to begin with, we
 * return 0.
 */
static int action_makes_visible (object *op) {

    if(op->invisible && QUERY_FLAG(op,FLAG_ALIVE)) {
	if(QUERY_FLAG(op,FLAG_MAKE_INVIS))
	    return 0;

	if (op->contr && op->contr->tmp_invis == 0) return 0;

	/* If monsters, they should become visible */
	if(op->hide || !op->contr || (op->contr && op->contr->tmp_invis)) {
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_MISC, MSG_SUBTYPE_NONE,
				 "You become %s!",
				 "You become %s!",
				 op->hide?"unhidden":"visible");
	    return 1;
	}
    }
    return 0;
}

/* op_on_battleground - checks if the given object op (usually
 * a player) is standing on a valid battleground-tile,
 * function returns TRUE/FALSE. If true x, y returns the battleground
 * -exit-coord. (and if x, y not NULL)
 * 19 March 2005 - josh@woosworld.net modifed to check if the battleground also has slaying, maxhp, and maxsp set
 * and if those are all set and the player has a marker that matches the slaying send them to a different x, y
 * Default is to do the same as before, so only people wanting to have different points need worry about this
 */
int op_on_battleground (object *op, int *x, int *y) {
  object *tmp;

  /* A battleground-tile needs the following attributes to be valid:
   * is_floor 1 (has to be the FIRST floor beneath the player's feet),
   * name="battleground", no_pick 1, type=58 (type BATTLEGROUND)
   * and the exit-coordinates sp/hp must both be > 0.
   * => The intention here is to prevent abuse of the battleground-
   * feature (like pickable or hidden battleground tiles). */
  for (tmp=op->below; tmp!=NULL; tmp=tmp->below) {
    if (QUERY_FLAG (tmp, FLAG_IS_FLOOR)) {
      if (QUERY_FLAG (tmp, FLAG_NO_PICK) &&
	  strcmp(tmp->name, "battleground")==0 &&
	  tmp->type == BATTLEGROUND && EXIT_X(tmp) && EXIT_Y(tmp)) {
	    /*before we assign the exit, check if this is a teambattle*/
	    if ( EXIT_ALT_X(tmp) && EXIT_ALT_Y(tmp) && EXIT_PATH(tmp) ){
	        object *invtmp;
	    	for(invtmp=op->inv; invtmp != NULL; invtmp=invtmp->below) {
	            if(invtmp->type==FORCE && invtmp->slaying &&
		      !strcmp(EXIT_PATH(tmp), invtmp->slaying)){
		         if (x != NULL && y != NULL)
		           *x=EXIT_ALT_X(tmp), *y=EXIT_ALT_Y(tmp);
	                 return 1;
		     }
	        }
	    }
	    if (x != NULL && y != NULL)
		*x=EXIT_X(tmp), *y=EXIT_Y(tmp);
	    return 1;
      }
    }
  }
  /* If we got here, did not find a battleground */
  return 0;
}

/* When a dragon-player gains a new stage of evolution,
 * he gets some treasure
 *
 * attributes:
 *      object *who        the dragon player
 *      int atnr           the attack-number of the ability focus
 *      int level          ability level
 */
void dragon_ability_gain(object *who, int atnr, int level) {
    treasurelist *trlist = NULL;    /* treasurelist */
    treasure *tr;		    /* treasure */
    object *tmp,*skop;		    /* tmp. object */
    object *item;		    /* treasure object */
    char buf[MAX_BUF];		    /* tmp. string buffer */
    int i=0, j=0;

    /* get the appropriate treasurelist */
    if (atnr == ATNR_FIRE)
	trlist = find_treasurelist("dragon_ability_fire");
    else if (atnr == ATNR_COLD)
	trlist = find_treasurelist("dragon_ability_cold");
    else if (atnr == ATNR_ELECTRICITY)
	trlist = find_treasurelist("dragon_ability_elec");
    else if (atnr == ATNR_POISON)
	trlist = find_treasurelist("dragon_ability_poison");

    if (trlist == NULL || who->type != PLAYER)
	return;

    for (i=0, tr = trlist->items; tr != NULL && i<level-1;
	 tr = tr->next, i++);

    if (tr == NULL || tr->item == NULL) {
	/* LOG(llevDebug, "-> no more treasure for %s\n", change_resist_msg[atnr]); */
	return;
    }

    /* everything seems okay - now bring on the gift: */
    item = &(tr->item->clone);

    if (item->type == SPELL) {
	if (check_spell_known (who, item->name))
	    return;

	draw_ext_info_format(NDI_UNIQUE|NDI_BLUE, 0, who,
			     MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
			     "You gained the ability of %s",
			     "You gained the ability of %s",
			     item->name);
	do_learn_spell (who, item, 0);
	return;
    }

    /* grant direct spell */
    if (item->type == SPELLBOOK) {
	if (!item->inv) {
	    LOG(llevDebug,"dragon_ability_gain: Broken spellbook %s\n",
		item->name);
	    return;
	}
	if (check_spell_known (who, item->inv->name))
	    return;
	if (item->invisible) {
	    draw_ext_info_format(NDI_UNIQUE|NDI_BLUE, 0, who,
				 MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
				 "You gained the ability of %s",
				 "You gained the ability of %s",
				 item->inv->name);
	    do_learn_spell (who, item->inv, 0);
	    return;
	}
    }
    else if (item->type == SKILL_TOOL && item->invisible) {
	if (item->subtype == SK_CLAWING && (skop=find_skill_by_name(who, item->skill))!=NULL) {

	    /* should this perhaps be (skop->attackyp & item->attacktype)!=item->attacktype ...
	     * in this way, if the player is missing any of the attacktypes, he gets
	     * them.  As it is now, if the player has any that match the granted skill,
	     * but not all of them, he gets nothing.
	     */
	    if (!(skop->attacktype & item->attacktype)) {
		/* Give new attacktype */
		skop->attacktype |= item->attacktype;

		/* always add physical if there's none */
		skop->attacktype |= AT_PHYSICAL;

		if (item->msg != NULL)
		    draw_ext_info(NDI_UNIQUE|NDI_BLUE, 0, who,
				  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
				  item->msg, item->msg);

		/* Give player new face */
		if (item->animation_id) {
		    who->face = skop->face;
		    who->animation_id = item->animation_id;
		    who->anim_speed = item->anim_speed;
		    who->last_anim = 0;
		    who->state = 0;
		    animate_object(who, who->direction);
		}
	    }
	}
    }
    else if (item->type == FORCE) {
	/* forces in the treasurelist can alter the player's stats */
	object *skin;
	/* first get the dragon skin force */
	for (skin=who->inv; skin!=NULL && strcmp(skin->arch->name, "dragon_skin_force")!=0;
	     skin=skin->below);
	if (skin == NULL) return;

	/* adding new spellpath attunements */
	if (item->path_attuned > 0 && !(skin->path_attuned & item->path_attuned)) {
	    skin->path_attuned |= item->path_attuned; /* add attunement to skin */

	    /* print message */
	    sprintf(buf, "You feel attuned to ");
	    for(i=0, j=0; i<NRSPELLPATHS; i++) {
		if(item->path_attuned & (1<<i)) {
		    if (j)
			strcat(buf," and ");
		    else
			j = 1;
		    strcat(buf, spellpathnames[i]);
		}
	    }
	    strcat(buf,".");
	    draw_ext_info(NDI_UNIQUE|NDI_BLUE, 0, who,
			  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
			  buf, buf);
	}

	/* evtl. adding flags: */
	if(QUERY_FLAG(item, FLAG_XRAYS))
	    SET_FLAG(skin, FLAG_XRAYS);
	if(QUERY_FLAG(item, FLAG_STEALTH))
	    SET_FLAG(skin, FLAG_STEALTH);
	if(QUERY_FLAG(item, FLAG_SEE_IN_DARK))
	    SET_FLAG(skin, FLAG_SEE_IN_DARK);

	/* print message if there is one */
	if (item->msg != NULL)
	    draw_ext_info(NDI_UNIQUE|NDI_BLUE, 0, who,
			  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
			  item->msg, item->msg);
    }
    else {
        /* generate misc. treasure */
        char name[HUGE_BUF];
        tmp = arch_to_object (tr->item);
        query_short_name(tmp, name, HUGE_BUF);
	draw_ext_info_format(NDI_UNIQUE|NDI_BLUE, 0, who,
			     MSG_TYPE_ITEM, MSG_TYPE_ITEM_ADD,
			     "You gained %s",
			     "You gained %s",
			     name);
	tmp = insert_ob_in_ob (tmp, who);
	if (who->type == PLAYER)
	    esrv_send_item(who, tmp);
    }
}

/* Unready an object for a player. This function does nothing if the object was
 * not readied.
 */
void player_unready_range_ob(player *pl, object *ob) {
    rangetype i;

    for (i = 0; i < range_size; i++) {
        if (pl->ranges[i] == ob) {
            pl->ranges[i] = NULL;
            if (pl->shoottype == i) {
                pl->shoottype = range_none;
            }
        }
    }
}
