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
    Q_PROPERTY(int level READ level);
    Q_PROPERTY(bool isMonster READ isMonster);
    Q_PROPERTY(bool isAlive READ isAlive);

    public:
        CREWrapperObject();

        void setObject(const object* obj);

        int type() const;
        int level() const;
        bool isMonster() const;
        bool isAlive() const;

    protected:
        const object* myObject;
};

#endif // CRE_WRAPPER_OBJECT_h
