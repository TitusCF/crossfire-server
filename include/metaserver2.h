/*
 * static char *rcsid_metaserver2_h =
 *   "$Id: map.h 6761 2007-07-09 06:20:18Z mwedel $";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2007 Mark Wedel & Crossfire Development Team

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
 * This file contains metaserver2 information - the metaserver2
 * implementation requires that a separate thread be used
 * for updates, so a structure to get updated data to the thread
 * is needed.
 */

#ifndef METASERVER2_H
#define METASERVER2_H

/**
 * The current implementation of crossfire is not very thread
 * friendly/safe - in fact, metaserver2 is the first part to
 * use an extra thread.
 * As such, it would be a lot of work to add the necessary
 * locks in the rest of the code for the metaserver2 to be able
 * to get the data it needs.  So instead, we use this
 * MS2UpdateInfo structure to get the data from the main thread
 * to the metaserver thread - basically, the main thread
 * gets a lock on the mutex, updates the fields in this structure
 * and releases the lock.  The metaserver2 thread will
 * get a lock when it needs to get this information, copy
 * it to the post form, then unlock the structure.
 * In that way, neither side holds the lock very long, which is especially
 * important because the metaserver2 thread could take quite a while
 * to do its updates.
 */

extern pthread_mutex_t ms2_info_mutex;

typedef struct _MetaServer2_UpdateInfo {
    int     num_players;        /* Number of players */
    int     in_bytes;           /* Number of bytes received */
    int     out_bytes;          /* Number of bytes sent */
    time_t  uptime;             /* How long server has been up */
} MetaServer2_UpdateInfo;

extern MetaServer2_UpdateInfo metaserver2_updateinfo;

#endif /* METASERVER2_H */
