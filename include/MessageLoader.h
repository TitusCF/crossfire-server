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

#ifndef MESSAGELOADER_H
#define MESSAGELOADER_H

#include "AssetLoader.h"
#include "Utils.h"

class Messages;

class MessageLoader : public AssetLoader {
public:
    MessageLoader(Messages *messages);

    virtual bool willLoad(const std::string &filename) override {
        return Utils::endsWith(filename, "/messages");
    }

    virtual void load(BufferReader *reader, const std::string &filename) override;

private:
    Messages *m_messages;
};

#endif /* MESSAGELOADER_H */

