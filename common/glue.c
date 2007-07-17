/*
 * static char *rcsid_glue_c =
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

#include <global.h>
#include <sproto.h>

/**
 * @file glue.c
 *
 * All this glue is currently needed to connect the game with the
 * server.  I'll try to make the library more able to "stand on it's
 * own legs" later; not done in 5 minutes to separate two parts of
 * the code which were connected, well, can you say "spagetti"?
 *
 * Glue has been cleaned, so this file is almost empty now :)
 * Ryo 2005-07-15
 */

/**
 * Error messages to display.
 */
static const char* const fatalmsgs[80]={
  "Failed to allocate memory",
  "Failed repeatedly to load maps",
  "Hashtable for archetypes is too small",
};

/**
 * fatal() is meant to be called whenever a fatal signal is intercepted.
 * It will call the emergency_save and the clean_tmp_files functions.
 *
 * @note
 * this function never returns, as it calls exit().
 */
void fatal(int err) {
  fprintf(logfile,"Fatal: %s\n",fatalmsgs[err]);
  emergency_save(0);
  clean_tmp_files();
  fprintf(logfile,"Exiting...\n");
  exit(err);
}
