/*
 * static char *rcsid_main_c =
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

#include <version.h>
#include <global.h>
#include <object.h>

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

static char days[7][4] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void version(object *op) {
  if(op!=NULL)
    clear_win_info(op);

  new_draw_info_format(NDI_UNIQUE, 0, op, "This is Crossfire v%s",VERSION);

/* If in a socket, don't print out the list of authors.  It confuses the
 * crossclient program.
 */
  if (op==NULL) return;
  new_draw_info(NDI_UNIQUE, 0,op,"Authors and contributors to this program:");
  new_draw_info(NDI_UNIQUE, 0,op,"mark@scruz.net (Mark Wedel)");
  new_draw_info(NDI_UNIQUE, 0,op,"frankj@ifi.uio.no (Frank Tore Johansen)");
  new_draw_info(NDI_UNIQUE, 0,op,"kjetilho@ifi.uio.no (Kjetil Torgrim Homme)");
  new_draw_info(NDI_UNIQUE, 0,op,"tvangod@ecst.csuchico.edu (Tyler Van Gorder)");
  new_draw_info(NDI_UNIQUE, 0,op,"elmroth@cd.chalmers.se (Tony Elmroth)");
  new_draw_info(NDI_UNIQUE, 0,op,"dougal.scott@fcit.monasu.edu.au (Dougal Scott)");
  new_draw_info(NDI_UNIQUE, 0,op,"wchuang@athena.mit.edu (William)");
  new_draw_info(NDI_UNIQUE, 0,op,"ftww@cs.su.oz.au (Geoff Bailey)");
  new_draw_info(NDI_UNIQUE, 0,op,"jorgens@flipper.pvv.unit.no (Kjetil Wiekhorst Jxrgensen)");
  new_draw_info(NDI_UNIQUE, 0,op,"c.blackwood@rdt.monash.edu.au (Cameron Blackwood)");
  new_draw_info(NDI_UNIQUE, 0,op,"jtraub+@cmu.edu (Joseph L. Traub)");
  new_draw_info(NDI_UNIQUE, 0,op,"rgg@aaii.oz.au (Rupert G. Goldie)");
  new_draw_info(NDI_UNIQUE, 0,op,"eanders+@cmu.edu (Eric A. Anderson)");
  new_draw_info(NDI_UNIQUE, 0,op,"eneq@Prag.DoCS.UU.SE (Rickard Eneqvist)");
  new_draw_info(NDI_UNIQUE, 0,op,"Jarkko.Sonninen@lut.fi (Jarkko Sonninen)");
  new_draw_info(NDI_UNIQUE, 0,op,"kholland@sunlab.cit.cornell.du (Karl Holland)");
  new_draw_info(NDI_UNIQUE, 0,op,"vick@bern.docs.uu.se (Mikael Lundgren)");
  new_draw_info(NDI_UNIQUE, 0,op,"mol@meryl.csd.uu.se (Mikael Olsson)");
  new_draw_info(NDI_UNIQUE, 0,op,"Tero.Haatanen@lut.fi (Tero Haatanen)");
  new_draw_info(NDI_UNIQUE, 0,op,"ylitalo@student.docs.uu.se (Lasse Ylitalo)");
  new_draw_info(NDI_UNIQUE, 0,op,"anipa@guru.magic.fi (Niilo Neuvo)");
  new_draw_info(NDI_UNIQUE, 0,op,"mta@modeemi.cs.tut.fi (Markku J{rvinen)");
  new_draw_info(NDI_UNIQUE, 0,op,"meunier@inf.enst.fr (Sylvain Meunier)");
  new_draw_info(NDI_UNIQUE, 0,op,"jfosback@darmok.uoregon.edu (Jason Fosback)");
  new_draw_info(NDI_UNIQUE, 0,op,"cedman@capitalist.princeton.edu (Carl Edman)");
  new_draw_info(NDI_UNIQUE, 0,op,"henrich@crh.cl.msu.edu (Charles Henrich)");
  new_draw_info(NDI_UNIQUE, 0,op,"schmid@fb3-s7.math.tu-berlin.de (Gregor Schmid)");
  new_draw_info(NDI_UNIQUE, 0,op,"quinet@montefiore.ulg.ac.be (Raphael Quinet)");
  new_draw_info(NDI_UNIQUE, 0,op,"jam@modeemi.cs.tut.fi (Jari Vanhala)");
  new_draw_info(NDI_UNIQUE, 0,op,"kivinen@joker.cs.hut.fi (Tero Kivinen)");
  new_draw_info(NDI_UNIQUE, 0,op,"peterm@soda.berkeley.edu (Peter Mardahl)");
  new_draw_info(NDI_UNIQUE, 0,op,"matt@cs.odu.edu (Matthew Zeher)");
  new_draw_info(NDI_UNIQUE, 0,op,"srt@sun-dimas.aero.org (Scott R. Turner)");
  new_draw_info(NDI_UNIQUE, 0,op,"huma@netcom.com (Ben Fennema)");
  new_draw_info(NDI_UNIQUE, 0,op,"njw@cs.city.ac.uk (Nick Williams)");
  new_draw_info(NDI_UNIQUE, 0,op,"Wacren@Gin.ObsPM.Fr (Laurent Wacrenier)");
  new_draw_info(NDI_UNIQUE, 0,op,"thomas@astro.psu.edu (Brian Thomas)");
  new_draw_info(NDI_UNIQUE, 0,op,"jsm@axon.ksc.nasa.gov (John Steven Moerk)");
  new_draw_info(NDI_UNIQUE, 0,op,"Images and art:");
  new_draw_info(NDI_UNIQUE, 0,op,"Peter Gardner");
  new_draw_info(NDI_UNIQUE, 0,op,"David Gervais       [david_eg@mail.com]");
  new_draw_info(NDI_UNIQUE, 0,op,"Mitsuhiro Itakura   [ita@gold.koma.jaeri.go.jp]");
  new_draw_info(NDI_UNIQUE, 0,op,"Hansjoerg Malthaner [hansjoerg.malthaner@danet.de]");
  new_draw_info(NDI_UNIQUE, 0,op,"Mårten Woxberg      [maxmc@telia.com]");
  new_draw_info(NDI_UNIQUE, 0,op,"And many more!");
}

