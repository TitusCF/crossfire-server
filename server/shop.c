/*
 * static char *rcsid_shop_c =
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

    The author can be reached via e-mail to master@rahul.net

*/

#include <global.h>
#include <spells.h>
#include <skills.h>
#include <living.h>
#include <newclient.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif


#define NUM_COINS 3	/* number of coin types */
static char *coins[] = {"platinacoin", "goldcoin", "silvercoin", NULL};


/* Added F_TRUE flag to define.h to mean that the price should not
 * be adjusted by players charisma. With F_TRUE, it returns the amount
 * that the item is worth, if it was sold, but unadjusted by charisma.
 * This is needed for alchemy, to to determine what value of gold nuggets
 * should be given (the gold nuggets, when sold, will have the adjustment
 * by charisma done at that time).  NULL could have been passed as the
 * who parameter, but then the adjustment for expensive items (>10000)
 * would not be done.
 * 
 * CF 0.91.4 - This function got changed around a bit.  Now the
 * number of object is multiplied by the value early on.  This fixes problems
 * with items worth very little.  What happened before is that various
 * divisions took place, the value got rounded to 0 (Being an int), and
 * thus remained 0.
 *
 * Mark Wedel (mwedel@pyramid.com)
 */
int query_cost(object *tmp, object *who, int flag) {
  int val;
  int number;	/* used to better calculate value */
  int charisma;

  if (tmp->type==MONEY) return (tmp->nrof * tmp->value);
  if (tmp->type==GEM) {
	if (flag==F_TRUE) return (tmp->nrof * tmp->value);
	if (flag==F_BUY) return (1.03 * tmp->nrof * tmp->value);
	if (flag==F_SELL) return (0.97 * tmp->nrof * tmp->value);
	LOG(llevError,"Query_cost: Gem type with unknown flag : %d\n", flag);
	return 0;
  }
  number = tmp->nrof;
  if (number==0) number=1;
  if (QUERY_FLAG(tmp, FLAG_IDENTIFIED) || !need_identify(tmp)) {
    if (QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED))
      return 0;
    else
      val=tmp->value * number;
  }
  /* This area deals with objects that are not identified, but can be */
  else {
    if (tmp->arch != NULL) {
      if (flag == F_BUY) {
        LOG(llevError, "Asking for buy-value of unidentified object.");
        val = tmp->arch->clone.value * 50 * number;
      }
      else	/* Trying to sell something, or get true value */
        if (tmp->type == POTION)
          val = number * 1000; /* Don't want to give anything away */
        else {
	  /* Get 2/3'rd value for applied objects, 1/3'rd for totally 
	   * unknown objects
	   */
	  if (QUERY_FLAG(tmp, FLAG_BEEN_APPLIED))
	    val = number * tmp->arch->clone.value *2 / 3;
	  else
	    val = number * tmp->arch->clone.value / 3;
	}
    } else { /* No archetype with this object */
      LOG(llevDebug,"In sell item: Have object with no archetype: %s\n", tmp->name);
      if (flag == F_BUY) {
        LOG(llevError, "Asking for buy-value of unidentified object without arch.");
        val = number * tmp->value * 10;
      }
      else
        val = number * tmp->value / 5;
    }
  }

  /* If the item has been applied or identifed or does not need to be
   * identified, AND the object is magical and the archetype is non
   * magical, then change values accordingly.  The tmp->arch==NULL is
   * really just a check to prevent core dumps for when it checks
   * tmp->arch->clone.magic for any magic.  The check for archetype
   * magic is to not give extra money for archetypes that are by
   * default magical.  This is because the archetype value should have
   * already figured in that value.
   */
  if((QUERY_FLAG(tmp, FLAG_IDENTIFIED)||!need_identify(tmp)||
     QUERY_FLAG(tmp, FLAG_BEEN_APPLIED)) &&
     tmp->magic&&(tmp->arch==NULL||!tmp->arch->clone.magic)) {
	if(tmp->magic>0)
      	    val*=(3*tmp->magic*tmp->magic*tmp->magic);
    	else
	  /* Note that tmp->magic is negative, so that this
	   * will actually be something like val /=2, /=3, etc.
	   */
          val/=(1-tmp->magic);
     }
