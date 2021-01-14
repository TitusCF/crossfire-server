#ifndef ARCHETYPEWRITER_H
#define ARCHETYPEWRITER_H

extern "C" {
#include "global.h"
}

#include "AssetWriter.h"

class ArchetypeWriter : public AssetWriter<archetype> {
  public:
    virtual void write(const archetype *arch, StringBuffer *buf);
};

#endif /* ARCHETYPEWRITER_H */

