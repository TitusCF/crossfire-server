/*
 * static char *rcsid_time_c =
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

/*
 * Routines that is executed from objects based on their speed have been
 * collected in this file.
 */

#include <global.h>
#include <spells.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/** The following removes doors.  The functions check to see if similar 
 * doors are next to the one that is being removed, and if so, set it
 * so those will be removed shortly (in a cascade like fashion.)
 */

void remove_door(object *op) {
  int i;
  object *tmp;
  for(i=1;i<9;i+=2)
    if((tmp=present(DOOR,op->map,op->x+freearr_x[i],op->y+freearr_y[i]))!=NULL) {
      tmp->speed = 0.1;
      update_ob_speed(tmp);
      tmp->speed_left= -0.2;
    }

  if(op->other_arch)
  {
      tmp=arch_to_object(op->other_arch);
      tmp->x=op->x;tmp->y=op->y;tmp->map=op->map;tmp->level=op->level;
      insert_ob_in_map(tmp,op->map,op,0);
  }
  remove_ob(op);
  free_object(op);
}

/**
 * Same as remove_door but for locked doors.
 */
void remove_door2(object *op) {
  int i;
  object *tmp;
  for(i=1;i<9;i+=2) {
    tmp=present(LOCKED_DOOR,op->map,op->x+freearr_x[i],op->y+freearr_y[i]);
    if(tmp && tmp->slaying == op->slaying) {/* same key both doors */
      tmp->speed = 0.1;
      update_ob_speed(tmp);
      tmp->speed_left= -0.2;
    }
  }
  if(op->other_arch)
  {
      tmp=arch_to_object(op->other_arch);
      tmp->x=op->x;tmp->y=op->y;tmp->map=op->map;tmp->level=op->level;
      insert_ob_in_map(tmp,op->map,op,0);
  }
  remove_ob(op);
  free_object(op);
}

/** Will generate a monster according to content
 * of generator.
 */
static void generate_monster_inv(object *gen) {
    int i;
    int nx, ny;
    object *op,*head=NULL;

    int qty=0;
    /* Code below assumes the generator is on a map, as it tries
     * to place the monster on the map.  So if the generator
     * isn't on a map, complain and exit.
     */
    if (gen->map == NULL) {
	LOG(llevError,"Generator (%s) not on a map?\n", gen->name);
	return;
    }
    /*First count numer of objects in inv*/
    for (op=gen->inv;op;op=op->below)
        qty++;
    if (!qty){
        LOG(llevError,"Generator (%s) has no inventory in generate_monster_inv?\n", gen->name);
        return;/*No inventory*/
        }
    qty=rndm(0,qty-1);
    for (op=gen->inv;qty;qty--)
        op=op->below;
    i=find_multi_free_spot_around(op, gen, &nx, &ny);
    if (i==-1)
        return;
    head=object_create_clone(op);
    CLEAR_FLAG(head, FLAG_IS_A_TEMPLATE);
    unflag_inv (head,FLAG_IS_A_TEMPLATE);
    if (rndm(0, 9))
        generate_artifact(head, gen->map->difficulty);
    insert_ob_in_map_at(head,gen->map,gen,0,nx,ny);
    if (QUERY_FLAG(head, FLAG_FREED)) return;
    fix_multipart_object(head);
    if(HAS_RANDOM_ITEMS(head))
        create_treasure(head->randomitems,head,GT_APPLY,
                      gen->map->difficulty,0);
}

static void generate_monster_arch(object *gen) {
    int i;
    int nx, ny;
    object *op,*head=NULL,*prev=NULL;
    archetype *at=gen->other_arch;

    if(gen->other_arch==NULL) {
	LOG(llevError,"Generator without other_arch: %s\n",gen->name);
	return;
    }
    /* Code below assumes the generator is on a map, as it tries
     * to place the monster on the map.  So if the generator
     * isn't on a map, complain and exit.
     */
    if (gen->map == NULL) {
	LOG(llevError,"Generator (%s) not on a map?\n", gen->name);
	return;
    }
    i=find_multi_free_spot_around(&at->clone, gen, &nx, &ny);
    if (i==-1) return;
    while(at!=NULL) {
	op=arch_to_object(at);
	op->x=nx+at->clone.x;
	op->y=ny+at->clone.y;

	if(head!=NULL)
	    op->head=head,prev->more=op;

	if (rndm(0, 9)) generate_artifact(op, gen->map->difficulty);
	insert_ob_in_map(op,gen->map,gen,0);
	if (QUERY_FLAG(op, FLAG_FREED)) return;
	if(HAS_RANDOM_ITEMS(op))
	    create_treasure(op->randomitems,op,GT_APPLY,
                      gen->map->difficulty,0);
	if(head==NULL)
	    head=op;
	prev=op;
	at=at->more;
    }
}

static void generate_monster(object *gen) {

    if(GENERATE_SPEED(gen)&&rndm(0, GENERATE_SPEED(gen)-1))
        return;
    if (QUERY_FLAG(gen,FLAG_CONTENT_ON_GEN))
        generate_monster_inv(gen);
    else
        generate_monster_arch(gen);

}

