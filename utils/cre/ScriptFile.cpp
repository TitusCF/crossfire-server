#include "ScriptFile.h"
#include "CREMapInformation.h"

ScriptFile::ScriptFile(const QString& path)
{
    myPath = path;
}

ScriptFile::~ScriptFile()
{
    qDeleteAll(myHooks);
}

void ScriptFile::addHook(HookInformation* hook)
{
    myHooks.append(hook);
}

bool ScriptFile::forMap(CREMapInformation* map) const
{
    foreach(HookInformation* hook, myHooks)
    {
        if (hook->map() == map)
        {
            return true;
        }
    }
    return false;
}

QList<HookInformation*> ScriptFile::hooks() const
{
    return myHooks;
}

const QString& ScriptFile::path() const
{
    return myPath;
}

bool ScriptFile::removeMap(CREMapInformation* map)
{
    QList<HookInformation*>::iterator hook = myHooks.begin();
    while (hook != myHooks.end())
    {
        if ((*hook)->map() == map)
        {
            HookInformation* h = *hook;
            hook = myHooks.erase(hook);
            delete h;
        }
        else
        {
            hook++;
        }
    }
    return myHooks.empty();
}

HookInformation::HookInformation(CREMapInformation* map, int x, int y, const QString& itemName, const QString& pluginName, const QString& eventName)
{
    myMap = map;
    myX = x;
    myY = y;
    myItemName = itemName;
    myPluginName = pluginName;
    myEventName = eventName;
}

const CREMapInformation* HookInformation::map() const
{
    return myMap;
}

int HookInformation::x() const
{
    return myX;
}

int HookInformation::y() const
{
    return myY;
}

QString HookInformation::itemName() const
{
    return myItemName;
}

QString HookInformation::pluginName() const
{
    return myPluginName;
}

QString HookInformation::eventName() const
{
  return myEventName;
}