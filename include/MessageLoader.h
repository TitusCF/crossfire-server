#ifndef MESSAGELOADER_H
#define MESSAGELOADER_H

#include "AssetLoader.h"
#include "Utils.h"

class Messages;

class MessageLoader : public AssetLoader {
public:
    MessageLoader(Messages *messages);

    virtual bool willProcess(const std::string &filename) {
        return Utils::endsWith(filename, "/messages");
    }

    virtual void processFile(FILE *file, const std::string &filename);

private:
    Messages *m_messages;
};

#endif /* MESSAGELOADER_H */

