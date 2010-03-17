#ifndef CLASS_CRE_MAP_INFORMATION_H
#define CLASS_CRE_MAP_INFORMATION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>

class CREMapInformation : public QObject
{
    Q_OBJECT

    public:
        CREMapInformation();
        CREMapInformation(const QString& path);

        const QString& path() const;
        void setPath(const QString& path);

        const QString& name() const;
        void setName(const QString& name);

        QStringList archetypes() const;
        void addArchetype(const QString& archetype);

        const QDateTime& mapTime() const;
        void setMapTime(const QDateTime& date);

        QStringList exitsTo() const;
        void addExitTo(const QString& path);

        QStringList accessedFrom() const;
        void addAccessedFrom(const QString& path);

    protected:
        QString myPath;
        QString myName;
        QStringList myArchetypes;
        QDateTime myMapTime;
        QStringList myExitsTo;
        QStringList myAccessedFrom;
};

#endif // CLASS_CRE_MAP_INFORMATION_H
