/*
 * static char *rcsid_define_h =
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

/*
 * The mapstruct is allocated each time a new map is opened.
 * It contains pointers (very indirectly) to all objects on the map.
 */

#ifndef MAP_H
#define MAP_H

#define MAP_LAYERS		4

/* This is when the map will reset */
#define MAP_WHEN_RESET(m)	((m)->reset_time)

#define MAP_RESET_TIMEOUT(m)	((m)->reset_timeout)
#define MAP_DIFFICULTY(m)	((m)->difficulty)
#define MAP_TIMEOUT(m)		((m)->timeout)
#define MAP_SWAP_TIME(m)	((m)->swap_time)
#define MAP_OUTDOORS(m)		((m)->outdoor)

/* mape darkness used to enforce the MAX_DARKNESS value.
 * but IMO, if it is beyond max value, that should be fixed
 * on the map or in the code.
 */
#define MAP_DARKNESS(m)	   	(m)->darkness

#define MAP_WIDTH(m)		(m)->width
#define MAP_HEIGHT(m)		(m)->height
/* Convenient function - total number of spaces is used
 * in many places.
 */
#define MAP_SIZE(m)		((m)->width * (m)->height)

#define MAP_ENTER_X(m)		(m)->enter_x
#define MAP_ENTER_Y(m)		(m)->enter_y

#define MAP_TEMP(m)		(m)->temp
#define MAP_PRESSURE(m)		(m)->pressure
#define MAP_HUMID(m)		(m)->humid
#define MAP_WINDSPEED(m)	(m)->windspeed
#define MAP_WINDDIRECTION(m)	(m)->winddir
#define MAP_SKYCOND(m)		(m)->sky

/* options passed to ready_map_name and load_original_map */
#define MAP_FLUSH	    0x1
#define MAP_PLAYER_UNIQUE   0x2
#define MAP_BLOCK	    0x4
#define MAP_STYLE	    0x8
#define MAP_OVERLAY	    0x10

/* Values for in_memory below.  Should probably be an enumerations */
#define MAP_IN_MEMORY 1
#define MAP_SWAPPED 2
#define MAP_LOADING 3
#define MAP_SAVING 4

#define GET_MAP_FLAGS(M,X,Y)	( (M)->spaces[(X) + (M)->width * (Y)].flags )
#define SET_MAP_FLAGS(M,X,Y,C)	( (M)->spaces[(X) + (M)->width * (Y)].flags = C )
#define GET_MAP_LIGHT(M,X,Y)	( (M)->spaces[(X) + (M)->width * (Y)].light )
#define SET_MAP_LIGHT(M,X,Y,L)	( (M)->spaces[(X) + (M)->width * (Y)].light = L )

#define GET_MAP_OB(M,X,Y)	( (M)->spaces[(X) + (M)->width * (Y)].bottom )
/* legacy */
#define get_map_ob		GET_MAP_OB

#define SET_MAP_OB(M,X,Y,tmp)	( (M)->spaces[(X) + (M)->width * (Y)].bottom = (tmp) )
#define set_map_ob SET_MAP_OB

#define SET_MAP_FACE(M,X,Y,C,L) ( (M)->spaces[(X) + (M)->width * (Y)].faces[L] = C )
#define GET_MAP_FACE(M,X,Y,L) ( (M)->spaces[(X) + (M)->width * (Y)].faces[L]  )

#define SET_MAP_FACE_OBJ(M,X,Y,C,L) ( (M)->spaces[(X) + (M)->width * (Y)].faces_obj[L] = C )
#define GET_MAP_FACE_OBJ(M,X,Y,L) ( (M)->spaces[(X) + (M)->width * (Y)].faces_obj[L]  )

/* You should really know what you are doing before using this - you
 * should almost always be using out_of_map instead, which takes into account
 * map tiling.
 */
#define OUT_OF_REAL_MAP(M,X,Y) ((X)<0 || (Y)<0 || (X)>=(M)->width || (Y)>=(M)->height)

/* These are used in the MapLook flags element.  They are not used in
 * in the object flags structure.
 */

#define P_BLOCKSVIEW	0x01
#define P_NO_MAGIC      0x02	/* Spells (some) can't pass this object */
#define P_NO_PASS       0x04	/* Nothing can pass (wall() is true) */
#define P_PASS_THRU     0x08	/* */
#define P_IS_ALIVE      0x10	/* something alive is on this space */
#define P_NO_CLERIC     0x20	/* no clerical spells cast here */
#define P_NEED_UPDATE	0x40	/* this space is out of date */
#define P_NO_ERROR      0x80	/* Purely temporary - if set, update_position
                                 * does not complain if the flags are different.
                                 */

