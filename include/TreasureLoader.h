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

#ifndef TREASURELOADER_H
#define TREASURELOADER_H

#include "AssetLoader.h"
#include "Utils.h"
class Treasures;
class Archetypes;

extern "C" {
#include "global.h"
#include "treasure.h"
}


class TreasureLoader : public AssetLoader {
public:
  TreasureLoader(Treasures *treasures, Archetypes *archetypes);

  virtual bool willProcess(const std::string &filename) {
      return Utils::endsWith(filename, ".trs");
  }

  virtual void processFile(FILE *file, const std::string &filename);

private:
    Treasures *m_treasures;
    Archetypes *m_archetypes;

    treasure *loadTreasure(FILE *fp, int *line);
};

#endif /* TREASURELOADER_H */

