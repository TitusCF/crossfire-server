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
 * The implementation of the Food class of objects.
 */

#include "global.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "ob_methods.h"
#include "ob_types.h"
#include "sounds.h"
#include "sproto.h"

static method_ret food_type_apply(object *food, object *applier, int aflags);
static void eat_special_food(object *who, object *food);
static int dragon_eat_flesh(object *op, object *meal);

/**
 * Initializer for the food object type.
 */
void init_type_food(void) {
    register_apply(FOOD, food_type_apply);
    register_apply(DRINK, food_type_apply);
    register_apply(FLESH, food_type_apply);
}

static void cursed_food_effects(object *who, object *food) {
    /* check for hp, sp change */
    if (food->stats.hp != 0 && !is_wraith_pl(who)) {
        if (QUERY_FLAG(food, FLAG_CURSED)) {
            safe_strncpy(who->contr->killer, food->name,
                    sizeof(who->contr->killer));
            hit_player(who, food->stats.hp, food, AT_POISON, 1);
            draw_ext_info(NDI_UNIQUE, 0, who, MSG_TYPE_APPLY, MSG_TYPE_APPLY_CURSED,
                "Eck!...that was poisonous!");
        } else {
            if (food->stats.hp > 0)
                draw_ext_info(NDI_UNIQUE, 0, who, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                    "You begin to feel better.");
            else
                draw_ext_info(NDI_UNIQUE, 0, who, MSG_TYPE_APPLY, MSG_TYPE_APPLY_CURSED,
                    "Eck!...that was poisonous!");
            who->stats.hp += food->stats.hp;
        }
    }
    if (food->stats.sp != 0) {
        if (QUERY_FLAG(food, FLAG_CURSED)) {
            draw_ext_info(NDI_UNIQUE, 0, who, MSG_TYPE_APPLY, MSG_TYPE_APPLY_CURSED,
                "You are drained of mana!");
            who->stats.sp -= food->stats.sp;
            if (who->stats.sp < 0)
                who->stats.sp = 0;
        } else {
            draw_ext_info(NDI_UNIQUE, 0, who, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                "You feel a rush of magical energy!");
            who->stats.sp += food->stats.sp;
            /* place limit on max sp from food? */
        }
    }
}

static void eat_common(object* applier, object* food) {
    char buf[MAX_BUF];
    int capacity_remaining = MAX_FOOD - applier->stats.food;
    applier->stats.food += food->stats.food;
    if (capacity_remaining < food->stats.food)
        applier->stats.hp += capacity_remaining / 50;
    else
        applier->stats.hp += food->stats.food / 50;
    if (applier->stats.hp > applier->stats.maxhp)
        applier->stats.hp = applier->stats.maxhp;
    if (applier->stats.food > MAX_FOOD)
        applier->stats.food = MAX_FOOD;
    const int wasted_food = food->stats.food - capacity_remaining;
    if (wasted_food > 0) {
        const int thresh = MAX_FOOD/4;
        int speed_penalty;
        if (wasted_food > thresh) {
            speed_penalty = 10;
        } else {
            speed_penalty = 5;
        }

        if (slow_living_by(applier, speed_penalty)) {
            snprintf(buf, sizeof(buf),
                     "%s the %s makes you %s full. You feel yourself moving %s "
                     "slower.",
                     food->type == DRINK ? "Drinking" : "Eating", food->name,
                     wasted_food > thresh ? "very" : "rather",
                     wasted_food > thresh ? "much" : "somewhat");
            draw_ext_info(NDI_UNIQUE, 0, applier, MSG_TYPE_ATTRIBUTE,
                          MSG_TYPE_ATTRIBUTE_BAD_EFFECT_START, buf);
        }
    }
}

/**
 * Handles applying food.
 * If player is applying, takes care of messages and dragon special food.
 * @param food The food to apply
 * @param applier The object attempting to apply the food
 * @param aflags Special flags (always apply/unapply)
 * @return METHOD_OK unless failure for some reason.
 */
