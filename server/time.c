/*
 * static char *rcsid_time_c =
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

/*
 * Routines that is executed from objects based on their speed have been
 * collected in this file.
 */

#include <global.h>
#include <spells.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/* The following removes doors.  The functions check to see if similar 
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
  remove_ob(op);
  free_object(op);
}

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
  remove_ob(op);
  free_object(op);
}

void generate_monster(object *gen) {
  int i;
  object *op,*head=NULL,*prev=NULL;
  archetype *at=gen->other_arch;

  if(GENERATE_SPEED(gen)&&RANDOM()%GENERATE_SPEED(gen))
    return;
  if(gen->other_arch==NULL) {
    LOG(llevError,"Generator without other_arch: %s\n",gen->name);
    return;
  }
  i=find_free_spot(at,gen->map,gen->x,gen->y,1,9);
  if (i==-1) return;
  while(at!=NULL) {
    op=arch_to_object(at);
    op->x=gen->x+freearr_x[i]+at->clone.x;
    op->y=gen->y+freearr_y[i]+at->clone.y;
    if(head!=NULL)
      op->head=head,prev->more=op;
    if (RANDOM()%10) generate_artifact(op, gen->map->difficulty);
    insert_ob_in_map(op,gen->map,gen);
    if (QUERY_FLAG(op, FLAG_FREED)) return;
    if(op->randomitems!=NULL)
      create_treasure(op->randomitems,op,GT_INVENTORY,
                      gen->map->difficulty,0);
    if(head==NULL)
      head=op;
    prev=op;
    at=at->more;
  }
}

void regenerate_rod(object *rod) {
  if(++rod->stats.food > rod->stats.hp/10 || rod->type == HORN) {
    rod->stats.food = 0;
    if (rod->stats.hp < rod->stats.maxhp) {
      rod->stats.hp += 1 + rod->stats.maxhp/10;
      if (rod->stats.hp > rod->stats.maxhp)
        rod->stats.hp = rod->stats.maxhp;
      fix_rod_speed(rod);
    }
  }
}

void remove_force(object *op) {
  if(op->env==NULL) {
    remove_ob(op);
    free_object(op);
    return;
  }
  CLEAR_FLAG(op, FLAG_APPLIED);
  change_abil(op->env,op);
  fix_player(op->env);
  remove_ob(op);
  free_object(op);
}

void remove_blindness(object *op) {
  if(--op->stats.food > 0)
    return;
  CLEAR_FLAG(op, FLAG_APPLIED);
  if(op->env!=NULL) { 
     change_abil(op->env,op);
     fix_player(op->env);
  }
  remove_ob(op);
  free_object(op);
}

void remove_confusion(object *op) {
  if(--op->stats.food > 0)
    return;
  if(op->env!=NULL) {
    CLEAR_FLAG(op->env, FLAG_CONFUSED);
    new_draw_info(NDI_UNIQUE, 0,op->env, "You regain your senses.\n");
  }
  remove_ob(op);
  free_object(op);
}

void execute_wor(object *op) {
  object *wor=op;
  while(op!=NULL&&op->type!=PLAYER)
    op=op->env;
  if(op!=NULL) {
    if(blocks_magic(op->map,op->x,op->y) && wor->stats.hp != 1)
      new_draw_info(NDI_UNIQUE, 0,op,"You feel something fizzle inside you.");
    else
      enter_exit(op,wor);
  }
  remove_ob(wor);
  free_object(wor);
}

void poison_more(object *op) {
  if(op->env==NULL||!QUERY_FLAG(op->env,FLAG_ALIVE)||op->env->stats.hp<0) {
    remove_ob(op);
    free_object(op);
    return;
  }
  if(op->stats.food==1) {
    /* need to remove the object before fix_player is called, else fix_player
     * will not do anything.
     */
    if(op->env->type==PLAYER) {
      CLEAR_FLAG(op, FLAG_APPLIED);
      fix_player(op->env);
      new_draw_info(NDI_UNIQUE, 0,op->env,"You feel much better now.");
    }
    remove_ob(op);
    free_object(op);
    return;
  }
  if(op->env->type==PLAYER) {
    op->env->stats.food--;
    new_draw_info(NDI_UNIQUE, 0,op->env,"You feel very sick...");
  }
  (void)hit_player(op->env,
                   op->stats.dam,
                   op,AT_INTERNAL);
}


