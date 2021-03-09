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

#include "PngLoader.h"
#include "Faces.h"

extern "C" {
#include "string.h"
#include "global.h"
#include "compat.h"
#include "image.h"
}
#include "assets.h"
#include "AssetsManager.h"

PngLoader::PngLoader(Faces *faces) : m_faces(faces) {
}

void PngLoader::load(BufferReader *reader, const std::string &filename) {
    auto slash = strrchr(filename.c_str(), '/');
    char *dup = slash ? strdup_local(strrchr(filename.c_str(), '/') + 1) : strdup(filename.c_str());
    char* split[20];

    size_t count = split_string(dup, split, 20, '.');
    if (count < 4) {
        free(dup);
        return;
    }

    char buf[500];
    buf[0] = '\0';
    /* char *set = split[count - 3]; */
    for (size_t p = 0; p < count - 1; p++) {
        if (p != count - 3) {
            if (p > 0) {
                strcat(buf, ".");
            }
            strcat(buf, split[p]);
        }
    }

    face_sets *set = getManager()->facesets()->get(split[count - 3]);

    const Face *face = m_faces->get(buf);
    if (face->number >= set->allocated) {
        set->faces = static_cast<face_info *>(realloc(set->faces, (face->number + 1) * sizeof(face_info)));
        for (int i = set->allocated; i <= face->number; i++) {
            set->faces[i].data = NULL;
            set->faces[i].datalen = 0;
            set->faces[i].checksum = 0;
        }
        set->allocated = face->number + 1;
    }

    if (set->faces[face->number].data) {
        LOG(llevDebug, "replacing facedata %s by %s\n", face->name, filename.c_str());
        free(set->faces[face->number].data);
    }

    set->faces[face->number].datalen = bufferreader_data_length(reader);
    set->faces[face->number].data = static_cast<uint8_t *>(malloc(set->faces[face->number].datalen));
    if (!set->faces[face->number].data) {
        fatal(OUT_OF_MEMORY);
    }
    memcpy(set->faces[face->number].data, bufferreader_data(reader), set->faces[face->number].datalen);
    set->faces[face->number].checksum = 0;
    for (size_t i = 0; i < set->faces[face->number].datalen; i++) {
        ROTATE_RIGHT(set->faces[face->number].checksum);
        set->faces[face->number].checksum += set->faces[face->number].data[i];
        set->faces[face->number].checksum &= 0xffffffff;
    }

    free(dup);
}
