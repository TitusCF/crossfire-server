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
 * Sound-related defines.
 */

#ifndef SOUNDS_H
#define SOUNDS_H

/**
 * @defgroup Soundtypes Sound types
 */
/*@{*/
#define SOUND_TYPE_LIVING       1
#define SOUND_TYPE_SPELL        2
#define SOUND_TYPE_ITEM         3
#define SOUND_TYPE_GROUND       4
#define SOUND_TYPE_HIT          5
#define SOUND_TYPE_HIT_BY       6
/*@}*/

/**
 * Those flags are for the 'socket.sound' field.
 */
#define SND_EFFECTS     1       /**< Client wands regular sounds. */
#define SND_MUSIC       2       /**< Client wants background music info. */
#define SND_MUTE        64      /**< Don't sent anything for now. */

#define MAX_SOUNDS_TICK     3 /**< Maximum number of sounds a player can receive for each tick. */

#endif /* SOUNDS_H */
