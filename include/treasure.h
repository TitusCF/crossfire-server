/*
 * static char *rcsid_treasure_h =
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

/*
 * defines and variables used by the artifact generation routines
 */

#ifndef TREASURE_H
#define TREASURE_H

#define MAGIC_VALUE		10000	/* Might need to increase later */
#define CHANCE_FOR_ARTIFACT	20

#define STARTMAL        100
#define STEPMAL         10

/* List, What type to clone_arch(), max magic, how many */

#define MAXMAGIC 4

#define DIFFLEVELS 31

/*
 * treasure is one element in a linked list, which together consist of a
 * complete treasure-list.  Any arch can point to a treasure-list
 * to get generated standard treasure when an archetype of that type
 * is generated (from a generator)
*/

typedef struct treasurestruct {
  struct archt *item;		    /* Which item this link can be */
  char *name;			    /* If non null, name of list to use
				       instead */
  struct treasurestruct *next;	    /* Next treasure-item in a linked list */
  struct treasurestruct *next_yes;  /* If this item was generated, use */
				    /* this link instead of ->next */
  struct treasurestruct *next_no;   /* If this item was not generated, */
				    /* then continue here */
  uint8 chance;			    /* Percent chance for this item */
  uint8 magic;			    /* Max magic bonus to item */
				    /* If the entry is a list transition,
				     * 'magic' contains the difficulty
				     * required to go to the new list
 				     */
  uint16 nrof;			    /* random 1 to nrof items are generated */
} treasure;

typedef struct treasureliststruct {
  char *name;				/* Usually monster-name/combination */
  sint16 total_chance;			/* If non-zero, only 1 item on this
					 * list should be generated.  The
					 * total_chance contains the sum of
					 * the chance for this list.
					 */
  struct treasureliststruct *next;	/* Next treasure-item in linked list */
  struct treasurestruct *items;		/* Items in this list, linked */
} treasurelist;



#endif
