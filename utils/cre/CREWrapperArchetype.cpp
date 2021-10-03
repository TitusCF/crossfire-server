#include "CREWrapperArchetype.h"
#include "CREWrapperObject.h"

CREWrapperArchetype::CREWrapperArchetype(CREWrapperObject* parent, archetype* arch)
{
    myArchetype = arch;
    myObject = parent;
}

void CREWrapperArchetype::setArchetype(archetype* arch)
{
    myArchetype = arch;
}

QString CREWrapperArchetype::name() const
{
    return myArchetype->name;
}

QObject* CREWrapperArchetype::clone()
{
    return myObject;
}
