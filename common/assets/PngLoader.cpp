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

void PngLoader::processFile(FILE *file, const std::string &filename) {
    fseek(file, 0L, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *data = static_cast<uint8_t *>(malloc(len));
    if (!data) {
        LOG(llevError, "Failed to allocate %d bytes of memory!", len);
        fatal(SEE_LAST_ERROR);
    }
    size_t actual = fread(data, 1, len, file);
    if (actual != len) {
        LOG(llevError, "Did not read desired amount of data, wanted %d, got %d for file %s\n", len, actual, filename.c_str());
        fatal(SEE_LAST_ERROR);
    }

    processData(data, len, filename);
}

void PngLoader::processData(uint8_t *data, size_t len, const std::string &filename) {
    auto slash = strrchr(filename.c_str(), '/');
    char *dup = slash ? strdup_local(strrchr(filename.c_str(), '/') + 1) : strdup(filename.c_str());
    char* split[20];

    size_t count = split_string(dup, split, 20, '.');
    if (count < 4) {
        free(dup);
        free(data);
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

    set->faces[face->number].datalen = len;
    set->faces[face->number].data = data;
    set->faces[face->number].checksum = 0;
    for (size_t i = 0; i < len; i++) {
        ROTATE_RIGHT(set->faces[face->number].checksum);
        set->faces[face->number].checksum += set->faces[face->number].data[i];
        set->faces[face->number].checksum &= 0xffffffff;
    }

    free(dup);
}
