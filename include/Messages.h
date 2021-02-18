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

#ifndef MESSAGES_H
#define MESSAGES_H

#include "AssetsCollection.h"

extern "C" {
#include "global.h"
#include "compat.h"
#include "book.h"
}

class Messages : public AssetsCollection<GeneralMessage> {

public:
    Messages();

    GeneralMessage *random();

protected:
    int m_totalChance;

    virtual GeneralMessage *create(const std::string& name);
    virtual void destroy(GeneralMessage *item);
    virtual void replace(GeneralMessage *existing, GeneralMessage *update);

    virtual void added(GeneralMessage *asset);
};

#endif /* MESSAGES_H */

