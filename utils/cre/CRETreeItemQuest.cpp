#include "CRETreeItemQuest.h"
#include "CREQuestPanel.h"
#include "Quest.h"
#include <QTreeWidgetItem>
#include "CREResourcesWindow.h"

CRETreeItemQuest::CRETreeItemQuest(Quest* quest, QTreeWidgetItem* item, CREResourcesWindow* window)
{
    myQuest = quest;
    Q_ASSERT(item);
    myItem = item;
    Q_ASSERT(window);
    myWindow = window;

    if (myQuest != NULL)
        connect(myQuest, SIGNAL(modified()), this, SLOT(questModified()));
}

CRETreeItemQuest::~CRETreeItemQuest()
{
}

QString CRETreeItemQuest::getPanelName() const
{
    if (myQuest)
        return "Quest";
    return "(dummy)";
}

void CRETreeItemQuest::fillPanel(QWidget* panel)
{
    if (myQuest == NULL)
        return;

    Q_ASSERT(myQuest);
    CREQuestPanel* p = static_cast<CREQuestPanel*>(panel);
    p->setQuest(myQuest);
}

void CRETreeItemQuest::questModified()
{
    myItem->setText(0, myQuest->code());
}

void CRETreeItemQuest::fillContextMenu(QMenu* menu)
{
    if (!myQuest)
        return;

    QAction* del = new QAction("delete quest", menu);
    connect(del, SIGNAL(triggered(bool)), this, SLOT(deleteQuest(bool)));
    menu->addAction(del);
}

void CRETreeItemQuest::deleteQuest(bool)
{
    Q_ASSERT(myQuest);
    myWindow->deleteQuest(myQuest);
}
