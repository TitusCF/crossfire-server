/*
 * static char *rcsid_gods_c =
 *   "$Id$";
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


/* Oct 3, 1995 - Code laid down for initial gods, priest alignment, and 
 * monster race initialization. b.t.
 */

/* Sept 1996 - moved code over to object -oriented gods -b.t. */

#include <global.h>
#include <living.h>
#include <object.h>
#include <spells.h>
#include <sounds.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/* define this if you want to allow gods to assign more gifts
 * and limitations to priests */
#define MORE_PRIEST_GIFTS

int lookup_god_by_name(char *name) {
  int godnr=-1,nmlen = strlen(name);
 
  if(name&&strcmp(name,"none")) { 
    godlink *gl;
    for(gl=first_god;gl;gl=gl->next)
      if(!strncmp(name,gl->name,MIN(strlen(gl->name),nmlen)))
	break;
    if(gl) godnr=gl->id;
  }
  return godnr;
}

object *find_god(char *name) {
  object *god=NULL;

  if(name) { 
    godlink *gl;
    for(gl=first_god;gl;gl=gl->next)
      if(!strcmp(name,gl->name)) break;
    if(gl) god=pntr_to_god_obj(gl);
  }
  return god;
}

void pray_at_altar(object *pl, object *altar) {
    object *pl_god=find_god(determine_god(pl));
 

#ifdef MULTIPLE_GODS
    /* If non consecrate altar, don't do anything */
    if (!altar->other_arch) return;

    /* hmm. what happend depends on pl's current god, level, etc */
    if(!pl_god) {  /*new convert */
	become_follower(pl,&altar->other_arch->clone);
	return;

    } else if(!strcmp(pl_god->name,altar->other_arch->clone.name)) { 	/* pray at your gods altar */
	int bonus = ((pl->stats.Wis/10)+(SK_level(pl)/10));

	/* we can get neg grace up faster */
	if(pl->stats.grace<0) pl->stats.grace+=(bonus>-1*(pl->stats.grace/10) ?
			bonus : -1*(pl->stats.grace/10));
	/* we can super-charge grace to 2x max */
	if(pl->stats.grace<(2*pl->stats.maxgrace)) {
	    pl->stats.grace+=bonus/2;
	}
	/* I think there was a bug here in that this was nested
	 * in the if immediately above
	 */
	if(pl->stats.grace>(2*pl->stats.maxgrace)) {
	    pl->stats.grace=(2*pl->stats.maxgrace);
	}

	/* Every once in a while, the god decides to checkup on their
	 * follower, and may intervene to help them out. 
	 */
	bonus += pl->stats.luck; /* -- DAMN -- */
	if((RANDOM()%500-bonus)<0) god_intervention(pl,pl_god);
 
    } else { /* praying to another god! */
	int loss = 0,angry=1;
 
	/* I believe the logic for detecting opposing gods was completely
	 * broken - I think it should work now.  altar->other_arch
	 * points to the god of this altar (which we have
	 * already verified is non null).  pl_god->other_arch
	 * is the opposing god - we need to verify that exists before
	 * using its values.
	 */
	if(pl_god->other_arch && (altar->other_arch->name==pl_god->other_arch->name)) {
	    angry=2;
	    if((RANDOM()%(SK_level(pl)+3))-5 > 0) { /* you really screwed up */
		angry=3;
		new_draw_info_format(NDI_UNIQUE|NDI_NAVY,0,pl,
                                "Foul Priest! %s punishes you!",pl_god->name);
		cast_mana_storm(pl,pl_god->level+20);
	    }
	    new_draw_info_format(NDI_UNIQUE|NDI_NAVY,0,pl,
                                "Foolish heretic! %s is livid!",pl_god->name);
	} else 
	    new_draw_info_format(NDI_UNIQUE|NDI_NAVY,0,pl,
                                "Heretic! %s is angered!",pl_god->name);
 
	/* whether we will be successfull in defecting or not -
	 * we lose experience from the clerical experience obj */

	loss = 0.1 * (float) pl->chosen_skill->exp_obj->stats.exp;
	if(loss) lose_priest_exp(pl,RANDOM()%(loss*angry));
 
	/* May switch Gods, but its random chance based on our current level
	 * note it gets harder to swap gods the higher we get */
	if((angry==1)&&!(RANDOM()%(pl->chosen_skill->exp_obj->level+1))) {
	    int i;  /* index over known_spells */
	    int sp;  /*  spell index */

	    become_follower(pl,&altar->other_arch->clone);

	    /* Forget all the special spells from your former God */
	    for(i=0;i<pl->contr->nrofknownspells;i++)
		/* Can this ever be false? */
		if((sp=pl->contr->known_spells[i])>0) {
		    if(spells[sp].cleric && spells[sp].books == 0) {
			pl->contr->nrofknownspells--;
			pl->contr->known_spells[i]=
			    pl->contr->known_spells[pl->contr->nrofknownspells];
			new_draw_info_format(NDI_UNIQUE|NDI_NAVY,0,pl,
				"You lose your knowledge of %s!",spells[sp].name);
			/* I think we can just do an i-- here and still
			 * have it work - i=0 is probably excessive */
			i=0;
			
		    } /* if cleric spell not normally found */
		} /* if ... */

	} /* If angry... switching gods */
    } /* If prayed at altar to other god */
#endif
}