static void remove_force(object *op) {
    if (--op->duration > 0) return;

    switch (op->subtype) {
	case FORCE_CONFUSION:
	    if(op->env!=NULL) {
		CLEAR_FLAG(op->env, FLAG_CONFUSED);
		draw_ext_info(NDI_UNIQUE, 0,op->env,
			      MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
			      "You regain your senses.", NULL);
	    }

	default:
	    if(op->env!=NULL) {
		CLEAR_FLAG(op, FLAG_APPLIED);
		change_abil(op->env,op);
        fix_object(op->env);
	    }
    }
    remove_ob(op);
    free_object(op);
}

static void remove_blindness(object *op) {
  if(--op->stats.food > 0)
    return;
  CLEAR_FLAG(op, FLAG_APPLIED);
  if(op->env!=NULL) { 
     change_abil(op->env,op);
     fix_object(op->env);
  }
  remove_ob(op);
  free_object(op);
}

static void poison_more(object *op) {
  if(op->env==NULL||!QUERY_FLAG(op->env,FLAG_ALIVE)||op->env->stats.hp<0) {
    remove_ob(op);
    free_object(op);
    return;
  }
  if(op->stats.food==1) {
    /* need to remove the object before fix_player is called, else fix_object
     * will not do anything.
     */
    if(op->env->type==PLAYER) {
      CLEAR_FLAG(op, FLAG_APPLIED);
      fix_object(op->env);
      draw_ext_info(NDI_UNIQUE, 0,op->env, 
		    MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
		    "You feel much better now.", NULL);
    }
    remove_ob(op);
    free_object(op);
    return;
  }
  if(op->env->type==PLAYER) {
    op->env->stats.food--;
    /* Not really the start of a bad effect, more the continuing effect */
    draw_ext_info(NDI_UNIQUE, 0,op->env, 
		  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_START,
		  "You feel very sick...", NULL);
  }
  (void)hit_player(op->env,
                   op->stats.dam,
                   op,AT_INTERNAL,1);
}


static void move_gate(object *op) { /* 1 = going down, 0 = goind up */
    object *tmp;

    if(op->stats.wc < 0 || (int)op->stats.wc  >= NUM_ANIMATIONS(op)) {
	LOG(llevError,"Gate error: animation was %d, max=%d\n",op->stats.wc,
	    NUM_ANIMATIONS(op));
	dump_object(op);
	LOG(llevError,"%s\n",errmsg);
	op->stats.wc=0;
    }

    /* We're going down */
    if(op->value) {
	if(--op->stats.wc<=0) { /* Reached bottom, let's stop */
	    op->stats.wc=0;
	    if(op->arch->clone.speed)
		op->value=0;
	    else {
		op->speed = 0;
		update_ob_speed(op);
	    }
	}
	if((int)op->stats.wc < (NUM_ANIMATIONS(op)/2+1)) {
	    op->move_block = 0;
	    CLEAR_FLAG(op, FLAG_BLOCKSVIEW);
	    update_all_los(op->map, op->x, op->y);
	}
	SET_ANIMATION(op, op->stats.wc);
	update_object(op,UP_OBJ_CHANGE);
	return;
    }

    /* We're going up */

    /* First, lets see if we are already at the top */
    if((unsigned char) op->stats.wc==(NUM_ANIMATIONS(op)-1)) {

	/* Check to make sure that only non pickable and non rollable
	 * objects are above the gate.  If so, we finish closing the gate,
	 * otherwise, we fall through to the code below which should lower
	 * the gate slightly.
	 */

	for (tmp=op->above; tmp!=NULL; tmp=tmp->above) 
	    if (!QUERY_FLAG(tmp, FLAG_NO_PICK)
		|| QUERY_FLAG(tmp, FLAG_CAN_ROLL)
		|| QUERY_FLAG(tmp, FLAG_ALIVE))
		break;

	if (tmp==NULL) {
	    if(op->arch->clone.speed)
		op->value=1;
	    else {
		op->speed = 0;
		update_ob_speed(op); /* Reached top, let's stop */
	    }
	    return;
	}
    }

    if(op->stats.food) {    /* The gate is going temporarily down */
	if(--op->stats.wc<=0) { /* Gone all the way down? */
	    op->stats.food=0;	    /* Then let's try again */
	    op->stats.wc=0;
	}
    } else {                /* The gate is still going up */
	op->stats.wc++;

	if((int)op->stats.wc >= (NUM_ANIMATIONS(op)))
	    op->stats.wc=(signed char)NUM_ANIMATIONS(op)-1;

	/* If there is something on top of the gate, we try to roll it off.
	 * If a player/monster, we don't roll, we just hit them with damage
	 */
	if((int)op->stats.wc >= NUM_ANIMATIONS(op)/2) {
	    /* Halfway or further, check blocks */
	    /* First, get the top object on the square. */
	    for(tmp=op->above;tmp!=NULL && tmp->above!=NULL;tmp=tmp->above);

	    if(tmp!=NULL) {
		if(QUERY_FLAG(tmp, FLAG_ALIVE)) {
		    hit_player(tmp, random_roll(1, op->stats.dam, tmp, PREFER_LOW), op, AT_PHYSICAL, 1);
		    if(tmp->type==PLAYER) 
			draw_ext_info_format(NDI_UNIQUE, 0, tmp,
					     MSG_TYPE_VICTIM, MSG_TYPE_VICTIM_WAS_HIT,
					     "You are crushed by the %s!",
					     "You are crushed by the %s!",
					     op->name);
		} else 
		    /* If the object is not alive, and the object either can
		     * be picked up or the object rolls, move the object
		     * off the gate.
		     */
		    if(!QUERY_FLAG(tmp, FLAG_ALIVE)
			&& (!QUERY_FLAG(tmp, FLAG_NO_PICK)
			   ||QUERY_FLAG(tmp,FLAG_CAN_ROLL))) {
		    /* If it has speed, it should move itself, otherwise: */
		    int i=find_free_spot(tmp,op->map,op->x,op->y,1,9);

		    /* If there is a free spot, move the object someplace */
		    if (i!=-1) {
			remove_ob(tmp);
			tmp->x+=freearr_x[i],tmp->y+=freearr_y[i];
			insert_ob_in_map(tmp,op->map,op,0);
		    }
		}
	    }

	    /* See if there is still anything blocking the gate */
	    for (tmp=op->above; tmp!=NULL; tmp=tmp->above) 
		if (!QUERY_FLAG(tmp, FLAG_NO_PICK)
			|| QUERY_FLAG(tmp, FLAG_CAN_ROLL)
			|| QUERY_FLAG(tmp, FLAG_ALIVE))
		    break;

	    /* IF there is, start putting the gate down  */
	    if(tmp) {
		    op->stats.food=1;
	    } else {
		op->move_block = MOVE_ALL;
		if(!op->arch->clone.stats.ac)
		    SET_FLAG(op, FLAG_BLOCKSVIEW);
		update_all_los(op->map, op->x, op->y);
	    }
	} /* gate is halfway up */

	SET_ANIMATION(op, op->stats.wc);
	update_object(op,UP_OBJ_CHANGE);
    } /* gate is going up */
}

