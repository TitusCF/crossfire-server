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

/**
 * The maximum characters per account is really driven by the size of
 * the buffer we use to read in the data.  Take 150 characters off for
 * the account name, password, overhead, and other wiggle room and
 * find a maximum.  From my quick calculations, this amounts to
 * 18 characters/account.  I think that is sufficient - moving to a
 * HUGE_BUF would allow 82.
 * The code could be more clever and look at the length of each
 * character name and total it up, but having the same limit for everyone
 * is better IMO.
 */
#define MAX_CHARACTERS_PER_ACCOUNT (VERY_BIG_BUF - 150) / (MAX_NAME+1)


/**
 * One character account.
 */
typedef struct account_char_struct {
    const char *name;         /**< Name of this character/player */
    const char *character_class;        /**< Class of this character */
    const char *race;         /**< Race of this character */
    uint8 level;              /**< Level of this character */
    const char *face;         /**< Face of this character */
    const char *party;        /**< Character this party belonged to */
    const char *map;          /**< Last map this character was on */
    struct account_char_struct  *next;
} Account_Char;

#endif /* OBJECT_H */
