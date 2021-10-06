#ifndef CLASS_CRE_MAP_INFORMATION_MANAGER_H
#define CLASS_CRE_MAP_INFORMATION_MANAGER_H

#include <QtCore>
#include "CREMapInformation.h"

extern "C" {
#include "global.h"
#include "quest.h"
}

class MessageManager;
class ScriptFileManager;
class CRERandomMap;

class CREMapInformationManager : public QObject
{
    Q_OBJECT

    public:
        CREMapInformationManager(QObject* parent, MessageManager* messageManager, ScriptFileManager* scriptManager);
        virtual ~CREMapInformationManager();

        bool browseFinished() const;
        void start();
        void cancel();
        void clearCache();

        QList<CREMapInformation*> allMaps();
        QList<CREMapInformation*> getArchetypeUse(const archetype* arch);
        QList<CREMapInformation*> getFaceUse(const Face* face);
        QList<CREMapInformation*> getAnimationUse(const Animations* anim);
        QList<CREMapInformation*> getMapsForRegion(const QString& region);
        QList<CREMapInformation*> getMapsForQuest(const quest_definition *quest);
        QList<CRERandomMap*> randomMaps();

    signals:
        void browsingMap(const QString& path);
        void finished();

    protected:
        MessageManager* myMessageManager;
        ScriptFileManager* myScriptManager;
        QHash<QString, CREMapInformation*> myInformation;
        QMultiHash<QString, CREMapInformation*> myArchetypeUse;
        QMultiHash<QString, CREMapInformation*> myFaceUse;
        QMultiHash<QString, CREMapInformation*> myAnimationUse;
        QMultiHash<QString, CREMapInformation*> myQuestUse;
        QStringList myToProcess;
        int myCurrentMap;
        QFuture<void> myWorker;
        bool myCancelled;
        QMutex myLock;
        QHash<QString, qint64> myExperience;

        void browseMaps();
        void process(const QString& path);
        void checkItem(const object* item, CREMapInformation* information, const object* env);
        void loadCache();
        void storeCache();
        CREMapInformation* getOrCreateMapInformation(const QString& path);
        void addArchetypeUse(const QString& name, CREMapInformation* map);
        void addFaceUse(const QString& name, CREMapInformation* map);
        void addAnimationUse(const QString& name, CREMapInformation* map);
        void addQuestUse(const QString &name, CREMapInformation *map);
        void checkEvent(const object* item, CREMapInformation* map, const object* env);
        void recurseStyleDirectory(const QString& from);
};

#endif // CLASS_CRE_MAP_INFORMATION_MANAGER_H
