/*
 * static char *rcsid_define_h =
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

/* This file is really too large.  With all the .h files
 * around, this file should be better split between them - things
 * that deal with objects should be in objects.h, things dealing
 * with players in player.h, etc.  As it is, everything just seems
 * to be dumped in here.
 */

#ifndef DEFINE_H
#define DEFINE_H

/*
 * Crossfire requires ANSI-C, but some compilers "forget" to define it.
 * Thus the prototypes made by cextract don't get included correctly.
 */
#if !defined(__STDC__)
/* Removed # from start of following line.  makedepend was picking it up.
 * The following should still hopefully result in an error.
 */
error - Your ANSI C compiler should be defining __STDC__;
#endif

#ifndef WIN32 /* ---win32 exclude unix configuration part */
#include <autoconf.h>
#endif

#define FONTDIR ""
#define FONTNAME ""

/* Decstations have trouble with fabs()... */
#define FABS(x) (x<0?-x:x)

#ifdef __NetBSD__
#include <sys/param.h>
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#define MAX_STAT		30	/* The maximum legal value of any stat */
#define MIN_STAT		1	/* The minimum legal value of any stat */

#define MAX_BUF			256	/* Used for all kinds of things */
#define VERY_BIG_BUF		1024
#define HUGE_BUF		4096 /* Used for messages - some can be quite long */
#define SOCKET_BUFLEN		4096	/* Max length a packet could be */

#define FONTSIZE		3000	/* Max chars in font */

#define MAX_ANIMATIONS		64

/* Fatal variables: */
#define OUT_OF_MEMORY		0
#define MAP_ERROR		1
#define ARCHTABLE_TOO_SMALL	2
#define TOO_MANY_ERRORS		3

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

#define CLASS 37  /* object for applying character class modifications to someone */



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
#define HOLY_ALTAR	56
#define PLAYER_CHANGER  57
#define BATTLEGROUND    58      /* battleground, by Andreas Vogl */

#define PEACEMAKER       59  /* Object owned by a player which can convert
			       a monster into a peaceful being incapable of
			       attack.  */
#define GEM		60
#define FIRECHEST	61
#define FIREWALL	62
#define ANVIL       63
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
#define AURA            89  /* aura spell object */

#define SPINNER		90
#define GATE		91
#define BUTTON		92
#define CF_HANDLE		93
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
#define POTION_EFFECT   115    /* a force, holding the effect of a potion */

/* Eneq(@csd.uu.se): Id for close_container archetype. */
#define CLOSE_CON	121
#define CONTAINER	122
#define ARMOUR_IMPROVER 123
#define WEAPON_IMPROVER 124

/* unused: 125 - 129
 * type 125 was MONEY_CHANGER
 */

#define SKILLSCROLL	130	/* can add a skill to player's inventory -bt.*/

#define DEEP_SWAMP	138
#define IDENTIFY_ALTAR	139

#define CANCELLATION	141

/* Mark Wedel (mark@pyramid.com) Shop inventories */
#define MENU		150

/* peterm:  ball lightning and color spray */
#define BALL_LIGHTNING 151

#define SWARM_SPELL 153
#define RUNE 154

#define POWER_CRYSTAL 156
#define CORPSE 157

#define DISEASE 158
#define SYMPTOM 159

#define MAX_NAME 16
#define BIG_NAME 32
#define MAX_EXT_TITLE 98

/* definitions for weapontypes */

#define WEAP_HIT	0  /* the basic */
#define WEAP_SLASH	1  /* slash */
#define WEAP_PIERCE	2  /* arrows, stiletto */
#define WEAP_CLEAVE	3  /* axe */
#define WEAP_SLICE	4  /* katana */
#define WEAP_STAB	5  /* knife, dagger */
#define WEAP_WHIP	6  /* whips n chains */
#define WEAP_CRUSH	7  /* big hammers, flails */
#define WEAP_BLUD	8  /* bludgeoning, club, stick */


/* definitions for detailed pickup descriptions.
 *   The objective is to define intelligent groups of items that the
 *   user can pick up or leave as he likes. */

