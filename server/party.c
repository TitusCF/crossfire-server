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
 * Party-structure related functions.
 */

#include "global.h"
#include "sproto.h"


static partylist *firstparty = NULL; /**< Keeps track of first party in list */
static partylist *lastparty = NULL;  /**< Keeps track of last party in list */


/**
 * Forms the party struct for a party called 'params'. it is the responsibility
 * of the caller to ensure that the name is unique.
 * New item is placed on the party list.
 * @param op
 * party creator.
 * @param params
 * party name.
 * @return
 * new party.
 */
partylist *party_form(object *op, const char *params) {
    partylist *newparty;

    if (op->contr->party != NULL) {
        char buf[MAX_BUF];

        snprintf(buf, sizeof(buf), "%s leaves party %s.", op->name, op->contr->party->partyname);
        party_send_message(op, buf);
    }
    newparty = (partylist *)malloc(sizeof(partylist));
    newparty->partyname = strdup_local(params);
    newparty->total_exp = 0;
    newparty->kills = 0;
    newparty->passwd[0] = '\0';
    newparty->next = NULL;
    newparty->partyleader = strdup_local(op->name);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                         "You have formed party: %s",
                         "You have formed party: %s",
                         newparty->partyname);
    op->contr->party = newparty;

    if (lastparty) {
        lastparty->next = newparty;
        lastparty = lastparty->next;
    } else {
        firstparty = newparty;
        lastparty = firstparty;
    }

    return newparty;
}

/**
 * Find a party by name.
 *
 * @param partyname
 * the party's name to find
 * @return
 * the party or NULL if no such party exists
 */
partylist *party_find(const char *partyname) {
    partylist *party;

    for (party = firstparty; party; party = party->next) {
        if (strcmp(party->partyname, partyname) == 0)
            return party;
    }
    return NULL;
}

/**
 * Removes and frees a party. Removes all members from the party.
 *
 * @param party
 * the party to remove
 *
 * @todo clean/simplify the mess.
 */
void party_remove(partylist *party) {
    partylist *tmpparty;
    partylist *previousparty;
    partylist *nextparty;
    player *pl;

    if (firstparty == NULL) {
        LOG(llevError, "party_remove: I was asked to remove party %s, but no parties are defined\n",
            party->partyname);
        return;
    }
    for (pl = first_player; pl != NULL; pl = pl->next)
        if (pl->party == party)
            pl->party = NULL;

    /* special case-ism for parties at the beginning and end of the list */
    if (party == firstparty) {
        if (lastparty == party)
            lastparty = NULL;
        firstparty = firstparty->next;
        free(party->partyleader);
        free(party->partyname);
        free(party);
        return;
    } else if (party == lastparty) {
        for (tmpparty = firstparty; tmpparty->next != NULL; tmpparty = tmpparty->next) {
            if (tmpparty->next == party) {
                lastparty = tmpparty;
                free(party->partyleader);
                free(party->partyname);
                free(party);
                lastparty->next = NULL;
                return;
            }
        }
    }
    for (tmpparty = firstparty; tmpparty->next != NULL; tmpparty = tmpparty->next)
        if (tmpparty->next == party) {
            previousparty = tmpparty;
            nextparty = tmpparty->next->next;
            /* this should be safe, because we already dealt with the lastparty case */

            previousparty->next = nextparty;
            free(party->partyleader);
            free(party->partyname);
            free(party);
            return;
        }
}

/**
 * Returns the first party from the list of all parties.
 *
 * @return
 * the first party or NULL if no party exists
 */
partylist *party_get_first(void) {
    return firstparty;
}

/**
 * Returns the next party from the list of all parties.
 *
 * @param party
 * the party to use
 * @return
 * the next party or NULL if party is the last one in list
 */
partylist *party_get_next(const partylist *party) {
    return party->next;
}

/**
 * Remove unused parties (no players), this could be made to scale a lot better.
 */
void party_obsolete_parties(void) {
    int player_count;
    player *pl;
    partylist *party;
    partylist *next = NULL;

    if (!firstparty)
        return; /* we can't obsolete parties if there aren't any */
    for (party = firstparty; party != NULL; party = next) {
        next = party->next;
        player_count = 0;
        for (pl = first_player; pl != NULL; pl = pl->next)
            if (pl->party == party)
                player_count++;
        if (player_count == 0)
            party_remove(party);
    }
}

/**
 * Returns the party's password.
 *
 * @param party
 * the party to query
 * @return
 * the password or an empty string if the party has no password
 */
const char *party_get_password(const partylist *party) {
    return party->passwd;
}

/**
 * Sets a party's password.
 *
 * @param party
 * the party to change
 * @param password
 * the new password to set
 */
void party_set_password(partylist *party, const char *password) {
    snprintf(party->passwd, sizeof(party->passwd), "%s", password);
}

/**
 * Checks whether a given password matches the party's password.
 *
 * @param party
 * the party to check
 * @param password
 * the password to check for
 * @return
 * whether the password matches
 */
int party_confirm_password(const partylist *party, const char *password) {
    return strcmp(party->passwd, password) == 0;
}

/**
 * Send a message to all party members except the speaker.
 *
 * @param op
 * player talking.
 * @param msg
 * message to send.
 *
 * @todo
 * should be moved to player.c?
 */
void party_send_message(object *op, char *msg) {
    player *pl;

    for (pl = first_player; pl != NULL; pl = pl->next)
        if (pl->ob->contr->party == op->contr->party && pl->ob != op)
            draw_ext_info(NDI_WHITE, 0, pl->ob, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_PARTY,
                          msg, NULL);
}

/**
 * Returns the name of the party's leader.
 *
 * @param party
 * the party to query
 * @return
 * the leader's name
 */
const char *party_get_leader(const partylist *party) {
    return party->partyleader;
}

#ifdef PARTY_KILL_LOG
/**
 * Logs a kill for a party.
 *
 * @param party
 * party to log for.
 * @param killer
 * name of the killer.
 * @param dead
 * victim's name.
 * @param exp
 * how much experience was gained.
 */
void party_add_kill(partylist *party, const char *killer, const char *dead, long exp) {
    int i, pos;

    if (party->kills >= PARTY_KILL_LOG) {
        pos = PARTY_KILL_LOG-1;
        for (i = 0; i < PARTY_KILL_LOG-1; i++)
            memcpy(&(party->party_kills[i]), &(party->party_kills[i+1]), sizeof(party->party_kills[0]));
    } else
        pos = party->kills;
    party->kills++;
    party->total_exp += exp;
    party->party_kills[pos].exp = exp;
    strncpy(party->party_kills[pos].killer, killer, MAX_NAME);
    strncpy(party->party_kills[pos].dead, dead, MAX_NAME);
    party->party_kills[pos].killer[MAX_NAME] = 0;
    party->party_kills[pos].dead[MAX_NAME] = 0;
}
#endif
