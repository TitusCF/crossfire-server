/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001-2010 Crossfire Development Team
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

    The authors can be reached via e-mail at crossfire@metalforge.org
*/

/**
@file
Quest-related low-level mechanisms.

You should only need to call the public functions, all that are not static.

Data is loaded on a need-only basis - when a player quest state is queried or modified, data is read.
Also, free_quest() can be called to release memory without preventing quest operations after.

Write is done for each player whenever the state changes, to ensure data integrity.

@todo
- add protocol commands to send notifications instead of using draw_ext_info()

*/

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/** Completed quest status. */
#define QC_COMPLETED -1

/** Information about a quest for a player. */
typedef struct quest_state {
    sstring code;               /**< Quest internal code. */
    sstring title;              /**< Player-readable title. */
    sstring description;        /**< Short quest description. */
    int state;                  /**< State for the player. */
    sstring state_description;  /**< State description for the player. */
    struct quest_state *next;   /**< Next quest on the list. */
} quest_state;

/** Information about a player. */
typedef struct quest_player {
    sstring player_name;        /**< Player's name. */
    struct quest_state *quests; /**< Quests done or in progress. */
    struct quest_player *next;  /**< Next player on the list. */
} quest_player;

/** Player quest state. */
static quest_player *player_states = NULL;

/**
 * Return a new quest_state*, calling fatal() if memory shortage.
 * @return new value, never NULL.
 */
static quest_state *get_new_quest_state() {
    quest_state *qs = calloc(1, sizeof(quest_state));
    if (qs == NULL)
        fatal(OUT_OF_MEMORY);
    return qs;
}

/**
 * Read quest-data information for a player.
 * @param pq player to read data for.
 */
static void quest_read_player_data(quest_player *pq) {
    FILE *file;
    char final[MAX_BUF], read[MAX_BUF], data[MAX_BUF];
    StringBuffer *buf = NULL;
    quest_state *qs = NULL;
    int warned = 0, state, in = 0;

    snprintf(final, sizeof(final), "%s/%s/%s/%s.quest", settings.localdir, settings.playerdir, pq->player_name, pq->player_name);

    file = fopen(final, "r");
    if (!file) {
        /* no quest yet, no big deal */
        return;
    }

    while (fgets(read, sizeof(read), file) != NULL) {
        if (in == 1) {
            if (strcmp(read, "end_description\n") == 0) {
                char *message;

                in = 0;

                message = stringbuffer_finish(buf);
                buf = NULL;

                /* we introduced a newline when saving, so remove it now */
                if (strlen(message) > 0 && message[strlen(message) - 1] == '\n')
                    message[strlen(message) - 1] = '\0';

                qs->description = add_string(message);
                free(message);

                continue;
            }
            stringbuffer_append_string(buf, read);
            continue;
        }

        if (in == 2) {
            if (strcmp(read, "end_state_description\n") == 0) {
                char *message;

                in = 0;

                message = stringbuffer_finish(buf);
                buf = NULL;

                /* we introduced a newline when saving, so remove it now */
                if (strlen(message) > 0 && message[strlen(message) - 1] == '\n')
                    message[strlen(message) - 1] = '\0';
                qs->state_description = add_string(message);
                free(message);

                continue;
            }
            stringbuffer_append_string(buf, read);
            continue;
        }

        if (sscanf(read, "quest %s\n", data)) {
            qs = get_new_quest_state();
            qs->code = add_string(data);
            qs->next = pq->quests;
            pq->quests = qs;
            continue;
        }

        if (!qs) {
            if (!warned)
                LOG(llevError, "quest: invalid file format for %s\n", final);
            warned = 1;
            continue;
        }

        if (strncmp(read, "title ", 6) == 0) {
            read[strlen(read) - 1] = '\0';
            qs->title = add_string(read + 6);
            continue;
        }
        if (strcmp(read, "description\n") == 0) {
            in = 1;
            buf = stringbuffer_new();
            continue;
        }
        if (strcmp(read, "state_description\n") == 0) {
            in = 2;
            buf = stringbuffer_new();
            continue;
        }
        if (sscanf(read, "state %d\n", &state)) {
            qs->state = state;
            continue;
        }
        if (strcmp(read, "end_quest\n") == 0) {
            qs = NULL;
            continue;
        }

        LOG(llevError, "quest: invalid line in %s: %s\n", final, read);
    }

    if (in == 1)
        LOG(llevError, "quest: missing end_description in %s\n", final);
    else if (in == 2)
        LOG(llevError, "quest: missing end_state_description in %s\n", final);
    else if (qs)
        LOG(llevError, "quest: missing end_quest in %s\n", final);
    if (buf)
        free(stringbuffer_finish(buf));

    fclose(file);
}