/* high bit as flag for new pickup options */
#define PU_NOTHING		0x00000000

#define PU_DEBUG		0x10000000
#define PU_INHIBIT		0x20000000
#define PU_STOP			0x40000000
#define PU_NEWMODE		0x80000000

#define PU_RATIO		0x0000000F

#define PU_FOOD			0x00000010
#define PU_DRINK		0x00000020
#define PU_VALUABLES		0x00000040
#define PU_BOW			0x00000080

#define PU_ARROW		0x00000100
#define PU_HELMET		0x00000200
#define PU_SHIELD		0x00000400
#define PU_ARMOUR		0x00000800

#define PU_BOOTS		0x00001000
#define PU_GLOVES		0x00002000
#define PU_CLOAK		0x00004000
#define PU_KEY			0x00008000

#define PU_MISSILEWEAPON	0x00010000
#define PU_ALLWEAPON		0x00020000
#define PU_MAGICAL		0x00040000
#define PU_POTION		0x00080000


/* Instead of using arbitrary constants for indexing the
 * freearr, add these values.  <= SIZEOFFREE1 will get you
 * within 1 space.  <= SIZEOFFREE2 wll get you withing 
 * 2 spaces, and the entire array (<= SIZEOFFREE) is 
 * three spaces
 */
#define SIZEOFFREE1 8
#define SIZEOFFREE2 24
#define SIZEOFFREE 49

#define PATH_NULL	0x00000000      /* 0 */
#define PATH_PROT	0x00000001      /* 1 */
#define PATH_FIRE	0x00000002      /* 2 */
#define PATH_FROST	0x00000004   /* 4 */
#define PATH_ELEC	0x00000008      /* 8 */
#define PATH_MISSILE	0x00000010   /* 16 */
#define PATH_SELF	0x00000020      /* 32 */
#define PATH_SUMMON	0x00000040   /* 64 */
#define PATH_ABJURE	0x00000080  /* 128 */
#define PATH_RESTORE	0x00000100  /* 256 */
#define PATH_DETONATE	0x00000200  /* 512 */
#define PATH_MIND	0x00000400    /* 1024 */
#define PATH_CREATE	0x00000800   /* 2048 */
#define PATH_TELE	0x00001000     /* 4096 */
#define PATH_INFO	0x00002000     /* 8192 */
#define PATH_TRANSMUTE	0x00004000   /* 16384 */
#define PATH_TRANSFER	0x00008000  /*  32768 */
#define PATH_TURNING	0x00010000  /* 65536 */
#define PATH_WOUNDING	0x00020000  /* 131072 */
#define PATH_DEATH	0x00040000  /* 262144 */
#define PATH_LIGHT	0x00080000  /* 524288 */

#define NRSPELLPATHS	20

#define NROFREALSPELLS	205	/* Number of different spells */

#define NROFSKILLS  33  /* match to the number of elements in skills[] array */

#define NROF_SOUNDS (23 + NROFREALSPELLS) /* Number of sounds */

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

/* convenience macros to determine what kind of things we are dealing with */

#define IS_LIVE(op) \
	(op->type == PLAYER || QUERY_FLAG(op, FLAG_MONSTER) || \
	(QUERY_FLAG(op, FLAG_ALIVE) && !QUERY_FLAG(op, FLAG_GENERATOR) && \
	!op->type == DOOR))

#define IS_ARROW(op) \
	(op->type==ARROW || op->type==MMISSILE || op->type==BULLET)

/* the flags */

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

/* this defines paralyze as spell on player, not onlya effect on speed */
#define FLAG_PARALYZED      23 /* Object is paralyzed */

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

/* Start of values in flags[2] */
#define FLAG_LIFESAVE		64 /* Saves a players' life once, then destr. */
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

#define FLAG_BERSERK            99 /* monster will attack closest living
				      object */
#define NUM_FLAGS		99 /* Should always be equal to the last
				    * defined flag */

/* Values can go up to 127 before the size of the flags array in the
 * object structure needs to be enlarged.
 */


