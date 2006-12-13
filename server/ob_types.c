/*
 * static char *rcsid_ob_types =
 *   "$Id: build_map.c 5057 2006-10-29 07:50:09Z mwedel $";
 */
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
#include <ob_types.h>
#include <ob_methods.h>

ob_methods type_methods[OBJECT_TYPE_MAX];

/**
 * Initializes the type_methods structure and runs the intialization functions
 * for all individual types.
 * @param base_type The default ob_methods fallback for all types
 */
void init_ob_types(ob_methods *base_type) {
    int tmp;
    for (tmp=0; tmp < OBJECT_TYPE_MAX; tmp++)
        init_ob_method_struct(&type_methods[tmp], base_type);
    
    /* init_type_foobar() here, where foobar is for a type. In other words,
     * from here, call functions that register object methods for types.
     */
}

/* Functions for registering methods for types */
/*
 * Example:
 * void register_apply(int ob_type, apply_func *methodptr) {
 *    type_methods[ob_type]->apply = methodptr;
 * }
 */
