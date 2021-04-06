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

#ifndef FACES_H
#define FACES_H

#include "AssetsCollection.h"
extern "C" {
#include "global.h"
}

class Faces : public AssetsCollection<Face> {
public:
  Faces();

  const Face *findById(uint16_t id);

  int checksum() const { return m_checksum; }

protected:
    int m_checksum;

    virtual Face *create(const std::string& name);
    virtual void destroy(Face *item);
    virtual void replace(Face *existing, Face *update);
    virtual void added(Face *face);
};

#endif /* FACES_H */
