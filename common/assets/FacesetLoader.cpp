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

#include "FacesetLoader.h"

extern "C" {
#include "global.h"
#include "image.h"
#include "string.h"
}
#include "assets.h"
#include "AssetsManager.h"

FacesetLoader::FacesetLoader() {
}

void FacesetLoader::load(BufferReader *reader, const std::string& filename) {
    char *buf;
    char *cps[7+1];

    while ((buf = bufferreader_next_line(reader)) != NULL) {
        if (buf[0] == '#' || buf[0] == '\0')
            continue;
        if (split_string(buf, cps, sizeof(cps)/sizeof(*cps), ':') != 6)
            LOG(llevError, "Bad line in image_info file %s, ignoring line:\n  %s", filename.c_str(), buf);
        else {
            face_sets *added = static_cast<face_sets *>(calloc(1, sizeof(face_sets)));
            added->prefix = strdup_local(cps[0]);
            added->fullname = strdup_local(cps[1]);
            if (cps[2][0] != '\0') {
                added->fallback = getManager()->facesets()->get(cps[2]);
            }
            added->size = strdup_local(cps[3]);
            added->extension = strdup_local(cps[4]);
            added->comment = strdup_local(cps[5]);

            getManager()->facesets()->define(added->prefix, added);
        }
    }
}
