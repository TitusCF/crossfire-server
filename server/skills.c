/*
 * static char *rcsid_skills_c =
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

/* Initial coding: 6 Sep 1994, Nick Williams (njw@cs.city.ac.uk) */
 
/* Generalized code + added hiding and lockpicking skills, */
/* March 3, 1995, brian thomas (thomas@astro.psu.edu) */

/* Added more skills, fixed bug in stealing code */
/* April 21, 1995, brian thomas (thomas@astro.psu.edu) */
 
/* Added more skills, fixed bugs, see skills.h */
/* May/June, 1995, brian thomas (thomas@astro.psu.edu) */

/* July 95 Code re-vamped. Now we add the experience objects, all
 * player activities which gain experience will bbe through the use
 * of skillls. Thus, I added hand_weapons, missile_weapons, and 
 * remove_traps skills -b.t. 
 */

/* Aug 95 - Added more skills (disarm traps, spellcasting, praying). 
 * Also, hand_weapons is now "melee_weapons". b.t. 
 */

/* Oct 95 - changed the praying skill to accomodate MULTIPLE_GODS
 * hack - b.t.
 */

/* Dec 95 - modified the literacy and inscription (writing) skills. b.t. 
 */ 
 
/* Mar 96 - modified the stealing skill. Objects with type FLESH or
 * w/o a type cannot be stolen by players. b.t.
 */

/* Sept 96 - changed parsing of params through use_skill command, also
 * added in throw skill -b.t. */

/* Oct 96 - altered hiding and stealing code for playbalance. -b.t. */

/* Sept 97 - yet another alteration to the stealing code. Lets allow
 * multiple stealing, after having alerted the victim. But only subsequent
 * steals made while we are unseen (in dark, invisible, hidden) will have
 * any chance of success. Also, on each subsequent attempt, we raise the
 * wisdom of the npc a bit, which makes it ultimately possible for the
 * npc to detect the theif, regardless of the situation. -b.t. */

#include <global.h>
#include <object.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <living.h>
#include <skills.h>
#include <spells.h>
#include <book.h>

/* 
 * When stealing: dependent on the intelligence/wisdom of whom you're
 * stealing from (op in attempt_steal), offset by your dexterity and
 * skill at stealing. They may notice your attempt, whether successful
 * or not. 
 */

int attempt_steal(object* op, object* who)
{
    object *success=NULL, *tmp=NULL, *next;
    int roll=0, chance=0, stats_value = get_weighted_skill_stats(who)*3;
    int victim_lvl=op->level*3, thief_lvl = SK_level(who)*10;
    rv_vector	rv;

    /* if the victim is aware of a thief in the area (FLAG_NO_STEAL set on them)
     * they will try to prevent stealing if they can. Only unseen theives will
     * have much chance of success. 
     */
    if(op->type!=PLAYER && QUERY_FLAG(op,FLAG_NO_STEAL)) {
	if(can_detect_enemy(op,who,&rv)) {
	    npc_call_help(op);
	    CLEAR_FLAG(op, FLAG_UNAGGRESSIVE);
	    new_draw_info(NDI_UNIQUE, 0,who,"Your attempt is prevented!");
	    return 0;
	} else /* help npc to detect thief next time by raising its wisdom */ 
	    op->stats.Wis += (op->stats.Int/5)+1;
    }

    /* Ok then, go thru their inventory, stealing */
    for(tmp = op->inv; tmp != NULL; tmp = next) {
	next = tmp->below;

	/* you can't steal worn items, starting items, wiz stuff, 
	 * innate abilities, or items w/o a type. Generally 
	 * speaking, the invisibility flag prevents experience or 
	 * abilities from being stolen since these types are currently
	 * always invisible objects. I was implicit here so as to prevent 
	 * future possible problems. -b.t. 
	 * Flesh items generated w/ fix_flesh_item should have FLAG_NO_STEAL 
	 * already  -b.t. 
	 */

	if (QUERY_FLAG(tmp,FLAG_WAS_WIZ) || QUERY_FLAG(tmp, FLAG_APPLIED) 
	    || !(tmp->type)
	    || tmp->type == EXPERIENCE || tmp->type == ABILITY
	    || QUERY_FLAG(tmp,FLAG_STARTEQUIP)
	    || QUERY_FLAG(tmp,FLAG_NO_STEAL)
	    || tmp->invisible ) continue;

	/* Okay, try stealing this item. Dependent on dexterity of thief,
	 * skill level, see the adj_stealroll fctn for more detail. */

	roll=die_roll(2, 100, who, PREFER_LOW)/2; /* weighted 1-100 */ 

	if((chance=adj_stealchance(who,op,(stats_value+thief_lvl-victim_lvl)))==-1)
	    return 0;
	else if (roll < chance ) {
	    if (op->type == PLAYER)
		esrv_del_item(op->contr, tmp->count);
	    pick_up(who, tmp);
	    if(can_pick(who,tmp)) {
		/* for players, play_sound: steals item */
		success = tmp;
		CLEAR_FLAG(tmp, FLAG_INV_LOCKED);
	    }
	    break;
	}
    } /* for loop looking for an item */

    /* If you arent high enough level, you might get something BUT
     * the victim will notice your stealing attempt. Ditto if you
     * attempt to steal something heavy off them, they're bound to notice 
     */

    if((roll>=SK_level(who))||!chance
      ||(tmp&&tmp->weight>(250*(random_roll(0, stats_value+thief_lvl-1, who, PREFER_LOW))))) {

	/* victim figures out where the thief is! */
	if(who->hide) make_visible(who);

	if(op->type != PLAYER) {

	    /* The unaggressives look after themselves 8) */
	    if(who->type==PLAYER) {
		npc_call_help(op);
		new_draw_info_format(NDI_UNIQUE, 0,who,
		  "%s notices your attempted pilfering!",query_name(op));
	    }
	    CLEAR_FLAG(op, FLAG_UNAGGRESSIVE);
	    /* all remaining npc items are guarded now. Set flag NO_STEAL 
	     * on the victim. */
	    SET_FLAG(op,FLAG_NO_STEAL);
	} else { /* stealing from another player */
	    char buf[MAX_BUF];
	    /* Notify the other player */
	    if (success && who->stats.Int > random_roll(0, 19, op, PREFER_LOW)) {
		sprintf(buf, "Your %s is missing!", query_name(success));
		} else {
		    sprintf(buf, "Your pack feels strangely lighter.");
		}
	    new_draw_info(NDI_UNIQUE, 0,op,buf);
	    if (!success) {
		if (who->invisible) {
		    sprintf(buf, "you feel itchy fingers getting at your pack.");
		} else {
		    sprintf(buf, "%s looks very shifty.", query_name(who));
		}
		new_draw_info(NDI_UNIQUE, 0,op,buf);
	    }
	} /* else stealing from another player */
	/* play_sound("stop! thief!"); kindofthing */
    } /* if you weren't 100% successful */
    return success? 1:0;
}

/* adj_stealchance() - increased values indicate better attempts */ 

int adj_stealchance (object *op, object *victim, int roll) {
  object *equip;
  int used_hands=0;

  if(!op||!victim||!roll) return -1;

  /* ADJUSTMENTS */

  /* Its harder to steal from hostile beings! */
  if(!QUERY_FLAG(victim, FLAG_UNAGGRESSIVE)) roll = roll/2;

  /* Easier to steal from sleeping beings, or if the thief is 
   * unseen */
  if(QUERY_FLAG(victim, FLAG_SLEEP)) 
    roll = roll*3;
  else if(op->invisible)  
    roll = roll*2;

  /* check stealing 'encumberance'. Having this equipment applied makes
   * it quite a bit harder to steal. */
  for(equip=op->inv;equip;equip=equip->below) { 
    if(equip->type==WEAPON&&QUERY_FLAG(equip,FLAG_APPLIED)) { 
      roll -= equip->weight/10000;
      used_hands++;
    }
    if(equip->type==BOW&&QUERY_FLAG(equip,FLAG_APPLIED)) 
      roll -= equip->weight/5000;
    if(equip->type==SHIELD&&QUERY_FLAG(equip,FLAG_APPLIED)) { 
      roll -= equip->weight/2000;
      used_hands++;
    }
    if(equip->type==ARMOUR&&QUERY_FLAG(equip,FLAG_APPLIED)) 
      roll -= equip->weight/5000;
    if(equip->type==GLOVES&&QUERY_FLAG(equip,FLAG_APPLIED)) 
      roll -= equip->weight/100;
  }
  
  if(roll<0) roll=0;
  if(op->type==PLAYER && used_hands>=2) {
    new_draw_info(NDI_UNIQUE, 0,op,"But you have no free hands to steal with!");
    roll=-1;
  }

  return roll;
}

