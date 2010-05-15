#ifndef _CRETREEITEMQUEST_H
#define	_CRETREEITEMQUEST_H

#include <QObject>
#include "CRETreeItem.h"

class Quest;
class QTreeWidgetItem;

class CRETreeItemQuest : public CRETreeItem
{
    Q_OBJECT

    public:
        CRETreeItemQuest(Quest* quest, QTreeWidgetItem* item);
        virtual ~CRETreeItemQuest();
        virtual QString getPanelName() const  { return "Quest"; }
        virtual void fillPanel(QWidget* panel);

    protected:
        Quest* myQuest;
        QTreeWidgetItem* myItem;

    protected slots:
        void questModified();
};

#endif	/* _CRETREEITEMQUEST_H */
