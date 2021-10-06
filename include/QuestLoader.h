/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef QUESTLOADER_H
#define QUESTLOADER_H

#include "AssetLoader.h"
#include "quest.h"
#include "Utils.h"

class Quests;
class Faces;
class AssetsTracker;

class QuestLoader : public AssetLoader {
public:
    QuestLoader(Quests *quests, Faces *faces, AssetsTracker *tracker);
    virtual ~QuestLoader() { }

    virtual bool willLoad(const std::string &filename) override {
      return Utils::endsWith(filename, ".quests");
    }

    virtual void load(BufferReader *reader, const std::string &filename) override;

private:
    Quests *quests;
    Faces *faces;
    AssetsTracker *tracker;
};

#endif /* QUESTLOADER_H */

