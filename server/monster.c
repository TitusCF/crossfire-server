/*
 * static char *rcsid_monster_c =
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
#include <spells.h>
#include <skills.h>
#endif

/*
 * When parsing a message-struct, the msglang struct is used
 * to contain the values.
 * This struct will be expanded as new features are added.
 * When things are stable, it will be parsed only once.
 */

typedef struct _msglang {
  char **messages;	/* An array of messages */
  char ***keywords;	/* For each message, an array of strings to match */
} msglang;


extern spell spells[NROFREALSPELLS];

#define MIN_MON_RADIUS 3 /* minimum monster detection radius */

/* checks npc->enemy and returns that enemy if still valid,
 * NULL otherwise.  This doesn't really get the enemy, so is
 * misnamed - it should really be 'check_enemy'.
 * this is map tile aware.
 */

object *get_enemy(object *npc, rv_vector *rv) {
    if ((npc->move_type & HI4) == PETMOVE) {
	if (npc->owner != NULL)
	    return npc->enemy = npc->owner->enemy;
	else npc->enemy = NULL;
    }
    /* periodically, a monster mayu change its target.  Also, if the object
     * has been destroyed, etc, clear the enemy.
     */
    if(npc->enemy) {
	if (QUERY_FLAG(npc->enemy,FLAG_REMOVED)||QUERY_FLAG(npc->enemy,FLAG_FREED)
	    ||!(RANDOM()%20)|| (npc->enemy->type!=PLAYER&&npc->enemy->type!=GOLEM)||
	    !on_same_map(npc, npc->enemy))
		npc->enemy=NULL;
    }
    return can_detect_enemy(npc,npc->enemy,rv)?npc->enemy:NULL;
}

/* Returns the nearest living creature (monster or generator).
 * Modified to deal with tiled maps properly.
 * Also fixed logic so that monsters in the lower directions were more
 * likely to be skipped - instead of just skipping the 'start' number
 * of direction, revisit them after looking at all the other spaces.
 *
 * Note that being this may skip some number of spaces, it will
 * not necessarily find the nearest living creature - it basically
 * chooses one from within a 3 space radius, and since it skips
 * the first few directions, it could very well choose something 
 * 3 spaces away even though something directly north is closer.
 *
 * this function is map tile aware.
 */
object *find_nearest_living_creature(object *npc) {
    int i,j=0,start;
    int nx,ny;
    mapstruct *m;
    object *tmp;

    start = (RANDOM()%8)+1;
    for(i=start;j<SIZEOFFREE;j++, i=(i+1)%SIZEOFFREE) {
	nx = npc->x + freearr_x[i];
	ny = npc->y + freearr_y[i];
	if (out_of_map(npc->map,nx,ny)) continue;
	m = get_map_from_coord(npc->map, &nx, &ny);

	if (GET_MAP_FLAGS(m, nx, ny) & P_IS_ALIVE) {
	    tmp=get_map_ob(m,nx,ny);
	    while(tmp!=NULL && !QUERY_FLAG(tmp,FLAG_MONSTER)&&
		  !QUERY_FLAG(tmp,FLAG_GENERATOR ) && tmp->type!=PLAYER) 
                tmp=tmp->above;

	    if (!tmp) {
		LOG(llevDebug,"find_nearest_living_creature: map %s (%d,%d) has is_alive set but did not find a mosnter?\n",
		    m->path, nx, ny);
	    }
	    else {
		if(can_see_monsterP(m,nx,ny,i))
		    return tmp;
	    }
	} /* is something living on this space */
    }
    return NULL;  /* nothing found */
}


/* Tries to find an enmy for npc.  We pass the range vector since
 * our caller will find the information useful.
 * Currently, only move_monster calls this function.
 */

object *find_enemy(object *npc, rv_vector *rv) {
    object *tmp=NULL;
  
    if(QUERY_FLAG(npc,FLAG_BERSERK)) {
	return find_nearest_living_creature(npc);
    }
    if ((npc->move_type & HI4) == PETMOVE)
	return get_pet_enemy(npc,rv);

    if((tmp=get_enemy(npc, rv))==NULL) {
	if(!QUERY_FLAG(npc, FLAG_UNAGGRESSIVE)) {
	    tmp = get_nearest_player(npc);
	    if(QUERY_FLAG(npc, FLAG_FRIENDLY)&&tmp)
		tmp = get_enemy(tmp,rv);
	}
    }
    return tmp;
}

/* Sees if this monster should wake up.
 * Currently, this is only called from move_monster, and
 * if enemy is set, then so should be rv.
 */

int check_wakeup(object *op, object *enemy, rv_vector *rv) {
    int radius = op->stats.Wis>MIN_MON_RADIUS?op->stats.Wis:MIN_MON_RADIUS;

    /* Trim work - if no enemy, no need to do anything below */
    if (!enemy) return 0;

    /* blinded monsters can only find nearby objects to attack */
    if(QUERY_FLAG(op, FLAG_BLIND) && !QUERY_FLAG(op, FLAG_SEE_INVISIBLE)) 
	radius = MIN_MON_RADIUS;

    /* This covers the situation where the monster is in the dark 
     * and has an enemy. If the enemy has no carried light (or isnt 
     * glowing!) then the monster has trouble finding the enemy. 
     * Remember we already checked to see if the monster can see in 
     * the dark. */

    else if(op->map&&op->map->darkness>0&&enemy&&!enemy->invisible&&
	    !stand_in_light(enemy)&&(!QUERY_FLAG(op,FLAG_SEE_IN_DARK)||
	    !QUERY_FLAG(op,FLAG_SEE_INVISIBLE))) {
		int dark = radius/(op->map->darkness);
		radius = (dark>MIN_MON_RADIUS)?(dark+1):MIN_MON_RADIUS;
    }
    else if(!QUERY_FLAG(op,FLAG_SLEEP)) return 1;

    /* enemy should already be on this map, so don't really need to check
     * for that.
     */
    if (rv->distance < QUERY_FLAG(enemy, FLAG_STEALTH)?(radius/2)+1:radius) {
	CLEAR_FLAG(op,FLAG_SLEEP);
	return 1;
    }
    return 0;
}

int move_randomly(object *op) {
    int i;

    /* Give up to 15 chances for a monster to move randomly */
    for(i=0;i<15;i++) {
	if(move_object(op,RANDOM()%8+1))
	    return 1;
    }
    return 0;
}

/*
 * Move-monster returns 1 if the object has been freed, otherwise 0.
 */

