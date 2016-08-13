/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * Those functions deal with shop handling, bargaining, things like that.
 * @todo
 * isn't there redundance with pay_for_item(), get_payment(), pay_for_amount()?
 */

#include "global.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "shop.h"
#include "sproto.h"

/**
 * This is a measure of how effective store specialisation is. A general store
 * will offer this proportion of the 'maximum' price, a specialised store will
 * offer a range of prices around it such that the maximum price is always one
 * therefore making this number higher, makes specialisation less effective.
 * setting this value above 1 or to a negative value would have interesting,
 * (though not useful) effects.
 */
#define SPECIALISATION_EFFECT 0.5

/** Price a shopkeeper will give to someone they disapprove of.*/
#define DISAPPROVAL_RATIO 0.2

/** Price a shopkeeper will give someone they neither like nor dislike */
#define NEUTRAL_RATIO 0.8

static uint64_t pay_from_container(object *pl, object *pouch, uint64_t to_pay);
static uint64_t value_limit(uint64_t val, int quantity, const object *who, int isshop);
static double shop_specialisation_ratio(const object *item, const mapstruct *map);
static double shop_greed(const mapstruct *map);

#define NUM_COINS 5     /**< Number of coin types */

#define LARGEST_COIN_GIVEN 2 /**< Never give amber or jade, but accept them */

/** Coins to use for shopping. */
static const char *const coins[] = {
    "ambercoin",
    "jadecoin",
    "platinacoin",
    "goldcoin",
    "silvercoin",
    NULL
};

uint64_t price_base(const object *tmp) {
    // When there are zero objects, there is really one.
    int number = (tmp->nrof == 0) ? 1 : tmp->nrof;
    uint64_t val = (uint64_t)tmp->value * number;

    // Objects with price adjustments skip the rest of the calculations.
    const char *key = object_get_value(tmp, "price_adjustment");
    if (key != NULL) {
        float ratio = atof(key);
        return val * ratio;
    }

    if (tmp->type == MONEY || tmp->type == GEM) {
        return val;
    }

    if (QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) {
        val *= 0.8;
    }

    // If an item has an archetype, compare its base enchantment.
    if (tmp->arch != NULL) {
        int diff = tmp->magic - tmp->arch->clone.magic;
        val *= pow(1.15, diff);
    }

    // FIXME: Is the 'baseline' 50 charges per wand?
    if (tmp->type == WAND) {
        val *= tmp->stats.food / 50.0;
    }

    /* we need to multiply these by 4.0 to keep buy costs roughly the same
     * (otherwise, you could buy a potion of charisma for around 400 pp.
     * Arguable, the costs in the archetypes should be updated to better
     * reflect values (potion charisma list for 1250 gold)
     */
    val *= 4;

    return val;
}

uint64_t price_approx(const object *tmp, object *who) {
    uint64_t val = price_base(tmp);

    /* If we are approximating, then the value returned should
     * be allowed to be wrong however merely using a random number
     * each time will not be sufficient, as then multiple examinations
     * would give different answers, so we'll use the count instead.
     * By taking the sine of the count, a value between -1 and 1 is
     * generated, we then divide by the square root of the bargaining
     * skill and the appropriate identification skills, so that higher
     * level players get better estimates. (We need a +1 there to
     * prevent dividing by zero.)
     */
    const typedata *tmptype = get_typedata(tmp->type);
    int lev_identify = 0;

    if (tmptype) {
        int idskill1 = tmptype->identifyskill;
        if (idskill1) {
            int idskill2 = tmptype->identifyskill2;
            if (find_skill_by_number(who, idskill1)) {
                lev_identify = find_skill_by_number(who, idskill1)->level;
            }
            if (idskill2 && find_skill_by_number(who, idskill2)) {
                lev_identify += find_skill_by_number(who, idskill2)->level;
            }
        }
    } else {
        LOG(llevError, "Query_cost: item %s hasn't got a valid type\n", tmp->name);
    }
    val += val * (sin(tmp->count) / sqrt(lev_identify * 3 + 1.0));

    return val;
}

/**
 * Calculate the buy price multiplier based on a player's charisma.
 *
 * @param charisma player's charisma.
 * @return buy multiplier between 2 and 0.5.
 */
static float shop_buy_multiplier(int charisma) {
    float multiplier = 1 / (0.38 * pow(1.06, charisma));

    if (multiplier > 2) {
        return 2;
    } else if (multiplier < 0.5) {
        return 0.5;
    } else {
        return multiplier;
    }
}

/**
 * Calculate the buy price multiplier based on a player's bargaining skill.
 * The reciprocal of this result can be used as a sell multiplier.
 *
 * @param lev_bargain player's bargaining level.
 * @return buy multiplier between 1 and 0.5.
 */
static float shop_bargain_multiplier(int lev_bargain) {
    return 1 - 0.5 * lev_bargain / settings.max_level;
}

