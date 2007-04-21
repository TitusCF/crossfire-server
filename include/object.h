/*
 * static char *rcsid_object_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel & Crossfire Development Team
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

#ifndef OBJECT_H
#define OBJECT_H

typedef uint32 tag_t;
#define NUM_BODY_LOCATIONS	12
#define BODY_ARMS		1

/* See common/item.c */

typedef struct body_locations_struct {
    const char *save_name;	/**< Name used to load/save it to disk */
    const char *use_name;	/**< Name used when describing an item we can use */
    const char *nonuse_name;	/**< Name to describe objects we can't use */
} body_locations_struct;

extern body_locations_struct body_locations[NUM_BODY_LOCATIONS];

/**
 * Each object (this also means archetypes!) could have a few of these
 * "dangling" from it; this could also end up containing 'parse errors'.
 *
 * key and value are shared-strings.
 *
 * Please use get_ob_key_value(), set_ob_key_value() from object.c rather than
 * accessing the list directly.
 * Exception is if you want to walk this list for some reason.
 */
typedef struct _key_value {
    const char * key;
    const char * value;
    struct _key_value * next;
} key_value;


/* Definition for WILL_APPLY values.  Replaces having harcoded values
 * sprinkled in the code.  Note that some of these also replace fields
 * that were in the can_apply area.  What is the point of having both
 * can_apply and will_apply?
 */
#define WILL_APPLY_HANDLE	0x1
#define WILL_APPLY_TREASURE	0x2
#define WILL_APPLY_EARTHWALL	0x4
#define WILL_APPLY_DOOR		0x8
#define WILL_APPLY_FOOD		0x10


/**
 * Note that the ordering of this structure is sort of relevent -
 * copy_object copies everything over beyond 'name' using memcpy.
 * Thus, values that need to be copied need to be located beyond that
 * point.
 *
 * However, if you're keeping a pointer of some sort, you probably
 * don't just want it copied, so you'll need to add to common/object.c,
 * e.g. copy-object
 *
 * I've tried to clean up this structure a bit (in terms of formatting)
 * by making it more consistent.  I've also tried to locate some of the fields
 * more logically together (put the item related ones together, the monster
 * related ones, etc.
 * This structure is best viewed with about a 100 width screen.
 * MSW 2002-07-05
 */