void move_gate(object *op) { /* 1 = going down, 0 = goind up */
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
	    CLEAR_FLAG(op, FLAG_NO_PASS);
	    CLEAR_FLAG(op, FLAG_BLOCKSVIEW);
	    update_all_los(op->map);
	}
	SET_ANIMATION(op, op->stats.wc);
	update_object(op);
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
		    hit_player(tmp,RANDOM()%(op->stats.dam+1)+1,op,AT_PHYSICAL);
		    if(tmp->type==PLAYER) 
			new_draw_info_format(NDI_UNIQUE, 0, tmp,
					     "You are crushed by the %s!",op->name);
		} else 
		    /* If the object is not alive, and the object either can
		     * be picked up or the object rolls, move the object
		     * off the gate.
		     */
		    if(!QUERY_FLAG(tmp, FLAG_ALIVE)
			&& (!QUERY_FLAG(tmp, FLAG_NO_PICK)
			   ||QUERY_FLAG(tmp,FLAG_CAN_ROLL))) {
		    /* If it has speed, it should move itself, otherwise: */
		    int i=find_free_spot(tmp->arch,op->map,op->x,op->y,1,9);

		    /* If there is a free spot, move the object someplace */
		    if (i!=-1) {
			remove_ob(tmp);
			tmp->x+=freearr_x[i],tmp->y+=freearr_y[i];
			insert_ob_in_map(tmp,op->map,op);
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
		SET_FLAG(op, FLAG_NO_PASS);    /* The coast is clear, block the way */
		if(!op->arch->clone.stats.ac)
		    SET_FLAG(op, FLAG_BLOCKSVIEW);
		update_all_los(op->map);
	    }
	} /* gate is halfway up */

	SET_ANIMATION(op, op->stats.wc);
	update_object(op);
    } /* gate is going up */
}

/*  hp      : how long door is open/closed
 *  maxhp   : initial value for hp
 *  sp      : 1 = open, 0 = close
 */
void move_timed_gate(object *op)
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

/*  slaying:    name of the thing the detector is to look for
 *	 speed:      frequency of 'glances'
 *	 connected:  connected value of detector
 *  sp:         1 if detection sets buttons
 *              -1 if detection unsets buttons
 */

void move_detector(object *op) 
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


void animate_trigger (object *op)
{
  if((unsigned char)++op->stats.wc >= NUM_ANIMATIONS(op)) {
    op->stats.wc = 0;
    check_trigger(op,NULL);
  } else {
    SET_ANIMATION(op, op->stats.wc);
    update_object(op);
  }
}

void move_hole(object *op) { /* 1 = opening, 0 = closing */
    object *next,*tmp;

    if(op->value) { /* We're opening */
	if(--op->stats.wc<=0) { /* Opened, let's stop */
	    op->stats.wc=0;
	    op->speed = 0;
	    update_ob_speed(op);
	    SET_FLAG(op, FLAG_WALK_ON);
	    for (tmp=op->above; tmp!=NULL; tmp=next) {
		next=tmp->above;
		move_apply(op,tmp,tmp);
	    }
	}
	SET_ANIMATION(op, op->stats.wc);
	update_object(op);
	return;
    }
    /* We're closing */
    CLEAR_FLAG(op, FLAG_WALK_ON);
    op->stats.wc++;
    if((int)op->stats.wc >= NUM_ANIMATIONS(op))
	op->stats.wc=NUM_ANIMATIONS(op)-1;
    SET_ANIMATION(op, op->stats.wc);
    update_object(op);
    if((unsigned char) op->stats.wc==(NUM_ANIMATIONS(op)-1)) {
	op->speed = 0;
	update_ob_speed(op); /* closed, let's stop */
	return;
    }
}


/* stop_arrow() - what to do when a non-living flying object
 * has to stop. Sept 96 - I added in thrown object code in 
 * here too. -b.t. 
 * op is arrow, tmp is what is stopping the arrow (can be NULL)
 */