uint64_t shop_price_buy(const object *tmp, object *who) {
    assert(who != NULL && who->type == PLAYER);
    uint64_t val = price_base(tmp);

    const char *key = object_get_value(tmp, "price_adjustment_buy");
    if (key != NULL) {
        float ratio = atof(key);
        return val * ratio;
    }

    if (tmp->type == GEM) {
        return 1.03 * val;
    }

    int bargain_level = 0;
    if (find_skill_by_number(who, SK_BARGAINING)) {
        bargain_level = find_skill_by_number(who, SK_BARGAINING)->level;
    }

    float multiplier = shop_bargain_multiplier(bargain_level);
    multiplier *= shop_buy_multiplier(who->stats.Cha);

    // Limit buy price multiplier to 0.5, no matter what.
    val *= multiplier > 0.5 ? multiplier : 0.5;

    /*
        * When buying, if the item was sold by another player, it is
        * ok to let the item be sold cheaper, according to the
        * specialisation of the shop. If a player sold an item here,
        * then his sale price was multiplied by the specialisation
        * ratio, to do the same to the buy price will not generate
        * extra money. However, the same is not true of generated
        * items, these have to /divide/ by the specialisation, so
        * that the price is never less than what they could
        * be sold for (otherwise players could camp map resets to
        * make money).
        * In game terms, a non-specialist shop might not recognise
        * the true value of the items it sells (much like how people
        * sometimes find antiques in a junk shop in real life).
        */
    if (QUERY_FLAG(tmp, FLAG_PLAYER_SOLD)) {
        val = (int64_t)val*shop_greed(who->map)
            *shop_specialisation_ratio(tmp, who->map)
            /shop_approval(who->map, who);
    } else {
        val = (int64_t)val*shop_greed(who->map)
            /(shop_specialisation_ratio(tmp, who->map)
                *shop_approval(who->map, who));
    }

    return val;
}

uint64_t shop_price_sell(const object *tmp, object *who) {
    assert(who != NULL && who->type == PLAYER);
    uint64_t val = price_base(tmp);
    bool identified = QUERY_FLAG(tmp, FLAG_IDENTIFIED) || !need_identify(tmp);

    const char *key = object_get_value(tmp, "price_adjustment_sell");
    if (key != NULL) {
        float ratio = atof(key);
        return val * ratio;
    }

    if (tmp->type == GEM) {
        return 0.97 * val;
    }

    // Shops value unidentified items less.
    if (!identified) {
        if (tmp->arch != NULL) {
            // Unidentified standard objects are only worth a little less.
            if (QUERY_FLAG(tmp, FLAG_BEEN_APPLIED)) {
                val *= 0.85;
            } else {
                val *= 0.70;
            }
        } else {
            // No archetype, so probably an artifact.
            val /= 2;
        }
    }

    // Selling to shops yields roughly 50% of the base price.
    val /= 2;

    /* Limit amount of money you can get for really great items. */
    int number = (tmp->nrof == 0) ? 1 : tmp->nrof;
    val = value_limit(val, number, who, 1);

    val = (int64_t)val*shop_specialisation_ratio(tmp, who->map)*
            shop_approval(who->map, who)/shop_greed(who->map);
    return val;
}

/**
 * Find the coin type that is worth more than 'c'.  Starts at the
 * cointype placement.
 *
 * @param c
 * value we're searching.
 * @param cointype
 * first coin type to search.
 * @return
 * coin archetype, NULL if none found.
 */
static archetype *find_next_coin(uint64_t c, int *cointype) {
    archetype *coin;

    do {
        if (coins[*cointype] == NULL)
            return NULL;
        coin = find_archetype(coins[*cointype]);
        if (coin == NULL)
            return NULL;
        *cointype += 1;
    } while (coin->clone.value > (int64_t) c);

    return coin;
}

/**
 * Converts a price to number of coins.
 *
 * While cost is 64 bit, the number of any coin is still really
 * limited to 32 bit (size of nrof field).  If it turns out players
 * have so much money that they have more than 2 billion platinum
 * coins, there are certainly issues - the easiest fix at that
 * time is to add a higher denomination (mithril piece with
 * 10,000 silver or something)
 *
 * @param cost
 * value to transform to currency.
 * @param largest_coin
 * maximum coin to give the price into, should be between 0 and NUM_COINS - 1.
 * @param buf
 * buffer to append to, if NULL a new one is returned.
 * @return
 * buffer containing the price, either buf or if NULL a new StringBuffer.
 */