void become_follower (object *op, object *new_god) {
    object *exp_obj = op->chosen_skill->exp_obj;

    if(!op||!new_god) return;

    if(op->race&&new_god->slaying&&strstr(op->race,new_god->slaying)) { 
	new_draw_info_format(NDI_UNIQUE|NDI_NAVY,0,op,"Fool! %s detests your kind!",
			     new_god->name);
        if(RANDOM()%(op->level)-5>0) 
	   cast_mana_storm(op,new_god->level+10);
	return;
    }

    new_draw_info_format(NDI_UNIQUE|NDI_NAVY,0,op,
	   "You become a follower of %s!",new_god->name);

    if(exp_obj->title) { /* get rid of old god */ 
       new_draw_info_format(NDI_UNIQUE,0,op,
	       "%s's blessing is withdrawn from you.",exp_obj->title);
       CLEAR_FLAG(exp_obj,FLAG_APPLIED); 
       (void) change_abil(op,exp_obj);
       free_string(exp_obj->title);
    }

   /* now change to the new gods attributes to exp_obj */
    exp_obj->title = add_string(new_god->name);
    exp_obj->path_attuned=new_god->path_attuned;
    exp_obj->path_repelled=new_god->path_repelled;
    exp_obj->path_denied=new_god->path_denied;
    /* This may copy immunities */
    memcpy(exp_obj->resist, new_god->resist, sizeof(new_god->resist));
#ifdef MORE_PRIEST_GIFTS
    exp_obj->stats.hp= (sint16) new_god->last_heal;  
    exp_obj->stats.sp= (sint16) new_god->last_sp; 
    exp_obj->stats.grace= (sint16) new_god->last_grace;
    exp_obj->stats.food= (sint16) new_god->last_eat;
    exp_obj->stats.luck= (sint8) new_god->stats.luck;
    /* gods may pass on certain flag properties */
    update_priest_flag(new_god,exp_obj,FLAG_SEE_IN_DARK);
    update_priest_flag(new_god,exp_obj,FLAG_REFL_SPELL);
    update_priest_flag(new_god,exp_obj,FLAG_REFL_MISSILE);
    update_priest_flag(new_god,exp_obj,FLAG_STEALTH);
    update_priest_flag(new_god,exp_obj,FLAG_MAKE_INVIS);
    update_priest_flag(new_god,exp_obj,FLAG_UNDEAD);
    update_priest_flag(new_god,exp_obj,FLAG_BLIND);
    update_priest_flag(new_god,exp_obj,FLAG_XRAYS); /* better have this if blind! */ 
#endif

    new_draw_info_format(NDI_UNIQUE,0,op, 
	"You are bathed in %s's aura.",new_god->name);

#ifdef MORE_PRIEST_GIFTS
    /* Weapon/armour use are special...handle flag toggles here as this can
     * only happen when gods are worshipped and if the new priest could
     * have used armour/weapons in the first place */
    update_priest_flag(new_god,exp_obj,FLAG_USE_WEAPON); 
    update_priest_flag(new_god,exp_obj,FLAG_USE_ARMOUR);

    if(worship_forbids_use(op,exp_obj,FLAG_USE_WEAPON,"weapons"))
	stop_using_item(op,WEAPON,2);

    if(worship_forbids_use(op,exp_obj,FLAG_USE_ARMOUR,"armour")) { 
	stop_using_item(op,ARMOUR,1);
	stop_using_item(op,HELMET,1);
	stop_using_item(op,BOOTS,1);
	stop_using_item(op,GLOVES,1);
	stop_using_item(op,SHIELD,1);
    }
#endif

    SET_FLAG(exp_obj,FLAG_APPLIED); 
    (void) change_abil(op,exp_obj);
} 

