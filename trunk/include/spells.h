/*
 * static char *rcsid_spells_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2003 Mark Wedel & Crossfire Development Team
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

#ifndef SPELLS_H
#define SPELLS_H

#define PATH_NULL	0x00000000      /* 0 */
#define PATH_PROT	0x00000001      /* 1 */
#define PATH_FIRE	0x00000002      /* 2 */
#define PATH_FROST	0x00000004	/* 4 */
#define PATH_ELEC	0x00000008      /* 8 */
#define PATH_MISSILE	0x00000010	/* 16 */
#define PATH_SELF	0x00000020      /* 32 */
#define PATH_SUMMON	0x00000040	/* 64 */
#define PATH_ABJURE	0x00000080	/* 128 */
#define PATH_RESTORE	0x00000100	/* 256 */
#define PATH_DETONATE	0x00000200	/* 512 */
#define PATH_MIND	0x00000400	/* 1024 */
#define PATH_CREATE	0x00000800	/* 2048 */
#define PATH_TELE	0x00001000	/* 4096 */
#define PATH_INFO	0x00002000	/* 8192 */
#define PATH_TRANSMUTE	0x00004000	/* 16384 */
#define PATH_TRANSFER	0x00008000	/*  32768 */
#define PATH_TURNING	0x00010000	/* 65536 */
#define PATH_WOUNDING	0x00020000	/* 131072 */
#define PATH_DEATH	0x00040000	/* 262144 */
#define PATH_LIGHT	0x00080000	/* 524288 */

#define PATH_SP_MULT(op,spell) (((op->path_attuned & spell->path_attuned) ? 0.8 : 1) * \
				((op->path_repelled & spell->path_attuned) ? 1.25 : 1))

#define NRSPELLPATHS	20
extern const char* const spellpathnames[NRSPELLPATHS];

/* The only place this is really used is to allocate an array
 * when printing out the spells the player knows.
 */
#define NROFREALSPELLS	1024

/* this is passed to SP_level_spellpoint_cost to determine
 * what to check.  These values are also used in other places
 * when we want to pass into a function if it is a cleric spell
 * or a wizard (mana) spell.
 */
#define SPELL_MANA	0x1
#define SPELL_GRACE	0x2
#define SPELL_HIGHEST	0x3

/* This is the subtype for the spells.  Start at 1 so that
 * it is easy to see 0 as an uninitialized value.
 * Note that for some spells, subtype pretty accurately
 * describes the entire spell (SP_DETECT_MAGIC).  But for other, the subtype
 * may not really say much (eg, SP_BOLT), and it is other
 * fields within the object which really determines its properties.
 * No effort is made to match these new numbers with the old ones,
 * and given there is not a 1:1 mapping, you can't do that anyways.
 */

#define SP_RAISE_DEAD	    1
#define SP_RUNE		    2
#define SP_MAKE_MARK	    3
#define SP_BOLT		    4
#define SP_BULLET	    5
#define SP_EXPLOSION	    6
#define SP_CONE		    7
#define SP_BOMB		    8
#define SP_WONDER	    9
#define SP_SMITE	    10
#define SP_MAGIC_MISSILE    11
#define SP_SUMMON_GOLEM	    12
#define SP_DIMENSION_DOOR   13
#define SP_MAGIC_MAPPING    14
#define SP_MAGIC_WALL	    15
#define SP_DESTRUCTION	    16
#define SP_PERCEIVE_SELF    17
#define SP_WORD_OF_RECALL   18
#define SP_INVISIBLE	    19
#define SP_PROBE	    20
#define SP_HEALING	    21
#define SP_CREATE_FOOD	    22
#define SP_EARTH_TO_DUST    23
#define SP_CHANGE_ABILITY   24
#define SP_BLESS	    25
#define SP_CURSE	    26
#define SP_SUMMON_MONSTER   27
#define SP_CHARGING	    28
#define SP_POLYMORPH	    29
#define SP_ALCHEMY	    30
#define SP_REMOVE_CURSE	    31
#define SP_IDENTIFY	    32
#define SP_DETECTION	    33
#define SP_MOOD_CHANGE	    34
#define SP_MOVING_BALL	    35
#define SP_SWARM	    36
#define SP_CHANGE_MANA	    37
#define SP_DISPEL_RUNE	    38
#define SP_CREATE_MISSILE   39
#define SP_CONSECRATE	    40
#define SP_ANIMATE_WEAPON   41
#define SP_LIGHT	    42
#define SP_CHANGE_MAP_LIGHT 43
#define SP_FAERY_FIRE	    44
#define SP_CAUSE_DISEASE    45
#define SP_AURA		    46
#define SP_TOWN_PORTAL	    47
/*#define SP_PARTY_SPELL      48*/
#define SP_ITEM_CURSE_BLESS 49

/* Potion subtypes */
#define POT_SPELL	    1
#define POT_DUST	    2
#define POT_FIGURINE	    3
#define POT_BALM	    4

/* This is for the force subtypes */
#define FORCE_CONFUSION		1
#define FORCE_CHANGE_ABILITY	2
#define FORCE_TRANSFORMED_ITEM  3

#define PATH_TIME_MULT(op,spell) (((op->path_attuned & spell->path_attuned) ? 0.8 : 1) * \
				((op->path_repelled & spell->path_attuned) ? 1.25 : 1))

/* These are some hard coded values that are used within the code
 * for spell failure effects or pieces of spells.  Rather
 * then hardcode the names, use defines so it is easier to
 * update if necessary.
 */
#define SP_MED_FIREBALL	"spell_medium_fireball"
#define LOOSE_MANA	"loose_magic"
#define SPELL_WONDER	"spell_wonder"
#define GOD_POWER	"god_power"
#define SPLINT		"splint"	/* for bombs */
#define SWARM_SPELL	"swarm_spell"
#define GENERIC_RUNE	"generic_rune"
#define HOLY_POSSESSION	"spell_holy_possession"
#define FORCE_NAME	"force"		/* instead of it being hardcoded */
/* This is used for fumbles - this arch is all set up to do
 * the right just by inserting it
 */
#define EXPLODING_FIREBALL  "exploding_fireball"

#endif /* SPELLS_H */
