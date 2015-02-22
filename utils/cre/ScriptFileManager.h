#ifndef SCRIPTFILEMANAGER_H
#define	SCRIPTFILEMANAGER_H

#include <QHash>

class ScriptFile;
class CREMapInformation;

class ScriptFileManager
{
    public:
        ScriptFileManager();
        virtual ~ScriptFileManager();

        QList<ScriptFile*> scriptsForMap(CREMapInformation* map);
        ScriptFile* getFile(const QString& path);
        void removeMap(CREMapInformation* map);

    private:
      QHash<QString, ScriptFile*> myScripts;
};

#endif	/* SCRIPTFILEMANAGER_H */

