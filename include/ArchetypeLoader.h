#ifndef ARCHETYPELOADER_H
#define ARCHETYPELOADER_H

#include "AssetLoader.h"
#include "Utils.h"

class Archetypes;

class ArchetypeLoader : public AssetLoader {
public:
    ArchetypeLoader(Archetypes *archetypes);

    virtual bool willProcess(const std::string &filename) {
        return Utils::endsWith(filename, ".arc");
    }

  virtual void processFile(FILE *file, const std::string& filename);

private:
    Archetypes *m_archetypes;

};

#endif /* ARCHETYPELOADER_H */

