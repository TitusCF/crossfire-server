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

#include "MessageWriter.h"

void MessageWriter::write(const GeneralMessage *message, StringBuffer *buf) {
    if (message->identifier[0] == '\n') {
        stringbuffer_append_string(buf, "MSG\n");
    } else {
        stringbuffer_append_printf(buf, "MSG %s\n", message->identifier);
    }
    if (message->title) {
        stringbuffer_append_printf(buf, "TITLE %s\n", message->title);
    }
    if (message->chance) {
        stringbuffer_append_printf(buf, "CHANCE %d\n", message->chance);
    }
    if (message->quest_code) {
        stringbuffer_append_printf(buf, "QUEST %s\n", message->quest_code);
    }
    if (message->face) {
        stringbuffer_append_printf(buf, "FACE %s\n", message->face->name);
    }
    if (message->message) {
        stringbuffer_append_string(buf, "TEXT\n");
        stringbuffer_append_string(buf, message->message);
    }
    stringbuffer_append_string(buf, "ENDMSG\n");
}
