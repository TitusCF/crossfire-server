/*
 * static char *rcsid_apply_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
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

    The author can be reached via e-mail to mark@pyramid.com
*/

#include <global.h>
#include <living.h>
#include <spells.h>
#include <skills.h>
#if defined(SunArchitecture) && !(OSMajorVersion == 5)
#include <sys/types.h>
#endif

#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/* Want this regardless of rplay. */
#include <sounds.h>


#if defined(vax) || defined(ibm032)
size_t strftime(char *, size_t, const char *, const struct tm *);
time_t mktime(struct tm *);
#endif

void draw_find(object *op, object *find) {
  new_draw_info_format(NDI_UNIQUE, 0, op, "You find %s in the chest.",
	query_name(find));
}

int apply_id_altar(object *money, object *altar)
{
    object *id, *pl, *marked;
    int success=0;

    /* Check for MONEY type is a special hack - it prevents 'nothing needs
     * identifying' from being printed out more than it needs to be.
     */
    if (!check_altar_sacrifice(altar) || money->type!=MONEY) return 0;

    pl=get_map_ob(altar->map, altar->x, altar->y);
    while (pl!=NULL && pl->type!=PLAYER)
	pl=pl->above;
    if (pl==NULL) {
	LOG(llevError,"Identify Altar: Can't find player!.\n");
	return 0;
    }

    marked=find_marked_object(pl);
    /* if the player has a marked item, identify that if it needs to be
     * identified.  IF it doesn't, then go through the player inventory.
     */
    if (marked && !QUERY_FLAG(marked, FLAG_IDENTIFIED) && need_identify(marked)) {
	if (check_altar(altar)) {
	    identify(marked);
	    new_draw_info_format(NDI_UNIQUE, 0, pl,
		"You have %s.", long_desc(marked));
            if (marked->msg) {
	        new_draw_info(NDI_UNIQUE, 0,pl, "The item has a story:");
	        new_draw_info(NDI_UNIQUE, 0,pl, marked->msg);
	    }
	    return 1;
	} 
    }

    for (id=pl->inv; id; id=id->below) {
	if (!QUERY_FLAG(id, FLAG_IDENTIFIED) && !id->invisible && 
	    need_identify(id)) {
		if (check_altar(altar)) {
		    identify(id);
		    new_draw_info_format(NDI_UNIQUE, 0, pl,
			"You have %s.", long_desc(id));
	            if (id->msg) {
		        new_draw_info(NDI_UNIQUE, 0,pl, "The item has a story:");
		        new_draw_info(NDI_UNIQUE, 0,pl, id->msg);
		    }
		    success=1;
		    /* If no more money, might as well quit now */
		    if (!check_altar_sacrifice(altar)) break;
		}
		else {
		    LOG(llevError,"check_id_altar:  Couldn't do sacrifice when we should have been able to\n");
		    break;
		}
	}
    }
    if (!success) new_draw_info(NDI_UNIQUE, 0,pl,"You have nothing that needs identifying");
    return 1;
}

int apply_potion(object *op, object *tmp)
{
    int got_one=0;

#if 0
   /* we now need this to happen */
    if(op->type!=PLAYER)
      return 0; /* This might change */
#endif

    if(op->type==PLAYER) { 
      if(QUERY_FLAG(tmp, FLAG_UNPAID)) {
        new_draw_info(NDI_UNIQUE,0,op,"You should pay for it first.");
        return 0;
      }
      if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED))
        identify(tmp);
    }

    /* only players get this */
    if (op->type==PLAYER&&(tmp->attacktype & AT_DEPLETE)) { /* Potion of restoration */
      object *depl;
      archetype *at;

      if (QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) {
	drain_stat(op);
        fix_player(op);
        decrease_ob(tmp);
        return 1;
      }
      if ((at = find_archetype("depletion"))==NULL) {
	LOG(llevError,"Could not find archetype depletion");
	return 0;
      }
      depl = present_arch_in_ob(at, op);
      if (depl!=NULL) {
	int i;
        for (i = 0; i < 7; i++)
          if (get_attr_value(&depl->stats, i)) {
            new_draw_info(NDI_UNIQUE,0,op, restore_msg[i]);
          }
	remove_ob(depl);
	free_object(depl);
        fix_player(op);
      }
      else
        new_draw_info(NDI_UNIQUE,0,op, "You feel a great loss...");

      decrease_ob(tmp);
      return 1;
    }
    /* only players get this */
    if(op->type==PLAYER&&tmp->attacktype&AT_GODPOWER) {    /* improvement potion */
	int i;

	for(i=1;i<MIN(11,op->level);i++) {
	    if (QUERY_FLAG(tmp,FLAG_CURSED) || QUERY_FLAG(tmp,FLAG_DAMNED)) {
		if (op->contr->levhp[i]!=1) {
		    op->contr->levhp[i]=1;
		    break;
		}
		if (op->contr->levsp[i]!=1) {
		    op->contr->levsp[i]=1;
		    break;
		}
		if (op->contr->levgrace[i]!=1) {
		    op->contr->levgrace[i]=1;
		    break;
		}
	    }
	    else {
			 if(op->contr->levhp[i]<9) { 
				 op->contr->levhp[i]=9;
				 break;
			 }
			 if(op->contr->levsp[i]<6) { 
				 op->contr->levsp[i]=6;
				 break;
			 }
			 if(op->contr->levgrace[i]<3) {
				 op->contr->levgrace[i]=3;
				 break;
			 }
		 }
	 }
	/* Just makes checking easier */
	if (i<MIN(11, op->level)) got_one=1;
	if (!QUERY_FLAG(tmp,FLAG_CURSED) && !QUERY_FLAG(tmp,FLAG_DAMNED)) {
	    if (got_one) {
		fix_player(op);
		new_draw_info(NDI_UNIQUE,0,op,"The Gods smile upon you and remake you");
		new_draw_info(NDI_UNIQUE,0,op,"a little more in their image.");
        	new_draw_info(NDI_UNIQUE,0,op,"You feel a little more perfect.");
	    }
	    else
		new_draw_info(NDI_UNIQUE,0,op,"The potion had no effect - you are already perfect");
	}
	else {	/* cursed potion */
	    if (got_one) {
		fix_player(op);
		new_draw_info(NDI_UNIQUE,0,op,"The Gods are angry and punish you.");
	    }
	    else 
		new_draw_info(NDI_UNIQUE,0,op,"You are fortunate that you are so pathetic.");
	}
	decrease_ob(tmp);
	return 1;
    }

    /* protection/immunity granting potions */
    if(tmp->immune||tmp->protected) {
      object *force;

      force=get_archetype("force");
      if(QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) {
        force->vulnerable = tmp->immune | tmp->protected;
/* Why set force->type to 0?  This makes the result permanent */
/*        force->type = 0; */
        force->stats.food*=10;
      } else {
        force->immune=tmp->immune;
        force->protected=tmp->protected;
      }
      force->speed_left= -1;
      force = insert_ob_in_ob(force,op);
      SET_FLAG(force,FLAG_APPLIED);
      change_abil(op,force);
      decrease_ob(tmp);
      return 1;
    }

    /* A potion that casts a spell.  Healing, restore spellpoint (power potion)
     * and heroism all fit into this category.
     */
    if (tmp->stats.sp) {
      if(QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) {
        new_draw_info(NDI_UNIQUE,0,op, "Yech!  Your lungs are on fire!");
        cast_spell(op,tmp, 0, 3, 1, spellPotion,NULL);
      } else
        cast_spell(op,tmp, op->facing, tmp->stats.sp, 1, spellPotion,NULL);
      decrease_ob(tmp);
      /* if youre dead, no point in doing this... */
      if(!QUERY_FLAG(op,FLAG_REMOVED)) fix_player(op);
      return 1;
    }

    /* Only thing left are the stat potions */
    if(op->type==PLAYER) { /* only for players */
      if((QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) 
	  && tmp->value!=0)
	CLEAR_FLAG(tmp, FLAG_APPLIED);
      else
	SET_FLAG(tmp, FLAG_APPLIED);
      if(!change_abil(op,tmp))
        new_draw_info(NDI_UNIQUE,0,op,"Nothing happened.");
     }

    /* CLEAR_FLAG is so that if the character has other potions
     * that were grouped with the one consumed, his
     * stat will not be raised by them.  fix_player just clears
     * up all the stats.
     */
    CLEAR_FLAG(tmp, FLAG_APPLIED);
    fix_player(op);
    decrease_ob(tmp);
    return 1;
}

/****************************************************************************
 * Weapon improvement code follows
 ****************************************************************************/

int check_item(object *op,char *item)
{
  int count=0;


  if (item==NULL) return 0;
  op=op->below;
  while(op!=NULL) {
    if (strcmp(op->arch->name,item)==0){
	  if (!QUERY_FLAG (op, FLAG_CURSED) && !QUERY_FLAG (op, FLAG_DAMNED) 
             /* Loophole bug? -FD- */ && !QUERY_FLAG (op, FLAG_UNPAID) )
	    {
	      if (op->nrof == 0)/* this is necessary for artifact sacrifices --FD-- */
		count++;
	      else
		count += op->nrof;
	    }
    }
    op=op->below;
  }
  return count;
}

void eat_item(object *op,char *item)
{
  object *prev;

  prev = op;
  op=op->below;
  
  while(op!=NULL) {
    if (strcmp(op->arch->name,item)==0) {
      decrease_ob_nr(op,op->nrof);
      op=prev;
    }
    prev = op;
    op=op->below;
  }
}

/* This checks to see of the player (who) is sufficient level to use a weapon
 * with improvs improvements (typically last_eat).  We take an int here
 * instead of the object so that the improvement code can pass along the
 * increased value to see if the object is usuable.
 * we return 1 (true) if the player can use the weapon.
 */
int check_weapon_power(object *who, int improvs)
{
    int level=who->level;

    /* The skill system hands out wc and dam bonuses to fighters
     * more generously than the old system (see fix_player). Thus
     * we need to curtail the power of player enchanted weapons. 
     * I changed this to 1 improvement per "fighter" level/5 -b.t. 
     * Note:  Nothing should break by allowing this ratio to be different or
     * using normal level - it is just a matter of play balance.
     */
#ifdef ALLOW_SKILLS
    if(who->type==PLAYER) { 
      object *wc_obj=NULL;

      for(wc_obj=who->inv;wc_obj;wc_obj=wc_obj->below)
	if(wc_obj->type==EXPERIENCE&&wc_obj->stats.Str) break;
      if(!wc_obj) 
	LOG(llevError,"Error: Player: %s lacks wc experience object\n",who->name);
      else
	level=wc_obj->level; 
    }
    return (improvs <= ((level/5)+5));
#else
    return (improvs <= (level+5));
#endif
}

/* Returns the object count that of the number of objects found that
 * improver wants.
 */
static int check_sacrifice(object *op,object *improver)
{
    int count=0;

    if (improver->slaying!=NULL) {
	count = check_item(op,improver->slaying);
	if (count<1) {
	    char buf[200];
	    sprintf(buf,"The gods want more %ss",improver->slaying);
	    new_draw_info(NDI_UNIQUE,0,op,buf);
	    return 0;
	}
    }
    else
	count=1;

    return count;
}

