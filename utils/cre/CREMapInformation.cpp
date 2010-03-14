#include "CREMapInformation.h"

CREMapInformation::CREMapInformation(const QString& path)
{
    myPath = path;
}

const QString& CREMapInformation::path() const
{
    return myPath;
}
