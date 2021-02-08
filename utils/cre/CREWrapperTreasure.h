#ifndef CREWRAPPERTREASURE_H
#define CREWRAPPERTREASURE_H

#include <QObject>
#include <QStringList>

extern "C" {
#include "global.h"
}

class CREWrapperTreasureList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int total_chance READ totalChance)
    Q_PROPERTY(int item_count READ itemCount)

public:
    CREWrapperTreasureList(const treasurelist* list);

    QString name() const { return myList->name; }
    int totalChance() const { return myList->total_chance; }
    int itemCount() const;

private:
    const treasurelist* myList;
};

#endif /* CREWRAPPERTREASURE_H */

