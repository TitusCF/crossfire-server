#include "CREMapInformation.h"

CREMapInformation::CREMapInformation()
{
    myExperience = 0;
}

CREMapInformation::CREMapInformation(const QString& path)
{
    myPath = path;
}

const QString& CREMapInformation::path() const
{
    return myPath;
}

void CREMapInformation::setPath(const QString& path)
{
    myPath = path;
}

const QString& CREMapInformation::name() const
{
    return myName;
}

void CREMapInformation::setName(const QString& name)
{
    myName = name;
}

QStringList CREMapInformation::archetypes() const
{
    return myArchetypes;
}

void CREMapInformation::addArchetype(const QString& archetype)
{
    if (!myArchetypes.contains(archetype))
        myArchetypes.append(archetype);
}

const QDateTime& CREMapInformation::mapTime() const
{
    return myMapTime;
}

void CREMapInformation::setMapTime(const QDateTime& date)
{
    myMapTime = date;
}

QStringList CREMapInformation::exitsTo() const
{
    return myExitsTo;
}

void CREMapInformation::addExitTo(const QString& path)
{
    if (!myExitsTo.contains(path))
        myExitsTo.append(path);
}

QStringList CREMapInformation::accessedFrom() const
{
    return myAccessedFrom;
}

void CREMapInformation::addAccessedFrom(const QString& path)
{
    if (!myAccessedFrom.contains(path))
        myAccessedFrom.append(path);
}

qint64 CREMapInformation::experience() const
{
    return myExperience;
}

void CREMapInformation::setExperience(qint64 experience)
{
    myExperience = experience;
}

const QString& CREMapInformation::region() const
{
    return myRegion;
}
void CREMapInformation::setRegion(const QString& region)
{
    myRegion = region;
}
