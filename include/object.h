/*
 * static char *rcsid_object_h =
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

    The author can be reached via e-mail to mark@pyramid.com.
*/

#ifndef OBJECT_H
#define OBJECT_H

/* Only add new values to this list if somewhere in the program code,
 * it is actually needed.  Just because you add a new monster does not
 * mean it has to have a type defined here.  That only needs to happen
 * if in some .c file, it needs to do certain special actions based on
 * the monster type, that can not be handled by any of the numerous
 * flags
 * Also, if you add new entries, try and fill up the holes in this list.
 */
#define PLAYER		1
#define BULLET		2
#define ROD		3
#define TREASURE	4
#define POTION		5
#define FOOD		6
#define POISON		7
#define BOOK		8
#define CLOCK		9
#define FBULLET		10
#define FBALL		11
#define LIGHTNING	12
#define ARROW		13
#define BOW		14
#define WEAPON		15
#define ARMOUR		16
#define PEDESTAL	17
#define ALTAR		18
#define CONFUSION	19
#define LOCKED_DOOR	20
#define SPECIAL_KEY	21
#define MAP		22
#define DOOR		23
#define KEY		24
#define MMISSILE	25
#define TIMED_GATE	26
#define TRIGGER		27
#define GRIMREAPER	28
#define MAGIC_EAR	29
#define TRIGGER_BUTTON	30
#define TRIGGER_ALTAR	31
#define TRIGGER_PEDESTAL 32
#define SHIELD		33
#define HELMET		34
#define HORN		35
#define MONEY		36
#define GRAVE		37
#define GRAVESTONE	38
#define AMULET		39
#define PLAYERMOVER	40
#define TELEPORTER	41
#define CREATOR		42
#define SKILL		43	/* Skills are similar to abilites, but
				 * not related to spells.  by njw@cs.city.ac.uk
				 */
#define EXPERIENCE	44	/* An experience 'object'. Needed for multi-exp/skills
				 * hack. -b.t. thomas@astro.psu.edu
			         */
#define EARTHWALL	45
#define GOLEM		46
#define BOMB		47
#define THROWN_OBJ	48
#define BLINDNESS	49
#define GOD		50

/*  peterm:  detector is an object which notices the presense of
	 another object and is triggered like buttons.  */
#define DETECTOR	51
#define SPEEDBALL	52
#define DEAD_OBJECT	53
#define DRINK		54
#define MARKER          55 /* inserts an invisible, weightless
			      force into a player with a specified string. */
#define LIQUID		56	/* potion poured out of its bottle -- DAMN */
#define BOTTLE		57	/* bottle drained of its potion -- DAMN */

#define GEM		60 
#define FIRECHEST	61 
#define FIREWALL	62

#define CHECK_INV	64	/* by b.t. thomas@nomad.astro.psu.edu */
#define MOOD_FLOOR	65	/* by b.t. thomas@nomad.astro.psu.edu 
				 * values of last_sp set how to change:
				 * 0 = furious,	all monsters become aggressive
				 * 1 = angry, all but friendly become aggressive
				 * 2 = calm, all aggressive monsters calm down 
				 * 3 = sleep, all monsters fall asleep 
				 * 4 = charm, monsters become pets
				 */
#define EXIT		66
#define ENCOUNTER	67
#define SHOP_FLOOR	68
#define SHOP_MAT	69
#define RING		70

#define FLESH		72	/* animal 'body parts' -b.t. */
#define INORGANIC	73	/* metals and minerals */ 

#define LIGHTER		75
#define TRAP_PART	76	/* Needed by set traps skill -b.t. */ 

#define SPELLBOOK	85

#define CLOAK		87
#define CONE		88

#define SPINNER		90
#define GATE		91
#define BUTTON		92
#define HANDLE		93
#define HOLE		94
#define TRAPDOOR	95
#define WORD_OF_RECALL	96
#define PARAIMAGE	97
#define SIGN		98
#define BOOTS		99
#define GLOVES		100

#define CONVERTER	103
#define BRACERS		104
#define POISONING	105
#define SAVEBED		106
#define POISONCLOUD	107
#define FIREHOLES	108
#define WAND		109
#define ABILITY		110
#define SCROLL		111
#define DIRECTOR	112
#define GIRDLE		113
#define FORCE		114


