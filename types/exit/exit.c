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
 * The implementation of the Exit class of objects.
 */
#include "global.h"

#include <string.h>

#include "ob_methods.h"
#include "ob_types.h"
#include "sounds.h"
#include "sproto.h"

static method_ret exit_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
static method_ret exit_type_apply(ob_methods *context, object *exit, object *op, int autoapply);

/**
 * Initializer for the EXIT object type.
 */
void init_type_exit(void) {
    register_move_on(EXIT, exit_type_move_on);
    register_apply(EXIT, exit_type_apply);
}

/**
 * Move on this Exit object.
 * @param context The method context
 * @param trap The Exit we're moving on
 * @param victim The object moving over this one
 * @param originator The object that caused the move_on event
 * @return METHOD_OK
 */
static method_ret exit_type_move_on(ob_methods *context, object *trap, object *victim, object *originator) {
    if (common_pre_ob_move_on(trap, victim, originator) == METHOD_ERROR)
        return METHOD_OK;
    if (victim->type == PLAYER && EXIT_PATH(trap)) {
        /* Basically, don't show exits leading to random maps the
         * players output.
         */
        if (trap->msg
        && strncmp(EXIT_PATH(trap), "/!", 2)
        && strncmp(EXIT_PATH(trap), "/random/", 8))
            draw_ext_info(NDI_NAVY, 0, victim, MSG_TYPE_APPLY, MSG_TYPE_APPLY_TRAP, trap->msg);
        enter_exit(victim, trap);
    }
    common_post_ob_move_on(trap, victim, originator);
    return METHOD_OK;
}

/**
 * This fonction return true if the exit is not a 2 ways one
 * or it is 2 ways, valid exit.
 * A valid 2 way exit means:
 *  - You can come back (there is another exit at the other side)
 *  - You are
 *     - the owner of the exit
 *     - or in the same party as the owner
 *
 * @note
 * a owner in a 2 way exit is saved as the owner's name
 * in the field exit->name cause the field exit->owner doesn't
 * survive in the swapping (in fact the whole exit doesn't survive).
 *
 * @param op
 * player to check for.
 * @param exit
 * exit object.
 * @return
 * 1 if exit is not 2 way, 0 else.
 */
static int is_legal_2ways_exit(object *op, object *exit) {
    object *tmp;
    object *exit_owner;
    player *pp;
    mapstruct *exitmap;

    if (exit->stats.exp != 1)
        return 1; /*This is not a 2 way, so it is legal*/
    if (!has_been_loaded(EXIT_PATH(exit)) && exit->race)
        return 0; /* This is a reset town portal */
    /* To know if an exit has a correspondent, we look at
     * all the exits in destination and try to find one with same path as
     * the current exit's position */
    if (!strncmp(EXIT_PATH(exit), settings.localdir, strlen(settings.localdir)))
        exitmap = ready_map_name(EXIT_PATH(exit), MAP_PLAYER_UNIQUE);
    else
        exitmap = ready_map_name(EXIT_PATH(exit), 0);
    if (exitmap) {
        tmp = GET_MAP_OB(exitmap, EXIT_X(exit), EXIT_Y(exit));
        if (!tmp)
            return 0;
        for ((tmp = GET_MAP_OB(exitmap, EXIT_X(exit), EXIT_Y(exit))); tmp; tmp = tmp->above) {
            if (tmp->type != EXIT)
                continue;  /*Not an exit*/
            if (!EXIT_PATH(tmp))
                continue; /*Not a valid exit*/
            if ((EXIT_X(tmp) != exit->x) || (EXIT_Y(tmp) != exit->y))
                continue; /*Not in the same place*/
            if (strcmp(exit->map->path, EXIT_PATH(tmp)) != 0)
                continue; /*Not in the same map*/

            /* From here we have found the exit is valid. However
             * we do here the check of the exit owner. It is important
             * for the town portals to prevent strangers from visiting
             * your apartments
             */
            if (!exit->race)
                return 1;  /*No owner, free for all!*/
            exit_owner = NULL;
            for (pp = first_player; pp; pp = pp->next) {
                if (!pp->ob)
                    continue;
                if (pp->ob->name != exit->race)
                    continue;
                exit_owner = pp->ob; /*We found a player which correspond to the player name*/
                break;
            }
            if (!exit_owner)
                return 0;    /* No more owner*/
            if (exit_owner->contr == op->contr)
                return 1;  /*It is your exit*/
            if (exit_owner    /*There is a owner*/
            && (op->contr)    /*A player tries to pass */
            && ((exit_owner->contr->party == NULL) /*No pass if controller has no party*/
                || (exit_owner->contr->party != op->contr->party))) /* Or not the same as op*/
                return 0;
            return 1;
        }
    }
    return 0;
}

/**
 * Handles applying an exit.
 * @param context The method context
 * @param exit The exit applied
 * @param op The object applying the exit
 * @param autoapply Set this to 1 to automatically apply the sign
 * @return METHOD_OK unless op is not a player, in which case METHOD_ERROR
 */
static method_ret exit_type_apply(ob_methods *context, object *exit, object *op, int autoapply) {
    if (op->type != PLAYER)
        return METHOD_ERROR;
    if (!EXIT_PATH(exit) || !is_legal_2ways_exit(op, exit)) {
        char name[MAX_BUF];

        query_name(exit, name, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                             "The %s is closed.", name);
    } else {
        /* Don't display messages for random maps. */
        if (exit->msg
        && strncmp(EXIT_PATH(exit), "/!", 2)
        && strncmp(EXIT_PATH(exit), "/random/", 8))
            draw_ext_info(NDI_NAVY, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS, exit->msg);
        enter_exit(op, exit);
    }
    return METHOD_OK;
}
