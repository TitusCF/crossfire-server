/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * Quest-related low-level mechanisms.
 *
 * You should only need to call the public functions, all that are not static.
 *
 * Data is loaded on a need-only basis - when a player quest state is queried
 * or modified, data is read. Also, free_quest() can be called to release
 * memory without preventing quest operations after.
 *
 * Write is done for each player whenever the state changes, to ensure data
 * integrity.
 */

#include "global.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "output_file.h"
#include "sproto.h"

#include "quest.h"
#include "assets.h"

/* Media tags for information messages prefix. */
#define TAG_START   "[color=#aa55ff]"
#define TAG_END     "[/color]"

/** Quest status that indicates a quest was completed and may be restarted. */
#define QC_CAN_RESTART -1

/** Information about a quest for a player. */
typedef struct quest_state {
    sstring code;               /**< Quest internal code. */
    int state;                  /**< State for the player. */
    int was_completed;          /**< Whether the quest was completed once or not, indepandently of the state. */
    int is_complete;            /**< Whether the quest is complete in the current playthrough */
    int sent_to_client;         /**< Whether this state was sent to the client or not. */
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
    quest_state *qs = NULL, *prev = NULL;
    int warned = 0, state;
    quest_definition *quest = NULL;

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
            quest = quest_get_by_code(qs->code);
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
            if (quest != NULL && state != -1) {
                quest_step_definition *step = quest_get_step(quest, state);
                if (step == NULL) {
                    LOG(llevError, "invalid quest step %d for %s in %s\n", state, quest->quest_code, final);
                }
                else if (step->is_completion_step)
                    qs->is_complete = 1;
            }
            continue;
        }
        if (strcmp(read, "end_quest\n") == 0) {
            if (quest == NULL) {
                LOG(llevDebug, "Unknown quest %s in quest file %s\n", qs->code, final);
                free(qs);
            } else {
                if (prev == NULL) {
                    pq->quests = qs;
                } else {
                    prev->next = qs;
                }
                prev = qs;
            }
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

    fclose(file);
}

/**
 * Write quest-data information for a player.
 * @param pq player to write data for.
 */
