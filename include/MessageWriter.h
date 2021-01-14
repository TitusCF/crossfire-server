#ifndef MESSAGEWRITER_H
#define MESSAGEWRITER_H

extern "C" {
#include "global.h"
}

#include "AssetWriter.h"

class MessageWriter : public AssetWriter<GeneralMessage> {
  public:
    virtual void write(const GeneralMessage *message, StringBuffer *buf);
};

#endif /* MESSAGEWRITER_H */

