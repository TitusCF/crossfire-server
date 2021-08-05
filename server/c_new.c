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
 * This file deals with administrative commands from the client.
 */

#include "global.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "commands.h"
#include "sproto.h"

#ifndef tolower
/** Simple macro to convert a letter to lowercase. */
#define tolower(C) (((C) >= 'A' && (C) <= 'Z') ? (C)-'A'+'a' : (C))
#endif

/**
 * Player wants to start running.
 *
 * @param op
 * player.
 * @param params
 * additional parameters.
 */
void command_run(object *op, const char *params) {
    int dir;

    dir = atoi(params);
    if (dir < 0 || dir >= 9) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Can't run into a non adjacent square.");
        return;
    }
    op->contr->run_on = 1;
    move_player(op, dir);
}

/**
 * Player wants to stop running.
 *
 * @param op
 * player.
 * @param params
 * ignored.
 * @return
 * 1.
 */
void command_run_stop(object *op, const char *params) {
    op->contr->run_on = 0;
}

/**
 * Player wants to start furing.
 *
 * @param op
 * player.
 * @param params
 * additional parameters.
 */
void command_fire(object *op, const char *params) {
    int dir;

    dir = atoi(params);
    if (dir < 0 || dir >= 9) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Can't fire to a non adjacent square.");
        return;
    }
    op->contr->fire_on = 1;
    move_player(op, dir);
}

/**
 * Player wants to stop firing.
 *
 * @param op
 * player.
 * @param params
 * ignored.
 */
void command_fire_stop(object *op, const char *params) {
    op->contr->fire_on = 0;
}

/**
 * Player wants to face a given direction.
 *
 * @param op
 * player.
 * @param params
 * additional parameters.
 */
void command_face(object *op, const char *params) {
    int dir;

    if ( !isdigit(*params) ) {
        if ( strcmp(params,"stay") == 0 || strcmp(params,"down") == 0 ) dir=0;
        else if ( strcmp(params,"north") == 0 || strcmp(params,"n") == 0 ) dir=1;
        else if ( strcmp(params,"northeast") == 0 || strcmp(params,"ne") == 0 ) dir=2;
        else if ( strcmp(params,"east") == 0 || strcmp(params,"e") == 0 ) dir=3;
        else if ( strcmp(params,"southeast") == 0 || strcmp(params,"se") == 0 ) dir=4;
        else if ( strcmp(params,"south") == 0 || strcmp(params,"s") == 0 ) dir=5;
        else if ( strcmp(params,"southwest") == 0 || strcmp(params,"sw") == 0 ) dir=6;
        else if ( strcmp(params,"west") == 0 || strcmp(params,"w") == 0 ) dir=7;
        else if ( strcmp(params,"northwest") == 0 || strcmp(params,"nw") == 0 ) dir=8;
        else {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                                 "Unknown direction to face: %s",params);
            return;
        }
    }
    else dir = atoi(params);
    if (dir < 0 || dir >= 9) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Can't face to a non adjacent square.");
        return;
    }
    face_player(op, dir);
}
