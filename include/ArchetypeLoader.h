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

    virtual bool willProcess(const std::string &filename) {
        return Utils::endsWith(filename, ".arc");
    }

  virtual void processFile(FILE *file, const std::string& filename);

private:
    Archetypes *m_archetypes;
    AssetsTracker<archetype> *m_tracker;
};

#endif /* ARCHETYPELOADER_H */