static void quest_write_player_data(const quest_player *pq) {
    FILE *file;
    OutputFile of;
    char fname[MAX_BUF];
    const quest_state *state;

    snprintf(fname, sizeof(fname), "%s/%s/%s/%s.quest", settings.localdir, settings.playerdir, pq->player_name, pq->player_name);

    file = of_open(&of, fname);
    if (file == NULL) {
        draw_ext_info(NDI_UNIQUE | NDI_ALL_DMS, 0, NULL, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOADSAVE, "File write error on server!");
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

    if (!of_close(&of)) {
        draw_ext_info(NDI_UNIQUE | NDI_ALL_DMS, 0, NULL, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOADSAVE, "File write error on server!");
        return;
    }
    /** @todo rename/backup, stuff like that */
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
        if (pq->quests != NULL) {
            quest_state *last;
            for (last = pq->quests ; last->next != NULL; last = last->next)
                ;
            last->next = qs;
        } else {
            pq->quests = qs;
        }
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

/* quest_set_state can call itself through the function update_quests, so it needs to be declared here */
static void quest_set_state(player* dm, player *pl, sstring quest_code, int state, int started);

/**
 * Checks whether the conditions for a given step are met.
 * @param condition the linked list of conditions to check.
 * @param pl the player to evaluate conditions for.
 * @return 1 if the conditions match, 0 if they don't.
 */
static int evaluate_quest_conditions(const quest_condition *condition, player *pl) {
    const quest_condition *cond;
    int current_step;

    if (!condition)
        return 0;
    cond = condition;
    while (cond) {
        current_step = quest_get_player_state(pl, cond->quest_code);
        if (cond->minstep < 0 && cond->maxstep < 0) {
            /* we are checking for the quest to have been completed. */
            if (!quest_was_completed(pl, cond->quest_code))
                return 0;
        } else {
            if (current_step < cond->minstep || current_step > cond->maxstep)
                return 0;
        }
        cond = cond->next;
    }
    return 1;
}

static void do_update(const quest_definition *quest, void *user) {
    player *pl = (player *)user;
    const quest_step_definition *step;
    int new_step = 0;
    step = quest->steps;
    while (step) {
        if (step->conditions)
            if (evaluate_quest_conditions(step->conditions, pl)) {
                new_step=new_step<step->step?step->step:new_step;
            }
        step = step->next;
    }
    if (new_step > 0) {
        int current_step = quest_get_player_state(pl, quest->quest_code);
        if (new_step > current_step) {
            quest_set_state(NULL, pl, quest->quest_code, new_step, 0);
        }
    }
}

/**
 * Look through all of the quests for the given player, and see if any need to be updated.
 * @param pl
 */
static void update_quests(player *pl) {
    quest_for_each(do_update, pl);
}

/**
 * Set the state of a quest for a player.
 * @param dm if NULL then the player is actually playing, else a DM is changing the quest's state manually.
 * @param pl player to set the state for.
 * @param quest_code quest internal code.
 * @param state new state for the quest, must be greater than 0 else forced to 100 and a warning is emitted.
 * @param started if 1, quest must have been started first or a warning is emitted, else it doesn't matter.
 */
static void quest_set_state(player* dm, player *pl, sstring quest_code, int state, int started) {
    quest_player *pq = get_or_create_quest(pl);
    quest_state *qs = get_or_create_state(pq, quest_code);
    quest_definition *quest = quest_find_by_code(quest_code);
    quest_step_definition *step;

    if (!quest) {
        if (dm) {
            draw_ext_info_format(NDI_UNIQUE, 0, dm->ob, MSG_TYPE_ADMIN_DM, MSG_TYPE_COMMAND_FAILURE, "Unknown quest %s!", quest_code);
        } else {
            LOG(llevError, "quest: asking for set_state of unknown quest %s!\n", quest_code);
        }
        return;
    }

    if (!dm && state <= 0) {
        LOG(llevDebug, "quest_set_player_state: warning: called with invalid state %d for quest %s, player %s\n", state, pl->ob->name, quest_code);
        state = 100;
    }

    if (started && qs->state == 0) {
        if (!dm) {
            LOG(llevDebug, "quest_set_player_state: warning: called for player %s not having started quest %s\n", pl->ob->name, quest_code);
        }
    }

    qs->state = state;
    if (state == 0) {
        qs->is_complete = 0;
        return;
    }

    step = quest_get_step(quest, state);
    if (!step) {
        if (dm) {
            draw_ext_info_format(NDI_UNIQUE, 0, dm->ob, MSG_TYPE_ADMIN_DM, MSG_TYPE_COMMAND_FAILURE, "Couldn't find state definition %d for quest %s", state, quest_code);
        } else {
            LOG(llevError, "quest_set_player_state: couldn't find state definition %d for quest %s, player %s\n", state, quest_code, pl->ob->name);
        }
        return;
    }

    if (step->is_completion_step) {
        /* don't send an update note if the quest was already completed, this is just to show the outcome afterwards. */
        if (!qs->is_complete)
            draw_ext_info_format(NDI_UNIQUE|NDI_DELAYED, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, "Quest %s completed.", quest->quest_title);
        qs->was_completed = 1;
        if (quest->quest_restart)
            qs->state = QC_CAN_RESTART;
        else
            qs->is_complete =1;

    } else {
        draw_ext_info_format(NDI_UNIQUE|NDI_DELAYED, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, "New objective for the quest '%s':", quest->quest_title);
        draw_ext_info(NDI_UNIQUE|NDI_DELAYED, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, step->step_description);
    }

    if (pl->socket.notifications > 0) {
        SockList *sl = player_get_delayed_buffer(pl);

        if (qs->sent_to_client) {
            SockList_AddString(sl, "updquest ");
        } else {
            SockList_AddString(sl, "addquest ");
        }

        SockList_AddInt(sl, quest->client_code);
        if (qs->sent_to_client == 0) {
            SockList_AddLen16Data(sl, quest->quest_title, strlen(quest->quest_title));
            if (quest->face && !(pl->socket.faces_sent[quest->face->number]&NS_FACESENT_FACE))
                esrv_send_face(&pl->socket, quest->face, 0);
            SockList_AddInt(sl, quest->face ? quest->face->number : 0);
            SockList_AddChar(sl, quest->quest_restart ? 1 : 0);
            SockList_AddInt(sl, quest->parent ? quest->parent->client_code : 0);
        }

        SockList_AddChar(sl, (step == NULL || step->is_completion_step) ? 1 : 0);
        assert(step != NULL);
        SockList_AddLen16Data(sl, step->step_description,
                strlen(step->step_description));

        qs->sent_to_client = 1;
    }

    if (pl->has_directory)
        quest_write_player_data(pq);
    update_quests(pl);
    LOG(llevDebug, "quest_set_player_state %s %s %d\n", pl->ob->name, quest_code, state);

}

/**
 * Utility function to display a quest list. Will show a header before the list if not empty.
 * @param pl player to display list of quests.
 * @param pq quests to display.
 * @param showall if 0, only shows quests in progress and a summary of completed quests, else shows all quests.
 * @param name either 'You' or the player's name, if pl is a DM asking about another player.
 */
static void quest_display(player *pl, quest_player *pq, int showall, const char* name) {
    quest_state *state;
    quest_definition *quest;
    const char *restart;
    int completed_count = 0, restart_count = 0, total_count = 0, current_count = 0;

    state = pq->quests;
    while (state) {
        quest = quest_find_by_code(state->code);
        if (quest->parent == NULL) {
            total_count++;
            /* count up the number of completed quests first */
            if (state->state == QC_CAN_RESTART) {
                restart_count++;
                completed_count++;
            } else if(state->is_complete) {
                completed_count++;
            }
        }
        state = state->next;
    }
    if (completed_count > 0) {
        if (!showall) {
            if (restart_count > 0)
                draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
                        "%s completed %d out of %d quests, of which %d may be restarted.", name, completed_count, quests_count(false), restart_count);
            else
                draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
                        "%s completed %d quests", name, completed_count);
            current_count = completed_count;
        } else {
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
                    "%s completed the following quests:", name);
            state = pq->quests;
            while (state) {
                quest = quest_find_by_code(state->code);
                if (quest->parent == NULL) {
                    if (state->state == QC_CAN_RESTART || state->is_complete) {

                        restart = state->state == QC_CAN_RESTART ? i18n(pl->ob, " (can be replayed)") : "";
                        draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
                            "(%3d) %s%s", ++current_count, quest->quest_title, restart);
                    }
                }
                state = state->next;
           }
        }
    }
    if (total_count > completed_count) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
                "%s started the following quests:", name);
        state = pq->quests;
        while (state) {
            quest = quest_find_by_code(state->code);
            if (quest->parent == NULL) {
                if (state->state != QC_CAN_RESTART && state->is_complete==0) {
                    quest = quest_find_by_code(state->code);
                    draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
                        "(%3d) %s", ++current_count, quest->quest_title);
                }
            }
            state = state->next;
        }
    }
}

