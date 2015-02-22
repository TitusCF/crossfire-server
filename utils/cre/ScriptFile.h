#ifndef SCRIPTFILE_H
#define	SCRIPTFILE_H

#include <QList>

class CREMapInformation;

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

class ScriptFile
{
    public:
        ScriptFile(const QString& path);
        virtual ~ScriptFile();

        void addHook(HookInformation* hook);
        bool forMap(CREMapInformation* map) const;
        bool removeMap(CREMapInformation* map);
        QList<HookInformation*> hooks() const;
        const QString& path() const;

    private:
      QString myPath;
      QList<HookInformation*> myHooks;
};

#endif	/* SCRIPTFILE_H */

