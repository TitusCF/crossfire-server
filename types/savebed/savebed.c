/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * Implement beds that are applied to save a player to disk.
 */

#include "global.h"

#include <string.h>

#include "ob_methods.h"
#include "ob_types.h"
#include "sounds.h"
#include "sproto.h"

static method_ret savebed_type_apply(ob_methods *context, object *op, object *applier, int aflags);
static void apply_savebed(object *pl);

/**
 * Initializer for the SAVEBED object type.
 */
void init_type_savebed(void) {
    register_apply(SAVEBED, savebed_type_apply);
}

/**
 * Attempts to apply a savebed.
 * @param context The method context
 * @param op The savebed to apply
 * @param applier The object attempting to apply the savebed. Ignore if not
 *   a player
 * @param aflags Special flags (always apply/unapply)
 * @return The return value is always METHOD_OK
 */
static method_ret savebed_type_apply(ob_methods *context, object *op, object *applier, int aflags) {
    if (applier->type == PLAYER)
        apply_savebed(applier);
    return METHOD_OK;
}

/**
 * Apply a bed to reality.
 *
 * @param pl
 * player who is applying the bed.
 */
static void apply_savebed(object *pl) {
    /* Refuse to save a player without any experience. */
    if (!pl->stats.exp) {
        draw_ext_info_format(NDI_UNIQUE | NDI_RED, 5, pl,
                MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOADSAVE,
                "You must gain a bit of experience before you can save.");
        return;
    }

    /* Lauwenmark : Here we handle the LOGOUT global event */
    execute_global_event(EVENT_LOGOUT, pl->contr, pl->contr->socket.host);

    /* Need to call pets_terminate_all()  before we remove the player ob */
    pets_terminate_all(pl);
    object_remove(pl);
    pl->direction = 0;
    draw_ext_info_format(NDI_UNIQUE|NDI_ALL|NDI_DK_ORANGE, 5, pl,
            MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_PLAYER,
            "%s leaves the game.", pl->name);

    /* update respawn position */
    safe_strncpy(pl->contr->savebed_map, pl->map->path,
            sizeof(pl->contr->savebed_map));
    pl->contr->bed_x = pl->x;
    pl->contr->bed_y = pl->y;

    strcpy(pl->contr->killer, "left");
    hiscore_check(pl, 0); /* Always check score */
    save_player(pl, 0);
    party_leave(pl);
#if MAP_MAXTIMEOUT
    MAP_SWAP_TIME(pl->map) = MAP_TIMEOUT(pl->map);
#endif
    play_again(pl);
    pl->speed = 0;
    object_update_speed(pl);
}
