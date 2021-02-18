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

#ifndef PNGLOADER_H
#define PNGLOADER_H

#include "AssetLoader.h"
#include "Utils.h"

class Faces;

class PngLoader : public AssetLoader {
public:
  PngLoader(Faces *faces);

  virtual bool willProcess(const std::string &filename) {
    return Utils::endsWith(filename, ".png");
  }

  virtual void processFile(FILE *file, const std::string &filename);

  void processData(uint8_t *data, size_t len, const std::string &filename);
protected:
    Faces *m_faces;
};

#endif /* PNGLOADER_H */

