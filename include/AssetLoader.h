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

#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <string>
#include "AssetsTracker.h"
#include "bufferreader.h"

/**
 * Abstract buffer handler, processing a BufferReader.
 * Inherited classes must implement load().
 */
class AssetLoader {
public:

    /** Virtual destructor for inheritance. */
    virtual ~AssetLoader() { };

    /**
     * Whether this instance will process or not the specified file.
     * @param filename full file path.
     * @return true if processFile() should be called, false else.
     */
    virtual bool willLoad(const std::string &filename) = 0;

    /**
     * Load assets from the specified reader. Must be implemented.
     * @param reader reader.
     * @param filename full filename.
     */
    virtual void load(BufferReader *reader, const std::string &filename) = 0;
};
#endif /* ASSETLOADER_H */