typedef struct obj {
    /* These variables are not changed by copy_object() */
    struct pl	*contr;		/**< Pointer to the player which control this object */
    struct obj	*next;		/**< Pointer to the next object in the free/used list */
    struct obj	*prev;		/**< Pointer to the previous object in the free/used list*/
    struct obj	*active_next;	/**< Next object in the 'active' list
                                 * This is used in process_events
                                 * so that the entire object list does not
                                 * need to be gone through.*/
    struct obj	*active_prev;	/**< Previous object in the 'active list
                                 * This is used in process_events
                                 * so that the entire object list does not
                                 * need to be gone through. */
    struct obj	*below;		/**< Pointer to the object stacked below this one */
    struct obj	*above;		/**< Pointer to the object stacked above this one */
                            /* Note: stacked in the *same* environment*/
    struct obj	*inv;		/**< Pointer to the first object in the inventory */
    struct obj	*container;	/**< Current container being used.  I think this
                              * is only used by the player right now. */
    struct obj	*env;		/**< Pointer to the object which is the environment.
                             * This is typically the container that the object is in. */
    struct obj	*more;		/**< Pointer to the rest of a large body of objects */
    struct obj	*head;		/**< Points to the main object of a large body */
    struct mapdef *map;		/**< Pointer to the map in which this object is present */

    tag_t	count;		/**< Unique object number for this object */

    /* These get an extra add_refcount(), after having been copied by memcpy().
     * All fields beow this point are automatically copied by memcpy.  If
     * adding something that needs a refcount updated, make sure you modify
     * copy_object to do so.  Everything below here also gets cleared
     * by clear_object()
     */
    const char	*name;		/**< The name of the object, obviously... */
    const char	*name_pl;	/**< The plural name of the object */
    const char	*title;		/**< Of foo, etc */
    const char	*race;		/**< Human, goblin, dragon, etc */
    const char	*slaying;	/**< Which race to do double damage to.
                              * If this is an exit, this is the filename */
    const char	*skill;		/**< Name of the skill this object uses/grants */
    const char	*msg;		/**< If this is a book/sign/magic mouth/etc */
    const char	*lore;		/**< Obscure information about this object,
                              * to get put into books and the like. */

    sint16	x,y;		/**< Position in the map for this object */
    sint16	ox,oy;		/**< For debugging: Where it was last inserted */
    float	speed;		/**< The overall speed of this object */
    float	speed_left;	/**< How much speed is left to spend this round */
    uint32	nrof;		/**< How many of the objects */
    New_Face	*face;		/**< Face with colors */
    sint8	direction;	/**< Means the object is moving that way. */
    sint8	facing;		/**< Object is oriented/facing that way. */

    /* This next big block are basically used for monsters and equipment */
    uint8	type;		/**< PLAYER, BULLET, etc.  See define.h */
    uint8	subtype;	/**< Subtype of object */
    uint16	client_type;	/**< Public type information.  see doc/Developers/objects */
    sint16	resist[NROFATTACKS];	/**< Resistance adjustments for attacks */
    uint32	attacktype;	/**< Bitmask of attacks this object does */
    uint32	path_attuned;	/**< Paths the object is attuned to */
    uint32	path_repelled;	/**< Paths the object is repelled from */
    uint32	path_denied; 	/**< Paths the object is denied access to */
    uint16	material;      	/**< What materials this object consist of */
    const char 	*materialname;  /**< Specific material name */
    sint8	magic;		/**< Any magical bonuses to this item */
    uint8	state;          /**< How the object was last drawn (animation) */
    sint32	value;		/**< How much money it is worth (or contains) */
    sint16	level;		/**< Level of creature or object */

    /* Note that the last_.. values are sometimes used for non obvious
     * meanings by some objects, eg, sp penalty, permanent exp.
     */
    sint32	last_heal;	/**< Last healed. Depends on constitution */
    sint32	last_sp;	/**< As last_heal, but for spell points */
    sint16	last_grace;	/**< As last_sp, except for grace */
    sint16	last_eat;	/**< How long since we last ate */
    sint16	invisible;	/**< How much longer the object will be invis */
    uint8	pick_up;        /**< See crossfire.doc */
    sint8	item_power;	/**< Power rating of the object */
    sint8	gen_sp_armour;	/**< Sp regen penalty this object has (was last_heal)*/
    sint32	weight;		/**< Attributes of the object */
    sint32	weight_limit;	/**< Weight-limit of object */
    sint32	carrying;	/**< How much weight this object contains */
    sint8	glow_radius;	/**< indicates the glow radius of the object */
    living	stats;		/**< Str, Con, Dex, etc */
    sint64	perm_exp;	/**< Permanent exp */
    struct obj	*current_weapon;   /**< Pointer to the weapon currently used */
    uint32	weapontype;	/**< Type of weapon */
    uint32	tooltype;	/**< Type of tool or build facility */
    sint8	body_info[NUM_BODY_LOCATIONS];	/**< Body info as loaded from the file */
    sint8	body_used[NUM_BODY_LOCATIONS];	/**< Calculated value based on items equipped */
				/* See the doc/Developers/objects for more info about body locations */

    /* Following mostly refers to fields only used for monsters */
    struct obj	*owner;		/**< Pointer to the object which controls this one.
                              * Owner should not be referred to directly -
                              * get_owner should be used instead. */
    tag_t	ownercount;	/**< What count the owner had (in case owner has been freed) */
    struct obj	*enemy;		/**< Monster/player to follow even if not closest */
    struct obj	*attacked_by;   /**< This object start to attack us! only player & monster */
    tag_t	attacked_by_count;         /**< The tag of attacker, so we can be sure */
    struct treasureliststruct *randomitems; /**< Items to be generated */
    uint16	run_away;	/**< Monster runs away if it's hp goes below this percentage. */
    struct obj	*chosen_skill;	/**< The skill chosen to use */
    uint32	hide;		/**< The object is hidden, not invisible */
    /* changes made by kholland@sunlab.cit.cornell.edu */
    /* allows different movement patterns for attackers */
    sint32	move_status;	/**< What stage in attack mode */
    uint16	attack_movement;/**< What kind of attack movement */
    uint8	will_apply;     /**< See crossfire.doc */
    struct obj	*spellitem;	/**< Spell ability monster is choosing to use */
    double	expmul;		/**< needed experience = (calc_exp*expmul) - means some *
                         * races/classes can need less/more exp to gain levels */

    /* Spell related information, may be useful elsewhere
     * Note that other fields are used - these files are basically
     * only used in spells.
     */
    sint16	duration;	/**< How long the spell lasts */
    uint8	duration_modifier; /**< how level modifies duration */
    sint16	casting_time;	/**< Time left before spell goes off */
    struct obj	*spell;		/**< Spell that was being cast */
    uint16	start_holding;
    char	*spellarg;
    uint8	dam_modifier;	/**< How going up in level effects damage */
    sint8	range;		/**< Range of the spell */
    uint8	range_modifier;	/**< How going up in level effects range  */

    /* Following are values used by any object */
    struct archt *arch;         /**< Pointer to archetype */
    struct archt *other_arch;	/**< Pointer used for various things - mostly used for what
                                 * this objects turns into or what this object creates */
    uint32	flags[4];	/**< Various flags */
    uint16	animation_id;	/**< An index into the animation array */
    uint8	anim_speed;	/**< Ticks between animation-frames */
    uint8	last_anim;	/**< Last sequence used to draw face */
    sint32	elevation;	/**< Elevation of this terrain - used in weather code */
    uint8	smoothlevel;    /**< how to smooth this square around*/
    uint8	map_layer;	/**< What level to draw this on the map */

    MoveType   move_type;	/**< Type of movement this object uses */
    MoveType   move_block;	/**< What movement types this blocks */
    MoveType   move_allow;	/**< What movement types explicitly allowed */
    MoveType   move_on;		/**< Move types affected moving on to this space */
    MoveType   move_off;	/**< Move types affected moving off this space */
    MoveType   move_slow;	/**< Movement types this slows down */
    float   move_slow_penalty;	/**< How much this slows down the object */

    const char  *custom_name; /**< Custom name assigned by player */
    key_value *key_values; /**< Fields not explictly known by the loader. */

    uint8       no_save;       /**< This field indicates that the object should never
                                * be saved even for map swapout. Not handled by the
                                * loading or saving code. */
    sint16* discrete_damage; /**< damage values, based on each attacktype. */
} object;

