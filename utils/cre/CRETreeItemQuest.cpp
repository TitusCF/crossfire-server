#include "CRETreeItemQuest.h"
#include "CREQuestPanel.h"
#include <QTreeWidgetItem>
#include "CREResourcesWindow.h"
#include "ResourcesManager.h"
#include "CREPixmap.h"
#include "assets.h"
#include "AssetsManager.h"

CRETreeItemQuest::CRETreeItemQuest(quest_definition *quest, QTreeWidgetItem* item, CREResourcesWindow* window) : CRETTreeItem(quest, "Quest")
{
    Q_ASSERT(item);
    myTreeItem = item;
    Q_ASSERT(window);
    myWindow = window;

    /*
    if (myItem != NULL)
        connect(myItem, SIGNAL(modified()), this, SLOT(questModified()));
     */
}

CRETreeItemQuest::~CRETreeItemQuest()
{
}

QString CRETreeItemQuest::getPanelName() const
{
    if (myItem)
        return "Quest";
    return "(dummy)";
}

void CRETreeItemQuest::fillPanel(QWidget* panel)
{
    if (myItem == NULL)
        return;

    CRETTreeItem::fillPanel(panel);
}

void CRETreeItemQuest::questModified()
{
    myTreeItem->setText(0, myItem->quest_code);
    myTreeItem->setIcon(0, QIcon());
    if (myItem->face != nullptr)
    {
        myTreeItem->setIcon(0, CREPixmap::getIcon(myItem->face->number));
    }
}

void CRETreeItemQuest::fillContextMenu(QMenu* menu)
{
    if (!myItem)
        return;

    QAction* del = new QAction("delete quest", menu);
    connect(del, SIGNAL(triggered(bool)), this, SLOT(deleteQuest(bool)));
    menu->addAction(del);
}

void CRETreeItemQuest::deleteQuest(bool)
{
    Q_ASSERT(myItem);
//    myWindow->deleteQuest(myItem);
}
