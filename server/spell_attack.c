/*
 * static char *rcsid_spell_attack_c =
 *   "$Id$";
 */


/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2006 Mark Wedel & Crossfire Development Team
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

/* This file contains all the spell attack code.  Grouping this code
 * together should hopefully make it easier to find the relevent bits
 * of code
 */

#include <global.h>
#include <object.h>
#include <living.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>
#include <sounds.h>

/* this function checks to see if a spell pushes objects as well
 * as flies over and damages them (only used for cones for now)
 * but moved here so it could be applied to bolts too
 * op is the spell object.
 */

static void check_spell_knockback(object *op) {
    object *tmp, *tmp2; /* object on the map */
    int weight_move;
	int frictionmod=2; /*poor man's physics - multipy targets weight by this amount */

	if(! op->weight) { /*shouldn't happen but if cone object has no weight drop out*/
		/*LOG (llevDebug, "DEBUG: arch weighs nothing\n");*/
		return;
	}else{
	   weight_move = op->weight +(op->weight * op->level) / 3;
	   /*LOG (llevDebug, "DEBUG: arch weighs %d and masses %d (%s,level %d)\n", op->weight,weight_move,op->name,op->level);*/
	}
	
    for(tmp=get_map_ob(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above)
    { 
	int num_sections = 1;

	/* don't move DM */
	if(QUERY_FLAG(tmp, FLAG_WIZ))
		return;
		
	/* don't move parts of objects */
	if(tmp->head) continue;

	/* don't move floors or immobile objects */
	if(QUERY_FLAG(tmp,FLAG_IS_FLOOR)||(!QUERY_FLAG(tmp,FLAG_ALIVE)&&QUERY_FLAG(tmp,FLAG_NO_PICK))) continue;

	/* count the object's sections */
	for(tmp2 = tmp; tmp2!=NULL;tmp2=tmp2->more) num_sections++;

	/* I'm not sure if it makes sense to divide by num_sections - bigger
	 * objects should be harder to move, and we are moving the entire
	 * object, not just the head, so the total weight should be relevant.
	 */
	
	 /* surface area? -tm */
	
	if (tmp->move_type & MOVE_FLYING)
		frictionmod = 1 ; /* flying objects loose the friction modifier */
	
	if(rndm(0, weight_move-1) > ((tmp->weight / num_sections) * frictionmod)) {  /* move it. */
	    /* move_object is really for monsters, but looking at 
	     * the move_object function, it appears that it should
	     * also be safe for objects.
	     * This does return if successful or not, but
	     * I don't see us doing anything useful with that information
	     * right now.
	     */
	    move_object(tmp, absdir(op->stats.sp));
	}

    }
}

/***************************************************************************
 *
 * BOLT CODE
 *
 ***************************************************************************/

/* Causes op to fork.  op is the original bolt, tmp
 * is the first piece of the fork.
 */

static void forklightning(object *op, object *tmp) {
    int new_dir=1;  /* direction or -1 for left, +1 for right 0 if no new bolt */
    int t_dir; /* stores temporary dir calculation */
    mapstruct *m;
    sint16  sx,sy;
    object *new_bolt;

    /* pick a fork direction.  tmp->stats.Con is the left bias
     * i.e., the chance in 100 of forking LEFT
     * Should start out at 50, down to 25 for one already going left
     * down to 0 for one going 90 degrees left off original path
     */
		  
    if(rndm(0, 99) < tmp->stats.Con)  /* fork left */
	new_dir = -1;
		  
    /* check the new dir for a wall and in the map*/
    t_dir = absdir(tmp->direction + new_dir);

    if(get_map_flags(tmp->map,&m, tmp->x + freearr_x[t_dir],tmp->y + freearr_y[t_dir],
		     &sx, &sy) & P_OUT_OF_MAP)
	return;

    if (OB_TYPE_MOVE_BLOCK(tmp, GET_MAP_MOVE_BLOCK(m, sx, sy)))
	return;

    /* OK, we made a fork */
    new_bolt = get_object();

    copy_object(tmp,new_bolt);

    /* reduce chances of subsequent forking */
    new_bolt->stats.Dex -= 10;  
    tmp->stats.Dex -= 10;  /* less forks from main bolt too */
    new_bolt->stats.Con += 25 * new_dir; /* adjust the left bias */
    new_bolt->speed_left = -0.1;
    new_bolt->direction = t_dir;
    new_bolt->duration++;
    new_bolt->x=sx;
    new_bolt->y=sy;
    new_bolt->stats.dam /= 2;  /* reduce daughter bolt damage */
    new_bolt->stats.dam++;
    tmp->stats.dam /= 2;  /* reduce father bolt damage */
    tmp->stats.dam++;
    new_bolt = insert_ob_in_map(new_bolt,m,op,0);
    update_turn_face(new_bolt);
}

/* move_bolt: moves bolt 'op'.  Basically, it just advances a space,
 * and checks for various things that may stop it.
 */

void move_bolt(object *op) {
    object *tmp;
    int mflags;
    sint16 x, y;
    mapstruct *m;

    if(--(op->duration)<0) {
	remove_ob(op);
	free_object(op);
	return;
    }
    hit_map(op,0,op->attacktype,1);
	
    if(!op->direction)
	return;

    if (--op->range<0) {
	op->range=0;
    } else {
	x = op->x+DIRX(op);
	y = op->y+DIRY(op);
	m = op->map;
	mflags = get_map_flags(m, &m, x, y, &x, &y);

	if (mflags & P_OUT_OF_MAP) return;

	/* We are about to run into something - we may bounce */
	/* Calling reflwall is pretty costly, as it has to look at all the objects
	 * on the space.  So only call reflwall if we think the data it returns
	 * will be useful.
	 */
	if (OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, x, y)) || 
	  ((mflags & P_IS_ALIVE) && reflwall(m, x, y, op))) {

	    if(!QUERY_FLAG(op, FLAG_REFLECTING))
		return;

	    /* Since walls don't run diagonal, if the bolt is in
	     * one of 4 main directions, it just reflects back in the
	     * opposite direction.  However, if the bolt is travelling
	     * on the diagonal, it is trickier - eg, a bolt travelling
	     * northwest bounces different if it hits a north/south
	     * wall (bounces to northeast) vs an east/west (bounces
	     * to the southwest.
	     */
	    if(op->direction&1)
		op->direction=absdir(op->direction+4);
	    else {
		int left, right;
		int mflags;

		/* Need to check for P_OUT_OF_MAP: if the bolt is tavelling
		 * over a corner in a tiled map, it is possible that
		 * op->direction is within an adjacent map but either
		 * op->direction-1 or op->direction+1 does not exist.
		 */
		mflags = get_map_flags(op->map, &m, op->x+freearr_x[absdir(op->direction-1)],
			       op->y+freearr_y[absdir(op->direction-1)], &x, &y);

		left = (mflags & P_OUT_OF_MAP) ? 0 : OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, x, y));

		mflags = get_map_flags(op->map, &m, op->x+freearr_x[absdir(op->direction+1)],
                              op->y+freearr_y[absdir(op->direction+1)], &x, &y);
		right = (mflags & P_OUT_OF_MAP) ? 0 : OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, x, y));

		if(left==right)
		    op->direction=absdir(op->direction+4);
		else if(left)
		    op->direction=absdir(op->direction+2);
		else if(right)
		    op->direction=absdir(op->direction-2);
	    }
	    update_turn_face(op); /* A bolt *must* be IS_TURNABLE */
	    return;
	}
	else { /* Create a copy of this object and put it ahead */
	    tmp=get_object();
	    copy_object(op,tmp);
	    tmp->speed_left= -0.1;
	    tmp->x+=DIRX(tmp),tmp->y+=DIRY(tmp);
	    tmp = insert_ob_in_map(tmp,op->map,op,0);
	    /* To make up for the decrease at the top of the function */
	    tmp->duration++;

	    /* New forking code.  Possibly create forks of this object
	     * going off in other directions. 
	     */

	    if(rndm(0, 99)< tmp->stats.Dex) {  /* stats.Dex % of forking */
		forklightning(op,tmp);
	    }
	    /* In this way, the object left behind sticks on the space, but
	     * doesn't create any bolts that continue to move onward.
	     */
	    op->range = 0;
	} /* copy object and move it along */
    } /* if move bolt along */
}

