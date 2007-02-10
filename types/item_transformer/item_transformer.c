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
/** @file item_transformer.c
 * The implementation of Item Transformer class of objects.
 * Basically those items, used with a marked item, transform both items into
 * something else.
 * "Transformer" item has food decreased by 1, removed if 0 (0 at start means
 * unlimited).
 * Change information is contained in the 'slaying' field of the marked item.
 * The format is as follow: transformer:[number ]yield[;transformer:...].
 * This way an item can be transformed in many things, and/or many objects.
 * The 'slaying' field for transformer is used as verb for the action.
 */ 

#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sproto.h>

/**
 * Initializer for the ITEM_TRANSFORMER object type.
 */
void init_type_item_transformer()
{
    register_apply(ITEM_TRANSFORMER, item_transformer_type_apply);
}

/**
 * Applies a transformer object.
 * @param context The method context
 * @param op The Item Transformer to apply
 * @param applier The object that tries to apply the Item Transformer
 * @param aflags Special flags (always apply/unapply)
 * @return This method always returns 1
*/
method_ret item_transformer_type_apply(ob_methods *context, object *op,
    object* applier, int aflags)
{
    object* marked;
    object* new_item;
    char* find;
    char* separator;
    int yield;
    char got[ MAX_BUF ];
    int len;
    char name_o[MAX_BUF], name_m[MAX_BUF];

    if (!op||!applier)
        return 1;
    query_name(op, name_o, MAX_BUF);
    marked = find_marked_object(applier);
    if (!marked)
    {
        draw_ext_info_format(NDI_UNIQUE, 0, applier,
            MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
            "Use the %s with what item?", "Use the %s with what item?",
            name_o);
        return 1;
    }
    query_name(marked, name_m, MAX_BUF);
    if (!marked->slaying)
    {
        draw_ext_info_format(NDI_UNIQUE, 0, applier,
            MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
            "You can't use the %s with your %s!",
            "You can't use the %s with your %s!",
            name_o, name_m);
        return 1;
    }
    /* check whether they are compatible or not */
    find = strstr(marked->slaying,op->arch->name);
    if (!find||(*(find + strlen(op->arch->name ))!=':'))
    {
        draw_ext_info_format( NDI_UNIQUE, 0, applier,
            MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
            "You can't use the %s with your %s!",
            "You can't use the %s with your %s!",
            name_o, name_m);
        return 1;
    }
    find += strlen(op->arch->name) + 1;
    /* Item can be used, now find how many and what it yields */
    if (isdigit(*(find)))
    {
        yield = atoi(find);
        if ( yield < 1 )
        {
            query_base_name(marked,0, name_m, MAX_BUF);
            LOG(llevDebug,
                "apply_item_transformer: item %s has slaying-yield %d.\n",
                name_m, yield);
            yield = 1;
        }
    }
    else
        yield = 1;

    while (isdigit(*find ))
        find++;
    while (*find == ' ')
        find++;
    memset(got, 0, MAX_BUF);
    if ((separator = strchr( find, ';' ))!=NULL)
        len = separator - find;
    else
        len = strlen(find);
    if ( len > MAX_BUF-1)
        len = MAX_BUF-1;
    strcpy( got, find );
    got[len] = '\0';

    /* Now create new item, remove used ones when required. */
    new_item = create_archetype(got);
    if (!new_item)
    {
        query_base_name(marked, 0, name_m, MAX_BUF);
        draw_ext_info_format( NDI_UNIQUE, 0, applier,
            MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
            "This %s is strange, better to not use it.",
            "This %s is strange, better to not use it.",
            name_m);
            return 1;
    }
    new_item->nrof = yield;
    query_base_name(marked,0, name_m, MAX_BUF);
    draw_ext_info_format( NDI_UNIQUE, 0, applier,
        MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
        "You %s the %s.", "You %s the %s.",
        op->slaying, name_m);
    insert_ob_in_ob(new_item, applier);
    esrv_send_inventory(applier, applier);
    /* Eat up one item */
    decrease_ob_nr(marked, 1);
    /* Eat one transformer if needed */
    if (op->stats.food )
        if (--op->stats.food == 0)
            decrease_ob_nr(op, 1);
    return 1;
}
