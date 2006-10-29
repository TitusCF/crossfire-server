/*
 * static char *rcsid_server_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
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

#include <global.h>
#include <object.h>
#include <tod.h>

#ifdef HAVE_DES_H
#include <des.h>
#else
#  ifdef HAVE_CRYPT_H
#  include <crypt.h>
#  endif
#endif

#ifndef __CEXTRACT__
#include <sproto.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <../random_maps/random_map.h>
#include <../random_maps/rproto.h>
#include "path.h"

static void process_events (mapstruct *map);

static char days[7][4] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void version(object *op) {

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_VERSION,
		       "This is Crossfire v%s",
		       "This is Crossfire v%s",
		       VERSION);
    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_VERSION,
		"The authors can be reached at crossfire@metalforge.org", NULL);

}

void start_info(object *op) {

    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
		       "Welcome to Crossfire, v%s!\nPress `?' for help\n",
		       "Welcome to Crossfire, v%s!\nPress `?' for help\n",
		       VERSION);

    draw_ext_info_format(NDI_UNIQUE | NDI_ALL | NDI_DK_ORANGE, 5, op,
			 MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_PLAYER,
			 "%s entered the game.",
			 "%s entered the game.",
			 op->name);

    if(!op->contr->name_changed) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
	      "Note that you must set your name with the name command to enter the highscore list.", NULL);
    }
}

/**
 * Really, there is no reason to crypt the passwords  any system.  But easier
 * to just leave this enabled for backward compatibility.  Put the
 * simple case at top - no encryption - makes it easier to read.
 */
char *crypt_string(char *str, char *salt) {
#if defined(WIN32) || (defined(__FreeBSD__) && !defined(HAVE_LIBDES))
    return(str);
#else
    static const char *c=
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
    char s[2];

    if(salt==NULL)
	s[0]= c[RANDOM() % (int)strlen(c)],
	s[1]= c[RANDOM() % (int)strlen(c)];
    else
	s[0]= salt[0],
	s[1]= salt[1];

#  ifdef HAVE_LIBDES
    return (char*)des_crypt(str,s);
#  endif
    /* Default case - just use crypt */
    return (char*)crypt(str,s);
#endif
}

int check_password(char *typed,char *crypted) {
  return !strcmp(crypt_string(typed,crypted),crypted);
}

/**
 * This is a basic little function to put the player back to his
 * savebed.  We do some error checking - its possible that the
 * savebed map may no longer exist, so we make sure the player
 * goes someplace.
 */
void enter_player_savebed(object *op)
{
    mapstruct	*oldmap = op->map;
    object  *tmp;

    tmp=get_object();

    EXIT_PATH(tmp) = add_string(op->contr->savebed_map);
    EXIT_X(tmp) = op->contr->bed_x;
    EXIT_Y(tmp) = op->contr->bed_y;
    enter_exit(op,tmp);
    /* If the player has not changed maps and the name does not match
     * that of the savebed, his savebed map is gone.  Lets go back
     * to the emergency path.  Update what the players savebed is
     * while we're at it.
     */
    if (oldmap == op->map && strcmp(op->contr->savebed_map, oldmap->path)) {
	LOG(llevDebug,"Player %s savebed location %s is invalid - going to emergency location (%s)\n",
	    settings.emergency_mapname, op->name, op->contr->savebed_map);
	strcpy(op->contr->savebed_map, settings.emergency_mapname);
	op->contr->bed_x = settings.emergency_x;
	op->contr->bed_y = settings.emergency_y;
	free_string(op->contr->savebed_map);
	EXIT_PATH(tmp) = add_string(op->contr->savebed_map);
	EXIT_X(tmp) = op->contr->bed_x;
	EXIT_Y(tmp) = op->contr->bed_y;
	enter_exit(op,tmp);
    }
    free_object(tmp);
}

/**
 * All this really is is a glorified remove_object that also updates
 * the counts on the map if needed.
 */
void leave_map(object *op)
{
    mapstruct *oldmap = op->map;

    remove_ob(op);

    if (oldmap) {
	if (!op->contr->hidden)
	    oldmap->players--;
	if (oldmap->players <= 0) { /* can be less than zero due to errors in tracking this */
	    set_map_timeout(oldmap);
	}
    }
}

/**
 *  enter_map():  Moves the player and pets from current map (if any) to
 * new map.  map, x, y must be set.  map is the map we are moving the
 * player to - it could be the map he just came from if the load failed for
 * whatever reason.  If default map coordinates are to be used, then
 * the function that calls this should figure them out.
 */