/**  hp      : how long door is open/closed
 *  maxhp   : initial value for hp
 *  sp      : 1 = open, 0 = close
 */
static void move_timed_gate(object *op)
{
  int v = op->value;

  if (op->stats.sp) {
    move_gate(op);
    if (op->value != v)   /* change direction ? */
      op->stats.sp = 0;
    return;
  } 
  if (--op->stats.hp <= 0) { /* keep gate down */
    move_gate(op);
    if (op->value != v) {  /* ready ? */
	op->speed = 0;
	update_ob_speed(op);
    }
  }
}

/**  slaying:    name of the thing the detector is to look for
 *	 speed:      frequency of 'glances'
 *	 connected:  connected value of detector
 *  sp:         1 if detection sets buttons
 *              -1 if detection unsets buttons
 */
static void move_detector(object *op) 
{
    object *tmp;
    int last = op->value;
    int detected;
    detected = 0;

    for(tmp = get_map_ob(op->map,op->x,op->y);tmp!=NULL&&!detected;tmp=tmp->above) {
		object *tmp2;
		if(op->stats.hp) {
		  for(tmp2= tmp->inv;tmp2;tmp2=tmp2->below) {
			 if(op->slaying && !strcmp(op->slaying,tmp->name)) detected=1;
			 if(tmp2->type==FORCE &&tmp2->slaying && !strcmp(tmp2->slaying,op->slaying)) detected=1;
		  }
		}
	if (op->slaying && !strcmp(op->slaying,tmp->name)) {
	  detected = 1;
	}
	else if (tmp->type==SPECIAL_KEY && tmp->slaying==op->slaying)
	    detected=1;
    }

    /* the detector sets the button if detection is found */
    if(op->stats.sp == 1)  {
	if(detected && last == 0) {
	    op->value = 1;
	    push_button(op);
	}
	if(!detected && last == 1) {
	    op->value = 0;
	    push_button(op);
	}
    }
    else { /* in this case, we unset buttons */
	if(detected && last == 1) {
	    op->value = 0;
	    push_button(op);
	}
	if(!detected && last == 0) {
	    op->value = 1;
	    push_button(op);
	}
    }
}


static void animate_trigger(object *op)
{
  if((unsigned char)++op->stats.wc >= NUM_ANIMATIONS(op)) {
    op->stats.wc = 0;
    check_trigger(op,NULL);
  } else {
    SET_ANIMATION(op, op->stats.wc);
    update_object(op,UP_OBJ_FACE);
  }
}

static void move_hole(object *op) { /* 1 = opening, 0 = closing */
    object *next,*tmp;

    if(op->value) { /* We're opening */
	if(--op->stats.wc<=0) { /* Opened, let's stop */
	    op->stats.wc=0;
	    op->speed = 0;
	    update_ob_speed(op);

	    /* Hard coding this makes sense for holes I suppose */
	    op->move_on = MOVE_WALK;
	    for (tmp=op->above; tmp!=NULL; tmp=next) {
		next=tmp->above;
		move_apply(op,tmp,tmp);
	    }
	}
	SET_ANIMATION(op, op->stats.wc);
	update_object(op,UP_OBJ_FACE);
	return;
    }
    /* We're closing */
    op->move_on = 0;

    op->stats.wc++;
    if((int)op->stats.wc >= NUM_ANIMATIONS(op))
	op->stats.wc=NUM_ANIMATIONS(op)-1;
    SET_ANIMATION(op, op->stats.wc);
    update_object(op,UP_OBJ_FACE);
    if((unsigned char) op->stats.wc==(NUM_ANIMATIONS(op)-1)) {
	op->speed = 0;
	update_ob_speed(op); /* closed, let's stop */
	return;
    }
}


/** stop_item() returns a pointer to the stopped object.  The stopped object
 * may or may not have been removed from maps or inventories.  It will not
 * have been merged with other items.
 *
 * This function assumes that only items on maps need special treatment.
 *
 * If the object can't be stopped, or it was destroyed while trying to stop
 * it, NULL is returned.
 *
 * fix_stopped_item() should be used if the stopped item should be put on
 * the map.
 */