static StringBuffer *cost_string_from_value(uint64_t cost, int largest_coin, StringBuffer *buf) {
    archetype *coin, *next_coin;
    uint32_t num;
    int cointype = largest_coin;

    if (cointype < 0)
        cointype = 0;
    else if (cointype >= NUM_COINS)
        cointype = NUM_COINS - 1;

    if (!buf)
        buf = stringbuffer_new();

    coin = find_next_coin(cost, &cointype);
    if (coin == NULL) {
        stringbuffer_append_string(buf, "nothing");
        return buf;
    }

    num = cost/coin->clone.value;
    /* so long as nrof is 32 bit, this is true.
     * If it takes more coins than a person can possibly carry, this
     * is basically true.
     */
    if ((cost/coin->clone.value) > UINT32_MAX) {
        stringbuffer_append_string(buf, "an unimaginable sum of money.");
        return buf;
    }

    cost -= (uint64_t)num*(uint64_t)coin->clone.value;
    if (num == 1)
        stringbuffer_append_printf(buf, "1 %s", coin->clone.name);
    else
        stringbuffer_append_printf(buf, "%u %ss", num, coin->clone.name);

    next_coin = find_next_coin(cost, &cointype);
    if (next_coin == NULL)
        return buf;

    do {
        coin = next_coin;
        num = cost/coin->clone.value;
        cost -= (uint64_t)num*(uint64_t)coin->clone.value;

        if (cost == 0)
            next_coin = NULL;
        else
            next_coin = find_next_coin(cost, &cointype);

        if (next_coin) {
            /* There will be at least one more string to add to the list,
             * use a comma.
             */
            stringbuffer_append_string(buf, ", ");
        } else {
            stringbuffer_append_string(buf, " and ");
        }
        if (num == 1)
            stringbuffer_append_printf(buf, "1 %s", coin->clone.name);
        else
            stringbuffer_append_printf(buf, "%u %ss", num, coin->clone.name);
    } while (next_coin);

    return buf;
}

/**
 * Returns a string representing the money's value, in plain coins.
 *
 * @param coin
 * coin. Must be of type MONEY.
 * @param buf
 * buffer to append to. Must not be NULL.
 * @return
 * buf with the value.
 */
static StringBuffer *real_money_value(const object *coin, StringBuffer *buf) {
    assert(coin->type == MONEY);
    assert(buf);

    stringbuffer_append_printf(buf, "%ld %s", (long)coin->nrof, coin->nrof == 1 ? coin->name : coin->name_pl);
    return buf;
}

char *cost_str(uint64_t cost) {
    return stringbuffer_finish(cost_string_from_value(cost, LARGEST_COIN_GIVEN, NULL));
}

char *cost_approx_str(const object *tmp, object *who) {
    uint64_t approx_val = price_approx(tmp, who);
    int idskill1 = 0;
    int idskill2 = 0;
    const typedata *tmptype;

    StringBuffer *buf = stringbuffer_new();

    /* money it's pretty hard to not give the exact price, so skip all logic and just return the real value. */
    if (tmp->type == MONEY) {
        return stringbuffer_finish(real_money_value(tmp, buf));
    }

    tmptype = get_typedata(tmp->type);
    if (tmptype) {
        idskill1 = tmptype->identifyskill;
        idskill2 = tmptype->identifyskill2;
    }

    /* we show an approximate price if
     * 1) we are approximating
     * 2) there either is no id skill(s) for the item, or we don't have them
     * 3) we don't have bargaining skill either
     */
    if (!idskill1 || !find_skill_by_number(who, idskill1)) {
        if (!idskill2 || !find_skill_by_number(who, idskill2)) {
            if (!find_skill_by_number(who, SK_BARGAINING)) {
                int num;
                int cointype = LARGEST_COIN_GIVEN;
                archetype *coin = find_next_coin(approx_val, &cointype);

                if (coin == NULL) {
                    stringbuffer_append_string(buf, "nothing");
                    return stringbuffer_finish(buf);
                }

                num = approx_val/coin->clone.value;
                if (num == 1)
                    stringbuffer_append_printf(buf, "about one %s", coin->clone.name);
                else if (num < 5)
                    stringbuffer_append_printf(buf, "a few %s", coin->clone.name_pl);
                else if (num < 10)
                    stringbuffer_append_printf(buf, "several %s", coin->clone.name_pl);
                else if (num < 25)
                    stringbuffer_append_printf(buf, "a moderate amount of %s", coin->clone.name_pl);
                else if (num < 100)
                    stringbuffer_append_printf(buf, "lots of %s", coin->clone.name_pl);
                else if (num < 1000)
                    stringbuffer_append_printf(buf, "a great many %s", coin->clone.name_pl);
                else
                    stringbuffer_append_printf(buf, "a vast quantity of %s", coin->clone.name_pl);
                return stringbuffer_finish(buf);
            }
        }
    }

    // If we get here, return the price we guessed.
    stringbuffer_delete(buf);
    return cost_str(approx_val);
}

uint64_t query_money(const object *op) {
    uint64_t total = 0;

    if (op->type != PLAYER && op->type != CONTAINER) {
        LOG(llevError, "Query money called with non player/container\n");
        return 0;
    }
    FOR_INV_PREPARE(op, tmp) {
        if (tmp->type == MONEY) {
            total += (uint64_t)tmp->nrof*(uint64_t)tmp->value;
        } else if (tmp->type == CONTAINER
        && QUERY_FLAG(tmp, FLAG_APPLIED)
        && (tmp->race == NULL || strstr(tmp->race, "gold"))) {
            total += query_money(tmp);
        }
    } FOR_INV_FINISH();
    return total;
}

/**
 * Takes the amount of money from the the player inventory and from it's various
 * pouches using the pay_from_container() function.
 *
 * @param to_pay
 * amount to pay.
 * @param pl
 * player paying.
 * @return
 * 0 if not enough money, in which case nothing is removed, 1 if money was removed.
 * @todo check if pl is a player, as query_money() expects that. Check if fix_object() call is required.
 */