static void enter_map(object *op, mapstruct *newmap, int x, int y) {
    mapstruct *oldmap = op->map;

    if (out_of_map(newmap, x, y)) {
	LOG(llevError,"enter_map: supplied coordinates are not within the map! (%s: %d, %d)\n",
	    newmap->path, x, y);
	x=MAP_ENTER_X(newmap);
	y=MAP_ENTER_Y(newmap);
	if (out_of_map(newmap, x, y)) {
	    LOG(llevError,"enter_map: map %s provides invalid default enter location (%d, %d) > (%d, %d)\n",
		newmap->path, x, y, MAP_WIDTH(newmap), MAP_HEIGHT(newmap));
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
			  "The exit is closed", NULL);
	    return;
	}
    }
    /* try to find a spot for the player */
    if (ob_blocked(op, newmap, x, y)) {	/* First choice blocked */
	/* We try to find a spot for the player, starting closest in.
	 * We could use find_first_free_spot, but that doesn't randomize it at all,
	 * So for example, if the north space is free, you would always end up there even
	 * if other spaces around are available.
	 * Note that for the second and third calls, we could start at a position other
	 * than one, but then we could end up on the other side of walls and so forth.
	 */
	int i = find_free_spot(op,newmap, x, y, 1, SIZEOFFREE1+1);
	if (i==-1) {
	    i = find_free_spot(op,newmap, x, y, 1, SIZEOFFREE2+1);
	    if (i==-1)
		i = find_free_spot(op,newmap, x, y, 1, SIZEOFFREE);
	}
	if (i != -1 ) {
	    x += freearr_x[i];
	    y += freearr_y[i];
	} else {
	    /* not much we can do in this case. */
	    LOG(llevInfo,"enter_map: Could not find free spot for player - will dump on top of object (%s: %d, %d)\n",
		newmap->path, x , y);
	}
    } /* end if looking for free spot */
	

    /* If it is a player login, he has yet to be inserted anyplace.
     * otherwise, we need to deal with removing the playe here.
     */
    if(!QUERY_FLAG(op, FLAG_REMOVED))
	remove_ob(op);
    if (op->map!=NULL)
    {
        /* Lauwenmark : Here we handle the MAPLEAVE global event */
        execute_global_event(EVENT_MAPLEAVE, op, op->map);
    }
    /* remove_ob clears these so they must be reset after the remove_ob call */
    op->x = x;
    op->y = y;
    op->map = newmap;
    insert_ob_in_map(op,op->map,NULL,INS_NO_WALK_ON);

    /* Lauwenmark : Here we handle the MAPENTER global event */
    execute_global_event(EVENT_MAPENTER, op, op->map);

    if (!op->contr->hidden)
	newmap->players++;

    newmap->timeout=0;
    op->enemy = NULL;

    if (op->contr) {
	strcpy(op->contr->maplevel, newmap->path);
	op->contr->count=0;
    }

    /* Update any golems */
    if(op->type == PLAYER && op->contr->ranges[range_golem] != NULL) {
	int i = find_free_spot(op->contr->ranges[range_golem],newmap,
			       x, y, 1, SIZEOFFREE);
	remove_ob(op->contr->ranges[range_golem]);
	if (i==-1) {
	    remove_friendly_object(op->contr->ranges[range_golem]);
	    free_object(op->contr->ranges[range_golem]);
	    op->contr->ranges[range_golem]=NULL;
	    op->contr->golem_count=0;
	}
	else {
	    object *tmp;
	    for (tmp=op->contr->ranges[range_golem]; tmp!=NULL; tmp=tmp->more) {
		tmp->x = x + freearr_x[i]+ (tmp->arch==NULL?0:tmp->arch->clone.x);
		tmp->y = y + freearr_y[i]+ (tmp->arch==NULL?0:tmp->arch->clone.y);
		tmp->map = newmap;
	    }
	    insert_ob_in_map(op->contr->ranges[range_golem], newmap, NULL,0);
	    op->contr->ranges[range_golem]->direction = 
		find_dir_2(op->x - op->contr->ranges[range_golem]->x, 
			   op->y - op->contr->ranges[range_golem]->y);
	}
    }
    op->direction=0;

    /* since the players map is already loaded, we don't need to worry
     * about pending objects.
     */
    remove_all_pets(newmap);

    /* If the player is changing maps, we need to do some special things
     * Do this after the player is on the new map - otherwise the force swap of the
     * old map does not work.
     */
    if (oldmap != newmap) {
        if (oldmap) /* adjust old map */
        {
            oldmap->players--;

            if (oldmap->players <= 0) /* can be less than zero due to errors in tracking this */
	        set_map_timeout(oldmap);
        }
    }
    swap_below_max (newmap->path);

    if( op->type == PLAYER)
        map_newmap_cmd(op->contr);
}

void set_map_timeout(mapstruct *oldmap)
{
#if MAP_MAXTIMEOUT
    oldmap->timeout = MAP_TIMEOUT(oldmap);
    /* Do MINTIMEOUT first, so that MAXTIMEOUT is used if that is
     * lower than the min value.
     */
#if MAP_MINTIMEOUT
    if (oldmap->timeout < MAP_MINTIMEOUT) {
	oldmap->timeout = MAP_MINTIMEOUT;
    }
#endif
    if (oldmap->timeout > MAP_MAXTIMEOUT) {
	oldmap->timeout = MAP_MAXTIMEOUT;
    }
#else
    /* save out the map */
    swap_map(oldmap);
#endif /* MAP_MAXTIMEOUT */
}


/**
 * clean_path takes a path and replaces all / with _
 * We do a strcpy so that we do not change the original string.
 */
static char *clean_path(const char *file)
{
    static char newpath[MAX_BUF],*cp;

    strncpy(newpath, file, MAX_BUF-1);
    newpath[MAX_BUF-1]='\0';
    for (cp=newpath; *cp!='\0'; cp++) {
	if (*cp=='/') *cp='_';
    }
    return newpath;
}


/**
 * unclean_path takes a path and replaces all _ with /
 * This basically undoes clean path.
 * We do a strcpy so that we do not change the original string.
 * We are smart enough to start after the last / in case we
 * are getting passed a string that points to a unique map
 * path.
 */
static char *unclean_path(const char *src)
{
    static char newpath[MAX_BUF],*cp;

    cp=strrchr(src, '/');
    if (cp)
	strncpy(newpath, cp+1, MAX_BUF-1);
    else
	strncpy(newpath, src, MAX_BUF-1);
    newpath[MAX_BUF-1]='\0';

    for (cp=newpath; *cp!='\0'; cp++) {
	if (*cp=='_') *cp='/';
    }
    return newpath;
}


/**
 * The player is trying to enter a randomly generated map.  In this case, generate the
 * random map as needed.
 */

