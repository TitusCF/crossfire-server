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

/** Quest status that indicates a quest was completed and may be restarted. */
#define QC_CAN_RESTART -1

/** Information about a quest for a player. */
typedef struct quest_state {
    sstring code;               /**< Quest internal code. */
    int state;                  /**< State for the player. */
    int was_completed;          /**< Whether the quest was completed once or not, indepandently of the state. */
    int is_complete;            /**< Whether the quest is complete in the current playthrough */
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

/** One step of a quest. */
typedef struct quest_step_definition {
    int step;                           /**< Step identifier. */
    sstring step_description;           /**< Step description to show player. */
    int is_completion_step:1;           /**< Whether this step completes the quest (1) or not (0) */
    struct quest_step_definition *next; /**< Next step. */
} quest_step_definition;

/** Definition of an in-game quest. */
typedef struct quest_definition {
    sstring quest_code;             /**< Quest internal code. */
    sstring quest_title;            /**< Quest title for player. */
    sstring quest_description;      /**< Quest longer description. */
    int quest_restart;              /**< If non zero, can be restarted. */
    quest_step_definition *steps;   /**< Quest steps. */
    struct quest_definition *next;  /**< Next quest in the definition list. */
} quest_definition;

static int quests_loaded = 0;           /**< Did we already read the 'default.quests' file? */
static quest_definition *quests = NULL; /**< All known quests. */

/**
 * Allocate a quest_step_definition, will call fatal() if out of memory.
 * @return new structure.
 */
static quest_step_definition *quest_create_step(void) {
    quest_step_definition *step = calloc(1, sizeof(quest_step_definition));
    if (!step)
        fatal(OUT_OF_MEMORY);
    return step;
}

/**
 * Allocate a quest_definition, will call fatal() if out of memory.
 * @return new structure.
 */
static quest_definition *quest_create_definition(void) {
    quest_definition *quest = calloc(1, sizeof(quest_definition));
    if (!quest)
        fatal(OUT_OF_MEMORY);
    return quest;
}

/**
 * Find a quest from its code. This is called by quest_get and also
 * used in the quest loading code
 * @param code quest to search.
 * @return quest, or NULL if no such quest.
 */
static quest_definition *quest_get_by_code(sstring code) {
    quest_definition *quest;

    quest = quests;
    while (quest) {
        if (quest->quest_code == code)
            return quest;

        quest = quest->next;
    }
    return NULL;
}

/**
 * Loads all of the quests which are found in the given file, any global states
 * for quest loading are passed down into this function, but not back up again.
 * @param filename filename to load quests from.
 * @return number of quests loaded from file, negative value if there was an error.
 */
static int load_quests_from_file(char *filename) {
    int i, in = 0; /* 0: quest file, 1: one quest, 2: quest description, 3: quest step, 4: step description */
    quest_definition *quest = NULL;
    char includefile[MAX_BUF];
    quest_step_definition *step = NULL;
    char final[MAX_BUF], read[MAX_BUF];
    FILE *file;
    StringBuffer *buf;

    int loaded_quests =0, found =0;
    snprintf(final, sizeof(final), "%s/%s/%s", settings.datadir, settings.mapdir, filename);
    file = fopen(final, "r");
    if (!file) {
        LOG(llevError, "Can't open %s for reading quests", filename);
        return -1;
    }

    while (fgets(read, sizeof(read), file) != NULL) {
        if (in == 4) {
            if (strcmp(read, "end_description\n") == 0) {
                char *message;

                in = 3;

                message = stringbuffer_finish(buf);
                buf = NULL;

                step->step_description = add_string(message);
                free(message);

                continue;
            }

            stringbuffer_append_string(buf, read);
            continue;
        }

        if (in == 3) {
            if (strcmp(read, "end_step\n") == 0) {
                step = NULL;
                in = 1;
                continue;
            }
            if (strcmp(read, "finishes_quest\n") == 0) {
                step->is_completion_step=1;
                continue;
            }
            if (strcmp(read, "description\n") == 0) {
                buf = stringbuffer_new();
                in = 4;
                continue;
            }
            LOG(llevError, "quests: invalid line %s in definition of quest %s in file %s!\n",
                    read, quest->quest_code, filename);
            continue;
        }

        if (in == 2) {
            if (strcmp(read, "end_description\n") == 0) {
                char *message;

                in = 1;

                message = stringbuffer_finish(buf);
                buf = NULL;

                quest->quest_description = add_string(message);
                free(message);

                continue;
            }
            stringbuffer_append_string(buf, read);
            continue;
        }

        if (in == 1) {
            if (strcmp(read, "end_quest\n") == 0) {
                quest = NULL;
                in = 0;
                continue;
            }

            if (strcmp(read, "description\n") == 0) {
                in = 2;
                buf = stringbuffer_new();
                continue;
            }

            if (strncmp(read, "title ", 6) == 0) {
                read[strlen(read) - 1] = '\0';
                quest->quest_title = add_string(read + 6);
                continue;
            }

            if (sscanf(read, "step %d\n", &i)) {
                step = quest_create_step();
                step->step = i;
                step->next = quest->steps;
                quest->steps = step;
                in = 3;
                continue;
            }

            if (sscanf(read, "restart %d\n", &i)) {
                quest->quest_restart = i;
                continue;
            }
        }

        if (read[0] == '#')
            continue;

        if (strncmp(read, "quest ", 6) == 0) {
            quest = quest_create_definition();
            read[strlen(read) - 1] = '\0';
            quest->quest_code = add_string(read + 6);
            if (quest_get_by_code(quest->quest_code)) {
                LOG(llevError, "Quest %s is listed in file %s, but this quest has already been defined\n", quest->quest_code, filename);
            }
            quest->next = quests;
            quests = quest;
            in = 1;
            loaded_quests++;
            continue;
        }
        if (sscanf(read, "include %s\n", includefile)) {
            char inc_path[HUGE_BUF];
            path_combine_and_normalize(filename, includefile, inc_path, sizeof(inc_path));
            found = load_quests_from_file(inc_path);
            if (found >=0) {
                LOG(llevDebug, "loaded %d quests from file %s\n", found, inc_path);
                loaded_quests += found;
            } else {
                LOG(llevError, "Failed to load quests from file %s\n", inc_path);
            }
            continue;
        }

        if (strcmp(read, "\n") == 0)
            continue;

        LOG(llevError, "quest: invalid file format for %s, I don't know what to do with the line %s\n", final, read);
    }
    return loaded_quests;
}

/** Load all quest definitions. Can be called multiple times, will be ignored. */
static void quest_load_definitions(void) {
    int found = 0;
    if (quests_loaded)
        return;
    quests_loaded = 1;
    found = load_quests_from_file("world.quests");
    if (found >= 0) {
        LOG(llevInfo, "%d quests found.\n", found);
    } else {
        LOG(llevError, "Quest Loading Failed");
    }
}

/**
 * Get a step for the specified quest.
 * @param quest quest to consider.
 * @param step step to find.
 * @return step, or NULL if no such step in which case a llevError is emitted.
 */
static quest_step_definition *quest_get_step(quest_definition *quest, int step) {
    quest_step_definition *qsd = quest->steps;

    while (qsd) {
        if (qsd->step == step)
            return qsd;

        qsd = qsd->next;
    }

    LOG(llevError, "quest %s has no required step %d\n", quest->quest_code, step);
    return NULL;
}

/**
 * Find a quest from its code.
 * @param code quest to search.
 * @return quest, , or NULL if no such quest in which case a llevError is emitted.
 */
static quest_definition *quest_get(sstring code) {
    quest_definition *quest;

    quest_load_definitions();

    quest = quest_get_by_code(code);
    if (!quest) {
        LOG(llevError, "quest %s required but not found!\n", code);
        return NULL;
    }
    return quest;
}

/**
 * Return a new quest_state*, calling fatal() if memory shortage.
 * @return new value, never NULL.
 */
static quest_state *get_new_quest_state(void) {
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
    int warned = 0, state;

    snprintf(final, sizeof(final), "%s/%s/%s/%s.quest", settings.localdir, settings.playerdir, pq->player_name, pq->player_name);

    file = fopen(final, "r");
    if (!file) {
        /* no quest yet, no big deal */
        return;
    }

    while (fgets(read, sizeof(read), file) != NULL) {
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

        if (sscanf(read, "state %d\n", &state)) {
            qs->state = state;
            continue;
        }
        if (strcmp(read, "end_quest\n") == 0) {
            qs = NULL;
            continue;
        }
        if (sscanf(read, "completed %d\n", &state)) {
            qs->was_completed = state ? 1 : 0;
            continue;
        }

        LOG(llevError, "quest: invalid line in %s: %s\n", final, read);
    }

    if (qs)
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
        fprintf(file, "state %d\n", state->state);
        fprintf(file, "completed %d\n", state->was_completed);
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
 * @param started if 1, quest must have been started first or a warning is emitted, else it doesn't matter.
 */
static void quest_set_state(player *pl, sstring quest_code, int state, int started) {
    quest_player *pq = get_or_create_quest(pl);
    quest_state *qs = get_or_create_state(pq, quest_code);
    quest_definition *quest = quest_get(quest_code);
    quest_step_definition *step;

    if (!quest) {
        LOG(llevError, "quest: asking for set_state of unknown quest %s!\n", quest_code);
        return;
    }

    if (state <= 0) {
        LOG(llevDebug, "quest_set_player_state: warning: called with invalid state %d for quest %s, player %s", state, pl->ob->name, quest_code);
        state = 100;
    }

    step = quest_get_step(quest, state);
    if (!step) {
        LOG(llevError, "quest_set_player_state: couldn't find state definition %d for quest %s, player %s", state, quest_code, pl->ob->name);
        return;
    }

    if (started && qs->state == 0) {
        LOG(llevDebug, "quest_set_player_state: warning: called for player %s not having started quest %s\n", pl->ob->name, quest_code);
    }

    qs->state = state;
    if (step->is_completion_step) {
        /* don't send an update note if the quest was already completed, this is just to show the outcome afterwards. */
        if (!qs->is_complete)
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Quest %s completed.", NULL, quest->quest_title);
        qs->was_completed = 1;
        if (quest->quest_restart)
            qs->state = QC_CAN_RESTART;
        else
            qs->is_complete =1;

    } else {
        draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "New objective for the quest '%s':", NULL, quest->quest_title);
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, step->step_description, NULL);
    }
    quest_write_player_data(pq);

    LOG(llevDebug, "quest_set_player_state %s %s %d\n", pl->ob->name, quest_code, state);

}

