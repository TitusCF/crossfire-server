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