static void enter_random_map(object *pl, object *exit_ob)
{
    mapstruct *new_map;
    char newmap_name[HUGE_BUF], *cp;
    static int reference_number = 0;
    RMParms rp;

    memset(&rp, 0, sizeof(RMParms));
    rp.Xsize=-1;
    rp.Ysize=-1;
    rp.region=get_region_by_map(exit_ob->map);
    if (exit_ob->msg) set_random_map_variable(&rp,exit_ob->msg);
    rp.origin_x = exit_ob->x;
    rp.origin_y = exit_ob->y;
    strcpy(rp.origin_map, pl->map->path);

    /* If we have a final_map, use it as a base name to give some clue
     * as where the player is.  Otherwise, use the origin map.
     * Take the last component (after the last slash) to give
     * shorter names without bogus slashes.
     */
    if (rp.final_map[0]) {
	cp = strrchr(rp.final_map, '/');
	if (!cp) cp = rp.final_map;
    } else {
	char buf[HUGE_BUF];

	cp = strrchr(rp.origin_map, '/');
	if (!cp) cp = rp.origin_map;
	/* Need to strip of any trailing digits, if it has them */
	strcpy(buf, cp);
	while (isdigit(buf[strlen(buf) - 1]))
	    buf[strlen(buf) - 1] = 0;
	cp = buf;
    }

    sprintf(newmap_name,"/random/%s%04d",cp+1, reference_number++);

    /* now to generate the actual map. */
    new_map=generate_random_map(newmap_name,&rp);

    /* Update the exit_ob so it now points directly at the newly created
     * random maps.  Not that it is likely to happen, but it does mean that a
     * exit in a unique map leading to a random map will not work properly.
     * It also means that if the created random map gets reset before
     * the exit leading to it, that the exit will no longer work.
     */
    if(new_map) {
	int x, y;
	x=EXIT_X(exit_ob) = MAP_ENTER_X(new_map);
	y=EXIT_Y(exit_ob) = MAP_ENTER_Y(new_map);
	EXIT_PATH(exit_ob) = add_string(newmap_name);
	strcpy(new_map->path, newmap_name);
	enter_map(pl, new_map, 	x, y);
    }
}

/**
 * The player is trying to enter a non-randomly generated template map.  In this
 * case, use a map file for a template
 */

static void enter_fixed_template_map(object *pl, object *exit_ob)
{
    mapstruct *new_map;
    char tmpnum[32], exitpath[HUGE_BUF], resultname[HUGE_BUF], tmpstring[HUGE_BUF], *sourcemap;
    const char *new_map_name;
    
    /* Split the exit path string into two parts, one
     * for where to store the map, and one for were
     * to generate the map from.
     */
    snprintf(exitpath, sizeof(exitpath), "%s", EXIT_PATH(exit_ob)+2);
    sourcemap = strchr(exitpath, '!');
    if (!sourcemap) {
	draw_ext_info_format(NDI_UNIQUE, 0, pl, 
			     MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
			     "The %s is closed.", 
			     "The %s is closed.", 
			     exit_ob->name);
	/* Should only occur when no source map is set.
	 */
	LOG(llevError,"enter_fixed_template_map: Exit %s (%d,%d) on map %s has no source template.\n",
		    exit_ob->name, exit_ob->x, exit_ob->y, exit_ob->map->path);
        return;
    }
    *sourcemap++ = '\0';
    
    /* If we are not coming from a template map, we can use reletive directories
     * for the map to generate from.
     */
    if (!exit_ob->map->template) {
        sourcemap = path_combine_and_normalize(exit_ob->map->path, sourcemap);
    }
    
    /* Do replacement of %x, %y, and %n to the x coord of the exit, the y coord
     * of the exit, and the name of the map the exit is on, respectively.
     */
    sprintf(tmpnum ,"%d", exit_ob->x);
    replace(exitpath, "%x", tmpnum, resultname,  sizeof(resultname));
    
    sprintf(tmpnum ,"%d", exit_ob->y);
    sprintf(tmpstring, "%s", resultname);
    replace(tmpstring, "%y", tmpnum, resultname,  sizeof(resultname));
    
    sprintf(tmpstring, "%s", resultname);
    replace(tmpstring, "%n", exit_ob->map->name, resultname,  sizeof(resultname));
    
    /* If we are coming from another template map, use reletive paths unless 
     * indicated otherwise.
     */
    if (exit_ob->map->template && (resultname[0] != '/')) {
        new_map_name = path_combine_and_normalize(exit_ob->map->path, resultname);
    } else {
        new_map_name = create_template_pathname(resultname);
    }
    
    /* Attempt to load the map, if unable to, then
     * create the map from the template.
     */
    new_map = ready_map_name(new_map_name, MAP_PLAYER_UNIQUE);
    if (!new_map) {
        new_map = load_original_map(create_pathname(sourcemap), MAP_PLAYER_UNIQUE);
        if (new_map) fix_auto_apply(new_map);
    }
    
    if (new_map) {
        /* set the path of the map to where it should be
         * so we don't just save over the source map.
         */
        strcpy(new_map->path, new_map_name);
	new_map->template = 1;
        enter_map(pl, new_map, EXIT_X(exit_ob), EXIT_Y(exit_ob));
    } else {
	draw_ext_info_format(NDI_UNIQUE, 0, pl,
			     MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
			     "The %s is closed.",
			     "The %s is closed.",
			     exit_ob->name);
	/* Should only occur when an invalid source map is set.
	 */
	LOG(llevDebug,"enter_fixed_template_map: Exit %s (%d,%d) on map %s leads no where.\n",
		    exit_ob->name, exit_ob->x, exit_ob->y, exit_ob->map->path);
    }
}


/**
 * The player is trying to enter a randomly generated template map.  In this
 * case, generate the map as needed.
 */