/**
 * Display current and completed player quests.
 * @param pl player to display to.
 * @param who player to display information for.
 * @param showall - whether to show all of the quests in full, just summary information for the completed ones
 * @param name either 'You' or the player's name, if pl is a DM asking about another player.
 */
static void quest_list(player *pl, player *who, int showall, const char* name) {
    quest_player *pq;

    /* ensure we load data if not loaded yet */
    pq = get_or_create_quest(who);
    if (!pq->quests) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, "%s didn't start any quest.", name);
        return;
    }

    quest_display(pl, pq, showall, name);
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
            if (!(quest_find_by_code(state->code)->parent) && (state->state == QC_CAN_RESTART || state->is_complete))
                if (++questnum == number) return state;
            state = state->next;
        }
    /* then active ones */
    state = pq->quests;
    while (state) {
        /* count up the number of completed quests first */
        if (!(quest_find_by_code(state->code)->parent) && state->state != QC_CAN_RESTART && state->is_complete ==0)
            if (++questnum == number) return state;
        state = state->next;
    }
    /* Ok, we didn't find our quest, return NULL*/
    return NULL;
}

/**
 * Give details about a quest.
 * @param pl player to give quest details to.
 * @param who player to give quest details of.
 * @param qs quest_state to give details about
 * @param level The level of recursion for the quest info that's being provided
 */
