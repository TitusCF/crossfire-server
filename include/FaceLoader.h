#ifndef FACELOADER_H
#define FACELOADER_H

#include "AssetLoader.h"
#include "Utils.h"

class Faces;
class AllAnimations;

class FaceLoader : public AssetLoader {
public:
    FaceLoader(Faces *faces, AllAnimations *animations);

    virtual bool willProcess(const std::string &filename) {
        return Utils::endsWith(filename, ".face");
    }

    virtual void processFile(FILE *file, const std::string &filename);

private:
    Faces *m_faces;
    AllAnimations *m_animations;

    void loadAnimationBlock(FILE *file, const std::string &full_path, const char *animation_name);
};

#endif /* FACELOADER_H */

