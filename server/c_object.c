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
 * Object commands, including picking/dropping, locking, etc.
 * @todo clean multiple variations of same stuff (pickup and such), or rename for less confusion.
 */

#include "global.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "living.h"
#include "loader.h"
#include "shop.h"
#include "skills.h"
#include "sproto.h"

/** Valid names for pickup types. */
static const char *pickup_names[] = {
    "debug", "inhibit", "stop", "food", "drink",
    "valuables", "bow", "arrow", "helmet", "shield",
    "armour", "boots", "gloves", "cloak", "key",
    "missile", "melee", "magical", "potion", "spellbook",
    "skillscroll", "readables", "magicdevice", "notcursed", "jewels",
    "flesh", "container", "cursed", NULL
};

/** Value in @ref PU_xxx associated with @ref pickup_names.*/
static const uint32_t pickup_modes[] = {
    PU_DEBUG, PU_INHIBIT, PU_STOP, PU_FOOD, PU_DRINK, PU_VALUABLES, PU_BOW, PU_ARROW, PU_HELMET,
    PU_SHIELD, PU_ARMOUR, PU_BOOTS, PU_GLOVES, PU_CLOAK, PU_KEY, PU_MISSILEWEAPON, PU_MELEEWEAPON,
    PU_MAGICAL, PU_POTION, PU_SPELLBOOK, PU_SKILLSCROLL, PU_READABLES, PU_MAGIC_DEVICE,
    PU_NOT_CURSED, PU_JEWELS, PU_FLESH, PU_CONTAINER, PU_CURSED, 0
};

/**
 * Return the pickup index in @ref pickup_names and @ref pickup_modes
 * associated with the specified name.
 * The name may be the start of a mode if no ambiguity exists,
 * that is 'me' means 'melee', but 'm' will not match anything.
 * @param name mode to get the index of.
 * @return index, -1 if the name is invalid.
 */
static int get_pickup_mode_index(const char *name) {
    int best = -1;
    size_t len = strlen(name);
    for (size_t mode = 0; pickup_names[mode]; mode++) {
        if (!strcmp(pickup_names[mode], name)) {
            return mode;
        }
        if (len < strlen(pickup_names[mode]) && strncmp(name, pickup_names[mode], len) == 0) {
            if (best != -2) {
                if (best != -1) {
                    best = -2;
                } else {
                    best = mode;
                }
            }
        }
    }
    return best != -2 ? best : -1;
}

static void set_pickup_mode(const object *op, int i);

/*
 * Object id parsing functions
 */

/**
 * Search from start and through below for what matches best with params.
 * we use object_matches_string above - this gives us consistent behaviour
 * between many commands.  Return the best match, or NULL if no match.
 *
 * @param start
 * first object to start searching at.
 * @param pl
 * what object we're searching for.
 * @param params
 * what to search for.
 * @param aflag
 * Either 0 or AP_APPLY or AP_UNAPPLY. Used with apply -u, and apply -a to
 * only unapply applied, or apply unapplied objects.
 * @return
 * matching object, or NULL if no suitable.
**/
static object *find_best_apply_object_match(object *start, object *pl, const char *params, int aflag) {
    object *tmp, *best = NULL;
    int match_val = 0, tmpmatch;

    tmp = start;
    FOR_OB_AND_BELOW_PREPARE(tmp) {
        if (tmp->invisible)
            continue;
        if (aflag == AP_APPLY && QUERY_FLAG(tmp, FLAG_APPLIED))
            continue;
        if (aflag == AP_UNAPPLY && !QUERY_FLAG(tmp, FLAG_APPLIED))
            continue;
        tmpmatch = object_matches_string(pl, tmp, params);
        if (tmpmatch > match_val) {
            match_val = tmpmatch;
            best = tmp;
        }
    } FOR_OB_AND_BELOW_FINISH();
    return best;
}

/**
 * Shortcut to find_best_apply_object_match(pl->inv, pl, params, AF_NULL);
 *
 * @param pl
 * who to search an item for.
 * @param params
 * what to search for.
 * @return
 * matching object, or NULL if no suitable.
 **/
static object *find_best_object_match(object *pl, const char *params) {
    return find_best_apply_object_match(pl->inv, pl, params, AP_NULL);
}

/**
 * 'use_skill' command.
 *
 * @param pl
 * player.
 * @param params
 * skill to use, and optional parameters.
 */
void command_uskill(object *pl, const char *params) {
    if (*params == '\0') {
        draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Usage: use_skill <skill name>");
        return;
    }
    use_skill(pl, params);
}

/**
 * 'ready_skill' command.
 *
 * @param pl
 * player.
 * @param params
 * skill name.
 */
void command_rskill(object *pl, const char *params) {
    object *skill;

    if (*params == '\0') {
        draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Usage: ready_skill <skill name>");
        return;
    }
    skill = find_skill_by_name(pl, params);

    if (!skill) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_MISSING,
                             "You have no knowledge of the skill %s",
                             params);
        return;
    }
    change_skill(pl, skill, 0);
}

/**
 * Attempt to use a skill from its subtype. If multiple skills share the same,
 * the first found is used.
 * @param op who is applying a skill.
 * @param skill_subtype skill subtype.
 * @param params additional parameters to the skill.
 * @param missing_message message to display if op doesn't know the skill.
 */
static void do_skill_by_number(object *op, int skill_subtype, const char *params,
                               const char *missing_message) {
    object *skop = find_skill_by_number(op, skill_subtype);
    if (skop) {
        do_skill(op, op, skop, op->facing, *params == '\0' ? NULL : params);
        return;
    }

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_MISSING,
                  missing_message);
}

/* These functions (command_search, command_disarm) are really juse wrappers for
 * things like 'use_skill ...').  In fact, they should really be obsoleted
 * and replaced with those.
 */
/**
 * 'search' command.
 *
 * @param op
 * player.
 * @param params
 * unused.
 */
void command_search(object *op, const char *params) {
    do_skill_by_number(op, SK_FIND_TRAPS, params, "You don't know how to search for unusual things.");
}

/**
 * 'disarm' command.
 *
 * @param op
 * player.
 * @param params
 * unused.
 */
void command_disarm(object *op, const char *params) {
    do_skill_by_number(op, SK_DISARM_TRAPS, params, "You don't know how to disarm traps.");
}

/**
 * 'throw' command.
 *
 * A little special because we do want to pass the full params along
 * as it includes the object to throw.
 *
 * @param op
 * player.
 * @param params
 * what to throw.
 */
void command_throw(object *op, const char *params) {
    do_skill_by_number(op, SK_THROWING, params, "You have no knowledge of the skill throwing.");
}

/**
 * 'apply' command.
 *
 * @param op
 * player.
 * @param params
 * what to apply.
 */
void command_apply(object *op, const char *params) {
    int aflag = 0;
    object *inv = op->inv;
    object *item;

    if (*params == '\0') {
        apply_by_living_below(op);
        return;
    }

    while (*params == ' ')
        params++;
    if (!strncmp(params, "-a ", 3)) {
        aflag = AP_APPLY;
        params += 3;
    }
    if (!strncmp(params, "-u ", 3)) {
        aflag = AP_UNAPPLY;
        params += 3;
    }
    if (!strncmp(params, "-o ", 3)) {
        aflag = AP_OPEN;
        params += 3;
    }
    if (!strncmp(params, "-b ", 3)) {
        params += 3;
        if (op->container)
            inv = op->container->inv;
        else {
            inv = op;
            while (inv->above)
                inv = inv->above;
        }
    }
    while (*params == ' ')
        params++;

    item = find_best_apply_object_match(inv, op, params, aflag);
    if (item == NULL)
        item = find_best_apply_object_match(inv, op, params, AP_NULL);
    if (item) {
        apply_by_living(op, item, aflag, 0);
    } else
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "Could not find any match to the %s.",
                             params);
}

/**
 * Check if an item op can be put into a sack. If pl exists then tell
 * a player the reason of failure.
 *
 * @param pl
 * player.
 * @param sack
 * container to try to put into.
 * @param op
 * what to put in the sack.
 * @param nrof
 * number of objects (op) we want to put in. We specify it separately instead of
 * using op->nrof because often times, a player may have specified a
 * certain number of objects to drop, so we can pass that number, and
 * not need to use split_ob() and stuff.
 * @return
 * 1 if it will fit, 0 if it will not.
 */
int sack_can_hold(const object *pl, const object *sack, const object *op, uint32_t nrof) {
    char name[MAX_BUF];
    query_name(sack, name, MAX_BUF);

    if (!QUERY_FLAG(sack, FLAG_APPLIED)) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "The %s is not active.",
                             name);
        return 0;
    }
    if (sack == op) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "You can't put the %s into itself.",
                             name);
        return 0;
    }
    if (sack->race
    && (sack->race != op->race || op->type == CONTAINER || (sack->stats.food && sack->stats.food != op->type))) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "You can put only %s into the %s.",
                             sack->race,  name);
        return 0;
    }
    if (op->type == SPECIAL_KEY && sack->slaying && op->slaying) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "You can't put the key into %s.",
                             name);
        return 0;
    }
    if (sack->weight_limit) {
        int32_t new_weight;

        new_weight = sack->carrying+(nrof ? nrof : 1)
            /* Most non-containers should have op->carrying == 0. Icecubes, however, will not,
             * and we need to handle those.
             * Daniel Hawkins 2021-01-21
             */
            *(op->weight+(op->type == CONTAINER ? op->carrying*op->stats.Str : op->carrying))
            *(100-sack->stats.Str)/100;
        if (new_weight > sack->weight_limit) {
            draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                "That won't fit in the %s!",
                name);
            return 0;
        }
    }
    /* All other checks pass, must be OK */
    return 1;
}

