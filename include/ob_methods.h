/*
 * static char *rcsid_ob_methods_h =
 *   "$Id: define.h 4998 2006-10-06 18:27:07Z qal21 $";
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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

#ifndef OB_METHODS_H
#define OB_METHODS_H

/**
 * Define some standard return values for callbacks which don't need to return
 * any other results. Later this might be expanded into a more complex return
 * value system if necessary
 */
typedef char method_ret;
#define METHOD_OK 0
#define METHOD_UNHANDLED 1
#define METHOD_ERROR 2

/**
 * Typedefs for ob_methods. Also used in ob_types.c for storing the pointers
 * temporarily. As a convention, callbacks parameters should begin with
 * "ob_methods *context, object *ob", and return method_ret unless it needs to
 * return something else.
 */
/* Example:
 * typedef method_ret (apply_func)(ob_methods *context, object *ob);
 */

/**
 * @struct ob_methods
 * This struct stores function pointers for actions that can be done to objects.
 * It is currently just used for type-specific object code. Add new function
 * pointers here when moving type specific code into the server/types/ *.c area.
 * When adding function pointers here, be sure to add to init_ob_methods() in
 * ob_types.c as necessary.
 */
typedef struct ob_methods ob_methods;
typedef method_ret  (*apply_func)(ob_methods*, object*, object*, int);
typedef method_ret  (*process_func)(ob_methods*, object*);
typedef const char* (*describe_func)(ob_methods*, object*, object*);
struct ob_methods
{
    apply_func      apply;          /**< The apply method */
    process_func    process;        /**< The process method */
    describe_func   describe;       /**< The describe method */
    struct ob_methods *fallback;    /**< ob_method structure to fallback to */
/* Example:
 * apply_func *apply;
 */
};

#endif /* OB_METHODS_H */
