/*
 * static char *rcsid_treasure_h =
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

/* Highest level of rods/staves/scrolls to generate. */
#define MAX_SPELLITEM_LEVEL 110

/*
 * Flags to generate_treasures():
 */

enum {
  GT_ENVIRONMENT = 0x0001,
  GT_INVISIBLE = 0x0002,
  GT_STARTEQUIP = 0x0004,
  GT_APPLY = 0x0008,
  GT_ONLY_GOOD = 0x0010,
  GT_UPDATE_INV = 0x0020,
  GT_MINIMAL = 0x0040	/* Do minimal adjustments */
};


/* when a treasure got cloned from archlist, we want perhaps change some default
 * values. All values in this structure will override the default arch.
 * TODO: It is a bad way to implement this with a special structure.
 * Because the real arch list is a at runtime not changed, we can grap for example
 * here a clone of the arch, store it in the treasure list and then run the original
 * arch parser over this clone, using the treasure list as script until an END comes. 
 * This will allow ANY changes which is possible and we use ony one parser.
 */

typedef struct _change_arch {
    const char *name;              /* is != NULL, copy this over the original arch name */
    const char *title;             /* is != NULL, copy this over the original arch name */
    const char *slaying;           /* is != NULL, copy this over the original arch name */
} _change_arch;


/*
 * treasure is one element in a linked list, which together consist of a
 * complete treasure-list.  Any arch can point to a treasure-list
 * to get generated standard treasure when an archetype of that type
 * is generated (from a generator)
*/

typedef struct treasurestruct {
  struct archt *item;		    /* Which item this link can be */
  const char *name;			    /* If non null, name of list to use
				       instead */
  struct treasurestruct *next;	    /* Next treasure-item in a linked list */
  struct treasurestruct *next_yes;  /* If this item was generated, use */
				    /* this link instead of ->next */
  struct treasurestruct *next_no;   /* If this item was not generated, */
				    /* then continue here */
  struct _change_arch change_arch;  /* override default arch values if set in treasure list */
  uint8 chance;			    /* Percent chance for this item */
  uint8 magic;			    /* Max magic bonus to item */
				    /* If the entry is a list transition,
				     * 'magic' contains the difficulty
				     * required to go to the new list
 				     */
  uint16 nrof;			    /* random 1 to nrof items are generated */
} treasure;


typedef struct treasureliststruct {
  const char *name;				/* Usually monster-name/combination */
  sint16 total_chance;			/* If non-zero, only 1 item on this
					 * list should be generated.  The
					 * total_chance contains the sum of
					 * the chance for this list.
					 */
  struct treasureliststruct *next;	/* Next treasure-item in linked list */
  struct treasurestruct *items;		/* Items in this list, linked */
} treasurelist;



#endif /* TREASURE_H */
