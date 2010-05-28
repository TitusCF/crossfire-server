/*
 * static char *rcsid_c_party_c =
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
 * Party-related functions and variables.
 */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>

/**
 * Is the password the player entered to join a party the right one?
 *
 * @param op
 * player. Must have party_to_join correctly set.
 * @retval 0
 * password is correct.
 * @retval 1
 * invalid password or party not found.
 */
int confirm_party_password(object *op) {
    const partylist *party;

    party = party_find(op->contr->party_to_join->partyname);
    return party == NULL || !party_confirm_password(party, op->contr->write_buf+1);
}

/**
 * Player entered a party password.
 *
 * @param op
 * player.
 */
void receive_party_password(object *op) {
    if (confirm_party_password(op) == 0) {
        party_join(op, op->contr->party_to_join);
        op->contr->party_to_join = NULL;
        op->contr->state = ST_PLAYING;
        return;
    }

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                  "You entered the wrong password");
    op->contr->party_to_join = NULL;
    op->contr->state = ST_PLAYING;
}

/**
 * 'gsay' command, talks to party.
 *
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_gsay(object *op, char *params) {
    char party_params[MAX_BUF];

    if (*params == '\0') {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR, "Say what?");
        return 0;
    }
    strcpy(party_params, "say ");
    strcat(party_params, params);
    command_party(op, party_params);
    return 0;
}

/**
 * Give help for party commands.
 *
 * @param op
 * player.
 */
static void party_help(object *op) {
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_HELP,
                  "To form a party type: party form <partyname>. "
                  "To join a party type: party join <partyname> "
                  "If the party has a passwd, it will you prompt you for it. "
                  "For a list of current parties type: party list. "
                  "To leave a party type: party leave "
                  "To change a passwd for a party type: party passwd <password> "
                  "There is an 8 character maximum password length. "
                  "To talk to party members type: party say <msg> "
                  "To see who is in your party: party who "
#ifdef PARTY_KILL_LOG
                  "To see what you've killed, type: party kills"
#endif
                 );
}

/**
 * 'party' command, subdivided in different sub commands.
 *
 * @param op
 * player.
 * @param params
 * additional parameters.
 * 1.
 * @todo split in different functions. clean the 'form' mess.
 */
int command_party(object *op, char *params) {
    char buf[MAX_BUF];

    if (*params == '\0') {
        if (op->contr->party == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "You are not a member of any party. "
                          "For help try: party help");
        } else {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                                 "You are a member of party %s.",
                                 op->contr->party->partyname);
        }
        return 1;
    }
    if (strcmp(params, "help") == 0) {
        party_help(op);
        return 1;
    }
#ifdef PARTY_KILL_LOG
    if (!strncmp(params, "kills", 5)) {
        int i, max;
        char chr;
        char buffer[80];
        float exp;

        if (op->contr->party == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "You are not a member of any party.");
            return 1;
        }
        tmpparty = op->contr->party;
        if (!tmpparty->kills) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                          "You haven't killed anything yet.");
            return 1;
        }
        max = tmpparty->kills-1;
        if (max > PARTY_KILL_LOG-1)
            max = PARTY_KILL_LOG-1;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "[fixed]Killed          |          Killer|     Exp\n----------------+----------------+--------"
                      "Killed          |          Killer|     Exp\n----------------+----------------+--------");


        for (i = 0; i <= max; i++) {
            exp = tmpparty->party_kills[i].exp;
            chr = ' ';
            if (exp > 1000000) {
                exp /= 1000000;
                chr = 'M';
            } else if (exp > 1000) {
                exp /= 1000;
                chr = 'k';
            }

            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                                 "[fixed]%16s|%16s|%6.1f%c",
                                 tmpparty->party_kills[i].dead,
                                 tmpparty->party_kills[i].killer, exp, chr);
        }
        exp = tmpparty->total_exp;
        chr = ' ';
        if (exp > 1000000) {
            exp /= 1000000;
            chr = 'M';
        } else if (exp > 1000) {
            exp /= 1000;
            chr = 'k';
        }

        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "[fixed]----------------+----------------+--------");
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                             "Totals: %d kills, %.1f%c exp", tmpparty->kills,
                             exp, chr);
        return 1;
    }
