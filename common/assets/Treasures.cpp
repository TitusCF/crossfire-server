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

#include "Treasures.h"
extern "C" {
#include "global.h"
}

treasurelist *Treasures::create(const std::string& name) {
    treasurelist *tl = (treasurelist *)calloc(1, sizeof(treasurelist));
    tl->name = add_string(name.c_str());
    return tl;
}

/**
 * Frees a treasure, including its yes, no and next items.
 *
 * @param t
 * treasure to free. Pointer is free()d too, so becomes invalid.
 */
static void free_treasurestruct(treasure *t) {
    if (t->next)
        free_treasurestruct(t->next);
    if (t->next_yes)
        free_treasurestruct(t->next_yes);
    if (t->next_no)
        free_treasurestruct(t->next_no);
    free(t);
}

void Treasures::destroy(treasurelist *item) {
    if (item->name)
        free_string(item->name);
    if (item->items)
        free_treasurestruct(item->items);
    free(item);
}

void Treasures::replace(treasurelist *existing, treasurelist *update) {
    if (existing->items) {
        free_treasurestruct(existing->items);
    }
    existing->items = update->items;
    existing->total_chance = update->total_chance;
    update->items = NULL;
    destroy(update);
}
