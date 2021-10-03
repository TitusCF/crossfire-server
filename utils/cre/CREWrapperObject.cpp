#include "CREWrapperObject.h"

CREWrapperObject::CREWrapperObject()
{
    myObject = NULL;
    myArchetype = NULL;
    myTreasure = nullptr;
}

CREWrapperObject::~CREWrapperObject()
{
    delete myArchetype;
    delete myTreasure;
}

void CREWrapperObject::setObject(object* obj)
{
    myObject = obj;
    if (myArchetype == NULL)
        myArchetype = new CREWrapperArchetype(this, obj->arch);
    else
        myArchetype->setArchetype(obj->arch);
    delete myTreasure;
    myTreasure = nullptr;
}

CREWrapperArchetype* CREWrapperObject::arch()
{
    return myArchetype;
}

QString CREWrapperObject::name() const
{
    return myObject->name;
}

QString CREWrapperObject::race() const
{
    return myObject->race;
}

int CREWrapperObject::type() const
{
    return myObject->type;
}

int CREWrapperObject::level() const
{
    return myObject->level;
}

bool CREWrapperObject::isMonster() const
{
    return QUERY_FLAG(myObject, FLAG_MONSTER);
}

bool CREWrapperObject::isAlive() const
{
    return QUERY_FLAG(myObject, FLAG_ALIVE);
}

quint32 CREWrapperObject::attacktype() const
{
    return myObject->attacktype;
}

QString CREWrapperObject::materialName() const
{
  return myObject->materialname;
}

CREWrapperTreasureList *CREWrapperObject::randomItems() {
    if (!myObject->randomitems) {
        return nullptr;
    }
    if (!myTreasure) {
        myTreasure = new CREWrapperTreasureList(myObject->randomitems);
    }
    return myTreasure;
}
