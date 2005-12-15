/*
 * static char *rcsid_attack_c =
 *   "$Id$";
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

    The authors can be reached via e-mail to crossfire-devel@real-time.com
*/
#include <assert.h>
#include <global.h>
#include <living.h>
#include <material.h>
#include <skills.h>

#ifndef __CEXTRACT__
#include <sproto.h>
#endif

#include <sounds.h>

typedef struct att_msg_str {
  char *msg1;
  char *msg2;
} att_msg;

/*#define ATTACK_DEBUG*/

/* cancels object *op.  Cancellation basically means an object loses
 * its magical benefits.
 */
void cancellation(object *op)
{
    object *tmp;

    if (op->invisible)
	return;
   
    if (QUERY_FLAG (op, FLAG_ALIVE) || op->type == CONTAINER  || op->type == THROWN_OBJ) {
	/* Recur through the inventory */
	for(tmp=op->inv;tmp!=NULL;tmp=tmp->below)
	if (!did_make_save_item(tmp, AT_CANCELLATION,op))
	    cancellation(tmp);
    }
    else if(FABS(op->magic)<=(rndm(0, 5))) {
	/* Nullify this object. This code could probably be more complete */
	/* in what abilities it should cancel */
	op->magic=0;
	CLEAR_FLAG(op, FLAG_DAMNED);
	CLEAR_FLAG(op, FLAG_CURSED);
	CLEAR_FLAG(op, FLAG_KNOWN_MAGICAL);
	CLEAR_FLAG(op, FLAG_KNOWN_CURSED);
	if (op->env && op->env->type == PLAYER) {
	    esrv_send_item (op->env, op);
	}
    }
}



/* did_make_save_item just checks to make sure the item actually
 * made its saving throw based on the tables.  It does not take
 * any further action (like destroying the item).
 */

int did_make_save_item(object *op, int type, object *originator) {
    int i, roll, saves=0, attacks=0, number;
    materialtype_t *mt;

    if (op->materialname == NULL) {
        for (mt = materialt; mt != NULL && mt->next != NULL; mt=mt->next) {
            if (op->material & mt->material)
                break;
        }
    } else
        mt = name_to_material(op->materialname);
    if (mt == NULL)
	return TRUE;
    roll = rndm(1, 20);

    /* the attacktypes have no meaning for object saves
     * If the type is only magic, don't adjust type - basically, if
     * pure magic is hitting an object, it should save.  However, if it
     * is magic teamed with something else, then strip out the
     * magic type, and instead let the fire, cold, or whatever component
     * destroy the item.  Otherwise, you get the case of poisoncloud
     * destroying objects because it has magic attacktype.
     */
    if (type != AT_MAGIC)
	type &= ~(AT_CONFUSION|AT_DRAIN|AT_GHOSTHIT|AT_POISON|AT_SLOW|
		    AT_PARALYZE|AT_TURN_UNDEAD|AT_FEAR|AT_DEPLETE|AT_DEATH|
		    AT_COUNTERSPELL|AT_HOLYWORD|AT_BLIND|AT_LIFE_STEALING|
		    AT_MAGIC);

    if (type == 0)
	return TRUE;
    if (roll == 20)
	return TRUE;
    if (roll == 1)
	return FALSE;

    for (number=0; number < NROFATTACKS; number++) {
	i = 1<<number;
	if (!(i&type))
	    continue;
	attacks++;
	if (op->resist[number] == 100)
	    saves++;
	else if (roll >= mt->save[number] - op->magic - op->resist[number]/100)
	    saves++;
	else if ((20-mt->save[number])/3 > originator->stats.dam)
	    saves++;
    }

    if (saves==attacks || attacks==0)
	return TRUE;
    if ((saves==0) || (rndm(1, attacks) > saves))
	return FALSE;
    return TRUE;
}

/* This function calls did_make_save_item.  It then performs the
 * appropriate actions to the item (such as burning the item up,
 * calling cancellation, etc.)
 */

void save_throw_object (object *op, int type, object *originator)
{
    if ( ! did_make_save_item (op, type,originator))
    {
	object *env=op->env;
	int x=op->x,y=op->y;
	mapstruct *m=op->map;

        op = stop_item (op);
        if (op == NULL)
            return;

	/* Hacked the following so that type LIGHTER will work. 
	 * Also, objects which are potenital "lights" that are hit by 
	 * flame/elect attacks will be set to glow. "lights" are any 
	 * object with +/- glow_radius and an "other_arch" to change to. 
	 * (and please note that we cant fail our save and reach this 
	 * function if the object doesnt contain a material that can burn. 
	 * So forget lighting magical swords on fire with this!) -b.t.
	 */ 
	if(type&(AT_FIRE|AT_ELECTRICITY)
           &&op->other_arch&&QUERY_FLAG(op, FLAG_IS_LIGHTABLE)) { 
		const char *arch=op->other_arch->name;

		op = decrease_ob_nr (op, 1);
                if (op)
                    fix_stopped_item (op, m, originator);
		if((op = get_archetype(arch))!=NULL) {
                   if(env) {  
			op->x=env->x,op->y=env->y;
			insert_ob_in_ob(op,env);
			if (env->contr)
			    esrv_send_item(env, op);
                   } else { 
                      op->x=x,op->y=y;
                      insert_ob_in_map(op,m,originator,0);
		   }
		}
		return;
        }
        if(type&AT_CANCELLATION) {          /* Cancellation. */
              cancellation(op);
              fix_stopped_item (op, m, originator);
              return;  
        }
	if(op->nrof>1) {
	      op = decrease_ob_nr(op,rndm(0, op->nrof-1));
              if (op)
                  fix_stopped_item (op, m, originator);
	} else {
	    if (op->env) {
		object *tmp= is_player_inv(op->env);

		if (tmp) {
		    esrv_del_item(tmp->contr, op->count);
		}
	    }
	    if ( ! QUERY_FLAG (op, FLAG_REMOVED))
                remove_ob(op);
	    free_object(op);
	}
	if(type&(AT_FIRE|AT_ELECTRICITY)) {
	      if(env) {
                op=get_archetype("burnout");
		op->x=env->x,op->y=env->y;
                insert_ob_in_ob(op,env);
	      } else { 
                replace_insert_ob_in_map("burnout",originator);
	      }
	}
	return;
    }
    /* The value of 50 is arbitrary. */
    if(type&AT_COLD &&(op->resist[ATNR_COLD]<50) &&
      !QUERY_FLAG(op,FLAG_NO_PICK)&&(RANDOM()&2)) {
        object *tmp;
        archetype *at = find_archetype("icecube");
        if (at == NULL)
          return;
        op = stop_item (op);
        if (op == NULL)
            return;
        if ((tmp = present_arch(at,op->map,op->x,op->y)) == NULL) {
	    tmp = arch_to_object(at);
	    tmp->x=op->x,tmp->y=op->y;
	    /* This was in the old (pre new movement code) -
	     * icecubes have slow_move set to 1 - don't want
	     * that for ones we create.
	     */
	    tmp->move_slow_penalty=0;
	    tmp->move_slow=0;
	    insert_ob_in_map(tmp,op->map,originator,0);
        }
        if ( ! QUERY_FLAG (op, FLAG_REMOVED))
            remove_ob(op);
        (void) insert_ob_in_ob(op,tmp);
        return;
    }
}

/* Object op is hitting the map.
 * op is going in direction 'dir'
 * type is the attacktype of the object.
 * full_hit is set if monster area does not matter.
 * returns 1 if it hits something, 0 otherwise.
 */

int hit_map(object *op, int dir, int type, int full_hit) {
    object *tmp, *next;
    mapstruct *map;
    sint16 x, y;
    int retflag=0;  /* added this flag..  will return 1 if it hits a monster */

    tag_t op_tag, next_tag=0;

    if (QUERY_FLAG (op, FLAG_FREED)) {
	LOG (llevError, "BUG: hit_map(): free object\n");
	return 0;
    }

    if (QUERY_FLAG (op, FLAG_REMOVED) || op->env != NULL) {
	LOG (llevError, "BUG: hit_map(): hitter (arch %s, name %s) not on a map\n",
	     op->arch->name, op->name);
	return 0;
    }

    if ( ! op->map) {
	LOG (llevError,"BUG: hit_map(): %s has no map\n", op->name);
	return 0;
    }

    if (op->head) op=op->head;

    op_tag = op->count;

    map = op->map;
    x = op->x + freearr_x[dir];
    y = op->y + freearr_y[dir];
    if (get_map_flags(map, &map, x, y, &x, &y) & P_OUT_OF_MAP)
	return 0;

    /* peterm:  a few special cases for special attacktypes --counterspell
     * must be out here because it strikes things which are not alive
     */

    if (type & AT_COUNTERSPELL) {
	counterspell(op,dir);  /* see spell_effect.c */

	/* If the only attacktype is counterspell or magic, don't need
	 * to do any further processing.
	 */
	if(!(type & ~(AT_COUNTERSPELL|AT_MAGIC))){
	    return 0;
	}
	type &= ~AT_COUNTERSPELL;
    }

    if(type & AT_CHAOS){
	shuffle_attack(op,1);  /*1 flag tells it to change the face */
	update_object(op,UP_OBJ_FACE);
	type &= ~AT_CHAOS;
    }

    next = get_map_ob (map, x, y);
    if (next)
	next_tag = next->count;

    while (next) {
	if (was_destroyed (next, next_tag)) {
	    /* There may still be objects that were above 'next', but there is no
	     * simple way to find out short of copying all object references and
	     * tags into a temporary array before we start processing the first
	     * object.  That's why we just abort.
	     *
	     * This happens whenever attack spells (like fire) hit a pile
	     * of objects. This is not a bug - nor an error. The errormessage
	     * below was spamming the logs for absolutely no reason.
	     */
	    /* LOG (llevDebug, "hit_map(): next object destroyed\n"); */
	    break;
	}
	tmp = next;
	next = tmp->above;
	if (next)
	    next_tag = next->count;

	if (QUERY_FLAG (tmp, FLAG_FREED)) {
	    LOG (llevError, "BUG: hit_map(): found freed object\n");
	    break;
	}

	/* Something could have happened to 'tmp' while 'tmp->below' was processed.
	 * For example, 'tmp' was put in an icecube.
	 * This is one of the few cases where on_same_map should not be used.
	 */
	if (tmp->map != map || tmp->x != x || tmp->y != y)
	    continue;

	if (QUERY_FLAG (tmp, FLAG_ALIVE)) {
	    hit_player(tmp,op->stats.dam,op,type,full_hit);
	    retflag |=1;
	    if (was_destroyed (op, op_tag))
		break;
	} 
	/* Here we are potentially destroying an object.  If the object has
	 * NO_PASS set, it is also immune - you can't destroy walls.  Note 
	 * that weak walls have is_alive set, which prevent objects from
	 * passing over/through them.  We don't care what type of movement
	 * the wall blocks - if it blocks any type of movement, can't be
	 * destroyed right now.
	 */
	else if ((tmp->material || tmp->materialname) && op->stats.dam > 0 &&
		   !tmp->move_block) {
	    save_throw_object(tmp,type,op);
	    if (was_destroyed (op, op_tag))
		break;
	}
    }
    return 0;
}

