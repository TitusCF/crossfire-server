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

#include "FacesetWriter.h"

void FacesetWriter::write(const face_sets *set, StringBuffer *buf) {
    if (set->prefix) {
        stringbuffer_append_printf(buf, "%s:%s:%s:%s:%s:%s\n",
            set->prefix, set->fullname, set->fallback ? set->fallback->prefix : "", set->size, set->extension, set->comment);
    }
}
