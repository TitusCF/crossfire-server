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
#include <stdlib.h>

static method_ret mimic_type_apply(ob_methods *context, object *op, object *applier, int aflags);

/**
 * Initializer for MIMIC type.
 */
void init_type_mimic(void) {
    register_apply(MIMIC, mimic_type_apply);
}

static inline const char *object_try_get_value(object *op, const char *key) {
    const char *val = object_get_value(op, key);
    return val ? val : "0";
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

        draw_ext_info(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS, "Ah! It's alive!");
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
            op->animation = find_animation(anim_name_buf);
            SET_FLAG(op, FLAG_ANIMATE);
        }
        SET_FLAG(op, FLAG_ALIVE);
        SET_FLAG(op, FLAG_MONSTER);
        // If we don't have a level set, use the map difficulty
        if (!op->level)
            op->level = op->map ? op->map->difficulty : 0;
        // Set the scalable stats based off the level given to the mimic at load.
        int level = op->level;
        op->stats.hp = op->stats.maxhp = op->stats.maxhp + (int16_t)(atof(object_try_get_value(op, "hp_per_level")) * level);
        op->stats.dam = op->stats.dam + (int16_t)(atof(object_try_get_value(op, "dam_per_level")) * level);
        op->stats.ac = op->stats.ac + (int8_t)(atof(object_try_get_value(op, "ac_per_level")) * level);
        op->stats.wc = op->stats.wc + (int8_t)(atof(object_try_get_value(op, "wc_per_level")) * level);
        op->stats.exp = op->stats.exp + (int64_t)(atof(object_try_get_value(op, "xp_per_level")) * level);
        op->speed = FABS(op->speed) + atof(object_try_get_value(op, "speed_per_level")) * level;
        // Set enemy to the triggerer.
        op->enemy = applier;
        // TODO: Should this be able to be set dynamically?
        FREE_AND_COPY(op->name, "mimic");
        return METHOD_OK;
    }
    return METHOD_UNHANDLED;
}
