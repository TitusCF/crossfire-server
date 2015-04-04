#ifndef _SHOP_H
#define _SHOP_H

/**
 * Adjust the value of the given item based on the player's skills. This
 * function should only be used when calculating "you reckon" prices.
 *
 * @param obj item in question. Must not be NULL.
 * @param who player trying to judge the value of the item. Must not be NULL.
 * @return approximate value of tmp.
 */
uint64_t price_approx(const object *obj, object *who);

/**
 * Determine the base (intrinsic) value of an item. This should not include
 * adjustments such as bargaining, charisma, or shop specialization.
 *
 * @param obj item in question.
 * @return base price.
 */
uint64_t price_base(const object *obj);

/**
 * Adjust the value of an item to be bought based on the player's bargaining skill and
 * charisma. This should only be used if the player is in a shop.
 *
 * @param obj item in question. Must not be NULL.
 * @param who player trying to judge the value of the item. Must not be NULL.
 * @return value of tmp.
 */
uint64_t shop_price_buy(const object *obj, object *who);

/**
 * Adjust the value of an item to be sold based on the player's bargaining skill and
 * charisma. This should only be used if the player is in a shop.
 *
 * @param obj item in question. Must not be NULL.
 * @param who player trying to judge the value of the item. Must not be NULL.
 * @return value of tmp.
 */
uint64_t shop_price_sell(const object *obj, object *who);

/**
 * Return the textual representation of a cost in a newly-allocated string.
 *
 * @param cost value to convert to text.
 * @return converted value the caller is responsible to free.
 */
char *cost_str(uint64_t cost);

/**
 * Return a textual cost approximation in a newly-allocated string.
 *
 * @param obj item to query the price of, must not be NULL.
 * @param who player asking for the price, must not be NULL.
 * @return converted value the caller is responsible to free.
 */
char *cost_approx_str(const object *obj, object *who);

/**
 * Determine the amount of money the given object contains, including what is
 * inside containers.
 *
 * @param op Player or container object
 * @return Total amount of money inside
 */
uint64_t query_money(const object *op);

int pay_for_amount(uint64_t to_pay, object *pl);
int can_pay(object *pl);

/**
 * Pay for each unpaid item carried by a player, including those inside
 * containers. It is a good idea to call can_pay() before using this function,
 * because items are paid for here in no particular order.
 *
 * @param pl Player making purchase
 * @param op Container to examine, usually the same player object
 * @retval 0 Player still has unpaid items
 * @retval 1 Player paid for all unpaid items
 */
int shop_pay_unpaid(object *pl, object *op);

int pay_for_item(object *op, object *pl);
void sell_item(object *op, object *pl);

/**
 * Return the approval ratio for a shop for a given player. This is based on
 * both the race of the shopkeeper and the player.
 *
 * @param map Map with a shop
 * @param player Player in question
 * @return Approval ratio between 0 and 1
 */
double shop_approval(const mapstruct *map, const object *player);

/**
 * Check if an object is in a shop.
 */
bool shop_contains(object *ob);

/**
 * Give the player a description of the shop on their current map. This is
 * used for the bargaining skill.
 *
 * @param op Non-null player to describe the shop to
 * @return Zero if the object is not a player, 1 otherwise
 */
int shop_describe(const object *op);

#endif
