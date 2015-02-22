#ifndef SCRIPTFILEMANAGER_H
#define	SCRIPTFILEMANAGER_H

#include <QHash>

class ScriptFile;
class CREMapInformation;

/** Manage scripts for items. */
class ScriptFileManager
{
    public:
        ScriptFileManager();
        virtual ~ScriptFileManager();

        QList<ScriptFile*> scriptsForMap(CREMapInformation* map);

        /**
         * Get information about a script path.
         * @param path full script path.
         * @return information about the script, never NULL.
         */
        ScriptFile* getFile(const QString& path);

        /**
         * Remove scripts linked to a map.
         * @param map map to remove scripts of.
         */
        void removeMap(CREMapInformation* map);

    private:
      QHash<QString, ScriptFile*> myScripts;
};

#endif	/* SCRIPTFILEMANAGER_H */

