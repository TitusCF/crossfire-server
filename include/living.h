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

/**
 * @file
 * Structure containing object statistics.
 */

#ifndef LIVING_H
#define LIVING_H

/**
 * @defgroup STATS Object statistics.
 */
/*@{*/
#define STR 0       /**< Strength. */
#define DEX 1       /**< Dexterity. */
#define CON 2       /**< Constitution. */
#define WIS 3       /**< Wisdom. */
#define CHA 4       /**< Charisma. */
#define INT 5       /**< Intelligence. */
#define POW 6       /**< Power. */
#define NUM_STATS 7 /**< Number of statistics. */
/*@}*/

/** Maximum level a player can reach. */
#define MAXLEVEL 115

extern const char *const attacks[NROFATTACKS];

extern const char *const restore_msg[NUM_STATS];
extern const char *const statname[NUM_STATS];
extern const char *const short_stat_name[NUM_STATS];
extern const char *const lose_msg[NUM_STATS];

/**
 * Various statistics of objects.
 */
typedef struct liv {
    sint8         Str, Dex, Con, Wis, Cha, Int, Pow;
    sint8         wc;         /**< Weapon Class, how skilled, the lower the better. */
    sint8         ac;         /**< Armour Class, how hard to hit, the lower the better. */
    sint8         luck;       /**< Affects thaco and ac from time to time */
    sint16        hp;         /**< Hit Points. */
    sint16        maxhp;      /**< Max hit points. */
    sint16        sp;         /**< Spell points.  Used to cast mage spells. */
    sint16        maxsp;      /**< Max spell points. */
    sint16        grace;      /**< Grace.  Used to invoke clerical prayers. */
    sint16        maxgrace;   /**< Maximum grace.  Used to invoke clerical prayers. */
    sint16        dam;        /**< How much damage this object does when hitting */
    sint64        exp;        /**< Experience.  Killers gain 1/10. */
    sint32        food;       /**< How much food in stomach.  0 = starved. */
} living;

int get_cha_bonus(int stat);
int get_dex_bonus(int stat);
int get_thaco_bonus(int stat);
uint32 get_weight_limit(int stat);
int get_learn_spell(int stat);
int get_cleric_chance(int stat);
int get_turn_bonus(int stat);
int get_dam_bonus(int stat);
float get_speed_bonus(int stat);
int get_fear_bonus(int stat);

#endif /* LIVING_H */
