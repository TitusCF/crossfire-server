/*
 * static char *rcsid_pets_c =
 *    "$Id$";
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

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/* given that 'pet' is a friendly object, this function returns a
 * monster the pet should attack, NULL if nothing appropriate is
 * found.  it basically looks for nasty things around the owner
 * of the pet to attack.
 * this is now tilemap aware.
 */

object *get_pet_enemy(object * pet, rv_vector *rv){
    object *owner, *tmp;
    int i,x,y;
    mapstruct *nm;

    if ((owner = get_owner(pet)) != NULL) {
	/* If the owner has turned on the pet, make the pet
	 * unfriendly.
	 */
	if ((get_enemy(owner,rv)) == pet) {
	    CLEAR_FLAG(pet, FLAG_FRIENDLY);
	    remove_friendly_object(pet);
	    pet->move_type &=~PETMOVE;
	    return owner;
	}
    } else {
	/* else the owner is no longer around, so the
	 * pet no longer needs to be friendly.
	 */
	CLEAR_FLAG(pet, FLAG_FRIENDLY);
	remove_friendly_object(pet);
	pet->move_type &=~PETMOVE;
	return NULL;
    }
    /* If they are not on the same map, the pet won't be agressive */
    if (!on_same_map(pet,owner))
	return NULL;

    /* We basically look for anything nasty around the owner that this
     * pet should go and attack.
     */
    for (i = 0; i < SIZEOFFREE; i++) {
	x = owner->x + freearr_x[i];
	y = owner->y + freearr_y[i];
	if (out_of_map(owner->map, x, y)) continue;
	else {
	    nm = get_map_from_coord(owner->map, &x, &y);
	    /* Only look on the space if there is something alive there. */
	    if (GET_MAP_FLAGS(nm, x,y)&P_IS_ALIVE) {
		for (tmp = get_map_ob(nm, x, y); tmp != NULL; tmp = tmp->above) {
		    object *tmp2 = tmp->head == NULL?tmp:tmp->head;
		    if (QUERY_FLAG(tmp2,FLAG_ALIVE) && !QUERY_FLAG(tmp2,FLAG_FRIENDLY)
			&& !QUERY_FLAG(tmp2,FLAG_UNAGGRESSIVE) &&
			tmp2 != owner && tmp2->type != PLAYER)
		    return tmp2;
		} /* for objects on this space */
	    } /* if there is something living on this space */
	} /* this is a valid space on the map */
    } /* for loop of spaces around the owner */

    /* Didn't find anything - return NULL */
    return NULL;
}

void terminate_all_pets(object *owner) {
  objectlink *obl, *next;
  for(obl = first_friendly_object; obl != NULL; obl = next) {
    object *ob = obl->ob;
    next = obl->next;
    if(get_owner(ob) == owner) {
      if(!QUERY_FLAG(ob, FLAG_REMOVED))
        remove_ob(ob);
      remove_friendly_object(ob);
      free_object(ob);
    }
  }
}

/*
 * Unfortunately, sometimes, the owner of a pet is in the
 * process of entering a new map when this is called.
 * Thus the map isn't loaded yet, and we have to remove
 * the pet...
 * Interesting enough, we don't use the passed map structure in
 * this function.
 */

void remove_all_pets(mapstruct *map) {
  objectlink *obl, *next;
  object *owner;

  for(obl = first_friendly_object; obl != NULL; obl = next) {
    next = obl->next;
    if(obl->ob->type != PLAYER && QUERY_FLAG(obl->ob,FLAG_FRIENDLY) &&
       (owner = get_owner(obl->ob)) != NULL && owner->map != obl->ob->map)
    {
	/* follow owner checks map status for us */
	follow_owner(obl->ob,owner);
	if(QUERY_FLAG(obl->ob, FLAG_REMOVED) && FABS(obl->ob->speed) > MIN_ACTIVE_SPEED) {
	    object *ob = obl->ob;
	    LOG(llevMonster,"(pet failed to follow)");
	    remove_friendly_object(ob);
	    free_object(ob);
	}
    }
  }
}

void follow_owner(object *ob, object *owner) {
  object *tmp;
  int dir;

  if (!QUERY_FLAG(ob,FLAG_REMOVED))
    remove_ob(ob);
  if(owner->map == NULL) {
    LOG(llevError,"Can't follow owner: no map.\n");
    return;
  }
  if(owner->map->in_memory != MAP_IN_MEMORY) {
    LOG(llevError,"Owner of the pet not on a map in memory!?\n");
    return;
  }
  dir = find_free_spot(ob->arch, owner->map,
                       owner->x, owner->y, 1, SIZEOFFREE+1);
  if (dir==-1) {
    LOG(llevMonster,"No space for pet to follow, freeing %s.\n",ob->name);
    return; /* Will be freed since it's removed */
  }
  for(tmp=ob;tmp!=NULL;tmp=tmp->more) {
    tmp->x = owner->x + freearr_x[dir]+(tmp->arch==NULL?0:tmp->arch->clone.x);
    tmp->y = owner->y + freearr_y[dir]+(tmp->arch==NULL?0:tmp->arch->clone.y);
  }
  insert_ob_in_map(ob, owner->map, NULL,0);
  if (owner->type == PLAYER) /* Uh, I hope this is always true... */
    new_draw_info(NDI_UNIQUE, 0,owner, "Your pet magically appears next to you");
  return;
}

void pet_move(object * ob)
{
    int dir,tag;
    object *ob2, *owner;

    /* Check to see if player pulled out */
    if ((owner = get_owner(ob)) == NULL) {
	remove_ob(ob); /* Will be freed when returning */
	remove_friendly_object(ob);
	free_object(ob);
	LOG(llevMonster, "Pet: no owner, leaving.\n");
	return;
    }

    /* move monster into the owners map if not in the same map */
    if (ob->map != owner->map) {
	follow_owner(ob, owner);
	return;
    }
    /* Calculate Direction */
    dir = find_dir_2(ob->x - ob->owner->x, ob->y - ob->owner->y);
    ob->direction = dir;

    tag = ob->count;
    if (!(move_ob(ob, dir,ob))) {
	object *part;

	/* the failed move_ob above may destroy the pet, so check here */
	if (was_destroyed(ob, tag)) return;

	for(part = ob; part != NULL; part = part->more)
	{
	    for (ob2 = get_map_ob(part->map, part->x + freearr_x[dir],
				  part->y + freearr_y[dir]); ob2 != NULL; ob2 = ob2->above)
	    {
		object *new_ob;
		new_ob = ob2->head?ob2->head:ob2;
		if(new_ob == ob)
		    break;
		if (new_ob == ob->owner)
		    return;
		if (get_owner(new_ob) == ob->owner)
		    break;
		if (QUERY_FLAG(new_ob,FLAG_ALIVE) && !QUERY_FLAG(ob,FLAG_UNAGGRESSIVE)
		    && !QUERY_FLAG(new_ob,FLAG_UNAGGRESSIVE) &&
		    !QUERY_FLAG(new_ob,FLAG_FRIENDLY)) {

		    ob->enemy = new_ob;
		    if(new_ob->enemy == NULL)
			new_ob->enemy = ob;
			return;
		} else if (new_ob->type == PLAYER) {
		    new_draw_info(NDI_UNIQUE, 0,new_ob, "You stand in the way of someones pet.");
		    return;
		}
	    }
	}
	dir = absdir(dir + 4 - (RANDOM() %5) - (RANDOM()%5));
	(void) move_ob(ob, dir, ob);
    }
    return;
}
