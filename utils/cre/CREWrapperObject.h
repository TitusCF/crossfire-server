#ifndef CRE_WRAPPER_OBJECT_h
#define CRE_WRAPPER_OBJECT_h

extern "C" {
#include "global.h"
}

#include <QObject>
#include "CREWrapperArchetype.h"
#include "CREWrapperTreasure.h"

class CREWrapperObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString race READ race)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QString slaying READ slaying)
    Q_PROPERTY(QString skill READ skill)
    Q_PROPERTY(int type READ type)
    Q_PROPERTY(int subType READ subType)
    Q_PROPERTY(int level READ level WRITE setLevel)
    Q_PROPERTY(bool isMonster READ isMonster)
    Q_PROPERTY(bool isAlive READ isAlive)
    Q_PROPERTY(bool isGenerator READ isGenerator)
    Q_PROPERTY(qint64 experience READ experience WRITE setExperience)
    Q_PROPERTY(quint32 attacktype READ attacktype)
    Q_PROPERTY(int ac READ ac WRITE setAc)
    Q_PROPERTY(int wc READ wc WRITE setWc)
    Q_PROPERTY(QObject* arch READ arch)
    Q_PROPERTY(int damage READ damage WRITE setDamage)
    Q_PROPERTY(int hp READ hp WRITE setHp)
    Q_PROPERTY(int weight READ weight WRITE setWeight)
    Q_PROPERTY(QString materialName READ materialName)
    Q_PROPERTY(QObject* randomItems READ randomItems)
    Q_PROPERTY(float speed READ speed WRITE setSpeed);

    public:
        CREWrapperObject();
        virtual ~CREWrapperObject();

        void setObject(object* obj);
        object *getObject() { return myObject; }

        CREWrapperArchetype* arch();
        QString name() const;
        QString race() const;
        QString title() const { return myObject->title; }
        QString slaying() const { return myObject->slaying; }
        QString skill() const { return myObject->skill; }
        int type() const;
        int subType() const { return myObject->subtype; }
        int level() const;
        void setLevel(int level) { myObject->level = level; }
        bool isMonster() const;
        bool isAlive() const;
        bool isGenerator() const { return QUERY_FLAG(myObject, FLAG_GENERATOR); }
        int64_t experience() const { return myObject->stats.exp; }
        void setExperience(int64_t experience) { myObject->stats.exp = experience; }
        quint32 attacktype() const;
        int8_t ac() const { return myObject->stats.ac; }
        void setAc(int8_t ac) { myObject->stats.ac = ac; }
        int8_t wc() const { return myObject->stats.wc; }
        void setWc(int8_t wc) { myObject->stats.wc = wc; }
        int16_t damage() const { return myObject->stats.dam; }
        void setDamage(int16_t damage) { myObject->stats.dam = damage; }
        int16_t hp() const { return myObject->stats.hp; }
        void setHp(int16_t hp) { myObject->stats.hp = hp; }
        int32_t weight() const { return myObject->weight; }
        void setWeight(int32_t weight) { myObject->weight = weight; }
        QString materialName() const;
        CREWrapperTreasureList *randomItems();
        float speed() const { return myObject->speed; }
        void setSpeed(float speed) { myObject->speed = speed; }

    protected:
        object* myObject;
        CREWrapperArchetype* myArchetype;
        CREWrapperTreasureList *myTreasure;
};

#endif // CRE_WRAPPER_OBJECT_h
