#ifndef CLASS_CRE_ARCHETYPE_PANEL_H
#define CLASS_CRE_ARCHETYPE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "CREPanel.h"

extern "C" {
#include "global.h"
}

class CREMapInformationManager;

class CREArchetypePanel : public CRETPanel<const archt>
{
    Q_OBJECT

    public:
        CREArchetypePanel(CREMapInformationManager* store, QWidget* parent);
        virtual void setItem(const archt* archetype);

    protected:
        QTextEdit* myDisplay;
        QTreeWidget* myUsing;
        CREMapInformationManager* myStore;
};

#endif // CLASS_CRE_ARCHETYPE_PANEL_H