/**
 * Try to pick up some item.
 *
 * @param pl
 * object (player or monster) picking up.
 * @param op
 * object to put tmp into.
 * @param tmp
 * object to pick up.
 * @param nrof
 * number of tmp to pick up (0 means all of them).
 */
static void pick_up_object(object *pl, object *op, object *tmp, int nrof) {
    /* buf needs to be big (more than 256 chars) because you can get
     * very long item names.
     */
    char buf[HUGE_BUF], name[MAX_BUF];
    object *env = tmp->env;
    uint32_t weight, effective_weight_limit;
    const int tmp_nrof = NROF(tmp);
    tag_t tag;
    mapstruct* map = tmp->map;
    int16_t x = tmp->x, y = tmp->y;

    /* IF the player is flying & trying to take the item out of a container
     * that is in his inventory, let him.  tmp->env points to the container
     * (sack, luggage, etc), tmp->env->env then points to the player (nested
     * containers not allowed as of now)
     */
    if ((pl->move_type&MOVE_FLYING)
    && !QUERY_FLAG(pl, FLAG_WIZ)
    && object_get_player_container(tmp) != pl) {
        draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "You are levitating, you can't reach the ground!");
        return;
    }
    if (QUERY_FLAG(tmp, FLAG_NO_DROP))
        return;

    if (QUERY_FLAG(tmp, FLAG_WAS_WIZ) && !QUERY_FLAG(pl, FLAG_WAS_WIZ)) {
        draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
                      "The object disappears in a puff of smoke! It must have been an illusion.");
        if (!QUERY_FLAG(tmp, FLAG_REMOVED))
            object_remove(tmp);
        object_free_drop_inventory(tmp);
        return;
    }

    if (nrof > tmp_nrof || nrof == 0)
        nrof = tmp_nrof;

    /* Figure out how much weight this object will add to the player */
    weight = tmp->weight*nrof;
    if (tmp->inv)
        weight += tmp->carrying*(100-tmp->stats.Str)/100;

    effective_weight_limit = get_weight_limit(MIN(pl->stats.Str, settings.max_stat));

    if (pl->weight+pl->carrying+weight > effective_weight_limit) {
        draw_ext_info(0, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
                      "That item is too heavy for you to pick up.");
        return;
    }

    if (settings.real_wiz == FALSE && QUERY_FLAG(pl, FLAG_WAS_WIZ))
        SET_FLAG(tmp, FLAG_WAS_WIZ);

    if (nrof != tmp_nrof) {
        char failure[MAX_BUF];

        tmp = object_split(tmp, nrof, failure, sizeof(failure));
        if (!tmp) {
            draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          failure);
            return;
        }
    } else {
        /* If the object is in a container, send a delete to the client.
         * - we are moving all the items from the container to elsewhere,
         * so it needs to be deleted.
         */
        if (!QUERY_FLAG(tmp, FLAG_REMOVED)) {
            object_remove(tmp); /* Unlink it */
        }
    }
    query_name(tmp, name, MAX_BUF);

    if (QUERY_FLAG(tmp, FLAG_UNPAID)) {
        char *value = cost_str(shop_price_buy(tmp, pl));
        if (op == pl) {
            snprintf(buf, sizeof(buf), "%s will cost you %s.", name, value);
        } else {
            snprintf(buf, sizeof(buf), "%s will cost you %s. You place it in your %s.", name, value, op->name);
        }
        free(value);
    } else {
        if (op == pl) {
            snprintf(buf, sizeof(buf), "You pick up the %s.", name);
        } else {
            snprintf(buf, sizeof(buf), "You pick up the %s and put it in your %s.", name, op->name);
        }
    }

    /* Now item is about to be picked. */
    tag = tmp->count;
    if (events_execute_object_event(tmp, EVENT_PICKUP, pl, op, NULL, SCRIPT_FIX_ALL) != 0) {
        /* put item back, if it still exists */
        if (tmp->count == tag && !QUERY_FLAG(tmp, FLAG_FREED)) {
          if (env != NULL) {
            object_insert_in_ob(tmp, env);
          } else {
            object_insert_in_map_at(tmp, map, NULL, 0, x, y);
          }
        }
        return;
    }

    draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                  buf);

    object_insert_in_ob(tmp, op);

    /* All the stuff below deals with client/server code, and is only
     * usable by players
     */
    if (pl->type != PLAYER)
        return;

    /* Additional weight changes speed, etc */
    fix_object(pl);

    /* These are needed to update the weight for the container we
     * are putting the object in.
     */
    if (op != pl) {
        esrv_update_item(UPD_WEIGHT, pl, op);
        esrv_update_item(UPD_WEIGHT, pl, pl);
    }

    /* Update the container the object was in */
    if (env && env != pl && env != op)
        esrv_update_item(UPD_WEIGHT, pl, env);
}

/**
 * Try to pick up an item.
 *
 * @param op
 * object trying to pick up.
 * @param alt
 * optional object op is trying to pick. If NULL, try to pick first item under op.
 */
void pick_up(object *op, object *alt) {
/* modified slightly to allow monsters use this -b.t. 5-31-95 */
    object *tmp = NULL, *tmp1;
    mapstruct *tmp_map = NULL;
    int count;

    /* Decide which object to pick. */
    if (alt) {
        if (!object_can_pick(op, alt)) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                                 "You can't pick up the %s.",
                                 alt->name);
            return;
        }
        tmp = alt;
    } else {
        if (op->below == NULL || !object_can_pick(op, op->below)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "There is nothing to pick up here.");
            return;
        }
        tmp = op->below;
    }

    /* it is possible that the object is a thrown object and is flying about.
     * in that case, what we want to pick up is the payload.  Objects
     * that are thrown are encapsulated into a thrown object.
     * stop_item() returns the payload (unlinked from map) and gets rid of the
     * container object.  If this object isn't picked up, we need to insert
     * it back on the map.
     * A bug here is that even attempting to pick up one of these objects will
     * result in this logic being called even if player is unable to pick it
     * up.
     */

    tmp_map = tmp->map;
    tmp1 = stop_item(tmp);
    if (tmp1 == NULL)
        return;

    /* If it is a thrown object, insert it back into the map here.
     * makes life easier further along.  Do no merge so pick up code
     * behaves more sanely.
     */
    if (tmp1 != tmp) {
        tmp = object_insert_in_map(tmp1, tmp_map, op, INS_NO_MERGE);
    }

    if (tmp == NULL)
        return;

    if (!object_can_pick(op, tmp))
        return;

    /* Establish how many of the object we are picking up */
    if (op->type == PLAYER) {
        count = op->contr->count;
        if (count == 0)
            count = tmp->nrof;
    } else
        count = tmp->nrof;

    /* container is open, so use it */
    if (op->container) {
        alt = op->container;
        if (alt != tmp->env && !sack_can_hold(op, alt, tmp, count))
            return;
    } else {
        /* non container pickup.  See if player has any
         * active containers.
         */
        object *container = NULL;

        /* Look for any active containers that can hold this item.
         * we cover two cases here - the perfect match case, where we
         * break out of the loop, and the general case (have a container),
         * Moved this into a single loop - reduces redundant code, is
         * more efficient and easier to follow.  MSW 2009-04-06
         */
        alt = op->inv;
        FOR_OB_AND_BELOW_PREPARE(alt) {
            if (alt->type == CONTAINER
            && QUERY_FLAG(alt, FLAG_APPLIED)
            && sack_can_hold(NULL, alt, tmp, count)) {
                if (alt->race && alt->race == tmp->race) {
                    break;  /* perfect match */
                } else if (!container) {
                    container = alt;
                }
            }
        } FOR_OB_AND_BELOW_FINISH();
        /* Note container could be null, but no reason to check for it */
        if (!alt)
            alt = container;

        if (!alt)
            alt = op; /* No free containers */
    }
    /* see if this object is already in this container.  If so,
     * move it to player inventory from this container.
     */
    if (tmp->env == alt) {
        alt = op;
    }

    /* Don't allow players to be put into containers.  Instead,
     * just put them in the players inventory.
     */
    if (tmp->type == CONTAINER && alt->type==CONTAINER) {
        alt = op;
    }
#ifdef PICKUP_DEBUG
    LOG(llevDebug, "Pick_up(): %s picks %s (%d) and inserts it %s.\n", op->name, tmp->name, op->contr->count, alt->name);
#endif

    /* startequip items are not allowed to be put into containers
     * Not sure why we have this limitation
     */
    if (op->type == PLAYER
    && alt->type == CONTAINER
    && QUERY_FLAG(tmp, FLAG_STARTEQUIP)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "This object cannot be put into containers!");
        return;
    }

    pick_up_object(op, alt, tmp, count);
    if (op->type == PLAYER)
        op->contr->count = 0;
}

/**
 * Checks if an item matches a specific pickup mode.
 * @param item item to check.
 * @param mode one value of @ref PU_xxx, not a combination of modes.
 * @return 0 if the item doesn't match, a non-zero value if it matches.
 */