int steal(object* op, int dir)
{
    object *tmp, *next;
    int x = op->x + freearr_x[dir];
    int y = op->y + freearr_y[dir];

    if(dir == 0) {
	/* Can't steal from ourself! */
	return 0;
    }

    if(wall(op->map,x,y)) {
	return 0;
    }

    /* Find the topmost object at this spot */
    for(tmp = get_map_ob(op->map,x,y);
	tmp != NULL && tmp->above != NULL;
        tmp = tmp->above);

    /* For all the stacked objects at this point, attempt a steal */
    for(; tmp != NULL; tmp = next) {
      next = tmp->below;
      /* Minor hack--for multi square beings - make sure we get 
       * the 'head' coz 'tail' objects have no inventory! - b.t. 
       */ 
      if (tmp->head) tmp=tmp->head;
      if(tmp->type!=PLAYER&&!QUERY_FLAG(tmp, FLAG_MONSTER)) continue;
      if (attempt_steal(tmp, op)) {
	  if(tmp->type==PLAYER) /* no xp for stealing from another player */
	    return 0;
	  else return (calc_skill_exp(op,tmp));
      }
    }
    return 0;
}

/* Implementation by bt. (thomas@astro.psu.edu)
 * monster implementation 7-7-95 by bt.
 */

int pick_lock(object *pl, int dir)
{
    char buf[MAX_BUF];
    object *tmp; 
    int x = pl->x + freearr_x[dir];
    int y = pl->y + freearr_y[dir];
    int success = 0;

    if(!dir) dir=pl->facing;

/* For all the stacked objects at this point find a door*/

    sprintf(buf, "There is no lock there.");
    if (out_of_map(pl->map,x,y)) {
      new_draw_info(NDI_UNIQUE, 0,pl,buf);
      return 0;
    }

    for(tmp=get_map_ob(pl->map,x,y); tmp; tmp=tmp->above) {
      if(!tmp) continue;
      switch(tmp->type) { 
        case DOOR:
	    if (!QUERY_FLAG(tmp, FLAG_NO_PASS)) {
		strcpy(buf,"The door has no lock!");
	    } else {
		if (attempt_pick_lock(tmp, pl)) { 
		    success = 1;
		    sprintf(buf, "you pick the lock.");
		} else {
		    sprintf(buf, "you fail to pick the lock.");
		}
	    }
	    break;
        case LOCKED_DOOR: 
            sprintf(buf, "you can't pick that lock!");
	    break;
   	default: 
	    break;
      }
    }
    new_draw_info(NDI_UNIQUE, 0,pl,buf);
    if(success)
        return calc_skill_exp(pl,NULL);
    else 
	return 0;
}      

int attempt_pick_lock ( object *door, object *pl)
{
    int bonus = SK_level(pl);
    int difficulty= pl->map->difficulty ? pl->map->difficulty : 0;
    int dex = get_skill_stat1(pl) ? get_skill_stat1(pl) : 10;
    int success = 0, number;        /* did we get anything? */

  /* If has can_pass set, then its not locked! */
   if(!QUERY_FLAG(door,FLAG_NO_PASS)) return 0;

  /* Try to pick the lock on this item (doors only for now). 
   * Dependent on dexterity/skill SK_level of the player and  
   * the map level difficulty. 
   */

    number = (die_roll(2, 40, pl, PREFER_LOW)-2)/2; 
    if (number < ((dex + bonus) - difficulty)) { 
      remove_door(door);
      success = 1;
    } else if (door->inv && door->inv->type==RUNE) {  /* set off any traps? */ 
		spring_trap(door->inv,pl); 	       
    } 
    return success;
}

/* HIDE CODE. The user becomes undetectable (not just 'invisible') for
 * a short while (success and duration dependant on player SK_level,
 * dexterity, charisma, and map difficulty). 
 * Players have a good chance of becoming 'unhidden' if they move
 * and like invisiblity will be come visible if they attack
 * Implemented by b.t. (thomas@astro.psu.edu)
 * July 7, 1995 - made hiding possible for monsters. -b.t.
 */ 

/* patched this to take terrain into consideration */

int hide(object *op) {
  char buf[MAX_BUF];
  int level= SK_level(op);

/* the preliminaries -- Can we really hide now? */
/* this keeps monsters from using invisibilty spells and hiding */

  if (QUERY_FLAG(op, FLAG_MAKE_INVIS)) {
        sprintf(buf,"You don't need to hide while invisible!");
        new_draw_info(NDI_UNIQUE, 0,op,buf);
        return 0;
  } else if (!op->hide && op->invisible>0 && op->type == PLAYER) { 
        sprintf(buf,"Your attempt to hide breaks the invisibility spell!"); 
        new_draw_info(NDI_UNIQUE, 0,op,buf);
 	make_visible(op);
        return 0;
  } 
 
  if(op->invisible>(50*level)) {
       new_draw_info(NDI_UNIQUE,0,op,"You are as hidden as you can get."); 
       return 0;
  }
  
  if(attempt_hide(op)) { 
     new_draw_info(NDI_UNIQUE, 0,op,"You hide in the shadows.");
     update_object(op,UP_OBJ_FACE);
     return calc_skill_exp(op, NULL);
  } 
  new_draw_info(NDI_UNIQUE,0,op,"You fail to conceal yourself.");
  return 0; 
}

int attempt_hide(object *op) {
  int level = SK_level(op);
  int success=0,number,difficulty=op->map->difficulty;
  int dexterity = get_skill_stat1(op); 
  int terrain = hideability(op);

  level = level>5?level/5:1;

  /* first things... no hiding next to a hostile monster */
  dexterity = dexterity ? dexterity : 15;

  if(terrain<-10) /* not enough cover here */
    return 0;

/*  Hiding success and duration dependant on SK_level,
 *  dexterity, map difficulty and terrain. 
 */

  number = (die_roll(2, 25, op, PREFER_LOW)-2)/2;
  if(!stand_near_hostile(op) && number  
     && (number < (dexterity + level + terrain - difficulty ))) 
  {
    success = 1;
    op->invisible += 100;  /* set the level of 'hiddeness' */
    if(op->type==PLAYER)
	op->contr->tmp_invis=1;
    op->hide=1;
  }
  return success;
}

/* stop_jump() - End of jump. Clear flags, restore the map, and 
 * freeze the jumper a while to simulate the exhaustion
 * of jumping.
 */

static int stop_jump(object *pl, int dist, int spaces) {
    /* int load=dist/(pl->speed*spaces); */ 

    CLEAR_FLAG(pl,FLAG_FLYING);
    insert_ob_in_map(pl,pl->map,pl,0);
    if (pl->type==PLAYER) draw(pl);  

    /* pl->speed_left= (int) -FABS((load*8)+1); */ 
    return 0;
}