static void enter_random_template_map(object *pl, object *exit_ob)
{
    mapstruct *new_map;
    char tmpnum[32], resultname[HUGE_BUF], tmpstring[HUGE_BUF];
    const char *new_map_name;
    RMParms rp;
    
    /* Do replacement of %x, %y, and %n to the x coord of the exit, the y coord
     * of the exit, and the name of the map the exit is on, respectively.
     */
    sprintf(tmpnum ,"%d", exit_ob->x);
    replace(EXIT_PATH(exit_ob)+3, "%x", tmpnum, resultname,  sizeof(resultname));
    
    sprintf(tmpnum ,"%d", exit_ob->y);
    sprintf(tmpstring, "%s", resultname);
    replace(tmpstring, "%y", tmpnum, resultname,  sizeof(resultname));
    
    sprintf(tmpstring, "%s", resultname);
    replace(tmpstring, "%n", exit_ob->map->name, resultname,  sizeof(resultname));
    
    /* If we are coming from another template map, use reletive paths unless 
     * indicated otherwise.
     */
    if (exit_ob->map->template && (resultname[0] != '/')) {
        new_map_name = path_combine_and_normalize(exit_ob->map->path, resultname);
    } else {
        new_map_name = create_template_pathname(resultname);
    }

    new_map = ready_map_name(new_map_name, MAP_PLAYER_UNIQUE);
    if (!new_map) {
	memset(&rp, 0, sizeof(RMParms));
	rp.Xsize=-1;
	rp.Ysize=-1;
	rp.region=get_region_by_map(exit_ob->map);
	if (exit_ob->msg) set_random_map_variable(&rp,exit_ob->msg);
	rp.origin_x = exit_ob->x;
	rp.origin_y = exit_ob->y;
	strcpy(rp.origin_map, pl->map->path);
	
	/* now to generate the actual map. */
	new_map=generate_random_map(new_map_name,&rp);
    }


    /* Update the exit_ob so it now points directly at the newly created
     * random maps.  Not that it is likely to happen, but it does mean that a
     * exit in a unique map leading to a random map will not work properly.
     * It also means that if the created random map gets reset before
     * the exit leading to it, that the exit will no longer work.
     */
    if(new_map) {
	int x, y;
	x=EXIT_X(exit_ob) = MAP_ENTER_X(new_map);
	y=EXIT_Y(exit_ob) = MAP_ENTER_Y(new_map);
	new_map->template = 1;
	enter_map(pl, new_map, 	x, y);
    }
}


/**
 * Code to enter/detect a character entering a unique map.
 */
static void enter_unique_map(object *op, object *exit_ob)
{
    char apartment[HUGE_BUF];
    mapstruct	*newmap;

    if (EXIT_PATH(exit_ob)[0]=='/') {
	sprintf(apartment, "%s/%s/%s/%s", settings.localdir,
	    settings.playerdir, op->name, clean_path(EXIT_PATH(exit_ob)));
	newmap = ready_map_name(apartment, MAP_PLAYER_UNIQUE);
	if (!newmap) {
	    newmap = load_original_map(create_pathname(EXIT_PATH(exit_ob)), MAP_PLAYER_UNIQUE);
	    if (newmap) fix_auto_apply(newmap);
	}
    } else { /* relative directory */
	char reldir[HUGE_BUF], tmpc[HUGE_BUF], *cp;

	if (exit_ob->map->unique) {

	    strcpy(reldir, unclean_path(exit_ob->map->path));

	    /* Need to copy this over, as clean_path only has one static return buffer */
	    strcpy(tmpc, clean_path(reldir));
	    /* Remove final component, if any */ 
	    if ((cp=strrchr(tmpc, '_'))!=NULL) *cp=0;

	    sprintf(apartment, "%s/%s/%s/%s_%s", settings.localdir,
		    settings.playerdir, op->name, tmpc,
		    clean_path(EXIT_PATH(exit_ob)));

	    newmap = ready_map_name(apartment, MAP_PLAYER_UNIQUE);
	    if (!newmap) {
		newmap = load_original_map(create_pathname(path_combine_and_normalize(reldir, EXIT_PATH(exit_ob))), MAP_PLAYER_UNIQUE);
		if (newmap) fix_auto_apply(newmap);
	    }
	}
	else {
	    /* The exit is unique, but the map we are coming from is not unique.  So
	     * use the basic logic - don't need to demangle the path name
	     */
	    sprintf(apartment, "%s/%s/%s/%s", settings.localdir,
		    settings.playerdir, op->name, 
		    clean_path(path_combine_and_normalize(exit_ob->map->path, EXIT_PATH(exit_ob))));
	    newmap = ready_map_name(apartment, MAP_PLAYER_UNIQUE);
	    if (!newmap) {
		newmap = ready_map_name(path_combine_and_normalize(exit_ob->map->path, EXIT_PATH(exit_ob)), 0);
		if (newmap) fix_auto_apply(newmap);
	    }
	}
    }

    if (newmap) {
	strcpy(newmap->path, apartment);
	newmap->unique = 1;
	enter_map(op, newmap, EXIT_X(exit_ob), EXIT_Y(exit_ob));
    } else {
	draw_ext_info_format(NDI_UNIQUE, 0, op,
			     MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
			     "The %s is closed.",
			     "The %s is closed.",
			     exit_ob->name);
	/* Perhaps not critical, but I would think that the unique maps
	 * should be new enough this does not happen.  This also creates
	 * a strange situation where some players could perhaps have visited
	 * such a map before it was removed, so they have the private
	 * map, but other players can't get it anymore.
	 */
	LOG(llevDebug,"enter_unique_map: Exit %s (%d,%d) on map %s is leads no where.\n",
		    exit_ob->name, exit_ob->x, exit_ob->y, exit_ob->map->path);
    }
	
}


/**
 * Tries to move 'op' to exit_ob.  op is the character or monster that is
 * using the exit, where exit_ob is the exit object (boat, door, teleporter,
 * etc.)  if exit_ob is null, then op->contr->maplevel contains that map to
 * move the object to.  This is used when loading the player.
 *
 * Largely redone by MSW 2001-01-21 - this function was overly complex
 * and had some obscure bugs.
 */