void stop_arrow(object *op,object *tmp) {

    if(wall(op->map,op->x,op->y))
	op->x-=DIRX(op),op->y-=DIRY(op);
    if(wall(op->map, op->x, op->y)) {
	free_object(op);
	return;
    }

    if(RANDOM() % 100 < op->stats.food) {
	/* Small chance of breaking */
	free_object(op);
	return;
    }

    op->direction=0;
    CLEAR_FLAG(op, FLAG_NO_PICK);
    CLEAR_FLAG(op, FLAG_WALK_ON);
    CLEAR_FLAG(op, FLAG_FLY_ON);
    CLEAR_FLAG(op, FLAG_FLYING);
    op->speed = 0;
    update_ob_speed(op);
    op->stats.wc = op->stats.sp;
    op->stats.dam= op->stats.hp;
    /* Reset these to zero, so that CAN_MERGE will work properly */
    op->stats.sp = 0;
    op->stats.hp = 0;
    op->face=op->arch->clone.face;

    /* this happens for thrown objects, which 'carry' the
     * real object in inventory. */ 
    if(op->inv) {
	object *old=op,*new=op->inv;
	remove_ob(op->inv);
	new->map = old->map; 
	free_object(old);
	op = new;
    }

    /* If the missile hit a player, we insert it in their inventory.
     * However, if the missile is heavy, we don't do so (assume it falls
     * to the ground after a hit).  What a good value for this is up to
     * debate - 5000 is 5 kg, so arrows, knives, and other light weapons
     * stick around.
     */
    if(op->weight <= 5000 && tmp!=NULL&&tmp->stats.hp>=0) {
	if(tmp->head != NULL)
	    tmp = tmp->head;
	op = insert_ob_in_ob(op,tmp);
	if (tmp->type== PLAYER)
	    esrv_send_item (tmp, op);
    } else
	insert_ob_in_map(op,op->map,op);
    op->owner=NULL; /* So that stopped arrows will be saved */
}


/* Move an arrow along its course.  op is the arrow or thrown object.
 */

void move_arrow(object *op) {
    object *tmp;

    if(op->map==NULL) {
	LOG(llevDebug,"Arrow had no map.\n");
	remove_ob(op);
	free_object(op);
	return;
    }

    remove_ob(op);

    /* we need to stop thrown objects at some point. Like here. */ 
    if(op->type==THROWN_OBJ) {
	if(op->last_sp-- < 0) { 
	    stop_arrow(op, NULL); 
	    return; 
	}
    }


    if(wall(op->map,op->x+DIRX(op),op->y+DIRY(op))) {
	/* if the object doesn't reflect, stop the arrow from moving */
	if(!QUERY_FLAG(op, FLAG_REFLECTING) || !(RANDOM()%20)) {
	    stop_arrow(op,NULL);
	    return;
	} else {    /* object is reflected */
	    /* If one of the major directions (n,s,e,w), just reverse it */
	    if(op->direction&1) {
		op->direction=absdir(op->direction+4);
	    } else {
		/* The below is just logic for figuring out what direction
		 * the object should now take.
		 */
	
		int left= wall(op->map,op->x+freearr_x[absdir(op->direction-1)],
		       op->y+freearr_y[absdir(op->direction-1)]),
		right=wall(op->map,op->x+freearr_x[absdir(op->direction+1)],
		   op->y+freearr_y[absdir(op->direction+1)]);

		if(left==right)
		    op->direction=absdir(op->direction+4);
		else if(left)
		    op->direction=absdir(op->direction+2);
		else if(right)
		    op->direction=absdir(op->direction-2);
	    }
	    /* Is the new direction also a wall?  If show, shuffle again */
	    if(wall(op->map,op->x+DIRX(op),op->y+DIRY(op))) {
		int left= wall(op->map,op->x+freearr_x[absdir(op->direction-1)],
			 op->y+freearr_y[absdir(op->direction-1)]),
		right=wall(op->map,op->x+freearr_x[absdir(op->direction+1)],
		     op->y+freearr_y[absdir(op->direction+1)]);

		if(!left)
		    op->direction=absdir(op->direction-1);
		else if(!right)
		    op->direction=absdir(op->direction+1);
		else {		/* is this possible? */
		    stop_arrow(op,NULL);
		    return;
		}
	    }
	    /* update object image for new facing */
	    /* many thrown objects *don't* have more than one face */
	    if(GET_ANIM_ID(op))
		SET_ANIMATION(op, op->direction);
	} /* object is reflected */
    } /* object ran into a wall */

    op->x+=DIRX(op),op->y+=DIRY(op);
    tmp=get_map_ob(op->map,op->x,op->y);

    /* See if there is any living object on this space */
    while(tmp!=NULL&&!QUERY_FLAG(tmp, FLAG_ALIVE))
	tmp=tmp->above;

    /* Nothing alive?  Insert arrow and return */
    if(tmp==NULL) {
	insert_ob_in_map(op,op->map,op);
	return;
    }

    /* Found living object, but it is reflecting the missile.  Update
     * as below.
     */
    if (QUERY_FLAG(tmp, FLAG_REFL_MISSILE)) {
	int number = op->face->number;

	op->direction=absdir(op->direction+4),op->state=0;
	if(GET_ANIM_ID(op)) {
	    number+=4;
	    if(number > GET_ANIMATION(op, 8))
		number-=8;
	    op->face = &new_faces[number];
	}
	insert_ob_in_map(op,op->map,op);
	return;
    }

    /* Attach the object.  IF successful, stop the arrow */
    if(attack_ob(tmp,op))
	stop_arrow(op,tmp);
    else {
	/* if we miss, insert it back into the map.  If no direction,
	 * stop the arrow.
	 */
	if(op->direction)
	    insert_ob_in_map(op,op->map,op);
	else 
	    stop_arrow(op,NULL);
    }
}