void attack_message(int dam, int type, object *op, object *hitter) {
  char buf[MAX_BUF], buf1[MAX_BUF], buf2[MAX_BUF];
  int i, found=0;
  mapstruct *map;
  object *next, *tmp;

  /* put in a few special messages for some of the common attacktypes
   *  a player might have.  For example, fire, electric, cold, etc
   *  [garbled 20010919]
   */

    if (dam == 9998 && op->type == DOOR) {
	sprintf(buf1, "unlock %s", op->name);
	sprintf(buf2, " unlocks");
	found++;
    }
    if(dam<0) {
        sprintf(buf1, "hit %s", op->name);
	sprintf(buf2, " hits");
	found++;
    } else if(dam==0) {
        sprintf(buf1, "missed %s", op->name);
        sprintf(buf2, " misses");
	found++;
    } else if ((hitter->type == DISEASE || hitter->type == SYMPTOM ||
	hitter->type == POISONING ||
	(type & AT_POISON && IS_LIVE(op))) && !found) {
        for (i=0; i < MAXATTACKMESS && attack_mess[ATM_SUFFER][i].level != -1;
	     i++)
	    if (dam < attack_mess[ATM_SUFFER][i].level) {
	        sprintf(buf1, "%s %s%s", attack_mess[ATM_SUFFER][i].buf1,
			op->name, attack_mess[ATM_SUFFER][i].buf2);
		sprintf(buf2, "%s", attack_mess[ATM_SUFFER][i].buf3);
		found++;
		break;
	    }
    } else if (op->type == DOOR && !found) {
        for (i=0; i < MAXATTACKMESS && attack_mess[ATM_DOOR][i].level != -1;
	     i++)
	    if (dam < attack_mess[ATM_DOOR][i].level) {
	        sprintf(buf1, "%s %s%s", attack_mess[ATM_DOOR][i].buf1,
			op->name, attack_mess[ATM_DOOR][i].buf2);
		sprintf(buf2, "%s", attack_mess[ATM_DOOR][i].buf3);
		found++;
		break;
	    }
    } else if (hitter->type == PLAYER  && IS_LIVE(op)) {
        if (USING_SKILL(hitter, SK_KARATE)) {
          for (i=0; i < MAXATTACKMESS && attack_mess[ATM_KARATE][i].level != -1;
	       i++)
	      if (dam < attack_mess[ATM_KARATE][i].level) {
	          sprintf(buf1, "%s %s%s", attack_mess[ATM_KARATE][i].buf1,
			  op->name, attack_mess[ATM_KARATE][i].buf2);
		  sprintf(buf2, "%s", attack_mess[ATM_KARATE][i].buf3);
		  found++;
		  break;
	      }
	} else if (USING_SKILL(hitter, SK_CLAWING)) {
          for (i=0; i < MAXATTACKMESS && attack_mess[ATM_CLAW][i].level != -1;
	       i++)
	      if (dam < attack_mess[ATM_CLAW][i].level) {
	          sprintf(buf1, "%s %s%s", attack_mess[ATM_CLAW][i].buf1,
			  op->name, attack_mess[ATM_CLAW][i].buf2);
		  sprintf(buf2, "%s", attack_mess[ATM_CLAW][i].buf3);
		  found++;
		  break;
	      }
	} else if (USING_SKILL(hitter, SK_PUNCHING)) {
          for (i=0; i < MAXATTACKMESS && attack_mess[ATM_PUNCH][i].level != -1;
	       i++)
	      if (dam < attack_mess[ATM_PUNCH][i].level) {
	          sprintf(buf1, "%s %s%s", attack_mess[ATM_PUNCH][i].buf1,
			  op->name, attack_mess[ATM_PUNCH][i].buf2);
		  sprintf(buf2, "%s", attack_mess[ATM_PUNCH][i].buf3);
		  found++;
		  break;
	      }
	}
    }
    if (IS_ARROW(hitter) && (type == AT_PHYSICAL || type == AT_MAGIC) &&
	!found) {
        sprintf(buf1, "hit"); /* just in case */
        for (i=0; i < MAXATTACKMESS; i++)
	    if (dam < attack_mess[ATM_ARROW][i].level) {
	        sprintf(buf2, "%s", attack_mess[ATM_ARROW][i].buf3);
		break;
	    }
    } else if (type & AT_DRAIN && IS_LIVE(op) && !found) {
      /* drain is first, because some items have multiple attypes */
        for (i=0; i < MAXATTACKMESS && attack_mess[ATM_DRAIN][i].level != -1;
	     i++)
	    if (dam < attack_mess[ATM_DRAIN][i].level) {
	        sprintf(buf1, "%s %s%s", attack_mess[ATM_DRAIN][i].buf1,
			op->name, attack_mess[ATM_DRAIN][i].buf2);
		sprintf(buf2, "%s", attack_mess[ATM_DRAIN][i].buf3);
		break;
	    }
    } else if (type & AT_ELECTRICITY && IS_LIVE(op) && !found) {
        for (i=0; i < MAXATTACKMESS && attack_mess[ATM_ELEC][i].level != -1;
	     i++)
	    if (dam < attack_mess[ATM_ELEC][i].level) {
	        sprintf(buf1, "%s %s%s", attack_mess[ATM_ELEC][i].buf1,
			op->name, attack_mess[ATM_ELEC][i].buf2);
		sprintf(buf2, "%s", attack_mess[ATM_ELEC][i].buf3);
		break;
	    }
    } else if (type & AT_COLD && IS_LIVE(op) && !found) {
        for (i=0; i < MAXATTACKMESS && attack_mess[ATM_COLD][i].level != -1;
	     i++)
	    if (dam < attack_mess[ATM_COLD][i].level) {
	        sprintf(buf1, "%s %s%s", attack_mess[ATM_COLD][i].buf1,
			op->name, attack_mess[ATM_COLD][i].buf2);
		sprintf(buf2, "%s", attack_mess[ATM_COLD][i].buf3);
		break;
	    }
    } else if (type & AT_FIRE && !found) {
        for (i=0; i < MAXATTACKMESS && attack_mess[ATM_FIRE][i].level != -1;
	     i++)
	    if (dam < attack_mess[ATM_FIRE][i].level) {
	        sprintf(buf1, "%s %s%s", attack_mess[ATM_FIRE][i].buf1,
			op->name, attack_mess[ATM_FIRE][i].buf2);
		sprintf(buf2, "%s", attack_mess[ATM_FIRE][i].buf3);
		break;
	    }
    } else if (hitter->current_weapon != NULL && !found) {
	int mtype;
	switch (hitter->current_weapon->weapontype) {
	    case WEAP_HIT: mtype = ATM_BASIC; break;
	    case WEAP_SLASH: mtype = ATM_SLASH; break;
	    case WEAP_PIERCE: mtype = ATM_PIERCE; break;
	    case WEAP_CLEAVE: mtype = ATM_CLEAVE; break;
	    case WEAP_SLICE: mtype = ATM_SLICE; break;
	    case WEAP_STAB: mtype = ATM_STAB; break;
	    case WEAP_WHIP: mtype = ATM_WHIP; break;
	    case WEAP_CRUSH: mtype = ATM_CRUSH; break;
	    case WEAP_BLUD: mtype = ATM_BLUD; break;
	    default: mtype = ATM_BASIC; break;
	}
        for (i=0; i < MAXATTACKMESS && attack_mess[mtype][i].level != -1;
	     i++)
	    if (dam < attack_mess[mtype][i].level) {
	        sprintf(buf1, "%s %s%s", attack_mess[mtype][i].buf1,
			op->name, attack_mess[mtype][i].buf2);
		sprintf(buf2, "%s", attack_mess[mtype][i].buf3);
		break;
	    }
    } else if (!found){
        for (i=0; i < MAXATTACKMESS && attack_mess[ATM_BASIC][i].level != -1;
	     i++)
	    if (dam < attack_mess[ATM_BASIC][i].level) {
	        sprintf(buf1, "%s %s%s", attack_mess[ATM_BASIC][i].buf1,
			op->name, attack_mess[ATM_BASIC][i].buf2);
		sprintf(buf2, "%s", attack_mess[ATM_BASIC][i].buf3);
		break;
	    }
    }

    /* bail out if a monster is casting spells */
    if (!(hitter->type == PLAYER ||
	(get_owner(hitter) != NULL && hitter->owner->type == PLAYER)))
      return;

    /* scale down magic considerably. */
    if (type & AT_MAGIC && rndm(0, 5))
      return;

    /* Did a player hurt another player?  Inform both! */
    /* only show half the player->player combat messages */
    if(op->type==PLAYER && rndm(0, 1) &&
       (get_owner(hitter)==NULL?hitter->type:hitter->owner->type)==PLAYER) {
	if(get_owner(hitter)!=NULL)
	    sprintf(buf,"%s's %s %s you.",
              hitter->owner->name, hitter->name, buf2);
	else {
	    sprintf(buf,"%s%s you.",hitter->name, buf2);
	    if (dam != 0) {
		if (dam < 10)
		    play_sound_player_only(op->contr, SOUND_PLAYER_IS_HIT1,0,0);
		else if (dam < 20)
		    play_sound_player_only(op->contr, SOUND_PLAYER_IS_HIT2,0,0);
		else
		    play_sound_player_only(op->contr, SOUND_PLAYER_IS_HIT3,0,0);
	    }
	}
	new_draw_info(NDI_BLACK, 0,op,buf);
    } /* end of player hitting player */

    /* scale down these messages too */
    if(hitter->type==PLAYER && rndm(0, 2) == 0) {
	sprintf(buf,"You %s.",buf1);
	if (dam != 0) {
	    if (dam < 10)
		play_sound_player_only(hitter->contr, SOUND_PLAYER_HITS1,0,0);
	    else if (dam < 20)
		play_sound_player_only(hitter->contr, SOUND_PLAYER_HITS2,0,0);
	    else
		play_sound_player_only(hitter->contr, SOUND_PLAYER_HITS3,0,0);
	}
	new_draw_info(NDI_BLACK, 0, hitter, buf);
    } else if(get_owner(hitter)!=NULL&&hitter->owner->type==PLAYER) {
      /* look for stacked spells and start reducing the message chances */
        if (hitter->type == SPELL_EFFECT && 
	    (hitter->subtype == SP_EXPLOSION ||
	     hitter->subtype == SP_BULLET ||
	     hitter->subtype == SP_CONE)) {
	   i=4;
	   map = hitter->map;
	   if (out_of_map(map, hitter->x, hitter->y))
	       return;
	   next = get_map_ob(map, hitter->x, hitter->y);
	   if (next)
	       while(next) {
		   if (next->type == SPELL_EFFECT && 
		       (next->subtype == SP_EXPLOSION || next->subtype==SP_BULLET ||
		       next->subtype == SP_CONE))
		       i*=3;
		   tmp = next;
		   next = tmp->above;
	       }
	   if (i < 0)
	       return;
	   if (rndm(0, i) != 0)
	       return;
	} else if (rndm(0, 5) != 0)
	    return;
        sprintf(buf,"Your %s%s %s.", hitter->name, buf2, op->name);
	play_sound_map(op->map, op->x, op->y, SOUND_PLAYER_HITS4);
	new_draw_info(NDI_BLACK, 0, hitter->owner, buf);
    }
}


