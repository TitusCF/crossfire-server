/*
 * static char *rcsid_living_h =
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

#ifndef LIVING_H
#define LIVING_H

#define STR 0
#define DEX 1
#define CON 2
#define WIS 3
#define CHA 4
#define INT 5
#define POW 6

/* Changed from NO_STAT to NO_STAT_VAL to fix conlfict on
 * AIX systems
 */
#define NO_STAT_VAL 99    /* needed by skills code -b.t. */

extern char *attacks[NROFATTACKS];
extern char *spellpathnames[NRSPELLPATHS];

extern float cha_bonus[MAX_STAT + 1];
extern int dex_bonus[MAX_STAT + 1];
extern int thaco_bonus[MAX_STAT + 1];
extern int turn_bonus[MAX_STAT + 1];
extern int max_carry[MAX_STAT + 1];
extern int dam_bonus[MAX_STAT + 1];
extern int savethrow[111];
extern int turn_bonus[MAX_STAT + 1];
extern int learn_prayer_chance[MAX_STAT + 1];
extern int learn_spell[];
extern char *restore_msg[7];
extern char *statname[7];
extern char *short_stat_name[7];
extern char *lose_msg[7];
extern float speed_bonus[MAX_STAT + 1];
extern int weight_limit[MAX_STAT + 1];

typedef struct liv { /* Mostly used by "alive" objects */
  sint8		Str,Dex,Con,Wis,Cha,Int,Pow;
  sint8		wc,ac;		/* Weapon Class and Armour Class */
  sint16	hp;		/* Hit Points. */
  sint16	maxhp;
  sint16	sp;		/* Spell points.  Used to cast mage spells. */
  sint16	maxsp;		/* Max spell points. */
  sint16	grace;		/* Grace.  Used to invoke clerical prayers. */
  sint16	maxgrace;	/* Grace.  Used to invoke clerical prayers. */
  sint32	exp;		/* Experience.  Killers gain 1/10. */
  sint16	food;		/* How much food in stomach.  0 = starved. */
  sint16	dam;		/* How much damage this object does when hitting */
  sint8		luck;		/* Affects thaco and ac from time to time */
} living;


#endif