int move_monster(object *op) {
    int dir, diff;
    object  *owner, *enemy, *part;
    rv_vector	rv;

    /* Monsters not on maps don't do anything.  These monsters are things
     * Like royal guards in city dwellers inventories.
     */
    if (!op->map) return 0;

    enemy= find_enemy(op, &rv);

    if(QUERY_FLAG(op, FLAG_SLEEP)||QUERY_FLAG(op, FLAG_BLIND)
       ||((op->map->darkness>0)&&!QUERY_FLAG(op,FLAG_SEE_IN_DARK)
	  &&!QUERY_FLAG(op,FLAG_SEE_INVISIBLE))) {
	if(!check_wakeup(op,enemy,&rv))
	    return 0;
    }

    /* check if monster pops out of hidden spot */
    if(op->hide) do_hidden_move(op);

    if(op->pick_up)
	monster_check_pickup(op);

    if(op->will_apply)
	monster_apply_below(op); /* Check for items to apply below */

    /*  generate hp, if applicable */
    if(op->stats.Con&&op->stats.hp<op->stats.maxhp) {

	/* last heal is in funny units.  Dividing by speed puts
	 * the regeneration rate on a basis of time instead of
	 * #moves the monster makes.  The scaling by 8 is
	 * to capture 8th's of a hp fraction regens 
	 */

	op->last_heal+= (int)((float)(8*op->stats.Con)/FABS(op->speed));
	op->stats.hp+=op->last_heal/32; /* causes Con/4 hp/tick */
	op->last_heal%=32;

	/* So if the monster has gained enough HP that they are no longer afraid */
	if (QUERY_FLAG(op,FLAG_RUN_AWAY) &&
	    op->stats.hp >= (signed short)(((float)op->run_away/(float)100)*
                        (float)op->stats.maxhp))
	    CLEAR_FLAG(op, FLAG_RUN_AWAY);

	if(op->stats.hp>op->stats.maxhp)
	    op->stats.hp=op->stats.maxhp;
    }

    /* generate sp, if applicable */
    if(op->stats.Pow&&op->stats.sp<op->stats.maxsp) {

	/*  last_sp is in funny units.  Dividing by speed puts
         * the regeneration rate on a basis of time instead of
         * #moves the monster makes.  The scaling by 8 is
         * to capture 8th's of a sp fraction regens 
	 */

	 op->last_sp+= (int)((float)(8*op->stats.Pow)/FABS(op->speed));
	op->stats.sp+=op->last_sp/128;  /* causes Pow/16 sp/tick */
	op->last_sp%=128;
	if(op->stats.sp>op->stats.maxsp)
	    op->stats.sp=op->stats.maxsp;
    }

    if(QUERY_FLAG(op, FLAG_SCARED)&&!(RANDOM()%20))
	CLEAR_FLAG(op,FLAG_SCARED); /* Time to regain some "guts"... */

    /* If we don't have an enemy, do special movement or the like */
    if(!enemy) {
	if(QUERY_FLAG(op, FLAG_ONLY_ATTACK)) {
	    remove_ob(op);
	    free_object(op);
	    return 1;
	}
	if (op->move_type & HI4) {
	    switch (op->move_type & HI4) {
		case (PETMOVE):
		    pet_move (op);
		    break;
		case (CIRCLE1):
		    circ1_move (op);
		    break;
		case (CIRCLE2):
		    circ2_move (op);
		    break;
		case (PACEV):
		    pace_movev(op);
		    break;
		case (PACEH):
		    pace_moveh(op);
		    break;
		case (PACEV2):
		    pace2_movev (op);
		    break;
		case (PACEH2):
		    pace2_moveh (op);
		    break;
		case (RANDO):
		    rand_move (op);
		    break;
		case (RANDO2):
		    move_randomly (op);
		    break;
	    } /* move hi */
	    if(QUERY_FLAG(op, FLAG_FREED))
		return 1;
	    return 0;
	}

	if (QUERY_FLAG(op,FLAG_RANDOM_MOVE))
	    (void) move_randomly(op);
	return 0;
    }

    /* We have an enemy.  Block immediately below is for pets */
    if((op->type&HI4) == PETMOVE && (owner = get_owner(op)) != NULL && !on_same_map(op,owner)) {
	follow_owner(op, owner);
	if(QUERY_FLAG(op, FLAG_REMOVED) && FABS(op->speed) > MIN_ACTIVE_SPEED) {
	    remove_friendly_object(op);
	    free_object(op);
	    return 1;
	}
	return 0;
    }
   
    /* doppleganger code to change monster facing to that of the nearest 
	player */
    if ( (op->race != NULL)&& strcmp(op->race,"doppleganger") == 0){
	op->face = enemy->face; 
	strcpy(op->name,enemy->name);
    }

    for (part=op; part!=NULL; part=part->more) {
	rv_vector   rv1;

	get_rangevector(part, enemy, &rv1, 0x1);
	
	dir=rv1.direction;

	if(QUERY_FLAG(op, FLAG_SCARED) || QUERY_FLAG(op,FLAG_RUN_AWAY))
	    dir=absdir(dir+4);

	if(QUERY_FLAG(op,FLAG_CONFUSED))
	    dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

	if (!QUERY_FLAG(op, FLAG_SCARED)) {
	    if(QUERY_FLAG(op,FLAG_CAST_SPELL))
		if(monster_cast_spell(op,part,enemy,dir,&rv1))
		    return 0;
	    if(QUERY_FLAG(op,FLAG_READY_WAND)&&!(RANDOM()%3))
		if(monster_use_wand(op,part,enemy,dir))
		    return 0;
	    if(QUERY_FLAG(op,FLAG_READY_ROD)&&!(RANDOM()%4))
		if(monster_use_rod(op,part,enemy,dir))
		    return 0;
	    if(QUERY_FLAG(op,FLAG_READY_HORN)&&!(RANDOM()%5))
		if(monster_use_horn(op,part,enemy,dir))
		    return 0;
	    if(QUERY_FLAG(op,FLAG_READY_SKILL)&&!(RANDOM()%3))
		if(monster_use_skill(op,part,enemy,dir))
		    return 0;
	    if(QUERY_FLAG(op,FLAG_READY_BOW)&&!(RANDOM()%2))
		if(monster_use_bow(op,part,enemy,dir))
		    return 0;
	}
    } /* for processing of all parts */

    
    get_rangevector(op, enemy, &rv, 0);
    part = rv.part;
    dir=rv.direction;

    if(QUERY_FLAG(op, FLAG_SCARED) || QUERY_FLAG(op,FLAG_RUN_AWAY))
	dir=absdir(dir+4);

    if(QUERY_FLAG(op,FLAG_CONFUSED))
	dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

    if ((op->move_type & LO4) && !QUERY_FLAG(op, FLAG_SCARED)) {
	switch (op->move_type & LO4) {
	    case DISTATT:
		dir = dist_att (dir,op,enemy,part,&rv);
		break;
	    case RUNATT:
		dir = run_att (dir,op,enemy,part,&rv);
		break;
	    case HITRUN:
		dir = hitrun_att(dir,op,enemy);
		break;
	    case WAITATT:
		dir = wait_att (dir,op,enemy,part,&rv);
		break;
	    case RUSH:
	    case ALLRUN:
		break; 
	    case DISTHIT:
		dir = disthit_att (dir,op,enemy,part,&rv);
		break;
	    case WAIT2:
		dir = wait_att2 (dir,op,enemy,part,&rv);
		break;
	    default:
		LOG(llevDebug,"Illegal low mon-move: %d\n",op->move_type & LO4);
	}
    }
    if (!dir)
	return 0;

    if (!QUERY_FLAG(op,FLAG_STAND_STILL)) {
	if(move_object(op,dir)) /* Can the monster move directly toward player? */
	    return 0;
	if(QUERY_FLAG(op, FLAG_SCARED) || !can_hit(part,enemy,&rv) 
	   || QUERY_FLAG(op,FLAG_RUN_AWAY)) {

	    /* Try move around corners if !close */
	    int maxdiff = (QUERY_FLAG(op, FLAG_ONLY_ATTACK) || RANDOM()&1) ? 1 : 2;
	    for(diff = 1; diff <= maxdiff; diff++) {
		/* try different detours */
		int m = 1-(RANDOM()&2);          /* Try left or right first? */
		if(move_object(op,absdir(dir + diff*m)) ||
		   move_object(op,absdir(dir - diff*m)))
		return 0;
	    }
	}
    } /* if monster is not standing still */

    /*
     * Eneq(@csd.uu.se): Patch to make RUN_AWAY or SCARED monsters move a random
     * direction if they can't move away.
     */
    if (!QUERY_FLAG(op, FLAG_ONLY_ATTACK)&&(QUERY_FLAG(op,FLAG_RUN_AWAY)||QUERY_FLAG(op, FLAG_SCARED)))
	if(move_randomly(op))
	    return 0;

    /*
     * Monster can't move...now see if it can hit the player...
     *    Eneq(@csd.uu.se): Added check to handle RUN_AWAY and berzerk attack from
     *    RUN_AWAY, locked in monster.
     */
    if (!QUERY_FLAG(op, FLAG_FRIENDLY) && enemy == op->enemy) {
	object *nearest_player = get_nearest_player(op);
	if (nearest_player && nearest_player != enemy && !can_hit(part,enemy,&rv)) {
	    op->enemy = NULL;
	    enemy = nearest_player;
	}
    }

    if(!QUERY_FLAG(op, FLAG_SCARED)&&can_hit(part,enemy,&rv)) {
	if(QUERY_FLAG(op,FLAG_RUN_AWAY)) {
	    signed char tmp = (signed char)((float)part->stats.wc*(float)2);
	    part->stats.wc+=tmp;
	    (void)skill_attack(enemy,part,0,NULL);
	    part->stats.wc-=tmp;
	} else
	    (void)skill_attack(enemy,part,0,NULL);
    } /* if monster is in attack range */

    if(QUERY_FLAG(part,FLAG_FREED))    /* Might be freed by ghost-attack or hit-back */
	return 1;
    if(QUERY_FLAG(op, FLAG_ONLY_ATTACK)) {
	remove_ob(op);
	free_object(op);
	return 1;
    }
    return 0;
}

int can_hit(object *ob1,object *ob2, rv_vector *rv) {
    if(QUERY_FLAG(ob1,FLAG_CONFUSED)&&!(RANDOM()%3))
	return 0;
    return abs(rv->distance_x)<2&&abs(rv->distance_y)<2;
}

/*Someday we may need this check */
int can_apply(object *who,object *item) {
  return 1;
}

#define MAX_KNOWN_SPELLS 20

object *monster_choose_random_spell(object *monster) {
  object *altern[MAX_KNOWN_SPELLS];
  object *tmp;
  spell *spell;
  int i=0,j;

