/*
 * static char *rcsid_attack_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

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

    The author can be reached via e-mail to frankj@ifi.uio.no.
*/
#include <global.h>
#include <living.h>

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

int did_make_save_item(object *op, int type) {
  int i, saves=0,materials=0, orig_type=type;

  type &= (~op->immune & ~op->protected);

  if(type&AT_CANCELLATION)
    type=15;
  else if(type&AT_COLD)
    type=4;
  else if(type&AT_ELECTRICITY)
    type=3;
  else if(type&AT_FIRE)
    type=2;
  else if(type&AT_PHYSICAL)
    type=0;                     /* This was 1 before... */
  /* If we are hite by pure magic, the item can get destroyed.  We need
   * to check the original type, because type is now set to what the object
   * is not immune to.  Thus, if an object is immune to cold and hit by
   * cold and magic, type would be magic here, and thus could be destroyed -
   * that should not happen.
   */
  else if(orig_type==AT_MAGIC) /* Only pure magic, not paralyze, etc */
    type=1;                     /* This was 0 before... */
  else return 1;
  for(i=0;i<NROFMATERIALS;i++)
    if(op->material&(1<<i)) {
      materials++;
      if(RANDOM()%20+1>=object_saves[type][i]-op->magic)
	saves++;
    }
  if (saves==materials || materials==0) return 1;
  if ((saves==0) || (RANDOM()%materials+1 > saves)) return 0;
  return 1;
}

/* This function calls did_make_save_item.  It then performs the
 * appropriate actions to the item (such as burning the item up,
 * calling cancellation, etc.)
 */

void save_throw_object(object *op, int type) {

    if (!did_make_save_item(op, type)) {
	object *env=op->env;
	int x=op->x,y=op->y;
	mapstruct *m=op->map;

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

		decrease_ob_nr(op,1);
		if((op = get_archetype(arch))!=NULL) {
                   if(env) {  
			op->x=env->x,op->y=env->y;
			insert_ob_in_ob(op,env);
			if (env->contr)
			    esrv_send_item(env, op);
                   } else { 
                      op->x=x,op->y=y;
                      insert_ob_in_map(op,m);
		   }
		}
		return;
        }
        if(type&AT_CANCELLATION) {          /* Cancellation. */
              cancellation(op);
              return;  
        }
	if(op->nrof>1)
	      decrease_ob_nr(op,RANDOM()%op->nrof);
	else {
	    if (op->env) {
		object *tmp= is_player_inv(op->env);

		if (tmp) {
		    esrv_del_item(tmp->contr, op->count);
		    esrv_update_item(UPD_WEIGHT, tmp, tmp);
		}
	    }
	    remove_ob(op);
	    free_object(op);
	}
	if(type&(AT_FIRE|AT_ELECTRICITY)) {
	      op=get_archetype("burnout");
	      if(env) {
		op->x=env->x,op->y=env->y;
                insert_ob_in_ob(op,env);
	      } else { 
		op->x=x,op->y=y;
	      	insert_ob_in_map(op,m);
	      }
	}
	return;
    }
    if(type&AT_COLD &&!(op->immune&type||op->protected&type) &&
      !QUERY_FLAG(op,FLAG_NO_PICK)&&(RANDOM()&2)) {
        object *tmp;
        archetype *at = find_archetype("icecube");
        if (at == NULL)
          return;
        if ((tmp = present_arch(at,op->map,op->x,op->y)) == NULL) {
          tmp = arch_to_object(at);
          tmp->x=op->x,tmp->y=op->y;
          insert_ob_in_map(tmp,op->map);
        }
        remove_ob(op);
        (void) insert_ob_in_ob(op,tmp);
        return;
    }
}

