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

/**
 * @file common/player.c
 * Player-structure related functions.
 */

#include <global.h>
#include <funcpoint.h>

/**
 * Clears player structure, including pointed object (through free_object()).
 *
 * @param pl
 * player to clear. Pointer is invalid after this call.
 */
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
    } else
        first_player=pl->next;

    if(pl->ob != NULL) {
        if (!QUERY_FLAG(pl->ob, FLAG_REMOVED)) remove_ob(pl->ob);
        free_object(pl->ob);
    }
    /* Clear item stack (used by DMs only) */
    if (pl->stack_items)
        free( pl->stack_items );

    client_spell *info = pl->spell_state;
    client_spell* next;
    while ( info )
    {
        next = info->next;
        free( info );
        info = next;
    }

    free(pl->socket.faces_sent);

    CFREE(pl);
}


/**
 * Determine if the attacktype represented by the
 * specified attack-number is enabled for dragon players.
 * A dragon player (quetzal) can gain resistances for
 * all enabled attacktypes.
 *
 * @param attacknr
 * attacktype to check
 * @return
 * TRUE if player can gain resistances in that, FALSE else.
 */
int atnr_is_dragon_enabled(int attacknr) {
  if (attacknr == ATNR_MAGIC || attacknr == ATNR_FIRE ||
      attacknr == ATNR_ELECTRICITY || attacknr == ATNR_COLD ||
      attacknr == ATNR_ACID || attacknr == ATNR_POISON)
    return 1;
  return 0;
}

/**
 * Checks if player is a dragon.
 *
 * @param op
 * player to check. Can be NULL.
 * @return
 * TRUE if the adressed object 'ob' is a player of the dragon race.
 */
int is_dragon_pl(const object* op) {
  if (op != NULL && op->type == PLAYER && op->arch != NULL
      && op->arch->clone.race != NULL &&
      strcmp(op->arch->clone.race, "dragon")==0)
    return 1;
  return 0;
}

/**
 * Gets the (client-side) spell state for specified spell. Will be created to empty state if not found.
 *
 * @param pl
 * player we're handling.
 * @param spell
 * spell for which to search data.
 * @return
 * state information for that spell.
 *
 * @todo
 * check malloc's return value for NULL.
 */
client_spell* get_client_spell_state(player* pl, object* spell)
{
    client_spell* info = pl->spell_state;
    while (info)
    {
        if (info->spell == spell)
            return info;
        info = info->next;
    }
    info = (client_spell*)malloc(sizeof(client_spell));
    memset(info, 0, sizeof(client_spell));
    info->next = pl->spell_state;
    info->spell = spell;
    pl->spell_state = info;
    return info;
}