static void quest_info(player *pl, player* who, quest_state *qs, int level) {
    quest_definition *quest, *child;
    quest_state *state;
    quest_player *pq = get_or_create_quest(who);
    quest_step_definition *step;
    const char *prefix;

    if (!qs) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, "Invalid quest number");
        return;
    }
    quest = quest_find_by_code(qs->code);
    if (!quest) {
        /* already warned by quest_get */
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, "Quest: (internal error)");
        return;
    }

    draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, TAG_START "Quest:" TAG_END " %s", quest->quest_title);
    if (QUERY_FLAG(pl->ob, FLAG_WIZ)) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, TAG_START "Code:" TAG_END " %s", quest->quest_code);
        for (step = quest->steps; step != NULL; step = step->next) {
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, " " TAG_START "Step:" TAG_END " %d (%s)", step->step, step->step_description);
        }
    }
    draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, TAG_START "Description:" TAG_END " %s", quest->quest_description);

    step = quest_get_step(quest, qs->state);
    if (qs->state == QC_CAN_RESTART || qs->is_complete) {
        const char *restart = "";
        if (quest->quest_restart)
            restart = i18n(pl->ob, " (can be replayed)");
        draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, TAG_START "This quest has been completed%s.[/color]", restart);
    }
    prefix = "";
    if (qs->state != QC_CAN_RESTART) {
        /* ie, if we are in progress or completed for a non-restartable quest */
        if (!step) {
            /* already warned by quest_get_step */
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, " \nOutcome: (invalid quest)");
            return;
        }
        if (level > 0) {
            prefix = " * ";
        } else if (qs->is_complete)
            prefix = "Outcome";
        else
            prefix = "Current Status";
        draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, " \n" TAG_START "%s:" TAG_END " %s", prefix, step->step_description);
    }

    /* ok, now check all of the player's other quests for any children, and print those in order */
    state = pq->quests;
    while (state) {
        child = quest_find_by_code(state->code);
        if (child->parent == quest)
            quest_info(pl, who, state, level+1);
        state = state->next;
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
    quest_definition *quest = quest_find_by_code(quest_code);

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
    quest_player *pq;
    quest_state *q;
    quest_definition *quest;

    quest = quest_find_by_code(quest_code);
    if (!quest) {
        LOG(llevError, "quest_start: requested unknown quest %s\n", quest_code);
        return;
    }
    pq = get_or_create_quest(pl);
    q = get_or_create_state(pq, quest_code);

    if (state <= 0) {
        state = 100;
        LOG(llevDebug, "quest_start: negative state %d for %s quest %s\n", state, pl->ob->name, quest_code);
    }

    /* if completed already, assume the player can redo it */
    if (q->state > 0) {
        LOG(llevDebug, "quest_start: warning: player %s has already started quest %s\n", pl->ob->name, quest_code);
    }

    draw_ext_info_format(NDI_UNIQUE | NDI_DELAYED, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, "New quest started: %s", quest->quest_title);

    quest_set_state(NULL, pl, quest_code, state, 0);

    /* saving state will be done in quest_set_state(). */
}