/**
 * Utility function to display a quest list. Will show a header before the list if not empty.
 * @param pl player to display list of quests.
 * @param pq quests to display.
 * @param showall if 0, only shows quests in progress and a summary of completed quests, else shows all quests.
 */
static void quest_display(player *pl, quest_player *pq, int showall) {
    quest_state *state;
    quest_definition *quest;
    const char *restart;
    int completed_count = 0, restart_count = 0, total_count = 0, current_count = 0;

    state = pq->quests;
    while (state) {
        total_count++;
        /* count up the number of completed quests first */
        if (state->state == QC_CAN_RESTART) {
            restart_count++;
            completed_count++;
        } else if(state->is_complete) {
            completed_count++;
            }
        state = state->next;
    }
    if (completed_count > 0) {
        if (!showall) {
            if (restart_count > 0)
                draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                        "You have completed %d quests, of which %d may be restarted", NULL, completed_count, restart_count);
            else
                draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                        "You have completed %d quests", NULL, completed_count);
            current_count = completed_count;
        } else {
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                    "You have completed the following quests:", NULL);
            state = pq->quests;
            while (state) {
                if (state->state == QC_CAN_RESTART || state->is_complete) {
                    quest = quest_get(state->code);
                    restart = state->state == QC_CAN_RESTART?" (can be replayed)":"";
                    draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                            "(%3d) %s%s", NULL, ++current_count, quest->quest_title, restart);
        }
        state = state->next;
           }
        }
    }
    if (total_count > completed_count) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                "You have started the following quests:", NULL);
        state = pq->quests;
        while (state) {
            if (state->state != QC_CAN_RESTART && state->is_complete==0) {
                quest = quest_get(state->code);
                draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                        "(%3d) %s", NULL, ++current_count, quest->quest_title);
            }
            state = state->next;
        }
    }
}

