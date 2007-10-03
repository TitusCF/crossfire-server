/*
 * static char *rcsid_pets_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
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
 * Those functions deal with pets.
 */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/**
 * Mark all inventory items as FLAG_NO_DROP.
 *
 * @param ob
 * the object to modify.
 */
static void mark_inventory_as_no_drop(object *ob) {
    object *tmp;

    for (tmp = ob->inv; tmp != NULL; tmp = tmp->below) {
        SET_FLAG(tmp, FLAG_NO_DROP);
    }
}

/**
 * Given that 'pet' is a friendly object, this function returns a
 * monster the pet should attack, NULL if nothing appropriate is
 * found.  it basically looks for nasty things around the owner
 * of the pet to attack.
 * This is now tilemap aware.
 *
 * @param pet
 * who is seeking an enemy.
 * @param[out] rv
 * will contain the path to the enemy.
 * @return
 * enemy, or NULL if nothing suitable.
 */
object *get_pet_enemy(object * pet, rv_vector *rv){
    object *owner, *tmp, *attacker, *tmp3;
    int i;
    sint16 x,y;
    mapstruct *nm;
    int search_arr[SIZEOFFREE];
    int mflags;

    attacker = pet->attacked_by; /*pointer to attacking enemy*/
    pet->attacked_by = NULL;     /*clear this, since we are dealing with it*/

