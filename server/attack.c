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
           &&op->other_arch&&op->glow_radius) { 
		char *arch=op->other_arch->name;

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
		    esrv_update_item(UPD_WEIGHT, tmp, tmp);
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
          insert_ob_in_map(tmp,op->map,originator,0);
        }
        if ( ! QUERY_FLAG (op, FLAG_REMOVED))
            remove_ob(op);
        (void) insert_ob_in_ob(op,tmp);
        return;
    }
}

int hit_map(object *op,int dir,int type) {
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

  if (op->head) op=op->head;

  op_tag = op->count;

  if ( ! op->map) {
    LOG (llevError,"BUG: hit_map(): %s has no map", op->name);
    return 0;
  }
  map = op->map;
  x = op->x + freearr_x[dir];
  y = op->y + freearr_y[dir];
  if (get_map_flags(map, &map, x, y, &x, &y) & P_OUT_OF_MAP)
      return 0;

 /* peterm:  a few special cases for special attacktypes --counterspell
        must be out here because it strikes things which are not alive*/
  if(type&AT_COUNTERSPELL) {
    counterspell(op,dir);  /* see newspells.c */
    if(!(type & ~(AT_COUNTERSPELL|AT_MAGIC))){
	return 0;  /* we're done, no other attacks */
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
  while (next)
  {
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
      hit_player(tmp,op->stats.dam,op,type);
      retflag |=1;
      if (was_destroyed (op, op_tag))
        break;
    } else if ((tmp->material || tmp->materialname) && op->stats.dam > 0) {
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
	} else if (USING_SKILL(hitter, SK_BOXING)) {
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
        if (hitter->type == FBALL || hitter->type == FBULLET ||
	    hitter->type == CONE) {
	   i=4;
	   map = hitter->map;
	   if (out_of_map(map, hitter->x, hitter->y))
	       return;
	   next = get_map_ob(map, hitter->x, hitter->y);
	   if (next)
	       while(next) {
		   if (next->type == FBALL || next->type == FBULLET ||
		       next->type == CONE)
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
    char *op_name = NULL;
    tag_t op_tag, hitter_tag;
    event *evt;

    if (get_attack_mode (&op, &hitter, &simple_attack))
        goto error;

    /* GROS: Handle for plugin attack event */
    if ((evt = find_event(op, EVENT_ATTACK)) != NULL)
    {
        CFParm CFP;
        int k, l, m;
        k = EVENT_ATTACK;
        l = SCRIPT_FIX_ALL;
        m = 0;
        CFP.Value[0] = &k;
        CFP.Value[1] = hitter;
        CFP.Value[2] = hitter;
        CFP.Value[3] = op;
        CFP.Value[4] = NULL;
        CFP.Value[5] = &m;
        CFP.Value[6] = &base_dam;
        CFP.Value[7] = &base_wc;
        CFP.Value[8] = &l;
        CFP.Value[9] = evt->hook;
        CFP.Value[10]= evt->options;
        if (findPlugin(evt->plugin)>=0)
            ((PlugList[findPlugin(evt->plugin)].eventfunc) (&CFP));
    }
    /* GROS: This is used to handle script_weapons with weapons. Only used for players */
    if (hitter->type==PLAYER)
    {
        if (hitter->current_weapon != NULL)
        {
            /* GROS: Handle for plugin attack event */
            if ((evt = find_event(hitter->current_weapon, EVENT_ATTACK)) != NULL)
            {
                CFParm CFP;
                int k, l, n;
                n = 0;
                k = EVENT_ATTACK;
                l = SCRIPT_FIX_ALL;
                CFP.Value[0] = &k;
                CFP.Value[1] = hitter;
                CFP.Value[2] = hitter->current_weapon;
                CFP.Value[3] = op;
                CFP.Value[4] = NULL;
                CFP.Value[5] = &n;
                CFP.Value[6] = &base_dam;
                CFP.Value[7] = &base_wc;
                CFP.Value[8] = &l;
                CFP.Value[9] = evt->hook;
                CFP.Value[10]= evt->options;
		if (findPlugin(evt->plugin)>=0)
		    (PlugList[findPlugin(evt->plugin)].eventfunc) (&CFP);
            }
        };
    };
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
	    if ((hitter->type == PLAYER)&&(hitter->casting > -1)){
		hitter->casting = -1;
		hitter->spell_state = 1;
		new_draw_info(NDI_UNIQUE, 0,hitter,"You attacked and lost "
		    "your spell!");
	    }
	    if ((op->casting > -1)&&(hitdam > 0)){
		op->casting = -1;
		op->spell_state = 1;
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
				      PREFER_LOW),op, op->attacktype);
	    if (was_destroyed (op, op_tag)
                || was_destroyed (hitter, hitter_tag)
                || abort_attack (op, hitter, simple_attack))
                goto leave;
	}

	/* In the new attack code, it should handle multiple attack
	 * types in its area, so remove it from here.
	 */
	dam=hit_player(op, random_roll(1, hitdam, hitter, PREFER_HIGH),
		       hitter, type);
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
    int sretval = 0; /* GROS - Needed for script return value */
    tag_t victim_tag, hitter_tag;
    sint16 victim_x, victim_y;
    event *evt;

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
    /* GROS: Handling plugin attack event for thrown items */
    if ((evt = find_event(op, EVENT_ATTACK)) != NULL)
    {
        CFParm CFP;
        CFParm* CFR;
        int k, l, m;
        k = EVENT_ATTACK;
        l = SCRIPT_FIX_ALL;
        m = 0;
        CFP.Value[0] = &k;
        CFP.Value[1] = hitter;
        CFP.Value[2] = op;
        CFP.Value[3] = victim;
        CFP.Value[4] = NULL;
        CFP.Value[5] = &m;
        CFP.Value[6] = &(op->stats.dam);
        CFP.Value[7] = &(op->stats.wc);
        CFP.Value[8] = &l;
        CFP.Value[9] = evt->hook;
        CFP.Value[10]= evt->options;
        if (findPlugin(evt->plugin)>=0)
        {
            CFR = (PlugList[findPlugin(evt->plugin)].eventfunc) (&CFP);
            sretval = *(int *)(CFR->Value[0]);
        }
    }
    else

        hit_something = attack_ob_simple (victim, hitter, op->stats.dam,
                                        op->stats.wc);

    /* Arrow attacks door, rune of summoning is triggered, demon is put on
     * arrow, move_apply() calls this function, arrow sticks in demon,
     * attack_ob_simple() returns, and we've got an arrow that still exists
     * but is no longer on the map. Ugh. (Beware: Such things can happen at
     * other places as well!) */
    if (was_destroyed (hitter, hitter_tag) || hitter->env != NULL) {
        if (container) {
            remove_ob (container);
            free_object (container);
        }
        return NULL;
    }

    /* Missile hit victim */
    /* if the speed is > 10, then this is a fast moving arrow, we go straight
       through the target */
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

        /* Else try to put arrow on victim's map square */
        if ((victim_x != hitter->x || victim_y != hitter->y)
            && !(get_map_flags(hitter->map, NULL, victim_x, victim_y, NULL, NULL) & P_WALL))
	{
            remove_ob (hitter);
            hitter->x = victim_x;
            hitter->y = victim_y;
            insert_ob_in_map (hitter, hitter->map, hitter,0);
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
	    CLEAR_FLAG(op, FLAG_NO_PASS);
	    CLEAR_FLAG(op, FLAG_ALIVE);
	}
    }
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
    if ((attacknum >= NROFATTACKS) || (attacknum < 0)) {
	LOG(llevError, "hit_player_attacktype: Invalid attacknumber passed: %x\n", attacknum);
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
    case ATNR_BLIND: {
        /* chance for inflicting a special attack depends on the
	   difference between attacker's and defender's level */
	int level_diff = MIN(110, MAX(0, op->level - hitter->level));

	/* First, only creatures/players with speed can be affected.
	 * Second, just getting hit doesn't mean it always affects
	 * you.  Third, you still get a saving through against the
	 * effect.  */
        if (op->speed && 
	    (QUERY_FLAG(op, FLAG_MONSTER) || op->type==PLAYER) &&
	    !(rndm(0, (attacknum == ATNR_SLOW?6:3)-1)) &&
	    !did_make_save(op, level_diff,  op->resist[attacknum]/10)) {

	  /* Player has been hit by something */
	  if (attacknum == ATNR_CONFUSION) confuse_player(op,hitter,dam);
	  else if (attacknum == ATNR_POISON) poison_player(op,hitter,dam);
	  else if (attacknum == ATNR_SLOW) slow_player(op,hitter,dam);
	  else if (attacknum == ATNR_PARALYZE) paralyze_player(op,hitter,dam);
	  else if (attacknum == ATNR_FEAR) SET_FLAG(op, FLAG_SCARED);
	  else if (attacknum == ATNR_CANCELLATION) cancellation(op);
	  else if (attacknum == ATNR_DEPLETE) drain_stat(op);
	  else if (attacknum == ATNR_BLIND  && !QUERY_FLAG(op,FLAG_UNDEAD) &&
		   !QUERY_FLAG(op,FLAG_GENERATOR)) blind_player(op,hitter,dam);
	}
	dam = 0; /* These are all effects and don't do real damage */
	} break;
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
	     */
	    if (!op_on_battleground(hitter, NULL, NULL) && !QUERY_FLAG(op,FLAG_WAS_WIZ)) {
		object *owner = get_owner(hitter), *old_skill;

		/* Try to find the owner of the thing doing the drain - thus, if
		 * you summon vampires, you get some exp.  Most of this code
		 * below is taken directly from kill_object
		 */
		if (owner && owner != hitter) {
		    old_skill = owner->chosen_skill;
		    owner->chosen_skill = hitter->chosen_skill;
		    owner->exp_obj=hitter->exp_obj;

		    /* Not sure if this will happen or not - I'm think it could with
		     * summoned pets - they may use a skill and thus the chosen_skill
		     * gets updated to something they have.
		     */
		    if (owner->chosen_skill && owner->chosen_skill->env != owner) {
			LOG(llevDebug,"kill_object: chosen skill doesn't belong to owner? (%s, %s)\n",
			    owner->chosen_skill->name, owner->name);
			owner->chosen_skill = NULL;
		    }
		    if (owner->exp_obj && owner->exp_obj->env != owner) {
			LOG(llevDebug,"kill_object: exp_obj doesn't belong to owner? (%s, %s)\n",
			    owner->exp_obj->name, owner->name);
			owner->exp_obj = NULL;
		    }
		    if (op->type != PLAYER || owner->type != PLAYER) 
			add_exp(owner,op->stats.exp/(rate*2));
		    owner->chosen_skill = old_skill;
		} else {
		    if (op->type != PLAYER || hitter->type != PLAYER) 
			add_exp(hitter,op->stats.exp/(rate*2));
		}
		add_exp(op,-op->stats.exp/rate);
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
	      SET_FLAG(op, FLAG_SCARED);
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
	  SET_FLAG(op, FLAG_SCARED);
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
    int maxdam=0;
    int battleg=0;    /* true if op standing on battleground */
    int killed_script_rtn = 0;
    object *owner=NULL, *old_skill;
    int evtid;
    CFParm CFP;
    event *evt;

    if (op->stats.hp>=0)
	return -1;

    /* GROS: Handle for plugin death event */
    if ((evt = find_event(op, EVENT_DEATH)) != NULL)
    {
        CFParm* CFR;
        int k, l, m;
        k = EVENT_DEATH;
        l = SCRIPT_FIX_ALL;
        m = 0;
        CFP.Value[0] = &k;
        CFP.Value[1] = hitter;
        CFP.Value[2] = op;
        CFP.Value[3] = NULL;
        CFP.Value[4] = NULL;
        CFP.Value[5] = &type;
        CFP.Value[6] = &m;
        CFP.Value[7] = &m;
        CFP.Value[8] = &l;
        CFP.Value[9] = evt->hook;
        CFP.Value[10]= evt->options;
        if (findPlugin(evt->plugin)>=0)
        {
            CFR =(PlugList[findPlugin(evt->plugin)].eventfunc) (&CFP);
            killed_script_rtn = *(int *)(CFR->Value[0]);
            if (killed_script_rtn)
                return 0;
        };
    }
    /* GROS: Handle for the global kill event */
    evtid = EVENT_GKILL;
    CFP.Value[0] = (void *)(&evtid);
    CFP.Value[1] = (void *)(hitter);
    CFP.Value[2] = (void *)(op);
    GlobalEvent(&CFP);
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
	if (get_owner (op) != NULL && op->owner->type == PLAYER)
	    op->owner->contr->golem=NULL;
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

	/* If a player kills another player with melee, not on
	 * battleground, the "killer" looses 1 luck. Since this is
	 * not reversible, it's actually quite a pain IMHO. -AV 
	 * Fix bug in that we were changing the luck of the hitter, not
	 * player that the object belonged to - so if you killed another player
	 * with spells, pets, whatever, there was no penalty.
	 */
	if(op->type == PLAYER && hitter != op && !battleg)
	    change_luck(owner, -1);
    } /* Was it a player that hit somethign */


    /* Pet (or spell) killed something. */
    if(owner != hitter ) {
	(void) sprintf(buf,"%s killed %s with %s%s.",owner->name,
	       query_name(op),query_name(hitter), battleg? " (duel)":"");
	owner->exp_obj=hitter->exp_obj;
    }
    else
	(void) sprintf(buf,"%s killed %s%s.",hitter->name,op->name,
	       battleg? " (duel)":"");

    new_draw_info(NDI_ALL, op->type==PLAYER?1:10, NULL, buf);

    /* If you didn't kill yourself, and your not the wizard */
    if(hitter!=op&&!QUERY_FLAG(op, FLAG_WAS_WIZ)) {
	int exp=op->stats.exp;

	if(!settings.simple_exp && hitter->level>op->level)
	    exp=(exp*(op->level+1))/MAX(hitter->level+1, 1);

	if (owner != hitter) {
	    old_skill = owner->chosen_skill;
	    owner->chosen_skill = hitter->chosen_skill;
	    owner->exp_obj=hitter->exp_obj;

	    /* Not sure if this will happen or not - I'm think it could with
	     * summoned pets - they may use a skill and thus the chosen_skill
	     * gets updated to something they have.
	     */
	    if (owner->chosen_skill && owner->chosen_skill->env != owner) {
		LOG(llevDebug,"kill_object: chosen skill doesn't belong to owner? (%s, %s)\n",
		    owner->chosen_skill->name, owner->name);
		owner->chosen_skill = NULL;
	    }
	    if (owner->exp_obj && owner->exp_obj->env != owner) {
		LOG(llevDebug,"kill_object: exp_obj doesn't belong to owner? (%s, %s)\n",
		    owner->exp_obj->name, owner->name);
		owner->exp_obj = NULL;
	    }
	}

	exp = calc_skill_exp(owner,op);

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

	if(owner->type!=PLAYER || owner->contr->party_number<=0) {
	    add_exp(owner,exp);
	}
	else {
	    int shares=0,count=0;

	    player *pl;

	    int no=owner->contr->party_number;
#ifdef PARTY_KILL_LOG
	    add_kill_to_party(no,query_name(owner),query_name(op),exp);
#endif
	    for(pl=first_player;pl!=NULL;pl=pl->next) {
		if(pl->ob->contr->party_number==no && on_same_map(pl->ob, owner)) {
		    count++;
		    shares+=(pl->ob->level+4);
		}
	    }
	    if(count==1 || shares>exp)
		add_exp(owner,exp);
	    else {
		int share=exp/shares,given=0,nexp;
		for(pl=first_player;pl!=NULL;pl=pl->next) {
		    if(pl->ob->contr->party_number==no && on_same_map(pl->ob, owner)) {
			nexp=(pl->ob->level+4)*share;
			add_exp(pl->ob,nexp);
			given+=nexp;
		    }
		}
		exp-=given;
		add_exp(owner,exp); /* give any remainder to the player */
	    }
	} /* else part of a party */

	/* set this back after we have added the experience */
	if (owner != hitter) owner->chosen_skill = old_skill;

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

	friendlyfire	= 0;
	
		if(op->type == PLAYER){
			
			if(hitter->type == PLAYER && hitter->contr->peaceful == 1)
				return 1;
		    
			if(owner = get_owner(hitter)){
				if(owner->type == PLAYER && owner->contr->peaceful == 1)
					friendlyfire = 2;
			}
			
			if(hitter->type == CONE || hitter->type == FBALL  || hitter->type == FIREWALL
				|| hitter->type == SWARM_SPELL || hitter->type == POISONCLOUD 
				|| hitter->type == POISONING || hitter->type == DISEASE || hitter->type == RUNE)
				
				friendlyfire = 0;			
	   }
	
	return friendlyfire;
}


/* This isn't used just for players, but in fact most objects.
 * op is the object to be hit, dam is the amount of damage, hitter
 * is what is hitting the object, and type is the attacktype.
 * dam is base damage - protections/vulnerabilities/slaying matches can
 * modify it.
 */

  /* Oct 95 - altered the following slightly for MULTIPLE_GODS hack
   * which needs new attacktype AT_HOLYWORD to work . b.t. */

int hit_player(object *op,int dam, object *hitter, int type) {
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
            if (tmp->type == RUNE) {
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
		
		/* if this is friendly fire then do a set % of damage only*/
		friendlyfire = friendly_fire(op, hitter);
		if (friendlyfire){
			
			maxdam = ((dam * settings.set_friendly_fire) / 100)+1;
			
			#ifdef ATTACK_DEBUG
			LOG(llevDebug,"Friendly fire (type:%d setting: %d%) did %d damage dropped to %d\n",
			friendlyfire, settings.set_friendly_fire, dam, maxdam);
			#endif
			}
		}
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
		SET_FLAG(op, FLAG_SCARED);
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
		tmp->move_type = PETMOVE;
		if (owner!=NULL)
		    set_owner(tmp,owner);
	    }
	    if (unaggressive)
		SET_FLAG(tmp, FLAG_UNAGGRESSIVE);
	    j=find_first_free_spot(tmp->arch,op->map,op->x,op->y);
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
		be able to kill things better than it does:
		damage should be dependent something--I choose to
		do this:  if it's a monster, the damage from the
		poisoning goes as the level of the monster/2.
		If anything else, goes as damage. */

	if(QUERY_FLAG(hitter,FLAG_ALIVE))
	   tmp->stats.dam += hitter->level/2;
	 else
	   tmp->stats.dam = dam;

	copy_owner(tmp,hitter);   /*  so we get credit for poisoning kills */
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
    
    tmp = present_in_ob(CONFUSION,op);
    if(!tmp) {
      tmp = get_archetype("confusion");
      tmp = insert_ob_in_ob(tmp,op);
    }
    
    /* Duration added per hit and max. duration of confusion both depend
       on the player's resistance */
    tmp->stats.food += MAX(1, 5*(100-op->resist[ATNR_CONFUSION])/100);
    maxduration = MAX(2, 30*(100-op->resist[ATNR_CONFUSION])/100);
    if( tmp->stats.food > maxduration)
      tmp->stats.food = maxduration;
    
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


/* Attempts to kill 'op'.  hitter is the attack object, dam i
 * the computed damaged.
 */
void deathstrike_player(object *op, object *hitter, int *dam) 
{    /*  The intention of a death attack is to kill outright things
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
    atk_lev = SK_level (hitter) / 2;
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
  if(QUERY_FLAG(attacker,FLAG_FLYING)!=QUERY_FLAG(target,FLAG_FLYING))
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

  if(op&&QUERY_FLAG(op,FLAG_FLYING)&& 
     (op->type==ARROW||op->type==THROWN_OBJ
       ||op->type==FBULLET||op->type==FBALL)) 
    return 1;
  return 0;
}
