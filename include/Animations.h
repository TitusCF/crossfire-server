/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020-2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "AssetsCollection.h"

extern "C" {
#include "global.h"
}

class AllAnimations : public AssetsCollection<Animations> {
public:
    AllAnimations();

protected:
    virtual Animations *create(const std::string& name);
    virtual void destroy(Animations *item);
    virtual void replace(Animations *existing, Animations *update);

    virtual void added(Animations *anim);
};

#endif /* ANIMATIONS_H */
