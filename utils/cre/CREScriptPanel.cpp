#include <QtGui>
#include "CREScriptPanel.h"
#include "ScriptFile.h"
#include "ScriptFile.h"
#include "CREMapInformation.h"
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

CREScriptPanel::CREScriptPanel()
{
    QGridLayout* layout = new QGridLayout(this);
    myMaps = new QTreeWidget(this);
    layout->addWidget(myMaps, 1, 1, 1, 1);
    myMaps->setHeaderLabel("Maps using this script");
    myMaps->setIconSize(QSize(32, 32));
    myMaps->setRootIsDecorated(true);

    fillEvents();
}

void CREScriptPanel::setItem(ScriptFile* script)
{
    myMaps->clear();

    QHash<QString, QTreeWidgetItem*> maps;

    foreach(const HookInformation* hook, script->hooks())
    {
        if (!maps.contains(hook->map()->path()))
        {
            QTreeWidgetItem* r = new QTreeWidgetItem(myMaps, QStringList(hook->map()->path()));
            maps[hook->map()->path()] = r;
            myMaps->addTopLevelItem(r);
            r->setExpanded(true);
        }

        QTreeWidgetItem* child = new QTreeWidgetItem(QStringList(QString("%1 (%2, %3), %4, %5").arg(hook->itemName()).arg(hook->x()).arg(hook->y()).arg(hook->eventName()).arg(hook->pluginName())));

        archt* arch = events[hook->eventName().toLower()];
        if (arch != NULL && arch->clone.face != NULL)
        {
            child->setIcon(0, CREPixmap::getIcon(arch->clone.face->number));
        }

        maps[hook->map()->path()]->addChild(child);
    }
}

