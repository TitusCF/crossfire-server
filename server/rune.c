/*
 * static char *rcsid_rune_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2003,2006 Mark Wedel & Crossfire Development Team
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
 * @file server/rune.c
 */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>


#ifndef sqr
#define sqr(x) ((x)*(x))
#endif


/*  peterm:  
 *  write_rune:
 *  op:  rune writer
 *  skop: skill object used for casting this rune
 *  dir:  orientation of rune, direction rune's contained spell will
 *	    be cast in, if applicable
 *  inspell: spell object to put into the rune, can be null if doing
 *    a marking rune.
 *  level:  level of casting of the rune
 *  runename:  name of the rune or message displayed by the rune for
 *		a rune of marking 
 */

int write_rune(object *op,object *caster, object *spell, int dir, const char *runename) { 
    object *tmp, *rune_spell, *rune;
    char buf[MAX_BUF];
    mapstruct *m;
    sint16 nx,ny;

    if(!dir) {
	dir=1;
    } 

    nx=op->x+freearr_x[dir];
    ny=op->y+freearr_y[dir];
    m = op->map;

    if (get_map_flags(m, &m, nx, ny, &nx, &ny)) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
		      "Can't make a rune there!", NULL);
	return 0;
    }
    for(tmp=get_map_ob(m,nx,ny);tmp!=NULL;tmp=tmp->above)
	if(tmp->type==RUNE) break;

    if(tmp){
      draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
		    "You can't write a rune there.", NULL);
      return 0;
    }

    if (spell->other_arch) {
	rune_spell = arch_to_object(spell->other_arch);
    } else {
	/* Player specified spell.  The player has to know the spell, so
	 * lets just look through the players inventory see if they know it
	 * use the item_matched_string for our typical matching method.
	 */
	int bestmatch = 0, ms;

	if (!runename || *runename == 0) {
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
			  "Write a rune of what?", NULL);
	    return 0;
	}

	rune_spell=NULL;
	for (tmp=op->inv; tmp; tmp=tmp->below) {
	    if (tmp->type == SPELL) {
		ms = item_matched_string(op, tmp, runename);
		if (ms > bestmatch) {
		    bestmatch = ms;
		    rune_spell = tmp;
		}
	    }
	}
	if (!rune_spell) {
	    draw_ext_info_format(NDI_UNIQUE, 0, op,
				 MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
				 "You don't know any spell named %s",
				 "You don't know any spell named %s",
				 runename);
	    return 0;
	}
	if (rune_spell->skill != spell->skill) {
	    draw_ext_info_format(NDI_UNIQUE, 0, op,
				 MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
				 "You can't cast %s with %s",
				 "You can't cast %s with %s",
				 rune_spell->name, spell->name);
	    return 0;
	}
	if (caster->path_denied & spell->path_attuned) {
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
				 "%s belongs to a spell path denied to you.",
				 "%s belongs to a spell path denied to you.",
				 rune_spell->name);
	    return 0;
	}
	if (caster_level(caster, rune_spell) < rune_spell->level) {
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
				 "%s is beyond your ability to cast!",
				 "%s is beyond your ability to cast!",
				 rune_spell->name);
	    return 0;
	}
        if (SP_level_spellpoint_cost(caster, rune_spell, SPELL_MANA) >  op->stats.sp) {
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
			  "You don't have enough mana.", NULL);
	    return 0;
	}
        if (SP_level_spellpoint_cost(caster, rune_spell, SPELL_GRACE) >  op->stats.grace) {
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
			  "You don't have enough grace.", NULL);
	    return 0;
	}
	op->stats.grace -= SP_level_spellpoint_cost(caster, rune_spell, SPELL_GRACE);
	op->stats.sp -= SP_level_spellpoint_cost(caster, rune_spell, SPELL_MANA);
    }
    /* already proper rune.  Note this should only be the case if other_arch was set */
    if (rune_spell->type == RUNE) {
	rune = rune_spell;
    } else {
	rune = create_archetype(GENERIC_RUNE);
	sprintf(buf,"You set off a rune of %s\n",rune_spell->name);
	rune->msg=add_string(buf);
	tmp = get_object();
	copy_object(rune_spell, tmp);
	insert_ob_in_ob(tmp, rune);
	if (spell->face != blank_face)
	    rune->face = spell->face;
    }
    rune->level = caster_level(caster, spell);
    rune->stats.Cha = rune->level/2;  /* the invisibility parameter */
    rune->x=nx;
    rune->y=ny;
    rune->map = m;
    rune->direction=dir;  /* where any spell will go upon detonation */
    set_owner(rune,op); /* runes without need no owner */
    set_spell_skill(op, caster, spell, rune);
    insert_ob_in_map(rune,m,op,0);
    return 1;

}