/* fire_bolt
 * object op (cast from caster) files a bolt in dir.
 * spob is the spell object for the bolt.
 * we remove the magic flag - that can be derived from
 * spob->attacktype.
 * This function sets up the appropriate owner and skill
 * pointers.
 */

int fire_bolt(object *op,object *caster,int dir,object *spob, object *skill) {
    object *tmp=NULL;
    int mflags;

    if (!spob->other_arch)
	return 0;

    tmp=arch_to_object(spob->other_arch);
    if(tmp==NULL)
	return 0;

    /*  peterm:  level dependency for bolts  */
    tmp->stats.dam = spob->stats.dam + SP_level_dam_adjust(caster,spob);
    tmp->attacktype = spob->attacktype;
    if (spob->slaying) tmp->slaying = add_refcount(spob->slaying);
    tmp->range = spob->range + SP_level_range_adjust(caster,spob);
    tmp->duration = spob->duration + SP_level_duration_adjust(caster,spob);
    tmp->stats.Dex = spob->stats.Dex;
    tmp->stats.Con = spob->stats.Con;

    tmp->direction=dir;
    if(QUERY_FLAG(tmp, FLAG_IS_TURNABLE))
	SET_ANIMATION(tmp, dir);

    set_owner(tmp,op);
    set_spell_skill(op, caster, spob, tmp);

    tmp->x=op->x + DIRX(tmp);
    tmp->y=op->y + DIRY(tmp);
    tmp->map = op->map;

    mflags = get_map_flags(tmp->map, &tmp->map, tmp->x, tmp->y, &tmp->x, &tmp->y);
    if (mflags & P_OUT_OF_MAP) {
	free_object(tmp);
	return 0;
    }
    if (OB_TYPE_MOVE_BLOCK(tmp, GET_MAP_MOVE_BLOCK(tmp->map, tmp->x, tmp->y))) {
	if(!QUERY_FLAG(tmp, FLAG_REFLECTING)) {
	    free_object(tmp);
	    return 0;
	}
	tmp->x=op->x;
	tmp->y=op->y;
	tmp->direction=absdir(tmp->direction+4);
	tmp->map = op->map;
    }
    if ((tmp = insert_ob_in_map(tmp,tmp->map,op,0)) != NULL)
    move_bolt (tmp);
    return 1;
}



/***************************************************************************
 *
 * BULLET/BALL CODE
 *
 ***************************************************************************/

/* expands an explosion.  op is a piece of the
 * explosion - this expans it in the different directions.
 * At least that is what I think this does.
 */
void explosion(object *op) {
    object *tmp;
    mapstruct *m=op->map;
    int i;

    if(--(op->duration)<0) {
	remove_ob(op);
	free_object(op);
	return;
    }
    hit_map(op,0,op->attacktype,0);

    if(op->range>0) {
	for(i=1;i<9;i++) {
	    sint16 dx,dy;

	    dx=op->x+freearr_x[i];
	    dy=op->y+freearr_y[i];
	    /* ok_to_put_more already does things like checks for walls, 
	     * out of map, etc.
	     */
	    if(ok_to_put_more(op->map,dx,dy,op,op->attacktype)) {
		tmp=get_object();
		copy_object(op,tmp);
		tmp->state=0;
		tmp->speed_left= -0.21;
		tmp->range--;
		tmp->value=0;
		tmp->x=dx;
		tmp->y=dy;
		insert_ob_in_map(tmp,m,op,0);
	    }
	}
    }
}


/* Causes an object to explode, eg, a firebullet,
 * poison cloud ball, etc.  op is the object to
 * explode.
 */
static void explode_bullet(object *op)
{
    tag_t op_tag = op->count;
    object *tmp, *owner;

    if (op->other_arch == NULL) {
	LOG (llevError, "BUG: explode_bullet(): op without other_arch\n");
	remove_ob (op);
	free_object (op);
	return;
    }

    if (op->env) {
	object *env;

	env = object_get_env_recursive(op);
	if (env->map == NULL || out_of_map (env->map, env->x, env->y)) {
	    LOG (llevError, "BUG: explode_bullet(): env out of map\n");
	    remove_ob (op);
	    free_object (op);
	    return;
	}
	remove_ob (op);
	op->x = env->x;
	op->y = env->y;
	insert_ob_in_map(op, env->map, op, INS_NO_MERGE | INS_NO_WALK_ON);
    } else if (out_of_map (op->map, op->x, op->y)) {
	LOG (llevError, "BUG: explode_bullet(): op out of map\n");
	remove_ob (op);
	free_object (op);
	return;
    }

    if (op->attacktype) {
	hit_map (op, 0, op->attacktype, 1);
	if (was_destroyed (op, op_tag))
	    return;
    }

    /* other_arch contains what this explodes into */
    tmp = arch_to_object (op->other_arch);

    copy_owner (tmp, op);
    if (tmp->skill) FREE_AND_CLEAR_STR(tmp->skill);
    if (op->skill) tmp->skill = add_refcount(op->skill);

    owner = get_owner(op);
    if ((tmp->attacktype & AT_HOLYWORD || tmp->attacktype & AT_GODPOWER) && owner &&
      !tailor_god_spell(tmp, owner)) {
	remove_ob (op);
	free_object (op);
	return;
    }
    tmp->x = op->x;
    tmp->y = op->y;

    /* special for bombs - it actually has sane values for these */
    if (op->type == SPELL_EFFECT && op->subtype == SP_BOMB) {
	tmp->attacktype = op->attacktype;
	tmp->range = op->range;
	tmp->stats.dam = op->stats.dam;
	tmp->duration = op->duration;
    } else {
	if (op->attacktype & AT_MAGIC) tmp->attacktype|=AT_MAGIC;
	/* Spell doc describes what is going on here */
	tmp->stats.dam = op->dam_modifier;
	tmp->range = op->stats.maxhp;
	tmp->duration = op->stats.hp;
	/* Used for spell tracking - just need a unique val for this spell -
	 * the count of the parent should work fine.
	 */
	tmp->stats.maxhp = op->count;
    }

    /* Set direction of cone explosion */
    if (tmp->type == SPELL_EFFECT && tmp->subtype == SP_CONE) 
	tmp->stats.sp = op->direction;

    /* Prevent recursion */
    op->move_on = 0;

    insert_ob_in_map(tmp, op->map, op, 0);
    /* remove the firebullet */
    if ( ! was_destroyed (op, op_tag)) {
	remove_ob (op);
	free_object (op);
    }
}



/* checks to see what op should do, given the space it is on
 * (eg, explode, damage player, etc)
 */

void check_bullet(object *op)
{
    tag_t op_tag = op->count, tmp_tag;
    object *tmp;
    int dam, mflags;
    mapstruct *m;
    sint16 sx, sy;

    mflags = get_map_flags(op->map,&m, op->x, op->y, &sx, &sy);

    if (!(mflags & P_IS_ALIVE) &&  !OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, sx, sy)))
        return;

    if (op->other_arch) {
	/* explode object will also remove op */
        explode_bullet (op);
        return;
    }

    /* If nothing alive on this space, no reason to do anything further */
    if (!(mflags & P_IS_ALIVE)) return;

    for (tmp = get_map_ob (op->map,op->x,op->y); tmp != NULL; tmp = tmp->above)
    {
        if (QUERY_FLAG (tmp, FLAG_ALIVE)) {
            tmp_tag = tmp->count;
            dam = hit_player (tmp, op->stats.dam, op, op->attacktype, 1);
            if (was_destroyed (op, op_tag) || ! was_destroyed (tmp, tmp_tag)
                || (op->stats.dam -= dam) < 0)
            {
		if(!QUERY_FLAG(op,FLAG_REMOVED)) { 
		    remove_ob (op);
		    free_object(op);
		    return;
		}
            }
        }
    }
}


