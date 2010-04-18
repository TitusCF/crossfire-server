#ifndef CRE_WRAPPER_OBJECT_h
#define CRE_WRAPPER_OBJECT_h

extern "C" {
#include "global.h"
}

#include <QObject>

class CREWrapperObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name);
    Q_PROPERTY(int type READ type);
    Q_PROPERTY(int level READ level);
    Q_PROPERTY(bool isMonster READ isMonster);
    Q_PROPERTY(bool isAlive READ isAlive);
    Q_PROPERTY(qint64 experience READ experience);
    Q_PROPERTY(quint32 attacktype READ attacktype);

    public:
        CREWrapperObject();

        void setObject(const object* obj);

        QString name() const;
        int type() const;
        int level() const;
        bool isMonster() const;
        bool isAlive() const;
        qint64 experience() const;
        quint32 attacktype() const;

    protected:
        const object* myObject;
};

#endif // CRE_WRAPPER_OBJECT_h
