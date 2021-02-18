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

#ifndef ARTIFACTWRITER_H
#define ARTIFACTWRITER_H

extern "C" {
#include "global.h"
}

#include "AssetWriter.h"

class ArtifactWriter : public AssetWriter<artifactlist> {
  public:
    virtual void write(const artifactlist *list, StringBuffer *buf);
};

#endif /* ARTIFACTWRITER_H */
