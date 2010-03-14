/*
 * static char *rcsid_account_char_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2010 Mark Wedel & Crossfire Development Team

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

/**
 * @file
 * Characters associated with an account.n
 */


#ifndef ACCOUNT_CHAR_H
#define ACCOUNT_CHAR_H


/** * One body location.
 * See common/item.c.
 */
typedef struct account_char_struct {
    const char *name;         /**< Name of this character/player */
    const char *class;        /**< Class of this character */
    const char *race;         /**< Race of this character */
    uint8 level;              /**< Level of this character */
    const char *face;         /**< Face of this character */
    const char *party;        /**< Character this party belonged to */
    const char *map;          /**< Last map this character was on */
    struct account_char_struct  *next;
} Account_Char;

#endif /* OBJECT_H */