int worship_forbids_use (object *op, object *exp_obj, uint32 flag, char *string) {

  if(QUERY_FLAG(&op->arch->clone,flag))
    if(QUERY_FLAG(op,flag)!=QUERY_FLAG(exp_obj,flag)) {
      update_priest_flag(exp_obj,op,flag);
      if(QUERY_FLAG(op,flag))
        new_draw_info_format(NDI_UNIQUE,0,op,"You may use %s again.",string);
      else {
        new_draw_info_format(NDI_UNIQUE,0,op,"You are forbidden to use %s.",string);
	return 1;
      }
    }
  return 0;
}

/* stop_using_item() - unapplies up to number worth of items of type */
void stop_using_item ( object *op, int type, int number ) {
  object *tmp;

  for(tmp=op->inv;tmp&&number;tmp=tmp->below)
    if(tmp->type==type&&QUERY_FLAG(tmp,FLAG_APPLIED)) { 
	manual_apply(op,tmp,0); /* this should unapply things properly */ 
	number--;
    }
}

/* update_priest_flag() - if the god does/doesnt have this flag, we
 * give/remove it from the experience object if it doesnt/does
 * already exist. For players only! 
 */
 
void update_priest_flag (object *god, object *exp_ob, uint32 flag) {

    if(QUERY_FLAG(god,flag)&&!QUERY_FLAG(exp_ob,flag))
        SET_FLAG(exp_ob,flag);
    else if(QUERY_FLAG(exp_ob,flag)&&!QUERY_FLAG(god,flag))
        CLEAR_FLAG(exp_ob,flag);
}


/* determine_god() - determines if op worships a god. Returns
 * the godname if they do. In the case of an NPC, if they have
 * no god, we give them a random one. -b.t. 
 */

char *determine_god(object *op) {
    int godnr = -1;

    /* spells */
    if ((op->type == FBULLET || op->type == CONE || op->type == FBALL
         || op->type == SWARM_SPELL) && op->title) 
    {
	if(lookup_god_by_name(op->title)>=0) return op->title;
    }

    if(op->type!= PLAYER && QUERY_FLAG(op,FLAG_ALIVE)) {
	if(!op->title) {
	    godlink *gl=first_god;

	    godnr = (RANDOM()%gl->id) + 1;
	    while(gl) {
		if(gl->id==godnr) break;
		gl=gl->next;
	    }
	    op->title = add_string(gl->name);
	}
	return op->title;
    }


    /* If we are player, lets search a bit harder for the god.  This
     * is a fix for perceive self (before, we just looked at the active
     * skill.)
     */
    if(op->type==PLAYER) {
	object *tmp;

	for (tmp=op->inv; tmp!=NULL; tmp=tmp->below) {
	    if (tmp->exp_obj && tmp->exp_obj->title) 
		return tmp->exp_obj->title;
	}
    }
	
  return ("none");
} 


/* god_intervention() - called from praying() currently. Every
 * once in a while the god will intervene to help the worshiper.
 * Later, this fctn can be used to supply quests, etc for the 
 * priest. -b.t. 
 */