    if ((owner = get_owner(pet)) != NULL) {
	/* If the owner has turned on the pet, make the pet
	 * unfriendly.
	 */
	if ((check_enemy(owner,rv)) == pet) {
	    CLEAR_FLAG(pet, FLAG_FRIENDLY);
	    remove_friendly_object(pet);
	    pet->attack_movement &=~PETMOVE;
	    return owner;
	}
    } else {
	/* else the owner is no longer around, so the
	 * pet no longer needs to be friendly.
	 */
	CLEAR_FLAG(pet, FLAG_FRIENDLY);
	remove_friendly_object(pet);
	pet->attack_movement &=~PETMOVE;
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
	nm = owner->map;
	/* Only look on the space if there is something alive there. */
	mflags = get_map_flags(nm, &nm, x, y, &x, &y);
	if (!(mflags & P_OUT_OF_MAP) && mflags & P_IS_ALIVE) {
	    for (tmp = get_map_ob(nm, x, y); tmp != NULL; tmp = tmp->above) {
		object *tmp2 = tmp->head == NULL?tmp:tmp->head;

	    if (QUERY_FLAG(tmp2,FLAG_ALIVE) && ((
				!QUERY_FLAG(tmp2, FLAG_FRIENDLY) &&
				tmp2->type != PLAYER) ||
			should_arena_attack(pet, owner, tmp2))
		&& !QUERY_FLAG(tmp2,FLAG_UNAGGRESSIVE) &&
		tmp2 != pet && tmp2 != owner &&
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
		}/* if this is a valid enemy */
	    }/* for objects on this space */
	}/* if there is something living on this space */
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
	    nm = pet->map;
	    /* Only look on the space if there is something alive there. */
	    mflags = get_map_flags(nm, &nm, x,y, &x, &y);
	    if (!(mflags & P_OUT_OF_MAP) && mflags & P_IS_ALIVE) {
		for (tmp = get_map_ob(nm, x, y); tmp != NULL; tmp = tmp->above) {
		    object *tmp2 = tmp->head == NULL?tmp:tmp->head;
		    if (QUERY_FLAG(tmp2,FLAG_ALIVE) && ((
					!QUERY_FLAG(tmp2, FLAG_FRIENDLY) &&
					tmp2->type != PLAYER) ||
				should_arena_attack(pet, owner, tmp2))
			&& !QUERY_FLAG(tmp2,FLAG_UNAGGRESSIVE) &&
			tmp2 != pet && tmp2 != owner &&
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
		}/* for objects on this space */
	    } /* if there is something living on this space */
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

/**
 * Removes all pets someone owns.
 *
 * @param owner
 * player we wish to remove all pets of.
 */
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

/**
 * This function checks all pets so they try to follow their master around the world.
 *
 * Unfortunately, sometimes, the owner of a pet is in the
 * process of entering a new map when this is called.
 * Thus the map isn't loaded yet, and we have to remove
 * the pet...
 */
void remove_all_pets() {
  objectlink *obl, *next;
  object *owner;

  for(obl = first_friendly_object; obl != NULL; obl = next) {
    next = obl->next;
    if(obl->ob->type != PLAYER && QUERY_FLAG(obl->ob,FLAG_FRIENDLY) &&
       (owner = get_owner(obl->ob)) != NULL && !on_same_map(owner, obl->ob))
    {
	/* follow owner checks map status for us.  Note that pet can
	 * die in follow_owner, so check for obl->ob existence
	 */
	follow_owner(obl->ob,owner);
	if(obl->ob && QUERY_FLAG(obl->ob, FLAG_REMOVED) && FABS(obl->ob->speed) > MIN_ACTIVE_SPEED) {
	    object *ob = obl->ob;
	    LOG(llevMonster,"(pet failed to follow)\n");
	    remove_friendly_object(ob);
	    free_object(ob);
	}
    }
  }
}

/**
 * A pet is trying to follow its owner.
 *
 * @param ob
 * pet trying to follow. Will be remove_ob()'d if can't follow.
 * @param owner
 * owner of ob.
 */
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
    dir = find_free_spot(ob, owner->map,
                       owner->x, owner->y, 1, SIZEOFFREE);

    if (dir==-1) {
	LOG(llevMonster,"No space for pet to follow, freeing %s.\n",ob->name);
	return; /* Will be freed since it's removed */
    }
    for(tmp=ob;tmp!=NULL;tmp=tmp->more) {
	tmp->x = owner->x + freearr_x[dir]+(tmp->arch==NULL?0:tmp->arch->clone.x);
	tmp->y = owner->y + freearr_y[dir]+(tmp->arch==NULL?0:tmp->arch->clone.y);
	tmp->map = owner->map;
	if (OUT_OF_REAL_MAP(tmp->map, tmp->x, tmp->y)) {
	    tmp->map = get_map_from_coord(tmp->map, &tmp->x, &tmp->y);
	}
    }
    insert_ob_in_map(ob, ob->map, NULL,0);
    if (owner->type == PLAYER) /* Uh, I hope this is always true... */
	draw_ext_info(NDI_UNIQUE, 0,owner, MSG_TYPE_SPELL, MSG_TYPE_SPELL_PET,
		      "Your pet magically appears next to you", NULL);
    return;
}

/**
 * Handles a pet's movement.
 *
 * @param ob
 * pet to move.
 */
void pet_move(object * ob)
{
    int dir, tag, i;
    sint16 dx, dy;
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
    if (!on_same_map(ob, owner)) {
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
	    m = ob->map;
	    if (get_map_flags(ob->map, &m, dx, dy, &dx, &dy) & P_OUT_OF_MAP)
		continue;
	    else if (OB_TYPE_MOVE_BLOCK(ob, GET_MAP_MOVE_BLOCK(m, dx, dy)))
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
		    draw_ext_info(NDI_UNIQUE, 0,new_ob,
				  MSG_TYPE_MISC, MSG_SUBTYPE_NONE,
				  "You stand in the way of someones pet.", NULL);
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

/****************************************************************************
 *
 * GOLEM SPELL CODE
 *
 ****************************************************************************/

/**
 * This makes multisquare/single square monsters
 * proper for map insertion.
 * @param at
 * archetype to prepare.
 * @param op
 * caster of the spell
 * @param dir
 * direction the monster should be placed in.
 * @param is_golem
 * if set then this is a golem spell.
 * @return
 * suitable golem.
 */
static object *fix_summon_pet(archetype *at, object *op, int dir, int is_golem) {
    archetype *atmp;
    object *tmp=NULL, *prev=NULL, *head=NULL;

    for(atmp = at; atmp!=NULL; atmp = atmp->more) {
        tmp = arch_to_object(atmp);
        if (atmp == at) {
            if(!is_golem)
                SET_FLAG(tmp, FLAG_MONSTER);
            set_owner(tmp, op);
            if (op->type == PLAYER) {
                tmp->stats.exp = 0;
                add_friendly_object(tmp);
                SET_FLAG(tmp, FLAG_FRIENDLY);
                if(is_golem) CLEAR_FLAG(tmp, FLAG_MONSTER);
            } else if(QUERY_FLAG(op, FLAG_FRIENDLY)) {
                object *owner = get_owner(op);
                if(owner != NULL) {/* For now, we transfer ownership */
                    set_owner(tmp,owner);
                    tmp->attack_movement = PETMOVE;
                    add_friendly_object(tmp);
                    SET_FLAG(tmp, FLAG_FRIENDLY);
                }
            }
            if(op->type!=PLAYER || !is_golem) {
                tmp->attack_movement = PETMOVE;
                tmp->speed_left = -1;
                tmp->type = 0;
                tmp->enemy = op->enemy;
            } else
                tmp->type = GOLEM;
        }
        if(head == NULL)
            head = tmp;
        tmp->x = op->x + freearr_x[dir] + tmp->arch->clone.x;
        tmp->y = op->y + freearr_y[dir] + tmp->arch->clone.y;
        tmp->map = op->map;
        if(tmp->invisible) tmp->invisible=0;
        if(head != tmp)
            tmp->head = head, prev->more = tmp;
        prev = tmp;
    }
    head->direction = dir;

    if (head->randomitems) {
        create_treasure(head->randomitems, head, GT_APPLY | GT_STARTEQUIP, 6, 0);
    }
    mark_inventory_as_no_drop(head);

    /* need to change some monster attr to prevent problems/crashing */
    head->last_heal=0;
    head->last_eat=0;
    head->last_grace=0;
    head->last_sp=0;
    head->other_arch=NULL;
    head->stats.exp = 0;
    CLEAR_FLAG(head,FLAG_CHANGING);
    CLEAR_FLAG(head,FLAG_STAND_STILL);
    CLEAR_FLAG(head,FLAG_GENERATOR);
    CLEAR_FLAG(head,FLAG_SPLITTING);
    if(head->attacktype&AT_GHOSTHIT) head->attacktype=(AT_PHYSICAL|AT_DRAIN);

    return head;
}

/**
 * Handles a golem's movement.
 *
 * Updated this to allow more than the golem 'head' to attack.
 * @param op
 * golem to be moved.
 */
void move_golem(object *op) {
    int made_attack=0;
    object *tmp;
    tag_t tag;

    if(QUERY_FLAG(op, FLAG_MONSTER))
	return; /* Has already been moved */

    if(get_owner(op)==NULL) {
	LOG(llevDebug,"Golem without owner destructed.\n");
	remove_ob(op);
	free_object(op);
	return;
    }
    /* It would be nice to have a cleaner way of what message to print
     * when the golem expires than these hard coded entries.
     * Note it is intentional that a golems duration is based on its
     * hp, and not duration
     */
    if(--op->stats.hp<0) {
	if (op->msg)
	    draw_ext_info(NDI_UNIQUE, 0,op->owner,MSG_TYPE_SPELL, MSG_TYPE_SPELL_PET,
			  op->msg, op->msg);
	op->owner->contr->ranges[range_golem]=NULL;
	op->owner->contr->golem_count = 0;
	remove_friendly_object(op);
	remove_ob(op);
	free_object(op);
	return;
    }

    /* Do golem attacks/movement for single & multisq golems.
     * Assuming here that op is the 'head' object. Pass only op to
     * move_ob (makes recursive calls to other parts)
     * move_ob returns 0 if the creature was not able to move.
     */
    tag = op->count;
    if(move_ob(op,op->direction,op)) return;
    if (was_destroyed (op, tag))
        return;

    for(tmp=op;tmp;tmp=tmp->more) {
	sint16 x=tmp->x+freearr_x[op->direction],y=tmp->y+freearr_y[op->direction];
	object *victim;
	mapstruct *m;
	int mflags;

	m = op->map;
	mflags = get_map_flags(m, &m, x, y, &x, &y);

	if (mflags & P_OUT_OF_MAP) continue;

	for(victim=get_map_ob(op->map,x,y);victim;victim=victim->above)
	    if(QUERY_FLAG(victim,FLAG_ALIVE)) break;

	/* We used to call will_hit_self to make sure we don't
	 * hit ourselves, but that didn't work, and I don't really
	 * know if that was more efficient anyways than this.
	 * This at least works.  Note that victim->head can be NULL,
	 * but since we are not trying to dereferance that pointer,
	 * that isn't a problem.
	 */
	if(victim && victim!=op && victim->head!=op) {

	    /* for golems with race fields, we don't attack
	     * aligned races
	     */

	    if(victim->race && op->race && strstr(op->race,victim->race)) {
		if(op->owner) draw_ext_info_format(NDI_UNIQUE, 0,op->owner,
			   MSG_TYPE_SPELL, MSG_TYPE_SPELL_PET,
			   "%s avoids damaging %s.",
			   "%s avoids damaging %s.",
			   op->name,victim->name);
	    } else if (victim == op->owner) {
		if(op->owner) draw_ext_info_format(NDI_UNIQUE, 0,op->owner,
			   MSG_TYPE_SPELL, MSG_TYPE_SPELL_PET,
			   "%s avoids damaging you.",
			   "%s avoids damaging you.",
			   op->name);
	    } else {
		attack_ob(victim,op);
		made_attack=1;
	    }
	} /* If victim */
    }
    if(made_attack) update_object(op,UP_OBJ_FACE);
}

/**
 * Makes the golem go in specified direction.
 * This is a really stupid function when you get down and
 * look at it.  Keep it here for the time being - makes life
 * easier if we ever decide to do more interesting thing with
 * controlled golems.
 *
 * @param op
 * golem.
 * @param dir
 * desired direction.
 * @todo trash.
 */
void control_golem(object *op,int dir) {
    op->direction=dir;
}

/**
 * Summons a monster.
 *
 * @param op
 * who is summoning.
 * @param caster
 * object casting the spell.
 * @param dir
 * direction to place the monster.
 * @param spob
 * spell object casting. At this stage, all spob is really used for is to
 * adjust some values in the monster.
 * @retval 0
 * failed to summon something.
 * @retval 1
 * summoned correctly something.
 */
int summon_golem(object *op,object *caster,int dir,object *spob) {
    object *tmp, *god=NULL;
    archetype *at;
    char buf[MAX_BUF];

    /* Because there can be different golem spells, player may want to
     * 'lose' their old golem.
     */
    if(op->type==PLAYER &&
       op->contr->ranges[range_golem]!=NULL &&
       op->contr->golem_count == op->contr->ranges[range_golem]->count) {
	    draw_ext_info(NDI_UNIQUE, 0, op,
			  MSG_TYPE_SPELL, MSG_TYPE_SPELL_PET,
			  "You dismiss your existing golem.", NULL);
	    remove_ob(op->contr->ranges[range_golem]);
	    free_object(op->contr->ranges[range_golem]);
	    op->contr->ranges[range_golem]=NULL;
	    op->contr->golem_count=-1;
    }

    if (spob->other_arch)
	at = spob->other_arch;
    else if (spob->race) {
	god = find_god(determine_god(caster));

	if (!god) {
	    draw_ext_info_format(NDI_UNIQUE, 0,op,
			 MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			 "You must worship a god to cast %s.",
			 "You must worship a god to cast %s.",
			 spob->name);
	    return 0;
	}
	at = determine_holy_arch (god, spob->race);
	if (!at) {
	    draw_ext_info_format(NDI_UNIQUE, 0,op,
			 MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			 "%s has no %s for you to call.",
			 "%s has no %s for you to call.",
			 god->name,spob->race);
	    return 0;
	}
    } else {
	LOG(llevError,"Spell %s lacks other_arch\n", spob->name);
	return 0;
    }

    if(!dir)
	dir=find_free_spot(NULL,op->map,op->x,op->y,1,SIZEOFFREE1+1);

    if ((dir==-1) || ob_blocked(&at->clone, op->map, op->x + freearr_x[dir], op->y + freearr_y[dir])) {
	draw_ext_info(NDI_UNIQUE, 0,op,
		      MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "There is something in the way.", NULL);
	return 0;
    }
    /* basically want to get proper map/coordinates for this object */

    if(!(tmp=fix_summon_pet(at,op,dir,GOLEM))) {
	draw_ext_info(NDI_UNIQUE, 0,op,
		      MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "Your spell fails.", NULL);
	return 0;
    }

    if(op->type==PLAYER) {
	tmp->type=GOLEM;
	set_owner(tmp,op);
	set_spell_skill(op, caster, spob, tmp);
	op->contr->ranges[range_golem]=tmp;
	op->contr->golem_count = tmp->count;
	/* give the player control of the golem */
	op->contr->shoottype=range_golem;
    } else {
	if(QUERY_FLAG(op, FLAG_FRIENDLY)) {
	    object *owner = get_owner(op);
	    if (owner != NULL) { /* For now, we transfer ownership */
		set_owner (tmp, owner);
		tmp->attack_movement = PETMOVE;
		add_friendly_object (tmp);
		SET_FLAG (tmp, FLAG_FRIENDLY);
	    }
	}
	SET_FLAG(tmp, FLAG_MONSTER);
    }

    /* make the speed positive.*/
    tmp->speed = FABS(tmp->speed);

    /*  This sets the level dependencies on dam and hp for monsters */
    /* players can't cope with too strong summonings. */
    /* but monsters can.  reserve these for players. */
    if(op->type==PLAYER) {
	tmp->stats.hp += spob->duration +
                     SP_level_duration_adjust(caster,spob);
	if (!spob->stats.dam)
	    tmp->stats.dam += SP_level_dam_adjust(caster,spob);
	else
	    tmp->stats.dam= spob->stats.dam +
                    SP_level_dam_adjust(caster,spob);
	tmp->speed += .02 * SP_level_range_adjust(caster,spob);
	tmp->speed = MIN(tmp->speed, 1.0);
	if (spob->attacktype) tmp->attacktype = spob->attacktype;
    }
    tmp->stats.wc -= SP_level_range_adjust(caster,spob);

    /* limit the speed to 0.3 for non-players, 1 for players. */

    /* make experience increase in proportion to the strength.
     * this is a bit simplistic - we are basically just looking at how
     * often the sp doubles and use that as the ratio.
     */
    tmp->stats.exp *= 1 + (MAX(spob->stats.maxgrace, spob->stats.sp) /
			   caster_level(caster, spob));
    tmp->speed_left= 0;
    tmp->direction=dir;

    /* Holy spell - some additional tailoring */
    if (god) {
	object *tmp2;

	sprintf(buf,"%s of %s",spob->name,god->name);
	buf[0] = toupper(buf[0]);
	for (tmp2=tmp; tmp2; tmp2=tmp2->more) {
	    if (tmp2->name) free_string(tmp2->name);
	    tmp2->name = add_string(buf);
	}
	tmp->attacktype |= god->attacktype;
	memcpy(tmp->resist, god->resist, sizeof(tmp->resist));
	if (tmp->race) FREE_AND_CLEAR_STR(tmp->race);
	if (god->race) tmp->race = add_string(god->race);
	if (tmp->slaying) FREE_AND_CLEAR_STR(tmp->slaying);
	if (god->slaying) tmp->slaying = add_string(god->slaying);
	/* safety, we must allow a god's servants some reasonable attack */
	if(!(tmp->attacktype&AT_PHYSICAL)) tmp->attacktype|=AT_PHYSICAL;
    }

    insert_ob_in_map(tmp,tmp->map,op,0);
    return 1;
}


/***************************************************************************
 *
 * Summon monster/pet/other object code
 *
 ***************************************************************************/

/**
 * Returns a monster (chosen at random) that this particular player (and his
 * god) find acceptable. This checks level, races allowed by god, etc
 * to determine what is acceptable.
 *
 * @param pl
 * player summoning.
 * @param god
 * god the player worships.
 * @param summon_level
 * summoning level.
 * @return
 * suitable monster, or NULL if no match found.
 */
static object *choose_cult_monster(object *pl, object *god, int summon_level) {
    char buf[MAX_BUF];
    const char *race;
    int racenr, mon_nr,i;
    racelink *list;
    objectlink *tobl;
    object *otmp;

    /* Determine the number of races available */
    racenr=0;
    strcpy(buf,god->race);
    race = strtok(buf,",");
    while(race) {
	racenr++;
	race = strtok(NULL,",");
     }

    /* next, randomly select a race from the aligned_races string */
    if(racenr>1) {
	racenr = rndm(0, racenr-1);
	strcpy(buf,god->race);
        race = strtok(buf,",");
        for(i=0;i<racenr;i++)
	     race = strtok(NULL,",");
    } else
        race = god->race;


    /* see if a we can match a race list of monsters.  This should not
     * happen, so graceful recovery isn't really needed, but this sanity
     * checking is good for cases where the god archetypes mismatch the
     * race file
     */
    if((list=find_racelink(race))==NULL) {
	draw_ext_info_format(NDI_UNIQUE, 0,pl,
	     MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
	    "The spell fails! %s's creatures are beyond the range of your summons",
	    "The spell fails! %s's creatures are beyond the range of your summons",
	     god->name);
	LOG(llevDebug,"choose_cult_monster() requested non-existent aligned race!\n");
	return 0;
    }

    /* search for an apprplritate monster on this race list */
    mon_nr=0;
    for(tobl=list->member;tobl;tobl=tobl->next) {
	otmp=tobl->ob;
	if(!otmp||!QUERY_FLAG(otmp,FLAG_MONSTER)) continue;
	if(otmp->level<=summon_level) mon_nr++;
    }

    /* If this god has multiple race entries, we should really choose another.
     * But then we either need to track which ones we have tried, or just
     * make so many calls to this function, and if we get so many without
     * a valid entry, assuming nothing is available and quit.
     */
    if (!mon_nr) return NULL;

    mon_nr = rndm(0, mon_nr-1);
    for(tobl=list->member;tobl;tobl=tobl->next) {
	otmp=tobl->ob;
	if(!otmp||!QUERY_FLAG(otmp,FLAG_MONSTER)) continue;
	if(otmp->level<=summon_level && !mon_nr--) return otmp;
    }
    /* This should not happen */
    LOG(llevDebug,"choose_cult_monster() mon_nr was set, but did not find a monster\n");
    return NULL;
}

/**
 * General purpose summoning function.
 *
 * @param op
 * who is summoning.
 * @param caster
 * what object did cast the summoning spell.
 * @param spell_ob
 * actual spell object for summoning.
 * @param dir
 * direction to summon in.
 * @param stringarg
 * additional parameters.
 * @retval 0
 * nothing was summoned.
 * @retval 1
 * something was summoned.
 */
int summon_object(object *op, object *caster, object *spell_ob, int dir, const char *stringarg)
{
    sint16 x,y, nrof=1, i;
    archetype *summon_arch;
    int ndir, mult;

    if (spell_ob->other_arch) {
        summon_arch = spell_ob->other_arch;
    } else if (spell_ob->randomitems) {
        int level = caster_level(caster, spell_ob);
        treasure *tr, *lasttr=NULL;;

        /* In old code, this was a very convuluted for statement,
         * with all the checks in the 'for' portion itself.  Much
         * more readable to break some of the conditions out.
         */
        for (tr=spell_ob->randomitems->items; tr; tr=tr->next) {
            if (level < tr->magic)
                break;
            lasttr = tr;
            if(stringarg && !strcmp(tr->item->name,stringarg))
                break;
            if (tr->next == NULL || tr->next->item == NULL)
                break;
        }
        if (!lasttr) {
            LOG(llevError,"Treasurelist %s did not generate a valid entry in summon_object\n",
                spell_ob->randomitems->name);
            draw_ext_info(NDI_UNIQUE, 0, op,
                MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
                "The spell fails to summon any monsters.", NULL);
            return 0;
        }
        summon_arch = lasttr->item;
        nrof = lasttr->nrof;

    } else if (spell_ob->race && !strcmp(spell_ob->race,"GODCULTMON")) {
        object *god=find_god(determine_god(op)), *mon, *owner;
        int summon_level, tries;

        if (!god && ((owner=get_owner(op))!=NULL)) {
            god = find_god(determine_god(owner));
        }
        /* If we can't find a god, can't get what monster to summon */
        if (!god)
            return 0;

        if (!god->race) {
            draw_ext_info_format(NDI_UNIQUE, 0,op,
                MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
                "%s has no creatures that you may summon!",
                "%s has no creatures that you may summon!",
                god->name);
            return 0;
        }
        /* the summon level */
        summon_level=caster_level(caster, spell_ob);
        if (summon_level==0)
            summon_level=1;
        tries = 0;
        do {
            mon = choose_cult_monster(op, god,summon_level);
            if (!mon) {
                draw_ext_info_format(NDI_UNIQUE, 0,op,
                    MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
                    "%s fails to send anything.",
                    "%s fails to send anything.",
                    god->name);
                return 0;
            }
            ndir = dir;
            if (!ndir)
                ndir = find_free_spot(mon, op->map, op->x, op->y, 1, SIZEOFFREE);
            if (ndir == -1 || ob_blocked(mon,op->map, op->x + freearr_x[ndir], op->y+freearr_y[ndir])) {
                ndir=-1;
                if (++tries == 5) {
                    draw_ext_info(NDI_UNIQUE, 0,op,
                        MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
                        "There is something in the way.", NULL);
                    return 0;
                }
            }
        } while (ndir == -1);
        if (mon->level > (summon_level/2))
            nrof = random_roll(1, 2, op, PREFER_HIGH);
        else
            nrof =  die_roll(2, 2, op, PREFER_HIGH);
        summon_arch = mon->arch;
    } else {
        summon_arch = NULL;
    }

    if (spell_ob->stats.dam)
        nrof += spell_ob->stats.dam + SP_level_dam_adjust(caster, spell_ob);

    if (!summon_arch) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
            "There is no monsters available for summoning.", NULL);
        return 0;
    }

    if (dir) {
        /* Only fail if caster specified a blocked direction. */
        x = freearr_x[dir];
        y = freearr_y[dir];
        if (ob_blocked(&summon_arch->clone, op->map, op->x + x, op->y + y)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
                        "There is something in the way.", NULL);

            return 0;
        }
    }

    mult = (RANDOM()%2 ? -1 : 1);

    for (i=1; i <= nrof; i++) {
        archetype *atmp;
        object *prev=NULL, *head=NULL, *tmp;

        if (dir) {
            ndir = absdir (dir + (i / 2) * mult);
            mult = - mult;
        } else
            ndir = find_free_spot(&summon_arch->clone, op->map, op->x, op->y, 1, SIZEOFFREE);

        if (ndir > 0) {
            x = freearr_x[ndir];
            y = freearr_y[ndir];
        }

        if (ndir == -1 || ob_blocked(&summon_arch->clone, op->map, op->x + x, op->y + y))
            continue;

        for (atmp = summon_arch; atmp!=NULL; atmp=atmp->more) {
            tmp = arch_to_object(atmp);
            if (atmp == summon_arch) {
                if (QUERY_FLAG(tmp, FLAG_MONSTER)) {
                    set_owner(tmp, op);
                    set_spell_skill(op, caster, spell_ob, tmp);
                    tmp->enemy = op->enemy;
                    tmp->type = 0;
                    CLEAR_FLAG(tmp, FLAG_SLEEP);
                    if (op->type == PLAYER || QUERY_FLAG(op, FLAG_FRIENDLY)) {
                        /* If this is not set, we make it friendly */
                        if (!QUERY_FLAG(spell_ob, FLAG_MONSTER)) {
                            SET_FLAG(tmp, FLAG_FRIENDLY);
                            add_friendly_object(tmp);
                            tmp->stats.exp = 0;
                            if (spell_ob->attack_movement)
                                tmp->attack_movement = spell_ob->attack_movement;
                            if (get_owner(op))
                                set_owner(tmp, get_owner(op));
                        }
                    }
                }
                if (tmp->speed > MIN_ACTIVE_SPEED) tmp->speed_left = -1;
            }
            if (head == NULL)
                head = tmp;
            else {
                tmp->head = head;
                prev->more = tmp;
            }
            prev = tmp;
            tmp->x = op->x + x + tmp->arch->clone.x;
            tmp->y = op->y + y + tmp->arch->clone.y;
	    tmp->map = get_map_from_coord(op->map, &tmp->x, &tmp->y);
        }
        head->direction = freedir[ndir];
        head->stats.exp = 0;
        head = insert_ob_in_map(head, head->map, op, 0);
        if (head && head->randomitems) {
            create_treasure(head->randomitems, head, GT_APPLY | GT_STARTEQUIP, 6, 0);
        }
        if (head != NULL) {
            mark_inventory_as_no_drop(head);
        }
    } /* for i < nrof */
    return 1;
}

/**
 * Recursively look through the owner property of objects until the real owner
 * is found
 *
 * @param ob
 * item we're searching the owner of.
 * @return
 * owner, NULL if nothing found.
 */
static object *get_real_owner(object *ob) {
	object *realowner = ob;

