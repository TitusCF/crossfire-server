/*
 * static char *rcsid_apply_c =
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

#include <global.h>
#include <living.h>
#include <spells.h>
#include <skills.h>


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

/*
 * Return value: 1 if money was destroyed, 0 if not.
 */
static int apply_id_altar (object *money, object *altar, object *pl)
{
    object *id, *marked;
    int success=0;

    if (pl == NULL || pl->type != PLAYER)
      return 0;

    /* Check for MONEY type is a special hack - it prevents 'nothing needs
     * identifying' from being printed out more than it needs to be.
     */
    if ( ! check_altar_sacrifice (altar, money) || money->type != MONEY)
      return 0;

    marked = find_marked_object (pl);
    /* if the player has a marked item, identify that if it needs to be
     * identified.  IF it doesn't, then go through the player inventory.
     */
    if (marked && ! QUERY_FLAG (marked, FLAG_IDENTIFIED)
        && need_identify (marked))
    {
	if (operate_altar (altar, &money)) {
	    identify (marked);
	    new_draw_info_format(NDI_UNIQUE, 0, pl,
		"You have %s.", long_desc(marked));
            if (marked->msg) {
	        new_draw_info(NDI_UNIQUE, 0,pl, "The item has a story:");
	        new_draw_info(NDI_UNIQUE, 0,pl, marked->msg);
	    }
	    return money == NULL;
	} 
    }

    for (id=pl->inv; id; id=id->below) {
	if (!QUERY_FLAG(id, FLAG_IDENTIFIED) && !id->invisible && 
	    need_identify(id)) {
		if (operate_altar(altar,&money)) {
		    identify(id);
		    new_draw_info_format(NDI_UNIQUE, 0, pl,
			"You have %s.", long_desc(id));
	            if (id->msg) {
		        new_draw_info(NDI_UNIQUE, 0,pl, "The item has a story:");
		        new_draw_info(NDI_UNIQUE, 0,pl, id->msg);
		    }
		    success=1;
		    /* If no more money, might as well quit now */
		    if (money == NULL || ! check_altar_sacrifice (altar,money))
			 break;
		}
		else {
		    LOG(llevError,"check_id_altar:  Couldn't do sacrifice when we should have been able to\n");
		    break;
		}
	}
    }
    if (!success) new_draw_info(NDI_UNIQUE, 0,pl,"You have nothing that needs identifying");
    return money == NULL;
}

