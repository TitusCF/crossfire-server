/*
 * static char *rcsid_material_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2000 Mark Wedel
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

    The author can be reached via e-mail to mwedel@scruz.net
*/

/* #defines are needed by living.h, so they must be loaded early */
#ifndef MATERIAL_H_1
#define MATERIAL_H_1

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

typedef struct {
  char	*name;
  sint8	 save[NROFATTACKS];
} materialtype;

EXTERN materialtype material[NROFMATERIALS];


#else	/* MATERIAL_H_1 */
/* the array get initialized later, and only once */
#ifndef MATERIAL_H_2
#define MATERIAL_H_2


materialtype material[NROFMATERIALS] = {
  /*  		  P  M  F  E  C  C  A  D  W  G  P S P T F  C D D C C G H B  I *
   *		  H  A  I  L  O  O  C  R  E  H  O L A U E  A E E H O O O L  N *
   *		  Y  G  R  E  L  N  I  A  A  O  I O R R A  N P A A U D L I  T *
   *		  S  I  E  C  D  F  D  I  P  S  S W A N R  C L T O N   Y N  R *
   *		  I  C     T     U     N  O  T  O   L      E E H S T P   D  N */
  {"paper", 	{15,10,17, 9, 5, 7,13, 0,20,15, 0,0,0,0,0,10,0,0,0,0,0,0,0,0}},
  {"iron", 	{ 2,12, 3,12, 2,10, 7, 0,20,15, 0,0,0,0,0,10,0,0,0,0,0,0,0,0}},
  {"glass", 	{14,11, 8, 3,10, 5, 1, 0,20,15, 0,0,0,0,0, 0,0,0,0,0,0,0,0,0}},
  {"leather", 	{ 5,10,10, 3, 3,10,10, 0,20,15, 0,0,0,0,0,12,0,0,0,0,0,0,0,0}},
  {"wood", 	{10,11,13, 2, 2,10, 9, 0,20,15, 0,0,0,0,0,12,0,0,0,0,0,0,0,0}},
  {"organics", 	{ 3,12, 9,11, 3,10, 9, 0,20,15, 0,0,0,0,0, 0,0,0,0,0,0,0,0,0}},
  {"stone", 	{ 2, 5, 2, 2, 2, 2, 1, 0,20,15, 0,0,0,0,0, 5,0,0,0,0,0,0,0,0}},
  {"cloth", 	{14,11,13, 4, 4, 5,10, 0,20,15, 0,0,0,0,0, 5,0,0,0,0,0,0,0,0}},
  {"adamant", 	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0, 0,0,0,0,0,0,0,0,0}},
  {"liquid", 	{ 0, 8, 9, 6,17, 0,15, 0,20,15,12,0,0,0,0,11,0,0,0,0,0,0,0,0}},
  {"soft metal",{ 6,12, 6,14, 2,10, 1, 0,20,15, 0,0,0,0,0,10,0,0,0,0,0,0,0,0}},
  {"bone", 	{10, 9, 4, 5, 3,10,10, 0,20,15, 0,0,0,0,0, 2,0,0,0,0,0,0,0,0}},
  {"ice", 	{14,11,16, 5, 0, 5, 6, 0,20,15, 0,0,0,0,0, 7,0,0,0,0,0,0,0,0}}
};

#endif

#endif
