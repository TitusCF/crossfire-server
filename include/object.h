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

/* See common/item.c */

typedef struct Body_Locations {
    char    *save_name;		/* Name used to load/save it to disk */
    char    *use_name;		/* Name used when describing an item we can use */
    char    *nonuse_name;	/* Name to describe objects we can't use */
} Body_Locations;

extern Body_Locations body_locations[NUM_BODY_LOCATIONS];

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


/* Note that the ordering of this structure is sort of relevent -
 * copy_object copies everything over beyond 'name' using memcpy.
 * Thus, values that need to be copied need to be located beyond that
 * point.
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
    struct pl	*contr;		/* Pointer to the player which control this object */
    struct obj	*next;		/* Pointer to the next object in the free/used list */
    struct obj	*prev;		/* Pointer to the previous object in the free/used list*/
    struct obj	*active_next;	/* Next & previous object in the 'active' */
    struct obj	*active_prev;	/* List.  This is used in process_events */
				/* so that the entire object list does not */
				/* need to be gone through. */
    struct obj	*below;		/* Pointer to the object stacked below this one */
    struct obj	*above;		/* Pointer to the object stacked above this one */
				/* Note: stacked in the *same* environment*/
    struct obj	*inv;		/* Pointer to the first object in the inventory */
    struct obj	*container;	/* Current container being used.  I think this
				 * is only used by the player right now.
				 */
    struct obj	*env;		/* Pointer to the object which is the environment.
				 * This is typically the container that the object is in.
				 */
    struct obj	*more;		/* Pointer to the rest of a large body of objects */
    struct obj	*head;		/* Points to the main object of a large body */
    struct mapdef *map;		/* Pointer to the map in which this object is present */

    tag_t	count;		/* Unique object number for this object */
    uint16	refcount;	/* How many objects points to this object */

    /* These get an extra add_refcount(), after having been copied by memcpy().
     * All fields beow this point are automatically copied by memcpy.  If
     * adding something that needs a refcount updated, make sure you modify
     * copy_object to do so.  Everything below here also gets cleared
     * by clear_object()
     */
    char	*name;		/* The name of the object, obviously... */
    char	*name_pl;	/* The plural name of the object */
    char	*title;		/* Of foo, etc */
    char	*race;		/* human, goblin, dragon, etc */
    char	*slaying;	/* Which race to do double damage to */
				/* If this is an exit, this is the filename */
    char	*msg;		/* If this is a book/sign/magic mouth/etc */

    sint16	x,y;		/* Position in the map for this object */
    sint16	ox,oy;		/* For debugging: Where it was last inserted */
    float	speed;		/* The overall speed of this object */
    float	speed_left;	/* How much speed is left to spend this round */
    float	casting_speed;	/* casting speed - used to fine tune monster casting */
    uint32	nrof;		/* How many of the objects */
    New_Face	*face;		/* Face with colors */
    sint8	direction;	/* Means the object is moving that way. */
    sint8	facing;		/* Object is oriented/facing that way. */

    /* This next big block are basically used for monsters and equipment */
    uint8	type;		/* PLAYER, BULLET, etc.  See define.h */
    uint16	client_type;	/* Public type information.  see doc/Developers/objects */
    sint16	resist[NROFATTACKS];	/* Resistance adjustments for attacks */
    uint32	attacktype;	/* Bitmask of attacks this object does */
    uint32	path_attuned;	/* Paths the object is attuned to */
    uint32	path_repelled;	/* Paths the object is repelled from */
    uint32	path_denied; 	/* Paths the object is denied access to */
    uint16	material;      	/* What materials this object consist of */
	char 	*materialname;  /* specific material name */
    sint8	magic;		/* Any magical bonuses to this item */
    sint8	thrownthaco;	/* How precise the throw is */
    uint8	state;          /* How the object was last drawn (animation) */
    sint32	value;		/* How much money it is worth (or contains) */
    sint16	level;		/* Level of creature or object */
    /* Note that the last_.. values are sometimes used for non obvious
     * meanings by some objects, eg, sp penalty, permanent exp.
     */
    sint32	last_heal;	/* Last healed. Depends on constitution */
    sint32	last_sp;	/* As last_heal, but for spell points */
    sint16	last_grace;	/* as last_sp, except for grace */
    sint16	last_eat;	/* How long since we last ate */
    sint16	invisible;	/* How much longer the object will be invis */
    uint8	pick_up;        /* See crossfire.doc */
    sint8	item_power;	/* power rating of the object */
    sint8	gen_sp_armour;	/* sp regen penalty this object has (was last_heal)*/
    sint32	weight;		/* Attributes of the object */
    sint32	weight_limit;	/* Weight-limit of object */
    sint32	carrying;	/* How much weight this object contains */
    sint16	glow_radius;	/* indicates the glow radius of the object */
    living	stats;		/* Str, Con, Dex, etc */
    char	*current_weapon_script;  /* The script of the currently used weapon. Executed */
				/* each time the object attacks something */
    struct obj	*current_weapon;   /* Pointer to the weapon currently used */
    uint32	weapontype;	/* type of weapon */
    sint8	body_info[NUM_BODY_LOCATIONS];	/* body info as loaded from the file */
    sint8	body_used[NUM_BODY_LOCATIONS];	/* Calculated value based on items equipped */
				/* See the doc/Developers/objects for more info about body locations */

    /* Following mostly refers to fields only used for monsters */
    struct obj	*owner;		/* Pointer to the object which controls this one */
				/* Owner should not be referred to directly - */
				/* get_owner should be used instead. */
    tag_t	ownercount;	/* What count the owner had (in case owner */
				/* has been freed) */
    struct obj	*enemy;		/* Monster/player to follow even if not closest */
    struct obj	*attacked_by;   /* This object start to attack us! only player & monster */
    tag_t	attacked_by_count;         /* the tag of attacker, so we can be sure */
    struct treasureliststruct *randomitems; /* Items to be generated */
    uint16	run_away;	/* Monster runs away if it's hp goes below this percentage. */
    struct obj	*chosen_skill;	/* the skill chosen to use */
    struct obj	*exp_obj;	/* the exp. obj (category) assoc. w/ this object */
    uint32	hide;		/* The object is hidden, not invisible */
    /* changes made by kholland@sunlab.cit.cornell.edu */
    /* allows different movement patterns for attackers */
    sint32	move_status;	/* What stage in attack mode */
    uint16	move_type;	/* What kind of attack movement */
    uint8	will_apply;     /* See crossfire.doc */
    struct obj	*spellitem;	/* Spell ability monster is choosing to use */
    double	expmul;		/* needed experience = (calc_exp*expmul) - means some */
				/* races/classes can need less/more exp to gain levels */
    sint16	casting;         /* time left before spell goes off */
    uint16	spell_state;
    uint16	start_holding;
    struct spell_struct *spell;
    uint32	spelltype;
    char	*spellarg;

    /* Following are values used by any object */
    struct archt *arch;         /* Pointer to archetype */
    struct archt *other_arch;	/* Pointer used for various things - mostly used for what */
				/* this objects turns into or what this object creates */
    uint32	flags[4];	/* various flags */
    uint16	animation_id;	/* An index into the animation array */
    uint8	anim_speed;	/* ticks between animation-frames */
    uint8	last_anim;	/* last sequence used to draw face */
    sint32  elevation;		/* elevation of this terrain - not currently used */

    char	*event_hook[30];    /* GROS - extensions for scripting events */
    char	*event_plugin[30];
    char	*event_options[30];

} object;

