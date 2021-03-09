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

#ifndef WRAPPERLOADER_H
#define WRAPPERLOADER_H

#include "AssetLoader.h"
#include "Utils.h"
#include <functional>
#include <string>
extern "C" {
#include "global.h"
}

/**
 * Loader calling a function for files ending with a specific string.
 */
class WrapperLoader : public AssetLoader {
public:
    /** Wrapper to the function to call for each file. */
    typedef std::function<void(BufferReader *, const char *)> wrapped;

    /**
     * Standard construction.
     * @param name suffix to match to load the file.
     * @param fct function to call to load the file.
     */
    WrapperLoader(const std::string& name, wrapped fct) : m_name(name), m_function(fct) {
    };

    virtual bool willLoad(const std::string &filename) override {
        return Utils::endsWith(filename, m_name);
    }

    virtual void load(BufferReader *reader, const std::string &filename) override {
      m_function(reader, filename.c_str());
    }

private:
    std::string m_name; /**< File name. */
    wrapped m_function; /**< Function to call. */
};

#endif /* WRAPPERLOADER_H */