int object_matches_pickup_mode(const object *item, int mode) {
    switch(mode) {
        case PU_FOOD:
            return item->type == FOOD;
        case PU_DRINK:
            return item->type == DRINK || (item->type == POISON && !QUERY_FLAG(item, FLAG_KNOWN_CURSED));
        case PU_FLESH:
            return item->type == FLESH;
        case PU_POTION:
            return item->type == POTION;
        case PU_SPELLBOOK:
            return item->type == SPELLBOOK;
        case PU_SKILLSCROLL:
            return item->type == SKILLSCROLL;
        case PU_READABLES:
            return item->type == BOOK || item->type == SCROLL;
        case PU_MAGIC_DEVICE:
            return item->type == WAND || item->type == ROD || item->type == WEAPON_IMPROVER || item->type == ARMOUR_IMPROVER || item->type == SKILL_TOOL;
        case PU_MAGICAL:
            return QUERY_FLAG(item, FLAG_KNOWN_MAGICAL) && !QUERY_FLAG(item, FLAG_KNOWN_CURSED);
        case PU_VALUABLES:
            return item->type == MONEY || item->type == GEM;
        case PU_JEWELS:
            return item->type == RING || item->type == AMULET;
        case PU_BOW:
            return item->type == BOW;
        case PU_ARROW:
            return item->type == ARROW;
        case PU_ARMOUR:
            return item->type == ARMOUR;
        case PU_HELMET:
            return item->type == HELMET;
        case PU_SHIELD:
            return item->type == SHIELD;
        case PU_BOOTS:
            return item->type == BOOTS;
        case PU_GLOVES:
            return item->type == GLOVES;
        case PU_CLOAK:
            return item->type == CLOAK;
        case PU_MISSILEWEAPON:
            return item->type == WEAPON && QUERY_FLAG(item, FLAG_IS_THROWN);
        case PU_MELEEWEAPON:
            return item->type == WEAPON;
        case PU_KEY:
            return item->type == KEY || item->type == SPECIAL_KEY;
        case PU_CONTAINER:
            return item->type == CONTAINER;
        case PU_CURSED:
            return QUERY_FLAG(item, FLAG_KNOWN_CURSED);
    }
    return 0;
}

/** Matching parameters. */
typedef struct {
    union {
        struct {
            int item_to_pick;   /**< Index of the item to pick, 1-based. */
            int item_number;    /**< Index of the checked item, 1-based. */
            int item_must_be_pickable;  /**< If non zero, then the item number is increased only if the item is pickable. */
        };
        char name[100];         /**< Name to match for. */
        int pickup_type;        /**< Value in @ref PU_xxx to match against. */
    };
    int missed;                 /**< How many items were missed when matching. */
} matcher_params;

/**
 * Prototype for a function checking if an object matches some parameters.
 * @param who object wanting to match another object.
 * @param params what to match against.
 * @param item object to check for a match.
 * @return 0 if the object does not match, 1 if matches.
 */
typedef int (*item_matcher)(object *who, matcher_params *params, object *item);

/**
 * Function allowing all objects.
 * @param who unused.
 * @param params unused.
 * @param item unused.
 * @return 1.
 */
static int matcher_all(object *who, matcher_params *params, object *item) {
    (void)who;
    (void)params;
    (void)item;
    return 1;
}

/**
 * Check if an item is the one at the desired position.
 * @param who unused.
 * @param params what to match against.
 * @param item unused.
 * @return 0 if the object does not match, 1 if matches.
 */
static int matcher_number(object *who, matcher_params *params, object *item) {
    if (params->item_must_be_pickable == 0 || object_can_pick(who, item)) {
        params->item_number++;
    }
    if (params->item_to_pick == params->item_number) {
        /* Since we don't always increase item_number, multiple items may have the same index,
         including unpickable ones, so to avoid failure messages just ensure no other item can be picked. */
        params->item_to_pick = 0;
        return 1;
    }
    return 0;
}

/**
 * Check if an item matches a string.
 * @param who object wanting to match another object.
 * @param params what to match against.
 * @param item object to check for a match.
 * @return 0 if the object does not match, 1 if matches.
 */
static int matcher_name(object *who, matcher_params *params, object *item) {
    int ival = object_matches_string(who, item, params->name);
    if (ival > 0) {
        if (ival <= 2 && !object_can_pick(who, item)) {
            if (!QUERY_FLAG(item, FLAG_IS_FLOOR))/* don't count floor tiles */
                params->missed++;
            return 0;
        }
        return 1;
    }
    return 0;
}

/**
 * Check if an item matches a pickup type.
 * @param who object wanting to match another object.
 * @param params what to match against.
 * @param item object to check for a match.
 * @return 0 if the object does not match, 1 if matches.
 */
static int matcher_pickup_type(object *who, matcher_params *params, object *item) {
    (void)who;
    return object_matches_pickup_mode(item, params->pickup_type);
}

/**
 * Parse parameters and sets up an item matcher based on them.
 * @param who object wanting to match another object.
 * @param params matching parameters.
 * @param[out] mp parsed parameters.
 * @return NULL if error in parameters, else an item matcher pointer.
 */
static item_matcher make_matcher(object *who, const char *params, matcher_params *mp) {
    memset(mp, 0, sizeof(*mp));
    if (params[0] == '\0') {
        mp->item_to_pick = 1;
        mp->item_must_be_pickable = 1;
        return &matcher_number;
    }
    if (params[0] == '#') {
        mp->item_to_pick = atoi(params + 1);
        if (mp->item_to_pick == 0) {
            draw_ext_info_format(NDI_UNIQUE, 0, who, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR, "Invalid item index");
            return NULL;
        }
        return &matcher_number;
    }
    if (params[0] == '*') {
        if (params[1] == '\0') {
            return &matcher_all;
        }
        int idx = get_pickup_mode_index(params + 1);
        if (idx == -1) {
            draw_ext_info_format(NDI_UNIQUE, 0, who, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR, "Invalid items %s", params + 1);
            return NULL;
        }
        mp->pickup_type = pickup_modes[idx];
        return &matcher_pickup_type;
    }
    strncpy(mp->name, params, sizeof(mp->name) - 1);
    return &matcher_name;
}

/**
 * This takes (picks up) an item.
 *
 * @param op
 * player who issued the command.
 * @param params
 * string to match against the item name.
 */
void command_take(object *op, const char *params) {
    object *tmp;
    int did_one = 0;

    if (op->container)
        tmp = op->container->inv;
    else {
        tmp = op->above;
        if (tmp)
            while (tmp->above) {
                tmp = tmp->above;
            }
        if (!tmp)
            tmp = op->below;
    }

    if (tmp == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Nothing to take!");
        return;
    }

    matcher_params mp;
    item_matcher matcher = make_matcher(op, params, &mp);
    if (!matcher) {
        return; // Player already informed of failure.
    }

    SET_FLAG(op, FLAG_NO_FIX_PLAYER);

    FOR_OB_AND_BELOW_PREPARE(tmp) {
        if (tmp->invisible) {
            continue;
        }
        if ((*matcher)(op, &mp, tmp)) {
            pick_up(op, tmp);
            did_one = 1;
        }
    } FOR_OB_AND_BELOW_FINISH();

    /* Nothing picked up, check if unable to pick or nothing to pick. */
    if (params[0] == '\0' && !did_one) {
        int found = 0;
        FOR_BELOW_PREPARE(op, tmp)
            if (!tmp->invisible) {
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                                     "You can't pick up a %s.",
                                     tmp->name ? tmp->name : "null");
                found = 1;
                break;
            }
        FOR_BELOW_FINISH();
        if (!found)
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "There is nothing to pick up.");
    }

    if (mp.missed == 1)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "You were unable to take one of the items.");
    else if (mp.missed > 1)
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
                             "You were unable to take %d of the items.",
                             mp.missed);

    /* Now update player and send information. */
    CLEAR_FLAG(op, FLAG_NO_FIX_PLAYER);
    fix_object(op);
    if (op->type == PLAYER) {
        op->contr->count = 0;
        esrv_update_item(UPD_WEIGHT, op, op);
    }
}

/**
 * Something tries to put an object into another.
 *
 * This function was part of drop(), now is own function.
 *
 * @note
 * the 'sack' in question can now be a transport,
 * so this function isn't named very good anymore.
 *
 * @param op
 * who is moving the item.
 * @param sack
 * where to put the object.
 * @param tmp
 * what to put into sack.
 * @param nrof
 * if non zero, then nrof objects is tried to put into sack, else everything is put.
 */
