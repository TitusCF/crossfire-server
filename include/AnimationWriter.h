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

#ifndef ANIMATIONWRITER_H
#define ANIMATIONWRITER_H

extern "C" {
#include "global.h"
}

#include "AssetWriter.h"

class AnimationWriter : public AssetWriter<Animations> {
  public:
    virtual void write(const Animations *anim, StringBuffer *buf);
};

#endif /* ANIMATIONWRITER_H */

