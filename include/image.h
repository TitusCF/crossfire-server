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

/**
 * @file
 * Image-related structures.
 */

#ifndef IMAGE_H
#define IMAGE_H

/** Actual image data the client will display. */
typedef struct face_info {
    uint8 *data;            /**< Image data. */
    uint16 datalen;         /**< Length of data. */
    uint32 checksum;        /**< Checksum of face data. */
} face_info;

/** Information about one face set */
typedef struct {
    char    *prefix;    /**< Faceset short name, used in pictures names (base, clsc). */
    char    *fullname;  /**< Full faceset name. */
    uint8   fallback;   /**< Faceset to use when an image is not found in this faceset, index in facesets. */
    char    *size;      /**< Human-readable set size. */
    char    *extension; /**< Supplementary description. */
    char    *comment;   /**< Human-readable comment for this set. */
    face_info   *faces; /**< images in this faceset */
} face_sets;
#define MAX_FACE_SETS   20  /**< Maximum number of image sets the program will handle */

extern face_sets facesets[MAX_FACE_SETS];

#define MAX_IMAGE_SIZE 10000

#endif /* IMAGE_H */
