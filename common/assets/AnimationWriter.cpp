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

#include "AnimationWriter.h"

void AnimationWriter::write(const Animations *anim, StringBuffer *buf) {
    stringbuffer_append_printf(buf, "animation %s\n", anim->name);
    if (anim->facings != 0) {
        stringbuffer_append_printf(buf, "facings %d\n", anim->facings);
    }
    for (uint8_t face = 0; face < anim->num_animations; face++) {
        stringbuffer_append_string(buf, anim->faces[face]->name);
        stringbuffer_append_string(buf, "\n");
    }
    stringbuffer_append_string(buf, "mina\n");
}
