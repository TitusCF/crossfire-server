#ifndef SCRIPTFILE_H
#define	SCRIPTFILE_H

#include <QList>

class CREMapInformation;

/** Information about an event hook, linked to a script file. */
class HookInformation
{
    public:
        HookInformation(CREMapInformation* map, int x, int y, const QString& itemName, const QString& pluginName, const QString& eventName);

        const CREMapInformation* map() const;
        int x() const;
        int y() const;
        QString itemName() const;
        QString pluginName() const;
        QString eventName() const;

    private:
        CREMapInformation* myMap;
        int myX;
        int myY;
        QString myItemName;
        QString myPluginName;
        QString myEventName;
};

/** Information about a script file. */
class ScriptFile
{
    public:
        ScriptFile(const QString& path);
        virtual ~ScriptFile();

        void addHook(HookInformation* hook);
        bool forMap(CREMapInformation* map) const;
        /**
         * Remove hooks linked to a map.
         * @param map map to remove scripts of.
         * @return true if this script has no more hooks, false else.
         */
        bool removeMap(CREMapInformation* map);
        QList<HookInformation*> hooks() const;
        const QString& path() const;

    private:
      QString myPath;
      QList<HookInformation*> myHooks;
};

#endif	/* SCRIPTFILE_H */