/* Basically, we move 'op' one square, and if it hits something,
 * call check_bullet.
 * This function is only applicable to bullets, but not to all
 * fired arches (eg, bolts).
 */

void move_bullet(object *op)
{
    sint16 new_x, new_y;
    int mflags;
    mapstruct *m;

#if 0
    /* We need a better general purpose way to do this */

    /* peterm:  added to make comet leave a trail of burnouts 
	it's an unadulterated hack, but the effect is cool.	*/
    if(op->stats.sp == SP_METEOR) {
      replace_insert_ob_in_map("fire_trail",op);
      if (was_destroyed (op, op_tag))
        return;
    } /* end addition.  */
#endif

    /* Reached the end of its life - remove it */
    if (--op->range <=0) {
        if (op->other_arch) {
            explode_bullet (op);
	} else {
	    remove_ob (op);
	    free_object (op);
	}
        return;
    }

    new_x = op->x + DIRX(op);
    new_y = op->y + DIRY(op);
    m = op->map;
    mflags = get_map_flags(m, &m, new_x, new_y, &new_x, &new_y);

    if (mflags & P_OUT_OF_MAP) {
        remove_ob (op);
        free_object (op);
        return;
    }

    if ( ! op->direction || OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, new_x, new_y))) {
        if (op->other_arch) {
            explode_bullet (op);
        } else {
            remove_ob (op);
            free_object (op);
        }
        return;
    }

    remove_ob (op);
    op->x = new_x;
    op->y = new_y;
    if ((op = insert_ob_in_map (op, m, op,0)) == NULL)
        return;

    if (reflwall (op->map, op->x, op->y, op)) {
        op->direction = absdir (op->direction + 4);
        update_turn_face (op);
    } else {
        check_bullet (op);
    }
}




/* fire_bullet
 * object op (cast from caster) files a bolt in dir.
 * spob is the spell object for the bolt.
 * we remove the magic flag - that can be derived from
 * spob->attacktype.
 * This function sets up the appropriate owner and skill
 * pointers.
 */

int fire_bullet(object *op,object *caster,int dir,object *spob) {
    object *tmp=NULL;
    int mflags;

    if (!spob->other_arch)
	return 0;

    tmp=arch_to_object(spob->other_arch);
    if(tmp==NULL)
	return 0;

    /*  peterm:  level dependency for bolts  */
    tmp->stats.dam = spob->stats.dam + SP_level_dam_adjust(caster,spob);
    tmp->attacktype = spob->attacktype;
    if (spob->slaying) tmp->slaying = add_refcount(spob->slaying);

    tmp->range = 50;
    
    /* Need to store duration/range for the ball to use */
    tmp->stats.hp = spob->duration + SP_level_duration_adjust(caster,spob);
    tmp->stats.maxhp = spob->range + SP_level_range_adjust(caster,spob);
    tmp->dam_modifier = spob->stats.food + SP_level_dam_adjust(caster,spob);

    tmp->direction=dir;
    if(QUERY_FLAG(tmp, FLAG_IS_TURNABLE))
	SET_ANIMATION(tmp, dir);

    set_owner(tmp,op);
    set_spell_skill(op, caster, spob, tmp);

    tmp->x=op->x + freearr_x[dir];
    tmp->y=op->y + freearr_y[dir];
    tmp->map = op->map;

    mflags = get_map_flags(tmp->map, &tmp->map, tmp->x, tmp->y, &tmp->x, &tmp->y);
    if (mflags & P_OUT_OF_MAP) {
	free_object(tmp);
	return 0;
    }
    if (OB_TYPE_MOVE_BLOCK(tmp, GET_MAP_MOVE_BLOCK(tmp->map, tmp->x, tmp->y))) {
	if(!QUERY_FLAG(tmp, FLAG_REFLECTING)) {
	    free_object(tmp);
	    return 0;
	}
	tmp->x=op->x;
	tmp->y=op->y;
	tmp->direction=absdir(tmp->direction+4);
	tmp->map = op->map;
    }
    if ((tmp = insert_ob_in_map(tmp,tmp->map,op,0)) != NULL) {
	check_bullet (tmp);
    }
    return 1;
}




/*****************************************************************************
 *
 * CONE RELATED FUNCTIONS
 *
 *****************************************************************************/


/* drops an object based on what is in the cone's "other_arch" */
static void cone_drop(object *op) {
    object *new_ob = arch_to_object(op->other_arch);

    new_ob->x = op->x;
    new_ob->y = op->y;
    new_ob->level = op->level;
    set_owner(new_ob,op->owner);

    /* preserve skill ownership */
    if(op->skill && op->skill != new_ob->skill) {
	if (new_ob->skill) free_string(new_ob->skill);
	new_ob->skill = add_refcount(op->skill);
    }
    insert_ob_in_map(new_ob,op->map,op,0);
  
}

/* move_cone: causes cone object 'op' to move a space/hit creatures */

void move_cone(object *op) {
    int i;
    tag_t tag;

    /* if no map then hit_map will crash so just ignore object */
    if (! op->map) {
	LOG(llevError,"Tried to move_cone object %s without a map.\n",
	    op->name ? op->name : "unknown");
        op->speed = 0;
        update_ob_speed (op);
	return;
    }

    /* lava saves it's life, but not yours  :) */
    if (QUERY_FLAG(op, FLAG_LIFESAVE)) {
	hit_map(op,0,op->attacktype,0);
	return;
    }

#if 0
    /* Disable this - enabling it makes monsters easier, as
     * when their cone dies when they die.
     */
    /* If no owner left, the spell dies out. */
    if(get_owner(op)==NULL) {
	remove_ob(op);
	free_object(op);
	return;
    }
#endif

    tag = op->count;
    hit_map(op,0,op->attacktype,0);

    /* Check to see if we should push anything.
     * Spell objects with weight push whatever they encounter to some
     * degree.  
     */
    if(op->weight) check_spell_knockback(op);

    if (was_destroyed (op, tag))
        return;

    if((op->duration--)<0) {
	remove_ob(op);
	free_object(op);
	return;
    }
    /* Object has hit maximum range, so don't have it move
     * any further.  When the duration above expires,
     * then the object will get removed.
     */
    if (--op->range < 0) {
	op->range=0;	/* just so it doesn't wrap */
	return;
    }

    for(i= -1;i<2;i++) {
	sint16 x=op->x+freearr_x[absdir(op->stats.sp+i)],
	    y=op->y+freearr_y[absdir(op->stats.sp+i)];

	if(ok_to_put_more(op->map,x,y,op,op->attacktype)) {
	    object *tmp=get_object();
            copy_object(op, tmp);
	    tmp->x=x;
	    tmp->y=y;

	    tmp->duration = op->duration + 1;

	    /* Use for spell tracking - see ok_to_put_more() */
	    tmp->stats.maxhp = op->stats.maxhp;
	    insert_ob_in_map(tmp,op->map,op,0);
	    if (tmp->other_arch) cone_drop(tmp);
	}
    }
}

/* cast_cone: casts a cone spell.
 * op: person firing the object.
 * caster: object casting the spell.
 * dir: direction to fire in.
 * spell: spell that is being fired.  It uses other_arch for the archetype
 * to fire.
 * returns 0 on failure, 1 on success.
 */
