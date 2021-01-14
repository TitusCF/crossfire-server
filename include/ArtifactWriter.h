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