#define NROFNEWOBJS(xyz)	((xyz)->stats.food)

#define SLOW_PENALTY(xyz)	((xyz)->stats.exp)/1000.0
#define SET_SLOW_PENALTY(xyz,fl)	(xyz)->stats.exp=(fl)*1000
#define SET_GENERATE_TYPE(xyz,va)	(xyz)->stats.sp=(va)
#define GENERATE_TYPE(xyz)	((xyz)->stats.sp)
#define GENERATE_SPEED(xyz)	((xyz)->stats.maxsp) /* if(!RANDOM()%<speed>) */

/* Note: These values are only a default value, resizing can change them */
#define INV_SIZE		12	/* How many items can be viewed in inventory */
#define LOOK_SIZE		6	/* ditto, but for the look-window */
#define MAX_INV_SIZE		40	/* For initializing arrays */
#define MAX_LOOK_SIZE		40	/* ditto for the look-window */

#define EDITABLE(xyz)		((xyz)->arch->editable)

#define E_MONSTER		0x00000001
#define E_EXIT			0x00000002
#define E_TREASURE		0x00000004
#define E_BACKGROUND		0x00000008
#define E_DOOR			0x00000010
#define E_SPECIAL		0x00000020
#define E_SHOP			0x00000040
#define E_NORMAL		0x00000080
#define E_FALSE_WALL		0x00000100
#define E_WALL			0x00000200
#define E_EQUIPMENT		0x00000400
#define E_OTHER			0x00000800
#define E_ARTIFACT		0x00001000

#define EXIT_PATH(xyz)		(xyz)->slaying
#define EXIT_LEVEL(xyz)		(xyz)->stats.food
#define EXIT_X(xyz)		(xyz)->stats.hp
#define EXIT_Y(xyz)		(xyz)->stats.sp

/* for use by the lighting code */
#define MAX_LIGHT_RADII		4	/* max radii for 'light' object, really
					 * large values allow objects that can
					 * slow down the game */
#define MAX_DARKNESS		5	/* maximum map darkness, there is no
					 * practical reason to exceed this */ 
#define BRIGHTNESS(xyz)		(xyz)->glow_radius>MAX_LIGHT_RADII? \
				  MAX_LIGHT_RADII:(xyz)->glow_radius;

#define F_BUY 0
#define F_SELL 1
#define F_TRUE 2	/* True value of item, unadjusted */

#define DIRX(xyz)	freearr_x[(xyz)->direction]
#define DIRY(xyz)	freearr_y[(xyz)->direction]

#define D_LOCK(xyz)	(xyz)->contr->freeze_inv=(xyz)->contr->freeze_look=1;
#define D_UNLOCK(xyz)	(xyz)->contr->freeze_inv=(xyz)->contr->freeze_look=0;

#define ARMOUR_SPEED(xyz)	(xyz)->last_sp
#define ARMOUR_SPELLS(xyz)	(xyz)->last_heal
#define WEAPON_SPEED(xyz)	(xyz)->last_sp

/* GET_?_FROM_DIR if used only for positional firing where dir is X and Y
   each of them signed char, concatenated in a int16 */
#define GET_X_FROM_DIR(dir) (signed char) (  dir & 0xFF )
#define GET_Y_FROM_DIR(dir) (signed char) ( (dir & 0xFF00) >> 8)
#define SET_DIR_FROM_XY(X,Y) (signed char)X + ( ((signed char)Y)<<8)
#define FIRE_DIRECTIONAL 0
#define FIRE_POSITIONAL  1

/******************************************************************************/
/* Monster Movements added by kholland@sunlab.cit.cornell.edu                 */
/******************************************************************************/
/* if your monsters start acting wierd, mail me                               */
/******************************************************************************/
/* the following definitions are for the attack_movement variable in monsters */
/* if the attack_variable movement is left out of the monster archetype, or is*/
/* set to zero                                                                */
/* the standard mode of movement from previous versions of crossfire will be  */
/* used. the upper four bits of movement data are not in effect when the monst*/
/* er has an enemy. these should only be used for non agressive monsters.     */
/* to program a monsters movement add the attack movement numbers to the movem*/
/* ment numbers example a monster that moves in a circle until attacked and   */
/* then attacks from a distance:                                              */
/*                                                      CIRCLE1 = 32          */
/*                                              +       DISTATT = 1           */
/*                                      -------------------                   */
/*                      attack_movement = 33                                  */
/******************************************************************************/
#define DISTATT  1 /* move toward a player if far, but mantain some space,  */
                   /* attack from a distance - good for missile users only  */
