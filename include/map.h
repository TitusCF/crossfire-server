/*
 * static char *rcsid_define_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel
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

/*
 * The mapstruct is allocated each time a new map is opened.
 * It contains pointers (very indirectly) to all objects on the map.
 */

#ifndef MAP_H
#define MAP_H

#if 0
/* Doing this causes problems anyplace else we want to use these as
 * variable names or structure elements.
 */
#define mapx map_object->x
#define mapy map_object->y
#endif

#define MAP_TIMEOUT(m)		((m)->map_object->value)
#define MAP_RESETTIME(m)	((m)->map_object->weight)

/* This is only used when loading the map.  For internal use (treausre
 * generation, etc). the map structure difficulty field should be used.
 */
#define MAP_DIFFICULTY(m)	((m)->map_object->level)

#define MAP_DARKNESS(m)	   	(m)->map_object->invisible>MAX_DARKNESS? \
				  MAX_DARKNESS:(m)->map_object->invisible;


/* options passed to ready_map_name and load_original_map */
#define MAP_FLUSH	    0x1
#define MAP_PLAYER_UNIQUE   0x2
#define MAP_BLOCK	    0x4
#define MAP_STYLE	    0x8

/* Values for in_memory below.  Should probably be an enumerations */
#define MAP_IN_MEMORY 1
#define MAP_SWAPPED 2
#define MAP_LOADING 3
#define MAP_SAVING 4

typedef struct mapdef {
  struct mapdef *next;	/* Next map, linked list */
  object *where;	/* What object were used to enter this map */
  char path[HUGE_BUF];	/* Filename of the map */
  char *tmpname;	/* Name of temporary file */
  long reset_time;	/* When < sec since 1970, load original instead */
  sint32 timeout;	/* When it reaches 0, the map will be swapped out */
  uint32 need_refresh:1;	/* Something went wrong, need sanity check */
  sint16 players;		/* How many plares are on this level right now */
  uint32 in_memory;	/* If not true, the map has been freed and must
		         * be loaded before used.  The map,omap and map_ob
		         * arrays will be allocated when the map is loaded */
  uint32 encounter:1;	/* True if this is a randomly generated map -
			 * prevents random encounter maps within random
			 * encounter maps
			 */
  uint8 compressed;  /* Compression method used */
  MapLook *map;
  MapLook *floor;
  MapLook *floor2;
  object **map_ob;	/* What object lies on the floor */
  uint16 difficulty;	/* What level the player should be to play here */

  /* lighting code uses these */
  uint32 do_los:1;	/* True if line of sight needs to be recalculated for 
		         * players on this map*/
  uint8 darkness;    	/* indicates level of darkness of map */
  objectlink *light;    /* linked list of lights (type object) in map */

  object *map_object;   /* Map object in new map format */
  oblinkpt *buttons;	/* Linked list of linked lists of buttons */
} mapstruct;

#endif
