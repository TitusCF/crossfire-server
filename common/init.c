/*
 * static char *rcsid_init_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

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

    The author can be reached via e-mail to frankj@ifi.uio.no.
*/

#define EXTERN
#include <global.h>
#include <object.h>

/* You unforunately need to looking in include/structs.h to see what these
 * correspond to.
 */
struct Settings settings = {
"logfile",			/* Logfile */
CSPORT,				/* Client/server port */

/* Debug level */
#ifdef DEBUG
  llevDebug,
#else
  llevError,
#endif

0, NULL, 0,    /* dumpvalues, dumparg, daemonmode */
0, /* argc */
NULL, /* argv */
DATADIR, 
LOCALDIR,
PLAYERDIR, MAPDIR, ARCHETYPES,TREASURES, 
UNIQUE_DIR, TMPDIR,
STAT_LOSS_ON_DEATH,
SIMPLE_EXP
};


/*
 * It is vital that init_library() is called by any functions
 * using this library.
 * If you want to lessen the size of the program using the library,
 * you can replace the call to init_library() with init_globals() and
 * init_function_pointers().  Good idea to also call init_vars and
 * init_hash_table if you are doing any object loading.
 */

void init_library() {
    init_environ();
    init_globals();
    init_function_pointers();
    init_hash_table();
    init_objects();
    init_vars();
    init_block();
    ReadBmapNames ();
    init_anim();    /* Must be after we read in the bitmaps */
    init_archetypes();	/* Reads all archetypes from file */
    init_dynamic ();
}

/* init_environ initializes values from the environmental variables.
 * it needs to be called very early, since command line options should
 * overwrite these if specified.
 */
void init_environ() {
    char *cp;

#ifndef SECURE
    cp=getenv("CROSSFIRE_LIBDIR");
    if (cp) settings.datadir=cp;
    cp=getenv("CROSSFIRE_LOCALDIR");
    if (cp) settings.localdir=cp;
    cp=getenv("CROSSFIRE_PLAYERDIR");
    if (cp) settings.playerdir=cp;
    cp=getenv("CROSSFIRE_MAPDIR");
    if (cp) settings.mapdir=cp;
    cp=getenv("CROSSFIRE_ARCHETYPES");
    if (cp) settings.archetypes=cp;
    cp=getenv("CROSSFIRE_TREASURES");
    if (cp) settings.treasures=cp;
    cp=getenv("CROSSFIRE_UNIQUEDIR");
    if (cp) settings.uniquedir=cp;
    cp=getenv("CROSSFIRE_TMPDIR");
    if (cp) settings.tmpdir=cp;
#endif
}
    

/*
 * Initialises all global variables.
 * Might use environment-variables as default for some of them.
 */

void init_globals() {
    logfile = stderr;
    exiting = 0;
    first_player=NULL;
    first_friendly_object=NULL;
    first_map=NULL;
    first_treasurelist=NULL;
    first_artifactlist=NULL;
    first_archetype=NULL;
    warn_archetypes=0;
    first_map=NULL;
    nroftreasures = 0;
    nrofartifacts = 0;
    nrofallowedstr=0;
    ring_arch = NULL;
    amulet_arch = NULL;
    staff_arch = NULL;
    undead_name = add_string("undead");
    trying_emergency_save = 0;
    num_animations=0;
    animations=NULL;
    animations_allocated=0;
    init_defaults();
}

/*
 * Sets up and initialises the linked list of free and used objects.
 * Allocates a certain chunk of objects and puts them on the free list.
 * Called by init_library();
 */

void init_objects() {
  int i;
/* Initialize all objects: */
  free_objects=objarray;
  objects=NULL;
  active_objects = NULL;
  objarray[0].prev=NULL,
  objarray[0].next= &objarray[1],
  SET_FLAG(&objarray[0], FLAG_REMOVED);
  SET_FLAG(&objarray[0], FLAG_FREED);
  for(i=1;i<STARTMAX-1;i++) {
    objarray[i].next= &objarray[i+1];
    objarray[i].prev= &objarray[i-1];
    SET_FLAG(&objarray[i], FLAG_REMOVED);
    SET_FLAG(&objarray[i], FLAG_FREED);
  }
  objarray[STARTMAX-1].next=NULL;
  objarray[STARTMAX-1].prev= &objarray[STARTMAX-2];
  SET_FLAG(&objarray[STARTMAX-1], FLAG_REMOVED);
  SET_FLAG(&objarray[STARTMAX-1], FLAG_FREED);
}

/*
 * Initialises global variables which can be changed by options.
 * Called by init_library().
 */

void init_defaults() {
  editor=0;
  nroferrors=0;
}


void init_dynamic () {
    archetype *at = first_archetype;
    while (at) {
	if (at->clone.type == MAP && EXIT_PATH (&at->clone)) {
	    strcpy (first_map_path, EXIT_PATH (&at->clone));
	    return;
	}
	at = at->next;
    }
    LOG(llevDebug,"You Need a archetype called 'map' and it have to contain start map\n");
    exit (-1);
}
