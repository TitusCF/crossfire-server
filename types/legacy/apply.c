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

    The authors can be reached via e-mail to crossfire-devel@real-time.com
*/
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>
#include <sproto.h>

/** @file legacy/apply.c
 * Legacy implementation of apply-related methods.
 */
/**
 * Main apply handler.
 *
 * Checks for unpaid items before applying.
 *
 * Return value:
 *   0: player or monster can't apply objects of that type
 *   1: has been applied, or there was an error applying the object
 *   2: objects of that type can't be applied if not in inventory
 *
 * op is the object that is causing object to be applied, tmp is the object
 * being applied.
 *
 * aflag is special (always apply/unapply) flags.  Nothing is done with
 * them in this function - they are passed to apply_special
 */

method_ret legacy_ob_apply(ob_methods *context, object *op, object *applier,
    int aflags)
{
    switch (op->type)
    {
        case CF_HANDLE:
            draw_ext_info(NDI_UNIQUE, 0,applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                "You turn the handle.", NULL);
            play_sound_map(applier->map, applier->x, applier->y,
                SOUND_TURN_HANDLE);
            op->value=op->value?0:1;
            SET_ANIMATION(op, op->value);
            update_object(op,UP_OBJ_FACE);
            push_button(op);
            return METHOD_OK;

        case SKILLSCROLL:
            if (applier->type == PLAYER)
            {
                legacy_apply_skillscroll (applier, op);
                return METHOD_OK;
            }
            return METHOD_UNHANDLED;

        case SPELLBOOK:
            if (applier->type == PLAYER)
            {
                legacy_apply_spellbook (applier, op);
                return METHOD_OK;
            }
            return METHOD_UNHANDLED;

        case SCROLL:
            apply_scroll (applier, op, applier->facing);
            return METHOD_OK;

        case POTION:
            (void) apply_potion(applier, op);
            return METHOD_OK;

        /* Eneq(at)(csd.uu.se): Handle apply on containers. */
        case CLOSE_CON:
            if (applier->type==PLAYER)
                legacy_apply_container (applier, op->env);
            return METHOD_OK;

        case CONTAINER:
            if (applier->type==PLAYER)
                legacy_apply_container (applier, op);
            return METHOD_OK;

        case TREASURE:
            if (applier->type == PLAYER)
            {
                legacy_apply_treasure (applier, op);
                return METHOD_OK;
            }
            return METHOD_UNHANDLED;

        case WEAPON:
        case ARMOUR:
        case BOOTS:
        case GLOVES:
        case AMULET:
        case GIRDLE:
        case BRACERS:
        case SHIELD:
        case HELMET:
        case RING:
        case CLOAK:
        case WAND:
        case ROD:
        case HORN:
        case SKILL:
        case BOW:
        case LAMP:
        case BUILDER:
        case SKILL_TOOL:
            if (op->env != applier)
                return METHOD_ERROR;   /* not in inventory */
            (void) apply_special (applier, op, aflags);
            return METHOD_OK;

        case DRINK:
        case FOOD:
        case FLESH:
            legacy_apply_food(applier, op);
            return METHOD_OK;

        case POISON:
            apply_poison(applier, op);
            return METHOD_OK;

        case SAVEBED:
            if (applier->type == PLAYER)
            {
                legacy_apply_savebed(applier);
                return METHOD_OK;
            }
            return METHOD_UNHANDLED;

        case WEAPON_IMPROVER:
            (void) legacy_check_improve_weapon(applier, op);
            return METHOD_OK;

        default:
            return METHOD_UNHANDLED;
    }
}
