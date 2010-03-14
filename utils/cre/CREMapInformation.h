#ifndef CLASS_CRE_MAP_INFORMATION_H
#define CLASS_CRE_MAP_INFORMATION_H

#include <QObject>
#include <QString>

class CREMapInformation : public QObject
{
    Q_OBJECT

    public:
        CREMapInformation(const QString& path);

        const QString& path() const;

        const QString& name() const;
        void setName(const QString& name);

    protected:
        QString myPath;
        QString myName;
};

#endif // CLASS_CRE_MAP_INFORMATION_H
