#ifndef CREARTIFACTPANEL_H
#define CREARTIFACTPANEL_H

#include <QObject>
#include <QtGui>
#include "CREPanel.h"
#include "CREAnimationControl.h"

extern "C" {
#include "global.h"
#include "artifact.h"
}

class CREArtifactPanel : public CRETPanel<const artifact>
{
    Q_OBJECT

    public:
        CREArtifactPanel();
        virtual void setItem(const artifact* artifact);

    protected:
        const artifact* myArtifact;
        QLineEdit* myName;
        QLineEdit* myChance;
        QLineEdit* myType;
        QLabel* myViaAlchemy;
        QTreeWidget* myArchetypes;
        QTextEdit* myValues;
        QTextEdit* myInstance;
        CREAnimationControl* myAnimation;
        CREAnimationWidget* myFace;

        void computeMadeViaAlchemy(const artifact* artifact) const;

    protected slots:
        void artifactChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
};

#endif // CREARTIFACTPANEL_H