int cast_cone(object *op, object *caster,int dir, object *spell)
{
    object *tmp;
    int i,success=0,range_min= -1,range_max=1;
    mapstruct *m;
    sint16  sx, sy;
    MoveType	movetype;

    if (!spell->other_arch) return 0;

    if (op->type == PLAYER && QUERY_FLAG(op, FLAG_UNDEAD) && 
     op->attacktype & AT_TURN_UNDEAD) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
	      "Your undead nature prevents you from turning undead!", NULL);
	return 0;
    }

    if(!dir) {
	range_min= 0;
	range_max=8;
    }

    /* Need to know what the movetype of the object we are about
     * to create is, so we can know if the space we are about to
     * insert it into is blocked.
     */
    movetype = spell->other_arch->clone.move_type;

    for(i=range_min;i<=range_max;i++) {
	sint16 x,y, d;

	/* We can't use absdir here, because it never returns
	 * 0.  If this is a rune, we want to hit the person on top
	 * of the trap (d==0).  If it is not a rune, then we don't want
	 * to hit that person.
	 */
	d = dir + i;
	while (d < 0) d+=8;
	while (d > 8) d-=8;

	/* If it's not a rune, we don't want to blast the caster.
	 * In that case, we have to see - if dir is specified,
	 * turn this into direction 8.  If dir is not specified (all
	 * direction) skip - otherwise, one line would do more damage
	 * becase 0 direction will go through 9 directions - necessary
	 * for the rune code.
	 */
	if (caster->type != RUNE && d==0) {
	    if (dir!=0) d=8;
	    else continue;
	}

	x = op->x+freearr_x[d];
	y = op->y+freearr_y[d];

	if(get_map_flags(op->map, &m, x,y, &sx, &sy) & P_OUT_OF_MAP)
	    continue;

	if ((movetype & GET_MAP_MOVE_BLOCK(m, sx, sy)) == movetype)
	    continue;

	success=1;
	tmp=arch_to_object(spell->other_arch);
	set_owner(tmp,op);
	set_spell_skill(op, caster, spell, tmp);
	tmp->level = caster_level (caster, spell);
	tmp->x = sx;
	tmp->y = sy;
	tmp->attacktype=spell->attacktype;

	/* holy word stuff */                
	if((tmp->attacktype & AT_HOLYWORD) || (tmp->attacktype & AT_GODPOWER)) {
            if(!tailor_god_spell(tmp,op)) return 0;  
	}
	    
	if(dir)
	    tmp->stats.sp=dir;
	else
	    tmp->stats.sp=i;

	tmp->range=spell->range + SP_level_range_adjust(caster,spell);

	/* If casting it in all directions, it doesn't go as far */
	if (dir == 0) {
	    tmp->range /= 4;
	    if (tmp->range < 2 && spell->range >=2) tmp->range = 2;
	}
	tmp->stats.dam=spell->stats.dam + SP_level_dam_adjust(caster,spell);
	tmp->duration=spell->duration + SP_level_duration_adjust(caster,spell);

	/* Special bonus for fear attacks */
	if (tmp->attacktype & AT_FEAR) {
	    if (caster->type == PLAYER) tmp->duration += fear_bonus[caster->stats.Cha];
	    else
		tmp->duration += caster->level/3;
	}
	if (tmp->attacktype & (AT_HOLYWORD | AT_TURN_UNDEAD)) {
	    if (caster->type == PLAYER) tmp->duration += turn_bonus[caster->stats.Wis]/5;
	    else
		tmp->duration += caster->level/3;
	}


	if ( !(tmp->move_type & MOVE_FLY_LOW))
	    LOG (llevDebug, "cast_cone(): arch %s doesn't have flying 1\n",
		 spell->other_arch->name);

	if (!tmp->move_on && tmp->stats.dam) {
	    LOG (llevDebug, 
		 "cast_cone(): arch %s doesn't have move_on set\n", 
		 spell->other_arch->name);
	}
	insert_ob_in_map(tmp,m,op,0);

	/* This is used for tracking spells so that one effect doesn't hit
	 * a single space too many times.
	 */
	tmp->stats.maxhp = tmp->count;

	if(tmp->other_arch) cone_drop(tmp);
    }
    return success;
}

/****************************************************************************
 *
 * BOMB related code
 *
 ****************************************************************************/


/* This handles an exploding bomb.
 * op is the original bomb object.
 */
void animate_bomb(object *op) {
    int i;
    object *env, *tmp;
    archetype *at;

    if(op->state!=NUM_ANIMATIONS(op)-1)
	return;

    env = object_get_env_recursive(op);

    if (op->env) {
        if (env->map == NULL)
            return;

	if (env->type == PLAYER)
	    esrv_del_item(env->contr, op->count);

	remove_ob(op);
	op->x = env->x;
	op->y = env->y;
	if ((op = insert_ob_in_map (op, env->map, op,0)) == NULL)
	    return;
    }

    /* This copies a lot of the code from the fire bullet,
     * but using the cast_bullet isn't really feasible,
     * so just set up the appropriate values.
     */
    at = find_archetype(SPLINT);
    if (at) {
	for(i=1;i<9;i++) {
	    if (out_of_map(op->map, op->x + freearr_x[i], op->y + freearr_x[i]))
		continue;
	    tmp = arch_to_object(at);
	    tmp->direction = i;
	    tmp->range = op->range;
	    tmp->stats.dam = op->stats.dam;
	    tmp->duration = op->duration;
	    tmp->attacktype = op->attacktype;
            copy_owner (tmp, op);
	    if(op->skill && op->skill != tmp->skill) {
		if (tmp->skill) free_string(tmp->skill);
		tmp->skill = add_refcount(op->skill);
	    }
	    if(QUERY_FLAG(tmp, FLAG_IS_TURNABLE))
		SET_ANIMATION(tmp, i);
	    tmp->x = op->x + freearr_x[i];
	    tmp->y = op->y + freearr_x[i];
	    insert_ob_in_map(tmp, op->map, op, 0);
	    move_bullet(tmp);
	}
    }

    explode_bullet(op);
}

int create_bomb(object *op,object *caster,int dir, object *spell) {

    object *tmp;
    int mflags;
    sint16 dx=op->x+freearr_x[dir], dy=op->y+freearr_y[dir];
    mapstruct *m;

    mflags = get_map_flags(op->map, &m, dx,dy, &dx,&dy);
    if ((mflags & P_OUT_OF_MAP) || (GET_MAP_MOVE_BLOCK(m, dx, dy) & MOVE_WALK)) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
		      "There is something in the way.", NULL);
	return 0;
    }
    tmp=arch_to_object(spell->other_arch);

    /*  level dependencies for bomb  */
    tmp->range=spell->range + SP_level_range_adjust(caster,spell);
    tmp->stats.dam=spell->stats.dam + SP_level_dam_adjust(caster,spell);
    tmp->duration = spell->duration + SP_level_duration_adjust(caster,spell);
    tmp->attacktype = spell->attacktype;

    set_owner(tmp,op);
    set_spell_skill(op, caster, spell, tmp);
    tmp->x=dx;
    tmp->y=dy;
    insert_ob_in_map(tmp,m,op,0);
    return 1;
}

/****************************************************************************
 *
 * smite related spell code.
 *
 ****************************************************************************/

/* get_pointed_target() - this is used by finger of death
 * and the 'smite' spells. Returns the pointer to the first
 * monster in the direction which is pointed to by op. b.t.
 * op is the caster - really only used for the source location.
 * dir is the direction to look in.
 * range is how far out to look.
 * type is the type of spell - either SPELL_MANA or SPELL_GRACE.
 * this info is used for blocked magic/unholy spaces.
 */
 
static object *get_pointed_target(object *op, int dir, int range, int type) {
    object *target;
    sint16 x,y;
    int dist, mflags;
    mapstruct *mp;

    if (dir==0) return NULL;

    for (dist=1; dist<range; dist++) {
	x = op->x + freearr_x[dir] * dist;
	y = op->y + freearr_y[dir] * dist;
	mp = op->map;
	mflags = get_map_flags(op->map, &mp, x, y, &x, &y);

	if (mflags & P_OUT_OF_MAP)  return NULL;
	if ((type & SPELL_MANA) && (mflags & P_NO_MAGIC)) return NULL;
	if ((type & SPELL_GRACE) && (mflags & P_NO_CLERIC)) return NULL;
	if (GET_MAP_MOVE_BLOCK(mp, x, y) & MOVE_FLY_LOW) return NULL;

	if (mflags & P_IS_ALIVE) {
	    for(target=get_map_ob(mp,x,y); target; target=target->above) {
		if(QUERY_FLAG(target->head?target->head:target,FLAG_MONSTER)) {
		    return target;
		}
	    }
	}
    }
    return NULL;
}


