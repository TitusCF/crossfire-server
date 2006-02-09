/*
 * static char *rcsid_friend_c =
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

/*
 * Add a new friendly object to the linked list of friendly objects.
 * No checking to see if the object is already in the linked list is done.
 */

void add_friendly_object(object *op) {
    objectlink *ol;

    /* Add some error checking.  This shouldn't happen, but the friendly
     * object list usually isn't very long, and remove_friendly_object
     * won't remove it either.  Plus, it is easier to put a breakpoint in
     * the debugger here and see where the problem is happening. 
     */
    for (ol=first_friendly_object; ol!=NULL; ol=ol->next) {
	if (ol->ob == op) {
	    LOG(llevError, "add_friendly_object: Trying to add object already on list (%s)\n",
		op->name);
	    return;
	}
    }

    ol=first_friendly_object;
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

    if (!first_friendly_object) {
	LOG(llevError,"remove_friendly_object called with empty friendly list, remove ob=%s\n", op->name);
	return;
    }
    /* if the first object happens to be the one, processing is pretty
     * easy.
     */
    if(first_friendly_object->ob==op) {
	this=first_friendly_object;
	first_friendly_object=this->next;
	free(this);
    } else {
	objectlink *prev=first_friendly_object;

	for (this=first_friendly_object->next; this!=NULL; this=this->next) {
	    if (this->ob == op) break;
	    prev=this;
	}
	if (this) {
	    /* This should not happen.  But if it does, presumably the
	     * call to remove it is still valid.
	     */
	    if (this->id != op->count) {
		LOG(llevError,"remove_friendly_object, tags do no match, %s, %d != %d\n",
		    op->name?op->name:"none", op->count, this->id);
	    }
	    prev->next = this->next;
	    free(this);
	}
    }
}

/*
 * Dumps all friendly objects.  Invoked in DM-mode with the G key.
 */

void dump_friendly_objects(void) {
    objectlink *ol;

    for(ol=first_friendly_object;ol!=NULL;ol=ol->next)
	LOG(llevError, "%s (%d)\n",ol->ob->name,ol->ob->count);
}

/* New function, MSW 2000-1-14
 * It traverses the friendly list removing objects that should not be here
 * (ie, do not have friendly flag set, freed, etc)
 */
void clean_friendly_list(void) {
    objectlink *this, *prev=NULL, *next;
    int count=0;

    for (this=first_friendly_object; this!=NULL; this=next) {
	next=this->next;
	if (QUERY_FLAG(this->ob, FLAG_FREED) || 
	    !QUERY_FLAG(this->ob, FLAG_FRIENDLY) ||
	    (this->id != this->ob->count)) {
	    if (prev) {
		prev->next = this->next;
	    }
	    else {
		first_friendly_object = this->next;
	    }
	    count++;
	    free(this);
	}
	/* If we removed the object, then prev is still valid.  */
	else prev=this;
    }
    if (count) 
	LOG(llevDebug,"clean_friendly_list: Removed %d bogus links\n", count);
}

/* Checks if the given object is already in the friendly list or not
 * Lauwenmark - 31/07/05
 */
int is_friendly(const object* op)
{
    objectlink *ol;

    for(ol=first_friendly_object;ol!=NULL;ol=ol->next)
        if (ol->ob == op)
            return 1;

    return 0;
}

