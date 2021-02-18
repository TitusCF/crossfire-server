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

#ifndef TREASURES_H
#define TREASURES_H

#include "AssetsCollection.h"

extern "C" {
#include "treasure.h"
}

class Treasures : public AssetsCollection<treasurelist> {
protected:
    virtual treasurelist *create(const std::string& name);
    virtual void destroy(treasurelist *item);
    virtual void replace(treasurelist *existing, treasurelist *update);
};

#endif /* TREASURES_H */

