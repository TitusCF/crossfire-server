/*
 * static char *rcsid_pets_c =
 *    "$Id$";
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
    object *owner, *tmp, *attacker, *tmp3;
    int i,x,y;
    mapstruct *nm;
    int search_arr[SIZEOFFREE];

    attacker = pet->attacked_by; /*pointer to attacking enemy*/
    pet->attacked_by = NULL;     /*clear this, since we are dealing with it*/

    if ((owner = get_owner(pet)) != NULL) {
	/* If the owner has turned on the pet, make the pet
	 * unfriendly.
	 */
	if ((check_enemy(owner,rv)) == pet) {
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

    /* See if the pet has an existing enemy. If so, don't start a new one*/
    if((tmp=check_enemy(pet, rv))!=NULL)
    {
	if(tmp == owner && !QUERY_FLAG(pet,FLAG_CONFUSED)
	    && QUERY_FLAG(pet,FLAG_FRIENDLY))
	    /* without this check, you can actually get pets with
	     * enemy set to owner! 
	     */
            pet->enemy = NULL;
	else
	    return tmp;
    }
    get_search_arr(search_arr);

    if (owner->type == PLAYER && owner->contr->petmode > pet_normal) {
	if (owner->contr->petmode == pet_sad) {
	    tmp = find_nearest_living_creature(pet);
	    if (tmp != NULL) {
		get_rangevector(pet, tmp, rv, 0);
		if(check_enemy(pet, rv) != NULL)
                    return tmp;
                else
                    pet->enemy = NULL;
	    }
	    /* if we got here we have no enemy */
	    /* we return NULL to avoid heading back to the owner */
	    pet->enemy = NULL;
	    return NULL;
	}
    }

    /* Since the pet has no existing enemy, look for anything nasty 
     * around the owner that it should go and attack.
     */
    tmp3 = NULL;
    for (i = 0; i < SIZEOFFREE; i++) {
	x = owner->x + freearr_x[search_arr[i]];
        y = owner->y + freearr_y[search_arr[i]];
	if (out_of_map(owner->map, x, y)) continue;
	else {
	    nm = get_map_from_coord(owner->map, &x, &y);
	    /* Only look on the space if there is something alive there. */
	    if (GET_MAP_FLAGS(nm, x,y)&P_IS_ALIVE) {
		for (tmp = get_map_ob(nm, x, y); tmp != NULL; tmp = tmp->above) {
		    object *tmp2 = tmp->head == NULL?tmp:tmp->head;
		    if (QUERY_FLAG(tmp2,FLAG_ALIVE) && !QUERY_FLAG(tmp2,FLAG_FRIENDLY)
			&& !QUERY_FLAG(tmp2,FLAG_UNAGGRESSIVE) && tmp2 != pet && 
			tmp2 != owner && tmp2->type != PLAYER &&
			can_detect_enemy(pet, tmp2, rv)) {

			if (!can_see_enemy(pet, tmp2)) {
			    if (tmp3 != NULL)
				tmp3 = tmp2;
			} else {
			    pet->enemy = tmp2;
			    if(check_enemy(pet, rv)!=NULL)
				return tmp2;
			    else
				pet->enemy = NULL;
			}
		    } /* if this is a valid enemy */
		} /* for objects on this space */
	    } /* if there is something living on this space */
	} /* this is a valid space on the map */
    } /* for loop of spaces around the owner */

    /* fine, we went through the whole loop and didn't find one we could
       see, take what we have */
    if (tmp3 != NULL) {
	pet->enemy = tmp3;
	if (check_enemy(pet, rv) != NULL)
	    return tmp3;
	else
	    pet->enemy = NULL;
    }

    /* No threat to owner, check to see if the pet has an attacker*/
    if (attacker)
    {
        /* need to be sure this is the right one! */
        if (attacker->count == pet->attacked_by_count)
        {
            /* also need to check to make sure it is not freindly */
   	    /* or otherwise non-hostile, and is an appropriate target */
            if (!QUERY_FLAG(attacker, FLAG_FRIENDLY) && on_same_map(pet, attacker))
	    {
	        pet->enemy = attacker;
		if (check_enemy(pet, rv) != NULL)
		    return attacker;
		else
		    pet->enemy = NULL;
	    }
        }
    }

    /* Don't have an attacker or legal enemy, so look for a new one!.
     * This looks for one around where the pet is.  Thus, you could lead
     * a pet to danger, then take a few steps back.  This code is basically
     * the same as the code that looks around the owner.
     */
    if (owner->type == PLAYER && owner->contr->petmode != pet_defend) {
	tmp3 = NULL;
	for (i = 0; i < SIZEOFFREE; i++) {
	    x = pet->x + freearr_x[search_arr[i]];
	    y = pet->y + freearr_y[search_arr[i]];
	    if (out_of_map(pet->map, x, y)) continue;
	    else {
		nm = get_map_from_coord(pet->map, &x, &y);
		/* Only look on the space if there is something alive there. */
		if (GET_MAP_FLAGS(nm, x,y)&P_IS_ALIVE) {
		    for (tmp = get_map_ob(nm, x, y); tmp != NULL; tmp = tmp->above) {
			object *tmp2 = tmp->head == NULL?tmp:tmp->head;
			if (QUERY_FLAG(tmp2,FLAG_ALIVE) &&
			    !QUERY_FLAG(tmp2,FLAG_FRIENDLY)
			    && !QUERY_FLAG(tmp2,FLAG_UNAGGRESSIVE) &&
			    tmp2 != pet && tmp2 != owner && tmp2->type != PLAYER &&
			    can_detect_enemy(pet, tmp2, rv)) {

			    if (!can_see_enemy(pet, tmp2)) {
				if (tmp3 != NULL)
				    tmp3 = tmp2;
			    } else {
				pet->enemy = tmp2;
				if(check_enemy(pet, rv)!=NULL)
				    return tmp2;
				else
				    pet->enemy = NULL;
			    }
			} /* make sure we can get to the bugger */
		    } /* for objects on this space */
		} /* if there is something living on this space */
	    } /* this is a valid space on the map */
	} /* for loop of spaces around the pet */
    } /* pet in defence mode */

    /* fine, we went through the whole loop and didn't find one we could
       see, take what we have */
    if (tmp3 != NULL) {
	pet->enemy = tmp3;
	if (check_enemy(pet, rv) != NULL)
	    return tmp3;
	else
	    pet->enemy = NULL;
    }

    /* Didn't find anything - return the owner's enemy or NULL */
    return check_enemy(pet, rv);
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
    int dir, tag, dx, dy, i;
    object *ob2, *owner;
    mapstruct *m;

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
    if (owner->type == PLAYER && owner->contr->petmode == pet_sad) {
	/* in S&D mode, if we have no enemy, run randomly about. */
	for (i=0; i < 15; i++) {
	    dir = rndm(1, 8);
	    dx = ob->x + freearr_x[dir];
	    dy = ob->y + freearr_y[dir];
	    if (out_of_map(owner->map, dx, dy) || wall(owner->map, dx, dy))
		continue;
	    else
		break;
	}
    } else {
        dir = find_dir_2(ob->x - ob->owner->x, ob->y - ob->owner->y);
    }
    ob->direction = dir;

    tag = ob->count;
    /* move_ob returns 0 if the object couldn't move.  If that is the
     * case, lets do some other work.
     */
    if (!(move_ob(ob, dir, ob))) {
	object *part;

	/* the failed move_ob above may destroy the pet, so check here */
	if (was_destroyed(ob, tag)) return;

	for(part = ob; part != NULL; part = part->more)
	{
	    dx = part->x + freearr_x[dir];
	    dy = part->y + freearr_y[dir];
	    m = get_map_from_coord(part->map, &dx, &dy);
	    if (!m) continue;

	    for (ob2 = get_map_ob(m, dx, dy); ob2 != NULL; ob2 = ob2->above)
	    {
		object *new_ob;
		new_ob = ob2->head?ob2->head:ob2;
		if(new_ob == ob)
		    break;
		if (new_ob == ob->owner)
		    return;
		if (get_owner(new_ob) == ob->owner)
		    break;

		/* Hmm.  Did we try to move into an enemy monster?  If so,
		 * make it our enemy.
		 */
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
	/* Try a different course */
	dir = absdir(dir + 4 - (RANDOM() %5) - (RANDOM()%5));
	(void) move_ob(ob, dir, ob);
    }
    return;
}
