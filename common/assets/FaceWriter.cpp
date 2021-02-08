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

#include "FaceWriter.h"

void FaceWriter::write(const Face *face, StringBuffer *buf) {
    /* No need to write faces with no specific information */
    if (face->visibility == 0 && face->magicmap == 0) {
        if (face->smoothface) {
            stringbuffer_append_printf(buf, "smoothface %s %s\n", face->name, face->smoothface->name);
        }
        return;
    }
    stringbuffer_append_printf(buf, "face %s\n", face->name);
    if (face->visibility) {
        stringbuffer_append_printf(buf, "visibility %d\n", face->visibility);
    }
    if (face->magicmap & FACE_FLOOR) {
        stringbuffer_append_string(buf, "is_floor 1\n");
    }
    if (face->magicmap & (~FACE_FLOOR)) {
        stringbuffer_append_printf(buf, "magicmap %s\n", get_colorname(face->magicmap & (~FACE_FLOOR)));
    }
    if (face->smoothface) {
        stringbuffer_append_printf(buf, "smoothface %s\n", face->smoothface->name);
    }
    stringbuffer_append_string(buf, "end\n");
}
