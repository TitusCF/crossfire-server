#ifndef CRE_WRAPPER_OBJECT_h
#define CRE_WRAPPER_OBJECT_h

extern "C" {
#include "global.h"
}

#include <QObject>

class CREWrapperObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int type READ type);

    public:
        CREWrapperObject();

        void setObject(const object* obj);

        int type() const;

    protected:
        const object* myObject;
};

#endif // CRE_WRAPPER_OBJECT_h