/**
 * Display current and completed player quests.
 * @param pl player to display for.
 * @param showall - whether to show all of the quests in full, just summary information for the completed ones
 */
static void quest_list(player *pl, int showall) {
    quest_player *pq;

    /* ensure we load data if not loaded yet */
    pq = get_or_create_quest(pl);
    if (!pq->quests) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "You didn't start any quest.", NULL);
        return;
    }

    quest_display(pl, pq, showall);
}

/**
 * Quest command help.
 * @param pl player to display help for.
 */
static void quest_help(player *pl) {
    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Quest commands:", NULL);
    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, " - list: displays quests you are currently attempting add 'all' to show completed quests also", NULL);
    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, " - info: displays information about the specified (by number) quest", NULL);
}

/**
 * returns the quest state which corresponds to a certain number for the given player.
 * @param pl player asking for details.
 * @param number quest number.
 * @return quest state corresponding to the number provided, NULL if there is no such quest state.
 */

static quest_state *get_quest_by_number(player *pl, int number) {
    quest_state *state;
    quest_player *pq = get_or_create_quest(pl);
    int questnum = 0;

    if (number <= 0 || !pq) {
        return NULL;
    }
    /* count through completed quests first */
    state = pq->quests;
    while (state) {
            /* count up the number of completed quests first */
            if (state->state == QC_CAN_RESTART || state->is_complete)
                if (++questnum == number) return state;
            state = state->next;
        }
    /* then active ones */
    state = pq->quests;
    while (state) {
        /* count up the number of completed quests first */
        if (state->state != QC_CAN_RESTART && state->is_complete ==0)
            if (++questnum == number) return state;
        state = state->next;
    }
    /* Ok, we didn't find our quest, return NULL*/
    return NULL;
}