void enter_exit(object *op, object *exit_ob) {
    #define PORTAL_DESTINATION_NAME "Town portal destination" /* this one should really be in a header file */
    object *tmp;
    /* It may be nice to support other creatures moving across
     * exits, but right now a lot of the code looks at op->contr,
     * so thta is an RFE.
     */
    if (op->type != PLAYER) return;

    /* Need to remove player from transport */
    if (op->contr->transport) apply_transport(op, op->contr->transport, AP_UNAPPLY);

    /* First, lets figure out what map the player is going to go to */
    if (exit_ob){ 

    /* check to see if we make a template map */
    if(EXIT_PATH(exit_ob)&&EXIT_PATH(exit_ob)[1]=='@') {
        if (EXIT_PATH(exit_ob)[2]=='!') {
            /* generate a template map randomly */
            enter_random_template_map(op, exit_ob);
        } else {
            /* generate a template map from a fixed template */
            enter_fixed_template_map(op, exit_ob);
        }
    }
    /* check to see if we make a randomly generated map */
    else if(EXIT_PATH(exit_ob)&&EXIT_PATH(exit_ob)[1]=='!') {
        enter_random_map(op, exit_ob);
    }
    else if (QUERY_FLAG(exit_ob, FLAG_UNIQUE)) {
        enter_unique_map(op, exit_ob);
    } else {
        int x=EXIT_X(exit_ob), y=EXIT_Y(exit_ob);
        /* 'Normal' exits that do not do anything special
        * Simple enough we don't need another routine for it.
        */
        mapstruct	*newmap;
        if (exit_ob->map) {
            char tmp_path[HUGE_BUF];
            tmp_path[0] = '\0';
            strncpy(tmp_path, path_combine_and_normalize(exit_ob->map->path, EXIT_PATH(exit_ob)), HUGE_BUF);
            tmp_path[HUGE_BUF - 1] = '\0';
            newmap = ready_map_name(tmp_path, 0);
            /* Random map was previously generated, but is no longer about.  Lets generate a new
             * map.
             */
            if (!newmap && !strncmp(EXIT_PATH(exit_ob),"/random/",8)) {
            /* Maps that go down have a message set.  However, maps that go
             * up, don't.  If the going home has reset, there isn't much
             * point generating a random map, because it won't match the maps.
             */
                if (exit_ob->msg) {
                    enter_random_map(op, exit_ob);
                } else {
                    draw_ext_info_format(NDI_UNIQUE, 0, op,
					 MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, 
					 "The %s is closed.",
					 "The %s is closed.",
					 exit_ob->name);
                    return;
                }

            /* For exits that cause damages (like pits).  Don't know if any
             * random maps use this or not.
             */
            if(exit_ob->stats.dam && op->type==PLAYER)
                hit_player(op,exit_ob->stats.dam,exit_ob,exit_ob->attacktype,1);
            return;
            }
        } else {
            /* For word of recall and other force objects
             * They contain the full pathname of the map to go back to,
             * so we don't need to normalize it.
             * But we do need to see if it is unique or not 
             */
            if (!strncmp(EXIT_PATH(exit_ob), settings.localdir, strlen(settings.localdir)))
                newmap = ready_map_name(EXIT_PATH(exit_ob), MAP_PLAYER_UNIQUE);
            else
                newmap = ready_map_name(EXIT_PATH(exit_ob), 0);
        }
        if (!newmap) {
            if (exit_ob->name)
                draw_ext_info_format(NDI_UNIQUE, 0, op,
				     MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
				     "The %s is closed.",
				     "The %s is closed.",
				     exit_ob->name);
                /* don't cry to momma if name is not set - as in tmp objects
                 * used by the savebed code and character creation */
            return;
        }

        /* This supports the old behaviour, but it really should not be used.
         * I will note for example that with this method, it is impossible to
         * set 0,0 destination coordinates.  Really, if we want to support
         * using the new maps default coordinates, the exit ob should use
         * something like -1, -1 so it is clear to do that.
         */
        if (x==0 && y==0) {
            x=MAP_ENTER_X(newmap);
            y=MAP_ENTER_Y(newmap);
            LOG(llevDebug,"enter_exit: Exit %s (%d,%d) on map %s is 0 destination coordinates\n",
                exit_ob->name?exit_ob->name:"(none)", exit_ob->x, exit_ob->y, 
                exit_ob->map?exit_ob->map->path:"(none)");
        }

        /* mids 02/13/2002 if exit is damned, update players death & WoR home-position and delete town portal */
        if (QUERY_FLAG(exit_ob, FLAG_DAMNED)) {
            /* remove an old force with a slaying field == PORTAL_DESTINATION_NAME */
            for(tmp=op->inv; tmp != NULL; tmp = tmp->below) {
                if(tmp->type == FORCE && tmp->slaying && !strcmp(tmp->slaying, PORTAL_DESTINATION_NAME)) break;
            }
            if(tmp) {
                remove_ob(tmp);
                free_object(tmp);
            }

            strcpy(op->contr->savebed_map, path_combine_and_normalize(exit_ob->map->path, EXIT_PATH(exit_ob)));
            op->contr->bed_x = EXIT_X(exit_ob), op->contr->bed_y = EXIT_Y(exit_ob);
            save_player(op, 1);
            /* LOG(llevDebug,"enter_exit: Taking damned exit %s to (%d,%d) on map %s\n",
             * exit_ob->name?exit_ob->name:"(none)", exit_ob->x, exit_ob->y,  
             * path_combine_and_normalize(exit_ob->map->path, EXIT_PATH(exit_ob))); */
        }

        enter_map(op, newmap, x, y);
    }
    /* For exits that cause damages (like pits) */
    if(exit_ob->stats.dam && op->type==PLAYER)
        hit_player(op,exit_ob->stats.dam,exit_ob,exit_ob->attacktype,1);
    } else {
        int flags = 0;
        mapstruct *newmap;


        /* Hypothetically, I guess its possible that a standard map matches
         * the localdir, but that seems pretty unlikely - unlikely enough that
         * I'm not going to attempt to try to deal with that possibility.
         * We use the fact that when a player saves on a unique map, it prepends
         * the localdir to that name.  So its an easy way to see of the map is
         * unique or not.
         */
        if (!strncmp(op->contr->maplevel, settings.localdir, strlen(settings.localdir)))
            flags = MAP_PLAYER_UNIQUE;

        /* newmap returns the map (if already loaded), or loads it for us. */
        newmap = ready_map_name(op->contr->maplevel, flags);
        if (!newmap) {
            LOG(llevError, "enter_exit: Pathname to map does not exist! (%s)\n", op->contr->maplevel);
            newmap = ready_map_name(settings.emergency_mapname, 0);
            op->x = settings.emergency_x;
            op->y = settings.emergency_y;
            /* If we can't load the emergency map, something is probably really
             * screwed up, so bail out now.
             */
            if (!newmap) {
                LOG(llevError,"enter_exit: could not load emergency map? Fatal error\n");
                abort();
            }
        }
        enter_map(op, newmap, op->x, op->y);
    }
}

