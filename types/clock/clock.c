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
/** @file clock.c
 * The implementation of the Clock class of objects.
 */
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>
#include <sproto.h>

static method_ret clock_type_apply(ob_methods *context, object *op,
    object* applier, int aflags);

/**
 * Initializer for the CLOCK object type.
 */
void init_type_clock(void)
{
    register_apply(CLOCK, clock_type_apply);
}

/**
 * Handles using a clock.
 * @param context The method context
 * @param op The clock to apply
 * @param applier The object attempting to view the clock
 * @param aflags Special flags (always apply/unapply)
 * @retval 0 If the clock wasn't viewed by a player
 * @retval 1 If applier was a player
 */
static method_ret clock_type_apply(ob_methods *context, object *op,
    object* applier, int aflags)
{
    if (applier->type == PLAYER)
    {
        timeofday_t tod;

        get_tod(&tod);
        play_sound_player_only(applier->contr, SOUND_CLOCK,0,0);
        draw_ext_info_format(NDI_UNIQUE, 0,applier,
             MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
             "It is %d minute%s past %d o'clock %s",
             "It is %d minute%s past %d o'clock %s",
             tod.minute+1, ((tod.minute+1 < 2) ? "" : "s"),
             ((tod.hour % 14 == 0) ? 14 : ((tod.hour)%14)),
             ((tod.hour >= 14) ? "pm" : "am"));
        return 1;
    }
    return 0;
}