/**
 * Give details about a quest.
 * @param pl player to give quest details to.
 * @param qs quest_state to give details about
 */
static void quest_info(player *pl, quest_state *qs) {
    quest_player *pq = get_or_create_quest(pl);
    quest_definition *quest;
    quest_step_definition *step;
    const char *prefix;

    if (!qs) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Invalid quest number", NULL);
        return;
    }
            quest = quest_get(qs->code);
            if (!quest) {
                /* already warned by quest_get */
                draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Quest: (internal error)", NULL);
                return;
            }

            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Quest: %s", NULL, quest->quest_title);
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Description: %s", NULL, quest->quest_description);

    step = quest_get_step(quest, qs->state);
    if (qs->state == QC_CAN_RESTART || qs->is_complete) {
                const char *restart = "";
                if (quest->quest_restart)
                    restart = " (can be replayed)";
        draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "This quest has been completed%s.", NULL, restart);
    }
    prefix = "";
    if (qs->state != QC_CAN_RESTART) {
        /* ie, if we are in progress or completed for a non-restartable quest */
                if (!step) {
                    /* already warned by quest_get_step */
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, " \nOutcome: (invalid quest)", NULL);
                    return;
                }
        if (qs->is_complete)
            prefix = "Outcome";
        else
            prefix = "Current Status";
        draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, " \n%s: %s", NULL, prefix, step->step_description);
            }
            return;
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
 * @return QC_COMPLETED if finished and quest can't be replayed, 0 if not started or finished and can be replayed, else quest-specific value.
 */