/**
 * Used to link together several objects
 */
typedef struct oblnk {
  object *ob;
  struct oblnk *next;
  tag_t id;
} objectlink;

/**
 * Used to link together several object links
 */
typedef struct oblinkpt {
  struct oblnk *link;
  long value;		/**< Used as connected value in buttons/gates */
  struct oblinkpt *next;
} oblinkpt;

/**
 * The archetype structure is a set of rules on how to generate and manipulate
 * objects which point to archetypes.
 * This probably belongs in arch.h, but there really doesn't appear to
 * be much left in the archetype - all it really is is a holder for the
 * object and pointers.  This structure should get removed, and just replaced
 * by the object structure
 */

typedef struct archt {
    const char *name;		    /**< More definite name, like "generate_kobold" */
    struct archt *next;	    /**< Next archetype in a linked list */
    struct archt *head;	    /**< The main part of a linked object */
    struct archt *more;	    /**< Next part of a linked object */
    object clone;	    /**< An object from which to do copy_object() */
    uint32 editable;	    /**< Editable flags (mainly for editor) */
    sint8   tail_x, tail_y; /**< Where the lower right most portion of the object is
                             * in comparison to the head. */
    int reference_count; /**< How many times this temporary archetype is used. If 0, "permanent" archetype. */
} archetype;

extern object *objects;
extern object *active_objects;
extern object *free_objects;
extern object objarray[STARTMAX];

extern int nrofallocobjects;
extern int nroffreeobjects;

/**
 * This returns TRUE if the object is somethign that
 * should be displayed in the look window
 */
#define LOOK_OBJ(ob) (!ob->invisible && ob->type!=PLAYER && ob->type!=EVENT_CONNECTOR)

/* Used by update_object to know if the object being passed is
 * being added or removed.
 */
#define UP_OBJ_INSERT   1
#define UP_OBJ_REMOVE   2
#define UP_OBJ_CHANGE   3
#define UP_OBJ_FACE     4   /* Only thing that changed was the face */

/* These are flags passed to insert_ob_in_map and 
 * insert_ob_in_ob.  Note that all flags may not be meaningful
 * for both functions.
 * Most are fairly explanatory:
 * INS_NO_MERGE: don't try to merge inserted object with ones alrady
 *    on space.
 * INS_ABOVE_FLOOR_ONLY: Put object immediatly above the floor.
 * INS_NO_WALK_ON: Don't call check_walk_on against the
 *    originator - saves cpu time if you know the inserted object
 *    is not meaningful in terms of having an effect.
 * INS_ON_TOP: Always put object on top.  Generally only needed when loading
 *     files from disk and ordering needs to be preserved. 
 * INS_BELOW_ORIGINATOR: Insert new object immediately below originator -
 *     Use for treasure chests so the new object is the highest thing
 *     beneath the player, but not actually above it.  Note - the
 *     map and x,y coordinates for the object to be inserted must
 *     match the originator.
 * INS_MAP_LOAD: disable lots of checkings done at insertion to
 *     speed up map loading process, as we assume the ordering in
 *     loaded map is correct.
 *
 * Note that INS_BELOW_ORIGINATOR, INS_ON_TOP, INS_ABOVE_FLOOR_ONLY
 * are mutually exclusive.  The behaviour for passing more than one
 * should be considered undefined - while you may notice what happens
 * right now if you pass more than one, that could very well change
 * in future revisions of the code.
 */
#define INS_NO_MERGE		0x0001
#define INS_ABOVE_FLOOR_ONLY	0x0002
#define INS_NO_WALK_ON		0x0004
#define INS_ON_TOP		0x0008
#define INS_BELOW_ORIGINATOR	0x0010
#define INS_MAP_LOAD		0x0020

#define ARCH_SINGULARITY        "singularity"
#define ARCH_SINGULARITY_LEN    11
#define ARCH_DETECT_MAGIC       "detect_magic"
#define ARCH_DEPLETION          "depletion"
#define ARCH_SYMPTOM            "symptom"

#endif /* OBJECT_H */
