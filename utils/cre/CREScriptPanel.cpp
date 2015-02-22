#include <QtGui>
#include "CREScriptPanel.h"
#include "ScriptFile.h"
#include "ScriptFile.h"
#include "CREMapInformation.h"

CREScriptPanel::CREScriptPanel()
{
    QGridLayout* layout = new QGridLayout(this);
    myMaps = new QTreeWidget(this);
    layout->addWidget(myMaps, 1, 1, 1, 1);
    myMaps->setHeaderLabel("Maps using this script");
    myMaps->setIconSize(QSize(32, 32));
    myMaps->setRootIsDecorated(true);
}

void CREScriptPanel::setScript(ScriptFile* script)
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
        maps[hook->map()->path()]->addChild(new QTreeWidgetItem(QStringList(QString("%1 (%2, %3), %4, %5").arg(hook->itemName()).arg(hook->x()).arg(hook->y()).arg(hook->eventName()).arg(hook->pluginName()))));
    }
}

