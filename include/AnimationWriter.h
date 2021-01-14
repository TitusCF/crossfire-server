#ifndef ANIMATIONWRITER_H
#define ANIMATIONWRITER_H

extern "C" {
#include "global.h"
}

#include "AssetWriter.h"

class AnimationWriter : public AssetWriter<Animations> {
  public:
    virtual void write(const Animations *anim, StringBuffer *buf);
};

#endif /* ANIMATIONWRITER_H */

