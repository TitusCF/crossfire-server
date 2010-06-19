#ifndef CLASS_CRE_MAP_INFORMATION_MANAGER_H
#define CLASS_CRE_MAP_INFORMATION_MANAGER_H

#include <QtCore>
#include "CREMapInformation.h"

extern "C" {
#include "global.h"
}

class CREMapInformationManager : public QObject
{
    Q_OBJECT

    public:
        CREMapInformationManager(QObject* parent);
        virtual ~CREMapInformationManager();

        bool browseFinished() const;
        void start();
        void cancel();

        QList<CREMapInformation*> getArchetypeUse(const archetype* arch);
        QList<CREMapInformation*> getMapsForRegion(const QString& region);

    signals:
        void browsingMap(const QString& path);
        void finished();

    protected:
        QHash<QString, CREMapInformation*> myInformation;
        QMultiHash<QString, CREMapInformation*> myArchetypeUse;
        QStringList myToProcess;
        int myCurrentMap;
        QFuture<void> myWorker;
        bool myCancelled;
        QMutex myLock;
        QHash<QString, qint64> myExperience;

        void browseMaps();
        void process(const QString& path);
        void loadCache();
        void storeCache();
        CREMapInformation* getOrCreateMapInformation(const QString& path);
        void addArchetypeUse(const QString& name, CREMapInformation* map);
};

#endif // CLASS_CRE_MAP_INFORMATION_MANAGER_H
