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

#ifndef ASSETCOLLECTOR_H
#define ASSETCOLLECTOR_H

#include <vector>
#include <string>
#include "AssetLoader.h"

/**
 * Assets collector, recursing in directories and calling loaders on found files.
 */
class AssetCollector {
public:
    /**
     * Constructor.
     */
    AssetCollector();
    /**
     * Destructor.
     */
    virtual ~AssetCollector();

    /**
     * Register a loader to be called on found files.
     * Ownership of the loader is taken by the AssetCollector.
     * @param loader instance to register.
     */
    void addLoader(AssetLoader *loader) { m_loaders.push_back(loader); }

    /**
     * Recurse in the specified directory, finding all files and calling loaders to process them.
     * @param directory starting path, with or without a trailing slash.
     */
    void collect(const std::string& directory);

protected:
    std::vector<AssetLoader *> m_loaders;   /**< Registered loaders. */

    /**
     * Process a list of files.
     * @param files full path of files to process.
     */
    void processFiles(const std::vector<std::string> &files);

    void processTar(const std::string &file);
};

#endif /* ASSETCOLLECTOR_H */

