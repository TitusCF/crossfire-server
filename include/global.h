/*
 * static char *rcsid_global_h =
 *   "$Id$";
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

#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef EXTERN
#define EXTERN extern
#endif

#include "includes.h"


/* Type defines for specific signed/unsigned variables of a certain number
 * of bits.  Not really used anyplace, but if a certain number of bits
 * is required, these type defines should then be used.  This will make
 * porting to systems that have different sized data types easier.
 *
 * Note: The type defines should just mean that the data type has at
 * least that many bits.  if a uint16 is actually 32 bits, no big deal,
 * it is just a waste of space.
 *
 * Note2:  When using something that is normally stored in a character
 * (ie strings), don't use the uint8/sint8 typdefs, use 'char' instead.
 * The signedness for char is probably not universal, and using char
 * will probably be more portable than sint8/unit8
 */
 
typedef unsigned int	uint32;
typedef signed int	sint32;
typedef unsigned short	uint16;
typedef signed short	sint16;
typedef unsigned char	uint8;
typedef signed char	sint8;
typedef unsigned short Fontindex;

/* This blob, in this order, is needed to actually define maps */
#include "face.h"
/* Include the basic defines from spells.h */
#include "attack.h" /* needs to be before material.h */
#include "material.h"
#include "living.h"
#include "object.h"
#include "map.h"

/* Pull in the socket structure - used in the player structure */
#include "newserver.h"

/* Pull in the player structure */
#include "player.h"

/* pull in treasure structure */
#include "treasure.h"

#include "commands.h"

/*
 * So far only used when dealing with artifacts.
 * (now used by alchemy and other code too. Nov 95 b.t).
 */
typedef struct linked_char {
  char *name;
  struct linked_char *next;
} linked_char;


/* Pull in artifacts */
#include "artifact.h"

/* Now for gods */
#include "god.h"

/* Now for races */
#include "race.h"

/* Now for recipe/alchemy */
#include "recipe.h"

/* Now for spells */
#include "spells.h"

#include "funcpoint.h"

/*****************************************************************************
 * GLOBAL VARIABLES:							     *
 *****************************************************************************/


#define NUM_COLORS		13

extern char *colorname[NUM_COLORS][2];

extern New_Face *new_faces;

/*
 * These are the beginnings of linked lists:
 */
EXTERN player *first_player;
EXTERN mapstruct *first_map;
EXTERN treasurelist *first_treasurelist;
EXTERN artifactlist *first_artifactlist;
EXTERN archetype *first_archetype;
EXTERN objectlink *first_friendly_object;	/* Objects monsters will go after */
EXTERN godlink *first_god;
EXTERN racelink *first_race;

/*
 * The editor uses these (will get them out of here later):
 */

EXTERN long editor;     /* if true, edit maps instead of playing (almost obsolete) */

/*
 * Variables set by different flags (see init.c):
 */

EXTERN long warn_archetypes;	/* If true, write warnings when failing */
				/* to find archetypes when loading from file */
EXTERN long init_done;			/* Ignores signals until init_done is true */
EXTERN long trying_emergency_save;	/* True when emergency_save() is reached */
EXTERN long nroferrors;		/* If it exceeds MAX_ERRORS, call fatal() */

extern long pticks;		/* used by various function to determine */
				/* how often to save the character */
/*
 * Misc global variables:
 */
EXTERN FILE *logfile;			/* Used by server/daemon.c */
EXTERN int exiting;			/* True if the game is about to exit */
EXTERN long nroftreasures;		/* Only used in malloc_info() */
EXTERN long nrofartifacts;		/* Only used in malloc_info() */
EXTERN long nrofallowedstr;		/* Only used in malloc_info() */

/* global stuff used by new skill/experience system -b.t. */ 
#define MAX_EXP_CAT 7 		/* This should be => # of exp obj in the game 
				 * remember to include the "NULL" exp object  
			         * EXP_NONE as part of the overall tally. 
				 */
#define EXP_NONE (MAX_EXP_CAT - 1)  /* "NULL" exp. object. This is the last 
				     * experience obj always.*/ 
EXTERN short nrofexpcat;	/* Current number of experience categories in the game */
EXTERN object *exp_cat[MAX_EXP_CAT];	/* Array of experience objects in the game */ 

EXTERN archetype *empty_archetype;	/* Nice to have fast access to it */
EXTERN archetype *map_archeytpe;
EXTERN char first_map_path[MAX_BUF];	/* The start-level */

EXTERN char errmsg[HUGE_BUF];
EXTERN long ob_count;
/*
 * Used in treasure.c
 */
EXTERN archetype *ring_arch,*amulet_arch,*staff_arch,*crown_arch;
EXTERN char *undead_name;	/* Used in hit_player() in main.c */