/* Can't use MapCell as that is used in newserver.h
 * Instead of having numerous arrays that have information on a
 * particular space (was map, floor, floor2, map_ob),
 * have this structure take care of that information.
 * This puts it all in one place, and should also make it easier
 * to extend information about a space.
 */

typedef struct MapSpace {
    object	*bottom;	/* lowest object on this space */
    New_Face	*faces[MAP_LAYERS];	/* faces for the 3 layers */
    object	*faces_obj[MAP_LAYERS];	/* face objects for the 3 layers */
    uint8	flags;		/* flags about this space (see the P_ values above) */
    sint8	light;		/* How much light this space provides */
} MapSpace;

/*
 * this is an overlay structure of the whole world.  It exists as a simple
 * high level map, which doesn't contain the full data of the underlying map.
 * in this map, only things such as weather are recorded.  By doing so, we
 * can keep the entire world parameters in memory, and act as a whole on
 * them at once.  We can then, in a separate loop, update the actual world
 * with the new values we have assigned.
 */

typedef struct wmapdef {
    char path[HUGE_BUF];	/* Filename of the map */
    char	*tmpname;	/* Name of temporary file */
    char 	*name;		/* Name of map as given by its creator */
    sint16	temp;		/* base temperature of this tile (F) */
    sint16	pressure;	/* barometric pressure (mb) */
    sint8	humid;		/* humitidy of this tile */
    sint8	windspeed;	/* windspeed of this tile */
    sint8	winddir;	/* direction of wind */
    sint8	sky;		/* sky conditions */
    sint32	avgelev;	/* average elevation */
    uint8 	darkness;	/* indicates level of darkness of map */
    uint8	water;		/* 0-100 percentage of water tiles */
} weathermap_t;

/* In general, code should always use the macros 
 * above (or functions in map.c) to access many of the 
 * values in the map structure.  Failure to do this will
 * almost certainly break various features.  You may think
 * it is safe to look at width and height values directly
 * (or even through the macros), but doing so will completely
 * break map tiling.
 */
typedef struct mapdef {
    struct mapdef *next;	/* Next map, linked list */
    char path[HUGE_BUF];	/* Filename of the map */
    char *tmpname;	/* Name of temporary file */
    char *name;		/* Name of map as given by its creator */
    uint32 reset_time;	/* when this map should reset */
    uint32 reset_timeout;  /* How many seconds must elapse before this map
			    * should be reset
			    */
    uint32 fixed_resettime:1;	/* if true, reset time is not affected by
				 * players entering/exiting map
				 */
    uint32 unique:1;	/* if set, this is a per player unique map */
    sint32 timeout;	/* swapout is set to this */
    sint32 swap_time;	/* When it reaches 0, the map will be swapped out */
    sint16 players;	/* How many plares are on this level right now */
    uint32 in_memory;	/* If not true, the map has been freed and must
		         * be loaded before used.  The map,omap and map_ob
		         * arrays will be allocated when the map is loaded */
    uint8 compressed;	/* Compression method used */
    uint16 difficulty;	/* What level the player should be to play here */

    uint8 darkness;    	/* indicates level of darkness of map */
    uint16 width,height;	/* Width and height of map. */
    uint16 enter_x;	/* enter_x and enter_y are default entrance location */
    uint16 enter_y;	/* on the map if none are set in the exit */
    uint32  outdoor:1;	/* True if an outdoor map */
    oblinkpt *buttons;	/* Linked list of linked lists of buttons */
    MapSpace	*spaces;    /* Array of spaces on this map */
    sint16	temp;	/* base temperature of this tile (F) */
    sint16    pressure;  /* barometric pressure (mb) */
    sint8	humid;	/* humitidy of this tile */
    sint8     windspeed; /* windspeed of this tile */
    sint8     winddir;   /* direction of wind */
    sint8     sky;	/* sky conditions */
    char    *msg;	/* Message map creator may have left */
    char    *tile_path[4];  /* path to adjoining maps */
    struct mapdef *tile_map[4];	/* Next map, linked list */
} mapstruct;

/* This is used by get_rangevector to determine where the other
 * creature is.  get_rangevector takes into account map tiling,
 * so you just can not look the the map coordinates and get the
 * righte value.  distance_x/y are distance away, which
 * can be negativbe.  direction is the crossfire direction scheme
 * that the creature should head.  part is the part of the
 * monster that is closest.
 * Note: distance should be always >=0. I changed it to UINT. MT
 */
typedef struct rv_vector {
    unsigned int	    distance;
    int	    distance_x;
    int	    distance_y;
    int	    direction;
    object  *part;
} rv_vector;

#endif
