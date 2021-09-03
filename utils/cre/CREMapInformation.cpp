#include "CREMapInformation.h"
#include "CRERandomMap.h"

CREMapInformation::CREMapInformation()
{
    myDifficulty = 0;
    myComputedDifficulty = 0;
    myExperience = 0;
    myShopGreed = 0;
    myShopMin = 0;
    myShopMax = 0;
}

CREMapInformation::CREMapInformation(const QString& path)
{
    myPath = path;
    myDifficulty = 0;
    myComputedDifficulty = 0;
    myExperience = 0;
    myShopGreed = 0;
    myShopMin = 0;
    myShopMax = 0;
}

CREMapInformation::~CREMapInformation()
{
    qDeleteAll(myRandomMaps);
}

CREMapInformation* CREMapInformation::clone() const
{
    CREMapInformation* clone = new CREMapInformation();
    clone->copy(*this);
    return clone;
}

void CREMapInformation::copy(const CREMapInformation& other)
{
    setPath(other.path());
    setName(other.name());
    myArchetypes.append(other.archetypes());
    setMapTime(other.mapTime());
    myExitsTo.append(other.exitsTo());
    myAccessedFrom.append(other.accessedFrom());
    setDifficulty(other.difficulty());
    setComputedDifficulty(other.computedDifficulty());
    setExperience(other.experience());
    setRegion(other.region());
    myShopItems = other.shopItems();
    myShopGreed = other.shopGreed();
    myShopRace = other.shopRace();
    myShopMin = other.shopMin();
    myShopMax = other.shopMax();
    /** @todo clone random maps? */
}

const QString& CREMapInformation::displayName() const
{
    if (myName.isEmpty())
        return myPath;
    return myName;
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

const QString& CREMapInformation::backgroundMusic()
{
    return myBackgroundMusic;
}

void CREMapInformation::setBackgroundMusic(const QString& music)
{
    myBackgroundMusic = music;
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

QStringList CREMapInformation::faces() const
{
    return myFaces.values();
}

void CREMapInformation::addFace(const QString& face)
{
    myFaces.insert(face);
}

QStringList CREMapInformation::animations() const
{
    return myAnimations.values();
}

void CREMapInformation::addAnimation(const QString& anim)
{
    myAnimations.insert(anim);
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

int CREMapInformation::difficulty() const
{
    return myDifficulty;
}

void CREMapInformation::setDifficulty(int difficulty)
{
    myDifficulty = difficulty;
}

int CREMapInformation::computedDifficulty() const
{
    return myComputedDifficulty;
}

void CREMapInformation::setComputedDifficulty(int computedDifficulty)
{
    myComputedDifficulty = computedDifficulty;
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

QStringList CREMapInformation::messages() const
{
    return myMessages;
}

void CREMapInformation::addMessage(const QString& message)
{
    if (!myMessages.contains(message))
        myMessages.append(message);
}

QStringList CREMapInformation::quests() const
{
    return myQuests;
}

void CREMapInformation::addQuest(const QString& quest)
{
    if (!myQuests.contains(quest))
        myQuests.append(quest);
}

QHash<QString, int>& CREMapInformation::shopItems()
{
    return myShopItems;
}

const QHash<QString, int>& CREMapInformation::shopItems() const
{
    return myShopItems;
}

double CREMapInformation::shopGreed() const
{
    return myShopGreed;
}

void CREMapInformation::setShopGreed(double greed)
{
    myShopGreed = greed;
}

const QString& CREMapInformation::shopRace() const
{
  return myShopRace;
}

void CREMapInformation::setShopRace(const QString& race)
{
  myShopRace = race;
}

quint64 CREMapInformation::shopMin() const
{
  return myShopMin;
}

void CREMapInformation::setShopMin(quint64 min)
{
  myShopMin = min;
}

quint64 CREMapInformation::shopMax() const
{
  return myShopMax;
}

void CREMapInformation::setShopMax(quint64 max)
{
  myShopMax = max;
}

QList<CRERandomMap*> CREMapInformation::randomMaps() const
{
  return myRandomMaps;
}

void CREMapInformation::addRandomMap(CRERandomMap* map)
{
  myRandomMaps.append(map);
}
