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
 * @file
 * Player-structure related functions.
 */

#include <global.h>

/**
 * Clears data in player structure.
 *
 * Socket isn't touched. Nor is anything that doesn't need to be freed. So you
 * may need to do a memset() to clear out values.
 *
 * @param pl
 * player to clear. Pointer is still valid, and can be reused for "play again".
 */
void clear_player(player *pl) {
    client_spell *info;
    client_spell *next;

    /* Clear item stack (used by DMs only) */
    free(pl->stack_items);
    pl->stack_position = 0;

    info = pl->spell_state;
    while (info) {
        next = info->next;
        free(info);
        info = next;
    }
}

/**
 * Clears player structure, including pointed object (through object_free()).
 *
 * @param pl
 * player to clear. Pointer is invalid after this call.
 */
void free_player(player *pl) {
    if (first_player != pl) {
        player *prev = first_player;

        while (prev != NULL && prev->next != NULL && prev->next != pl)
            prev = prev->next;
        if (prev->next != pl) {
            LOG(llevError, "Free_player: Can't find previous player.\n");
            exit(1);
        }
        prev->next = pl->next;
    } else
        first_player = pl->next;

    if (pl->ob != NULL) {
        if (!QUERY_FLAG(pl->ob, FLAG_REMOVED))
            object_remove(pl->ob);
        object_free(pl->ob);
    }

    clear_player(pl);

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
    if (attacknr == ATNR_MAGIC
    || attacknr == ATNR_FIRE
    || attacknr == ATNR_ELECTRICITY
    || attacknr == ATNR_COLD
    || attacknr == ATNR_ACID
    || attacknr == ATNR_POISON)
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
int is_dragon_pl(const object *op) {
    if (op != NULL
    && op->type == PLAYER
    && op->arch != NULL
    && op->arch->clone.race != NULL
    && strcmp(op->arch->clone.race, "dragon") == 0)
        return 1;
    return 0;
}

/**
 * Gets the (client-side) spell state for specified spell. Will be created to empty state if not found.
 *
 * @note
 * will fatal() in case of memory allocation failure.
 * @param pl
 * player we're handling.
 * @param spell
 * spell for which to search data.
 * @return
 * state information for that spell.
 */
client_spell *get_client_spell_state(player *pl, object *spell) {
    client_spell *info = pl->spell_state;

    while (info) {
        if (info->spell == spell)
            return info;
        info = info->next;
    }
    info = (client_spell *)malloc(sizeof(client_spell));
    if (info == NULL)
        fatal(OUT_OF_MEMORY);
    memset(info, 0, sizeof(client_spell));
    info->next = pl->spell_state;
    info->spell = spell;
    pl->spell_state = info;
    return info;
}

/**
 * Tests if a player is a wraith.
 *
 * @param op
 * player to check.
 * @return
 * true if the adressed object 'ob' is a wraith player, false else.
 */
int is_wraith_pl(object *op) {
    return op != NULL && op->type == PLAYER && op->arch != NULL && object_find_by_name(op, "wraith feed") != NULL;
}

/**
 * Checks if player is a wraith without the 'wraith feed' skill.
 *
 * @param op
 * player to check.
 * @return
 * true if the adressed object 'ob' is an old wraith player, false else.
 */
int is_old_wraith_pl(object *op) {
    return op != NULL && op->type == PLAYER && op->arch != NULL && object_find_by_name(op, "Wraith_Force") != NULL && !is_wraith_pl(op);
}

/**
 * Updates the title of a dragon player to reflect the current level, attack
 * type, and resistances.
 *
 * @param pl
 * the player to update
 * @param level
 * the dragon's current level
 * @param attack
 * the dragon's current attack focus
 * @param skin_resist
 * the dragon's skin resistance for attack
 */
void player_set_dragon_title(struct pl *pl, int level, const char *attack, int skin_resist) {
    if (level == 0)
        snprintf(pl->title, sizeof(pl->title), "%s hatchling", attack);
    else if (level == 1)
        snprintf(pl->title, sizeof(pl->title), "%s wyrm", attack);
    else if (level == 2)
        snprintf(pl->title, sizeof(pl->title), "%s wyvern", attack);
    else if (level == 3)
        snprintf(pl->title, sizeof(pl->title), "%s dragon", attack);
    /* special titles for extra high resistance! */
    else if (skin_resist > 80)
        snprintf(pl->title, sizeof(pl->title), "legendary %s dragon", attack);
    else if (skin_resist > 50)
        snprintf(pl->title, sizeof(pl->title), "ancient %s dragon", attack);
    else
        snprintf(pl->title, sizeof(pl->title), "big %s dragon", attack);
    pl->own_title[0] = '\0';
}

/**
 * Returns the player's title. The returned title is never empty and includes a
 * "the" prefix if necessary.
 *
 * @param pl
 * the player to return the title of
 * @param buf
 * returns the title
 * @param bufsize
 * the size of buf in byte
 */
void player_get_title(const struct pl *pl, char *buf, size_t bufsize) {
    if (pl->own_title[0] == '\0')
        snprintf(buf, bufsize, "the %s", pl->title);
    else
        snprintf(buf, bufsize, "%s", pl->own_title);
}

/**
 * Returns whether the player has a custom title.
 *
 * @param pl
 * the player to check
 * @return
 * whether the player has a custom title
 */
int player_has_own_title(const struct pl *pl) {
    return pl->own_title[0] != '\0';
}

/**
 * Returns the player's own title. The returned value must not be modified and
 * points into the player structure.
 *
 * @param pl
 * the player
 * @return
 * the own title
 */
const char *player_get_own_title(const struct pl *pl) {
    return pl->own_title;
}

/**
 * Sets the custom title.
 *
 * @param pl
 * the player to modify
 * @param title
 * the new title to set; empty string to unset
 */
void player_set_own_title(struct pl *pl, const char *title) {
    snprintf(pl->own_title, sizeof(pl->own_title), "%s", title);
}