object *stop_item (object *op)
{
    if (op->map == NULL)
        return op;

    switch (op->type)
    {
    case THROWN_OBJ:
        {
            object *payload = op->inv;
            if (payload == NULL)
                return NULL;
            remove_ob (payload);
            remove_ob (op);
            free_object (op);
            return payload;
        }

    case ARROW:
        if (op->speed >= MIN_ACTIVE_SPEED)
            op = fix_stopped_arrow (op);
        return op;

    default:
        return op;
    }
}

/** fix_stopped_item() - put stopped item where stop_item() had found it.
 * Inserts item into the old map, or merges it if it already is on the map.
 *
 * 'map' must be the value of op->map before stop_item() was called.
 */
void fix_stopped_item (object *op, mapstruct *map, object *originator)
{
    if (map == NULL)
        return;
    if (QUERY_FLAG (op, FLAG_REMOVED))
        insert_ob_in_map (op, map, originator,0);
    else if (op->type == ARROW)
        merge_ob (op, NULL);   /* only some arrows actually need this */
}


object *fix_stopped_arrow (object *op)
{
    if(rndm(0, 99) < op->stats.food) {
	/* Small chance of breaking */
        remove_ob (op);
	free_object(op);
	return NULL;
    }

    op->direction=0;
    op->move_on=0;
    op->move_type=0;
    op->speed = 0;
    update_ob_speed(op);
    op->stats.wc = op->stats.sp;
    op->stats.dam= op->stats.hp;
    op->attacktype = op->stats.grace;
    if (op->slaying != NULL)
	FREE_AND_CLEAR_STR(op->slaying);

    if (op->skill != NULL)
	FREE_AND_CLEAR_STR(op->skill);

    if (op->spellarg != NULL) {
	op->slaying = add_string(op->spellarg);
	free(op->spellarg);
	op->spellarg = NULL;
    } else
	op->slaying = NULL;

    /* Reset these to zero, so that can_merge will work properly */
    op->spellarg = NULL;
    op->stats.sp = 0;
    op->stats.hp = 0;
    op->stats.grace = 0;
    op->level = 0;
    op->face=op->arch->clone.face;
    op->owner=NULL; /* So that stopped arrows will be saved */
    update_object (op,UP_OBJ_FACE);
    return op;
}

/** stop_arrow() - what to do when a non-living flying object
 * has to stop. Sept 96 - I added in thrown object code in
 * here too. -b.t.
 *
 * Returns a pointer to the stopped object (which will have been removed
 * from maps or inventories), or NULL if was destroyed.
 */

static void stop_arrow (object *op)
{
    /* Lauwenmark: Handle for plugin stop event */
    execute_event(op, EVENT_STOP,NULL,NULL,NULL,SCRIPT_FIX_NOTHING);
    if (op->inv) {
	object *payload = op->inv;
	remove_ob (payload);
	clear_owner(payload);
        insert_ob_in_map (payload, op->map, payload,0);
        remove_ob (op);
	free_object (op);
    } else {
        op = fix_stopped_arrow (op);
        if (op)
            merge_ob (op, NULL);
    }
}

/** Move an arrow along its course.  op is the arrow or thrown object.
 */