/* Eneq(@csd.uu.se): Id for close_container archetype. */
#define CLOSE_CON	121
#define CONTAINER	122
#define ARMOUR_IMPROVER 123
#define WEAPON_IMPROVER 124
#define MONEY_CHANGER	125

#define SKILLSCROLL	130	/* can add a skill to player's inventory -bt.*/

#define DEEP_SWAMP	138
#define IDENTIFY_ALTAR	139

#define CANCELLATION	141

/* Mark Wedel (mark@pyramid.com) Shop inventories */
#define MENU		150

/* peterm:  ball lightning and color spray */
#define BALL_LIGHTNING	151

#define SWARM_SPELL	153
#define RUNE		154

#define POWER_CRYSTAL	156
#define CORPSE		157

#define DISEASE		158
#define SYMPTOM		159


/* Flag structure now changed.
 * Each flag is now a bit offset, starting at zero.  The macros
 * will update/read the appropriate flag element in the object
 * structure.
 *
 * Hopefully, since these offsets are integer constants set at run time,
 * the compiler will reduce the macros something as simple as the 
 * old system was.
 *
 * Flags now have FLAG as the prefix.  This to be clearer, and also
 * to make sure F_ names are not still being used anyplace.
 *
 * The macros below assume that the flag size for each element is 32
 * bits.  IF it is smaller, bad things will happen.  See structs.h
 * for more info.
 *
 * All functions should use the macros below.  In process of converting
 * to the new system, I find several files that did not use the previous
 * macros.
 * 
 * If any FLAG's are added, be sure to add them to the flag_links structure
 * in common/loader.c, if necessary.
 *
 * flags[0] is 0 to 31
 * flags[1] is 32 to 63
 * flags[2] is 64 to 95
 * flags[3] is 96 to 127
 */
/* Basic routines to do above */
#define SET_FLAG(xyz, p) \
	((xyz)->flags[p/32] |= (1U << (p % 32)))
#define CLEAR_FLAG(xyz, p) \
	((xyz)->flags[p/32] &= ~(1U << (p % 32)))
#define QUERY_FLAG(xyz, p) \
	((xyz)->flags[p/32] & (1U << (p % 32)))

#define FLAG_ALIVE	 	0 /* Object can fight (or be fought) */
#define FLAG_WIZ	 	1 /* Object has special privilegies */
#define FLAG_REMOVED	 	2 /* Object is not in any map or invenory */
#define FLAG_FREED	 	3 /* Object is in the list of free objects */
#define FLAG_WAS_WIZ	 	4 /* Player was once a wiz */
#define FLAG_APPLIED	 	5 /* Object is ready for use by living */
#define FLAG_UNPAID	 	6 /* Object hasn't been paid for yet */
#define FLAG_AN		 	7 /* Name must be prepended by "an", not "a"*/
#define FLAG_NO_PICK	 	8 /* Object can't be picked up */
#define FLAG_WALK_ON	 	9 /* Applied when it's walked upon */
#define FLAG_NO_PASS		10 /* Nothing can pass (wall() is true) */
#define FLAG_ANIMATE		11 /* The object looks at archetype for faces */
#define FLAG_SLOW_MOVE		12 /* Uses the stats.exp/1000 to slow down */
#define FLAG_FLYING		13 /* Not affected by WALK_ON or SLOW_MOVE) */
#define FLAG_MONSTER		14 /* Will attack players */
#define FLAG_FRIENDLY		15 /* Will help players */
#define FLAG_GENERATOR		16 /* Will generate type ob->stats.food */
#define FLAG_IS_THROWN		17 /* Object is designed to be thrown. */
#define FLAG_AUTO_APPLY		18 /* Will be applied when created */
#define FLAG_TREASURE		19 /* Will generate treasure when applied */
#define FLAG_APPLY_ONCE		20 /* Will dissapear when applied */
#define FLAG_SEE_INVISIBLE 	21 /* Will see invisible player */
#define FLAG_CAN_ROLL		22 /* Object can be rolled */
/* FLAG_IS_TURNING is no longer used */
/*#define FLAG_IS_TURNING		23 *//* Object will turn after player */
#define FLAG_IS_TURNABLE 	24 /* Object can change face with direction */
#define FLAG_WALK_OFF		25 /* Object is applied when left */
#define FLAG_FLY_ON		26 /* As WALK_ON, but only with FLAG_FLYING */
#define FLAG_FLY_OFF		27 /* As WALK_OFF, but only with FLAG_FLYING */
#define FLAG_IS_USED_UP		28 /* When (--food<0) the object will exit */
#define FLAG_IDENTIFIED		29 /* Not implemented yet */
#define FLAG_REFLECTING		30 /* Object reflects from walls (lightning) */
#define FLAG_CHANGING		31 /* Changes to other_arch when anim is done*/
/* Start of values in flags[1] */
#define FLAG_SPLITTING		32 /* Object splits into stats.food other objs */
#define FLAG_HITBACK		33 /* Object will hit back when hit */
#define FLAG_STARTEQUIP		34 /* Object was given to player at start */
#define FLAG_BLOCKSVIEW		35 /* Object blocks view */
#define FLAG_UNDEAD		36 /* Monster is undead */
#define FLAG_SCARED		37 /* Monster is scared (mb player in future)*/
#define FLAG_UNAGGRESSIVE	38 /* Monster doesn't attack players */
#define FLAG_REFL_MISSILE	39 /* Arrows will reflect from object */
#define FLAG_REFL_SPELL		40 /* Spells (some) will reflect from object */
#define FLAG_NO_MAGIC		41 /* Spells (some) can't pass this object */
#define FLAG_NO_FIX_PLAYER	42 /* fix_player() won't be called */
#define FLAG_NEED_IE		43 /* 1 ruby -> 2 rubies */
#define FLAG_TEAR_DOWN		44 /* at->faces[hp*animations/maxhp] at hit */
#define FLAG_RUN_AWAY		45 /* Object runs away from nearest player \
				      but can still attack at a distance */