int improve_weapon_stat(object *op,object *improver,object *weapon,
			signed char *stat,int sacrifice_count,char *statname)
{

  new_draw_info(NDI_UNIQUE,0,op,"Your sacrifice was accepted.");
  *stat += sacrifice_count;
  weapon->last_eat++;
  new_draw_info_format(NDI_UNIQUE,0,op,
	"Weapon's bonus to %s improved by %d",statname,sacrifice_count);
  decrease_ob(improver);

  /* So it updates the players stats and the window */
  fix_player(op);
  return 1;
}

/* Types of improvements, hidden in the sp field. */
#define IMPROVE_PREPARE 1
#define IMPROVE_DAMAGE 2
#define IMPROVE_WEIGHT 3
#define IMPROVE_ENCHANT 4
#define IMPROVE_STR 5
#define IMPROVE_DEX 6
#define IMPROVE_CON 7
#define IMPROVE_WIS 8
#define IMPROVE_CHA 9
#define IMPROVE_INT 10
#define IMPROVE_POW 11


/* This does the prepare weapon scroll */

int prepare_weapon(object *op, object *improver, object *weapon)
{
    int sacrifice_count;
    char buf[MAX_BUF];

    if (weapon->level!=0) {
      new_draw_info(NDI_UNIQUE,0,op,"Weapon already prepared.");
      return 0;
    }
    /* If someone wants to do a weapon that makes them vulnerable,
     * let them.
     */
    if (weapon->immune || weapon->protected ||
	weapon->stats.hp ||	/* regeneration */
	weapon->stats.sp ||	/* sp regeneration */
	weapon->stats.exp ||	/* speed */
	weapon->stats.ac)	/* AC - only taifu's I think */
    {
      new_draw_info(NDI_UNIQUE,0,op,"Cannot prepare magic weapons.");
      return 0;
    }
    sacrifice_count=check_sacrifice(op,improver);
    if (sacrifice_count<=0)
      return 0;
    sacrifice_count = isqrt(sacrifice_count);
    weapon->level=sacrifice_count;
    new_draw_info(NDI_UNIQUE,0,op,"Your sacrifice was accepted.");
    eat_item(op, improver->slaying);

    new_draw_info_format(NDI_UNIQUE, 0, op,"Your *%s may be improved %d times.",
	    weapon->name,sacrifice_count);

    sprintf(buf,"%s's %s",op->name,weapon->name);
    weapon->name=add_string(buf);
    weapon->nrof=0;  /*  prevents preparing n weapons in the same
			 slot at once! */
    decrease_ob(improver);
    weapon->last_eat=0;
    return 1;
}


/* This is the new improve weapon code */
/* build_weapon returns 0 if it was not able to work. */
/* #### We are hiding extra information about the weapon in the level and
   last_eat numbers for an object.  Hopefully this won't break anything ?? 
   level == max improve last_eat == current improve*/
int improve_weapon(object *op,object *improver,object *weapon)
{
  int sacrifice_count, sacrifice_needed=0;

  if(improver->stats.sp==IMPROVE_PREPARE) {
	return prepare_weapon(op, improver, weapon);
  }
  if (weapon->level==0) {
    new_draw_info(NDI_UNIQUE, 0,op,"This weapon has not been prepared.");
    return 0;
  }
  if (weapon->level==weapon->last_eat) {
    new_draw_info(NDI_UNIQUE, 0,op,"This weapon cannot be improved any more.");
    return 0;
  }
  if (QUERY_FLAG(weapon, FLAG_APPLIED) && 
      !check_weapon_power(op, weapon->last_eat+1)) {
	new_draw_info(NDI_UNIQUE, 0,op,"Improving the weapon will make it too");
	new_draw_info(NDI_UNIQUE, 0,op,"powerful for you to use.  Unready it if you");
	new_draw_info(NDI_UNIQUE, 0,op,"really want to improve it.");
	return 0;
  }
  /* This just increases damage by 5 points, no matter what.  No sacrifice
   * is needed.  Since stats.dam is now a 16 bit value and not 8 bit,
   * don't put any maximum value on damage - the limit is how much the
   * weapon  can be improved.
   */
  if (improver->stats.sp==IMPROVE_DAMAGE) {
	weapon->stats.dam += 5;
	weapon->weight += 5000;		/* 5 KG's */
	new_draw_info_format(NDI_UNIQUE, 0, op,
	    "Damage has been increased by 5 to %d", weapon->stats.dam);
	weapon->last_eat++;
	decrease_ob(improver);
	return 1;
  }
  if (improver->stats.sp == IMPROVE_WEIGHT) {
	/* Reduce weight by 20% */
	weapon->weight = (weapon->weight * 8)/10;
	if (weapon->weight < 1) weapon->weight = 1;
	new_draw_info_format(NDI_UNIQUE, 0, op,
		"Weapon weight reduced to %6.1f kg",
		(float)weapon->weight/1000.0);
	weapon->last_eat++;
	decrease_ob(improver);
	return 1;
  }
  if (improver->stats.sp == IMPROVE_ENCHANT) {
	weapon->magic++;
	weapon->last_eat++;
	new_draw_info_format(NDI_UNIQUE, 0, op
		,"Weapon magic increased to %d",weapon->magic);
	decrease_ob(improver);
	return 1;
  }

  sacrifice_needed = weapon->stats.Str + weapon->stats.Int + weapon->stats.Dex+
	weapon->stats.Pow + weapon->stats.Con + weapon->stats.Cha +
	weapon->stats.Wis;

  if (sacrifice_needed<1)
	sacrifice_needed =1;
  sacrifice_needed *=2;

  sacrifice_count = check_sacrifice(op,improver);
  if (sacrifice_count < sacrifice_needed) {
	new_draw_info_format(NDI_UNIQUE, 0, op,
	    "You need at least %d %s", sacrifice_needed, improver->slaying);
	return 0;
  }
  eat_item(op,improver->slaying);

  switch (improver->stats.sp) {
   case IMPROVE_STR:
    return improve_weapon_stat(op,improver,weapon,
                               (signed char *) &(weapon->stats.Str),
			       1,(char *) "strength");
   case IMPROVE_DEX:
    return improve_weapon_stat(op,improver,weapon,
                               (signed char *) &(weapon->stats.Dex),
			       1,(char *) "dexterity");
   case IMPROVE_CON:
    return improve_weapon_stat(op,improver,weapon,
                               (signed char *) &(weapon->stats.Con),
			       1,(char *) "constitution");
   case IMPROVE_WIS:
    return improve_weapon_stat(op,improver,weapon,
                               (signed char *) &(weapon->stats.Wis),
			       1,(char *) "wisdom");
   case IMPROVE_CHA:
    return improve_weapon_stat(op,improver,weapon,
                               (signed char *) &(weapon->stats.Cha),
			       1,(char *) "charisma");
   case IMPROVE_INT:
    return improve_weapon_stat(op,improver,weapon,
                               (signed char *) &(weapon->stats.Int),
			       1,(char *) "intelligence");
   case IMPROVE_POW:
    return improve_weapon_stat(op,improver,weapon,
                               (signed char *) &(weapon->stats.Pow),
                               1,(char *) "power");
   default:
    new_draw_info(NDI_UNIQUE, 0,op,"Unknown improvement type.");
  }
  LOG(llevError,"improve_weapon: Got to end of function\n");
  return 0;
}

int check_improve_weapon (object *op, object *tmp)
{
    object *otmp;

    if(op->type!=PLAYER)
      return 0;
    if(QUERY_FLAG(tmp, FLAG_UNPAID)) {
	new_draw_info(NDI_UNIQUE, 0,op,"You should pay for it first.");
	return 0;
    }
    if (blocks_magic(op->map,op->x,op->y)) {
	new_draw_info(NDI_UNIQUE, 0,op,"Something blocks the magic of the scroll.");
	return 0;
    }
    otmp=find_marked_object(op);
    if(!otmp) {
      new_draw_info(NDI_UNIQUE, 0, op, "You need to mark a weapon object.");
      return 0;
    }
    if (otmp->type!=WEAPON) {
      new_draw_info(NDI_UNIQUE, 0,op,"Marked item is not a weapon");
      return 0;
    }
    new_draw_info(NDI_UNIQUE, 0,op,"Applied weapon builder.");
    improve_weapon(op,tmp,otmp);
    esrv_send_item(op, otmp);
    return 1;
}

/* this code is by b.t. (thomas@nomad.astro.psu.edu) -
 * only 'enchantment' of armour is possible - improving
 * the stats of a player w/ armour as well as a weapon
 * will probably horribly unbalance the game. Magic enchanting
 * depends on the level of the character - ie the plus
 * value (magic) of the armour can never be increased beyond
 * the level of the character / 10 -- rounding upish, nor may
 * the armour value of the piece of equipment exceed either 
 * the users level or 99)
 */
 
int improve_armour(object *op, object *improver, object *armour)
{
    int addarm;
 
    addarm = armour->armour/25 + op->level/20 + 1;

    if (armour->magic>=(op->level/10+1) || ((armour->armour + 
		addarm) >= op->level )) {
        new_draw_info(NDI_UNIQUE, 0,op,"You are not yet powerfull enough");
        new_draw_info(NDI_UNIQUE, 0,op,"to improve this armour.");
        return 0;
    }

    if( (armour->armour + addarm) <= 99)  {
        armour->magic++;
	armour->armour+=addarm;
	armour->weight+=armour->weight*0.05;
	if (op->type == PLAYER) {
	  esrv_send_item(op, armour);
	  if(QUERY_FLAG(armour, FLAG_APPLIED))
	    fix_player(op);
	}
        decrease_ob(improver);
        return 1;
    } else {
        armour->magic++;
        new_draw_info(NDI_UNIQUE, 0,op,"The armour value of this equipment");
        new_draw_info(NDI_UNIQUE, 0,op,"cannot be further improved.");
        decrease_ob(improver);
	return 1;
    } 

 }


/* archetypes don't contain any MONEY_CHANGER object,
 * so it this function really used/useful?
 * Converters seems to make almost same.  -Tero
 * op is the player, towhat is what we turn into.
 */
void money_change(object *op,char *towhat)
{
  object *buying;
  char buf[MAX_BUF];

  buying = get_archetype(towhat);

  if (buying==NULL) {
    LOG(llevError,"Unable to find archetype %s\n",towhat);
    return;
  }
  buying->nrof=100;
  strncpy(buf,query_cost_string(buying,op,F_BUY),MAX_BUF);
  if (pay_for_item(buying,op)) {
    new_draw_info_format(NDI_UNIQUE, 0, op, 
	"You paid %s for %s.",buf,query_name(buying));
    (void) insert_ob_in_ob(buying,op);
  } else {
    new_draw_info_format(NDI_UNIQUE, 0, op, 
	"You can't afford %s.",query_name(buying));
    free_object(buying);
  }
}

/*
 * convert_item() returns 1 if anything was converted, otherwise 0
 */
#define CONV_FROM(xyz)	xyz->slaying
#define CONV_TO(xyz)	xyz->other_arch
#define CONV_NR(xyz)	(unsigned char) xyz->stats.sp
#define CONV_NEED(xyz)	(unsigned long) xyz->stats.food