  for(tmp=monster->inv;tmp!=NULL;tmp=tmp->below)
    if(tmp->type==ABILITY||tmp->type==SPELLBOOK) {
	 /*  Check and see if it's actually a useful spell */
		if((spell=find_spell(tmp->stats.sp))!=NULL 
			&&!(spell->path&(PATH_INFO|PATH_TRANSMUTE|PATH_TRANSFER|PATH_LIGHT))) {
			 if(tmp->stats.maxsp)
				for(j=0;i<MAX_KNOWN_SPELLS&&j<tmp->stats.maxsp;j++)
				  altern[i++]=tmp;
			 else
				altern[i++]=tmp;
			 if(i==MAX_KNOWN_SPELLS)
				break;
		}
	
	 }
  if(!i)
    return NULL;
  return altern[RANDOM()%i];
}

int monster_cast_spell(object *head, object *part,object *pl,int dir, rv_vector *rv) {
    object *spell_item;
    spell *sp;
    int sp_typ, ability;
    object *owner;
    rv_vector	rv1;

    if(!(RANDOM()%3)) /* Don't want to cast spells so often */
	return 0;

    /* If you want monsters to cast spells over friends, this spell should
     * be removed.  It probably should be in most cases, since monsters still
     * don't care about residual effects (ie, casting a cone which may have a 
     * clear path to the player, the side aspects of the code will still hit
     * other monsters)
     */
    if(!(dir=path_to_player(part,pl,0)))
	return 0;

    if(QUERY_FLAG(head,FLAG_FRIENDLY) && (owner = get_owner(head)) != NULL) {
	get_rangevector(head, owner, &rv1, 0x1);
	if(dirdiff(dir,rv1.direction) < 2)
	    return 0; /* Might hit owner with spell */
    }
    if(QUERY_FLAG(head,FLAG_CONFUSED))
	dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

    /*  If the monster hasn't already chosen a spell, choose one */
    if(head->spellitem==NULL) {
	if((spell_item=monster_choose_random_spell(head))==NULL) {
	    LOG(llevMonster,"Turned off spells in %s\n",head->name);
	    CLEAR_FLAG(head, FLAG_CAST_SPELL); /* Will be turned on when picking up book */
	    return 0;
	}
    }
    else
	spell_item=head->spellitem; 

    if(spell_item->stats.hp) {
	/* Alternate long-range spell: check how far away enemy is */
	if(rv->distance>6)
	    sp_typ=spell_item->stats.hp;
	else
	    sp_typ=spell_item->stats.sp;
    } else
	sp_typ=spell_item->stats.sp;

    if((sp=find_spell(sp_typ))==NULL) {
	LOG(llevError,"Warning: Couldn't find spell in item.\n");
	return 0;
    }
    if (sp->onself) /* Spell should be cast on caster (ie, heal, strength) */
	dir = 0;
  
    if(head->stats.sp<SP_level_spellpoint_cost(head,head,sp_typ))
	/* Monster doesn't have enough spell-points */
	return 0;

    ability = (spell_item->type==ABILITY && !(spell_item->attacktype&AT_MAGIC));


    /* If we cast a spell, only use up casting_time speed */
    head->speed_left+=1.0 - spells[sp_typ].time/20.0*FABS(head->speed); 

    head->stats.sp-=SP_level_spellpoint_cost(head,head,sp_typ);
    /* choose the spell the monster will cast next */
    /* choose the next spell to cast */
    head->spellitem = monster_choose_random_spell(head);  
    return cast_spell(part,part,dir,sp_typ,ability, spellNormal,NULL);
}

/* monster_use_skill()-implemented 95-04-28 to allow monster skill use.
 * Note that monsters do not need the skills SK_MELEE_WEAPON and
 * SK_MISSILE_WEAPON to make those respective attacks, if we
 * required that we would drastically increase the memory
 * requirements of CF!! 
 *
 * The skills we are treating here are all but those. -b.t. 
 */  

int monster_use_skill(object *head, object *part, object *pl,int dir) {
object *skill, *owner;

  if(!(dir=path_to_player(part,pl,0)))
    return 0;
  if(QUERY_FLAG(head,FLAG_FRIENDLY) && (owner = get_owner(head)) != NULL) {
    int dir2 = find_dir_2(head->x-owner->x, head->y-owner->y);
    if(dirdiff(dir,dir2) < 1)
      return 0; /* Might hit owner with skill -thrown rocks for example ?*/
  }
  if(QUERY_FLAG(head,FLAG_CONFUSED))
    dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

  /* skill selection - monster will use the next unused skill.
   * well...the following scenario will allow the monster to 
   * toggle between 2 skills. One day it would be nice to make
   * more skills available to monsters.  
   */
 
  for(skill=head->inv;skill!=NULL;skill=skill->below)
    if(skill->type==SKILL && skill!=head->chosen_skill) { 
        head->chosen_skill=skill; 
        break;
    }

  if(!skill && !head->chosen_skill) {
    LOG(llevDebug,"Error: Monster %s (%d) has FLAG_READY_SKILL without skill.\n",
        head->name,head->count);
    CLEAR_FLAG(head, FLAG_READY_SKILL);
    return 0;
  }
/* use skill */
  return do_skill(head,dir,NULL);
}


/* For the future: Move this function together with case 3: in fire() */

int monster_use_wand(object *head,object *part,object *pl,int dir) {
  object *wand, *owner;
  if(!(dir=path_to_player(part,pl,0)))
    return 0;
  if(QUERY_FLAG(head,FLAG_FRIENDLY) && (owner = get_owner(head)) != NULL) {
    int dir2 = find_dir_2(head->x-owner->x, head->y-owner->y);
    if(dirdiff(dir,dir2) < 2)
      return 0; /* Might hit owner with spell */
  }
  if(QUERY_FLAG(head,FLAG_CONFUSED))
    dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);
  for(wand=head->inv;wand!=NULL;wand=wand->below)
    if(wand->type==WAND&&QUERY_FLAG(wand,FLAG_APPLIED))
      break;
  if(wand==NULL) {
    LOG(llevError,"Error: Monster %s (%d) HAS_READY_WAND() without wand.\n",
            head->name,head->count);
    CLEAR_FLAG(head, FLAG_READY_WAND);
    return 0;
  }
  if(wand->stats.food<=0) {
    manual_apply(head,wand,0);
    CLEAR_FLAG(head, FLAG_READY_WAND);
    if (wand->arch) {
      CLEAR_FLAG(wand, FLAG_ANIMATE);
      wand->face = wand->arch->clone.face;
      wand->speed = 0;
      update_ob_speed(wand);
    }
    return 0;
  }
  if(cast_spell(part,wand,dir,wand->stats.sp,0,spellWand,NULL)) {
    wand->stats.food--;
    return 1;
  }
  return 0;
}

int monster_use_rod(object *head,object *part,object *pl,int dir) {
  object *rod, *owner;
  if(!(dir=path_to_player(part,pl,0)))
    return 0;
  if(QUERY_FLAG(head,FLAG_FRIENDLY) && (owner = get_owner(head)) != NULL) {
    int dir2 = find_dir_2(head->x-owner->x, head->y-owner->y);
    if(dirdiff(dir,dir2) < 2)
      return 0; /* Might hit owner with spell */
  }
  if(QUERY_FLAG(head,FLAG_CONFUSED))
    dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);
  for(rod=head->inv;rod!=NULL;rod=rod->below)
    if(rod->type==ROD&&QUERY_FLAG(rod,FLAG_APPLIED))
      break;
  if(rod==NULL) {
    LOG(llevError,"Error: Monster %s (%d) HAS_READY_ROD() without rod.\n",
            head->name,head->count);
    CLEAR_FLAG(head, FLAG_READY_ROD);
    return 0;
  }
  if(rod->stats.hp<spells[rod->stats.sp].sp) {
    return 0; /* Not recharged enough yet */
  }
  if(cast_spell(part,rod,dir,rod->stats.sp,0,spellRod,NULL)) {
    drain_rod_charge(rod);
    return 1;
  }
  return 0;
}

int monster_use_horn(object *head,object *part,object *pl,int dir) {
  object *horn, *owner;
  if(!(dir=path_to_player(part,pl,0)))
    return 0;
  if(QUERY_FLAG(head,FLAG_FRIENDLY) && (owner = get_owner(head)) != NULL) {
    int dir2 = find_dir_2(head->x-owner->x, head->y-owner->y);
    if(dirdiff(dir,dir2) < 2)
      return 0; /* Might hit owner with spell */
  }
  if(QUERY_FLAG(head,FLAG_CONFUSED))
    dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);
  for(horn=head->inv;horn!=NULL;horn=horn->below)
    if(horn->type==ROD&&QUERY_FLAG(horn,FLAG_APPLIED))
      break;
  if(horn==NULL) {
    LOG(llevError,"Error: Monster %s (%d) HAS_READY_HORN() without horn.\n",
            head->name,head->count);
    CLEAR_FLAG(head, FLAG_READY_HORN);
    return 0;
  }
  if(horn->stats.hp<spells[horn->stats.sp].sp) {
    return 0; /* Not recharged enough yet */
  }
  if(cast_spell(part,horn,dir,horn->stats.sp,0,spellHorn,NULL)) {
    drain_rod_charge(horn);
    return 1;
  }
  return 0;
}

