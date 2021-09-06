/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2021 The Crossfire Development Team
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
 * Implementation of the Lightable class of objects.
 *
 * This allows us to make lightable objects be lit by simply applying them,
 * rather than by marking the lightable item, then applying the lighter.
 */

#include "global.h"

#include "ob_methods.h"
#include "sproto.h"

static method_ret lightable_type_apply(object *lightable, object *applier, int aflags);

/**
 * Initializer for the LIGHTABLE object type.
 */
void init_type_lightable(void) {
    register_apply(LIGHTABLE, lightable_type_apply);
}

/**
 * Attempt to find a lighter, wielded fire weapon, or fire spell to light with.
 *
 * @param lightable
 * The lightable object we applied.
 * @param applier
 * The object applying the lightable object.
 * Usually will be a player, but not guaranteed.
 * @param aflags
 * special flags (always apply/unapply)
 *
 * @return
 * METHOD_OK. Even when we fail to light the object,
 * we did handle the apply, so it would not be unhandled.
 */
static method_ret lightable_type_apply(object *lightable, object *applier, int aflags){
    // We will find our lighter tool with this.
    // Note that we can use some things with fire attacktype that aren't lighters, because reasons.
    // wannabe_tool is used as a kind of temporary storage for tools that may become the selected tool, but might not as well.
    // unused_tool is a storage for when an item could be the tool of choice, but curses prevent something else from being unapplied.
    object *tool = NULL, *wannabe_tool = find_marked_object(applier), *unused_tool = NULL;
    int current_weapon_is_cursed = 0, low_mana_amt = 999;
    // First, use the marked item if it is a lighter or has fire attacktype.
    if (wannabe_tool && ((wannabe_tool->attacktype & AT_FIRE) || wannabe_tool->type == LIGHTER)) {
        tool = wannabe_tool;
    }
    // Next, let us look at the current weapon of the applier. If it has a fire attacktype, use that.
    else if (applier->current_weapon && (applier->current_weapon->attacktype & AT_FIRE)) {
        tool = applier->current_weapon;
    }
    else {
        // We're done using wannabe_tool for the marked object. Clear it out.
        wannabe_tool = NULL;
        // Look for other ways to light the item.
        // Loop through the inventory for a lighter, the pyromancy skill, or a fire spell.
        FOR_INV_PREPARE(applier, tmp) {
            switch (tmp->type) {
                case LIGHTER:
                    wannabe_tool = tmp;
                    break;
                case SKILL:
                case SKILL_TOOL:
                    // FIRE_MAGIC isn't used AFAIK, but account for it in case it ever is.
                    // Either way, we need 10 mana to use this directly.
                    if ((tmp->subtype == SK_PYROMANCY || tmp->subtype == SK_FIRE_MAGIC) && applier->stats.sp > 10)
                        wannabe_tool = tmp;
                    break;
                case WEAPON:
                    if (tmp->attacktype & AT_FIRE) {
                        // Make sure our wielded weapon is not cursed or damned.
                        // If it is, we can't set it down to use this one for a sec.
                        if (!applier->current_weapon || !(QUERY_FLAG(applier->current_weapon, FLAG_CURSED) || QUERY_FLAG(applier->current_weapon, FLAG_DAMNED))) {
                            wannabe_tool = tmp;
                        }
                        else {
                            // Note that the equipped weapon is cursed. We will print a message at the end.
                            current_weapon_is_cursed = 1;
                            unused_tool = tmp;
                        }
                    }
                    break;
                case SPELL:
                    // Don't choose a spell if it is too expensive to cast.
                    if (tmp->attacktype & AT_FIRE && tmp->stats.sp < applier->stats.sp && tmp->stats.grace < applier->stats.grace) {
                        wannabe_tool = tmp;
                    }
                    break;
            }
            // Determine the tool to use
            // Priority goes in this order:
            // WEAPON -> highest priority, use first encountered
            // SPELL -> next priority, lower mana cost is better (ignores level scaling, since we are using that spell in a careful, low-energy way)
            // SKILL, SKILL_TOOL, defaults to 10 mana, first one found suffices.
            // LIGHTER, first one found suffices.
            //
            // This prevents us from wasting mana or lighter charges if we have another option.
            if (wannabe_tool) {
                if (!tool) {
                    tool = wannabe_tool;
                }
                else if (wannabe_tool->type == WEAPON ||
                        (wannabe_tool->type == SPELL && tool->type != WEAPON && !(tool->type == SPELL && low_mana_amt < wannabe_tool->stats.sp + wannabe_tool->stats.grace)) ||
                        ((wannabe_tool->type == SKILL || wannabe_tool->type == SKILL_TOOL) && tool->type == LIGHTER)) {
                            tool = wannabe_tool;
                            // Keep track of used mana amount, since we want to ensure that we take the smallest mana + grace cost.
                            if (tool->type == SPELL)
                                low_mana_amt = tool->stats.sp + tool->stats.grace;
                }
            }
            // Weapon has the shallowest cost, so if we found that, then use it.
            if (tool && tool->type == WEAPON)
                break;
        } FOR_INV_FINISH();
    }
    // All searching is done. Time to see what we got.
    if (tool) {
        if (applier->type == PLAYER && current_weapon_is_cursed) {
            // Print a message to players telling them they can't unequip current_weapon to wield something else.
            draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE, "Could not use %s on %s because your %s is cursed!",
                unused_tool->name, lightable->name, applier->current_weapon->name);
        }
        // Actually attempt to thaw/light the lightable item.
        // If we are using a lighter, then go all the way through the lighter apply code.
        // This way we aren't having to rewrite any extra code here.
        if (tool->type == LIGHTER) {
            if (applier->type == PLAYER) {
                // Store the marked item for safekeeping, then make the lightable be our marked object.
                object *old_marked = find_marked_object(applier);
                // Don't do anything special if old_marked is the same as lightable.
                if (old_marked == lightable) {
                    // Just set to null so when we are done, we set the mark pointer to null.
                    old_marked = NULL;
                }
                else {
                    applier->contr->mark = lightable;
                    applier->contr->mark_count = lightable->count;
                }
                // Apply the lighter.
                apply_manual(applier, tool, aflags);
                // When done, return the marked object back to its original object.
                applier->contr->mark = old_marked;
                if (old_marked)
                    applier->contr->mark_count = old_marked->count;
                else
                    applier->contr->mark_count = 0;
                // Note we don't call do_light here -- it is called from the lighter apply
            }
            else {
                 // TODO: Implement directly, since monsters can't mark objects.
            }
        }
        else {
            do_light(lightable, tool->name, applier);
            // We already checked for available mana, so we should be able to just deduct mana outright here and remain above zero.
            if (tool->type == SPELL) {
                // Make sure we handle both magic and prayer costs.
                applier->stats.sp -= tool->stats.sp;
                applier->stats.grace -= tool->stats.grace;
            }
            else if (tool->type == SKILL || tool->type == SKILL_TOOL) {
                applier->stats.sp -= 10;
            }
        }
    }
    else if (applier->type == PLAYER) {
        draw_ext_info_format(NDI_UNIQUE, 0, applier, MSG_TYPE_APPLY, MSG_TYPE_APPLY_FAILURE, "You have no source of heat to light %s.", lightable->name);
    }

    // We're done looking,
    return METHOD_OK;
}
