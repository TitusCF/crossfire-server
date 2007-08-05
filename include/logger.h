/*
 * static char *rcsid_logger_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

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

    The author can be reached via e-mail to mark@pyramid.com.
*/

/**
 * @file
 * Log levels.
 */

#ifndef LOGGER_H
#define LOGGER_H

/** Log levels for the LOG() function. */
typedef enum LogLevel {
  llevError = 0,    /**< Error, serious thing. */
  llevInfo = 1,     /**< Information. */
  llevDebug = 2,    /**< Only for debugging purposes. */
  llevMonster = 3   /**< Many many details. */
} LogLevel;

#endif /* LOGGER_H */