static int get_attack_mode (object **target, object **hitter,
	int *simple_attack)
{
    if (QUERY_FLAG (*target, FLAG_FREED) || QUERY_FLAG (*hitter, FLAG_FREED)) {
        LOG (llevError, "BUG: get_attack_mode(): freed object\n");
        return 1;
    }
    if ((*target)->head)
        *target = (*target)->head;
    if ((*hitter)->head)
        *hitter = (*hitter)->head;
    if ((*hitter)->env != NULL || (*target)->env != NULL) {
        *simple_attack = 1;
        return 0;
    }
    if (QUERY_FLAG (*target, FLAG_REMOVED)
        || QUERY_FLAG (*hitter, FLAG_REMOVED)
        || (*hitter)->map == NULL || !on_same_map((*hitter), (*target)))
    {
        LOG (llevError, "BUG: hitter (arch %s, name %s) with no relation to "
             "target\n", (*hitter)->arch->name, (*hitter)->name);
        return 1;
    }
    *simple_attack = 0;
    return 0;
}

static int abort_attack (object *target, object *hitter, int simple_attack)
{
/* Check if target and hitter are still in a relation similar to the one
 * determined by get_attack_mode().  Returns true if the relation has changed.
 */
    int new_mode;

    if (hitter->env == target || target->env == hitter)
        new_mode = 1;
    else if (QUERY_FLAG (hitter, FLAG_REMOVED)
             || QUERY_FLAG (target, FLAG_REMOVED)
             || hitter->map == NULL || !on_same_map(hitter, target))
        return 1;
    else
        new_mode = 0;
    return new_mode != simple_attack;
}

static void thrown_item_effect (object *, object *);

static int attack_ob_simple (object *op, object *hitter, int base_dam,
	int base_wc)
{
    int simple_attack, roll, dam=0;
    uint32 type;
    const char *op_name = NULL;
    tag_t op_tag, hitter_tag;

    if (get_attack_mode (&op, &hitter, &simple_attack))
        goto error;

    /* Lauwenmark: Handle for plugin attack event */
    execute_event(op, EVENT_ATTACK,hitter,hitter,NULL,SCRIPT_FIX_ALL);

    /* Lauwenmark: This is used to handle script_weapons with weapons.
     * Only used for players.
     */
    if (hitter->type==PLAYER)
    {
        if (hitter->current_weapon != NULL)
        {
            /* Lauwenmark: Handle for plugin attack event */
            execute_event(hitter, EVENT_ATTACK,hitter->current_weapon,
                          op,NULL,SCRIPT_FIX_ALL);
        }
            }
    op_tag = op->count;
    hitter_tag = hitter->count;

    /*
     * A little check to make it more difficult to dance forward and back
     * to avoid ever being hit by monsters.
     */
    if ( ! simple_attack && QUERY_FLAG (op, FLAG_MONSTER)
        && op->speed_left > -(FABS(op->speed))*0.3)
    {
	/* Decrease speed BEFORE calling process_object.  Otherwise, an
	 * infinite loop occurs, with process_object calling move_monster,
	 * which then gets here again.  By decreasing the speed before
	 * we call process_object, the 'if' statement above will fail.
	 */
	op->speed_left--;
	process_object(op);
	if (was_destroyed (op, op_tag) || was_destroyed (hitter, hitter_tag)
            || abort_attack (op, hitter, simple_attack))
		goto error;
    }

    add_refcount(op_name = op->name);

    roll=random_roll(1, 20, hitter, PREFER_HIGH);

    /* Adjust roll for various situations. */
    if ( ! simple_attack)
        roll += adj_attackroll(hitter,op);

    /* See if we hit the creature */
    if(roll==20 || op->stats.ac>=base_wc-roll) {
	int hitdam = base_dam;
	if (settings.casting_time == TRUE) {
	    if ((hitter->type == PLAYER)&&(hitter->casting_time > -1)){
		hitter->casting_time = -1;
		new_draw_info(NDI_UNIQUE, 0,hitter,"You attacked and lost "
		    "your spell!");
	    }
	    if ((op->casting_time > -1)&&(hitdam > 0)){
		op->casting_time = -1;
		if (op->type == PLAYER)  {
		    new_draw_info(NDI_UNIQUE, 0,op,"You were hit and lost "
			"your spell!");
		    new_draw_info_format(NDI_ALL|NDI_UNIQUE,5,NULL,
			"%s was hit by %s and lost a spell.",
			op_name,hitter->name);
		}
	    }
	}
	if ( ! simple_attack)
        {
            /* If you hit something, the victim should *always* wake up.
             * Before, invisible hitters could avoid doing this. 
             * -b.t. */
            if (QUERY_FLAG (op, FLAG_SLEEP))
                CLEAR_FLAG(op,FLAG_SLEEP);

            /* If the victim can't see the attacker, it may alert others
             * for help. */
            if (op->type != PLAYER && ! can_see_enemy (op, hitter)
                && ! get_owner (op) && rndm(0, op->stats.Int))
                npc_call_help (op);

            /* if you were hidden and hit by a creature, you are discovered*/
            if (op->hide && QUERY_FLAG (hitter, FLAG_ALIVE)) {
                make_visible (op);
                if (op->type == PLAYER)
                    new_draw_info (NDI_UNIQUE, 0, op,
                                   "You were hit by a wild attack. "
                                   "You are no longer hidden!");
            }

            /* thrown items (hitter) will have various effects
             * when they hit the victim.  For things like thrown daggers,
             * this sets 'hitter' to the actual dagger, and not the
             * wrapper object.
             */
            thrown_item_effect (hitter, op);
            if (was_destroyed (hitter, hitter_tag)
                || was_destroyed (op, op_tag)
                || abort_attack (op, hitter, simple_attack))
                goto leave;
        }

	/* Need to do at least 1 damage, otherwise there is no point
	 * to go further and it will cause FPE's below.
	 */
	if (hitdam<=0) hitdam=1;

	type=hitter->attacktype;
	if(!type) type=AT_PHYSICAL;
	/* Handle monsters that hit back */
	if ( ! simple_attack && QUERY_FLAG (op, FLAG_HITBACK)
	    && QUERY_FLAG (hitter, FLAG_ALIVE))
	{
	    if (op->attacktype & AT_ACID && hitter->type==PLAYER)
		new_draw_info(NDI_UNIQUE, 0,hitter,"You are splashed by acid!\n");
	    hit_player(hitter, random_roll(0, (op->stats.dam), hitter,
				      PREFER_LOW),op, op->attacktype, 1);
	    if (was_destroyed (op, op_tag)
                || was_destroyed (hitter, hitter_tag)
                || abort_attack (op, hitter, simple_attack))
                goto leave;
	}

	/* In the new attack code, it should handle multiple attack
	 * types in its area, so remove it from here.
	 */
	dam=hit_player(op, random_roll(1, hitdam, hitter, PREFER_HIGH),
		       hitter, type, 1);
	if (was_destroyed (op, op_tag) || was_destroyed (hitter, hitter_tag)
            || abort_attack (op, hitter, simple_attack))
	    goto leave;
    } /* end of if hitter hit op */
    /* if we missed, dam=0 */

    /*attack_message(dam, type, op, hitter);*/

    goto leave;

  error:
    dam = 1;
    goto leave;

  leave:
    if (op_name)
        free_string (op_name);

    return dam;
}

int attack_ob (object *op, object *hitter)
{

    if (hitter->head)
        hitter = hitter->head;
    return attack_ob_simple (op, hitter, hitter->stats.dam, hitter->stats.wc);
}

/* op is the arrow, tmp is what is stopping the arrow.
 *
 * Returns 1 if op was inserted into tmp's inventory, 0 otherwise.
 */
static int stick_arrow (object *op, object *tmp)
{
    /* If the missile hit a player, we insert it in their inventory.
     * However, if the missile is heavy, we don't do so (assume it falls
     * to the ground after a hit).  What a good value for this is up to
     * debate - 5000 is 5 kg, so arrows, knives, and other light weapons
     * stick around.
     */
    if (op->weight <= 5000 && tmp->stats.hp >= 0) {
	if(tmp->head != NULL)
	    tmp = tmp->head;
        remove_ob (op);
	op = insert_ob_in_ob(op,tmp);
	if (tmp->type== PLAYER)
	    esrv_send_item (tmp, op);
        return 1;
    } else
	return 0;
}