/* This routine doesnt seem to work for "inanimate" objects that
 * are being carried, ie a held torch leaps from your hands!. 
 * Modified this routine to allow held objects. b.t. */

void change_object(object *op) { /* Doesn`t handle linked objs yet */
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
    tmp->stats.hp=op->stats.hp; /* The only variable it keeps. */
    if(env) {
        tmp->x=env->x,tmp->y=env->y;
	tmp=insert_ob_in_ob(tmp,env);
	/* If this object is the players inventory, we need to tell the
	 * client of the change.  Insert_ob_in_map takes care of the
	 * updating the client, so we don't need to do that below.
	 */
	if ((pl=is_player_inv(env))!=NULL) {
	    esrv_del_item(pl->contr, op->count);
	    esrv_send_item(pl, tmp);
	}
    } else {
        j=find_first_free_spot(tmp->arch,op->map,op->x,op->y);
	if (j==-1)  /* No free spot */
	    free_object(tmp);
	else {
	    tmp->x=op->x+freearr_x[j],tmp->y=op->y+freearr_y[j];
	    insert_ob_in_map(tmp,op->map,op);
	}
    }
  }
  free_object(op);
}

void move_teleporter(object *op) {
  
  if(op->above!=NULL) {
    if(EXIT_PATH(op)) {
      if(op->above->type==PLAYER) 
        enter_exit(op->above,op);
      else
        return;
    }
    else if(EXIT_X(op)||EXIT_Y(op))
    {
      if (out_of_map(op->map, EXIT_X(op), EXIT_Y(op)))
      {
        LOG(llevError, "Removed illegal teleporter.\n");
        remove_ob(op);
        free_object(op);
        return;
      }
      transfer_ob(op->above,EXIT_X(op),EXIT_Y(op),0,op);
    } else
      teleport(op,TELEPORTER,op);
  }
}

/*  This object will teleport someone to a different map
    and will also apply changes to the player from its inventory.
    This was invented for giving classes, but there's no reason it
    can't be generalized.
*/

void move_player_changer(object *op) {
  object *player;
  object *walk;
   if(op->above!=NULL) {
    if(EXIT_PATH(op)) {
      if(op->above->type==PLAYER) {
	player=op->above;
	for(walk=op->inv;walk!=NULL;walk=walk->below) 
	  apply_changes_to_player(player,walk);
	link_player_skills(op->above);
	esrv_send_inventory(op->above,op->above);
	esrv_update_item(UPD_FACE, op->above, op->above);
	enter_exit(op->above,op);
      }
      else
        return;
    }
   }
}

/*  peterm:  firewalls generalized to be able to shoot any type
    of spell at all.  the stats.dam field of a firewall object
    contains it's spelltype.      The direction of the wall is stored
    in op->stats.sp.  maxsp also has some meaning, i'm not sure what.
    walls can have hp, so they can be torn down. */
void move_firewall(object *op) {
  if ( ! op->map)
    return;   /* dm has created a firewall in his inventory */
  cast_spell(op,op,op->stats.sp?op->stats.sp:(RANDOM()%8)+1,op->stats.dam,
	1,spellNormal,NULL);
}

void move_firechest(object *op) {
  if ( ! op->map)
    return;   /* dm has created a firechest in his inventory */
  fire_a_ball(op,(RANDOM()%8)+1,7);
}


