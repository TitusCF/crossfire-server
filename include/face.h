/*
 * static char *rcsid_face_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
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

    The author can be reached via e-mail to mark@pyramid.com
*/

/**
 * @file
 * Face-related structures.
 */

#ifndef FACE_H
#define FACE_H

/**
 * New face structure - this enforces the notion that data is face by
 * face only - you can not change the color of an item - you need to instead
 * create a new face with that color.
 */
typedef struct new_face_struct {
    uint16	number;		/**< This is the image id.  It should be the
                          * same value as its position in the array */
    const char	*name;  /**< Face name, as used by archetypes and such. */
    uint8	visibility;
    uint8	magicmap;	/**< Color to show this in magic map */
} New_Face;

/**
 * This represents one animation. It points to different faces.
 */
typedef struct {
    const char *name;       /**< Name of the animation sequence */
    uint8 num_animations;   /**< How many different faces to animate */
    uint8 facings;          /**< How many facings (1,2,4,8) */
    Fontindex *faces;       /**< The different animations */
    uint16  num;            /**< Where we are in the array */
} Animations;

#endif /* FACE_H */