static int attempt_jump (object *pl, int dir, int spaces) {
    object *tmp;
    int i,exp=0,dx=freearr_x[dir],dy=freearr_y[dir];

    /* Jump loop. Go through spaces opject wants to jump. Halt the
     * jump if a wall or creature is in the way. We set FLAG_FLYING
     * temporarily to allow player to aviod exits/archs that are not 
     * fly_on, fly_off. This will also prevent pickup of objects 
     * while jumping over them.  
     */ 

    remove_ob(pl);
    SET_FLAG(pl,FLAG_FLYING);
    for(i=0;i<=spaces;i++) { 
	if (out_of_map(pl->map,pl->x+dx,pl->y+dy)) {
	    (void) stop_jump(pl,i,spaces);
	    return calc_skill_exp(pl,NULL);
	}
	for(tmp=get_map_ob(pl->map,pl->x+dx,pl->y+dy); tmp;tmp=tmp->above) { 
	    if(wall(tmp->map,tmp->x,tmp->y)) {           /* Jump into wall*/ 
		new_draw_info(NDI_UNIQUE, 0,pl,"Your jump is blocked.");
		(void) stop_jump(pl,i,spaces);
		return 0;
	    }
	    /* Jump into creature */ 
	    if(QUERY_FLAG(tmp,FLAG_MONSTER) || tmp->type==PLAYER ) {   
		new_draw_info_format(NDI_UNIQUE, 0,pl,"You jump into%s%s.", 
		    tmp->type == PLAYER ? " " : " the ", tmp->name);
		if(tmp->type!=PLAYER || 
		   (pl->type==PLAYER && pl->contr->party_number==-1) ||
		   (pl->type==PLAYER && tmp->type==PLAYER &&
		    pl->contr->party_number!=tmp->contr->party_number)) 
 	     	    exp = skill_attack(tmp,pl,pl->facing,"kicked"); /* pl makes an attack */ 
		(void) stop_jump(pl,i,spaces);
		return exp;  /* note that calc_skill_exp() is already called by skill_attack() */ 
	    }
	     /* If the space has fly on set (no matter what the space is),
	      * we should get the effects - after all, the player is
	      * effectively flying.
	      */
	    if(QUERY_FLAG(tmp, FLAG_FLY_ON)) { 
		pl->x+=dx,pl->y+=dy;
		(void) stop_jump(pl,i,spaces);
		return calc_skill_exp(pl,NULL);
	    }
	}
	pl->x+=dx;
	pl->y+=dy;
    }
    (void) stop_jump(pl,i,spaces);
    return calc_skill_exp(pl,NULL);
}

/* jump() - this is both a new type of movement for player/monsters and
 * an attack as well. -b.t.
 */ 

int jump(object *pl, int dir) 
{
    int spaces=0,stats;
    int str = get_skill_stat1(pl); 
    int dex = get_skill_stat2(pl);

    dex = dex ? dex : 15;
    str = str ? str : 10; 

    stats=str*str*str*dex;

    if(pl->carrying!=0)		/* don't want div by zero !! */	 
	spaces=(int) (stats/pl->carrying);
    else
	spaces=2;	/* pl has no objects - gets the far jump */ 

    if(spaces>2)
	 spaces = 2;
    else if(spaces==0) {
	new_draw_info(NDI_UNIQUE, 0,pl,"You are carrying too much weight to jump.");
	return 0;
    }
    return attempt_jump(pl,dir,spaces);
}


/* skill_ident() - this code is supposed to allow players to identify 
 * classes of objects with the various "auto-ident" skills. Player must 
 * have unidentified objects of the right type in order for the skill
 * to work. While multiple classes of objects may be identified, 
 * this code is kind of yucky -- it would be nice to make it a bit
 * more generalized. Right now, skill indices are embedded in this routine.  
 * Returns amount of experience gained (on successful ident).
 * - b.t. (thomas@astro.psu.edu) 
 */

int skill_ident(object *pl) {
  char buf[MAX_BUF];
  int success=0;

	if(!pl->chosen_skill) 	/* should'nt happen... */ 
		return 0;

	if(pl->type != PLAYER) return 0;  /* only players will skill-identify */

    	sprintf(buf, "You look at the objects nearby...");
    	new_draw_info(NDI_UNIQUE, 0,pl,buf);   

	switch (pl->chosen_skill->stats.sp) {
	   case SK_SMITH: 
	      	success += do_skill_ident(pl,WEAPON) + do_skill_ident(pl,ARMOUR)
		    + do_skill_ident(pl,BRACERS) + do_skill_ident(pl,CLOAK)
		    + do_skill_ident(pl,BOOTS) + do_skill_ident(pl,SHIELD)
		    + do_skill_ident(pl,GIRDLE) + do_skill_ident(pl,HELMET) 
		    + do_skill_ident(pl,GLOVES);
		break;
	   case SK_BOWYER:
		success += do_skill_ident(pl,BOW) + do_skill_ident(pl,ARROW);
                break;
	   case SK_ALCHEMY:
                success += do_skill_ident(pl,POTION) + do_skill_ident(pl,POISON)
		    + do_skill_ident(pl,AMULET) + do_skill_ident(pl,CONTAINER)
		    + do_skill_ident(pl,DRINK) + do_skill_ident(pl,INORGANIC);
		break;
           case SK_WOODSMAN:  
                success += do_skill_ident(pl,FOOD) + do_skill_ident(pl,DRINK) 
		    + do_skill_ident(pl,FLESH);
                break; 
           case SK_JEWELER:
                success += do_skill_ident(pl,GEM) + do_skill_ident(pl,RING);
                break; 
	   case SK_LITERACY:
                success += do_skill_ident(pl,SPELLBOOK) 
		    + do_skill_ident(pl,SCROLL) + do_skill_ident(pl,BOOK);
		break;
           case SK_THAUMATURGY:
                success += do_skill_ident(pl,WAND) + do_skill_ident(pl,ROD) 
		    + do_skill_ident(pl,HORN);
                break;
	   case SK_DET_CURSE:
                success = do_skill_detect_curse(pl);
                if(success) 
                   new_draw_info(NDI_UNIQUE, 0,pl,"...and discover cursed items!");
                break;   
	   case SK_DET_MAGIC:
		success = do_skill_detect_magic(pl);
		if(success)
               	   new_draw_info(NDI_UNIQUE, 0,pl,
		      "...and discover items imbued with mystic forces!");
		break;
	   default:
		LOG(llevError,"Error: bad call to skill_ident()");
		return 0;
		break;
	} 
	if(!success) {
		sprintf(buf,"...and learn nothing more.");
    		new_draw_info(NDI_UNIQUE, 0,pl,buf);
	}

        return success;
}
 
int do_skill_detect_curse(object *pl) {
object *tmp;
int success=0;
 
/* check the player inventory - stop after 1st success or 
 * run out of unidented items 
 */ 
    for(tmp=pl->inv;tmp;tmp=tmp->below)
        if(!QUERY_FLAG(tmp,FLAG_IDENTIFIED) && !QUERY_FLAG(tmp,FLAG_KNOWN_CURSED)
            && (QUERY_FLAG(tmp,FLAG_CURSED) || QUERY_FLAG(tmp,FLAG_DAMNED)) ) {
                SET_FLAG(tmp,FLAG_KNOWN_CURSED);
		esrv_update_item(UPD_FLAGS, pl, tmp);
                success+=calc_skill_exp(pl,tmp);
        }
    return success;
}

int do_skill_detect_magic(object *pl) {
object *tmp;
int success=0;

/* check the player inventory - stop after 1st success or
 * run out of unidented items
 */
    for(tmp=pl->inv;tmp;tmp=tmp->below)
        if(!QUERY_FLAG(tmp,FLAG_IDENTIFIED) && !QUERY_FLAG(tmp,FLAG_KNOWN_MAGICAL)
	    && (is_magical(tmp)) ) { 
            	SET_FLAG(tmp,FLAG_KNOWN_MAGICAL);
		esrv_update_item(UPD_FLAGS, pl, tmp);
		success+=calc_skill_exp(pl,tmp);
	}
    return success;
}

/* Helper function for do_skill_ident, so that we can loop
over inventory AND objects on the ground conveniently.  */
int do_skill_ident2(object *tmp,object *pl, int obj_class) 
{
   int success=0,chance;
  int skill_value = SK_level(pl) + get_weighted_skill_stats(pl);

	if(!QUERY_FLAG(tmp,FLAG_IDENTIFIED) && !QUERY_FLAG(tmp,FLAG_NO_SKILL_IDENT) 
	   && need_identify(tmp) 
	   && !tmp->invisible && tmp->type==obj_class) { 
		chance = die_roll(3, 10, pl, PREFER_LOW)-3 +
			rndm(0, (tmp->magic ? tmp->magic*5 : 1)-1); 
		if(skill_value >= chance) {
		  identify(tmp);
     		  if (pl->type==PLAYER) {
        	    new_draw_info_format(NDI_UNIQUE, 0, pl,
                      "You identify %s.", long_desc(tmp));
        	    if (tmp->msg) {
          		new_draw_info(NDI_UNIQUE, 0,pl, "The item has a story:");
          		new_draw_info(NDI_UNIQUE, 0,pl, tmp->msg);
        	    }
		    /* identify will take care of updating the item if
		     * it is in the players inventory.  IF on map, do it
		     * here
		     */
		    if (tmp->map)
			esrv_send_item(pl, tmp);
		  }
	          success += calc_skill_exp(pl,tmp);
        	} else 
		  SET_FLAG(tmp, FLAG_NO_SKILL_IDENT);
        }
	return success;
}
/* do_skill_ident() - workhorse for skill_ident() -b.t. */
/*  Sept 95. I put in a probability for identification of artifacts.
 *  highly magical artifacts will be more difficult to ident -b.t.
 */