int monster_use_bow(object *head, object *part, object *pl, int dir) {
  object *bow, *arrow, *owner;
  int tag;

  if(!(dir=path_to_player(part,pl,0)))
    return 0;
  if(QUERY_FLAG(head,FLAG_CONFUSED))
    dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);
  if(QUERY_FLAG(head,FLAG_FRIENDLY) && (owner = get_owner(head)) != NULL) {
    int dir2 = find_dir_2(head->x-owner->x, head->y-owner->y);
    if(dirdiff(dir,dir2) < 1)
      return 0; /* Might hit owner with spell */
  }
  for(bow=head->inv;bow!=NULL;bow=bow->below)
    if(bow->type==BOW&&QUERY_FLAG(bow,FLAG_APPLIED))
      break;
  if(bow==NULL) {
    LOG(llevError,"Error: Monster %s (%d) HAS_READY_BOW() without bow.\n",
            head->name,head->count);
    CLEAR_FLAG(head, FLAG_READY_BOW);
    return 0;
  }
  if((arrow=find_arrow(head,bow->race)) == NULL) {
    /* Out of arrows */
    manual_apply(head,bow,0);
    CLEAR_FLAG(head, FLAG_READY_BOW);
    return 0;
  }
  arrow=get_split_ob(arrow,1);
  set_owner(arrow,head);
  arrow->direction=dir;
  arrow->x=part->x,arrow->y=part->y;
  arrow->speed = 1;
  update_ob_speed(arrow);
  arrow->speed_left=0;
  SET_ANIMATION(arrow, dir);
  arrow->stats.sp = arrow->stats.wc; /* save original wc and dam */
  arrow->stats.hp = arrow->stats.dam; 
  arrow->stats.dam+= (QUERY_FLAG(bow, FLAG_NO_STRENGTH) ? 0 : head->level)
                     +bow->stats.dam+bow->magic+arrow->magic;
  arrow->stats.wc= head->stats.wc - bow->magic - arrow->magic - 
                   arrow->stats.wc;
  arrow->map=head->map;
  SET_FLAG(arrow, FLAG_FLYING);
  SET_FLAG(arrow, FLAG_FLY_ON);
  SET_FLAG(arrow, FLAG_WALK_ON);
  tag = arrow->count;
  insert_ob_in_map(arrow,head->map,head,0);
  if (!was_destroyed(arrow, tag))
    move_arrow(arrow);
  return 1;
}

int check_good_weapon(object *who, object *item) {
  object *other_weap;
  int prev_dam=who->stats.dam;
  for(other_weap=who->inv;other_weap!=NULL;other_weap=other_weap->below)
    if(other_weap->type==item->type&&QUERY_FLAG(other_weap,FLAG_APPLIED))
      break;
  if(other_weap==NULL) /* No other weapons */
    return 1;
  if (monster_apply_special(who,item,0)) {
    LOG(llevMonster,"Can't wield %s(%d).\n",item->name,item->count);
    return 0;
  }
  if(who->stats.dam < prev_dam && !QUERY_FLAG(other_weap,FLAG_FREED)) {
    /* New weapon was worse.  (Note ^: Could have been freed by merging) */
    if (monster_apply_special(who,other_weap,0))
      LOG(llevMonster,"Can't rewield %s(%d).\n",item->name,item->count);
    return 0;
  }
  return 1;
}

int check_good_armour(object *who, object *item) {
  object *other_armour;
  int prev_ac = who->stats.ac;
  for (other_armour = who->inv; other_armour != NULL;
       other_armour = other_armour->below)
    if (other_armour->type == item->type && QUERY_FLAG(other_armour,FLAG_APPLIED))
      break;
  if (other_armour == NULL) /* No other armour, use the new */
    return 1;
  if (monster_apply_special(who, item,0)) {
    LOG(llevMonster, "Can't take off %s(%d).\n",item->name,item->count);
    return 0;
  }
  if(who->stats.ac < prev_ac && !QUERY_FLAG(other_armour,FLAG_FREED)) {
    /* New armour was worse. *Note ^: Could have been freed by merging) */
    if (monster_apply_special(who, other_armour,0))
      LOG(llevMonster,"Can't rewear %s(%d).\n", item->name, item->count);
    return 0;
  }
  return 1;
}

/*
 * monster_check_pickup(): checks for items that monster can pick up.
 *
 * Vick's (vick@bern.docs.uu.se) fix 921030 for the sweeper blob.
 * Each time the blob passes over some treasure, it will
 * grab it a.s.a.p.
 *
 * Eneq(@csd.uu.se): This can now be defined in the archetypes, added code
 * to handle this.
 *
 * This function was seen be continueing looping at one point (tmp->below
 * became a recursive loop.  It may be better to call monster_check_apply
 * after we pick everything up, since that function may call others which
 * affect stacking on this space.
 */

void monster_check_pickup(object *monster) {
  object *tmp,*next;
  int next_tag;

  for(tmp=monster->below;tmp!=NULL;tmp=next) {
    next=tmp->below;
    if (next) next_tag = next->count;
    if (monster_can_pick(monster,tmp)) {
      remove_ob(tmp);
      tmp = insert_ob_in_ob(tmp,monster);
      (void) monster_check_apply(monster,tmp);
    }
    /* We could try to re-establish the cycling, of the space, but probably
     * not a big deal to just bail out.
     */
    if (next && was_destroyed(next, next_tag)) return;
  }
}

/*
 * monster_can_pick(): If the monster is interested in picking up
 * the item, then return 0.  Otherwise 0.
 * Instead of pick_up, flags for "greed", etc, should be used.
 * I've already utilized flags for bows, wands, rings, etc, etc. -Frank.
 */

int monster_can_pick(object *monster, object *item) {
  int flag=0;
  if(!can_pick(monster,item))
    return 0;
  if(QUERY_FLAG(item,FLAG_UNPAID))
    return 0;
  if (monster->pick_up&64)           /* All */
    flag=1;
  else switch(item->type) {
  case MONEY:
  case GEM:
    flag=monster->pick_up&2;
    break;
  case FOOD:
    flag=monster->pick_up&4;
    break;
  case WEAPON:
    flag=(monster->pick_up&8)||QUERY_FLAG(monster,FLAG_USE_WEAPON);
    break;
  case ARMOUR:
  case SHIELD:
  case HELMET:
  case BOOTS:
  case GLOVES:
  case GIRDLE:
    flag=(monster->pick_up&16)||QUERY_FLAG(monster,FLAG_USE_ARMOUR);
    break;
  case SKILL:
    flag=QUERY_FLAG(monster,FLAG_CAN_USE_SKILL);
    break;
  case RING:
    flag=QUERY_FLAG(monster,FLAG_USE_RING);
    break;
  case WAND:
    flag=QUERY_FLAG(monster,FLAG_USE_WAND);
    break;
  case SPELLBOOK:
    flag=(monster->arch!=NULL&&QUERY_FLAG((&monster->arch->clone),FLAG_CAST_SPELL));
    break;
  case BOW:
  case ARROW:
    flag=QUERY_FLAG(monster,FLAG_USE_BOW);
    break;
  }
  if (((!(monster->pick_up&32))&&flag) || ((monster->pick_up&32)&&(!flag)))
    return 1;
  return 0;
}

/*
 * monster_apply_below():
 * Vick's (vick@bern.docs.uu.se) @921107 -> If a monster who's
 * eager to apply things, encounters something apply-able,
 * then make him apply it
 */

void monster_apply_below(object *monster) {
  object *tmp, *next;

  for(tmp=monster->below;tmp!=NULL;tmp=next) {
    next=tmp->below;
    switch (tmp->type) {
    case CF_HANDLE:
    case TRIGGER:
      if (monster->will_apply&1)
        manual_apply(monster,tmp,0);
      break;
    case TREASURE:
      if (monster->will_apply&2)
        manual_apply(monster,tmp,0);
      break;
    case SCROLL:  /* Ideally, they should wait until they meet a player */
      if (QUERY_FLAG(monster,FLAG_USE_SCROLL))
        manual_apply(monster,tmp,0); 
      break;
    }
    if (QUERY_FLAG (tmp, FLAG_IS_FLOOR))
        break;
  }
}

/*
 * monster_check_apply() is meant to be called after an item is
 * inserted in a monster.
 * If an item becomes outdated (monster found a better item),
 * a pointer to that object is returned, so it can be dropped.
 * (so that other monsters can pick it up and use it)
 */