void info_keys(object *op) {
  clear_win_info(op);
  new_draw_info(NDI_UNIQUE, 0,op,"Push `hjklynub' to walk in a direction.");
  new_draw_info(NDI_UNIQUE, 0,op,"Shift + dir = fire, Ctrl + dir = run");
  new_draw_info(NDI_UNIQUE, 0,op,"(To fire at yourself, hit `.'");
  new_draw_info(NDI_UNIQUE, 0,op,"To attack, walk into the monsters.");
  new_draw_info(NDI_UNIQUE, 0,op,"\"  = speak        ' = extended command");
  new_draw_info(NDI_UNIQUE, 0,op,"i  = inventory    , = get         : = look");
  new_draw_info(NDI_UNIQUE, 0,op,"<> = rotate       d = drop        ? = help");
  new_draw_info(NDI_UNIQUE, 0,op,"a  = apply        A = apply below t = throw");
  new_draw_info(NDI_UNIQUE, 0,op,"e  = examine      E = exa below   @ = autopick");
  new_draw_info(NDI_UNIQUE, 0,op,"C  = configure    s = brace       v = version");
  new_draw_info(NDI_UNIQUE, 0,op,"+- = change range <tab> = browse spells");
  new_draw_info(NDI_UNIQUE, 0,op,"x  = change inventory type");
  new_draw_info(NDI_UNIQUE, 0,op,"Mouse: L = examine,  M = apply,  R = drop/get");
  new_draw_info(NDI_UNIQUE, 0,op,"'help  = info about extended commands.");
  new_draw_info(NDI_UNIQUE, 0,op,"Ctrl-R = refresh   Ctrl-C = clear");
  new_draw_info(NDI_UNIQUE, 0,op,"You can type a number before most commands.");
  new_draw_info(NDI_UNIQUE, 0,op,"(For instance 3d drops 3 items.)");
}

void start_info(object *op) {
  char buf[MAX_BUF];

  sprintf(buf,"Welcome to Crossfire, v%s!",VERSION);
  new_draw_info(NDI_UNIQUE, 0,op,buf);
  new_draw_info(NDI_UNIQUE, 0,op,"Press `?' for help");
  new_draw_info(NDI_UNIQUE, 0,op," ");
  new_draw_info_format(NDI_UNIQUE | NDI_ALL, 5, op,
	"%s entered the game.",op->name);
  if(!op->contr->name_changed) {
    new_draw_info(NDI_UNIQUE, 0,op,"Note that you must set your name with the name");
    new_draw_info(NDI_UNIQUE, 0,op,"command to enter the highscore list.");
    new_draw_info(NDI_UNIQUE, 0,op,"(You can also use the crossfire.name X-resource.)");
  }
}

