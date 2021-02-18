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

#ifndef ASSETWRITER_H
#define ASSETWRITER_H

extern "C" {
#include "global.h"
}

/**
 * Abstract writer of an asset to a StringBuffer.
 */
template<class T>
class AssetWriter {
  public:
    /** Destructor in case inherited classes need it. */
    virtual ~AssetWriter() { }
    /**
     * Write the specified asset to the StringBuffer.
     * @param asset asset to write.
     * @param buf where to write.
     */
    virtual void write(const T *asset, StringBuffer *buf) = 0;
};

#endif /* ASSETWRITER_H */
