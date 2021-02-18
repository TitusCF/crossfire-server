/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020-2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef ASSETSTRACKER_H
#define ASSETSTRACKER_H

#include <string>

template<class T>
class AssetsTracker {
  public:
    virtual void assetDefined(const T *asset, const std::string &filename) = 0;
};

#endif /* ASSETSTRACKER_H */