int hit_map(object *op,int dir,int type) {
  object *tmp,*next=NULL;
  int retflag=0;  /* added this flag..  will return 1 if it hits a monster */
  
  if (op->head) op=op->head;

  if (!op->map) {
    LOG(llevDebug,"hit_map called, but %s has no map", op->name);
    return 0;
  }
  if(out_of_map(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]))
      return 0;

 /* peterm:  a few special cases for special attacktypes --counterspell
        must be out here because it strikes things which are not alive*/
  if(type&AT_COUNTERSPELL) {
    counterspell(op);  /* see newspells.c */
    if(!(type & ~(AT_COUNTERSPELL|AT_MAGIC))){
#ifdef NO_CONE_PROPOGATE
	return 1;
#else
	return 0;  /* we're done, no other attacks */
#endif
    }
    type&= ~AT_COUNTERSPELL;
  }

  if(type&AT_CHAOS){
    shuffle_attack(op,1);  /*1 flag tells it to change the face */
    update_object(op);
    type &= ~AT_CHAOS;
  }


  for(tmp=get_map_ob(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]);
      tmp!=NULL;tmp=next)
  {
    next=tmp->above;

    /* Since we are traversing a stack, it is possible that the stack
     * gets messed up.  So do some checks.
     */
    if (QUERY_FLAG(tmp, FLAG_FREED)) {
	LOG(llevDebug,"Warning: in hit_map, found free object\n");
	break;
    }
    else if(QUERY_FLAG(tmp,FLAG_ALIVE)) {
      hit_player(tmp,op->stats.dam,op,type);
      retflag |=1;
    } /* It is possible the object has been relocated to know longer be
       * on the map (ie, in an icecube.)  If we no longer have a map,
       * just ignore.
       */
    else if(tmp->material && tmp->map)
      save_throw_object(tmp,type);
  }
  /*if(tmp==NULL) return 0;  This doesn't work, of course */
#ifdef NO_CONE_PROPOGATE
  return retflag;
#else
  return 0;
#endif
}

static att_msg *attack_message(int dam) {
  static att_msg messages;
  static char buf1[MAX_BUF],buf2[MAX_BUF];
  if(dam==-1) {
    strcpy(buf1,"hit");
    buf2[0]='\0';
  } else if(dam==0) {
    strcpy(buf1,"missed");
    buf2[0]='\0';
  } else if(dam<3) {
    strcpy(buf1,"grazed");
    buf2[0]='\0';
  } else if(dam<6) {
    strcpy(buf1,"hit");
    buf2[0]='\0';
  } else if(dam<9) {
    strcpy(buf1,"hit");
    strcpy(buf2," hard");
  } else if(dam<12) {
    strcpy(buf1,"hit");
    strcpy(buf2," very hard");
  } else if(dam<16) {
    strcpy(buf1,"hit");
    strcpy(buf2," extremely hard");
  } else if(dam<20) {
    strcpy(buf1,"crush");
    strcpy(buf2," very hard");
  } else if(dam<25) {
    strcpy(buf1,"smash");
    strcpy(buf2," with a bonecrunching sound");
  } else if(dam<35) {
    strcpy(buf1,"grind");
    strcpy(buf2," to dust");
  } else {
    strcpy(buf1,"shred");
    strcpy(buf2," to pieces");
  }
  messages.msg1=buf1,messages.msg2=buf2;
  return &messages;
}

/*
 * attack_ob() returns 1 on a hit, and 0 on a miss.
 * op is what is being attacked, hitter is what is hitting (Arrow, player,
 * whatever)
 */