/* Sept 96, fixed this so skills will be readied -b.t.*/

void monster_check_apply(object *mon, object *item) {

  if(item->type==SPELLBOOK&&
     mon->arch!=NULL&&(QUERY_FLAG((&mon->arch->clone),FLAG_CAST_SPELL))) {
    SET_FLAG(mon, FLAG_CAST_SPELL);
    return;
  }
  if(QUERY_FLAG(mon,FLAG_USE_BOW) && item->type==ARROW)
  { /* Check for the right kind of bow */
    object *bow;
    for(bow=mon->inv;bow!=NULL;bow=bow->below)
      if(bow->type==BOW && bow->race==item->race) {
        SET_FLAG(mon, FLAG_READY_BOW);
        LOG(llevMonster,"Found correct bow for arrows.\n");
        if(!QUERY_FLAG(bow, FLAG_APPLIED))
          manual_apply(mon,bow,0);
        break;
      }
  }
/* Mol: (mol@meryl.csd.uu.se) If can_apply <number> is defined in the objects
   archetype it can apply the object. See global.h for more info. */
  if (can_apply(mon,item)) {
    int flag=0;
    if (mon->can_apply&64)         /* All */
        flag=1;
    else switch(item->type) {
    case TREASURE:
      flag=0;
    break;
    case POTION:
      flag=mon->can_apply&2;
      break;
    case FOOD: /* Can a monster eat food ?  Yes! (it heals) */
      flag=mon->can_apply&4;
      break;
    case WEAPON:
/*
 * Apply only if it's a better weapon than the used one.
 * All "standard" monsters need to adjust their wc to use the can_apply on
 * weapons.
 */
      flag=((mon->can_apply&8)||QUERY_FLAG(mon,FLAG_USE_WEAPON))&&
            check_good_weapon(mon,item);
      break;
    case ARMOUR:
    case HELMET:
    case SHIELD:
      flag=((mon->can_apply&16)||QUERY_FLAG(mon,FLAG_USE_ARMOUR))&&
            check_good_armour(mon,item);
      break;
    case SKILL:
      if((flag=QUERY_FLAG(mon,FLAG_CAN_USE_SKILL))) {
        if(!QUERY_FLAG(item,FLAG_APPLIED)) manual_apply(mon,item,0);
        if (item->type==SKILL&&present_in_ob(SKILL,mon)!=NULL)
	  SET_FLAG(mon, FLAG_READY_SKILL);
      }
      break;
    case RING:
      flag=QUERY_FLAG(mon,FLAG_USE_RING);
      break;
    case WAND:
      flag=QUERY_FLAG(mon,FLAG_USE_WAND);
      break;
    case BOW:
      flag=QUERY_FLAG(mon,FLAG_USE_BOW);
    }
    if (((!(mon->can_apply&32))&&flag) ||((mon->can_apply&32)&&(!flag))) {
        /* &32 reverses behaviour. See global.h */
        if(!QUERY_FLAG(item,FLAG_APPLIED))
          manual_apply(mon,item,0);
        if (item->type==BOW&&present_in_ob(item->stats.maxsp,mon)!=NULL)
	  SET_FLAG(mon, FLAG_READY_BOW);
    }
    return;
#if 0
    if(!QUERY_FLAG(item,FLAG_APPLIED))
      return item;
    {
      object *tmp;
      for(tmp=mon->inv;tmp!=NULL;tmp=tmp->below)
        if(tmp!=item&&tmp->type==item->type)
          return tmp;
    }
#endif
  }
  return;
}

void npc_call_help(object *op) {
  int x,y;
  object *npc;

  for(x = -3; x < 4; x++)
    for(y = -3; y < 4; y++) {
      if(out_of_map(op->map,op->x+x,op->y+y))
        continue;
      for(npc = get_map_ob(op->map,op->x+x,op->y+y);npc!=NULL;npc=npc->above)
        if(QUERY_FLAG(npc, FLAG_ALIVE)&&QUERY_FLAG(npc, FLAG_UNAGGRESSIVE))
          npc->enemy = op->enemy;
    }
}

int dist_att (int dir , object *ob, object *enemy, object *part, rv_vector *rv) {
  int dist;

    if (can_hit(part,enemy,rv))
	return dir;
    if (rv->distance < 10)
	return absdir(dir+4);
    /* This was 81 below?  That seems outragously far - I'm thinking that was
     * a typo and it shoud be 18
     */
    else if (dist>18) {
	return dir;
    }
    return 0;
}

int run_att (int dir, object *ob, object *enemy,object *part, rv_vector *rv) {

    if ((can_hit (part,enemy,rv) && ob->move_status <20) || ob->move_status <20) {
	ob->move_status++;
	return (dir);
    }
    else if (ob->move_status >20)
	ob->move_status = 0;
    return absdir (dir+4);
}

int hitrun_att (int dir, object *ob,object *enemy) {
    if (ob->move_status ++ < 25)  
	return dir;
    else if (ob->move_status <50) 
	return absdir (dir+4); 
    else 
	ob->move_status = 0;
    return absdir(dir+4);
}

int wait_att (int dir, object *ob,object *enemy,object *part,rv_vector *rv) {

    int inrange = can_hit (part, enemy,rv);
      
    if (ob->move_status || inrange)
	ob->move_status++;

    if (ob->move_status == 0)
	return 0;
    else if (ob->move_status <10)
	return dir;
    else if (ob->move_status <15)
	return absdir(dir+4);
    ob->move_status = 0;
    return 0;
}

int disthit_att (int dir, object *ob, object *enemy, object *part,rv_vector *rv) {

    /* The logic below here looked plain wrong before.  Basically, what should
     * happen is that if the creatures hp percentage falls below run_away,
     * the creature should run away (dir+4)
     * I think its wrong for a creature to have a zero maxhp value, but
     * at least one map has this set, and whatever the map contains, the
     * server should try to be resilant enough to avoid the problem
     */
    if (ob->stats.maxhp && (ob->stats.hp*100)/ob->stats.maxhp < ob->run_away)
	return absdir(dir+4);
    return dist_att (dir,ob,enemy,part,rv);
}

int wait_att2 (int dir, object *ob,object *enemy,object *part, rv_vector *rv) {
    if (rv->distance < 9)
	return absdir (dir+4);
    return 0;
}

void circ1_move (object *ob) {
  static int circle [12] = {3,3,4,5,5,6,7,7,8,1,1,2};
  if(++ob->move_status > 11)
    ob->move_status = 0;
  if (!(move_object(ob,circle[ob->move_status])))
    (void) move_object(ob,RANDOM()%8+1);
}

void circ2_move (object *ob) {
  static int circle[20] = {3,3,3,4,4,5,5,5,6,6,7,7,7,8,8,1,1,1,2,2};
  if(++ob->move_status > 19)
    ob->move_status = 0;
  if(!(move_object(ob,circle[ob->move_status])))
    (void) move_object(ob,RANDOM()%8+1);
}

void pace_movev(object *ob) {
  if (ob->move_status++ > 6)
    ob->move_status = 0;
  if (ob->move_status < 4)
    (void) move_object (ob,5);
  else
    (void) move_object(ob,1);
}

void pace_moveh (object *ob) {
  if (ob->move_status++ > 6)
    ob->move_status = 0;
  if (ob->move_status < 4)
    (void) move_object(ob,3);
  else
    (void) move_object(ob,7);
}

void pace2_movev (object *ob) {
  if (ob->move_status ++ > 16)
    ob->move_status = 0;
  if (ob->move_status <6)
    (void) move_object (ob,5);
  else if (ob->move_status < 8)
    return;
  else if (ob->move_status <13)
    (void) move_object (ob,1);
  else return;
}       

void pace2_moveh (object *ob) {
  if (ob->move_status ++ > 16)
    ob->move_status = 0;
  if (ob->move_status <6)
    (void) move_object (ob,3);
  else if (ob->move_status < 8)
    return;
  else if (ob->move_status <13)
    (void) move_object (ob,7);
  else return;
}       

void rand_move (object *ob) {
  int i;
  if (ob->move_status <1 || ob->move_status >8 ||
      !(move_object(ob,ob->move_status|| ! (RANDOM()% 9))))
    for (i = 0; i < 5; i++)
      if (move_object(ob,ob->move_status = RANDOM()%8+1))
        return;
}

void check_earthwalls(object *op, int x, int y) {
  object *tmp;
  tmp = get_map_ob(op->map, x, y);
  if (tmp!= NULL)
    while(tmp->above != NULL)
      tmp=tmp->above;
  if (tmp!= NULL && tmp->type == EARTHWALL)
    hit_player(tmp,op->stats.dam,op,AT_PHYSICAL);
}

