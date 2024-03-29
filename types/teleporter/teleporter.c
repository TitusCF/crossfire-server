/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2008 Crossfire Development Team

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
 * The implementation of @ref page_type_41 "teleporter" objects.
 */

#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>
#include <sproto.h>
#include <string.h>

static method_ret teleporter_type_process(object *op);
static method_ret teleporter_type_trigger(object *op, object *cause, int state);

/**
 * Initializer for the @ref page_type_41 "teleporter" object type.
 */
void init_type_teleporter(void) {
    register_process(TELEPORTER, teleporter_type_process);
    register_trigger(TELEPORTER, teleporter_type_trigger);
}

/**
 * Move function for @ref page_type_41 "teleporter" objects.
 *
 * @param op
 * teleporter.
 */
static void move_teleporter(object *op) {
    object *tmp, *head;

    /* if this is a multipart teleporter, handle the other parts
    * The check for speed isn't strictly needed - basically, if
    * there is an old multipart teleporter in which the other parts
    * have speed, we don't really want to call it twice for the same
    * function - in fact, as written below, part N would get called
    * N times without the speed check.
    */
    if (op->more && FABS(op->more->speed) < MIN_ACTIVE_SPEED)
        move_teleporter(op->more);

    head = HEAD(op);
    for (tmp = op->above; tmp != NULL; tmp = tmp->above) {
        if (QUERY_FLAG(tmp, FLAG_IS_FLOOR)) continue; /* Ignore floors */
        if (op->other_arch && strcmp(op->other_arch->name,tmp->name) != 0 ) continue; /* Ignore if other_arch doesn't match */
        if (QUERY_FLAG(tmp, FLAG_WIZPASS)) continue; /* Don't teleport the DM */
        /*
         * Use the subtype as a bitmask to specify types of objects to *ignore*:
         *  1 -- non-player
         *  2 -- players
         *  4 -- non-monster
         *  8 -- monsters
         *  16 -- no-pick objects
         *  ... -- add more as needed
         *
         * For most purposes, restricting players/non-players is the big win here.
         * For objects, use the other_arch field to restrict it to the particular object.
         */
        if ( (op->subtype & 1) && tmp->type != PLAYER ) continue; /* If flagged for player only */
        if ( (op->subtype & 2) && tmp->type == PLAYER ) continue; /* If flagged for non-player only */
        if ( (op->subtype & 4) && !QUERY_FLAG(tmp, FLAG_MONSTER) ) continue; /* If flagged for monster only */
        if ( (op->subtype & 8) && QUERY_FLAG(tmp, FLAG_MONSTER) ) continue; /* If flagged for non-monster only */
        if ( (op->subtype & 16) && QUERY_FLAG(tmp, FLAG_NO_PICK) ) continue; /* If flagged skip non-pickupable objects */
        /* ... */
        break;
    }

    /* If nothing above us to move, nothing to do */
    if (!tmp) return;

    if (EXIT_PATH(head)) {
        if (tmp->type == PLAYER) {
            if (events_execute_object_event(op, EVENT_TRIGGER, tmp, NULL, NULL, SCRIPT_FIX_ALL) != 0)
                return;
            enter_exit(tmp, head);
        }
        else
            /* Currently only players can transfer maps */
            return;
    } else if (EXIT_X(head) || EXIT_Y(head)) {
        if (out_of_map(head->map, EXIT_X(head), EXIT_Y(head))) {
            LOG(llevError, "Removed illegal teleporter.\n");
            object_remove(head);
            object_free_drop_inventory(head);
            return;
        }
        if (events_execute_object_event(op, EVENT_TRIGGER, tmp, NULL, NULL, SCRIPT_FIX_ALL) != 0)
            return;
        transfer_ob(tmp, EXIT_X(head), EXIT_Y(head), 0, head);
    } else {
        /* Random teleporter */
        if (events_execute_object_event(op, EVENT_TRIGGER, tmp, NULL, NULL, SCRIPT_FIX_ALL) != 0)
            return;
        teleport(head, TELEPORTER, tmp);
    }
}

/**
 * Processes a @ref page_type_41 "teleporter".
 * @param op The teleporter to process
 * @retval METHOD_OK
 */
static method_ret teleporter_type_process(object *op) {
    move_teleporter(op);
    return METHOD_OK;
}

/**
 * A @ref page_type_41 "teleporter" is triggered.
 * @param op The object being triggered
 * @param cause Ignored.
 * @param state Ignored.
 * @retval METHOD_OK
 */
static method_ret teleporter_type_trigger(object *op, object *cause, int state) {
    (void)cause;
    if (state) {
        move_teleporter(op);
    }
    return METHOD_OK;
}
