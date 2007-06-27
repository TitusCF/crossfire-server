/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2007 Mark Wedel & Crossfire Development Team
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

/** @file book.h
 * Describes fundental parameters of 'books' - objects with type==BOOK
 */

#ifndef BOOK_H
#define BOOK_H

/**
 * Maximum message buf size for books. If this is changed, keep in mind that big
 * strings may be unreadable by the player as the tail of the message can scroll
 * over the beginning (as of v0.92.2).
 * @note
 * Note that the book messages are stored in the msg buf,
 * which is limited by 'HUGE_BUF' in the loader.
 */
#define BOOK_BUF        HUGE_BUF-10

/**
 * Defines the base value that BOOKSIZE should return for books with no weight.
 * @note
 * If little books arent getting enough text generated, enlarge this.
 */
#define BASE_BOOK_BUF   250

/**
 * Get the book buffer size for an individual book object. Make heavy books hold
 * more text but never let a book msg buffer exceed the max. size (BOOK_BUF)
 * @param xyz
 * The book object to calculate the buffer size for.
 */
#define BOOKSIZE(xyz)   BASE_BOOK_BUF+((xyz)->weight/10)>BOOK_BUF? \
                                BOOK_BUF:BASE_BOOK_BUF+((xyz)->weight/10);
/**
 * Struct to store the message_type and message_subtype for signs and books used by the player.
 */
typedef struct {
	uint8 message_type;     /**< Message type to be sent to the client. */
	uint8 message_subtype;  /**< Message subtype to be sent to the client. */
} readable_message_type;

#endif /* BOOK_H */
