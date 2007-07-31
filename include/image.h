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

#ifndef IMAGE_H
#define IMAGE_H

/** Information about one image */
typedef struct face_info {
    uint8 *data;		    /**< image data */
    uint16 datalen;	    /**< length of the xpm data */
    uint32 checksum;	    /**< Checksum of face data */
} face_info;

/** Information about one face set */
typedef struct {
    char    *prefix;    /**< */
    char    *fullname;
    uint8   fallback;   /**< faceset to use when an image is not found in this faceset */
    char    *size;
    char    *extension;
    char    *comment;
    face_info	*faces; /**< images in this faceset */
} face_sets;
#define MAX_FACE_SETS	20  /**< Maximum number of image sets the program will handle */

extern face_sets facesets[MAX_FACE_SETS];

#define MAX_IMAGE_SIZE 10000

#endif /* IMAGE_H */
