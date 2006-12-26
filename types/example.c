/** @file example.c
 * An example of how to define a type for the object 'method' system. This
 * example is for a simple food type with no special behavoir except for when it
 * is applied it gives the food value to the player. Do not use this for real code,
 and just consider it an example of how to
 */

/**
 * Initialize the food object type. Call this from init_ob_types in
 * server/ob_types.c
 */
void init_type_food() {
    register_apply(FOOD, food_type_apply);
    register_apply(FLESH, food_type_apply);
    register_apply(DRINK, food_type_apply);
}

/**
 * ob_method handler for FOOD, FLESH and DRINK object types.
 * @todo Handle cursed food
 * @todo Add hook for dragon resistance gaining
 * @todo Give special messages when full
 * @note Remember this is just an example ;-)
 */
method_ret food_type_apply(ob_methods *context, object *ob, object *pl) {
    method_ret can_apply;
    
    /* 
     * Call the 'can_apply' method for the player to check if the player can
     * indeed apply it (checking if the player can reach it, etc).
     */
    can_apply = ob_can_apply(pl, ob);
    if (can_apply == METHOD_OK) {
	pl->stats.food += ob->stats.food;
        if (pl->stats.food > 999)
            pl->stats.food = 999;
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
		     "You eat the %s!",
		     "You eat the %s!",
		     query_name(ob));
        decrease_ob(ob);
    }
    return METHOD_OK;
}