int attack_ob(object *op,object *hitter) {
    int roll,dam=0;
    char buf[MAX_BUF];
    uint32 type;
    att_msg *msg;
    char *op_name;
    signed char luck=0;

    if(op->head!=NULL)
	op=op->head;
    if(op->name==NULL) {
	if(settings.debug) {
	    dump_object(op);
	    LOG(llevDebug,"Object without name tried to attack.\n%s\n",errmsg);
	}
	if (QUERY_FLAG(op, FLAG_REMOVED) && !QUERY_FLAG(op, FLAG_FREED))
	    free_object(op);
	return 1;
    }

    /*
     * A little check to make it more difficult to dance forward and back
     * to avoid ever being hit by monsters.
     */
    if (QUERY_FLAG(op, FLAG_MONSTER) && op->speed_left > -(FABS(op->speed))*0.3) {

	/* Decrease speed BEFORE calling process_object.  Otherwise, an
	 * infinite loop occurs, with process_object calling move_monster,
	 * which then gets here again.  By decreasing the speed before
	 * we call process_object, the 'if' statement above will fail.
	 */
	op->speed_left--;
	process_object(op);
	if (QUERY_FLAG(op, FLAG_FREED))
	return 1;
    }

    add_refcount(op_name = op->name);
    if(hitter->head!=NULL)
	hitter=hitter->head;

    if (hitter->name==NULL) {
	if(settings.debug) {
	    dump_object(hitter);
	    LOG(llevDebug,"Object without name tried to attack.\n%s\n",errmsg);
	}
	return 1;
    }
	
    if(op->stats.luck) {
	luck=RANDOM()%abs(op->stats.luck);
	if(op->stats.luck<0)
	    luck= -luck;
    }
    if((int)luck < -5)
	roll= -20;
    else
	roll=RANDOM()%20+1+luck;

    /* Adjust roll for various situations. */
    roll += adj_attackroll(hitter,op); 

    /* See if we hit the creature */
    if(roll==(20+luck)||op->stats.ac>=hitter->stats.wc-roll) {
	int hitdam=hitter->stats.dam+luck;
#ifdef CASTING_TIME
	if ((hitter->type == PLAYER)&&(hitter->casting > -1)){
	    hitter->casting = -1;
	    hitter->spell_state = 1;
	    new_draw_info(NDI_UNIQUE, 0,hitter,"You attacked and lost your spell!");
	}
	if ((op->casting > -1)&&(hitdam > 0)){
	    op->casting = -1;
	    op->spell_state = 1;
	    if (op->type == PLAYER)  {
		new_draw_info(NDI_UNIQUE, 0,op,"You were hit and lost your spell!");
		new_draw_info_format(NDI_ALL|NDI_UNIQUE,5,NULL,
		    "%s was hit by %s and lost a spell.",op->name,hitter->name);
	    }
	}
#endif
	/* If you hit something, the victim should *always* wake up.
	 * Before, invisible hitters could avoid doing this. 
	 * -b.t. */
	if(QUERY_FLAG(op,FLAG_SLEEP)) CLEAR_FLAG(op,FLAG_SLEEP);

	/* If the victim can't see the attacker, it may alert others
	 * for help. */
	if(op->type!=PLAYER&&!can_see_enemy(op,hitter)
	   &&!get_owner(op)&&RANDOM()%(op->stats.Int+1))
	    npc_call_help(op);

	/* if you were hidden and hit by a creature, you are discovered*/
	if(op->hide && QUERY_FLAG(hitter,FLAG_ALIVE)) {
	    make_visible(op);
	    if(op->type==PLAYER) new_draw_info(NDI_UNIQUE, 0,op,
		"You were hit by a wild attack. You are no longer hidden!");
	}

	/* thrown items (hitter) will have various effects
	 * when they hit the victim.  For things like thrown daggers,
	 * this sets 'hitter' to the actual dagger, and not the
	 * wrapper object.
	 */
	if((hitter=thrown_item_effect(hitter,op))==NULL) goto leave;

	/* Need to do at least 1 damage, otherwise there is no point
	 * to go further and it will cause FPE's below.
	 */
	if (hitdam<=0) hitdam=1;

	type=hitter->attacktype;
	if(!type) type=AT_PHYSICAL;
	/* Handle monsters that hit back */
	if (QUERY_FLAG(op, FLAG_HITBACK) && QUERY_FLAG(hitter, FLAG_ALIVE)) {
	    if (op->attacktype & AT_ACID && hitter->type==PLAYER)
		new_draw_info(NDI_UNIQUE, 0,hitter,"You are splashed by acid!\n");
	    hit_player(hitter, RANDOM()%(op->stats.dam+1), op, op->attacktype);
	    if (QUERY_FLAG(op, FLAG_FREED)) goto leave;
	}

	/* In the new attack code, it should handle multiple attack
	 * types in its area, so remove it from here.
	 */
	dam=hit_player(op, (RANDOM()%hitdam)+1, hitter, type);
	if (QUERY_FLAG(op, FLAG_FREED))
	    goto leave;
    } /* end of if hitter hit op */
    /* if we missed, dam=0 */

    msg=attack_message(dam);

    /* Did a player hurt another player?  Inform both! */
    if(op->type==PLAYER&&
       (get_owner(hitter)==NULL?hitter->type:hitter->owner->type)==PLAYER) {
	if(get_owner(hitter)!=NULL)
	    sprintf(buf,"%s %ss you%s with %s.",
              hitter->owner->name,msg->msg1,msg->msg2,hitter->name);
	else {
	    sprintf(buf,"%s %ss you%s.",hitter->name,msg->msg1,msg->msg2);
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

    if(hitter->type==PLAYER) {
	sprintf(buf,"You %s %s%s.",msg->msg1,op_name,msg->msg2);
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
	sprintf(buf,"You %s %s%s with %s.",
            msg->msg1,op_name,msg->msg2,hitter->name);
	play_sound_map(op->map, op->x, op->y, SOUND_PLAYER_HITS4);
	new_draw_info(NDI_BLACK, 0, hitter->owner, buf);
    }

leave:
    free_string(op_name);
    return dam;
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
    update_object(op);
    if(perc==NUM_ANIMATIONS(op)-1) { /* Reached the last animation */
	if(op->face==blank_face) {
	    /* If the last face is blank, remove the ob */
	    remove_ob(op); /* Should update LOS */
	    free_object(op);

	    /* remove_ob should call update_position for us */
	    /*update_position(m, x, y);*/

	} else { /* The last face was not blank, leave an image */
	    CLEAR_FLAG(op, FLAG_BLOCKSVIEW);
	    update_all_los(op->map);
	    CLEAR_FLAG(op, FLAG_NO_PASS);
	    CLEAR_FLAG(op, FLAG_ALIVE);
	}
    }
}

/* This returns the amount of damage hitter does to op with the appropriate
 * attacktype.  Only 1 attacktype should be set at a time.  This doesn't
 * damage the player, but returns how much it should take.  However, it will
 * do other effects (paralyzation, slow, etc.)
 */

int hit_player_attacktype(object *op, object *hitter, int dam, 
	uint32 attacktype, int magic) {

    int does_slay=0;

#ifdef ATTACK_DEBUG
    LOG(llevDebug, "\thit_player_attacktype: attacktype %x, dam %d\n",
	attacktype, dam);
#endif

    if (!attacktype) {
	LOG(llevError,"hit_player_attacktype called without an attacktype\n");
	return 0;
    }
    /* AT_INTERNAL is supposed to do exactly dam.  Put a case here so
     * people can't mess with that or it otherwise get confused.
     */
    if (attacktype & AT_INTERNAL) return dam;

    if (hitter->slaying) {
	if (((op->race !=NULL && strstr(hitter->slaying,op->race)) ||
#ifndef MULTIPLE_GODS
	  (strstr(hitter->slaying, undead_name) && QUERY_FLAG(op,FLAG_UNDEAD)) ||
#endif
	  (op->arch && op->arch->name!=NULL && strstr(op->arch->name, hitter->slaying)))) {
	    does_slay=1;
	    dam*=3;
	}
    }
    if (op->vulnerable & attacktype) dam*=2;
    if (op->protected & attacktype) dam/=2;

    /* Special hack.  By default, if immune to something, you shouldn't need
     * to worry.  However, acid is an exception, since it can still damage
     * your items.  Only include attacktypes if special processing is needed
     */
    if (!does_slay && (op->immune & attacktype) && (attacktype!=AT_ACID)) 
	return 0;

    /* Keep this in order - makes things easier to find */

    if (attacktype & AT_PHYSICAL) {
	if (op->armour) dam=((100-op->armour)*dam/100);
	/*  here also check for diseases */
	check_physically_infect(op,hitter);
 /* Don't need to do anything for magic, fire, electricity, cold */    
    } else if (attacktype & 
      (AT_CONFUSION|AT_POISON|AT_SLOW|AT_PARALYZE|AT_FEAR|AT_CANCELLATION|
       AT_DEPLETE|AT_BLIND)) {

        if (op->speed && (QUERY_FLAG(op, FLAG_MONSTER) || op->type==PLAYER) &&
          !(RANDOM()%((attacktype&AT_SLOW?6:3))) && 
	  (RANDOM()%20+((op->protected&attacktype)?5:1) < savethrow[op->level])) {
	  /* Player has been hit by something */
	    if (attacktype & AT_CONFUSION) confuse_player(op,hitter,dam);
	    else if (attacktype & AT_POISON) poison_player(op,hitter,dam);
	    else if (attacktype & AT_SLOW) slow_player(op,hitter,dam);
	    else if (attacktype & AT_PARALYZE) paralyze_player(op,hitter,dam);
	    else if (attacktype & AT_FEAR) SET_FLAG(op, FLAG_SCARED);
	    else if (attacktype & AT_CANCELLATION) cancellation(op);
	    else if (attacktype & AT_DEPLETE) drain_stat(op);
	    else if (attacktype & AT_BLIND && !QUERY_FLAG(op,FLAG_UNDEAD) &&
		 !QUERY_FLAG(op,FLAG_GENERATOR)) blind_player(op,hitter,dam);
	}
	dam=0;	/* Confusion is an effect - doesn't damage */
    }
    else if (attacktype & AT_ACID) {
	if (!(op->immune & AT_ACID ) && !(op->protected & AT_ACID)) {
	    object *tmp;  /*  If someone is both immune and protected from acid, so is his stuff */
	    int flag=0;
	    char buf[256];

	    for(tmp=op->inv;tmp!=NULL;tmp=tmp->below) {
		if(!QUERY_FLAG(tmp,FLAG_APPLIED)||tmp->immune&AT_ACID||
		   (tmp->protected&AT_ACID&&RANDOM()&1))
			continue;
		if(!(tmp->material&M_IRON))
			continue;
		if(tmp->magic< -4) /* Let's stop at -5 */
			continue;
		if(tmp->type==RING||tmp->type==GLOVES||tmp->type==BOOTS||
		   tmp->type==GIRDLE||tmp->type==AMULET||tmp->type==WAND||
		   tmp->type==ROD||tmp->type==HORN)
			continue; /* To avoid some strange effects */
		/* High damage acid has better chance of corroding objects */
		if(RANDOM()%(dam+5)>RANDOM()%40) {
		    if(op->type==PLAYER) {
			strcpy(buf,"The ");
			strcat(buf,query_name(hitter));
			strcat(buf,"'s acid corrodes your ");
			strcat(buf,query_name(tmp));
			strcat(buf,"!");
			esrv_send_item(op, tmp);
			new_draw_info(NDI_UNIQUE, 0,op,buf);
			flag=1;
		    }
		    tmp->magic--;
		}
	    }
	    if(flag) {	/* Something was corroded */
		fix_player(op);
	    }
	}
	/* Get around check up above */
	if (op->immune & AT_ACID && !does_slay) dam=0;
    }
    else if (attacktype & AT_DRAIN) {
	if(op->stats.exp<=((op->protected & attacktype)?100:50)) {
	    if(op->type==GOLEM)
		dam=999; /* It's force is "sucked" away. 8) */
	    else /* If we can't drain, lets try to do physical damage */
		dam=hit_player_attacktype(op, hitter, dam, AT_PHYSICAL, magic);
	} else {
	    if(hitter->stats.hp<hitter->stats.maxhp &&
	       (op->level > hitter->level) &&
	       RANDOM()%(op->level-hitter->level+3)>3)
		    hitter->stats.hp++;
	    if(!QUERY_FLAG(op,FLAG_WAS_WIZ))
		add_exp(hitter,op->stats.exp/(100+(op->protected&attacktype)?100:0));
	    add_exp(op,-op->stats.exp/(50+(op->protected&attacktype)?50:0));
	    dam=0;	/* Drain is an effect */
	}
    /* weaponmagic, ghosthit not needed, poison, slow, paralyze handled above */
    } else if (attacktype & AT_TURN_UNDEAD) {
	if (QUERY_FLAG(op,FLAG_UNDEAD)) {
	    object *owner=get_owner(hitter)==NULL?hitter:get_owner(hitter);

	    if(op->level<turn_bonus[owner->stats.Wis]+owner->level)
		SET_FLAG(op, FLAG_SCARED);
	}
	else dam=0; /*don't damage non undead - should we damage undead? */
    /* fear, cancelleation, deplete handled above */
    } else if (attacktype & AT_DEATH) {
	 deathstrike_player(op, hitter, &dam);
    } else if (attacktype & AT_CHAOS) {
	dam=0;	/* I think this is right - Chaos attacks with other types */
    }
    else if (attacktype & AT_COUNTERSPELL) {
	dam=0;	/* I don't think it is supposed to attack things */

    /* Godpower does normal effect? */
    } else if (attacktype & AT_HOLYWORD) {
	/* Holyword only effects a limited range of creatures */
	if (op->race && hitter->slaying && strstr(hitter->slaying,op->race)) {
	    object *owner=get_owner(hitter)==NULL?hitter:get_owner(hitter);

	    if(op->level<turn_bonus[owner->stats.Wis]+owner->level)
		SET_FLAG(op, FLAG_SCARED);
	}
	else dam=0;	/* If not one of the creatures, no effect */
    }
    /* if we get here, take default case.  godpower uses this */
    return dam;
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
    char buf[MAX_BUF];
    object *old_hitter=NULL; /* this is used in case of servant monsters */ 
    int maxdam=0,ndam,attacktype=1,attacknum,magic=(type & AT_MAGIC);

    if(op->head!=NULL) {
	if(op->head==op) {
	    LOG(llevError,"Recursive head error!\n");
	    return 0;
	}
#if 0
	/* To paralyze/slow a creature, we must hit its head with the attacktype.
	 * If we are going to do this, this should probably be expanded.
	 */
	if(type&AT_PARALYZE || type&AT_SLOW)
	    return 0;
#else
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
#endif
	op=op->head;
    }

    if(op->type==DOOR && op->inv && op->inv->type==RUNE)
	spring_trap(op->inv,hitter);

    /* If its already dead, or we're the wizard, don't attack it - no point */
    if(QUERY_FLAG(op,FLAG_WIZ)||!QUERY_FLAG(op,FLAG_ALIVE)||op->stats.hp<0)
	return 0;

    /* If its already dead, or we're the wizard, don't attack it - no point */
    if(hitter->name==NULL) {
	if (settings.debug) 
	    LOG(llevDebug, "hit_player: hitter has no name\n");
	return 0;
    }

#ifdef ATTACK_DEBUG
    LOG(llevDebug,"hit player: attacktype %d, dam %d\n", type, dam);
#endif

    /* By default, if a creature is immune to magic, it is immune to any
     * attacktype that has magic as part of it.
     */
    if ((type & AT_MAGIC) && (op->immune & AT_MAGIC)) return 0;

    /* Holyword is really an attacktype modifier (like magic is).  If
     * holyword is part of an attacktype, then make sure the creature is
     * a proper match, otherwise no damage.
     */
    if (type & AT_HOLYWORD) {
	if (!op->race || !hitter->slaying || !strstr(hitter->slaying,op->race))
	    return 0;
    }

    for (attacknum=0; attacknum<NROFATTACKS; attacknum++, attacktype=attacktype<<1) {
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
	    ndam=hit_player_attacktype(op,hitter,dam,attacktype,magic);
	    maxdam=(maxdam>ndam)?maxdam:ndam;
	}
    }
#if 0

    LOG(llevDebug,"Attacktype %d did %d damage\n", type, maxdam);

    /* It doesn't appear that being scared will have anything to do with
     * hitback.  In fact, hitback (monster hitting player) before the
     * player hitting monster code is called.
     */
    CLEAR_FLAG(op,FLAG_SCARED); /* Or the monster won't hit back */
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

    if(type&AT_MAGIC && (RANDOM()%20+1)>=savethrow[op->level])
	maxdam=maxdam/2;

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
	tear_down_wall(op);
	return maxdam;	/* nothing more to do for wall */
    }

    /* Object has been killed.  Lets clean it up */
    if(op->stats.hp<0) {
	object *owner=NULL;
	maxdam+=op->stats.hp+1;
	if(QUERY_FLAG(op,FLAG_BLOCKSVIEW))
	    update_all_los(op->map); /* makes sure los will be recalculated */

	if(op->type==DOOR) {
	    op->speed = 0.1;
	    update_ob_speed(op);
	    op->speed_left= -0.05;
	    return maxdam;
	}
	if(op->type==GOLEM) {
	    remove_friendly_object(op);
	    if(get_owner(op)!=NULL)
		op->owner->contr->golem=NULL;
	    else
		LOG(llevDebug,"Encountered golem without owner.\n");
	    remove_ob(op);
	    free_object(op);
	    return maxdam;
	}

	/* Now lets start dealing with experience we get for killing something */
	owner=get_owner(hitter);
	if(owner==NULL)
	    owner=hitter;

	/* Player killed something */
	if(owner->type==PLAYER) {
	    Log_Kill(owner->name,
		     query_name(op),op->type,
		     (owner!=hitter) ? query_name(hitter) : NULL,
		     (owner!=hitter) ? hitter->type : 0);

	    /* This appears to be doing primitive filtering to only
	     * display the more interesting monsters.
	     */
	    if ( owner->level/2<op->level || op->stats.exp>1000) {
		if(owner!=hitter) {
		    (void) sprintf(buf,"You killed %s with %s.",query_name(op)
				       ,query_name(hitter));
#ifdef ALLOW_SKILLS
		    old_hitter = hitter;
		    owner->exp_obj=hitter->exp_obj; 
#endif
		} else {
			(void) sprintf(buf,"You killed %s.",query_name(op));
		}
		play_sound_map(owner->map, owner->x, owner->y, SOUND_PLAYER_KILLS);
		new_draw_info(NDI_BLACK, 0,owner,buf);
	    }/* message should be displayed */

	    if(op->type == PLAYER && hitter != op)
		 change_luck(hitter, -1);
	} /* was a player that hit this creature */


	/* Pet killed something. */
	if(get_owner(hitter)!=NULL) {
	    (void) sprintf(buf,"%s killed %s with %s.",hitter->owner->name,
		query_name(op),query_name(hitter));
#ifdef ALLOW_SKILLS
	    old_hitter = hitter;
	    owner->exp_obj=hitter->exp_obj;
#endif
	    hitter=hitter->owner;
	}
	else
	    (void) sprintf(buf,"%s killed %s.",hitter->name,op->name);

	/* If you didn't kill yourself, and your not the wizard */
	if(hitter!=op&&!QUERY_FLAG(op, FLAG_WAS_WIZ)) {
	    int exp=op->stats.exp;

	    if(!settings.simple_exp && hitter->level>op->level)
		exp=(exp*(op->level+1))/MAX(hitter->level+1, 1);

/* new exp system in here. Try to insure the right skill is modifying gained exp */ 
#ifdef ALLOW_SKILLS 
	    if(hitter->type==PLAYER && !old_hitter) 
		exp = calc_skill_exp(hitter,op); 
	    /* case for attack spells, summoned monsters killing */ 
	    if (old_hitter && hitter->type==PLAYER) {	
		object *old_skill = hitter->chosen_skill; 

		hitter->chosen_skill=old_hitter->chosen_skill;
		exp = calc_skill_exp(hitter,op); 
		hitter->chosen_skill = old_skill;
	    }
#endif /* ALLOW_SKILLS */

	    /* Really don't give much experience for killing other players */
	    if (op->type==PLAYER)
		exp/=10;

	    /* Don't know why this is set this way - doesn't make
	     * sense to just divide everything by two for no reason.
	     */
	    if (!settings.simple_exp)
		exp=exp/2;

	    if(hitter->type!=PLAYER || hitter->contr->party_number<=0) {
		add_exp(hitter,exp);
	    } else {
		int shares=0,count=0;
		player *pl;
		int no=hitter->contr->party_number;
#ifdef PARTY_KILL_LOG
		add_kill_to_party(no,query_name(hitter),query_name(op),exp);
#endif
		for(pl=first_player;pl!=NULL;pl=pl->next) {
		    if(pl->ob->contr->party_number==no && (pl->ob->map == hitter->map)) {
			count++;
			shares+=(pl->ob->level+4);
		    }
		}
		if(count==1 || shares>exp)
		    add_exp(hitter,exp);
		else {
		    int share=exp/shares,given=0,nexp;
		    for(pl=first_player;pl!=NULL;pl=pl->next) {
			if(pl->ob->contr->party_number==no && (pl->ob->map == hitter->map)) {
			    nexp=(pl->ob->level+4)*share;
			    add_exp(pl->ob,nexp);
			    given+=nexp;
			}
		    }
		    exp-=given;
		    add_exp(hitter,exp); /* give any remainder to the player */
		}
	    }
	}
	if(op->type!=PLAYER) {
	    new_draw_info(NDI_ALL, 10, NULL, buf);
	    if(QUERY_FLAG(op,FLAG_FRIENDLY)) {
		object *owner = get_owner(op);
		if(owner!= NULL && owner->type == PLAYER) {
		    sprintf(buf,"Your pet, the %s, is killed by %s.",
			op->name,hitter->name);
		    play_sound_player_only(owner->contr, SOUND_PET_IS_KILLED,0,0);
		    new_draw_info(NDI_UNIQUE, 0,owner,buf);
		}
		remove_friendly_object(op);
	    }
	    remove_ob(op);
	    free_object(op);
	}
	/* Player has been killed! */
	else {
	    new_draw_info(NDI_ALL, 1, NULL, buf);
	    if(hitter->type==PLAYER) {
		sprintf(buf,"%s the %s",hitter->name,hitter->contr->title);
		strncpy(op->contr->killer,buf,BIG_NAME);
	    } else {
		strncpy(op->contr->killer,hitter->name,BIG_NAME);
		op->contr->killer[BIG_NAME-1]='\0';
	    }
	}
    } /* End of creature kill processing */

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
		insert_ob_in_map(tmp,op->map);
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

	set_owner(tmp,hitter);   /*  so we get credit for poisoning kills */
	tmp->stats.food+=dam;  /*  more damage, longer poisoning */

        if(op->type==PLAYER) {
          tmp->stats.Con= -(dam/4+1);
          tmp->stats.Str= -(dam/3+2);
          tmp->stats.Dex= -(dam/6+1);
          tmp->stats.Int= -dam/7;
          SET_FLAG(tmp,FLAG_APPLIED);
          fix_player(op);
          new_draw_info(NDI_UNIQUE, 0,op,"You suddenly feel very ill.");
        }
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
    tmp = present_in_ob(CONFUSION,op);
    if(!tmp) {
      tmp = get_archetype("confusion");
      tmp = insert_ob_in_ob(tmp,op);
    }
    tmp->stats.food += 5;
    if( tmp->stats.food > 30)
      tmp->stats.food = 30;
    if(op->type == PLAYER && !QUERY_FLAG(op,FLAG_CONFUSED))
      new_draw_info(NDI_UNIQUE, 0,op,"You suddenly feel very confused!");
    SET_FLAG(op, FLAG_CONFUSED);
}

