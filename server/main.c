/*
 * static char *rcsid_main_c =
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

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <version.h>
#include <global.h>
#include <object.h>

#ifndef __CEXTRACT__
#include <sproto.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <../random_maps/rproto.h>

static char days[7][4] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void version(object *op) {
  if(op!=NULL)
    clear_win_info(op);

  new_draw_info_format(NDI_UNIQUE, 0, op, "This is Crossfire v%s%s",VERSION,PATCH);

/* If in a socket, don't print out the list of authors.  It confuses the
 * crossclient program.
 */
  if (op==NULL) return;
  new_draw_info(NDI_UNIQUE, 0,op,"Authors and contributors to this program:");
  new_draw_info(NDI_UNIQUE, 0,op,"mark@pyramid.com (Mark Wedel)");
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

  sprintf(buf,"Welcome to Crossfire, v%s%s!",VERSION,PATCH);
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
  return (char*)crypt(str,s);
}

int check_password(char *typed,char *crypted) {
  return !strcmp(crypt_string(typed,crypted),crypted);
}

char *normalize_path (char *src, char *dst) {
    char *p, *q;
    char buf[MAX_BUF];
    static char path[MAX_BUF];

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

/* Tries to move 'op' to exit_ob.  op is the character or monster that is
 * using the exit, where exit_ob is the exit object (boat, door, teleporter,
 * etc.)  if exit_ob is null, then op->maplevel contains that map to
 * move the object to (and use default map coordiantes
 */

void enter_exit(object *op, object *exit_ob) {
  char apartment[MAX_BUF];
  mapstruct *oldmap, dummy_map;
  int x=0, y=0, removed=QUERY_FLAG(op,FLAG_REMOVED);
  char *newpath=NULL, *lastlevel=NULL;
  int last_x = op->x, last_y = op->y, unique=0;

  dummy_map.pending = (objectlink *) NULL;
  dummy_map.in_memory = MAP_LOADING;
  *apartment='\0';

  /* First, lets figure out what map the player is going to go to */
  if (exit_ob) {
	 x=EXIT_X(exit_ob);
	 y=EXIT_Y(exit_ob);

	 /* check to see if we make a randomly generated map */
	 if(EXIT_PATH(exit_ob)&&EXIT_PATH(exit_ob)[1]=='!') 
		{
		  mapstruct *new_map;
		  FILE *newmap_params;  /* give the new map its parameters */
		  newmap_params=fopen("/tmp/rmap_params","w");
		  if(newmap_params!=NULL) {
			 char newmap_name[MAX_BUF];
			 char oldmap_name[MAX_BUF];
			 int i;

			 /* write the map parameters to the file. */
			 fprintf(newmap_params,"%s",exit_ob->msg);

			 /* provide for returning to where we came from. */
			 fprintf(newmap_params,"origin_map %s\n",op->map->path);
			 fprintf(newmap_params,"origin_x %d\n",exit_ob->x);
			 fprintf(newmap_params,"origin_y %d\n",exit_ob->y);
			 /* cause there to be treasure. */
			 fprintf(newmap_params,"generate_treasure_now 1\n");
			 fclose(newmap_params);
			 
			 /* change the old map path into a single token */
			 strcpy(oldmap_name,op->map->path);
			 for(i=0;i<strlen(oldmap_name);i++) {
				if(oldmap_name[i]=='/' || oldmap_name[i]=='.') oldmap_name[i]='_';
			 }

			 /* pick a new pathname for the new map:  it is of the form
			  oldmapname_x_y with underscores instead of '/' and '.', with
			  the entrance coordinates tacked on. */
			 sprintf(newmap_name,"/random/%s_%d_%d",oldmap_name,exit_ob->x,exit_ob->y);

			 /* now to generate the actual map. */
			 new_map=(mapstruct *)generate_random_map("/tmp/rmap_params",newmap_name);
			 
			 /* set the hp,sp,path of the exit for the new */
			 if(new_map) {
				x=exit_ob->stats.hp = new_map->map_object->stats.hp;	
				y=exit_ob->stats.sp = new_map->map_object->stats.sp;
				exit_ob->slaying = add_string(newmap_name);
			 }
		  }
		  else {
			 LOG(llevError,"Couldn't open parameter-passing file for random map.");
		  }
		}
	 if (EXIT_PATH(exit_ob)) {
		newpath = normalize_path (exit_ob->map->path, EXIT_PATH(exit_ob));
	 } else {
		if (EXIT_LEVEL (exit_ob) > 0)
		  LOG(llevError,"Number Map levels are no more supported\n");
		return;
	 }
  } else
	 newpath = op->contr->maplevel;


  /* If no map path has been found yet, just keep the player on the
	* map he is on right now
	*/
  if(!newpath)
	 newpath = op->map->path;

  /* If the exit is marked unique, this is a special 'apartment' map -
	* a unique one for each player.
	*/
  if (op->type==PLAYER && exit_ob && QUERY_FLAG(exit_ob, FLAG_UNIQUE)) {
	 sprintf(apartment, "%s/%s/%s/%s", settings.localdir,
				settings.playerdir, op->name, clean_path(newpath));

	 /* If we already have a unique map for the player, use it.
	  * Otherwise, we fall through below where
	  * depending on values where it checks for the original map again
	  */
	 if (check_path(apartment,0)!= -1) {
		newpath = apartment;
		unique=1;
	 }
  }

  /* If notunique has been cleared, then the stuff above has already
	* found a map, so no need to check again.  Otherwise, check
	* to see if the actual file exists.
	* We check to see if a unique version also exists - needed if the
	* player saves on the unique map.
	*/
  if (!unique && !has_been_loaded(newpath) && 
      check_path(newpath,!unique) == -1) {
	 if (check_path(newpath, 0) == -1) {
		new_draw_info_format(NDI_UNIQUE, 0,op, "The %s is closed.", newpath);
		return;
	 }
	 else {
		unique=1;
	 }
  }

  /* Clear the player's count, and reset direction */
  op->direction=0;
  op->contr->count=0;
  op->contr->count_left=0;

  /* For exits that cause damages (like pits) */
  if(exit_ob && exit_ob->stats.dam && op->type==PLAYER)
	 hit_player(op,exit_ob->stats.dam,exit_ob,exit_ob->attacktype);

  /* Keep track of the map the player is on right now */
  oldmap=op->map;


  /* When logging in, the object is already removed.  If that is
	* not the case, then the following is used */
  if(!removed) {
	 remove_ob(op);

	 lastlevel = oldmap->path;
	 op->map = NULL;

	 /* If we are changing maps, do some extra logic.  In theory,
	  * if the player is going to the same map, we could do this much
	  * simply and just update the player x,y
	  */
	 if(strcmp (newpath, oldmap->path)) {
		/* Remove any golems */
		if(op->contr->golem) {
		  remove_friendly_object(op->contr->golem);
		  remove_ob(op->contr->golem);
		  free_object(op->contr->golem);
		  op->contr->golem=NULL;
		}

		/* This stuff should probably be moved to ready_map: */
		op->map = &dummy_map;

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
		/* Swap out the oldest map if low on mem */
		swap_below_max (newpath);
#else
		/* save out the map */
		swap_map(oldmap);
#endif /* MAP_MAXTIMEOUT */
	 }
	 oldmap->players--;
  }

  /* Do any processing to get the map loaded and ready */
  op->map = ready_map_name(newpath,(unique?MAP_PLAYER_UNIQUE:0));


  /* Did the load fail for some reason?  If so, put the player back on the
	* map they came from and remove the exit that pointed the player to
	* the bad map.
	*/
  if (op->map==NULL) { /* Something went wrong, try to go back */
	 int x2,y2;
	 object *enc;

	 LOG(llevError,"Error, couldn't open map %s.\n", newpath);
	 op->map = ready_map_name (lastlevel, 0);
	 x = last_x;
	 y = last_y;
	 LOG(llevDebug, "Trying to remove all entries to the map.\n");

	 for (x2 = (-2); x2 < 3; x2++) {
		for (y2 = (-2); y2 < 3; y2++) {
		  if (out_of_map(op->map, x2 + op->x, y2 + op->y))
		    continue;
		  enc = get_map_ob(op->map, x2 + op->x, y2 + op->y);
		  if (!enc)
		    continue;
		  if (enc->type != ENCOUNTER || enc->slaying == NULL ||
				strcmp(enc->slaying, newpath))
			 continue;
		  free_string(enc->slaying);
		  enc->slaying = (char *) NULL;
		}
	 }
  }
  /* If we got the map we wanted and it is UNIQUE, we need to update
	* it so it gets saved in the right place.  Set unique so that
	* when we save it, it knows to save it in the right place
	*/
  else if (exit_ob && QUERY_FLAG(exit_ob, FLAG_UNIQUE)) {
	 strcpy(op->map->path, apartment);
	 SET_FLAG(op->map->map_object, FLAG_UNIQUE);
  }
	    
  op->map->players++;

  /* If objects are being updated while the map is loading, things like
	* pets will be put into the players map, pending element.  That is
	* really just the dummy_map.  Since the map has actually loaded now,
	* we want to append that list to the 'real' loaded maps pending
	* list -- MSW
	*/
  if(dummy_map.pending != (objectlink *) NULL) {
	 objectlink *obl;

	 for(obl = op->map->pending; obl!= NULL && obl->next != NULL; 
		  obl=obl->next);
	 if(obl == NULL)
		op->map->pending = dummy_map.pending;
	 else
		obl->next = dummy_map.pending;
  }

  op->contr->loading = op->map;
  op->contr->new_x = x;
  op->contr->new_y = y;
  op->contr->removed = removed;
  op->map->timeout = 0;

#ifdef USE_LIGHTING
  if(op->lights||op->glow_radius>0) remove_carried_lights(op,oldmap);
#endif
  enter_map(op);
}

/*
 *  enter_map():  handles the final stages of entering a new map.
 */

void enter_map(object *op) {
  op->map = op->contr->loading;
  op->contr->loading = NULL;
  if((op->contr->new_x || op->contr->new_y) &&
     !out_of_map(op->map,op->contr->new_x,op->contr->new_y))
    op->x = op->contr->new_x, op->y = op->contr->new_y;
  else {
    int i = find_free_spot(op->arch,op->map,
                           EXIT_X(op->map->map_object),
			   EXIT_Y(op->map->map_object),1,9);
    /* If no free spot, put on preset destination */
    if (i==-1) i=0;
    op->x = EXIT_X(op->map->map_object) + freearr_x[i];
    op->y = EXIT_Y(op->map->map_object) + freearr_y[i];
    if (out_of_map (op->map, op->x, op->y)) {
	LOG (llevDebug, "Broken map: Start position is out of map (%s)!\n",
	     op->map->path);
	op->x = 0;
	op->y = 0;
    }
  }
  if(!op->contr->removed) {
    SET_FLAG(op, FLAG_NO_APPLY);
    insert_ob_in_map(op,op->map);
    CLEAR_FLAG(op, FLAG_NO_APPLY);
  }
  op->enemy = NULL;
#if 0
  op->contr->drawn[5][5].number = 0;
  /*
   * draw_colorpix routine cannot find out, if the background of player has
   * changed, and would not redraw player...
   */
#endif
#ifdef USE_LIGHTING
  if(op->lights) add_carried_lights(op);
#endif
  enter_pending_objects(op->map);
}

/*
 * process_active_maps(): Works like process_events(), but it only
 * processes maps which are loaded and readied.
 * It will check that it isn't called too often, and abort
 * if time since last call is less than MAX_TIME.
 *
 */

void process_active_maps() {
  mapstruct *map;

#if 0
  /* Even in single player mode, it is nice to still be able to do stuff
   * while a map is saving.
   */
  if(first_player==NULL || first_player->next==NULL)
    return; /* No point in the following in one-player mode */
#endif

/*
 * If enough time has elapsed, do some work.
 */
  if(enough_elapsed_time())
    for(map=first_map;map!=NULL;map=map->next)
      if(map->in_memory == MAP_IN_MEMORY) {
        if(players_on_map(map)==0)
          continue;
        process_events(map);
      }
}

#if 0
/* process_events has been extended to take a map arguement, and
 * if that is non null, only process objects on that map.  As such,
 * process_map is no longer needed.
 */
/*
 * process_map(map): works like process_events(), but it only processes
 * objects within the map 'map'.
 */

void process_map(mapstruct *map) {
  int flag;
  object *op, *next;

  process_players1(map);

  for(op=objects;op!=NULL;op=next) {
    if(QUERY_FLAG(op, FLAG_FREED)) { /* Was this object somehow freed as a result of */
                       /* the actions of the previous object? */ 
      flag=0;
      break;
    }
    next=op->next;

    if(op->map!=map || op->type == PLAYER)
      continue;

    if (op->last_anim>=op->anim_speed && op->anim_speed != 0) {
      animate_object(op);
      op->last_anim=1;
    } else op->last_anim++; 
#ifdef CASTING_TIME
    if (op->casting > 0){
      op->casting--;
    }
#endif
    if(op->speed_left>0) {
      --op->speed_left;
      if(process_object(op))
        continue;
    }
  }
/* Now go through all objects and give them new speed */
  for(op=objects;op!=NULL;op=op->next)
    if(op->map==map&&op->speed&&op->speed_left<=0)
      op->speed_left+=op->speed>0?op->speed:-op->speed;

  process_players2(map);
}
#endif

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
	draw(pl->ob);
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


void process_events(mapstruct *map) {
  object *op, *next;

 process_players1(map);

 for (op=active_objects; op!=NULL; op=next) {
    if(QUERY_FLAG(op, FLAG_FREED)) { /* Was this object somehow freed as a result of */
                       /* the actions of the previous object? */ 
      LOG(llevDebug,"Free object on list, process_events\n");
      next=op->active_next;
	continue; /* If we have a free object, any values it might
		   * have are likely garbage.  breaking is probabyl
		   * best, but let as at least continue and hope
		   * for the best
		   */
/*      break;*/
    }

    next=op->active_next;

    if (op->map == NULL && op->env == NULL && op->name &&
        op->type!=MAP && map==NULL)
    {
      LOG(llevError, "Object without map or inventory: %s (%d)\n",
              op->name, op->count);
      continue;
    }

    if (map!=NULL && op->map!=map)
	continue;

    if(!op->speed) {
      /* This is not a real problem, unless it repeats for the the same
       * object.  IF it just gets printed once, it likely means that that
       * object was killed/changed by the previous object
       */
      LOG(llevDebug, "Object %s has no speed, but is on active list\n",
	op->arch->name);
      continue;
    }

/* Eneq(@csd.uu.se): Handle archetype-field anim_speed differently when
   it comes to the animation. If we have a value on this we don't animate it
   at speed-events. */

    if (op->anim_speed && op->last_anim>=op->anim_speed) {
      animate_object(op);
      op->last_anim=1;
    } else op->last_anim++; 

    if(op->speed_left>0) {
      --op->speed_left;
      process_object(op);
    }
#ifdef CASTING_TIME
    if (op->casting > 0)
      op->casting--;
#endif
    if (op->speed_left <= 0)
	op->speed_left+=(op->speed>0?op->speed:-op->speed);
  }

  process_players2(map);
}

void clean_tmp_files() {
  mapstruct *m;

  LOG(llevError,"Cleaning up...\n");

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
 */


void do_specials() {
  static int special_count = 0, special2 = 0;

  if(++special_count < 500)
    return;
  special_count = 0;
  special2++;

#ifdef WATCHDOG
  watchdog();
#endif

  flush_old_maps();    /* Clears the tmp-files of maps which have reset */
  if(!(special2%5)) {
    fix_weight();        /* Hack to fix weightproblems caused by bugs */
    if (!(special2%25))
      fix_luck();
  }
  if (!(special2%10)) write_book_archive();
}

/*
 * last_time is when the last tick was executed.
 * We don't need to know the timezone since we're only interested in
 * the delta time since the last 'tick' .
 */
extern struct timeval last_time;
struct timezone dummy_timezone;

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
