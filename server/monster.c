/*
 * static char *rcsid_monster_c =
 *    "$Id$";
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

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#include <spells.h>
#include <skills.h>
#endif

static int can_hit(object *ob1, object *ob2, rv_vector *rv);
static int monster_cast_spell(object *head, object *part,object *pl,int dir, rv_vector *rv);
static int monster_use_scroll(object *head, object *part, object *pl, int dir, rv_vector *rv);
static int monster_use_skill(object *head, object *part, object *pl, int dir);
static int monster_use_range(object *head, object *part, object *pl, int dir);
static int monster_use_bow(object *head, object *part, object *pl, int dir);
static void monster_check_pickup(object *monster);
static int monster_can_pick(object *monster, object *item);
static void monster_apply_below(object *monster);
static int dist_att(int dir, object *ob, object *enemy, object *part, rv_vector *rv);
static int run_att(int dir, object *ob, object *enemy, object *part, rv_vector *rv);
static int hitrun_att(int dir, object *ob, object *enemy);
static int wait_att(int dir, object *ob, object *enemy, object *part, rv_vector *rv);
static int disthit_att(int dir, object *ob, object *enemy, object *part, rv_vector *rv);
static int wait_att2(int dir, object *ob, object *enemy, object *part, rv_vector *rv);
static void circ1_move(object *ob);
static void circ2_move(object *ob);
static void pace_movev(object *ob);
static void pace_moveh(object *ob);
static void pace2_movev(object *ob);
static void pace2_moveh(object *ob);
static void rand_move(object *ob);
static int talk_to_npc(object *op, object *npc, const char *txt);
static int talk_to_wall(object *pl, object *npc, const char *txt);


#define MIN_MON_RADIUS 3 /* minimum monster detection radius */


/* checks npc->enemy and returns that enemy if still valid,
 * NULL otherwise.
 * this is map tile aware.
 * If this returns an enemy, the range vector rv should also be
 * set to sane values.
 */
object *check_enemy(object *npc, rv_vector *rv) {
    
    /* if this is pet, let him attack the same enemy as his owner
     * TODO: when there is no ower enemy, try to find a target,
     * which CAN attack the owner. */
    if ((npc->attack_movement & HI4) == PETMOVE)
    {
	if (npc->owner == NULL)
	    npc->enemy = NULL;
	else if (npc->enemy == NULL)
	    npc->enemy = npc->owner->enemy;
    }

    /* periodically, a monster mayu change its target.  Also, if the object
     * has been destroyed, etc, clear the enemy.
     * TODO: this should be changed, because it invokes to attack forced or 
     * attacked monsters to leave the attacker alone, before it is destroyed 
     */
    /* i had removed the random target leave, this invokes problems with friendly
     * objects, getting attacked and defending herself - they don't try to attack
     * again then but perhaps get attack on and on 
     * If we include a aggravated flag in , we can handle evil vs evil and good vs good
     * too. */

