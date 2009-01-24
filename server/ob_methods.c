/*
 * static char *rcsid_ob_methods =
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

/**
 * @file
 * Those functions deal with the object/type system.
 */

#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>

#ifndef __CEXTRACT__
#include <sproto.h>
#endif

static ob_methods base_type;

static ob_methods legacy_type;

/**
 * Initializes the ob_method system. This means initializing legacy_type,
 * base_type, and also calling init_ob_types() from ob_types.c
 */
void init_ob_methods(void) {
    /* Init legacy_type. Note, this is just used as a transitionary fallback
     * until refactoring of type-specific code is complete, and when it is this
     * ob_methods struct should be removed.
     */
    init_ob_method_struct(&legacy_type, NULL);
    legacy_type.apply = legacy_ob_apply;
    legacy_type.process = legacy_ob_process;
    legacy_type.describe = legacy_ob_describe;
    legacy_type.move_on = NULL;

    /* Init base_type, inheriting from legacy_type. The base_type is susposed to
     * be a base class of object that all other object types inherit methods
     * they don't handle individually. Things such as generic drop/pickup code
     * should go here, in addition some other things such as "I don't know how
     * to apply that." messages should be handled from here.
     */
    init_ob_method_struct(&base_type, &legacy_type);
    /* base_type.foobar = common_ob_foobar; */

    /* Init object types methods, inheriting from base_type. */
    init_ob_types(&base_type);
    register_all_ob_types();
}
