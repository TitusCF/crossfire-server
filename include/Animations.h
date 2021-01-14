#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "AssetsCollection.h"

extern "C" {
#include "global.h"
}

class AllAnimations : public AssetsCollection<Animations> {
public:
    AllAnimations();

protected:
    virtual Animations *create(const std::string& name);
    virtual void destroy(Animations *item);
    virtual void replace(Animations *existing, Animations *update);

    virtual void added(Animations *anim);
};

#endif /* ANIMATIONS_H */
