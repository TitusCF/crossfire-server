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

#include "AssetsManager.h"

#include <functional>
extern "C" {
#include "image.h"
}

AssetsManager::AssetsManager() {
    m_listener = std::bind(&AssetsManager::archetypeUpdated, this, std::placeholders::_1, std::placeholders::_2);
    m_archetypes.setReplaceListener(m_listener);

    auto bug = m_animations.get("###none");
    bug->faces = (const Face **)malloc(sizeof(Face*));
    bug->faces[0] = m_faces.get("bug.111");
    bug->num_animations = 1;
}

AssetsManager::~AssetsManager() {
    m_archetypes.clearReplaceListener();
    m_treasures.clear();
    m_animations.clear();
    m_archetypes.clear();
    m_faces.clear();
    m_messages.clear();
    m_facesets.clear();
}

void AssetsManager::archetypeUpdated(archetype *, archetype *update) {
    auto artifacts = first_artifactlist;
    while (artifacts != NULL) {
        auto items = artifacts->items;
        while (items != NULL) {
            if (items->item) {
                Archetypes::recursive_update(items->item->inv, update);
            }
            items = items->next;
        }
        artifacts = artifacts->next;
    }
}

static int dump(const std::set<std::string>& items, const char *name) {
    if (!items.empty()) {
        LOG(llevError, "Referenced %s which are not defined:\n", name);
        for (const auto& item : items) {
            LOG(llevError, " - %s\n", item.c_str());
        }
    }
    return items.size();
}

size_t AssetsManager::dumpUndefined() {
    size_t count = 0;
    count += dump(m_archetypes.undefined(), "archetypes");
    count += dump(m_treasures.undefined(), "treasures");
    // count += dump(m_faces.undefined(), "faces");    /** Faces are actually not defined usually, just referenced. */
    count += dump(m_animations.undefined(), "animations");
    count += dump(m_facesets.undefined(), "facesets");
    return count;
}