void move_arrow(object *op) {
    object *tmp;
    sint16 new_x, new_y;
    int was_reflected, mflags;
    mapstruct *m;

    if(op->map==NULL) {
	LOG (llevError, "BUG: Arrow had no map.\n");
	remove_ob(op);
	free_object(op);
	return;
    }

    /* we need to stop thrown objects at some point. Like here. */ 
    if(op->type==THROWN_OBJ) {
	/* If the object that the THROWN_OBJ encapsulates disappears,
	 * we need to have this object go away also - otherwise, you get
	 * left over remnants on the map.  Where this currently happens
	 * is if the player throws a bomb - the bomb explodes on its own,
	 * but this object sticks around.  We could handle the cleanup in the
	 * bomb code, but there are potential other cases where that could happen,
	 * and it is easy enough to clean it up here.
	 */
        if (op->inv == NULL) {
	    remove_ob(op);
	    free_object(op);
            return;
	}
	if(op->last_sp-- < 0) { 
	    stop_arrow (op);
	    return; 
	}
    }

    /* if the arrow is moving too slow.. stop it.  0.5 was chosen as lower
       values look rediculous. */
    if (op->speed < 0.5 && op->type==ARROW) {
	stop_arrow(op);
	return;
    }

    /* Calculate target map square */
    new_x = op->x + DIRX(op);
    new_y = op->y + DIRY(op);
    was_reflected = 0;

    m = op->map;
    mflags = get_map_flags(m, &m, new_x, new_y, &new_x, &new_y);

    if (mflags & P_OUT_OF_MAP) {
	stop_arrow(op);
	return;
    }

    /* only need to look for living creatures if this flag is set */
    if (mflags & P_IS_ALIVE) {
	for (tmp = get_map_ob(m, new_x, new_y); tmp != NULL; tmp=tmp->above)
	     if (QUERY_FLAG(tmp, FLAG_ALIVE)) break;

    
	/* Not really fair, but don't let monsters hit themselves with
	 * their own arrow - this can be because they fire it then
	 * move into it.
	 */

	if (tmp != NULL && tmp != op->owner) {
	    /* Found living object, but it is reflecting the missile.  Update
	     * as below. (Note that for living creatures there is a small
	     * chance that reflect_missile fails.)
	     */

	    if (QUERY_FLAG (tmp, FLAG_REFL_MISSILE)  &&
		(rndm(0, 99)) < (90-op->level/10)) {

		int number = op->face->number;
	    
		op->direction = absdir (op->direction + 4);
		op->state = 0;
		if (GET_ANIM_ID (op)) {
		    number += 4;
		    if (number > GET_ANIMATION (op, 8))
			number -= 8;
		    op->face = &new_faces[number];
		}
		was_reflected = 1;   /* skip normal movement calculations */
	    }
	     else {
		/* Attack the object. */
		op = hit_with_arrow (op, tmp);
		if (op == NULL)
		    return;
	     }
	} /* if this is not hitting its owner */
    } /* if there is something alive on this space */


    if (OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, new_x, new_y))) {
	int retry=0;

	/* if the object doesn't reflect, stop the arrow from moving
	 * note that this code will now catch cases where a monster is
	 * on a wall but has reflecting - the arrow won't reflect.
	 * Mapmakers shouldn't put monsters on top of wall in the first
	 * place, so I don't consider that a problem.
	 */
	if(!QUERY_FLAG(op, FLAG_REFLECTING) || !(rndm(0, 19))) {
	    stop_arrow (op);
	    return;
	} else {
	    /* If one of the major directions (n,s,e,w), just reverse it */
	    if(op->direction&1) {
		op->direction=absdir(op->direction+4);
		retry=1;
	    }
	    /* There were two blocks with identical code -
	     * use this retry here to make this one block
	     * that did the same thing.
	     */
	    while (retry<2) {
		int left, right, mflags;
		mapstruct *m1;
		sint16	x1, y1;

		retry++;

		/* Need to check for P_OUT_OF_MAP: if the arrow is tavelling
		 * over a corner in a tiled map, it is possible that
		 * op->direction is within an adjacent map but either
		 * op->direction-1 or op->direction+1 does not exist.
		 */
		mflags = get_map_flags(op->map,&m1, op->x+freearr_x[absdir(op->direction-1)],
		       op->y+freearr_y[absdir(op->direction-1)], &x1, &y1);
		left = (mflags & P_OUT_OF_MAP) ? 0 : OB_TYPE_MOVE_BLOCK(op, (GET_MAP_MOVE_BLOCK(m1, x1, y1)));

		mflags = get_map_flags(op->map,&m1, op->x+freearr_x[absdir(op->direction+1)],
		   op->y+freearr_y[absdir(op->direction+1)], &x1, &y1);
		right = (mflags & P_OUT_OF_MAP) ? 0 : OB_TYPE_MOVE_BLOCK(op, (GET_MAP_MOVE_BLOCK(m1, x1, y1)));

		if(left==right)
		    op->direction=absdir(op->direction+4);
		else if(left)
		    op->direction=absdir(op->direction+2);
		else if(right)
		    op->direction=absdir(op->direction-2);

		mflags = get_map_flags(op->map,&m1, op->x+DIRX(op),op->y+DIRY(op), &x1, &y1);

		/* If this space is not out of the map and not blocked, valid space -
		 * don't need to retry again.
		 */
		if (!(mflags & P_OUT_OF_MAP) && 
		  !OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m1, x1, y1))) break;

	    }
	    /* Couldn't find a direction to move the arrow to - just
	     * top it from moving.
	     */
	    if (retry==2) {
		stop_arrow (op);
		return;
	    }
	    /* update object image for new facing */
	    /* many thrown objects *don't* have more than one face */
	    if(GET_ANIM_ID(op))
		SET_ANIMATION(op, op->direction);
	} /* object is reflected */
    } /* object ran into a wall */

    /* Move the arrow. */
    remove_ob (op);
    op->x = new_x;
    op->y = new_y;

    /* decrease the speed as it flies. 0.05 means a standard bow will shoot
     * about 17 squares. Tune as needed.
     */
    op->speed -= 0.05;
    insert_ob_in_map (op, m, op,0);
}

/** This routine doesnt seem to work for "inanimate" objects that
 * are being carried, ie a held torch leaps from your hands!. 
 * Modified this routine to allow held objects. b.t. */

static void change_object(object *op) { /* Doesn`t handle linked objs yet */
  object *tmp,*env,*pl;
  int i,j;

  if(op->other_arch==NULL) {
    LOG(llevError,"Change object (%s) without other_arch error.\n", op->name);
    return;
  }

  /* In non-living items only change when food value is 0 */
  if(!QUERY_FLAG(op,FLAG_ALIVE)) {
	if(op->stats.food-- > 0) return; 
	else op->stats.food=1; /* so 1 other_arch is made */
  }
  env=op->env;
  remove_ob(op);
  for(i=0;i<NROFNEWOBJS(op);i++) {
    tmp=arch_to_object(op->other_arch);
    if (op->type == LAMP)
	  tmp->stats.food = op->stats.food-1;
    tmp->stats.hp=op->stats.hp; /* The only variable it keeps. */
    if(env) {
        tmp->x=env->x,tmp->y=env->y;
	tmp=insert_ob_in_ob(tmp,env);
	/* If this object is the players inventory, we need to tell the
	 * client of the change.  Insert_ob_in_map takes care of the
	 * updating the client, so we don't need to do that below.
	 */
	if ((pl=get_player_container(env))!=NULL) {
	    esrv_del_item(pl->contr, op->count);
	    esrv_send_item(pl, tmp);
	}
    } else {
        j=find_first_free_spot(tmp,op->map,op->x,op->y);
	if (j==-1)  /* No free spot */
	    free_object(tmp);
	else {
	    tmp->x=op->x+freearr_x[j],tmp->y=op->y+freearr_y[j];
	    insert_ob_in_map(tmp,op->map,op,0);
	}
    }
  }
  free_object(op);
}

