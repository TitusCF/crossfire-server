#ifndef TREASURES_H
#define TREASURES_H

#include "AssetsCollection.h"

extern "C" {
#include "treasure.h"
}

class Treasures : public AssetsCollection<treasurelist> {
protected:
    virtual treasurelist *create(const std::string& name);
    virtual void destroy(treasurelist *item);
    virtual void replace(treasurelist *existing, treasurelist *update);
};

#endif /* TREASURES_H */