void put_object_in_sack(object *op, object *sack, object *tmp, uint32_t nrof) {
    object *sack2, *orig = sack;
    char name_sack[MAX_BUF], name_tmp[MAX_BUF];

    if (sack == tmp)
        return; /* Can't put an object in itself */
    query_name(sack, name_sack, MAX_BUF);
    if (sack->type != CONTAINER && sack->type != TRANSPORT) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "The %s is not a container.",
                             name_sack);
        return;
    }
    if (QUERY_FLAG(tmp, FLAG_STARTEQUIP)) {
        query_name(tmp, name_tmp, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "You cannot put the %s in the %s.",
                             name_tmp, name_sack);
        return;
    }
    if (tmp->type == CONTAINER) {
        if (tmp->inv) {
            if (tmp->slaying)
                return;
            /* Eneq(@csd.uu.se): If the object to be dropped is a container
             * and does not require a key to be opened,
             * we instead move the contents of that container into the active
             * container, this is only done if the object has something in it.
             * If object is container but need a key, just don't do anything
             */
            sack2 = tmp;
            query_name(tmp, name_tmp, MAX_BUF);
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                             "You move the items from %s into %s.",
                             name_tmp, name_sack);

            FOR_INV_PREPARE(tmp, tmp2) {
                if ((sack->type == CONTAINER && sack_can_hold(op, op->container, tmp2, tmp2->nrof))
                   || (sack->type == TRANSPORT && transport_can_hold(sack, tmp2, tmp2->nrof))) {
                    put_object_in_sack(op, sack, tmp2, 0);
                } else {
                    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND,
                                      MSG_TYPE_COMMAND_FAILURE,
                                     "Your %s fills up.",
                                     name_sack);
                    break;
                }
            } FOR_INV_FINISH();
            esrv_update_item(UPD_WEIGHT, op, sack2);
            return;
        } else {
            query_name(tmp, name_tmp, MAX_BUF);
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                                 "You can not put a %s into a %s",
                                 name_tmp,
                                 name_sack);
            return;
        }
    }

    /* Don't worry about this for containers - our caller should have
     * already checked this.
     */
    if (sack->type == CONTAINER && !sack_can_hold(op, sack, tmp, nrof ? nrof : tmp->nrof))
        return;

    if (QUERY_FLAG(tmp, FLAG_APPLIED)) {
        if (apply_special(op, tmp, AP_UNAPPLY|AP_NO_MERGE))
            return;
    }

    /* we want to put some portion of the item into the container */
    if (nrof && tmp->nrof != nrof) {
        char failure[MAX_BUF];

        tmp = object_split(tmp, nrof, failure, sizeof(failure));

        if (!tmp) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          failure);
            return;
        }
    } else
        object_remove(tmp);

    if (sack->nrof > 1) {
        orig = object_split(sack, sack->nrof-1, NULL, 0);
        set_object_face_main(orig);
        CLEAR_FLAG(orig, FLAG_APPLIED);
        if (sack->env) {
            object_insert_in_ob(orig, sack->env);
        } else {
            object_insert_in_map_at(orig, sack->map, NULL, 0, sack->x, sack->y);
            orig->move_off = 0;
        }
    }

    query_name(tmp, name_tmp, MAX_BUF);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                         "You put the %s in %s.",
                         name_tmp, name_sack);

    object_insert_in_ob(tmp, sack);
    if (!QUERY_FLAG(op, FLAG_NO_FIX_PLAYER))
        fix_object(op); /* This is overkill, fix_player() is called somewhere */
    /* in object.c */

    /* If a transport, need to update all the players in the transport
     * the view of what is in it.
     */
    if (sack->type == TRANSPORT) {
        FOR_INV_PREPARE(sack, tmp)
            if (tmp->type == PLAYER)
                tmp->contr->socket.update_look = 1;
        FOR_INV_FINISH();
    } else {
        /* update the sacks weight */
        esrv_update_item(UPD_WEIGHT, op, sack);
    }
}

/**
 * Try to drop an object on the floor.
 *
 * This function was part of drop, now is own function.
 *
 * @param op
 * who is dropping the item.
 * @param tmp
 * item to drop.
 * @param nrof
 * if is non zero, then nrof objects is tried to be dropped.
 * @return
 * object dropped, NULL if it was destroyed.
 * @todo shouldn't tmp be NULL if object_was_destroyed returns true?
 */
object *drop_object(object *op, object *tmp, uint32_t nrof) {
    tag_t tmp_tag;

    if (QUERY_FLAG(tmp, FLAG_NO_DROP)) {
        return NULL;
    }

    if (QUERY_FLAG(tmp, FLAG_APPLIED)) {
        if (apply_special(op, tmp, AP_UNAPPLY|AP_NO_MERGE))
            return NULL;  /* can't unapply it */
    }

    if (events_execute_object_event(tmp, EVENT_DROP, op, NULL, NULL, SCRIPT_FIX_ALL) != 0)
        return NULL;

    /* ensure the plugin didn't destroy the object */
    if (QUERY_FLAG(tmp, FLAG_REMOVED))
        return NULL;

    /* We are only dropping some of the items.  We split the current objec
     * off
     */
    if (nrof && tmp->nrof != nrof) {
        char failure[MAX_BUF];

        tmp = object_split(tmp, nrof, failure, sizeof(failure));
        if (!tmp) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          failure);
            return NULL;
        }
    } else
        object_remove(tmp);

    if (QUERY_FLAG(tmp, FLAG_STARTEQUIP)) {
        char name[MAX_BUF];

        query_name(tmp, name, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                             "You drop the %s. The gods who lent it to you retrieve it.",
                             name);
        object_free_drop_inventory(tmp);

        if (!QUERY_FLAG(op, FLAG_NO_FIX_PLAYER))
            fix_object(op);

        return NULL;
    }

    /*  If SAVE_INTERVAL is commented out, we never want to save
     *  the player here.
     */
#ifdef SAVE_INTERVAL
    /* I'm not sure why there is a value check - since the save
     * is done every SAVE_INTERVAL seconds, why care the value
     * of what he is dropping?
     */
    if (op->type == PLAYER
    && !QUERY_FLAG(tmp, FLAG_UNPAID)
    && (tmp->nrof ? tmp->value*tmp->nrof : tmp->value > 2000)
    && op->contr->last_save_time+SAVE_INTERVAL <= time(NULL)) {
        save_player(op, 1);
        op->contr->last_save_time = time(NULL);
    }
#endif /* SAVE_INTERVAL */


    tmp_tag = tmp->count;
    object_insert_in_map_at(tmp, op->map, op, INS_BELOW_ORIGINATOR, op->x, op->y);
    if (!object_was_destroyed(tmp, tmp_tag) && !QUERY_FLAG(tmp, FLAG_UNPAID) && tmp->type != MONEY && shop_contains(op)) {
        sell_item(tmp, op);
    }

    /* Call this before we update the various windows/players.  At least
     * that we, we know the weight is correct.
     */
    if (!QUERY_FLAG(op, FLAG_NO_FIX_PLAYER)) {
        fix_object(op); /* This is overkill, fix_player() is called somewhere */
        /* in object.c */

        /* Need to update weight of player */
        if (op->type == PLAYER)
            esrv_update_item(UPD_WEIGHT, op, op);
    }
    return tmp;
}

/**
 * Drop an item, either on the floor or in a container.
 *
 * @param op
 * who is dropping an item.
 * @param tmp
 * what object to drop.
 */
void drop(object *op, object *tmp) {
    /* Hopeful fix for disappearing objects when dropping from a container -
     * somehow, players get an invisible object in the container, and the
     * old logic would skip over invisible objects - works fine for the
     * playes inventory, but drop inventory wants to use the next value.
     */
    if (tmp->invisible) {
        /* if the following is the case, it must be in an container. */
        if (tmp->env && tmp->env->type != PLAYER) {
            /* Just toss the object - probably shouldn't be hanging
             * around anyways
             */
            object_remove(tmp);
            object_free_drop_inventory(tmp);
            return;
        } else {
            FOR_OB_AND_BELOW_PREPARE(tmp)
                if (!tmp->invisible)
                    break;
            FOR_OB_AND_BELOW_FINISH();
        }
    }

    if (tmp == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "You don't have anything to drop.");
        return;
    }
    if (QUERY_FLAG(tmp, FLAG_INV_LOCKED)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "This item is locked");
        return;
    }
    if (QUERY_FLAG(tmp, FLAG_NO_DROP)) {
        return;
    }

    if (op->container) {
        if (op->type == PLAYER) {
            put_object_in_sack(op, op->container, tmp, op->contr->count);
        } else {
            put_object_in_sack(op, op->container, tmp, 0);
        };
    } else {
        if (op->type == PLAYER) {
            drop_object(op, tmp, op->contr->count);
        } else {
            drop_object(op, tmp, 0);
        };
    }
    if (op->type == PLAYER)
        op->contr->count = 0;
}

/**
 * Command to drop all items that have not been locked.
 *
 * @param op
 * player.
 * @param params
 * optional specifier, like 'armour', 'weapon' and such.
 */
void command_dropall(object *op, const char *params) {
    int count = 0;

    if (op->inv == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op,
                      MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Nothing to drop!");
        return;
    }

    if (op->contr)
        count = op->contr->count;

    /* Set this so we don't call it for _every_ object that
     * is dropped.
     */
    SET_FLAG(op, FLAG_NO_FIX_PLAYER);

    /*
     * This is the default.  Drops everything not locked or considered
     * not something that should be dropped.
     * Care must be taken that the next item pointer is not to money as
     * the drop() routine will do unknown things to it when dropping
     * in a shop. --Tero.Pelander@utu.fi
     */
    if (*params == '\0') {
        FOR_INV_PREPARE(op, curinv) {
            if (!QUERY_FLAG(curinv, FLAG_INV_LOCKED)
            && curinv->type != MONEY
            && curinv->type != FOOD
            && curinv->type != KEY
            && curinv->type != SPECIAL_KEY
            && curinv->type != GEM
            && !curinv->invisible
            && (curinv->type != CONTAINER || op->container != curinv)) {
                drop(op, curinv);
                if (op->contr)
                    op->contr->count = count;
            }
        } FOR_INV_FINISH();
    } else if (strcmp(params, "weapons") == 0) {
        FOR_INV_PREPARE(op, curinv) {
            if (!QUERY_FLAG(curinv, FLAG_INV_LOCKED)
            && (curinv->type == WEAPON || curinv->type == BOW || curinv->type == ARROW)) {
                drop(op, curinv);
                if (op->contr)
                    op->contr->count = count;
            }
        } FOR_INV_FINISH();
    } else if (strcmp(params, "armor") == 0 || strcmp(params, "armour") == 0) {
        FOR_INV_PREPARE(op, curinv) {
            if (!QUERY_FLAG(curinv, FLAG_INV_LOCKED)
            && (curinv->type == ARMOUR || curinv->type == SHIELD || curinv->type == HELMET)) {
                drop(op, curinv);
                if (op->contr)
                    op->contr->count = count;
            }
        } FOR_INV_FINISH();
    } else if (strcmp(params, "food") == 0) {
        FOR_INV_PREPARE(op, curinv) {
            if (!QUERY_FLAG(curinv, FLAG_INV_LOCKED)
            && (curinv->type == FOOD || curinv->type == DRINK)) {
                drop(op, curinv);
                if (op->contr)
                    op->contr->count = count;
            }
        } FOR_INV_FINISH();
    } else if (strcmp(params, "flesh") == 0) {
        FOR_INV_PREPARE(op, curinv) {
            if (!QUERY_FLAG(curinv, FLAG_INV_LOCKED)
            && (curinv->type == FLESH)) {
                drop(op, curinv);
                if (op->contr)
                    op->contr->count = count;
            }
        } FOR_INV_FINISH();
    } else if (strcmp(params, "misc") == 0) {
        FOR_INV_PREPARE(op, curinv) {
            if (!QUERY_FLAG(curinv, FLAG_INV_LOCKED)
            && !QUERY_FLAG(curinv, FLAG_APPLIED)) {
                switch (curinv->type) {
                case BOOK:
                case SPELLBOOK:
                case GIRDLE:
                case AMULET:
                case RING:
                case CLOAK:
                case BOOTS:
                case GLOVES:
                case BRACERS:
                case SCROLL:
                case ARMOUR_IMPROVER:
                case WEAPON_IMPROVER:
                case WAND:
                case ROD:
                case POTION:
                    drop(op, curinv);
                    if (op->contr)
                        op->contr->count = count;
                    break;

                default:
                    break;
                }
            }
        } FOR_INV_FINISH();
    }
    op->contr->socket.update_look = 1;
    CLEAR_FLAG(op, FLAG_NO_FIX_PLAYER);
    /* call it now, once */
    fix_object(op);
    /* Need to update weight of player.  Likewise, only do it once */
    if (op->type == PLAYER)
        esrv_update_item(UPD_WEIGHT, op, op);
}

