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
/** @file power_crystal.c
 * The implementation of a Power Crystal.
 * A Power Crystal can store mana from a player, and give it back when needed.
 */
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sproto.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static method_ret power_crystal_type_apply(ob_methods *context, object *op, object *applier, int aflags);

#define LEVELS 7

/** Existing levels, dot-terminated. */
static const char* crystal_levels[LEVELS] = {
    "empty.",
    "almost empty.",
    "partially filled.",
    "half full.",
    "well charged.",
    "almost full.",
    "fully charged.",
};

/**
 * Return the level, [0..LEVELS[, of charge of the item.
 * @param op crystal to get the level of, only for the maxsp.
 * @param sp sp value to get the level for.
 * @return level.
 */
static int crystal_level(const object *op, int sp) {
    int i = (sp*10) / op->stats.maxsp;
    if (sp == 0)
        return 0;
    else if (i == 0)
        return 1;
    else if (i < 3)
        return 2;
    else if (i < 6)
        return 3;
    else if (i < 9)
        return 4;
    else if (sp == op->stats.maxsp)
        return 6;
    else
        return 5;
}

/**
 * Add the capacity of the crystal to the buffer. Sentence will be dot-terminated.
 * @param op crystal to add the capacity of.
 * @param buf where to put the capacity.
 * @param previous_sp if -1 then ignored, else previous sp of the crystal to
 * add 'still ' or 'now ' to the message to signal level changes.
 */
static void add_capacity(const object *op, StringBuffer *buf, int previous_sp) {
    int current = crystal_level(op, op->stats.sp);

    assert(current >= 0 && current < LEVELS);

    if (previous_sp != -1) {
        int previous = crystal_level(op, previous_sp);
        assert(previous >= 0 && previous < LEVELS);
        if (current == previous) {
            stringbuffer_append_string(buf, "still ");
        } else {
            stringbuffer_append_string(buf, "now ");
        }
    }
    
    stringbuffer_append_string(buf, crystal_levels[current]);
}

static void power_crystal_describe(const ob_methods *context, const object *op, const object *observer, char *buf, size_t size) {
    StringBuffer *sb = stringbuffer_new();
    char *final;

    buf[0] = '\0';
    query_name(op, buf, size-1);
    buf[size-1] = 0;

    /* Avoid division by zero... */
    if (op->stats.maxsp == 0) {
        stringbuffer_append_printf(sb, "(capacity %d).", op->stats.maxsp);
    } else {
        int i;
        if (op->stats.maxsp > 1000) { /*higher capacity crystals*/
          i = (op->stats.maxsp%1000)/100;
          if (i)
              stringbuffer_append_printf(sb, "(capacity %d.%dk). It is ", op->stats.maxsp/1000, i);
          else
            stringbuffer_append_printf(sb, "(capacity %dk). It is ", op->stats.maxsp/1000);
        } else
            stringbuffer_append_printf(sb, "(capacity %d). It is ", op->stats.maxsp);
        add_capacity(op, sb, -1);
    }

    final = stringbuffer_finish(sb);
    strncat(buf, final, size);
    free(final);
}

/**
 * Initializer for the POWER_CRYSTAL object type.
 */
void init_type_power_crystal(void) {
    register_apply(POWER_CRYSTAL, power_crystal_type_apply);
    register_describe(POWER_CRYSTAL, power_crystal_describe);
}

/**
 * This function handles the application of power crystals.
 * Power crystals, when applied, either suck power from the applier,
 * if he's at full spellpoints, or gives him power, if it's got
 * spellpoins stored.
 * @param context The method context
 * @param op The power crystal to apply
 * @param applier The object applying the crystal
 * @param aflags Special flags (always apply/unapply)
 * @return The return value is always METHOD_OK
 */
static method_ret power_crystal_type_apply(ob_methods *context, object *op, object *applier, int aflags) {
    int available_power;
    int power_space;
    int power_grab;
    StringBuffer *sb = stringbuffer_new();
    char name[MAX_BUF], *message;

    available_power = applier->stats.sp-applier->stats.maxsp;
    power_space = op->stats.maxsp-op->stats.sp;
    power_grab = 0;
    query_name(op, name, sizeof(name));
    if (available_power >= 0 && power_space > 0) {
        power_grab = MIN(power_space, 0.5*applier->stats.sp);
        stringbuffer_append_string(sb, "You transfer power to the ");
    }
    if (available_power < 0 && op->stats.sp > 0) {
        power_grab = -MIN(-available_power, op->stats.sp);
        stringbuffer_append_string(sb, "You grab power from the ");
    }

    if (power_grab == 0)
        stringbuffer_append_string(sb, "Nothing happens.");
    else {
        int sp = op->stats.sp;
        stringbuffer_append_printf(sb, "%s. It is ", name);
        applier->stats.sp -= power_grab;
        op->stats.sp += power_grab;
        op->speed = (float)op->stats.sp/(float)op->stats.maxsp;
        object_update_speed(op);
        if (applier->type == PLAYER)
            esrv_update_item(UPD_ANIMSPEED, applier, op);
        add_capacity(op, sb, sp);
    }

    message = stringbuffer_finish(sb);
    if (applier->type == PLAYER)
        draw_ext_info(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS, message);
    free(message);

    return METHOD_OK;
}
