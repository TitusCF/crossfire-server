/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2007 Mark Wedel & Crossfire Development Team
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
/** @file potion.c
 * The implementation of the Potion class of objects.
 */
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>
#include <sproto.h>

static method_ret potion_type_apply(ob_methods *context, object *potion,
    object* applier, int aflags);

/**
 * Initializer for the potion object type.
 */
void init_type_potion(void)
{
    register_apply(POTION, potion_type_apply);
}

/**
 * Handles applying a potion.
 * @param context The method context
 * @param potion The potion to apply
 * @param applier The object attempting to apply the potion
 * @param aflags Special flags (always apply/unapply)
 * @return METHOD_OK unless failure for some reason.
 */
static method_ret potion_type_apply(ob_methods *context, object *potion,
    object* applier, int aflags) {
    int got_one=0,i;
    object *force;

    if(applier->type==PLAYER) {
        if (!QUERY_FLAG(potion, FLAG_IDENTIFIED))
            identify(potion);
    }

    handle_apply_yield(potion);

        /* Potion of restoration - only for players */
    if (applier->type==PLAYER&&(potion->attacktype & AT_DEPLETE)) {
        object *depl;
        archetype *at;

        if (QUERY_FLAG(potion, FLAG_CURSED) || QUERY_FLAG(potion, FLAG_DAMNED)) {
            drain_stat(applier);
            fix_object(applier);
            decrease_ob(potion);
            return METHOD_OK;
        }
        if ((at = find_archetype(ARCH_DEPLETION))==NULL) {
            LOG(llevError,"Could not find archetype depletion\n");
            return METHOD_ERROR;
        }
        depl = present_arch_in_ob(at, applier);
        if (depl!=NULL && (potion->level != 0 && potion->level >= applier->level)) {
            for (i = 0; i < NUM_STATS; i++)
                if (get_attr_value(&depl->stats, i)) {
                    draw_ext_info(NDI_UNIQUE,0,applier, MSG_TYPE_ATTRIBUTE,
                                  MSG_TYPE_ATTRIBUTE_STAT_GAIN,
                                  restore_msg[i], NULL);
                }
            remove_ob(depl);
            free_object(depl);
            fix_object(applier);
        }
        else
            draw_ext_info(NDI_UNIQUE,0,applier, MSG_TYPE_APPLY,
                          MSG_TYPE_APPLY_FAILURE,
                          "You potion had no effect.", NULL);

        decrease_ob(potion);
        return METHOD_OK;
    }

        /* improvement potion - only for players */
    if(applier->type==PLAYER&&potion->attacktype&AT_GODPOWER) {

        for(i=1;i<MIN(11,applier->level);i++) {
            if (QUERY_FLAG(potion,FLAG_CURSED) || QUERY_FLAG(potion,FLAG_DAMNED)) {
                if (applier->contr->levhp[i]!=1) {
                    applier->contr->levhp[i]=1;
                    break;
                }
                if (applier->contr->levsp[i]!=1) {
                    applier->contr->levsp[i]=1;
                    break;
                }
                if (applier->contr->levgrace[i]!=1) {
                    applier->contr->levgrace[i]=1;
                    break;
                }
            }
            else {
                if(applier->contr->levhp[i]<9) {
                    applier->contr->levhp[i]=9;
                    break;
                }
                if(applier->contr->levsp[i]<6) {
                    applier->contr->levsp[i]=6;
                    break;
                }
                if(applier->contr->levgrace[i]<3) {
                    applier->contr->levgrace[i]=3;
                    break;
                }
            }
        }
            /* Just makes checking easier */
        if (i<MIN(11, applier->level)) got_one=1;
        if (!QUERY_FLAG(potion,FLAG_CURSED) && !QUERY_FLAG(potion,FLAG_DAMNED)) {
            if (got_one) {
                fix_object(applier);
                draw_ext_info(NDI_UNIQUE,0,applier,
                              MSG_TYPE_APPLY, MSG_TYPE_APPLY_SUCCESS,
                              "The Gods smile upon you and remake you a little more in their image."
                              "You feel a little more perfect.", NULL);
            }
            else
                draw_ext_info(NDI_UNIQUE,0,applier,
                              MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                              "The potion had no effect - you are already perfect", NULL);
        }
        else {  /* cursed potion */
            if (got_one) {
                fix_object(applier);
                draw_ext_info(NDI_UNIQUE,0,applier,
                              MSG_TYPE_APPLY, MSG_TYPE_APPLY_CURSED,
                              "The Gods are angry and punish you.", NULL);
            }
            else
                draw_ext_info(NDI_UNIQUE,0,applier,
                              MSG_TYPE_APPLY, MSG_TYPE_APPLY_CURSED,
                              "You are fortunate that you are so pathetic.", NULL);
        }
        decrease_ob(potion);
        return METHOD_OK;
    }


        /* A potion that casts a spell.  Healing, restore spellpoint
         * (power potion) and heroism all fit into this category.
         * Given the spell object code, there is no limit to the number
         * of spells that potions can be cast, but direction is
         * problematic to try and imbue fireball potions for example.
         */
    if (potion->inv) {
        if(QUERY_FLAG(potion, FLAG_CURSED) || QUERY_FLAG(potion, FLAG_DAMNED)) {
            object *fball;

            draw_ext_info(NDI_UNIQUE,0,applier,
                          MSG_TYPE_APPLY, MSG_TYPE_APPLY_CURSED,
                          "Yech!  Your lungs are on fire!", NULL);
                /* Explodes a fireball centered at player */
            fball = create_archetype(EXPLODING_FIREBALL);
            fball->dam_modifier=random_roll(1, applier->level, applier, PREFER_LOW)/5+1;
            fball->stats.maxhp=random_roll(1, applier->level, applier, PREFER_LOW)/10+2;
            fball->x = applier->x;
            fball->y = applier->y;
            insert_ob_in_map(fball, applier->map, NULL, 0);
        } else
            cast_spell(applier,potion, applier->facing, potion->inv, NULL);

        decrease_ob(potion);
            /* if youre dead, no point in doing this... */
        if(!QUERY_FLAG(applier,FLAG_REMOVED))
            fix_object(applier);
        return METHOD_OK;
    }

        /* Deal with protection potions */
    force=NULL;
    for (i=0; i<NROFATTACKS; i++) {
        if (potion->resist[i]) {
            if (!force) force=create_archetype(FORCE_NAME);
            memcpy(force->resist, potion->resist, sizeof(potion->resist));
            force->type=POTION_EFFECT;
            break;  /* Only need to find one protection since we cappliery entire batch */
        }
    }
        /* This is a protection potion */
    if (force) {
            /* cursed items last longer */
        if(QUERY_FLAG(potion, FLAG_CURSED) || QUERY_FLAG(potion, FLAG_DAMNED)) {
            force->stats.food*=10;
            for (i=0; i<NROFATTACKS; i++)
                if (force->resist[i] > 0)
                    force->resist[i] = -force->resist[i];  /* prot => vuln */
        }
        force->speed_left= -1;
        force = insert_ob_in_ob(force,applier);
        CLEAR_FLAG(potion, FLAG_APPLIED);
        SET_FLAG(force,FLAG_APPLIED);
        change_abil(applier,force);
        decrease_ob(potion);
        return METHOD_OK;
    }

        /* Only thing left are the stat potions */
    if(applier->type==PLAYER) { /* only for players */
        if((QUERY_FLAG(potion, FLAG_CURSED) || QUERY_FLAG(potion, FLAG_DAMNED))
           && potion->value!=0)
            CLEAR_FLAG(potion, FLAG_APPLIED);
        else
            SET_FLAG(potion, FLAG_APPLIED);
        if(!change_abil(applier,potion))
            draw_ext_info(NDI_UNIQUE,0,applier,
                          MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE,
                          "Nothing happened.", NULL);
    }

        /* CLEAR_FLAG is so that if the character has other potions
         * that were grouped with the one consumed, his
         * stat will not be raised by them.  fix_object just clears
         * up all the stats.
         */
    CLEAR_FLAG(potion, FLAG_APPLIED);
    fix_object(applier);
    decrease_ob(potion);
    return METHOD_OK;
}
