#ifndef CREARTIFACTPANEL_H
#define CREARTIFACTPANEL_H

#include <QObject>
#include <QtGui>

extern "C" {
#include "global.h"
#include "artifact.h"
}

class CREArtifactPanel : public QWidget
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
        QTreeWidget* myArchetypes;
};

#endif // CREARTIFACTPANEL_H
