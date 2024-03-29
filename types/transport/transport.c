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
 * The implementation of the Transport class of objects.
 * A transport is basically little more than a container in which players
 * can enter, and that can be moved around on the map.
 */

#include "global.h"

#include <stdlib.h>

#include "ob_methods.h"
#include "ob_types.h"
#include "shop.h"
#include "sounds.h"
#include "sproto.h"

static method_ret transport_type_apply(object *op, object *applier, int aflags);
static method_ret transport_type_process(object *op);

/**
 * Initializer for the TRANSPORT object type.
 */
void init_type_transport(void) {
    register_apply(TRANSPORT, transport_type_apply);
    register_process(TRANSPORT, transport_type_process);
}

/**
 * Player is trying to use a transport.  This returns same values as
 * apply_manual() does.  This function basically checks to see if
 * the player can use the transport, and if so, sets up the appropriate
 * pointers.
 * @param op The Transport to apply
 * @param applier The object attempting to apply the Transport
 * @param aflags Special flags (always apply/unapply)
 */
static method_ret transport_type_apply(object *op, object *applier, int aflags) {
    object *old_transport = applier->contr->transport;
    object *inv;
    char name_op[MAX_BUF], name_old[MAX_BUF];

    /* Only players can use transports right now */
    if (applier->type != PLAYER)
        return METHOD_UNHANDLED;

    query_name(op, name_op, MAX_BUF);

    /* If player is currently on a transport but not this transport, they need
     * to exit first.  Perhaps transport to transport transfers should be
     * allowed.
     */
    if (old_transport && old_transport != op) {
        query_name(old_transport, name_old, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
            "You must exit %s before you can board %s.",
            name_old, name_op);
        return METHOD_SILENT_ERROR;
    }

    /* player is currently on a transport.  This must mean he
     * wants to exit.
     */
    if (old_transport) {
        /* Should we print a message if the player only wants to
         * apply?
         */
        if (aflags&AP_APPLY)
            return 1;

        query_name(old_transport, name_old, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_UNAPPLY,
            "You disembark from %s.",
            name_old);
        object_remove(applier);
        if (applier->contr == old_transport->contr)
            old_transport->contr = NULL;

        applier->contr->transport = NULL;
        object_insert_in_map_at(applier, old_transport->map, applier, 0, old_transport->x, old_transport->y);
        if ( applier->map ) applier->map->players--; // Incremented player count incorrectly; adjust to correct
        object_sum_weight(old_transport);

        /* Possible for more than one player to be using a transport.
         * if that is the case, we don't want to reset the face, as the
         * transport is still occupied.
         */
        inv = object_find_by_type(old_transport, PLAYER);
        if (!inv) {
            old_transport->face = old_transport->arch->clone.face;
            old_transport->animation = old_transport->arch->clone.animation;
        } else {
            old_transport->contr = inv->contr;
            draw_ext_info_format(NDI_UNIQUE, 0, inv, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                "%s has disembarked.  You are now the captain of %s",
                applier->name, name_old);
        }
        return METHOD_OK;
    } else {
        /* player is trying to board a transport */
        int pc = 0, p_limit;
        const char *kv;
        int16_t ox, oy;

        if (aflags&AP_UNAPPLY)
            return 1;

        /* Can this transport hold the weight of this player? */
        if (!transport_can_hold(op, applier, 1)) {
            draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                "The %s is unable to hold your weight!",
                name_op);
            return METHOD_SILENT_ERROR;
        }

        /* If the player is holding the transport, drop it. */
        if (op->env == applier) {
            old_transport = op;
            /* Don't drop transports in shops. */
            if (!shop_contains(applier)) {
                op = drop_object(applier, op, 1);
            } else {
                draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                    "You cannot drop the %s in a shop to use it.",
                    name_old);
                return METHOD_SILENT_ERROR;
            }
            /* Did it fail to drop? */
            if (!op) {
                draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                    "You need to drop the %s to use it.",
                    name_old);
                return METHOD_SILENT_ERROR;
            }
        }

        /* Does this transport have space for more players? */
        FOR_INV_PREPARE(op, inv)
            if (inv->type == PLAYER)
                pc++;
        FOR_INV_FINISH();

        kv = object_get_value(op, "passenger_limit");
        if (!kv)
            p_limit = 1;
        else
            p_limit = atoi(kv);
        if (pc >= p_limit) {
            draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                "The %s does not have space for any more people",
                name_op);
            return METHOD_SILENT_ERROR;
        }

        /* Everything checks out OK - player can get on the transport */
        applier->contr->transport = op;

        if (op->contr) {
            draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                "The %s's captain is currently %s",
                name_op, op->contr->ob->name);
        } else {
            draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                "You're the %s's captain",
                name_op);
            op->contr = applier->contr;
        }

        if ( applier->map ) applier->map->players++; // About to subtract the player incorrectly
        object_remove(applier);
        /* object_insert_in_ob clear applier->x and applier->y, so store them away */
        ox = applier->x;
        oy = applier->y;
        object_insert_in_ob(applier, op);
        object_sum_weight(op);
        applier->map = op->map;
        if (ox != op->x || oy != op->y) {
            esrv_map_scroll(&applier->contr->socket, (ox-op->x), (oy-op->y));
        }
        applier->contr->socket.update_look = 1;
        applier->contr->socket.look_position = 0;
        applier->x = op->x;
        applier->y = op->y;

        /* Might need to update face, animation info */
        if (!pc) {
            const char *str;

            str = object_get_value(op, "face_full");
            if (str)
                op->face = try_find_face(str, op->face);
            str = object_get_value(op, "anim_full");
            if (str)
                op->animation = find_animation(str);
        }

        /* Does speed of this object change based on weight? */
        kv = object_get_value(op, "weight_speed_ratio");
        if (kv) {
            int wsr = atoi(kv);
            float base_speed;

            kv = object_get_value(op, "base_speed");
            if (kv)
                base_speed = atof(kv);
            else
                base_speed = op->arch->clone.speed;

            op->speed = base_speed-(base_speed*op->carrying*wsr)/(op->weight_limit*100);

            /* Put some limits on min/max speeds */
            if (op->speed < 0.10)
                op->speed = 0.10;
            if (op->speed > 1.0)
                op->speed = 1.0;
        }
    } /* else if player is boarding the transport */
    return METHOD_OK;
}

/**
 * Processes a Transport.
 * @param context The method context
 * @param op The Transport to process
 * @retval 0 If the remaining speed of the transport was > 0.0
 * @retval 1 If the remaining speed of the transport was < 0.0
 */
static method_ret transport_type_process(object *op) {
    /* Transports are directed by players - thus, there
     * speed is reduced when the player moves them about.
     * So give them back there speed here, since process_objects()
     * has decremented it.
     */
    if (op->speed_left < 0.0) {
        op->speed_left += 1.0;
        return 1;
    }
    return 0;
}