/**
 * 'drop' command.
 *
 * @param op
 * player.
 * @param params
 * what to drop.
 */
void command_drop(object *op, const char *params) {
    int did_one = 0;
    int missed = 0;

    if (*params == '\0') {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Drop what?");
        return;
    }

    matcher_params mp;
    item_matcher matcher = make_matcher(op, params, &mp);
    if (!matcher) {
        return;
    }

    SET_FLAG(op, FLAG_NO_FIX_PLAYER);

    FOR_INV_PREPARE(op, tmp) {
        if (QUERY_FLAG(tmp, FLAG_NO_DROP) || tmp->invisible)
            continue;
        if ((*matcher)(op, &mp, tmp)) {
            if (QUERY_FLAG(tmp, FLAG_INV_LOCKED)) {
                missed++;
            } else {
                drop(op, tmp);
            }
            did_one = 1;
        }
    } FOR_INV_FINISH();
    if (!did_one)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Nothing to drop.");
    if (missed == 1)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "One item couldn't be dropped because it was locked.");
    else if (missed > 1)
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "%d items couldn't be dropped because they were locked.",
                             missed);

    /* Now update player and send information. */
    CLEAR_FLAG(op, FLAG_NO_FIX_PLAYER);
    fix_object(op);
    if (op->type == PLAYER) {
        op->contr->count = 0;
        esrv_update_item(UPD_WEIGHT, op, op);
    }
}

/**
 * Put all contents of the container on the ground below the player or in opened container, except locked items.
 *
 * @param container
 * what to empty.
 * @param pl
 * player to drop for.
 */
static void empty_container(object *container, object *pl) {
    int left = 0;
    char name[MAX_BUF];

    if (!container->inv)
        return;

    FOR_INV_PREPARE(container, inv) {
        object *next;

        if (QUERY_FLAG(inv, FLAG_INV_LOCKED)) {
            /* you can have locked items in container. */
            left++;
            continue;
        }
        next = inv->below;
        drop(pl, inv);
        if (inv->below == next)
            /* item couldn't be dropped for some reason. */
            left++;
    } FOR_INV_FINISH();
    esrv_update_item(UPD_WEIGHT, pl, container);

    query_name(container, name, sizeof(name));
    if (left)
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS, "You empty the %s except %d items.", name, left);
    else
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS, "You empty the %s.", name);
}

/**
 * 'empty' command.
 *
 * @param op
 * player.
 * @param params
 * item specifier.
 */
void command_empty(object *op, const char *params) {
    object *container;

    if (*params == '\0') {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Empty what?");
        return;
    }

    if (strcmp(params, "all") == 0) {
        FOR_INV_PREPARE(op, inv)
            if (inv->type == CONTAINER)
                empty_container(inv, op);
        FOR_INV_FINISH();
        return;
    }

    container = find_best_object_match(op, params);
    if (!container) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "No such item.");
        return;
    }
    if (container->type != CONTAINER) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "This is not a container!");
        return;
    }
    empty_container(container, op);
}

/**
 * 'examine' command.
 *
 * @param op
 * player.
 * @param params
 * optional item specifier.
 */
void command_examine(object *op, const char *params) {
    if (*params == '\0') {
        FOR_BELOW_PREPARE(op, tmp)
            if (LOOK_OBJ(tmp)) {
                examine(op, tmp);
                break;
            }
        FOR_BELOW_FINISH();
    } else {
        object *tmp = find_best_object_match(op, params);

        if (tmp)
            examine(op, tmp);
        else
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                                 "Could not find an object that matches %s",
                                 params);
    }
}

/**
 * Return the object the player has marked with the 'mark' command
 * below.  If no match is found (or object has changed), we return
 * NULL.  We leave it up to the calling function to print messages if
 * nothing is found.
 *
 * @param op
 * object. Should be a player.
 * @return
 * marked object if still valid, NULL else.
 */
object *find_marked_object(object *op) {
    if (!op || !op->contr || !op->contr->mark)
        return NULL;

    /* This may seem like overkill, but we need to make sure that they
     * player hasn't dropped the item.  We use count on the off chance that
     * an item got reincarnated at some point.
     */
     /*
    FOR_INV_PREPARE(op, tmp) {
        if (tmp->invisible)
            continue;
        if (tmp == op->contr->mark) {
            if (tmp->count == op->contr->mark_count)
                return tmp;
            else {
                op->contr->mark = NULL;
                op->contr->mark_count = 0;
                return NULL;
            }
        }
    } FOR_INV_FINISH();
    */
    /* Try a different way of doing this
     * We check the environment of the marked object
     * to make sure it is still in the player's inventory.
     * In addition, we ensure there is the correct tag for that item.
     *
     * Daniel Hawkins 2018-10-23
     */
    if (op->contr->mark->env == op && op->contr->mark->count == op->contr->mark_count)
        return op->contr->mark;
    // Otherwise reset the mark, since it is no longer valid.
    op->contr->mark = NULL;
    op->contr->mark_count = 0;
    return NULL;
}

/**
 * 'mark' command, to mark an item for some effects (enchant armor, ...).
 *
 * @param op
 * player.
 * @param params
 * If empty, we print out the currently marked object.
 * Otherwise, try to find a matching object - try best match first.
 */
void command_mark(object *op, const char *params) {
    char name[MAX_BUF];

    if (!op->contr)
        return;
    if (*params == '\0') {
        object *mark = find_marked_object(op);
        if (!mark)
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "You have no marked object.");
        else {
            query_name(mark, name, MAX_BUF);
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                                 "%s is marked.",
                                 name);
        }
    } else {
        object *mark1 = find_best_object_match(op, params);

        if (!mark1) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                                 "Could not find an object that matches %s",
                                 params);
            return;
        } else {
            op->contr->mark = mark1;
            op->contr->mark_count = mark1->count;
            query_name(mark1, name, MAX_BUF);
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                                 "Marked item %s",
                                 name);
            return;
        }
    }
    /*shouldnt get here */
}

/**
 * Player examine a monster.
 *
 * @param op
 * player.
 * @param tmp
 * monster being examined.
 * @param level
 * level of the probe, to have a persistant marker for some duration.
 */
void examine_monster(object *op, object *tmp, int level) {
    object *mon = HEAD(tmp), *probe;

    if (QUERY_FLAG(mon, FLAG_UNDEAD))
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "It is an undead force.");
    if (mon->level > op->level)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "It is likely more powerful than you.");
    else if (mon->level < op->level)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "It is likely less powerful than you.");
    else
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "It is probably as powerful as you.");

    if (mon->attacktype&AT_ACID)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "You smell an acrid odor.");

    /* Anyone know why this used to use the clone value instead of the
     * maxhp field?  This seems that it should give more accurate results.
     */
    switch ((mon->stats.hp+1)*4/(mon->stats.maxhp+1)) {
        /* From 0-4+, since hp can be higher than maxhp if defined in map. */
    case 0:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "It is critically wounded.");
        break;

    case 1:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "It is in a bad shape.");
        break;

    case 2:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "It is hurt.");
        break;

    case 3:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "It is somewhat hurt.");
        break;

    default:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "It is in excellent shape.");
        break;
    }
    if (object_present_in_ob(POISONING, mon) != NULL)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "It looks very ill.");

    if (level < 10)
        return;
    knowledge_show_monster_detail(op, mon->arch->name);

    if (level < 15)
        return;

    probe = object_find_by_type_and_name(mon, FORCE, "probe_force");
    if (probe != NULL && probe->level > level)
        return;

    if (probe == NULL) {
        probe = create_archetype(FORCE_NAME);
        free_string(probe->name);
        probe->name = add_string("probe_force");
        SET_FLAG(probe, FLAG_APPLIED);
        SET_FLAG(probe, FLAG_PROBE);
        object_insert_in_ob(probe, mon);
        fix_object(mon);
    }
    probe->level = level;
    if (level / 10 > probe->duration)
        probe->duration = level / 10;
}