/*  move_rune:  peterm
  comments on runes:
    rune->level	    :	    level at which rune will cast its spell.
    rune->hp	    :	    number of detonations before rune goes away
    rune->msg	    :	    message the rune displays when it goes off
    rune->direction :	    direction it will cast a spell in
    rune->dam	    :	    damage the rune will do if it doesn't cast spells
    rune->attacktype:	    type of damage it does, if not casting spells
    rune->other_arch:       spell in the rune
    rune->Cha       :       how hidden the rune is
    rune->maxhp     :       number of spells the rune casts
*/

void move_rune(object *op) {
    int det=0;
    if(!op->level) {return;}  /* runes of level zero cannot detonate. */
    det=op->invisible;
    if(!(rndm(0, MAX(1,(op->stats.Cha))-1))) {
	op->invisible=0;
	op->speed_left-=1;
    }
    else
	op->invisible=1;
    if(op->invisible!=det)
	update_object(op,UP_OBJ_FACE);
}


/*  peterm: rune_attack
 * function handles those runes which detonate but do not cast spells.  
 */


static void rune_attack(object *op,object *victim)
{
    if(victim) {
         tag_t tag = victim->count;
	 hit_player(victim,op->stats.dam,op,op->attacktype,1);
         if (was_destroyed (victim, tag))
                return;
	 /*  if there's a disease in the needle, put it in the player */
	 if(HAS_RANDOM_ITEMS(op)) create_treasure(op->randomitems,op,0,
		(victim->map?victim->map->difficulty:1),0);
	 if(op->inv && op->inv->type == DISEASE) {
		object *disease=op->inv;
		infect_object(victim, disease, 1);
		remove_ob(disease);
		free_object(disease);
	 }
    }
    else  hit_map(op,0,op->attacktype,1);
}

/*  This function generalizes attacks by runes/traps.  This ought to make
 *  it possible for runes to attack from the inventory, 
 *  it'll spring the trap on the victim.
 */
