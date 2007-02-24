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

/** @file exit/exit.c
 * The implementation of the Exit class of objects.
 */
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>

static method_ret exit_type_move_on(ob_methods* context, object* trap,
    object* victim, object* originator);

/**
 * Initializer for the EXIT object type.
 */
void init_type_exit()
{
    register_move_on(EXIT, exit_type_move_on);
}

/**
 * Move on this Exit object.
 * @param context The method context
 * @param trap The Exit we're moving on
 * @param victim The object moving over this one
 * @param originator The object that caused the move_on event
 * @return METHOD_OK
 */
static method_ret exit_type_move_on(ob_methods* context, object* trap,
    object* victim, object* originator)
{
    if (common_pre_ob_move_on(trap, victim, originator)==METHOD_ERROR)
        return METHOD_OK;
    if (victim->type == PLAYER && EXIT_PATH (trap))
    {
        /* Basically, don't show exits leading to random maps the
         * players output.
         */
        if (trap->msg && strncmp(EXIT_PATH(trap),"/!",2) &&
            strncmp(EXIT_PATH(trap), "/random/", 8))
            draw_ext_info (NDI_NAVY, 0, victim, MSG_TYPE_APPLY,
                MSG_TYPE_APPLY_TRAP, trap->msg, NULL);
        enter_exit(victim, trap);
    }
    common_post_ob_move_on(trap, victim, originator);
    return METHOD_OK;
}