int pay_for_amount(uint64_t to_pay, object *pl) {
    if (to_pay == 0)
        return 1;
    if (to_pay > query_money(pl))
        return 0;

    to_pay = pay_from_container(pl, pl, to_pay);

    FOR_INV_PREPARE(pl, pouch) {
        if (to_pay <= 0)
            break;
        if (pouch->type == CONTAINER
        && QUERY_FLAG(pouch, FLAG_APPLIED)
        && (pouch->race == NULL || strstr(pouch->race, "gold"))) {
            to_pay = pay_from_container(pl, pouch, to_pay);
        }
    } FOR_INV_FINISH();
    if (to_pay > 0) {
        LOG(llevError, "pay_for_amount: Cannot remove enough money -- %"FMT64U" remains\n", to_pay);
    }

    fix_object(pl);
    return 1;
}

/**
 * DAMN: This is now a wrapper for pay_from_container, which is
 * called for the player, then for each active container that can hold
 * money until op is paid for.  Change will be left wherever the last
 * of the price was paid from.
 *
 * @param op
 * object to buy.
 * @param pl
 * player buying.
 * @return
 * 1 if object was bought, 0 else.
 * @todo check if pl is a player, as query_money() expects a player.
 */
int pay_for_item(object *op, object *pl) {
    uint64_t to_pay = shop_price_buy(op, pl);

    if (to_pay == 0)
        return 1;
    if (to_pay > query_money(pl))
        return 0;

    /* We compare the paid price with the one for a player
     * without bargaining skill.
     * This determins the amount of exp (if any) gained for bargaining.
     */
    int64_t saved_money = price_base(op) - to_pay;
    if (saved_money > 0)
        change_exp(pl, saved_money, "bargaining", SK_EXP_NONE);

    to_pay = pay_from_container(pl, pl, to_pay);

    FOR_INV_PREPARE(pl, pouch) {
        if (to_pay <= 0)
            break;
        if (pouch->type == CONTAINER
        && QUERY_FLAG(pouch, FLAG_APPLIED)
        && (pouch->race == NULL || strstr(pouch->race, "gold"))) {
            to_pay = pay_from_container(pl, pouch, to_pay);
        }
    } FOR_INV_FINISH();
    if (to_pay > 0) {
        LOG(llevError, "pay_for_item: Cannot remove enough money -- %"FMT64U" remains\n", to_pay);
    }
    if (settings.real_wiz == FALSE && QUERY_FLAG(pl, FLAG_WAS_WIZ))
        SET_FLAG(op, FLAG_WAS_WIZ);
    fix_object(pl);
    return 1;
}

/**
 * This function removes a given amount from a list of coins.
 *
 * @param coin_objs
 * the list coins to remove from; the list must be ordered
 * from least to most valuable coin.
 * @param remain
 * the value (in silver coins) to remove
 * @return
 * the value remaining
 */
static int64_t remove_value(object *coin_objs[], int64_t remain) {
    int i;

    for (i = 0; i < NUM_COINS; i++) {
        int count;
        int64_t num_coins;

        if ((int64_t)coin_objs[i]->nrof * coin_objs[i]->value > remain) {
            num_coins = remain/coin_objs[i]->value;
            if ((uint64_t)num_coins*(uint64_t)coin_objs[i]->value < (uint64_t) remain) {
                num_coins++;
            }
        } else {
            num_coins = coin_objs[i]->nrof;
        }
        remain -= (int64_t)num_coins*(int64_t)coin_objs[i]->value;
        coin_objs[i]->nrof -= num_coins;
        /* Now start making change.  Start at the coin value
         * below the one we just did, and work down to
         * the lowest value.
         */
        count = i-1;
        while (remain < 0 && count >= 0) {
            num_coins = -remain/coin_objs[count]->value;
            coin_objs[count]->nrof += num_coins;
            remain += num_coins*coin_objs[count]->value;
            count--;
        }
    }

    return remain;
}

/**
 * This function adds a given amount to a list of coins.
 *
 * @param coin_objs the list coins to add to; the list must be ordered
 * from least to most valuable coin
 *
 * @param value the value (in silver coins) to add
 */
static void add_value(object *coin_objs[], int64_t value) {
    int i;

    for (i = NUM_COINS-LARGEST_COIN_GIVEN-1; i >= 0; i--) {
        uint32_t nrof;

        nrof = (uint32_t)(value/coin_objs[i]->value);
        value -= nrof*coin_objs[i]->value;
        coin_objs[i]->nrof += nrof;
    }
}

/**
 * Insert a list of objects into a player object.
 *
 * @param pl the player to add to
 *
 * @param container the container (inside the player object) to add to
 *
 * @param objects the list of objects to add; the objects will be either
 * inserted into the player object or freed
 *
 * @param objects_len the length of objects
 */
static void insert_objects(object *pl, object *container, object *objects[], int objects_len) {
    int i, one = 0;

    for (i = 0; i < objects_len; i++) {
        if (objects[i]->nrof > 0) {
            object_insert_in_ob(objects[i], container);
            one = 1;
        } else {
            object_free_drop_inventory(objects[i]);
        }
    }
    if (one)
        esrv_update_item(UPD_WEIGHT, pl, container);
}