/**
 * Player examines some object. The item may be identified automatically
 * if the player has the correct skill for that.
 *
 * @param op
 * player.
 * @param tmp
 * object to examine.
 */
void examine(object *op, object *tmp) {
    char buf[VERY_BIG_BUF];
    int in_shop;
    int i, exp = 0, conn;

    /* we use this to track how far along we got with trying to identify an item,
     * so that we can give the appropriate message to the player */
    int id_attempted = 0;
    char prefix[MAX_BUF] = "That is";
    const typedata *tmptype;
    object *skill;

    buf[0] = '\0';

    if (tmp == NULL || tmp->type == CLOSE_CON)
        return;
    tmptype = get_typedata(tmp->type);
    if (!tmptype) {
        LOG(llevError, "Attempted to examine item %d with type %d, which is invalid", tmp->count, tmp->type);
        return;
    }
    /* first of all check whether this is an item we need to identify, and identify it as best we can.*/
    if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED)) {
        /* We will look for magic status, cursed status and then try to do a full skill-based ID, in that order */
        skill = find_skill_by_number(op, SK_DET_MAGIC);
        if (skill && (object_can_pick(op, tmp))) {
            exp = detect_magic_on_item(op, tmp, skill);
            if (exp) {
                change_exp(op, exp, skill->skill, SK_SUBTRACT_SKILL_EXP);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                        "You discover mystic forces on %s", tmp->nrof <= 1?"that item":"those items" );
            }
        }
        skill = find_skill_by_number(op, SK_DET_CURSE);
        /* Cauldrons are a special case of item where it should be possible to detect a curse */
        if (skill && (object_can_pick(op, tmp) || QUERY_FLAG(tmp, FLAG_IS_CAULDRON))) {
            exp = detect_curse_on_item(op, tmp, skill);
            if (exp) {
                change_exp(op, exp, skill->skill, SK_SUBTRACT_SKILL_EXP);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                        "You have a bad feeling about %s", tmp->nrof <= 1?"that item":"those items" );
            }
        }
        if (!QUERY_FLAG(tmp, FLAG_NO_SKILL_IDENT)) {

            id_attempted = 1;
            skill = find_skill_by_number(op, tmptype->identifyskill);
            if (skill) {
                id_attempted = 2;

                /* identify_object_with_skill() may merge tmp with another
                 * object, so once that happens, we really can not do
                 * any further processing with tmp.  It would be possible
                 * to modify identify_object_with_skill() to return
                 * the merged object, but it is currently set to return
                 * exp, so it would have to do it via modifying the
                 * passed in value, but many other consumers would
                 * need to be modified for that.
                 */
                exp = identify_object_with_skill(tmp, op, skill, 1);
                if (exp) {
                    change_exp(op, exp, skill->skill, SK_SUBTRACT_SKILL_EXP);
                    return;
                }
            }
            if(!exp) {
                /* The primary id skill didn't work, let's try the secondary one */
                skill = find_skill_by_number(op, tmptype->identifyskill2);
                if (skill) {
                    /* if we've reached here, then the first skill will have been attempted
                     * and failed; this will have set FLAG_NO_SKILL_IDENT we want to clear
                     * that now, and try with the secondary ID skill, if it fails, then the
                     * flag will be reset anyway, if it succeeds, it won't matter.*/
                    CLEAR_FLAG(tmp, FLAG_NO_SKILL_IDENT);
                    id_attempted = 2;
                    exp = identify_object_with_skill(tmp, op, skill, 1);
                    if (exp) {
                        change_exp(op, exp, skill->skill, SK_SUBTRACT_SKILL_EXP);
                        return;
                    }
                }
            }
        }
    }
    if (!exp) {
        /* if we did get exp we'll already have propulated prefix */
        if (tmptype->identifyskill || tmptype->identifyskill2) {
            switch (id_attempted) {
                case 1:
                    snprintf(prefix, MAX_BUF, "You lack the skill to understand %s:",  tmp->nrof <= 1?"that fully; it is":"those fully, they are");
                    break;
                case 2:
                    snprintf(prefix, MAX_BUF, "You fail to understand %s:",  tmp->nrof <= 1?"that fully; it is":"those fully, they are");
                    break;
                default:
                    snprintf(prefix, MAX_BUF, "%s:",  tmp->nrof <= 1?"That is":"Those are");
            }
        } else snprintf(prefix, MAX_BUF, "%s:",  tmp->nrof <= 1?"That is":"Those are");
    }
    /* now we need to get the rest of the object description */
    ob_describe(tmp, op, 1, buf, sizeof(buf));

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                                            "%s %s", prefix, buf);
    buf[0] = '\0';
    if (tmp->custom_name) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                             "You name it %s",
                             tmp->custom_name);
    }

    switch (tmp->type) {
    case SKILLSCROLL:
    case SKILL_TOOL:
        // Embedded skills are stored as an archetype name and don't get reified
        // until the player actually reads/equips the object, so we need to
        // handle it differently from spells, which are stored in the inv.
        if (!tmp->skill) break;  // Blank skill scroll, somehow.
        archetype *skill = get_archetype_by_skill_name(tmp->skill, SKILL);
        if (!skill) {
            // Skill name doesn't correspond to any actual skill.
            draw_ext_info(NDI_UNIQUE|NDI_BLUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_INFO,
                "Unfortunately the scroll is damaged and unreadable.");
            break;
        }
        // Only print the flavor text once we have identified.
        if (is_identified(tmp) && skill->clone.msg) {
            StringBuffer *sb = stringbuffer_new();
            stringbuffer_append_string(sb, skill->clone.msg);
            stringbuffer_trim_whitespace(sb);
            char *const fluff = stringbuffer_finish(sb);
            // SPELL_INFO is not a perfect match here, but it should display in the
            // same manner as the spell descriptions below and there's no SKILL_INFO
            // message type.
            draw_ext_info(NDI_UNIQUE|NDI_BLUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_INFO, fluff);
            free(fluff);
        }
        break;

    case SPELLBOOK:
    case SCROLL:
    case WAND:
    case ROD:
    case POTION:
        // Only print the flavor text once we have identified.
        if (is_identified(tmp) && tmp->inv && tmp->inv->msg) {
            // If the embedded spell has a msg, display it here so that the
            // player knows what it does before they actually read/use the item.
            // Strip trailing newlines so that the output of examine() is
            // contiguous.
            // TODO: It might be better to strip the newlines in object_set_msg
            // and append them at print time, rather than ensuring that the msg
            // is newline-terminated and stripping off the newlines when we don't
            // want them; C string handling makes the latter a lot less convenient.
            StringBuffer *sb = stringbuffer_new();
            stringbuffer_append_string(sb, tmp->inv->msg);
            stringbuffer_trim_whitespace(sb);
            char *const fluff = stringbuffer_finish(sb);
            draw_ext_info(NDI_UNIQUE|NDI_BLUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_INFO, fluff);
            free(fluff);
        }
        if (tmp->type == WAND && is_identified(tmp)) {
            snprintf(buf, sizeof(buf), "It has %d charges left.", tmp->stats.food);
        }
        break;

    case BOOK:
        if (tmp->msg != NULL)
            snprintf(buf, sizeof(buf), "Something is written in it.");
        break;

    case CONTAINER:
        if (tmp->race != NULL) {
            if (tmp->weight_limit && tmp->stats.Str < 100)
                snprintf(buf, sizeof(buf), "It can hold only %s and its weight limit is %.1f kg.", tmp->race, tmp->weight_limit/(10.0*(100-tmp->stats.Str)));
            else
                snprintf(buf, sizeof(buf), "It can hold only %s.", tmp->race);
        } else
            if (tmp->weight_limit && tmp->stats.Str < 100)
                snprintf(buf, sizeof(buf), "Its weight limit is %.1f kg.", tmp->weight_limit/(10.0*(100-tmp->stats.Str)));
        break;
    }

    if (buf[0] != '\0')
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      buf);

    // TODO: combine with weight and properly pluralize, so you get:
    // It is made of stone and weighs 15.0 kg.
    // They are made of paper and weigh 3 kg.
    if (tmp->materialname != NULL && !tmp->msg) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                             "It is made of: %s.",
                             tmp->materialname);
    }
    /* Where to wear this item */
    for (i = 0; i < NUM_BODY_LOCATIONS; i++) {
        if (tmp->body_info[i]) {
            if (op->body_info[i]) {
                if (tmp->body_info[i] < -1) {
                    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                                     "%s %s (%d)", tmp->nrof > 1 ? "They go" : "It goes",
                                     body_locations[i].use_name, -tmp->body_info[i]);
                } else {
                    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                                     "%s %s", tmp->nrof > 1 ? "They go" : "It goes",
                                     body_locations[i].use_name);
                }
            } else {
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                                     "%s %s", tmp->nrof > 1 ? "They go" : "It goes",
                                     body_locations[i].nonuse_name);
            }
        }
    }

    if (tmp->weight) {
        snprintf(buf, sizeof(buf), tmp->nrof > 1 ? "They weigh %3.3f kg." : "It weighs %3.3f kg.", tmp->weight*((float)(tmp->nrof ? tmp->nrof : 1)/1000.0));
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      buf);
    }

    in_shop = shop_contains(op);

    if (tmp->value && !QUERY_FLAG(tmp, FLAG_STARTEQUIP) && !QUERY_FLAG(tmp, FLAG_NO_PICK)) {
        if (in_shop) {
            char *value;
            if (QUERY_FLAG(tmp, FLAG_UNPAID)) {
                value = cost_str(shop_price_buy(tmp, op));
                snprintf(buf, sizeof(buf), "%s would cost you %s.", tmp->nrof > 1 ? "They" : "It", value);
                free(value);
            } else {
                value = cost_str(shop_price_sell(tmp, op));
                snprintf(buf, sizeof(buf), "You are offered %s for %s.", value, tmp->nrof > 1 ? "them" : "it");
                free(value);
            }
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                          buf);
        }
    }

    if (QUERY_FLAG(tmp, FLAG_MONSTER))
        examine_monster(op, tmp, 0);

    /* Is this item buildable? */
    if (QUERY_FLAG(tmp, FLAG_IS_BUILDABLE)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                      "This is a buildable item.");
        if (QUERY_FLAG(tmp, FLAG_IS_LINKED)){
            conn = get_button_value(tmp);
            if (conn) {
                FOR_INV_PREPARE(op, tmp_inv) {
                    if (tmp_inv->type == FORCE && tmp_inv->slaying != NULL
                        && strcmp(tmp_inv->slaying, op->map->path) == 0
                        && tmp_inv->msg != NULL
                        && tmp_inv->path_attuned == (uint32_t) conn) {

                        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND,
                                  MSG_TYPE_COMMAND_EXAMINE, "Connected with: %s",
                                  tmp_inv->msg);

                        break;
                    }
                } FOR_INV_FINISH();
            }
        }
    }

    /* Does the object have a message?  Don't show message for all object
     * types - especially if the first entry is a match
     */
    if (tmp->msg
    && tmp->type != EXIT
    && tmp->type != BOOK
    && tmp->type != CORPSE
    && !tmp->move_on
    && strncasecmp(tmp->msg, "@match", 6)) {
        /* This is just a hack so when identifying the items, we print
         * out the extra message. Do this only for "identifiable types", e.g.
         * we don't want to print this message when the player looks at a
         * locked door (where msg is used to store the message they get when
         * trying to force it open).
         *
         * Also, don't print the message for the object unless it has been identified
         * 		-- SilverNexus 2015-05-20
         */
        if (is_identifiable_type(tmp) && is_identified(tmp)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                          "The object has a story:");

		    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
					      tmp->msg);
        }
    }
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_EXAMINE,
                  " "); /* Blank line */

    if (is_identified(tmp)) {
        knowledge_item_can_be_used_alchemy(op, tmp);
    }
}

