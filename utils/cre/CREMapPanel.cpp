#include "CREMapPanel.h"
#include "CREMapInformation.h"
#include "CREMainWindow.h"
#include "ScriptFileManager.h"
#include "ScriptFile.h"

CREMapPanel::CREMapPanel(ScriptFileManager* manager)
{
    Q_ASSERT(manager != NULL);
    myManager = manager;
    QGridLayout* layout = new QGridLayout(this);

    layout->addWidget(new QLabel(tr("Name:"), this), 0, 0);
    myName = new QLabel();
    layout->addWidget(myName, 0, 1);

    myExitsFrom = new QTreeWidget(this);
    myExitsFrom->setHeaderLabel(tr("Exits from this map"));
    layout->addWidget(myExitsFrom, 1, 0, 1, 2);

    myExitsTo = new QTreeWidget(this);
    myExitsTo->setHeaderLabel(tr("Exits leading to this map"));
    layout->addWidget(myExitsTo, 2, 0, 1, 2);

    myScripts = new QTreeWidget(this);
    myScripts->setHeaderLabel(tr("Scripts on this map"));
    layout->addWidget(myScripts, 3, 0, 1, 2);
}

CREMapPanel::~CREMapPanel()
{
}

void CREMapPanel::setMap(CREMapInformation* map)
{
    myName->setText(map->name());

    myExitsFrom->clear();
    foreach(QString path, map->accessedFrom())
        myExitsFrom->addTopLevelItem(new QTreeWidgetItem(QStringList(path)));

    myExitsTo->clear();
    foreach(QString path, map->exitsTo())
        myExitsTo->addTopLevelItem(new QTreeWidgetItem(QStringList(path)));

    myScripts->clear();
    foreach(ScriptFile* script, myManager->scriptsForMap(map))
    {
        foreach(HookInformation* hook, script->hooks())
        {
            if (hook->map() == map)
            {
                myScripts->addTopLevelItem(new QTreeWidgetItem(QStringList(QString("%1 [%2, %3], %4, %5, %6").arg(hook->itemName()).arg(hook->x()).arg(hook->y()).arg(hook->eventName()).arg(hook->pluginName()).arg(script->path()))));
            }
        }
    }
}
