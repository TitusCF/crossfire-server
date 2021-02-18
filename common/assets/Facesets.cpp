/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020-2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "Facesets.h"

extern "C" {
#include "string.h"
}

face_sets *Facesets::findById(int id) {
    auto found = std::find_if(m_assets.begin(), m_assets.end(), [&id] (const auto fs) {
        return fs.second->id == id;
    });
    return found == m_assets.end() ? nullptr : found->second;
}

face_sets *Facesets::create(const std::string& prefix) {
    auto fs = static_cast<face_sets *>(calloc(1, sizeof(face_sets)));
    fs->prefix = strdup_local(prefix.c_str());
    return fs;
}

void Facesets::destroy(face_sets *item) {
    free(item->prefix);
    free(item->fullname);
    free(item->size);
    free(item->extension);
    free(item->comment);
    free(item->faces);
    free(item);
}

void Facesets::replace(face_sets *existing, face_sets *update) {
    auto id = existing->id;
    free(existing->prefix);
    free(existing->fullname);
    free(existing->size);
    free(existing->extension);
    free(existing->comment);
    if (update->allocated) {
        free(existing->faces);
    } else {
        free(update->faces);
        update->faces = existing->faces;
        update->allocated = existing->allocated;
    }
    *existing = *update;
    existing->id = id;
    free(update);
}

void Facesets::added(face_sets *face) {
    face->id = m_assets.size() - 1;
}