#endif /* PARTY_KILL_LOG */
    if (strncmp(params, "say ", 4) == 0) {
        if (op->contr->party == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "You are not a member of any party.");
            return 1;
        }
        params += 4;
        snprintf(buf, MAX_BUF-1, "<%s> %s says: %s", op->contr->party->partyname, op->name, params);
        party_send_message(op, buf);
        draw_ext_info_format(NDI_WHITE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_PARTY,
                             "<%s> You say: %s",
                             op->contr->party->partyname, params);
        return 1;
    }

    if (strncmp(params, "form ", 5) == 0) {
        params += 5;

        if (party_form(op, params) == NULL) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                                 "The party %s already exists, pick another name",
                                 params);
            return 1;
        }
        return 0;
    } /* form */

    if (strcmp(params, "leave") == 0) {
        if (op->contr->party == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "You are not a member of any party.");
            return 1;
        }
        party_leave(op);
        return 1;
    }
    if (strcmp(params, "who") == 0) {
        if (op->contr->party == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "You are not a member of any party.");
            return 1;
        }
        list_players(op, NULL, op->contr->party);
        return 1;
    } /* leave */

    if (strncmp(params, "passwd ", 7) == 0) {
        params += 7;

        if (op->contr->party == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "You are not a member of a party");
            return 1;
        }

        if (strlen(params) > 8) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "The password must not exceed 8 characters");
            return 1;
        }

        party_set_password(op->contr->party, params);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                             "The password for party %s is %s",
                             op->contr->party->partyname, party_get_password(op->contr->party));

        snprintf(buf, MAX_BUF, "Password for party %s is now %s, changed by %s",
                 op->contr->party->partyname, party_get_password(op->contr->party), op->name);
        party_send_message(op, buf);
        return 0;
    } /* passwd */

    if (strcmp(params, "list") == 0) {
        partylist *party;

        if (party_get_first() == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "There are no parties active right now");
            return 1;
        }

        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "[fixed]Party name                       Leader\n----------                       ------");
        for (party = party_get_first(); party != NULL; party = party_get_next(party)) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                                 "[fixed]%-32s %s",
                                 party->partyname, party_get_leader(party));
        }

        return 0;
    } /* list */

    if (strncmp(params, "join ", 5) == 0) {
        partylist *party;

        params += 5;

        party = party_find(params);
        if (party == NULL) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                                 "Party %s does not exist.  You must form it first.",
                                 params);
            return 1;
        }

        if (op->contr->party == party) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                                 "You are already a member of party: %s",
                                 party->partyname);
            return 1;
        }

        if (get_party_password(op, party)) {
            return 0;
        }

        party_join(op, party);
        return 0;
    } /* join */

    party_help(op);
    return 1;
}

/** Valid modes for 'party_rejoin'. @todo document that */
static const char *rejoin_modes[] = {
    "no",
    "if_exists",
    "always",
    NULL
};

/**
 * Handles the 'party_rejoin' command.
 * @param op
 * player.
 * @param params
 * optional parameters.
 * @return
 * 1.
 */
int command_party_rejoin(object *op, char *params) {
    int mode;

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                             "party rejoin: %s", rejoin_modes[op->contr->rejoin_party]);
        return 1;
    }
    for (mode = 0; rejoin_modes[mode] != NULL; mode++) {
        if (strcmp(rejoin_modes[mode], params) == 0) {
            op->contr->rejoin_party = mode;
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                                 "party rejoin is now: %s", rejoin_modes[op->contr->rejoin_party]);
            return 1;
        }
    }
    if (strlen(params) > 50)
        params[50] = '\0';
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                         "invalid mode: %s", params);
    return 1;
}