int convert_item(object *item, object *converter) {
  int nr=0;
  object *tmp;

  /* We make some assumptions - we assume if it takes money as it type,
   * it wants some amount.  We don't make change (ie, if something costs
   * 3 gp and player drops a platinum, tough luck)
   */
  if (!strcmp(CONV_FROM(converter),"money")) {
    int cost;
    nr=(item->nrof*item->value)/CONV_NEED(converter);
    if (!nr) return 0;
    cost=nr*CONV_NEED(converter)/item->value;
    /* take into account rounding errors */
    if (nr*CONV_NEED(converter)%item->value) cost++;
    decrease_ob_nr(item, cost);
  }
  else {
    if(item->type==PLAYER||CONV_FROM(converter)!=item->arch->name||
      (CONV_NEED(converter)&&CONV_NEED(converter)>item->nrof))
      return 0;
    if(CONV_NEED(converter)) {
      nr=item->nrof/CONV_NEED(converter);
      decrease_ob_nr(item,nr*CONV_NEED(converter));
    } else {
      remove_ob(item);
      free_object(item);
    }
  }
  item=arch_to_object(converter->other_arch);
  if(CONV_NR(converter))
    item->nrof=CONV_NR(converter);
  if(nr)
    item->nrof*=nr;
  for(tmp=get_map_ob(converter->map,converter->x,converter->y);
      tmp!=NULL;
      tmp=tmp->above) {
    if(tmp->type==SHOP_FLOOR)
      break;
  }
  if(tmp!=NULL)
    SET_FLAG(item,FLAG_UNPAID);
  item->x=converter->x;
  item->y=converter->y;
  insert_ob_in_map(item,converter->map);
  return 1;
}
  
/*
 * Eneq(@csd.uu.se): Handle apply on containers. 
 * Moved to own function and added many features [Tero.Haatanen@lut.fi]
 */
/* added the alchemical cauldron to the code -b.t. */

int apply_container (object *op, object *sack)
{
    char buf[MAX_BUF];
    object *tmp;

    if(op->type!=PLAYER)
	return 0; /* This might change */

    if (sack==NULL || sack->type != CONTAINER) {
	LOG (llevError, "apply_container: %s is not container!\n",sack?sack->name:"NULL");
	return 0;
    }
    op->contr->last_used = NULL;
    op->contr->last_used_id = 0;

    if (sack->env!=op) {
	if (sack->other_arch == NULL || sack->env != NULL) {
	    new_draw_info(NDI_UNIQUE, 0,op,"You must get it first.");
	    return 1;
	}
	/* It's on the ground, the problems begin */
	if (op->container != sack) {
	    /* it's closed OR some player has opened it */
	    if (QUERY_FLAG(sack, FLAG_APPLIED)) {
		for(tmp=get_map_ob(sack->map, sack->x, sack->y); 
		    tmp && tmp->container != sack; tmp=tmp->above);
		if (tmp) {
		    /* some other player have opened it */
		    new_draw_info_format(NDI_UNIQUE, 0, op,
			"%s is already occupied.", query_name(sack));
		    return 1;
		}
	    }
	}
	if ( QUERY_FLAG(sack, FLAG_APPLIED)) {
	    if (op->container == NULL) {
		tmp = arch_to_object (sack->other_arch);
		/* not good, but insert_ob_in_ob() is too smart */
		CLEAR_FLAG (tmp, FLAG_REMOVED);
		tmp->x= tmp->y = tmp->ox = tmp->oy = 0;
		tmp->map = NULL;
		tmp->env = sack;
		if (sack->inv)
		    sack->inv->above = tmp;
		tmp->below = sack->inv;
		tmp->above = NULL;
		sack->inv = tmp;
		SET_FLAG (sack, FLAG_WALK_OFF); /* trying force closing it */
		SET_FLAG (sack, FLAG_FLY_OFF);
	    } else {
		CLEAR_FLAG (sack, FLAG_WALK_OFF);
		CLEAR_FLAG (sack, FLAG_FLY_OFF);
		tmp = sack->inv;
		if (tmp && tmp->type ==  CLOSE_CON) {
		    remove_ob(tmp);
		    free_object (tmp);
		}
	    }
	}
    }

    if (QUERY_FLAG (sack, FLAG_APPLIED)) {
	if (op->container) {
	    if (op->container != sack) {
		tmp = op->container;
		apply_container (op, tmp);
		sprintf (buf, "You close %s and open ", query_name(tmp));
		op->container = sack;
		strcat (buf, query_name(sack));
		strcat (buf, ".");
	    } else {
		CLEAR_FLAG (sack, FLAG_APPLIED);
		op->container = NULL;
		sprintf (buf, "You close %s.", query_name(sack));
	    }
	} else {
	    CLEAR_FLAG (sack, FLAG_APPLIED);
	    sprintf (buf, "You open %s.", query_name(sack));
	    SET_FLAG (sack, FLAG_APPLIED);
	    op->container = sack;
	}
    } else { /* not applied */
	if (sack->slaying) { /* it's locked */
	  tmp = FindKey(sack, op->inv);
	  if (tmp) {
	    sprintf (buf, "You unlock %s with %s.", query_name(sack), query_name(tmp));
	    SET_FLAG (sack, FLAG_APPLIED);
	    if (sack->env == NULL) { /* if it's on ground,open it also */
	      new_draw_info (NDI_UNIQUE,0,op, buf);
	      apply_container (op, sack);
	      return 1;
	    }
	  } else {
	    sprintf (buf, "You don't have the key to unlock %s.",
		     query_name(sack));
	  }
	} else {
	    sprintf (buf, "You readied %s.", query_name(sack));
	    SET_FLAG (sack, FLAG_APPLIED);
	    if (sack->env == NULL) {  /* if it's on ground,open it also */
		new_draw_info (NDI_UNIQUE, 0, op, buf);
		apply_container (op, sack);
		return 1;
	    }
	}
    }
    new_draw_info (NDI_UNIQUE, 0, op, buf);
    if (op->contr) op->contr->socket.update_look=1;
    return 1;
}

/*
 * Eneq(@csd.uu.se): Handle apply on containers. 
 * Moved to own function and added many features [Tero.Haatanen@lut.fi]
 * This version is for client/server mode.
 * op is the player, sack is the container the player is opening or closing.
 * return 1 if an object is apllied somehow or another, 0 if error/no apply
 *
 * Reminder - there are three states for any container - closed (non applied),
 * applied (not open, but objects that match get tossed into it), and open
 * (applied flag set, and op->container points to the open container)
 */

int esrv_apply_container (object *op, object *sack)
{
    object *tmp=op->container;

    if(op->type!=PLAYER)
	return 0; /* This might change */

    if (sack==NULL || sack->type != CONTAINER) {
	LOG (llevError, "esrv_apply_container: %s is not container!\n",sack?sack->name:"NULL");
	return 0;
    }

    /* If we have a currently open container, then it needs to be closed in all cases
     * if we are opening this one up.  We then fall through if appropriate for
     * openening the new container.
     */

    if (op->container && QUERY_FLAG(sack, FLAG_APPLIED)) {
	if (op->container->env != op) { /* if container is on the ground */
	    CLEAR_FLAG (op->container, FLAG_WALK_OFF);
	    CLEAR_FLAG (op->container, FLAG_FLY_OFF);
	}
	new_draw_info_format(NDI_UNIQUE, 0, op, "You close %s.",
		      query_name(op->container));
	CLEAR_FLAG(op->container, FLAG_APPLIED);
	op->container=NULL;
	esrv_update_item (UPD_FLAGS, op, tmp);
	if (tmp == sack) return 1;
    }
		      

    /* If the player is trying to open it (which he must be doing if we got here),
     * and it is locked, check to see if player has the equipment to open it.
     */

    if (sack->slaying) { /* it's locked */
      tmp=FindKey(sack, op->inv);
      if (tmp) {
	new_draw_info_format(NDI_UNIQUE, 0, op, "You unlock %s with %s.", query_name(sack), query_name(tmp));
      } else {
	new_draw_info_format(NDI_UNIQUE, 0, op,  "You don't have the key to unlock %s.",
			     query_name(sack));
	return 0;
      }
    }

    /* By the time we get here, we have made sure any other container has been closed and
     * if this is a locked container, the player they key to open it.
     */

    /* There are really two cases - the sack is either on the ground, or the sack is
     * part of the players inventory.  If on the ground, we assume that the player is
     * opening it, since if it was being closed, that would have been taken care of above.
     */


    if (sack->env != op) {
	/* Hypothetical case - the player is trying to open a sack that belong to someone
	 * else.  This normally should not happen, but a misbehaving client/player could
	 * try to do it, so lets handle it gracefully.
	 */
	if (sack->env) {
	    new_draw_info_format(NDI_UNIQUE, 0, op, "You can't open %s",
				 query_name(sack));
	    return 0;
	}
	/* set these so when the player walks off, we can unapply the sack */
	SET_FLAG (sack, FLAG_WALK_OFF); /* trying force closing it */
	SET_FLAG (sack, FLAG_FLY_OFF);

	CLEAR_FLAG (sack, FLAG_APPLIED);
	new_draw_info_format(NDI_UNIQUE, 0, op, "You open %s.", query_name(sack));
	SET_FLAG (sack, FLAG_APPLIED);
	op->container = sack;
	esrv_update_item (UPD_FLAGS, op, sack);
	esrv_send_inventory (op, sack);

    } else { /* sack is in players inventory */
	if (QUERY_FLAG (sack, FLAG_APPLIED)) { /* readied sack becoming open */
	    CLEAR_FLAG (sack, FLAG_APPLIED);
	    new_draw_info_format(NDI_UNIQUE, 0, op, "You open %s.", query_name(sack));
	    SET_FLAG (sack, FLAG_APPLIED);
	    op->container = sack;
	    esrv_update_item (UPD_FLAGS, op, sack);
	    esrv_send_inventory (op, sack);
	}
	else {
	    CLEAR_FLAG (sack, FLAG_APPLIED);
	    new_draw_info_format(NDI_UNIQUE, 0, op, "You readied %s.", query_name(sack));
	    SET_FLAG (sack, FLAG_APPLIED);
	    esrv_update_item (UPD_FLAGS, op, sack);
	}
    }
    return 1;
}

/*
 * Returns pointer a static string containing gravestone text
 */
char *gravestone_text (object *op)
{
    static char buf2[MAX_BUF];
    char buf[MAX_BUF];
    time_t now = time (NULL);

    strcpy (buf2, "                 R.I.P.\n\n");
    if (op->type == PLAYER)
	sprintf(buf, "%s the %s\n", op->name, op->contr->title);
    else
	sprintf(buf, "%s\n", op->name);
    strncat (buf2, "                    ",  20 - strlen(buf) / 2);
    strcat (buf2, buf);
    if (op->type == PLAYER)
	sprintf(buf, "who was in level %d when killed\n", op->level);
    else
	sprintf(buf, "who was in level %d when died.\n\n", op->level);
    strncat (buf2, "                    ",  20 - strlen(buf) / 2);
    strcat (buf2, buf);
    if (op->type == PLAYER) {
	sprintf(buf, "by %s.\n\n", op->contr->killer);
	strncat (buf2, "                    ",  21 - strlen(buf) / 2);
	strcat (buf2, buf);
    }
    strftime (buf, MAX_BUF, "%b %d %Y\n", localtime(&now));
    strncat (buf2, "                    ",  20 - strlen(buf) / 2);
    strcat (buf2, buf);
    return buf2;
}


