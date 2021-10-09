/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef QUEST_H
#define QUEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"

/** One condition to automatically move to a quest step. */
typedef struct quest_condition {
    sstring quest_code;          /**< The quest that triggers the condition */
    int minstep;                  /**< The earliest step in the quest that triggers the condition,
                                    -1 means finished, 0 means not started */
    int maxstep;                  /**< The latest step that triggers the condition, to match,
                                        the stages must be between minstep and maxstep */
    struct quest_condition *next; /**< The next condition to check */
} quest_condition;

/** One step of a quest. */
typedef struct quest_step_definition {
    int step;                           /**< Step identifier. */
    sstring step_description;           /**< Step description to show player. */
    int is_completion_step:1;           /**< Whether this step completes the quest (1) or not (0) */
    struct quest_step_definition *next; /**< Next step. */
    quest_condition *conditions;        /**< The conditions that must be satisfied to trigger the step */
} quest_step_definition;

/** Definition of an in-game quest. */
typedef struct quest_definition {
    sstring quest_code;             /**< Quest internal code. */
    sstring quest_title;            /**< Quest title for player. */
    sstring quest_description;      /**< Quest longer description. */
    sstring quest_comment;          /**< Quest comment, not visible to players. */
    int quest_restart;              /**< If non zero, can be restarted. */
    const Face *face;           /**< Face associated with this quest. */
    uint32_t client_code;             /**< The code used to communicate with the client, merely a unique index. */
    bool quest_is_system;           /**< If set then the quest isn't counted or listed. */
    quest_step_definition *steps;   /**< Quest steps. */
    struct quest_definition *parent;/**< Parent for this quest, NULL if it is a 'top-level' quest */
} quest_definition;

typedef void(*quest_op)(const quest_definition *, void *);

void quest_for_each(quest_op op, void *user);

#ifdef __cplusplus
}
#endif

#endif /* QUEST_H */
