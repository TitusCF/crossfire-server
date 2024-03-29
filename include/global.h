/**
 * @file
 * Global type definitions and header inclusions.
 */

#ifndef GLOBAL_H
#define GLOBAL_H

/**
 * Define external variables. This is used in an ugly hack where EXTERN is
 * re-defined to "" in 'init.c' in order to resolve missing symbols. Do not
 * use this macro in new code.
 */
#ifndef EXTERN
#define EXTERN extern
#endif

/* Include this first, because it lets us know what we are missing */
#if defined(WIN32) || defined(_WIN32) || defined(WIN64)
#include "win32.h"
#else
#include "autoconf.h"
/* socklen_t is defined in this file on some systems, and that type is
 * used in newserver.h, which is used in all other files
 */
#include <sys/socket.h>
#endif

#ifdef HAVE_LIBDMALLOC
#include <dmalloc.h>
#endif

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "compat.h"

/** Strings that should be manipulated through add_string() and free_string(). */
typedef const char *sstring;

#include "config.h"
#include "define.h"
#include "logger.h"
#include "shared/newclient.h"

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

/* This defines the Account_Char structure which is used in the socket */
#include "account_char.h"

/* Pull in the socket structure - used in the player structure */
#include "newserver.h"

#include "party.h"

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

/*****************************************************************************
 * GLOBAL VARIABLES:                                                         *
 *****************************************************************************/

/**
 * @defgroup first_xxx Beginnings of linked lists.
 */
/*@{*/
EXTERN player *first_player;                /**< First player. */
EXTERN mapstruct *first_map;                /**< First map. */
EXTERN region *first_region;                /**< First region. */
EXTERN artifactlist *first_artifactlist;    /**< First artifact. */
EXTERN objectlink *first_friendly_object;   /**< Objects monsters will go after. */
EXTERN godlink *first_god;                  /**< God list. */
/*@}*/

/*
 * Variables set by different flags (see init.c):
 */

EXTERN long trying_emergency_save; /**< True when emergency_save() is reached. */
EXTERN long nroferrors;            /**< If it exceeds MAX_ERRORS, call fatal() */

extern uint32_t pticks;              /**< Used by various function to determine
                                    * how often to save the character */
/**
 * @defgroup GLOBAL_VARIABLES Misc global variables.
 */
/*@{*/
EXTERN FILE *logfile;                    /**< Used by server/daemon.c */
extern int reopen_logfile;
EXTERN int exiting;                      /**< True if the game is about to exit. */
EXTERN long nrofartifacts;               /**< Only used in malloc_info(). */
EXTERN long nrofallowedstr;              /**< Only used in malloc_info(). */

EXTERN archetype *empty_archetype;       /**< Nice to have fast access to it. */
EXTERN char first_map_path[MAX_BUF];     /**< The start-level. */
EXTERN char first_map_ext_path[MAX_BUF]; /**< Path used for per-race start maps. */

EXTERN long ob_count;
#define SPELL_MAPPINGS  206
extern const char *const spell_mapping[SPELL_MAPPINGS];
/*@}*/

/*
 * Used in treasure.c
 */
EXTERN archetype *ring_arch, *amulet_arch, *crown_arch;
EXTERN const char *undead_name; /* Used in hit_player() in main.c */

/* Rotate right from bsd sum. This is used in various places for checksumming */
#define ROTATE_RIGHT(c) if ((c)&01) (c) = ((c)>>1)+0x80000000; else (c) >>= 1;

#define SET_ANIMATION(ob, newanim) { ob->face = (ob->temp_animation ? ob->temp_animation : ob->animation)->faces[newanim]; }
#define GET_ANIMATION(ob, anim) ((ob->temp_animation ? ob->temp_animation : ob->animation)->faces[anim])
#define GET_ANIM(ob) (ob->temp_animation ? ob->temp_animation : ob->animation)
#define GET_ANIM_ID(ob) (ob->temp_animation ? ob->temp_animation->num : (ob->animation ? ob->animation->num : 0))
/* NUM_ANIMATIONS returns the number of animations allocated.  The last
 * usuable animation will be NUM_ANIMATIONS-1 (for example, if an object
 * has 8 animations, NUM_ANIMATIONS will return 8, but the values will
 * range from 0 through 7.
 */
#define NUM_ANIMATIONS(ob) ((ob->temp_animation ? ob->temp_animation : ob->animation)->num_animations)
#define NUM_FACINGS(ob) ((ob->temp_animation ? ob->temp_animation : ob->animation)->facings)

extern short freearr_x[SIZEOFFREE], freearr_y[SIZEOFFREE];
extern int maxfree[SIZEOFFREE], freedir[SIZEOFFREE];

extern const Face *blank_face, *empty_face;
extern const Face *smooth_face;

extern uint32_t max_time; /* loop time */
extern socket_struct *init_sockets;