/**
 * Set the state of a quest for a player.
 * @param pl player to set the state for.
 * @param quest_code quest internal code.
 * @param state new state for the quest, must be greater than 0 else forced to 100 and a warning is emitted.
 */
void quest_set_player_state(player *pl, sstring quest_code, int state) {
    quest_set_state(NULL, pl, quest_code, state, 1);
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
 */
void command_quest(object *op, const char *params) {
    /* who to display information about, used when called in DM mode */
    object *who;
    const char *name;

    if (!op->contr) {
        LOG(llevError, "command_quest called for a non player!\n");
        return;
    }

    if (!params || *params == '\0') {
        command_help(op, "quest");
        return;
    }

    if (QUERY_FLAG(op, FLAG_WIZ)) {
        char* dup = strdup(params);
        char* space = strchr(dup, ' ');
        player* other;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN_DM, MSG_TYPE_COMMAND_QUESTS, "Command 'quest' called in DM mode.");
        if (space == NULL) {
            free(dup);
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN_DM, MSG_TYPE_COMMAND_FAILURE, "Please specify a player name.");
            return;
        }
        params = params + (space - dup) + 1;
        *space = '\0';
        other = find_player_partial_name(dup);
        if (other == NULL) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN_DM, MSG_TYPE_COMMAND_FAILURE, "%s is not online, or ambiguous name.", dup);
            free(dup);
            return;
        }
        free(dup);
        who = other->ob;
        name = who->name;
    } else {
        who = op;
        name = i18n(op, "You");
    }

    if (strcmp(params, "list all") == 0) {
        quest_list(op->contr, who->contr, 1, name);
        return;
    }

    if (strcmp(params, "list") == 0) {
        quest_list(op->contr, who->contr, 0, name);
        return;
    }

    if (strncmp(params, "info ", 5) == 0) {
        int number = atoi(params+5);
        quest_info(op->contr, who->contr, get_quest_by_number(who->contr, number), 0);
        return;
    }

    /* 
     * Quest display for clients using the quest system, similar to 'info' above
     * but using the (shared) quest's client_code instead of the (player unique) index.
     */
    if (strncmp(params, "info_c ", 7) == 0) {
        int number = atoi(params+7);
        quest_player *qp = get_quest(who->contr);
        quest_state *qs = qp ? qp->quests : NULL;
        while (qs) {
            quest_definition *q = quest_get_by_code(qs->code);
            if (q && q->client_code == number) {
                break;
            }
            qs = qs->next;
        }
        if (qs) {
            quest_info(op->contr, who->contr, qs, 0);
            return;
        }
        draw_ext_info(NDI_UNIQUE, 0, who, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "Invalid quest number");
        return;
    }

    if (QUERY_FLAG(op, FLAG_WIZ) && strncmp(params, "set ", 4) == 0) {
        char *dup = strdup(params + 4);
        char *space = strrchr(dup, ' ');
        int state, number;
        quest_state* q;
        if (space == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN_DM, MSG_TYPE_COMMAND_FAILURE, "Syntax is: quest (player name) (quest number) (state).");
            free(dup);
            return;
        }
        *space = '\0';
        number = atoi(dup);
        q = get_quest_by_number(who->contr, number);
        if (q == NULL) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN_DM, MSG_TYPE_COMMAND_FAILURE, "Invalid quest number %d.", number);
            free(dup);
            return;
        }
        state = atoi(space + 1);
        quest_set_state(op->contr, who->contr, q->code, state, 0);
        free(dup);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN_DM, MSG_TYPE_COMMAND_SUCCESS, "Set changed.");
        return;
    }

    command_help(op, "quest");
}

