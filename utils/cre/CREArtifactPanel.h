#ifndef CREARTIFACTPANEL_H
#define CREARTIFACTPANEL_H

#include <QObject>
#include <QtGui>
#include "CREPanel.h"

extern "C" {
#include "global.h"
#include "artifact.h"
}

class CREArtifactPanel : public CREPanel
{
    Q_OBJECT

    public:
        CREArtifactPanel();
        void setArtifact(const artifact* artifact);

    protected:
        const artifact* myArtifact;
        QLineEdit* myName;
        QLineEdit* myChance;
        QLineEdit* myType;
        QCheckBox* myViaAlchemy;
        QTreeWidget* myArchetypes;
        QTextEdit* myValues;
        QComboBox* myDisplay;
        QTextEdit* myInstance;

        bool madeViaAlchemy(const artifact* artifact) const;

    protected slots:
        void displayArchetypeChanged(int index);
};

#endif // CREARTIFACTPANEL_H
