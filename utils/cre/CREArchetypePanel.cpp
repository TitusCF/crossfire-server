#include <Qt>
#include "CREMapInformation.h"
#include "CREMapInformationManager.h"

extern "C" {
#include "global.h"
}

#include "CREArchetypePanel.h"
#include "CREUtils.h"
#include "ResourcesManager.h"

#include "assets.h"
#include "AssetsManager.h"
#include "Archetypes.h"

CREArchetypePanel::CREArchetypePanel(CREMapInformationManager* store, QWidget* parent) : CRETPanel(parent)
{
    Q_ASSERT(store);
    myStore = store;

    QGridLayout* layout = new QGridLayout(this);

    myDisplay = new QTextEdit(this);
    layout->addWidget(myDisplay, 1, 1);

    myUsing = new QTreeWidget(this);
    myUsing->setHeaderLabel(tr("Used by"));
    myUsing->setIconSize(QSize(32, 32));
    myUsing->setSortingEnabled(true);
    myUsing->sortByColumn(0, Qt::AscendingOrder);
    layout->addWidget(myUsing, 2, 1);
}

void CREArchetypePanel::setItem(const archt* archetype)
{
    myArchetype = archetype;
    StringBuffer* dump = stringbuffer_new();
    object_dump(&myArchetype->clone, dump);
    char* final = stringbuffer_finish(dump);
    myDisplay->setText(final);
    free(final);

    myUsing->clear();

    QTreeWidgetItem* rootArch = nullptr, *rootTreasure = nullptr, *rootMap = nullptr, *rootCrafting = nullptr;

    ResourcesManager::archetypeUse(myArchetype, myStore,
            [this, &rootArch, &rootTreasure, &rootMap, &rootCrafting]
            (ArchetypeUse use, const archt* arch, const treasurelist* list, const CREMapInformation* map, const recipe* rec) -> bool
    {
        if (use == OTHER_ARCH || use == DEATH_ANIM)
        {
            if (rootArch == NULL)
            {
                rootArch = CREUtils::archetypeNode(NULL);
                myUsing->addTopLevelItem(rootArch);
                rootArch->setExpanded(true);
            }
            auto node = CREUtils::archetypeNode(arch, rootArch);
            if (use == DEATH_ANIM) {
                node->setText(0, node->text(0) + " (as death animation)");
            }
        }
        if (use == TREASURE_USE)
        {
            if (rootTreasure == NULL)
            {
                rootTreasure = CREUtils::treasureNode(NULL);
                myUsing->addTopLevelItem(rootTreasure);
                rootTreasure->setExpanded(true);
            }
            CREUtils::treasureNode(list, rootTreasure);
        }
        if (use == MAP_USE || use == RANDOM_MAP_FINAL_EXIT)
        {
            if (rootMap == nullptr)
            {
                rootMap = new QTreeWidgetItem(myUsing, QStringList("Maps"));
                rootMap->setExpanded(true);
            }

            auto node = CREUtils::mapNode(map, rootMap);
            if (use == RANDOM_MAP_FINAL_EXIT)
            {
                node->setText(0, node->text(0) + " (final exit of random map)");
            }
        }
        if (use == ALCHEMY_PRODUCT)
        {
            if (rootCrafting == nullptr)
            {
                rootCrafting = new QTreeWidgetItem(myUsing, QStringList("Alchemy product"));
                myUsing->addTopLevelItem(rootCrafting);
                rootCrafting->setExpanded(true);
            }
            CREUtils::formulaeNode(rec, rootCrafting);
        }

        return true;
    });

    if (rootArch)
        CREUtils::addCountSuffix(rootArch);
    if (rootTreasure)
        CREUtils::addCountSuffix(rootTreasure);
    if (rootMap)
        CREUtils::addCountSuffix(rootMap);
    if (rootCrafting)
        CREUtils::addCountSuffix(rootCrafting);
    myUsing->scrollToTop();
}