#define RUNATT   2 /* run but attack if player catches up to object         */
#define HITRUN   3 /* run to then hit player then run away cyclicly         */
#define WAITATT  4 /* wait for player to approach then hit, move if hit     */
#define RUSH     5 /* Rush toward player blindly, similiar to dumb monster  */
#define ALLRUN   6 /* always run never attack good for sim. of weak player  */
#define DISTHIT  7 /* attack from a distance if hit as recommended by Frank */
#define WAIT2    8 /* monster does not try to move towards player if far    */
                   /* maintains comfortable distance                        */
#define PETMOVE 16 /* if the upper four bits of move_type / attack_movement */
                   /* are set to this number, the monster follows a player  */
                   /* until the owner calls it back or off                  */
                   /* player followed denoted by 0b->owner                  */
                   /* the monster will try to attack whatever the player is */
                   /* attacking, and will continue to do so until the owner */
                   /* calls off the monster - a key command will be         */
                   /* inserted to do so                                     */
#define CIRCLE1 32 /* if the upper four bits of move_type / attack_movement */
                   /* are set to this number, the monster will move in a    */
                   /* circle until it is attacked, or the enemy field is    */
                   /* set, this is good for non-aggressive monsters and NPC */ 
#define CIRCLE2 48 /* same as above but a larger circle is used             */
#define PACEH   64 /* The Monster will pace back and forth until attacked   */
                   /* this is HORIZONTAL movement                           */
#define PACEH2  80 /* the monster will pace as above but the length of the  */
                   /* pace area is longer and the monster stops before      */
                   /* changing directions                                   */
                   /* this is HORIZONTAL movement                           */
#define RANDO   96 /* the monster will go in a random direction until       */
                   /* it is stopped by an obstacle, then it chooses another */
                   /* direction.                                            */
#define RANDO2 112 /* constantly move in a different random direction       */
#define PACEV  128 /* The Monster will pace back and forth until attacked   */
                   /* this is VERTICAL movement                             */
#define PACEV2 144 /* the monster will pace as above but the length of the  */
                   /* pace area is longer and the monster stops before      */
                   /* changing directions                                   */
                   /* this is VERTICAL movement                             */
#define LO4     15 /* bitmasks for upper and lower 4 bits from 8 bit fields */
#define HI4    240

/*
 * Use of the state-variable in player objects:
 */

#define ST_PLAYING      0
#define ST_PLAY_AGAIN   1
#define ST_ROLL_STAT    2
#define ST_CHANGE_CLASS 3
#define ST_CONFIRM_QUIT 4
#define ST_CONFIGURE    5
#define ST_GET_NAME     6
#define ST_GET_PASSWORD 7
#define ST_CONFIRM_PASSWORD     8
#define ST_GET_PARTY_PASSWORD 10

#define BLANK_FACE_NAME "blank.111"
#define EMPTY_FACE_NAME "empty.111"
#define POTION_FACE_NAME	"potiongen.111" 
#define DARK_FACE1_NAME		"dark1.111"
#define DARK_FACE2_NAME		"dark2.111"
#define DARK_FACE3_NAME		"dark3.111"

/*
 * Defines for the luck/random functions to make things more readable
 */

#define PREFER_HIGH	1
#define PREFER_LOW	0

/* Simple function we use below to keep adding to the same string
 * but also make sure we don't overwrite that string.
 */
