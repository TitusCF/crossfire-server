#ifndef FORMULAEWRITER_H
#define FORMULAEWRITER_H

extern "C" {
#include "global.h"
}

#include "AssetWriter.h"

class FormulaeWriter : public AssetWriter<recipelist> {
  public:
    virtual void write(const recipelist *list, StringBuffer *buf);
};

#endif /* FORMULAEWRITER_H */

