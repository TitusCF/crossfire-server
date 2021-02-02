/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "Archetypes.h"

#include <string.h>

archetype *Archetypes::create(const std::string& name) {
    auto arch = get_archetype_struct();
    CLEAR_FLAG(&arch->clone, FLAG_REMOVED);
    arch->name = add_string(name.c_str());
    return arch;
}

void Archetypes::destroy(archetype *item) {
    object_free_inventory(&item->clone);
    free_arch(item);
}

void Archetypes::recursive_update(object *item, archetype *updated) {
    if (!item)
        return;
    if (item->arch->name == updated->name) {
        object_free_inventory(item);
        auto diff = stringbuffer_new();
        get_ob_diff(diff, item, &item->arch->clone);
        auto arch = item->arch;
        object_copy_with_inv(&updated->clone, item);
        item->arch = arch;
        auto finished = stringbuffer_finish(diff);
        set_variable(item, finished);
        free(finished);
    }
    recursive_update(item->below, updated);
    recursive_update(item->inv, updated);
}

/**
 * Recursively mark all items in inventory as not removed.
 * @param item what to start from.
 */
static void mark_inv_not_removed(object *item) {
    auto inv = item->inv;
    while (inv) {
        CLEAR_FLAG(inv, FLAG_REMOVED);
        mark_inv_not_removed(inv);
        inv = inv->below;
    }
}

void Archetypes::replace(archetype *existing, archetype *update) {
    for (auto arch : m_assets) {
        recursive_update(arch.second->clone.inv, update);
    }
    if (m_updateListener) {
        m_updateListener(existing, update);
    }

    // We need to mark items as not removed before calling object_free_inventory().
    // The flag is set at object initialisation but never cleared since an
    // archetype's object is never inserted anywhere.
    mark_inv_not_removed(&existing->clone);
    object_free_inventory(&existing->clone);
    object_copy_with_inv(&update->clone, &existing->clone);
    existing->clone.arch = existing;
    mark_inv_not_removed(&update->clone);
    object_free_inventory(&update->clone);
    free_arch(update);
}

archetype *Archetypes::findByObjectName(const char *name) {
    if (name == NULL)
        return (archetype *)NULL;
    auto tmp = find_string(name);
    if (!tmp)
        return NULL;

    for (auto arch : m_assets) {
        if (arch.second->clone.name == tmp) {
            return arch.second;
        }
    }
    return NULL;
}

archetype *Archetypes::findByObjectTypeName(int type, const char *name) {
    if (name == NULL)
        return NULL;

    for (auto arch : m_assets) {
        if (arch.second->clone.type == type && strcmp(arch.second->clone.name, name) == 0)
            return arch.second;
    }

    return NULL;
}

archetype *Archetypes::findBySkillNameAndType(const char *skill, int type) {
    if (skill == NULL)
        return NULL;

    for (auto arch : m_assets) {
        if (((type == -1) || (type == arch.second->clone.type))
        && (arch.second->clone.skill) && (!strcmp(arch.second->clone.skill, skill)))
            return arch.second;
    }
    return NULL;
}

archetype *Archetypes::findByTypeSubtype(int type, int subtype) {
    for (auto arch : m_assets) {
        if (((type == -1) || (type == arch.second->clone.type))
        && (subtype == -1 || subtype == arch.second->clone.subtype))
            return arch.second;
    }
    return NULL;
}
