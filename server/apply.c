/*
 * static char *rcsid_apply_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel & Crossfire Development Team
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
#include <spells.h>
#include <skills.h>
#include <tod.h>

#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/* Want this regardless of rplay. */
#include <sounds.h>

/* need math lib for double-precision and pow() in dragon_eat_flesh() */
#include <math.h>

/**
 * This handles a player dropping money on an altar to identify stuff.
 * It'll identify marked item, if none all items up to dropped money.
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
		"You have %s.", long_desc(marked, pl));
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
			"You have %s.", long_desc(id, pl));
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

/**
 * Handles applying a potion.
 */
int apply_potion(object *op, object *tmp)
{
    int got_one=0,i;
    object *force;

    if(op->type==PLAYER) { 
      if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED))
        identify(tmp);
    }

    /* Potion of restoration - only for players */
    if (op->type==PLAYER&&(tmp->attacktype & AT_DEPLETE)) {
	object *depl;
	archetype *at;

	if (QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) {
	    drain_stat(op);
	    fix_player(op);
	    decrease_ob(tmp);
	    return 1;
	}
	if ((at = find_archetype("depletion"))==NULL) {
	    LOG(llevError,"Could not find archetype depletion\n");
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
	    new_draw_info(NDI_UNIQUE,0,op, "You potion had no effect.");

	decrease_ob(tmp);
	return 1;
    }

    /* improvement potion - only for players */
    if(op->type==PLAYER&&tmp->attacktype&AT_GODPOWER) {

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
     * and heroism all fit into this category.  Given the spell object code,
     * there is no limit to the number of spells that potions can be cast,
     * but direction is problematic to try and imbue fireball potions for example.
     */
    if (tmp->inv) {
	if(QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) {
	    object *fball;

	    new_draw_info(NDI_UNIQUE,0,op, "Yech!  Your lungs are on fire!");
            /* Explodes a fireball centered at player */
            fball = get_archetype(EXPLODING_FIREBALL);
            fball->dam_modifier=random_roll(1, op->level, op, PREFER_LOW)/5+1;
            fball->stats.maxhp=random_roll(1, op->level, op, PREFER_LOW)/10+2;
            fball->x = op->x;
            fball->y = op->y;
            insert_ob_in_map(fball, op->map, NULL, 0);
	} else
	    cast_spell(op,tmp, op->facing, tmp->inv, NULL);

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
	if((QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) && tmp->value!=0)
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

/**
 * This returns the sum of nrof of item (arch name).
 */
int check_item(object *op,const char *item)
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

/**
 * This removes 'nrof' of what item->slaying says to remove.
 * op is typically the player, which is only
 * really used to determine what space to look at.
 * Modified to only eat 'nrof' of objects.
 */
static void eat_item(object *op,char *item, int nrof)
{
    object *prev;

    prev = op;
    op=op->below;

    while(op!=NULL) {
	if (strcmp(op->arch->name,item)==0) {
	    if (op->nrof >= nrof) {
		decrease_ob_nr(op,nrof);
		return;
	    } else {
		decrease_ob_nr(op,op->nrof);
		nrof -= op->nrof;
	    }
	    op=prev;
	}
	prev = op;
	op=op->below;
    }
}

/**
 * This checks to see of the player (who) is sufficient level to use a weapon
 * with improvs improvements (typically last_eat).  We take an int here
 * instead of the object so that the improvement code can pass along the
 * increased value to see if the object is usuable.
 * we return 1 (true) if the player can use the weapon.
 */
int check_weapon_power(object *who, int improvs)
{
/* Old code is below (commented out).  Basically, since weapons are the only
 * object players really have any control to improve, it's a bit harsh to
 * require high level in some combat skill, so we just use overall level.
 */
#if 1
    if (((who->level/5)+5) >= improvs) return 1;
    else return 0;

#else
    int level=0;

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
	    if (wc_obj->type == SKILL && IS_COMBAT_SKILL(wc_obj->subtype) && wc_obj->level > level)
		level  = wc_obj->level;

	if (!level )  {
	    LOG(llevError,"Error: Player: %s lacks wc experience object\n",who->name);
	    level = who->level;
	}
    }
    else
	level=who->level;

    return (improvs <= ((level/5)+5));
#endif
}

/**
 * Returns how many items of type improver->slaying there are under op.
 * Will display a message if none found, and 1 if improver->slaying is NULL.
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

/**
 * Actually improves the weapon, and tells user.
 */
int improve_weapon_stat(object *op,object *improver,object *weapon,
			signed char *stat,int sacrifice_count,const char *statname)
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


/**
 * This does the prepare weapon scroll.
 * Checks for sacrifice, and so on.
 */

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
	(weapon->stats.sp && weapon->type == WEAPON) ||	/* sp regeneration */
	weapon->stats.exp ||	/* speed */
	weapon->stats.ac)	/* AC - only taifu's I think */
    {
      new_draw_info(NDI_UNIQUE,0,op,"Cannot prepare magic weapons.");
      return 0;
    }
    sacrifice_count=check_sacrifice(op,improver);
    if (sacrifice_count<=0)
      return 0;
    weapon->level=isqrt(sacrifice_count);
    new_draw_info(NDI_UNIQUE,0,op,"Your sacrifice was accepted.");
    eat_item(op, improver->slaying, sacrifice_count);

    new_draw_info_format(NDI_UNIQUE, 0, op,"Your *%s may be improved %d times.",
	    weapon->name,weapon->level);

    sprintf(buf,"%s's %s",op->name,weapon->name);
    free_string(weapon->name);
    FREE_AND_COPY(weapon->name, buf);
    FREE_AND_COPY(weapon->name_pl, buf);
    weapon->nrof=0;  /*  prevents preparing n weapons in the same
			 slot at once! */
    decrease_ob(improver);
    weapon->last_eat=0;
    return 1;
}


/**
 * Does the dirty job for 'improve weapon' scroll, prepare or add something.
 * This is the new improve weapon code.
 * Returns 0 if it was not able to work for some reason.
 *
 * Checks if weapon was prepared, if enough potions on the floor, ...
 *
 * We are hiding extra information about the weapon in the level and
 * last_eat numbers for an object.  Hopefully this won't break anything ?? 
 * level == max improve last_eat == current improve
 */
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
  if (weapon->level==weapon->last_eat && weapon->item_power >=100) {
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

	weapon->item_power++;
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
	weapon->item_power++;
	decrease_ob(improver);
	return 1;
  }
  if (improver->stats.sp == IMPROVE_ENCHANT) {
	weapon->magic++;
	weapon->last_eat++;
	new_draw_info_format(NDI_UNIQUE, 0, op
		,"Weapon magic increased to %d",weapon->magic);
	decrease_ob(improver);
	weapon->item_power++;
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
  eat_item(op,improver->slaying, sacrifice_needed);
  weapon->item_power++;

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

/**
 * Handles the applying of improve/prepare/enchant weapon scroll.
 * Checks a few things (not on a non-magic square, marked weapon, ...),
 * then calls improve_weapon to do the dirty work.
 */
int check_improve_weapon (object *op, object *tmp)
{
    object *otmp;

    if(op->type!=PLAYER)
      return 0;
    if (get_map_flags(op->map,NULL, op->x,op->y, NULL, NULL) & P_NO_MAGIC) {
	new_draw_info(NDI_UNIQUE, 0,op,"Something blocks the magic of the scroll.");
	return 0;
    }
    otmp=find_marked_object(op);
    if(!otmp) {
      new_draw_info(NDI_UNIQUE, 0, op, "You need to mark a weapon object.");
      return 0;
    }
    if (otmp->type != WEAPON && otmp->type != BOW) {
      new_draw_info(NDI_UNIQUE, 0,op,"Marked item is not a weapon or bow");
      return 0;
    }
    new_draw_info(NDI_UNIQUE, 0,op,"Applied weapon builder.");
    improve_weapon(op,tmp,otmp);
    esrv_send_item(op, otmp);
    return 1;
}

/**
 * This code deals with the armour improvment scrolls.
 * Change limits on improvement - let players go up to
 * +5 no matter what level, but they are limited by item
 * power.
 * Try to use same improvement code as in the common/treasure.c
 * file, so that if you make a +2 full helm, it will be just
 * the same as one you find in a shop.
 *
 * deprecated comment:
 * this code is by b.t. (thomas@nomad.astro.psu.edu) -
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
    object *tmp;

    if (armour->magic >= settings.armor_max_enchant) {
        new_draw_info(NDI_UNIQUE, 0,op,"This armour can not be enchanted any further.");
	return 0;
    }
    /* Dealing with random artifact armor is a lot trickier (in terms of value, weight,
     * etc), so take the easy way out and don't worry about it.
     * Note - maybe add scrolls which make the random artifact versions (eg, armour
     * of gnarg and what not?)
     */
    if (armour->title) {
	new_draw_info(NDI_UNIQUE, 0, op, "This armour will not accept further enchantment.");
	return 0;
    }
	
    /* Split objects if needed.  Can't insert tmp until the
     * end of this function - otherwise it will just re-merge.
     */
    if(armour->nrof > 1)
	tmp = get_split_ob(armour,armour->nrof - 1);
    else
	tmp = NULL;

    armour->magic++;

    if ( !settings.armor_speed_linear )
        {
        int base = 100;
        int pow = 0;
        while ( pow < armour->magic )
            {
            base = base - ( base * settings.armor_speed_improvement ) / 100;
            pow++;
            }

        ARMOUR_SPEED( armour ) = ( ARMOUR_SPEED( &armour->arch->clone ) * base ) / 100;
        }
    else
        ARMOUR_SPEED( armour ) = ( ARMOUR_SPEED( &armour->arch->clone ) * ( 100 + armour->magic * settings.armor_speed_improvement ) )/100;

    if ( !settings.armor_weight_linear )
        {
        int base = 100;
        int pow = 0;
        while ( pow < armour->magic )
            {
            base = base - ( base * settings.armor_weight_reduction ) / 100;
            pow++;
            }

        armour->weight = ( armour->arch->clone.weight * base ) / 100;
        }
    else
        armour->weight = ( armour->arch->clone.weight * ( 100 - armour->magic * settings.armor_weight_reduction ) ) / 100;

    if ( armour->weight <= 0 )
        {
        LOG( llevInfo, "Warning: enchanted armours can have negative weight\n." );
        armour->weight = 1;
        }

    armour->item_power = get_power_from_ench(armour->arch->clone.item_power + armour->magic);

    if (op->type == PLAYER) {
        esrv_send_item(op, armour);
        if(QUERY_FLAG(armour, FLAG_APPLIED))
            fix_player(op);
    }
    decrease_ob(improver);
    if (tmp) {
	insert_ob_in_ob(tmp, op);
	esrv_send_item(op, tmp);
    }
    return 1;
}


