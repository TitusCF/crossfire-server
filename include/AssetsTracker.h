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

/**
 * Interface to be informed of where an asset is defined.
 */
template<class T>
class AssetsTracker {
  public:
    /**
     * Function called when an asset is defined in a file.
     * It may be called multiple times with the same asset.
     * @param asset asset which is defined.
     * @param filename full pathname of where the asset was defined.
     */
    virtual void assetDefined(const T *asset, const std::string &filename) = 0;
};

#endif /* ASSETSTRACKER_H */