void check_doors(object *op, int x, int y) {
  object *tmp;
  tmp = get_map_ob(op->map, x, y);
  if (tmp!= NULL)
    while(tmp->above != NULL)
      tmp=tmp->above;
  if (tmp!= NULL && tmp->type == DOOR)
    hit_player(tmp,1000,op,AT_PHYSICAL);
}

/*
 * move_object() tries to move object op in the direction "dir".
 * If it fails (something blocks the passage), it returns 0,
 * otherwise 1.
 * This is an improvement from the previous move_ob(), which
 * removed and inserted objects even if they were unable to move.
 */

int move_object(object *op, int dir) {
    int newx = op->x+freearr_x[dir];
    int newy = op->y+freearr_y[dir];
    object *tmp;

    /* 0.94.2 - we need to set the direction for the new animation code.
     * it uses it to figure out face to use - I can't see it
     * breaking anything, but it might.
     */
    op->direction = dir;
    if(blocked_link(op, newx, newy)) /* Not all features from blocked_two yet */
	return 0;                      /* (Not efficient enough yet) */
    if(op->more != NULL && !move_object(op->more, dir))
	return 0;
    if(op->will_apply&4)
	check_earthwalls(op,newx,newy);
    if(op->will_apply&8)
	check_doors(op,newx,newy);

    /* 0.94.1 - I got a stack trace that showed it crash with remove_ob trying
     * to remove a removed object, and this function was the culprit.  A possible
     * guess I have is that check_doors above ran into a trap, killing the
     * monster.
     *
     * Unfortunately, it doesn't appear that the calling functions of move_object
     * deal very well with op being killed, so all this might do is just
     * migrate the problem someplace else.
     */

    if (QUERY_FLAG(op, FLAG_REMOVED)) {
	LOG(llevDebug,"move_object: monster has been removed - will not process further\n");
	/* Was not successful, but don't want to try and move again */
	return 1;
    }
    if(op->head)
	return 1;

    remove_ob(op);

    for(tmp = op; tmp != NULL; tmp = tmp->more)
	tmp->x+=freearr_x[dir], tmp->y+=freearr_y[dir];

    insert_ob_in_map(op, op->map, op,0);
    return 1;
}

static void free_messages(msglang *msgs) {
  int messages, keywords;

  if (!msgs)
    return;
  for(messages = 0; msgs->messages[messages]; messages++) {
    if(msgs->keywords[messages]) {
      for(keywords = 0; msgs->keywords[messages][keywords]; keywords++)
        free(msgs->keywords[messages][keywords]);
      free(msgs->keywords[messages]);
    }
    free(msgs->messages[messages]);
  }
  free(msgs->messages);
  free(msgs->keywords);
  free(msgs);
}

static msglang *parse_message(char *msg) {
  msglang *msgs;
  int nrofmsgs, msgnr, i;
  char *cp, *line, *last;
  char *buf = strdup_local(msg);

  /* First find out how many messages there are.  A @ for each. */
  for (nrofmsgs = 0, cp = buf; *cp; cp++)
    if (*cp == '@')
      nrofmsgs++;
  if (!nrofmsgs)
    return NULL;

  msgs = (msglang *) malloc(sizeof(msglang));
  msgs->messages = (char **) malloc(sizeof(char *) * (nrofmsgs + 1));
  msgs->keywords = (char ***) malloc(sizeof(char **) * (nrofmsgs + 1));
  for(i=0; i<=nrofmsgs; i++) {
    msgs->messages[i] = NULL;
    msgs->keywords[i] = NULL;
  }

  for (last = NULL, cp = buf, msgnr = 0;*cp; cp++)
    if (*cp == '@') {
      int nrofkeywords, keywordnr;
      *cp = '\0'; cp++;
      if(last != NULL)
        msgs->messages[msgnr++] = strdup_local(last);
      if(strncmp(cp,"match",5)) {
        LOG(llevError,"Unsupported command in message.\n");
        free(buf);
        return NULL;
      }
      for(line = cp + 6, nrofkeywords = 0; *line != '\n' && *line; line++)
        if(*line == '|')
          nrofkeywords++;
      if(line > cp + 6)
        nrofkeywords++;
      if(nrofkeywords < 1) {
        LOG(llevError,"Too few keywords in message.\n");
        free(buf);
        free_messages(msgs);
        return NULL;
      }
      msgs->keywords[msgnr] = (char **) malloc(sizeof(char **) * (nrofkeywords +1));
      msgs->keywords[msgnr][nrofkeywords] = NULL;
      last = cp + 6;
      cp = strchr(cp,'\n');
      if(cp != NULL)
        cp++;
      for(line = last, keywordnr = 0;line<cp && *line;line++)
        if(*line == '\n' || *line == '|') {
          *line = '\0';
          if (last != line)
            msgs->keywords[msgnr][keywordnr++] = strdup_local(last);
	  else {
	        if (keywordnr<nrofkeywords)
		{
		   /* Whoops, Either got || or |\n in @match. Not good */
		   msgs->keywords[msgnr][keywordnr++] = strdup_local("xxxx");
		   /* We need to set the string to something sensible to    *
		    * prevent crashes later. Unfortunately, we can't set to *
		    * NULL, as that's used to terminate the for loop in     *
		    * talk_to_npc.  Using xxxx should also help map         *
		    * developers track down the problem cases.              */
		   LOG (llevError, "Tried to set a zero length message in parse_message\n");
		   /* I think this is a error worth reporting at a reasonably *
		    * high level. When logging gets redone, this should       *
		    * be something like MAP_ERROR, or whatever gets put in    *
		    * place. */
		   if (keywordnr>1)
			   /* This is purely addtional information, should *
			    * only be gieb if asked */
			   LOG(llevDebug, "Msgnr %d, after keyword %s\n",msgnr+1,msgs->keywords[msgnr][keywordnr-2]);
		   else
			   LOG(llevDebug, "Msgnr %d, first keyword\n",msgnr+1);
		}
	  }
          last = line + 1;
        }
      last = cp;
    }
  if(last != NULL)
    msgs->messages[msgnr++] = strdup_local(last);
  free(buf);
  return msgs;
}

static void dump_messages(msglang *msgs) {
  int messages, keywords;
  for(messages = 0; msgs->messages[messages]; messages++) {
    LOG(llevDebug, "@match ");
    for(keywords = 0; msgs->keywords[messages][keywords]; keywords++)
      LOG(llevDebug, "%s ",msgs->keywords[messages][keywords]);
    LOG(llevDebug, "\n%s\n",msgs->messages[messages]);
  }
}

void communicate(object *op, char *txt) {
  object *npc;
  int i;
  int flag=1; /*hasn't spoken to a NPC yet*/
  for(i = 0; i <= SIZEOFFREE2; i++)
    if (!out_of_map(op->map, op->x+freearr_x[i], op->y+freearr_y[i]))
      for(npc = get_map_ob(op->map,op->x+freearr_x[i],op->y+freearr_y[i]);
          npc != NULL; npc = npc->above) {
        if (npc->type == MAGIC_EAR)
          (void) talk_to_wall(npc, txt); /* Maybe exit after 1. success? */
        else if(flag)  {
          if (talk_to_npc(op, npc,txt))
            flag=0; /* Can be crowded */
	}
      }
}