static method_ret food_type_apply(object *food, object *applier, int aflags) {
    (void)aflags;
    if (QUERY_FLAG(food, FLAG_NO_PICK)) {
        draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY,
                             MSG_TYPE_APPLY_FAILURE, "You can't %s that!",
                             food->type == DRINK ? "drink" : "eat");
        return METHOD_OK;
    }

    if (applier->type != PLAYER) {
        applier->stats.hp = applier->stats.maxhp;
    } else {
        char buf[MAX_BUF];
        if (food->type == FLESH && is_dragon_pl(applier)) {
            /* check if this is a dragon (player), eating some flesh */
            if (!QUERY_FLAG(food, FLAG_CURSED)) {
                dragon_eat_flesh(applier, food);
                eat_common(applier, food);
            } else {
                cursed_food_effects(applier, food);
            }
        } else if (is_old_wraith_pl(applier)) {
            /* Check for old wraith player, give them the feeding skill */
            object *skill = give_skill_by_name(applier, "wraith feed");
            if (skill) {
                SET_FLAG(skill, FLAG_CAN_USE_SKILL);
                link_player_skills(applier);

                snprintf(buf, sizeof(buf), "You have been dead for too long to taste %s, ", food->name);
                draw_ext_info(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                    buf);
                draw_ext_info(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                    "and seem to have obtained a taste for living flesh.");
            } else
                LOG(llevError, "wraith feed skill not found\n");
        } else if (is_wraith_pl(applier)) {
            /* Wraith player gets no food from eating. */
            snprintf(buf, sizeof(buf), "You can no longer taste %s, and do not feel less hungry after %s it.", food->name, food->type == DRINK ? "drinking" : "eating");
            draw_ext_info(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                buf);
        } else {
            /* usual case - not a wraith or a dragon eating non-flesh */
            if (!QUERY_FLAG(food, FLAG_CURSED)) {
                if (!is_dragon_pl(applier)) {
                    /* eating message for normal players*/
                    if (food->type == DRINK)
                        snprintf(buf, sizeof(buf), "Ahhh...that %s tasted good.", food->name);
                    else
                        snprintf(buf, sizeof(buf), "The %s tasted %s", food->name, food->type == FLESH ? "terrible!" : "good.");
                } else {
                    /* eating message for dragon players*/
                    snprintf(buf, sizeof(buf), "The %s tasted terrible!", food->name);
                }
                draw_ext_info(0, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS, buf);
                eat_common(applier, food);
            } else {
                cursed_food_effects(applier, food);
            }

            /* special food hack -b.t. */
            if (food->title)
                eat_special_food(applier, food);
        }
    }
    if (food->type == DRINK) {
        play_sound_map(SOUND_TYPE_ITEM, applier, 0, "drink");
    }
    apply_handle_yield(food);
    object_decrease_nrof_by_one(food);
    return METHOD_OK;
}

/**
 * Handles player eating food that temporarily changes status
 * (resistances, stats).
 * This used to call cast_change_attr(), but
 * that doesn't work with the new spell code.  Since we know what
 * the food changes, just grab a force and use that instead.
 *
 * @param who
 * living eating food.
 * @param food
 * eaten food.
 */
static void eat_special_food(object *who, object *food) {
    object *force;
    int i, did_one = 0;
    int8_t k;

    force = create_archetype(FORCE_NAME);

    for (i = 0; i < NUM_STATS; i++) {
        k = get_attr_value(&food->stats, i);
        if (k) {
            set_attr_value(&force->stats, i, k);
            did_one = 1;
        }
    }

    /* check if we can protect the eater */
    for (i = 0; i < NROFATTACKS; i++) {
        if (food->resist[i] > 0) {
            force->resist[i] = food->resist[i]/2;
            did_one = 1;
        }
    }
    if (did_one) {
        force->speed = MOVE_PER_SECOND;
        object_update_speed(force);
        /* bigger morsel of food = longer effect time */
        force->duration = food->stats.food/4;
        SET_FLAG(force, FLAG_IS_USED_UP);
        object_insert_in_ob(force, who);
        SET_FLAG(force, FLAG_APPLIED);
        change_abil(who, force);
        if (food->other_arch != NULL && who->map != NULL) {
            object_insert_in_map_at(arch_to_object(food->other_arch), who->map, NULL, INS_ON_TOP, who->x, who->y);
        }
    } else {
        object_free_drop_inventory(force);
    }

    fix_object(who);
}

/**
 * A dragon is eating some flesh. If the flesh contains resistances,
 * there is a chance for the dragon's skin to get improved.
 *
 * @param op
 * object (dragon player) eating the flesh.
 * @param meal
 * flesh item, getting chewed in dragon's mouth.
 * @return
 * 1 if meal was eaten, 0 else.
 * @note
 * meal's nrof isn't decreased, caller is responsible for that.
 */
