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
 * Add a new friendly object to the linked list of friendly objects.
 * No checking to see if the object is already in the linked list is done.
 */

void add_friendly_object(object *op) {
  objectlink *ol=first_friendly_object;
  first_friendly_object=get_objectlink();
  first_friendly_object->ob = op;
  first_friendly_object->id = op->count;
  first_friendly_object->next=ol;
}

/*
 * Removes the specified object from the linked list of friendly objects.
 */

void remove_friendly_object(object *op) {
  objectlink *this;
  CLEAR_FLAG(op,FLAG_FRIENDLY);
  if(first_friendly_object->ob!=op) {
    objectlink *prev=first_friendly_object;
    while(prev!=NULL&&prev->next!=NULL&&
          (prev->next->ob!=op || prev->next->id != op->count))
      prev=prev->next;
    if(prev==NULL||prev->next==NULL||
       prev->next->ob!=op||prev->next->id!=op->count) {
      LOG(llevDebug,"Remove_friendly_object: Can't find object %s (%d).\n",
          op->name,op->count);
      return;
    }
    this=prev->next;
    prev->next=this->next;
  } else {
    this=first_friendly_object;
    first_friendly_object=this->next;
  }
  CFREE(this);
}

/*
 * Dumps all friendly objects.  Invoked in DM-mode with the G key.
 */

void dump_friendly_objects() {
  objectlink *ol;
  for(ol=first_friendly_object;ol!=NULL;ol=ol->next)
    LOG(llevError, "%s (%d)\n",ol->ob->name,ol->ob->count);
}
