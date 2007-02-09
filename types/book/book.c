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
/** @file book.c
 * The implementation of the Book class of objects.
 */
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>
#include <sproto.h>

/**
 * Initializer for the BOOK object type.
 */
void init_type_book()
{
    register_apply(BOOK, book_type_apply);
}

/**
 * Handles reading a regular (ie not containing a spell) book.
 * @param context The method context
 * @param op The Book to apply
 * @param applier The object attempting to apply the Book
 * @param aflags Special flags (always apply/unapply)
 * @retval 0 If the Book wasn't read by a player
 * @retval 1 If applier was a player
 */
method_ret book_type_apply(ob_methods *context, object *op,
    object* applier, int aflags)
{
    int lev_diff;
    object *skill_ob;

    if (applier->type != PLAYER)
        return 0;

    if(QUERY_FLAG(applier, FLAG_BLIND)&&!QUERY_FLAG(applier,FLAG_WIZ))
    {
        draw_ext_info(NDI_UNIQUE, 0,applier,
            MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
            "You are unable to read while blind.", NULL);
        return 1;
    }
    if(op->msg==NULL)
    {
        draw_ext_info_format(NDI_UNIQUE, 0, applier,
            MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
            "You open the %s and find it empty.",
            "You open the %s and find it empty.",
            op->name);
        return 1;
    }

    /* need a literacy skill to read stuff! */
    skill_ob = find_skill_by_name(applier, op->skill);
    if ( ! skill_ob)
    {
        draw_ext_info(NDI_UNIQUE, 0,applier,
            MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
            "You are unable to decipher the strange symbols.", NULL);
        return 1;
    }
    lev_diff = op->level - (skill_ob->level + 5);
    if (!QUERY_FLAG(applier, FLAG_WIZ) && lev_diff > 0)
    {
        if (lev_diff < 2)
            draw_ext_info(NDI_UNIQUE, 0,applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                "This book is just barely beyond your comprehension.", NULL);
        else if (lev_diff < 3)
            draw_ext_info(NDI_UNIQUE, 0,applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                "This book is slightly beyond your comprehension.", NULL);
        else if (lev_diff < 5)
            draw_ext_info(NDI_UNIQUE, 0,applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                "This book is beyond your comprehension.", NULL);
        else if (lev_diff < 8)
            draw_ext_info(NDI_UNIQUE, 0,applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                "This book is quite a bit beyond your comprehension.", NULL);
        else if (lev_diff < 15)
            draw_ext_info(NDI_UNIQUE, 0,applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                "This book is way beyond your comprehension.", NULL);
        else
            draw_ext_info(NDI_UNIQUE, 0,applier,
                MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                "This book is totally beyond your comprehension.", NULL);
        return 1;
    }


    /* Lauwenmark: Handle for plugin book event */
    /*printf("Book apply: %s\n", tmp->name);
    execute_event(tmp, EVENT_APPLY,op,NULL,SCRIPT_FIX_ALL);
    printf("Book applied: %s\n", tmp->name);*/
    {
        readable_message_type* msgType = get_readable_message_type(op);
        draw_ext_info_format(NDI_UNIQUE | NDI_NAVY, 0, applier,
            msgType->message_type, msgType->message_subtype,
            "You open the %s and start reading.\n%s",
            "You open the %s and start reading.\n%s",
            long_desc(op,applier), op->msg);
    }

    /* gain xp from reading */
    if(!QUERY_FLAG(op,FLAG_NO_SKILL_IDENT))
    { /* only if not read before */
        int exp_gain=calc_skill_exp(applier,op, skill_ob);
        if(!QUERY_FLAG(op,FLAG_IDENTIFIED))
        {
            /*exp_gain *= 2; because they just identified it too */
            SET_FLAG(op,FLAG_IDENTIFIED);
            /* If in a container, update how it looks */
            if(op->env)
                esrv_update_item(UPD_FLAGS|UPD_NAME, applier,op);
            else
                applier->contr->socket.update_look=1;
        }
        change_exp(applier,exp_gain, skill_ob->skill, 0);
        /* so no more xp gained from this book */
        SET_FLAG(op,FLAG_NO_SKILL_IDENT);
    }
    return 1;
}
