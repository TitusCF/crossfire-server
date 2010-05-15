#ifndef _CRETREEITEMQUEST_H
#define	_CRETREEITEMQUEST_H

#include "CRETreeItem.h"

class Quest;

class CRETreeItemQuest : public CRETreeItem
{
    public:
        CRETreeItemQuest(Quest* quest);
        virtual ~CRETreeItemQuest();
        virtual QString getPanelName() const  { return "Quest"; }
        virtual void fillPanel(QWidget* panel);

    protected:
        Quest* myQuest;
};

#endif	/* _CRETREEITEMQUEST_H */
