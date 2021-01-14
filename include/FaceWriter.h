#ifndef FACEWRITER_H
#define FACEWRITER_H

extern "C" {
#include "global.h"
}

#include "AssetWriter.h"

class FaceWriter : public AssetWriter<Face> {
  public:
    virtual void write(const Face *face, StringBuffer *buf);
};

#endif /* FACEWRITER_H */