void move_teleporter(object *op) {
    object *tmp, *head=op;

    /* if this is a multipart teleporter, handle the other parts
     * The check for speed isn't strictly needed - basically, if
     * there is an old multipart teleporter in which the other parts
     * have speed, we don't really want to call it twice for the same
     * function - in fact, as written below, part N would get called
     * N times without the speed check.
     */
    if (op->more && FABS(op->more->speed)<MIN_ACTIVE_SPEED) move_teleporter(op->more);

    if (op->head) head=op->head;

    for (tmp=op->above; tmp!=NULL; tmp=tmp->above)
	if (!QUERY_FLAG(tmp, FLAG_IS_FLOOR)) break;

    /* If nothing above us to move, nothing to do */
    if (!tmp || QUERY_FLAG(tmp, FLAG_WIZPASS)) return;

    if(EXIT_PATH(head)) {
	if(tmp->type==PLAYER) {
            /* Lauwenmark: Handle for plugin TRIGGER event */
            if (execute_event(op, EVENT_TRIGGER, tmp, NULL, NULL, SCRIPT_FIX_ALL) != 0)
                return;
	    enter_exit(tmp, head);
	}
	else
	    /* Currently only players can transfer maps */
	    return;
    }
    else if(EXIT_X(head)||EXIT_Y(head)) {
	if (out_of_map(head->map, EXIT_X(head), EXIT_Y(head))) {
	    LOG(llevError, "Removed illegal teleporter.\n");
	    remove_ob(head);
	    free_object(head);
	    return;
	}
        /* Lauwenmark: Handle for plugin TRIGGER event */
        if (execute_event(op, EVENT_TRIGGER, tmp, NULL, NULL, SCRIPT_FIX_ALL) != 0)
        return;
	transfer_ob(tmp,EXIT_X(head),EXIT_Y(head),0,head);
    }
    else {
	/* Random teleporter */
        /* Lauwenmark: Handle for plugin TRIGGER event */
        if (execute_event(op, EVENT_TRIGGER, tmp, NULL, NULL, SCRIPT_FIX_ALL) != 0)
            return;
	teleport(head, TELEPORTER, tmp);
    }
}


/**  This object will teleport someone to a different map
  *  and will also apply changes to the player from its inventory.
  *  This was invented for giving classes, but there's no reason it
  *  can't be generalized.
  */
void move_player_changer(object *op) {
    object *player;
    object *walk;
    char c;

    if (!op->above || !EXIT_PATH(op)) return;

    /* This isn't all that great - means that the player_mover
     * needs to be on top.
     */
    if(op->above->type==PLAYER) {
        /* Lauwenmark: Handle for plugin TRIGGER event */
        if (execute_event(op, EVENT_TRIGGER,op->above,NULL,NULL,SCRIPT_FIX_NOTHING)!=0)
            return;
	player=op->above;
	for(walk=op->inv;walk!=NULL;walk=walk->below)
	    apply_changes_to_player(player,walk);

    fix_object(player);
	esrv_send_inventory(op->above,op->above);
	esrv_update_item(UPD_FACE, op->above, op->above);
	
	/* update players death & WoR home-position */
	sscanf(EXIT_PATH(op), "%c", &c);
	if (c == '/') {
	    strcpy(player->contr->savebed_map, EXIT_PATH(op));
	    player->contr->bed_x = EXIT_X(op);
	    player->contr->bed_y = EXIT_Y(op);
	}
	else
            LOG(llevDebug,
                "WARNING: destination '%s' in player_changer must be an absolute path!\n",
		EXIT_PATH(op));
	
	enter_exit(op->above,op);
	save_player(player, 1);
    }
}

/**
 * firewalls fire other spells.
 * The direction of the wall is stored in op->stats.sp.
 * walls can have hp, so they can be torn down.
 */
void move_firewall(object *op) {
    object *spell;

    if ( ! op->map)
	return;   /* dm has created a firewall in his inventory */

    spell = op->inv;
    if (!spell || spell->type != SPELL) spell=&op->other_arch->clone;
    if (!spell) {
	LOG(llevError,"move_firewall: no spell specified (%s, %s, %d, %d)\n",
	    op->name, op->map->name, op->x, op->y);
	return;
    }

    cast_spell(op,op,op->stats.sp?op->stats.sp:rndm(1, 8),spell, NULL);
}


/**
 * move_player_mover:  this function takes a "player mover" as an
 * argument, and performs the function of a player mover, which is:
 *
 * a player mover finds any players that are sitting on it.  It
 * moves them in the op->stats.sp direction.  speed is how often it'll move.
 * If attacktype is nonzero it will paralyze the player.  If lifesave is set,
 * it'll dissapear after hp+1 moves.  If hp is set and attacktype is set,
 * it'll paralyze the victim for hp*his speed/op->speed
 */
