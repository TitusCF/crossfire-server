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
        static CREMapInformationManager* instance();

        bool browseFinished() const;
        void start();
        void cancel();

        QList<CREMapInformation*> getArchetypeUse(const archetype* arch);

    signals:
        void browsingMap(const QString& path);
        void finished();

    protected:
        QHash<QString, CREMapInformation*> myInformation;
        QMultiHash<const archetype*, CREMapInformation*> myArchetypeUse;
        QStringList myToProcess;
        int myCurrentMap;
        QFuture<void> myWorker;
        bool myCancelled;
        QMutex myLock;

        void browseMaps();
        void process(const QString& path);

    private:
        CREMapInformationManager();
        static CREMapInformationManager* theInstance;
};

#endif // CLASS_CRE_MAP_INFORMATION_MANAGER_H
