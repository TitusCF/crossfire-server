#include "CREMapInformation.h"

CREMapInformation::CREMapInformation(const QString& path)
{
    myPath = path;
}

const QString& CREMapInformation::path() const
{
    return myPath;
}

const QString& CREMapInformation::name() const
{
    return myName;
}

void CREMapInformation::setName(const QString& name)
{
    myName = name;
}
