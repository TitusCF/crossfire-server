/*
 * static char *rcsid_material_h =
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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/* #defines are needed by living.h, so they must be loaded early */
#ifndef MATERIAL_H
#define MATERIAL_H

#define NROFMATERIALS		13

#define M_PAPER			1
#define M_IRON			2
#define M_GLASS			4
#define M_LEATHER		8
#define M_WOOD			16
#define M_ORGANIC		32
#define M_STONE			64
#define M_CLOTH			128
#define M_ADAMANT		256
#define M_LIQUID		512
#define M_SOFT_METAL		1024
#define M_BONE			2048
#define M_ICE			4096
#define M_SPECIAL		8192	/* when displaying names, don't show the
								   materialname */

typedef struct _materialtype {
    const char	*name;
	const char 	*description;
    int		material;
    sint8	save[NROFATTACKS];
    sint8	mod[NROFATTACKS];
    sint8	chance;
    sint8	difficulty;
    sint8	magic;
    sint8	damage;
    sint8	wc;
    sint8	ac;
	sint8	sp;
    int		weight;
    int		value;
    struct _materialtype *next;
} materialtype_t;

EXTERN materialtype_t *materialt;

#endif	/* MATERIAL_H */

