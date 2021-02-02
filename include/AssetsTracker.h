#ifndef ASSETSTRACKER_H
#define ASSETSTRACKER_H

#include <string>

template<class T>
class AssetsTracker {
  public:
    virtual void assetDefined(const T *asset, const std::string &filename) = 0;
};

#endif /* ASSETSTRACKER_H */

