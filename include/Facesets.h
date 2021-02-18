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

#ifndef FACESETS_H
#define FACESETS_H

#include "AssetsCollection.h"
extern "C" {
#include "global.h"
#include "image.h"
}

/**
 * Available facesets for clients.
 */
class Facesets : public AssetsCollection<face_sets> {
public:
  /**
   * Attempt to find a faceset from its identifier.
   * @param id faceset identifier.
   * @return matching faceset, nullptr if no such faceset exists.
   */
  face_sets *findById(int id);

protected:
    virtual face_sets *create(const std::string& prefix);
    virtual void destroy(face_sets *item);
    virtual void replace(face_sets *existing, face_sets *update);
    virtual void added(face_sets *face);
};

#endif /* FACESETS_H */