/*  move_player_mover:  this function takes a "player mover" as an
 * argument, and performs the function of a player mover, which is:

 * a player mover finds any players that are sitting on it.  It
 * moves them in the op->stats.sp direction.  speed is how often it'll move.
 * If attacktype is nonzero it will paralyze the player.  If lifesave is set,
 * it'll dissapear after hp+1 moves.  If hp is set and attacktype is set,
 * it'll paralyze the victim for hp*his speed/op->speed

*/
void move_player_mover(object *op) {
  object *victim, *nextmover;

  for(victim=get_map_ob(op->map,op->x,op->y); victim !=NULL; victim=victim->above)
    if(QUERY_FLAG(victim, FLAG_ALIVE)&& (!(QUERY_FLAG(victim,FLAG_FLYING))||op->stats.maxhp))
      {

      if(QUERY_FLAG(op,FLAG_LIFESAVE)&&op->stats.hp--<0) {
	remove_ob(op);
	free_object(op);
	return;
      }

      for(nextmover=get_map_ob(op->map,op->x+freearr_x[op->stats.sp],op->y+freearr_y[op->stats.sp]); nextmover !=NULL; nextmover=nextmover->above) {
	if(nextmover->type == PLAYERMOVER) 
	   { nextmover->speed_left=-.99;}
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
		move_player(victim, op->stats.sp?op->stats.sp:(RANDOM()%8)+1);
	    }
	    else return;
      }
      else move_object(victim,op->stats.sp?op->stats.sp:(RANDOM()%8)+1);

      if(!op->stats.maxsp&&op->attacktype) op->stats.maxsp=2.0;

      if(op->attacktype)  /* flag to paralyze the player */
        victim->speed_left= -FABS(op->stats.maxsp*victim->speed/op->speed);
      }

}

/*  move_creator (by peterm) 
  it has the creator object create it's other_arch right on top of it.
  connected:  what will trigger it
  hp:  how many times it may create before stopping
  lifesave:  if set, it'll never disappear but will go on creating
	everytime it's triggered
  other_arch:  the object to create
*/

void move_creator(object *op) {
  object *tmp;
  op->stats.hp--;
  if(op->stats.hp < 0 && !QUERY_FLAG(op,FLAG_LIFESAVE)) 
	{ op->stats.hp=-1;return;}
  tmp=arch_to_object(op->other_arch);
  if(op->slaying) {
	 if (tmp->name) free_string (tmp->name);
	 if (tmp->title) free_string (tmp->title);
	 tmp->name = add_string(op->slaying);
	 tmp->title = add_string(op->slaying);
  }
  tmp->x=op->x;tmp->y=op->y;tmp->map=op->map;tmp->level=op->level;
  insert_ob_in_map(tmp,op->map,op);
}

/* move_marker --peterm@soda.csua.berkeley.edu
   when moved, a marker will search for a player sitting above
   it, and insert an invisible, weightless force into him
   with a specific code as the slaying field.
   At that time, it writes the contents of its own message
   field to the player.  The marker will decrement hp to
   0 and then delete itself every time it grants a mark.
   unless hp was zero to start with, in which case it is infinite.*/