int do_skill_ident(object *pl, int obj_class) {
  object *tmp;
  int success=0; 
/* check the player inventory */
    for(tmp=pl->inv;tmp;tmp=tmp->below)
		success+=do_skill_ident2(tmp,pl,obj_class);
	 /*  check the ground */
	 for(tmp=get_map_ob(pl->map,pl->x,pl->y);tmp;tmp=tmp->above)
		success+=do_skill_ident2(tmp,pl,obj_class);
		  
    return success;
}  

/* players using this skill can 'charm' a monster --
 * into working for them. It can only be used on 
 * non-special (see below) 'neutral' creatures. 
 * -b.t. (thomas@astro.psu.edu)
 */

int use_oratory(object *pl, int dir) {
    int x=pl->x+freearr_x[dir],y=pl->y+freearr_y[dir],chance;
    int stat1 = get_skill_stat1(pl);
    object *tmp;
 
    if(pl->type!=PLAYER) return 0;	/* only players use this skill */ 
    if(out_of_map(pl->map,x,y)) return 0;

    for(tmp=get_map_ob(pl->map,x,y);tmp;tmp=tmp->above) { 
       	if(!tmp) return 0;
	if(!QUERY_FLAG(tmp,FLAG_MONSTER)) continue; 
        /* can't persude players - return because there is nothing else
	 * on that space to charm.  Same for multi space monsters and
	 * special monsters - we don't allow them to be charmed, and there
	 * is no reason to do further processing since they should be the
	 * only monster on the space.
	 */ 
       	if(tmp->type==PLAYER) return 0;
       	if(tmp->more || tmp->head) return 0;
	if(tmp->msg) return 0;


	new_draw_info_format(NDI_UNIQUE, 
			0,pl, "You orate to the %s.",query_name(tmp));

	/* the following conditions limit who may be 'charmed' */

	/* it's hostile! */ 
	if(!QUERY_FLAG(tmp,FLAG_UNAGGRESSIVE) && !QUERY_FLAG(tmp, FLAG_FRIENDLY)) {  
               new_draw_info_format(NDI_UNIQUE, 0,pl, 
			   "Too bad the %s isn't listening!\n",query_name(tmp));
		return 0;	
	}
	/* it's already allied! */ 
        if(QUERY_FLAG(tmp,FLAG_FRIENDLY)&&(tmp->move_type==PETMOVE)){  
	    if(get_owner(tmp)==pl) {
		new_draw_info(NDI_UNIQUE, 0,pl, 
			      "Your follower loves your speach.\n");
		return 0;
	    } else if(SK_level(pl)>tmp->level) { /* you steal the follower! */
	   	set_owner(tmp,pl);	
                new_draw_info_format(NDI_UNIQUE, 0,pl, 
			"You convince the %s to follow you instead!\n",
			query_name(tmp));
		/* Abuse fix - don't give exp since this can otherwise
		 * be used by a couple players to gets lots of exp.
		 */
		return 0;
	    }
	} /* Creature was already a pet of someone */

	chance=SK_level(pl)*2+(stat1-2*tmp->stats.Int)/2;

	/* Ok, got a 'sucker' lets try to make them a follower */
       	if(chance>0 && tmp->level<(random_roll(0, chance-1, pl, PREFER_HIGH)-1)) {
	    new_draw_info_format(NDI_UNIQUE, 0,pl, 
		"You convince the %s to become your follower.\n", 
		query_name(tmp));

	    set_owner(tmp,pl);
	    SET_FLAG(tmp,FLAG_MONSTER);
	    tmp->stats.exp = 0;
	    add_friendly_object(tmp);
	    SET_FLAG(tmp,FLAG_FRIENDLY);
	    tmp->move_type = PETMOVE;
	    return calc_skill_exp(pl,tmp);
	}
	/* Charm failed.  Creature may be angry now */
       	else if((SK_level(pl)+((stat1-10)/2)) <
		random_roll(1, 2*tmp->level, pl, PREFER_LOW)) {
	    new_draw_info_format(NDI_UNIQUE, 0,pl, 
		  "Your speach angers the %s!\n",query_name(tmp)); 
	    if(QUERY_FLAG(tmp,FLAG_FRIENDLY)) {
		CLEAR_FLAG(tmp,FLAG_FRIENDLY);
		remove_friendly_object(tmp);
		tmp->move_type = 0; 	/* needed? */ 
	    }
	    CLEAR_FLAG(tmp,FLAG_UNAGGRESSIVE);
	}
    } /* For loop cyclign through the objects on this space */
    return 0;	/* Fall through - if we get here, we didn't charm anything */
}

/* Singing() -this skill allows the player to pacify nearby creatures.
 * There are few limitations on who/what kind of 
 * non-player creatures that may be pacified. Right now, a player
 * may pacify creatures which have Int == 0. In this routine, once
 * successfully pacified the creature gets Int=1. Thus, a player 
 * may only pacify a creature once. 
 * BTW, I appologize for the naming of the skill, I couldnt think
 * of anything better! -b.t. 
 */

int singing(object *pl, int dir) {
    int i,exp = 0,stat1=get_skill_stat1(pl),chance;
    object *tmp;

    if(pl->type!=PLAYER) return 0;    /* only players use this skill */

    new_draw_info_format(NDI_UNIQUE,0,pl, "You sing");
    for(i=dir;i<(dir+MIN(SK_level(pl),SIZEOFFREE));i++) {
	if (out_of_map(pl->map,pl->x+freearr_x[i],pl->y+freearr_y[i]))
		continue;
        for(tmp=get_map_ob(pl->map,pl->x+freearr_x[i],pl->y+freearr_y[i]);
		  tmp;tmp=tmp->above) {
            if(!tmp) return 0;
	    if(!QUERY_FLAG(tmp,FLAG_MONSTER)) continue;

	    /* can't affect players */
            if(tmp->type==PLAYER) break;

	    /* Only the head listens to music - not other parts.  head
	     * is only set if an object has extra parts.  This is also
	     * necessary since the other parts may not have appropriate
	     * skills/flags set.
	     */
	    if (tmp->head) break;

	    /* the following monsters can't be calmed */

	    if(QUERY_FLAG(tmp,FLAG_SPLITTING)	/* have no ears! */ 
	       || QUERY_FLAG(tmp,FLAG_HITBACK)) break;	

/*	    if(tmp->stats.Int>0) break;	*//* is too smart */
	    if(tmp->level>SK_level(pl)) break;	/* too powerfull */
	    if(QUERY_FLAG(tmp,FLAG_UNDEAD)) break; /* undead dont listen! */ 

	    if(QUERY_FLAG(tmp,FLAG_UNAGGRESSIVE) /* already calm */
	       ||QUERY_FLAG(tmp,FLAG_FRIENDLY)) 
			break;	

	    /* stealing isn't really related (although, maybe it should
	     * be).  This is mainly to prevent singing to the same monster
	     * over and over again and getting exp for it.
	     */
	    chance=SK_level(pl)*2+(stat1-5-tmp->stats.Int)/2;
	    if(chance && tmp->level*2<random_roll(0, chance-1, pl, PREFER_HIGH)) {
		SET_FLAG(tmp,FLAG_UNAGGRESSIVE);
		new_draw_info_format(NDI_UNIQUE, 0,pl,
                   "You calm down the %s\n",query_name(tmp));
		tmp->stats.Int = 1; /* this prevents re-pacification */
		/* Give exp only if they are not aware */
		if(!QUERY_FLAG(tmp,FLAG_NO_STEAL))
		    exp += calc_skill_exp(pl,tmp);
		SET_FLAG(tmp,FLAG_NO_STEAL);
	    } else { 
                 new_draw_info_format(NDI_UNIQUE, 0,pl,
                 	"Too bad the %s isn't listening!\n",query_name(tmp));
	    }
	}
    }
    return exp;
}

