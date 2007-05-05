/*
 * static char *rcsid_global_h =
 *   "$Id$";
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

    The author can be reached via e-mail to crossfire-devel@real-time.com
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

#ifndef UINT32_MAX
#define UINT32_MAX	4294967295U
#endif

typedef signed int	sint32;
#define SINT32_MAX	2147483647

typedef unsigned short	uint16;
#ifndef UINT16_MAX
#define UINT16_MAX	65535
#endif

typedef signed short	sint16;
#define SINT16_MAX	32767

typedef unsigned char	uint8;
#ifndef UINT8_MAX
#define UINT8_MAX	255
#endif

typedef signed char	sint8;
#define SINT8_MAX	127

/** Strings that should be manipulated through add_string() and free_string(). */
typedef const char* sstring;

typedef unsigned short Fontindex;

#ifdef WIN32
/* Python plugin stuff defines SIZEOF_LONG_LONG as 8, and besides __int64 is a 64b type on MSVC...
 * So let's force the typedef */
typedef unsigned __int64	uint64;
typedef signed __int64		sint64;
/* Needed for experience */
#define atoll	_atoi64

#define FMT64		    "I64d"
#define FMT64U		    "I64u"

/* To reduce number of warnings */
#pragma warning( disable: 4244 ) /* conversion from 'xxx' to 'yyy', possible loss of data */
#pragma warning( disable: 4305 ) /* initializing float f = 0.05; instead of f = 0.05f; */

#else /* WIN32 */

#if SIZEOF_LONG == 8

typedef unsigned long	    uint64;
typedef signed long	    sint64;
#define FMT64		    "ld"
#define FMT64U		    "ld"

#elif SIZEOF_LONG_LONG == 8
typedef unsigned long long	uint64;
typedef signed long long 	sint64;
#define FMT64		    "lld"
#define FMT64U		    "lld"

#else
#error do not know how to get a 64 bit value on this system.
#error correct and send mail to crossfire-devel on how to do this
#endif

#endif

/* This blob, in this order, is needed to actually define maps */
#include "face.h"
/* Include the basic defines from spells.h */
#include "attack.h" /* needs to be before material.h */
#include "material.h"
#include "living.h"
#include "object.h"
#include "map.h"
#include "tod.h"

#include "skills.h"

/* Pull in the socket structure - used in the player structure */
#include "newserver.h"

/* Pull in the player structure */
#include "player.h"

/* pull in treasure structure */
#include "treasure.h"

#include "commands.h"

/* pull in book structures */
#include "book.h"

/* ob_methods and ob_types subsystem */
#include "ob_methods.h"
#include "ob_types.h"

/*
 * So far only used when dealing with artifacts.
 * (now used by alchemy and other code too. Nov 95 b.t).
 */