void spring_trap(object *trap,object *victim)
{
    object *env;
    tag_t trap_tag = trap->count;
    rv_vector rv;
    int i, has_spell;

    /* Prevent recursion */
    if (trap->stats.hp <= 0)
	return;

    if (QUERY_FLAG(trap,FLAG_IS_LINKED))
	  use_trigger(trap);
 
    /* Check if this trap casts a spell */
    has_spell = ((trap->inv && trap->inv->type == SPELL) ||
	(trap->other_arch && trap->other_arch->clone.type == SPELL));

    env = object_get_env_recursive(trap);

    /* If the victim is not next to this trap, and the trap doesn't cast
     * a spell, don't set it off.
     */
    get_rangevector(env, victim, &rv, 0);
    if (rv.distance > 1 && !has_spell) return;
     
    /* Only living objects can trigger runes that don't cast spells, as
     * doing direct damage to a non-living object doesn't work anyway.
     * Typical example is an arrow attacking a door.
     */
    if ( ! QUERY_FLAG (victim, FLAG_ALIVE) && !has_spell)
	return;

    trap->stats.hp--;  /*decrement detcount */

    if(victim && victim->type==PLAYER && trap->msg != NULL)
        draw_ext_info(NDI_UNIQUE, 0,victim,MSG_TYPE_APPLY, MSG_TYPE_APPLY_TRAP,
            trap->msg, trap->msg);

    /*  Flash an image of the trap on the map so the poor sod
     *   knows what hit him.  
     */
    trap_show(trap,env);  

    /* Only if it is a spell do we proceed here */
    if (has_spell) {
	object *spell;

	/* This is necessary if the trap is inside something else */
	remove_ob(trap);
	trap->x=victim->x;
	trap->y=victim->y;
	insert_ob_in_map(trap,victim->map,trap,0);

	if (was_destroyed (trap, trap_tag))
	    return;

	for(i = 0; i < MAX(1, trap->stats.maxhp); i++) {
	    if (trap->inv)
		cast_spell(trap,trap,trap->direction,trap->inv,NULL);
	    else {
		spell = arch_to_object(trap->other_arch);
		cast_spell(trap,trap,trap->direction,spell,NULL);
		free_object(spell);
	    }
	}
    } else {
	rune_attack(trap,victim); 
	if (was_destroyed (trap, trap_tag))
	    return;
    }

    if (trap->stats.hp <= 0) {
	trap->type=SIGN;  /* make the trap impotent */
	trap->stats.food=20;  /* make it stick around until its spells are gone */
	SET_FLAG(trap,FLAG_IS_USED_UP);
    }
}

/* dispel_rune:  by peterm  
 * dispels the target rune, depending on the level of the actor
 * and the level of the rune  risk flag, if true, means that there is
 * a chance that the trap/rune will detonate 
 */
int dispel_rune(object *op,object *caster, object *spell, object *skill, int dir)
{
    object *tmp,*tmp2;
    int searchflag = 1, mflags;
    sint16 x,y;
    mapstruct *m;

    x = op->x+freearr_x[dir];
    y = op->y+freearr_y[dir];
    m = op->map;

    mflags = get_map_flags(m, &m, x, y, &x, &y);

    /* Should we perhaps not allow player to disable traps if a monster/
     * player is standing on top? 
     */
    if (mflags & P_OUT_OF_MAP) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "There's nothing there!", NULL);
	return 0;
    }

    /* This can happen if a player does a 'magic rune of dispel'.  Without
     * any skill, chance of success is zero, and we don't know who to tell
     * (as otherwise we would have a skill pointer).  Plus, trap_disarm()
     * presumes skill is not null and will crash if it is.
     */
    if (!skill) return 0;

    for(tmp=get_map_ob(m,x, y); tmp!=NULL;  tmp=tmp->above)  {
	if(tmp->type==RUNE || tmp->type==TRAP) break;

	/* we could put a probability chance here, but since nothing happens
	 * if you fail, no point on that.  I suppose we could do a level
	 * comparison so low level players can't erase high level players runes.
	 */
	if (tmp->type == SIGN && !strcmp(tmp->arch->name,"rune_mark")) {
	    remove_ob(tmp);
	    free_object(tmp);
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_SUCCESS,
			  "You wipe out the rune of marking!", NULL);
	    return 1;
	}

	/* now search tmp's inventory for traps
	 * This is for chests, where the rune is in the chests inventory.
	 */
	for(tmp2=tmp->inv;tmp2!=NULL;tmp2=tmp2->below) {
	    if(tmp2->type==RUNE || tmp2->type==TRAP) { 
		tmp=tmp2;
		searchflag=0;
		break;
	    }
	}
	if(!searchflag) break;
    }
		
    /* no rune there. */
    if(tmp==NULL) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "There's nothing there!", NULL);
	return 0;
    }
    trap_disarm(op,tmp,0, skill);
    return 1;
	
}