#include "stringbuffer.h"
#include "libproto.h"
#include "sockproto.h"
#include "typesproto.h"

/**
 * Free the pointer and then set it to NULL.
 * This is generally done as a safety, and having this macro
 * makes the code a bit cleaner when doing so.
 */
#define FREE_AND_CLEAR(xyz) { free(xyz); xyz = NULL; }
/**
 * Release the shared string, and set it to NULL.
 * Must not be NULL.
 */
#define FREE_AND_CLEAR_STR(xyz) { free_string(xyz); xyz = NULL; }

#define FREE_AND_CLEAR_STR_IF(xyz) { if (xyz) { free_string(xyz); xyz = NULL; } }
/**
 * Release the shared string if not NULL, and make it a reference to nv.
 */
#define FREE_AND_COPY(sv, nv) { if (sv) free_string(sv); sv = add_string(nv); }

#define FREE_AND_COPY_IF(sv, nv) { if (sv) free_string(sv); sv = nv ? add_string(nv) : NULL; }

#ifndef WIN32 /* ---win32 we define this stuff in win32.h */
#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
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

/**
 * Convert saved total experience into permanent experience
 */
#define PERM_EXP(exptotal) (exptotal * settings.permanent_exp_ratio / 100 )
#define MAX_TOTAL_EXPERIENCE (settings.permanent_exp_ratio ? (MAX_EXPERIENCE * 100 / settings.permanent_exp_ratio) : 0)

typedef void(*collectorHook)(BufferReader *, const char *);

/**
 * Server settings.
 */
