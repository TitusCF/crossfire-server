/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef QUESTWRITER_H
#define QUESTWRITER_H

#include "AssetWriter.h"
#include "quest.h"

class QuestWriter : public AssetWriter<quest_definition> {
public:
    virtual void write(const quest_definition *quest, StringBuffer *buf);
};

#endif /* QUESTWRITER_H */