/**
 * Prints object's inventory.
 *
 * @param op
 * who to print for.
 * @param inv
 * if NULL then print op's inventory, else print the inventory of inv.
 */
void inventory(object *op, object *inv) {
    const char *in;
    int items = 0, length;
    char weight[MAX_BUF], name[MAX_BUF];

    if (inv == NULL && op == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Inventory of what object?");
        return;
    }
    FOR_INV_PREPARE(inv ? inv : op, tmp)
        if ((!tmp->invisible && (inv == NULL || inv->type == CONTAINER || QUERY_FLAG(tmp, FLAG_APPLIED)))
        || !op || QUERY_FLAG(op, FLAG_WIZ))
            items++;
    FOR_INV_FINISH();
    if (inv == NULL) { /* player's inventory */
        if (items == 0) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "You carry nothing.");
            return;
        } else {
            length = 28;
            in = "";
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INVENTORY,
                          "Inventory:");
        }
    } else {
        if (items == 0)
            return;
        else {
            length = 28;
            in = "  ";
        }
    }
    FOR_INV_PREPARE(inv ? inv : op, tmp) {
        if ((!op || !QUERY_FLAG(op, FLAG_WIZ))
        && (tmp->invisible || (inv && inv->type != CONTAINER && !QUERY_FLAG(tmp, FLAG_APPLIED))))
            continue;
        query_weight(tmp, weight, MAX_BUF);
        query_name(tmp, name, MAX_BUF);
        if (!op || QUERY_FLAG(op, FLAG_WIZ))
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INVENTORY,
                                 "[fixed]%s- %-*.*s (%5d) %-8s",
                                 in, length, length, name, tmp->count, weight);
        else
            draw_ext_info_format(NDI_UNIQUE, 0, op,  MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INVENTORY,
                                 "[fixed]%s- %-*.*s %-8s",
                                 in, length+8, length+8, name, weight);
    } FOR_INV_FINISH();
    if (!inv && op) {
        query_weight(op, weight, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INVENTORY,
                             "[fixed]%-*s %-8s",
                             41, "Total weight :", weight);
    }
}

/**
 * Utility function to display the pickup mode for a player.
 *
 * @param op
 * must be a player.
 * @param old
 * previous pickup mode.
 */
static void display_new_pickup(const object *op, int old) {
    int i = op->contr->mode;

    esrv_send_pickup(op->contr);

    if (!(i&PU_NEWMODE) || !(i&PU_DEBUG)) {
        if ((old & PU_INHIBIT) != (op->contr->mode & PU_INHIBIT)) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                "Pickup is %s.", (old & PU_INHIBIT) ? "active" : "inhibited");
        }
        return;
    }

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d NEWMODE",
                         i&PU_NEWMODE ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d DEBUG",
                         i&PU_DEBUG ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d INHIBIT",
                         i&PU_INHIBIT ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d STOP",
                         i&PU_STOP ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d <= x pickup weight/value RATIO (0==off)",
                         (i&PU_RATIO)*5);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d FOOD",
                         i&PU_FOOD ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d DRINK",
                         i&PU_DRINK ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d VALUABLES",
                         i&PU_VALUABLES ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d BOW",
                         i&PU_BOW ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d ARROW",
                         i&PU_ARROW ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d HELMET",
                         i&PU_HELMET ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d SHIELD",
                         i&PU_SHIELD ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d ARMOUR",
                         i&PU_ARMOUR ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d BOOTS",
                         i&PU_BOOTS ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d GLOVES",
                         i&PU_GLOVES ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d CLOAK",
                         i&PU_CLOAK ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d KEY",
                         i&PU_KEY ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d MISSILEWEAPON",
                         i&PU_MISSILEWEAPON ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d MELEEWEAPON",
                         i&PU_MELEEWEAPON ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d MAGICAL",
                         i&PU_MAGICAL ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d POTION",
                         i&PU_POTION ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d SPELLBOOK",
                         i&PU_SPELLBOOK ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d SKILLSCROLL",
                         i&PU_SKILLSCROLL ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d READABLES",
                         i&PU_READABLES ? 1 : 0);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d MAGICDEVICE",
                         i&PU_MAGIC_DEVICE ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d NOT CURSED",
                         i&PU_NOT_CURSED ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d JEWELS",
                         i&PU_JEWELS ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d FLESH",
                         i&PU_FLESH ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d CONTAINER",
                         i&PU_CONTAINER ? 1 : 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "%d CURSED",
                         i&PU_CURSED ? 1 : 0);

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                  "");
}

/**
 * 'pickup' command.
 *
 * @param op
 * player.
 * @param params
 * pickup mode. Can be empty to display the current mode.
 * @todo trash old pickup mode, merge with new pickup.
 */
void command_pickup(object *op, const char *params) {
    uint32_t i;

    if (*params == '\0') {
        /* if the new mode is used, just print the settings */
        if (op->contr->mode&PU_NEWMODE) {
            display_new_pickup(op, op->contr->mode);
            return;
        }
        if (1)
            LOG(llevDebug, "command_pickup: !params\n");
        set_pickup_mode(op, op->contr->mode > 6 ? 0 : op->contr->mode+1);
        return;
    }

    while (*params == ' ')
        params++;

    if (*params == '+' || *params == '-' || *params == '!') {
        int index = get_pickup_mode_index(params + 1);

        if (index != -1) {
            int old = op->contr->mode;
            i = op->contr->mode;
            if (!(i&PU_NEWMODE))
                i = PU_NEWMODE;
            if (*params == '+')
                i = i|pickup_modes[index];
            else if (*params == '-')
                i = i&~pickup_modes[index];
            else {
                if (i&pickup_modes[index])
                    i = i&~pickup_modes[index];
                else
                    i = i|pickup_modes[index];
            }
            op->contr->mode = i;
            display_new_pickup(op, old);
            return;
        }
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "Pickup: invalid item %s\n",
                             params);
        return;
    }

    if (sscanf(params, "%u", &i) != 1) {
        if (1)
            LOG(llevDebug, "command_pickup: params==NULL\n");
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Usage: pickup <0-7> or <value_density> .");
        return;
    }
    set_pickup_mode(op, i);
    display_new_pickup(op, op->contr->mode);
}

/**
 * Sets the 'old' pickup mode.
 *
 * @param op
 * player.
 * @param i
 * new pickup mode.
 */
static void set_pickup_mode(const object *op, int i) {
    op->contr->mode = i;
    switch (op->contr->mode) {
    case 0:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "Mode: Don't pick up.");
        break;

    case 1:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "Mode: Pick up one item.");
        break;

    case 2:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "Mode: Pick up one item and stop.");
        break;

    case 3:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "Mode: Stop before picking up.");
        break;

    case 4:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "Mode: Pick up all items.");
        break;

    case 5:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "Mode: Pick up all items and stop.");
        break;

    case 6:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "Mode: Pick up all magic items.");
        break;

    case 7:
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "Mode: Pick up all coins and gems");
        break;
    }
}

/**
 * 'search-items' command.
 *
 * @param op
 * player.
 * @param params
 * options.
 */
