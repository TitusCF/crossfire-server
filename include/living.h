/*
 * static char *rcsid_living_h =
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

#ifndef LIVING_H
#define LIVING_H

#define STR 0
#define DEX 1
#define CON 2
#define WIS 3
#define CHA 4
#define INT 5
#define POW 6

#define NO_STAT 99    /* needed by skills code -b.t. */

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

#define SLOW_PENALTY(xyz)		((xyz)->stats.exp)/1000.0
#define SET_SLOW_PENALTY(xyz,fl)	(xyz)->stats.exp=(fl)*1000
#define SET_GENERATE_TYPE(xyz,va)	(xyz)->stats.sp=(va)
#define GENERATE_TYPE(xyz)		((xyz)->stats.sp)
#define GENERATE_SPEED(xyz)		((xyz)->stats.maxsp) /* if(!RANDOM()%<speed>) */

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
extern int object_saves[NROFATTACKS][NROFMATERIALS];
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
