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

/** @file spell_effect/spell_effect.c
 * The implementation of the Spell Effect class of objects.
 */
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>
#include <sproto.h>

static method_ret spell_effect_type_move_on(ob_methods* context, object* trap,
    object* victim, object* originator);

/**
 * Initializer for the SPELL_EFFECT object type.
 */
void init_type_spell_effect()
{
    register_move_on(SPELL_EFFECT, spell_effect_type_move_on);
}
/**
 * Move on this Spell Effect object.
 * @param context The method context
 * @param trap The Spell Effect we're moving on
 * @param victim The object moving over this one
 * @param originator The object that caused the move_on event
 * @return METHOD_OK
 */
static method_ret spell_effect_type_move_on(ob_methods* context, object* trap,
    object* victim, object* originator)
{
    if (common_pre_ob_move_on(trap, victim, originator)==METHOD_ERROR)
        return METHOD_OK;

    switch (trap->subtype)
    {
        case SP_CONE:
            if (QUERY_FLAG(victim, FLAG_ALIVE) && trap->speed
                && trap->attacktype)
                hit_player(victim, trap->stats.dam, trap, trap->attacktype, 0);
            break;

        case SP_MAGIC_MISSILE:
            if (QUERY_FLAG (victim, FLAG_ALIVE))
            {
                tag_t spell_tag = trap->count;
                hit_player(victim, trap->stats.dam, trap, trap->attacktype, 1);
                if (!was_destroyed(trap, spell_tag))
                {
                    remove_ob(trap);
                    free_object (trap);
                }
            }
            break;

        case SP_MOVING_BALL:
            if (QUERY_FLAG (victim, FLAG_ALIVE))
                hit_player(victim, trap->stats.dam, trap, trap->attacktype, 1);
            else if (victim->material || victim->materialname)
                save_throw_object(victim, trap->attacktype, trap);
            break;
    }
    common_post_ob_move_on(trap, victim, originator);
    return METHOD_OK;
}