char *crypt_string(char *str, char *salt) {
  static char *c=
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
  char s[2];
  if(salt==NULL)
    s[0]= c[RANDOM() % (int)strlen(c)],
    s[1]= c[RANDOM() % (int)strlen(c)];
  else
    s[0]= salt[0],
    s[1]= salt[1];
#ifdef HAVE_LIBDES
  return (char*)des_crypt(str,s);
#else
  return (char*)crypt(str,s);
#endif
}

int check_password(char *typed,char *crypted) {
  return !strcmp(crypt_string(typed,crypted),crypted);
}

/* This is a basic little function to put the player back to his
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
	LOG(llevDebug,"Player %s savebed location %s is invalid - going to EMERGENCY_MAPPATH\n",
	    op->name, op->contr->savebed_map);
	strcpy(op->contr->savebed_map, EMERGENCY_MAPPATH);
	op->contr->bed_x = EMERGENCY_X;
	op->contr->bed_y = EMERGENCY_Y;
	free_string(op->contr->savebed_map);
	EXIT_PATH(tmp) = add_string(op->contr->savebed_map);
	EXIT_X(tmp) = op->contr->bed_x;
	EXIT_Y(tmp) = op->contr->bed_y;
	enter_exit(op,tmp);
    }
    free_object(tmp);
}


static char *normalize_path (char *src, char *dst) {
    char *p, *q;
    char buf[HUGE_BUF];
    static char path[HUGE_BUF];

    /* LOG(llevDebug,"path before normalization >%s<>%s<\n", src, dst); */

    if (*dst == '/') {
	strcpy (buf, dst);

    } else {
	strcpy (buf, src);
	if ((p = strrchr (buf, '/')))
	    p[1] = '\0';
	else
	    strcpy (buf, "/");
	strcat (buf, dst);
    }

    q = p = buf;
    while ((q = strstr (q, "//")))
	p = ++q;	

    *path = '\0';
    q = path;
    p = strtok (p, "/");
    while (p) {
	if (!strcmp (p, "..")) {
	    q = strrchr (path, '/');
	    if (q)
		*q = '\0';
	    else {
		*path = '\0';
		LOG (llevError, "Illegal path.\n");
	    }
	} else {
	    strcat (path, "/");
	    strcat (path, p);
	}
	p = strtok (NULL, "/");
    }
    /* LOG(llevDebug,"path after normalization >%s<\n", path); */

    return (path);
}

/* All this really is is a glorified remove_object that also updates
 * the counts on the map if needed.
 */
void leave_map(object *op)
{
    mapstruct *oldmap = op->map;

    remove_ob(op);

    if (oldmap) {
	oldmap->players--;
	if (oldmap->players <= 0) { /* can be less than zero due to errors in tracking this */
	    set_map_timeout(oldmap);
	}
    }
}

