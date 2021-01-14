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

#include "Utils.h"
#include <vector>

bool Utils::endsWith(const std::string& str, const std::string& with) {
    return
            str.length() >= with.length() &&
            str.rfind(with) == str.length() - with.length();
}

void Utils::writeLinkedChar(const linked_char* list, StringBuffer* buf) {
    std::vector<std::string> items;
    while (list) {
        items.push_back(list->name);
        list = list->next;
    }

    bool first = true;
    for (auto a = items.rbegin(); a != items.rend(); a++) {
        stringbuffer_append_printf(buf, "%c%s", first ? ' ' : ',', (*a).c_str());
        first = false;
    }
}

void Utils::writeStringArray(char **items, size_t count, StringBuffer *buf) {
    bool first = true;
    for (size_t item = 0; item < count; item++) {
        stringbuffer_append_printf(buf, "%c%s", first ? ' ' : ',', items[item]);
        first = false;
    }
}