EXTERN Animations *animations;
EXTERN int num_animations,animations_allocated;


#define SET_ANIMATION(ob,newanim) ob->face=&new_faces[animations[ob->animation_id].faces[newanim]]
#define GET_ANIMATION(ob,anim) (animations[ob->animation_id].faces[anim])
#define GET_ANIM_ID(ob) (ob->animation_id)
/* NUM_ANIMATIONS returns the number of animations allocated.  The last
 * usuable animation will be NUM_ANIMATIONS-1 (for example, if an object
 * has 8 animations, NUM_ANIMATIONS will return 8, but the values will
 * range from 0 through 7.
 */
#define NUM_ANIMATIONS(ob) (animations[ob->animation_id].num_animations)
#define NUM_FACINGS(ob) (animations[ob->animation_id].facings)

extern int freearr_x[SIZEOFFREE], freearr_y[SIZEOFFREE];
extern int maxfree[SIZEOFFREE], freedir[SIZEOFFREE];

extern New_Face *blank_face;
extern New_Face *dark_faces[];
extern New_Face *potion_face;
extern MapLook blank_look;


extern long max_time;	/* loop time */
extern NewSocket *init_sockets;


EXTERN char *font_graphic;

#ifndef __CEXTRACT__
#include "libproto.h"
#include "sockproto.h"
#endif


#define set_map(M,X,Y,C)	((M)->map[(X)+((M)->mapx*(Y))]=*(C))
#define get_map(M,X,Y)		(&(M)->map[(X)+((M)->mapx*(Y))])
#define set_map_ob(M,X,Y,tmp)	((M)->map_ob[(X)+((M)->mapx*(Y))]=(tmp))
#define get_map_ob(M,X,Y)	((M)->map_ob[(X)+((M)->mapx*(Y))])
#define out_of_map(M,X,Y)	((X)<0||(X)>=(M)->mapx||(Y)<0||(Y)>=(M)->mapy)
#define get_map_floor(M,X,Y)	(&(M)->floor[(X)+((M)->mapx*(Y))])
#define set_map_floor(M,X,Y,C)	((M)->floor[(X)+((M)->mapx*(Y))]=*(C))

#define get_map_floor2(M,X,Y)	(&(M)->floor2[(X)+((M)->mapx*(Y))])
#define set_map_floor2(M,X,Y,C)	((M)->floor2[(X)+((M)->mapx*(Y))]=*(C))

#define decrease_ob(xyz) decrease_ob_nr(xyz,1)

#ifdef CALLOC
#undef CALLOC
#endif

#ifdef USE_CALLOC
# define CALLOC(x,y)	calloc(x,y)
# define CFREE(x)	free(x)
#else
# define CALLOC(x,y)	malloc(x*y)
# define CFREE(x)	free(x)
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirnet)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

typedef struct Settings {
    char    *logfilename;   /* logfile to use */
    uint16  csport;	    /* port for new client/server */
    LogLevel debug;	    /* Default debugging level */
    uint8   dumpvalues;	    /* Set to dump various values/tables */
    char    *dumparg;       /* additional argument for some dump functions */
    uint8   daemonmode;     /* If true, detach and become daemon */
    int	    argc;	    /* parameters that were passed to the program */
    char    **argv;	    /* Only used by xio.c, so will go away at some time */
    char    *datadir;	    /* read only data files */
    char    *localdir;	    /* read/write data files */
    char    *playerdir;	    /* Where the player files are */
    char    *mapdir;	    /* Where the map files are */
    char    *archetypes;    /* name of the archetypes file - libdir is prepended */
    char    *treasures;	    /* location of the treasures file. */
    char    *uniquedir;	    /* directory for the unique items */
    char    *tmpdir;	    /* Directory to use for temporary files */
    uint8   stat_loss_on_death;	/* If true, chars lose a random stat when they die */
    uint8   use_permanent_experience; /* If true, players can gain perm exp */
    uint8   balanced_stat_loss; /* If true, Death stat depletion based on level etc */
    uint8   simple_exp;	    /* If true, use the simple experience system */
    int	    reset_loc_time; /* Number of seconds to put player back at home */

    /* The meta_ is information for the metaserver.  These are set in 
     * the lib/settings file.
     */
    uint8   meta_on:1;		    /* True if we should send updates */
    char    meta_server[MAX_BUF];   /* Hostname/ip addr of the metaserver */
    char    meta_host[MAX_BUF];	    /* Hostname of this host */
    uint16  meta_port;		    /* Port number to use for updates */
    char    meta_comment[MAX_BUF];  /* Comment we send to the metaserver */
} Settings;

extern Settings settings;

#endif /* GLOBAL_H */
