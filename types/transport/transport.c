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

/** @file transport.c
 * The implementation of the Transport class of objects.
 * A transport is basically little more than a container in which players
 * can enter, and that can be moved around on the map.
 */
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>

/**
 * Initializer for the TRANSPORT object type.
 */
void init_type_transport()
{
    register_apply(TRANSPORT, transport_type_apply);
    register_process(TRANSPORT, transport_type_process);
}

/**
 * Player is trying to use a transport.  This returns same values as
 * manual_apply() does.  This function basically checks to see if
 * the player can use the transport, and if so, sets up the appropriate
 * pointers.
 * @param context The method context
 * @param op The Transport to apply
 * @param applier The object attempting to apply the Transport
 * @param aflags Special flags (always apply/unapply)
 * @retval 0 If the applier was not a player
 * @retval 1 If the applier was a player
 */
method_ret transport_type_apply(ob_methods *context, object *op,
    object* applier, int aflags)
{
    object* old_transport = applier->contr->transport;
    object* inv;

    /* Only players can use transports right now */
    if (applier->type != PLAYER)
        return 0;

    /* If player is currently on a transport but not this transport, they need
     * to exit first.  Perhaps transport to transport transfers should be
     * allowed.
     */
    if (old_transport && old_transport != op)
    {
        draw_ext_info_format(NDI_UNIQUE, 0, applier,
            MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
            "You must exit %s before you can board %s.",
            "You must exit %s before you can board %s.",
            query_name(old_transport), query_name(op));
        return 1;
    }

    /* player is currently on a transport.  This must mean he
     * wants to exit.
     */
    if (old_transport)
    {
        /* Should we print a message if the player only wants to
         * apply?
         */
        if (aflags & AP_APPLY)
            return 1;

        draw_ext_info_format(NDI_UNIQUE, 0, applier,
            MSG_TYPE_APPLY, MSG_TYPE_APPLY_UNAPPLY,
            "You disembark from %s.", "You disembark from %s.",
            query_name(old_transport));
        remove_ob(applier);
        applier->map = old_transport->map;
        applier->x = old_transport->x;
        applier->y = old_transport->y;
        if (applier->contr == old_transport->contr)
            old_transport->contr = NULL;

        applier->contr->transport = NULL;
        insert_ob_in_map(applier, applier->map, applier, 0);
        sum_weight(old_transport);

        /* Possible for more than one player to be using a transport.
         * if that is the case, we don't want to reset the face, as the
         * transport is still occupied.
         */
        for (inv=old_transport->inv; inv; inv=inv->below)
            if (inv->type == PLAYER) break;
        if (!inv)
        {
            old_transport->face = old_transport->arch->clone.face;
            old_transport->animation_id = old_transport->arch->clone.animation_id;
        }
        else
        {
            old_transport->contr = inv->contr;
            draw_ext_info_format(NDI_UNIQUE, 0, inv,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                "%s has disembarked.  You are now the captain of %s",
                "%s has disembarked.  You are now the captain of %s",
                applier->name, query_name(old_transport));
        }
        return 1;
    }
    else
    {
        /* player is trying to board a transport */
        int pc=0, p_limit;
        const char *kv;

        if (aflags & AP_UNAPPLY)
            return 1;

        /* Can this transport hold the weight of this player? */
        if (!transport_can_hold(op, applier, 1))
        {
            draw_ext_info_format(NDI_UNIQUE, 0, applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                "The %s is unable to hold your weight!",
                "The %s is unable to hold your weight!",
                query_name(op));
            return 1;
        }

        /* If the player is holding the transport, drop it. */
        if (op->env == applier)
        {
            old_transport = op;
            /* Don't drop transports in shops. */
            if (!is_in_shop(applier))
            {
                op = drop_object(applier, op, 1);
            }
            else
            {
                draw_ext_info_format(NDI_UNIQUE, 0, applier,
                    MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                    "You cannot drop the %s in a shop to use it.",
                    "You cannot drop the %s in a shop to use it.",
                    query_name(old_transport));
                return 1;
            }
            /* Did it fail to drop? */
            if(!op)
            {
                draw_ext_info_format(NDI_UNIQUE, 0, applier,
                    MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                    "You need to drop the %s to use it.",
                    "You need to drop the %s to use it.",
                    query_name(old_transport));
                return 1;
            }
        }

        /* Does this transport have space for more players? */
        for (inv=op->inv; inv; inv=inv->below)
            if (inv->type == PLAYER) pc++;

        kv = get_ob_key_value(op, "passenger_limit");
        if (!kv) p_limit=1;
        else p_limit = atoi(kv);
        if (pc >= p_limit)
        {
            draw_ext_info_format(NDI_UNIQUE, 0, applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                "The %s does not have space for any more people",
                "The %s does not have space for any more people",
                query_name(op));
            return 1;
        }

        /* Everything checks out OK - player can get on the transport */
        applier->contr->transport = op;

        if (op->contr)
        {
            draw_ext_info_format(NDI_UNIQUE, 0, applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                "The %s's captain is currently %s",
                "The %s's captain is currently %s",
                query_name(op), op->contr->ob->name);
        }
        else
        {
            draw_ext_info_format(NDI_UNIQUE, 0, applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                "You're the %s's captain",
                "You're the %s's captain",
                query_name(op));
            op->contr = applier->contr;
        }

        remove_ob(applier);
        insert_ob_in_ob(applier, op);
        sum_weight(op);
        applier->map = op->map;
        if (applier->x != op->x || applier->y != op->y)
        {
            esrv_map_scroll(&applier->contr->socket,
                (applier->x - op->x), (applier->y - op->y));
            applier->contr->socket.update_look=1;
            applier->contr->socket.look_position=0;
            applier->x = op->x;
            applier->y = op->y;
        }

        /* Might need to update face, animation info */
        if (!pc)
        {
            const char *str;

            str = get_ob_key_value(op, "face_full");
            if (str)
                op->face = &new_faces[find_face(str, op->face->number)];
            str = get_ob_key_value(op, "anim_full");
            if (str)
                op->animation_id = find_animation(str);
        }

        /* Does speed of this object change based on weight? */
        kv = get_ob_key_value(op, "weight_speed_ratio");
        if (kv)
        {
            int wsr = atoi(kv);
            float base_speed;

            kv = get_ob_key_value(op, "base_speed");
            if (kv) base_speed = atof(kv);
            else base_speed = op->arch->clone.speed;

            op->speed = base_speed - (base_speed * op->carrying * wsr)
                / (op->weight_limit * 100);

            /* Put some limits on min/max speeds */
            if (op->speed < 0.10) op->speed = 0.10;
            if (op->speed > 1.0)  op->speed = 1.0;
        }
    } /* else if player is boarding the transport */
    return 1;
}
/**
 * Processes a Transport.
 * @param context The method context
 * @param op The Transport to process
 * @retval 0 If the remaining speed of the transport was > 0.0
 * @retval 1 If the remaining speed of the transport was < 0.0
 */
method_ret transport_type_process(ob_methods *context, object *op)
{
    /* Transports are directed by players - thus, there
     * speed is reduced when the player moves them about.
     * So give them back there speed here, since process_objects()
     * has decremented it.
     */
    if (op->speed_left < 0.0) op->speed_left += 1.0;
        return 1;
    return 0;
}