/*
 * convert_item() returns 1 if anything was converted, otherwise 0
 */
#define CONV_FROM(xyz)	xyz->slaying
#define CONV_TO(xyz)	xyz->other_arch
#define CONV_NR(xyz)	(unsigned char) xyz->stats.sp
#define CONV_NEED(xyz)	(unsigned long) xyz->stats.food

/* Takes one items and makes another.
 * converter is the object that is doing the conversion.
 * item is the object that triggered the converter - if it is not
 * what the converter wants, this will not do anything.
 */
int convert_item(object *item, object *converter) {
    int nr=0;
    object *tmp;

    /* We make some assumptions - we assume if it takes money as it type,
     * it wants some amount.  We don't make change (ie, if something costs
     * 3 gp and player drops a platinum, tough luck)
     */
    if (!strcmp(CONV_FROM(converter),"money")) {
	int cost;

	if(item->type!=MONEY)
	    return 0;

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
    if (!item) {
	LOG(llevError,"Broken converter %s at %s (%d, %d)\n", 
	    converter->name, converter->map->path, converter->x, converter->y);
	return 0;
    }
    fix_generated_item(item, converter, 0, 0, GT_MINIMAL);
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
  
/**
 * Handle apply on containers. 
 * By Eneq(@csd.uu.se).
 * Moved to own function and added many features [Tero.Haatanen@lut.fi]
 * added the alchemical cauldron to the code -b.t.
 */

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

/**
 * Eneq(@csd.uu.se): Handle apply on containers.  This is for containers
 * the player has in their inventory, eg, sacks, luggages, etc.
 *
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
    event *evt;
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
	/* GROS: Handle for plugin close event */
    if ((evt = find_event(tmp, EVENT_CLOSE)) != NULL)
	{
		CFParm CFP;
		CFParm* CFR;
		int k, l, m;
		uint32 n;
		int rtn_script = 0;
		m = 0;
		n = 0;
		k = EVENT_CLOSE;
		l = SCRIPT_FIX_ALL;
		CFP.Value[0] = &k;
		CFP.Value[1] = op;
		CFP.Value[2] = tmp;
		CFP.Value[3] = NULL;
		CFP.Value[4] = NULL;
		CFP.Value[5] = &n;
		CFP.Value[6] = &m;
		CFP.Value[7] = &m;
		CFP.Value[8] = &l;
		CFP.Value[9] = evt->hook;
		CFP.Value[10]= evt->options;
		if (findPlugin(evt->plugin)>=0)
		{
			CFR = (PlugList[findPlugin(evt->plugin)].eventfunc) (&CFP);
			rtn_script = *(int *)(CFR->Value[0]);
			if (rtn_script!=0) return 1;
		}
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


/**
 * Handles dropping things on altar.
 * Returns true if sacrifice was accepted.
 */
static int apply_altar (object *altar, object *sacrifice, object *originator)
{
    /* Only players can make sacrifices on spell casting altars. */
    if (altar->inv && ( ! originator || originator->type != PLAYER))
	return 0;

    if (operate_altar (altar, &sacrifice)) {
	/* Simple check.  Unfortunately, it means you can't cast magic bullet
	 * with an altar.  We call it a Potion - altars are stationary - it
	 * is up to map designers to use them properly.
	 */
	if (altar->inv && altar->inv->type==SPELL) {
	    new_draw_info_format (NDI_BLACK, 0, originator, "The altar casts %s.",
				  altar->inv->name);
	    cast_spell (originator, altar, 0, altar->inv, NULL);
	    /* If it is connected, push the button.  Fixes some problems with
	     * old maps.
	     */
/*	    push_button (altar);*/
	} else {
	    altar->value = 1;  /* works only once */
	    push_button (altar);
	}
	return sacrifice == NULL;
    } else {
	return 0;
    }
}


/**
 * Handles 'movement' of shop mats.
 * Returns 1 if 'op' was destroyed, 0 if not.
 * Largely re-written to not use nearly as many gotos, plus
 * some of this code just looked plain out of date.
 * MSW 2001-08-29
 */
static int apply_shop_mat (object *shop_mat, object *op)
{
    int rv = 0;
    object *tmp, *next;

    SET_FLAG (op,FLAG_NO_APPLY);   /* prevent loops */

    if (op->type != PLAYER) {
	/* Remove all the unpaid objects that may be carried here.
	 * This could be pets or monsters that are somehow in
	 * the shop.
	 */
	for (tmp=op->inv; tmp; tmp=next) {
	    next = tmp->below;
	    if (QUERY_FLAG(tmp, FLAG_UNPAID)) {
		int i = find_free_spot (tmp->arch, op->map, op->x, op->y, 1, 9);

		remove_ob(tmp);
		if (i==-1) i=0;
		tmp->map = op->map;
		tmp->x = op->x + freearr_x[i];
		tmp->y = op->y + freearr_y[i];
		insert_ob_in_map(tmp, op->map, op, 0);
	    }
	}

	/* Don't teleport things like spell effects */
	if (QUERY_FLAG(op, FLAG_NO_PICK)) return 0;

	/* unpaid objects, or non living objects, can't transfer by
	 * shop mats.  Instead, put it on a nearby space.
	 */
	if (QUERY_FLAG(op, FLAG_UNPAID) || !QUERY_FLAG(op, FLAG_ALIVE)) {

	    /* Somebody dropped an unpaid item, just move to an adjacent place. */
	    int i = find_free_spot (op->arch, op->map, op->x, op->y, 1, 9);
	    if (i != -1) {
		rv = transfer_ob (op, op->x + freearr_x[i], op->y + freearr_y[i], 0,
                       shop_mat);
	    }
	    return 0;
	}
	/* Removed code that checked for multipart objects - it appears that
	 * the teleport function should be able to handle this just fine.
	 */
	rv = teleport (shop_mat, SHOP_MAT, op);
    }
    /* immediate block below is only used for players */
    else if (get_payment (op, op->inv)) {
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
	    esrv_map_scroll(&op->contr->socket, freearr_x[i],freearr_y[i]);
	    op->contr->socket.update_look=1;
	    op->contr->socket.look_position=0;
	}
    }
    CLEAR_FLAG (op, FLAG_NO_APPLY);
    return rv;
}

/**
 * Handles applying a sign.
 */
static void apply_sign (object *op, object *sign, int autoapply)
{
    readable_message_type* msgType;
    char newbuf[HUGE_BUF];
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
    msgType=get_readable_message_type(sign);
    snprintf(newbuf,sizeof(newbuf),"%hhu %s", autoapply?1:0,sign->msg);
    draw_ext_info(NDI_UNIQUE | NDI_NAVY, 0, op, msgType->message_type, msgType->message_subtype, newbuf, sign->msg);
}


/**
 * 'victim' moves onto 'trap' (trap has FLAG_WALK_ON or FLAG_FLY_ON set) or
 * 'victim' leaves 'trap' (trap has FLAG_WALK_OFF or FLAG_FLY_OFF) set.
 *
 * originator: Player, monster or other object that caused 'victim' to move
 * onto 'trap'.  Will receive messages caused by this action.  May be NULL.
 * However, some types of traps require an originator to function.
 */
void move_apply (object *trap, object *victim, object *originator)
{
  static int recursion_depth = 0;
  event *evt;

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
  /* GROS: Handle for plugin close event */
  if ((evt = find_event(trap, EVENT_TRIGGER))!=NULL)
  {
    CFParm CFP;
    CFParm* CFR;
    int k, l, m;
    uint32 n;
    int rtn_script = 0;
    m = 0;
    n = 0;
    k = EVENT_TRIGGER;
    l = SCRIPT_FIX_ALL;
    CFP.Value[0] = &k;
    CFP.Value[1] = trap;
    CFP.Value[2] = originator;
    CFP.Value[3] = victim;
    CFP.Value[4] = NULL;
    CFP.Value[5] = &n;
    CFP.Value[6] = &m;
    CFP.Value[7] = &m;
    CFP.Value[8] = &l;
    CFP.Value[9] = evt->hook;
    CFP.Value[10]= evt->options;
    if (findPlugin(evt->plugin)>=0)
    {
      CFR = (PlugList[findPlugin(evt->plugin)].eventfunc) (&CFP);
      rtn_script = *(int *)(CFR->Value[0]);
      if (rtn_script!=0) return;
    }
  }
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

  case THROWN_OBJ:
    if (trap->inv == NULL)
      goto leave;
    /* fallthrough */
  case ARROW:

      /* bad bug: monster throw a object, make a step forwards, step on object ,
       * trigger this here and get hit by own missile - and will be own enemy.
       * Victim then is his own enemy and will start to kill herself (this is
       * removed) but we have not synced victim and his missile. To avoid senseless
       * action, we avoid hits here */
      if ((QUERY_FLAG (victim, FLAG_ALIVE) && trap->speed) && trap->owner != victim)
      hit_with_arrow (trap, victim);
    goto leave;


    case SPELL_EFFECT:
	apply_spell_effect(trap, victim);
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
        check_trigger (trap, victim);
    goto leave;

  case DEEP_SWAMP:
    walk_on_deep_swamp (trap, victim);
    goto leave;

  case CHECK_INV:
        check_inv (victim, trap);
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
    if (victim->type != PLAYER && trap->stats.food > 0)
      goto leave; /* monsters musn't apply magic_mouths with counters */
    apply_sign (victim, trap, 1);
    goto leave;

  case CONTAINER:
    if (victim->type==PLAYER)
      (void) esrv_apply_container (victim, trap);
    else
      (void) apply_container (victim, trap);
    goto leave;

  case RUNE:
  case TRAP:
    if (trap->level && QUERY_FLAG (victim, FLAG_ALIVE))
    {
        spring_trap(trap, victim);
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

/**
 * Handles reading a regular (ie not containing a spell) book.
 */
static void apply_book (object *op, object *tmp)
{
    int lev_diff;
    object *skill_ob;
    event *evt;

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
    skill_ob = find_skill_by_name(op, tmp->skill);
    if ( ! skill_ob) {
	new_draw_info(NDI_UNIQUE, 0,op,
		      "You are unable to decipher the strange symbols.");
	return;
    }
    lev_diff = tmp->level - (skill_ob->level + 5);
    if ( ! QUERY_FLAG (op, FLAG_WIZ) && lev_diff > 0) {
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


    /* GROS: Handle for plugin trigger event */
    if ((evt = find_event(tmp, EVENT_APPLY)) != NULL)
    {
        CFParm CFP;
        int k, l, m;
        uint32 n;
        new_draw_info_format (NDI_UNIQUE, 0, op,
                      "You open the %s and start reading.", tmp->name);
        k = EVENT_APPLY;
        l = SCRIPT_FIX_ALL;
        m = 0;
        n = 0;
        CFP.Value[0] = &k;
        CFP.Value[1] = op;
        CFP.Value[2] = tmp;
        CFP.Value[3] = NULL;
        CFP.Value[4] = NULL;
        CFP.Value[5] = &n;
        CFP.Value[6] = &m;
        CFP.Value[7] = &m;
        CFP.Value[8] = &l;
        CFP.Value[9] = evt->hook;
        CFP.Value[10]= evt->options;
        if (findPlugin(evt->plugin)>=0)
            ((PlugList[findPlugin(evt->plugin)].eventfunc) (&CFP));
    }
    else{
    	readable_message_type* msgType = get_readable_message_type(tmp);
    	draw_ext_info_format(NDI_UNIQUE | NDI_NAVY, 0, op,
                msgType->message_type, msgType->message_subtype,
                "You open the %s and start reading.\n%s", 
                "%s\n%s",
                long_desc(tmp,op), tmp->msg);
    }

    /* gain xp from reading */
    if(!QUERY_FLAG(tmp,FLAG_NO_SKILL_IDENT)) { /* only if not read before */
	int exp_gain=calc_skill_exp(op,tmp, skill_ob);
      if(!QUERY_FLAG(tmp,FLAG_IDENTIFIED)) {
        /*exp_gain *= 2; because they just identified it too */
        SET_FLAG(tmp,FLAG_IDENTIFIED);
        /* If in a container, update how it looks */
        if(tmp->env) esrv_update_item(UPD_FLAGS|UPD_NAME, op,tmp);
        else op->contr->socket.update_look=1;
      }
      change_exp(op,exp_gain, skill_ob->skill, 0);
      SET_FLAG(tmp,FLAG_NO_SKILL_IDENT); /* so no more xp gained from this book */
    }
}

/**
 * Handles the applying of a skill scroll, calling learn_skill straight.
 * op is the person learning the skill, tmp is the skill scroll object
 */
static void apply_skillscroll (object *op, object *tmp)
{
    switch ((int) learn_skill (op, tmp)) {
	case 0:
	    new_draw_info(NDI_UNIQUE, 0,op,"You already possess the knowledge ");
	    new_draw_info_format(NDI_UNIQUE, 0,op,"held within the %s.\n",query_name(tmp));
	    return;

	case 1:
	    new_draw_info_format(NDI_UNIQUE, 0,op,"You succeed in learning %s",
			 tmp->skill);
	    new_draw_info_format(NDI_UNIQUE, 0, op,
			 "Type 'bind ready_skill %s",tmp->skill);
	    new_draw_info(NDI_UNIQUE, 0,op,"to store the skill in a key.");
	    decrease_ob(tmp);
	    return;

	default:
	    new_draw_info_format(NDI_UNIQUE,0,op,
		    "You fail to learn the knowledge of the %s.\n",query_name(tmp));
	    decrease_ob(tmp);
	    return;
    }
}

/**
 * Actually makes op learn spell.
 * Informs player of what happens.
 */
void do_learn_spell (object *op, object *spell, int special_prayer)
{
    object *tmp;

    if (op->type != PLAYER) {
        LOG (llevError, "BUG: do_learn_spell(): not a player\n");
        return;
    }

    /* Upgrade special prayers to normal prayers */
    if ((tmp=check_spell_known (op, spell->name))!=NULL) {
        if (special_prayer && !QUERY_FLAG(tmp, FLAG_STARTEQUIP)) {
            LOG (llevError, "BUG: do_learn_spell(): spell already known, but not marked as startequip\n");
            return;
        }
        return;
    }

    play_sound_player_only (op->contr, SOUND_LEARN_SPELL, 0, 0);
    tmp = get_object();
    copy_object(spell, tmp);
    insert_ob_in_ob(tmp, op);
    
    if (special_prayer) {
	SET_FLAG(tmp, FLAG_STARTEQUIP);
    }

    new_draw_info_format (NDI_UNIQUE, 0, op, 
            "Type 'bind cast %s", spell->name);
    new_draw_info (NDI_UNIQUE, 0, op, "to store the spell in a key.");
}

/**
 * Erases spell from player's inventory.
 */
void do_forget_spell (object *op, const char *spell)
{
    object *spob;

    if (op->type != PLAYER) {
        LOG (llevError, "BUG: do_forget_spell(): not a player\n");
        return;
    }
    if ( (spob=check_spell_known (op, spell)) == NULL) {
        LOG (llevError, "BUG: do_forget_spell(): spell not known\n");
        return;
    }
    
    new_draw_info_format (NDI_UNIQUE|NDI_NAVY, 0, op,
			  "You lose knowledge of %s.", spell);
    remove_ob(spob);
    free_object(spob);
}

/**
 * Handles player applying a spellbook.
 * Checks whether player has knowledge of required skill, doesn't already know the spell,
 * stuff like that. Random learning failure too.
 */
static void apply_spellbook (object *op, object *tmp)
{
    object *skop, *spell, *spell_skill;

    if(QUERY_FLAG(op, FLAG_BLIND)&&!QUERY_FLAG(op,FLAG_WIZ)) {
	new_draw_info(NDI_UNIQUE, 0,op,"You are unable to read while blind.");
	return;
    }

    /* artifact_spellbooks have 'slaying' field point to a spell name,
     * instead of having their spell stored in stats.sp.  These are
     * legacy spellbooks
     */
 
    if(tmp->slaying != NULL) {
	spell=arch_to_object(find_archetype_by_object_name(tmp->slaying));
	if (!spell) {
	    new_draw_info_format(NDI_UNIQUE, 0, op,
		"The book's formula for %s is incomplete", tmp->slaying);
	    return;
	}
	else
	    insert_ob_in_ob(spell, tmp);
	free_string(tmp->slaying);
	tmp->slaying=NULL;
    } 

    skop = find_skill_by_name(op, tmp->skill);

    /* need a literacy skill to learn spells. Also, having a literacy level
     * lower than the spell will make learning the spell more difficult */
    if ( !skop) {
	new_draw_info(NDI_UNIQUE, 0,op,"You can't read! Your attempt fails.");
	return;
    }

    spell = tmp->inv;
    if (!spell) {
	LOG(llevError,"apply_spellbook: Book %s has no spell in it!\n", tmp->name);
	new_draw_info(NDI_UNIQUE, 0,op,"The spellbook symbols make no sense.");
    return;
    }
    if (spell->level > (skop->level+10)) {
	new_draw_info(NDI_UNIQUE, 0,op,"You are unable to decipher the strange symbols.");
	return;
    } 

    new_draw_info_format(NDI_UNIQUE, 0, op, 
	"The spellbook contains the %s level spell %s.",
            get_levelnumber(spell->level), spell->name);

    if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED)) {
	identify(tmp);
	if (tmp->env)
	    esrv_update_item(UPD_FLAGS|UPD_NAME,op,tmp);
	else
	    op->contr->socket.update_look=1;
    }

    /* I removed the check for special_prayer_mark here - it didn't make
     * a lot of sense - special prayers are not found in spellbooks, and
     * if the player doesn't know the spell, doesn't make a lot of sense that
     * they would have a special prayer mark.
     */
    if (check_spell_known (op, spell->name)) {
	new_draw_info(NDI_UNIQUE, 0,op,"You already know that spell.\n");
	return;
    }

    if (spell->skill) {
	spell_skill = find_skill_by_name(op, spell->skill);
	if (!spell_skill) {
	    new_draw_info_format(NDI_UNIQUE, 0, op, 
				 "You lack the skill %s to use this spell",
				 spell->skill);
	    return;
	}
	if (spell_skill->level < spell->level) {
	    new_draw_info_format(NDI_UNIQUE, 0, op, 
				 "You need to be level %d in %s to learn this spell.",
				 spell->level, spell->skill);
	    return;
	}
    }

    /* Logic as follows
     *
     *  1- MU spells use Int to learn, Cleric spells use Wisdom
     *
     *  2- The learner's skill level in literacy adjusts the chance to learn
     *     a spell.
     *
     *  3 -Automatically fail to learn if you read while confused
     * 
     * Overall, chances are the same but a player will find having a high 
     * literacy rate very useful!  -b.t. 
     */ 
    if(QUERY_FLAG(op,FLAG_CONFUSED)) { 
	new_draw_info(NDI_UNIQUE,0,op,"In your confused state you flub the wording of the text!");
	scroll_failure(op, 0 - random_roll(0, spell->level, op, PREFER_LOW), MAX(spell->stats.sp, spell->stats.grace));
    } else if(QUERY_FLAG(tmp,FLAG_STARTEQUIP) || 
	(random_roll(0, 100, op, PREFER_LOW)-(5*skop->level)) <
	      learn_spell[spell->stats.grace ? op->stats.Wis : op->stats.Int]) {

	new_draw_info(NDI_UNIQUE, 0,op,"You succeed in learning the spell!");
	do_learn_spell (op, spell, 0);

	/* xp gain to literacy for spell learning */
	if ( ! QUERY_FLAG (tmp, FLAG_STARTEQUIP))
	    change_exp(op,calc_skill_exp(op,tmp,skop), skop->skill, 0);
    } else {
	play_sound_player_only(op->contr, SOUND_FUMBLE_SPELL,0,0);
	new_draw_info(NDI_UNIQUE, 0,op,"You fail to learn the spell.\n");
    }
    decrease_ob(tmp);
}

/**
 * Handles applying a spell scroll.
 */
void apply_scroll (object *op, object *tmp, int dir)
{
    object *skop;

    if(QUERY_FLAG(op, FLAG_BLIND)&&!QUERY_FLAG(op,FLAG_WIZ)) {
	new_draw_info(NDI_UNIQUE, 0,op, "You are unable to read while blind.");
	return;
    }

    if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED)) 
	identify(tmp);

    if (!tmp->inv || tmp->inv->type != SPELL) {
        new_draw_info (NDI_UNIQUE, 0, op,
                       "The scroll just doesn't make sense!");
        return;
    }

    if(op->type==PLAYER) {
	/* players need a literacy skill to read stuff! */
	int exp_gain=0;

	/* hard code literacy - tmp->skill points to where the exp
	 * should go for anything killed by the spell.
	 */
	skop = find_skill_by_name(op, skill_names[SK_LITERACY]);

        if ( ! skop) {
	    new_draw_info(NDI_UNIQUE, 0,op,
		  "You are unable to decipher the strange symbols.");
	    return;
        } 

	if((exp_gain = calc_skill_exp(op,tmp, skop)))
	    change_exp(op,exp_gain, skop->skill, 0);
    }

    new_draw_info_format(NDI_BLACK, 0, op,
		     "The scroll of %s turns to dust.", tmp->inv->name);


    cast_spell(op,tmp,dir,tmp->inv, NULL);
    decrease_ob(tmp);
}

/**
 * Applies a treasure object - by default, chest.  op
 * is the person doing the applying, tmp is the treasure
 * chest.
 */
static void apply_treasure (object *op, object *tmp)
{
    object *treas;
    tag_t tmp_tag = tmp->count, op_tag = op->count;


    /* Nice side effect of new treasure creation method is that the treasure
     * for the chest is done when the chest is created, and put into the chest
     * inventory.  So that when the chest burns up, the items still exist.  Also
     * prevents people fromt moving chests to more difficult maps to get better
     * treasure
     */

    treas = tmp->inv;
    if(treas==NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"The chest was empty.");
	decrease_ob(tmp);
	return;
    }
    while (tmp->inv) {
	treas = tmp->inv;

	remove_ob(treas);
	new_draw_info_format(NDI_UNIQUE, 0, op, "You find %s in the chest.",
			   query_name(treas));

	treas->x=op->x;
	treas->y=op->y;
	treas = insert_ob_in_map (treas, op->map, op,INS_BELOW_ORIGINATOR);

	if (treas && (treas->type == RUNE || treas->type == TRAP) && treas->level
	    && QUERY_FLAG (op, FLAG_ALIVE))
	    spring_trap (treas, op);
	/* If either player or container was destroyed, no need to do
	 * further processing.  I think this should be enclused with
	 * spring trap above, as I don't think there is otherwise
	 * any way for the treasure chest or player to get killed
	 */
	if (was_destroyed (op, op_tag) || was_destroyed (tmp, tmp_tag))
	    break;
    }

    if ( ! was_destroyed (tmp, tmp_tag) && tmp->inv == NULL)
      decrease_ob (tmp);

}

