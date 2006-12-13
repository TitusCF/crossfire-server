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

#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>

static ob_methods base_type;
static ob_methods legacy_type;

/**
 * Initializes a ob_methods struct. Make sure this always matches ob_methods.h
 * @param methods ob_method structure to initialize
 * @param fallback Default fallback for the ob_method
 */
void init_ob_method_struct(ob_methods *methods, ob_methods *fallback) {
    methods->fallback = fallback;
    /* Example:
     * methods->apply = NULL;
     */
}

/**
 * Initializes a the ob_method system. This means initializing legacy_type,
 * base_type, and also calling init_ob_types() from ob_types.c
 */
void init_ob_methods() {
    /* Init legacy_type. Note, this is just used as a transitionary fallback
     * until refactoring of type-specific code is complete, and when it is this
     * ob_methods struct should be removed.
     */
    init_ob_method_struct(&legacy_type, NULL);
    /* legacy_type->foobar = legacy_ob_foobar; */
    
    /* Init base_type, inheriting from legacy_type. The base_type is susposed to
     * be a base class of object that all other object types inherit methods
     * they don't handle individually. Things such as generic drop/pickup code
     * should go here, in addition some other things such as "I don't know how
     * to apply that." messages should be handled from here.
     */
    init_ob_method_struct(&base_type, &legacy_type);
    /* base_type->foobar = common_ob_foobar; */
    
    /* Init object types methods, inheriting from base_type. */
    init_ob_types(&base_type);
}

/*
 * The following functions are meant for calling methods. No actual behavoir
 * logic should be contained in this code. Code in the common/ directory should
 * be used for logic common to all types, and should always be called by
 * individual method code (i.e. all apply methods should call 'can_apply' from
 * common/). Defaults for all types should not be put here either, as that code
 * belongs in the common/ directory also, referenced to by base_type.
 */

/*
 * Example:
 * int ob_apply(object *ob) {
 *     ob_methods *methods;
 *     for (methods = type_methods[ob->type]; methods; methods = methods->fallback)
 *         if (methods->apply)
 *             return methods->apply(methods, ob);
 *     return METHOD_UNHANDLED;
 * }
 */
