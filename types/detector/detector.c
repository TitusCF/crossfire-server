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
 * The implementation of @ref page_type_51 "detector" objects.
 */

#include "global.h"

#include <string.h>

#include "ob_methods.h"
#include "ob_types.h"
#include "sounds.h"
#include "sproto.h"

static method_ret detector_type_process(ob_methods *context, object *op);

/**
 * Initializer for the @ref page_type_51 "detector" object type.
 */
void init_type_detector(void) {
    register_process(DETECTOR, detector_type_process);
}

/**
 * Move a @ref page_type_51 "detector".
 *
 * - slaying:    name of the thing the detector is to look for
 * - speed:      frequency of 'glances'
 * - connected:  connected value of detector
 * - sp:         1 if detection sets buttons
 *              -1 if detection unsets buttons
 *
 * @param op
 * detector to move.
 */
static void move_detector(object *op) {
    object *tmp;
    int last = op->value;
    int detected;
    detected = 0;

    if (!op->slaying) {
        if (op->map)
            LOG(llevError, "Detector with no slaying set at %s (%d,%d)\n", op->map->path, op->x, op->y);
        else if (op->env)
            LOG(llevError, "Detector with no slaying in %s\n", op->env->name);
        else
            LOG(llevError, "Detector with no slaying nowhere?\n");
        op->speed = 0;
        object_update_speed(op);
        return;
    }

    for (tmp = GET_MAP_OB(op->map, op->x, op->y); tmp != NULL; tmp = tmp->above) {
        if (op->stats.hp) {
            if (object_find_by_name(tmp, op->slaying) != NULL
            || object_find_by_type_and_slaying(tmp, FORCE, op->slaying)) {
                detected = 1;
                break;
            }
        }
        if (op->slaying == tmp->name) {
            detected = 1;
            break;
        }
        if (tmp->type == PLAYER && !strcmp(op->slaying, "player")) {
            detected = 1;
            break;
        }
        if (tmp->type == SPECIAL_KEY && tmp->slaying == op->slaying) {
            detected = 1;
            break;
        }
    }

    /* the detector sets the button if detection is found */
    if (op->stats.sp == 1)  {
        if (detected && last == 0) {
            op->value = 1;
            push_button(op);
        }
        if (!detected && last == 1) {
            op->value = 0;
            push_button(op);
        }
    } else { /* in this case, we unset buttons */
        if (detected && last == 1) {
            op->value = 0;
            push_button(op);
        }
        if (!detected && last == 0) {
            op->value = 1;
            push_button(op);
        }
    }
}

/**
 * Processes a @ref page_type_51 "detector".
 * @param context The method context, ignored
 * @param op The detector to process
 * @retval METHOD_OK
 */
static method_ret detector_type_process(ob_methods *context, object *op) {
    move_detector(op);
    return METHOD_OK;
}
