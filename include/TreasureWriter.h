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

#ifndef TREASUREWRITER_H
#define TREASUREWRITER_H

extern "C" {
#include "global.h"
}

#include "AssetWriter.h"

class TreasureWriter : public AssetWriter<treasurelist> {
  public:
    virtual void write(const treasurelist *list, StringBuffer *buf);
};

#endif /* TREASUREWRITER_H */