int apply_potion(object *op, object *tmp)
{
    int got_one=0,i;
    object *force;

#if 0
   /* we now need this to happen */
    if(op->type!=PLAYER)
      return 0; /* This might change */
#endif

    if(op->type==PLAYER) { 
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

    /* Deal with protection potions */
    force=NULL;
    for (i=0; i<NROFATTACKS; i++) {
	if (tmp->resist[i]) {
	    if (!force) force=get_archetype("force");
	    memcpy(force->resist, tmp->resist, sizeof(tmp->resist));
	    force->type=POTION_EFFECT;
	    break;  /* Only need to find one protection since we copy entire batch */
	}
    }
    /* This is a protection potion */
    if (force) {
	/* cursed items last longer */
	if(QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) {
	  force->stats.food*=10;
	  for (i=0; i<NROFATTACKS; i++)
	    if (force->resist[i] > 0)
	      force->resist[i] = -force->resist[i];  /* prot => vuln */ 
	}
	force->speed_left= -1;
	force = insert_ob_in_ob(force,op);
	CLEAR_FLAG(tmp, FLAG_APPLIED);
	SET_FLAG(force,FLAG_APPLIED);
	change_abil(op,force);
	decrease_ob(tmp);
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
    int sacrifice_count,i;
    char buf[MAX_BUF];

    if (weapon->level!=0) {
      new_draw_info(NDI_UNIQUE,0,op,"Weapon already prepared.");
      return 0;
    }
    for (i=0; i<NROFATTACKS; i++)
	if (weapon->resist[i]) break;

    /* If we break out, i will be less than nrofattacks, preventing
     * improvement of items that already have protections.
     */
    if (i<NROFATTACKS || 
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
 * the users level or 90)
 * Modified by MSW for partial resistance.  Only support
 * changing of physical area right now.
 */
 
int improve_armour(object *op, object *improver, object *armour)
{
    int new_armour;
 
    new_armour = armour->resist[ATNR_PHYSICAL] + armour->resist[ATNR_PHYSICAL]/25 + op->level/20 + 1;
    if (new_armour > 90)
        new_armour = 90;

    if (armour->magic >= (op->level / 10 + 1)
        || new_armour > op->level)
    {
        new_draw_info(NDI_UNIQUE, 0,op,"You are not yet powerfull enough");
        new_draw_info(NDI_UNIQUE, 0,op,"to improve this armour.");
        return 0;
    }

    if (new_armour > armour->resist[ATNR_PHYSICAL]) {
	armour->resist[ATNR_PHYSICAL] = new_armour;
	armour->weight += armour->weight * 0.05;
    } else {
        new_draw_info(NDI_UNIQUE, 0,op,"The armour value of this equipment");
        new_draw_info(NDI_UNIQUE, 0,op,"cannot be further improved.");
    } 
    armour->magic++;
    if (op->type == PLAYER) {
        esrv_send_item(op, armour);
        if(QUERY_FLAG(armour, FLAG_APPLIED))
            fix_player(op);
    }
    decrease_ob(improver);
    return 1;
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
  insert_ob_in_map(item,converter->map,converter,0);
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
	  tmp = find_key(op, op, sack);
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
      tmp=find_key(op, op, sack);
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
        sprintf (buf, "%s the %s\n", op->name, op->contr->title);
    else
        sprintf (buf, "%s\n", op->name);
    strncat (buf2, "                    ",  20 - strlen (buf) / 2);
    strcat (buf2, buf);
    if (op->type == PLAYER)
        sprintf (buf, "who was in level %d when killed\n", op->level);
    else
        sprintf (buf, "who was in level %d when died.\n\n", op->level);
    strncat (buf2, "                    ",  20 - strlen (buf) / 2);
    strcat (buf2, buf);
    if (op->type == PLAYER) {
        sprintf (buf, "by %s.\n\n", op->contr->killer);
        strncat (buf2, "                    ",  21 - strlen (buf) / 2);
        strcat (buf2, buf);
    }
    strftime (buf, MAX_BUF, "%b %d %Y\n", localtime (&now));
    strncat (buf2, "                    ",  20 - strlen (buf) / 2);
    strcat (buf2, buf);
    return buf2;
}


/*
 * Returns true if sacrifice was accepted.
 */
static int apply_altar (object *altar, object *sacrifice, object *originator)
{
  /* Only players can make sacrifices on spell casting altars. */
  if (altar->stats.sp && ( ! originator || originator->type != PLAYER))
    return 0;
  if (operate_altar (altar, &sacrifice))
  {
    /* Simple check.  Unfortunately, it means you can't cast magic bullet
     * with an altar.  We call it a Potion - altars are stationary - it
     * is up to map designers to use them properly.
     */
    if (altar->stats.sp)
    {
      new_draw_info_format (NDI_BLACK, 0, originator, "The altar casts %s.",
                            spells[altar->stats.sp].name);
      cast_spell (originator, altar, 0, altar->stats.sp, 0, spellPotion, NULL);
      /* If it is connected, push the button.  Fixes some problems with
       * old maps.
       */
      push_button (altar);
    } else {
      altar->value = 1;  /* works only once */
      push_button (altar);
    }
    return sacrifice == NULL;
  } else {
    return 0;
  }
}


/*
 * Returns 1 if 'op' was destroyed, 0 if not.
 * Largely re-written to not use nearly as many gotos, plus
 * some of this code just looked plain out of date.
 * MSW 2001-08-29
 */
static int apply_shop_mat (object *shop_mat, object *op)
{
    int rv = 0;
    object *tmp;

    SET_FLAG (op,FLAG_NO_APPLY);   /* prevent loops */

    if (op->type != PLAYER) {
	if (QUERY_FLAG(op, FLAG_UNPAID)) {

	    /* Somebody dropped an unpaid item, just move to an adjacent place. */
	    int i = find_free_spot (op->arch, op->map, op->x, op->y, 1, 9);
	    if (i != -1) {
		rv = transfer_ob (op, op->x + freearr_x[i], op->y + freearr_y[i], 0,
                       shop_mat);
	    }
	}
	/* Removed code that checked for multipart objects - it appears that
	 * the teleport function should be able to handle this just fine.
	 */
	rv = teleport (shop_mat, SHOP_MAT, op);
    }
    /* immediate block below is only used for players */
    else if (get_payment (op)) {
	rv = teleport (shop_mat, SHOP_MAT, op);
	if (shop_mat->msg) {
	    new_draw_info (NDI_UNIQUE, 0, op, shop_mat->msg);
	}
	/* This check below is a bit simplistic - generally it should be correct,
	 * but there is never a guarantee that the bottom space on the map is
	 * actually the shop floor.
	 */
	else if ( ! rv && (tmp = get_map_ob (op->map, op->x, op->y)) != NULL
		   && tmp->type != SHOP_FLOOR) {
	    new_draw_info (NDI_UNIQUE, 0, op, "Thank you for visiting our shop.");
	}
    }
    else {
	/* if we get here, a player tried to leave a shop but was not able
	 * to afford the items he has.  We try to move the player so that
	 * they are not on the mat anymore
	 */

	int i = find_free_spot (op->arch, op->map, op->x, op->y, 1, 9);
	if(i == -1) {
	    LOG (llevError, "Internal shop-mat problem.\n");
	} else {
	    remove_ob (op);
	    op->x += freearr_x[i];
	    op->y += freearr_y[i];
	    rv = insert_ob_in_map (op, op->map, shop_mat,0) == NULL;
	}
    }

  CLEAR_FLAG (op, FLAG_NO_APPLY);
  return rv;
}

static void apply_sign (object *op, object *sign)
{
    if (sign->msg == NULL) {
        new_draw_info (NDI_UNIQUE, 0, op, "Nothing is written on it.");
        return;
    }

    if (sign->stats.food) {
      if (sign->last_eat >= sign->stats.food) {
        if (!QUERY_FLAG (sign, FLAG_WALK_ON) && !QUERY_FLAG (sign, FLAG_FLY_ON))
          new_draw_info (NDI_UNIQUE, 0, op, "You cannot read it anymore.");
        return;
      }
      sign->last_eat++;
    }

    /* Sign or magic mouth?  Do we need to see it, or does it talk to us?
     * No way to know for sure.
     *
     * This check fails for signs with FLAG_WALK_ON/FLAG_FLY_ON.  Checking
     * for FLAG_INVISIBLE instead of FLAG_WALK_ON/FLAG_FLY_ON would fail
     * for magic mouths that have been made visible.
     */
    if (QUERY_FLAG (op, FLAG_BLIND) && ! QUERY_FLAG (op, FLAG_WIZ)
        && ! QUERY_FLAG (sign, FLAG_WALK_ON)
        && ! QUERY_FLAG (sign, FLAG_FLY_ON))
    {
        new_draw_info (NDI_UNIQUE, 0, op,
                       "You are unable to read while blind.");
        return;
    }

    new_draw_info (NDI_UNIQUE | NDI_NAVY, 0, op, sign->msg);
}


/* 'victim' moves onto 'trap' (trap has FLAG_WALK_ON or FLAG_FLY_ON set) or
 * 'victim' leaves 'trap' (trap has FLAG_WALK_OFF or FLAG_FLY_OFF) set.
 *
 * originator: Player, monster or other object that caused 'victim' to move
 * onto 'trap'.  Will receive messages caused by this action.  May be NULL.
 * However, some types of traps require an originator to function.
 */
void move_apply (object *trap, object *victim, object *originator)
{
  static int recursion_depth = 0;

  /* move_apply() is the most likely candidate for causing unwanted and
   * possibly unlimited recursion. */
  /* The following was changed because it was causing perfeclty correct
     maps to fail.  1)  it's not an error to recurse:
     rune detonates, summoning monster.  monster lands on nearby rune.
     nearby rune detonates.  This sort of recursion is expected and
     proper.  This code was causing needless crashes. */
  if (recursion_depth >= 500) { 
    LOG (llevDebug, "WARNING: move_apply(): aborting recursion "
         "[trap arch %s, name %s; victim arch %s, name %s]\n",
         trap->arch->name, trap->name, victim->arch->name, victim->name);
    return;
  }
  recursion_depth++;

  if (trap->head) trap=trap->head;
  switch (trap->type)
  {
  case PLAYERMOVER:
    if (trap->attacktype && (trap->level || victim->type!=PLAYER)) {
	if (!trap->stats.maxsp) trap->stats.maxsp=2.0;
	/* Is this correct?  From the docs, it doesn't look like it
	 * should be divided by trap->speed
	 */
	victim->speed_left = -FABS(trap->stats.maxsp*victim->speed/trap->speed);
	/* Just put in some sanity check.  I think there is a bug in the
	 * above with some objects have zero speed, and thus the player
	 * getting permanently paralyzed.
	 */
	if (victim->speed_left<-50.0) victim->speed_left=-50.0;
/*	fprintf(stderr,"apply, playermove, player speed_left=%f\n", victim->speed_left);*/
    }
    goto leave;

  case SPINNER:
    if(victim->direction) {
      victim->direction=absdir(victim->direction-trap->stats.sp);
      update_turn_face(victim);
    }
    goto leave;

  case DIRECTOR:
    if(victim->direction) {
      victim->direction=trap->stats.sp;
      update_turn_face(victim);
    }
    goto leave;

  case BUTTON:
  case PEDESTAL:
    update_button(trap);
    goto leave;

  case ALTAR:
    /* sacrifice victim on trap */
    apply_altar (trap, victim, originator);
    goto leave;

  case MMISSILE:
    if (QUERY_FLAG (victim, FLAG_ALIVE)) {
      tag_t trap_tag = trap->count;
      hit_player (victim, trap->stats.dam, trap, AT_MAGIC);
      if ( ! was_destroyed (trap, trap_tag)) {
          remove_ob (trap);
          free_object (trap);
      }
    }
    goto leave;

  case THROWN_OBJ:
    if (trap->inv == NULL)
      goto leave;
    /* fallthrough */
  case ARROW:
    if (QUERY_FLAG (victim, FLAG_ALIVE) && trap->speed)
      hit_with_arrow (trap, victim);
    goto leave;

  case CANCELLATION:
  case BALL_LIGHTNING:
    if (QUERY_FLAG (victim, FLAG_ALIVE))
      hit_player (victim, trap->stats.dam, trap, trap->attacktype);
    else if (victim->material)
      save_throw_object (victim, trap->attacktype, trap);
    goto leave;

  case CONE:
    if(QUERY_FLAG(victim, FLAG_ALIVE)&&trap->speed) {
      uint32 attacktype = trap->attacktype & ~AT_COUNTERSPELL;
      if (attacktype)
        hit_player(victim,trap->stats.dam,trap,attacktype);
    }
    goto leave;

  case FBULLET:
  case BULLET:
    if (QUERY_FLAG (victim, FLAG_NO_PASS)
        || QUERY_FLAG (victim, FLAG_ALIVE))
      check_fired_arch (trap);
    goto leave;

  case TRAPDOOR:
    {
      int max, sound_was_played;
      object *ab;
      if(!trap->value) {
        int tot;
        for(ab=trap->above,tot=0;ab!=NULL;ab=ab->above)
          if(!QUERY_FLAG(ab,FLAG_FLYING))
            tot += (ab->nrof ? ab->nrof : 1) * ab->weight + ab->carrying;
        if(!(trap->value=(tot>trap->weight)?1:0))
          goto leave;
	SET_ANIMATION(trap, trap->value);
        update_object(trap,UP_OBJ_FACE);
      }
      for (ab = trap->above, max=100, sound_was_played = 0;
           --max && ab && ! QUERY_FLAG (ab, FLAG_FLYING); ab=ab->above)
      {
        if ( ! sound_was_played) {
          play_sound_map(trap->map, trap->x, trap->y, SOUND_FALL_HOLE);
          sound_was_played = 1;
        }
        new_draw_info(NDI_UNIQUE, 0,ab,"You fall into a trapdoor!");
        transfer_ob(ab,(int)EXIT_X(trap),(int)EXIT_Y(trap),0,ab);
      }
      goto leave;
    }

  case CONVERTER:
    convert_item (victim, trap);
    goto leave;

  case TRIGGER_BUTTON:
  case TRIGGER_PEDESTAL:
  case TRIGGER_ALTAR:
  /* GROS: Handle for script_trigger event */
    if (trap->script_trigger != NULL)
    {
      guile_call_event(victim, trap, NULL, 0, NULL,0,0,trap->script_trigger, SCRIPT_FIX_ALL);
    }
    else
    {
      if (trap->script_str_trigger != NULL)
      {
        guile_call_event_str(victim, trap, NULL, 0, NULL,0,0,trap->script_str_trigger, SCRIPT_FIX_ALL);
      }
      else
      {
        check_trigger (trap, victim);
      }
    };
    goto leave;

  case DEEP_SWAMP:
    walk_on_deep_swamp (trap, victim);
    goto leave;

  case CHECK_INV:
  /* GROS: Handle for script_trigger event */
    if (trap->script_trigger != NULL)
    {
      guile_call_event(victim, trap, NULL, 0, NULL,0,0,trap->script_trigger, SCRIPT_FIX_ALL);
    }
    else
    {
      if (trap->script_str_trigger != NULL)
      {
        guile_call_event_str(victim, trap, NULL, 0, NULL,0,0,trap->script_str_trigger, SCRIPT_FIX_ALL);
      }
      else
      {
        check_inv (victim, trap);
      }
    };
    goto leave;

  case HOLE:
    /* Hole not open? */
    if(trap->stats.wc > 0)
      goto leave;
    /* Is this a multipart monster and not the head?  If so, return.
     * Processing will happen if the head runs into the pit
     */
    if (victim->head)
      goto leave;
    play_sound_map (victim->map, victim->x, victim->y, SOUND_FALL_HOLE);
    new_draw_info (NDI_UNIQUE, 0, victim, "You fall through the hole!\n");
    transfer_ob (victim, EXIT_X (trap), EXIT_Y (trap), 1, victim);
    goto leave;

  case EXIT:
    if (victim->type == PLAYER && EXIT_PATH (trap)) {
	/* Basically, don't show exits leading to random maps the
	 * players output.
	 */
	if (trap->msg && strncmp(EXIT_PATH(trap),"/!",2) && strncmp(EXIT_PATH(trap), "/random/", 8))
	    new_draw_info (NDI_NAVY, 0, victim, trap->msg);
      /* GROS: Handle for script_trigger event */
      if (trap->script_trigger != NULL)
      {
        guile_call_event(victim, trap, NULL, 0, NULL,0,0,trap->script_trigger, SCRIPT_FIX_ALL);
      }
      else
      {
        if (trap->script_str_trigger != NULL)
        {
          guile_call_event_str(victim, trap, NULL, 0, NULL,0,0,trap->script_str_trigger, SCRIPT_FIX_ALL);
        }
      };
      enter_exit (victim, trap);
    }
    goto leave;

  case ENCOUNTER:
    /* may be some leftovers on this */
    goto leave;

  case SHOP_MAT:
    apply_shop_mat (trap, victim);
    goto leave;

  /* Drop a certain amount of gold, and have one item identified */
  case IDENTIFY_ALTAR:
    apply_id_altar (victim, trap, originator);
    goto leave;

  case SIGN:
    apply_sign (victim, trap);
    goto leave;

  case CONTAINER:
    if (victim->type==PLAYER)
      (void) esrv_apply_container (victim, trap);
    else
      (void) apply_container (victim, trap);
    goto leave;

  case RUNE:
    if (trap->level && QUERY_FLAG (victim, FLAG_ALIVE))
    {
      /* GROS: Handle for script_trigger event */
      if (trap->script_trigger != NULL)
      {
        guile_call_event(victim, trap, NULL, 0, NULL,0,0,trap->script_trigger, SCRIPT_FIX_ALL);
      }
      else
      {
        if (trap->script_str_trigger != NULL)
        {
          guile_call_event_str(victim, trap, NULL, 0, NULL,0,0,trap->script_str_trigger, SCRIPT_FIX_ALL);
        }
        else
        {
          spring_trap(trap, victim);
        }
      };
    };
    goto leave;

  default:
    LOG (llevDebug, "name %s, arch %s, type %d with fly/walk on/off not "
         "handled in move_apply()\n", trap->name, trap->arch->name,
         trap->type);
    goto leave;
  }

 leave:
  recursion_depth--;
}


static void apply_book (object *op, object *tmp)
{
    int lev_diff;

    if(QUERY_FLAG(op, FLAG_BLIND)&&!QUERY_FLAG(op,FLAG_WIZ)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You are unable to read while blind.");
      return;
    }
    if(tmp->msg==NULL) {
      new_draw_info_format(NDI_UNIQUE, 0, op, 
	"You open the %s and find it empty.", tmp->name);
      return;
    }

    /* need a literacy skill to read stuff! */
    if ( ! change_skill(op,SK_LITERACY)) {
      new_draw_info(NDI_UNIQUE, 0,op,
	"You are unable to decipher the strange symbols.");
      return;
    }
    lev_diff = tmp->level - (SK_level(op) + 5);
    if ( ! QUERY_FLAG (op, FLAG_WIZ) && lev_diff > 0)
    {
      if (lev_diff < 2)
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
      return;
    }

    new_draw_info_format (NDI_UNIQUE, 0, op,
                          "You open the %s and start reading.", tmp->name);

    /* GROS: Handling for reading scripted books */
    if (tmp->script_apply != NULL)
    {
      guile_call_event(op,tmp, NULL, 0, NULL,0,0,tmp->script_apply, SCRIPT_FIX_ALL);
    }
    else
    {
      if (tmp->script_str_apply != NULL)
      {
        guile_call_event_str(op,tmp, NULL, 0, NULL,0,0,tmp->script_str_apply, SCRIPT_FIX_ALL);
      }
      else
        new_draw_info(NDI_UNIQUE | NDI_NAVY, 0, op, tmp->msg);
    };

    /* gain xp from reading */
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
}


static void apply_skillscroll (object *op, object *tmp)
{
  switch ((int) learn_skill (op, tmp))
  {
    case 0:
      new_draw_info(NDI_UNIQUE, 0,op,"You already possess the knowledge ");
      new_draw_info_format(NDI_UNIQUE, 0,op,"held within the %s.\n",query_name(tmp));
      return;

    case 1:
      new_draw_info_format(NDI_UNIQUE, 0,op,"You succeed in learning %s",
      skills[tmp->stats.sp].name);
      new_draw_info_format(NDI_UNIQUE, 0, op,
          "Type 'bind ready_skill %s",skills[tmp->stats.sp].name);
      new_draw_info(NDI_UNIQUE, 0,op,"to store the skill in a key.");
      fix_player(op); /* to immediately link new skill to exp object */
      decrease_ob(tmp);
      return;

    default:
      new_draw_info_format(NDI_UNIQUE,0,op,
          "You fail to learn the knowledge of the %s.\n",query_name(tmp));
      decrease_ob(tmp);
      return;
    }
}


/* Special prayers are granted by gods and lost when the follower decides
 * to pray to a different gods.  'Force' objects keep track of which
 * prayers are special.
 */

static object *find_special_prayer_mark (object *op, int spell)
{
    object *tmp;

    for (tmp = op->inv; tmp; tmp = tmp->below)
        if (tmp->type == FORCE && tmp->slaying
            && strcmp (tmp->slaying, "special prayer") == 0
            && tmp->stats.sp == spell)
            return tmp;
    return 0;
}

static void insert_special_prayer_mark (object *op, int spell)
{
    object *force = get_archetype ("force");
    force->speed = 0;
    update_ob_speed (force);
    force->slaying = add_string ("special prayer");
    force->stats.sp = spell;
    insert_ob_in_ob (force, op);
}

extern void do_learn_spell (object *op, int spell, int special_prayer)
{
    object *tmp = find_special_prayer_mark (op, spell);

    if (op->type != PLAYER) {
        LOG (llevError, "BUG: do_forget_spell(): not a player\n");
        return;
    }

    /* Upgrade special prayers to normal prayers */
    if (check_spell_known (op, spell)) {
        if (special_prayer || ! tmp) {
            LOG (llevError, "BUG: do_learn_spell(): spell already known, but "
             "can't upgrade it\n");
            return;
        }
        remove_ob (tmp);
        free_object (tmp);
        return;
    }

    /* Learn new spell/prayer */
    if (tmp) {
        LOG (llevError, "BUG: do_learn_spell(): spell unknown, but special "
             "prayer mark present\n");
        remove_ob (tmp);
        free_object (tmp);
    }
    play_sound_player_only (op->contr, SOUND_LEARN_SPELL, 0, 0);
    op->contr->known_spells[op->contr->nrofknownspells++] = spell;
    if (op->contr->nrofknownspells == 1)
        op->contr->chosen_spell = spell;
    
    /* For godgiven spells the player gets a reminder-mark inserted,
       that this spell must be removed on changing cults! */
    if (special_prayer)
      insert_special_prayer_mark (op, spell);

    new_draw_info_format (NDI_UNIQUE, 0, op, 
            "Type 'bind cast %s", spells[spell].name);
    new_draw_info (NDI_UNIQUE, 0, op, "to store the spell in a key.");
}

extern void do_forget_spell (object *op, int spell)
{
    object *tmp;
    int i;

    if (op->type != PLAYER) {
        LOG (llevError, "BUG: do_forget_spell(): not a player\n");
        return;
    }
    if ( ! check_spell_known (op, spell)) {
        LOG (llevError, "BUG: do_forget_spell(): spell not known\n");
        return;
    }
    
    new_draw_info_format (NDI_UNIQUE|NDI_NAVY, 0, op,
			  "You lose knowledge of %s.", spells[spell].name);

    tmp = find_special_prayer_mark (op, spell);
    if (tmp) {
        remove_ob (tmp);
        free_object (tmp);
    }

    for (i = 0; i < op->contr->nrofknownspells; i++)
    {
        if (op->contr->known_spells[i] == spell) {
            op->contr->known_spells[i] =
                    op->contr->known_spells[--op->contr->nrofknownspells];
            return;
        }
    }
    LOG (llevError, "BUG: do_forget_spell(): couldn't find spell\n");
}

static void apply_spellbook (object *op, object *tmp)
{
    if(QUERY_FLAG(op, FLAG_BLIND)&&!QUERY_FLAG(op,FLAG_WIZ)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You are unable to read while blind.");
      return;
    }

    /* artifact_spellbooks have 'slaying' field point to a spell name,
    ** instead of having their spell stored in stats.sp.  We should update
    ** stats->sp to point to that spell */
 
    if(tmp->slaying != NULL) {
       if((tmp->stats.sp = look_up_spell_name(tmp->slaying)) <0 ){
	  tmp->stats.sp = -1; 
	  new_draw_info_format(NDI_UNIQUE, 0, op,
		"The book's formula for %s is incomplete", tmp->slaying);
	  return;
       }
       /* now clear tmp->slaying since we no longer need it */
       free_string(tmp->slaying);
       tmp->slaying=NULL;
    }

    /* need a literacy skill to learn spells. Also, having a literacy level
     * lower than the spell will make learning the spell more difficult */
    if ( ! change_skill(op,SK_LITERACY)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You can't read! Your attempt fails.");
      return;
    }
    if(tmp->stats.sp < 0 || tmp->stats.sp > NROFREALSPELLS
        || spells[tmp->stats.sp].level>(SK_level(op)+10)) {
      new_draw_info(NDI_UNIQUE, 0,op,"You are unable to decipher the strange symbols.");
      return;
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

    if (check_spell_known (op, tmp->stats.sp) && (tmp->stats.Wis ||
        find_special_prayer_mark (op, tmp->stats.sp) == NULL))
    {
	new_draw_info(NDI_UNIQUE, 0,op,"You already know that spell.\n");
	return;
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
      /* this needs to be a - number [garbled] */
      scroll_failure(op, 0 - random_roll(0, spells[tmp->stats.sp].level, op, PREFER_LOW), spells[tmp->stats.sp].sp);
    } else if(QUERY_FLAG(tmp,FLAG_STARTEQUIP) || random_roll(0, 149, op, PREFER_LOW)-(2*SK_level(op)) <
	learn_spell[spells[tmp->stats.sp].cleric ? op->stats.Wis : op->stats.Int]) {
      new_draw_info(NDI_UNIQUE, 0,op,"You succeed in learning the spell!");
      do_learn_spell (op, tmp->stats.sp, 0);

      /* xp gain to literacy for spell learning */
      if ( ! QUERY_FLAG (tmp, FLAG_STARTEQUIP))
        add_exp(op,calc_skill_exp(op,tmp));
    } else {
      play_sound_player_only(op->contr, SOUND_FUMBLE_SPELL,0,0);
      new_draw_info(NDI_UNIQUE, 0,op,"You fail to learn the spell.\n");
    }
    decrease_ob(tmp);
}


static void apply_scroll (object *op, object *tmp)
{
    int scroll_spell=tmp->stats.sp, old_spell=0;
    rangetype old_shoot=range_none;

    if(QUERY_FLAG(op, FLAG_BLIND)&&!QUERY_FLAG(op,FLAG_WIZ)) {
      new_draw_info(NDI_UNIQUE, 0,op, "You are unable to read while blind.");
      return;
    }

    if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED)) 
      identify(tmp);

    if( scroll_spell < 0 || scroll_spell >= NROFREALSPELLS) {
        new_draw_info (NDI_UNIQUE, 0, op,
                       "The scroll just doesn't make sense!");
        return;
    }

    if(op->type==PLAYER) {
	/* players need a literacy skill to read stuff! */
	int exp_gain=0;

        if ( ! change_skill(op,SK_LITERACY)) {
          new_draw_info(NDI_UNIQUE, 0,op,
            "You are unable to decipher the strange symbols.");
          return;
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
        old_shoot= op->contr->shoottype;
        old_spell = op->contr->chosen_spell;
        op->contr->shoottype=range_scroll;
        op->contr->chosen_spell = scroll_spell;
    }

    new_draw_info_format(NDI_BLACK, 0, op,
        "The scroll of %s turns to dust.", spells[tmp->stats.sp].name);
    {
      char buf[MAX_BUF];

      sprintf(buf, "%s reads a scroll of %s.",op->name,spells[tmp->stats.sp].name);
      new_info_map(NDI_ORANGE, op->map, buf);
    }

    cast_spell(op,tmp,0,scroll_spell,0,spellScroll,NULL);
    decrease_ob(tmp);
    if(op->type==PLAYER) {
      if(op->contr->golem==NULL) {
        op->contr->shoottype=old_shoot;
	op->contr->chosen_spell = old_spell;
      }
    }
}


static void apply_treasure (object *op, object *tmp)
{
    object *treas;
    tag_t tmp_tag = tmp->count, op_tag = op->count;

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
      return;
    }
    do {
      remove_ob(treas);
      draw_find(op,treas);
      treas->x=op->x,treas->y=op->y;
      treas = insert_ob_in_map (treas, op->map, op,0);
      if (treas && treas->type == RUNE && treas->level
          && QUERY_FLAG (op, FLAG_ALIVE))
        spring_trap (treas, op);
      if (was_destroyed (op, op_tag) || was_destroyed (tmp, tmp_tag))
        break;
    } while ((treas=tmp->inv)!=NULL);
    
    if ( ! was_destroyed (tmp, tmp_tag) && tmp->inv == NULL)
      decrease_ob (tmp);

#if 0
    /* Can't rely on insert_ob_in_map to do any restacking,
     * so lets disable this.
     */
    if ( ! was_destroyed (op, op_tag)) {
      /* Done to re-stack map with player on top? */
      SET_FLAG (op, FLAG_NO_APPLY);
      remove_ob (op);
      insert_ob_in_map (op, op->map, NULL,0);
      CLEAR_FLAG (op, FLAG_NO_APPLY);
    }
#endif
}


static void apply_food (object *op, object *tmp)
{
    int capacity_remaining;

    if(op->type!=PLAYER)
      op->stats.hp=op->stats.maxhp;
    else {
      if(op->stats.food+tmp->stats.food>999) {
	if(tmp->type==FOOD || tmp->type==FLESH)
	  new_draw_info(NDI_UNIQUE, 0,op,"You feel full, but what a waste of food!");
	else
	  new_draw_info(NDI_UNIQUE, 0,op,"Most of the drink goes down your face not your throat!");
      }

      if(!QUERY_FLAG(tmp, FLAG_CURSED)) {
        char buf[MAX_BUF];

        if(tmp->type==DRINK)
            sprintf(buf,"Ahhh...that %s tasted good.",tmp->name);
        else 
            sprintf(buf,"The %s tasted %s",tmp->name,
		tmp->type==FLESH?"terrible!":"good.");

        new_draw_info(NDI_UNIQUE, 0,op,buf);
	capacity_remaining = 999 - op->stats.food;
        op->stats.food+=tmp->stats.food;
	if(capacity_remaining < tmp->stats.food)
	    op->stats.hp += capacity_remaining / 50;
	else
	  op->stats.hp+=tmp->stats.food/50;
        if(op->stats.hp>op->stats.maxhp)
          op->stats.hp=op->stats.maxhp;
	if (op->stats.food > 999)
	    op->stats.food = 999;
      }
      /* special food hack -b.t. */
      if(tmp->title || QUERY_FLAG(tmp,FLAG_CURSED)) eat_special_food(op,tmp);
    }
    decrease_ob(tmp);
}


static void apply_savebed (object *player)
{
    if(!player->contr->name_changed||!player->stats.exp) {
      new_draw_info(NDI_UNIQUE, 0,player,"You don't deserve to save your character yet.");
      return;
    }
    if(QUERY_FLAG(player,FLAG_WAS_WIZ)) {
      new_draw_info(NDI_UNIQUE, 0,player,"Since you have cheated you can't save.");
      return;
    }
    remove_ob(player);
    player->direction=0;
    player->contr->count_left=0;
    new_draw_info_format(NDI_UNIQUE | NDI_ALL, 5, player,
	"%s leaves the game.",player->name);
    
    /* update respawn position */
    strcpy(player->contr->savebed_map, player->map->path);
    player->contr->bed_x = player->x;
    player->contr->bed_y = player->y;
    
    strcpy(player->contr->killer,"left");
    check_score(player); /* Always check score */
    (void)save_player(player,0);
    play_again(player);
    player->map->players--;
#if MAP_MAXTIMEOUT 
    MAP_SWAP_TIME(player->map) = MAP_TIMEOUT(player->map);
#endif
}


static void apply_armour_improver (object *op, object *tmp)
{
    object *armor;

    if (blocks_magic(op->map,op->x,op->y)) {
        new_draw_info(NDI_UNIQUE, 0,op,"Something blocks the magic of the scroll.");
        return;
    }
    armor=find_marked_object(op);
    if ( ! armor) {
      new_draw_info(NDI_UNIQUE, 0, op, "You need to mark an armor object.");
      return;
    }
    if (armor->type != ARMOUR
	&& armor->type != CLOAK
	&& armor->type != BOOTS && armor->type != GLOVES
	&& armor->type != BRACERS && armor->type != SHIELD
	&& armor->type != HELMET)
    {
        new_draw_info(NDI_UNIQUE, 0,op,"Your marked item is not armour!\n");
        return;
    }

    new_draw_info(NDI_UNIQUE, 0,op,"Applying armour enchantment.");
    improve_armour(op,tmp,armor);
}


extern void apply_poison (object *op, object *tmp)
{
    if (op->type == PLAYER) {
      play_sound_player_only(op->contr, SOUND_DRINK_POISON,0,0);
      new_draw_info(NDI_UNIQUE, 0,op,"Yech!  That tasted poisonous!");
      strcpy(op->contr->killer,"poisonous booze");
    }
    if (tmp->stats.hp > 0) {
      LOG(llevDebug,"Trying to poison player/monster for %d hp\n",
          tmp->stats.hp);
      hit_player(op, tmp->stats.hp, tmp, AT_POISON);
    }
    op->stats.food-=op->stats.food/4;
    decrease_ob(tmp);
}

   /* is_legal_2ways_exit (object* op, object *exit)
    * this fonction return true if the exit
    * is not a 2 ways one or it is 2 ways, valid exit.
    * A valid 2 way exit means:
    *   -You can come back (there is another exit at the other side)
    *   -You are
    *         ° the owner of the exit
    *         ° or in the same party as the owner
    *
    * Note: a owner in a 2 way exit is saved as the owner's name
    * in the field exit->name cause the field exit->owner doesn't
    * survive in the swapping (in fact the whole exit doesn't survive).
    */
 int is_legal_2ways_exit (object* op, object *exit)
   {
   object * tmp;
   object * exit_owner;
   player * pp;
   mapstruct * exitmap;
   if (exit->stats.exp!=1) return 1; /*This is not a 2 way, so it is legal*/
    /* To know if an exit has a correspondant, we look at
     * all the exits in destination and try to find one with same path as
     * the current exit's position */
   if (!strncmp(EXIT_PATH (exit), settings.localdir, strlen(settings.localdir)))
      exitmap = ready_map_name(EXIT_PATH (exit), MAP_PLAYER_UNIQUE);
   else exitmap = ready_map_name(EXIT_PATH (exit), 0);
   if (exitmap)
     {
     tmp=get_map_ob (exitmap,EXIT_X(exit),EXIT_Y(exit));
     if (!tmp) return 0;
     for ( (tmp=get_map_ob(exitmap,EXIT_X(exit),EXIT_Y(exit)));tmp;tmp=tmp->above)
       {
       if (tmp->type!=EXIT) continue;  /*Not an exit*/
       if (!EXIT_PATH (tmp)) continue; /*Not a valid exit*/
       if ( (EXIT_X(tmp)!=exit->x) || (EXIT_Y(tmp)!=exit->y)) continue; /*Not in the same place*/
       if (strcmp(exit->map->path,EXIT_PATH(tmp))!=0) continue; /*Not in the same map*/

       /* From here we have found the exit is valid. However we do
        * here the check of the exit owner. It is important for the
        * town portals to prevent strangers from visiting your appartments
        */
       if (!exit->race) return 1;  /*No owner, free for all!*/
       exit_owner=NULL;
       for (pp=first_player;pp;pp=pp->next)
         {
         if (!pp->ob) continue;
         if (pp->ob->name!=exit->race) continue;
         exit_owner= pp->ob; /*We found a player which correspond to the player name*/
         break;
         }
       if (!exit_owner) return 0;    /* No more owner*/
       if (exit_owner->contr==op->contr) return 1;  /*It is your exit*/
       if  ( exit_owner &&                          /*There is a owner*/
            (op->contr) &&                          /*A player tries to pass */
            ( (exit_owner->contr->party_number<=0) || /*No pass if controller has no party*/
              (exit_owner->contr->party_number!=op->contr->party_number)) ) /* Or not the same as op*/
           return 0;
       return 1;
       }
     }
   return 0;
   }

/* Return value:
 *   0: player or monster can't apply objects of that type
 *   2: objects of that type can't be applied if not in inventory
 *   1: has been applied, or there was an error applying the object
 *
 * op is the object that is causing object to be applied, tmp is the object
 * being applied.
 *
 * aflag is special (always apply/unapply) flags.  Nothing is done with
 * them in this function - they are passed to apply_special
 */

int manual_apply (object *op, object *tmp, int aflag)
{
  int rtn_script;
  if (tmp->head) tmp=tmp->head;

  if (QUERY_FLAG (tmp, FLAG_UNPAID) && ! QUERY_FLAG (tmp, FLAG_APPLIED)) {
    if (op->type == PLAYER) {
      new_draw_info (NDI_UNIQUE, 0, op, "You should pay for it first.");
      return 1;
    } else {
      return 0;   /* monsters just skip unpaid items */
    }
  }
  /* GROS: This is used to handle apply scripts */
  if (tmp->script_apply!=NULL)
  {
    rtn_script = guile_call_event(op,tmp, NULL, aflag, NULL,0,0,tmp->script_apply, SCRIPT_FIX_ALL);
    if (rtn_script!=0) return 1;
  };
  if (tmp->script_str_apply!=NULL)
  {
    rtn_script = guile_call_event_str(op,tmp, NULL, aflag, NULL,0,0,tmp->script_str_apply, SCRIPT_FIX_ALL);
    if (rtn_script!=0) return 1;
  };

  switch (tmp->type)
  {
  case CF_HANDLE:
    new_draw_info(NDI_UNIQUE, 0,op,"You turn the handle.");
    play_sound_map(op->map, op->x, op->y, SOUND_TURN_HANDLE);
    tmp->value=tmp->value?0:1;
    SET_ANIMATION(tmp, tmp->value);
    update_object(tmp,UP_OBJ_FACE);
    push_button(tmp);
    return 1;

  case TRIGGER:
    if (check_trigger (tmp, op)) {
        new_draw_info (NDI_UNIQUE, 0, op, "You turn the handle.");
        play_sound_map (tmp->map, tmp->x, tmp->y, SOUND_TURN_HANDLE);
    } else {
        new_draw_info (NDI_UNIQUE, 0, op, "The handle doesn't move.");
    }
    return 1;

  case EXIT:
    if (op->type != PLAYER)
      return 0;
    if( ! EXIT_PATH (tmp) || !is_legal_2ways_exit(op,tmp)) {
      new_draw_info_format(NDI_UNIQUE, 0, op,
	"The %s is closed.",query_name(tmp));
    } else {
	/* Don't display messages for random maps. */
	if (tmp->msg && strncmp(EXIT_PATH(tmp),"/!",2) && strncmp(EXIT_PATH(tmp), "/random/", 8))
	    new_draw_info (NDI_NAVY, 0, op, tmp->msg);
	enter_exit(op,tmp);
    }
    return 1;

  case SIGN:
    apply_sign (op, tmp);
    return 1;

  case BOOK:
    if (op->type == PLAYER) {
      apply_book (op, tmp);
      return 1;
    } else {
      return 0;
    }

  case SKILLSCROLL:
    if (op->type == PLAYER) {
      apply_skillscroll (op, tmp);
      return 1;
    }
    return 0;

  case SPELLBOOK:
    if (op->type == PLAYER) {
      apply_spellbook (op, tmp);
      return 1;
    }
    return 0;

  case SCROLL:
    apply_scroll (op, tmp);
    return 1;

  case POTION:
    SET_FLAG(tmp,FLAG_APPLIED);
    (void) check_skill_to_apply(op,tmp);
    CLEAR_FLAG(tmp,FLAG_APPLIED);
    (void) apply_potion(op, tmp);
    return 1;

/* Eneq(@csd.uu.se): Handle apply on containers. */
  case CLOSE_CON:
    if (op->type==PLAYER)
      (void) esrv_apply_container (op, tmp->env);
    else
      (void) apply_container (op, tmp->env);
    return 1;

  case CONTAINER:
    if (op->type==PLAYER)
      (void) esrv_apply_container (op, tmp);
    else
      (void) apply_container (op, tmp);
    return 1;

  case TREASURE:
    apply_treasure (op, tmp);
    return 1;

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
  case WAND:
  case ROD:
  case HORN:
  case SKILL:
  case BOW:
    if (tmp->env != op)
      return 2;   /* not in inventory */
    (void) apply_special (op, tmp, aflag);
    return 1;

  case DRINK:
  case FOOD:
  case FLESH:
    apply_food (op, tmp);
    return 1;

  case POISON:
    apply_poison (op, tmp);
    return 1;

  case SAVEBED:
    if (op->type == PLAYER) {
      apply_savebed (op);
      return 1;
    } else {
      return 0;
    }

  case ARMOUR_IMPROVER:
    if (op->type == PLAYER) {
      apply_armour_improver (op, tmp);
      return 1;
    } else {
      return 0;
    }

  case WEAPON_IMPROVER:
    (void) check_improve_weapon(op, tmp);
    return 1;

  case CLOCK:
    if (op->type == PLAYER) {
	char buf[MAX_BUF];
	time_t t = time(NULL);
	strftime(buf, sizeof(buf), "Time is %I:%M %p", localtime(&t));
	play_sound_player_only(op->contr, SOUND_CLOCK,0,0);
	new_draw_info(NDI_UNIQUE, 0,op, buf);
	return 1;
    } else {
        return 0;
    }

  case MENU: 
    if (op->type == PLAYER) {
      shop_listing (op);
      return 1;
    } else {
      return 0;
    }

  case POWER_CRYSTAL:
    apply_power_crystal(op,tmp);  /*  see egoitem.c */
    return 1;

  case LIGHTER:		/* for lighting torches/lanterns/etc */ 
    if (op->type == PLAYER) {
      apply_lighter(op,tmp);
      return 1;
    } else {
      return 0;
    }

  default:
    return 0;
  }
}


/* quiet suppresses the "don't know how to apply" and "you must get it first"
 * messages as needed by player_apply_below().  But there can still be
 * "but you are floating high above the ground" messages.
 *
 * Same return value as apply() function.
 */
int player_apply (object *pl, object *op, int aflag, int quiet)
{
    int tmp;

    if (op->env == NULL && QUERY_FLAG (pl, FLAG_FLYING))
    {
        /* player is flying and applying object not in inventory */
        if ( ! QUERY_FLAG (pl, FLAG_WIZ)
            && ! QUERY_FLAG (op, FLAG_FLYING)
            && ! QUERY_FLAG (op, FLAG_FLY_ON))
        {
            new_draw_info (NDI_UNIQUE, 0, pl, "But you are floating high "
                           "above the ground!");
            return 0;
        }
    }

    if (QUERY_FLAG (op, FLAG_WAS_WIZ) && ! QUERY_FLAG (pl, FLAG_WAS_WIZ))
    {
        play_sound_map (pl->map, pl->x, pl->y, SOUND_OB_EVAPORATE);
        new_draw_info (NDI_UNIQUE, 0, pl, "The object disappears in a puff "
                       "of smoke!");
        new_draw_info (NDI_UNIQUE, 0, pl, "It must have been an illusion.");
        remove_ob (op);
        free_object (op);
        return 1;
    }

    pl->contr->last_used = op;
    pl->contr->last_used_id = op->count;

    tmp = manual_apply (pl, op, aflag);
    if ( ! quiet) {
        if (tmp == 0)
            new_draw_info_format (NDI_UNIQUE, 0, pl,
                                  "I don't know how to apply the %s.",
                                  query_name (op));
        else if (tmp == 2)
            new_draw_info_format (NDI_UNIQUE, 0, pl,
                                  "You must get it first!\n");
    }
    return tmp;
}

/* player_apply_below attempts to apply the object 'below' the player.
 * If the player has an open container, we use that for below, otherwise
 * we use the ground.
 */

void player_apply_below (object *pl)
{
    object *tmp, *next;
    int floors;

    /* If using a container, set the starting item to be the top
     * item in the container.  Otherwise, use the map.
     */
    tmp = (pl->container != NULL) ? pl->container->inv : pl->below;

    /* This is perhaps more complicated.  However, I want to make sure that
     * we don't use a corrupt pointer for the next object, so we get the
     * next object in the stack before applying.  This is can only be a
     * problem if player_apply() has a bug in that it uses the object but does
     *  not return a proper value.
     */
    for (floors = 0; tmp!=NULL; tmp=next) {
	next = tmp->below;
        if (QUERY_FLAG (tmp, FLAG_IS_FLOOR))
            floors++;
        else if (floors > 0)
            return;   /* process only floor objects after first floor object */
	if ( ! tmp->invisible || QUERY_FLAG (tmp, FLAG_WALK_ON)
            || QUERY_FLAG (tmp, FLAG_FLY_ON))
        {
            if (player_apply (pl, tmp, 0, 1) == 1)
                return;
        }
        if (floors >= 2)
            return;   /* process at most two floor objects */
    }
}


/* who is the object using the object.
 * op is the object they are using.
 * aflags is special flags (0 - normal/toggle, AP_APPLY=always apply,
 * AP_UNAPPLY=always unapply).
 *
 * Optional flags:
 *   AP_NO_MERGE: don't merge an unapplied object with other objects
 *   AP_IGNORE_CURSE: unapply cursed items
 *
 * Usage example:  apply_special (who, op, AP_UNAPPLY | AP_IGNORE_CURSE)
 *
 * apply_special() doesn't check for unpaid items.
 */
int apply_special (object *who, object *op, int aflags)
{ /* wear/wield */
  int basic_flag = aflags & AP_BASIC_FLAGS;
  object *tmp;
  char buf[HUGE_BUF];
  int i;

  if(who==NULL) {
    LOG(llevError,"apply_special() from object without environment.\n");
    return 1;
  }

  if(op->env!=who)
    return 1;   /* op is not in inventory */

  /* GROS: This is used to handle apply scripts */
  if (op->script_apply!=NULL)
  {
      guile_call_event(who,op, NULL, aflags, NULL,0,0,op->script_apply, SCRIPT_FIX_ALL);
  };
  if (op->script_str_apply!=NULL)
  {
        guile_call_event_str(who,op, NULL, aflags, NULL,0,0,op->script_str_apply, SCRIPT_FIX_ALL);
  };

  buf[0]='\0';	    /* Needs to be initialized */
  if (QUERY_FLAG(op,FLAG_APPLIED)) {
    /* always apply, so no reason to unapply */
    if (basic_flag == AP_APPLY) return 0;
    if ( ! (aflags & AP_IGNORE_CURSE)
        && (QUERY_FLAG(op, FLAG_CURSED) || QUERY_FLAG(op, FLAG_DAMNED)))
    {
      new_draw_info_format(NDI_UNIQUE, 0, who,
	"No matter how hard you try, you just can't\nremove %s.",
	      query_name(op));
      return 1;
    }
    CLEAR_FLAG(op, FLAG_APPLIED);
    switch(op->type) {
    case WEAPON:
      (void) change_abil (who,op);
      /* 'unready' melee weapons skill if it is current skill */
      (void) check_skill_to_apply(who,op);
      if(QUERY_FLAG(who,FLAG_READY_WEAPON))
		CLEAR_FLAG(who,FLAG_READY_WEAPON);
      /* GROS: update the current_weapon_script field (used with script_attack for weapons) */
      who->current_weapon_script = NULL;
      who->current_weapon = NULL;
      sprintf(buf,"You unwield %s.",query_name(op));
      break;

    case SKILL:         /* allows objects to impart skills */
      if (op != who->chosen_skill) {
          LOG (llevError, "BUG: apply_special(): applied skill is not "
               "chosen skill\n");
      }
      if (who->type==PLAYER) {
          who->contr->shoottype = range_none;
          who->contr->last_value = -1;
          if ( ! op->invisible) {
              /* its a tool, need to unlink it */
              unlink_skill (op);
              new_draw_info_format (NDI_UNIQUE, 0, who,
                                    "You stop using the %s.", query_name(op));
              new_draw_info_format (NDI_UNIQUE, 0, who,
                                    "You can no longer use the skill: %s.",
                                    skills[op->stats.sp].name);
	  }
      }
      (void) change_abil (who, op);
      who->chosen_skill = NULL;
      CLEAR_FLAG (who, FLAG_READY_SKILL);
      buf[0] = '\0';
      break;

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
      (void) change_abil (who,op);
      sprintf(buf,"You unwear %s.",query_name(op));
      break;
    case BOW:
    case WAND:
    case ROD:
    case HORN:
      (void) check_skill_to_apply(who,op);
      sprintf(buf,"You unready %s.",query_name(op));
      if(who->type==PLAYER) {
        who->contr->shoottype = range_none;
        who->contr->last_value = -1;
      } else {
        switch (op->type) {
          case WAND: CLEAR_FLAG (who, FLAG_READY_WAND); break;
          case ROD:  CLEAR_FLAG (who, FLAG_READY_ROD); break;
          case HORN: CLEAR_FLAG (who, FLAG_READY_HORN); break;
          case BOW:  CLEAR_FLAG (who, FLAG_READY_BOW); break;
        }
      }
      break;
    default:
      sprintf(buf,"You unapply %s.",query_name(op));
      break;
    }
    if (buf[0] != '\0')
      new_draw_info(NDI_UNIQUE, 0,who,buf);
    fix_player(who);

    if ( ! (aflags & AP_NO_MERGE)) {
        tag_t del_tag = op->count;
        tmp = merge_ob (op, NULL);
        if (who->type == PLAYER) {
            if (tmp) {  /* it was merged */
                esrv_del_item (who->contr, del_tag);
                op = tmp;
            }
            esrv_send_item (who, op);
        }
    }

    return 0;
  }
  if (basic_flag == AP_UNAPPLY) return 0;
  i=0;
  /* This goes through and checks to see if the player already has something
   * of that type applied - if so, unapply it.
   */
  for(tmp=who->inv;tmp!=NULL;tmp=tmp->below)
    if(tmp->type==op->type&&QUERY_FLAG(tmp, FLAG_APPLIED)&&tmp!=op) {
      if(tmp->type==RING&&!i)
        i=1;
      else if(apply_special(who,tmp,0))
        return 1;
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
      return 1;
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
	new_draw_info(NDI_UNIQUE, 0,who,"The weapon does not recognize you as its owner.");
        if(tmp!=NULL)
          (void) insert_ob_in_ob(tmp,who);
	return 1;
	}


    SET_FLAG(op, FLAG_APPLIED);

	/* check for melee weapons skill, alter player status.
	 * Note that we need to call this *before* change_abil */
    if(!check_skill_to_apply(who,op)) return 1;
    if(!QUERY_FLAG(who,FLAG_READY_WEAPON))
         SET_FLAG(who, FLAG_READY_WEAPON);

    (void) change_abil (who,op);
    /* GROS: update the current_weapon_script field (used with script_attack for weapons) */
    if ((op->script_attack != NULL)|(op->script_str_attack != NULL))
    {
        LOG(llevDebug, "Scripting Weapon wielded\n");
        if (who->current_weapon_script) free_string(who->current_weapon_script);
        who->current_weapon_script=add_string(query_name(op));
        who->current_weapon = op;
    };
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
      return 1;
    }
  case RING:
  case AMULET:
    SET_FLAG(op, FLAG_APPLIED);
    (void) change_abil (who,op);
    sprintf(buf,"You wear %s.",query_name(op));
    break;

  /* this part is needed for skill-tools */ 
  case SKILL:
    if (who->chosen_skill) {
        LOG (llevError, "BUG: apply_special(): can't apply two skills\n");
        return 1;
    }
    if (who->type == PLAYER) {
        who->contr->shoottype = range_skill;
        if ( ! op->invisible) {
            /* for tools */
            if (op->exp_obj)
                LOG (llevError, "BUG: apply_special(SKILL): found unapplied "
                     "tool with experience object\n");
            else
                (void) link_player_skill (who, op);
            new_draw_info_format (NDI_UNIQUE, 0, who, "You ready %s.",
                                  query_name (op));
            new_draw_info_format (NDI_UNIQUE, 0, who,
                                  "You can now use the skill: %s.",
                                  skills[op->stats.sp].name);
        } else {
            new_draw_info_format (NDI_UNIQUE, 0, who, "Readied skill: %s.",
                                  skills[op->stats.sp].name);
        }
    }
    SET_FLAG (op, FLAG_APPLIED);
    (void) change_abil (who, op);
    who->chosen_skill = op;
    SET_FLAG (who, FLAG_READY_SKILL);
    buf[0] = '\0';
    break;

  case WAND:
  case ROD:
  case HORN:
  case BOW:
    /* check for skill, alter player status */ 
    SET_FLAG(op, FLAG_APPLIED);
    if(!check_skill_to_apply(who,op)) return 1;
    new_draw_info_format (NDI_UNIQUE, 0, who, "You ready %s.", query_name(op));
    if(who->type==PLAYER) {
      switch (op->type) {
        case BOW:  who->contr->shoottype = range_bow; break;
        case WAND: who->contr->shoottype = range_wand; break;
        case ROD:  who->contr->shoottype = range_rod; break;
        case HORN: who->contr->shoottype = range_horn; break;
      }
      if (op->type == BOW) {
        new_draw_info_format (NDI_UNIQUE, 0, who,
                              "You will now fire %s with %s.",
	                      op->race ? op->race : "nothing", query_name(op));
      } else {
        who->contr->chosen_item_spell = op->stats.sp;
        who->contr->known_spell = (QUERY_FLAG (op, FLAG_BEEN_APPLIED)
                                   || QUERY_FLAG (op, FLAG_IDENTIFIED));
      }
    } else {
      switch (op->type) {
        case WAND: SET_FLAG (who, FLAG_READY_WAND); break;
        case ROD:  SET_FLAG (who, FLAG_READY_ROD); break;
        case HORN: SET_FLAG (who, FLAG_READY_HORN); break;
        case BOW:  SET_FLAG (who, FLAG_READY_BOW); break;
      }
    }
    break;

  default:
    sprintf(buf,"You apply %s.",query_name(op));
  }
  if(!QUERY_FLAG(op, FLAG_APPLIED))
      SET_FLAG(op, FLAG_APPLIED);
  if (buf[0] != '\0')
      new_draw_info (NDI_UNIQUE, 0, who, buf);
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


int monster_apply_special (object *who, object *op, int aflags)
{
  if (QUERY_FLAG (op, FLAG_UNPAID) && ! QUERY_FLAG (op, FLAG_APPLIED))
    return 1;
  return apply_special (who, op, aflags);
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
    insert_ob_in_map(tmp,op->map,NULL,0);
    CLEAR_FLAG(op,FLAG_AUTO_APPLY);
    identify(tmp);
    break;

  case TREASURE:
    while ((op->stats.hp--)>0)
      create_treasure(op->randomitems, op, op->map?GT_ENVIRONMENT:0,
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

  if(m==NULL) return;

  for(x=0;x<MAP_WIDTH(m);x++)
    for(y=0;y<MAP_HEIGHT(m);y++)
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
			create_treasure(invtmp->randomitems, invtmp, 0,
                            m->difficulty,0);
		}
	    }
	}

	if(QUERY_FLAG(tmp,FLAG_AUTO_APPLY))
          auto_apply(tmp);
        else if((tmp->type==TREASURE || (tmp->type==CONTAINER))&&tmp->randomitems) {
	  while ((tmp->stats.hp--)>0)
            create_treasure(tmp->randomitems, tmp, 0,
                            m->difficulty,0);
	}
	else if(tmp->type==TIMED_GATE) {
	  tmp->speed = 0;
	  update_ob_speed(tmp);
	}
        if(tmp && tmp->arch && tmp->type!=PLAYER && tmp->type!=TREASURE &&
	  tmp->randomitems)
            create_treasure(tmp->randomitems, tmp, GT_APPLY,
                            m->difficulty,0);
      }
  for(x=0;x<MAP_WIDTH(m);x++)
    for(y=0;y<MAP_HEIGHT(m);y++)
      for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=tmp->above)
	if (tmp->above &&
            (tmp->type == TRIGGER_BUTTON || tmp->type == TRIGGER_PEDESTAL))
	  check_trigger (tmp, tmp->above);
}