/* hit_with_arrow() disassembles the missile, attacks the victim and
 * reassembles the missile.
 *
 * It returns a pointer to the reassembled missile, or NULL if the missile
 * isn't available anymore.
 */
object *hit_with_arrow (object *op, object *victim)
{
    object *container, *hitter;
    int hit_something;
    tag_t victim_tag, hitter_tag;
    sint16 victim_x, victim_y;

    /* Disassemble missile */
    if (op->inv) {
        container = op;
        hitter = op->inv;
        remove_ob (hitter);
        insert_ob_in_map(hitter, container->map,hitter,INS_NO_MERGE | INS_NO_WALK_ON);
        /* Note that we now have an empty THROWN_OBJ on the map.  Code that
         * might be called until this THROWN_OBJ is either reassembled or
         * removed at the end of this function must be able to deal with empty
         * THROWN_OBJs. */
    } else {
        container = NULL;
        hitter = op;
    }

    /* Try to hit victim */
    victim_x = victim->x;
    victim_y = victim->y;
    victim_tag = victim->count;
    hitter_tag = hitter->count;
    /* Lauwenmark: Handling plugin attack event for thrown items */
    if (execute_event(op, EVENT_ATTACK,hitter,victim,NULL,SCRIPT_FIX_ALL) == 0)
        hit_something = attack_ob_simple (victim, hitter, op->stats.dam,
                                        op->stats.wc);

    /* Arrow attacks door, rune of summoning is triggered, demon is put on
     * arrow, move_apply() calls this function, arrow sticks in demon,
     * attack_ob_simple() returns, and we've got an arrow that still exists
     * but is no longer on the map. Ugh. (Beware: Such things can happen at
     * other places as well!)
     */
    if (was_destroyed (hitter, hitter_tag) || hitter->env != NULL) {
        if (container) {
            remove_ob (container);
            free_object (container);
        }
        return NULL;
    }

    /* Missile hit victim */
    /* if the speed is > 10, then this is a fast moving arrow, we go straight
     * through the target 
     */
    if (hit_something && op->speed <= 10.0)
    {
        /* Stop arrow */
        if (container == NULL) {
            hitter = fix_stopped_arrow (hitter);
            if (hitter == NULL)
                return NULL;
        } else {
            remove_ob (container);
            free_object (container);
        }

        /* Try to stick arrow into victim */
        if ( ! was_destroyed (victim, victim_tag)
             && stick_arrow (hitter, victim))
            return NULL;

        /* Else try to put arrow on victim's map square
	 * remove check for P_WALL here.  If the arrow got to this
	 * space, that is good enough - with the new movement code,
	 * there is now the potential for lots of spaces where something
	 * can fly over but not otherwise move over.  What is the correct
	 * way to handle those otherwise?
	 */
        if (victim_x != hitter->x || victim_y != hitter->y) {
            remove_ob (hitter);
            hitter->x = victim_x;
            hitter->y = victim_y;
            insert_ob_in_map (hitter, victim->map, hitter,0);
        } else {
            /* Else leave arrow where it is */
            merge_ob (hitter, NULL);
        }
        return NULL;
    }

    if (hit_something && op->speed >= 10.0)
	op->speed -= 1.0;

    /* Missile missed victim - reassemble missile */
    if (container) {
        remove_ob (hitter);
        insert_ob_in_ob (hitter, container);
    }
    return op;
}


void tear_down_wall(object *op)
{
    int perc=0;

    if (!op->stats.maxhp) {
	LOG(llevError, "TEAR_DOWN wall %s had no maxhp.\n", op->name);
	perc = 1;
    } else if(!GET_ANIM_ID(op)) {
	/* Object has been called - no animations, so remove it */
	if(op->stats.hp<0) {
	    remove_ob(op); /* Should update LOS */
	    free_object(op);
	    /* Don't know why this is here - remove_ob should do it for us */
	    /*update_position(m, x, y);*/
	}
	return;	/* no animations, so nothing more to do */
    }
    perc = NUM_ANIMATIONS(op)
	- ((int)NUM_ANIMATIONS(op)*op->stats.hp)/op->stats.maxhp;
    if (perc >= (int) NUM_ANIMATIONS(op))
	perc = NUM_ANIMATIONS(op)-1;
    else if (perc < 1)
	perc = 1;
    SET_ANIMATION(op, perc);
    update_object(op,UP_OBJ_FACE);
    if(perc==NUM_ANIMATIONS(op)-1) { /* Reached the last animation */
	if(op->face==blank_face) {
	    /* If the last face is blank, remove the ob */
	    remove_ob(op); /* Should update LOS */
	    free_object(op);

	    /* remove_ob should call update_position for us */
	    /*update_position(m, x, y);*/

	} else { /* The last face was not blank, leave an image */
	    CLEAR_FLAG(op, FLAG_BLOCKSVIEW);
	    update_all_los(op->map, op->x, op->y);
	    op->move_block = 0;
	    CLEAR_FLAG(op, FLAG_ALIVE);
	}
    }
}

void scare_creature(object *target, object *hitter)
{
    object *owner = get_owner(hitter);

    if (!owner) owner=hitter;

    SET_FLAG(target, FLAG_SCARED);
    if (!target->enemy) target->enemy=owner;
}


/* This returns the amount of damage hitter does to op with the
 * appropriate attacktype.  Only 1 attacktype should be set at a time.
 * This doesn't damage the player, but returns how much it should
 * take.  However, it will do other effects (paralyzation, slow, etc.)
 * Note - changed for PR code - we now pass the attack number and not
 * the attacktype.  Makes it easier for the PR code.  */

