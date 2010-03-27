#include "CREWrapperObject.h"

CREWrapperObject::CREWrapperObject()
{
    myObject = NULL;
}

void CREWrapperObject::setObject(const object* obj)
{
    myObject = obj;
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