#if 0 /* cha_bonus now takes this into account */
  if (flag==F_BUY)
    val*=6; /* For charisma 25, the price is now x1.5 */
#endif
  if (tmp->type==WAND) {
    if (QUERY_FLAG(tmp, FLAG_IDENTIFIED) || !need_identify(tmp))
      /* Give money based on how many charges the item could have.  Not
       * really a good method - some spells are certainly better than others.
       */
      val=(val*tmp->stats.food)/spells[tmp->stats.sp].charges;
    else
      val/=3;
  }

  /* Limit amount of money you can get for really great items. */
  if (flag==F_TRUE || flag==F_SELL) {
    if (val/number>25000) {
	val=8000+isqrt((int)val/number)*20;
	val *= number;
    }
  }

/* this  modification is for merchant skill 
 * now players with readied merchant skill get 
 * more Cha up to the limit of modified Cha = 30.
 * -b.t. thomas@nomad.astro.psu.edu 
 */

  if (who!=NULL && who->type==PLAYER) { 
      float diff;

      charisma = who->stats.Cha;  /* used for SK_BARGAINING modification */ 

      if (find_skill(who,SK_BARGAINING)) {
	charisma = who->stats.Cha + (who->level+2)/3;
	if(charisma>30) charisma = 30;
      }
      if (cha_bonus[charisma]<=1.0) {
	LOG(llevError,"Illegal charisma bonus, %d <=1.0", cha_bonus[charisma]);
	diff=0.9;	/*pretty bad case */
      }
      else 
	/* Diff is now a float between 0 and 1 */
	diff=(cha_bonus[charisma]-1)/(1+cha_bonus[charisma]);

      /* we need to multiply these by 4.0 to keep buy costs roughly the same
       * (otherwise, you could buy a potion of charisma for around 400 pp.
       * Arguable, the costs in the archetypes should be updated to better
       * reflect values (potion charisma list for 1250 gold)
       */
      if(flag==F_BUY)
          val=(4.0*(float)val*(1.0+diff));
      else if (flag==F_SELL)
          val=(4.0*(float)val*(1.0-diff));
      else val *=4;
  }

  /* I don't understand this code...., I'm changing it to 
	  use 0 instead of 1000000 if we're selling*/
  if(val<0) {
	 if(flag==F_SELL) 
		val=0;
	 else
		val=1000000;
  }

  /* Unidentified stuff won't sell for more than 60gp */
  if(flag==F_SELL && !QUERY_FLAG(tmp, FLAG_IDENTIFIED) && need_identify(tmp)) {
	 val = (val > 600)? 600:val;
  }
  return (int)val;
}

/* Find the coin type that is worth more the 'c'.  Starts at the
 * cointype placement.
 */

static archetype *find_next_coin(int c, int *cointype) {
  archetype *coin;

  do {
    if (coins[*cointype]==NULL) return NULL;
    coin = find_archetype(coins[*cointype]);
    if (coin == NULL)
      return NULL;
    *cointype += 1;
  } while (coin->clone.value > c);

  return coin;
}

/* This returns a string of how much somethign is worth based on
 * an integer being passed.
 */
char *cost_string_from_value(int cost)
{
  static char buf[MAX_BUF];
  archetype *coin, *next_coin;
  char *endbuf;
  int num, cointype = 0;

  coin = find_next_coin(cost, &cointype);
  if (coin == NULL)
    return "nothing";

  num = cost / coin->clone.value;
  cost -= num * coin->clone.value;
  if (num == 1)
    sprintf(buf, "1 %s", coin->clone.name);
  else
    sprintf(buf, "%d %ss", num, coin->clone.name);

  next_coin = find_next_coin(cost, &cointype);
  if (next_coin == NULL)
    return buf;

  do {
    endbuf = buf + strlen(buf);

    coin = next_coin;
    num = cost / coin->clone.value;
    cost -= num * coin->clone.value;

    if (cost == 0)
      next_coin = NULL;
    else
      next_coin = find_next_coin(cost, &cointype);

    if (next_coin) {
      /* There will be at least one more string to add to the list,
       * use a comma.
       */
      strcat(endbuf, ", "); endbuf += 2;
    } else {
      strcat(endbuf, " and "); endbuf += 5;
    }
    if (num == 1)
      sprintf(endbuf, "1 %s", coin->clone.name);
    else
      sprintf(endbuf, "%d %ss", num, coin->clone.name);
  } while (next_coin);

  return buf;
}