int hit_player_attacktype(object *op, object *hitter, int dam, 
	uint32 attacknum, int magic) {
  
    int doesnt_slay = 1;

    /* Catch anyone that may be trying to send us a bitmask instead of the number */
    if (attacknum >= NROFATTACKS) {
	LOG(llevError, "hit_player_attacktype: Invalid attacknumber passed: %u\n", attacknum);
	return 0;
    }
    
    if (dam < 0) {
	LOG(llevError,"hit_player_attacktype called with negative damage: %d\n", dam);
	return 0;
    }
    
    /* AT_INTERNAL is supposed to do exactly dam.  Put a case here so
     * people can't mess with that or it otherwise get confused.  */
    if (attacknum == ATNR_INTERNAL) return dam;
    
    if (hitter->slaying) {
      if(((op->race != NULL) && strstr(hitter->slaying, op->race)) ||
	 (op->arch && (op->arch->name != NULL) && 
	  strstr(op->arch->name, hitter->slaying))
	 ){
	doesnt_slay = 0;
	dam *= 3;
      }
    }

    /* Adjust the damage for resistance. Note that neg. values increase damage. */
    if (op->resist[attacknum]) {
      /* basically:  dam = dam*(100-op->resist[attacknum])/100;
       * in case 0>dam>1, we try to "simulate" a float value-effect */
      dam *= (100-op->resist[attacknum]);
      if (dam >= 100) dam /= 100;
      else
	dam = (dam > (random_roll(0, 99, op, PREFER_LOW))) ? 1 : 0;
    }

    /* Special hack.  By default, if immune to something, you
     * shouldn't need to worry.  However, acid is an exception, since
     * it can still damage your items.  Only include attacktypes if
     * special processing is needed */
    
    if ((op->resist[attacknum] >= 100) && 
	doesnt_slay && (attacknum != ATNR_ACID))
      return 0;

    /* Keep this in order - makes things easier to find */
    
    switch(attacknum) {
    case ATNR_PHYSICAL:
	/*  here also check for diseases */
	check_physically_infect(op, hitter);
	break;
	
	/* Don't need to do anything for:
	     magic, 
	     fire, 
	     electricity,
	     cold */
	
    case ATNR_CONFUSION:
    case ATNR_POISON:
    case ATNR_SLOW:
    case ATNR_PARALYZE:
    case ATNR_FEAR:
    case ATNR_CANCELLATION:
    case ATNR_DEPLETE:
    case ATNR_BLIND:
	{
	    /* chance for inflicting a special attack depends on the
	     * difference between attacker's and defender's level 
	     */
	    int level_diff = MIN(110, MAX(0, op->level - hitter->level));

	    /* First, only creatures/players with speed can be affected.
	     * Second, just getting hit doesn't mean it always affects
	     * you.  Third, you still get a saving through against the
	     * effect.  
	     */
	    if (op->speed && 
		(QUERY_FLAG(op, FLAG_MONSTER) || op->type==PLAYER) &&
		!(rndm(0, (attacknum == ATNR_SLOW?6:3)-1)) &&
		!did_make_save(op, level_diff,  op->resist[attacknum]/10)) {

		/* Player has been hit by something */
		if (attacknum == ATNR_CONFUSION) confuse_player(op,hitter,dam);
		else if (attacknum == ATNR_POISON) poison_player(op,hitter,dam);
		else if (attacknum == ATNR_SLOW) slow_player(op,hitter,dam);
		else if (attacknum == ATNR_PARALYZE) paralyze_player(op,hitter,dam);
		else if (attacknum == ATNR_FEAR) scare_creature(op, hitter);
		else if (attacknum == ATNR_CANCELLATION) cancellation(op);
		else if (attacknum == ATNR_DEPLETE) drain_stat(op);
		else if (attacknum == ATNR_BLIND  && !QUERY_FLAG(op,FLAG_UNDEAD) &&
		   !QUERY_FLAG(op,FLAG_GENERATOR)) blind_player(op,hitter,dam);
	    }
	    dam = 0; /* These are all effects and don't do real damage */
	}
	break;
    case ATNR_ACID:
      {
	int flag=0;

	/* Items only get corroded if you're not on a battleground and
         * if your acid resistance is below 50%. */
	if (!op_on_battleground(op, NULL, NULL) && 
	    (op->resist[ATNR_ACID] < 50))
	  {
	    object *tmp;
	    for(tmp=op->inv; tmp!=NULL; tmp=tmp->below) {
		if(tmp->invisible)
		  continue;
		if(!QUERY_FLAG(tmp, FLAG_APPLIED) ||
		   (tmp->resist[ATNR_ACID] >= 10))
		  /* >= 10% acid res. on itmes will protect these */
		  continue;
		if(!(tmp->material & M_IRON))
		  continue;
		if(tmp->magic < -4) /* Let's stop at -5 */
		  continue;
		if(tmp->type==RING || 
		   /* removed boots and gloves from exclusion list in
                      PR */
		   tmp->type==GIRDLE ||
		   tmp->type==AMULET ||
		   tmp->type==WAND ||
		   tmp->type==ROD ||
		   tmp->type==HORN)
		  continue; /* To avoid some strange effects */

		/* High damage acid has better chance of corroding
                   objects */
		if(rndm(0, dam+4) >
		    random_roll(0, 39, op, PREFER_HIGH)+2*tmp->magic) {
		    if(op->type == PLAYER)
			/* Make this more visible */
			new_draw_info_format(NDI_UNIQUE|NDI_RED,0, op,
				 "The %s's acid corrodes your %s!",
				 query_name(hitter), query_name(tmp));
		    flag = 1;
		    tmp->magic--;
		    if(op->type == PLAYER)
			esrv_send_item(op, tmp);
		}
	    }
	    if(flag)
	      fix_player(op); /* Something was corroded */
	}
      }
      break;
    case ATNR_DRAIN:
      {
	/* rate is the proportion of exp drained.  High rate means
	 * not much is drained, low rate means a lot is drained.
	 */
	int rate;
	
	if(op->resist[ATNR_DRAIN] >= 0) 
	  rate = 50 + op->resist[ATNR_DRAIN] / 2;
	else if(op->resist[ATNR_DRAIN] < 0)
	  rate = 5000 / (100 - op->resist[ATNR_DRAIN]);
	
	/* full protection has no effect.  Nothing else in this
	 * function needs to get done, so just return.  */
	if(!rate)
	    return 0;

	if(op->stats.exp <= rate) {
	    if(op->type == GOLEM)
		dam = 999; /* Its force is "sucked" away. 8) */
	    else 
	      /* If we can't drain, lets try to do physical damage */
		dam = hit_player_attacktype(op, hitter, dam, ATNR_PHYSICAL, magic);
	} else {
	    /* Randomly give the hitter some hp */
	    if(hitter->stats.hp<hitter->stats.maxhp &&
		(op->level > hitter->level) &&
		random_roll(0, (op->level-hitter->level+2), hitter, PREFER_HIGH)>3)
		hitter->stats.hp++;

	    /* Can't do drains on battleground spaces.
	     * Move the wiz check up here - before, the hitter wouldn't gain exp
	     * exp, but the wiz would still lose exp!  If drainee is a wiz,
	     * nothing happens.
	     * Try to credit the owner.  We try to display player -> player drain
	     * attacks, hence all the != PLAYER checks.
	     */
	    if (!op_on_battleground(hitter, NULL, NULL) && !QUERY_FLAG(op,FLAG_WAS_WIZ)) {
		object *owner = get_owner(hitter);

		if (owner && owner != hitter) {
		    if (op->type != PLAYER || owner->type != PLAYER)
			change_exp(owner, op->stats.exp/(rate*2), 
				      hitter->chosen_skill? hitter->chosen_skill->skill:NULL, SK_EXP_TOTAL);
		} else if (op->type != PLAYER || hitter->type != PLAYER) {
		    change_exp(hitter, op->stats.exp/(rate*2), 
			      hitter->chosen_skill?hitter->chosen_skill->skill:NULL, 0);
		}
		change_exp(op,-op->stats.exp/rate, NULL, 0);
	    } 
	    dam = 1;	/* Drain is an effect.  Still return 1 - otherwise, if you have pure
			 * drain attack, you won't know that you are actually sucking out EXP,
			 * as the messages will say you missed
			 */
	}
      } 
      break;
    case ATNR_TURN_UNDEAD:
      {
	if (QUERY_FLAG(op,FLAG_UNDEAD)) {
	    object *owner = get_owner(hitter) == NULL ? hitter : get_owner(hitter);
            object *god = find_god (determine_god (owner));
            int div = 1;

	    /* if undead are not an enemy of your god, you turn them
             * at half strength */
            if (! god || ! god->slaying ||
		 strstr (god->slaying, undead_name) == NULL)
                div = 2;
	    /* Give a bonus if you resist turn undead */
	    if (op->level * div <
		(turn_bonus[owner->stats.Wis]+owner->level +
		 (op->resist[ATNR_TURN_UNDEAD]/100)))
		scare_creature(op, owner);
	}
	else 
	  dam = 0; /* don't damage non undead - should we damage
                      undead? */
      } break;
    case ATNR_DEATH:
	deathstrike_player(op, hitter, &dam);
	break;
    case ATNR_CHAOS:
	LOG(llevError,
	    "%s was hit by %s with non-specific chaos.\n",
	    query_name(op),
	    query_name(hitter));
	dam = 0;
	break;
    case ATNR_COUNTERSPELL: 
	LOG(llevError,
	    "%s was hit by %s with counterspell attack.\n",
	    query_name(op),
	    query_name(hitter));
	dam = 0;
	/* This should never happen.  Counterspell is handled
	 * seperately and filtered out.  If this does happen,
	 * Counterspell has no effect on anything but spells, so it
	 * does no damage. */
	break;
    case ATNR_HOLYWORD:
      {
	/* This has already been handled by hit_player, 
	 *  no need to check twice  -- DAMN */
	
	object *owner = get_owner(hitter)==NULL?hitter:get_owner(hitter);
	
	/* As with turn undead above, give a bonus on the saving throw */
	if((op->level+(op->resist[ATNR_HOLYWORD]/100)) <
	   owner->level+turn_bonus[owner->stats.Wis])
	    scare_creature(op, owner);
      } break;
    case ATNR_LIFE_STEALING:
      {
	int new_hp;
	/* this is replacement to drain for players, instead of taking
	 * exp it takes hp. It is geared for players, probably not
	 * much use giving it to monsters
	 *
	 * life stealing doesn't do a lot of damage, but it gives the
	 * damage it does do to the player.  Given that,
	 * it only does 1/10'th normal damage (hence the divide by
	 * 1000).
	 */
	/* You can't steal life from something undead */
	if ((op->type == GOLEM) || (QUERY_FLAG(op, FLAG_UNDEAD))) return 0;
	/* If drain protection is higher than life stealing, use that */
	if (op->resist[ATNR_DRAIN] >= op->resist[ATNR_LIFE_STEALING])
	  dam = (dam*(100 - op->resist[ATNR_DRAIN])) / 3000;
	else dam = (dam*(100 - op->resist[ATNR_LIFE_STEALING])) / 3000;
	/* You die at -1 hp, not zero. */
	if (dam > (op->stats.hp+1)) dam = op->stats.hp+1;
	new_hp = hitter->stats.hp + dam;
	if (new_hp > hitter->stats.maxhp) new_hp = hitter->stats.maxhp;
	if (new_hp > hitter->stats.hp) hitter->stats.hp = new_hp;
      }
    }
    return dam;
}


/* GROS: This code comes from hit_player. It has been made external to
 * allow script procedures to "kill" objects in a combat-like fashion.
 * It was initially used by (kill-object) developed for the Collector's
 * Sword. Note that nothing has been changed from the original version
 * of the following code.
 * op is what is being killed.
 * dam is the damage done to it.
 * hitter is what is hitting it.
 * type is the attacktype.
 *
 * This function was a bit of a mess with hitter getting changed,
 * values being stored away but not used, etc.  I've cleaned it up
 * a bit - I think it should be functionally equivalant.
 * MSW 2002-07-17
 */
