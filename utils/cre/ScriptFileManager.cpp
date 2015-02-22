#include "ScriptFileManager.h"
#include "ScriptFile.h"

ScriptFileManager::ScriptFileManager()
{
}

ScriptFileManager::~ScriptFileManager()
{
    qDeleteAll(myScripts.values());
}

QList<ScriptFile*> ScriptFileManager::scriptsForMap(CREMapInformation* map)
{
    QList<ScriptFile*> list;
    foreach(ScriptFile* script, myScripts.values())
    {
        if (script->forMap(map))
        {
            list.append(script);
        }
    }
    return list;
}

ScriptFile* ScriptFileManager::getFile(const QString& path)
{
    if (!myScripts.contains(path))
    {
        myScripts.insert(path, new ScriptFile(path));
    }
    return myScripts[path];
}

void ScriptFileManager::removeMap(CREMapInformation* map)
{
    QHash<QString, ScriptFile*>::iterator script = myScripts.begin();
    while (script != myScripts.end())
    {
        if ((*script)->removeMap(map))
        {
            ScriptFile* s = *script;
            script = myScripts.erase(script);
            delete s;
        }
        else
        {
            script++;
        }
    }
}

QList<ScriptFile*> ScriptFileManager::scripts() const
{
    return myScripts.values();
}