typedef struct Settings {
    const char *logfilename;    /**< Logfile to use */
    uint16_t  csport;             /**< Port for new client/server */
    LogLevel debug;             /**< Default debugging level */
    uint8_t   dumpvalues;         /**< Set to dump various values/tables */
    const char *dumparg;        /**< Additional argument for some dump functions */
    const char *confdir;        /**< Configuration files */
    const char *datadir;        /**< Read only data files */
    const char *localdir;       /**< Read/write data files */
    const char *playerdir;      /**< Where the player files are */
    const char *mapdir;         /**< Where the map files are */
    const char *regions;        /**< Name of the regions file - libdir is prepended */
    const char *uniquedir;      /**< Directory for the unique items */
    const char *templatedir;    /**< Directory for the template map */
    const char *tmpdir;         /**< Directory to use for temporary files */
    uint8_t   stat_loss_on_death; /**< If true, chars lose a random stat when they die */
    int16_t  pk_luck_penalty;    /**< Amount by which player luck is reduced if they PK */
    uint8_t   permanent_exp_ratio; /**< How much exp should be 'permenant' and unable to be lost*/
    uint8_t   death_penalty_ratio; /**< Hhow much exp should be lost at death */
    uint8_t   death_penalty_level; /**< How many levels worth of exp may be lost on one death */
    uint8_t   balanced_stat_loss; /**< If true, Death stat depletion based on level etc */
    uint8_t   not_permadeth;      /**< If true, death is non-permament */
    uint8_t   simple_exp;         /**< If true, use the simple experience system */
    int     reset_loc_time;     /**< Number of seconds to put player back at home */
    uint8_t   set_title;          /**< Players can set thier title */
    uint8_t   resurrection;       /**< Ressurection possible w/ permadeth on */
    uint8_t   search_items;       /**< Search_items command */
    uint8_t   spell_encumbrance;  /**< Encumbrance effects spells */
    uint8_t   spell_failure_effects; /**< Nasty backlash to spell failures */
    uint8_t   casting_time;       /**< It takes awhile to cast a spell */
    uint8_t   real_wiz;           /**< Use mud-like wizards */
    uint8_t   recycle_tmp_maps;   /**< Re-use tmp maps. */
    uint8_t   always_show_hp;     /**< 'probe' spell HP bars for all living things (0, 1, or 2) */
    uint8_t   spellpoint_level_depend; /**< Spell costs go up with level */
    uint16_t  set_friendly_fire;       /**< Percent of damage done by peaceful player vs player damage */
    char    who_format[MAX_BUF];     /**< The format that the who command should use */
    char    who_wiz_format[MAX_BUF]; /**< The format that the who command should use when called by a dm*/
    char    motd[MAX_BUF];      /**< Name of the motd file */
    const char *rules;          /**< Name of rules file*/
    const char *news;           /**< Name of news file*/

    /* The meta_ is information for the metaserver.  These are set in
     * the lib/settings file.
     */
    unsigned int meta_on:1;         /**< True if we should send updates */
    char    meta_server[MAX_BUF];   /**< Hostname/ip addr of the metaserver */
    char    meta_host[MAX_BUF];     /**< Hostname of this host */
    uint16_t  meta_port;              /**< Port number to use for updates */
    char    meta_comment[MAX_BUF];  /**< Comment we send to the metaserver */

    uint32_t  worldmapstartx;         /**< Starting x tile for the worldmap */
    uint32_t  worldmapstarty;         /**< Starting y tile for the worldmap */
    uint32_t  worldmaptilesx;         /**< Number of tiles wide the worldmap is */
    uint32_t  worldmaptilesy;         /**< Number of tiles high the worldmap is */
    uint32_t  worldmaptilesizex;      /**< Number of squares wide in a wm tile */
    uint32_t  worldmaptilesizey;      /**< Number of squares high in a wm tile */
    uint8_t   fastclock;              /**< If true, clock goes warp 9 */

    char    *emergency_mapname;       /**< Map to return players to in emergency */
    uint16_t  emergency_x, emergency_y; /**< Coordinates to use on that map */

    int16_t  max_level;                /**< This is read out of exp_table */
    float   item_power_factor;        /**< See note in setings file */

    int     armor_max_enchant;        /**< Maximum number of times an armor can be enchanted */
    int     armor_weight_reduction;   /**< Weight reduction per enchantment */
    uint8_t   armor_weight_linear;      /**< If 1, weight reduction is linear, else exponantiel */
    int     armor_speed_improvement;  /**< Speed improvement */
    uint8_t   armor_speed_linear;       /**< If 1, speed improvement is linear, else exponantiel */
    uint8_t   no_player_stealing;       /**< If 1, can not steal from other players */
    uint8_t   create_home_portals;      /**< If 1, can create portals in unique maps (apartments) */
    uint8_t   personalized_blessings;   /**< If 1, blessed weapons get an owner and a willpower value */
    int64_t  pk_max_experience;        /**< Maximum experience one can get for PKing. Ignore if negative. */
    int     pk_max_experience_percent; /**< Percentage of experience of victim the killer gets. */
    int     allow_denied_spells_writing; /**< If set, players can write spells they can't cast. */
    int     allow_broken_converters;  /**< If set, converters will work even if price of generated item is higher
                                          * than the price of converted items. */
    int     log_timestamp;            /**< If set, log will comport a timestamp. */
    char    *log_timestamp_format;    /**< Format for timestap, if log_timestamp is set. */
    uint8_t   starting_stat_min;        /**< Minimum value of a starting stat */
    uint8_t   starting_stat_max;        /**< Maximum value of a starting stat */
    uint8_t   starting_stat_points;     /**< How many stat points character starts with */
    uint8_t   roll_stat_points;         /**< How many stat points legacy (rolled) chars start with */
    uint8_t   max_stat;                 /**< Maximum stat value - 255 should be sufficient */
    uint8_t   special_break_map;        /**< If set, then submaps in random maps can break the walls. */
    linked_char *disabled_plugins;    /**< List of disabled plugins, 'All' means all. */
    uint8_t   ignore_plugin_compatibility;    /**< If set, don't check plugin version. */
    uint8_t   account_block_create;           /** Block account creation for untrusted hosts. */
    char*   account_trusted_host;     /**< Trusted host for account creation, defaults to 127.0.0.1. */
    uint8_t crypt_mode;               /**< 0 for legacy behavior, 1 for always Traditional */
    uint8_t min_name;                         /**< Minimum characters for an account or player name */
    uint8_t hooks_count;              /**< Number of items in hooks_filename and hooks. */
    const char *hooks_filename[20];   /**< Filenames to process for each hook. */
    collectorHook hooks[20];          /**< Collect hooks, called when the filename matches. */
    int     ignore_assets_errors;     /**< If set then go on running even if there are errors in assets. */
    void    *archetypes_tracker;      /**< Must be a pointer to a AssetsTracker<archetype> if not NULL. */
} Settings;

/**
 * Server settings.
 */
extern Settings settings;

/**
 * This is used for various performance tracking statistics,
 * or just how often certain events are done.  It is much better
 * to use a common structure then variables about.
 * Note that since these are often counters, I'm using uint64s -
 * this may be overkill, but it is just a couple extra bytes.
 * This is initialized/declared in common/init.c.  Note that if
 * it only contains integer values, very easy to initialize/clear it -
 * just a memset.
 */
typedef struct Statistics {
    uint64_t spell_merges;        /**< Number of spell merges done */
    uint64_t spell_hash_full;     /**< Number of times spell hash was full*/
    uint64_t spell_suppressions;  /**< Number of times ok_to_put_more() returned FALSE*/
} Statistics;

/**
 * Merged spell statistics.
 */
extern Statistics statistics;

/**
 * @defgroup SCRIPT_FIX_xxx For plugin events
 * GROS: Those are used by plugin events (argument fixthem).
 */
#define SCRIPT_FIX_ACTIVATOR 2
#define SCRIPT_FIX_ALL 1
#define SCRIPT_FIX_NOTHING 0

#include "plugin.h"

#endif /* GLOBAL_H */
