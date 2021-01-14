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

#include "TreasureWriter.h"

#define W(x, n) { if (item->x) { stringbuffer_append_printf(buf, n "\n", item->x); } }

static void writeItem(const treasure *item, StringBuffer *buf) {
    if (item->item) {
        stringbuffer_append_printf(buf, "arch %s\n", item->item->name);
    }
    W(name, "list %s");
    W(change_arch.name, "change_name %s");
    W(change_arch.title, "change_title %s");
    W(change_arch.slaying, "change_slaying %s");
    if (item->chance != 100) {
        stringbuffer_append_printf(buf, "chance %d\n", item->chance);
    }
    W(nrof, "nrof %d");
    W(magic, "magic %d");
    if (item->next_yes) {
        stringbuffer_append_string(buf, "yes\n");
        writeItem(item->next_yes, buf);
    }
    if (item->next_no) {
        stringbuffer_append_string(buf, "no\n");
        writeItem(item->next_no, buf);
    }
    if (item->next) {
        stringbuffer_append_string(buf, "more\n");
        writeItem(item->next, buf);
    } else {
        stringbuffer_append_string(buf, "end\n");
    }
}

void TreasureWriter::write(const treasurelist *list, StringBuffer *buf) {
    if (list->total_chance == 0) {
        stringbuffer_append_string(buf, "treasure ");
    } else {
        stringbuffer_append_string(buf, "treasureone ");
    }
    stringbuffer_append_string(buf, list->name);
    stringbuffer_append_string(buf, "\n");
    writeItem(list->items, buf);
}