/**
 * Write quest-data information for a player.
 * @param pq player to write data for.
 */
static void quest_write_player_data(const quest_player *pq) {
    FILE *file;
    char write[MAX_BUF], final[MAX_BUF];
    const quest_state *state;

    snprintf(final, sizeof(final), "%s/%s/%s/%s.quest", settings.localdir, settings.playerdir, pq->player_name, pq->player_name);
    snprintf(write, sizeof(write), "%s.new", final);

    file = fopen(write, "w+");
    if (!file) {
        LOG(llevError, "quest: couldn't open player quest file %s!", write);
        draw_ext_info(NDI_UNIQUE | NDI_ALL_DMS, 0, NULL, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOADSAVE, "File write error on server!", NULL);
        return;
    }

    state = pq->quests;

    while (state) {
        fprintf(file, "quest %s\n", state->code);
        fprintf(file, "title %s\n", state->title);
        fprintf(file, "description\n%s\nend_description\n", state->description);
        fprintf(file, "state %d\n", state->state);
        fprintf(file, "state_description\n%s\nend_state_description\n", state->state_description);
        fprintf(file, "end_quest\n");
        state = state->next;
    }

    fclose(file);
    /** @todo rename/backup, stuff like that */
    unlink(final);
    rename(write, final);
}

/**
 * Get the state of a quest for a player, not creating if not existing yet.
 * @param pq player to get state for.
 * @param name quest to get state of.
 * @return NULL if quest isn't started yet for this player, else quest's state information.
 */
static quest_state *get_state(quest_player *pq, sstring name) {
    quest_state *qs = pq->quests;

    while (qs) {
        if (qs->code == name)
            return qs;
        qs = qs->next;
    }

    return NULL;
}

/**
 * Get the state of a quest for a player, creating it if not existing yet.
 * @param pq player to get state for.
 * @param name quest to get state of.
 * @return quest's state information, newly created if it wasn't done yet.
 */
static quest_state *get_or_create_state(quest_player *pq, sstring name) {
    quest_state *qs = get_state(pq, name);

    if (!qs) {
        qs = calloc(1, sizeof(quest_state));
        if (!qs)
            fatal(OUT_OF_MEMORY);
        qs->code = add_refcount(name);
        qs->next = pq->quests;
        pq->quests = qs;
    }

    return qs;
}

/**
 * Get quest status for a player, not creating it if it doesn't exist.
 * @param pl player to get information of.
 * @return quest status, NULL if not loaded/found yet.
 */
static quest_player *get_quest(player *pl) {
    quest_player *pq = player_states;

    while (pq) {
        if (pq->player_name == pl->ob->name)
            return pq;
        pq = pq->next;
    }

    return NULL;
}

/**
 * Get quest status for a player, creating it if it doesn't exist yet.
 * Calls fatal() if memory allocation error.
 * @param pl player to get information of.
 * @return quest status, never NULL.
 */
static quest_player *get_or_create_quest(player *pl) {
    quest_player *pq = get_quest(pl);

    if (!pq) {
        pq = calloc(1, sizeof(quest_player));
        if (!pq)
            fatal(OUT_OF_MEMORY);
        pq->player_name = add_refcount(pl->ob->name);
        pq->next = player_states;
        player_states = pq;
        quest_read_player_data(pq);
    }

    return pq;
}

