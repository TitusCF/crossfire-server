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
 * The implementation of the Gate and Timed Gate classes of objects.
 * @todo merge GATE and TIMED_GATE object types.
 */
#include "global.h"

#include <stdlib.h>

#include "ob_methods.h"
#include "ob_types.h"
#include "sounds.h"
#include "sproto.h"

static method_ret gate_type_process(object *op);
static method_ret timed_gate_type_process(object *op);

/**
 * Initializer for the gate object type.
 */
void init_type_gate(void) {
    register_process(GATE, gate_type_process);
    register_process(TIMED_GATE, timed_gate_type_process);
}

/**
 * Handle ob_process for all gate objects.
 * @param op The gate that's being processed.
 * @return METHOD_OK
 */
static method_ret gate_type_process(object *op) {
    object *tmp, *part;

    if (op->stats.wc < 0 || (int)op->stats.wc >= NUM_ANIMATIONS(op)) {
        StringBuffer *sb;
        char *diff;

        LOG(llevError, "Gate error: animation was %d, max=%d, on %s (%d, %d)\n", op->stats.wc, NUM_ANIMATIONS(op), map_get_path(op), op->x, op->y);
        sb = stringbuffer_new();
        object_dump(op, sb);
        diff = stringbuffer_finish(sb);
        LOG(llevError, "%s\n", diff);
        free(diff);
        op->stats.wc = 0;
    }

    /* We're going down */
    if (op->value) {
        if (--op->stats.wc <= 0) { /* Reached bottom, let's stop */
            op->stats.wc = 0;
            if (op->arch->clone.speed)
                op->value = 0;
            else {
                op->speed = 0;
                object_update_speed(op);
            }
        }

        for (part = op; part != NULL; part = part->more) {
            if ((int)op->stats.wc < (NUM_ANIMATIONS(op)/2+1)) {
                part->move_block = 0;
                CLEAR_FLAG(part, FLAG_BLOCKSVIEW);
                update_all_los(part->map, part->x, part->y);
            }
            SET_ANIMATION(part, op->stats.wc);
        }
        object_update(op, UP_OBJ_CHANGE);
        play_sound_map(SOUND_TYPE_GROUND, op, 0, "gate moving");
        return METHOD_OK;
    }

    /* We're going up */

    /* First, lets see if we are already at the top */
    if ((unsigned char)op->stats.wc == (NUM_ANIMATIONS(op)-1)) {
        /* Check to make sure that only non pickable and non rollable
         * objects are above the gate.  If so, we finish closing the gate,
         * otherwise, we fall through to the code below which should lower
         * the gate slightly.
         */

        for (tmp = op->above; tmp != NULL; tmp = tmp->above)
            if (!QUERY_FLAG(tmp, FLAG_NO_PICK)
            || QUERY_FLAG(tmp, FLAG_CAN_ROLL)
            || QUERY_FLAG(tmp, FLAG_ALIVE))
                break;

        if (tmp == NULL) {
            if (op->arch->clone.speed)
                op->value = 1;
            else {
                op->speed = 0;
                object_update_speed(op); /* Reached top, let's stop */
            }
            return METHOD_OK;
        }
    }

    if (op->stats.food) {    /* The gate is going temporarily down */
        if (--op->stats.wc <= 0) { /* Gone all the way down? */
            op->stats.food = 0;     /* Then let's try again */
            op->stats.wc = 0;
        }
    } else {                /* The gate is still going up */
        op->stats.wc++;

        if ((int)op->stats.wc >= (NUM_ANIMATIONS(op)))
            op->stats.wc = (signed char)NUM_ANIMATIONS(op)-1;

        /* If there is something on top of the gate, we try to roll it off.
         * If a player/monster, we don't roll, we just hit them with damage
         */
        if ((int)op->stats.wc >= NUM_ANIMATIONS(op)/2) {
            /* Halfway or further, check blocks */
            /* First, get the top object on the square. */
            for (tmp = op->above; tmp != NULL && tmp->above != NULL; tmp = tmp->above)
                ;

            if (tmp != NULL) {
                if (QUERY_FLAG(tmp, FLAG_ALIVE)) {
                    hit_player(tmp, random_roll(1, op->stats.dam, tmp, PREFER_LOW), op, AT_PHYSICAL, 1);
                    if (tmp->type == PLAYER)
                        draw_ext_info_format(NDI_UNIQUE, 0, tmp, MSG_TYPE_VICTIM, MSG_TYPE_VICTIM_WAS_HIT,
                                             "You are crushed by the %s!",
                                             op->name);
                } else
                    /* If the object is not alive, and the object either can
                     * be picked up or the object rolls, move the object
                     * off the gate.
                     *
                     * If the top item is not interactable, then check items below it
                     * instead. This allows us to use gates with translucent roofs
                     * or ambient weather effects on the map that end up above the other items.
                     */
                    while (tmp != op) {
                        if (!QUERY_FLAG(tmp, FLAG_ALIVE)
                        && (!QUERY_FLAG(tmp, FLAG_NO_PICK) || QUERY_FLAG(tmp, FLAG_CAN_ROLL))) {
                            /* If it has speed, it should move itself, otherwise: */
                            int i = object_find_free_spot(tmp, op->map, op->x, op->y, 1, 9);

                            /* If there is a free spot, move the object someplace */
                            if (i != -1) {
                                object_remove(tmp);
                                object_insert_in_map_at(tmp, op->map, op, 0, op->x+freearr_x[i], op->y+freearr_y[i]);
                            }
                            break;
                        }
                        tmp = tmp->below;
                    }
            }

            /* See if there is still anything blocking the gate */
            for (tmp = op->above; tmp != NULL; tmp = tmp->above)
                if (!QUERY_FLAG(tmp, FLAG_NO_PICK)
                || QUERY_FLAG(tmp, FLAG_CAN_ROLL)
                || QUERY_FLAG(tmp, FLAG_ALIVE))
                    break;

            /* IF there is, start putting the gate down  */
            if (tmp) {
                op->stats.food = 1;
            } else {
                object* part;
                for (part = op; part != NULL; part = part->more) {
                    if (!part->stats.luck)
                        part->move_block = MOVE_ALL;
                    if (!part->stats.ac) {
                        SET_FLAG(part, FLAG_BLOCKSVIEW);
                        update_all_los(part->map, part->x, part->y);
                    }
                }
            }
        } /* gate is halfway up */

        for (part = op; part != NULL; part = part->more) {
            SET_ANIMATION(part, op->stats.wc);
        }
        object_update(op, UP_OBJ_CHANGE);
        play_sound_map(SOUND_TYPE_GROUND, op, 0, "gate moving");
    } /* gate is going up */

    return METHOD_OK;
}

 /**
 * Handle ob_process for all timed gate objects.
 * - hp      : how long door is open/closed
 * - maxhp   : initial value for hp
 * - sp      : 1 = open, 0 = close
 * @param op The timed gate that's being processed.
 * @return METHOD_OK
 * @todo Split function into more managable functions.
 */
static method_ret timed_gate_type_process(object *op) {
    int v = op->value;

    if (op->stats.sp) {
        gate_type_process(op);
        if (op->value != v)   /* change direction ? */
            op->stats.sp = 0;
        return METHOD_OK;
    }
    if (--op->stats.hp <= 0) { /* keep gate down */
        gate_type_process(op);
        if (op->value != v) {  /* ready ? */
            op->speed = 0;
            object_update_speed(op);
        }
    }
    return METHOD_OK;
}