/**
 * This pays for the item, and takes the proper amount of money off
 * the player.
 *
 * DAMN: This function is used for the player, then for any active
 * containers that can hold money.
 *
 * @param pl
 * player paying.
 * @param pouch
 * container (pouch or player) to remove the coins from.
 * @param to_pay
 * required amount.
 * @return
 * amount still not paid after using "pouch".
 */
static uint64_t pay_from_container(object *pl, object *pouch, uint64_t to_pay) {
    size_t i;
    int64_t remain;
    object *coin_objs[NUM_COINS];
    object *other_money[16]; /* collects MONEY objects not matching coins[] */
    size_t other_money_len; /* number of allocated entries in other_money[] */
    archetype *at;

    if (pouch->type != PLAYER && pouch->type != CONTAINER)
        return to_pay;

    remain = to_pay;
    for (i = 0; i < NUM_COINS; i++)
        coin_objs[i] = NULL;

    /* This hunk should remove all the money objects from the player/container */
    other_money_len = 0;
    FOR_INV_PREPARE(pouch, tmp) {
        if (tmp->type == MONEY) {
            for (i = 0; i < NUM_COINS; i++) {
                if (!strcmp(coins[NUM_COINS-1-i], tmp->arch->name)
                && (tmp->value == tmp->arch->clone.value)) {
                    /* This should not happen, but if it does, just
                     * merge the two.
                     */
                    if (coin_objs[i] != NULL) {
                        LOG(llevError, "%s has two money entries of (%s)\n", pouch->name, coins[NUM_COINS-1-i]);
                        object_remove(tmp);
                        coin_objs[i]->nrof += tmp->nrof;
                        object_free_drop_inventory(tmp);
                    } else {
                        object_remove(tmp);
                        coin_objs[i] = tmp;
                    }
                    break;
                }
            }
            if (i == NUM_COINS) {
                if (other_money_len >= sizeof(other_money)/sizeof(*other_money)) {
                    LOG(llevError, "pay_for_item: Cannot store non-standard money object %s\n", tmp->arch->name);
                } else {
                    object_remove(tmp);
                    other_money[other_money_len++] = tmp;
                }
            }
        }
    } FOR_INV_FINISH();

    /* Fill in any gaps in the coin_objs array - needed to make change.      */
    /* Note that the coin_objs array goes from least value to greatest value */
    for (i = 0; i < NUM_COINS; i++)
        if (coin_objs[i] == NULL) {
            at = find_archetype(coins[NUM_COINS-1-i]);
            if (at == NULL)
                LOG(llevError, "Could not find %s archetype\n", coins[NUM_COINS-1-i]);
            coin_objs[i] = object_new();
            object_copy(&at->clone, coin_objs[i]);
            coin_objs[i]->nrof = 0;
        }

    /* Try to pay from standard coins first. */
    remain = remove_value(coin_objs, remain);

    /* Now pay from non-standard coins until all is paid. */
    for (i = 0; i < other_money_len && remain > 0; i++) {
        uint32_t nrof;
        object *coin;

        coin = other_money[i];

        /* Find the minimal number of coins to use. This prevents converting
         * excess non-standard coins to standard money.
         */
        nrof = (remain+coin->value-1)/coin->value;
        if (nrof > coin->nrof) {
            nrof = coin->nrof;
        }
        coin->nrof -= nrof;
        add_value(coin_objs, nrof*coin->value);

        remain = remove_value(coin_objs, remain);
    }

    /* re-insert remaining coins into player */
    insert_objects(pl, pouch, coin_objs, NUM_COINS);
    insert_objects(pl, pouch, other_money, other_money_len);

    return(remain);
}

/**
 * Helper function for can_pay(). Checks all items from item and
 * item->below, and recurse if inventory found.
 * coincount is supposed to be of size NUM_COINS. Parameters can't be NULL.
 *
 * @param pl
 * player.
 * @param item
 * item to check for.
 * @param[out] unpaid_count
 * how many unpaid items are left.
 * @param[out] unpaid_price
 * total price unpaid.
 * @param coincount
 * array of NUM_COINS size, will contain how many coins of the type the player has.
 */
static void count_unpaid(object *pl, object *item, int *unpaid_count, uint64_t *unpaid_price, uint32_t *coincount) {
    int i;

    FOR_OB_AND_BELOW_PREPARE(item) {
        if (QUERY_FLAG(item, FLAG_UNPAID)) {
            (*unpaid_count)++;
            (*unpaid_price) += shop_price_buy(item, pl);
        }
        /* Merely converting the player's monetary wealth won't do.
         * If we did that, we could print the wrong numbers for the
         * coins, so we count the money instead.
         */
        for (i = 0; i < NUM_COINS; i++)
            if (!strcmp(coins[i], item->arch->name)) {
                coincount[i] += item->nrof;
                break;
            }
        if (item->inv)
            count_unpaid(pl, item->inv, unpaid_count, unpaid_price, coincount);
    } FOR_OB_AND_BELOW_FINISH();
}

