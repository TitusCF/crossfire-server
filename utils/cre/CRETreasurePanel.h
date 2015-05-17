#ifndef CRETREASUREPANEL_H
#define CRETREASUREPANEL_H

#include <QObject>
#include <QtGui>
#include "CREPanel.h"

extern "C" {
#include "global.h"
}

class CRETreasurePanel : public CRETPanel<const treasurelist>
{
    Q_OBJECT

    public:
        CRETreasurePanel();

        virtual void setItem(const treasurelist* treasure);

    public slots:
      void onGenerate(bool pressed);

    protected:
        const treasurelist* myTreasure;
        QTreeWidget* myUsing;
        QTreeWidget* myGenerated;
        QSpinBox* myDifficulty;

};

#endif // CRETREASUREPANEL_H