#if 0
/* process_active_maps no longer used - should perhaps be removed.
 * MSW 2006-06-02
 */

/**
 * process_active_maps(): Works like process_events(), but it only
 * processes maps which a player is on.
 * It will check that it isn't called too often, and abort
 * if time since last call is less than MAX_TIME.
 */

static void process_active_maps(void) {
  mapstruct *map;

    /*
     * If enough time has elapsed, do some work.
     */
    if(enough_elapsed_time()) {
	for(map=first_map;map!=NULL;map=map->next) {
	    if(map->in_memory == MAP_IN_MEMORY) {
		if(players_on_map(map,TRUE))
		    process_events(map);
	    }
	}
    }
}
#endif

/**
 * process_players1 and process_players2 do all the player related stuff.
 * I moved it out of process events and process_map.  This was to some
 * extent for debugging as well as to get a better idea of the time used
 * by the various functions.  process_players1() does the processing before
 * objects have been updated, process_players2() does the processing that
 * is needed after the players have been updated.
 */

static void process_players1(mapstruct *map)
{
    int flag;
    player *pl,*plnext;

    /* Basically, we keep looping until all the players have done their actions. */
    for(flag=1;flag!=0;) {
	flag=0;
	for(pl=first_player;pl!=NULL;pl=plnext) {
	    plnext=pl->next; /* In case a player exits the game in handle_player() */

	    if (pl->ob == NULL) continue;

	    if (map!=NULL && pl->ob->map!=map) continue;

            if(pl->ob->speed_left>0) {
		if (handle_newcs_player(pl->ob))
		    flag=1;
	    } /* end if player has speed left */

	    /* If the player is not actively playing, don't make a
	     * backup save - nothing to save anyway.  Plus, the
	     * map may not longer be valid.  This can happen when the
	     * player quits - they exist for purposes of tracking on the map,
	     * but don't actually reside on any actual map.
	     */
	    if (QUERY_FLAG(pl->ob, FLAG_REMOVED)) continue;

#ifdef AUTOSAVE
	    /* check for ST_PLAYING state so that we don't try to save off when
	     * the player is logging in.
	     */
	    if ((pl->last_save_tick+AUTOSAVE)<pticks && pl->state==ST_PLAYING) {
		/* Don't save the player on unholy ground.  Instead, increase the
		 * tick time so it will be about 10 seconds before we try and save
		 * again.
		 */
		if (get_map_flags(pl->ob->map, NULL, pl->ob->x, pl->ob->y, NULL, NULL) & P_NO_CLERIC) {
		    pl->last_save_tick += 100;
		} else {
		    save_player(pl->ob,1);
		    pl->last_save_tick = pticks;
		}
	    }
#endif
	} /* end of for loop for all the players */
    } /* for flag */
    for(pl=first_player;pl!=NULL;pl=pl->next) {
	if (map!=NULL && (pl->ob == NULL || pl->ob->map!=map))
	    continue;
	if (settings.casting_time == TRUE) {
	    if (pl->ob->casting_time > 0){
		pl->ob->casting_time--;
		pl->ob->start_holding = 1;
	    }
	    /* set spell_state so we can update the range in stats field */
	    if ((pl->ob->casting_time == 0) && (pl->ob->start_holding ==1)){
		pl->ob->start_holding = 0;
	    }
	}
	do_some_living(pl->ob);
/*	draw(pl->ob);*/	/* updated in socket code */
    }
}

static void process_players2(mapstruct *map)
{
    player *pl;

    /* Then check if any players should use weapon-speed instead of speed */
    for(pl=first_player;pl!=NULL;pl=pl->next) {
	if (map!=NULL) {
	    if(pl->ob == NULL || QUERY_FLAG(pl->ob,FLAG_REMOVED))
		continue;
	    else if(pl->loading != NULL) /* Player is blocked */
		pl->ob->speed_left -= pl->ob->speed;
	    if (pl->ob->map!=map) continue;
	}

	/* The code that did weapon_sp handling here was out of place -
	 * this isn't called until after the player has finished there
	 * actions, and is thus out of place.  All we do here is bounds
	 * checking.
	 */
	if (pl->has_hit) {
	    if (pl->ob->speed_left > pl->weapon_sp) pl->ob->speed_left = pl->weapon_sp;

	    /* This needs to be here - if the player is running, we need to
	     * clear this each tick, but new commands are not being received
	     * so execute_newserver_command() is never called
	     */
	    pl->has_hit=0;

	} else if (pl->ob->speed_left>pl->ob->speed)
	    pl->ob->speed_left = pl->ob->speed;
    }
}

#define SPEED_DEBUG