static inline void safe_strcat(char *dest, char *orig, int *curlen, int maxlen)
{
    if (*curlen == (maxlen-1)) return;
    strncpy(dest+*curlen, orig, maxlen-*curlen-1);
    dest[maxlen-1]=0;
    *curlen += strlen(orig);
    if (*curlen>(maxlen-1)) *curlen=maxlen=1;
}


/* The SAFE versions of these call the safe_strcat function above.
 * Ideally, all functions should use the SAFE functions, but they 
 * require some extra support in the calling function to remain as
 * efficient.
 */
#define DESCRIBE_ABILITY_SAFE(retbuf, variable, name, len, maxlen) \
    if(variable) { \
      int i,j=0; \
      safe_strcat(retbuf,"(" name ": ", len, maxlen); \
      for(i=0; i<NROFATTACKS; i++) \
        if(variable & (1<<i)) { \
          if (j) \
            safe_strcat(retbuf,", ", len, maxlen); \
          else \
            j = 1; \
          safe_strcat(retbuf, attacks[i], len, maxlen); \
        } \
      safe_strcat(retbuf,")",len,maxlen); \
    }
 

/* separated this from the common/item.c file. b.t. Dec 1995 */

#define DESCRIBE_ABILITY(retbuf, variable, name) \
    if(variable) { \
      int i,j=0; \
      strcat(retbuf,"(" name ": "); \
      for(i=0; i<NROFATTACKS; i++) \
        if(variable & (1<<i)) { \
          if (j) \
            strcat(retbuf,", "); \
          else \
            j = 1; \
          strcat(retbuf, attacks[i]); \
        } \
      strcat(retbuf,")"); \
    }
 
 
#define DESCRIBE_PATH(retbuf, variable, name) \
    if(variable) { \
      int i,j=0; \
      strcat(retbuf,"(" name ": "); \
      for(i=0; i<NRSPELLPATHS; i++) \
        if(variable & (1<<i)) { \
          if (j) \
            strcat(retbuf,", "); \
          else \
            j = 1; \
          strcat(retbuf, spellpathnames[i]); \
        } \
      strcat(retbuf,")"); \
    }


#define DESCRIBE_PATH_SAFE(retbuf, variable, name, len, maxlen) \
    if(variable) { \
      int i,j=0; \
      safe_strcat(retbuf,"(" name ": ", len, maxlen); \
      for(i=0; i<NRSPELLPATHS; i++) \
        if(variable & (1<<i)) { \
          if (j) \
            safe_strcat(retbuf,", ", len, maxlen); \
          else \
            j = 1; \
          safe_strcat(retbuf, spellpathnames[i], len, maxlen); \
        } \
      safe_strcat(retbuf,")", len, maxlen); \
    }

/* Flags for apply_special() */
enum apply_flag {
  /* Basic flags, always use one of these */
	AP_NULL			= 0,
	AP_APPLY		= 1,
	AP_UNAPPLY		= 2,

        AP_BASIC_FLAGS		= 15,

  /* Optional flags, for bitwise or with a basic flag */
        AP_NO_MERGE		= 16,
	AP_IGNORE_CURSE		= 32,
};

/* Cut off point of when an object is put on the active list or not */
#define MIN_ACTIVE_SPEED	0.00001

/*
 * random() is much better than rand().  If you have random(), use it instead.
 * You shouldn't need to change any of this
 *
 * 0.93.3: It looks like linux has random (previously, it was set below
 * to use rand).  Perhaps old version of linux lack rand?  IF you run into
 * problems, add || defined(linux) the #if immediately below.
 *
 * 0.94.2 - you probably shouldn't need to change any of the rand stuff
 * here.
 */

#ifdef HAVE_SRANDOM
#define RANDOM() random()
#define SRANDOM(xyz) srandom(xyz)
#else
#  ifdef HAVE_SRAND48
#  define RANDOM() lrand48()
#  define SRANDOM(xyz) srand48(xyz)
#  else
#    ifdef HAVE_SRAND
#      define RANDOM() rand()
#      define SRANDOM(xyz) srand(xyz)
#    else
#      error "Could not find a usable random routine"
#    endif
#  endif
#endif

#define PLUGINS
#endif /* DEFINE_H */