/**
 * op eats food.
 * If player, takes care of messages and dragon special food.
 */
static void apply_food (object *op, object *tmp)
{
    int capacity_remaining;

    if(op->type!=PLAYER)
      op->stats.hp=op->stats.maxhp;
    else {
      /* check if this is a dragon (player), eating some flesh */
      if (tmp->type==FLESH && is_dragon_pl(op) && dragon_eat_flesh(op, tmp))
	;
      else {
	/* usual case - no dragon meal: */
	if(op->stats.food+tmp->stats.food>999) {
	  if(tmp->type==FOOD || tmp->type==FLESH)
	    new_draw_info(NDI_UNIQUE, 0,op,"You feel full, but what a waste of food!");
	  else
	    new_draw_info(NDI_UNIQUE, 0,op,"Most of the drink goes down your face not your throat!");
	}
      
	if(!QUERY_FLAG(tmp, FLAG_CURSED)) {
	  char buf[MAX_BUF];
	  
	  if (!is_dragon_pl(op)) {
	    /* eating message for normal players*/
	    if(tmp->type==DRINK)
	      sprintf(buf,"Ahhh...that %s tasted good.",tmp->name);
	    else 
	      sprintf(buf,"The %s tasted %s",tmp->name,
		      tmp->type==FLESH?"terrible!":"good.");
	  }
	  else {
	    /* eating message for dragon players*/
	    sprintf(buf,"The %s tasted terrible!",tmp->name);
	  }

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
	if(tmp->title || QUERY_FLAG(tmp,FLAG_CURSED))
	  eat_special_food(op,tmp);
      }
    }
    decrease_ob(tmp);
}

/**
 * A dragon is eating some flesh. If the flesh contains resistances,
 * there is a chance for the dragon's skin to get improved.
 *
 * attributes:
 *     object *op        the object (dragon player) eating the flesh
 *     object *meal      the flesh item, getting chewed in dragon's mouth
 * return:
 *     int               1 if eating successful, 0 if it doesn't work
 */
int dragon_eat_flesh(object *op, object *meal) {
  object *skin = NULL;    /* pointer to dragon skin force*/
  object *abil = NULL;    /* pointer to dragon ability force*/
  object *tmp = NULL;     /* tmp. object */
  
  char buf[MAX_BUF];            /* tmp. string buffer */
  double chance;                /* improvement-chance of one resistance type */
  double totalchance=1;         /* total chance of gaining one resistance */
  double bonus=0;               /* level bonus (improvement is easier at lowlevel) */
  double mbonus=0;              /* monster bonus */
  int atnr_winner[NROFATTACKS]; /* winning candidates for resistance improvement */
  int winners=0;                /* number of winners */
  int i;                        /* index */
  
  /* let's make sure and doublecheck the parameters */
  if (meal->type!=FLESH || !is_dragon_pl(op))
    return 0;
  
  /* now grab the 'dragon_skin'- and 'dragon_ability'-forces
     from the player's inventory */
  for (tmp=op->inv; tmp!=NULL; tmp=tmp->below) {
    if (tmp->type == FORCE) {
      if (strcmp(tmp->arch->name, "dragon_skin_force")==0)
	skin = tmp;
      else if (strcmp(tmp->arch->name, "dragon_ability_force")==0)
	abil = tmp;
    }
  }
  
  /* if either skin or ability are missing, this is an old player
     which is not to be considered a dragon -> bail out */
  if (skin == NULL || abil == NULL) return 0;
  
  /* now start by filling stomache and health, according to food-value */
  if((999 - op->stats.food) < meal->stats.food)
    op->stats.hp += (999 - op->stats.food) / 50;
  else
    op->stats.hp += meal->stats.food/50;
  if(op->stats.hp>op->stats.maxhp)
    op->stats.hp=op->stats.maxhp;
  
  op->stats.food = MIN(999, op->stats.food + meal->stats.food);
  
  /*printf("-> player: %d, flesh: %d\n", op->level, meal->level);*/
  
  /* on to the interesting part: chances for adding resistance */
  for (i=0; i<NROFATTACKS; i++) {
    if (meal->resist[i] > 0 && atnr_is_dragon_enabled(i)) {
      /* got positive resistance, now calculate improvement chance (0-100) */
      
      /* this bonus makes resistance increase easier at lower levels */
      bonus = (settings.max_level - op->level) * 30. / ((double)settings.max_level);
      if (i == abil->stats.exp)
	bonus += 5;  /* additional bonus for resistance of ability-focus */
      
      /* monster bonus increases with level, because high-level
         flesh is too rare */
      mbonus = op->level * 20. / ((double)settings.max_level);
      
      chance = (((double)MIN(op->level+bonus, meal->level+bonus+mbonus))*100. /
		((double)settings.max_level)) - skin->resist[i];
      
      if (chance >= 0.)
	chance += 1.;
      else
	chance = (chance < -12) ? 0. : 1./pow(2., -chance);
      
      /* chance is proportional to amount of resistance (max. 50) */
      chance *= ((double)(MIN(meal->resist[i], 50)))/50.;
      
      /* doubled chance for resistance of ability-focus */
      if (i == abil->stats.exp)
	chance = MIN(100., chance*2.);
      
      /* now make the throw and save all winners (Don't insert luck bonus here!) */
      if (RANDOM()%10000 < (int)(chance*100)) {
	atnr_winner[winners] = i;
	winners++;
      }
      
      if (chance >= 0.01 ) totalchance *= 1 - chance/100;
      
      /*printf("   %s: bonus %.1f, chance %.1f\n", attacks[i], bonus, chance);*/
    }
  }
  
  /* inverse totalchance as until now we have the failure-chance   */
  totalchance = 100 - totalchance*100;
  /* print message according to totalchance */
  if (totalchance > 50.)
    sprintf(buf, "Hmm! The %s tasted delicious!", meal->name);
  else if (totalchance > 10.)
    sprintf(buf, "The %s tasted very good.", meal->name);
  else if (totalchance > 1.)
    sprintf(buf, "The %s tasted good.", meal->name);
  else if (totalchance > 0.1)
    sprintf(buf, "The %s tasted bland.", meal->name);
  else if (totalchance >= 0.01)
    sprintf(buf, "The %s had a boring taste.", meal->name);
  else if (meal->last_eat > 0 && atnr_is_dragon_enabled(meal->last_eat))
    sprintf(buf, "The %s tasted strange.", meal->name);
  else
    sprintf(buf, "The %s had no taste.", meal->name);
  new_draw_info(NDI_UNIQUE, 0, op, buf);
  
  /* now choose a winner if we have any */
  i = -1;
  if (winners>0)
    i = atnr_winner[RANDOM()%winners];
  
  if (i >= 0 && i < NROFATTACKS && skin->resist[i] < 95) {
    /* resistance increased! */
    skin->resist[i]++;
    fix_player(op);
    
    sprintf(buf, "Your skin is now more resistant to %s!", change_resist_msg[i]);
    new_draw_info(NDI_UNIQUE|NDI_RED, 0, op, buf);
  }
  
  /* if this flesh contains a new ability focus, we mark it
     into the ability_force and it will take effect on next level */
  if (meal->last_eat > 0 && atnr_is_dragon_enabled(meal->last_eat)
      && meal->last_eat != abil->last_eat) {
    abil->last_eat = meal->last_eat; /* write: last_eat <new attnr focus> */
    
    if (meal->last_eat != abil->stats.exp) {
      sprintf(buf, "Your metabolism prepares to focus on %s!",
	      change_resist_msg[meal->last_eat]);
      new_draw_info(NDI_UNIQUE, 0, op, buf);
      sprintf(buf, "The change will happen at level %d", abil->level + 1);
      new_draw_info(NDI_UNIQUE, 0, op, buf);
    }
    else {
      sprintf(buf, "Your metabolism will continue to focus on %s.",
	      change_resist_msg[meal->last_eat]);
      new_draw_info(NDI_UNIQUE, 0, op, buf);
      abil->last_eat = 0;
    }
  }
  return 1;
}

static void apply_savebed (object *pl)
{
    if(!pl->contr->name_changed||!pl->stats.exp) {
      new_draw_info(NDI_UNIQUE, 0,pl,"You don't deserve to save your character yet.");
      return;
    }
    if(QUERY_FLAG(pl,FLAG_WAS_WIZ)) {
      new_draw_info(NDI_UNIQUE, 0,pl,"Since you have cheated you can't save.");
      return;
    }
    /* Need to call terminate_all_pets()  before we remove the player ob */
    terminate_all_pets(pl);
    remove_ob(pl);
    pl->direction=0;
    new_draw_info_format(NDI_UNIQUE | NDI_ALL | NDI_DK_ORANGE, 5, pl,
	"%s leaves the game.",pl->name);
    
    /* update respawn position */
    strcpy(pl->contr->savebed_map, pl->map->path);
    pl->contr->bed_x = pl->x;
    pl->contr->bed_y = pl->y;
    
    strcpy(pl->contr->killer,"left");
    check_score(pl); /* Always check score */
    (void)save_player(pl,0);
    pl->map->players--;
#if MAP_MAXTIMEOUT 
    MAP_SWAP_TIME(pl->map) = MAP_TIMEOUT(pl->map);
#endif
    play_again(pl);
    pl->speed = 0;
    update_ob_speed(pl);
}

/**
 * Handles applying an improve armor scroll.
 * Does some sanity checks, then calls improve_armour.
 */
static void apply_armour_improver (object *op, object *tmp)
{
    object *armor;

    if (get_map_flags(op->map,NULL, op->x,op->y,NULL, NULL) & P_NO_MAGIC) {
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

/**
 * This fonction return true if the exit is not a 2 ways one or it is 2 ways, valid exit.
 * A valid 2 way exit means:
 *   -You can come back (there is another exit at the other side)
 *   -You are
 *         � the owner of the exit
 *         � or in the same party as the owner
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
   if (!has_been_loaded(EXIT_PATH(exit)) && exit->race) return 0; /* This is a reset town portal */
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


/**
 * Main apply handler.
 *
 * Checks for unpaid items before applying.
 *
 * Return value:
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
  event *evt;

  if (tmp->head) tmp=tmp->head;

  if (QUERY_FLAG (tmp, FLAG_UNPAID) && ! QUERY_FLAG (tmp, FLAG_APPLIED)) {
    if (op->type == PLAYER) {
      new_draw_info (NDI_UNIQUE, 0, op, "You should pay for it first.");
      return 1;
    } else {
      return 0;   /* monsters just skip unpaid items */
    }
  }

  /* monsters mustn't apply random chests, nor magic_mouths with a counter */
  if (op->type != PLAYER && tmp->type == TREASURE)
    return 0;

  /* GROS: Handle for plugin trigger event */
  if ((evt = find_event(tmp, EVENT_APPLY)) != NULL)
  {
    CFParm CFP;
    CFParm* CFR;
    int k, l, m;
    uint32 n;
    m = 0;
    n = aflag;
    k = EVENT_APPLY;
    l = SCRIPT_FIX_ALL;
    CFP.Value[0] = &k;
    CFP.Value[1] = op;
    CFP.Value[2] = tmp;
    CFP.Value[3] = NULL;
    CFP.Value[4] = NULL;
    CFP.Value[5] = &n;
    CFP.Value[6] = &m;
    CFP.Value[7] = &m;
    CFP.Value[8] = &l;
    CFP.Value[9] = evt->hook;
    CFP.Value[10]= evt->options;
    if (findPlugin(evt->plugin)>=0)
    {
        CFR = (PlugList[findPlugin(evt->plugin)].eventfunc) (&CFP);
        rtn_script = *(int *)(CFR->Value[0]);
        if (rtn_script!=0) return 1;
    }
  }
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
    apply_sign (op, tmp, 0);
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
    apply_scroll (op, tmp, 0);
    return 1;

  case POTION:
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
  case LAMP:
  case BUILDER:
  case SKILL_TOOL:
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
	timeofday_t tod;

	get_tod(&tod);
	sprintf(buf, "It is %d minute%s past %d o'clock %s",
	  tod.minute+1, ((tod.minute+1 < 2) ? "" : "s"),
	  ((tod.hour % 14 == 0) ? 14 : ((tod.hour)%14)),
	  ((tod.hour >= 14) ? "pm" : "am"));
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

  case GPS:
    apply_positioning_system( op, tmp );
    return 1;

  case ITEM_TRANSFORMER:
    apply_item_transformer( op, tmp );
    return 1;
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

/**
 * player_apply_below attempts to apply the object 'below' the player.
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

/**
 * Unapplies specified item.
 * No check done on cursed/damned.
 * Break this out of apply_special - this is just done
 * to keep the size of apply_special to a more managable size.
 */
static int unapply_special (object *who, object *op, int aflags)
{
    object *tmp2;

    CLEAR_FLAG(op, FLAG_APPLIED);
    switch(op->type) {
	case WEAPON:
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You unwield %s.",query_name(op));

	    (void) change_abil (who,op);
	    if(QUERY_FLAG(who,FLAG_READY_WEAPON))
		CLEAR_FLAG(who,FLAG_READY_WEAPON);
	    /* GROS: update the current_weapon_script field (used with script_attack for weapons) */
	    who->current_weapon_script = NULL;
	    who->current_weapon = NULL;
	    clear_skill(who);
	    break;

	case SKILL:         /* allows objects to impart skills */
	case SKILL_TOOL:
	    if (op != who->chosen_skill) {
		LOG (llevError, "BUG: apply_special(): applied skill is not a chosen skill\n");
	    }
	    if (who->type==PLAYER) {
		if (who->contr->shoottype == range_skill)
		    who->contr->shoottype = range_none;
		if ( ! op->invisible) {
		    new_draw_info_format (NDI_UNIQUE, 0, who,
                                    "You stop using the %s.", query_name(op));
		} else {
		    new_draw_info_format (NDI_UNIQUE, 0, who,
                                    "You can no longer use the skill: %s.",
                                    op->skill);
		}
	    }
	    (void) change_abil (who, op);
	    who->chosen_skill = NULL;
	    CLEAR_FLAG (who, FLAG_READY_SKILL);
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
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You unwear %s.",query_name(op));
	    (void) change_abil (who,op);
	    break;
        case LAMP:
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You turn off your %s.",
				 op->name);
	    tmp2 = arch_to_object(op->other_arch);
	    tmp2->x = op->x;
	    tmp2->y = op->y;
	    tmp2->map = op->map;
	    tmp2->below = op->below;
	    tmp2->above = op->above;
	    tmp2->stats.food = op->stats.food;
	    CLEAR_FLAG(tmp2, FLAG_APPLIED);
	    if (who->type == PLAYER)
		esrv_del_item(who->contr, (tag_t)op->count);
	    remove_ob(op);
	    free_object(op);
	    insert_ob_in_ob(tmp2, who);
	    fix_player(who);
	    if (QUERY_FLAG(op, FLAG_CURSED) || QUERY_FLAG(op, FLAG_DAMNED)) {
		if (who->type == PLAYER) {
		    new_draw_info(NDI_UNIQUE, 0,who, "Oops, it feels deadly cold!");
		    SET_FLAG(tmp2, FLAG_KNOWN_CURSED);
		}
	    }
	    if(who->type==PLAYER)
		esrv_send_item(who, tmp2);
	    return 1; /* otherwise, an attempt to drop causes problems */
	    break;
	case BOW:
	case WAND:
	case ROD:
	case HORN:
	    clear_skill(who);
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You unready %s.",query_name(op));
	    if(who->type==PLAYER) {
		who->contr->shoottype = range_none;
	    } else {
		if (op->type == BOW)
		    CLEAR_FLAG (who, FLAG_READY_BOW);
		else 
		    CLEAR_FLAG(who, FLAG_READY_RANGE);
	    }
	    break;

    case BUILDER:
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You unready %s.",query_name(op));
        who->contr->shoottype = range_none;
        who->contr->ranges[ range_builder ] = NULL;
        break;

	default:
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You unapply %s.",query_name(op));
	    break;
    }

    fix_player(who);

    if ( ! (aflags & AP_NO_MERGE)) {
	object *tmp;

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

/**
 * Returns the object that is using location 'loc'.
 * Note that 'start' is the first object to start examing - we
 * then go through the below of this.  In this way, you can do
 * something like:
 * tmp = get_item_from_body_location(who->inv, 1);
 * if (tmp) tmp1 = get_item_from_body_location(tmp->below, 1);
 * to find the second object that may use this location, etc.
 * Returns NULL if no match is found.
 * loc is the index into the array we are looking for a match.
 * don't return invisible objects unless they are skill objects
 * invisible other objects that use
 * up body locations can be used as restrictions.
 */
object *get_item_from_body_location(object *start, int loc)
{
    object *tmp;

    if (!start) return NULL;

    for (tmp=start; tmp; tmp=tmp->below)
	if (QUERY_FLAG(tmp, FLAG_APPLIED) && tmp->body_info[loc] && 
	    (!tmp->invisible || tmp->type==SKILL)) return tmp;

    return NULL;
}



/**
 * 'op' wants to apply an object, but can't because of other equipment.
 * This should only be called when it is known
 * that there are objects to unapply.  This makes pretty heavy
 * use of get_item_from_body_location.  It makes no intelligent choice
 * on objects - rather, the first that is matched is used.
 * Returns 0 on success, returns 1 if there is some problem.
 * if aflags is AP_PRINT, we instead print out waht to unapply
 * instead of doing it.  This is a lot less code than having
 * another function that does just that.
 */
int unapply_for_ob(object *who, object *op, int aflags)
{
    int i;
    object *tmp=NULL, *last;

    /* If we are applying a shield or weapon, unapply any equipped shield
     * or weapons first - only allowed to use one weapon/shield at a time.
     */
    if (op->type == WEAPON || op->type == SHIELD) {
        for (tmp=who->inv; tmp; tmp=tmp->below) {
	    if (QUERY_FLAG(tmp, FLAG_APPLIED) && tmp->type == op->type) {
		if ((aflags & AP_IGNORE_CURSE) ||  (aflags & AP_PRINT) ||
		    (!QUERY_FLAG(tmp, FLAG_CURSED) && !QUERY_FLAG(tmp, FLAG_DAMNED))) {
		    if (aflags & AP_PRINT) 
			new_draw_info(NDI_UNIQUE, 0, who, query_name(tmp));
		    else
			unapply_special(who, tmp, aflags);
		}
		else {
		    /* In this case, we want to try and remove a cursed item.
		     * While we know it won't work, we want unapply_special to
		     * at least generate the message.
		     */
		    new_draw_info_format(NDI_UNIQUE, 0, who,
				 "No matter how hard you try, you just can't\nremove %s.",
				 query_name(tmp));
		    return 1;
		}

	    }
	}
    }

    for (i=0; i<NUM_BODY_LOCATIONS; i++) {
	/* this used up a slot that we need to free */
	if (op->body_info[i]) {
	    last = who->inv;

	    /* We do a while loop - may need to remove several items in order
	     * to free up enough slots.
	     */
	    while ((who->body_used[i] + op->body_info[i]) < 0) {
		tmp = get_item_from_body_location(last, i);
		if (!tmp) {
#if 0
		    /* Not a bug - we'll get this if the player has cursed items
		     * equipped.
		     */
		    LOG(llevError,"Can't find object using location %d (%s) on %s\n", 
			i, body_locations[i].save_name, who->name);
#endif
		    return 1;
		}
		/* If we are just printing, we don't care about cursed status */
		if ((aflags & AP_IGNORE_CURSE) ||  (aflags & AP_PRINT) ||
		    (!(QUERY_FLAG(tmp, FLAG_CURSED) && !QUERY_FLAG(tmp, FLAG_DAMNED)))) {
		    if (aflags & AP_PRINT) 
			new_draw_info(NDI_UNIQUE, 0, who, query_name(tmp));
		    else
			unapply_special(who, tmp, aflags);
		}
		else {
		    /* Cursed item that we can't unequip - tell the player.
		     * Note this could be annoying if this is just one of a few,
		     * so it may not be critical (eg, putting on a ring and you have
		     * one cursed ring.)
		     */
		    new_draw_info_format(NDI_UNIQUE, 0, who, "The %s just won't come off", query_name(tmp));
		}
		last = tmp->below;
	    }
	    /* if we got here, this slot is freed up - otherwise, if it wasn't freed up, the
	     * return in the !tmp would have kicked in.
	     */
	} /* if op is using this body location */
    } /* for body lcoations */
    return 0;
}

/**
 * Checks to see if 'who' can apply object 'op'.
 * Returns 0 if apply can be done without anything special.
 * Otherwise returns a bitmask - potentially several of these may be
 * set, but largely depends on circumstance - in the future, processing
 * may be  pruned once we know some status (eg, once CAN_APPLY_NEVER
 * is set, do we really are what the other flags may be?)
 *
 * See include/define.h for detailed description of the meaning of
 * these return values.
 */
int can_apply_object(object *who, object *op)
{
    int i, retval=0;
    object *tmp=NULL, *ws=NULL;

    /* Players have 2 'arm's, so they could in theory equip 2 shields or
     * 2 weapons, but we don't want to let them do that.  So if they are
     * trying to equip a weapon or shield, see if they already have one
     * in place and store that way.
     */
    if (op->type == WEAPON || op->type == SHIELD) {
        for (tmp=who->inv; tmp && !ws; tmp=tmp->below) {
	    if (QUERY_FLAG(tmp, FLAG_APPLIED) && tmp->type == op->type) {
		retval = CAN_APPLY_UNAPPLY;
		ws = tmp;
	    }
	}
    }
	

    for (i=0; i<NUM_BODY_LOCATIONS; i++) {
	if (op->body_info[i]) {
	    /* Item uses more slots than we have */
	    if (FABS(op->body_info[i]) > who->body_info[i]) {
		/* Could return now for efficiently - rest of info below isn'
		 * really needed.
		 */
		retval |= CAN_APPLY_NEVER;
	    } else if ((who->body_used[i] + op->body_info[i]) < 0) {
		/* in this case, equipping this would use more free spots than
		 * we have.
		 */
		object *tmp1;


		/* if we have an applied weapon/shield, and unapply it would free
		 * enough slots to equip the new item, then just set this can
		 * continue.  We don't care about the logic below - if you have
		 * shield equipped and try to equip another shield, there is only
		 * one choice.  However, the check for the number of body locations 
		 * does take into the account cases where what is being applied
		 * may be two handed for example.
		 */
		if (ws) {
		    if ((who->body_used[i] - ws->body_info[i] + op->body_info[i]) >=0) {
			retval |= CAN_APPLY_UNAPPLY;
			continue;
		    }
		}

		tmp1 = get_item_from_body_location(who->inv, i);
		if (!tmp1) {
#if 0
		    /* This is sort of an error, but happens a lot when old players
		     * join in with more stuff equipped than they are now allowed.
		     */
		    LOG(llevError,"Can't find object using location %d on %s\n",
			i, who->name);
#endif
		    retval |= CAN_APPLY_NEVER;
		} else {
		    /* need to unapply something.  However, if this something
		     * is different than we had found before, it means they need
		     * to apply multiple objects
		     */
		    retval |= CAN_APPLY_UNAPPLY;
		    if (!tmp) tmp = tmp1;
		    else if (tmp != tmp1) {
			retval |= CAN_APPLY_UNAPPLY_MULT;
		    }
		    /* This object isn't using up all the slots, so there must
		     * be another.  If so, and it the new item doesn't need all
		     * the slots, the player then has a choice.  
		     */
		    if (((who->body_used[i] - tmp1->body_info[i]) != who->body_info[i]) &&
			(FABS(op->body_info[i]) < who->body_info[i]))
			retval |= CAN_APPLY_UNAPPLY_CHOICE;

		    /* Does unequippint 'tmp1' free up enough slots for this to be
		     * equipped?  If not, there must be something else to unapply.
		     */
		    if ((who->body_used[i] + op->body_info[i] - tmp1->body_info[i]) < 0)
			retval |= CAN_APPLY_UNAPPLY_MULT;

		}
	    } /* if not enough free slots */
	} /* if this object uses location i */
    } /* for i -> num_body_locations loop */

    /* Note that we don't check for FLAG_USE_ARMOUR - that should
     * really be controlled by use of body locations.  We do have
     * the weapon/shield checks, and the range checks for monsters,
     * because you can't control those just by body location - bows, shields,
     * and weapons all use the same slot.  Similar for horn/rod/wand - they
     * all use the same location.
     */
    if (op->type == WEAPON && !QUERY_FLAG(who,FLAG_USE_WEAPON))
	retval |= CAN_APPLY_RESTRICTION;
    if (op->type == SHIELD && !QUERY_FLAG(who,FLAG_USE_SHIELD))
	retval |= CAN_APPLY_RESTRICTION;


    if (who->type != PLAYER) {
	if ((op->type == WAND || op->type == HORN || op->type==ROD)
	    && !QUERY_FLAG(who, FLAG_USE_RANGE))
		retval |= CAN_APPLY_RESTRICTION;
	if (op->type == BOW && !QUERY_FLAG(who, FLAG_USE_BOW))
	    retval |= CAN_APPLY_RESTRICTION;
	if (op->type == RING && !QUERY_FLAG(who, FLAG_USE_RING))
	    retval |= CAN_APPLY_RESTRICTION;
	if (op->type == BOW && !QUERY_FLAG(who, FLAG_USE_BOW))
	    retval |= CAN_APPLY_RESTRICTION;
    }
    return retval;
}

		

/**
 * who is the object using the object.  It can be a monster
 * op is the object they are using.  op is an equipment type item,
 * eg, one which you put on and keep on for a while, and not something
 * like a potion or scroll.
 *
 * function returns 1 if the action could not be completed, 0 on
 * success.  However, success is a matter of meaning - if the
 * user passes the 'apply' flag to an object already applied,
 * nothing is done, and 0 is returned.
 *
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
{
    int basic_flag = aflags & AP_BASIC_FLAGS;
    object *tmp, *tmp2, *skop=NULL;
    event *evt;
    int i;

    if(who==NULL) {
	LOG(llevError,"apply_special() from object without environment.\n");
	return 1;
    }

    if(op->env!=who)
	return 1;   /* op is not in inventory */

    /* trying to unequip op */
    if (QUERY_FLAG(op,FLAG_APPLIED)) {
	/* always apply, so no reason to unapply */
	if (basic_flag == AP_APPLY) return 0;

	if ( ! (aflags & AP_IGNORE_CURSE)
	    && (QUERY_FLAG(op, FLAG_CURSED) || QUERY_FLAG(op, FLAG_DAMNED))) {
	    new_draw_info_format(NDI_UNIQUE, 0, who,
				 "No matter how hard you try, you just can't\nremove %s.",
				 query_name(op));
	    return 1;
	}
	return unapply_special(who, op, aflags);
    }

    if (basic_flag == AP_UNAPPLY) return 0;

    i = can_apply_object(who, op);

    /* Can't just apply this object.  Lets see what not and what to do */
    if (i) {
	if (i & CAN_APPLY_NEVER) {
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You don't have the body to use a %s\n", query_name(op));
	    return 1;
	} else if (i & CAN_APPLY_RESTRICTION) {
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You have a prohibition against using a %s\n", query_name(op));
	    return 1;
	}
	if (who->type != PLAYER) {
	    /* Some error, so don't try to equip something more */
	    if (unapply_for_ob(who, op, aflags)) return 1;
	} else {
	    if (who->contr->unapply == unapply_never || 
		(i & CAN_APPLY_UNAPPLY_CHOICE && who->contr->unapply == unapply_nochoice)) {
		new_draw_info(NDI_UNIQUE, 0, who, "You need to unapply some item(s):");
		unapply_for_ob(who, op, AP_PRINT);
		return 1;
	    }
	    else if (who->contr->unapply == unapply_always || !(i & CAN_APPLY_UNAPPLY_CHOICE)) {
		i = unapply_for_ob(who, op, aflags);
		if (i) return 1;
	    }
	}
    }
    if (op->skill && op->type != SKILL && op->type != SKILL_TOOL) {
	skop=find_skill_by_name(who, op->skill);
	if (!skop) {
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You need the %s skill to use this item!", op->skill);
	    return 1;
	} else {
	    /* While experience will be credited properly, we want to change the
	     * skill so that the dam and wc get updated 
	     */
	    change_skill(who, skop, 0);
	}
    }
	
    if (who->type == PLAYER && op->item_power && 
	(op->item_power + who->contr->item_power) > (settings.item_power_factor * who->level)) {
	new_draw_info(NDI_UNIQUE, 0, who, "Equipping that combined with other items would consume your soul!");
	return 1;
    }
	

    /* Ok.  We are now at the state where we can apply the new object.
     * Note that we don't have the checks for can_use_...
     * below - that is already taken care of by can_apply_object. 
     */
			  

    if(op->nrof > 1)
	tmp = get_split_ob(op,op->nrof - 1);
    else
	tmp = NULL;

    switch(op->type) {
	case WEAPON:
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
		/* (Ragnarok's sword attempted to be used by Foo: won't work) */
		new_draw_info(NDI_UNIQUE, 0,who,"The weapon does not recognize you as its owner.");
		if(tmp!=NULL)
		    (void) insert_ob_in_ob(tmp,who);
		return 1;
	    }
	    SET_FLAG(op, FLAG_APPLIED);

	    if (skop) change_skill(who, skop, 1);
	    if(!QUERY_FLAG(who,FLAG_READY_WEAPON))
		SET_FLAG(who, FLAG_READY_WEAPON);

	    new_draw_info_format(NDI_UNIQUE, 0, who, "You wield %s.",query_name(op));

	    (void) change_abil (who,op);
	    /* GROS: update the current_weapon_script field (used with EVENT_ATTACK for weapons) */
        if ((evt = find_event(op, EVENT_ATTACK)) != NULL) {
		LOG(llevDebug, "Scripting Weapon wielded\n");
		if (who->current_weapon_script) free_string(who->current_weapon_script);
		who->current_weapon_script=add_string(query_name(op));
	    }
	    who->current_weapon = op;
	    break;

	case ARMOUR:
	case HELMET:
	case SHIELD:
	case BOOTS:
	case GLOVES:
	case GIRDLE:
	case BRACERS:
	case CLOAK:
	case RING:
	case AMULET:
	    SET_FLAG(op, FLAG_APPLIED);
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You wear %s.",query_name(op));
	    (void) change_abil (who,op);
	    break;
        case LAMP:
	    if (op->stats.food < 1) {
		new_draw_info_format(NDI_UNIQUE, 0, who, "Your %s is out of"
				     " fuel!", op->name);
		return 1;
	    }
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You turn on your %s.",
				 op->name);
	    tmp2 = arch_to_object(op->other_arch);
	    tmp2->stats.food = op->stats.food;
	    SET_FLAG(tmp2, FLAG_APPLIED);
	    insert_ob_in_ob(tmp2, who);

	    /* Remove the old lantern */
	    if (who->type == PLAYER)
		esrv_del_item(who->contr, (tag_t)op->count);
	    remove_ob(op);
	    free_object(op);

	    /* insert the portion that was split off */
	    if(tmp!=NULL) {
		(void) insert_ob_in_ob(tmp,who);
		if(who->type==PLAYER)
		    esrv_send_item(who, tmp);
	    }
	    fix_player(who);
	    if (QUERY_FLAG(op, FLAG_CURSED) || QUERY_FLAG(op, FLAG_DAMNED)) {
		if (who->type == PLAYER) {
		    new_draw_info(NDI_UNIQUE, 0,who, "Oops, it feels deadly cold!");
		    SET_FLAG(tmp2, FLAG_KNOWN_CURSED);
		}
	    }
	    if(who->type==PLAYER)
		esrv_send_item(who, tmp2);
	    return 0;
	    break;

	/* this part is needed for skill-tools */ 
	case SKILL:
	case SKILL_TOOL:
	    if (who->chosen_skill) {
		LOG (llevError, "BUG: apply_special(): can't apply two skills\n");
		return 1;
	    }
	    if (who->type == PLAYER) {
		who->contr->shoottype = range_skill;
		who->contr->ranges[range_skill] = op;
		if ( ! op->invisible) {
		    new_draw_info_format (NDI_UNIQUE, 0, who, "You ready %s.",
                                  query_name (op));
		    new_draw_info_format (NDI_UNIQUE, 0, who,
			      "You can now use the skill: %s.",
				op->skill);
		} else {
		    new_draw_info_format (NDI_UNIQUE, 0, who, "Readied skill: %s.",
				      op->skill? op->skill:op->name);
		}
	    }
	    SET_FLAG (op, FLAG_APPLIED);
	    (void) change_abil (who, op);
	    who->chosen_skill = op;
	    SET_FLAG (who, FLAG_READY_SKILL);
	    break;
	
	case BOW:
	    if (!check_weapon_power(who, op->last_eat)) {
		new_draw_info(NDI_UNIQUE, 0, who,
		    "That item is too powerful for you to use.");
		new_draw_info(NDI_UNIQUE, 0, who, "It would consume your soul!.");
		if(tmp != NULL)
		    (void)insert_ob_in_ob(tmp,who);
		return 1;
	    }
	    if( op->level && (strncmp(op->name,who->name,strlen(who->name)))) {
		new_draw_info(NDI_UNIQUE, 0, who,
		    "The weapon does not recognize you as its owner.");
		if(tmp != NULL)
		    (void)insert_ob_in_ob(tmp,who);
		return 1;
	    }
	    /*FALLTHROUGH*/
	case WAND:
	case ROD:
	case HORN:
	    /* check for skill, alter player status */ 
	    SET_FLAG(op, FLAG_APPLIED);
	    if (skop) change_skill(who, skop, 0);
	    new_draw_info_format (NDI_UNIQUE, 0, who, "You ready %s.", query_name(op));

	    if(who->type==PLAYER) {
		if (op->type == BOW) {
		    (void)change_abil(who, op);
		    new_draw_info_format (NDI_UNIQUE, 0, who,
                              "You will now fire %s with %s.",
	                      op->race ? op->race : "nothing", query_name(op));
		    who->contr->shoottype = range_bow;
		} else {
		    who->contr->shoottype = range_misc;
		}
	    } else {
		if (op->type == BOW)
		    SET_FLAG (who, FLAG_READY_BOW);
		else
		    SET_FLAG (who, FLAG_READY_RANGE);
	    }
	    break;

    case BUILDER:
        if ( who->contr->ranges[ range_builder ] )
            unapply_special( who, who->contr->ranges[ range_builder ], 0 );
        who->contr->shoottype = range_builder;
        who->contr->ranges[ range_builder ] = op;
        new_draw_info_format( NDI_UNIQUE, 0, who, "You ready your %s.", query_name( op ) );
        break;

	default:
	    new_draw_info_format(NDI_UNIQUE, 0, who, "You apply %s.",query_name(op));
    } /* end of switch op->type */

    SET_FLAG(op, FLAG_APPLIED);

    if(tmp!=NULL)
	tmp = insert_ob_in_ob(tmp,who);

    fix_player(who);

    /* We exclude spell casting objects.  The fire code will set the
     * been applied flag when they are used - until that point,
     * you don't know anything about them.
     */
    if (who->type == PLAYER && op->type!=WAND && op->type!=HORN &&
	op->type!=ROD)
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