int quest_get_player_state(player *pl, sstring quest_code) {
    quest_player *q = get_or_create_quest(pl);
    quest_state *s = get_state(q, quest_code);
    quest_definition *quest = quest_get(quest_code);

    if (!s)
        return 0;

    if (s->state == QC_CAN_RESTART && quest && quest->quest_restart)
        return 0;

    return s->state;
}

/**
 * Start a quest for a player. Will notify the player.
 * @param pl player.
 * @param quest_code internal quest code.
 * @param state initial quest state, must be greater than 0 else forced to 100 and warning emitted.
 */
void quest_start(player *pl, sstring quest_code, int state) {
    quest_player *pq = get_or_create_quest(pl);
    quest_state *q = get_or_create_state(pq, quest_code);
    quest_definition *quest = quest_get(quest_code);
    quest_step_definition *step;

    if (state <= 0) {
        state = 100;
        LOG(llevDebug, "quest_start: negative state %d for %s quest %s\n", state, pl->ob->name, quest_code);
    }

    step = quest_get_step(quest, state);

    /* if completed already, assume the player can redo it */
    if (q->state > 0) {
        LOG(llevDebug, "quest_start: warning: player %s has already started quest %s\n", pl->ob->name, quest_code);
    }

    draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "New quest started: %s", NULL, quest->quest_title);

    quest_set_state(pl, quest_code, state, 0);

    /* saving state will be done in quest_set_state(). */
}

/**
 * Set the state of a quest for a player.
 * @param pl player to set the state for.
 * @param quest_code quest internal code.
 * @param state new state for the quest, must be greater than 0 else forced to 100 and a warning is emitted.
 */
void quest_set_player_state(player *pl, sstring quest_code, int state) {
    quest_set_state(pl, quest_code, state, 1);
}

/**
 * Check if a quest was completed once for a player, without taking account the current state.
 * @param pl who to check for.
 * @param quest_code quest internal code.
 * @return 1 if the quest was already completed at least once, 0 else.
 */
int quest_was_completed(player *pl, sstring quest_code) {
    quest_player *qp = get_or_create_quest(pl);
    quest_state *state = get_state(qp, quest_code);

    return (state && state->was_completed);
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
    if (strcmp(params, "list all") == 0) {
        quest_list(op->contr, 1);
        return 0;
    }

    if (strcmp(params, "list") == 0) {
        quest_list(op->contr, 0);
        return 0;
    }

    if (strncmp(params, "info ", 5) == 0) {
        int number = atoi(params+5);
        quest_info(op->contr, get_quest_by_number(op->contr, number));
        return 0;
    }

    quest_help(op->contr);
    return 0;
}

/**
 * Dump all of the quests, then calls exit() - useful in terms of debugging to make sure that
 * quests are set up and recognised correctly.
 */
void dump_quests(void) {
    quest_definition *quest;
    quest_step_definition *step;
    quest_load_definitions();
    int questcount, stepcount;
    quest = quests;
    while (quest) {
        questcount++;
        stepcount=0;
        step = quest->steps;
        while (step) {
            stepcount++;
            step= step->next;
        }
        fprintf(logfile, "%s - %s - %d steps (%srestartable)\n", quest->quest_code, quest->quest_title, stepcount, quest->quest_restart?"":"not ");
        quest = quest->next;
    }
    exit(0);
}

/**
 * Free all quest status structures. It is all right to call quest functions again after that.
 */
void free_quest(void) {
    quest_player *pq = player_states, *next;

    while (pq) {
        next = pq->next;
        free_state(pq);
        free_string(pq->player_name);
        pq = next;
    }
    player_states = NULL;
}
