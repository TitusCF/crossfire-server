/*
 * static char *rcsid_object_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
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
 * Defines for loader.l / loader.c.
 */

#ifndef LOADER_H
#define LOADER_H

#define LL_IGNORED  -1
#define LL_EOF      0
#define LL_MORE     1
#define LL_NORMAL   2

/* see loader.l for more details on this */
#define LO_REPEAT   0
#define LO_LINEMODE 1
#define LO_NEWFILE  2
#define LO_NOREAD   3

extern int nrofpixmaps;

#endif /* LOADER_H */