/*
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
	x=EXIT_X(newmap->map_object);
	y=EXIT_Y(newmap->map_object);
    }
    /* try to find a spot for the player */
    if (arch_blocked(op->arch, newmap, x, y)) {	/* First choice blocked */
	/* We try to find a spot for the player, starting closest in.
	 * We could use find_first_free_spot, but that doesn't randomize it at all,
	 * So for example, if the north space is free, you would always end up there even
	 * if other spaces around are available.
	 * Note that for the second and third calls, we could start at a position other
	 * than one, but then we could end up on the other side of walls and so forth.
	 */
	int i = find_free_spot(op->arch,newmap, x, y, 1, SIZEOFFREE1+1);
	if (i==-1) {
	    i = find_free_spot(op->arch,newmap, x, y, 1, SIZEOFFREE2+1);
	    if (i==-1)
		i = find_free_spot(op->arch,newmap, x, y, 1, SIZEOFFREE+1);
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
	
#ifdef USE_LIGHTING
    if(op->lights||op->glow_radius>0) remove_carried_lights(op,oldmap);
#endif

    /* If it is a player login, he has yet to be inserted anyplace.
     * otherwise, we need to deal with removing the playe here.
     */
    if(!QUERY_FLAG(op, FLAG_REMOVED))
	remove_ob(op);

    /* remove_ob clears these so they must be reset after the remove_ob call */
    op->x = x;
    op->y = y;
    op->map = newmap;
    SET_FLAG(op, FLAG_NO_APPLY);
    insert_ob_in_map(op,op->map,NULL);
    CLEAR_FLAG(op, FLAG_NO_APPLY);

    newmap->players++;
    newmap->timeout=0;
    op->enemy = NULL;

    if (op->contr) {
	strcpy(op->contr->maplevel, newmap->path);
	op->contr->count=0;
	op->contr->count_left=0;
    }

#ifdef USE_LIGHTING
    if(op->lights) add_carried_lights(op);
#endif
    /* Update any golems */
    if(op->type == PLAYER && op->contr->golem != NULL) {
	int i = find_free_spot(op->contr->golem->arch,newmap, x, y, 1, SIZEOFFREE+1);

	remove_ob(op->contr->golem);
	if (i==-1) {
	    remove_friendly_object(op->contr->golem);
	    free_object(op->contr->golem);
	    op->contr->golem=NULL;
	}
	else {
	    object *tmp;
	    for (tmp=op->contr->golem; tmp!=NULL; tmp=tmp->more) {
		tmp->x = x + freearr_x[i]+ (tmp->arch==NULL?0:tmp->arch->clone.x);
		tmp->y = y + freearr_y[i]+ (tmp->arch==NULL?0:tmp->arch->clone.y);
		tmp->map = newmap;
	    }
	    insert_ob_in_map(op->contr->golem, newmap, NULL);
	    op->contr->golem->direction = find_dir_2(op->x - op->contr->golem->x, op->y - op->contr->golem->y);
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
    if (oldmap && oldmap != newmap) {
	oldmap->players--;
	if (oldmap->players <= 0) { /* can be less than zero due to errors in tracking this */
	    set_map_timeout(oldmap);
	}
    }
    swap_below_max (EXIT_PATH(newmap->map_object));
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


/* clean_path takes a path and replaces all / with _
 * We do a strcpy so that we do not change the original string.
 */
char *clean_path(char *file)
{
    static char newpath[MAX_BUF],*cp;

    strncpy(newpath, file, MAX_BUF-1);
    newpath[MAX_BUF-1]='\0';
    for (cp=newpath; *cp!='\0'; cp++) {
	if (*cp=='/') *cp='_';
    }
    return newpath;
}


/* unclean_path takes a path and replaces all _ with /
 * This basically undoes clean path.
 * We do a strcpy so that we do not change the original string.
 * We are smart enough to start after the last / in case we
 * are getting passed a string that points to a unique map
 * path.
 */
char *unclean_path(char *src)
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


/* The player is trying to enter a randomly generated map.  In this case, generate the
 * random map as needed.
 */

static void enter_random_map(object *pl, object *exit_ob)
{
    mapstruct *new_map;
    char newmap_name[HUGE_BUF];
    static int reference_number = 0;
    RMParms rp;

    memset(&rp, 0, sizeof(RMParms));
    rp.Xsize=-1;
    rp.Ysize=-1;
    if (exit_ob->msg) set_random_map_variable(&rp,exit_ob->msg);
    rp.origin_x = exit_ob->x;
    rp.origin_y = exit_ob->y;
    rp.generate_treasure_now = 1;
    strcpy(rp.origin_map, pl->map->path);

    /* pick a new pathname for the new map.  Currently, we just
     * use a static variable and increment the counter one each time.
     */
    sprintf(newmap_name,"/random/%016d",reference_number++);

    /* now to generate the actual map. */
    new_map=(mapstruct *)generate_random_map(newmap_name,&rp);

    /* Update the exit_ob so it now points directly at the newly created
     * random maps.  Not that it is likely to happen, but it does mean that a
     * exit in a unique map leading to a random map will not work properly.
     * It also means that if the created random map gets reset before
     * the exit leading to it, that the exit will no longer work.
     */
    if(new_map) {
	int x, y;
	x=EXIT_X(exit_ob) = EXIT_X(new_map->map_object);
	y=EXIT_Y(exit_ob) = EXIT_Y(new_map->map_object);
	EXIT_PATH(exit_ob) = add_string(newmap_name);
	EXIT_PATH(new_map->map_object) = add_string(newmap_name);
	enter_map(pl, new_map, 	x, y);
    }
}


/* Code to enter/detect a character entering a unique map.
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

	if (QUERY_FLAG(exit_ob->map->map_object, FLAG_UNIQUE)) {

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
		newmap = load_original_map(create_pathname(normalize_path(reldir, EXIT_PATH(exit_ob))), MAP_PLAYER_UNIQUE);
		if (newmap) fix_auto_apply(newmap);
	    }
	}
	else {
	    /* The exit is unique, but the map we are coming from is not unique.  So
	     * use the basic logic - don't need to demangle the path name
	     */
	    sprintf(apartment, "%s/%s/%s/%s", settings.localdir,
		    settings.playerdir, op->name, 
		    clean_path(normalize_path(exit_ob->map->path, EXIT_PATH(exit_ob))));
	    newmap = ready_map_name(apartment, MAP_PLAYER_UNIQUE);
	    if (!newmap) {
		newmap = ready_map_name(normalize_path(exit_ob->map->path, EXIT_PATH(exit_ob)), 0);
		if (newmap) fix_auto_apply(newmap);
	    }
	}
    }

    if (newmap) {
	strcpy(newmap->path, apartment);
	SET_FLAG(newmap->map_object, FLAG_UNIQUE);
	enter_map(op, newmap, EXIT_X(exit_ob), EXIT_Y(exit_ob));
    } else {
	new_draw_info_format(NDI_UNIQUE, 0, op, "The %s is closed.", exit_ob->name);
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


/* Tries to move 'op' to exit_ob.  op is the character or monster that is
 * using the exit, where exit_ob is the exit object (boat, door, teleporter,
 * etc.)  if exit_ob is null, then op->contr->maplevel contains that map to
 * move the object to.  This is used when loading the player.
 *
 * Largely redone by MSW 2001-01-21 - this function was overly complex
 * and had some obscure bugs.
 */

void enter_exit(object *op, object *exit_ob) {


    /* It may be nice to support other creatures moving across
     * exits, but right now a lot of the code looks at op->contr,
     * so thta is an RFE.
     */
    if (op->type != PLAYER) return;

    /* First, lets figure out what map the player is going to go to */
    if (exit_ob) {
	/* check to see if we make a randomly generated map */
	if(EXIT_PATH(exit_ob)&&EXIT_PATH(exit_ob)[1]=='!') {
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
		newmap = ready_map_name(normalize_path(exit_ob->map->path, EXIT_PATH(exit_ob)), 0);
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
		new_draw_info_format(NDI_UNIQUE, 0, op, "The %s is closed.", exit_ob->name);
		return;
	    }

	    /* This supports the old behaviour, but it really should not be used.
	     * I will note for example that with this method, it is impossible to
	     * set 0,0 destination coordinates.  Really, if we want to support
	     * using the new maps default coordinates, the exit ob should use
	     * something like -1, -1 so it is clear to do that.
	     */
	    if (x==0 && y==0) {
		x=EXIT_X(newmap->map_object);
		y=EXIT_Y(newmap->map_object);
		LOG(llevDebug,"enter_exit: Exit %s (%d,%d) on map %s is 0 destination coordinates\n",
		    exit_ob->name?exit_ob->name:"(none)", exit_ob->x, exit_ob->y, 
		    exit_ob->map?exit_ob->map->path:"(none)");
	    }
	    enter_map(op, newmap, x, y);
	}
	/* For exits that cause damages (like pits) */
	if(exit_ob->stats.dam && op->type==PLAYER)
	    hit_player(op,exit_ob->stats.dam,exit_ob,exit_ob->attacktype);
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

	/* newmap returns the map (if already loaded), or loads it for
	 * us.
	 */
	newmap = ready_map_name(op->contr->maplevel, flags);
	if (!newmap) {
	    LOG(llevError,"enter_exit: Pathname to map does not exist! (%s)\n", op->contr->maplevel);
	    newmap = ready_map_name(EMERGENCY_MAPPATH, 0);
	    /* If we can't load the emergency map, something is probably really
	     * screwed up, so bail out now.
	     */
	    if (!newmap) {
		LOG(llevError,"enter_exit: could not load emergency map? Fata error\n");
		abort();
	    }
	}
	enter_map(op, newmap, op->x, op->y);
    }
}

/*
 * process_active_maps(): Works like process_events(), but it only
 * processes maps which a player is on.
 * It will check that it isn't called too often, and abort
 * if time since last call is less than MAX_TIME.
 *
 */

void process_active_maps() {
  mapstruct *map;


    /*
     * If enough time has elapsed, do some work.
     */
    if(enough_elapsed_time()) {
	for(map=first_map;map!=NULL;map=map->next) {
	    if(map->in_memory == MAP_IN_MEMORY) {
		if(players_on_map(map))
		    process_events(map);
	    }
	}
    }
}

/* process_players1 and process_players2 do all the player related stuff.
 * I moved it out of process events and process_map.  This was to some
 * extent for debugging as well as to get a better idea of the time used
 * by the various functions.  process_players1() does the processing before
 * objects have been updated, process_players2() does the processing that
 * is needed after the players have been updated.
 */

void process_players1(mapstruct *map)
{
    int flag;
    player *pl,*plnext;

    /* Basically, we keep looping until all the players have done their actions. */
    for(flag=1;flag!=0;) {
	flag=0;
	for(pl=first_player;pl!=NULL;pl=plnext) {


	    plnext=pl->next; /* In case a player exits the game in handle_player() */
	    if (map!=NULL && (pl->ob == NULL || pl->ob->map!=map)) continue;

#ifdef AUTOSAVE
	    /* check for ST_PLAYING state so that we don't try to save off when
	     * the player is logging in.
	     */
	    if ((pl->last_save_tick+AUTOSAVE)<pticks && pl->state==ST_PLAYING) {
		/* Don't save the player on unholy ground.  Instead, increase the
		 * tick time so it will be about 10 seconds before we try and save
		 * again.
		 */
		if (blocks_cleric(pl->ob->map, pl->ob->x, pl->ob->y)) {
		    pl->last_save_tick += 100;
		} else {
		    save_player(pl->ob,1);
		    pl->last_save_tick = pticks;
		}
	    }
#endif
	    if(pl->ob->speed_left>0) {
		if (handle_newcs_player(pl->ob))
		    flag=1;
	    } /* end if player has speed left */
	} /* end of for loop for all the players */
    } /* for flag */
    for(pl=first_player;pl!=NULL;pl=pl->next) {
	if (map!=NULL && (pl->ob == NULL || pl->ob->map!=map)) continue;
#ifdef CASTING_TIME
	if (pl->ob->casting > 0){
	    pl->ob->casting--;
	    pl->ob->start_holding = 1;
	}
	/* set spell_state so we can update the range in stats field */ 
	if ((pl->ob->casting == 0) && (pl->ob->start_holding ==1)){
	    pl->ob->start_holding = 0;
	    pl->ob->spell_state = 1;
	}
#endif
	do_some_living(pl->ob);
/*	draw(pl->ob);*/	/* updated in socket code */
    }
}

void process_players2(mapstruct *map)
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

	if(pl->has_hit) {
	    float hit_speed = pl->ob->speed/pl->weapon_sp - pl->ob->speed;

	    if (hit_speed<pl->ob->speed) hit_speed = pl->ob->speed;

	    pl->ob->speed_left+=hit_speed;
	    /* This could happen if the player still has left over speed */
	    if (pl->ob->speed_left>hit_speed) pl->ob->speed_left=hit_speed;
	}
	else if (pl->ob->speed_left>pl->ob->speed)
	    pl->ob->speed_left = pl->ob->speed;
    }
}