/* The FIND_TRAPS skill. This routine is taken mostly from the 
 * command_search loop. It seemed easier to have a separate command,
 * rather than overhaul the existing code - this makes sure things 
 * still work for those people who don't want to have skill code 
 * implemented.
 */

int find_traps (object *pl) {  
   object *tmp,*tmp2;
   int i,expsum=0;
  /*First we search all around us for runes and traps, which are
    all type RUNE */
   for(i=0;i<9;i++) { 
        /*  Check everything in the square for trapness */
        if(out_of_map(pl->map,pl->x + freearr_x[i],pl->y + freearr_y[i])) continue;
        for(tmp = get_map_ob(pl->map, pl->x + freearr_x[i], pl->y +freearr_y[i]);
            tmp!=NULL;tmp=tmp->above) {

            /*  And now we'd better do an inventory traversal of each
                of these objects' inventory */

            for(tmp2=tmp->inv;tmp2!=NULL;tmp2=tmp2->below)
                if(tmp2->type==RUNE)  
		  if(trap_see(pl,tmp2)) { 
			trap_show(tmp2,tmp); 
  			if(tmp2->stats.Cha>1) { 
			    if (!tmp2->owner || tmp2->owner->type!=PLAYER)
				expsum += calc_skill_exp(pl,tmp2);
			    /* do the following so calc_skill_exp will know 
			     * how much xp to award for disarming
			     */
			    tmp2->stats.exp = tmp2->stats.Cha * tmp2->level; 
			    tmp2->stats.Cha = 1; /* unhide the trap */ 
			}
		  }
            if(tmp->type==RUNE)  
		  if(trap_see(pl,tmp)) { 
			trap_show(tmp,tmp); 
  			if(tmp->stats.Cha>1) {
			    if (!tmp->owner || tmp->owner->type!=PLAYER)
				expsum += calc_skill_exp(pl,tmp);
			    /* do the following so calc_skill_exp will know 
			     * how much xp to award for disarming
			     */
			    tmp->stats.exp = tmp->stats.Cha * tmp->level; 
			    tmp->stats.Cha = 1; /* unhide the trap */ 
			}
            	  }
	   }
   }
   return expsum;
}  

/* pray() - when this skill is called from do_skill(), it allows
 * the player to regain lost grace points at a faster rate. -b.t.
 * This always returns 0 - return value is used by calling function
 * such that if it returns true, player gets exp in that skill.  This
 * the effect here can be done on demand, we probably don't want to
 * give infinite exp by returning true in any cases.
 */

int pray (object *pl) {
    char buf[MAX_BUF];
    object *tmp;

    if(pl->type!=PLAYER) return 0;

    strcpy(buf,"You pray.");
    /* Check all objects - we could stop at floor objects,
     * but if someone buries an altar, I don't see a problem with
     * going through all the objects, and it shouldn't be much slower
     * than extra checks on object attributes.
     */
    for (tmp=pl->below; tmp!=NULL; tmp=tmp->below) {
	/* Only if the altar actually belongs to someone do you get special benefits */
	if(tmp && tmp->type==HOLY_ALTAR && tmp->other_arch) {
	    sprintf(buf,"You pray over the %s.",tmp->name);
	    pray_at_altar(pl,tmp);
	    break;  /* Only pray at one altar */
	}
    }

    new_draw_info(NDI_UNIQUE,0,pl,buf);
     
    if(pl->stats.grace < pl->stats.maxgrace) {
	pl->stats.grace++;
	pl->last_grace = -1;
    } else return 0;

    /* Is this really right?  This will basically increase food
     * consumption, hp & sp regeneration, and everything else that
     * do_some_living does.
     */
    do_some_living(pl);
    return 0;
}

/* This skill allows the player to regain a few sp or hp for a 
 * brief period of concentration. No armour or weapons may be 
 * wielded/applied for this to work. The amount of time needed
 * to concentrate and the # of points regained is dependant on
 * the level of the user. - b.t. thomas@astro.psu.edu 
 */ 

/* July 95 I commented out 'factor' - this should now be handled by 
 * get_skill_time() -b.t. */

/* Sept 95. Now meditation is level dependant (score). User may
 * meditate w/ more armour on as they get higher level 
 * Probably a better way to do this is based on overall encumberance 
 * -b.t.
 */ 

void meditate (object *pl) {
  object *tmp;
  int lvl = pl->level; 
  /* int factor = 10/(1+(pl->level/10)+(pl->stats.Int/15)+(pl->stats.Wis/15)); */ 

    if(pl->type!=PLAYER) return;	/* players only */

    /* check if pl has removed encumbering armour and weapons */ 

    if(QUERY_FLAG(pl,FLAG_READY_WEAPON) && (lvl<6)) { 
        new_draw_info(NDI_UNIQUE,0,pl, 
	  "You can't concentrate while wielding a weapon!\n");	
	return;
    } else {
	for(tmp=pl->inv;tmp;tmp=tmp->below)
          if(( (tmp->type==ARMOUR && lvl<12) 
		|| (tmp->type==HELMET && lvl<10) 
		|| (tmp->type==SHIELD && lvl<6) 
		|| (tmp->type==BOOTS && lvl<4) 
		|| (tmp->type==GLOVES && lvl<2) )
  	     && QUERY_FLAG(tmp,FLAG_APPLIED)) {
         	new_draw_info(NDI_UNIQUE,0,pl, 
		  "You can't concentrate while wearing so much armour!\n");	
	  	return;
	  } 
    } 

    /* ok let's meditate!  Spell points are regained first, then once
     * they are maxed we get back hp. Actual incrementing of values
     * is handled by the do_some_living() (in player.c). This way magical
     * bonuses for healing/sp regeneration are included properly
     * No matter what, we will eat up some playing time trying to 
     * meditate. (see 'factor' variable for what sets the amount of time) 
     */
 
        new_draw_info(NDI_UNIQUE,0,pl, "You meditate."); 
     /*   pl->speed_left -= (int) FABS(factor); */ 

	if(pl->stats.sp < pl->stats.maxsp) {
	   pl->stats.sp++;
	   pl->last_sp = -1;
	} else if (pl->stats.hp < pl->stats.maxhp)  {
	   pl->stats.hp++;
	   pl->last_heal = -1;
	} else return;
	
	do_some_living(pl); 	
}

/* write_on_item() - wrapper for write_note and write_scroll */

int write_on_item (object *pl,char *params) {
    object *item;
    char *string=params;
    int msgtype;
    int i;

    if(pl->type!=PLAYER) return 0;
    if (!params) {
	params="";
	string=params;
    }

    /* Need to be able to read before we can write! */

    if(!find_skill(pl,SK_LITERACY)) {
	new_draw_info(NDI_UNIQUE,0,pl,
	   "You must learn to read before you can write!");
	return 0;
    }

    /* if skill name occurs at begining of the string
     * we have to reset pointer to miss it and trailing space(s)
     */
    /*
     * GROS: Bugfix here. if you type
     * use_skill inscription bla
     * params will contain "bla" only, so looking for the skill name
     * shouldn't be done anymore.
     */
/*  if(lookup_skill_by_name(params)>=0){
	for(i=strcspn(string," ");i>0;i--) string++;
	for(i=strspn(string," ");i>0;i--) string++;
    }
*/
    /* if there is a message then it goes in a book and no message means
     * write active spell into the scroll
     */
    msgtype = (string[0]!='\0') ? BOOK : SCROLL;

    /* find an item of correct type to write on */
    if ( !(item = find_marked_object(pl))){
	new_draw_info(NDI_UNIQUE,0,pl,"You don't have anything marked.");
	return 0;
    }

    if(item) {
	if(QUERY_FLAG(item,FLAG_UNPAID)) {
	    new_draw_info(NDI_UNIQUE,0,pl,
		"You had better pay for that before you write on it.");
	    return 0;
	}
	switch(item->type) {
	    case SCROLL:
		return write_scroll(pl,item);
		break;
	    case BOOK: { 
		return write_note(pl,item,string);
		break;
	    }
	    default:
		break;
	}
    }
    new_draw_info_format(NDI_UNIQUE,0,pl,"You have no %s to write on",
			 msgtype==BOOK ? "book" : "scroll");
    return 0;
}