/**
 * Checks all unpaid items in op's inventory, adds up all the money they
 * have, and checks that they can actually afford what they want to buy.
 * Prints appropriate messages to the player.
 *
 * @param pl
 * player trying to bug.
 * @retval 1
 * player could buy the items.
 * @retval 0
 * some items can't be bought.
 */
int can_pay(object *pl) {
    int unpaid_count = 0, i;
    uint64_t unpaid_price = 0;
    uint64_t player_wealth = query_money(pl);
    uint32_t coincount[NUM_COINS];

    if (!pl || pl->type != PLAYER) {
        LOG(llevError, "can_pay(): called against something that isn't a player\n");
        return 0;
    }

    for (i = 0; i < NUM_COINS; i++)
        coincount[i] = 0;

    count_unpaid(pl, pl->inv, &unpaid_count, &unpaid_price, coincount);

    if (unpaid_price > player_wealth) {
        char buf[MAX_BUF], coinbuf[MAX_BUF];
        int denominations = 0;
        char *value = cost_str(unpaid_price);

        snprintf(buf, sizeof(buf), "You have %d unpaid items that would cost you %s, ", unpaid_count, value);
        free(value);
        for (i = 0; i < NUM_COINS; i++) {
            if (coincount[i] > 0 && coins[i]) {
                if (denominations == 0)
                    snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "but you only have");
                denominations++;
                snprintf(coinbuf, sizeof(coinbuf), " %u %s,", coincount[i], find_archetype(coins[i])->clone.name_pl);
                snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "%s", coinbuf);
            }
        }
        if (denominations == 0)
            snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "but you don't have any money.");
        else if (denominations > 1)
            make_list_like(buf);
        draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_SHOP,
                      MSG_TYPE_SHOP_PAYMENT, buf);
        return 0;
    } else
        return 1;
}

int shop_pay_unpaid(object *pl, object *op) {
    char name_op[MAX_BUF];
    int ret = 1;

    if (op != NULL && op->inv)
        ret = shop_pay_unpaid(pl, op->inv);

    if (!ret)
        return 0;

    if (op != NULL && op->below)
        ret = shop_pay_unpaid(pl, op->below);

    if (!ret)
        return 0;

    if (op != NULL && QUERY_FLAG(op, FLAG_UNPAID)) {
        uint64_t price = shop_price_buy(op, pl);
        if (!pay_for_item(op, pl)) {
            uint64_t i = price - query_money(pl);
            char *missing = cost_str(i);

            CLEAR_FLAG(op, FLAG_UNPAID);
            query_name(op, name_op, MAX_BUF);
            draw_ext_info_format(NDI_UNIQUE, 0, pl,
                                 MSG_TYPE_SHOP, MSG_TYPE_SHOP_PAYMENT,
                                 "You lack %s to buy %s.",
                                 missing, name_op);
            free(missing);
            SET_FLAG(op, FLAG_UNPAID);
            return 0;
        } else {
            object *tmp;
            char *value = cost_str(price);

            CLEAR_FLAG(op, FLAG_UNPAID);
            CLEAR_FLAG(op, FLAG_PLAYER_SOLD);
            query_name(op, name_op, MAX_BUF);
            draw_ext_info_format(NDI_UNIQUE, 0, pl,
                                 MSG_TYPE_SHOP, MSG_TYPE_SHOP_PAYMENT,
                                 "You paid %s for %s.",
                                 value, name_op);
            free(value);
            tmp = object_merge(op, NULL);
            if (pl->type == PLAYER && !tmp) {
                /* If item wasn't merged we update it. If merged, object_merge() handled everything for us. */
                esrv_update_item(UPD_FLAGS|UPD_NAME, pl, op);
            }
        }
    }
    return 1;
}

/**
 * Player is selling an item. Give money, print appropriate messages.
 *
 * This function uses the coins[] array to know what coins are available.
 *
 * Modified to fill available race: gold containers before dumping
 * remaining coins in character's inventory.
 *
 * @param op
 * object to sell.
 * @param pl
 * player. Shouldn't be NULL or non player.
 */
