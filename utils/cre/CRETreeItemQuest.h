#ifndef _CRETREEITEMQUEST_H
#define _CRETREEITEMQUEST_H

#include <QObject>
#include "CRETreeItem.h"

class Quest;
class QTreeWidgetItem;
class CREResourcesWindow;

class CRETreeItemQuest : public CRETTreeItem<Quest>
{
    Q_OBJECT

    public:
        CRETreeItemQuest(Quest* quest, QTreeWidgetItem* item, CREResourcesWindow* window);
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