int make_gravestone (object *op, object *grave)
{
    char buf[MAX_BUF];
    object *tmp, *stone = NULL, *corpse = NULL;
    
    for (tmp=get_map_ob(op->map, op->x, op->y); tmp; tmp=tmp->above)
	if (tmp->type == GRAVESTONE)
	    stone = tmp;
	else if (tmp->type == CORPSE)
	    corpse = tmp;
    for (tmp = op->inv; tmp; tmp = tmp->below)
	if (tmp->type == GRAVESTONE && !stone)
	    stone = tmp;
	else if (tmp->type == CORPSE && !corpse)
	    corpse = tmp;
    
    if (! stone) {
	new_draw_info(NDI_UNIQUE, 0,op, "You need a gravestone.");
	return 1;
    } else if (! corpse) {
	new_draw_info(NDI_UNIQUE, 0,op, "You need a corpse.");
	return 1;
    }

    /* make a gravestone */
    if(stone->name)
	free_string(stone->name);
    sprintf (buf, "%s's %s", corpse->name, stone->name);
    stone->name=add_string(buf);
    if (stone->msg)
	free_string(stone->msg);
    if (corpse->msg)
	stone->msg = add_string(corpse->msg);
    else
	stone->msg = add_string(gravestone_text(corpse));
    stone->x = op->x;
    stone->y = op->y;
    SET_FLAG (stone, FLAG_NO_PICK);
    remove_ob (stone);
    insert_ob_in_map(stone, op->map);

    /* delete grave and corpse */
    remove_ob (corpse);
    free_object (corpse);
    remove_ob (grave);
    free_object (grave);
    return 1;
}

/* apply returns 0 if it wasn't possible to apply that object, 1
 * if the object was applied, 2 if the object is now a different
 * object.
 */
/* op is the object that is causing object to be applied, tmp is the object
 * being applied.
 * aflag is special (always apply/unapply) flags.  Nothing is done with
 * them in this function - they are passed to apply_special
 */