int kill_object(object *op,int dam, object *hitter, int type)
{
    char buf[MAX_BUF];
    const char *skill;
    int maxdam=0;
    int battleg=0;    /* true if op standing on battleground */
    int pk=0;         /* true if op and what controls hitter are both players*/
    object *owner=NULL;
    object *skop=NULL;

    if (op->stats.hp>=0)
	return -1;

    /* Lauwenmark: Handle for plugin death event */
    if (execute_event(op, EVENT_DEATH,hitter,NULL,NULL,SCRIPT_FIX_ALL)!=0)
                return 0;
    /* Lauwenmark: Handle for the global kill event */
    execute_global_event(EVENT_GKILL, op, hitter);

    /* maxdam needs to be the amount of damage it took to kill
     * this creature.  The function(s) that call us have already
     * adjusted the creatures HP total, so that is negative.
     */
    maxdam = dam + op->stats.hp + 1;

    if(QUERY_FLAG(op,FLAG_BLOCKSVIEW))
	update_all_los(op->map,op->x, op->y); /* makes sure los will be recalculated */

    if(op->type==DOOR) {
	op->speed = 0.1;
	update_ob_speed(op);
	op->speed_left= -0.05;
	return maxdam;	    
    }
    if(QUERY_FLAG (op, FLAG_FRIENDLY) && op->type != PLAYER) {
	remove_friendly_object(op);
	if (get_owner (op) != NULL && op->owner->type == PLAYER && 
	    op->owner->contr->ranges[range_golem] == op) {
	    op->owner->contr->ranges[range_golem]=NULL;
	    op->owner->contr->golem_count=0;
	}
	else
	    LOG (llevError, "BUG: hit_player(): Encountered golem without owner.\n");

	remove_ob(op);
	free_object(op);
	return maxdam;
    }

    /* Now lets start dealing with experience we get for killing something */

    owner=get_owner(hitter);
    if(owner==NULL)
	owner=hitter;

    /* is the victim (op) standing on battleground? */
    if (op_on_battleground(op, NULL, NULL)) battleg=1;
    
    /* is this player killing?*/
    if (op->type == PLAYER && owner->type == PLAYER) pk=1;

    /* Player killed something */
    if(owner->type==PLAYER) {
	Log_Kill(owner->name,
		 query_name(op),op->type,
                (owner!=hitter) ? query_name(hitter) : NULL,
                (owner!=hitter) ? hitter->type : 0);

	/* Log players killing other players - makes it easier to detect
	 * and filter out malicious player killers - that is why the
	 * ip address is included.
	 */
	if (op->type == PLAYER && !battleg)  {
	    time_t t=time(NULL);
	    struct tm *tmv;
	    char buf[256];

	    tmv = localtime(&t);
	    strftime(buf, 256,"%a %b %d %H:%M:%S %Y", tmv);

	    LOG(llevInfo,"%s PLAYER_KILL_PLAYER: %s (%s) killed %s\n",
		buf, owner->name, owner->contr->socket.host, query_name(op));
	}

	/* try to filter some things out - basically, if you are
	 * killing a level 1 creature and your level 20, you
	 * probably don't want to see that.
	 */
	if ( owner->level < op->level * 2|| op->stats.exp>1000) {
	    if(owner!=hitter) {
		new_draw_info_format(NDI_BLACK, 0, owner,
			"You killed %s with %s.",query_name(op),
			 query_name(hitter));
	    }
	    else {
		new_draw_info_format(NDI_BLACK, 0, owner, 
			"You killed %s.",query_name(op));
	    }
	    /* Only play sounds for melee kills */
	    if (hitter->type == PLAYER)
		play_sound_map(owner->map, owner->x, owner->y, SOUND_PLAYER_KILLS);
	}

	/* If a player kills another player, not on
	 * battleground, the "killer" looses 1 luck. Since this is
	 * not reversible, it's actually quite a pain IMHO. -AV 
	 * Fix bug in that we were changing the luck of the hitter, not
	 * player that the object belonged to - so if you killed another player
	 * with spells, pets, whatever, there was no penalty.
	 * Changed to make luck penalty configurable in settings.
	 */
	if(op->type == PLAYER && owner != op && !battleg)
	    change_luck(owner, -settings.pk_luck_penalty);

	/* This code below deals with finding the appropriate skill
	 * to credit exp to.  This is a bit problematic - we should
         * probably never really have to look at current_weapon->skill
	 */
	skill = NULL;
	if (hitter->skill && hitter->type!=PLAYER) skill = hitter->skill;
	else if (owner->chosen_skill) {
	    skill = owner->chosen_skill->skill;
	    skop = owner->chosen_skill;
	}
	else if (QUERY_FLAG(owner, FLAG_READY_WEAPON)) skill=owner->current_weapon->skill;
	else 
	    LOG(llevError,"kill_object - unable to find skill that killed monster\n");

	/* We have the skill we want to credit to - now find the object this goes
	 * to.  Make sure skop is an actual skill, and not a skill tool!
	 */
	if ((!skop || skop->type != SKILL) && skill) {
	    int i;

	    for (i=0; i<NUM_SKILLS; i++)
                if (owner->contr->last_skill_ob[i] &&
		  !strcmp(owner->contr->last_skill_ob[i]->skill, skill)) {
		    skop = owner->contr->last_skill_ob[i];
		    break;
		}
	}
    } /* Was it a player that hit somethign */
    else {
	skill = NULL;
    }

    /* Pet (or spell) killed something. */
    if(owner != hitter ) {
	(void) sprintf(buf,"%s killed %s with %s%s%s.",owner->name,
	       query_name(op),query_name(hitter), battleg? " (duel)":"", pk? " (pk)":"");
    }
    else {
	(void) sprintf(buf,"%s killed %s in hand to hand combat %s%s.",hitter->name,op->name,
	       battleg? " (duel)":"", pk? " (pk)":"");
    }
    /* These may have been set in the player code section above */
    if (!skop) skop = hitter->chosen_skill;
    if (!skill && skop) skill=skop->skill;

    new_draw_info(NDI_ALL, op->type==PLAYER?1:10, NULL, buf);


    /* If you didn't kill yourself, and your not the wizard */
    if(owner!=op && !QUERY_FLAG(op, FLAG_WAS_WIZ)) {
	int exp;

	exp = calc_skill_exp(owner,op, skop);

	/* Really don't give much experience for killing other players */
	if (op->type==PLAYER) {
	    if (battleg) {
		new_draw_info(NDI_UNIQUE, 0,owner, "Your foe has fallen!");
		new_draw_info(NDI_UNIQUE, 0,owner, "VICTORY!!!");
	    }
	    else
		exp = MIN(5000000, MAX(0, exp/10));
	}

	/* Don't know why this is set this way - doesn't make
	 * sense to just divide everything by two for no reason.
	 */

	if (!settings.simple_exp)
	    exp=exp/2;

	/* if op is standing on "battleground" (arena), no way to gain
	 * exp by killing him 
	 */
	if (battleg) exp = 0;

	if(owner->type!=PLAYER || owner->contr->party==NULL) {
	    change_exp(owner,exp, skill, 0);
	}
	else {
	    int shares=0,count=0;

	    player *pl;

	    partylist *party=owner->contr->party;
#ifdef PARTY_KILL_LOG
	    add_kill_to_party(party, query_name(owner), query_name(op), exp);
#endif
	    for(pl=first_player;pl!=NULL;pl=pl->next) {
		if(party && pl->ob->contr->party==party && on_same_map(pl->ob, owner)) {
		    count++;
		    shares+=(pl->ob->level+4);
		}
	    }
	    if(count==1 || shares>exp)
		change_exp(owner,exp, skill, SK_EXP_TOTAL);
	    else {
		int share=exp/shares,given=0,nexp;
		for(pl=first_player;pl!=NULL;pl=pl->next) {
		    if(party && pl->ob->contr->party==party && on_same_map(pl->ob, owner)) {
			nexp=(pl->ob->level+4)*share;
			change_exp(pl->ob,nexp, skill, SK_EXP_TOTAL);
			given+=nexp;
		    }
		}
		exp-=given;
		/* give any remainder to the player */
		change_exp(owner,exp, skill, SK_EXP_ADD_SKILL);
	    }
	} /* else part of a party */

    } /* end if person didn't kill himself */

    if(op->type!=PLAYER) {
	if(QUERY_FLAG(op,FLAG_FRIENDLY)) {
	    object *owner1 = get_owner(op);

	    if(owner1!= NULL && owner1->type == PLAYER) {
		play_sound_player_only(owner1->contr, SOUND_PET_IS_KILLED,0,0);
		/* Maybe we should include the owner that killed this, maybe not */
		new_draw_info_format(NDI_UNIQUE, 0,owner1,"Your pet, the %s, is killed by %s.",
				     op->name,hitter->name);
	    }
	    remove_friendly_object(op);
	}
	remove_ob(op);
	free_object(op);
    }
    /* Player has been killed! */
    else {
	if(owner->type==PLAYER) {
	    snprintf(op->contr->killer, BIG_NAME, "%s the %s",owner->name,owner->contr->title);
	}
	else {
	    strncpy(op->contr->killer,hitter->name,BIG_NAME);
	    op->contr->killer[BIG_NAME-1]='\0';
	}
    }
    /* This was return -1 - that doesn't seem correct - if we return -1, process
     * continues in the calling function.
     */
    return maxdam;
}

/* Find out if this is friendly fire (PVP and attacker is peaceful) or not 
 *  Returns 0 this is not friendly fire
 */

int friendly_fire(object *op, object *hitter){
    object *owner;
    int friendlyfire;
	
    if (hitter->head) hitter=hitter->head;

    friendlyfire = 0;
	
    if(op->type == PLAYER) {
			
	if(hitter->type == PLAYER && hitter->contr->peaceful == 1)
	    return 1;
		    
	if((owner = get_owner(hitter))!=NULL) {
	    if(owner->type == PLAYER && owner->contr->peaceful == 1)
		friendlyfire = 2;
	}
			
	if (hitter->type == SPELL || hitter->type == POISONING || 
	    hitter->type == DISEASE || hitter->type == RUNE)
	    friendlyfire = 0;			
    }
    return friendlyfire;
}


/* This isn't used just for players, but in fact most objects.
 * op is the object to be hit, dam is the amount of damage, hitter
 * is what is hitting the object, type is the attacktype, and
 * full_hit is set if monster area does not matter.
 * dam is base damage - protections/vulnerabilities/slaying matches can
 * modify it.
 */

  /* Oct 95 - altered the following slightly for MULTIPLE_GODS hack
   * which needs new attacktype AT_HOLYWORD to work . b.t. */