/* cast_smite_arch() - the priest points to a creature and causes
 * a 'godly curse' to decend. 
 * usual params -
 * op = player
 * caster = object casting the spell.
 * dir = direction being cast
 * spell = spell object
 */

int cast_smite_spell (object *op, object *caster,int dir, object *spell) {
    object *effect, *target;
    object *god = find_god(determine_god(op));
    int range;

    range = spell->range + SP_level_range_adjust(caster,spell);
    target = get_pointed_target(op,dir, 50, spell->stats.grace?SPELL_GRACE:SPELL_MANA);

    /* Bunch of conditions for casting this spell.  Note that only 
     * require a god if this is a cleric spell (requires grace).
     * This makes this spell much more general purpose - it can be used
     * by wizards also, which is good, because I think this is a very
     * interesting spell.
     * if it is a cleric spell, you need a god, and the creature
     * can't be friendly to your god.
     */

    if(!target || QUERY_FLAG(target,FLAG_REFL_SPELL)
      ||(!god && spell->stats.grace) 
      ||(target->title && god && !strcmp(target->title,god->name))
      ||(target->race && god && strstr(target->race,god->race))) {
        draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "Your request is unheeded.", NULL);
        return 0;
    }

    if (spell->other_arch)
	effect = arch_to_object(spell->other_arch);
    else
	return 0;

    /* tailor the effect by priest level and worshipped God */
    effect->level = caster_level (caster, spell);
    effect->attacktype = spell->attacktype;
    if (effect->attacktype & (AT_HOLYWORD | AT_GODPOWER)) {
        if(tailor_god_spell(effect,op))
           draw_ext_info_format(NDI_UNIQUE,0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_SUCCESS,
			"%s answers your call!",
			"%s answers your call!",
			determine_god(op));
        else {
           draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			 "Your request is ignored.", NULL);
           return 0;
	}
    }
 
    /* size of the area of destruction */
    effect->range=spell->range + 
                SP_level_range_adjust(caster,spell);
    effect->duration=spell->duration + 
                SP_level_range_adjust(caster,spell);

    if (effect->attacktype & AT_DEATH) {
	effect->level=spell->stats.dam + 
                SP_level_dam_adjust(caster,spell);

	/* casting death spells at undead isn't a good thing */
	if QUERY_FLAG(target, FLAG_UNDEAD) {
	    if(random_roll(0, 2, op, PREFER_LOW)) {
		draw_ext_info(NDI_UNIQUE,0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			      "Idiot! Your spell boomerangs!", NULL);
		effect->x=op->x;
		effect->y=op->y;
	    } else {
		draw_ext_info_format(NDI_UNIQUE,0,op,
				     MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
				     "The %s looks stronger!",
				     "The %s looks stronger!",
				     query_name(target));
		target->stats.hp = target->stats.maxhp*2;
		free_object(effect);
		return 0;
	    }
	}
    } else {
	/* how much woe to inflict :) */
	effect->stats.dam=spell->stats.dam + 
                SP_level_dam_adjust(caster,spell);
    }

    set_owner(effect,op);
    set_spell_skill(op, caster, spell, effect);
 
    /* ok, tell it where to be, and insert! */
    effect->x=target->x;
    effect->y=target->y;
    insert_ob_in_map(effect,target->map,op,0);
 
    return 1;
}


/****************************************************************************
 *
 * MAGIC MISSILE code.
 * note that the fire_bullet is used to fire the missile.  The
 * code here is just to move the missile.
 ****************************************************************************/

/* op is a missile that needs to be moved */
void move_missile(object *op) {
    int i, mflags;
    object *owner;
    sint16 new_x, new_y;
    mapstruct *m;

    if (op->range-- <=0) {
	remove_ob(op);
	free_object(op);
	return;
    }

    owner = get_owner(op);
#if 0
    /* It'd make things nastier if this wasn't here - spells cast by
     * monster that are then killed would continue to survive
     */
    if (owner == NULL) {
	remove_ob(op);
	free_object(op);
	return;
    }
#endif

    new_x = op->x + DIRX(op);
    new_y = op->y + DIRY(op);

    mflags = get_map_flags(op->map, &m, new_x, new_y, &new_x, &new_y);

    if (!(mflags & P_OUT_OF_MAP) &&
	((mflags & P_IS_ALIVE) || OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, new_x, new_y)))) {
	tag_t tag = op->count;
	hit_map (op, op->direction, AT_MAGIC, 1);
	/* Basically, missile only hits one thing then goes away.
	 * we need to remove it if someone hasn't already done so.
	 */
	if ( ! was_destroyed (op, tag)) {
	    remove_ob (op);
	    free_object(op);
	}
	return;
    }

    remove_ob(op);
    if ( ! op->direction || (mflags & P_OUT_OF_MAP)) {
	free_object(op);
	return;
    }
    op->x = new_x;
    op->y = new_y;
    op->map = m;
    i=spell_find_dir(op->map, op->x, op->y, get_owner(op));
    if(i > 0 && i != op->direction){
	op->direction=i;
	SET_ANIMATION(op, op->direction);
    }
    insert_ob_in_map(op,op->map,op,0);
}

/****************************************************************************
 * Destruction
 ****************************************************************************/
/* make_object_glow() - currently only makes living objects glow.
 * we do this by creating a force and inserting it in the 
 * object. if time is 0, the object glows permanently. To truely
 * make this work for non-living objects, we would have to 
 * give them the capability to have an inventory. b.t.
 */

static int make_object_glow(object *op, int radius, int time) {
    object *tmp;

    /* some things are unaffected... */
    if(op->path_denied&PATH_LIGHT) 
	return 0;

    tmp=create_archetype(FORCE_NAME);
    tmp->speed = 0.01;
    tmp->stats.food = time;
    SET_FLAG(tmp, FLAG_IS_USED_UP);
    tmp->glow_radius=radius;
    if (tmp->glow_radius > MAX_LIGHT_RADII)
	tmp->glow_radius = MAX_LIGHT_RADII;

    tmp->x=op->x;
    tmp->y=op->y;
    if(tmp->speed<MIN_ACTIVE_SPEED) tmp->speed = MIN_ACTIVE_SPEED; /* safety */
    tmp=insert_ob_in_ob(tmp,op);
    if (tmp->glow_radius > op->glow_radius)
	op->glow_radius = tmp->glow_radius;

    if(!tmp->env||op!=tmp->env) { 
	LOG(llevError,"make_object_glow() failed to insert glowing force in %s\n", 
		op->name); 
	return 0; 
   }
   return 1;
}


 

