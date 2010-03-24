#include "CREWrapperArchetype.h"

CREWrapperArchetype::CREWrapperArchetype()
{
    myArchetype = NULL;
}

void CREWrapperArchetype::setArchetype(const archetype* arch)
{
    myArchetype = arch;
    myObject.setObject(&arch->clone);
}

QString CREWrapperArchetype::name() const
{
    return myArchetype->name;
}

QObject* CREWrapperArchetype::clone()
{
    return &myObject;
}
