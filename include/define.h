/*
 * static char *rcsid_define_h =
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

/* This file is really too large.  With all the .h files
 * around, this file should be better split between them - things
 * that deal with objects should be in objects.h, things dealing
 * with players in player.h, etc.  As it is, everything just seems
 * to be dumped in here.
 */

/*
 * Crossfire requires ANSI-C, but some compilers "forget" to define it.
 * Thus the prototypes made by cextract don't get included correctly.
 */
#if !defined(__STDC__) 
#error - Your ANSI C compiler should be defining __STDC__
#endif

#include <autoconf.h>

#define FONTDIR ""
#define FONTNAME ""

/* Decstations have trouble with fabs()... */
#define FABS(x) (x<0?-x:x)

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#define MAX_STAT		30	/* The maximum legal value of any stat */
#define MIN_STAT		1	/* The minimum legal value of any stat */

#define MAX_BUF			256	/* Used for all kinds of things */
#define VERY_BIG_BUF		1024
#define HUGE_BUF		4096 /* Used for messages - some can be quite long */
#define SOCKET_BUFLEN		4096	/* Max length a packet could be */

#define FONTSIZE		3000	/* Max chars in font */

#define MAX_ANIMATIONS		64

/* Fatal variables: */
#define OUT_OF_MEMORY		0
#define MAP_ERROR		1
#define ARCHTABLE_TOO_SMALL	2
#define TOO_MANY_ERRORS		3

#define MAX_NAME		16
#define BIG_NAME		32
#define SIZEOFFREE		49

/*
 * Attacktypes:
 */

#define NROFATTACKS		24

#define AT_PHYSICAL	0x00000001 /*       1 */
#define AT_MAGIC	0x00000002 /*       2 */
#define AT_FIRE		0x00000004 /*       4 */
#define AT_ELECTRICITY	0x00000008 /*       8 */
#define AT_COLD		0x00000010 /*      16 */
#define AT_CONFUSION	0x00000020 /*      32 The spell will use this one */
#define AT_ACID		0x00000040 /*      64 Things might corrode when hit */
#define AT_DRAIN	0x00000080 /*     128 */
#define AT_WEAPONMAGIC	0x00000100 /*     256 Very special, use with care */
#define AT_GHOSTHIT	0x00000200 /*     512 Attacker dissolves */
#define AT_POISON	0x00000400 /*    1024 */
#define AT_SLOW		0x00000800 /*    2048 */
#define AT_PARALYZE	0x00001000 /*    4096 */
#define AT_TURN_UNDEAD	0x00002000 /*    8192 */
#define AT_FEAR		0x00004000 /*   16384 */
#define AT_CANCELLATION 0x00008000 /*   32768 ylitalo@student.docs.uu.se */
#define AT_DEPLETE      0x00010000 /*   65536 vick@bern.docs.uu.se */
#define AT_DEATH        0x00020000 /*  131072 peterm@soda.berkeley.edu */
#define AT_CHAOS        0x00040000 /*  262144 peterm@soda.berkeley.edu*/
#define AT_COUNTERSPELL 0x00080000 /*  524288 peterm@soda.berkeley.edu*/
#define AT_GODPOWER	0x00100000 /* 1048576  peterm@soda.berkeley.edu */
#define AT_HOLYWORD	0x00200000 /* 2097152 race selective attack thomas@astro.psu.edu */ 
#define AT_BLIND	0x00400000 /* 4194304 thomas@astro.psu.edu */ 
#define AT_INTERNAL	0x00800000 /* Only used for internal calculations */

#define NROF_SOUNDS		(23 + NROFREALSPELLS)	/* Number of sounds */

/* These are used in the MapLook flags element.  They are not used in
 * in the object flags structure.
 */

#define P_BLOCKSVIEW		0x00000001
#define P_NO_MAGIC		0x00000002 /* Spells (some) can't pass this object */
#define P_NO_PASS		0x00000004 /* Nothing can pass (wall() is true) */
#define P_PASS_THRU		0x00000008 /* */
#define P_IS_ALIVE		0x00000010 /* */
#define P_NO_CLERIC		0x00000020 /* no clerical spells cast here */

#define NROFNEWOBJS(xyz)	((xyz)->stats.food)

/* Note: These values are only a default value, resizing can change them */
#define INV_SIZE		12	/* How many items can be viewed in inventory */
#define LOOK_SIZE		6	/* ditto, but for the look-window */
#define MAX_INV_SIZE		40	/* For initializing arrays */
#define MAX_LOOK_SIZE		40	/* ditto for the look-window */