typedef struct linked_char {
  const char *name;
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

extern New_Face *new_faces;

/*
 * These are the beginnings of linked lists:
 */
EXTERN player *first_player;
EXTERN mapstruct *first_map;
EXTERN region *first_region;
EXTERN treasurelist *first_treasurelist;
EXTERN artifactlist *first_artifactlist;
EXTERN archetype *first_archetype;
EXTERN objectlink *first_friendly_object;	/* Objects monsters will go after */
EXTERN godlink *first_god;
EXTERN racelink *first_race;

#define NROF_COMPRESS_METHODS 4
EXTERN const char *uncomp[NROF_COMPRESS_METHODS][3];
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

extern uint32 pticks;		/* used by various function to determine */
				/* how often to save the character */
/*
 * Misc global variables:
 */
EXTERN FILE *logfile;			/* Used by server/daemon.c */
EXTERN int exiting;			/* True if the game is about to exit */
EXTERN long nroftreasures;		/* Only used in malloc_info() */
EXTERN long nrofartifacts;		/* Only used in malloc_info() */
EXTERN long nrofallowedstr;		/* Only used in malloc_info() */

EXTERN archetype *empty_archetype;	/* Nice to have fast access to it */
EXTERN archetype *map_archeytpe;
EXTERN char first_map_path[MAX_BUF];	/* The start-level */
EXTERN char first_map_ext_path[MAX_BUF]; /* Path used for per-race start maps */

EXTERN long ob_count;
/*
 * Used in treasure.c
 */
EXTERN archetype *ring_arch,*amulet_arch,*staff_arch,*crown_arch;
EXTERN const char *undead_name;	/* Used in hit_player() in main.c */

EXTERN Animations *animations;
EXTERN int num_animations,animations_allocated, bmaps_checksum;

/* Rotate right from bsd sum. This is used in various places for checksumming */
#define ROTATE_RIGHT(c) if ((c) & 01) (c) = ((c) >>1) + 0x80000000; else (c) >>= 1;


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

extern short freearr_x[SIZEOFFREE], freearr_y[SIZEOFFREE];
extern int maxfree[SIZEOFFREE], freedir[SIZEOFFREE];
extern int rightof_x[9], rightof_y[9];
extern int leftof_x[9], leftof_y[9];

extern New_Face *blank_face, *empty_face;
extern New_Face *dark_faces[];
extern New_Face *smooth_face;


extern uint32 max_time;	/* loop time */
extern socket_struct *init_sockets;


#ifndef __CEXTRACT__
#include "libproto.h"
#include "sockproto.h"
#include "typesproto.h"
#endif


#define decrease_ob(xyz) decrease_ob_nr(xyz,1)

/* FREE_AND_CLEAR frees the pointer and then sets it to NULL.
 * This is generally done as a safety, and having this macro
 * makes the code a bit cleaner when doing so.
 */
#define FREE_AND_CLEAR(xyz) {free((void*)xyz); xyz=NULL; }
#define FREE_AND_CLEAR_STR(xyz) {free_string(xyz); xyz=NULL; }

/* FREE_AND_COPY is for the shared string - it is handy enough
 * to use all over the place.
 */
#define FREE_AND_COPY(sv,nv) { if (sv) free_string(sv); sv=add_string(nv); }

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

#ifndef WIN32 /* ---win32 we define this stuff in win32.h */
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
#endif

typedef struct Settings {
    char    *logfilename;   /**< Logfile to use */
    uint16  csport;	    /**< Port for new client/server */
    LogLevel debug;	    /**< Default debugging level */
    uint8   dumpvalues;	    /**< Set to dump various values/tables */
    char    *dumparg;       /**< Additional argument for some dump functions */
    uint8   daemonmode;     /**< If true, detach and become daemon */
    int	    argc;	    /**< Parameters that were passed to the program */
    char    **argv;	    /**< Only used by xio.c, so will go away at some time */
    char    *confdir;	    /**< Configuration files */
    char    *datadir;	    /**< Read only data files */
    char    *localdir;	    /**< Read/write data files */
    char    *playerdir;	    /**< Where the player files are */
    char    *mapdir;	    /**< Where the map files are */
    char    *archetypes;    /**< Name of the archetypes file - libdir is prepended */
    char    *regions;       /**< Name of the regions file - libdir is prepended */
    char    *treasures;	    /**< Location of the treasures file. */
    char    *uniquedir;	    /**< Directory for the unique items */
    char    *templatedir;   /**< Directory for the template map */
    char    *tmpdir;	    /**< Directory to use for temporary files */
    uint8   stat_loss_on_death;	/**< If true, chars lose a random stat when they die */
    sint16  pk_luck_penalty;    /**< Amount by which player luck is reduced if they PK */
    uint8   permanent_exp_ratio; /**< How much exp should be 'permenant' and unable to be lost*/
    uint8   death_penalty_ratio; /**< Hhow much exp should be lost at death */
    uint8   death_penalty_level; /**< How many levels worth of exp may be lost on one death */
    uint8   balanced_stat_loss; /**< If true, Death stat depletion based on level etc */
    uint8   not_permadeth;  /**< If true, death is non-permament */
    uint8   simple_exp;	    /**< If true, use the simple experience system */
    int	    reset_loc_time; /**< Number of seconds to put player back at home */
    uint8   set_title;	    /**< Players can set thier title */
    uint8   resurrection;   /**< Ressurection possible w/ permadeth on */
    uint8   search_items;   /**< Search_items command */
    uint8   spell_encumbrance; /**< Encumbrance effects spells */
    uint8   spell_failure_effects; /**< Nasty backlash to spell failures */
    uint16  set_friendly_fire;	/**< Percent of damage done by peaceful player vs player damage */
    uint8   casting_time;   /**< It takes awhile to cast a spell */
    uint8   real_wiz;       /**< Use mud-like wizards */
    uint8   recycle_tmp_maps; /**< Re-use tmp maps. */
    uint8   explore_mode;     /**< Explore mode allowed? */
    uint8   spellpoint_level_depend; /**< Spell costs go up with level */
    char    who_format[MAX_BUF]; /**< The format that the who command should use */
    char    who_wiz_format[MAX_BUF]; /**< The format that the who command should use when called by a dm*/
    char    motd[MAX_BUF];    /**< Name of the motd file */
    char    *rules;           /**< Name of rules file*/
    char    *news;            /**< Name of news file*/
    char    dm_mail[MAX_BUF]; /**< DM's Email address */

    /* The meta_ is information for the metaserver.  These are set in 
     * the lib/settings file.
     */
    uint8   meta_on:1;		    /**< True if we should send updates */
    char    meta_server[MAX_BUF];   /**< Hostname/ip addr of the metaserver */
    char    meta_host[MAX_BUF];	    /**< Hostname of this host */
    uint16  meta_port;		    /**< Port number to use for updates */
    char    meta_comment[MAX_BUF];  /**< Comment we send to the metaserver */

    uint32  worldmapstartx;	    /**< Starting x tile for the worldmap */
    uint32  worldmapstarty;	    /**< Starting y tile for the worldmap */
    uint32  worldmaptilesx;	    /**< Number of tiles wide the worldmap is */
    uint32  worldmaptilesy;	    /**< Number of tiles high the worldmap is */
    uint32  worldmaptilesizex;	    /**< Number of squares wide in a wm tile */
    uint32  worldmaptilesizey;	    /**< Number of squares high in a wm tile */
    uint16  dynamiclevel;	    /**< How dynamic is the world? */
    uint8   fastclock;		    /**< If true, clock goes warp 9 */

    char    *emergency_mapname;	    /**< Map to return players to in emergency */
    uint16  emergency_x, emergency_y;	/**< Coordinates to use on that map */

    sint16  max_level;		    /**< This is read out of exp_table */
    float   item_power_factor;	    /**< See note in setings file */

    int     armor_max_enchant;  /**< Maximum number of times an armor can be enchanted */
    int     armor_weight_reduction; /**< Weight reduction per enchantment */
    uint8   armor_weight_linear;  /**< If 1, weight reduction is linear, else exponantiel */
    int     armor_speed_improvement;    /**< Speed improvement */
    uint8   armor_speed_linear;         /**< If 1, speed improvement is linear, else exponantiel */
    uint8   no_player_stealing;     /**< If 1, can not steal from other players */
    uint8   create_home_portals;     /**< If 1, can create portals in unique maps (apartments) */
    uint8   personalized_blessings; /**< If 1, blessed weapons get an owner and a willpower value */
    sint64  pk_max_experience; /**< Maximum experience one can get for PKing. Ignore if negative. */
    int     pk_max_experience_percent; /**< Percentage of experience of victim the killer gets. */
    int     allow_denied_spells_writing; /**< If set, players can write spells they can't cast. */
    int     allow_broken_converters; /**< If set, converters will work even if price of generated item is higher
                                       * than the price of converted items. */
} Settings;

extern Settings settings;

/* 0.94.1 - change to GETTIMEOFDAY macro - SNI systems only one one option.
 * rather than have complex #ifdefs throughout the file, lets just figure
 * it out once, here at the top.
 * Have no idea if that is the right symbol to check on for NetBSD,
 * but NetBSD does use 2 params.
 * Move this to global.h from time.c since its also used in arch.c
 */

#ifdef GETTIMEOFDAY_TWO_ARGS
#define GETTIMEOFDAY(last_time) gettimeofday(last_time, (struct timezone *) NULL);
#else
#define GETTIMEOFDAY(last_time) gettimeofday(last_time);
#endif

/* GROS: Those are used by plugin events (argument fixthem) */
#define SCRIPT_FIX_ACTIVATOR 2
#define SCRIPT_FIX_ALL 1
#define SCRIPT_FIX_NOTHING 0

#include "plugin.h"

#endif /* GLOBAL_H */
