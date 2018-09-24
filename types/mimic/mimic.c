/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2018 Mark Wedel & Crossfire Development Team
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

/**
 * Implementation of mimics
 * This should be usable for any item masquerading as a container (or other item, for that matter)
 * that activates when you try to pick it up or apply it.
 */

#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>
#include <sproto.h>
#include <string.h>

static method_ret mimic_type_apply(ob_methods *context, object *op, object *applier, int aflags);

/**
 * Initializer for MIMIC type.
 */
void init_type_mimic(void) {
    register_apply(MIMIC, mimic_type_apply);
}

/**
 * Handles applying a mimic.
 * It just becomes a monster and gains an animation.
 * @param context The method context
 * @param op The object being applied
 * @param applier The object doing the applying
 * @param aflags Special flags
 * @retval METHOD_UNHANDLED If a non-player tries to open it.
 * @retval METHOD_OK If a player does the opening.
 */
static method_ret mimic_type_apply(ob_methods *context, object *op, object *applier, int aflags) {
    if (applier->type == PLAYER) {
        
        draw_ext_info(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS, "Ah! Its alive!");
        /* We become a monster */
        op->type = MONSTER;
        /* We are animated, too, assuming we have a face right now. */
        if (op->face)
        {
            char anim_name_buf[128];
            int anim_name_len = strlen(op->face->name);
            strncpy(anim_name_buf, op->face->name, 128);
            // Remove the .11x from the end of the face name.
            anim_name_buf[anim_name_len-4] = '\0';
            strncat(anim_name_buf, "_mimic", 128-anim_name_len+4);
            op->animation_id = find_animation(anim_name_buf);
            SET_FLAG(op, FLAG_ANIMATE);
        }
        SET_FLAG(op, FLAG_ALIVE);
        SET_FLAG(op, FLAG_MONSTER);
        op->enemy = applier;
        // TODO: Should this be able to be set dynamically?
        FREE_AND_COPY(op->name, "mimic");
        return METHOD_OK;
    }
    return METHOD_UNHANDLED;
}