void move_player_mover(object *op) {
    object *victim, *nextmover;
    int dir = op->stats.sp;
    sint16 nx, ny;
    mapstruct *m;

    /* Determine direction now for random movers so we do the right thing */
    if (!dir) dir=rndm(1, 8);

    for(victim=get_map_ob(op->map,op->x,op->y); victim !=NULL; victim=victim->above) {
	if(QUERY_FLAG(victim, FLAG_ALIVE) && !QUERY_FLAG(victim, FLAG_WIZPASS) && 
	   (victim->move_type & op->move_type || !victim->move_type)) {

	    if (victim->head) victim = victim->head;

	    if(QUERY_FLAG(op,FLAG_LIFESAVE)&&op->stats.hp--<0) {
		remove_ob(op);
		free_object(op);
		return;
	    }
	    nx = op->x+freearr_x[dir];
	    ny = op->y+freearr_y[dir];
	    m = op->map;
	    if (get_map_flags(m, &m, nx, ny, &nx, &ny) & P_OUT_OF_MAP) {
		LOG(llevError,"move_player_mover: Trying to push player off the map! map=%s (%d, %d)\n",
		    m->path, op->x, op->y);
		return ;
	    }
	    
	    if (should_director_abort(op, victim)) return ;

	    for(nextmover=get_map_ob(m,nx, ny); nextmover !=NULL; nextmover=nextmover->above) {
		if(nextmover->type == PLAYERMOVER) 
		    nextmover->speed_left=-.99;
		if(QUERY_FLAG(nextmover,FLAG_ALIVE)) {
		    op->speed_left=-1.1;  /* wait until the next thing gets out of the way */
		}
	    }

	    if(victim->type==PLAYER) { 
		/*  only level >=1 movers move people */
		if(op->level) {
		    /* Following is a bit of hack.  We need to make sure it
		     * is cleared, otherwise the player will get stuck in
		     * place.  This can happen if the player used a spell to
		     * get to this space.
		     */
		    victim->contr->fire_on=0;
		    victim->speed_left=-FABS(victim->speed);
		    move_player(victim, dir);
		}
		else return;
	    }
	    else move_object(victim,dir);

	    if(!op->stats.maxsp&&op->attacktype) op->stats.maxsp=2.0;

	    if(op->attacktype)  { /* flag to paralyze the player */

		victim->speed_left= -FABS(op->stats.maxsp*victim->speed/op->speed);
		/* Not sure why, but for some chars on metalforge, they
		 * would sometimes get -inf speed_left, and from the
		 * description, it could only happen here, so just put
		 * a lower sanity limit.  My only guess is that the 
		 * mover has 0 speed.
		 */
		if (victim->speed_left < -5.0) victim->speed_left=-5.0;
	    }
	}
    }
}

/**
 * Will duplicate a specified object placed on top of it.
 * connected: what will trigger it.
 * level: multiplier.  0 to destroy.
 * other_arch: the object to look for and duplicate.
 */

void move_duplicator(object *op) {
    object *tmp;

    if ( !op->other_arch ) {
        LOG(llevInfo, "Duplicator with no other_arch! %d %d %s\n", op->x, op->y, op->map ? op->map->path : "nullmap");
        return;
    }

    if (op->above == NULL)
	return;
    for (tmp=op->above; tmp != NULL; tmp=tmp->above) {
	if (strcmp(op->other_arch->name, tmp->arch->name) == 0) {
	    if (op->level <= 0) {
		remove_ob(tmp);
		free_object(tmp);
	    } else {
		uint64 new_nrof = (uint64)tmp->nrof*op->level;
		if (new_nrof >= 1UL<<31)
		    new_nrof = 1UL<<31;
		tmp->nrof = new_nrof;
	    }
	    break;
	}
    }
}

/**
 * move_creator (by peterm) 
 * it has the creator object create it's other_arch right on top of it.
 * connected:  what will trigger it
 * hp:  how many times it may create before stopping
 * lifesave:  if set, it'll never disappear but will go on creating
 *    everytime it's triggered
 * other_arch:  the object to create
 * Note this can create large objects, however, in that case, it
 * has to make sure that there is in fact space for the object.
 * It should really do this for small objects also, but there is
 * more concern with large objects, most notably a part being placed
 * outside of the map which would cause the server to crash
*/
void move_creator(object *creator) {
    object *new_ob;

    if(!QUERY_FLAG(creator, FLAG_LIFESAVE) && --creator->stats.hp < 0) {
	creator->stats.hp=-1;
	return;
    }

    if (creator->inv != NULL) {
	object *ob;
	int i;
	object *ob_to_copy;

	/* select random object from inventory to copy */
	ob_to_copy = creator->inv;
	for (ob = creator->inv->below, i = 1; ob != NULL; ob = ob->below, i++) {
	    if (rndm(0, i) == 0) {
		ob_to_copy = ob;
	    }
	}
	new_ob = object_create_clone(ob_to_copy);
	CLEAR_FLAG(new_ob, FLAG_IS_A_TEMPLATE);
	unflag_inv(new_ob, FLAG_IS_A_TEMPLATE);
    } else {
	if (creator->other_arch == NULL) {
	    LOG(llevError,"move_creator: Creator doesn't have other arch set: %s (%s, %d, %d)\n", creator->name ? creator->name : "(null)", creator->map->path, creator->x, creator->y);
	    return;
	}

	new_ob = object_create_arch(creator->other_arch);
	fix_generated_item(new_ob, creator, 0, 0, GT_MINIMAL);
    }

    /* Make sure this multipart object fits */
    if (new_ob->arch->more && ob_blocked(new_ob, creator->map, creator->x, creator->y)) {
	free_object(new_ob);
	return;
    }

    if (creator->level != 0)
        new_ob->level = creator->level;

    insert_ob_in_map_at(new_ob, creator->map, creator, 0, creator->x, creator->y);
    if (QUERY_FLAG(new_ob, FLAG_FREED))
	return;

    if (creator->slaying) {
	FREE_AND_COPY(new_ob->name, creator->slaying);
	FREE_AND_COPY(new_ob->title, creator->slaying);
    }
}