/* eat_special_food() - some food may (temporarily) alter
 * player status. We do it w/ this routine and cast_change_attr().
 * Note the dircection is set to "99"  so that cast_change_attr()
 * will only modify the user's status. We shouldnt be able to 
 * effect others by eating food!
 * -b.t.
 */

void eat_special_food(object *who, object *food) {
    /* Corresponding spell to cast to get protection to
     * the attactkype.  This matches the order of ATNR values
     * in attack.h
     */
    static int sp_type[NROFATTACKS] = { SP_ARMOUR, SP_PROT_MAGIC, SP_PROT_FIRE, 
	SP_PROT_ELEC, SP_PROT_COLD, SP_PROT_CONFUSE, -1 /*acid */, 
	SP_PROT_DRAIN, 	-1 /*weaponmagic*/, -1 /*ghosthit*/, SP_PROT_POISON, 
	SP_PROT_SLOW, SP_PROT_PARALYZE, -1 /* turn undead */,
	-1 /* fear */, SP_PROT_CANCEL, SP_PROT_DEPLETE, -1 /* death*/,
	-1 /* chaos */, -1 /*counterspell */, -1 /* godpower */,
	-1 /*holyword */, -1 /*blind*/, -1 /*internal */ };

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
    for (i=0; i<NROFATTACKS; i++) {
	if (food->resist[i]>0 && sp_type[i]!=-1)
	    cast_change_attr(who,who,99,sp_type[i]);
    }

  /* check for hp, sp change */
  if(food->stats.hp!=0) {
     if(QUERY_FLAG(food, FLAG_CURSED)) { 
	strcpy(who->contr->killer,food->name);
	hit_player(who, food->stats.hp, food, AT_POISON);
	new_draw_info(NDI_UNIQUE, 0,who,"Eck!...that was poisonous!");
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

	save_throw_object(item,AT_FIRE,who);
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
     op->stats.sp -= random_roll(0, power-1, op, PREFER_LOW);
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

void apply_changes_to_player(object *pl, object *change) {
    int excess_stat=0;  /* if the stat goes over the maximum
                         for the race, put the excess stat some
                         where else. */

    switch (change->type) {
	case CLASS: {
	    living *stats = &(pl->contr->orig_stats);
	    living *ns = &(change->stats);
	    object *walk;
	    int flag_change_face=1;

	    /* the following code assigns stats up to the stat max
	     * for the race, and if the stat max is exceeded, 
	     * tries to randomly reassign the excess stat 
	     */ 
	    int i,j;
	    for(i=0;i<7;i++) {
		int stat=get_attr_value(stats,i);
		int race_bonus = get_attr_value(&(pl->arch->clone.stats),i);
		stat += get_attr_value(ns,i);
		if(stat > 20 + race_bonus) {
		    excess_stat++;
		    stat = 20+race_bonus;
		}
		set_attr_value(stats,i,stat);
	    }

	    for(j=0;excess_stat >0 && j<100;j++)  {/* try 100 times to assign excess stats */
		int i = RANDOM() %7;
		int stat=get_attr_value(stats,i);
		int race_bonus = get_attr_value(&(pl->arch->clone.stats),i);
		if(i==CHA) continue;  /* exclude cha from this */
		if( stat < 20 + race_bonus) {
		    change_attr_value(stats,i,1);
		    excess_stat--;
		}
	    }

	    /* insert the randomitems from the change's treasurelist into
	     * the player ref: player.c
	     */
	    if(change->randomitems!=NULL) 
		give_initial_items(pl,change->randomitems);


	    /* set up the face, for some races. */

	    /* first, look for the force object banning 
	     * changing the face.  Certain races never change face with class. 
	     */
	    for(walk=pl->inv;walk!=NULL;walk=walk->below)
		if (!strcmp(walk->name,"NOCLASSFACECHANGE")) flag_change_face=0;

	    if(flag_change_face) {
		pl->animation_id = GET_ANIM_ID(change);
		pl->face = change->face;

		if(QUERY_FLAG(change,FLAG_ANIMATE)) 
		    SET_FLAG(pl,FLAG_ANIMATE);
		else
		    CLEAR_FLAG(pl,FLAG_ANIMATE);
	    }

	    /* check the special case of can't use weapons */
	    /*if(QUERY_FLAG(change,FLAG_USE_WEAPON)) CLEAR_FLAG(pl,FLAG_USE_WEAPON);*/
	    if(!strcmp(change->name,"monk")) CLEAR_FLAG(pl,FLAG_USE_WEAPON);

	    break;
	}
    }
}


