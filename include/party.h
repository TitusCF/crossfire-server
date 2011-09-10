/*
 * static char *rcsid_player_h =
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

/**
 * @file
 * Party-specific structures.
 */

#ifndef PARTY_H
#define PARTY_H

/** One party. First item is ::firstparty. */
typedef struct party_struct {
    char *partyleader;          /**< Who is the leader. */
    char passwd[9];             /**< Party password. */
    struct party_struct *next;  /**< Next party in list. */
    char *partyname;            /**< Party name. */

#ifdef PARTY_KILL_LOG
    struct party_kill {
        char killer[MAX_NAME+1], dead[MAX_NAME+1];
        sint64 exp;
    } party_kills[PARTY_KILL_LOG];
    sint64 total_exp;
    uint32  kills;
#endif
} partylist;

#endif