/**
 * Set the state of a quest for a player.
 * @param pl player to set the state for.
 * @param quest_code quest internal code.
 * @param state new state for the quest, must be greater than 0 else forced to 100 and a warning is emitted.
 * @param state_description state description.
 * @param started if 1, quest must have been started first or a warning is emitted, else it doesn't matter.
 */
static void quest_set_state(player *pl, sstring quest_code, int state, sstring state_description, int started) {
    quest_player *pq = get_or_create_quest(pl);
    quest_state *qs = get_or_create_state(pq, quest_code);

    if (state <= 0) {
        LOG(llevDebug, "quest_set_player_state: warning: called with invalid state %d for quest %s, player %s", state, pl->ob->name, quest_code);
        state = 100;
    }

    if (started && qs->state == 0) {
        LOG(llevDebug, "quest_set_player_state: warning: called for player %s not having started quest %s\n", pl->ob->name, quest_code);
    }
    if (qs->state_description)
        FREE_AND_CLEAR_STR(qs->state_description);

    qs->state = state;
    qs->state_description = add_refcount(state_description);
    quest_write_player_data(pq);

    LOG(llevDebug, "quest_set_player_state %s %s %d\n", pl->ob->name, quest_code, state);

    draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "New objective for the quest '%s':", NULL, qs->title);
    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, qs->state_description, NULL);
}

/**
 * Utility function to display a quest list. Will show a header before the list if not empty.
 * @param pl player to display list of quests.
 * @param pq quests to display.
 * @param completed if 0, only shows quests in progress, else shows quests completed only.
 */
static void quest_display(player *pl, quest_player *pq, int completed) {
    quest_state *state;
    int header = 0, count = 1;

    state = pq->quests;
    while (state) {
        if ((completed && state->state == QC_COMPLETED) || (!completed && state->state != QC_COMPLETED)) {
            if (!header) {
                if (completed)
                    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "You completed the following quests:", NULL);
                else
                    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "You started the following quests:", NULL);

                header = 1;
            }

            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "(%3d) %s", NULL, count, state->title);
        }

        state = state->next;
        count++;
    }
}

/**
 * Display current and completed player quests.
 * @param pl player to display for.
 */
static void quest_list(player *pl) {
    quest_player *pq;
    int started = 0, completed = 0;

    /* ensure we load data if not loaded yet */
    pq = get_or_create_quest(pl);
    if (!pq->quests) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "You didn't start any quest.", NULL);
        return;
    }

    quest_display(pl, pq, 1);
    quest_display(pl, pq, 0);
}

/**
 * Quest command help.
 * @param pl player to display help for.
 */
static void quest_help(player *pl) {
    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Quest commands:", NULL);
    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, " - list: displays quests you started or completed", NULL);
    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, " - info: displays information about the specified (by number) quest", NULL);
}

/**
 * Give details about a quest.
 * @param pl player asking for details.
 * @param params quest number.
 */
static void quest_info(player *pl, const char *params) {
    int number = atoi(params);
    /* load if required */
    quest_player *pq = get_or_create_quest(pl);
    quest_state *qs;

    if (number <= 0 || !pq) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Invalid quest number", NULL);
        return;
    }

    qs = pq->quests;
    while (qs) {
        if (number == 1) {
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Quest: %s", NULL, qs->title);
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Description: %s", NULL, qs->description);
            if (qs->state == QC_COMPLETED)
                draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "This quest is completed.", NULL);
            else
                draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, " \nNext objective: %s", NULL, qs->state_description);
            return;
        }

        number--;
        qs = qs->next;
    }

    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Invalid quest number.", NULL);
}

/**
 * Free quests structures.
 * @param pq what to free.
 */
static void free_state(quest_player *pq) {
    quest_state *qs = pq->quests, *next;

    while (qs) {
        next = qs->next;
        free_string(qs->code);
        free_string(qs->title);
        free_string(qs->description);
        free_string(qs->state_description);
        free(qs);
        qs = next;
    }
    pq->quests = NULL;
}


/* public functions */

/**
 * Get the quest state for a player.
 * @param pl player.
 * @param quest_code internal quest code.
 * @return QC_COMPLETED if finished, 0 if not started, else quest-specific value.
 */