/* write_note() - this routine allows players to inscribe messages in 
 * ordinary 'books' (anything that is type BOOK). b.t.
 */

int write_note(object *pl, object *item, char *msg) {
  char buf[BOOK_BUF]; 
  object *newBook = NULL;

  /* a pair of sanity checks */
  if(!item||item->type!=BOOK) return 0;

  if(!msg) { 
  	new_draw_info(NDI_UNIQUE,0,pl,"No message to write!");
	new_draw_info_format(NDI_UNIQUE,0,pl,"Usage: use_skill %s <message>",
	  skills[SK_INSCRIPTION].name);
	return 0;
  }
#ifdef PLUGINS
  /* GROS: Handle for plugin book writing (trigger) event */
  if(item->event_hook[EVENT_TRIGGER] != NULL)
  {
    CFParm CFP;
    int k, l, m;
    k = EVENT_TRIGGER;
    l = SCRIPT_FIX_ALL;
    m = 0;
    CFP.Value[0] = &k;
    CFP.Value[1] = pl;
    CFP.Value[2] = item;
    CFP.Value[3] = NULL;
    CFP.Value[4] = msg;
    CFP.Value[5] = &m;
    CFP.Value[6] = &m;
    CFP.Value[7] = &m;
    CFP.Value[8] = &l;
    CFP.Value[9] = item->event_hook[k];
    CFP.Value[10]= item->event_options[k];
    if (findPlugin(item->event_plugin[k])>=0)
    {
        ((PlugList[findPlugin(item->event_plugin[k])].eventfunc) (&CFP));
        return strlen(msg);
    }
   }
#endif
  if(!book_overflow(item->msg,msg,BOOK_BUF)) { /* add msg string to book */
    if(item->msg) {
      strcpy(buf,item->msg);
      free_string(item->msg);
    }
    strcat(buf,msg);
    strcat(buf,"\n"); /* new msg needs a LF */
    if(item->nrof > 1) {
      newBook = get_object();
      copy_object(item, newBook);
      decrease_ob(item);
      esrv_send_item(pl, item);
      newBook->nrof = 1;
      newBook->msg = add_string(buf);
      newBook = insert_ob_in_ob(newBook, pl);
      esrv_send_item(pl, newBook);
    } else {
      item->msg=add_string(buf); 
      esrv_send_item(pl, item);
    }
    new_draw_info_format(NDI_UNIQUE,0,pl, "You write in the %s.",query_short_name(item));
    return strlen(msg);
  } else
    new_draw_info_format(NDI_UNIQUE,0,pl, "Your message won't fit in the %s!",query_short_name(item)); 
  return 0;
}

/* write_scroll() - this routine allows players to inscribe spell scrolls
 * of spells which they know. Backfire effects are possible with the
 * severity of the backlash correlated with the difficulty of the scroll 
 * that is attempted. -b.t. thomas@astro.psu.edu
 */

int write_scroll (object *pl, object *scroll) {
    int success=0,confused=0,chosen_spell=-1,stat1=get_skill_stat1(pl);
    object *newScroll;

    /* this is a sanity check */
    if (scroll->type!=SCROLL) {
	new_draw_info(NDI_UNIQUE,0,pl,"A spell can only be inscribed into a scroll!"); 
	return 0; 
    }

    /* Check if we are ready to attempt inscription */
    chosen_spell=pl->contr->chosen_spell;
    if(chosen_spell<0) {
	new_draw_info(NDI_UNIQUE,0,pl,
		    "You need a spell readied in order to inscribe!"); 
	return 0; 
    }
    if(spells[chosen_spell].scroll_chance==0) { /* Tried to write non-scroll spell */
       	new_draw_info_format(NDI_UNIQUE,0,pl,"The spell %s cannot be inscribed.",
		spells[chosen_spell].name);
	return 0;
    }
    if(spells[chosen_spell].cleric && spells[chosen_spell].sp>pl->stats.grace) {
       	new_draw_info_format(NDI_UNIQUE,0,pl,
	     "You don't have enough grace to write a scroll of %s."
	      ,spells[chosen_spell].name);
	return 0;
    }
    else if(spells[chosen_spell].sp>pl->stats.sp) {
       	new_draw_info_format(NDI_UNIQUE,0,pl,
	     "You don't have enough mana to write a scroll of %s."
	      ,spells[chosen_spell].name);
	return 0;
    }

    /* if there is a spell already on the scroll then player could easily
     * accidently read it while trying to write the new one.  give player
     * a 50% chance to overwrite spell at their own level
     */
    if(scroll->stats.sp &&
       random_roll(0, scroll->level*2, pl, PREFER_LOW)>SK_level(pl)) {
         	new_draw_info_format(NDI_UNIQUE,0,pl,
			"Oops! You accidently read it while trying to write on it.");
		manual_apply(pl,scroll,0);
		change_skill(pl,SK_INSCRIPTION);
		return 0;
    }

    /* ok, we are ready to try inscription */

    if(QUERY_FLAG(pl,FLAG_CONFUSED)) confused = 1;

    /* Lost mana/grace no matter what */
    if (spells[chosen_spell].cleric)
	pl->stats.grace-=spells[chosen_spell].sp;
    else
	pl->stats.sp-=spells[chosen_spell].sp;

    if (random_roll(0, spells[chosen_spell].level*4-1, pl, PREFER_LOW) <
	SK_level(pl)) {
	newScroll = get_object();
	copy_object(scroll, newScroll);
	decrease_ob(scroll);
	newScroll->nrof = 1;

	if(!confused) {
	    newScroll->level= (SK_level(pl)>spells[chosen_spell].level ? 
			SK_level(pl) : spells[chosen_spell].level);
	} else {
		/* a  confused scribe gets a random spell */ 
	    do
			chosen_spell=rndm(0, NROFREALSPELLS-1);
	    while (spells[chosen_spell].scroll_chance==0);

	    newScroll->level=SK_level(pl)>spells[chosen_spell].level ? 
	       spells[chosen_spell].level :
	       (random_roll(1, SK_level(pl), pl, PREFER_HIGH));
	}

	if(newScroll->stats.sp==chosen_spell) 
	    new_draw_info(NDI_UNIQUE,0,pl, "You overwrite the scroll.");
	else { 
	    new_draw_info(NDI_UNIQUE,0,pl,
			"You succeed in writing a new scroll.");
	    newScroll->stats.sp=chosen_spell; 
	}

	/* wait until finished manipulating the scroll before inserting it */
	newScroll=insert_ob_in_ob(newScroll,pl);
	esrv_send_item(pl, newScroll);
	success = calc_skill_exp(pl,newScroll);
	if(!confused) success *= 2;
	return success; 

    } else { /* Inscription has failed */

	if(spells[chosen_spell].level>SK_level(pl) || confused){ /*backfire!*/
       	   new_draw_info(NDI_UNIQUE,0,pl,
		"Ouch! Your attempt to write a new scroll strains your mind!");
	   if(random_roll(0, 1, pl, PREFER_LOW)==1)   
		drain_specific_stat(pl,4); 
	     else { 
	        confuse_player(pl,pl,99);
/*		return (-3*calc_skill_exp(pl,newScroll));*/
		return (-30*spells[chosen_spell].level);
	     }
	} else if(random_roll(0, stat1-1, pl, PREFER_HIGH) < 15) { 
       	   new_draw_info(NDI_UNIQUE,0,pl,
		"Your attempt to write a new scroll rattles your mind!");
	   confuse_player(pl,pl,99);
	} else
       	   new_draw_info(NDI_UNIQUE,0,pl,"You fail to write a new scroll.");
    }
/*    return (-1*calc_skill_exp(pl,newScroll));*/
    return (-10*spells[chosen_spell].level);
}

/* remove_trap() - This skill will disarm any previously discovered trap 
 * the algorithm is based (almost totally) on the old command_disarm() - b.t. 
 */ 

