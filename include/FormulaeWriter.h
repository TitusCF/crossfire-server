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

#ifndef FORMULAEWRITER_H
#define FORMULAEWRITER_H

extern "C" {
#include "global.h"
}

#include "AssetWriter.h"

class FormulaeWriter : public AssetWriter<recipelist> {
  public:
    virtual void write(const recipelist *list, StringBuffer *buf);
};

#endif /* FORMULAEWRITER_H */