int hit_player(object *op,int dam, object *hitter, int type, int full_hit) {
    int maxdam=0, ndam=0, attacktype=1, magic=(type & AT_MAGIC);
    int maxattacktype, attacknum;
    int body_attack = op && op->head;   /* Did we hit op's head? */
    int simple_attack;
    tag_t op_tag, hitter_tag;
    int rtn_kill = 0;
    int friendlyfire;

    if (get_attack_mode (&op, &hitter, &simple_attack))
        return 0;

    /* very simple: if our target has no_damage 1 set or is wiz, we can't hurt him */
    if (QUERY_FLAG (op, FLAG_WIZ) || QUERY_FLAG (op, FLAG_NO_DAMAGE))
        return 0;

    op_tag = op->count;
    hitter_tag = hitter->count;

    if (body_attack) {
	/* slow and paralyze must hit the head.  But we don't want to just
	 * return - we still need to process other attacks the spell still
	 * might have.  So just remove the paralyze and slow attacktypes,
	 * and keep on processing if we have other attacktypes.
	 * return if only magic or nothing is left - under normal code
	 * we don't attack with pure magic if there is another attacktype.
	 * Only do processing if the initial attacktype includes one of those
	 * attack so we don't cancel out things like magic bullet.
	 */
	if (type & (AT_PARALYZE | AT_SLOW)) {
	    type &= ~(AT_PARALYZE | AT_SLOW);
	    if (!type || type==AT_MAGIC) return 0;
	}
    }

    if ( ! simple_attack && op->type == DOOR) {
        object *tmp;
        for (tmp = op->inv; tmp != NULL; tmp = tmp->below)
            if (tmp->type == RUNE || tmp->type == TRAP) {
                spring_trap (tmp, hitter);
                if (was_destroyed (hitter, hitter_tag)
                    || was_destroyed (op, op_tag)
                    || abort_attack (op, hitter, simple_attack))
                    return 0;
                break;
            }
    }

    if ( ! QUERY_FLAG (op, FLAG_ALIVE) || op->stats.hp < 0) {
        /* FIXME: If a player is killed by a rune in a door, the
         * was_destroyed() check above doesn't return, and might get here.
         */
        LOG (llevDebug, "victim (arch %s, name %s) already dead in "
             "hit_player()\n", op->arch->name, op->name);
	return 0;
    }

#ifdef ATTACK_DEBUG
    LOG(llevDebug,"hit player: attacktype %d, dam %d\n", type, dam);
#endif

    if (magic) {
      /* basically:  dam = dam*(100-op->resist[attacknum])/100;
       * in case 0>dam>1, we try to "simulate" a float value-effect */
      dam = dam*(100-op->resist[ATNR_MAGIC]);
      if (dam >= 100)
	dam /= 100;
      else
	dam = (dam > (rndm(0, 99))) ? 1 : 0;
    }

    /* AT_CHAOS here is a weapon or monster.  Spells are handled by hit_map
     * If the attacktype still has chaos, shuffle it, then clear the Chaos bit
     */
    if(type & AT_CHAOS){
      shuffle_attack(op,0);  /*0 flag tells it to not change the face */
      update_object(op,UP_OBJ_FACE);
      type &= ~AT_CHAOS;
    }

    /* Holyword is really an attacktype modifier (like magic is).  If
     * holyword is part of an attacktype, then make sure the creature is
     * a proper match, otherwise no damage.
     */
    if (type & AT_HOLYWORD) {
        object *god;
	if ((!hitter->slaying || 
	     (!(op->race && strstr(hitter->slaying,op->race)) &&
	     !(op->name && strstr(hitter->slaying,op->name)))) &&
	    (!QUERY_FLAG(op,FLAG_UNDEAD) ||
	     (hitter->title != NULL
              && (god = find_god(determine_god(hitter))) != NULL
              && god->race != NULL
	      && strstr(god->race,undead_name) != NULL)))
	    return 0;
    }

    maxattacktype = type; /* initialize this to something */
    for (attacknum=0; attacknum<NROFATTACKS; attacknum++, attacktype=1<<attacknum) {
	/* Magic isn't really a true attack type - it gets combined with other
	 * attack types.  As such, skip it over.  However, if magic is
	 * the only attacktype in the group, then still attack with it
	 */
	if ((attacktype==AT_MAGIC) && (type & ~AT_MAGIC)) continue;

	/* Go through and hit the player with each attacktype, one by one.
	 * hit_player_attacktype only figures out the damage, doesn't inflict
	 * it.  It will do the appropriate action for attacktypes with
	 * effects (slow, paralization, etc.
         */
	if (type & attacktype) {
	    ndam=hit_player_attacktype(op,hitter,dam,attacknum,magic);
	    /* the >= causes us to prefer messages from special attacks, if
	     * the damage is equal.
	     */
	    if (ndam >= maxdam) {
	      maxdam = ndam;
	      maxattacktype = 1<<attacknum;
	    }
	}
    }
	
    /* if this is friendly fire then do a set % of damage only
     * Note - put a check in to make sure this attack is actually
     * doing damage - otherwise, the +1 in the coe below will make
     * an attack do damage before when it otherwise didn't
     */
    friendlyfire = friendly_fire(op, hitter);
    if (friendlyfire && maxdam){
	maxdam = ((dam * settings.set_friendly_fire) / 100)+1;
			
#ifdef ATTACK_DEBUG
	LOG(llevDebug,"Friendly fire (type:%d setting: %d%) did %d damage dropped to %d\n",
	friendlyfire, settings.set_friendly_fire, dam, maxdam);
#endif
    }

    if (!full_hit) {
	archetype *at;
	int area;
	int remainder;

	area = 0;
	for(at = op->arch; at != NULL; at = at->more)
	    area++;
	assert(area > 0);

	/* basically: maxdam /= area; we try to "simulate" a float
	   value-effect */
	remainder = 100*(maxdam%area)/area;
	maxdam /= area;
	if (RANDOM()%100 < remainder)
	    maxdam++;
    }

#ifdef ATTACK_DEBUG
    LOG(llevDebug,"Attacktype %d did %d damage\n", type, maxdam);
#endif

    if(get_owner(hitter))
	op->enemy=hitter->owner;
    else if (QUERY_FLAG(hitter,FLAG_ALIVE))
	op->enemy=hitter;

    if(QUERY_FLAG(op,FLAG_UNAGGRESSIVE) && op->type != PLAYER) {
	/* The unaggressives look after themselves 8) */
	CLEAR_FLAG(op, FLAG_UNAGGRESSIVE);
	npc_call_help(op);
    }

    if (magic && did_make_save(op, op->level, 0))
	maxdam=maxdam/2;

    attack_message(maxdam, maxattacktype, op, hitter);

    op->stats.hp-=maxdam;

    /* Eneq(@csd.uu.se): Check to see if monster runs away. */
    if ((op->stats.hp>=0) &&
	(QUERY_FLAG(op, FLAG_MONSTER) || op->type == PLAYER) &&
	op->stats.hp<(signed short)(((float)op->run_away/(float)100)*
	(float)op->stats.maxhp)) {

	    if (QUERY_FLAG(op, FLAG_MONSTER))
		SET_FLAG(op, FLAG_RUN_AWAY);
	    else
		scare_creature(op, hitter);
    }

    if(QUERY_FLAG(op,FLAG_TEAR_DOWN)) {
	if (maxdam)
	    tear_down_wall(op);
	return maxdam;	/* nothing more to do for wall */
    }

    /* See if the creature has been killed */
    rtn_kill = kill_object(op, maxdam, hitter, type);
    if (rtn_kill != -1)
	return rtn_kill;


    /* Used to be ghosthit removal - we now use the ONE_HIT flag.  Note
     * that before if the player was immune to ghosthit, the monster
     * remained - that is no longer the case.
     */
    if(QUERY_FLAG(hitter, FLAG_ONE_HIT)) {
	if(QUERY_FLAG(hitter,FLAG_FRIENDLY))
	    remove_friendly_object(hitter);
	remove_ob(hitter);
	free_object(hitter);
    } 
    /* Lets handle creatures that are splitting now */
    else if(type&AT_PHYSICAL&&!QUERY_FLAG(op, FLAG_FREED)&&QUERY_FLAG(op,FLAG_SPLITTING)) {
	int i;
	int friendly = QUERY_FLAG(op,FLAG_FRIENDLY);
	int unaggressive = QUERY_FLAG(op, FLAG_UNAGGRESSIVE);
	object *owner = get_owner(op);

	if(!op->other_arch) {
	    LOG(llevError,"SPLITTING without other_arch error.\n");
	    return maxdam;
	}
	remove_ob(op);
	for(i=0;i<NROFNEWOBJS(op);i++) { /* This doesn't handle op->more yet */
	    object *tmp=arch_to_object(op->other_arch);
	    int j;

	    tmp->stats.hp=op->stats.hp;
	    if (friendly) {
		SET_FLAG(tmp, FLAG_FRIENDLY);
		add_friendly_object(tmp);
		tmp->attack_movement = PETMOVE;
		if (owner!=NULL)
		    set_owner(tmp,owner);
	    }
	    if (unaggressive)
		SET_FLAG(tmp, FLAG_UNAGGRESSIVE);
	    j=find_first_free_spot(tmp,op->map,op->x,op->y);
	    if (j==-1) /* No spot to put this monster */
		free_object(tmp);
	    else {
		tmp->x=op->x+freearr_x[j],tmp->y=op->y+freearr_y[j];
		insert_ob_in_map(tmp,op->map,NULL,0);
	    }
	}
	if(friendly)
	    remove_friendly_object(op);
	free_object(op);
    }
    else if(type & AT_DRAIN &&  hitter->type==GRIMREAPER&&hitter->value++>10) {
	remove_ob(hitter);
	free_object(hitter);
    }
    return maxdam;
}


void poison_player(object *op, object *hitter, int dam)
{
    archetype *at = find_archetype("poisoning");
    object *tmp=present_arch_in_ob(at,op);

    if(tmp==NULL) {
	if((tmp=arch_to_object(at))==NULL)
	    LOG(llevError,"Failed to clone arch poisoning.\n");
	else {
	    tmp = insert_ob_in_ob(tmp,op);
	    /*  peterm:  give poisoning some teeth.  It should
	     * be able to kill things better than it does:
	     * damage should be dependent something--I choose to
	     * do this:  if it's a monster, the damage from the
	     * poisoning goes as the level of the monster/2.
	     * If anything else, goes as damage. 
	     */

	    if(QUERY_FLAG(hitter,FLAG_ALIVE))
		tmp->stats.dam += hitter->level/2;
	    else
		tmp->stats.dam = dam;

	    copy_owner(tmp,hitter);   /*  so we get credit for poisoning kills */
	    if(hitter->skill && hitter->skill != tmp->skill) {
		if (tmp->skill) free_string(tmp->skill);
		tmp->skill = add_refcount(hitter->skill);
	    }

	    tmp->stats.food+=dam;  /*  more damage, longer poisoning */

	    if(op->type==PLAYER) {
		/* player looses stats, maximum is -10 of each */
		tmp->stats.Con= MAX(-(dam/4+1), -10);
		tmp->stats.Str= MAX(-(dam/3+2), -10);
		tmp->stats.Dex= MAX(-(dam/6+1), -10);
		tmp->stats.Int= MAX(-dam/7, -10);
		SET_FLAG(tmp,FLAG_APPLIED);
		fix_player(op);
		new_draw_info(NDI_UNIQUE, 0,op,"You suddenly feel very ill.");
	    }
	    if (hitter->type == PLAYER)
		new_draw_info_format(NDI_UNIQUE, 0, hitter, "You poison %s.",
		     op->name);
	    else if (get_owner(hitter) != NULL && hitter->owner->type == PLAYER)
		new_draw_info_format(NDI_UNIQUE, 0, hitter->owner,
		     "Your %s poisons %s.", hitter->name, op->name);
	}
	tmp->speed_left=0;
    }
    else
	tmp->stats.food++;
}

