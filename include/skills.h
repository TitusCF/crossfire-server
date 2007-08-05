/*
 * static char *rcsid_skills_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2003 Mark Wedel & Crossfire Development Team
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

/**
 * @file
 * Skill-related defines, including subtypes.
 */

#ifndef SKILLS_H
#define SKILLS_H

/**
 * @defgroup SK_xxx Skill subtypes
 *
 * This list is just a subtype <-> skill (code wise) in the
 * server translation.  In theory, the processing of the different
 * skills could be done via strncmp
 * This list doesn't really try to identify what the skills do.
 * The order of this list has no special meaning.  0 is not used
 * to denote improperly set objects.
 */
/*@{*/
#define SK_LOCKPICKING		1
#define SK_HIDING		2
#define SK_SMITHERY		3
#define SK_BOWYER		4
#define SK_JEWELER		5
#define SK_ALCHEMY		6
#define SK_STEALING		7
#define SK_LITERACY		8
#define SK_BARGAINING		9
#define SK_JUMPING		10
#define SK_DET_MAGIC		11
#define SK_ORATORY		12
#define SK_SINGING		13
#define SK_DET_CURSE		14
#define SK_FIND_TRAPS		15
#define SK_MEDITATION		16
#define SK_PUNCHING		17
#define SK_FLAME_TOUCH		18
#define SK_KARATE		19
#define SK_CLIMBING		20
#define SK_WOODSMAN		21
#define SK_INSCRIPTION		22
#define SK_ONE_HANDED_WEAPON	23
#define SK_MISSILE_WEAPON	24
#define SK_THROWING		25
#define SK_USE_MAGIC_ITEM	26
#define SK_DISARM_TRAPS		27
#define SK_SET_TRAP		28
#define SK_THAUMATURGY		29
#define SK_PRAYING		30
#define	SK_CLAWING		31
#define SK_LEVITATION		32
#define SK_SUMMONING		33
#define SK_PYROMANCY		34
#define SK_EVOCATION		35
#define SK_SORCERY		36
#define SK_TWO_HANDED_WEAPON	37
#define SK_WRAITH_FEED		38
#define SK_HARVESTING 39
/*@}*/

/**
 * This is the highest number skill in the table +1
 * This is used to store pointers to the actual skills -
 * to make life easier, we use the value above as index,
 * eg, SK_EVOCATION (35) will be in last_skills[35].
 */
#define NUM_SKILLS		40


/**
 * @defgroup SK_EXP_xxx Experience flags
 * This is used in the exp functions - basically what to do if
 * the player doesn't have the skill he should get exp in.
 */
/*@{*/
#define SK_EXP_ADD_SKILL	0   /**< Give the player the skill. */
#define SK_EXP_TOTAL		1   /**< Give player exp to total, no skill. */
#define SK_EXP_NONE		2   /**< Player gets nothing. */
#define SK_SUBTRACT_SKILL_EXP	3   /**< Used when removing exp. */
/*@}*/

/** True if op is using skill, false else. */
#define USING_SKILL(op, skill)  ((op)->chosen_skill && (op)->chosen_skill->subtype == skill)

/**
 * This macro is used in fix_object() to define if this is a sill
 * that should be used to calculate wc's and the like.
 */
#define IS_COMBAT_SKILL(num) \
    ((num==SK_PUNCHING) || (num==SK_FLAME_TOUCH) || (num==SK_KARATE) || \
     (num==SK_ONE_HANDED_WEAPON) || (num==SK_MISSILE_WEAPON) || \
     (num==SK_THROWING) || (num==SK_CLAWING) || (num==SK_TWO_HANDED_WEAPON) || \
     (num==SK_WRAITH_FEED))

/**
 * Like IS_COMBAT_SKILL above, but instead this is used to determine
 * how many mana points the player has.
 */
#define IS_MANA_SKILL(num) \
    ((num==SK_SORCERY) || (num==SK_EVOCATION) || \
     (num==SK_PYROMANCY) || (num==SK_SUMMONING))

/**
 * Currently only one of these, but put the define here to make
 * it easier to expand it in the future */
#define IS_GRACE_SKILL(num) \
    (num==SK_PRAYING)



extern const char *skill_names[NUM_SKILLS];

#ifdef WANT_UNARMED_SKILLS
/** Table of unarmed attack skills.  Terminated by -1.  This
 * is also the list that we should try to use skills when
 * automatically applying one for the player.
 * Note it is hardcoded in the skill_util.c that dragons always
 * want clawing if possible.
 * included in a #ifdef so we don't get bunches of warnings about
 * unused values.  it is located here instead of a .c file to make
 * updates easier and put it in a more central place - it shouldn't
 * change very often, but it make sense to have it with the enumerated
 * skill numbers above.
 */
static uint8 unarmed_skills[] = {
SK_KARATE,
SK_CLAWING,
SK_FLAME_TOUCH,
SK_PUNCHING,
SK_WRAITH_FEED,
-1
};

/* Just in case one file includes this more than once */
#undef WANT_UNARMED_SKILLS

#endif

#endif /* SKILLS_H */