static void process_events (mapstruct *map)
{
    object *op;
    object marker;
    tag_t tag;

    process_players1 (map);

    memset(&marker, 0, sizeof(object));
    /* Put marker object at beginning of active list */
    marker.active_next = active_objects;

    if (marker.active_next)
        marker.active_next->active_prev = &marker;
    marker.active_prev = NULL;
    active_objects = &marker;

    while (marker.active_next)
    {
        op = marker.active_next;
        tag = op->count;

        /* Move marker forward - swap op and marker */
        op->active_prev = marker.active_prev;

        if (op->active_prev)
            op->active_prev->active_next = op;
        else
            active_objects = op;

        marker.active_next = op->active_next;

        if (marker.active_next)
            marker.active_next->active_prev = &marker;
        marker.active_prev = op;
        op->active_next = &marker;

        /* Now process op */
        if (QUERY_FLAG (op, FLAG_FREED))
        {
            LOG (llevError, "BUG: process_events(): Free object on list\n");
            op->speed = 0;
            update_ob_speed (op);
            continue;
        }

        /* I've seen occasional crashes due to this - the object is removed,
         * and thus the map it points to (last map it was on) may be bogus
         * The real bug is to try to find out the cause of this - someone
         * is probably calling remove_ob without either an insert_ob or
         * free_object afterwards, leaving an object dangling.  But I'd
         * rather log this and continue on instead of crashing.
         * Don't remove players - when a player quits, the object is in
         * sort of a limbo, of removed, but something we want to keep
         * around.
         */
        if (QUERY_FLAG (op, FLAG_REMOVED) && op->type != PLAYER &&
            op->map && op->map->in_memory != MAP_IN_MEMORY)
        {
            LOG (llevError, "BUG: process_events(): Removed object on list\n");
            dump_object(op);
            LOG(llevError, errmsg);
            free_object(op);
            continue;
        }

        if ( ! op->speed)
        {
            LOG (llevError, "BUG: process_events(): Object %s has no speed, "
                "but is on active list\n", op->arch->name);
            update_ob_speed (op);
            continue;
        }

        if (op->map == NULL && op->env == NULL && op->name &&
            op->type != MAP && map == NULL)
        {
            LOG (llevError, "BUG: process_events(): Object without map or "
                "inventory is on active list: %s (%d)\n", op->name, op->count);
            op->speed = 0;
            update_ob_speed (op);
            continue;
	}

        if (map != NULL && op->map != map)
            continue;

        /* Animate the object.  Bug of feature that andim_speed
         * is based on ticks, and not the creatures speed?
         */
        if (op->anim_speed && op->last_anim >= op->anim_speed)
        {
            if ((op->type==PLAYER)||(op->type==MONSTER))
                animate_object(op, op->facing);
            else
                animate_object (op, op->direction);
            op->last_anim = 1;
        }
        else
        {
            op->last_anim++;
        }

        if (op->speed_left > 0)
        {
#if 0
	    /* I've seen occasional crashes in move_symptom() with it
	     * crashing because op is removed - add some debugging to
	     * track if it is removed at this point.
	     * This unfortunately is a bit too verbose it seems - not sure
	     * why - I think what happens is a map is freed or something and
	     * some objects get 'lost' - removed never to be reclaimed.
	     * removed objects generally shouldn't exist.
	     */
	    if (QUERY_FLAG(op, FLAG_REMOVED)) {
		LOG(llevDebug,"process_events: calling process_object with removed object %s\n",
		    op->name?op->name:"null");
	    }
#endif
            --op->speed_left;
            process_object (op);
            if (was_destroyed (op, tag))
                continue;
        }
        if (settings.casting_time == TRUE && op->casting_time > 0)
            op->casting_time--;
        if (op->speed_left <= 0)
            op->speed_left += FABS (op->speed);
    }

    /* Remove marker object from active list */
    if (marker.active_prev != NULL)
        marker.active_prev->active_next = NULL;
    else
        active_objects = NULL;

    process_players2 (map);
}

void clean_tmp_files(void) {
  mapstruct *m, *next;

  LOG(llevInfo,"Cleaning up...\n");

  /* We save the maps - it may not be intuitive why, but if there are unique
   * items, we need to save the map so they get saved off.  Perhaps we should
   * just make a special function that only saves the unique items.
   */
  for(m=first_map;m!=NULL;m=next) {
      next=m->next;
      if (m->in_memory == MAP_IN_MEMORY) {
	/* If we want to reuse the temp maps, swap it out (note that will also
	 * update the log file.  Otherwise, save the map (mostly for unique item
         * stuff).  Note that the clean_tmp_map is called after the end of
	 * the for loop but is in the #else bracket.  IF we are recycling the maps,
         * we certainly don't want the temp maps removed.
	 */

	  /* XXX The above comment is dead wrong */
	  if (settings.recycle_tmp_maps == TRUE)
	      swap_map(m);
	  else {
	      new_save_map(m, 0); /* note we save here into a overlay map */
	      clean_tmp_map(m);
	  }
      }
  }
  write_todclock(); /* lets just write the clock here */
}

/** clean up everything before exiting */
void cleanup(void)
{
    LOG(llevDebug,"Cleanup called.  freeing data.\n");
    clean_tmp_files();
    write_book_archive();
#ifdef MEMORY_DEBUG
    free_all_maps();
    free_style_maps();
    free_all_object_data();
    free_all_archs();
    free_all_treasures();
    free_all_images();
    free_all_newserver();
    free_all_recipes();
    free_all_readable();
    free_all_god();
    free_all_anim();
    /* See what the string data that is out there that hasn't been freed. */
/*    LOG(llevDebug, ss_dump_table(0xff));*/
#endif
    exit(0);
}

