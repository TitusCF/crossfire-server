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