void god_intervention(object *op, object *god) {
  int level=SK_level(op);

  /*safety, shouldnt happen */
  if(!god) return;

  /* lets do some checks of whether we are kosher 
   * with our god */
  if(god_examines_priest(op,god)<0) return;

  new_draw_info(NDI_UNIQUE|NDI_UNIQUE,0,op,"You feel a holy presence!");

  /* So, what can we do to help out? We do some minimal checking
   * of the god's attributes to ensure there is a bit of consistency
   * between what happens and what the god can do. We do benefits 
   * in 2 sections. In the first, we check if the god can fix
   * problems of the priest. In the second, rare boons are given.
   * In either case, as soon as the god gives a gift, we return */
 

  /* FIRST SECTION */

  /* Restore the priest to a better state of grace. */ 
  if(op->stats.grace<0) {
        new_draw_info(NDI_UNIQUE,0,op,"You are returned to a state of grace.");
	op->stats.grace=RANDOM()%10;
	return;
  } 

 /* Heal damage */
  if(!(god->path_denied&PATH_RESTORE)) { 
     if(op->stats.hp<op->stats.maxhp) { 
	op->stats.hp=op->stats.maxhp;
        new_draw_info(NDI_UNIQUE,0,op,"A white light surrounds and heals you!");
	return;
     }
  }

 /* cure confusion? */
  if(!(god->attacktype&AT_CONFUSION))
    if(cast_heal(op,0,SP_CURE_CONFUSION)) return;

 /* remove poison? */
  if(!(god->attacktype&AT_POISON))
    if(cast_heal(op,0,SP_CURE_POISON)) return;

 /* Remove cursed/damned items? This is better than the spell, here we
  * allow unapplied items to be uncursed */
  if(!(RANDOM()%2)&&!(god->path_denied&PATH_TURNING)) {
    object *tmp;
    int success = 0;

    for (tmp = op->inv; tmp; tmp = tmp->below)
      if (QUERY_FLAG(tmp, FLAG_CURSED)||QUERY_FLAG(tmp, FLAG_DAMNED)) {
	    success++;
	    if(QUERY_FLAG(tmp, FLAG_DAMNED)) CLEAR_FLAG(tmp, FLAG_DAMNED);
	    CLEAR_FLAG(tmp, FLAG_CURSED);
	    CLEAR_FLAG(tmp, FLAG_KNOWN_CURSED);
	    esrv_send_item(op, tmp);
       }  
    if (op->type==PLAYER&&success) {
	new_draw_info(NDI_UNIQUE, 0,op, "You feel like someone is helping you.");
	return;
    }
  }

  /* Fix drained stats? */
  if(!(RANDOM()%2)&&!(god->attacktype&(AT_DRAIN||AT_DEPLETE))) {
     object *depl;
     archetype *at;

     if ((at = find_archetype("depletion"))==NULL) {
        LOG(llevError,"Could not find archetype depletion");
        return;
     }  
     depl = present_arch_in_ob(at, op);
     if (depl!=NULL) {   
        int i;
        new_draw_info(NDI_UNIQUE,0,op,"Shimmering light surrounds and restores you!");
        for (i = 0; i < 7; i++)
            if (get_attr_value(&depl->stats, i)) {
              new_draw_info(NDI_UNIQUE,0,op, restore_msg[i]);
            }
	remove_ob(depl);
	free_object(depl);
        fix_player(op);
        return;
     }
  }
  
  /* Special knowledge of the God? */
  if((!RANDOM()%10)&&!(god->path_denied&PATH_INFO)) { 
      if(god->slaying) {
          new_draw_info_format(NDI_UNIQUE,0,op,
	    "You are filled with a desire to slay all things which are %s.",
	    god->slaying);
	  return;
      } 
      if(god->race) {
          new_draw_info_format(NDI_UNIQUE,0,op,
	    "You feel a bond with all things which are %s.",
	    god->race);
	  return;
      } 
  }

  /* SECOND SECTION */

  /* Now, for the special section. If the priest is in a "state of grace"
   * (ie grace.stats>100&&grace>maxgrace) we get one of the good benefits
   * else, just a "super-blessing" via "holy possession" spell */

  /* blessing via "holy possesion" spell */
  if((op->stats.grace<80)||(op->stats.grace<op->stats.maxgrace)) {
     (void) cast_change_attr(op,op,0,SP_HOLY_POSSESSION);
     return;
  }

  /* Enchant/bless your weapon upto priest level/5  */
  if(!(RANDOM()%2)&&QUERY_FLAG(op,FLAG_READY_WEAPON)) {
    object *weapon=NULL;

    for(weapon=op->inv;weapon;weapon=weapon->below) 
       if(weapon->type==WEAPON&&QUERY_FLAG(weapon,FLAG_APPLIED)) break;

    if(weapon&&god_examines_item(god,weapon)>0) {
      /* allow the weapon to slay enemies */
      if(!weapon->slaying&& god->slaying) {
        char buf[MAX_BUF];

	weapon->slaying = add_string(god->slaying);
        new_draw_info(NDI_UNIQUE,0,op,"Your weapon quivers as if struck!"); 
	if(!weapon->title) {
          new_draw_info_format(NDI_UNIQUE,0,op,
             "Your %s now hungers to slay enemies of your god!",
	     weapon->name);
	  sprintf(buf,"of %s",god->name);
	  weapon->title=add_string(buf);
          if(op->type==PLAYER) 
	    esrv_update_item(UPD_NAME, op, weapon);
	}
	return;
      }
 
    /* add the gods attacktype*/
      if(!(RANDOM()%2)&&!(weapon->attacktype&god->attacktype)) {
	char buf[MAX_BUF];
        new_draw_info(NDI_UNIQUE,0,op,"Your weapon suddenly glows!");
	if (weapon->attacktype==0)
	    weapon->attacktype = AT_PHYSICAL;
	weapon->attacktype=weapon->attacktype|god->attacktype;

        if(!weapon->title) {
          new_draw_info_format(NDI_UNIQUE,0,op,
             weapon->name);
          sprintf(buf,"of %s",god->name);
          weapon->title=add_string(buf);
	  if(op->type==PLAYER)
	    esrv_update_item(UPD_NAME, op, weapon);
          }
	return;
      }

    /* higher magic value */
      if(!(RANDOM()%2)&&weapon->magic<(level/5)) {
        new_draw_info(NDI_UNIQUE,0,op,
          "A phosphorescent glow envelops your weapon!");
        weapon->magic++;
        if(op->type==PLAYER) 
	    esrv_update_item(UPD_NAME, op, weapon);
        return;
      }
    }
  }

 /* If they qualify, grant the priest use of a special spell */
 
  if(RANDOM() % 100 < learn_prayer_chance[op->stats.Wis]) { 

#define RARE_PRAYER(index) (spells[(index)].books == 0 && \
  spells[(index)].cleric && spells[(index)].path != PATH_NULL)

    static int rare_prayers = -1;
    int spell, i;

   /* get number of rare prayers */
    if (rare_prayers < 0) {
      for (rare_prayers = i = 0; i < NROFREALSPELLS; i++)
        if (RARE_PRAYER(i))
          rare_prayers++;
    }

   /*generate a random rare clerical spell*/  
    spell = RANDOM() % rare_prayers;

   /* find this spell */
    for (i = 0; i < NROFREALSPELLS; i++) {
      if (RARE_PRAYER(i)) {
        if (spell == 0)
          break;
        spell--;
      }
    }
    spell = i;

    /* The god will only teach the spell if its not against the nature
     * of the cult, the priest is high enough in level *and* the priest
     * doesnt already know it */
    /* Also, there are some spells which can really disturb playbalance,
     * to keep these out of player hands, we discard any spell which 
     * has PATH_NULL.  
     * (already checked in RARE_PRAYER)
     */
    /* Only teach spells of paths this god is attuned to.  Also check	*
     * that the spell has no repelled or denied paths, in case any	*
     * multi-path spells come along.  --DAMN				*/
    if ( (god->path_attuned  & spells[spell].path) &&
	!(god->path_repelled & spells[spell].path) &&
	!(god->path_denied   & spells[spell].path) &&
	 (spells[spell].level <= level) &&
	 (!check_spell_known(op,spell)) ) { 

    	play_sound_player_only(op->contr, SOUND_LEARN_SPELL,0,0); 
    	new_draw_info_format(NDI_UNIQUE, 0,op,
		"%s grants you use of a special prayer!",god->name); 
    	op->contr->known_spells[op->contr->nrofknownspells++]=spell; 
    	if(op->contr->nrofknownspells == 1) 
        	op->contr->chosen_spell=spell; 
    	new_draw_info_format(NDI_UNIQUE, 0, op, 
        	"Type 'bind cast %s",spells[spell].name); 
    	new_draw_info(NDI_UNIQUE, 0,op,"to store the spell in a key."); 
        return;
    } 

  } 
   
