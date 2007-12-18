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

/**
 * @file
 * Routines that is executed from objects based on their speed have been
 * collected in this file.
 */

#include <global.h>
#include <spells.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/**
 * Remove non locked doors. The functions check to see if similar
 * doors are next to the one that is being removed, and if so, set it
 * so those will be removed shortly (in a cascade like fashion.)
 *
 * @sa remove_locked_door().
 *
 * @param op
 * door to remove.
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
 * Same as remove_door() but for locked doors.
 *
 * @param op
 * door to remove.
 */
void remove_locked_door(object *op) {
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

/**
 * Will generate a monster according to parameters of generator.
 *
 * What is generated should be in the generator's inventory.
 *
 * See generate_monster() for the main generator function.
 *
 * @param gen
 * generator.
 */
static void generate_monster_inv(object *gen) {
    int i;
    int nx, ny;
    object *op,*head=NULL;
    const char *code;

    int qty=0;
    /* Code below assumes the generator is on a map, as it tries
     * to place the monster on the map.  So if the generator
     * isn't on a map, complain and exit.
     */
    if (gen->map == NULL) {
	LOG(llevError,"Generator (%s) not on a map?\n", gen->name);
	return;
    }
    /*First count number of objects in inv*/
    for (op=gen->inv;op;op=op->below)
        qty++;
    if (!qty){
        LOG(llevError,"Generator (%s) has no inventory in generate_monster_inv?\n", gen->name);
        return;/*No inventory*/
        }
    qty=rndm(0,qty-1);
    for (op=gen->inv;qty;qty--)
        op=op->below;
    i=find_multi_free_spot_within_radius(op, gen, &nx, &ny);
    if (i==-1)
        return;
    head=object_create_clone(op);
    CLEAR_FLAG(head, FLAG_IS_A_TEMPLATE);
    unflag_inv (head,FLAG_IS_A_TEMPLATE);
    if (rndm(0, 9))
        generate_artifact(head, gen->map->difficulty);
    code = get_ob_key_value(gen, "generator_code");
    if(code){
        set_ob_key_value(head, "generator_code", code, 1);
    }
    insert_ob_in_map_at(head,gen->map,gen,0,nx,ny);
    if (QUERY_FLAG(head, FLAG_FREED)) return;
    fix_multipart_object(head);
    if(HAS_RANDOM_ITEMS(head))
        create_treasure(head->randomitems,head,GT_APPLY,
                      gen->map->difficulty,0);
}

/**
 * Generate a monster from the other_arch field.
 *
 * See generate_monster() for the main generator function.
 *
 * @param gen
 * generator.
 */
static void generate_monster_arch(object *gen) {
    int i;
    int nx, ny;
    object *op,*head=NULL,*prev=NULL;
    archetype *at=gen->other_arch;
    const char *code;

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
    i=find_multi_free_spot_within_radius(&at->clone, gen, &nx, &ny);
    if (i==-1) return;
    while(at!=NULL) {
	op=arch_to_object(at);
	op->x=nx+at->clone.x;
	op->y=ny+at->clone.y;

	if(head!=NULL)
	    op->head=head,prev->more=op;

	if (rndm(0, 9)) generate_artifact(op, gen->map->difficulty);
    code = get_ob_key_value(gen, "generator_code");
    if(code){
        set_ob_key_value(head, "generator_code", code, 1);
    }
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

/**
 * Main generator function. Will generate a monster based on the parameters.
 *
 * @param gen
 * generator.
 */
static void generate_monster(object *gen) {
    sint8 children;
    sint8 max_children;
    sint8 x, y;
    object *tmp;
    const char *code;
    const char *value;

    if(GENERATE_SPEED(gen)&&rndm(0, GENERATE_SPEED(gen)-1))
        return;
        /* See if generator has a generator_max limit set */
    value = get_ob_key_value(gen, "generator_max");
    if(value){
        max_children = (sint8)strtol_local((char *)value, NULL, 10);
        if( max_children < 1 )
            return;
        code = get_ob_key_value(gen, "generator_code");
        if( code ){
                /* Generator has a limit and has created some,
                 * so count how many already exist
                 */
            children = 0;
            for(x = 0; x < MAP_WIDTH(gen->map); x++){
                for(y = 0; y < MAP_HEIGHT(gen->map); y++){
                    for(tmp = get_map_ob(gen->map,x,y);
                        tmp!=NULL; tmp=tmp->above){
                        value = get_ob_key_value(tmp, "generator_code");
                        if(value && value == code){
                            children++;
                        }
                    }
                }
            }
                /* and return without generating if there are already enough */
            if( children >= max_children+1 )
                return;
        } else {
                /* Generator has a limit, but hasn't created anything yet,
                 * so no need to count, just set code and go
                 */
            value = get_ob_key_value(gen, "generator_name");
            if( value ){
                set_ob_key_value( gen, "generator_code", value, 1 );
            } else if(gen->name) {
                set_ob_key_value( gen, "generator_code", gen->name, 1 );
            } else {
                set_ob_key_value( gen, "generator_code", "generator", 1 );
            }
        }
    }
    if (QUERY_FLAG(gen,FLAG_CONTENT_ON_GEN))
        generate_monster_inv(gen);
    else
        generate_monster_arch(gen);
}

/**
 * Move for ::FORCE objects.
 *
 * @param op
 * force to test.
 * @todo rename to move_force?
 */
static void remove_force(object *op) {
    if (--op->duration > 0) {
        check_spell_expiry(op);
        return;
    }

    switch (op->subtype) {
	case FORCE_CONFUSION:
	    if(op->env!=NULL) {
		CLEAR_FLAG(op->env, FLAG_CONFUSED);
		draw_ext_info(NDI_UNIQUE, 0,op->env,
			      MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
			      "You regain your senses.", NULL);
	    }
        break;

    case FORCE_TRANSFORMED_ITEM:
            /* The force is into the item that was created */
            if (op->env != NULL && op->inv != NULL) {
                object* inv = op->inv;
                object* pl = get_player_container(op);
                remove_ob(inv);
                inv->weight = (inv->nrof ? (sint32)(op->env->weight / inv->nrof) : op->env->weight);
                if (op->env->env) {
                    insert_ob_in_ob(inv, op->env->env);
                    if (pl) {
                        char name[HUGE_BUF];
                        query_short_name(inv, name, HUGE_BUF);
                        draw_ext_info_format(NDI_UNIQUE, 0,pl,
                            MSG_TYPE_ITEM, MSG_TYPE_ITEM_CHANGE,
                            "Your %s recovers its original form.",
                            "Your %s recovers its original form.",
                            name);
                    }
                }
                else {
                    /* Object on map */
                    inv->x = op->env->x;
                    inv->y = op->env->y;
                    insert_ob_in_map(inv, op->env->map, NULL, 0);
                }
                inv = op->env;
                remove_ob(op);
                free_object(op);
                remove_ob(inv);
            }
            return;

	default:
        break;
    }

    if(op->env!=NULL) {
        CLEAR_FLAG(op, FLAG_APPLIED);
        change_abil(op->env,op);
        fix_object(op->env);
    }
    remove_ob(op);
    free_object(op);
}

/**
 * Move for blindness object.
 *
 * @param op
 * blindness to check.
 * @todo rename to something more meaningful.
 */
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

/**
 * Move a ::DETECTOR.
 *
 * - slaying:    name of the thing the detector is to look for
 * - speed:      frequency of 'glances'
 * - connected:  connected value of detector
 * - sp:         1 if detection sets buttons
 *              -1 if detection unsets buttons
 *
 * @param op
 * detector to move.
 */
static void move_detector(object *op)
{
    object *tmp, *tmp2;
    int last = op->value;
    int detected;
    detected = 0;

    if (!op->slaying) {
        if (op->map)
            LOG(llevError, "Detector with no slaying set at %s (%d,%d)\n", op->map->path, op->x, op->y);
        else if (op->env)
            LOG(llevError, "Detector with no slaying in %s\n", op->env->name);
        else
            LOG(llevError, "Detector with no slaying nowhere?\n");
        op->speed = 0;
        update_ob_speed(op);
        return;
    }

    for(tmp = get_map_ob(op->map, op->x, op->y); tmp!=NULL ;tmp = tmp->above) {
        if (op->stats.hp) {
            for(tmp2= tmp->inv;tmp2;tmp2=tmp2->below) {
                if (op->slaying == tmp2->name) {
                    detected=1;
                    break;
                }
                if (tmp2->type==FORCE && tmp2->slaying == op->slaying) {
                    detected=1;
                    break;
                }
            }
        }
        if (op->slaying == tmp->name) {
            detected = 1;
            break;
        }
        if (tmp->type == PLAYER && !strcmp(op->slaying, "player")) {
            detected = 1;
            break;
        }
        if (tmp->type==SPECIAL_KEY && tmp->slaying==op->slaying) {
            detected=1;
            break;
        }
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

/**
 * Animate a ::TRIGGER.
 *
 * @param op
 * trigger.
 */
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

/**
 * Move a ::HOLE.
 *
 * @param op
 * hole to move.
 */
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
        ob_move_on(op,tmp,tmp);
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


/**
 * An item (::ARROW or such) stops moving.
 *
 * stop_item() returns a pointer to the stopped object.  The stopped object
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
 *
 * @param op
 * object to check.
 * @return
 * pointer to stopped object, NULL if destroyed or can't be stopped.
 */
object *stop_item (object *op)
{
    if (free_no_drop(op))
        return NULL;

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

/**
 * Put stopped item where stop_item() had found it.
 * Inserts item into the old map, or merges it if it already is on the map.
 *
 * @param op
 * object to stop.
 * @param map
 * must be the value of op->map before stop_item() was called.
 * @param originator
 * what caused op to be stopped.
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

/**
 * An ::ARROW stops moving.
 *
 * @param op
 * arrow stopping.
 * @return
 * arrow, or NULL if it was broken.
 */
object *fix_stopped_arrow (object *op)
{
    if (free_no_drop(op))
        return NULL;

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

/**
 * Check whether the given object is FLAG_NO_DROP. If so, (optionally) remove
 * and free it.
 *
 * @param op
 * the object to check
 * @return
 * whether the object was freed
 */
int free_no_drop(object *op) {
    if (!QUERY_FLAG(op, FLAG_NO_DROP)) {
        return 0;
    }

    if(!QUERY_FLAG(op, FLAG_REMOVED)) {
        remove_ob(op);
    }

    free_object2(op, 1);
    return 1;
}

/**
 * Replaces op with its other_arch if it has reached its end of life.
 *
 * This routine doesnt seem to work for "inanimate" objects that
 * are being carried, ie a held torch leaps from your hands!.
 * Modified this routine to allow held objects. b.t.
 *
 * @param op
 * object to change. Will be removed and replaced.
 */
static void change_object(object *op) { /* Doesn`t handle linked objs yet */
  object *tmp,*env;
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

/**
 * Move function for ::TELEPORTER objects.
 *
 * @param op
 * teleporter.
 */
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


/**
 * Move for ::PLAYER_CHANGER.
 *
 * This object will teleport someone to a different map
 * and will also apply changes to the player from its inventory.
 *
 * This was invented for giving classes, but there's no reason it
 *  can't be generalized.
 *
 * @param op
 * changer to move.
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
 * Move for ::FIREWALL.
 *
 * firewalls fire other spells.
 * The direction of the wall is stored in op->stats.sp.
 * walls can have hp, so they can be torn down.
 *
 * @param op
 * firewall.
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
 * This function takes a ::PLAYERMOVER as an
 * argument, and performs the function of a player mover, which is:
 *
 * a player mover finds any players that are sitting on it.  It
 * moves them in the op->stats.sp direction.  speed is how often it'll move.
 * - If attacktype is nonzero it will paralyze the player.  If lifesave is set,
 * - it'll dissapear after hp+1 moves.  If hp is set and attacktype is set,
 * - it'll paralyze the victim for hp*his speed/op->speed
 *
 * @param op
 * mover.
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
 * Move for ::DUPLICATOR.
 *
 * Will duplicate a specified object placed on top of it.
 * - connected: what will trigger it.
 * - level: multiplier.  0 to destroy.
 * - other_arch: the object to look for and duplicate.
 *
 * @param op
 * duplicator.
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
 * Move for ::CREATOR.
 *
 * it has the creator object create it's other_arch right on top of it.
 * connected:  what will trigger it
 * - hp:  how many times it may create before stopping
 * - lifesave:  if set, it'll never disappear but will go on creating
 *    everytime it's triggered
 * - other_arch:  the object to create
 *
 * Note this can create large objects, however, in that case, it
 * has to make sure that there is in fact space for the object.
 * It should really do this for small objects also, but there is
 * more concern with large objects, most notably a part being placed
 * outside of the map which would cause the server to crash
 *
 * @param creator
 * creator to move.
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
 * Move for ::MARKER.
 *
 * When moved, a marker will search for a player sitting above
 * it, and insert an invisible, weightless force into him
 * with a specific code as the slaying field.
 *
 * At that time, it writes the contents of its own message
 * field to the player.  The marker will decrement hp to
 * 0 and then delete itself every time it grants a mark.
 * unless hp was zero to start with, in which case it is infinite.
 * @author peterm@soda.csua.berkeley.edu
 *
 * @param op
 * marker to move.
 */
void move_marker(object *op) {
    object *tmp,*tmp2;

    /* 
     * markers not on a map for any reason should not crash server
     */
    if (!op->map){
        return;
    }
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

/**
 * Main object move function.
 *
 * @param op
 * object to move.
 * @return
 * 0 if object didn't move, 1 else?
 * @todo remove unused return value?
 */
int process_object(object *op) {
    if (QUERY_FLAG(op, FLAG_IS_A_TEMPLATE))
	return 0;

    /* Lauwenmark: Handle for plugin time event */
    if (execute_event(op, EVENT_TIME,NULL,NULL,NULL,SCRIPT_FIX_NOTHING) != 0)
        return 0;

    if(QUERY_FLAG(op, FLAG_MONSTER))
	if(move_monster(op) || QUERY_FLAG(op, FLAG_FREED))
	    return 1;

    if((QUERY_FLAG(op, FLAG_ANIMATE) && op->anim_speed==0)||(op->temp_animation_id && op->temp_anim_speed==0)) {
        op->state++;
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
	    remove_ob(op);
	    if (QUERY_FLAG(op, FLAG_SEE_ANYWHERE))
		make_sure_not_seen(op);
	    free_object(op);
	}
	return 1;
    }
    return (ob_process(op) == METHOD_OK ? 1 : 0);
}
void legacy_move_detector(object *op)
{
    move_detector(op);
}
void legacy_remove_force(object *op)
{
    remove_force(op);
}
void legacy_animate_trigger(object *op)
{
    animate_trigger(op);
}
void legacy_remove_blindness(object *op)
{
    remove_blindness(op);
}
void legacy_move_hole(object *op)
{
    move_hole(op);
}