/**
 * Map was just loaded, handle op's initialisation.
 *
 * Generates shop floor's item, and treasures.
 */
int auto_apply (object *op) {
    object *tmp = NULL, *tmp2;
    int i;

    switch(op->type) {
	case SHOP_FLOOR:
	    if (!HAS_RANDOM_ITEMS(op)) return 0;
	    do {
		i=10; /* let's give it 10 tries */
		while((tmp=generate_treasure(op->randomitems,
					     op->stats.exp?(int)op->stats.exp:MAX(op->map->difficulty, 5)))==NULL&&--i);
		if(tmp==NULL)
		    return 0;
		if(QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) {
		    free_object(tmp);
		    tmp = NULL;
		}
	    } while(!tmp);
	    tmp->x=op->x;
	    tmp->y=op->y;
	    SET_FLAG(tmp,FLAG_UNPAID);
	    insert_ob_in_map(tmp,op->map,NULL,0);
	    CLEAR_FLAG(op,FLAG_AUTO_APPLY);
	    identify(tmp);
	    break;

	case TREASURE:
	    if (QUERY_FLAG(op,FLAG_IS_A_TEMPLATE))
		return 0;
	    while ((op->stats.hp--)>0)
		create_treasure(op->randomitems, op, op->map?GT_ENVIRONMENT:0,
				op->stats.exp ? (int)op->stats.exp : 
				op->map == NULL ?  14: op->map->difficulty,0);

	    /* If we generated an object and put it in this object inventory,
	     * move it to the parent object as the current object is about
	     * to disappear.  An example of this item is the random_* stuff
	     * that is put inside other objects.
	     */
	    for (tmp=op->inv; tmp; tmp=tmp2) {
		tmp2 = tmp->below;
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

/**
 * fix_auto_apply goes through the entire map (only the first time
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
			else if(invtmp->type==TREASURE && HAS_RANDOM_ITEMS(invtmp)) {
			    while ((invtmp->stats.hp--)>0)
				create_treasure(invtmp->randomitems, invtmp, 0,
						m->difficulty,0);
				invtmp->randomitems = NULL;
			}
			else if (invtmp && invtmp->arch && 
				invtmp->type!=TREASURE &&
				invtmp->type != SPELL && 
				invtmp->type != CLASS &&
				HAS_RANDOM_ITEMS(invtmp)) {
				    create_treasure(invtmp->randomitems, invtmp, 0,
						    m->difficulty,0);
				/* Need to clear this so that we never try to create 
				 * treasure again for this object
				 */
				invtmp->randomitems = NULL;
			}
		    }
		    /* This is really temporary - the code at the bottom will
		     * also set randomitems to null.  The problem is there are bunches
		     * of maps/players already out there with items that have spells
		     * which haven't had the randomitems set to null yet.
		     * MSW 2004-05-13
		     *
		     * And if it's a spellbook, it's better to set randomitems to NULL too,
		     * else you get two spells in the book ^_-
		     * Ryo 2004-08-16
		     */
		    if (tmp->type == WAND || tmp->type == ROD || tmp->type == SCROLL ||
			tmp->type == HORN || tmp->type == FIREWALL || tmp->type == POTION ||
			tmp->type == ALTAR || tmp->type == SPELLBOOK)
			    tmp->randomitems = NULL;

		}

		if(QUERY_FLAG(tmp,FLAG_AUTO_APPLY))
		    auto_apply(tmp);
		else if((tmp->type==TREASURE || (tmp->type==CONTAINER))&& HAS_RANDOM_ITEMS(tmp)) {
		    while ((tmp->stats.hp--)>0)
			create_treasure(tmp->randomitems, tmp, 0,
                            m->difficulty,0);
		    tmp->randomitems = NULL;
		}
		else if(tmp->type==TIMED_GATE) {
		    tmp->speed = 0;
		    update_ob_speed(tmp);
		}
		/* This function can be called everytime a map is loaded, even when
		 * swapping back in.  As such, we don't want to create the treasure
		 * over and ove again, so after we generate the treasure, blank out
		 * randomitems so if it is swapped in again, it won't make anything.
		 * This is a problem for the above objects, because they have counters
		 * which say how many times to make the treasure.
		 */
		else if(tmp && tmp->arch && tmp->type!=PLAYER && tmp->type!=TREASURE &&
		   tmp->type != SPELL && tmp->type != PLAYER_CHANGER && tmp->type != CLASS &&
		   HAS_RANDOM_ITEMS(tmp)) {
		    create_treasure(tmp->randomitems, tmp, GT_APPLY,
                            m->difficulty,0);
		    tmp->randomitems = NULL;
		}
	    }

    for(x=0;x<MAP_WIDTH(m);x++)
	for(y=0;y<MAP_HEIGHT(m);y++)
	    for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=tmp->above)
		if (tmp->above &&
		    (tmp->type == TRIGGER_BUTTON || tmp->type == TRIGGER_PEDESTAL))
			check_trigger (tmp, tmp->above);
}