    if(npc->enemy)
    {
	/* I broke these if's apart to better be able to see what
	 * the grouping checks are.  Code is the same.
	 */
	if ( QUERY_FLAG(npc->enemy,FLAG_REMOVED)    ||
	     QUERY_FLAG(npc->enemy,FLAG_FREED)	    || 
	     !on_same_map(npc, npc->enemy)	    ||
	     npc == npc->enemy			    || 
             QUERY_FLAG(npc, FLAG_NEUTRAL)	    || 
	     QUERY_FLAG(npc->enemy, FLAG_NEUTRAL))
		npc->enemy = NULL;

  	else if (QUERY_FLAG(npc, FLAG_FRIENDLY) && (
 		 	(QUERY_FLAG(npc->enemy, FLAG_FRIENDLY) && !(should_arena_attack(npc, npc->owner, npc->enemy))) || 
 		 	((npc->enemy->type == PLAYER) && !(should_arena_attack(npc, npc->owner, npc->enemy))) 
 			 || npc->enemy == npc->owner))
  		 npc->enemy = NULL;
		    
		    
	else if (!QUERY_FLAG(npc, FLAG_FRIENDLY) && 
             (!QUERY_FLAG(npc->enemy, FLAG_FRIENDLY) && npc->enemy->type!=PLAYER))
                npc->enemy=NULL;

	/* I've noticed that pets could sometimes get an arrow as the
	 * target enemy - this code below makes sure the enemy is something
	 * that should be attacked.  My guess is that the arrow hits
	 * the creature/owner, and so the creature then takes that
	 * as the enemy to attack.
	 */
	else if (!QUERY_FLAG(npc->enemy, FLAG_MONSTER) && 
                  !QUERY_FLAG(npc->enemy,FLAG_GENERATOR) && 
		 npc->enemy->type!=PLAYER && npc->enemy->type != GOLEM)
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
    int i,mflags;
    sint16 nx,ny;
    mapstruct *m;
    object *tmp;
    int search_arr[SIZEOFFREE];

    get_search_arr(search_arr);
    for(i=0;i<SIZEOFFREE;i++) {
        /* modified to implement smart searching using search_arr
         * guidance array to determine direction of search order
         */
        nx = npc->x + freearr_x[search_arr[i]];
        ny = npc->y + freearr_y[search_arr[i]];
	m = npc->map;

	mflags = get_map_flags(m, &m, nx, ny, &nx, &ny);
	if (mflags & P_OUT_OF_MAP) continue;

	if (mflags & P_IS_ALIVE) {
	    tmp=get_map_ob(m,nx,ny);
	    while(tmp!=NULL && !QUERY_FLAG(tmp,FLAG_MONSTER)&&
		  !QUERY_FLAG(tmp,FLAG_GENERATOR ) && tmp->type!=PLAYER) 
                tmp=tmp->above;

	    if (!tmp) {
		LOG(llevDebug,"find_nearest_living_creature: map %s (%d,%d) has is_alive set but did not find a monster?\n",
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
 * Fix function so that we always make calls to get_rangevector
 * if we have a valid target - function as not doing so in
 * many cases.
 */

static object *find_enemy(object *npc, rv_vector *rv)
{
    object *attacker, *tmp=NULL;

    attacker = npc->attacked_by; /* save this for later use. This can be a attacker. */
    npc->attacked_by = NULL;     /* always clear the attacker entry */    

    /* if we berserk, we don't care about others - we attack all we can find */
    if(QUERY_FLAG(npc,FLAG_BERSERK)) {
	tmp = find_nearest_living_creature(npc);
	if (tmp) get_rangevector(npc, tmp, rv, 0);
	return tmp;
    }

    /* Here is the main enemy selection.
     * We want this: if there is an enemy, attack him until its not possible or 
     * one of both is dead.
     * If we have no enemy and we are...
     * a monster: try to find a player, a pet or a friendly monster
     * a friendly: only target a monster which is targeting you first or targeting a player
     * a neutral: fight a attacker (but there should be none), then do nothing
     * a pet: attack player enemy or a monster
     */

    /* pet move */
    if ((npc->attack_movement & HI4) == PETMOVE) {
        tmp= get_pet_enemy(npc,rv);
	if (tmp) get_rangevector(npc, tmp, rv, 0);
	return tmp;
    }
    
    /* we check our old enemy. */
    if((tmp=check_enemy(npc, rv))==NULL)
    {
        if(attacker) /* if we have an attacker, check him */
        {
            /* we want be sure this is the right one! */
            if(attacker->count == npc->attacked_by_count)
            {
                /* TODO: thats not finished */
                /* we don't want a fight evil vs evil or good against non evil */

                if( QUERY_FLAG(npc, FLAG_NEUTRAL) || QUERY_FLAG(attacker, FLAG_NEUTRAL) || /* neutral */
                    (QUERY_FLAG(npc, FLAG_FRIENDLY) && QUERY_FLAG(attacker, FLAG_FRIENDLY)) ||
                    (!QUERY_FLAG(npc, FLAG_FRIENDLY) && 
                    (!QUERY_FLAG(attacker, FLAG_FRIENDLY) && attacker->type!=PLAYER)) )        
                         CLEAR_FLAG(npc,FLAG_SLEEP); /* skip it, but lets wakeup */
                else if(on_same_map(npc, attacker)) /* thats the only thing we must know... */
                {
                    CLEAR_FLAG(npc,FLAG_SLEEP); /* well, NOW we really should wake up! */
                    npc->enemy = attacker;
                    return attacker; /* yes, we face our attacker! */
                }
            }
        }
                    
        /* we have no legal enemy or attacker, so we try to target a new one */
        if(!QUERY_FLAG(npc, FLAG_UNAGGRESSIVE) && !QUERY_FLAG(npc, FLAG_FRIENDLY) &&
            !QUERY_FLAG(npc, FLAG_NEUTRAL))
        {
	    npc->enemy = get_nearest_player(npc);
	    if (npc->enemy) 
		tmp = check_enemy(npc,rv);
        }
        
    }

    return tmp;
}

/* Sees if this monster should wake up.
 * Currently, this is only called from move_monster, and
 * if enemy is set, then so should be rv.
 * returns 1 if the monster should wake up, 0 otherwise.
 */

static int check_wakeup(object *op, object *enemy, rv_vector *rv) {
    int radius = op->stats.Wis>MIN_MON_RADIUS?op->stats.Wis:MIN_MON_RADIUS;

    /* Trim work - if no enemy, no need to do anything below */
    if (!enemy) return 0;

    /* blinded monsters can only find nearby objects to attack */
    if(QUERY_FLAG(op, FLAG_BLIND))
	radius = MIN_MON_RADIUS;

    /* This covers the situation where the monster is in the dark 
     * and has an enemy. If the enemy has no carried light (or isnt 
     * glowing!) then the monster has trouble finding the enemy. 
     * Remember we already checked to see if the monster can see in 
     * the dark. */

    else if(op->map && op->map->darkness>0 && enemy && !enemy->invisible &&
	    !stand_in_light(enemy) && 
	    (!QUERY_FLAG(op,FLAG_SEE_IN_DARK) || !QUERY_FLAG(op,FLAG_SEE_INVISIBLE))) {
		int dark = radius/(op->map->darkness);
		radius = (dark>MIN_MON_RADIUS)?(dark+1):MIN_MON_RADIUS;
    }
    else if(!QUERY_FLAG(op,FLAG_SLEEP)) return 1;

    /* enemy should already be on this map, so don't really need to check
     * for that.
     */
    if (rv->distance < (QUERY_FLAG(enemy, FLAG_STEALTH)?(radius/2)+1:radius)) {
	CLEAR_FLAG(op,FLAG_SLEEP);
	return 1;
    }
    return 0;
}

static int move_randomly(object *op) {
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
    object  *owner, *enemy, *part, *oph=op;
    rv_vector	rv;

    /* Monsters not on maps don't do anything.  These monsters are things
     * Like royal guards in city dwellers inventories.
     */
    if (!op->map) return 0;

    /* for target facing, we copy this value here for fast access */
    if(oph->head)           /* force update the head - one arch one pic */
        oph = oph->head;
    
    if (QUERY_FLAG(op, FLAG_NO_ATTACK))  /* we never ever attack */
        enemy = op->enemy = NULL;
    else if((enemy= find_enemy(op, &rv)))
    {
        /* we have an enemy, just tell him we want him dead */
        enemy->attacked_by = op;       /* our ptr */
        enemy->attacked_by_count = op->count; /* our tag */
    }
    
    if ( QUERY_FLAG(op, FLAG_SLEEP) || QUERY_FLAG(op, FLAG_BLIND) ||
       ((op->map->darkness>0) && !QUERY_FLAG(op,FLAG_SEE_IN_DARK) &&
	  !QUERY_FLAG(op,FLAG_SEE_INVISIBLE))) {
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
    if(op->stats.Con > 0 && op->stats.hp < op->stats.maxhp) {

	/* last heal is in funny units.  Dividing by speed puts
	 * the regeneration rate on a basis of time instead of
	 * #moves the monster makes.  The scaling by 8 is
	 * to capture 8th's of a hp fraction regens 
	 *
	 * Cast to sint32 before comparing to maxhp since otherwise an (sint16)
	 * overflow might produce monsters with negative hp.
	 */

	op->last_heal+= (int)((float)(8*op->stats.Con)/FABS(op->speed));
	op->stats.hp = MIN((sint32)op->stats.hp+op->last_heal/32, op->stats.maxhp); /* causes Con/4 hp/tick */
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
    if(op->stats.Pow > 0 && op->stats.sp < op->stats.maxsp) {

	/*  last_sp is in funny units.  Dividing by speed puts
         * the regeneration rate on a basis of time instead of
         * #moves the monster makes.  The scaling by 8 is
         * to capture 8th's of a sp fraction regens 
	 *
	 * Cast to sint32 before comparing to maxhp since otherwise an (sint16)
	 * overflow might produce monsters with negative sp.
	 */

	op->last_sp+= (int)((float)(8*op->stats.Pow)/FABS(op->speed));
	op->stats.sp = MIN(op->stats.sp+op->last_sp/128, op->stats.maxsp);  /* causes Pow/16 sp/tick */
	op->last_sp%=128;
    }

    /* this should probably get modified by many more values.
     * (eg, creatures resistance to fear, level, etc. )
     */
    if(QUERY_FLAG(op, FLAG_SCARED) &&!(RANDOM()%20)) {
	CLEAR_FLAG(op,FLAG_SCARED); /* Time to regain some "guts"... */
    }

    /* If we don't have an enemy, do special movement or the like */
    if(!enemy) {
        if(QUERY_FLAG(op, FLAG_ONLY_ATTACK))  {
            remove_ob(op);
	    free_object(op);
            return 1;
	}

	/* Probably really a bug for a creature to have both
	 * stand still and a movement type set.
	 */
        if(!QUERY_FLAG(op, FLAG_STAND_STILL))  {
            if (op->attack_movement & HI4)
            {
                switch (op->attack_movement & HI4) {
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
		}
		return 0;
	    }
    	    else if (QUERY_FLAG(op,FLAG_RANDOM_MOVE))
                (void) move_randomly(op);

	} /* stand still */
        return 0;
    } /* no enemy */

    /* We have an enemy.  Block immediately below is for pets */
    if((op->attack_movement&HI4) == PETMOVE && (owner = get_owner(op)) != NULL && !on_same_map(op,owner))
    {
	follow_owner(op, owner);
	/* If the pet was unable to follow the owner, free it */
	if(QUERY_FLAG(op, FLAG_REMOVED) && FABS(op->speed) > MIN_ACTIVE_SPEED) {
	    remove_friendly_object(op);
	    free_object(op);
	    return 1;
	}
        return 0;
    }
   
    /* doppleganger code to change monster facing to that of the nearest
     * player.  Hmm.  The code is here, but no monster in the current
     * arch set uses it.
     */
    if ( (op->race != NULL)&& strcmp(op->race,"doppleganger") == 0)
    {
	op->face = enemy->face;
    if (op->name)
        free_string(op->name);
	add_refcount(op->name = enemy->name);
    }

    /* Calculate range information for closest body part - this
     * is used for the 'skill' code, which isn't that smart when
     * it comes to figuring it out - otherwise, giants throw boulders
     * into themselves.
     */
    get_rangevector(op, enemy, &rv, 0);
    if ( op->direction != rv.direction )
    {
    	op->direction = rv.direction;
     	op->facing = op->direction;
	if (op->animation_id)
	    animate_object(op, op->direction);
    }

    /* Move the check for scared up here - if the monster was scared,
     * we were not doing any of the logic below, so might as well save
     * a few cpu cycles.
     */
    if (!QUERY_FLAG(op, FLAG_SCARED))
    {
	rv_vector   rv1;

        /* now we test every part of an object .... this is a real ugly piece of code */
	    for (part=op; part!=NULL; part=part->more) {
	        get_rangevector(part, enemy, &rv1, 0x1);	
	        dir=rv1.direction;

		/* hm, not sure about this part - in original was a scared flag here too
		 * but that we test above... so can be old code here 
		 */
	        if(QUERY_FLAG(op,FLAG_RUN_AWAY))
		        dir=absdir(dir+4);
	        if(QUERY_FLAG(op,FLAG_CONFUSED))
		        dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

	        if(QUERY_FLAG(op,FLAG_CAST_SPELL) && !(RANDOM()%3)) {
		    if(monster_cast_spell(op,part,enemy,dir,&rv1))
			return 0;
		}

	        if(QUERY_FLAG(op,FLAG_READY_SCROLL) && !(RANDOM()%3)) {
		    if(monster_use_scroll(op,part,enemy,dir,&rv1))
			return 0;
		}

	        if(QUERY_FLAG(op,FLAG_READY_RANGE)&&!(RANDOM()%3)) {
		    if(monster_use_range(op,part,enemy,dir))
			return 0;
		}
	        if(QUERY_FLAG(op,FLAG_READY_SKILL)&&!(RANDOM()%3)) {
		    if(monster_use_skill(op,rv.part,enemy,rv.direction))
			return 0;
		}
	        if(QUERY_FLAG(op,FLAG_READY_BOW)&&!(RANDOM()%2)) {
		    if(monster_use_bow(op,part,enemy,dir))
			return 0;
		}
	    } /* for processing of all parts */        
    } /* If not scared */

    
    part = rv.part;
    dir=rv.direction;

    if(QUERY_FLAG(op, FLAG_SCARED) || QUERY_FLAG(op,FLAG_RUN_AWAY))
    	dir=absdir(dir+4);

    if(QUERY_FLAG(op,FLAG_CONFUSED))
	dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

    if ((op->attack_movement & LO4) && !QUERY_FLAG(op, FLAG_SCARED)) 
    {        
	switch (op->attack_movement & LO4) {
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

	    case RUSH: /* default - monster normally moves towards player */ 
	    case ALLRUN:
		break; 

	    case DISTHIT:
		dir = disthit_att (dir,op,enemy,part,&rv);
		break;

	    case WAIT2:
	        dir = wait_att2 (dir,op,enemy,part,&rv);
		break;

	    default:
		LOG(llevDebug,"Illegal low mon-move: %d\n",op->attack_movement & LO4);
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
     * Try giving the monster a new enemy - the player that is closest
     * to it.  In this way, it won't just keep trying to get to a target
     * that is inaccessible.
     * This could be more clever - it should go through a list of several
     * enemies, as it is now, you could perhaps get situations where there
     * are two players flanking the monster at close distance, but which
     * the monster can't get to, and a third one at a far distance that
     * the monster could get to - as it is, the monster won't look at that
     * third one.
     */
    if (!QUERY_FLAG(op, FLAG_FRIENDLY) && enemy == op->enemy)
    {
	object *nearest_player = get_nearest_player(op);
	if (nearest_player && nearest_player != enemy && !can_hit(part,enemy,&rv)) {
	    op->enemy = NULL;
	    enemy = nearest_player;
	}
    }

    if(!QUERY_FLAG(op, FLAG_SCARED)&&can_hit(part,enemy,&rv))
    {
	/* The adjustement to wc that was here before looked totally bogus -
	 * since wc can in fact get negative, that would mean by adding
	 * the current wc, the creature gets better?  Instead, just
	 * add a fixed amount - nasty creatures that are runny away should
	 * still be pretty nasty.
	 */
        if(QUERY_FLAG(op,FLAG_RUN_AWAY))
        {
	    part->stats.wc+=10;
	    (void)skill_attack(enemy,part,0,NULL, NULL);
	    part->stats.wc-=10;
	}
        else
            (void)skill_attack(enemy,part,0,NULL, NULL);
    } /* if monster is in attack range */

    if(QUERY_FLAG(part,FLAG_FREED))    /* Might be freed by ghost-attack or hit-back */
    	return 1;

    if(QUERY_FLAG(op, FLAG_ONLY_ATTACK))
    {
	remove_ob(op);
	free_object(op);
	return 1;
    }
    return 0;
}

static int can_hit(object *ob1,object *ob2, rv_vector *rv) {
    object *more;
    rv_vector rv1;

    if(QUERY_FLAG(ob1,FLAG_CONFUSED)&&!(RANDOM()%3))
	return 0;

    if (abs(rv->distance_x)<2&&abs(rv->distance_y)<2) return 1;

    /* check all the parts of ob2 - just because we can't get to
     * its head doesn't mean we don't want to pound its feet
     */
    for (more = ob2->more; more!=NULL; more = more->more) {
	get_rangevector(ob1, more, &rv1, 0);
	if (abs(rv1.distance_x)<2&&abs(rv1.distance_y)<2) return 1;
    }
    return 0;

}

/* Returns 1 is monster should cast spell sp at an enemy
 * Returns 0 if the monster should not cast this spell.
 *
 * Note that this function does not check to see if the monster can
 * in fact cast the spell (sp dependencies and what not.)  That is because
 * this function is also sued to see if the monster should use spell items
 * (rod/horn/wand/scroll).
 * Note that there are certainly other offensive spells that could be
 * included, but I decided to leave out the spells that may kill more
 * monsters than players (eg, disease).
 *
 * This could be a lot smarter - if there are few monsters around,
 * then disease might not be as bad. Likewise, if the monster is damaged,
 * the right type of healing spell could be useful.
 */

static int monster_should_cast_spell(object *monster, object *spell_ob)
{
    if (spell_ob->subtype == SP_BOLT || spell_ob->subtype == SP_BULLET ||
	spell_ob->subtype == SP_EXPLOSION || spell_ob->subtype == SP_CONE ||
	spell_ob->subtype == SP_BOMB || spell_ob->subtype == SP_SMITE ||
	spell_ob->subtype == SP_MAGIC_MISSILE || spell_ob->subtype == SP_SUMMON_GOLEM ||
	spell_ob->subtype == SP_MAGIC_WALL || spell_ob->subtype == SP_SUMMON_MONSTER ||
	spell_ob->subtype == SP_MOVING_BALL || spell_ob->subtype == SP_SWARM ||
	spell_ob->subtype == SP_INVISIBLE)

	return 1;

    return 0;
}


#define MAX_KNOWN_SPELLS 20

/* Returns a randomly selected spell.    This logic is still
 * less than ideal.  This code also only seems to deal with
 * wizard spells, as the check is against sp, and not grace.
 * can mosnters know cleric spells?
 */
static object *monster_choose_random_spell(object *monster) {
    object *altern[MAX_KNOWN_SPELLS];
    object *tmp;
    int i=0;

    for(tmp=monster->inv;tmp!=NULL;tmp=tmp->below)
	if (tmp->type==SPELLBOOK || tmp->type==SPELL) {
	    /* Check and see if it's actually a useful spell.
	     * If its a spellbook, the spell is actually the inventory item.
	     * if it is a spell, then it is just the object itself.
	     */
	    if (monster_should_cast_spell(monster, (tmp->type==SPELLBOOK)?tmp->inv:tmp)) {
		altern[i++]=tmp;
		if(i==MAX_KNOWN_SPELLS)
		    break;
	    }
	}
    if(!i)
	return NULL;
    return altern[RANDOM()%i];
}

/* This checks to see if the monster should cast a spell/ability.
 * it returns true if the monster casts a spell, 0 if he doesn't.
 * head is the head of the monster.
 * part is the part of the monster we are checking against.
 * pl is the target.
 * dir is the direction to case.
 * rv is the vector which describes where the enemy is.
 */

static int monster_cast_spell(object *head, object *part,object *pl,int dir, rv_vector *rv) {
    object *spell_item;
    object *owner;
    rv_vector	rv1;

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
	if(dirdiff(dir,rv1.direction) < 2) {
	        return 0; /* Might hit owner with spell */
	}
    }

    if(QUERY_FLAG(head,FLAG_CONFUSED))
	dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

    /* If the monster hasn't already chosen a spell, choose one
     * I'm not sure if it really make sense to pre-select spells (events
     * could be different by the time the monster goes again).
     */
    if(head->spellitem==NULL) {
	if((spell_item=monster_choose_random_spell(head))==NULL) {
	    LOG(llevMonster,"Turned off spells in %s\n",head->name);
	    CLEAR_FLAG(head, FLAG_CAST_SPELL); /* Will be turned on when picking up book */
	    return 0;
	}
	if (spell_item->type == SPELLBOOK) {
	    if (!spell_item->inv) {
		LOG(llevError,"spellbook %s does not contain a spell?\n", spell_item->name);
		return 0;
	    }
	    spell_item=spell_item->inv;
	}
    }
    else
	spell_item=head->spellitem; 

    if (!spell_item) return 0;

    /* Best guess this is a defensive/healing spell */
    if (spell_item->range<=1 || spell_item->stats.dam < 0)
	dir = 0;
  
    /* Monster doesn't have enough spell-points */
    if(head->stats.sp<SP_level_spellpoint_cost(head,spell_item, SPELL_MANA))
	return 0;

    if(head->stats.grace<SP_level_spellpoint_cost(head,spell_item, SPELL_GRACE))
	return 0;

    head->stats.sp-=SP_level_spellpoint_cost(head,spell_item, SPELL_MANA);
    head->stats.grace-=SP_level_spellpoint_cost(head,spell_item, SPELL_GRACE);

    /* set this to null, so next time monster will choose something different */
    head->spellitem = NULL;
    
    return cast_spell(part,part,dir, spell_item, NULL);
}


static int monster_use_scroll(object *head, object *part,object *pl,int dir, rv_vector *rv) {
    object *scroll;
    object *owner;
    rv_vector	rv1;

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
	if(dirdiff(dir,rv1.direction) < 2) {
	        return 0; /* Might hit owner with spell */
	}
    }

    if(QUERY_FLAG(head,FLAG_CONFUSED))
	dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

    for (scroll=head->inv; scroll; scroll=scroll->below)
	if (scroll->type == SCROLL && monster_should_cast_spell(head, scroll->inv)) break;

    /* Used up all his scrolls, so nothing do to */
    if (!scroll) {
	CLEAR_FLAG(head, FLAG_READY_SCROLL);
	return 0;
    }

    /* Spell should be cast on caster (ie, heal, strength) */
    if (scroll->inv->range==0)
	dir = 0;

    apply_scroll(part, scroll, dir);
    return 1;
}

/* monster_use_skill()-implemented 95-04-28 to allow monster skill use.
 * Note that monsters do not need the skills SK_MELEE_WEAPON and
 * SK_MISSILE_WEAPON to make those respective attacks, if we
 * required that we would drastically increase the memory
 * requirements of CF!! 
 *
 * The skills we are treating here are all but those. -b.t. 
 *
 * At the moment this is only useful for throwing, perhaps for
 * stealing. TODO: This should be more integrated in the game. -MT, 25.11.01
 */  

static int monster_use_skill(object *head, object *part, object *pl,int dir) {
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
    return do_skill(head, part, head->chosen_skill,dir,NULL);
}

/* Monster will use a ranged spell attack. */

static int monster_use_range(object *head,object *part,object *pl,int dir)
    {
    object *wand, *owner;
    int at_least_one = 0;

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
        {
        if (wand->type == WAND)
            {
            /* Found a wand, let's see if it has charges left */
            at_least_one = 1;
	        if( wand->stats.food<=0 )
                continue;

            cast_spell( head, wand, dir, wand->inv, NULL );

            if ( !( --wand->stats.food ) )
                {
		        if ( wand->arch )
                    {
		            CLEAR_FLAG(wand, FLAG_ANIMATE);
		            wand->face = wand->arch->clone.face;
		            wand->speed = 0;
		            update_ob_speed(wand);
		            }
                }
            /* Success */
            return 1;
            }
        else if ( wand->type == ROD || wand->type==HORN ) {
            /* Found rod/horn, let's use it if possible */
	    at_least_one = 1;
	    if( wand->stats.hp < MAX( wand->inv->stats.sp, wand->inv->stats.grace ) )
                continue;

	    /* drain charge before casting spell - can be a case where the
	     * spell destroys the monster, and rod, so if done after, results
	     * in crash.
	     */
	    drain_rod_charge( wand );
            cast_spell( head, wand, dir, wand->inv, NULL );

            /* Success */
            return 1;
            }
        }

    if ( at_least_one )
        return 0;

    LOG(llevError,"Error: Monster %s (%d) HAS_READY_RANG() without wand/horn/rod.\n",
            head->name,head->count);
	CLEAR_FLAG(head, FLAG_READY_RANGE);
	return 0;
    }

static int monster_use_bow(object *head, object *part, object *pl, int dir) {
    object *owner;

    if(!(dir=path_to_player(part,pl,0)))
	return 0;
    if(QUERY_FLAG(head,FLAG_CONFUSED))
	dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

    if(QUERY_FLAG(head,FLAG_FRIENDLY) && (owner = get_owner(head)) != NULL) {
	int dir2 = find_dir_2(head->x-owner->x, head->y-owner->y);
	if(dirdiff(dir,dir2) < 1)
	    return 0; /* Might hit owner with arrow */
    }

    /* in server/player.c */
    return fire_bow(head, part, NULL, dir, 0, part->x, part->y);

}

/* Checks if putting on 'item' will make 'who' do more
 * damage.  This is a very simplistic check - also checking things
 * like speed and ac are also relevant.
 *
 * return true if item is a better object.
 */

static int check_good_weapon(object *who, object *item) {
    object *other_weap;
    int val=0, i;

    for(other_weap=who->inv;other_weap!=NULL;other_weap=other_weap->below)
	if(other_weap->type==item->type&&QUERY_FLAG(other_weap,FLAG_APPLIED))
	    break;

    if(other_weap==NULL) /* No other weapons */
	return 1;

    /* Rather than go through and apply the new one, and see if it is
     * better, just do some simple checks
     * Put some multipliers for things that hvae several effects,
     * eg, magic affects both damage and wc, so it has more weight
     */

    val = item->stats.dam - other_weap->stats.dam;
    val += (item->magic - other_weap->magic) * 3;
    /* Monsters don't really get benefits from things like regen rates
     * from items.  But the bonus for their stats are very important.
     */
    for (i=0; i<NUM_STATS; i++) 
	val += (get_attr_value(&item->stats, i) - get_attr_value(&other_weap->stats, i))*2;

    if (val > 0) return 1;
    else return 0;

}

static int check_good_armour(object *who, object *item) {
    object *other_armour;
    int val=0,i;

    for (other_armour = who->inv; other_armour != NULL;
	 other_armour = other_armour->below)
	if (other_armour->type == item->type && QUERY_FLAG(other_armour,FLAG_APPLIED))
	    break;

    if (other_armour == NULL) /* No other armour, use the new */
	return 1;

    /* Like above function , see which is better */
    val = item->stats.ac - other_armour->stats.ac;
    val = (item->resist[ATNR_PHYSICAL] - other_armour->resist[ATNR_PHYSICAL])/5;
    val += (item->magic - other_armour->magic) * 3;

    /* for the other protections, do weigh them very much in the equation -
     * it is the armor protection which is most important, because there is
     * no good way to know what the player may attack the monster with.	
     * So if the new item has better protection than the old, give that higher
     * value.  If the reverse, then decrease the value of this item some.
     */
    for (i=1; i <NROFATTACKS; i++) {
	if (item->resist[i] > other_armour->resist[i]) val++;
	else if (item->resist[i] < other_armour->resist[i]) val--;
    }

    /* Very few armours have stats, so not much need to worry about those. */

    if (val > 0) return 1;
    else return 0;

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

static void monster_check_pickup(object *monster) {
    object *tmp,*next;
    int next_tag;

    for(tmp=monster->below;tmp!=NULL;tmp=next) {
	next=tmp->below;
	next_tag = next ? next->count : 0;
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

static int monster_can_pick(object *monster, object *item) {
    int flag=0;
    int i;

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
	case HORN:
	case ROD:
	    flag=QUERY_FLAG(monster,FLAG_USE_RANGE);
	    break;

	case SPELLBOOK:
	    flag=(monster->arch!=NULL&&QUERY_FLAG((&monster->arch->clone),FLAG_CAST_SPELL));
	    break;

	case SCROLL:
	    flag = QUERY_FLAG(monster,FLAG_USE_SCROLL);
	    break;

	case BOW:
	case ARROW:
	    flag=QUERY_FLAG(monster,FLAG_USE_BOW);
	    break;
    }
    /* Simplistic check - if the monster has a location to equip it, he will
     * pick it up.  Note that this doesn't handle cases where an item may
     * use several locations.
     */
    for (i=0; i < NUM_BODY_LOCATIONS; i++) {
	if (monster->body_info[i] && item->body_info[i]) {
	    flag=1;
	    break;
	}
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

static void monster_apply_below(object *monster) {
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
 * Note that as things are now, monsters never drop something -
 * they can pick up all that they can use.
 */

/* Sept 96, fixed this so skills will be readied -b.t.*/

void monster_check_apply(object *mon, object *item) {

    int flag = 0;

    if(item->type==SPELLBOOK&&
       mon->arch!=NULL&&(QUERY_FLAG((&mon->arch->clone),FLAG_CAST_SPELL))) {
	SET_FLAG(mon, FLAG_CAST_SPELL);
	return;
    }

    /* If for some reason, this item is already applied, no more work to do */
    if(QUERY_FLAG(item,FLAG_APPLIED)) return;

    /* Might be better not to do this - if the monster can fire a bow,
     * it is possible in his wanderings, he will find one to use.  In
     * which case, it would be nice to have ammo for it.
     */
    if(QUERY_FLAG(mon,FLAG_USE_BOW) && item->type==ARROW) {
	/* Check for the right kind of bow */
	object *bow;
	for(bow=mon->inv;bow!=NULL;bow=bow->below)
	    if(bow->type==BOW && bow->race==item->race) {
		SET_FLAG(mon, FLAG_READY_BOW);
		LOG(llevMonster,"Found correct bow for arrows.\n");
		return;	    /* nothing more to do for arrows */
	    }
    }

    if (item->type == TREASURE && mon->will_apply & WILL_APPLY_TREASURE) flag=1;
    /* Eating food gets hp back */
    else if (item->type == FOOD && mon->will_apply & WILL_APPLY_FOOD) flag=1;
    else if (item->type == SCROLL && QUERY_FLAG(mon, FLAG_USE_SCROLL)) {
        if (!item->inv)
            LOG(llevDebug,"Monster %d having scroll %d with empty inventory!\n", mon->count, item->count);
        else if (monster_should_cast_spell(mon, item->inv))
            SET_FLAG(mon, FLAG_READY_SCROLL);
        /* Don't use it right now */
        return;
    }
    else if (item->type == WEAPON) flag = check_good_weapon(mon,item);
    else if (IS_ARMOR(item)) flag = check_good_armour(mon,item);
    /* Should do something more, like make sure this is a better item */
    else if (item->type == RING)
        flag=1;
    else if ( item->type==WAND || item->type == ROD || item->type==HORN )
        {
	    /* We never really 'ready' the wand/rod/horn, because that would mean the
	    * weapon would get undone.
	    */
    	if (!(can_apply_object(mon, item) & CAN_APPLY_NOT_MASK))
            {
	        SET_FLAG(mon, FLAG_READY_RANGE);
            SET_FLAG(item, FLAG_APPLIED);
            }
	    return;
        }
    else if (item->type == BOW) {
	    /* We never really 'ready' the bow, because that would mean the
	    * weapon would get undone.
	    */
    	if (!(can_apply_object(mon, item) & CAN_APPLY_NOT_MASK))
	        SET_FLAG(mon, FLAG_READY_BOW);
	    return;
        }
    else if ( item->type == SKILL )
        {
        /*
         * skills are specials: monsters must have the 'FLAG_READY_SKILL' flag set,
         * else they can't use the skill...
         * Skills also don't need to get applied, so return now.
         */
        SET_FLAG(mon, FLAG_READY_SKILL);
        return;
        }


    /* if we don't match one of the above types, return now.
     * can_apply_object will say that we can apply things like flesh, 
     * bolts, and whatever else, because it only checks against the
     * body_info locations.
     */
    if (!flag) return;

    /* Check to see if the monster can use this item.  If not, no need
     * to do further processing.  Note that can_apply_object already checks
     * for the CAN_USE flags.
     */
    if (can_apply_object(mon, item) & CAN_APPLY_NOT_MASK) return;

    /* should only be applying this item, not unapplying it.
     * also, ignore status of curse so they can take off old armour.
     * monsters have some advantages after all.
     */
    manual_apply(mon, item, AP_APPLY | AP_IGNORE_CURSE);

    return;
}

void npc_call_help(object *op) {
    int x,y, mflags;
    object *npc;
    sint16 sx, sy;
    mapstruct *m;

    for(x = -3; x < 4; x++)
	for(y = -3; y < 4; y++) {
	    m = op->map;
	    sx = op->x + x;
	    sy = op->y + y;
	    mflags = get_map_flags(m, &m, sx, sy, &sx, &sy);
	    /* If nothing alive on this space, no need to search the space. */
	    if ((mflags & P_OUT_OF_MAP) || !(mflags & P_IS_ALIVE))
		continue;

	    for(npc = get_map_ob(m,sx,sy);npc!=NULL;npc=npc->above)
		if(QUERY_FLAG(npc, FLAG_ALIVE)&&QUERY_FLAG(npc, FLAG_UNAGGRESSIVE))
		    npc->enemy = op->enemy;
	}
}


static int dist_att (int dir , object *ob, object *enemy, object *part, rv_vector *rv) {

    if (can_hit(part,enemy,rv))
    	return dir;
    if (rv->distance < 10)
    	return absdir(dir+4);
    else if (rv->distance>18)
    	return dir;
    return 0;
}

static int run_att (int dir, object *ob, object *enemy,object *part, rv_vector *rv) {

    if ((can_hit (part,enemy,rv) && ob->move_status <20) || ob->move_status <20) {
	ob->move_status++;
	return (dir);
    }
    else if (ob->move_status >20)
	ob->move_status = 0;
    return absdir (dir+4);
}

static int hitrun_att (int dir, object *ob,object *enemy) {
    if (ob->move_status ++ < 25)  
	return dir;
    else if (ob->move_status <50) 
	return absdir (dir+4); 
    else 
	ob->move_status = 0;
    return absdir(dir+4);
}

static int wait_att (int dir, object *ob,object *enemy,object *part,rv_vector *rv) {

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

static int disthit_att (int dir, object *ob, object *enemy, object *part,rv_vector *rv) {

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

static int wait_att2 (int dir, object *ob,object *enemy,object *part, rv_vector *rv) {
    if (rv->distance < 9)
	return absdir (dir+4);
    return 0;
}

static void circ1_move (object *ob) {
  static int circle [12] = {3,3,4,5,5,6,7,7,8,1,1,2};
  if(++ob->move_status > 11)
    ob->move_status = 0;
  if (!(move_object(ob,circle[ob->move_status])))
    (void) move_object(ob,RANDOM()%8+1);
}

static void circ2_move (object *ob) {
  static int circle[20] = {3,3,3,4,4,5,5,5,6,6,7,7,7,8,8,1,1,1,2,2};
  if(++ob->move_status > 19)
    ob->move_status = 0;
  if(!(move_object(ob,circle[ob->move_status])))
    (void) move_object(ob,RANDOM()%8+1);
}

static void pace_movev(object *ob) {
  if (ob->move_status++ > 6)
    ob->move_status = 0;
  if (ob->move_status < 4)
    (void) move_object (ob,5);
  else
    (void) move_object(ob,1);
}

static void pace_moveh (object *ob) {
  if (ob->move_status++ > 6)
    ob->move_status = 0;
  if (ob->move_status < 4)
    (void) move_object(ob,3);
  else
    (void) move_object(ob,7);
}

static void pace2_movev (object *ob) {
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

static void pace2_moveh (object *ob) {
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

static void rand_move (object *ob) {
  int i;
  if (ob->move_status <1 || ob->move_status >8 ||
      !(move_object(ob,ob->move_status|| ! (RANDOM()% 9))))
    for (i = 0; i < 5; i++)
      if (move_object(ob,ob->move_status = RANDOM()%8+1))
        return;
}

void check_earthwalls(object *op, mapstruct *m, int x, int y) {
    object *tmp;
    for (tmp = get_map_ob(m, x, y); tmp!=NULL; tmp=tmp->above) {
	if (tmp->type == EARTHWALL) {
	    hit_player(tmp,op->stats.dam,op,AT_PHYSICAL,1);
	    return;
	}
    }
}

void check_doors(object *op, mapstruct *m, int x, int y) {
    object *tmp;
    for (tmp = get_map_ob(m, x, y); tmp!=NULL; tmp=tmp->above) {
	if (tmp->type == DOOR) {
	    hit_player(tmp,1000,op,AT_PHYSICAL,1);
	    return;
	}
    }
}

/* This replaces all the msglang stuff about which seems to be a lot of
 * unneeded complication - since the setup of that data is never re-used
 * (say 'hi' to monster, then 'yes', it would re-do the entire parse-message)
 * it seems to me to make more sense to just have simple function that returns
 * the 'text' portion of the message that it matches - this savees us a bunch
 * of malloc's and free's, as well as that setup.
 * This function takes the message to be parsed in 'msg', the text to
 * match in 'match', and returns the portion of the message.  This
 * returned portion is in a malloc'd buf that should be freed.
 * Returns NULL if no match is found.
 * The player is passed too, so that quest-related messages can be checked too.
 */
static char *find_matching_message(object* pl, const char *msg, const char *match)
{
    const char *cp=msg, *cp1, *cp2;
    char *cp3, regex[MAX_BUF], gotmatch=0;

    while (1) {
	if (strncmp(cp, "@match ", 7)) {
	    LOG(llevDebug,"find_matching_message: Invalid message %s\n", msg);
	    return NULL;
	}
	else {
	    /* Find the end of the line, and copy the regex portion into it */
	    cp2 = strchr(cp+7, '\n');
	    strncpy(regex, cp+7, (cp2 - cp -7 ));
	    regex[cp2 - cp -7] = 0;

	    /* Find the next match command */
	    cp1 = strstr(cp+6, "\n@match");

	    /* Got a match - handle * as special case - proper regex would be .*,
	     * but lots of messages don't use that form.
	     */
	    if (regex[0] == '*') gotmatch=1;
	    else {
		char *pipe, *pnext=NULL;
		/* need to parse all the | seperators.  Our re_cmp isn't
		 * realy a fully blown regex parser.
		 */
		for (pipe=regex; pipe != NULL; pipe = pnext) {
		    pnext = strchr(pipe, '|');
		    if (pnext) {
			*pnext = 0;
			pnext ++;
		    }
		    if (re_cmp(match, pipe)) {
			gotmatch = 1;
			break;
		    }
		}
	    }
	    if (gotmatch) {
		if (cp1) {
		    cp3 = malloc(cp1 - cp2 + 1);
		    strncpy(cp3, cp2+1, cp1 - cp2);
		    cp3[cp1 - cp2] = 0;
		}
		else {	/* if no next match, just want the rest of the string */
		    cp3 = strdup_local(cp2+1);
		}
        return cp3;
	    }
        gotmatch = 0;
	    if (cp1) cp = cp1 + 1;
	    else return NULL;
	}
    }
    /* Should never get reached */
}

/* This function looks for an object or creature that is listening.
 * I've disabled the bit that has only the first npc monster listen -
 * we'll see how this works out.  only the first npc listens, which
 * is sort of bogus since it uses the free_arr which has a preference
 * to certain directions.
 *
 * There is a rare even that the orig_map is used for - basically, if
 * a player says the magic word that gets him teleported off the map,
 * it can result in the new map putting the object count too high,
 * which forces the swap out of some other map.  In some cases, the
 * map the player was just on now gets swapped out - thus, the
 * object on that map are no longer in memory.  So check to see if the
 * players map changes, and if so, don't process any further.
 * If it does change, most likely we don't care about the results
 * of further conversation.  Also, depending on the value of i,
 * the conversation would continue on the new map, which probably isn't
 * what is really wanted either.
 */
void communicate(object *op, const char *txt) {
    object *npc;
    int i, mflags;
    sint16 x, y;
    mapstruct *mp, *orig_map = op->map;

    int flag=1; /*hasn't spoken to a NPC yet*/
    for(i = 0; i <= SIZEOFFREE2; i++) {

	mp = op->map;
	x = op->x + freearr_x[i];
	y = op->y + freearr_y[i];

	mflags = get_map_flags(mp, &mp, x, y, &x, &y);
	if (mflags & P_OUT_OF_MAP) continue;

	for(npc = get_map_ob(mp,x,y); npc != NULL; npc = npc->above) {
	    if (npc->type == MAGIC_EAR) {
		(void) talk_to_wall(op, npc, txt); /* Maybe exit after 1. success? */
		if (orig_map != op->map) {
		    LOG(llevDebug,"Warning: Forced to swap out very recent map - MAX_OBJECTS should probably be increased\n");
		    return;
		}
	    }
	    else if (flag)  {
#if 0
		if (talk_to_npc(op, npc,txt))
		flag=0; /* Can be crowded */
#else
		talk_to_npc(op, npc,txt);
#endif
		if (orig_map != op->map) {
		    LOG(llevDebug,"Warning: Forced to swap out very recent map - MAX_OBJECTS should probably be increased\n");
		    return;
		}
	    }
	}
    }
}

static int do_talk_npc(object* op, object* npc, object* override, const char* txt)
{
    char* cp;

    if(override->msg == NULL || *override->msg != '@')
	return 0;

    cp = find_matching_message(op, override->msg, txt);
    if (cp) {
        npc_say(npc, cp);
        quest_apply_items(override,op->contr);
        free(cp);
        return 1;
    }
    return 0;
}

void npc_say(object *npc, char *cp) {
    char buf[MAX_BUF];
    snprintf(buf, sizeof(buf), "%s says:", query_name(npc));
    ext_info_map(NDI_NAVY|NDI_UNIQUE, npc->map, MSG_TYPE_DIALOG, MSG_TYPE_DIALOG_NPC,
		 buf, buf);
    ext_info_map(NDI_NAVY|NDI_UNIQUE, npc->map, MSG_TYPE_DIALOG, MSG_TYPE_DIALOG_NPC,
		 cp, cp);
}

static int talk_to_npc(object *op, object *npc, const char *txt) {
    object *cobj;

    /* Move this commone area up here - shouldn't cost much extra cpu
     * time, and makes the function more readable */
    /* Lauwenmark: Handle for plugin say event */
    if (op==npc) return 0;
    if (execute_event(npc, EVENT_SAY,op,NULL,txt,SCRIPT_FIX_ALL)!=0)
	    return 0;
    /* Lauwenmark - Here we let the objects inside inventories hear and answer, too. */
    /* This allows the existence of "intelligent" weapons you can discuss with */
    for(cobj=npc->inv;cobj!=NULL; cobj = cobj->below)
    {
        if (execute_event(cobj, EVENT_SAY,npc,NULL,txt,SCRIPT_FIX_ALL)!=0)
                return 0;
            }
    for ( cobj = npc->inv; cobj; cobj = cobj->below )
        if ( quest_is_override_compatible( cobj, op ) )
            if ( do_talk_npc( op, npc, cobj, txt ) )
                return 1;
    return do_talk_npc( op, npc, npc, txt );
}

static int do_talk_wall(object* pl, object* npc, object* override, const char* txt)
{
    char* cp;
    if(override->msg == NULL || *override->msg != '@')
	return 0;

    cp = find_matching_message(pl, override->msg, txt);
    if (!cp)
	return 0;

    ext_info_map(NDI_NAVY | NDI_UNIQUE, npc->map, 
		 MSG_TYPE_DIALOG, MSG_TYPE_DIALOG_MAGIC_MOUTH,
		 cp, cp);
    use_trigger(npc);
    quest_apply_items(npc, pl->contr);
    free(cp);

    return 1;
}

static int talk_to_wall(object* pl, object *npc, const char *txt) {

    object* inv;

    for ( inv = npc->inv; inv; inv = inv->below)
        if ( quest_is_override_compatible(inv, pl ) )
            if ( do_talk_wall( pl, npc, inv, txt ) )
                return 1;

    return do_talk_wall( pl, npc, npc, txt );;
}

/* find_mon_throw_ob() - modeled on find_throw_ob
 * This is probably overly simplistic as it is now - We want
 * monsters to throw things like chairs and other pieces of
 * furniture, even if they are not good throwable objects.
 * Probably better to have the monster throw a throwable object
 * first, then throw any non equipped weapon.
 */

object *find_mon_throw_ob(object *op) {
    object *tmp = NULL;
  
    if(op->head) tmp=op->head; else tmp=op;  

    /* New throw code: look through the inventory. Grap the first legal is_thrown
     * marked item and throw it to the enemy.
     */

    for(tmp=op->inv;tmp;tmp=tmp->below) {

	/* Can't throw invisible objects or items that are applied */
      if(tmp->invisible || QUERY_FLAG(tmp,FLAG_APPLIED)) continue;

      if(QUERY_FLAG(tmp,FLAG_IS_THROWN)) 
          break;
          
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
 * Returns 0 if enemy can not be detected, 1 if it is detected
 */

int can_detect_enemy (object *op, object *enemy, rv_vector *rv) {
    int radius = MIN_MON_RADIUS, hide_discovery;

    /* null detection for any of these condtions always */
    if(!op || !enemy || !op->map || !enemy->map)
        return 0;

    /* If the monster (op) has no way to get to the enemy, do nothing */
    if (!on_same_map(op, enemy))
        return 0;

    get_rangevector(op, enemy, rv, 0);

    /* Monsters always ignore the DM */
    if ( ( op->type != PLAYER ) && QUERY_FLAG( enemy, FLAG_WIZ ) )
        return 0;

    /* simple check.  Should probably put some range checks in here. */
    if(can_see_enemy(op,enemy)) return 1;

    /* The rest of this is for monsters. Players are on their own for
     * finding enemies!
     */
    if(op->type==PLAYER) return 0;

    /* Quality invisible? Bah, we wont see them w/o SEE_INVISIBLE
     * flag (which was already checked) in can_see_enmy (). Lets get out of here 
     */
    if(enemy->invisible && (!enemy->contr || (!enemy->contr->tmp_invis && !enemy->contr->hidden)))
	return 0;

    /* use this for invis also */
    hide_discovery = op->stats.Int/5;

    /* Determine Detection radii */
    if(!enemy->hide)  /* to detect non-hidden (eg dark/invis enemy) */
	radius = (op->stats.Wis/5)+1>MIN_MON_RADIUS?(op->stats.Wis/5)+1:MIN_MON_RADIUS;
    else { /* a level/INT/Dex adjustment for hiding */
	object *sk_hide;
	int bonus = (op->level/2) + (op->stats.Int/5);

	if(enemy->type==PLAYER) {
	    if((sk_hide = find_skill_by_number(enemy,SK_HIDING)))
		bonus -= sk_hide->level;
	    else { 
		LOG(llevError,"can_detect_enemy() got hidden player w/o hiding skill!\n");
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
	/* on dark maps body heat can help indicate location with infravision
	 * undead don't have body heat, so no benefit detecting them.
	 */
	if(QUERY_FLAG(op,FLAG_SEE_IN_DARK) && !is_true_undead(enemy))
	    radius += op->map->darkness/2;
	else
	    radius -= op->map->darkness/2;

	/* op next to a monster (and not in complete darkness) 
	 * the monster should have a chance to see you. 
	 */
	if(radius<MIN_MON_RADIUS && op->map->darkness<5 && rv->distance<=1)
	    radius = MIN_MON_RADIUS;
    } /* if on dark map */

    /* Lets not worry about monsters that have incredible detection
     * radii, we only need to worry here about things the player can
     * (potentially) see.  This is 13, as that is the maximum size the player
     * may have for their map - in that way, creatures at the edge will
     * do something.  Note that the distance field in the
     * vector is real distance, so in theory this should be 18 to
     * find that.
     */
    if(radius>13) radius = 13;

    /* Enemy in range! Now test for detection */
    if ((int) rv->distance <= radius) {
	/* ah, we are within range, detected? take cases */
	if(!enemy->invisible) /* enemy in dark squares... are seen! */
	    return 1;

	/* hidden or low-quality invisible */  
	if(enemy->hide && (rv->distance <= 1) && (RANDOM()%100<=hide_discovery)) {
	    make_visible(enemy);
	    /* inform players of new status */
	    if(enemy->type==PLAYER && player_can_view(enemy,op)) 
		draw_ext_info_format(NDI_UNIQUE,0, enemy,
			     MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			     "You are discovered by %s!",
			     "You are discovered by %s!",
			     op->name);
		return 1; /* detected enemy */ 
	}
	else if (enemy->invisible) {
	    /* Change this around - instead of negating the invisible, just
	     * return true so that the mosnter that managed to detect you can
	     * do something to you.  Decreasing the duration of invisible
	     * doesn't make a lot of sense IMO, as a bunch of stupid creatures
	     * can then basically negate the spell.  The spell isn't negated -
	     * they just know where you are!
	     */
	    if ((RANDOM() % 50) <= hide_discovery) {
		if (enemy->type == PLAYER) {
		    draw_ext_info_format(NDI_UNIQUE,0, enemy,
			 MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			 "You see %s noticing your position.", 
			 "You see %s noticing your position.", 
			 query_name(op));
		}
		return 1;
	    }
	}
    } /* within range */

    /* Wasn't detected above, so still hidden */
    return 0;
}

/* determine if op stands in a lighted square. This is not a very
 * intellegent algorithm. For one thing, we ignore los here, SO it 
 * is possible for a bright light to illuminate a player on the 
 * other side of a wall (!). 
 */

int stand_in_light( object *op) {
    sint16 nx,ny;
    mapstruct *m;


    if(!op) return 0;
    if(op->glow_radius > 0) return 1;

    if(op->map) {
	int x, y, x1, y1;
	
	

	/* Check the spaces with the max light radius to see if any of them
	 * have lights, and if any of them light the player enough, then return 1.
	 */
	for (x = op->x - MAX_LIGHT_RADII; x <= op->x + MAX_LIGHT_RADII; x++) {
	    for (y = op->y - MAX_LIGHT_RADII; y <= op->y + MAX_LIGHT_RADII; y++) {
		m = op->map;
		nx = x;
		ny = y;

		if (get_map_flags(m, &m, nx, ny, &nx, &ny) & P_OUT_OF_MAP) continue;

		x1 = abs(x - op->x)*abs(x - op->x);
		y1 = abs(y - op->y)*abs(y - op->y);
		if (isqrt(x1 + y1) <  GET_MAP_LIGHT(m, nx, ny)) return 1; 
	    }
	}
    }
    return 0;
}


/* assuming no walls/barriers, lets check to see if its *possible* 
 * to see an enemy. Note, "detection" is different from "seeing".
 * See can_detect_enemy() for more details. -b.t.
 * return 0 if can't be seen, 1 if can be
 */

int can_see_enemy (object *op, object *enemy) {
    object *looker = op->head?op->head:op;

    /* safety */
    if(!looker||!enemy||!QUERY_FLAG(looker,FLAG_ALIVE))
	return 0; 

    /* we dont give a full treatment of xrays here (shorter range than normal,
     * see through walls). Should we change the code elsewhere to make you 
     * blind even if you can xray? 
     */
    if(QUERY_FLAG(looker,FLAG_BLIND) && !QUERY_FLAG(looker,FLAG_XRAYS))
	    return 0;

    /* checking for invisible things */
    if(enemy->invisible) {
	/* HIDDEN ENEMY. by definition, you can't see hidden stuff! 
	 * However,if you carry any source of light, then the hidden
	 * creature is seeable (and stupid) */

	if(has_carried_lights(enemy)) { 
	    if(enemy->hide) { 
		make_visible(enemy);
		draw_ext_info(NDI_UNIQUE,0, enemy,
		      MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
		      "Your light reveals your hiding spot!",
		      NULL);
	    }
	    return 1;
	} else if (enemy->hide) return 0; 

	/* Invisible enemy.  Break apart the check for invis undead/invis looker
	 * into more simple checks - the QUERY_FLAG doesn't return 1/0 values,
	 * and making it a conditional makes the code pretty ugly.
	 */
	if (!QUERY_FLAG(looker,FLAG_SEE_INVISIBLE)) {
	    if (makes_invisible_to(enemy, looker)) return 0;
	}
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