void leave(player *pl, int draw_exit) {

    if (pl!=NULL) {
	pl->socket.status=Ns_Dead;
	LOG(llevInfo,"LOGOUT: Player named %s from ip %s\n", pl->ob->name,
	    pl->socket.host);

	/* If this player is the captain of the transport, need to do
	 * some extra work.  By the time we get here, remove_ob()
	 * should have already been called.
	 */
	if (pl->transport && pl->transport->contr == pl) {
	    /* If inv is a non player, inv->contr will be NULL, but that
	     * is OK.
	     */
	    if (pl->transport->inv)
		pl->transport->contr = pl->transport->inv->contr;
	    else
		pl->transport->contr = NULL;

	    if (pl->transport->contr) {
		draw_ext_info_format(NDI_UNIQUE, 0, pl->transport->contr->ob,
		     MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_PLAYER,
		     "%s has left.  You are now the captain of %s",
		     "%s has left.  You are now the captain of %s",
				     pl->ob->name, query_name(pl->transport));
	    }
	}

	if (pl->ob->map) {
	    if (pl->ob->map->in_memory==MAP_IN_MEMORY)
		pl->ob->map->timeout = MAP_TIMEOUT(pl->ob->map);
	    pl->ob->map->players--;
	    pl->ob->map=NULL;
	}
	pl->ob->type = DEAD_OBJECT; /* To avoid problems with inventory window */
    }
    /* If a hidden dm dropped connection do not create
    * inconsistencies by showing that they have left the game 
    */
    if (!(QUERY_FLAG(pl->ob,FLAG_WIZ) && pl->ob->contr->hidden) && 
        (pl!=NULL && draw_exit) && 
	   (pl->state != ST_GET_NAME && pl->state!=ST_GET_PASSWORD && pl->state != ST_CONFIRM_PASSWORD)) 

	    draw_ext_info_format(NDI_UNIQUE | NDI_ALL | NDI_DK_ORANGE, 5, NULL,
			  MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_PLAYER,
			  "%s left the game.",
			  "%s left the game.",
			  pl->ob->name);
}

int forbid_play(void)
{
#if !defined(_IBMR2) && !defined(___IBMR2) && defined(PERM_FILE)
    char buf[MAX_BUF], day[MAX_BUF];
    FILE *fp;
    time_t clock;
    struct tm *tm;
    int i, start, stop, forbit=0, comp;

    clock = time (NULL);
    tm = (struct tm *) localtime (&clock);

    sprintf (buf, "%s/%s", settings.confdir, PERM_FILE);
    if ((fp = open_and_uncompress(buf, 0, &comp)) == NULL)
	return 0;

    while (fgets (buf, MAX_BUF, fp)) {
	if (buf[0]=='#') continue;
	if (!strncmp (buf, "msg", 3)) {
	    if (forbit)
		while (fgets (buf, MAX_BUF, fp))  /* print message */
		    fputs (buf, logfile);
	    break;

	} else if (sscanf (buf, "%s %d%*c%d\n", day, &start, &stop) != 3) {
	    LOG(llevDebug, "Warning: Incomplete line in permission file ignored.\n");
	    continue;
	}

	for (i=0; i< 7; i++) {
	    if (!strncmp (buf, days[i], 3) && (tm->tm_wday == i) && 
		(tm->tm_hour >= start) && (tm->tm_hour < stop))
		forbit = 1;
	}
    }

    close_and_delete(fp, comp);

    return forbit;
#else
    return 0;
#endif
}

/**
 *  do_specials() is a collection of functions to call from time to time.
 * Modified 2000-1-14 MSW to use the global pticks count to determine how
 * often to do things.  This will allow us to spred them out more often.
 * I use prime numbers for the factor count - in that way, it is less likely
 * these actions will fall on the same tick (compared to say using 500/2500/15000
 * which would mean on that 15,000 tick count a whole bunch of stuff gets
 * done).  Of course, there can still be times where multiple specials are
 * done on the same tick, but that will happen very infrequently
 *
 * I also think this code makes it easier to see how often we really are
 * doing the various things.
 */

extern unsigned long todtick;

static void do_specials(void) {

#ifdef WATCHDOG
    if (!(pticks % 503))
	watchdog();
#endif

    if (!(pticks % PTICKS_PER_CLOCK))
	tick_the_clock();

    if (!(pticks % 509))
	flush_old_maps();    /* Clears the tmp-files of maps which have reset */

    if (!(pticks % 2503))
	fix_weight();        /* Hack to fix weightproblems caused by bugs */

    if (!(pticks % 2521))
	metaserver_update();    /* 2500 ticks is about 5 minutes */

    if (!(pticks % 5003))
	write_book_archive();

    if (!(pticks % 5009))
	clean_friendly_list();

    if (!(pticks % 5011))
	obsolete_parties();

    if (!(pticks % 12503))
      fix_luck();
}

int server_main(int argc, char **argv)
{
#ifdef WIN32 /* ---win32 this sets the win32 from 0d0a to 0a handling */
    _fmode = _O_BINARY ;
    bRunning = 1;
#endif

#ifdef DEBUG_MALLOC_LEVEL
  malloc_debug(DEBUG_MALLOC_LEVEL);
#endif

  settings.argc=argc;
  settings.argv=argv;
  init(argc, argv);
  initPlugins();        /* GROS - Init the Plugins */
#ifdef WIN32
  while ( bRunning )
      {
#else
  for(;;) {
#endif 
    nroferrors = 0;

    doeric_server();
    process_events(NULL);    /* "do" something with objects with speed */
    cftimer_process_timers();/* Process the crossfire Timers */    
    /* Lauwenmark : Here we handle the CLOCK global event */
    execute_global_event(EVENT_CLOCK);
    check_active_maps(); /* Removes unused maps after a certain timeout */
    do_specials();       /* Routines called from time to time. */

    sleep_delta();	/* Slepp proper amount of time before next tick */
  }
  emergency_save( 0 );
  cleanup( );
  return 0;
}
