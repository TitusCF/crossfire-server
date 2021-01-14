#ifndef TREASUREWRITER_H
#define TREASUREWRITER_H

extern "C" {
#include "global.h"
}

#include "AssetWriter.h"

class TreasureWriter : public AssetWriter<treasurelist> {
  public:
    virtual void write(const treasurelist *list, StringBuffer *buf);
};

#endif /* TREASUREWRITER_H */

