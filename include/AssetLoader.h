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

/**
 * Abstract file handler for files during the collect process.
 */
class AssetLoader {
public:
    /** Destructor for inheritance. */
    virtual ~AssetLoader() {};

    /**
     * Whether this instance will process or not the specified file.
     * @param filename full file path.
     * @return true if processFile() should be called, false else.
     */
    virtual bool willProcess(const std::string &filename) = 0;

    /**
     * Actually process a file, called when willProcess() returned true.
     * The file should not be closed.
     * @param file opened file handle, binary mode.
     * @param filename full file path.
     */
    virtual void processFile(FILE *file, const std::string &filename) = 0;
};

#endif /* ASSETLOADER_H */

