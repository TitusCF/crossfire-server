/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2021 The Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file light_object.c
 * This file contains methods used for lighting objects, whether via applying the tool
 * or by applying the lightable object.
 */

#include "global.h"
#include "sproto.h"

/**
 * Common code for both lighting an item for applying the lighter
 * and for lighting the lightable object.
 *
 * @param item
 * The item to light.
 *
 * @param lighter_name
 * The name of the tool used to light the item
 *
 * @param applier
 * The object applying the lighter/lightable
 */
void do_light(object *item, const char *lighter_name, object *applier) {
    int is_player_env = 0;
    tag_t count;
    char item_name[MAX_BUF];
    uint32_t nrof;
    /* Perhaps we should split what we are trying to light on fire?
     * I can't see many times when you would want to light multiple
     * objects at once. */
    nrof = item->nrof;
    count = item->count;
    /* If the item is destroyed, we don't have a valid pointer to the
     * name object, so make a copy so the message we print out makes
     * some sense. */
    safe_strncpy(item_name, item->name, sizeof(item_name));
    if (applier == object_get_player_container(item))
        is_player_env = 1;

    save_throw_object(item, AT_FIRE, applier);

    /* Change to check count and not freed, since the object pointer
        * may have gotten recycled */
    if ((nrof != item->nrof) || (count != item->count)) {
        draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
            "You light the %s with the %s.",
            item_name, lighter_name);

        /* Need to update the player so that the players glow radius
         * gets changed. */
        if (is_player_env)
            fix_object(applier);
    } else {
        draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
            "You attempt to light the %s with the %s and fail.",
            item->name, lighter_name);
    }
}
