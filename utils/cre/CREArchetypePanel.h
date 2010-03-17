#ifndef CLASS_CRE_ARCHETYPE_PANEL_H
#define CLASS_CRE_ARCHETYPE_PANEL_H

#include <QObject>
#include <QtGui>

extern "C" {
#include "global.h"
}

class CREMapInformationManager;

class CREArchetypePanel : public QWidget
{
    Q_OBJECT

    public:
        CREArchetypePanel(CREMapInformationManager* store);
        void setArchetype(const archt* archetype);

    protected:
        const archt* myArchetype;
        QTextEdit* myDisplay;
        QTreeWidget* myUsing;
        CREMapInformationManager* myStore;
};

#endif // CLASS_CRE_ARCHETYPE_PANEL_H
