#ifndef CLASS_CRE_ARCHETYPE_PANEL_H
#define CLASS_CRE_ARCHETYPE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "CREPanel.h"

extern "C" {
#include "global.h"
}

class CREMapInformationManager;
class ResourcesManager;

class CREArchetypePanel : public CRETPanel<archt>
{
    Q_OBJECT

    public:
        CREArchetypePanel(CREMapInformationManager* store, ResourcesManager* resources, QWidget* parent);
        virtual void setItem(archt* archetype);

        virtual void commitData() override;

    protected:
        CREMapInformationManager* myStore;
        ResourcesManager *myResources;
        QTextEdit* myDisplay;
        std::string myInitialArch;
        QTreeWidget* myUsing;
        archt* myArchetype;
};

#endif // CLASS_CRE_ARCHETYPE_PANEL_H