/* Last message, sorry charlie, nothing was given except good vibes :) */
  new_draw_info(NDI_UNIQUE, 0,op,"You feel rapture."); 

}

int god_examines_priest (object *op, object *god) {
  int reaction=1;
  object *item=NULL;

  for(item=op->inv;item;item=item->below) {
    if(QUERY_FLAG(item,FLAG_APPLIED)) {
      reaction+=god_examines_item(god,item)*(item->magic?abs(item->magic):1);
    }
  }

  /* well, well. Looks like we screwed up. Time for god's revenge */
  if(reaction<0) { 
    char buf[MAX_BUF];
    int loss = 10000000;
    int angry = abs(reaction);
    if(op->chosen_skill->exp_obj)
      loss = 0.05 * (float) op->chosen_skill->exp_obj->stats.exp;
    lose_priest_exp(op,RANDOM()%(loss*angry));
    if(RANDOM()%(angry+1)) 
      cast_mana_storm(op,SK_level(op)+(angry*3));
    sprintf(buf,"%s becomes angry and punishes you!",god->name);
    new_draw_info(NDI_UNIQUE|NDI_NAVY,0,op,buf); 
  }

  return reaction;
}

/* god_likes_item() - your god looks at the item you
 * are using and we return either -1 (bad), 0 (neutral) or
 * 1 (item is ok). If you are using the item of an enemy
 * god, it can be bad...-b.t. */

