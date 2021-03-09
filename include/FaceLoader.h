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

#ifndef FACELOADER_H
#define FACELOADER_H

#include "AssetLoader.h"
#include "Utils.h"

class Faces;
class AllAnimations;

class FaceLoader : public AssetLoader {
public:
    FaceLoader(Faces *faces, AllAnimations *animations);

    virtual bool willLoad(const std::string &filename) override {
        return Utils::endsWith(filename, ".face");
    }

    virtual void load(BufferReader *reader, const std::string &filename) override;

private:
    Faces *m_faces;
    AllAnimations *m_animations;

    void loadAnimationBlock(BufferReader *reader, const std::string &full_path, const char *animation_name);
};

#endif /* FACELOADER_H */