void sell_item(object *op, object *pl) {
    object *tmp;
    archetype *at;
    char obj_name[MAX_BUF];

    query_name(op, obj_name, MAX_BUF);

    if (pl == NULL || pl->type != PLAYER) {
        LOG(llevDebug, "Object other than player tried to sell something.\n");
        return;
    }

    if (execute_event(op, EVENT_SELLING, pl, NULL, NULL, SCRIPT_FIX_ALL) != 0)
        return;

    if (op->custom_name)
        FREE_AND_CLEAR_STR(op->custom_name);

    uint64_t price = shop_price_sell(op, pl);
    if (price == 0) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl,
                             MSG_TYPE_SHOP, MSG_TYPE_SHOP_SELL,
                             "We're not interested in %s.",
                             obj_name);
        return;
    }

    /* We compare the price with the one for a player
     * without bargaining skill.
     * This determins the amount of exp (if any) gained for bargaining.
     * exp/10 -> 1 for each gold coin
     */
    int64_t extra_gain = price - price_base(op);
    if (extra_gain > 0) {
        change_exp(pl, extra_gain/10, "bargaining", SK_EXP_NONE);
    }

    char *value_str = cost_str(price);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_SHOP, MSG_TYPE_SHOP_SELL,
            "You receive %s for %s.", value_str, obj_name);
    free(value_str);

    for (int count = LARGEST_COIN_GIVEN; coins[count] != NULL; count++) {
        at = find_archetype(coins[count]);
        if (at == NULL)
            LOG(llevError, "Could not find %s archetype\n", coins[count]);
        else if ((price/at->clone.value) > 0) {
            FOR_INV_PREPARE(pl, pouch) {
                if (pouch->type == CONTAINER
                && QUERY_FLAG(pouch, FLAG_APPLIED)
                && pouch->race
                && strstr(pouch->race, "gold")) {
                    int w = at->clone.weight*(100-pouch->stats.Str)/100;
                    int n = price/at->clone.value;

                    if (w == 0)
                        w = 1;    /* Prevent divide by zero */
                    if (n > 0
                    && (!pouch->weight_limit || pouch->carrying+w <= pouch->weight_limit)) {
                        if (pouch->weight_limit
                        && (pouch->weight_limit-pouch->carrying)/w < n)
                            n = (pouch->weight_limit-pouch->carrying)/w;

                        tmp = object_new();
                        object_copy(&at->clone, tmp);
                        tmp->nrof = n;
                        price -= (uint64_t)tmp->nrof*(uint64_t)tmp->value;
                        tmp = object_insert_in_ob(tmp, pouch);
                        esrv_update_item(UPD_WEIGHT, pl, pl);
                    }
                }
            } FOR_INV_FINISH();
            if (price/at->clone.value > 0) {
                tmp = object_new();
                object_copy(&at->clone, tmp);
                tmp->nrof = price/tmp->value;
                price -= (uint64_t)tmp->nrof*(uint64_t)tmp->value;
                tmp = object_insert_in_ob(tmp, pl);
                esrv_update_item(UPD_WEIGHT, pl, pl);
            }
        }
    }

    if (price != 0) {
        LOG(llevError, "Warning - payment not zero: %" PRIo64 "\n", price);
    }

    SET_FLAG(op, FLAG_UNPAID);
    identify(op);
}

/**
 * returns a double that is the ratio of the price that a shop will offer for
 * item based on the shops specialisation. Does not take account of greed,
 * returned value is between (2*SPECIALISATION_EFFECT-1) and 1 and in any
 * event is never less than 0.1 (calling functions divide by it)
 *
 * @param item
 * item to get ratio of.
 * @param map
 * shop map.
 * @return
 * ratio specialisation for the item.
 */
static double shop_specialisation_ratio(const object *item, const mapstruct *map) {
    shopitems *items = map->shopitems;
    double ratio = SPECIALISATION_EFFECT, likedness = 0.001;
    int i;

    if (item == NULL) {
        LOG(llevError, "shop_specialisation_ratio: passed a NULL item for map %s\n", map->path);
        return 0;
    }
    if (item->type == (uint8_t)-1) {
        LOG(llevError, "shop_specialisation_ratio: passed an item with an invalid type\n");
        /*
         * I'm not really sure what the /right/ thing to do here is,
         * these types of item shouldn't exist anyway, but returning
         * the ratio is probably the best bet.."
         */
        return ratio;
    }
    if (map->shopitems) {
        for (i = 0; i < items[0].index; i++)
            if (items[i].typenum == item->type || (items[i].typenum == -1 && likedness == 0.001))
                likedness = items[i].strength/100.0;
    }
    if (likedness > 1.0) { /* someone has been rather silly with the map headers. */
        LOG(llevDebug, "shop_specialisation ratio: item type %d on map %s is above 100%%\n", item->type, map->path);
        likedness = 1.0;
    }
    if (likedness < -1.0) {
        LOG(llevDebug, "shop_specialisation ratio: item type %d on map %s is below -100%%\n", item->type, map->path);
        likedness = -1.0;
    }
    ratio = ratio+(1.0-ratio)*likedness;
    if (ratio <= 0.1)
        ratio = 0.1; /* if the ratio were much lower than this, we would get silly prices */
    return ratio;
}

/**
 * Gets shop's greed.
 *
 * @param map
 * map to get greed.
 * @return
 * greed of the shop on map, or 1 if it isn't specified.
 */
static double shop_greed(const mapstruct *map) {
    double greed = 1.0;

    if (map->shopgreed)
        return map->shopgreed;
    return greed;
}

double shop_approval(const mapstruct *map, const object *player) {
    double approval = 1.0;

    if (map->shoprace) {
        approval = NEUTRAL_RATIO;
        if (player->race && !strcmp(player->race, map->shoprace))
            approval = 1.0;
    }
    return approval;
}

/**
 * Limit the value of items based on the wealth of the shop.
 * If the item is close to the maximum value a shop will offer,
 * we start to reduce it, if the item is below the minimum value
 * the shop is prepared to trade in, then we don't want it and
 * offer nothing. If it isn't a shop, check whether we should do
 * generic value reduction.
 *
 * @param val
 * current price.
 * @param quantity
 * number of items.
 * @param who
 * player selling.
 * @param isshop
 * 0 if not a shop, 1 if a shop.
 * @return
 * maximum global value.
 */