#define FLAG_PASS_THRU		46 /* Objects with can_pass_thru can pass \
				      thru this object as if it wasn't there */
#define FLAG_CAN_PASS_THRU	47 /* Can pass thru... */
#define FLAG_PICK_UP		48 /* Can pick up */
#define FLAG_UNIQUE		49 /* Item is really unique (UNIQUE_ITEMS) */
#define FLAG_NO_DROP		50 /* Object can't be dropped */
#define FLAG_NO_PRETEXT		51 /* No text is added before name. */
#define FLAG_CAST_SPELL		52 /* (Monster) can learn and cast spells */
#define FLAG_USE_SCROLL		53 /* (Monster) can read scroll */
#define FLAG_USE_WAND		54 /* (Monster) can apply and use wands */
#define FLAG_USE_BOW		55 /* (Monster) can apply and fire bows */
#define FLAG_USE_ARMOUR		56 /* (Monster) can wear armour/shield/helmet */
#define FLAG_USE_WEAPON		57 /* (Monster) can wield weapons */
#define FLAG_USE_RING		58 /* (Monster) can use rings, boots, gauntlets, etc */
#define FLAG_READY_WAND		59 /* (Monster) has a wand readied... 8) */
#define FLAG_READY_BOW		60 /* not implemented yet */
#define FLAG_XRAYS		61 /* X-ray vision */
#define FLAG_NO_APPLY		62 /* Avoids step_on/fly_on to this object */
#define FLAG_IS_FLOOR		63 /* Can't see what's underneath this object */
#define FLAG_LIFESAVE		64 /* Saves a players' life once, then destr. */
/* Start of values in flags[2] */
#define FLAG_NO_STRENGTH	65 /* Strength-bonus not added to wc/dam */
#define FLAG_SLEEP		66 /* NPC is sleeping */
#define FLAG_STAND_STILL	67 /* NPC will not (ever) move */
#define FLAG_RANDOM_MOVE	68 /* NPC will move randomly */
#define FLAG_ONLY_ATTACK	69 /* NPC will evaporate if there is no enemy */
#define FLAG_CONFUSED		70 /* Will also be unable to cast spells */
#define FLAG_STEALTH		71 /* Will wake monsters with less range */
#define FLAG_WIZPASS		72 /* The wizard can go through walls */
#define FLAG_IS_LINKED		73 /* The object is linked with other objects */
#define FLAG_CURSED		74 /* The object is cursed */
#define FLAG_DAMNED		75 /* The object is _very_ cursed */
#define FLAG_SEE_ANYWHERE	76 /* The object will be visible behind walls */
#define FLAG_KNOWN_MAGICAL	77 /* The object is known to be magical */
#define FLAG_KNOWN_CURSED	78 /* The object is known to be cursed */
#define FLAG_CAN_USE_SKILL	79 /* The monster can use skills */
#define FLAG_BEEN_APPLIED	80 /* The object has been applied */
#define FLAG_READY_ROD		81 /* (Monster) has a rod readied... 8) */
#define FLAG_USE_ROD		82 /* (Monster) can apply and use rods */
#define FLAG_READY_HORN		83 /* (Monster) has a horn readied */
#define FLAG_USE_HORN		84 /* (Monster) can apply and use horns */
#define FLAG_MAKE_INVIS	        85 /* (Item) gives invisibility when applied */
#define FLAG_INV_LOCKED		86 /* Item will not be dropped from inventory */
#define FLAG_IS_WOODED		87 /* Item is wooded terrain */
#define FLAG_IS_HILLY		88 /* Item is hilly/mountain terrain */
#define FLAG_READY_SKILL	89 /* (Monster or Player) has a skill readied */
#define FLAG_READY_WEAPON	90 /* (Monster or Player) has a weapon readied */
#define FLAG_NO_SKILL_IDENT	91 /* If set, item cannot be identified w/ a skill */
#define FLAG_BLIND		92 /* If set, object cannot see (visually) */
#define FLAG_SEE_IN_DARK	93 /* if set ob not effected by darkness */ 
#define FLAG_IS_CAULDRON	94 /* container can make alchemical stuff */
#define FLAG_DUST		95 /* item is a 'powder', effects throwing */
/* Start of values in flags[3] */
#define FLAG_NO_STEAL		96 /* Item can't be stolen */
#define FLAG_ONE_HIT		97 /* Monster can only hit once before going
				    * away (replaces ghosthit)
				    */
