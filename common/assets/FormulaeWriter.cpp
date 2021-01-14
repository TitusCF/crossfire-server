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

#include "FormulaeWriter.h"
#include "Utils.h"

#define DO(v, s) { if (recipe->v) { stringbuffer_append_printf(buf, s "\n", recipe->v); } }

static void writeRecipe(const recipe *recipe, StringBuffer *buf) {
    if (!recipe) {
        return;
    }

    stringbuffer_append_printf(buf, "Object %s\n", recipe->title);
    if (recipe->arch_names) {
        stringbuffer_append_string(buf, "arch");
        Utils::writeStringArray(recipe->arch_name, recipe->arch_names, buf);
        stringbuffer_append_string(buf, "\n");
    }
    DO(keycode, "keycode %s");
    DO(transmute, "trans %d");
    DO(yield, "yield %d");
    DO(chance, "chance %d");
    DO(exp, "exp %d");
    DO(diff, "diff %d");
    stringbuffer_append_string(buf, "ingred");
    Utils::writeLinkedChar(recipe->ingred, buf);
    stringbuffer_append_string(buf, "\n");
    DO(skill, "skill %s");
    DO(cauldron, "cauldron %s");
    DO(failure_arch, "failure_arch %s");
    DO(failure_message, "failure_message %s");
    DO(min_level, "min_level %d");
    DO(is_combination, "combination %d");
    if (recipe->tool_size) {
        stringbuffer_append_string(buf, "tool");
        Utils::writeStringArray(recipe->tool, recipe->tool_size, buf);
        stringbuffer_append_string(buf, "\n");
    }
    writeRecipe(recipe->next, buf);
}

void FormulaeWriter::write(const recipelist *list, StringBuffer *buf) {
    writeRecipe(list->items, buf);
}