void blind_player(object *op, object *hitter, int dam)
{
    object *tmp,*owner;

    tmp = present_in_ob(BLINDNESS,op);
    if(!tmp) { 
      tmp = get_archetype("blindness");
      SET_FLAG(tmp, FLAG_BLIND);
      SET_FLAG(tmp, FLAG_APPLIED);
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
{    object *tmp;
    if((tmp=present(PARAIMAGE,op->map,op->x,op->y))==NULL) {
      tmp=clone_arch(PARAIMAGE);
      tmp->x=op->x,tmp->y=op->y;
      insert_ob_in_map(tmp,op->map);
    }
    op->speed_left-=(float)FABS(op->speed)*(dam*3);
    tmp->stats.food+=(signed short) (dam*3)/op->speed;
    if(op->speed_left< -(FABS(op->speed)*40)) {
      op->speed_left  = (float) -(FABS(op->speed)*40);
      tmp->stats.food = (signed short) (40/FABS(op->speed));
    }
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
	if(!( (QUERY_FLAG(op,FLAG_UNDEAD)&&strstr(hitter->slaying,"undead")) ||
		(op->race&&strstr(hitter->slaying,op->race))))	return;

    def_lev = op->level;
/*  atk_lev  = (hitter->level)/2; */ 
    atk_lev  = SK_level(hitter)/2;
    LOG(llevDebug,"Deathstrike - attack level %d, defender level %d\n",
	atk_lev, def_lev);

    if(atk_lev >= def_lev ){
	kill_lev = RANDOM() % atk_lev;

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
	    *dam *= kill_lev / (def_lev == 0 ? 1 : def_lev);
	}
    } else {
      *dam = 0;  /* no harm done */
    }
}

/* thrown_item_effect() - handles any special effects of thrown
 * items (like attacking living creatures--a potion thrown at a
 * monster). We return the hitter item for further
 * possible (ie physical) attacks. Other posibilities
 * include spilling containers, and lighting stuff on fire
 * with thrown torches.
 */
object *thrown_item_effect( object *hitter, object *victim) {
  object *tmp=hitter;
 
  if(hitter->type==THROWN_OBJ) tmp = hitter->inv;
  if(!tmp) return hitter;
 
  if(!QUERY_FLAG(hitter,FLAG_ALIVE)) {
    switch (tmp->type) {
      case POTION:
        if(QUERY_FLAG(victim,FLAG_ALIVE)&&!QUERY_FLAG(victim,FLAG_UNDEAD)
	  &&!(victim->immune&AT_MAGIC)) (void) apply_potion(victim,tmp);
        break;
      case FOOD:
	/* cursed food is (often) poisonous....but it won't 'explode'
     	 * like poison (drink) will. Lets just insert it in inventory
	 * (coded elsewhere) and later figure out if the monster wants 
  	 * to eat it (based on INT). */
        break;
      case POISON: /* poison drinks */
        if(QUERY_FLAG(victim,FLAG_ALIVE)&&!QUERY_FLAG(victim,FLAG_UNDEAD)
	  &&!(victim->immune&AT_POISON)) apply(victim,tmp,0);
        break;
      case CONTAINER: 
        /* spill_container(victim,RANDOM()%(hitter->stats.dam+1)); */
        break;
      default:
        break;
    }
#if 0
    /* glow objects (torches) are on fire.. */
    if(!tmp->type&&tmp->glow_radius>0) {
    }
#endif
  }
 
  return tmp;
}

/* adj_attackroll() - adjustments to attacks by various conditions */

int adj_attackroll (object *hitter, object *target) {
  object *attacker = hitter;
  int adjust=0;

  /* safety */
  if(!target||!hitter||!hitter->map||!target->map||hitter->map!=target->map)
    return 0;

  /* aimed missiles use the owning object's sight */
  if(is_aimed_missile(hitter)) {
    if ((attacker = get_owner(hitter))==NULL) attacker = hitter;
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