int remove_trap (object *op, int dir) {
  object *tmp,*tmp2;
  int i,x,y,success=0;    

   for(i=0;i<9;i++) {
      x = op->x + freearr_x[i];
      y = op->y + freearr_y[i];
      if(out_of_map(op->map,x,y))
	continue;

  /*  Check everything in the square for trapness */
   for(tmp = get_map_ob(op->map,x,y);tmp!=NULL;tmp=tmp->above) {

      /* And now we'd better do an inventory traversal of each
       * of these objects' inventory */

      for(tmp2=tmp->inv;tmp2!=NULL;tmp2=tmp2->below)
         if(tmp2->type==RUNE&&tmp2->stats.Cha<=1) {
              trap_show(tmp2,tmp);
              if(trap_disarm(op,tmp2,1) && (!tmp2->owner || tmp2->owner->type!=PLAYER))
		   success += calc_skill_exp(op,tmp2);
         }

      if(tmp->type==RUNE&&tmp->stats.Cha<=1) {
         trap_show(tmp,tmp);
         if (trap_disarm(op,tmp,1) && (!tmp->owner || tmp->owner->type!=PLAYER))
		   success += calc_skill_exp(op,tmp);
      }  
    }
  }
 
   return success;
}

int skill_throw (object *op, int dir, char *params) {
  int success = 0;
 
  if(op->type==PLAYER) do_throw(op,find_throw_ob(op,params),dir);
  else do_throw(op,find_mon_throw_ob(op->head?op->head:op),dir);
 
  return success;
}

/* find_throw_ob() - if we request an object, then
 * we search for it in the inventory of the owner (you've
 * got to be carrying something in order to throw it!).
 * If we didnt request an object, then the top object in inventory
 * (that is "throwable", ie no throwing your skills away!)
 * is the object of choice. Also check to see if object is
 * 'throwable' (ie not applied cursed obj, worn, etc).
 */

object *find_throw_ob( object *op, char *request ) {
  object *tmp;
   
  if(!op) { /* safety */
    LOG(llevError,"find_throw_ob(): confused! have a NULL thrower!\n");
    return (object *) NULL;
  }

  /* look through the inventory */
  for(tmp=op->inv;tmp;tmp=tmp->below) {
       /* can't toss invisible or inv-locked items */
      if(tmp->invisible||QUERY_FLAG(tmp,FLAG_INV_LOCKED)) continue;
      if(!request||!strcmp(query_name(tmp),request)
          ||!strcmp(tmp->name,request)) break;
  }
 
  /* this should prevent us from throwing away
   * cursed items, worn armour, etc. Only weapons
   * can be thrown from 'hand'.  */
  if(tmp) {
    if (QUERY_FLAG(tmp,FLAG_APPLIED)) {
      if(tmp->type!=WEAPON) {
        new_draw_info_format(NDI_UNIQUE, 0,op,
          "You can't throw %s.",query_name(tmp));
        tmp = NULL;
      } else if (QUERY_FLAG(tmp,FLAG_CURSED)||QUERY_FLAG(tmp,FLAG_DAMNED)) {
        new_draw_info_format(NDI_UNIQUE, 0,op,
          "The %s sticks to your hand!",query_name(tmp));
        tmp = NULL;
      } else {
        if (apply_special (op, tmp, AP_UNAPPLY | AP_NO_MERGE)) {
          LOG (llevError, "BUG: find_throw_ob(): couldn't unapply\n");
          tmp = NULL;
        }
      }
    }
  }

  if (tmp && QUERY_FLAG (tmp, FLAG_INV_LOCKED)) {
    LOG (llevError, "BUG: find_throw_ob(): object is locked\n");
    tmp=NULL;
  }

  return tmp;
}

/* make_throw_ob() We construct the 'carrier' object in
 * which we will insert the object that is being thrown.
 * This combination  becomes the 'thrown object'. -b.t.
 */

object *make_throw_ob (object *orig) {
  object *toss_item=NULL;

  if(orig) {
    toss_item=get_object();
    if (QUERY_FLAG (orig, FLAG_APPLIED)) {
      LOG (llevError, "BUG: make_throw_ob(): ob is applied\n");
      /* insufficient workaround, but better than nothing */
      CLEAR_FLAG (orig, FLAG_APPLIED);
    }
    copy_object(orig,toss_item);
    toss_item->type = THROWN_OBJ;
    CLEAR_FLAG(toss_item,FLAG_CHANGING);
    toss_item->stats.dam = 0; /* default damage */
#ifdef DEBUG_THROW
    LOG(llevDebug," inserting %s(%d) in toss_item(%d)\n",
        orig->name,orig->count,toss_item->count);
#endif 
    insert_ob_in_ob(orig,toss_item);
  }

  return toss_item;
}


/* do_throw() - op throws any object toss_item. This code
 * was borrowed from fire_bow (see above), so probably these
 * two functions should be merged together since they are
 * almost the same. I left them apart for now for debugging
 * purposes, and also, so as to not screw up fire_bow()!
 * This function is useable by monsters.  -b.t.
 */

