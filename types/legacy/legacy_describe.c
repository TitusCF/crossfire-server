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
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
/** @file legacy_describe.c
 * Legacy implementation of description-related methods.
 */

const char* legacy_ob_describe(ob_methods* context, object* op, object* observer)
{
    static char buf[VERY_BIG_BUF];
    char *cp;
    if(op==NULL)
        return "";

    buf[0]='\0';
    switch(op->type)
    {
        case RING:
        case SKILL:
        case WEAPON:
        case ARMOUR:
        case BRACERS:
        case HELMET:
        case SHIELD:
        case BOOTS:
        case GLOVES:
        case AMULET:
        case GIRDLE:
        case BOW:
        case ARROW:
        case CLOAK:
        case FOOD:
        case DRINK:
        case FLESH:
        case SKILL_TOOL:
            return common_ob_describe(context, op, observer);
        default:
            if(buf[0]=='\0')
            {
                query_name(op, buf, VERY_BIG_BUF-1);
                buf[VERY_BIG_BUF-1]=0;
            }
            return buf;
    }
}
