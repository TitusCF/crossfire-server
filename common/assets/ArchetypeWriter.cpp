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

#include "ArchetypeWriter.h"

static void doWrite(const archetype *arch, StringBuffer *buf) {
    stringbuffer_append_printf(buf, "Object %s\n", arch->name);
    get_ob_diff(buf, &arch->clone, &empty_archetype->clone);
    /** @todo check why not included in get_ob_diff */
    if (arch->clone.anim_speed) {
        stringbuffer_append_printf(buf, "anim_speed %ld\n", arch->clone.anim_speed);
    }
    if (arch->clone.casting_time) {
        stringbuffer_append_printf(buf, "casting_time %d\n", arch->clone.casting_time);
    }
    auto inv = arch->clone.inv;
    while (inv) {
        stringbuffer_append_printf(buf, "arch %s\n", inv->arch->name);
        get_ob_diff(buf, inv, &inv->arch->clone);
        stringbuffer_append_string(buf, "end\n");
        inv = inv->below;
    }
    stringbuffer_append_string(buf, "end\n");
    if (arch->more) {
        stringbuffer_append_string(buf, "more\n");
        doWrite(arch->more, buf);
    }
}

void ArchetypeWriter::write(const archetype *arch, StringBuffer *buf) {
    if (arch->head) {
        return;
    }
    doWrite(arch, buf);
}
