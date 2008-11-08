#ifndef CRETREASUREPANEL_H
#define CRETREASUREPANEL_H

#include <QObject>
#include <QtGui>

extern "C" {
#include "global.h"
}

class CRETreasurePanel : public QWidget
{
    Q_OBJECT

    public:
        CRETreasurePanel();

        void setTreasure(const treasurelist* treasure);

    protected:
        const treasurelist* myTreasure;
        QTreeWidget* myUsing;
};

#endif // CRETREASUREPANEL_H
