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

#ifndef FACESETLOADER_H
#define FACESETLOADER_H

#include "AssetLoader.h"
#include "Utils.h"

class FacesetLoader : public AssetLoader {
public:
  FacesetLoader();

  virtual bool willLoad(const std::string &filename) override {
    return Utils::endsWith(filename, "/image_info");
  }

  virtual void load(BufferReader *reader, const std::string &filename) override;
};

#endif /* FACESETLOADER_H */