char *query_cost_string(object *tmp,object *who,int flag) {
  return cost_string_from_value(query_cost(tmp,who,flag));
}

/* This function finds out how much money the player is carrying,
 * and returns that value
 */

int query_money(object *op) {
    object *tmp;
    int	total=0;

    if (op->type!=PLAYER) {
	LOG(llevError, "Query money called with non player");
	return 0;
    }
    for (tmp = op->inv; tmp; tmp= tmp->below) {
	if (tmp->type==MONEY)
		total += tmp->nrof * tmp->value;
    }
    return total;
}

/* This pays for the item, and takes the proper amount of money off
 * the player. 
 * CF 0.91.4 - this function is mostly redone in order to fix a bug
 * with weight not be subtracted properly.  We now remove and
 * insert the coin objects -  this should update the weight
 * appropriately
 */

int pay_for_item(object *op,object *pl) {
    int item_cost=query_cost(op,pl,F_BUY),i, count;
    int to_pay;
    object *tmp, *coin_objs[NUM_COINS], *next;
    archetype *at;

    if (item_cost==0) return 1;
    if(item_cost>query_money(pl))
        return 0;


    for (i=0; i<NUM_COINS; i++) {
	coin_objs[i] = NULL;
    }

    /* This hunk should remove all the money objects from the player */
    for (tmp=pl->inv; tmp; tmp=next) {
	next = tmp->below;

	if (tmp->type == MONEY) {
	    for (i=0; i<NUM_COINS; i++) {
		if (!strcmp(coins[NUM_COINS-1-i], tmp->arch->name) &&
		    (tmp->value == tmp->arch->clone.value) ) {

		    /* This should not happen, but if it does, just
		     * merge the two.
		     */
		    if (coin_objs[i]!=NULL) {
			LOG(llevError,"%s has two money entries of (%s)\n",
			    pl->name, coins[NUM_COINS-1-i]);
			remove_ob(tmp);
			coin_objs[i]->nrof += tmp->nrof;
			esrv_del_item(pl->contr, tmp->count);
			free_object(tmp);
		    }
		    else {
			remove_ob(tmp);
			esrv_del_item(pl->contr, tmp->count);
			coin_objs[i] = tmp;
		    }
		    break;
		}
	    }
	    if (i==NUM_COINS)
		LOG(llevError,"in pay_for_item: Did not find string match for %s\n", tmp->arch->name);
	}
    }

    /* Fill in any gaps in the coin_objs array - needed to make change. */
    /* Note that the coin_objs array goes from least value to greatest value */
    for (i=0; i<NUM_COINS; i++)
	if (coin_objs[i]==NULL) {
	    at = find_archetype(coins[NUM_COINS-1-i]);
	    if (at==NULL) LOG(llevError, "Could not find %s archetype", coins[NUM_COINS-1-i]);
	    coin_objs[i] =get_object();
	    copy_object(&at->clone, coin_objs[i]);
	    coin_objs[i]->nrof = 0;
	}

    to_pay = item_cost;
    for (i=0; i<NUM_COINS; i++) {
	int num_coins;

	if (coin_objs[i]->nrof*coin_objs[i]->value> to_pay) {
		num_coins = to_pay / coin_objs[i]->value;
		if (num_coins*coin_objs[i]->value< to_pay) num_coins++;
	}
	else 
		num_coins = coin_objs[i]->nrof;

	to_pay -= num_coins * coin_objs[i]->value;
	coin_objs[i]->nrof -= num_coins;
	/* Now start making change.  Start at the coin value
	 * below the one we just did, and work down to
	 * the lowest value.
	 */
	count=i-1;
	while (to_pay<0 && count>=0) {
		num_coins = -to_pay/ coin_objs[count]->value;
		coin_objs[count]->nrof += num_coins;
		to_pay += num_coins * coin_objs[count]->value;
		count--;
	}
    }
    for (i=0; i<NUM_COINS; i++) {
	if (coin_objs[i]->nrof) {
	    object *tmp = insert_ob_in_ob(coin_objs[i], pl);
	    esrv_send_item(pl, tmp);
	} else
	    free_object(coin_objs[i]);
    }
#ifndef REAL_WIZ
    if(QUERY_FLAG(pl,FLAG_WAS_WIZ))
      SET_FLAG(op, FLAG_WAS_WIZ);
#endif
    fix_player(pl);
    return 1;
}

