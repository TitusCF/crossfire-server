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

#include "Faces.h"
#include "string.h"

Faces::Faces() : m_checksum(0) {
    blank_face = get(BLANK_FACE_NAME);
    empty_face = get(EMPTY_FACE_NAME);
    smooth_face = get(SMOOTH_FACE_NAME);
}

Face *Faces::create(const std::string& name) {
    Face *face = (Face *)calloc(1, sizeof(Face));
    face->name = add_string(name.c_str());
    face->number = m_assets.size();
    return face;
}

void Faces::destroy(Face *item) {
    free_string(item->name);
    free(item);
}

void Faces::replace(Face *existing, Face *update) {
    existing->visibility = update->visibility;
    existing->magicmap = update->magicmap;
    if (update->smoothface) {
        existing->smoothface = update->smoothface;
    }
    destroy(update);
}

const Face *Faces::findByIndex(int index) {
    auto face = m_assets.begin();
    while (index > 0 && face != m_assets.end()) {
        index--;
        face++;
    }
    return face == m_assets.end() ? NULL : face->second;
}

const Face *Faces::findById(uint16_t id) {
    auto face = std::find_if(m_assets.begin(), m_assets.end(), [&id] (auto& face) {
        return face.second->number == id;
    });
    return face == m_assets.end() ? NULL : face->second;
}

void Faces::added(Face *face) {
    face->number = m_assets.size() - 1;

    ROTATE_RIGHT(m_checksum);
    m_checksum += face->number & 0xff;
    m_checksum &= 0xffffffff;

    ROTATE_RIGHT(m_checksum);
    m_checksum += (face->number >> 8) & 0xff;
    m_checksum &= 0xffffffff;
    for (size_t l = 0; l < strlen(face->name); l++) {
        ROTATE_RIGHT(m_checksum);
        m_checksum += face->name[l];
        m_checksum &= 0xffffffff;
    }
}