int quest_get_player_state(player *pl, sstring quest_code) {
    quest_player *q = get_or_create_quest(pl);
    quest_state *s = get_state(q, quest_code);

    if (!s)
        return 0;

    return s->state;
}

/**
 * Start a quest for a player. Will notify the player.
 * @param pl player.
 * @param quest_code internal quest code.
 * @param quest_title quest short description.
 * @param quest_description quest longer description.
 * @param state initial quest state, must be greater than 0 else forced to 100 and warning emitted.
 * @param state_description initial quest state description for player.
 */
void quest_start(player *pl, sstring quest_code, sstring quest_title, sstring quest_description, int state, sstring state_description) {
    quest_player *pq = get_or_create_quest(pl);
    quest_state *q = get_or_create_state(pq, quest_code);

    if (state <= 0) {
        state = 100;
        LOG(llevDebug, "quest_start: negative state %d for %s quest %s\n", state, pl->ob->name, quest_code);
    }

    if (q->state != 0) {
        LOG(llevDebug, "quest_start: warning: player %s has already started quest %s\n", pl->ob->name, quest_code);
    }
    if (q->title)
        FREE_AND_CLEAR_STR(q->title);
    if (q->description)
        FREE_AND_CLEAR_STR(q->description);

    q->title = add_refcount(quest_title);
    q->description = add_refcount(quest_description);

    draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "New quest started: %s", NULL, q->title);

    quest_set_state(pl, quest_code, state, state_description, 0);

    /* saving state will be done in quest_set_state(). */
}

/**
 * Complete a quest for a player. Will notify the player. Warnings will be emitted if the quest is not started yet,
 * OR if the quest status was never queried.
 * @param pl player.
 * @param quest_code quest internal code.
 * @todo remove warning if quest status not queried?
 */
void quest_end(player *pl, sstring quest_code) {
    quest_player *ps = get_quest(pl);
    quest_state *qs;
    if (!ps) {
        /* maybe this could happen, if a quest can be completed whatever its state (no call to quest_get_player_state()) */
        LOG(llevDebug, "quest_end: called for not loaded player %s, quest %s\n", pl->ob->name, quest_code);
        ps = get_or_create_quest(pl);
    }

    qs = get_state(ps, quest_code);
    if (!qs) {
        LOG(llevDebug, "quest_end: called for player %s having not started quest, quest %s\n", pl->ob->name, quest_code);
        qs = get_or_create_state(ps, quest_code);
    }

    qs->state = QC_COMPLETED;
    quest_write_player_data(ps);

    draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Quest %s completed.", NULL, qs->title);
}

/**
 * Set the state of a quest for a player.
 * @param pl player to set the state for.
 * @param quest_code quest internal code.
 * @param state new state for the quest, must be greater than 0 else forced to 100 and a warning is emitted.
 * @param state_description state description.
 */
void quest_set_player_state(player *pl, sstring quest_code, int state, sstring state_description) {
    quest_set_state(pl, quest_code, state, state_description, 1);
}

/**
 * Command handler for 'quest'.
 * @param op player asking for information, warning emitted if not a player.
 * @param params extra parameters for command.
 * @return 0
 */
int command_quest(object *op, char *params) {
    if (!op->contr) {
        LOG(llevError, "command_quest called for a non player!\n");
        return 0;
    }

    if (!params || *params == '\0') {
        quest_help(op->contr);
        return 0;
    }

    if (strcmp(params, "list") == 0) {
        quest_list(op->contr);
        return 0;
    }

    if (strncmp(params, "info ", 5) == 0) {
        quest_info(op->contr, params + 5);
        return 0;
    }

    quest_help(op->contr);
    return 0;
}

/**
 * Free all quest status structures. It is all right to call quest functions again after that.
 */
void free_quest() {
    quest_player *pq = player_states, *next;

    while (pq) {
        next = pq->next;
        free_state(pq);
        free_string(pq->player_name);
        pq = next;
    }
    player_states = NULL;
}
