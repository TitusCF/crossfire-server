/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
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
#include <sproto.h>

/** @file process.c
 * Legacy implementation of the process method.
 */
method_ret legacy_ob_process(ob_methods *context, object *op)
{
    switch(op->type)
    {
        case SPELL_EFFECT:
            move_spell_effect(op);
            return 1;

        case ROD:
        case HORN:
            regenerate_rod(op);
            return 1;

        case FORCE:
        case POTION_EFFECT:
            legacy_remove_force(op);
            return 1;

        case BLINDNESS:
            legacy_remove_blindness(op);
            return 0;

        case POISONING:
            legacy_poison_more(op);
            return 0;

        case DISEASE:
            move_disease(op);
            return 0;

        case SYMPTOM:

            move_symptom(op);
            return 0;

        case DOOR:
            remove_door(op);
            return 0;

        case LOCKED_DOOR:
            remove_door2(op);
            return 0;

        case TELEPORTER:
            move_teleporter(op);
            return 0;

        case GOLEM:
            move_golem(op);
            return 0;

        case EARTHWALL:
            hit_player(op, 2, op, AT_PHYSICAL, 1);
            return 0;

        case FIREWALL:
            move_firewall(op);
            if (op->stats.maxsp)
                animate_turning(op);
            return 0;

        case MOOD_FLOOR:
            do_mood_floor(op, op);
            return 0;

        case GATE:
            legacy_move_gate(op);
            return 0;

        case TIMED_GATE:
            legacy_move_timed_gate(op);
            return 0;

        case TRIGGER_BUTTON:
        case TRIGGER_PEDESTAL:
        case TRIGGER_ALTAR:
            legacy_animate_trigger(op);
            return 0;

        case DETECTOR:
            legacy_move_detector(op);

        case DIRECTOR:
            if (op->stats.maxsp)
                animate_turning(op);
            return 0;

        case HOLE:
            legacy_move_hole(op);
            return 0;

        case RUNE:
        case TRAP:
            move_rune(op);
            return 0;

        case PLAYERMOVER:
            move_player_mover(op);
            return 0;

        case CREATOR:
            move_creator(op);
            return 0;

        case MARKER:
            move_marker(op);
            return 0;

        case PLAYER_CHANGER:
            move_player_changer(op);
            return 0;

        case PEACEMAKER:
            move_peacemaker(op);
            return 0;
    }
    return 0;
}