/**
 * Handles player eating food that temporarily changes status (resistances, stats).
 * This used to call cast_change_attr(), but
 * that doesn't work with the new spell code.  Since we know what
 * the food changes, just grab a force and use that instead.
 */

void eat_special_food(object *who, object *food) {
    object *force;
    int i, did_one=0; 
    sint8 k;

    force = get_archetype(FORCE_NAME);

    for (i=0; i < NUM_STATS; i++) {
	k = get_attr_value(&food->stats, i);
	if (k) {
	    set_attr_value(&force->stats, i, k);
	    did_one = 1;
	}
    }

    /* check if we can protect the eater */
    for (i=0; i<NROFATTACKS; i++) {
	if (food->resist[i]>0) {
	    force->resist[i] = food->resist[i] / 2;
	    did_one = 1;
	}
    }
    if (did_one) {
	force->speed = 0.1;
	update_ob_speed(force);
	/* bigger morsel of food = longer effect time */
	force->stats.food = food->stats.food / 5;
	SET_FLAG(force, FLAG_IS_USED_UP);   
	SET_FLAG(force, FLAG_APPLIED);
	change_abil(who, force);
	insert_ob_in_ob(force, who);
    } else {
	free_object(force);
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
    fix_player(who);
}


/**
 * Designed primarily to light torches/lanterns/etc.
 * Also burns up burnable material too. First object in the inventory is
 * the selected object to "burn". -b.t.
 */

void apply_lighter(object *who, object *lighter) {
    object *item;
    int count,nrof, is_player_env=0;
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
	if (who == is_player_inv(item)) is_player_env=1;

	save_throw_object(item,AT_FIRE,who);
	/* Change to check count and not freed, since the object pointer
	 * may have gotten recycled
	 */
	if ((nrof != item->nrof ) || (count != item->count)) {
	    new_draw_info_format(NDI_UNIQUE, 0,who,
		 "You light the %s with the %s.",item_name,lighter->name);
	    /* Need to update the player so that the players glow radius
	     * gets changed.
	     */
	    if (is_player_env) fix_player(who);
	} else {
	    new_draw_info_format(NDI_UNIQUE, 0,who,
		 "You attempt to light the %s with the %s and fail.",item->name,lighter->name);
	}

   } else /* nothing to light */
	new_draw_info(NDI_UNIQUE, 0,who,"You need to mark a lightable object.");

}