/**
 * move_marker --peterm@soda.csua.berkeley.edu
 * when moved, a marker will search for a player sitting above
 * it, and insert an invisible, weightless force into him
 * with a specific code as the slaying field.
 * At that time, it writes the contents of its own message
 * field to the player.  The marker will decrement hp to
 * 0 and then delete itself every time it grants a mark.
 * unless hp was zero to start with, in which case it is infinite.
 */
void move_marker(object *op) {
    object *tmp,*tmp2;
  
    for(tmp=get_map_ob(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above) {
	if(tmp->type == PLAYER) { /* we've got someone to MARK */

	    if ( quest_on_activate(op, tmp->contr) )
		return;

	    /* remove an old force with a slaying field == op->name */
	    for(tmp2=tmp->inv;tmp2 !=NULL; tmp2=tmp2->below) {
		if(tmp2->type == FORCE && tmp2->slaying && !strcmp(tmp2->slaying,op->name)) break;
	    }

	    if(tmp2) {
		remove_ob(tmp2);
		free_object(tmp2);
	    }

	    /* cycle through his inventory to look for the MARK we want to 
	     * place 
	     */
	    for(tmp2=tmp->inv;tmp2 !=NULL; tmp2=tmp2->below) {
		if(tmp2->type == FORCE && tmp2->slaying && !strcmp(tmp2->slaying,op->slaying)) break;
	    }
      
	    /* if we didn't find our own MARK */
	    if(tmp2==NULL) {
		object *force = create_archetype(FORCE_NAME);

		force->speed = 0;
		if(op->stats.food) {
		    force->speed = 0.01;
		    force->speed_left = -op->stats.food;
		}
		update_ob_speed (force);
		/* put in the lock code */
		force->slaying = add_string(op->slaying);

		if ( op->lore )
		    force->lore = add_string( op->lore );

		insert_ob_in_ob(force,tmp);
		if(op->msg)
		    draw_ext_info(NDI_UNIQUE|NDI_NAVY,0,tmp, 
				  MSG_TYPE_MISC, MSG_SUBTYPE_NONE,
				  op->msg, op->msg);

		if(op->stats.hp > 0) { 
		    op->stats.hp--;
		    if(op->stats.hp==0) {
			/* marker expires--granted mark number limit */
			remove_ob(op);
			free_object(op);
			return;
		    }
		}
	    } /* if tmp2 == NULL */
	} /* if tmp->type == PLAYER */
    } /* For all objects on this space */
}
 
int process_object(object *op) {
    if (QUERY_FLAG(op, FLAG_IS_A_TEMPLATE))
	return 0;

    if(QUERY_FLAG(op, FLAG_MONSTER))
	if(move_monster(op) || QUERY_FLAG(op, FLAG_FREED)) 
	    return 1;

    if(QUERY_FLAG(op, FLAG_ANIMATE) && op->anim_speed==0) {
	if (op->type == PLAYER)
	    animate_object(op, op->facing);
	else
	    animate_object(op, op->direction);

	if (QUERY_FLAG(op, FLAG_SEE_ANYWHERE))
	    make_sure_seen(op);
    }
    if(QUERY_FLAG(op, FLAG_CHANGING)&&!op->state) {
	change_object(op);
	return 1;
    }
    if(QUERY_FLAG(op, FLAG_GENERATOR)&&!QUERY_FLAG(op, FLAG_FRIENDLY))
	generate_monster(op);

    if(QUERY_FLAG(op, FLAG_IS_USED_UP)&&--op->stats.food<=0) {
	if(QUERY_FLAG(op, FLAG_APPLIED))
	    remove_force(op);
	else {
	    /* IF necessary, delete the item from the players inventory */
	    object *pl=get_player_container(op);
	    if (pl)
		esrv_del_item(pl->contr, op->count);
	    remove_ob(op);
	    if (QUERY_FLAG(op, FLAG_SEE_ANYWHERE))
		make_sure_not_seen(op);
	    free_object(op);
	}
	return 1;
    }
    /* Lauwenmark: Handle for plugin time event */
    execute_event(op, EVENT_TIME,NULL,NULL,NULL,SCRIPT_FIX_NOTHING);
    return ob_process(op);
}
void legacy_move_detector(object *op)
{
    move_detector(op);
}
void legacy_remove_force(object *op)
{
    remove_force(op);
}
void legacy_move_timed_gate(object *op)
{
    move_timed_gate(op);
}
void legacy_animate_trigger(object *op)
{
    animate_trigger(op);
}
void legacy_remove_blindness(object *op)
{
    remove_blindness(op);
}
void legacy_poison_more(object* op)
{
    poison_more(op);
}
void legacy_move_gate(object *op)
{
    move_gate(op);
}
void legacy_move_hole(object *op)
{
    move_hole(op);
}
