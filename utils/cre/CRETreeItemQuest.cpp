#include "CRETreeItemQuest.h"
#include "CREQuestPanel.h"
#include "Quest.h"
#include <QTreeWidgetItem>

CRETreeItemQuest::CRETreeItemQuest(Quest* quest, QTreeWidgetItem* item)
{
    myQuest = quest;
    Q_ASSERT(item);
    myItem = item;

    connect(myQuest, SIGNAL(modified()), this, SLOT(questModified()));
}

CRETreeItemQuest::~CRETreeItemQuest()
{
}

void CRETreeItemQuest::fillPanel(QWidget* panel)
{
    Q_ASSERT(myQuest);
    CREQuestPanel* p = static_cast<CREQuestPanel*>(panel);
    p->setQuest(myQuest);
}

void CRETreeItemQuest::questModified()
{
    myItem->setText(0, myQuest->code());
}