/* Eneq(@csd.uu.se): Better get_payment, descends containers looking for
   unpaid items. get_payment is now used as a link. To make it simple
   we need the player-object here. */

int get_payment2 (object *pl, object *op) {
    char buf[MAX_BUF];
    int ret=1;

    if (op!=NULL&&op->inv)
        ret = get_payment2(pl, op->inv);

    if (!ret) 
        return 0;

    if (op!=NULL&&op->below) 
        ret = get_payment2 (pl, op->below);

    if (!ret) 
        return 0;
   
    if(op!=NULL&&QUERY_FLAG(op,FLAG_UNPAID)) {
        strncpy(buf,query_cost_string(op,pl,F_BUY),MAX_BUF);
        if(!pay_for_item(op,pl)) {
            int i=query_cost(op,pl,F_BUY) - query_money(pl);
	    CLEAR_FLAG(op, FLAG_UNPAID);
	    new_draw_info_format(NDI_UNIQUE, 0, pl,
		"You lack %s to buy %s.", cost_string_from_value(i),
		query_name(op));
	    SET_FLAG(op, FLAG_UNPAID);
            return 0;
        } else {
	    object *tmp;
	    long c = op->count;

	    CLEAR_FLAG(op, FLAG_UNPAID);
	    new_draw_info_format(NDI_UNIQUE, 0, op,
		"You paid %s for %s.",buf,query_name(op));
	    tmp=merge_ob(op,NULL);
	    if (pl->type == PLAYER) {
		if (tmp) {      /* it was merged */
		    esrv_del_item (pl->contr, c);
		    op = tmp;
		}
	        esrv_send_item(pl, op);
	    }
	}
    }
    return 1;
}

int get_payment(object *pl) {
  int ret;

  ret = get_payment2 (pl, pl->inv);

  return ret;
}

/* Modified function to give out platinum coins.  This function is
 * not as general as pay_for_item in finding money types - each
 * new money type needs to be explicity code in here.
 */

void sell_item(object *op, object *pl) {
  int i=query_cost(op,pl,F_SELL), count;
  object *tmp;
  archetype *at;

  if(pl==NULL||pl->type!=PLAYER) {
    LOG(llevDebug,"Object other than player tried to sell something.\n");
    return;
  }
  if(!i) {
    new_draw_info_format(NDI_UNIQUE, 0, pl,
	"We're not interested in %s.",query_name(op));

    /* Even if the character doesn't get anything for it, it may still be
     * worth something.  If so, make it unpaid
     */
    if (op->value)
      SET_FLAG(op, FLAG_UNPAID);
    identify(op);
    return;
  }
  for (count=0; coins[count]!=NULL; count++) {
    at = find_archetype(coins[count]);
    if (at==NULL) LOG(llevError, "Could not find %s archetype", coins[count]);
    else if ((i/at->clone.value)) {
	tmp = get_object();
	copy_object(&at->clone, tmp);
	tmp->nrof = i/tmp->value;
	i -= tmp->nrof * tmp->value;
	tmp = insert_ob_in_ob(tmp, pl);
	esrv_send_item (pl, tmp);
    }
  }

  if (i!=0) 
	LOG(llevError,"Warning - payment not zero: %d\n", i);

  new_draw_info_format(NDI_UNIQUE, 0, pl,
	"You receive %s for %s.",query_cost_string(op,pl,1),
          query_name(op));
  SET_FLAG(op, FLAG_UNPAID);
  identify(op);
}