#define EDITABLE(xyz)		((xyz)->arch->editable)

#define E_MONSTER		0x00000001
#define E_EXIT			0x00000002
#define E_TREASURE		0x00000004
#define E_BACKGROUND		0x00000008
#define E_DOOR			0x00000010
#define E_SPECIAL		0x00000020
#define E_SHOP			0x00000040
#define E_NORMAL		0x00000080
#define E_FALSE_WALL		0x00000100
#define E_WALL			0x00000200
#define E_EQUIPMENT		0x00000400
#define E_OTHER			0x00000800
#define E_ARTIFACT		0x00001000

#define EXIT_PATH(xyz)		(xyz)->slaying
#define EXIT_LEVEL(xyz)		(xyz)->stats.food
#define EXIT_X(xyz)		(xyz)->stats.hp
#define EXIT_Y(xyz)		(xyz)->stats.sp

/* for use by the lighting code */
#define MAX_LIGHT_RADII		4	/* max radii for 'light' object, really
					 * large values allow objects that can
					 * slow down the game */
#define MAX_DARKNESS		5	/* maximum map darkness, there is no
					 * practical reason to exceed this */ 
#define BRIGHTNESS(xyz)		(xyz)->glow_radius>MAX_LIGHT_RADII? \
				  MAX_LIGHT_RADII:(xyz)->glow_radius;

#define F_BUY			0
#define F_SELL			1
#define F_TRUE			2	/* True value of item, unadjusted */

#define DIRX(xyz)		freearr_x[(xyz)->direction]
#define DIRY(xyz)		freearr_y[(xyz)->direction]

#define D_LOCK(xyz)		(xyz)->contr->freeze_inv=(xyz)->contr->freeze_look=1;
#define D_UNLOCK(xyz)		(xyz)->contr->freeze_inv=(xyz)->contr->freeze_look=0;


/*
 * Flags to generate_treasures():
 */

#define GT_INVENTORY		0
#define GT_ENVIRONMENT		1
#define GT_INVISIBLE		2

#define BLANK_FACE_NAME		"blank.111"
#define BLOCKED_FACE_NAME	"blocked.111"
#define STIPPLE1_FACE_NAME	"stipple.111"
#define STIPPLE2_FACE_NAME	"stipple.112"
#define POTION_FACE_NAME	"potiongen.111" 

#define INV_CURSE_FACE_NAME	"inv-curse.111"
#define INV_DAMN_FACE_NAME	"inv-damn.111"
#define INV_EQUIP_FACE_NAME	"inv-equip.111"
#define INV_LOCK_FACE_NAME	"inv-lock.111"
#define INV_MAGIC_FACE_NAME	"inv-magic.111"
#define INV_UNPAID_FACE_NAME	"inv-unpaid.111"

#define DARK_FACE1_NAME		"dark1.111"
#define DARK_FACE2_NAME		"dark2.111"
#define DARK_FACE3_NAME		"dark3.111"

/* separated this from the common/item.c file. b.t. Dec 1995 */

#define DESCRIBE_ABILITY(retbuf, variable, name) \
    if(variable) { \
      int i,j=0; \
      strcat(retbuf,"(" name ": "); \
      for(i=0; i<NROFATTACKS; i++) \
        if(variable & (1<<i)) { \
          if (j) \
            strcat(retbuf,", "); \
          else \
            j = 1; \
          strcat(retbuf, attacks[i]); \
        } \
      strcat(retbuf,")"); \
    }

#define AP_APPLY		1
#define AP_UNAPPLY		2


/*
 * random() is much better than rand().  If you have random(), use it instead.
 * You shouldn't need to change any of this
 *
 * 0.93.3: It looks like linux has random (previously, it was set below
 * to use rand).  Perhaps old version of linux lack rand?  IF you run into
 * problems, add || defined(linux) the #if immediately below.
 *
 * 0.94.2 - you probably shouldn't need to change any of the rand stuff
 * here.
 */

#ifdef HAVE_SRANDOM
#define RANDOM() random()
#define SRANDOM(xyz) srandom(xyz)
#else
#  ifdef HAVE_SRAND48
#  define RANDOM() lrand48()
#  define SRANDOM(xyz) srand48(xyz)
#  else
#    ifdef HAVE_SRAND
#      define RANDOM() rand()
#      define SRANDOM(xyz) srand(xyz)
#    else
#      error "Could not find a usable random routine"
#    endif
#  endif
#endif