typedef struct oblnk { /* Used to link together several objects */
  object *ob;
  struct oblnk *next;
  int id;
} objectlink;

typedef struct oblinkpt { /* Used to link together several object links */
  struct oblnk *link;
  long value;		/* Used as connected value in buttons/gates */
  struct oblinkpt *next;
} oblinkpt;

/*
 * The archetype structure is a set of rules on how to generate and manipulate
 * objects which point to archetypes.
 * This probably belongs in arch.h, but there really doesn't appear to
 * be much left in the archetype - all it really is is a holder for the
 * object and pointers.  This structure should get removed, and just replaced
 * by the object structure
 */

typedef struct archt {
    char *name;		    /* More definite name, like "generate_kobold" */
    struct archt *next;	    /* Next archetype in a linked list */
    struct archt *head;	    /* The main part of a linked object */
    struct archt *more;	    /* Next part of a linked object */
    object clone;	    /* An object from which to do copy_object() */
    uint32 editable;	    /* editable flags (mainly for editor) */
    sint8   tail_x, tail_y; /* Where the lower right most portion of the object is
			     * in comparison to the head.
			     */
} archetype;

extern object *objects;
extern object *active_objects;
extern object *free_objects;
extern object objarray[STARTMAX];

extern int nrofallocobjects;
extern int nroffreeobjects;

/* This returns TRUE if the object is somethign that
 * should be displayed in the look window
 */
#define LOOK_OBJ(ob) (!ob->invisible && ob->type!=PLAYER)

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

#endif