int apply(object *op, object *tmp, int aflag) {
  char buf[MAX_BUF];
  int inven;

  if(tmp==NULL) {
    if(op!=NULL&&op->type==PLAYER)
      new_draw_info(NDI_UNIQUE, 0,op,"There is nothing to apply.");
    return 0;
  }
  inven=(tmp->env!=NULL);
  if(op->type==PLAYER&&!inven&&QUERY_FLAG(op,FLAG_FLYING)&&
	!QUERY_FLAG(tmp,FLAG_FLYING)&&!QUERY_FLAG(tmp,FLAG_FLY_ON)&&
     !QUERY_FLAG(op,FLAG_WIZ)) {
    new_draw_info(NDI_UNIQUE, 0,op,"But you are floating high above the ground!");
    return 0;
  }
  while(tmp!=NULL&&(!(QUERY_FLAG(tmp,FLAG_WALK_ON)||QUERY_FLAG(tmp,FLAG_FLY_ON))
	&&tmp->invisible))
    tmp=inven?tmp->below:tmp->above;
  if(tmp==NULL)
    return 0;
  if(QUERY_FLAG(tmp,FLAG_WAS_WIZ)&&!QUERY_FLAG(op,FLAG_WAS_WIZ)&&op->type==PLAYER) {
    play_sound_map(op->map, op->x, op->y, SOUND_OB_EVAPORATE);
    new_draw_info(NDI_UNIQUE, 0,op,"The object disappears in a puff of smoke!");
    new_draw_info(NDI_UNIQUE, 0,op,"It must have been an illusion.");
    remove_ob(tmp);
    free_object(tmp);
    return 1;
  }
  if(op->type==PLAYER) {
    op->contr->last_used=tmp;
    op->contr->last_used_id=tmp->count;
  }
  switch(tmp->type) {
  case PLAYERMOVER:
    if (tmp->attacktype && (tmp->level || op->type!=PLAYER)) {
	if (!tmp->stats.maxsp) tmp->stats.maxsp=2.0;
	/* Is this correct?  From the docs, it doesn't look like it
	 * should be divided by tmp->speed
	 */
	op->speed_left = -FABS(tmp->stats.maxsp*op->speed/tmp->speed);
	/* Just put in some sanity check.  I think there is a bug in the
	 * above with some objects have zero speed, and thus the player
	 * getting permanently paralyzed.
	 */
	if (op->speed_left<-50.0) op->speed_left=-50.0;
/*	fprintf(stderr,"apply, playermove, player speed_left=%f\n", op->speed_left);*/
    }
    return 1;

  case SPINNER:
    if(op->direction) {
      op->direction=absdir(op->direction-tmp->stats.sp);
      update_turn_face(op);
    }
    return 1;
  case DIRECTOR:
    if(op->direction) {
      op->direction=tmp->stats.sp;
      update_turn_face(op);
    }
    return 1;
  case BUTTON:
  case PEDESTAL:
    update_button(tmp);
    return 1;
  case ALTAR:
    /* If it is a player that moved on top, just return.  A player can't
     * be the sacrifice.  This prevents double donation problems.
     */
    if (op->type==PLAYER) return 0;

    if (check_altar (tmp)) {
      /* Simple check.  Unfortunately, it means you can't cast magic bullet
       * with an altar.  We call it a Potion - altars are stationary - it
       * is up to map designers to use them properly.
       */
      if (tmp->stats.sp) {
        object *pl=get_map_ob(tmp->map, tmp->x, tmp->y);
	while (pl!=NULL && pl->type!=PLAYER)
	    pl=pl->above;
	if (pl==NULL) {
	    LOG(llevError,"Identify Altar: Can't find player!.\n");
	    return 0;
	}
	cast_spell(pl, tmp, 0, tmp->stats.sp, 0, spellPotion, NULL);
	new_draw_info_format(NDI_BLACK, 0, op,
             "The altar casts %s.", spells[tmp->stats.sp].name);
	/* If it is connected, push the button.  Fixes some problems with
	 * old maps.
	 */
	push_button(tmp);
      } else {
	tmp->value = 1;  /* works only once */
	push_button(tmp);
      }
    }
    return 1;
  case ARROW:
    if(QUERY_FLAG(op, FLAG_ALIVE)&&tmp->speed) {
      if(attack_ob(op,tmp)) {
	/* There can be cases where a monster 'kills' an arrow.  Typically
	 * happens for things like black puddings that have hitback properties.
	 */
	if (!QUERY_FLAG(tmp, FLAG_FREED)) {
	    remove_ob(tmp);
	    stop_arrow(tmp,op);
	}
      }
      return 1;
    }
    return 0;
  case CONE: /* A cone in the form of a wall */
    if(QUERY_FLAG(op, FLAG_ALIVE)&&tmp->speed)
      hit_player(op,tmp->stats.dam*20,tmp,tmp->attacktype);
    break;
  case FBULLET:
  case BULLET:
    check_fired_arch(tmp);
    return 1;
  case TRAPDOOR:
    play_sound_map(op->map, op->x, op->y, SOUND_FALL_HOLE);
    {
      int max;
      object *ab;
      if(!tmp->value) {
        int tot;
        for(ab=tmp->above,tot=0;ab!=NULL;ab=ab->above)
          if(!QUERY_FLAG(ab,FLAG_FLYING))
            tot+=ab->weight+ab->carrying;
        if(!(tmp->value=(tot>tmp->weight)?1:0))
          return 1;
	SET_ANIMATION(tmp, tmp->value);
        update_object(tmp);
      }
      for(ab=tmp->above,max=100;--max&&ab&&!QUERY_FLAG(ab, FLAG_FLYING);ab=ab->above) {
        transfer_ob(ab,(int)EXIT_X(tmp),(int)EXIT_Y(tmp));
        new_draw_info(NDI_UNIQUE, 0,ab,"You fall into a trapdoor!");
      }
    }
    return 1;
  case CONVERTER:
    if(convert_item(op,tmp))
      return 2;
    return 0;
  case HANDLE:
    new_draw_info(NDI_UNIQUE, 0,op,"You turn the handle.");
    play_sound_map(op->map, op->x, op->y, SOUND_TURN_HANDLE);
    tmp->value=tmp->value?0:1;
    SET_ANIMATION(tmp, tmp->value);
    update_object(tmp);
    push_button(tmp);
    return 1;
  case TRIGGER:
  case TRIGGER_BUTTON:
  case TRIGGER_PEDESTAL:
  case TRIGGER_ALTAR:
    check_trigger(tmp);
    return 1;
  case DEEP_SWAMP:
    deep_swamp(tmp, 1);
    return 1;
  case CHECK_INV:
    check_inv(op, tmp);
    break;
  case HOLE:
    if(tmp->stats.wc>0) /* Is the hole open? */
      return 0; /* Nope.  Change return to 0*/
    /* Is this a multipart monster and not the head?  If so, return.
     * Processing will happen if the head runs into the pit
     */
    if (op->head) return 0;
    {
	int i=find_free_spot(op->arch,op->map,EXIT_X(tmp),EXIT_Y(tmp),0,SIZEOFFREE);
	object *part;

	/* If no free spot, return */
	if (i==-1) return 0;
	remove_ob(op);
	for (part=op; part!=NULL; part=part->more) {
	    part->x=EXIT_X(tmp)+freearr_x[i] + (part->arch?part->arch->clone.x:0);
	    part->y=EXIT_Y(tmp)+freearr_y[i] + (part->arch?part->arch->clone.y:0);
	}
	insert_ob_in_map(op,op->map);
    }
    play_sound_map(op->map, op->x, op->y, SOUND_FALL_HOLE);
    new_draw_info(NDI_UNIQUE, 0,op,"You fall through the hole!\n");
    return 1;
  case EXIT:
    if(op->type!=PLAYER)
      return 0;
    if(tmp->head!=NULL)
      tmp=tmp->head;
    if(!EXIT_PATH(tmp)) {
      new_draw_info_format(NDI_UNIQUE, 0, op, 
	"The %s is closed.",query_name(tmp));
      return 1;
    }
    if (tmp->msg)
	new_draw_info(NDI_NAVY, 0, op, tmp->msg);
    enter_exit(op,tmp);
    break;
  case ENCOUNTER:
#ifdef RANDOM_ENCOUNTERS
    random_encounter(op, tmp);
#endif
    break;
  case SHOP_MAT:
    {
      SET_FLAG(op,FLAG_NO_APPLY);
      if(op->type!=PLAYER) {
        if(QUERY_FLAG(op, FLAG_UNPAID)) { /* Just move the item to an adjacent place */
          int i=find_free_spot(op->arch,op->map,op->x,op->y,1,9);
          if(i==-1) return 1;
          transfer_ob(op,op->x+freearr_x[i],op->y+freearr_y[i]);
          CLEAR_FLAG(op, FLAG_NO_APPLY);
          return 1;
        }
        if(op->more||op->head) return 1; /* Some nasty bug has to be fixed here... */
        teleport(tmp,SHOP_MAT);
        CLEAR_FLAG(op, FLAG_NO_APPLY);
        return 1;
      }
      if(get_payment(op)) {
        teleport(tmp,SHOP_MAT);
        if((tmp=get_map_ob(op->map,op->x,op->y))==NULL||tmp->type!=SHOP_FLOOR)
          new_draw_info(NDI_UNIQUE, 0,op,"Thank you for visiting our shop.");
      }
      else {
        int i=find_free_spot(op->arch,op->map,op->x,op->y,1,9);
        if(i==-1)
          LOG(llevError,"Internal shop-mat problem.\n");
        else {
          remove_ob(op);
          op->x+=freearr_x[i],op->y+=freearr_y[i];
          insert_ob_in_map(op,op->map);
        }
      }
      CLEAR_FLAG(op, FLAG_NO_APPLY);
      return 1;
    }
  case SIGN:
    if (tmp->stats.food && ++tmp->last_eat>tmp->stats.food) {
	new_draw_info(NDI_UNIQUE, 0, op, "Nothing is written on it.");
	return 1;
    }
    if(QUERY_FLAG(op, FLAG_BLIND)&&!QUERY_FLAG(op,FLAG_WIZ))
      new_draw_info(NDI_UNIQUE, 0,op,"You are unable to read while blind.");
    else if(tmp->msg==NULL)
      new_draw_info(NDI_UNIQUE, 0,op,"Nothing is written on it.");
    else
      new_draw_info(NDI_UNIQUE | NDI_NAVY,0, op,tmp->msg);
    return 1;
  case BOOK: 
    if(op->type!=PLAYER) return 1;
    if(QUERY_FLAG(tmp,FLAG_UNPAID)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You should pay for it first.");
      break;
    }
    if(QUERY_FLAG(op, FLAG_BLIND)&&!QUERY_FLAG(op,FLAG_WIZ)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You are unable to read while blind.");
      break;
    }
    if(tmp->msg==NULL) {
      new_draw_info_format(NDI_UNIQUE, 0, op, 
	"You open the %s and find it empty.", tmp->name);
    } else 
#ifdef ALLOW_SKILLS /* need a literacy skill to read stuff! */
    if(!QUERY_FLAG(op,FLAG_WIZ)&&!change_skill(op,SK_LITERACY)) 
      new_draw_info(NDI_UNIQUE, 0,op,
	"You are unable to decipher the strange symbols.");
    else if(QUERY_FLAG(op,FLAG_WIZ)||((SK_level(op)+5)>=tmp->level)) 
#endif
    {
      new_draw_info_format(NDI_UNIQUE, 0, op, 
	"You open the %s and start reading.", tmp->name);
      new_draw_info(NDI_UNIQUE | NDI_NAVY, 0, op, 
	tmp->msg);
#ifdef ALLOW_SKILLS /* gain xp from reading */  
      if(!QUERY_FLAG(tmp,FLAG_NO_SKILL_IDENT)) { /* only if not read before */ 
	  int exp_gain=calc_skill_exp(op,tmp);
	  if(!QUERY_FLAG(tmp,FLAG_IDENTIFIED)) {
	    /*exp_gain *= 2; because they just identified it too */
	    SET_FLAG(tmp,FLAG_IDENTIFIED);
	    /* If in a container, update how it looks */
	    if(tmp->env) esrv_update_item(UPD_FLAGS|UPD_NAME, op,tmp);
	    else op->contr->socket.update_look=1;
	  }
          add_exp(op,exp_gain);
          SET_FLAG(tmp,FLAG_NO_SKILL_IDENT); /* so no more xp gained from this book */ 
      }   
    } else { 
      int lev_diff = tmp->level - SK_level(op) -5;
      if (lev_diff < 1)
	LOG(llevError,"Couldn't read book when player was high enough level");
      else if (lev_diff < 2)
	new_draw_info(NDI_UNIQUE, 0,op,"This book is just barely beyond your comprehension.");
      else if (lev_diff < 3)
	new_draw_info(NDI_UNIQUE, 0,op,"This book is slightly beyond your comprehension.");
      else if (lev_diff < 5)
	new_draw_info(NDI_UNIQUE, 0,op,"This book is beyond your comprehension.");
      else if (lev_diff < 8)
	new_draw_info(NDI_UNIQUE, 0,op,"This book is quite a bit beyond your comprehension.");
      else if (lev_diff < 15)
	new_draw_info(NDI_UNIQUE, 0,op,"This book is way beyond your comprehension.");
      else 
	new_draw_info(NDI_UNIQUE, 0,op,"This book is totally beyond your comprehension.");
#endif
    }
    return 1;

/* these scrolls allow acquistion of skills by players
 * -b.t. thomas@nomad.astro.psu.edu
 */
  case SKILLSCROLL: 
#ifdef ALLOW_SKILLS /* calls here shouldnt happen, but to be on safe side put here */ 
    if(op->type!=PLAYER)
      return 0;
    if(QUERY_FLAG(tmp,FLAG_UNPAID)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You should pay for it first.");
      break;
    }
    switch((int)learn_skill(op,tmp)) {
      case 0:
      	  new_draw_info(NDI_UNIQUE, 0,op,"You already possess the knowledge "); 
      	  new_draw_info_format(NDI_UNIQUE, 0,op,"held within the %s.\n",query_name(tmp)); 
	  return 1;
      case 1: 
      	  new_draw_info_format(NDI_UNIQUE, 0,op,"You succeed in learning %s",
	    skills[tmp->stats.sp].name);
      	  new_draw_info_format(NDI_UNIQUE, 0, op,
             "Type 'bind ready_skill %s",skills[tmp->stats.sp].name);
          new_draw_info(NDI_UNIQUE, 0,op,"to store the skill in a key.");
	  fix_player(op); /* to immediately link new skill to exp object */
      	  break;
      default:
      	  new_draw_info_format(NDI_UNIQUE,0,op,
		"You fail to learn the knowledge of the %s.\n",query_name(tmp));
	  break;
    }
    decrease_ob(tmp);
#endif
    return 1; 
  case SPELLBOOK:
    if(op->type!=PLAYER)
      return 0;
    if(QUERY_FLAG(tmp,FLAG_UNPAID)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You should pay for it first.");
      break;
    }
    if(QUERY_FLAG(op, FLAG_BLIND)&&!QUERY_FLAG(op,FLAG_WIZ)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You are unable to read while blind.");
      break;
    }

    /* artifact_spellbooks have 'slaying' field point to a spell name,
    ** instead of having their spell stored in stats.sp.  We should update
    ** stats->sp to point to that spell */
 
    if(tmp->slaying != NULL) {
       if((tmp->stats.sp = look_up_spell_name(tmp->slaying)) <0 ){
	  tmp->stats.sp = -1; 
	  new_draw_info_format(NDI_UNIQUE, 0, op,
		"The book's formula for %s is incomplete", tmp->slaying);
	  return 1;
       }
       /* now clear tmp->slaying since we no longer need it */
       free_string(tmp->slaying);
       tmp->slaying=NULL;
    }

	/* need a literacy skill to learn spells. Also, having a literacy level
	 * lower than the spell will make learning the spell more difficult */
#ifdef ALLOW_SKILLS 
    if(op->type==PLAYER&&!change_skill(op,SK_LITERACY)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You can't read! Your attempt fails.");
      return 1;
    }
#endif
    if(tmp->stats.sp < 0 || tmp->stats.sp > NROFREALSPELLS
        || spells[tmp->stats.sp].level>(SK_level(op)+10)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You are unable to decipher the strange symbols.");
      return 1;
    } 

    new_draw_info_format(NDI_UNIQUE, 0, op, 
	"The spellbook contains the %s level spell %s.",
            get_levelnumber(spells[tmp->stats.sp].level),
            spells[tmp->stats.sp].name);

    if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED)) {
	identify(tmp);
	if (tmp->env)
	    esrv_update_item(UPD_FLAGS|UPD_NAME,op,tmp);
	else
	    op->contr->socket.update_look=1;
    }

    if(check_spell_known(op,tmp->stats.sp)) {
	new_draw_info(NDI_UNIQUE, 0,op,"You already know that spell.\n");
	return 1;
    }

    /* I changed spell learning in 3 ways:
     *
     *  1- MU spells use Int to learn, Cleric spells use Wisdom
     *
     *  2- The learner's level (in skills sytem level==literacy level; if no 
     *     skills level == overall level) impacts the chances of spell learning. 
     *
     *  3 -Automatically fail to learn if you read while confused
     * 
     * Overall, chances are the same but a player will find having a high 
     * literacy rate very useful!  -b.t. 
     */ 
    if(QUERY_FLAG(op,FLAG_CONFUSED)) { 
      new_draw_info(NDI_UNIQUE,0,op,"In your confused state you flub the wording of the text!");
      scroll_failure(op,RANDOM()%(spells[tmp->stats.sp].level+1),spells[tmp->stats.sp].sp);
    } else if(QUERY_FLAG(tmp,FLAG_STARTEQUIP) || RANDOM()%150-(2*SK_level(op)) <
	learn_spell[spells[tmp->stats.sp].cleric ? op->stats.Wis : op->stats.Int]) {
      play_sound_player_only(op->contr, SOUND_LEARN_SPELL,0,0);
      new_draw_info(NDI_UNIQUE, 0,op,"You succeed in learning the spell!");
      op->contr->known_spells[op->contr->nrofknownspells++]=tmp->stats.sp;
      if(op->contr->nrofknownspells == 1)
	op->contr->chosen_spell=tmp->stats.sp;
      new_draw_info_format(NDI_UNIQUE, 0, op, 
	"Type 'bind cast %s",spells[tmp->stats.sp].name);
      new_draw_info(NDI_UNIQUE, 0,op,"to store the spell in a key.");
#ifdef ALLOW_SKILLS /* xp gain to literacy for spell learning */
      if(op->type==PLAYER) add_exp(op,calc_skill_exp(op,tmp));
#endif
    } else {
      play_sound_player_only(op->contr, SOUND_FUMBLE_SPELL,0,0);
      new_draw_info(NDI_UNIQUE, 0,op,"You fail to learn the spell.\n");
    }
    decrease_ob(tmp);
    return 1;
  case SCROLL: {
      int scroll_spell=tmp->stats.sp, old_spell=0;
      rangetype old_shoot=range_none;

    if(QUERY_FLAG(tmp, FLAG_UNPAID)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You should pay for it first.");
      break;
    }
    if(QUERY_FLAG(op, FLAG_BLIND)&&!QUERY_FLAG(op,FLAG_WIZ)) {
      new_draw_info(NDI_UNIQUE, 0,op, "You are unable to read while blind.");
      break;
    }    

    if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED)) 
      identify(tmp);

    if( scroll_spell < 0 || scroll_spell >= NROFREALSPELLS) {
        sprintf(buf, "The scroll just doesn't make sense!");
        break;
    }

    if(op->type==PLAYER) {
#ifdef ALLOW_SKILLS /* players need a literacy skill to read stuff! */
	int exp_gain=0;

        if(!QUERY_FLAG(op,FLAG_WIZ)&&!change_skill(op,SK_LITERACY)){ 
          new_draw_info(NDI_UNIQUE, 0,op,
            "You are unable to decipher the strange symbols.");
          break;
        } 

	/* We give exp for reading the scroll.  I reduced this (0.95.3) to
	 * give half as much since there is no longer any risk.  Even this
	 * could be excessive.
	 */
	if((exp_gain = calc_skill_exp(op,tmp)))
	    add_exp(op,exp_gain/2);


        /* Now, call here so the right skill is readied -- literacy 
	 * isnt necesarily connected to the exp obj to which the xp 
	 * will go (for kills made by the magic of the scroll) 
	 */ 
        SET_FLAG(tmp,FLAG_APPLIED);
        (void) check_skill_to_apply(op,tmp);
        CLEAR_FLAG(tmp,FLAG_APPLIED);

#endif/* ALLOW_SKILLS */ 
        old_shoot= op->contr->shoottype;
        old_spell = op->contr->chosen_spell;
        op->contr->shoottype=range_scroll;
        op->contr->chosen_spell = scroll_spell;
    }

    new_draw_info_format(NDI_BLACK, 0, op,
        "The scroll of %s turns to dust.", spells[tmp->stats.sp].name);
    sprintf(buf, "%s reads a scroll of %s.",op->name,spells[tmp->stats.sp].name);
    new_info_map(NDI_ORANGE, op->map, buf);

    cast_spell(op,tmp,0,scroll_spell,0,spellScroll,NULL);
    decrease_ob(tmp);
    if(op->type==PLAYER) {
      if(op->contr->golem==NULL) {
        op->contr->shoottype=old_shoot;
	op->contr->chosen_spell = old_spell;
      }
    }
    break;
  }
  case POTION:
    SET_FLAG(tmp,FLAG_APPLIED);
    (void) check_skill_to_apply(op,tmp);
    CLEAR_FLAG(tmp,FLAG_APPLIED);
    return apply_potion(op, tmp);