#define FLAG_CLIENT_SENT	98 /* THIS IS A DEBUG FLAG ONLY.  We use it to
				    * detect cases were the server is trying
				    * to send an upditem when we have not
				    * actually sent the item.
				    */
#define NUM_FLAGS		98 /* Should always be equal to the last
				    * defined flag */

#define ARMOUR_SPEED(xyz)	(xyz)->last_sp
#define ARMOUR_SPELLS(xyz)	(xyz)->last_heal
#define WEAPON_SPEED(xyz)	(xyz)->last_sp

/* Values can go up to 127 before the size of the flags array in the
 * object structure needs to be enlarged.
 */

typedef struct obj {
/* These variables are not changed by copy_object(): */
  struct pl *contr;    /* Pointer to the player which control this object */
  struct obj *next;    /* Pointer to the next object in the free/used list */
  struct obj *prev;    /* Pointer to the previous object in the free/used list*/
  struct obj *active_next;	/* Next & previous object in the 'active' */
  struct obj *active_prev;	/* List.  This is used in process_events */
				/* so that the entire object list does not */
				/* need to be gone through. */
  struct obj *below;   /* Pointer to the object stacked below this one */
  struct obj *above;   /* Pointer to the object stacked above this one */
	         /* Note: stacked in the *same* environment*/
  struct obj *inv;     /* Pointer to the first object in the inventory */
  struct obj *container; /* Current container being used.  I think this
			  * is only used by the player right now.
			  */
  struct obj *env;     /* Pointer to the object which is the environment.
			* This is typically the container that the object is in.
			*/
  struct obj *more;    /* Pointer to the rest of a large body of objects */
  struct obj *head;    /* Points to the main object of a large body */
  struct mapdef *map;  /* Pointer to the map in which this object is present */

  uint32	count;         /* Which nr. of object created this is. */
  uint16	refcount;	/* How many objects points to this object */
  struct oblnk *sk_list;	/* Pointer to linked list of skill objects for this ob*/

/* These get an extra add_refcount(), after having been copied by memcpy() */
  char *name;			/* The name of the object, obviously... */
  char *title;			/* Of foo, etc */
  char *race;			/* human, goblin, dragon, etc */
  char *slaying;		/* Which race to do double damage to */
  char *msg;			/* If this is a book/sign/magic mouth/etc */
				/* If this is an exit, this is the filename */
/* These variables are copied by memcpy() in copy_object(): */
  sint16 x,y;			/* Position in the map for this object */
  sint16 ox,oy;			/* For debugging: Where it was last inserted */
  float speed;                  /* The overall speed of this object */
  float speed_left;             /* How much speed is left to spend this round */
  uint32	nrof;		/* How many of the objects */
  New_Face *face;		/* Face with colors */
  sint8		direction;	/* Means the object is moving that way. */
  sint8 facing;			/* Object is oriented/facing that way. */
  uint8 type; 		        /* PLAYER, BULLET, etc.  See define.h */
  /* The next 6 variables actually only really use 24 bits each */
  uint32	immune;		/* Attacks which the object is immune against */
  uint32	protected;	/* Attacks which does half damage */
  uint32	attacktype;	/* Same bitmask as immune/protected */
  uint32	vulnerable;	/* Attacks which does double damage */
  uint32	path_attuned;	/* Paths the object is attuned to */
  uint32	path_repelled;	/* Paths the object is repelled from */
  uint32	path_denied; 	/* Paths the object is denied access to */
  uint16	material;      	/* What materials this object consist of */
  sint8		magic;		/* Any magical bonuses to this item */
  sint8		thrownthaco;	/* How precise the throw is */
  uint8		state;          /* How the object was last drawn (animation) */
  sint32	value;		/* How much money it is worth (or contains) */
  signed short level;
  sint32 last_heal;       /* Last healed. Depends on constitution */
  sint32 last_sp;         /* As last_heal, but for spell points */
  signed short last_grace;      /* as last_sp, except for grace */
  signed short last_eat;	/* How long since we last ate */
  signed short invisible;	/* How much longer the object will be invis */
  sint8		armour;		/* How many % is subtracted from phys. damage */
  unsigned char pick_up;        /* See crossfire.doc */
  struct obj *owner;  /* Pointer to the object which controls this one */
                      /* Owner should not be referred to directly - */
                      /* get_owner should be used instead. */
  struct obj *enemy;   /* Monster/player to follow even if not closest */
  struct archt *arch;           /* Pointer to archetype */
  struct archt *other_arch;	/* Pointer used for various things */
  signed long weight;           /* Attributes of the object */
  signed long carrying;         /* How much weight this object contains */
  uint32 flags[4];		/* various flags */
  uint32 ownercount;		/* What count the owner had (in case owner */
				/* has been freed) */
  struct treasureliststruct *randomitems; /* Items to be generated */
#if 0
  unsigned short thrown;        /* How much further the object will fly */
#endif
/* Some debug variables: */
  unsigned short run_away;	/* Monster runs away if it's hp goes below \
				   this percentage. */
 
/* Not commented out by ALLOW_SKILLS - to many pieces of code want this
 * information
 */
  struct obj *chosen_skill;	/* the skill chosen to use */
  struct obj *exp_obj;		/* the exp. obj (category) assoc. w/ this object */
  uint32 hide;			/* The object is hidden, not invisible */

/* lighting code uses these - b.t. */
  struct oblnk *lights;   	/* list of lights the object carries */
  signed short glow_radius;	/* indicates the glow radius of the object */

/* changes made by kholland@sunlab.cit.cornell.edu */
/* allows different movement patterns for attackers */
  signed long move_status;      /* What stage in attack mode */
  unsigned short move_type;     /* What kind of attack movement */

  signed long weight_limit;     /* Weight-limit of object */
  unsigned char can_apply;      /* See crossfire.doc */
  unsigned char will_apply;     /* See crossfire.doc */
    uint16 animation_id;    /* An index into the animation array */
    uint8 anim_speed, last_anim; /* ticks between animation-frames */
  living stats;

#ifdef CASTING_TIME
  signed short casting;         /* time left before spell goes off */
  unsigned short spell_state;
  unsigned short start_holding;
  struct spell_struct *spell;
  unsigned int spelltype;
  char *spellarg;
#endif

  struct obj *spellitem;

/* This modification will allow variable experience requirements 
   for advancement (eanders@cmu.edu) */
  double expmul;        /* needed experience = (calc_exp*expmul) */
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
    unsigned long editable; /* editable flags (mainly for editor) */
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

#endif