/**
 * op made some mistake with a scroll, this takes care of punishment.
 * scroll_failure()- hacked directly from spell_failure
 */
void scroll_failure(object *op, int failure, int power)
{
    if(abs(failure/4)>power) power=abs(failure/4); /* set minimum effect */

    if(failure<= -1&&failure > -15) {/* wonder */
	object *tmp;

	new_draw_info(NDI_UNIQUE, 0,op,"Your spell warps!.");
	tmp=get_archetype(SPELL_WONDER);
	cast_wonder(op, op, 0, tmp);
	free_object(tmp);
    } else if (failure <= -15&&failure > -35) {/* drain mana */
	new_draw_info(NDI_UNIQUE, 0,op,"Your mana is drained!.");
	op->stats.sp -= random_roll(0, power-1, op, PREFER_LOW);
	if(op->stats.sp<0) op->stats.sp = 0;
    } else if (settings.spell_failure_effects == TRUE) {
	if (failure <= -35&&failure > -60) { /* confusion */
	    new_draw_info(NDI_UNIQUE, 0,op,"The magic recoils on you!");
	    confuse_player(op,op,power);
	} else if (failure <= -60&&failure> -70) {/* paralysis */
	    new_draw_info(NDI_UNIQUE, 0,op,"The magic recoils and paralyzes "
		"you!");
	    paralyze_player(op,op,power);
	} else if (failure <= -70&&failure> -80) {/* blind */
	    new_draw_info(NDI_UNIQUE, 0,op,"The magic recoils on you!");
	    blind_player(op,op,power);
	} else if (failure <= -80) {/* blast the immediate area */
	    object *tmp;
	    tmp=get_archetype(LOOSE_MANA);
	    cast_magic_storm(op,tmp, power);
	    new_draw_info(NDI_UNIQUE, 0,op,"You unlease uncontrolled mana!");
	    free_object(tmp);
	}
    }
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
		sint8 stat=get_attr_value(stats,i);
		int race_bonus = get_attr_value(&(pl->arch->clone.stats),i);
		stat += get_attr_value(ns,i);
		if(stat > 20 + race_bonus) {
		    excess_stat++;
		    stat = 20+race_bonus;
		}
		set_attr_value(stats,i,stat);
	    }

	    for(j=0;excess_stat >0 && j<100;j++)  {/* try 100 times to assign excess stats */
		int i = rndm(0, 6);
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

/**
 * This handles the positioning system.
 * Useful to know a player's position.
 * If item is also marked, then will reset the origin.
 * If gps's food is 0, not yet reset -> can't use.
 * Origin's x & y are stored in hp/sp.
 * Can only be used on 'world_xxx_xxx' maps.
 */
void apply_positioning_system( object* pl, object* gps )
    {
    int x, y;
    int reset;
    int map_x, map_y;

    if ( !pl->type == PLAYER )
        /* Non players have no need for that */
        return;

    reset = 0;
    if ( find_marked_object( pl ) == gps )
        reset = 1;

    if ( gps->stats.food == 0 && !reset )
        {
        new_draw_info( NDI_UNIQUE, 0, pl, "You must fix the origin of the positioning system first!" );
        return;
        }

    if ( !pl->map || !pl->map->name )
        {
        new_draw_info( NDI_UNIQUE, 0, pl, "This is a too weird place to try to position yourself." );
        return;
        }

    /* Check map is a world one */
    if ( sscanf( pl->map->name, "world_%d_%d", &map_x, &map_y ) != 2 )
        {
        new_draw_info( NDI_UNIQUE, 0, pl, "You must be in the world to position yourself." );
        return;
        }

    /* Now compute actual position */
    map_x -= 99;
    map_y -= 99;
    x = map_x * settings.worldmaptilesizex + pl->x;
    y = map_y * settings.worldmaptilesizey + pl->y;

    if ( reset )
        {
        gps->stats.hp = x;
        gps->stats.sp = y;
        gps->stats.food = 1;
        new_draw_info( NDI_UNIQUE, 0, pl, "You reset the origin of the system." );
        return;
        }

    x -= gps->stats.hp;
    y -= gps->stats.sp;

    /* Display location */
    new_draw_info_format( NDI_UNIQUE, 0, pl, "You are at %d:%d.", x, y );
    }

/**
 * This handles items of type 'transformer'.
 * Basically those items, used with a marked item, transform both items into something
 * else.
 * "Transformer" item is removed too, unless the level is zero.
 * Change information is contained in the 'slaying' field of the marked item.
 * The format is as follow: transformer:[number ]yield[;transformer:...].
 * This way an item can be transformed in many things, and/or many objects.
 * The 'slaying' field for transformer is used as verb for the action.
 * Created item is in the 'other_arch' field.
 */
void apply_item_transformer( object* pl, object* transformer )
    {
    object* marked;
    object* new_item;
    char* find;
    char* separator;
    int yield;
    char got[ MAX_BUF ];

    if ( !pl || !transformer )
        return;
    marked = find_marked_object( pl );
    if ( !marked )
        {
        new_draw_info_format( NDI_UNIQUE, 0, pl, "Use the %s with what item?", query_name( transformer ) );
        return;
        }
    if ( !marked->slaying )
        {
        new_draw_info_format( NDI_UNIQUE, 0, pl, "You can't use the %s with your %s!", query_name( transformer ), query_name( marked ) );
        return;
        }
    /* check whether they are compatible or not */
    find = strstr( marked->slaying, transformer->arch->name );
    if ( !find || ( *( find + strlen( transformer->arch->name ) ) != ':' ) )
        {
        new_draw_info_format( NDI_UNIQUE, 0, pl, "You can't use the %s with your %s!", query_name( transformer ), query_name( marked ) );
        return;
        }
    memset( got, 0, MAX_BUF );
    find += strlen( transformer->arch->name ) + 1;
    /* Item can be used, now find how many and what it yields */
    if ( isdigit( *( find ) ) )
        {
        yield = atoi( find );
        if ( yield < 1 )
            {
            LOG( llevDebug, "apply_item_transformer: item %s has slaying-yield %d.", query_base_name( marked, 0 ), yield );
            yield = 1;
            }
        }
    else
        yield = 1;

    while ( isdigit( *find ) )
        find++;
    while ( *find == ' ' )
        find++;
    if ( (separator = strchr( find, ';' ))!=NULL)
        {
        strncpy( got, find, MIN( separator - find, MAX_BUF ) );
        }
    else
        {
        strncpy( got, find, MAX_BUF );
        }

    /* Now create new item, remove used ones when required. */
    new_item = get_archetype( got );
    if ( !new_item )
        {
        new_draw_info_format( NDI_UNIQUE, 0, pl, "This %s is strange, better to not use it.", query_base_name( marked, 0 ) );
        return;
        }
    new_item->nrof = yield;
    new_draw_info_format( NDI_UNIQUE, 0, pl, "You %s the %s.", transformer->slaying, query_base_name( marked, 0 ) );
    insert_ob_in_ob( new_item, pl );
    esrv_send_inventory( pl, pl );
    /* Eat up one item */
    decrease_ob_nr( marked, 1 );
    /* Eat one transformer if needed */
    if ( transformer->level )
        decrease_ob_nr( transformer, 1 );
    }