int talk_to_npc(object *op, object *npc, char *txt) {
  msglang *msgs;
  int i,j;
  object *cobj;
#ifdef PLUGINS
  /* GROS: Handle for plugin say event */
  if(npc->event_hook[EVENT_SAY] != NULL)
  {
    CFParm CFP;
    int k, l, m;
    k = EVENT_SAY;
    l = SCRIPT_FIX_ALL;
    m = 0;
    CFP.Value[0] = &k;
    CFP.Value[1] = op;
    CFP.Value[2] = npc;
    CFP.Value[3] = NULL;
    CFP.Value[4] = txt;
    CFP.Value[5] = &m;
    CFP.Value[6] = &m;
    CFP.Value[7] = &m;
    CFP.Value[8] = &l;
    CFP.Value[9] = npc->event_hook[k];
    CFP.Value[10]= npc->event_options[k];
    if (findPlugin(npc->event_plugin[k])>=0)
    {
        ((PlugList[findPlugin(npc->event_plugin[k])].eventfunc) (&CFP));
        return 0;
    }
  }
  /* GROS - Here we let the objects inside inventories hear and answer, too. */
  /* This allows the existence of "intelligent" weapons you can discuss with */
  for(cobj=npc->inv;cobj!=NULL;)
  {
    if(cobj->event_hook[EVENT_SAY] != NULL)
    {
      CFParm CFP;
      int k, l, m;
      k = EVENT_SAY;
      l = SCRIPT_FIX_ALL;
      m = 0;
      CFP.Value[0] = &k;
      CFP.Value[1] = op;
      CFP.Value[2] = cobj;
      CFP.Value[3] = NULL;
      CFP.Value[4] = txt;
      CFP.Value[5] = &m;
      CFP.Value[6] = &m;
      CFP.Value[7] = &m;
      CFP.Value[8] = &l;
      CFP.Value[9] = cobj->event_hook[k];
      CFP.Value[10]= cobj->event_options[k];
      if (findPlugin(cobj->event_plugin[k])>=0)
      {
          ((PlugList[findPlugin(cobj->event_plugin[k])].eventfunc) (&CFP));
          return 0;
      }
    }
    cobj = cobj->below;
  }

  /* GROS: Then we parse the target inventory */
  /*if (npc!=NULL)
  for(cobj=npc->inv;cobj!=NULL;cobj=cobj->next)
  {
    printf("Name is %s\n", cobj->name);
  if(npc->event_hook[EVENT_SAY] != NULL)
  {
    CFParm CFP;
    int k, l, m;
    k = EVENT_SAY;
    l = SCRIPT_FIX_ALL;
    m = 0;
    CFP.Value[0] = &k;
    CFP.Value[1] = op;
    CFP.Value[2] = cobj;
    CFP.Value[3] = NULL;
    CFP.Value[4] = txt;
    CFP.Value[5] = &m;
    CFP.Value[6] = &m;
    CFP.Value[7] = &m;
    CFP.Value[8] = &l;
    CFP.Value[9] = cobj->event_hook[k];
    CFP.Value[10]= cobj->event_options[k];
    if (findPlugin(cobj->event_plugin[k])>=0)
    {
        ((PlugList[findPlugin(cobj->event_plugin[k])].eventfunc) (&CFP));
        return 0;
    }
  }
  }*/
#endif
  if(npc->msg == NULL || *npc->msg != '@')
    return 0;
  if((msgs = parse_message(npc->msg)) == NULL)
    return 0;
#if 0 /* Turn this on again when enhancing parse_message() */
  if(debug)
    dump_messages(msgs);
#endif
  for(i=0; msgs->messages[i]; i++)
    for(j=0; msgs->keywords[i][j]; j++)
      if(msgs->keywords[i][j][0] == '*' || re_cmp(txt,msgs->keywords[i][j])) {
        char buf[MAX_BUF];
        sprintf(buf,"The %s says:",query_name(npc));
	new_info_map(NDI_NAVY|NDI_UNIQUE, npc->map,buf);
	new_info_map(NDI_NAVY | NDI_UNIQUE, npc->map, msgs->messages[i]);
        free_messages(msgs);
        return 1;
      }
  free_messages(msgs);
  return 0;
}

int talk_to_wall(object *npc, char *txt) {
  msglang *msgs;
  int i,j;

  if(npc->msg == NULL || *npc->msg != '@')
    return 0;
  if((msgs = parse_message(npc->msg)) == NULL)
    return 0;
  if(settings.debug >= llevDebug)
    dump_messages(msgs);
  for(i=0; msgs->messages[i]; i++)
    for(j=0; msgs->keywords[i][j]; j++)
      if(msgs->keywords[i][j][0] == '*' || re_cmp(txt,msgs->keywords[i][j])) {
        if (msgs->messages[i] && *msgs->messages[i] != 0)
	  new_info_map(NDI_NAVY | NDI_UNIQUE, npc->map,msgs->messages[i]);
        free_messages(msgs);
	use_trigger(npc);
        return 1;
      }
  free_messages(msgs);
  return 0;
}

/* find_mon_throw_ob() - modeled on find_throw_ob */

object *find_mon_throw_ob( object *op ) {
  object *tmp = NULL, *heaviest = NULL;
  int stupid,dumb;

  if(op->head) tmp=op->head; else tmp=op;  
  stupid = tmp->stats.Int<6?1:0;
  dumb=tmp->stats.Int<10?1:0;
   
  if(!op) { /* safety */
    LOG(llevError,"find_mon_throw_ob(): confused! have a NULL thrower!\n");
    return (object *) NULL;
  }

  /* look through the inventory. Smarter creatures will throw
   * thrown weapons first, followed by heaviest objects. */

  for(tmp=op->inv;tmp;tmp=tmp->below) {
    if(!tmp) break;
     /* no throwing your arms,hands, head, etc. or invisible 
      * items like skills. No throwing special keys either! */
    if(tmp->invisible||tmp->type==FLESH||tmp->type==SPECIAL_KEY) continue;
     /* this should prevent us from throwing away
      * cursed items, worn armour, etc. */
    if(QUERY_FLAG(tmp,FLAG_APPLIED)&&
	(QUERY_FLAG(tmp,FLAG_CURSED)||QUERY_FLAG(tmp,FLAG_DAMNED)))
 	  continue;

    /* based on smarts, we decline to throw some stuff */
    if(!dumb) { 
       /* throwing weapons are *always* preferable */ 
      if(QUERY_FLAG(tmp,FLAG_IS_THROWN)&&!QUERY_FLAG(tmp,FLAG_APPLIED)) 
	  break;
       /* toss dirt/dust for blindness/damage */
      if(QUERY_FLAG(tmp,FLAG_DUST)) break;
       /* toss those non-self, non-summoning, spellcasting potions for damage! */
      if(tmp->type==POTION&&tmp->stats.sp&&!IS_SUMMON_SPELL(tmp->stats.sp)
	&&!spells[tmp->stats.sp].onself) break;
      if(QUERY_FLAG(tmp,FLAG_APPLIED)) continue;
      else if(tmp->type==BOW||tmp->type==ARROW||tmp->type==SCROLL
	||tmp->type==BOOK||tmp->type==SPELLBOOK||tmp->type==AMULET
	||tmp->type==MONEY||tmp->type==GEM||tmp->type==SKILL
	||tmp->type==KEY||(tmp->type==CONTAINER&&tmp->inv)
	||(tmp->glow_radius>0&&!QUERY_FLAG(op,FLAG_SEE_IN_DARK)) 
        ||tmp->magic>0||tmp->value>(10*SK_level(op))) 
	  continue;
       /* 'throw' away 'trash'--spent wands and cursed items*/ 
      if((tmp->type==WAND&&tmp->stats.food==0)||QUERY_FLAG(tmp,FLAG_CURSED)
	||QUERY_FLAG(tmp,FLAG_DAMNED))
	  continue;
    } else if(!stupid) { 
      if(QUERY_FLAG(tmp,FLAG_IS_THROWN)) break;
      if(monster_can_pick(op,tmp)) continue;
    } else
      if(tmp->type==FOOD) continue;

    if(!heaviest) heaviest=tmp;
    if(tmp->weight>heaviest->weight) heaviest=tmp;
  }

  if ( ! tmp)
    tmp = heaviest;
  if (tmp && QUERY_FLAG (tmp, FLAG_APPLIED)) {
    if (apply_special (op, tmp, AP_UNAPPLY | AP_NO_MERGE)) {
      LOG (llevError, "BUG: find_mon_throw_ob(): couldn't unapply\n");
      tmp = NULL;
    }
  }

#ifdef DEBUG_THROW
  LOG(llevDebug,"%s chooses to throw: %s (%d)\n",op->name,
	!(tmp)?"(nothing)":query_name(tmp),tmp?tmp->count:-1);
#endif
  return tmp;
}

/* determine if we can 'detect' the enemy. Check for walls blocking the
 * los. Also, just because its hidden/invisible, we may be sensitive/smart 
 * enough (based on Wis & Int) to figure out where the enemy is. -b.t. 
 * modified by MSW to use the get_rangevector so that map tiling works
 * properly.  I also so odd code in place that checked for x distance
 * OR y distance being within some range - that seemed wrong - both should
 * be within the valid range. MSW 2001-08-05
 */