typedef struct {
    const quest_definition *parent;
    int level;
} dump;
/**
 * Dump all defined quests on the logfile. Will call itself recursively.
 * @param parent only quests with a parent of this value will be displayed.
 * Use NULL to display top-level quests.
 * @param level number of '-' to display before the quest's name.
 */
static void output_quests(const quest_definition *quest, void *user) {
    dump *d = (dump *)user;
    if (d->parent != quest->parent)
        return;

    char prefix[MAX_BUF];
    prefix[0] = '\0';
    for (int i = 0; i < d->level; i++) {
        strncat(prefix, "-", MAX_BUF - 1);
    }
    prefix[MAX_BUF - 1] = '\0';

    int stepcount = 0;
    quest_step_definition *step = quest->steps;
    while (step) {
        stepcount++;
        step = step->next;
    }

    fprintf(logfile, "%s%s - %s - %d steps (%srestartable)\n", prefix, quest->quest_code, quest->quest_title, stepcount, quest->quest_restart ? "" : "not ");

    dump r;
    r.parent = quest;
    r.level = d->level + 1;
    quest_for_each(output_quests, &r);
}

/**
 * Dump all of the quests, then calls exit() - useful in terms of debugging to make sure that
 * quests are set up and recognised correctly.
 */
void dump_quests(void) {
    dump d;
    d.parent = NULL;
    d.level = 0;
    quest_for_each(output_quests, &d);
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
        free(pq);
        pq = next;
    }
    player_states = NULL;
}

/**
 * Send the current quest states for the specified player, if the client
 * supports those notifications.
 * @param pl who to send quests for.
 */
void quest_send_initial_states(player *pl) {
    quest_player *states = NULL;
    quest_state *state = NULL;
    SockList sl;
    size_t size;
    quest_definition *quest;
    quest_step_definition *step;

    if (pl->socket.notifications < 1)
        return;

    states = get_or_create_quest(pl);

    SockList_Init(&sl);
    SockList_AddString(&sl, "addquest ");
    for (state = states->quests; state != NULL; state = state->next) {

        quest = quest_get_by_code(state->code);
        if (state->state == -1)
            step = NULL;
        else
            step = quest_get_step(quest, state->state);

        size = 2 + (2 + strlen(quest->quest_title)) + 4 + 1 + (2 + (step != NULL ? strlen(step->step_description) : 0));

        if (SockList_Avail(&sl) < size) {
            Send_With_Handling(&pl->socket, &sl);
            SockList_Reset(&sl);
            SockList_AddString(&sl, "addquest ");
        }

        SockList_AddInt(&sl, quest->client_code);
        SockList_AddLen16Data(&sl, quest->quest_title, strlen(quest->quest_title));
        if (quest->face && !(pl->socket.faces_sent[quest->face->number]&NS_FACESENT_FACE))
            esrv_send_face(&pl->socket, quest->face, 0);
        SockList_AddInt(&sl, quest->face ? quest->face->number : 0);
        SockList_AddChar(&sl, quest->quest_restart ? 1 : 0);
        SockList_AddInt(&sl, quest->parent ? quest->parent->client_code : 0);
        SockList_AddChar(&sl, (step == NULL || step->is_completion_step) ? 1 : 0);
        if (step != NULL)
            SockList_AddLen16Data(&sl, step->step_description, strlen(step->step_description));
        else
            SockList_AddShort(&sl, 0);

        state->sent_to_client = 1;
    }

    Send_With_Handling(&pl->socket, &sl);
    SockList_Term(&sl);
}

/**
 * Ensure the quest state is correctly saved for a player.
 * This function should only be called once, when the player's save directory
 * is created. All other quest functions save the state automatically, but save
 * can only happen when the player directory exists.
 * @param pl who to save quests for.
 */
void quest_first_player_save(player *pl) {
    quest_player *qp = get_quest(pl);
    if (qp != NULL && qp->quests != NULL) {
        quest_write_player_data(qp);
    }
}