	if (realowner == NULL) return NULL;

	while(get_owner(realowner) != NULL)
	{
		realowner = get_owner(realowner);
	}
	return realowner;
}

/**
 * Determines if checks so pets don't attack players or other pets should be
 * overruled by the arena petmode.
 *
 * @param pet
 * pet considered.
 * @param owner
 * pet's owner.
 * @param target
 * potential pet target.
 * @retval 0
 * pet shouldn't attack target.
 * @retval 1
 * target is a suitable victim for the pet.
 */
int should_arena_attack(object *pet,object *owner,object *target) {
	object *rowner, *towner;

	/* exit if the target, pet, or owner is null. */
	if ((target == NULL) || (pet == NULL) || (owner == NULL)) return 0;

	/* get the owners of itself and the target, this is to deal with pets of
	pets */
	rowner = get_real_owner(owner);
	if (target->type != PLAYER) {
		towner = get_real_owner(target);
	} else {
		towner = 0;
	}

	/* if the pet has no owner, exit with error */
	if (rowner == NULL) {
		LOG(llevError,"Pet has no owner.\n");
		return 0;
	}

	/* if the target is not a player, and has no owner, we shouldn't be here
	*/
	if (towner == NULL && target->type != PLAYER) {
		LOG(llevError,"Target is not a player but has no owner. We should not be here.\n");
		return 0;
	}

	/* make sure that the owner is a player */
	if (rowner->type != PLAYER) return 0;

	/* abort if the petmode is not arena */
	if (rowner->contr->petmode != pet_arena) return 0;

	/* abort if the pet, it's owner, or the target is not on battleground*/
	if (!(op_on_battleground(pet, NULL, NULL) &&
	     op_on_battleground(owner, NULL, NULL) &&
	     op_on_battleground(target, NULL, NULL)))
	     return 0;

	/* if the target is a monster, make sure it's owner is not the same */
	if (target->type != PLAYER && rowner == towner) return 0;

	/* check if the target is a player which affects how it will handle
	parties */
	if (target->type != PLAYER) {
		/* if the target is owned by a player make sure than make sure
		it's not in the same party */
		if (towner->type == PLAYER && rowner->contr->party != NULL) {
			if (rowner->contr->party == towner->contr->party) return 0;
		}
	} else {
		/* if the target is a player make sure than make sure it's not
		in the same party */
		if (rowner->contr->party != NULL){
			if (rowner->contr->party == target->contr->party) return 0;
		}
	}

	return 1;
}