int cast_destruction(object *op, object *caster, object *spell_ob) {
    int i,j, range, mflags, friendly=0, dam, dur;
    sint16 sx,sy;
    mapstruct *m;
    object *tmp;
    const char *skill;

    range = spell_ob->range + SP_level_range_adjust(caster, spell_ob);
    dam = spell_ob->stats.dam + SP_level_dam_adjust(caster, spell_ob);
    dur = spell_ob->duration + SP_level_duration_adjust(caster, spell_ob);
    if (QUERY_FLAG(op, FLAG_FRIENDLY) || op->type == PLAYER) friendly=1;

    /* destruction doesn't use another spell object, so we need
     * update op's skill pointer so that exp is properly awarded.
     * We do some shortcuts here - since this is just temporary
     * and we'll reset the values back, we don't need to go through
     * the full share string/free_string route.
     */
    skill = op->skill;
    if (caster == op) op->skill = spell_ob->skill;
    else if (caster->skill) op->skill = caster->skill;
    else op->skill = NULL;

    change_skill(op,  find_skill_by_name(op, op->skill), 1);

    for(i= -range; i<range; i++) {
	for(j=-range; j<range ; j++) {
	    m = op->map;
	    sx = op->x + i;
	    sy = op->y + j;
	    mflags = get_map_flags(m, &m, sx, sy, &sx, &sy);
	    if (mflags & P_OUT_OF_MAP) continue;
	    if (mflags & P_IS_ALIVE) {
		for (tmp=get_map_ob(m, sx, sy); tmp; tmp=tmp->above) {
		    if (QUERY_FLAG(tmp, FLAG_ALIVE) || tmp->type==PLAYER) break;
		}
		if (tmp) {
		    if (tmp->head) tmp=tmp->head;

		    if ((friendly && !QUERY_FLAG(tmp, FLAG_FRIENDLY) && tmp->type!=PLAYER) ||
		       (!friendly && (QUERY_FLAG(tmp, FLAG_FRIENDLY) || tmp->type==PLAYER))) {
			if (spell_ob->subtype == SP_DESTRUCTION) {
			    hit_player(tmp,dam,op,spell_ob->attacktype,0);
			    if (spell_ob->other_arch) {
				tmp = arch_to_object(spell_ob->other_arch);
				tmp->x = sx;
				tmp->y = sy;
				insert_ob_in_map(tmp, m, op, 0);
			    }
			}
			else if (spell_ob->subtype == SP_FAERY_FIRE &&
				 tmp->resist[ATNR_MAGIC]!=100) {
			    if (make_object_glow(tmp, 1, dur) && spell_ob->other_arch) {
				object *effect = arch_to_object(spell_ob->other_arch);
				effect->x = sx;
				effect->y = sy;
				insert_ob_in_map(effect, m, op, 0);
			    }
			}
		    }
		}
	    }
	}
    }
    op->skill = skill;
    return 1;
}

/***************************************************************************
 *
 * CURSE
 *
 ***************************************************************************/

int cast_curse(object *op, object *caster, object *spell_ob, int dir) {
    object *god = find_god(determine_god(op));
    object *tmp, *force;

    tmp =  get_pointed_target(op, (dir==0)?op->direction:dir,
		spell_ob->range, SPELL_GRACE);
    if (!tmp) {
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
	      "There is no one in that direction to curse.", NULL);
	return 0;
    }

    /* If we've already got a force of this type, don't add a new one. */
    for(force=tmp->inv; force!=NULL; force=force->below) {
	if (force->type==FORCE && force->subtype == FORCE_CHANGE_ABILITY)  {
	    if (force->name == spell_ob->name) {
		break;
	    }
	    else if (spell_ob->race && spell_ob->race == force->name) {
		draw_ext_info_format(NDI_UNIQUE, 0, op,
			     MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			     "You can not cast %s while %s is in effect",
			     "You can not cast %s while %s is in effect",
			     spell_ob->name, force->name_pl);
		return 0;
	    }
	}
    }

    if(force==NULL) {
	force=create_archetype(FORCE_NAME);
	force->subtype = FORCE_CHANGE_ABILITY;
	free_string(force->name);
	if (spell_ob->race) 
	    force->name = add_refcount(spell_ob->race);
	else
	    force->name = add_refcount(spell_ob->name);
	free_string(force->name_pl);
	force->name_pl = add_refcount(spell_ob->name);

    } else {
	int duration;

	duration = spell_ob->duration + SP_level_duration_adjust(caster, spell_ob) * 50;
	if (duration > force->duration) {
	    force->duration = duration;
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_SUCCESS,
			  "You recast the spell while in effect.", NULL);
	} else {
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			  "Recasting the spell had no effect.", NULL);
	}
	return 1;
    }
    force->duration = spell_ob->duration + SP_level_duration_adjust(caster, spell_ob) * 50;
    force->speed = 1.0;
    force->speed_left = -1.0;
    SET_FLAG(force, FLAG_APPLIED);

    if(god) {
	if (spell_ob->last_grace)
	    force->path_repelled=god->path_repelled;
	if (spell_ob->last_grace)
	    force->path_denied=god->path_denied;
	draw_ext_info_format(NDI_UNIQUE, 0,tmp, MSG_TYPE_VICTIM, MSG_TYPE_VICTIM_SPELL,
			     "You are a victim of %s's curse!",
			     "You are a victim of %s's curse!",
			     god->name);
    } else 
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "Your curse seems empty.", NULL);


    if(tmp!=op && op->type==PLAYER)
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_SUCCESS,
			     "You curse %s!",
			     "You curse %s!",
			     tmp->name);

    force->stats.ac = spell_ob->stats.ac;
    force->stats.wc = spell_ob->stats.wc;

    change_abil(tmp,force);     /* Mostly to display any messages */
    insert_ob_in_ob(force,tmp);
    fix_object(tmp);
    return 1;

}


/**********************************************************************
 * mood change
 * Arguably, this may or may not be an attack spell.  But since it
 * effects monsters, it seems best to put it into this file
 ***********************************************************************/

/* This covers the various spells that change the moods of monsters -
 * makes them angry, peacful, friendly, etc.
 */
int mood_change(object *op, object *caster, object *spell) {
    object *tmp, *god, *head;
    int done_one, range, mflags, level, at, best_at;
    sint16 x, y, nx, ny;
    mapstruct	*m;
    const char *race;

    /* We precompute some values here so that we don't have to keep
     * doing it over and over again.
     */
    god=find_god(determine_god(op));
    level=caster_level(caster, spell);
    range = spell->range + SP_level_range_adjust(caster, spell);

    /* On the bright side, no monster should ever have a race of GOD_...
     * so even if the player doesn't worship a god, if race=GOD_.., it
     * won't ever match anything.
     */
    if (!spell->race) race=NULL;
    else if (god && !strcmp(spell->race, "GOD_SLAYING")) race = god->slaying;
    else if (god && !strcmp(spell->race, "GOD_FRIEND")) race = god->race;
    else race = spell->race;
    

    for (x = op->x - range; x <= op->x + range; x++)
	for (y = op->y - range; y <= op->y + range; y++) {

	    done_one=0;
	    m = op->map;
	    nx = x;
	    ny = y;
	    mflags = get_map_flags(m, &m, x, y, &nx, &ny);
	    if (mflags & P_OUT_OF_MAP) continue;

	    /* If there is nothing living on this space, no need to go further */
	    if (!(mflags & P_IS_ALIVE)) continue;

	    for (tmp = get_map_ob(m, nx, ny); tmp; tmp = tmp->above)
		if (QUERY_FLAG(tmp, FLAG_MONSTER)) break;

	    /* There can be living objects that are not monsters */
	    if (!tmp || tmp->type==PLAYER) continue;

	    /* Only the head has meaningful data, so resolve to that */
	    if (tmp->head) head=tmp->head;
	    else head=tmp;

	    /* Make sure the race is OK.  Likewise, only effect undead if spell specifically allows it */
	    if (race && head->race && !strstr(race, head->race)) continue;
	    if (QUERY_FLAG(head, FLAG_UNDEAD) && !QUERY_FLAG(spell, FLAG_UNDEAD)) continue;

	    /* Now do a bunch of stuff related to saving throws */
	    best_at = -1;
	    if (spell->attacktype) {
		for (at=0; at < NROFATTACKS; at++)
		    if (spell->attacktype & (1 << at))
			if (best_at == -1 || head->resist[at] > head->resist[best_at]) best_at = at;

            if (best_at == -1) at=0;
	        else {
		        if (head->resist[best_at] == 100) continue;
		        else at = head->resist[best_at] / 5;
	        }
	        at -= level / 5;
	        if (did_make_save(head, head->level, at)) continue;
	    }
        else    /* spell->attacktype */
            /*
                Spell has no attacktype (charm & such), so we'll have a specific saving:
                 * if spell level < monster level, no go
                 * else, chance of effect = 20 + min( 50, 2 * ( spell level - monster level ) )

                The chance will then be in the range [20-70] percent, not too bad.

                This is required to fix the 'charm monster' abuse, where a player level 1 can
                charm a level 125 monster...

                Ryo, august 14th
            */
            {
            if ( head->level > level ) continue;
            if ( random_roll( 0, 100, caster, PREFER_LOW ) >= ( 20 + MIN( 50, 2 * ( level - head->level ) ) ) )
                /* Failed, no effect */
                continue;
            }

	    /* Done with saving throw.  Now start effecting the monster */

	    /* aggravation */
	    if (QUERY_FLAG(spell, FLAG_MONSTER)) {
		CLEAR_FLAG(head, FLAG_SLEEP);
		if (QUERY_FLAG(head, FLAG_FRIENDLY))
		    remove_friendly_object(head);

		done_one = 1;
		head->enemy = op;
	    }

	    /* calm monsters */
	    if (QUERY_FLAG(spell, FLAG_UNAGGRESSIVE) && !QUERY_FLAG(head, FLAG_UNAGGRESSIVE)) {
		SET_FLAG(head, FLAG_UNAGGRESSIVE);
		head->enemy = NULL;
		done_one = 1;
	    }

	    /* berserk monsters */
	    if (QUERY_FLAG(spell, FLAG_BERSERK) && !QUERY_FLAG(head, FLAG_BERSERK)) {
		SET_FLAG(head, FLAG_BERSERK);
		done_one = 1;
	    }
	    /* charm */
	    if (QUERY_FLAG(spell, FLAG_NO_ATTACK) && !QUERY_FLAG(head, FLAG_FRIENDLY)) {
		SET_FLAG(head, FLAG_FRIENDLY);
        /* Prevent uncontolled outbreaks of self replicating monsters.
           Typical use case is charm, go somwhere, use aggravation to make hostile.
           This could lead to fun stuff like mice outbreak in bigworld and server crawl. */
        CLEAR_FLAG(head, FLAG_GENERATOR);
        set_owner(head, op);
		set_spell_skill(op, caster, spell, head);
		add_friendly_object(head);
		head->attack_movement = PETMOVE;
		done_one = 1;
		change_exp(op, head->stats.exp / 2, head->skill, SK_EXP_ADD_SKILL);
		head->stats.exp = 0;
	    }

	    /* If a monster was effected, put an effect in */
	    if (done_one && spell->other_arch) {
		tmp = arch_to_object(spell->other_arch);
		tmp->x = nx;
		tmp->y = ny;
		insert_ob_in_map(tmp, m, op, 0);
	    }
	} /* for y */

    return 1;
}


