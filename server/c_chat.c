/*
 * static char *rcsid_c_chat_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copryight (C) 2000 Mark Wedel
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

#include <global.h>
#include <loader.h>
#include <sproto.h>

int command_say (object *op, char *params)
{
    char buf[MAX_BUF];

    if (!params) return 0;
    /* This broken apart from one sprintf to avoid buffer overuns.
     * This could be done via snprintf, but I am not sure how widely
     * available that is. MSW 5/22/2000
     */
    sprintf(buf, "%s says: ",op->name);
    strncat(buf, params, MAX_BUF - strlen(buf)-1);
    buf[MAX_BUF-1]=0;

    new_info_map(NDI_WHITE,op->map, buf);
    communicate(op, params);
  
    return 0;
}

int command_last (object *op, char *params)
{
  new_draw_info(NDI_UNIQUE, 0,op, "Last not yet implemented.");
  return 0;
}