int can_detect_enemy (object *op, object *enemy, rv_vector *rv) {

    /* null detection for any of these condtions always */
    if(!op || !enemy || !op->map || !enemy->map)
	return 0;

    /* If the monster (op) has no way to get to the enemy, do nothing */
    if (!on_same_map(op, enemy))
	return 0;

#if 0
    /* this causes problems, dunno why.. */
    /* are we trying to look through a wall? */ 
    /* probably isn't safe for multipart maps either */
    if(path_to_player(op->head?op->head:op,enemy,0)==0) return 0;
#endif

    get_rangevector(op, enemy, rv, 0);

    /* opponent is unseen? We still have a chance to find them if
     * they are 1) standing in dark square, 2) hidden or 3) low-quality
     * invisible (the basic invisibility spell).
     */
    if(!can_see_enemy(op,enemy)) {
	int radius = MIN_MON_RADIUS;
	/* This is percentage change of being discovered while standing
	 * *adjacent* to the monster */
	int hide_discovery = enemy->hide?op->stats.Int/5:-1;

	/* The rest of this is for monsters. Players are on their own for
	 * finding enemies!
	 */
	if(op->type==PLAYER) return 0;

	/* Quality invisible? Bah, we wont see them w/o SEE_INVISIBLE
	 * flag (which was already checked). Lets get out of here 
	 */
	if(enemy->invisible && (!enemy->contr || !enemy->contr->tmp_invis))
	    return 0;

	/* Determine Detection radii */
	if(!enemy->hide)  /* to detect non-hidden (eg dark/invis enemy) */
	    radius = (op->stats.Wis/5)+1>MIN_MON_RADIUS?(op->stats.Wis/5)+1:MIN_MON_RADIUS;
	else { /* a level/INT/Dex adjustment for hiding */
	    object *sk_hide;
	    int bonus = (op->level/2) + (op->stats.Int/5);

	    if(enemy->type==PLAYER) {
		if((sk_hide = find_skill(enemy,SK_HIDING)))
		    bonus -= sk_hide->level;
		else { 
		    LOG(llevError,"can_detect_enemy() got hidden player w/o hiding skill!");
		    make_visible(enemy);
		    radius=radius<MIN_MON_RADIUS?MIN_MON_RADIUS:radius;
		}
	    }
	    else /* enemy is not a player */
		bonus -= enemy->level;

	    radius += bonus/5;
	    hide_discovery += bonus*5;
	} /* else creature has modifiers for hiding */

	/* Radii stealth adjustment. Only if you are stealthy 
	 * will you be able to sneak up closer to creatures */ 
	if(QUERY_FLAG(enemy,FLAG_STEALTH)) 
	    radius = radius/2, hide_discovery = hide_discovery/3;

	/* Radii adjustment for enemy standing in the dark */ 
	if(op->map->darkness>0 && !stand_in_light(enemy)) {

	    /* on dark maps body heat can help indicate location with infravision.
	     * There was a check for immunity for fire here (to increase radius) -
	     * I'm not positive if that makes sense - something could be immune to fire
	     * but not be any warmer blooded than something else.
	     */
	    if(QUERY_FLAG(op,FLAG_SEE_IN_DARK) && is_true_undead(enemy))
		radius += op->map->darkness/2;
	    else
		radius -= op->map->darkness/2;

	    /* op next to a monster (and not in complete darkness) 
	    * the monster should have a chance to see you. */
	    if(radius<MIN_MON_RADIUS && op->map->darkness<5 && rv->distance<=1)
		radius = MIN_MON_RADIUS;
	} /* if on dark map */

	/* Lets not worry about monsters that have incredible detection
	 * radii, we only need to worry here about things the player can
	 * (potentially) see. 
	 * Increased this from 5 to 13 - with larger map code, things that
	 * far out are visible.  Note that the distance field in the
	 * vector is real distance, so in theory this should be 18 to
	 * find that.
	 */
	if(radius>10) radius = 13;

	/* Enemy in range! Now test for detection */
	if (rv->distance <= radius) {
	    /* ah, we are within range, detected? take cases */
	    if(!enemy->invisible) /* enemy in dark squares... are seen! */
		return 1;
	    else if(enemy->hide||(enemy->contr&&enemy->contr->tmp_invis)) { 
		/* hidden or low-quality invisible */  

		/* There is a a small chance each time we check this function 
		 * that we can detect hidden enemy. This means the longer you stay 
		 * near something, the greater the chance you have of being 
		 * discovered. */
		if(enemy->hide && (rv->distance <= 1) && (RANDOM()%100<=hide_discovery)) {
		    make_visible(enemy);
		    /* inform players of new status */
		    if(enemy->type==PLAYER && player_can_view(enemy,op)) 
			new_draw_info_format(NDI_UNIQUE,0, enemy,
					     "You are discovered by %s!",op->name);
		    return 1; /* detected enemy */ 
		} /* if enemy is hiding */

		/* If the hidden/tmp_invis enemy is nearby we accellerate the time of 
		 * becoming unhidden/visible (ie as finding the enemy is easier)
		 * In order to leave actual discovery (invisible=0 state) to
		 * be handled above (not here) we only decrement so that 
		 * enemy->invisible>1 is preserved. 
		 */
		if((enemy->invisible-=RANDOM()%(op->stats.Int+2))<1) 
		    enemy->invisible=1;
	    } /* enemy is hidding or invisible */

	    /* MESSAGING: SO we didnt find them (wah!), we may warn a 
	     * player that the monster is getting close to discovering them. 
	     *
	     * We only warn the player if: 
	     *   1) player has los to the monster
	     *   2) random value based on player Int value
	     */
	    if(enemy->type==PLAYER 
	       && (RANDOM()%(enemy->stats.Int+10)> MAX_STAT/2)
	       && player_can_view(enemy,op)) { 
		    new_draw_info_format(NDI_UNIQUE,0, enemy, 
					 "You see %s noticing your position.", query_name(op));
	    } /* enemy is a player */
	    return 0;
	} /* creature is withing range */
    } /* if creature can see its enemy */
    /* returning 1 here suggests to me that if the enemy is visible, no matter
     * how far away, the creature can see them.  Is that really what we want?
---------------9     */
    return 1;
}

/* determine if op stands in a lighted square. This is not a very
 * intellegent algorithm. For one thing, we ignore los here, SO it 
 * is possible for a bright light to illuminate a player on the 
 * other side of a wall (!). 
 */

int stand_in_light( object *op) {

    if(!op) return 0;
    if(op->glow_radius) return 1;

    if(op->map) {
	int x, y;

	/* Check the spacs with the max light radius to see if any of them
	 * have lights, and if the light is bright enough to illuminate
	 * this object.  Like the los.c logic, this presumes a square
	 * lighting area.
	 */
	for (x = op->x - MAX_LIGHT_RADII; x< op->x + MAX_LIGHT_RADII; x++) {
	    for (y = op->y - MAX_LIGHT_RADII; y< op->y + MAX_LIGHT_RADII; y++) {
		if (out_of_map(op->map, x, y)) continue;

		if (GET_MAP_LIGHT(op->map, x, y) > MAX(abs(x - op->x), abs(y - op->y))) return 1;
	    }
	}
    }
    return 0;
}

/* assuming no walls/barriers, lets check to see if its *possible* 
 * to see an enemy. Note, "detection" is different from "seeing".
 * See can_detect_enemy() for more details. -b.t.
 */

int can_see_enemy (object *op, object *enemy) {
  object *looker = op->head?op->head:op;

  /* safety */
  if(!looker||!enemy||!QUERY_FLAG(looker,FLAG_ALIVE))
    return 0; 

  /* we dont give a full treatment of xrays here (shorter range than normal,
   * see through walls). Should we change the code elsewhere to make you 
   * blind even if you can xray? */
  if(QUERY_FLAG(looker,FLAG_BLIND)&&
    (!QUERY_FLAG(looker,FLAG_SEE_INVISIBLE)||QUERY_FLAG(looker,FLAG_XRAYS)))
    return 0;

  /* checking for invisible things */
  if(enemy->invisible) {
  
    /* HIDDEN ENEMY. by definition, you can't see hidden stuff! 
     * However,if you carry any source of light, then the hidden
     * creature is seeable (and stupid) */
    if(has_carried_lights(enemy)) { 
      if(enemy->hide) { 
	make_visible(enemy);
        new_draw_info(NDI_UNIQUE,0, enemy,
	  "Your light reveals your hidding spot!");
      }
      return 1;
    } else if (enemy->hide) return 0; 

    /* INVISIBLE ENEMY. */
    if(!QUERY_FLAG(looker,FLAG_SEE_INVISIBLE)
      &&(is_true_undead(looker)==QUERY_FLAG(enemy,FLAG_UNDEAD)))
      return 0;

  } else if(looker->type==PLAYER) /* for players, a (possible) shortcut */
      if(player_can_view(looker,enemy)) return 1;

  /* ENEMY IN DARK MAP. Without infravision, the enemy is not seen 
   * unless they carry a light or stand in light. Darkness doesnt
   * inhibit the undead per se (but we should give their archs
   * CAN_SEE_IN_DARK, this is just a safety  
   * we care about the enemy maps status, not the looker.
   * only relevant for tiled maps, but it is possible that the
   * enemy is on a bright map and the looker on a dark - in that
   * case, the looker can still see the enemy
   */
  if(enemy->map->darkness>0&&!stand_in_light(enemy) 
     &&(!QUERY_FLAG(looker,FLAG_SEE_IN_DARK)||
        !is_true_undead(looker)||!QUERY_FLAG(looker,FLAG_XRAYS)))
    return 0;

  return 1;
}
