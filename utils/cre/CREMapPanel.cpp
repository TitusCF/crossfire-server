#include "CREMapPanel.h"
#include "CREMapInformation.h"
#include "CREMainWindow.h"
#include "ScriptFileManager.h"
#include "ScriptFile.h"
#include "CREPixmap.h"

extern "C" {
#include "define.h"
}

static QHash<QString, archetype*> events;

void static fillEvents()
{
    if (!events.isEmpty())
    {
        return;
    }

    for (archt* arch = first_archetype; arch != NULL; arch = arch->next)
    {
        if (arch->clone.type == EVENT_CONNECTOR)
        {
            events.insert(arch->name, arch);
        }
    }
}

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
    myScripts->setIconSize(QSize(32, 32));
   layout->addWidget(myScripts, 3, 0, 1, 2);

    fillEvents();
}

CREMapPanel::~CREMapPanel()
{
}

void CREMapPanel::setItem(CREMapInformation* map)
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
                QTreeWidgetItem* child = new QTreeWidgetItem(QStringList(QString("%1 [%2, %3], %4, %5, %6").arg(hook->itemName()).arg(hook->x()).arg(hook->y()).arg(hook->eventName()).arg(hook->pluginName()).arg(script->path())));

                archt* arch = events[hook->eventName().toLower()];
                if (arch != NULL && arch->clone.face != NULL)
                {
                    child->setIcon(0, CREPixmap::getIcon(arch->clone.face->number));
                }

                myScripts->addTopLevelItem(child);
            }
        }
    }
}