#define SPEED_DEBUG


void process_events (mapstruct *map)
{
  object *op;
  object marker;
  tag_t tag;

  process_players1 (map);

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
    if (QUERY_FLAG (op, FLAG_FREED)) {
      LOG (llevError, "BUG: process_events(): Free object on list\n");
      op->speed = 0;
      update_ob_speed (op);
      continue;
    }

    if ( ! op->speed) {
      LOG (llevError, "BUG: process_events(): Object %s has no speed, "
           "but is on active list\n", op->arch->name);
      update_ob_speed (op);
      continue;
    }

    if (op->map == NULL && op->env == NULL && op->name &&
        op->type != MAP && map == NULL)
    {
      LOG (llevError, "BUG: process_events(): Object without map or "
           "inventory is on active list: %s (%d)\n",
           op->name, op->count);
      op->speed = 0;
      update_ob_speed (op);
      continue;
    }

    if (map != NULL && op->map != map)
      continue;

/* Eneq(@csd.uu.se): Handle archetype-field anim_speed differently when
   it comes to the animation. If we have a value on this we don't animate it
   at speed-events. */

    if (op->anim_speed && op->last_anim >= op->anim_speed) {
      animate_object (op);
      op->last_anim = 1;
    } else {
      op->last_anim++;
    }

    if (op->speed_left > 0) {
      --op->speed_left;
      process_object (op);
      if (was_destroyed (op, tag))
        continue;
    }

#ifdef CASTING_TIME
    if (op->casting > 0)
      op->casting--;
#endif
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

void clean_tmp_files() {
  mapstruct *m;

  LOG(llevInfo,"Cleaning up...\n");

  /* We save the maps - it may not be intuitive why, but if there are unique
   * items, we need to save the map so they get saved off.  Perhaps we should
   * just make a special function that only saves the unique items.
   */
  for(m=first_map;m!=NULL;m=m->next) {
    if (m->in_memory == MAP_IN_MEMORY)
/* If we want to reuse the temp maps, swap it out (note that will also
 * update the log file.  Otherwise, save the map (mostly for unique item
 * stuff).  Note that the clean_tmp_map is called after the end of
 * the for loop but is in the #else bracket.  IF we are recycling the maps,
 * we certainly don't want the temp maps removed.
 */
#ifdef RECYCLE_TMP_MAPS
	swap_map(m);
#else
	new_save_map(m, 0);
    clean_tmp_map(m);
#endif
  }
}

/* clean up everything before exiting */
void cleanup()
{
    clean_tmp_files();
    write_book_archive();
    free_all_object_data();
    free_all_archs();
    free_all_treasures();
    free_all_images();
    free_all_ericserver();
    free_all_recipes();
    free_all_readable();
    free_all_god();
    /* See what the string data that is out there that hasn't been freed. */
/*    LOG(llevDebug, ss_dump_table(0xff));*/
    exit(0);
}

void leave(player *pl) {
    char buf[MAX_BUF];

    if (pl!=NULL) {
	/* We do this so that the socket handling routine can do the final
	 * cleanup.  We also leave that loop to actually handle the freeing
	 * of the data.
	 */
	pl->socket.status=Ns_Dead;
	LOG(llevInfo,"LOGOUT: Player named %s from ip %s\n", pl->ob->name,
	    pl->socket.host);

	(void) sprintf(buf,"%s left the game.",pl->ob->name);
	if (pl->ob->map) {
	    if (pl->ob->map->in_memory==MAP_IN_MEMORY)
		pl->ob->map->timeout = MAP_TIMEOUT(pl->ob->map);
	    pl->ob->map->players--;
	    pl->ob->map=NULL;
	}
	pl->ob->type = DEAD_OBJECT; /* To avoid problems with inventory window */
    }
  if (pl!=NULL) new_draw_info(NDI_UNIQUE | NDI_ALL, 5, NULL, buf);
}

int forbid_play()
{
#if !defined(_IBMR2) && !defined(___IBMR2) && defined(PERM_FILE)
    char buf[MAX_BUF], day[MAX_BUF];
    FILE *fp;
    time_t clock;
    struct tm *tm;
    int i, start, stop, forbit=0, comp;

    clock = time (NULL);
    tm = (struct tm *) localtime (&clock);

    sprintf (buf, "%s/%s", settings.datadir, PERM_FILE);
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

/*
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

void do_specials() {

#ifdef WATCHDOG
    if (!(pticks % 503))
	watchdog();
#endif

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

    if (!(pticks % 12503))
      fix_luck();
}


int main(int argc,char **argv)
{

#ifdef DEBUG_MALLOC_LEVEL
  malloc_debug(DEBUG_MALLOC_LEVEL);
#endif

  settings.argc=argc;
  settings.argv=argv;
  init(argc, argv);

  for(;;) {
    nroferrors = 0;

    doeric_server();
    process_events(NULL);    /* "do" something with objects with speed */
    check_active_maps(); /* Removes unused maps after a certain timeout */
    do_specials();       /* Routines called from time to time. */

    sleep_delta();	/* Slepp proper amount of time before next tick */
  }
}
