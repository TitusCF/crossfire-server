#ifndef FACES_H
#define FACES_H

#include "AssetsCollection.h"
extern "C" {
#include "global.h"
}

class Faces : public AssetsCollection<Face> {
public:
  Faces();

  const Face *findByIndex(int index);
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