/* Eneq(@csd.uu.se): Handle apply on containers. */
  case CLOSE_CON:
    if (op->type==PLAYER)
      return esrv_apply_container (op, tmp->env);
    else
      return apply_container (op, tmp->env);
  case CONTAINER:
    if (op->type==PLAYER)
      return esrv_apply_container (op, tmp);
    else
      return apply_container (op, tmp);

  case TREASURE: {
    object *treas;
    if(QUERY_FLAG(tmp, FLAG_UNPAID)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You should pay for it first.");
      break;
    }
/*  Nice side effect of new treasure creation method is that the treasure
    for the chest is done when the chest is created, and put into the chest
    inventory.  So that when the chest burns up, the items still exist.  Also
    prevents people fromt moving chests to more difficult maps to get better
    treasure
*/
    treas = tmp->inv;
    if(treas==NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"The chest was empty.");
      decrease_ob(tmp);
      return 1;
    }
    do {
      remove_ob(treas);
      draw_find(op,treas);
      treas->x=op->x,treas->y=op->y;

      /* Changed (0.91.2) - always drop treasure to floor (needed for
       * trap code
       */
      insert_ob_in_map(treas,op->map);
    } while ((treas=tmp->inv)!=NULL);
    
    decrease_ob(tmp);

    /* Done to re-stack map with player on top? */
    remove_ob(op);
    insert_ob_in_map(op,op->map);
    break;
  }
  case WAND:
    if(apply_special(op,tmp,aflag))
      return 1;
    if(op->type==PLAYER) {
      if(QUERY_FLAG(tmp, FLAG_APPLIED)) {
        op->contr->shoottype=range_wand;
	op->contr->chosen_item_spell= tmp->stats.sp;
        if (QUERY_FLAG(tmp,FLAG_BEEN_APPLIED) || QUERY_FLAG(tmp, FLAG_IDENTIFIED))
          op->contr->known_spell = 1;
        else
          op->contr->known_spell = 0;
        fix_player(op);
      }
    } else {
      if(QUERY_FLAG(tmp, FLAG_APPLIED))
	SET_FLAG(op, FLAG_READY_WAND);
      else
	CLEAR_FLAG(op, FLAG_READY_WAND);
    }
    return 1;
  case ROD:
    if(apply_special(op,tmp,aflag))
      return 1;
    if(op->type==PLAYER) {
      if(QUERY_FLAG(tmp, FLAG_APPLIED)) {
        op->contr->shoottype=range_rod;
        op->contr->chosen_item_spell=tmp->stats.sp;
        if (QUERY_FLAG(tmp, FLAG_BEEN_APPLIED) || QUERY_FLAG(tmp, FLAG_IDENTIFIED))
          op->contr->known_spell = 1;
        else
          op->contr->known_spell = 0;
        fix_player(op);
      }
    } else {
      if(QUERY_FLAG(tmp, FLAG_APPLIED))
	SET_FLAG(op, FLAG_READY_ROD);
      else
	CLEAR_FLAG(op, FLAG_READY_ROD);
    }
    return 1;
  case HORN:
    if(apply_special(op,tmp,aflag))
      return 1;
    if(op->type==PLAYER) {
      if(QUERY_FLAG(tmp, FLAG_APPLIED)) {
        op->contr->shoottype=range_horn;
        op->contr->chosen_item_spell=tmp->stats.sp;
        if (QUERY_FLAG(tmp, FLAG_BEEN_APPLIED) || QUERY_FLAG(tmp, FLAG_IDENTIFIED))
          op->contr->known_spell = 1;
        else
          op->contr->known_spell = 0;
        fix_player(op);
      }
    } else {
      if(QUERY_FLAG(tmp, FLAG_APPLIED))
	SET_FLAG(op, FLAG_READY_HORN);
      else
	CLEAR_FLAG(op, FLAG_READY_HORN);
    }
    return 1;
  case BOW:
    if(apply_special(op,tmp,aflag))
      return 0;
    if(QUERY_FLAG(tmp, FLAG_APPLIED)) {
      new_draw_info_format(NDI_UNIQUE, 0, op,
	"You will now fire %s with %s.",
	      tmp->race ? tmp->race : "nothing", query_name(tmp));
      if(op->type==PLAYER) {
        op->contr->shoottype=range_bow;
        fix_player(op);
      }
    }
    return 1;
#ifdef ALLOW_SKILLS
  case SKILL:   /* allows skill tools  to be used -b.t */
    if(apply_special(op,tmp,aflag))
      return 0;
    if((!tmp->invisible||op->type!=PLAYER)&&QUERY_FLAG(tmp, FLAG_APPLIED)) {
        SET_FLAG(op, FLAG_READY_SKILL);
        op->chosen_skill=tmp;
        if(op->type==PLAYER) {
          new_draw_info_format(NDI_UNIQUE, 0, op,
             "You can now use the skill: %s.",skills[tmp->stats.sp].name);
          op->contr->shoottype=range_skill;
	  link_player_skill(op, tmp);
          fix_player(op);
        }
    } else { 
        CLEAR_FLAG(op, FLAG_READY_SKILL);
	unlink_skill(tmp);
    }
    return 1;
#endif
  case WEAPON:
  case ARMOUR:
  case BOOTS:
  case GLOVES:
  case AMULET:
  case GIRDLE:
  case BRACERS:
  case SHIELD:
  case HELMET:
  case RING:
  case CLOAK:
/* Mol(mol@meryl.csd.uu.se) compressed return & apply_s into one statement */
/* Frank: If done this way, a ! must be prepended */
    return !apply_special(op,tmp,aflag);
  case DRINK:
  case FOOD:
  case FLESH:
    if(QUERY_FLAG(tmp, FLAG_UNPAID)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You should pay for it first.");
      return 1;
    }
    if(op->type!=PLAYER)
      op->stats.hp=op->stats.maxhp;
    else {
      if(op->stats.food+tmp->stats.food>999) {
	if(tmp->type==FOOD || tmp->type==FLESH)
	  new_draw_info(NDI_UNIQUE, 0,op,"You can't possibly eat all that now.");
	else
	  new_draw_info(NDI_UNIQUE, 0,op,"You can't possibly drink all that now.");
	return 1;
      }
      if(!QUERY_FLAG(tmp, FLAG_CURSED)) {
        char buf[MAX_BUF];
        if(tmp->type==DRINK)
            sprintf(buf,"Ahhh...that %s tasted good.",tmp->name);
        else 
            sprintf(buf,"The %s tasted %s",tmp->name,
		tmp->type==FLESH?"terrible!":"good.");

        new_draw_info(NDI_UNIQUE, 0,op,buf); 
        op->stats.food+=tmp->stats.food;
        op->stats.hp+=tmp->stats.food/50;
        if(op->stats.hp>op->stats.maxhp)
          op->stats.hp=op->stats.maxhp;
      }
      /* special food hack -b.t. */
      if(tmp->title || QUERY_FLAG(tmp,FLAG_CURSED)) eat_special_food(op,tmp);
    }
    decrease_ob(tmp);
    return 1;
  case POISON:
#if 0
   /* monsters can now use this */
    if(op->type!=PLAYER)
      return 0;
#endif
    if(QUERY_FLAG(tmp, FLAG_UNPAID)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You should pay for it first.");
      return 1;
    }
    play_sound_player_only(op->contr, SOUND_DRINK_POISON,0,0);
/*    op->stats.hp+=tmp->stats.hp;*/
    /* I think this is better */
    LOG(llevDebug,"Trying to poison player for %d hp\n", tmp->stats.hp);
    hit_player(op, tmp->stats.hp, tmp, AT_POISON);
    op->stats.food-=op->stats.food/4;
    if(op->type==PLAYER) {
      new_draw_info(NDI_UNIQUE, 0,op,"Yech!  That tasted poisonous!");
      strcpy(op->contr->killer,"poisonous booze");
    }
    decrease_ob(tmp);
    return 1;
  case SAVEBED:
    if(op->type!=PLAYER)
      return 0;
    if(!op->contr->name_changed||!op->stats.exp) {
      new_draw_info(NDI_UNIQUE, 0,op,"You don't deserve to save your character yet.");
      return 1;
    }
    if(QUERY_FLAG(op,FLAG_WAS_WIZ)) {
      new_draw_info(NDI_UNIQUE, 0,op,"Since you have cheated you can't save.");
      return 1;
    }
    remove_ob(op);
    op->direction=0;
    op->contr->count_left=0;
    new_draw_info_format(NDI_UNIQUE | NDI_ALL, 5, op,
	"%s leaves the game.",op->name);

    strcpy(op->contr->killer,"left");
    check_score(op); /* Always check score */
    (void)save_player(op,0);
    play_again(op);
    op->map->players--;
#if MAP_MAXTIMEOUT 
    op->map->timeout = MAP_TIMEOUT(op->map);
#endif
    return 1;

   case ARMOUR_IMPROVER: {
    object *armor;

    if(op->type!=PLAYER)
      return 0;
    if(QUERY_FLAG(tmp, FLAG_UNPAID)) {
        new_draw_info(NDI_UNIQUE, 0,op,"You should pay for it first.");
        return 0;
    }
    if (blocks_magic(op->map,op->x,op->y)) {
        new_draw_info(NDI_UNIQUE, 0,op,"Something blocks the magic of the scroll.");
        return 0;
    }
    armor=find_marked_object(op);
#if 0
    if (op->inv==NULL)  
	    	return 0;
    if (op->inv->type==ARMOUR
	|| op->inv->type==CLOAK
	|| op->inv->type==BOOTS || op->inv->type==GLOVES
	||op->inv->type==BRACERS || op->inv->type==SHIELD
	||op->inv->type==HELMET)
#endif
    if(!armor) {
      new_draw_info(NDI_UNIQUE, 0, op, "You need to mark an armor object.");
      return 0;
    }
    if (armor->type==ARMOUR
	|| armor->type==CLOAK
	|| armor->type==BOOTS || armor->type==GLOVES
	||armor->type==BRACERS || armor->type==SHIELD
	||armor->type==HELMET)
    {
    	    new_draw_info(NDI_UNIQUE, 0,op,"Applying armour enchantment.");
    	    improve_armour(op,tmp,armor);
	    esrv_send_item(op, tmp);
	    return 1;
    } 

    new_draw_info(NDI_UNIQUE, 0,op,"Your marked item is not armour!\n");
    return 0;
   }

   case WEAPON_IMPROVER:
    return check_improve_weapon(op, tmp);

   case MONEY_CHANGER:
    if(op->type!=PLAYER)
      return 0;
    new_draw_info(NDI_UNIQUE, 0,op,"Applied money changer.");
    if (tmp->slaying==NULL) {
      LOG(llevError,"Money changer converts to nothing???\n");
 /* If this is so critical, we should at least the object and generate
    a core file */
/*      kill(getpid(),1);*/
      return 0;
    }
    money_change(op,tmp->slaying);
    return 1;
   case CLOCK: {
	time_t t = time(NULL);
	strftime(buf, sizeof(buf), "Time is %I:%M %p", localtime(&t));
	play_sound_player_only(op->contr, SOUND_CLOCK,0,0);
	new_draw_info(NDI_UNIQUE, 0,op, buf);
	return 1;
   }
  case MENU: 
	if (op->type!=PLAYER) return 0;
	shop_listing(op);
	return 1;
  case POWER_CRYSTAL:
    apply_power_crystal(op,tmp);  /*  see egoitem.c */
    return 1;
  case LIGHTER:		/* for lighting torches/lanterns/etc */ 
    apply_lighter(op,tmp);
    return 1;
  case GRAVE:
    if (op->type != PLAYER)
      return 0;
    return make_gravestone (op, tmp);
  /* Drop a certain amount of gold, and have one item identified */
  case IDENTIFY_ALTAR:
    return apply_id_altar(op, tmp);
  default:
#if 0
    if(op->type==PLAYER) {
      new_draw_info(NDI_UNIQUE, 0,op,
      	"I don't know how to apply the %s.",query_name(tmp));
    }
#endif
    return 0;
  }
  return 1;
}

/* Apply_Below attempts to apply the object 'below' the player (op).
 * If the player has an open container, we use that for below, otherwise
 * we use the ground.
 */

void apply_below(object *op) {
    object *tmp, *next;

    /* If using a container, set the starting item to be the top
     * item in the container.  Otherwise, use the map.
     */
    if (op->container!=NULL) tmp=op->container->inv;
    else tmp=op->below;

    /* This is perhaps more complicated.  However, I want to make sure that
     * we don't use a corrupt pointer for the next object, so we get the
     * next object in the stack before applying.  This is probably only a 
     * problem if apply actually uses the object but does not return a
     * proper value.
     */
    for ( ; tmp!=NULL; tmp=next) {
	next = tmp->below;
	if (apply(op, tmp, 0)) return;
    }
}

/* who is the object using the object.
 * op is the object they are using.
 * aflags is special flags (0 - normal/toggel, 1=always apply, 2=always unapply
 * (see define.h, AP_* values)
 */
int apply_special(object *who,object *op, int aflags) { /* wear/wield */
  object *tmp;
  char buf[MAX_BUF];
  int i;

  if(who==NULL) {
    LOG(llevError,"apply_special() from object without environment.\n");
    return 1;
  }

  if(op->env!=who) {
    new_draw_info(NDI_UNIQUE, 0,who,"You must get it first.");
    return 1;
  }
  if(QUERY_FLAG(op,FLAG_APPLIED)) {
    /* always apply, so no reason to unapply */
    if (aflags == AP_APPLY) return 0;
    if (QUERY_FLAG(op, FLAG_CURSED) || QUERY_FLAG(op, FLAG_DAMNED))
    {
      new_draw_info_format(NDI_UNIQUE, 0, who,
	"No matter how hard you try, you just can't\nremove %s.",
	      query_name(op));
      return 1;
    }
    CLEAR_FLAG(op, FLAG_APPLIED);
    switch(op->type) {
    case WEAPON:
      if(!change_abil(who,op)) {
#if 0
	new_draw_info_format(NDI_UNIQUE, 0, who,
	    "You can't get rid of the %s.",query_name(op));
        SET_FLAG(op, FLAG_APPLIED);
        return 1;
#endif
      }
#ifdef ALLOW_SKILLS /* 'unready' melee weapons skill if it is current skill */ 
      (void) check_skill_to_apply(who,op);
      if(QUERY_FLAG(who,FLAG_READY_WEAPON)) 
		CLEAR_FLAG(who,FLAG_READY_WEAPON); 
#endif
      sprintf(buf,"You unwield %s.",query_name(op));
      break;
#ifdef ALLOW_SKILLS
    case SKILL:         /* allows objects to impart skills */
      if(!change_abil(who,op)) {
#if 0
        new_draw_info_format(NDI_UNIQUE, 0, who,
            "You can't stop using the %s.",query_name(op));
        SET_FLAG(op, FLAG_APPLIED);
        return 1;
#endif
      }
      if(who->type==PLAYER) {
        new_draw_info_format(NDI_UNIQUE, 0, who,
            "You can no longer use the skill: %s.",skills[op->stats.sp].name);
        who->contr->shoottype=range_none;
        who->contr->last_value = -1;
      }  
      CLEAR_FLAG(who, FLAG_READY_SKILL);
      who->chosen_skill=NULL;
      sprintf(buf,"You stop using the %s.",query_name(op));
      break;
#endif
    case ARMOUR:
    case HELMET:
    case SHIELD:
    case RING:
    case BOOTS:
    case GLOVES:
    case AMULET:
    case GIRDLE:
    case BRACERS:
    case CLOAK:
      if(!change_abil(who,op)) {
#if 0
	new_draw_info_format(NDI_UNIQUE, 0, who,
	    "You can't remove the %s.",query_name(op));
        SET_FLAG(op, FLAG_APPLIED);
        return 1;
#endif
      }
      sprintf(buf,"You unwear %s.",query_name(op));
      break;
    case BOW:
    case WAND:
    case ROD:
    case HORN:
#ifdef ALLOW_SKILLS
      (void) check_skill_to_apply(who,op);
#endif
      sprintf(buf,"You unready %s.",query_name(op));
      if(who->type==PLAYER) {
        who->contr->shoottype = range_none;
        who->contr->last_value = -1;
      }
      break;
    default:
      sprintf(buf,"You unapply %s.",query_name(op));
      break;
    }
    new_draw_info(NDI_UNIQUE, 0,who,buf);
    tmp=merge_ob(op,NULL);
    fix_player(who);
    if(who->type==PLAYER) {
	  if (tmp) {  /* it was merged */
	      esrv_del_item (who->contr, op->count);
	      op = tmp;
	  }
	  esrv_send_item(who, op);
    }

    return 0;
  }
  if (aflags == AP_UNAPPLY) return 0;
  if (QUERY_FLAG(op, FLAG_UNPAID)) {
    new_draw_info(NDI_UNIQUE, 0,who, "You should pay for it first.\n");
    return 0;
  }
  i=0;
  /* This goes through and checks to see if the player already has something
   * of that type applied - if so, unapply it.
   */
  for(tmp=who->inv;tmp!=NULL;tmp=tmp->below)
    if(tmp->type==op->type&&QUERY_FLAG(tmp, FLAG_APPLIED)&&tmp!=op) {
      if(tmp->type==RING&&!i)
        i=1;
      else if(apply_special(who,tmp,0))
        return 0;
    }
  if(op->nrof > 1)
    tmp = get_split_ob(op,op->nrof - 1);
  else
    tmp = NULL;
  switch(op->type) {
  case WEAPON: {

    if(!QUERY_FLAG(who,FLAG_USE_WEAPON)) {
      sprintf(buf,"You can't use %s.",query_name(op));
      if(tmp!=NULL)
        (void) insert_ob_in_ob(tmp,who);
      new_draw_info(NDI_UNIQUE, 0,who,buf);
      return 0;
    }
    if (!check_weapon_power(who, op->last_eat)) {
      new_draw_info(NDI_UNIQUE, 0,who,	
		    "That weapon is too powerful for you to use.");
      new_draw_info(NDI_UNIQUE, 0, who,	"It would consume your soul!.");
      if(tmp!=NULL)
        (void) insert_ob_in_ob(tmp,who);
      return 1;
      }
    if( op->level &&  (strncmp(op->name,who->name,strlen(who->name)))) {
	/* if the weapon does not have the name as the character, can't use it. */
	/*		(Ragnarok's sword attempted to be used by Foo: won't work) */
	new_draw_info(NDI_UNIQUE, 0,who,"The weapon does not recognize you as it's owner.");
        if(tmp!=NULL)
          (void) insert_ob_in_ob(tmp,who);
	return 1;
	}
	

    SET_FLAG(op, FLAG_APPLIED);

#ifdef ALLOW_SKILLS       /*check for melee weapons skill, alter player status.
			   * Note that we need to call this *before* change_abil */
    if(!check_skill_to_apply(who,op)) return 0;
    if(!QUERY_FLAG(who,FLAG_READY_WEAPON))
         SET_FLAG(who, FLAG_READY_WEAPON);
#endif

    if(!change_abil(who,op)) {
#if 0
      CLEAR_FLAG(op, FLAG_APPLIED);
      sprintf(buf,"You receive a jolt when you try to wield %s.",
              query_name(op));
      new_draw_info(NDI_UNIQUE, 0,who,buf);
      if(tmp!=NULL)
        (void) insert_ob_in_ob(tmp,who);
      return 1;
#endif
    }
    sprintf(buf,"You wield %s.",query_name(op));
    break;
  }
  case ARMOUR:
  case HELMET:
  case SHIELD:
  case BOOTS:
  case GLOVES:
  case GIRDLE:
  case BRACERS:
  case CLOAK:
    if(!QUERY_FLAG(who, FLAG_USE_ARMOUR)) {
      sprintf(buf,"You can't use %s.",query_name(op));
      new_draw_info(NDI_UNIQUE, 0,who,buf);
      if(tmp!=NULL)
        (void) insert_ob_in_ob(tmp,who);
      return 0;
    }
  case RING:
  case AMULET:
    SET_FLAG(op, FLAG_APPLIED);
    if(!change_abil(who,op)) {
#if 0
      CLEAR_FLAG(op, FLAG_APPLIED);
      sprintf(buf,"You receive a jolt when you try to wear %s.",
              query_name(op));
      new_draw_info(NDI_UNIQUE, 0,who,buf);
      if(tmp!=NULL)
        (void) insert_ob_in_ob(tmp,who);
      return 1;
#endif
    }
    sprintf(buf,"You wear %s.",query_name(op));
    break;
#ifdef ALLOW_SKILLS /* this part is needed for skill-tools */ 
  case SKILL:
    SET_FLAG(op, FLAG_APPLIED);
    if(!change_abil(who,op)) {
#if 0
      CLEAR_FLAG(op, FLAG_APPLIED);
      sprintf(buf,"You receive a jolt when you try to use %s.",
              query_name(op));
      new_draw_info(NDI_UNIQUE, 0,who,buf);
      if(tmp!=NULL)
        (void) insert_ob_in_ob(tmp,who);
      return 1;
#endif
    }
    sprintf(buf,"You ready %s.",query_name(op));
    who->chosen_skill=op;
    SET_FLAG(who, FLAG_READY_SKILL);
    break;
#endif

/* For new skills/exp system- must have 'missile weapons' skill in order to
 * weild missile weapons! -b.t. 
 */
  case BOW:
#ifdef ALLOW_SKILLS /*check for missile weapons skill, alter player status */ 
    SET_FLAG(op, FLAG_APPLIED);
    if(!check_skill_to_apply(who,op)) return 0; 
#endif
  case WAND:
#ifdef ALLOW_SKILLS
    SET_FLAG(op, FLAG_APPLIED);
    if(!check_skill_to_apply(who,op)) return 0; 
#endif
    sprintf(buf,"You ready %s.",query_name(op));
    break;
  case ROD:
  case HORN:
#ifdef ALLOW_SKILLS
    SET_FLAG(op, FLAG_APPLIED);
    if(!check_skill_to_apply(who,op)) return 0;
#endif
  default:
    sprintf(buf,"You apply %s.",query_name(op));
  }
  if(!QUERY_FLAG(op, FLAG_APPLIED))
      SET_FLAG(op, FLAG_APPLIED);
  new_draw_info(NDI_UNIQUE, 0,who,buf);
  if(tmp!=NULL)
    tmp = insert_ob_in_ob(tmp,who);
  fix_player(who);
  if(op->type != WAND && who->type == PLAYER)
    SET_FLAG(op,FLAG_BEEN_APPLIED);
  if (QUERY_FLAG(op, FLAG_CURSED) || QUERY_FLAG(op, FLAG_DAMNED)) {
    if (who->type == PLAYER) {
      new_draw_info(NDI_UNIQUE, 0,who, "Oops, it feels deadly cold!");
      SET_FLAG(op,FLAG_KNOWN_CURSED);
    }
  }
  if(who->type==PLAYER) {
      /* if multiple objects were applied, update both slots */
      if (tmp)            
	esrv_send_item(who, tmp);
      esrv_send_item(who, op);
  }
  return 0;
}