void move_marker(object *op) {
  object *tmp,*tmp2;
  
  for(tmp=get_map_ob(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above) {
	 

    if(tmp->type == PLAYER) { /* we've got someone to MARK */

		/* remove an old force with a slaying field == op->name */
      for(tmp2=tmp->inv;tmp2 !=NULL; tmp2=tmp2->below) {
		  if(tmp2->type == FORCE && tmp2->slaying && !strcmp(tmp2->slaying,op->name)) break;
      }
		if(tmp2) {
		  remove_ob(tmp2);
		  free_object(tmp2);
		}

      /* cycle through his inventory to look for the MARK we want to place */
      for(tmp2=tmp->inv;tmp2 !=NULL; tmp2=tmp2->below) {
		  if(tmp2->type == FORCE && tmp2->slaying && !strcmp(tmp2->slaying,op->slaying)) break;
      }
      
      /* if we didn't find our own MARK */
      if(tmp2==NULL) {
	         
		  object *force = get_archetype("force");
		  force->speed = 0;
		  if(op->stats.food) {
			 force->speed = 0.01;
			 force->speed_left = -op->stats.food;
		  }
		  update_ob_speed (force);
		  /* put in the lock code */
		  force->slaying = add_string(op->slaying);
		  insert_ob_in_ob(force,tmp);
		  if(op->msg)
		    new_draw_info(NDI_UNIQUE|NDI_NAVY,0,tmp,op->msg);
		  if(op->stats.hp > 0) { 
		    op->stats.hp--;
		    if(op->stats.hp==0) {
		      /* marker expires--granted mark number limit */
		      remove_ob(op);
		      free_object(op);
		      return;
		    }
		  }
      }

    }

  }
}
 
int process_object(object *op) {

  if(QUERY_FLAG(op, FLAG_MONSTER))
    if(move_monster(op) || QUERY_FLAG(op, FLAG_FREED)) 
      return 1;
  if(QUERY_FLAG(op, FLAG_ANIMATE) && op->anim_speed==0) {
    animate_object(op);
    if (QUERY_FLAG(op, FLAG_SEE_ANYWHERE))
      make_sure_seen(op);
  }
  if(QUERY_FLAG(op, FLAG_CHANGING)&&!op->state) {
    change_object(op);
    return 1;
  }
  if(QUERY_FLAG(op, FLAG_GENERATOR))
    generate_monster(op);
  if(QUERY_FLAG(op, FLAG_IS_USED_UP)&&--op->stats.food<=0) {
    if(QUERY_FLAG(op, FLAG_APPLIED))
      remove_force(op);
    else {
	/* IF necessary, delete the item from the players inventory */
	object *pl=is_player_inv(op);
	if (pl)
	    esrv_del_item(pl->contr, op->count);
      remove_ob(op);
      if (QUERY_FLAG(op, FLAG_SEE_ANYWHERE))
        make_sure_not_seen(op);
      free_object(op);
    }
    return 1;
  }
  switch(op->type) {
  case ROD:
  case HORN:
    regenerate_rod(op);
    return 1;
  case FORCE:
    remove_force(op);
    return 1;
  case BLINDNESS:
    remove_blindness(op);
    return 0;
  case CONFUSION:
    remove_confusion(op);
    return 0;
  case POISONING:
    poison_more(op);
    return 0;
  case DISEASE:
	 move_disease(op);
	 return 0;
  case SYMPTOM:
	 move_symptom(op);
	 return 0;
  case WORD_OF_RECALL:
    execute_wor(op);
    return 0;
  case BULLET:
    move_fired_arch(op);
    return 0;
  case MMISSILE:
    move_missile(op);
    return 0;
  case THROWN_OBJ:
  case ARROW:
    move_arrow(op);
    return 0;
  case FBULLET:
    move_fired_arch(op);
    return 0;
  case FBALL:
  case POISONCLOUD:
    explosion(op);
    return 0;
  case LIGHTNING: /* It now moves twice as fast */
    move_bolt(op);
    return 0;
  case CONE:
    move_cone(op);
    return 0;
  case DOOR:
    remove_door(op);
    return 0;
  case LOCKED_DOOR:
    remove_door2(op);
    return 0;
  case TELEPORTER:
    move_teleporter(op);
    return 0;
  case BOMB:
    animate_bomb(op);
    return 0;
  case GOLEM:
    move_golem(op);
    return 0;
  case EARTHWALL:
    hit_player(op, 2, op, AT_PHYSICAL);
    return 0;
  case FIREWALL:
    move_firewall(op);
    if (op->stats.maxsp)
      animate_turning(op);
    return 0;
  case FIRECHEST:
    move_firechest(op);
    return 0;
  case MOOD_FLOOR:
    do_mood_floor(op, op);
    return 0;
  case GATE:
    move_gate(op);
    return 0;
  case TIMED_GATE:
    move_timed_gate(op);
    return 0;
  case TRIGGER:
  case TRIGGER_BUTTON:
  case TRIGGER_PEDESTAL:
  case TRIGGER_ALTAR:
    animate_trigger(op);
    return 0;
  case DETECTOR:
	 move_detector(op);
  case DIRECTOR:
    if (op->stats.maxsp)
      animate_turning(op);
    return 0;
  case HOLE:
    move_hole(op);
    return 0;
  case DEEP_SWAMP:
    move_deep_swamp(op);
    return 0;
  case CANCELLATION:
    move_cancellation(op);
    return 0;
  case BALL_LIGHTNING:
    move_ball_lightning(op);
    return 0;
  case SWARM_SPELL:
    move_swarm_spell(op);
    return 0;
  case RUNE:
    move_rune(op);
    return 0;
  case PLAYERMOVER:
    move_player_mover(op);
    return 0;
  case CREATOR:
    move_creator(op);
    return 0;
  case MARKER:
    move_marker(op);
    return 0;
  case PLAYER_CHANGER:
    move_player_changer(op);
    return 0;
  }

  return 0;
}