/* Move_ball_spell: This handles ball type spells that just sort of wander
 * about.  was called move_ball_lightning, but since more than the ball
 * lightning spell used it, that seemed misnamed.
 * op is the spell effect.
 * note that duration is handled by process_object() in time.c
 */

void move_ball_spell(object *op) {
    int i,j,dam_save,dir, mflags;
    sint16 nx,ny, hx, hy;
    object *owner;
    mapstruct *m;

    owner = get_owner(op);

    /* the following logic makes sure that the ball doesn't move into a wall,
     * and makes sure that it will move along a wall to try and get at it's
     * victim.  The block immediately below more or less chooses a random
     * offset to move the ball, eg, keep it mostly on course, with some
     * deviations.
     */

    dir = 0;
    if(!(rndm(0, 3)))
	j = rndm(0, 1);
    else j=0;

    for(i = 1; i < 9; i++) {
	/* i bit 0: alters sign of offset
	 * other bits (i / 2): absolute value of offset
	 */

	int offset = ((i ^ j) & 1) ? (i / 2) : - (i / 2);
	int tmpdir = absdir (op->direction + offset);

	nx = op->x + freearr_x[tmpdir];
	ny = op->y + freearr_y[tmpdir];
	if ( ! (get_map_flags(op->map, &m, nx, ny, &nx, &ny) & P_OUT_OF_MAP) &&
	    !(OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, nx, ny)))) {
	    dir = tmpdir;
	    break;
	}
    }
    if (dir == 0) {
	nx = op->x;
	ny = op->y;
	m = op->map;
    }

    remove_ob(op);
    op->y=ny;
    op->x=nx;
    insert_ob_in_map(op,m,op,0);
	 
    dam_save = op->stats.dam;  /* save the original dam: we do halfdam on 
                                surrounding squares */

    /* loop over current square and neighbors to hit.
     * if this has an other_arch field, we insert that in
     * the surround spaces.
     */
    for(j=0;j<9;j++) {
	object *new_ob;

	hx = nx+freearr_x[j];
	hy = ny+freearr_y[j];

	m = op->map;
	mflags = get_map_flags(m, &m, hx, hy, &hx, &hy);

	if (mflags & P_OUT_OF_MAP) continue;

	/* first, don't ever, ever hit the owner.  Don't hit out
	 * of the map either.
	 */

	if((mflags & P_IS_ALIVE) && (!owner || owner->x!=hx || owner->y!=hy || !on_same_map(owner,op))) {
	    if(j) op->stats.dam = dam_save/2;
	    hit_map(op,j,op->attacktype,1);

	}

	/* insert the other arch */
	if(op->other_arch && !(OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, hx, hy)))) {
	    new_ob = arch_to_object(op->other_arch);
	    new_ob->x = hx;
	    new_ob->y = hy;
	    insert_ob_in_map(new_ob,m,op,0);
	}
    }

    /* restore to the center location and damage*/
    op->stats.dam = dam_save;

    i=spell_find_dir(op->map,op->x,op->y,get_owner(op));

    if(i>=0) { /* we have a preferred direction!  */
	/* pick another direction if the preferred dir is blocked. */
	if (get_map_flags(op->map,&m, nx + freearr_x[i], ny + freearr_y[i],&hx,&hy) & P_OUT_OF_MAP ||
	   OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, hx, hy))) {
	    i= absdir(i + rndm(0, 2) -1);  /* -1, 0, +1 */
	}
	op->direction=i;
    }
}


/* move_swarm_spell:  peterm 
 * This is an implementation of the swarm spell.  It was written for
 * meteor swarm, but it could be used for any swarm.  A swarm spell
 * is a special type of object that casts swarms of other types
 * of spells.  Which spell it casts is flexible.  It fires the spells
 * from a set of squares surrounding the caster, in a given direction. 
 */

void move_swarm_spell(object *op)
{
    static int cardinal_adjust[9] = { -3, -2, -1, 0, 0, 0, 1, 2, 3 };
    static int diagonal_adjust[10] = { -3, -2, -2, -1, 0, 0, 1, 2, 2, 3 };
    sint16 target_x, target_y, origin_x, origin_y;
    int basedir, adjustdir;
    mapstruct *m;
    object *owner;

    owner = get_owner(op);
    if(op->duration == 0 || owner == NULL || owner->x != op->x || owner->y != op->y) {
	remove_ob(op);
	free_object(op);
	return;
    }
    op->duration--;

    basedir = op->direction;
    if(basedir == 0) {
        /* spray in all directions! 8) */
        basedir = rndm(1, 8);
    }

    /* new offset calculation to make swarm element distribution
     * more uniform 
     */
    if(op->duration) {
        if(basedir & 1) { 
            adjustdir = cardinal_adjust[rndm(0, 8)];
        } else {
            adjustdir = diagonal_adjust[rndm(0, 9)];
        }
    } else {
        adjustdir = 0;  /* fire the last one from forward. */
    }

    target_x = op->x + freearr_x[absdir(basedir + adjustdir)];
    target_y = op->y + freearr_y[absdir(basedir + adjustdir)];

    /* back up one space so we can hit point-blank targets, but this
     * necessitates extra out_of_map check below 
     */
    origin_x = target_x - freearr_x[basedir];
    origin_y = target_y - freearr_y[basedir];


    /* spell pointer is set up for the spell this casts.  Since this
     * should just be a pointer to the spell in some inventory,
     * it is unlikely to disappear by the time we need it.  However,
     * do some sanity checking anyways.
     */
    
    if (op->spell && op->spell->type == SPELL && 
	!(get_map_flags(op->map, &m, target_x, target_y, &target_x, &target_y) & P_OUT_OF_MAP)) {

	/* Bullet spells have a bunch more customization that needs to be done */
	if (op->spell->subtype == SP_BULLET)
	    fire_bullet(owner, op, basedir, op->spell);
	else if (op->spell->subtype == SP_MAGIC_MISSILE)
	    fire_arch_from_position (owner, op, origin_x, origin_y, basedir, op->spell);
    }
}