typedef struct shopinv {
    char	*item_sort;
    char	*item_real;
    uint16	type;
    uint32	nrof;
} shopinv;

/* There are a lot fo extra casts in here just to suppress warnings - it
 * makes it look uglier than it really it.
 * The format of the strings we get is type:name.  So we first want to
 * sort by type (numerical) - if the same type, then sort by name.
 */
static int shop_sort(const void *a1, const void *a2)
{
    shopinv *s1 = (shopinv*)a1, *s2= (shopinv*)a2;

    if (s1->type<s2->type) return -1;
    if (s1->type>s2->type) return 1;
    /* the type is the same (what atoi gets), so do a strcasecmp to sort
     * via alphabetical order
     */
    return strcasecmp(s1->item_sort, s2->item_sort);
}

void shop_listing(object *op)
{
    int i,j,numitems=0,numallocated=0;
    char *map_mark = (char *) malloc(op->map->mapx * op->map->mapy),
	buf[MAX_BUF];
    object	*stack,*tmp,*menu;
    shopinv	*items;

    /* Should never happen, but just in case a monster does apply a sign */
    if (op->type!=PLAYER) return;

    new_draw_info(NDI_UNIQUE, 0, op, "\nThe shop contains:");

    memset(map_mark, 0, op->map->mapx * op->map->mapy);
    magic_mapping_mark(op, map_mark, 3);
    items=malloc(40*sizeof(shopinv));
    numallocated=40;
    menu=get_object();

    /* Find all the appropriate items */
    for (i=0; i<op->map->mapx; i++) {
	for (j=0; j<op->map->mapy; j++) {
	    if (map_mark[i + op->map->mapx * j]) {
		stack  =get_map_ob(op->map,i,j);
		while (stack) {
		    if (QUERY_FLAG(stack, FLAG_UNPAID)) {
			tmp=get_object();
			copy_object(stack, tmp);
			insert_ob_in_ob(tmp, menu);
		    }
		    stack = stack->above;
		}
	    }
	}
    }
    if (menu->inv == NULL) {
	new_draw_info(NDI_UNIQUE, 0, op, "The shop is currently empty.\n");
	free_object(menu);
	free(map_mark);
	free(items);
	return;
    }
    for (tmp=menu->inv; tmp!=NULL; tmp=tmp->below) {
	if (numitems==numallocated) {
	    items=realloc(items, sizeof(shopinv)*(numallocated+10));
	    numallocated+=10;
	}
	/* clear unpaid flag so that doesn't come up in query
	 * string.  We clear nrof so that we can better sort
	 * the object names.
         */
	CLEAR_FLAG(tmp, FLAG_UNPAID);
	items[numitems].nrof=tmp->nrof;
	items[numitems].type=tmp->type;
	switch (tmp->type) {
	    case RING:
	    case AMULET:
	    case BRACERS:
	    case BOOTS:
	    case GLOVES:
	    case GIRDLE:
		sprintf(buf,"%s %s",query_base_name(tmp),describe_item(tmp));
	        items[numitems].item_sort = strdup_local(buf);
		sprintf(buf,"%s %s",query_name(tmp),describe_item(tmp));
	        items[numitems++].item_real = strdup_local(buf);
		break;

	    default:
	        items[numitems].item_sort = strdup_local(query_base_name(tmp));
	        items[numitems++].item_real = strdup_local(query_name(tmp));
		break;
	}
    }
    /* Free_object should also free all the objects in the inventory */
    free_object(menu);
    qsort(items, numitems, sizeof(shopinv), (int (*)())shop_sort);

    for (i=0; i<numitems; i++) {
	new_draw_info(NDI_UNIQUE, 0, op, items[i].item_real);
	free(items[i].item_sort);
    }
    free(map_mark);
    free(items);
}

