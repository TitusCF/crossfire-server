/*
 * static char *rcsid_friend_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

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

    The author can be reached via e-mail to frankj@ifi.uio.no.
*/

#include <global.h>

/*
 * Allocates a new objectlink structure, initialises it, and returns
 * a pointer to it.
 */

objectlink *get_objectlink(void) {
  objectlink *ol=(objectlink *)CALLOC(1,sizeof(objectlink));
  ol->ob=NULL;
  ol->next=NULL;
  ol->id = 0;
  return ol;
}

/*
 * Allocates a new oblinkpt structure, initialises it, and returns
 * a pointer to it.
 */

oblinkpt *get_objectlinkpt(void) {
  oblinkpt *obp = (oblinkpt *) malloc(sizeof(oblinkpt));
  obp->link = NULL;
  obp->next = NULL;
  obp->value = 0;
  return obp;
}

/*
 * Recursively frees all objectlinks
 */

void free_objectlink(objectlink *ol) {
  if (ol->next)
    free_objectlink(ol->next);
  free(ol);
}

/*
 * Recursively frees all linked list of objectlink pointers
 */

void free_objectlinkpt(oblinkpt *obp) {
  if (obp->next)
    free_objectlinkpt(obp->next);
  if (obp->link)
    free_objectlink(obp->link);
  free(obp);
}