int god_examines_item(object *god, object *item) {
  char buf[MAX_BUF];

  if(!god||!item) return 0;

  if(!item->title) return 1; /* unclaimed item are ok */

  sprintf(buf,"of %s",god->name);
  if(!strcmp(item->title,buf)) return 1; /* belongs to that God */ 

  if(god->title) { /* check if we have any enemy blessed item*/ 
    sprintf(buf,"of %s",god->title);
    if(!strcmp(item->title,buf)) {
      if(item->env) {
        char buf[MAX_BUF];
        sprintf(buf,"Heretic! You are using %s!",query_name(item));
        new_draw_info(NDI_UNIQUE|NDI_NAVY,0,item->env,buf);
      }
      return -1;
    }
  }

  return 0; /* item is sacred to a non-enemy god/or is otherwise magical */ 
}

/* get_god() - returns the gods index in linked list 
 * if exists, if not, it returns -1. -b.t.  */

int get_god(object *priest) {
  int godnr=lookup_god_by_name(determine_god(priest)); 

  return godnr;
}


/* tailor_god_spell() - changes the attributes of cone, smite, 
 * and ball spells as needed by the code. Returns false if there
 * was no race to assign to the slaying field of the spell, but
 * the spell attacktype contains AT_HOLYWORD.  -b.t.
 */

int tailor_god_spell(object *spellop, object *caster) {
    object *god=find_god(determine_god(caster));
    int caster_is_spell=0; 

    if(caster->type==FBULLET
       ||caster->type==CONE
       ||caster->type==FBALL
       ||caster->type==SWARM_SPELL) caster_is_spell=1;

    if ( ! god || (spellop->attacktype & AT_HOLYWORD && ! god->race)) {
        if ( ! caster_is_spell)
            new_draw_info(NDI_UNIQUE, 0, caster,
              "This prayer is useless unless you worship an appropriate god");
        else
            LOG (llevError, "BUG: tailor_god_spell(): no god\n");
        free_object(spellop);
        return 0;
    }

    /* either holy word or godpower attacks will set the slaying field */
    if(spellop->attacktype&AT_HOLYWORD||spellop->attacktype&AT_GODPOWER) { 
         if (spellop->slaying) {
             free_string(spellop->slaying);
             spellop->slaying = NULL;
         }
         if(!caster_is_spell)
            spellop->slaying = add_string(god->slaying);
	 else if(caster->slaying) 
	    spellop->slaying = add_string(caster->slaying);
    }

    /* only the godpower attacktype adds the god's attack onto the spell */
    if(spellop->attacktype & AT_GODPOWER)
         spellop->attacktype=spellop->attacktype|god->attacktype;

    /* tack on the god's name to the spell */
    if(spellop->attacktype&AT_HOLYWORD||spellop->attacktype&AT_GODPOWER) { 
         if(spellop->title) 
	   free_string(spellop->title);
         spellop->title=add_string(god->name);
         if(spellop->title){
	   char buf[MAX_BUF]; 
	   sprintf(buf,"%s of %s",spellop->name,spellop->title);
	   if (spellop->name) free_string(spellop->name);
	   spellop->name=add_string(buf);
	}
    } 

    return 1;
}

void lose_priest_exp(object *pl, int loss) {

  if(!pl||pl->type!=PLAYER||!pl->chosen_skill
     ||!pl->chosen_skill->exp_obj) 
  {
    LOG(llevError,"Bad call to lose_priest_exp() \n");
    return;
  }
  if((loss = check_dm_add_exp_to_obj(pl->chosen_skill->exp_obj,loss))) {
    pl->chosen_skill->exp_obj->stats.exp -= loss;
    pl->stats.exp -= loss;
    add_exp(pl,0);
  }
}