void slow_player(object *op,object *hitter,int dam)
{    archetype *at = find_archetype("slowness");
    object *tmp;
    if(at == NULL) {
      LOG(llevError,"Can't find slowness archetype.\n");
    }
    if((tmp=present_arch_in_ob(at,op)) == NULL) {
      tmp = arch_to_object(at);
      tmp = insert_ob_in_ob(tmp,op);
      new_draw_info(NDI_UNIQUE, 0,op,"The world suddenly moves very fast!");
    } else
      tmp->stats.food++;
    SET_FLAG(tmp, FLAG_APPLIED);
    tmp->speed_left=0;
    fix_player(op);
}

void confuse_player(object *op, object *hitter, int dam)
{
    object *tmp;
    int maxduration;
    
    tmp = present_in_ob_by_name(FORCE,"confusion", op);
    if(!tmp) {
	tmp = get_archetype(FORCE_NAME);
	tmp = insert_ob_in_ob(tmp,op);
    }
    
    /* Duration added per hit and max. duration of confusion both depend
     *  on the player's resistance 
     */
    tmp->speed = 0.05;
    tmp->subtype = FORCE_CONFUSION;
    tmp->duration = 8 + MAX(1, 5*(100-op->resist[ATNR_CONFUSION])/100);
    if (tmp->name) free_string(tmp->name);
    tmp->name = add_string("confusion");
    maxduration = MAX(2, 30*(100-op->resist[ATNR_CONFUSION])/100);
    if( tmp->duration > maxduration)
	tmp->duration = maxduration;
    
    if(op->type == PLAYER && !QUERY_FLAG(op,FLAG_CONFUSED))
	new_draw_info(NDI_UNIQUE, 0,op,"You suddenly feel very confused!");
    SET_FLAG(op, FLAG_CONFUSED);
}

void blind_player(object *op, object *hitter, int dam)
{
    object *tmp,*owner;

    /* Save some work if we know it isn't going to affect the player */
    if (op->resist[ATNR_BLIND]==100) return;

    tmp = present_in_ob(BLINDNESS,op);
    if(!tmp) { 
      tmp = get_archetype("blindness");
      SET_FLAG(tmp, FLAG_BLIND);
      SET_FLAG(tmp, FLAG_APPLIED);
      /* use floats so we don't lose too much precision due to rounding errors.
       * speed is a float anyways.
       */
      tmp->speed =  tmp->speed * (100.0 - (float)op->resist[ATNR_BLIND]) / 100;

      tmp = insert_ob_in_ob(tmp,op);
      change_abil(op,tmp);   /* Mostly to display any messages */
      fix_player(op);        /* This takes care of some other stuff */

      if(hitter->owner) owner = get_owner(hitter);
      else owner = hitter;

      new_draw_info_format(NDI_UNIQUE,0,owner,
	  "Your attack blinds %s!",query_name(op));
    } 
    tmp->stats.food += dam;
    if(tmp->stats.food > 10) tmp->stats.food = 10;
}

void paralyze_player(object *op, object *hitter, int dam) 
{
    float effect,max;
    /* object *tmp; */

    /* This is strange stuff... someone knows for what this is
     * written? Well, i think this can and should be removed 
    */

    /*
    if((tmp=present(PARAIMAGE,op->map,op->x,op->y))==NULL) {
	tmp=clone_arch(PARAIMAGE);
	tmp->x=op->x,tmp->y=op->y;
	insert_ob_in_map(tmp,op->map,tmp,INS_NO_MERGE | INS_NO_WALK_ON);
    }
    */

    /* Do this as a float - otherwise, rounding might very well reduce this to 0 */
    effect = (float)dam * 3.0 * (100.0 - op->resist[ATNR_PARALYZE]) / 100;

    if (effect==0) return;

    op->speed_left-=FABS(op->speed)*effect;
    /* tmp->stats.food+=(signed short) effect/op->speed; */

    /* max number of ticks to be affected for. */
    max = (100 - op->resist[ATNR_PARALYZE])/ 2;
    if (op->speed_left< -(FABS(op->speed)*max))
        op->speed_left  = (float) -(FABS(op->speed)*max);

/*      tmp->stats.food = (signed short) (max/FABS(op->speed)); */
}


/* Attempts to kill 'op'.  hitter is the attack object, dam is
 * the computed damaged.
 */
void deathstrike_player(object *op, object *hitter, int *dam) 
{
    /*  The intention of a death attack is to kill outright things
    **  that are a lot weaker than the attacker, have a chance of killing
    **  things somewhat weaker than the caster, and no chance of
    **  killing something equal or stronger than the attacker.
    **  Also, if a deathstrike attack has a slaying, any monster
    **  whose name or race matches a comma-delimited list in the slaying
    **  field of the deathstriking object  */

    int atk_lev, def_lev, kill_lev;

    if(hitter->slaying) 
	if(!( (QUERY_FLAG(op,FLAG_UNDEAD)&&strstr(hitter->slaying,undead_name)) ||
		(op->race&&strstr(hitter->slaying,op->race))))	return;

    def_lev = op->level;
    if (def_lev < 1) {
        LOG (llevError, "BUG: arch %s, name %s with level < 1\n",
             op->arch->name, op->name);
        def_lev = 1;
    }
    atk_lev = (hitter->chosen_skill?hitter->chosen_skill->level:hitter->level) / 2;
    /* LOG(llevDebug,"Deathstrike - attack level %d, defender level %d\n",
       atk_lev, def_lev); */

    if(atk_lev >= def_lev ){
	kill_lev = random_roll(0, atk_lev-1, hitter, PREFER_HIGH);

	/* Note that the below effectively means the ratio of the atk vs
	 * defener level is important - if level 52 character has very little
	 * chance of killing a level 50 monster.  This should probably be
	 * redone.
	 */
	if(kill_lev >= def_lev) {
	    *dam = op->stats.hp+10; /* take all hp. they can still save for 1/2 */
	    /* I think this doesn't really do much.  Because of 
	     * integer rounding, this only makes any difference if the 
	     * attack level is double the defender level.
	     */
	    *dam *= kill_lev / def_lev;
	}
    } else {
      *dam = 0;  /* no harm done */
    }
}

/* thrown_item_effect() - handles any special effects of thrown
 * items (like attacking living creatures--a potion thrown at a
 * monster).
 */
static void thrown_item_effect (object *hitter, object *victim)
{
    if(!QUERY_FLAG(hitter,FLAG_ALIVE)) {
	/* May not need a switch for just 2 types, but this makes it 
	 * easier for expansion.
	 */
	switch (hitter->type) {
	    case POTION:
		/* should player get a save throw instead of checking magic protection? */
		if(QUERY_FLAG(victim,FLAG_ALIVE)&&!QUERY_FLAG(victim,FLAG_UNDEAD)
		   &&(victim->resist[ATNR_MAGIC]<60)) (void) apply_potion(victim,hitter);
		break;

	    case POISON: /* poison drinks */
		/* As with potions, should monster get a save? */
		if(QUERY_FLAG(victim,FLAG_ALIVE)&&!QUERY_FLAG(victim,FLAG_UNDEAD)
		   &&(victim->resist[ATNR_POISON]<60)) apply_poison(victim,hitter);
		break;

	    /* Removed case statements that did nothing.
	     * food may be poisonous, but monster must be willing to eat it,
	     * so we don't handle it here.
	     * Containers should perhaps break open, but that code was disabled.
	     */
	}
    }
}

/* adj_attackroll() - adjustments to attacks by various conditions */

int adj_attackroll (object *hitter, object *target) {
  object *attacker = hitter;
  int adjust=0;

  /* safety */
  if(!target||!hitter||!hitter->map||!target->map||!on_same_map(hitter,target)) {
    LOG (llevError, "BUG: adj_attackroll(): hitter and target not on same "
         "map\n");
    return 0;
  }

  /* aimed missiles use the owning object's sight */
  if(is_aimed_missile(hitter)) {
    if ((attacker = get_owner(hitter))==NULL) attacker = hitter;
    /* A player who saves but hasn't quit still could have objects
     * owned by him - need to handle that case to avoid crashes.
     */
    if (QUERY_FLAG(attacker, FLAG_REMOVED)) attacker = hitter;
  }
  else if(!QUERY_FLAG(hitter,FLAG_ALIVE))
    return 0;

   /* determine the condtions under which we make an attack.  
    * Add more cases, as the need occurs. */

  if(!can_see_enemy(attacker,target)) {
     /* target is unseen */
    if(target->invisible || QUERY_FLAG(attacker,FLAG_BLIND))
      adjust -= 10;
     /* dark map penalty for the hitter (lacks infravision if we got here). */
    else if(target->map&&target->map->darkness>0&&!stand_in_light(target))
      adjust -= target->map->darkness;
  }

  if(QUERY_FLAG(attacker,FLAG_SCARED))
    adjust -= 3;

  if(QUERY_FLAG(target,FLAG_UNAGGRESSIVE))
    adjust += 1;

  if(QUERY_FLAG(target,FLAG_SCARED))
    adjust += 1;

  if(QUERY_FLAG(attacker,FLAG_CONFUSED))
    adjust -= 3;

  /* if we attack at a different 'altitude' its harder */
  if((attacker->move_type & target->move_type)==0)
    adjust -= 2;

#if 0
  /* slower attacks are less likely to succeed. We should use a
   * comparison between attacker/target speeds BUT, players have
   * a generally faster speed, so this will wind up being a HUGE
   * disadantage for the monsters! Too bad, because missiles which
   * fly fast should have a better chance of hitting a slower target.
   */
  if(hitter->speed<target->speed) 
    adjust += ((float) hitter->speed-target->speed);
#endif

#if 0
  LOG(llevDebug,"adj_attackroll() returns %d (%d)\n",adjust,attacker->count);
#endif

  return adjust;
} 


/* determine if the object is an 'aimed' missile */
int is_aimed_missile ( object *op) {

    /* I broke what used to be one big if into a few nested
     * ones so that figuring out the logic is at least possible.
     */
    if (op && (op->move_type & MOVE_FLYING)) {
	if (op->type==ARROW || op->type==THROWN_OBJ)
	    return 1;
	else if (op->type==SPELL_EFFECT && (op->subtype == SP_BULLET ||
	      op->subtype == SP_EXPLOSION))
	    return 1;
    }
    return 0;
} 

