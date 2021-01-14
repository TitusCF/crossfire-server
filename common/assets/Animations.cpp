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

#include "Animations.h"
extern "C" {
#include "global.h"
}
#include "assets.h"

AllAnimations::AllAnimations() {
    get("###none");
    m_undefined.clear();
}

Animations *AllAnimations::create(const std::string& name) {
    Animations *anim = (Animations *)calloc(1, sizeof(Animations));
    anim->name = add_string(name.c_str());
    anim->num_animations = 1;
    return anim;
}

void AllAnimations::destroy(Animations *item) {
    free_string(item->name);
    free(item->faces);
    free(item);
}
void AllAnimations::replace(Animations *existing, Animations *update) {
    free(existing->faces);
    existing->faces = update->faces;
    update->faces = nullptr;
    existing->facings = update->facings;
    existing->num_animations = update->num_animations;
    free_string(update->name);
    free(update);
}

void AllAnimations::added(Animations *anim) {
    anim->num = m_assets.size() - 1;
}
