/*
 * static char *rcsid_living_h =
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

#ifndef LIVING_H
#define LIVING_H

#define STR 0
#define DEX 1
#define CON 2
#define WIS 3
#define CHA 4
#define INT 5
#define POW 6
#define NUM_STATS 7

#define MAXLEVEL      115

/* Changed from NO_STAT to NO_STAT_VAL to fix conlfict on
 * AIX systems
 */
#define NO_STAT_VAL 99    /* needed by skills code -b.t. */

extern const char *const attacks[NROFATTACKS];

extern const float cha_bonus[MAX_STAT + 1];
extern const int dex_bonus[MAX_STAT + 1];
extern const int thaco_bonus[MAX_STAT + 1];
extern const int turn_bonus[MAX_STAT + 1];
extern const int max_carry[MAX_STAT + 1];
extern const int dam_bonus[MAX_STAT + 1];
extern const int learn_spell[];
extern const char *const restore_msg[NUM_STATS];
extern const char *const statname[NUM_STATS];
extern const char *const short_stat_name[NUM_STATS];
extern const char *const lose_msg[NUM_STATS];
extern const float speed_bonus[MAX_STAT + 1];
extern const uint32 weight_limit[MAX_STAT + 1];
extern const int cleric_chance[MAX_STAT + 1];
extern const int fear_bonus[MAX_STAT + 1];

/**
 * Mostly used by "alive" objects
 * @todo fix comment, living is used for many things :)
 * also fix comments for fields (should probably be in a separate file).
 */
typedef struct liv {
  sint8		Str,Dex,Con,Wis,Cha,Int,Pow;
  sint8		wc,ac;      /**< Weapon Class and Armour Class */
  sint16	hp;         /**< Hit Points. */
  sint16	maxhp;      /**< Max hit points. */
  sint16	sp;         /** Spell points.  Used to cast mage spells. */
  sint16	maxsp;      /**< Max spell points. */
  sint16	grace;      /**< Grace.  Used to invoke clerical prayers. */
  sint16	maxgrace;   /**< Grace.  Used to invoke clerical prayers. */
  sint64	exp;        /**< Experience.  Killers gain 1/10. */
  sint16	food;       /**< How much food in stomach.  0 = starved. */
  sint16	dam;        /**< How much damage this object does when hitting */
  sint8		luck;       /**< Affects thaco and ac from time to time */
} living;

#endif /* LIVING_H */
