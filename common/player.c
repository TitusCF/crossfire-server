/*
 * static char *rcsid_player_c =
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

#include <global.h>
#include <funcpoint.h>

void free_player(player *pl) {

    if (first_player!=pl) {
	player *prev=first_player;
	while(prev!=NULL&&prev->next!=NULL&&prev->next!=pl)
	    prev=prev->next;
	if(prev->next!=pl) {
	    LOG(llevError,"Free_player: Can't find previous player.\n");
	    exit(1);
	}
	prev->next=pl->next;
    } else first_player=pl->next;

    if(pl->ob != NULL) {
	if (!QUERY_FLAG(pl->ob, FLAG_REMOVED)) remove_ob(pl->ob);
	free_object(pl->ob);
    }

    CFREE(pl);
}


/* Determine if the attacktype represented by the
 * specified attack-number is enabled for dragon players.
 * A dragon player (quetzal) can gain resistances for
 * all enabled attacktypes.
 */
int atnr_is_dragon_enabled(int attacknr) {
  if (attacknr == ATNR_MAGIC || attacknr == ATNR_FIRE ||
      attacknr == ATNR_ELECTRICITY || attacknr == ATNR_COLD ||
      attacknr == ATNR_ACID || attacknr == ATNR_POISON)
    return 1;
  return 0;
}

/*
 * returns true if the adressed object 'ob' is a player
 * of the dragon race.
 */
int is_dragon_pl(object* op) {
  if (op != NULL && op->type == PLAYER && op->arch != NULL
      && op->arch->clone.race != NULL &&
      strcmp(op->arch->clone.race, "dragon")==0)
    return 1;
  return 0;
}