void do_throw(object *op, object *toss_item, int dir) {
    object *throw_ob=toss_item, *left=NULL;
    tag_t left_tag;
    int eff_str = 0,maxc,str=op->stats.Str,dam=0;
    int pause_f,weight_f=0;
    float str_factor=1.0,load_factor=1.0,item_factor=1.0;

    if(throw_ob==NULL) {
	if(op->type==PLAYER) {
	    new_draw_info(NDI_UNIQUE, 0,op,"You have nothing to throw.");
	    op->contr->count_left=0; /* ?? is this needed?? */
	}
	return;
    }
    if (QUERY_FLAG(throw_ob, FLAG_STARTEQUIP)) {
	if (op->type==PLAYER) {
	    new_draw_info(NDI_UNIQUE, 0, op, "The gods won't let you throw that.");
	    op->contr->count_left=0; /* ?? is this needed?? */
	}
	return;
    }
	

    /* Because throwing effectiveness must be reduced by the
     * encumbrance of the thrower and weight of the object. THus,
     * we use the concept of 'effective strength' as defined below. 
     */
 
    /* if str exceeds MAX_STAT (30, eg giants), lets assign a str_factor > 1 */
    if(str>MAX_STAT) { 
	str_factor = (float) str /(float) MAX_STAT; str = MAX_STAT; 
    }
   
    /* the more we carry, the less we can throw. Limit only on players */
    maxc=max_carry[str]*1000;
    if(op->carrying>maxc&&op->type==PLAYER)
	load_factor = (float)maxc/(float) op->carrying;
 
    /* lighter items are thrown harder, farther, faster */
    if(throw_ob->weight>0) 
	item_factor = (float) maxc/(float) (3.0 * throw_ob->weight);
    else { /* 0 or negative weight?!? Odd object, can't throw it */
	new_draw_info_format(NDI_UNIQUE, 0,op,"You can't throw %s.\n",
			     query_name(throw_ob)); 
	return;
    }
   
    eff_str = str * (load_factor<1.0?load_factor:1.0);
    eff_str = (float) eff_str * item_factor * str_factor;
   
    /* alas, arrays limit us to a value of MAX_STAT (30). Use str_factor to
     * account for super-strong throwers. */
    if(eff_str>MAX_STAT) eff_str=MAX_STAT;

#ifdef DEBUG_THROW
    LOG(llevDebug,"%s carries %d, eff_str=%d\n",op->name,op->carrying,eff_str);
    LOG(llevDebug," max_c=%d, item_f=%f, load_f=%f, str=%d\n",maxc,
        item_factor,load_factor,op->stats.Str);
    LOG(llevDebug," str_factor=%f\n",str_factor);
    LOG(llevDebug," item %s weight= %d\n",throw_ob->name,throw_ob->weight);
#endif

    /* 3 things here prevent a throw, you aimed at your feet, you
     * have no effective throwing strength, or you threw at a wall
     */ 
    if(!dir || (eff_str <= 1) ||
       wall(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir])) {

	/* bounces off 'wall', and drops to feet */
	 remove_ob(throw_ob);
	 throw_ob->x = op->x; throw_ob->y = op->y;
	 insert_ob_in_map(throw_ob,op->map,op,0);
	if(op->type==PLAYER) {
	    if(eff_str<=1) {
		new_draw_info_format(NDI_UNIQUE, 0,op,
			"Your load is so heavy you drop %s to the ground.",
			query_name(throw_ob));
	    }
	    else if(!dir) {
		new_draw_info_format(NDI_UNIQUE, 0,op,"You throw %s at the ground.",
				     query_name(throw_ob));
	    }
	    else
		new_draw_info(NDI_UNIQUE, 0,op,"Something is in the way.");
	    op->contr->count_left=0; /* needed?? */
	}
	return;
    } /* if object can't be thrown */
 
    left = throw_ob; /* these are throwing objects left to the player */
    left_tag = left->count;

  /* sometimes get_split_ob can't split an object (because op->nrof==0?)
   * and returns NULL. We must use 'left' then 
   */

    if((throw_ob = get_split_ob(throw_ob, 1))==NULL) {
#ifdef DEBUG_THROW
	LOG(llevDebug," get_splt_ob faild to split throw ob %s\n",left->name);
#endif
	throw_ob = left;
	remove_ob(left);
	if (op->type==PLAYER)
	    esrv_del_item(op->contr, left->count);
    }
    else if (op->type==PLAYER) {
	if (was_destroyed (left, left_tag))
	    esrv_del_item(op->contr, left_tag);
	else 
	    esrv_update_item(UPD_NROF, op, left);
    }

    /* special case: throwing powdery substances like dust, dirt */
    if(QUERY_FLAG(throw_ob,FLAG_DUST)) { 
	cast_dust(op,throw_ob,dir); 
	return;
    }

    /* Make a thrown object -- insert real object in a 'carrier' object.
     * If unsuccessfull at making the "thrown_obj", we just reinsert
     * the original object back into inventory and exit
     */
    if((toss_item = make_throw_ob(throw_ob)))
	throw_ob = toss_item;
    else {
	insert_ob_in_ob(throw_ob,op);
	return;
    }

    set_owner(throw_ob,op);
    /* At some point in the attack code, the actual real object (op->inv)
     * becomes the hitter.  As such, we need to make sure that has a proper
     * owner value so exp goes to the right place.
     */
    /*    set_owner(throw_ob,op->inv);
	  Set thrown object's owner to first object in player's inventory???
	  Don't you mean to set player as owner of object in throw_ob's inv?  */
    set_owner(throw_ob->inv,op);
    throw_ob->direction=dir;
    throw_ob->x = op->x;
    throw_ob->y = op->y;

    /* the damage bonus from the force of the throw */
    dam = str_factor * dam_bonus[eff_str];

    /* Now, lets adjust the properties of the thrown_ob. */

    /* how far to fly */
    throw_ob->last_sp = (eff_str*3)/5;
     
    /* speed */
    throw_ob->speed = (speed_bonus[eff_str]+1.0)/1.5;
    throw_ob->speed = MIN(1.0,throw_ob->speed); /* no faster than an arrow! */
     
    /* item damage. Eff_str and item weight influence damage done */
    weight_f = (throw_ob->weight/2000)>MAX_STAT?MAX_STAT:(throw_ob->weight/2000);
    throw_ob->stats.dam += (dam/3) + dam_bonus[weight_f]
        + (throw_ob->weight/15000) - 2;

    /* chance of breaking. Proportional to force used and weight of item */
    throw_ob->stats.food = (dam/2) + (throw_ob->weight/60000);
     
    /* replace 25 with a call to clone.arch wc? messes up w/ NPC */
    throw_ob->stats.wc = 25 - dex_bonus[op->stats.Dex?dex_bonus[op->stats.Dex]:0]
	- thaco_bonus[eff_str] - SK_level(op);
 

    /* the properties of objects which are meant to be thrown (ie dart,
     * throwing knife, etc) will differ from ordinary items. Lets tailor
     * this stuff in here. 
     */

    if(QUERY_FLAG(throw_ob->inv,FLAG_IS_THROWN)) {
	throw_ob->last_sp += eff_str/3; /* fly a little further */
	throw_ob->stats.dam += throw_ob->inv->stats.dam + throw_ob->magic + 2;
	throw_ob->stats.wc -= throw_ob->magic + throw_ob->inv->stats.wc;
	/* only throw objects get directional faces */
	if(GET_ANIM_ID(throw_ob) && NUM_ANIMATIONS(throw_ob))
	    SET_ANIMATION(throw_ob, dir);
    } else {
	/* some materials will adjust properties.. */
	if(throw_ob->material&M_LEATHER) {
	    throw_ob->stats.dam -= 1;
	    throw_ob->stats.food -= 10;
	}
	if(throw_ob->material&M_GLASS) throw_ob->stats.food += 60;
 
	if(throw_ob->material&M_ORGANIC) {
	    throw_ob->stats.dam -= 3;
	    throw_ob->stats.food += 55;
	}
	if(throw_ob->material&M_PAPER||throw_ob->material&M_CLOTH) {
	    throw_ob->stats.dam -= 5; throw_ob->speed *= 0.8;
	    throw_ob->stats.wc += 3; throw_ob->stats.food -= 30;
	}
	/* light obj have more wind resistance, fly slower*/
	if(throw_ob->weight>500) throw_ob->speed *= 0.8;
	if(throw_ob->weight>50) throw_ob->speed *= 0.5;
 
    } /* else tailor thrown object */
 
    /* some limits, and safeties (needed?) */
    if(throw_ob->stats.dam<0) throw_ob->stats.dam=0;
    if(throw_ob->last_sp>eff_str) throw_ob->last_sp=eff_str;
    if(throw_ob->stats.food<0) throw_ob->stats.food=0;
    if(throw_ob->stats.food>100) throw_ob->stats.food=100;
    if(throw_ob->stats.wc>30) throw_ob->stats.wc=30;

    /* We shouldn't need a call to fix player just for throwing an
     * object.
     */
/*    fix_player(op);*/

    /* how long to pause the thrower. Higher values mean less pause */
    pause_f = ((2*eff_str)/3)+20+SK_level(op);

    /* Put a lower limit on this */
    if (pause_f < 10) pause_f=10;
    if (pause_f > 100) pause_f=100;

    /* Changed in 0.94.2 - the calculation before was really goofy.
     * In short summary, a throw can take anywhere between speed 5 and
     * speed 0.5
     */

    op->speed_left -=  50 / pause_f;

    update_ob_speed(throw_ob);
    throw_ob->speed_left = 0;
    throw_ob->map = op->map;

    SET_FLAG(throw_ob, FLAG_FLYING);
    SET_FLAG(throw_ob, FLAG_FLY_ON);
    SET_FLAG(throw_ob, FLAG_WALK_ON);
#if 0
    /* need to put in a good sound for this */
    play_sound_map(op->map, op->x, op->y, SOUND_THROW_OBJ);
#endif
#ifdef PLUGINS
/* GROS - Now we can call the associated script_throw event (if any) */
    if(throw_ob->event_hook[EVENT_THROW] != NULL)
    {
        CFParm CFP;
        int k, l, m;
        k = EVENT_THROW;
        l = SCRIPT_FIX_ACTIVATOR;
        m = 0;
        CFP.Value[0] = &k;
        CFP.Value[1] = op;
        CFP.Value[2] = throw_ob;
        CFP.Value[3] = NULL;
        CFP.Value[4] = NULL;
        CFP.Value[5] = &m;
        CFP.Value[6] = &m;
        CFP.Value[7] = &m;
        CFP.Value[8] = &l;
        CFP.Value[9] = throw_ob->event_hook[k];
        CFP.Value[10]= throw_ob->event_options[k];
        if (findPlugin(throw_ob->event_plugin[k])>=0)
            ((PlugList[findPlugin(throw_ob->event_plugin[k])].eventfunc) (&CFP));
    }
#endif
#ifdef DEBUG_THROW
    LOG(llevDebug," pause_f=%d \n",pause_f);
    LOG(llevDebug," %s stats: wc=%d dam=%d dist=%d spd=%f break=%d\n",
        throw_ob->name,throw_ob->stats.wc,throw_ob->stats.dam,
        throw_ob->last_sp,throw_ob->speed,throw_ob->stats.food);
    LOG(llevDebug,"inserting tossitem (%d) into map\n",throw_ob->count);
#endif
    insert_ob_in_map(throw_ob,op->map,op,0);
    move_arrow(throw_ob);
}