static uint64_t value_limit(uint64_t val, int quantity, const object *who, int isshop) {
    uint64_t newval, unit_price;
    mapstruct *map;

    unit_price = val/quantity;
    if (!isshop || !who) {
        if (unit_price > 10000)
            newval = 8000+isqrt(unit_price)*20;
        else
            newval = unit_price;
    } else {
        if (!who->map) {
            LOG(llevError, "value_limit: asked shop price for ob %s on NULL map\n", who->name);
            return val;
        }
        map = who->map;
        if (map->shopmin && unit_price < map->shopmin)
            return 0;
        else if (map->shopmax && unit_price > map->shopmax/2)
            newval = MIN((map->shopmax/2)+isqrt(unit_price-map->shopmax/2), map->shopmax);
        else if (unit_price > 10000)
            newval = 8000+isqrt(unit_price)*20;
        else
            newval = unit_price;
    }
    newval *= quantity;
    return newval;
}

int shop_describe(const object *op) {
    mapstruct *map = op->map;
    /*shopitems *items=map->shopitems;*/
    int pos = 0, i;
    double opinion = 0;
    char tmp[MAX_BUF] = "\0", *value;

    if (op->type != PLAYER)
        return 0;

    /*check if there is a shop specified for this map */
    if (map->shopitems
    || map->shopgreed
    || map->shoprace
    || map->shopmin
    || map->shopmax) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SHOP, MSG_TYPE_SHOP_LISTING,
                      "From looking at the nearby shop you determine that it trades in:");

        if (map->shopitems) {
            for (i = 0; i < map->shopitems[0].index; i++) {
                if (map->shopitems[i].name && map->shopitems[i].strength > 10) {
                    snprintf(tmp+pos, sizeof(tmp)-pos, "%s, ", map->shopitems[i].name_pl);
                    pos += strlen(tmp+pos);
                }
            }
        }
        if (!pos)
            strcpy(tmp, "a little of everything.");

        /* format the string into a list */
        make_list_like(tmp);
        draw_ext_info(NDI_UNIQUE, 0, op,
                      MSG_TYPE_SHOP, MSG_TYPE_SHOP_LISTING, tmp);

        if (map->shopmax) {
            value = cost_str(map->shopmax);
            draw_ext_info_format(NDI_UNIQUE, 0, op,
                                 MSG_TYPE_SHOP, MSG_TYPE_SHOP_MISC,
                                 "It won't trade for items above %s.",
                                 value);
            free(value);
        }

        if (map->shopmin) {
            value = cost_str(map->shopmin);
            draw_ext_info_format(NDI_UNIQUE, 0, op,
                                 MSG_TYPE_SHOP, MSG_TYPE_SHOP_MISC,
                                 "It won't trade in items worth less than %s.",
                                 value);
            free(value);
        }

        if (map->shopgreed) {
            if (map->shopgreed > 2.0)
                draw_ext_info(NDI_UNIQUE, 0, op,
                              MSG_TYPE_SHOP, MSG_TYPE_SHOP_MISC,
                              "It tends to overcharge massively.");
            else if (map->shopgreed > 1.5)
                draw_ext_info(NDI_UNIQUE, 0, op,
                              MSG_TYPE_SHOP, MSG_TYPE_SHOP_MISC,
                              "It tends to overcharge substantially.");
            else if (map->shopgreed > 1.1)
                draw_ext_info(NDI_UNIQUE, 0, op,
                              MSG_TYPE_SHOP, MSG_TYPE_SHOP_MISC,
                              "It tends to overcharge slightly.");
            else if (map->shopgreed < 0.9)
                draw_ext_info(NDI_UNIQUE, 0, op,
                              MSG_TYPE_SHOP, MSG_TYPE_SHOP_MISC,
                              "It tends to undercharge.");
        }
        if (map->shoprace) {
            opinion = shop_approval(map, op);
            if (opinion > 0.8)
                draw_ext_info(NDI_UNIQUE, 0, op,
                              MSG_TYPE_SHOP, MSG_TYPE_SHOP_MISC,
                              "You think the shopkeeper likes you.");
            else if (opinion > 0.5)
                draw_ext_info(NDI_UNIQUE, 0, op,
                              MSG_TYPE_SHOP, MSG_TYPE_SHOP_MISC,
                              "The shopkeeper seems unconcerned by you.");
            else
                draw_ext_info(NDI_UNIQUE, 0, op,
                              MSG_TYPE_SHOP, MSG_TYPE_SHOP_MISC,
                              "The shopkeeper seems to have taken a dislike to you.");
        }
    } else draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SHOP, MSG_TYPE_SHOP_MISC,
                             "There is no shop nearby.");

    return 1;
}

/**
 * Check if the given map coordinates are in a shop.
 */
static bool coords_in_shop(mapstruct *map, int x, int y) {
    FOR_MAP_PREPARE(map, x, y, floor)
        if (floor->type == SHOP_FLOOR) return true;
    FOR_MAP_FINISH();
    return false;
}

bool shop_contains(object *ob) {
    if (!ob->map) return 0;
    return coords_in_shop(ob->map, ob->x, ob->y);
}