int auto_apply (object *op) {
  object *tmp = NULL;
  int i;

  switch(op->type) {
  case SHOP_FLOOR:
    if (op->randomitems==NULL) return 0;
    do {
      i=10; /* let's give it 10 tries */
      while((tmp=generate_treasure(op->randomitems,op->stats.exp?
		op->stats.exp:5))==NULL&&--i);
      if(tmp==NULL)
	  return 0;
      if(QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED))
      {
        free_object(tmp);
        tmp = NULL;
      }
    } while(!tmp);

    tmp->x=op->x,tmp->y=op->y;
    SET_FLAG(tmp,FLAG_UNPAID);
    insert_ob_in_map(tmp,op->map);
    CLEAR_FLAG(op,FLAG_AUTO_APPLY);
    identify(tmp);
    break;

  case TREASURE:
    while ((op->stats.hp--)>0)
      create_treasure(op->randomitems, op, op->map?GT_ENVIRONMENT:GT_INVENTORY,
	op->stats.exp ? op->stats.exp : 
	op->map == NULL ?  14: op->map->difficulty,0);

    /* If we generated on object and put it in this object inventory,
     * move it to the parent object as the current object is about
     * to disappear.  An example of this item is the random_* stuff
     * that is put inside other objects.
     */
    if (op->inv) {
	tmp=op->inv;
	remove_ob(tmp);
	if (op->env) insert_ob_in_ob(tmp, op->env);
	else free_object(tmp);
    }
    remove_ob(op);
    free_object(op);
    break;
  }

  return tmp ? 1 : 0;
}

/* fix_auto_apply goes through the entire map (only the first time
 * when an original map is loaded) and performs special actions for
 * certain objects (most initialization of chests and creation of
 * treasures and stuff).  Calls auto_apply if appropriate.
 */

void fix_auto_apply(mapstruct *m) {
  object *tmp,*above=NULL;
  int x,y;

  for(x=0;x<m->mapx;x++)
    for(y=0;y<m->mapy;y++)
      for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=above) {
        above=tmp->above;

	if (tmp->inv) {
	    object *invtmp, *invnext;
	    for (invtmp=tmp->inv; invtmp != NULL; invtmp = invnext) {
		invnext = invtmp->below;
		if(QUERY_FLAG(invtmp,FLAG_AUTO_APPLY))
		    auto_apply(invtmp);
		else if(invtmp->type==TREASURE) {
		    while ((invtmp->stats.hp--)>0)
			create_treasure(invtmp->randomitems, invtmp, GT_INVENTORY,
                            m->difficulty,0);
		}
	    }
	}

	if(QUERY_FLAG(tmp,FLAG_AUTO_APPLY))
          auto_apply(tmp);
        else if(tmp->type==TREASURE) {
	  while ((tmp->stats.hp--)>0)
            create_treasure(tmp->randomitems, tmp, GT_INVENTORY,
                            m->difficulty,0);
	}
	else if(tmp->type==TIMED_GATE) {
	  tmp->speed = 0;
	  update_ob_speed(tmp);
	}
        if(tmp && tmp->arch && tmp->type!=PLAYER && tmp->type!=TREASURE &&
	  tmp->randomitems)
            create_treasure(tmp->randomitems, tmp, GT_INVENTORY,
                            m->difficulty,0);
      }
  for(x=0;x<m->mapx;x++)
    for(y=0;y<m->mapy;y++)
      for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=tmp->above)
	if (tmp->type == TRIGGER || tmp->type == TRIGGER_BUTTON ||
            tmp->type == TRIGGER_PEDESTAL || tmp->type == TRIGGER_ALTAR)
	  check_trigger(tmp);
}

/* eat_special_food() - some food may (temporarily) alter
 * player status. We do it w/ this routine and cast_change_attr().
 * Note the dircection is set to "99"  so that cast_change_attr()
 * will only modify the user's status. We shouldnt be able to 
 * effect others by eating food!
 * -b.t.
 */

void eat_special_food(object *who, object *food) {
  /* matrix for attacks we can gain protection/immunity too from eating 
   * be sure to perserve the ordering between at_type[] and sp_type[]! */
  static int at_type[] = { AT_PHYSICAL, AT_MAGIC, AT_FIRE, AT_ELECTRICITY,
	AT_COLD, AT_DRAIN, AT_POISON, AT_SLOW, AT_PARALYZE, AT_CANCELLATION,
	AT_DEPLETE, AT_FEAR }; 
  static int sp_type[] = { SP_ARMOUR, SP_PROT_MAGIC, SP_PROT_FIRE, 
	SP_PROT_ELEC, SP_PROT_COLD, SP_PROT_DRAIN, SP_PROT_POISON, 
	SP_PROT_SLOW, SP_PROT_PARALYZE, SP_PROT_CANCEL, SP_PROT_DEPLETE, 
	SP_HEROISM }; 

  /* matrix for stats we can increase by eating */
  int i; 
  /* Declare these static so they only get initialzed once.  IT doesn't appear
   * that we are modifying these, so this works out ok.
   */
  static int stat[] = { STR, DEX, CON, CHA }, 
    mod_stat[] = {SP_STRENGTH, SP_DEXTERITY, SP_CONSTITUTION, SP_CHARISMA };
  
  /* check if food modifies stats of the eater */
  for(i=0; i<(sizeof(stat)/sizeof(int)); i++)
     if(get_attr_value(&food->stats, stat[i]))
          cast_change_attr(who,who,99,mod_stat[i]);

  /* check if we can protect the eater */
  if(food->protected)
     for(i=0; i<(sizeof(at_type)/sizeof(int)); i++)
  	if(food->protected&at_type[i]) cast_change_attr(who,who,99,sp_type[i]);

  /* check for hp, sp change */
  if(food->stats.hp!=0) {
     if(QUERY_FLAG(food, FLAG_CURSED)) { 
         new_draw_info(NDI_UNIQUE, 0,who,"Eck!...that was poisonous!");
 	 who->stats.hp -= food->stats.hp;
     } else { 
         if(food->stats.hp>0) 
	    new_draw_info(NDI_UNIQUE, 0,who,"You begin to feel better.");
	 else 
            new_draw_info(NDI_UNIQUE, 0,who,"Eck!...that was poisonous!");
  	 who->stats.hp += food->stats.hp;
     }
  }
  if(food->stats.sp!=0) {
     if(QUERY_FLAG(food, FLAG_CURSED)) { 
         new_draw_info(NDI_UNIQUE, 0,who,"You are drained of mana!");
	 who->stats.sp -= food->stats.sp; 
         if(who->stats.sp<0) who->stats.sp=0;
     } else { 
         new_draw_info(NDI_UNIQUE, 0,who,"You feel a rush of magical energy!");
	 who->stats.sp += food->stats.sp; 
	/* place limit on max sp from food? */
     }
  }
}


/* apply_lighter() - designed primarily to light torches/lanterns/etc.
 * Also burns up burnable material too. First object in the inventory is
 * the selected object to "burn". -b.t.
 */

void apply_lighter(object *who, object *lighter) {
    object *item;
    int count,nrof;
    char item_name[MAX_BUF];

    item=find_marked_object(who);
    if(item) {
        if(lighter->last_eat && lighter->stats.food) { /* lighter gets used up */
        /* Split multiple lighters if they're being used up.  Otherwise	*
	 * one charge from each would be used up.  --DAMN		*/
	  if(lighter->nrof > 1) {
	    object *oneLighter = get_object();
	    copy_object(lighter, oneLighter);
	    lighter->nrof -= 1;
	    oneLighter->nrof = 1;
	    oneLighter->stats.food--;
	    esrv_send_item(who, lighter);
	    oneLighter=insert_ob_in_ob(oneLighter, who);
	    esrv_send_item(who, oneLighter);
	  } else {
	    lighter->stats.food--;
	  }

	} else if(lighter->last_eat) { /* no charges left in lighter */ 
	     new_draw_info_format(NDI_UNIQUE, 0,who,
				  "You attempt to light the %s with a used up %s.",
				  item->name, lighter->name);
	     return;
        }
	/* Perhaps we should split what we are trying to light on fire?
	 * I can't see many times when you would want to light multiple
	 * objects at once.
	 */
	nrof=item->nrof;
	count=item->count;
	/* If the item is destroyed, we don't have a valid pointer to the
	 * name object, so make a copy so the message we print out makes
	 * some sense.
	 */
	strcpy(item_name, item->name);

	save_throw_object(item,AT_FIRE);
	/* Change to check count and not freed, since the object pointer
	 * may have gotten recycled
	 */
	if ((nrof != item->nrof ) || (count != item->count)) {
	    new_draw_info_format(NDI_UNIQUE, 0,who,
		 "You light the %s with the %s.",item_name,lighter->name);
	} else {
	    new_draw_info_format(NDI_UNIQUE, 0,who,
		 "You attempt to light the %s with the %s and fail.",item->name,lighter->name);
	}

   } else /* nothing to light */ 
	new_draw_info(NDI_UNIQUE, 0,who,"You need to mark a lightable object.");

}

/* scroll_failure()- hacked directly from spell_failure */ 

void scroll_failure(object *op, int failure, int power)
{ 
  if(abs(failure/4)>power) power=abs(failure/4); /* set minimum effect */

  if(failure<= -1&&failure > -15) /* wonder */
    {
     new_draw_info(NDI_UNIQUE, 0,op,"Your spell warps!.");
     cast_cone(op,op,0,10,SP_WOW,spellarch[SP_WOW],0);
    }

  else if (failure <= -15&&failure > -35) /* drain mana */
    {
     new_draw_info(NDI_UNIQUE, 0,op,"Your mana is drained!.");
     op->stats.sp -= RANDOM()%power;
     if(op->stats.sp<0) op->stats.sp = 0;
    }

  /* even nastier effects continue...*/ 
#ifdef SPELL_FAILURE_EFFECTS 
  else if (failure <= -35&&failure > -60) /* confusion */
   {
    new_draw_info(NDI_UNIQUE, 0,op,"The magic recoils on you!");
    confuse_player(op,op,power);
   }

  else if (failure <= -60&&failure> -70) /* paralysis */
  {
    new_draw_info(NDI_UNIQUE, 0,op,"The magic recoils and paralyzes you!");
    paralyze_player(op,op,power);
  }

  else if (failure <= -70&&failure> -80) /* blind */
  {
    new_draw_info(NDI_UNIQUE, 0,op,"The magic recoils on you!");
    blind_player(op,op,power);
  }

  else if (failure <= -80) /* blast the immediate area */
  { 
    new_draw_info(NDI_UNIQUE, 0,op,"You unlease uncontrolled mana!");
    cast_mana_storm(op,power);
  }
#endif
}

