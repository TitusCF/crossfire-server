/*
 * static char *rcsid_newclient_h =
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

/* This file defines various flags that both the new client and
 * newserver uses.  These should never be changed, only expanded.
 * Changing them will likely cause all old clients to not work properly.
 * While called newclient, it is really used by both the client and
 * server to keep some values the same.
 *
 * Name format is CS_(command)_(flag)
 * CS = Client/Server.
 * (command) is protocol command, ie ITEM
 * (flag) is the flag name
 */

/* It is trivial to keep a link of copy of this file in the client
 * or server area.  But keeping one common file should make things
 * more reliable, as both the client and server will definately be
 * talking about the same values.
 */


#ifndef NEWCLIENT_H
#define NEWCLIENT_H

/* Maximum size of any packet we expect.  Using this makes it so we don't need to
 * allocated and deallocated teh same buffer over and over again and the price
 * of using a bit of extra memory.  IT also makes the code simpler.
 */
#define MAXSOCKBUF 10240


#define CS_QUERY_YESNO	0x1	/* Yes/no question */
#define CS_QUERY_SINGLECHAR 0x2	/* Single character response expected */
#define CS_QUERY_HIDEINPUT 0x4	/* Hide input being entered */

#define CS_SAY_NORMAL	0x1	/* Normal say command */
#define CS_SAY_SHOUT	0x2	/* Text is shouted. */
#define CS_SAY_GSAY	0x4	/* Text is group say command */

/* These are multiplication values that should be used when changing 
 * floats to ints, and vice version.  MULTI is integer representatin
 * (float to int), MULTF is float, for going from int to float.
 */
#define FLOAT_MULTI	100000
#define FLOAT_MULTF	100000.0

/* ID's for the various stats that get sent across. */

#define CS_STAT_HP	 1
#define CS_STAT_MAXHP	 2
#define CS_STAT_SP	 3
#define CS_STAT_MAXSP	 4
#define CS_STAT_STR	 5
#define CS_STAT_INT	 6
#define CS_STAT_WIS	 7
#define CS_STAT_DEX	 8
#define CS_STAT_CON	 9
#define CS_STAT_CHA	10
#define CS_STAT_EXP	11
#define CS_STAT_LEVEL	12
#define CS_STAT_WC	13
#define CS_STAT_AC	14
#define CS_STAT_DAM	15
#define CS_STAT_ARMOUR	16
#define CS_STAT_SPEED	17
#define CS_STAT_FOOD	18
#define CS_STAT_WEAP_SP 19
#define CS_STAT_RANGE	20
#define CS_STAT_TITLE	21
#define CS_STAT_POW	22
#define CS_STAT_GRACE	23
#define CS_STAT_MAXGRACE	24
#define CS_STAT_FLAGS	25
#define CS_STAT_WEIGHT_LIM	26

/* These are used with CS_STAT_FLAGS above to communicate S->C what the
 * server thinks the fireon & runon states are.
 */
#define SF_FIREON   0x01
#define SF_RUNON    0x02

/* The following are the color flags passed to new_draw_info.
 *
 * We also set up some control flags
 *
 * NDI = New Draw Info
 */

/* Color specifications - note these match the order in xutil.c */
/* Note 2:  Black, the default color, is 0.  Thus, it does not need to
 * be implicitly specified.
 */
#define NDI_BLACK	0
#define NDI_WHITE	1
#define NDI_NAVY	2
#define NDI_RED		3
#define NDI_ORANGE	4
#define NDI_BLUE	5	/* Actually, it is Dodger Blue */
#define NDI_DK_ORANGE	6	/* DarkOrange2 */
#define NDI_GREEN	7	/* SeaGreen */
#define NDI_LT_GREEN	8	/* DarkSeaGreen,  which is actually paler */
				/* Than seagreen - also background color */
#define NDI_GREY	9
#define NDI_BROWN	10	/* Sienna */
#define NDI_GOLD	11
#define NDI_TAN		12	/* Khaki */

#define NDI_MAX_COLOR	12	/* Last value in */
#define NDI_COLOR_MASK	0xff	/* Gives lots of room for expansion - we are */
				/* using an int anyways, so we have the space */
				/* to still do all the flags */


#define NDI_UNIQUE	0x100	/* Print this out immediately, don't buffer */
#define NDI_ALL		0x200	/* Inform all players of this message */

/* Flags for the item command */
enum {a_none, a_readied, a_wielded, a_worn, a_active, a_applied};
#define F_APPLIED       0x000F
#define F_LOCATION      0x00F0
#define F_UNPAID        0x0200
#define F_MAGIC         0x0400
#define F_CURSED        0x0800
#define F_DAMNED        0x1000
#define F_OPEN          0x2000
#define F_NOPICK        0x4000
#define F_LOCKED        0x8000

#define CF_FACE_NONE	0
#define CF_FACE_BITMAP	1
#define CF_FACE_XPM	2
#define CF_FACE_CACHE	0x10

/* Used in the new_face structure on the magicmap field.  Low bits
 * are color informatin.  For now, only high bit information we need
 * is floor information.
 */
#define FACE_FLOOR	0x80
#define FACE_WALL	0x40	/* Or'd into the color value by the server
				 * right before sending.
				 */
#define FACE_COLOR_MASK	0xf

#define UPD_LOCATION	0x01
#define UPD_FLAGS	0x02
#define UPD_WEIGHT	0x04
#define UPD_FACE	0x08
#define UPD_NAME	0x10
#define UPD_ANIM	0x20
#define UPD_ANIMSPEED	0x40
#define UPD_NROF	0x80

#define SOUND_NORMAL	0
#define SOUND_SPELL	1

/* Contains the base information we use to make up a packet we want to send. */
typedef struct SockList {
    int len;
    unsigned char *buf;
} SockList;

typedef struct CS_Stats {
    int	    ibytes;	/* ibytes, obytes are bytes in, out */
    int	    obytes;
    short   max_conn;	/* Maximum connections received */
    time_t  time_start;	/* When we started logging this */
} CS_Stats;


extern CS_Stats cst_tot, cst_lst;

#endif
