/*
 * static char *rcsid_shop_c =
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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
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
uint64 query_cost(object *tmp, object *who, int flag) {
    uint64 val;
    int number;	/* used to better calculate value */
    int no_bargain;
    float diff;
    float ratio;

    no_bargain = flag & F_NO_BARGAIN;
    flag &= ~F_NO_BARGAIN;

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
	    else {	/* Trying to sell something, or get true value */
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

    if (tmp->type==WAND) {
	/* Value of the wand is multiplied by the number of
	 * charges.  the treasure code already sets up the value
	 * 50 charges is used as the baseline.
	 */
	if (QUERY_FLAG(tmp, FLAG_IDENTIFIED) || !need_identify(tmp))
	    val=(val*tmp->stats.food) / 50;
	else /* if not identified, presume one charge */
	    val/=50;
    }

    /* Limit amount of money you can get for really great items. */
    if (flag==F_TRUE || flag==F_SELL) {
	if (val/number>10000) {
	    val=8000+isqrt(val/number)*20;
	    val *= number;
	}
    }

    /* This modification is for bargaining skill.
     * Now only players with max level in bargaining
     * AND Cha = 30 will get optimal price.
     * Thus charisma will never get useless.
     * -b.e. edler@heydernet.de
     */

    if (who!=NULL && who->type==PLAYER) {
	int lev_bargain = 0;

	/* ratio determines how much of the price modification
	 * will come from the basic stat charisma
	 * the rest will come from the level in bargaining skill
	 */
	ratio = 0.5;

	if (find_skill_by_number(who,SK_BARGAINING)) {
	    lev_bargain = find_skill_by_number(who,SK_BARGAINING)->level;
	}
	if ( !no_bargain && (lev_bargain>0) )
	    diff = (0.8 - 0.6*((lev_bargain+settings.max_level*0.05)
                         /(settings.max_level*1.05)));
	else
	    diff = 0.8;

	diff *= 1-ratio;

	/* Diff is now a float between 0.2 and 0.8 */
	diff+=(cha_bonus[who->stats.Cha]-1)/(1+cha_bonus[who->stats.Cha])*ratio;

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

    /* I don't think this should really happen - if it does, it indicates and
     * overflow of diff above.  That shoudl only happen if
     * we are selling objects - in that case, the person just
     * gets no money.
     */
    if(val<0)
	val=0;

    /* Unidentified stuff won't sell for more than 60gp */
    if(flag==F_SELL && !QUERY_FLAG(tmp, FLAG_IDENTIFIED) && need_identify(tmp)) {
	 val = (val > 600)? 600:val;
    }
    return val;
}

/* Find the coin type that is worth more the 'c'.  Starts at the
 * cointype placement.
 */

static archetype *find_next_coin(uint64 c, int *cointype) {
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

/* This returns a string of how much something is worth based on
 * an integer being passed.
 * cost is the cost we need to represent.
 * While cost is 64 bit, the number of any coin is still really
 * limited to 32 bit (size of nrof field).  If it turns out players
 * have so much money that they have more than 2 billion platinum
 * coins, there are certainly issues - the easiest fix at that
 * time is to add a higher denomination (mithril piece with
 * 10,000 silver or something)
 */
char *cost_string_from_value(uint64 cost)
{
    static char buf[MAX_BUF];
    archetype *coin, *next_coin;
    char *endbuf;
    int num, cointype = 0;

    coin = find_next_coin(cost, &cointype);
    if (coin == NULL)
	return "nothing";

    num = cost / coin->clone.value;
    /* so long as nrof is 32 bit, this is true.
     * If it takes more coins than a person can possibly carry, this
     * is basically true.
     */
    if ( (cost / coin->clone.value) > UINT32_MAX) {
	strcpy(buf,"an unimaginable sum of money.");
	return buf;
    }

    cost -= (uint64)num * (uint64)coin->clone.value;
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
	cost -= (uint64)num * (uint64)coin->clone.value;

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
 * including what is in containers.
 */
uint64 query_money(object *op) {
    object *tmp;
    uint64 total=0;

    if (op->type!=PLAYER && op->type!=CONTAINER) {
	LOG(llevError, "Query money called with non player/container");
	return 0;
    }
    for (tmp = op->inv; tmp; tmp= tmp->below) {
	if (tmp->type==MONEY) {
	    total += (uint64)tmp->nrof * (uint64)tmp->value;
	} else if (tmp->type==CONTAINER &&
		   QUERY_FLAG(tmp,FLAG_APPLIED) &&
		   (tmp->race==NULL || strstr(tmp->race,"gold"))) {
	    total += query_money(tmp);
	}
    }
    return total;
}
/* TCHIZE: This function takes the amount of money from the
 * the player inventory and from it's various pouches using the
 * pay_from_container function.
 * returns 0 if not possible. 1 if success
 */
int pay_for_amount(int to_pay,object *pl) {
    object *pouch;

    if (to_pay==0) return 1;
    if (to_pay > query_money(pl)) return 0;

    to_pay = pay_from_container(NULL, pl, to_pay);

    for (pouch=pl->inv; (pouch!=NULL) && (to_pay>0); pouch=pouch->below) {
	if (pouch->type == CONTAINER
	    && QUERY_FLAG(pouch, FLAG_APPLIED)
	    && (pouch->race == NULL || strstr(pouch->race, "gold"))) {
	    to_pay = pay_from_container(NULL, pouch, to_pay);
	}
    }
    fix_player(pl);
    return 1;
}

/* DAMN: This is now a wrapper for pay_from_container, which is
 * called for the player, then for each active container that can hold
 * money until op is paid for.  Change will be left wherever the last
 * of the price was paid from.
 */
int pay_for_item(object *op,object *pl) {
    uint64 to_pay = query_cost(op,pl,F_BUY);
    object *pouch;
    uint64 saved_money;

    if (to_pay==0) return 1;
    if (to_pay>query_money(pl)) return 0;

    /* We compare the paid price with the one for a player
     * without bargaining skill.
     * This determins the amount of exp (if any) gained for bargaining.
     */
    saved_money = query_cost(op,pl,F_BUY | F_NO_BARGAIN) - to_pay;

    if (saved_money > 0)
      change_exp(pl,saved_money,"bargaining",SK_EXP_NONE);

    to_pay = pay_from_container(op, pl, to_pay);

    for (pouch=pl->inv; (pouch!=NULL) && (to_pay>0); pouch=pouch->below) {
	if (pouch->type == CONTAINER
	    && QUERY_FLAG(pouch, FLAG_APPLIED)
	    && (pouch->race == NULL || strstr(pouch->race, "gold"))) {
	    to_pay = pay_from_container(op, pouch, to_pay);
	}
    }
    if (settings.real_wiz == FALSE && QUERY_FLAG(pl, FLAG_WAS_WIZ))
	SET_FLAG(op, FLAG_WAS_WIZ);
    fix_player(pl);
    return 1;
}

/* This pays for the item, and takes the proper amount of money off
 * the player.
 * CF 0.91.4 - this function is mostly redone in order to fix a bug
 * with weight not be subtracted properly.  We now remove and
 * insert the coin objects -  this should update the weight
 * appropriately
 *
 * DAMN: This function is used for the player, then for any active
 * containers that can hold money, until the op is paid for.
 */
uint64 pay_from_container(object *op, object *pouch, int to_pay) {
    int count, i;
    uint64 remain;
    object *tmp, *coin_objs[NUM_COINS], *next;
    archetype *at;
    object *who;

    if (pouch->type != PLAYER && pouch->type != CONTAINER) return to_pay;

    remain = to_pay;
    for (i=0; i<NUM_COINS; i++) coin_objs[i] = NULL;

    /* This hunk should remove all the money objects from the player/container */
    for (tmp=pouch->inv; tmp; tmp=next) {
	next = tmp->below;

	if (tmp->type == MONEY) {
	    for (i=0; i<NUM_COINS; i++) {
		if (!strcmp(coins[NUM_COINS-1-i], tmp->arch->name) &&
		    (tmp->value == tmp->arch->clone.value) ) {

		    /* This should not happen, but if it does, just		*
		     * merge the two.						*/
		    if (coin_objs[i]!=NULL) {
			LOG(llevError,"%s has two money entries of (%s)\n",
			    pouch->name, coins[NUM_COINS-1-i]);
			remove_ob(tmp);
			coin_objs[i]->nrof += tmp->nrof;
			esrv_del_item(pouch->contr, tmp->count);
			free_object(tmp);
		    }
		    else {
			remove_ob(tmp);
			if(pouch->type==PLAYER) esrv_del_item(pouch->contr, tmp->count);
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
	    coin_objs[i] = get_object();
	    copy_object(&at->clone, coin_objs[i]);
	    coin_objs[i]->nrof = 0;
	}

    for (i=0; i<NUM_COINS; i++) {
	int num_coins;

	if (coin_objs[i]->nrof*coin_objs[i]->value> remain) {
	    num_coins = remain / coin_objs[i]->value;
	    if ((uint64)num_coins*(uint64)coin_objs[i]->value < remain) num_coins++;
	} else {
	    num_coins = coin_objs[i]->nrof;
	}

	remain -= (uint64)num_coins * (uint64)coin_objs[i]->value;
	coin_objs[i]->nrof -= num_coins;
	/* Now start making change.  Start at the coin value
	 * below the one we just did, and work down to
	 * the lowest value.
	 */
	count=i-1;
	while (remain<0 && count>=0) {
		num_coins = -remain/ coin_objs[count]->value;
		coin_objs[count]->nrof += num_coins;
		remain += num_coins * coin_objs[count]->value;
		count--;
	}
    }
    for (i=0; i<NUM_COINS; i++) {
	if (coin_objs[i]->nrof) {
	    object *tmp = insert_ob_in_ob(coin_objs[i], pouch);
	    for (who = pouch; who && who->type!=PLAYER && who->env!=NULL; who=who->env) ;
	    esrv_send_item(who, tmp);
	    esrv_send_item (who, pouch);
	    esrv_update_item (UPD_WEIGHT, who, pouch);
	    if (pouch->type != PLAYER) {
		esrv_send_item (who, who);
		esrv_update_item (UPD_WEIGHT, who, who);
	    }
	} else {
	    free_object(coin_objs[i]);
	}
    }
    return(remain);
}

/* Better get_payment, descends containers looking for
 * unpaid items, and pays for them.
 * returns 0 if the player still has unpaid items.
 * returns 1 if the player has paid for everything.
 * pl is the player buying the stuff.
 * op is the object we are examining.  If op has
 * and inventory, we examine that.  IF there are objects
 * below op, we descend down.
 */
int get_payment(object *pl, object *op) {
    char buf[MAX_BUF];
    int ret=1;

    if (op!=NULL&&op->inv)
        ret = get_payment(pl, op->inv);

    if (!ret)
        return 0;

    if (op!=NULL&&op->below)
        ret = get_payment (pl, op->below);

    if (!ret) 
        return 0;
   
    if(op!=NULL&&QUERY_FLAG(op,FLAG_UNPAID)) {
        strncpy(buf,query_cost_string(op,pl,F_BUY),MAX_BUF);
        if(!pay_for_item(op,pl)) {
            uint64 i=query_cost(op,pl,F_BUY) - query_money(pl);
	    CLEAR_FLAG(op, FLAG_UNPAID);
	    new_draw_info_format(NDI_UNIQUE, 0, pl,
		"You lack %s to buy %s.", cost_string_from_value(i),
		query_name(op));
	    SET_FLAG(op, FLAG_UNPAID);
            return 0;
        } else {
	    object *tmp;
	    tag_t c = op->count;

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


/* Modified function to give out platinum coins.  This function uses
 * the coins[] array to know what coins are available, just like
 * buy item.
 *
 * Modified to fill available race: gold containers before dumping
 * remaining coins in character's inventory.
 */
void sell_item(object *op, object *pl) {
    uint64 i=query_cost(op,pl,F_SELL), extra_gain;
    int count;
    object *tmp, *pouch;
    archetype *at;

    if(pl==NULL||pl->type!=PLAYER) {
	LOG(llevDebug,"Object other than player tried to sell something.\n");
	return;
    }

    if(op->custom_name) FREE_AND_CLEAR_STR(op->custom_name);

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

    /* We compare the price with the one for a player
     * without bargaining skill.
     * This determins the amount of exp (if any) gained for bargaining.
     * exp/10 -> 1 for each gold coin
     */
    extra_gain = i - query_cost(op,pl,F_SELL | F_NO_BARGAIN);

    if (extra_gain > 0)
	change_exp(pl,extra_gain/10,"bargaining",SK_EXP_NONE);
  
    for (count=0; coins[count]!=NULL; count++) {
	at = find_archetype(coins[count]);
	if (at==NULL) LOG(llevError, "Could not find %s archetype", coins[count]);
	else if ((i/at->clone.value) > 0) {
	    for ( pouch=pl->inv ; pouch ; pouch=pouch->below ) {
		if ( pouch->type==CONTAINER && QUERY_FLAG(pouch, FLAG_APPLIED) && pouch->race && strstr(pouch->race, "gold") ) {
		    int w = at->clone.weight * (100-pouch->stats.Str)/100;
		    int n = i/at->clone.value;

		    if (w==0) w=1;    /* Prevent divide by zero */
		    if ( n>0 && (!pouch->weight_limit || pouch->carrying+w<=pouch->weight_limit)) {
			if (pouch->weight_limit && (pouch->weight_limit-pouch->carrying)/w<n)
			    n = (pouch->weight_limit-pouch->carrying)/w;

			tmp = get_object();
			copy_object(&at->clone, tmp);
			tmp->nrof = n;
			i -= (uint64)tmp->nrof * (uint64)tmp->value;
			tmp = insert_ob_in_ob(tmp, pouch);
			esrv_send_item (pl, tmp);
			esrv_send_item (pl, pouch);
			esrv_update_item (UPD_WEIGHT, pl, pouch);
			esrv_send_item (pl, pl);
			esrv_update_item (UPD_WEIGHT, pl, pl);
		    }
		}
	    }
	    if (i/at->clone.value > 0) {
		tmp = get_object();
		copy_object(&at->clone, tmp);
		tmp->nrof = i/tmp->value;
		i -= (uint64)tmp->nrof * (uint64)tmp->value;
		tmp = insert_ob_in_ob(tmp, pl);
		esrv_send_item (pl, tmp);
		esrv_send_item (pl, pl);
		esrv_update_item (UPD_WEIGHT, pl, pl);
	    }
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

static void add_shop_item(object *tmp, shopinv *items, int *numitems, int *numallocated)
{
#if 0
    char buf[MAX_BUF];
#endif
    /* clear unpaid flag so that doesn't come up in query
     * string.  We clear nrof so that we can better sort
     * the object names.
     */

    CLEAR_FLAG(tmp, FLAG_UNPAID);
    items[*numitems].nrof=tmp->nrof;
    /* Non mergable items have nrof of 0, but count them as one
     * so the display is properly.
     */
    if (tmp->nrof == 0) items[*numitems].nrof++;
    items[*numitems].type=tmp->type;

    switch (tmp->type) {
#if 0
	case BOOTS:
	case GLOVES:
	case RING:
	case AMULET:
	case BRACERS:
	case GIRDLE:
	    sprintf(buf,"%s %s",query_base_name(tmp,0),describe_item(tmp, NULL));
	    items[*numitems].item_sort = strdup_local(buf);
	    sprintf(buf,"%s %s",query_name(tmp),describe_item(tmp, NULL));
	    items[*numitems].item_real = strdup_local(buf);
	    (*numitems)++;
	    break;
#endif

	default:
	    items[*numitems].item_sort = strdup_local(query_base_name(tmp, 0));
	    items[*numitems].item_real = strdup_local(query_base_name(tmp, 1));
	    (*numitems)++;
	    break;
    }
    SET_FLAG(tmp, FLAG_UNPAID);
}

void shop_listing(object *op)
{
    int i,j,numitems=0,numallocated=0, nx, ny;
    char *map_mark = (char *) calloc(MAGIC_MAP_SIZE * MAGIC_MAP_SIZE,1);
    object	*stack;
    shopinv	*items;

    /* Should never happen, but just in case a monster does apply a sign */
    if (op->type!=PLAYER) return;

    new_draw_info(NDI_UNIQUE, 0, op, "\nThe shop contains:");

    magic_mapping_mark(op, map_mark, 3);
    items=malloc(40*sizeof(shopinv));
    numallocated=40;

    /* Find all the appropriate items */
    for (i=0; i<MAP_WIDTH(op->map); i++) {
	for (j=0; j<MAP_HEIGHT(op->map); j++) {
	    /* magic map code now centers the map on the object at MAGIC_MAP_HALF.
	     *
	     */
	    nx = i - op->x + MAGIC_MAP_HALF;
	    ny = j - op->y + MAGIC_MAP_HALF;
	    /* unlikely, but really big shops could run into this issue */
	    if (nx < 0 || ny < 0 || nx > MAGIC_MAP_SIZE || ny > MAGIC_MAP_SIZE) continue;

	    if (map_mark[nx + MAGIC_MAP_SIZE * ny] & FACE_FLOOR) {
		stack  =get_map_ob(op->map,i,j);

		while (stack) {
		    if (QUERY_FLAG(stack, FLAG_UNPAID)) {
			if (numitems==numallocated) {
			    items=realloc(items, sizeof(shopinv)*(numallocated+10));
			    numallocated+=10;
			}
			add_shop_item(stack, items, &numitems, &numallocated);
		    }
		    stack = stack->above;
		}
	    }
	}
    }
    free(map_mark);
    if (numitems == 0) {
	new_draw_info(NDI_UNIQUE, 0, op, "The shop is currently empty.\n");
	free(items);
	return;
    }
    qsort(items, numitems, sizeof(shopinv), (int (*)())shop_sort);

    for (i=0; i<numitems; i++) {
	/* Collapse items of the same name together */
	if ((i+1)<numitems && !strcmp(items[i].item_real, items[i+1].item_real)) {
	    items[i+1].nrof += items[i].nrof;
	    free(items[i].item_sort);
	    free(items[i].item_real);
	} else {
	    new_draw_info_format(NDI_UNIQUE, 0, op, "%d %s", 
				 items[i].nrof? items[i].nrof:1,
			 items[i].nrof==1?items[i].item_sort: items[i].item_real);
	    free(items[i].item_sort);
	    free(items[i].item_real);
	}
    }
    free(items);
}