/*  fire_swarm:
 *  The following routine creates a swarm of objects.  It actually
 *  sets up a specific swarm object, which then fires off all
 *  the parts of the swarm.  
 *
 *  op:  the owner
 *  caster: the caster (owner, wand, rod, scroll)
 *  dir: the direction everything will be fired in
 *  spell - the spell that is this spell.
 *  n:  the number to be fired.
 */

int fire_swarm (object *op, object *caster, object *spell, int dir)
{
    object *tmp;
    int i;

    if (!spell->other_arch) return 0;

    tmp=create_archetype(SWARM_SPELL);
    tmp->x=op->x;
    tmp->y=op->y;	    
    set_owner(tmp,op);       /* needed so that if swarm elements kill, caster gets xp.*/
    set_spell_skill(op, caster, spell, tmp);

    tmp->level=caster_level(caster, spell);   /*needed later, to get level dep. right.*/
    tmp->spell = arch_to_object(spell->other_arch);

    tmp->attacktype = tmp->spell->attacktype;

    if (tmp->attacktype & AT_HOLYWORD || tmp->attacktype & AT_GODPOWER) {
	if ( ! tailor_god_spell (tmp, op))
	    return 1;
    }
    tmp->duration = SP_level_duration_adjust(caster, spell);
    for (i=0; i< spell->duration; i++) 
	tmp->duration += die_roll(1, 3, op, PREFER_HIGH);

    tmp->direction=dir; 
    tmp->invisible=1;
    insert_ob_in_map(tmp,op->map,op,0);
    return 1;
}


/* See the spells documentation file for why this is its own
 * function.
 */
int cast_light(object *op,object *caster,object *spell, int dir) {
    object  *target=NULL,*tmp=NULL;
    sint16  x,y;
    int	    dam, mflags;
    mapstruct *m;

    dam = spell->stats.dam +  SP_level_dam_adjust(caster,spell);

    if(!dir) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
		      "In what direction?", NULL);
	return 0;
    }

    x=op->x+freearr_x[dir];
    y=op->y+freearr_y[dir];
    m = op->map;

    mflags = get_map_flags(m, &m, x, y, &x, &y);

    if (mflags & P_OUT_OF_MAP) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "Nothing is there.", NULL);
	return 0;
    }

    if (mflags & P_IS_ALIVE && spell->attacktype) {
	for(target=get_map_ob(m,x,y);target;target=target->above)
	    if(QUERY_FLAG(target,FLAG_MONSTER)) {
		/* oky doky. got a target monster. Lets make a blinding attack */
		if(target->head) target = target->head;
		(void) hit_player(target,dam,op,spell->attacktype,1);
		return 1; /* one success only! */
	    }
    }

    /* no live target, perhaps a wall is in the way? */
    if (OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, x, y))) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
		      "Something is in the way.", NULL);
	return 0;
    }

    /* ok, looks groovy to just insert a new light on the map */
    tmp=arch_to_object(spell->other_arch);
    if(!tmp) { 
	LOG(llevError,"Error: spell arch for cast_light() missing.\n");
	return 0;
    }
    tmp->stats.food = spell->duration + SP_level_duration_adjust(caster,spell);
    if (tmp->glow_radius) {
	tmp->glow_radius= spell->range + SP_level_range_adjust(caster,spell);
	if (tmp->glow_radius > MAX_LIGHT_RADII) tmp->glow_radius = MAX_LIGHT_RADII;
    }
    tmp->x=x;
    tmp->y=y;
    insert_ob_in_map(tmp,m,op,0);
    return 1;
}




/* cast_cause_disease:  this spell looks along <dir> from the
 * player and infects someone. 
 * op is the player/monster, caster is the object, dir is the direction
 * to cast, disease_arch is the specific disease, and type is the spell number
 * perhaps this should actually be in disease.c?
 */

int cast_cause_disease(object *op, object *caster, object *spell, int dir) {
    sint16  x,y;
    int	i,  mflags, range, dam_mod, dur_mod;
    object  *walk, *target_head;
    mapstruct *m;

    x = op->x;
    y = op->y;

    /* If casting from a scroll, no direction will be available, so refer to the 
     * direction the player is pointing.
     */
    if (!dir) dir=op->facing;
    if (!dir) return 0;	    /* won't find anything if casting on ourself, so just return */

    /* Calculate these once here */
    range = spell->range + SP_level_range_adjust(caster, spell);
    dam_mod = SP_level_dam_adjust(caster, spell);
    dur_mod = SP_level_duration_adjust(caster, spell);

    /* search in a line for a victim */
    for(i=1; i<range; i++) {
	x = op->x + i * freearr_x[dir];
	y = op->y + i * freearr_y[dir];
	m = op->map;

	mflags = get_map_flags(m, &m, x, y, &x, &y);

	if (mflags & P_OUT_OF_MAP) return 0;

	/* don't go through walls - presume diseases are airborne */
	if (GET_MAP_MOVE_BLOCK(m, x, y) &  MOVE_FLY_LOW) return 0;

	/* Only bother looking on this space if there is something living here */
	if (mflags & P_IS_ALIVE) {
	    /* search this square for a victim */
        for(walk=get_map_ob(m,x,y);walk;walk=walk->above) {
            /* Flags for monster is set on head only, so get it now */
            target_head = walk;
            while (target_head->head)
                target_head = target_head->head;
            if (QUERY_FLAG(target_head,FLAG_MONSTER) || (target_head->type==PLAYER)) {  /* found a victim */
                object *disease = arch_to_object(spell->other_arch);

                set_owner(disease,op);
                set_spell_skill(op, caster, spell, disease);
                disease->stats.exp = 0;
                disease->level = caster_level(caster, spell);

                /* do level adjustments */
                if(disease->stats.wc)
                disease->stats.wc +=  dur_mod/2;

                if(disease->magic> 0)
                disease->magic += dur_mod/4;

                if(disease->stats.maxhp>0)
                disease->stats.maxhp += dur_mod;

                if(disease->stats.maxgrace>0)
                disease->stats.maxgrace += dur_mod;

                if(disease->stats.dam) {
                if(disease->stats.dam > 0)
                    disease->stats.dam += dam_mod;
                else disease->stats.dam -= dam_mod;
                }

                if(disease->last_sp) {
                disease->last_sp -= 2*dam_mod;
                if(disease->last_sp <1) disease->last_sp = 1;
                }

                if(disease->stats.maxsp) {
                if(disease->stats.maxsp > 0)
                    disease->stats.maxsp += dam_mod;
                else disease->stats.maxsp -= dam_mod;
                }

                if(disease->stats.ac) 
                disease->stats.ac += dam_mod;

                if(disease->last_eat)
                disease->last_eat -= dam_mod;

                if(disease->stats.hp)
                disease->stats.hp -= dam_mod;

                if(disease->stats.sp)
                disease->stats.sp -= dam_mod;

                if(infect_object(target_head,disease,1)) {
                object *flash;  /* visual effect for inflicting disease */

                draw_ext_info_format(NDI_UNIQUE, 0, op, 
				     MSG_TYPE_SPELL, MSG_TYPE_SPELL_SUCCESS,
				     "You inflict %s on %s!",
				     disease->name,target_head->name);

                free_object(disease); /* don't need this one anymore */
                flash=create_archetype(ARCH_DETECT_MAGIC);
                flash->x = x;
                flash->y = y;
                flash->map = walk->map;
                insert_ob_in_map(flash,walk->map,op,0);
                return 1;
            	}
            	free_object(disease);
            } /* Found a victim */
        } /* Search squares for living creature */
	} /* if living creature on square */
    } /* for range of spaces */
    draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		  "No one caught anything!", NULL);
    return 1;
}