void command_search_items(object *op, const char *params) {
    if (settings.search_items == FALSE)
        return;

    if (!params || *params == '\0') {
        if (op->contr->search_str[0] == '\0') {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                          "Example: search magic+1 "
                          "Would automatically pick up all "
                          "items containing the word 'magic+1'.");
            return;
        }
        op->contr->search_str[0] = '\0';
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                      "Search mode turned off.");
        fix_object(op);
        return;
    }
    if ((int)strlen(params) >= MAX_BUF) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Search string too long.");
        return;
    }
    strcpy(op->contr->search_str, params);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                         "Searching for '%s'.",
                         op->contr->search_str);
    fix_object(op);
}

/**
 * Changing the custom name of an item
 *
 * Syntax is: rename \<what object\> to \<new name\>
 * - if 'what object' is omitted, marked object is used
 * - if 'to new name' is omitted, custom name is cleared
 *
 * Names are considered for all purpose having a length <=127 (max length sent to client
 * by server).
 *
 * @param op
 * player.
 * @param params
 * how to rename.
 */
void command_rename_item(object *op, const char *params) {
    char buf[VERY_BIG_BUF], name[MAX_BUF];
    tag_t itemnumber;
    object *item = NULL;
    object *tmp;
    char *closebrace;
    size_t counter;

    if (*params != '\0') {
        /* Let's skip white spaces */
        while (' ' == *params)
            params++;

        /* Checking the first part */
        itemnumber = atoi(params);
        if (itemnumber != 0) {
            int found = 0;
            FOR_INV_PREPARE(op, item)
                if (item->count == itemnumber && !item->invisible) {
                    found = 1;
                    break;
                }
            FOR_INV_FINISH();
            if (!found) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "Tried to rename an invalid item.");
                return;
            }
            while (isdigit(*params) || ' ' == *params)
                params++;
        } else if ('<' == *params) {
            /* Got old name, let's get it & find appropriate matching item */
            closebrace = strchr(params, '>');
            if (!closebrace) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "Syntax error!");
                return;
            }
            /* Sanity check for buffer overruns */
            if (closebrace-params > 127) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "Old name too long (up to 127 characters allowed)!");
                return;
            }
            /* Copy the old name */
            snprintf(buf, sizeof(buf), "%.*s", (int)(closebrace-(params+1)), params+1);

            /* Find best matching item */
            item = find_best_object_match(op, buf);
            if (!item) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "Could not find a matching item to rename.");
                return;
            }

            /* Now need to move pointer to just after > */
            params = closebrace+1;
            while (' ' == *params)
                params++;
        } else {
            /* Use marked item */
            item = find_marked_object(op);
            if (!item) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "No marked item to rename.");
                return;
            }
        }

        /* Now let's find the new name */
        if (!strncmp(params, "to ", 3)) {
            params += 3;
            while (' ' == *params)
                params++;
            if ('<' != *params) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "Syntax error, expecting < at start of new name!");
                return;
            }
            closebrace = strchr(params+1, '>');
            if (!closebrace) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "Syntax error, expecting > at end of new name!");
                return;
            }

            /* Sanity check for buffer overruns */
            if (closebrace-params > 127) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "New name too long (up to 127 characters allowed)!");
                return;
            }

            /* Copy the new name */
            snprintf(buf, sizeof(buf), "%.*s", (int)(closebrace-(params+1)), params+1);

            /* Let's check it for weird characters */
            for (counter = 0; counter < strlen(buf); counter++) {
                if (isalnum(buf[counter]))
                    continue;
                if (' ' == buf[counter])
                    continue;
                if ('\'' == buf[counter])
                    continue;
                if ('+' == buf[counter])
                    continue;
                if ('_' == buf[counter])
                    continue;
                if ('-' == buf[counter])
                    continue;

                /* If we come here, then the name contains an invalid character...
                 * tell the player & exit
                 */
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "Invalid new name!");
                return;
            }
        } else {
            /* If param contains something, then syntax error... */
            if (strlen(params)) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "Syntax error, expected 'to <' after old name!");
                return;
            }
            /* New name is empty */
            buf[0] = '\0';
        }
    } else {
        /* Last case: *params=='\0' */
        item = find_marked_object(op);
        if (!item) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "No marked item to rename.");
            return;
        }
        buf[0] = '\0';
    }

    /* Coming here, everything is fine... */
    if (!strlen(buf)) {
        /* Clear custom name */
        if (item->custom_name == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "This item has no custom name.");
            return;
        }

        FREE_AND_CLEAR_STR(item->custom_name);
        query_base_name(item, item->nrof > 1 ? 1 : 0, name, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                             "You stop calling your %s with weird names.",
                             name);
    } else {
        if (item->custom_name != NULL && strcmp(item->custom_name, buf) == 0) {
            query_base_name(item, item->nrof > 1 ? 1 : 0, name, MAX_BUF);
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                                 "You keep calling your %s %s.",
                                 name, buf);
            return;
        }

        /* Set custom name */
        FREE_AND_COPY(item->custom_name, buf);

        query_base_name(item, item->nrof > 1 ? 1 : 0, name, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                             "Your %s will now be called %s.",
                             name, buf);
    }

    tmp = object_merge(item, NULL);
    if (tmp == NULL) {
        /* object was not merged - if it was, object_merge() handles updating for us. */
        esrv_update_item(UPD_NAME, op, item);
    }
}

/**
 * Alternate way to lock/unlock items (command line).
 *
 * @param op
 * player
 * @param params
 * sent command line.
 */
void command_lock_item(object *op, const char *params) {
    object *item;
    object *tmp;
    char name[HUGE_BUF];

    if (*params == '\0' || strlen(params) == 0) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
                      "Lock what item?");
        return;
    }

    item = find_best_object_match(op, params);
    if (!item) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
                      "Can't find any matching item.");
        return;
    }

    query_short_name(item, name, HUGE_BUF);
    if (QUERY_FLAG(item, FLAG_INV_LOCKED)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
                             "Unlocked %s.", name);
        CLEAR_FLAG(item, FLAG_INV_LOCKED);
    } else {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE,
                             "Locked %s.", name);
        SET_FLAG(item, FLAG_INV_LOCKED);
    }

    tmp = object_merge(item, NULL);
    if (tmp == NULL) {
        /* object was not merged, if it was object_merge() handles updates for us */
        esrv_update_item(UPD_FLAGS, op, item);
    }
}

/**
 * Try to use an item on another. Items are checked for key/values matching.
 * @param op
 * player.
 * @param params
 * sent string, with all parameters.
 */
void command_use(object *op, const char *params) {
    char *with, copy[MAX_BUF];
    object *first, *second/*, *add*/;
    /*archetype *arch;*/
    /*int count;*/
    /*sstring data;*/
    recipe *transformation;

    if (!IS_PLAYER(op))
        return;

    strlcpy(copy, params, sizeof(copy));
    with = strstr(copy, " with ");
    if (!with) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "Syntax is use <item> with <item>.");
        return;
    }

    with[0] = '\0';
    with = with+strlen(" with ");

    first = find_best_object_match(op, copy);
    if (!first) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "No match for %s.", copy);
        return;
    }
    second = find_best_object_match(op, with);
    if (!second) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "No match for %s.", with);
        return;
    }

    transformation = NULL;
    while ((transformation = find_recipe_for_tool(first->arch->name, transformation))) {
        /** @todo handle multiple ingredients */
        if (transformation->ingred_count != 1)
            continue;

/*        LOG(llevDebug, "use: check %s\n", transformation->title);*/
        if (strcmp(second->name, transformation->ingred->name) == 0) {
            /** @todo handle ingredient count, handle batches, and such */
            object *generated = create_archetype(transformation->arch_name[0]);
            if (transformation->yield)
                generated->nrof = transformation->yield;
            object_insert_in_ob(generated, op);
            /*draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "Found recipe %s", transformation->title);*/
            object_decrease_nrof_by_one(second);
            return;
        }
    }
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "Nothing happens.");
    return;

    /*
    snprintf(copy, sizeof(copy), "on_use_with_%s", first->arch->name);
    data = object_get_value(second, copy);
    if (!data) {
        snprintf(copy, sizeof(copy), "on_use_with_%d_%d", first->type, first->subtype);
        data = object_get_value(second, copy);
        if (!data) {
            snprintf(copy, sizeof(copy), "on_use_with_%d", first->type);
            data = object_get_value(second, copy);
            if (!data) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_FAILURE, "Nothing happens.");
                return 1;
            }
        }
    }

    while (data != NULL) {
        if (strncmp(data, "add ", 4) == 0) {
            data += 4;
            if (isdigit(*data)) {
                count = atol(data);
                data = strchr(data, ' ')+1;
            } else
                count = 1;
            with = strchr(data, ' ');
            if (!with) {
                strncpy(copy, data, sizeof(copy));
                data = NULL;
            } else {
                *with = '\0';
                strncpy(copy, data, sizeof(copy));
                data += strlen(copy)+1;
            }
            arch = find_archetype(copy);
            if (!arch) {
                LOG(llevError, "Use: invalid archetype %s in %s.\n", copy, second->name);
                return 1;
            }
            add = object_create_arch(arch);
            add->nrof = count;
            object_insert_in_ob(add, op);
        } else if (strncmp(data, "remove $", 8) == 0) {
            data += 8;
            if (*data == '1') {
                if (first)
                    first = object_decrease_nrof_by_one(first);
                data += 2;
            } else if (*data == '2') {
                if (second)
                    second = object_decrease_nrof_by_one(second);
                data += 2;
            } else {
                LOG(llevError, "Use: invalid use string %s in %s\n", data, second->name);
                return 1;
            }
        } else {
            LOG(llevError, "Use: invalid use string %s in %s\n", data, second->name);
            return 1;
        }
    }

    return 1;
    */
}