int trap_see(object *op,object *trap) {
    int chance;

    chance = random_roll(0, 99, op, PREFER_HIGH);;
  
    /*  decide if we see the rune or not */
    if((trap->stats.Cha==1) || (chance > MIN(95,MAX(5,((int)((float) (op->map->difficulty 
	+ trap->level + trap->stats.Cha-op->level)/10.0 * 50.0)))))) {
	draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			     "You spot a %s!",
			     "You spot a %s!",
			     trap->name);
	return 1;
    }
    return 0;
}

int trap_show(object *trap, object *where) {
    object *tmp2;

    if(where==NULL) return 0;
    tmp2=create_archetype("runedet");
    tmp2->face=&new_faces[GET_ANIMATION(trap, 0)];
    tmp2->x=where->x;tmp2->y=where->y;tmp2->map=where->map;
    insert_ob_in_map(tmp2,where->map,NULL,0);
    return 1;

}


int trap_disarm(object *disarmer, object *trap, int risk, object *skill) {
    int trapworth;  /* need to compute the experience worth of the trap
                     before we kill it */

    /* this formula awards a more reasonable amount of exp */
    trapworth =  MAX(1,trap->level)  * disarmer->map->difficulty *
	sqr(MAX(trap->stats.dam,trap->inv?trap->inv->level:1)) /
	skill->level;

    if(!(random_roll(0, (MAX(2, MIN(20,trap->level-skill->level
	   +5 - disarmer->stats.Dex/2))-1), disarmer, PREFER_LOW)))
        {
            draw_ext_info_format(NDI_UNIQUE, 0,disarmer, 
			 MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			 "You successfully disarm the %s!",
			 "You successfully disarm the %s!",
			 trap->name);
            destroy_object(trap);
	    /* If it is your own trap, (or any players trap), don't you don't
	     * get exp for it.
	     */
	    if (trap->owner && trap->owner->type!=PLAYER && risk)
		return trapworth;
	    else return 1; /* give minimal exp and say success */
        }
    else
        {
            draw_ext_info_format(NDI_UNIQUE, 0,disarmer, 
				 MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
				 "You fail to disarm the %s.",
				 "You fail to disarm the %s.",
				 trap->name);
	    if(! (random_roll(0, (MAX(2,skill->level-trap->level 
	       + disarmer->stats.Dex/2-6))-1, disarmer, PREFER_LOW)) &&risk) {
		draw_ext_info(NDI_UNIQUE, 0,disarmer, 
			      MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			      "In fact, you set it off!", NULL);
		spring_trap(trap,disarmer);
	    }
            return 0;
        }
}


/*  traps need to be adjusted for the difficulty of the map.  The
 * default traps are too strong for wimpy level 1 players, and 
 * unthreatening to anyone of high level 
 */

void trap_adjust(object *trap, int difficulty) {
    int i;

    /* now we set the trap level to match the difficulty of the level
     * the formula below will give a level from 1 to (2*difficulty) with 
     * a peak probability at difficulty 
     */

    trap->level = rndm(0, difficulty-1) + rndm(0, difficulty-1);
    if(trap->level < 1)
	trap->level = 1;

    /* set the hiddenness of the trap, similar formula to above */
    trap->stats.Cha = rndm(0, 19) + rndm(0, difficulty-1) + rndm(0, difficulty-1);

	if (!trap->other_arch && !trap->inv) {
	/* set the damage of the trap.
	 * we get 0-4 pts of damage per level of difficulty of the map in
	 * the trap
	 */

	trap->stats.dam = 0;
	for(i=0;i<difficulty;i++) 
	    trap->stats.dam+=rndm(0, 4);
    
	/*  the poison trap special case */
	if(trap->attacktype & AT_POISON) {
	    trap->stats.dam = rndm(0, difficulty-1);
	    if(trap->stats.dam < 1)
		trap->stats.dam = 1;
	}

	/*  so we get an appropriate amnt of exp for AT_DEATH traps */
	if(trap->attacktype & AT_DEATH) trap->stats.dam = 127;
    }

}

