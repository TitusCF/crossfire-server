#include "CRETreeItemQuest.h"
#include "CREQuestPanel.h"

CRETreeItemQuest::CRETreeItemQuest(Quest* quest)
{
    myQuest = quest;
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
