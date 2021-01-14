#ifndef FACESETWRITER_H
#define FACESETWRITER_H

extern "C" {
#include "global.h"
#include "image.h"
}

#include "AssetWriter.h"

class FacesetWriter : public AssetWriter<face_sets> {
  public:
    virtual void write(const face_sets *set, StringBuffer *buf);
};

#endif /* FACESETWRITER_H */