static int dragon_eat_flesh(object *op, object *meal) {
    object *skin = NULL;    /* pointer to dragon skin force*/
    object *abil = NULL;    /* pointer to dragon ability force*/

    char buf[MAX_BUF];            /* tmp. string buffer */
    double chance;                /* improvement-chance of one resist type */
    double totalchance = 1;       /* total chance of gaining one resistance */
    double bonus = 0;             /* level bonus (improvement is easier at lowlevel) */
    double mbonus = 0;            /* monster bonus */
    int atnr_winner[NROFATTACKS]; /* winning candidates for resistance improvement */
    int winners = 0;              /* number of winners */
    int i;                        /* index */

    /* let's make sure and doublecheck the parameters */
    if (meal->type != FLESH || !is_dragon_pl(op))
        return 0;

    /* now grab the 'dragon_skin'- and 'dragon_ability'-forces
     * from the player's inventory
     */
    skin = object_find_by_type_and_arch_name(op, FORCE, "dragon_skin_force");
    abil = object_find_by_type_and_arch_name(op, FORCE, "dragon_ability_force");

    /* if either skin or ability are missing, this is an old player
     * which is not to be considered a dragon -> bail out
     */
    if (skin == NULL || abil == NULL)
        return 0;

    /*LOG(llevDebug, "-> player: %d, flesh: %d\n", op->level, meal->level);*/

    /* on to the interesting part: chances for adding resistance */
    for (i = 0; i < NROFATTACKS; i++) {
        if (meal->resist[i] > 0 && atnr_is_dragon_enabled(i)) {
            /* got positive resistance, now calculate improvement chance (0-100) */

            /* this bonus makes resistance increase easier at lower levels */
            bonus = (settings.max_level-op->level)*30./((double)settings.max_level);
            if (i == abil->stats.exp)
                bonus += 5;  /* additional bonus for resistance of ability-focus */

            /* monster bonus increases with level, because high-level
             * flesh is too rare
             */
            mbonus = op->level*20./((double)settings.max_level);

            chance = (((double)MIN(op->level+bonus, meal->level+bonus+mbonus))*100./((double)settings.max_level))-skin->resist[i];

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
            if (RANDOM()%10000 < (unsigned int)(chance*100)) {
                atnr_winner[winners] = i;
                winners++;
            }

            if (chance >= 0.01)
                totalchance *= 1-chance/100;

            /*LOG(llevDebug, "   %s: bonus %.1f, chance %.1f\n", attacks[i], bonus, chance);*/
        }
    }

    /* inverse totalchance as until now we have the failure-chance   */
    totalchance = 100-totalchance*100;
    /* print message according to totalchance */
    if (totalchance > 50.)
        snprintf(buf, sizeof(buf), "Hmm! The %s tasted delicious!", meal->name);
    else if (totalchance > 10.)
        snprintf(buf, sizeof(buf), "The %s tasted very good.", meal->name);
    else if (totalchance > 1.)
        snprintf(buf, sizeof(buf), "The %s tasted good.", meal->name);
    else if (totalchance > 0.1)
        snprintf(buf, sizeof(buf), "The %s tasted bland.", meal->name);
    else if (totalchance >= 0.01)
        snprintf(buf, sizeof(buf), "The %s had a boring taste.", meal->name);
    else if (meal->last_eat > 0 && atnr_is_dragon_enabled(meal->last_eat))
        snprintf(buf, sizeof(buf), "The %s tasted strange.", meal->name);
    else
        snprintf(buf, sizeof(buf), "The %s had no taste.", meal->name);
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
        buf);

        /* now choose a winner if we have any */
    i = -1;
    if (winners > 0)
        i = atnr_winner[RANDOM()%winners];

    if (i >= 0 && i < NROFATTACKS && skin->resist[i] < 95) {
        /* resistance increased! */
        skin->resist[i]++;
        fix_object(op);

        draw_ext_info_format(NDI_UNIQUE|NDI_RED, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_PROTECTION_GAIN,
            "Your skin is now more resistant to %s!",
            change_resist_msg[i]);
    }

    /* if this flesh contains a new ability focus, we mark it
     * into the ability_force and it will take effect on next level
     */
    if (meal->last_eat > 0
    && atnr_is_dragon_enabled(meal->last_eat)
    && meal->last_eat != abil->last_eat) {
        abil->last_eat = meal->last_eat; /* write:last_eat <new attnr focus> */

        if (meal->last_eat != abil->stats.exp) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
                "Your metabolism prepares to focus on %s!",
                change_resist_msg[meal->last_eat]);
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
                "The change will happen at level %d",
                abil->level+1);
        } else {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
                "Your metabolism will continue to focus on %s.",
                change_resist_msg[meal->last_eat]);
            abil->last_eat = 0;
        }
    }
    return 1;
}
