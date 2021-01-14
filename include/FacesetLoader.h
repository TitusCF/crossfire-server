#ifndef FACESETLOADER_H
#define FACESETLOADER_H

#include "AssetLoader.h"
#include "Utils.h"

class FacesetLoader : public AssetLoader {
public:
  FacesetLoader();

  virtual bool willProcess(const std::string &filename) {
    return Utils::endsWith(filename, "/image_info");
  }

  virtual void processFile(FILE *file, const std::string &filename);
};

#endif /* FACESETLOADER_H */

