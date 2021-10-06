#ifndef _CRETREEITEMQUEST_H
#define _CRETREEITEMQUEST_H

#include <QObject>
#include "CRETreeItem.h"

extern "C" {
#include "quest.h"
}

class QTreeWidgetItem;
class CREResourcesWindow;

class CRETreeItemQuest : public CRETTreeItem<quest_definition>
{
    Q_OBJECT

    public:
        CRETreeItemQuest(quest_definition *quest, QTreeWidgetItem* item, CREResourcesWindow* window);
        virtual ~CRETreeItemQuest();
        virtual QString getPanelName() const;
        virtual void fillPanel(QWidget* panel);
        virtual void fillContextMenu(QMenu* menu);

    protected:
        QTreeWidgetItem* myTreeItem;
        CREResourcesWindow* myWindow;

    protected slots:
        void questModified();
        void deleteQuest(bool);
};

#endif /* _CRETREEITEMQUEST_H */
