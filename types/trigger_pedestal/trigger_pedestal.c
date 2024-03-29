/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2007 Mark Wedel & Crossfire Development Team
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

/** @file trigger_pedestal.c
 * The implementation of the Trigger-Pedestal class of objects.
 */
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>
#include <sproto.h>

static method_ret trigger_pedestal_type_move_on(object *trap, object *victim, object *originator);

/**
 * Initializer for the TRIGGER_PEDESTAL object type.
 */
void init_type_trigger_pedestal(void) {
    register_move_on(TRIGGER_PEDESTAL, trigger_pedestal_type_move_on);
}

/**
 * Move on this Trigger Pedestal object.
 * @param trap The Trigger Pedestal we're moving on
 * @param victim The object moving over this one
 * @param originator The object that caused the move_on event
 * @return METHOD_OK
 */
static method_ret trigger_pedestal_type_move_on(object *trap, object *victim, object *originator) {
    trap = HEAD(trap);
    if (common_pre_ob_move_on(trap, victim, originator) == METHOD_ERROR)
        return METHOD_OK;
    check_trigger(trap, victim);
    common_post_ob_move_on(trap, victim, originator);
    return METHOD_OK;
}
