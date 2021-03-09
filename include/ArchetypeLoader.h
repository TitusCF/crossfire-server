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

#ifndef ARCHETYPELOADER_H
#define ARCHETYPELOADER_H

#include "AssetLoader.h"
#include "Utils.h"
#include "AssetsTracker.h"

class Archetypes;

class ArchetypeLoader : public AssetLoader {
public:
    ArchetypeLoader(Archetypes *archetypes);

    void setTracker(AssetsTracker<archetype> *tracker) {
      m_tracker = tracker;
    };

    virtual bool willLoad(const std::string &filename) override {
        return Utils::endsWith(filename, ".arc");
    }

  virtual void load(BufferReader *reader, const std::string& filename) override;

private:
    Archetypes *m_archetypes;
    AssetsTracker<archetype> *m_tracker;
};

#endif /* ARCHETYPELOADER_H */

