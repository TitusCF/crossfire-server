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

/**
 * @file
 * Those functions deal with the object/type system.
 */

#include <global.h>
#include <ob_types.h>
#include <ob_methods.h>

#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/** Registered method handlers. */
ob_methods type_methods[OBJECT_TYPE_MAX];

/**
 * Initializes a ob_methods struct. Make sure this always matches ob_methods.h
 * @param methods ob_method structure to initialize
 * @param fallback Default fallback for the ob_method
 */
void init_ob_method_struct(ob_methods *methods, ob_methods *fallback) {
    methods->fallback = fallback;
    methods->apply = NULL;
    methods->process = NULL;
    methods->describe = NULL;
    methods->move_on = NULL;
    methods->trigger = NULL;
}

/**
 * Initializes the object system.
 *
 * @param base_type
 * base type to use as a base for all types.
 * @todo when migration is complete, the parameter should go, and this function should be called from
 * init_library() instead of init_ob_methods() in server/ob_methods.c.
 */
void init_ob_types(ob_methods *base_type) {
    int tmp;

    for (tmp = 0; tmp < OBJECT_TYPE_MAX; tmp++)
        init_ob_method_struct(&type_methods[tmp], base_type);
}

/* Functions for registering methods for types */
/**
 * Registers the apply method for the given type.
 * @param ob_type The type of object to register this method to
 * @param method The method to link
 */
void register_apply(int ob_type, apply_func method) {
    type_methods[ob_type].apply = method;
}

/**
 * Registers the process method for the given type.
 * @param ob_type The type of object to register this method to
 * @param method The method to link
 */
void register_process(int ob_type, process_func method) {
    type_methods[ob_type].process = method;
}

/**
 * Registers the describe method for the given type.
 * @param ob_type The type of object to register this method to
 * @param method The method to link
 */
void register_describe(int ob_type, describe_func method) {
    type_methods[ob_type].describe = method;
}

/**
 * Registers the move_on method for the given type.
 * @param ob_type The type of object to register this method to
 * @param method The method to link
 */
void register_move_on(int ob_type, move_on_func method) {
    type_methods[ob_type].move_on = method;
}

/**
 * Registers the trigger method for the given type.
 * @param ob_type The type of object to register this method to
 * @param method The method to link
 */
void register_trigger(int ob_type, trigger_func method) {
    type_methods[ob_type].trigger = method;
}
